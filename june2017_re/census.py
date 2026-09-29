"""June2017 effect-engine key census and gap matrix (Phase 1 of JUNE2017_EFFECT_REBUILD_PLAN.md).

    python census.py               # re-derive everything, write the matrix + census.tsv
    python census.py --summary     # re-derive everything, print counts per status and phase
    python census.py --reuse-pp    # development only: reuse pp/*.i instead of re-running cl /P

Outputs: ../JUNE2017_EFFECT_KEY_MATRIX.md (generated; do not edit by hand) and
census.tsv (one row per census row, for scripts). Exit status 1 when a check fails.
Later phases run this as their exit test, so every input is re-read from the tree:

  June data    june_lua/*.src.lua, cross-checked key-for-key against the bytecode
               (reconcile.source_keys / bytecode_keys; the run aborts on a mismatch).
  June binary  xorstrings.tsv (ok rows only) + unpacked_x2.exe, through reconcile.Binary
               and reconcile.Explainer, the same machinery reconcile.md is built with.
  March        X2Lib's parser translation units, preprocessed with cl /P under the
               X2Lib US_SERVICE defines + UNICODE (the vcxproj's own include paths).
               A read counts only if it survives the preprocessor, so an #ifdef'd-out
               PLUA_GET_VALUE is not "implemented", and a stringized key
               (CASE_BEHAVIOR_FACTOR -> STRINGIZEA2(BBT_STUN)) arrives as the literal
               "BBT_STUN". Raw-source quoted-literal and bare-identifier hits are kept
               as leads only.

One census row is (family, scope root, sub-path, key). A row's status is exactly one of:

  implemented            June reads it and March reads it at the same sub-table path
  to-implement           June reads it and March does not; `phase` names the owner
  unsupported            as to-implement, but the key belongs to a system this build
                         lacks (ABSENT SYSTEMS in the matrix gives the evidence)
  binary-only            a June reader asks for it and no in-scope June data uses it
                         (still implemented under plan rule 3; `phase` names the owner)
  june-ignores           the data uses it and June itself never reads it: a studio typo,
                         dead copy-pasted data, a removed key, a positional entry, or a
                         key under a sub-table June never opens. Nothing to implement.
                         This fifth status is not in the plan's list of four; see the
                         matrix preamble.

`semantics` (implemented rows): `same`, `differ(type|default)` or `unchecked(...)`, from
June's getter call at the decrypt site against March's expanded GetValue statement.
"""
import argparse
import bisect
import collections
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..'))
# The rebuild's own flags. Code under them is not evidence for the absent-system search:
# plan rule 5 makes a phase parse, store and log an unsupported key, which names the key.
JUNE_FLAG_PREFIX = 'SERV_IRUHADEV_JUNE_'
sys.path.insert(0, HERE)
import reconcile as R    # noqa: E402
import xordecrypt as X   # noqa: E402

MATRIX = os.path.join(TRUNK, 'JUNE2017_EFFECT_KEY_MATRIX.md')
TSV = os.path.join(HERE, 'census.tsv')
PP_DIR = os.path.join(HERE, 'pp')
JUNE_ENUM = os.path.join(R.LUA, 'Enum.src.lua')
MARCH_ENUMS = [os.path.join(TRUNK, 'march2014', 'data036', f) for f in ('Enum.src.lua', 'DungeonEnum.src.lua')]

# ------------------------------------------------------------------ March preprocessing
VS = r'D:\Program Files\VS\Microsoft Visual Studio 10.0'
WINSDK_INC = r'C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include'
# X2Lib_2010.vcxproj, US_SERVICE|Win32: AdditionalIncludeDirectories, then IncludePath.
X2LIB_INC = ['./', '../luaLib', '../KTDXLIB', './Common', '../KTDXLIB/DXUT', '../X2ServerProtocol',
             '../X2ServerProtocol/Socket/', '../KncWX2Server/Common/', '../NexonModule',
             '../NexonModule/NMClass', '../X2', '../X2/dat', '../X2/dat/script', '../libxml']
X2LIB_INC_SOLN = [r'Libs\ExternalLib\sqlite3', r'Libs\ExternalLib\DXSDK\Include',
                  r'Libs\ExternalLib\Boost\boost_1_44_0', r'KNCSDK\Include_2010',
                  r'KOGGamePerformanceCheck\include', r'Libs\ExternalLib\freetype-2.4.11\include',
                  r'XTRAP\Client', 'Steamworks']
# PreprocessorDefinitions for US_SERVICE, plus UNICODE/_UNICODE from CharacterSet=Unicode.
X2LIB_DEFS = ['WIN32', 'NDEBUG', '_LIB', '_USE_32BIT_TIME_T', '_IN_HOUSE_', 'SERV_COUNTRY_US',
              'CLIENT_COUNTRY_US', '_HAS_ITERATOR_DEBUGGING=0', '_CONVERT_VS_2010', 'UNICODE', '_UNICODE']
TUS = ['X2DamageEffect.cpp', 'X2EffectSet.cpp', 'X2DamageManager.cpp', 'X2BuffFactor.cpp',
       'X2BuffBehaviorTemplet.cpp', 'X2BuffFinalizerTemplet.cpp', 'X2BuffTempletManager.cpp']
FLAG_PROBE = '_census_flag_probe.cpp'   # written into pp/, compiled with X2Lib as cwd


def cl_env():
    env = dict(os.environ)
    env['PATH'] = VS + r'\Common7\IDE;' + VS + r'\VC\bin;' + env.get('PATH', '')
    env['INCLUDE'] = ';'.join([VS + r'\VC\include', VS + r'\VC\atlmfc\include', WINSDK_INC])
    return env


def cl_cmd(src, out):
    inc = X2LIB_INC + [os.path.join(TRUNK, s) for s in X2LIB_INC_SOLN]
    return ([VS + r'\VC\bin\cl.exe', '/nologo', '/P', '/Zm600', '/Zc:wchar_t', '/Fi' + out] +
            ['/I' + i for i in inc] + ['/D' + d for d in X2LIB_DEFS] + [src])


def preprocess(reuse, flags):
    """cl /P every TU in TUS (in parallel) and a flag probe -> {tu: path of .i}, {flag: bool}."""
    os.makedirs(PP_DIR, exist_ok=True)
    probe = os.path.join(PP_DIR, FLAG_PROBE)
    body = ['#include "stdafx.h"']
    for f in sorted(flags):
        # The name goes in a string literal: a defined flag is usually an empty macro, so a
        # bare `CENSUS_FLAG_ON FLAG` would preprocess to `CENSUS_FLAG_ON ` and lose it.
        body += ['#ifdef %s' % f, 'CENSUS_FLAG_ON "%s"' % f, '#else', 'CENSUS_FLAG_OFF "%s"' % f, '#endif']
    open(probe, 'w', newline='\r\n').write('\n'.join(body) + '\n')
    jobs = [(tu, tu, os.path.join(PP_DIR, tu[:-4] + '.i')) for tu in TUS]
    jobs.append(('<flags>', probe, os.path.join(PP_DIR, '_census_flag_probe.i')))
    procs = []
    env = cl_env()
    for name, src, out in jobs:
        if reuse and os.path.exists(out) and name != '<flags>':
            continue
        procs.append((name, out, subprocess.Popen(cl_cmd(src, out), cwd=os.path.join(TRUNK, 'X2Lib'), env=env,
                                                  stdout=subprocess.PIPE, stderr=subprocess.STDOUT)))
    for name, out, p in procs:
        log = p.communicate()[0].decode('latin1', 'replace')
        if p.returncode != 0 or not os.path.exists(out):
            sys.exit('cl /P failed for %s (exit %s):\n%s' % (name, p.returncode, log[-3000:]))
    text = open(os.path.join(PP_DIR, '_census_flag_probe.i'), 'rb').read().decode('latin1')
    state = {}
    for m in re.finditer(r'CENSUS_FLAG_(ON|OFF) "(\w+)"', text):
        state[m.group(2)] = m.group(1) == 'ON'
    return dict((tu, os.path.join(PP_DIR, tu[:-4] + '.i')) for tu in TUS), state


class TU:
    """One preprocessed translation unit: its own .cpp's lines, with source line numbers."""
    LINE = re.compile(r'^#line (\d+) "(.*)"')

    def __init__(self, tu, path):
        self.tu = tu
        self.all_text = open(path, 'rb').read().decode('latin1')
        parts, lines = [], []
        keep, ln = False, 0
        want = tu.lower()
        for s in self.all_text.split('\n'):
            if s.startswith('#line '):
                m = self.LINE.match(s)
                if m:
                    ln = int(m.group(1))
                    keep = os.path.basename(m.group(2).replace('\\\\', '\\')).lower() == want
                    continue
            if keep:
                parts.append(s)
                lines.append(ln)
            ln += 1
        self.text = '\n'.join(parts)
        self.offs = []   # start offset of each kept line
        o = 0
        for s in parts:
            self.offs.append(o)
            o += len(s) + 1
        self.lines = lines

    def line_at(self, off):
        return self.lines[bisect.bisect_right(self.offs, off) - 1]

    def functions(self, qual):
        """-> [(body_start, body_end)] for every definition of `qual` in this TU."""
        out = []
        for m in re.finditer(r'(?<![\w:~])' + re.escape(qual) + r'\s*\(', self.text):
            close = balanced(self.text, m.end() - 1)
            if close is None:
                continue
            j = close + 1
            rest = self.text[j:j + 4000]
            k = re.match(r'\s*(const\s*)?', rest).end()
            if rest[k:k + 1] not in ('{', ':'):
                continue            # a call or a declaration, not a definition
            b = self.text.find('{', j + k) if rest[k] == '{' else skip_init_list(self.text, j + k)
            e = balanced(self.text, b, '{', '}')
            if e is not None:
                out.append((b, e))
        return out


def skip_init_list(t, i):
    """From the ':' of a ctor initializer list to the body's '{'."""
    depth = 0
    while i < len(t):
        c = t[i]
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif c == '{' and depth == 0:
            return i
        elif c == '"':
            i = skip_str(t, i)
        i += 1
    return None


def skip_str(t, i):
    q = t[i]
    i += 1
    while i < len(t) and t[i] != q:
        i += 2 if t[i] == '\\' else 1
    return i


def balanced(t, i, o='(', c=')'):
    """Index of the bracket closing t[i] (== o), skipping string/char literals."""
    if i is None or i >= len(t) or t[i] != o:
        return None
    depth = 0
    while i < len(t):
        ch = t[i]
        if ch in '"\'':
            i = skip_str(t, i)
        elif ch == o:
            depth += 1
        elif ch == c:
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return None


STR = re.compile(r'(L?)"((?:[^"\\\n]|\\.)*)"')
KEYLIKE = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*(%d)?[A-Za-z0-9_]*$')


def march_reads(tu, b, e):
    """Every narrow string literal in a function body with the Lua sub-table path it is
    read under. BeginTable(...) pushes its name ("NAME" + index arg -> NAME%d; a variable
    -> '*'). Scope of a push: a BeginTable in an if/while condition covers the next
    block (or, negated, the rest of the enclosing block); a statement BeginTable lasts
    until its EndTable at the same brace depth."""
    t = tu.text
    out = []
    stack = []           # [name, depth, mode, armed]
    depth = 0
    i = b
    while i <= e:
        ch = t[i]
        if ch == '{':
            depth += 1
            for s in stack:
                if s[2] == 'block' and not s[3] and depth == s[1] + 1:
                    s[3] = True
        elif ch == '}':
            depth -= 1
            stack = [s for s in stack if not ((s[2] == 'block' and s[3] and depth == s[1]) or depth < s[1])]
        elif ch == "'":
            i = skip_str(t, i) + 1
            continue
        elif ch == '"' or (ch == 'L' and t[i + 1:i + 2] == '"' and not (t[i - 1].isalnum() or t[i - 1] == '_')):
            m = STR.match(t, i)
            if m:
                if not m.group(1) and KEYLIKE.match(m.group(2)):
                    out.append((tuple(s[0] for s in stack), m.group(2), m.start()))
                i = m.end()
                continue
        elif t.startswith('BeginTable', i) and not t[i - 1].isalnum():
            j = t.find('(', i)
            close = balanced(t, j)
            args = split_args(t[j + 1:close])
            name = '*'
            sm = STR.fullmatch(args[0].strip()) if args else None
            if sm:
                name = sm.group(2)
            if len(args) >= 2:
                name += '%d'
            if name != '*':
                out.append((tuple(s[0] for s in stack), name, i))   # opening a table is reading its name
            cond = cond_context(t, b, i)
            if cond is None:
                mode = 'end'
            else:
                neg = (re.match(r'\s*==\s*false', t[close + 1:close + 20]) or
                       re.search(r'(!|false\s*==)\s*[\w.\->]*$', t[cond + 1:i]))
                mode = 'rest' if neg else 'block'
            stack.append([name, depth, mode, False])
            i = close + 1
            continue
        elif t.startswith('EndTable', i) and not t[i - 1].isalnum():
            for k in range(len(stack) - 1, -1, -1):
                if stack[k][2] == 'end' and stack[k][1] == depth:
                    del stack[k]
                    break
        i += 1
    return out


def cond_context(t, b, i):
    """Start of the if/while condition that contains position i, if any."""
    depth = 0
    k = i - 1
    while k > b:
        c = t[k]
        if c == ')':
            depth += 1
        elif c == '(':
            if depth == 0:
                pre = t[max(b, k - 12):k].rstrip()
                if re.search(r'\b(if|while)$', pre):
                    return k
                # an enclosing call such as `true == luaManager.BeginTable(` -> keep going
            else:
                depth -= 1
        elif c in ';{}':
            return None
        k -= 1
    return None


def split_args(s):
    out, depth, cur = [], 0, ''
    i = 0
    while i < len(s):
        c = s[i]
        if c in '"\'':
            j = skip_str(s, i)
            cur += s[i:j + 1]
            i = j + 1
            continue
        if c in '([{':
            depth += 1
        elif c in ')]}':
            depth -= 1
        if c == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += c
        i += 1
    if cur.strip():
        out.append(cur)
    return out


def getvalue_stmt(t, pos):
    """At a key literal inside GetValue( ("KEY"), VAR ), parse the expanded
    PLUA_GET_VALUE / _ENUM statement -> dict(var, default, enum) or None."""
    pre = t[max(0, pos - 60):pos]
    m = re.search(r'GetValue\w*\s*(\()\s*\(\s*$', pre)
    if not m:
        return None
    call_open = pos - len(pre) + m.start(1)
    close = balanced(t, call_open)
    if close is None:
        return None
    k = t.find(',', t.find('"', pos + 1) + 1)
    if k == -1 or k > close:
        return None
    var = t[k + 1:close].strip()
    tail = t[close + 1:close + 600]
    em = re.match(r'\s*==\s*false\s*\)\s*\{\s*\((.*?)\)\s*=\s*\((.*?)\)\s*;\s*\}\s*else\s*\{\s*\(.*?\)\s*=\s*\(([^()]*)\)\s*intBuffer', tail, re.S)
    if var == 'intBuffer' and em:
        return {'var': em.group(1).strip(), 'default': em.group(2).strip(), 'enum': em.group(3).strip()}
    pm = re.match(r'\s*==\s*false\s*\)\s*', tail)
    if not pm:
        return {'var': strip_paren(var), 'default': None, 'enum': None, 'shape': 'no ==false default'}
    j = pm.end()
    if tail[j:j + 1] != '(':
        return {'var': strip_paren(var), 'default': None, 'enum': None, 'shape': tail[j:j + 30]}
    lhs_end = balanced(tail, j)
    eq = re.match(r'\s*=\s*', tail[lhs_end + 1:])
    if not eq:
        return {'var': strip_paren(var), 'default': None, 'enum': None, 'shape': 'no assignment'}
    d0 = lhs_end + 1 + eq.end()
    if tail[d0:d0 + 1] == '(':
        d1 = balanced(tail, d0)
        default = tail[d0 + 1:d1].strip()
    else:
        default = tail[d0:tail.find(';', d0)].strip()
    return {'var': strip_paren(var), 'default': default, 'enum': None}


def strip_paren(s):
    s = s.strip()
    while s.startswith('(') and balanced(s, 0) == len(s) - 1:
        s = s[1:-1].strip()
    return s


# ------------------------------------------------------------------ scope table
CE = 0x31f7aab0
DE_TOP_READERS = [CE, 0x31f8ba80, 0x31f8b370]
DD_READERS = [0x32632390, 0x31f79c20, 0x32630d20]
DE_CPP = 'X2DamageEffect.cpp'
DM_CPP = 'X2DamageManager.cpp'
ES_CPP = 'X2EffectSet.cpp'
CE_LOADERS = ['LoadLockOnData', 'LoadBlackHoleData', 'LoadSinMoveData', 'LoadDieDamageEffect', 'LoadStartParticle',
              'LoadPassiveParticle', 'LoadTimeParticle', 'LoadCameraData', 'LoadScreenData', 'LoadSoundData',
              'LoadDamageEffect', 'LoadHitParticle', 'LoadHitDamageEffect', 'LoadAfterImage', 'LoadDieCheckCondition',
              'LoadHitEffectSet', 'LoadDieCreateNpc', 'LoadTouchLandDamageEffect']


def ce(name, prefix=('*',)):
    return (DE_CPP, 'CX2DamageEffect::CEffect::' + name, prefix)


# (root id, family, first-path-component regex, June readers, March [(tu, function, path prefix)],
#  phase, subsystem). prefix = the March sub-table path the root's own keys are read under
# inside that function: ('*',) when the function opens the root table itself, () when its
# caller did. The DE root "top" also lists every loader at (), because a loader opening
# BeginTable("LOCK_ON_DATA") is March reading the top-level key LOCK_ON_DATA.
ROOTS = [
    ('DE.top', 'DE', None, DE_TOP_READERS,
     [(DE_CPP, 'CX2DamageEffect::CEffect::CEffect', ())] + [ce(n, ()) for n in CE_LOADERS], '5', 'DamageEffect CEffect'),
    ('DE.DAMAGE_DATA', 'DE', r'DAMAGE_DATA(_ULTIMATE|_NEXT(_ULTIMATE)?(%d)?)?', DD_READERS,
     [(DM_CPP, 'CX2DamageManager::SetDamageDataFromLUA', ('*',)), (DM_CPP, 'CX2DamageManager::ParsingBuffFactorID', ()),
      (DM_CPP, 'CX2DamageManager::ParsingBuffFactor', ())], '8', 'DamageData'),
    ('DE.PARTICLE', 'DE', r'(START|PASSIVE|TIME|HIT)_PARTICLE%d', [0x31f66e60, 0x31f6a7b0, 0x31f6a940, 0x31f6aad0, 0x31f6cd00],
     [ce('LoadParticleTable', ())] + [ce(n) for n in ('LoadStartParticle', 'LoadPassiveParticle', 'LoadTimeParticle', 'LoadHitParticle')],
     '6b', 'DamageEffect sub-loaders'),
    ('DE.CREATE_DAMAGE_EFFECT', 'DE', r'CREATE_DAMAGE_EFFECT%d', [0x31f6ac60], [ce('LoadDamageEffect')], '6a', 'DamageEffect sub-loaders'),
    ('DE.DIE_DAMAGE_EFFECT', 'DE', r'DIE_DAMAGE_EFFECT(_ULTIMATE)?%d', [0x31f68f30, 0x31f6e7e0], [ce('LoadDieDamageEffect')], '6a', 'DamageEffect sub-loaders'),
    ('DE.HIT_DAMAGE_EFFECT', 'DE', r'HIT_DAMAGE_EFFECT(_ULTIMATE)?%d', [0x31f6ceb0, 0x31f6e850], [ce('LoadHitDamageEffect')], '6a', 'DamageEffect sub-loaders'),
    ('DE.DIE_CHECK_CONDITION', 'DE', r'DAMAGE_EFFECT_DIE_CHECK_CONDITION', [0x31f66700, CE],
     [ce('LoadDieCheckCondition'), ce('LoadDieCheckConditionBlock', ())], '6a', 'DamageEffect sub-loaders'),
    ('DE.LOCK_ON_DATA', 'DE', r'LOCK_ON_DATA(_ULTIMATE)?', [0x31f71930], [ce('LoadLockOnData'), ce('LoadLockOnDataBlock', ())], '6b', 'DamageEffect sub-loaders'),
    ('DE.BLACKHOLE_DATA', 'DE', r'BLACKHOLE_DATA(_ULTIMATE)?', [0x31f637e0], [ce('LoadBlackHoleData'), ce('LoadBlackHoleDataBlock', ())], '6b', 'DamageEffect sub-loaders'),
    ('DE.SIN_MOVE_DATA', 'DE', r'SIN_MOVE_DATA', [0x31f64200], [ce('LoadSinMoveData')], '6b', 'DamageEffect sub-loaders'),
    ('DE.CAMERA', 'DE', r'CAMERA%d', [0x31f650f0], [ce('LoadCameraData')], '6b', 'DamageEffect sub-loaders'),
    ('DE.SCREEN', 'DE', r'SCREEN%d', [0x31f65730], [ce('LoadScreenData')], '6b', 'DamageEffect sub-loaders'),
    ('DE.SOUND', 'DE', r'SOUND%d', [0x31f73f30], [ce('LoadSoundData')], '6b', 'DamageEffect sub-loaders'),
    ('DE.AFTER_IMAGE', 'DE', r'AFTER_IMAGE', [0x31f74a80], [ce('LoadAfterImage')], '6b', 'DamageEffect sub-loaders'),
    ('DE.TOUCH_LAND_DAMAGE_EFFECT', 'DE', r'TOUCH_LAND_DAMAGE_EFFECT(%d)?', [0x31f76a90], [ce('LoadTouchLandDamageEffect')], '6b', 'DamageEffect sub-loaders'),
    ('DE.HIT_EFFECT_SET', 'DE', r'HIT_EFFECT_SET%d', [0x31f64c80], [ce('LoadHitEffectSet')], '6b', 'DamageEffect sub-loaders'),
    ('DE.DIE_CREATE_NPC', 'DE', r'DIE_CREATE_NPC%d', [0x31f66230], [ce('LoadDieCreateNpc')], '6b', 'DamageEffect sub-loaders'),
    ('ES.templet', 'ES', None, None, [(ES_CPP, 'CX2EffectSet::AddEffectSetTemplet_LUA', ())], '2', 'EffectSet'),
    ('ES.EFFECT', 'ES', r'EFFECT%d', None, [(ES_CPP, 'CX2EffectSet::AddEffectSetTemplet_LUA', ('EFFECT%d',))], '3', 'EffectSet'),
    ('ES.CAMERA_SHAKE', 'ES', r'CAMERA_SHAKE%d', None, [(ES_CPP, 'CX2EffectSet::AddEffectSetTemplet_LUA', ('CAMERA_SHAKE%d',))], '3', 'EffectSet'),
    ('ES.SOUND', 'ES', r'SOUND%d', None, [(ES_CPP, 'CX2EffectSet::AddEffectSetTemplet_LUA', ('SOUND%d',))], '3', 'EffectSet'),
    # EffectSet_Description.lua: March reads it only under #ifdef EFFECT_TOOL (X2EffectSet.cpp:3380),
    # compiled out of the client; June's reader is not identified. Not in any phase.
    ('ES.Description', 'ESD', None, [], [(ES_CPP, 'CX2EffectSet::AddEffectSet_Description_LUA', ())], 'unassigned',
     'EffectSet_Description (effect-tool only)'),
    ('InsertDamageData', 'DD', None, [0x32621080], [], 'unassigned', 'InsertDamageData_LUA (Elsword_DamageData.lua)'),
    ('DamageSound', 'DS', None, [0x32610ff0], [(DM_CPP, 'CX2DamageManager::ParseDamageSoundBlock', ('*',))], 'unassigned', 'DamageSound'),
]
ROOT = dict((r[0], r) for r in ROOTS)
FILE_FAM = dict([(s, 'DE') for s in R.DE_FILES] + [(s, 'ES') for s in R.ES_FILES if s != 'EffectSet_Description'] +
                [('EffectSet_Description', 'ESD'), ('Elsword_DamageData', 'DD'), ('DamageSound', 'DS')])
FAM_TOP = {'DE': 'DE.top', 'ES': 'ES.templet', 'ESD': 'ES.Description', 'DD': 'InsertDamageData', 'DS': 'DamageSound'}
# Buff factors nested in DAMAGE_DATA are the buff system's parser, which no phase of the
# plan owns. Its March readers are matched path-insensitively over these TUs.
BUFF_TUS = ['X2BuffFactor.cpp', 'X2BuffBehaviorTemplet.cpp', 'X2BuffFinalizerTemplet.cpp', 'X2BuffTempletManager.cpp']
# ---- sub-table levels inside a parser function, by VA
ES_PARSER, DD_PARSER = 0x321e48e0, 0x32632390
LUA_OPEN_N, LUA_OPEN, LUA_CLOSE = 0x31c7b980, 0x32fe9940, 0x32fe9a10   # BeginTable(name,i) / BeginTable(name) / EndTable
LUA_OPEN_I = 0x32fe99b0   # BeginTable(int index): opens the i-th entry of a list table
# 321e48e0 by hand: a mechanical BeginTable/EndTable pass is fooled by its loops (an
# early-continue EndTable @321ec23e / 321eb2f3, then the loop-bottom re-test @321edd1c /
# 321ebbc5 of the table just closed). Each range runs from the BeginTable call to the
# loop-bottom re-test (numbered tables) or the EndTable (named ones); check_levels()
# verifies every start is a BeginTable on that name. Anything outside them is templet level.
ES_LEVELS = [
    (('EFFECT%d',), 0x321e5cf8, 0x321ec0cf),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST'), 0x321ea68d, 0x321ebbdc),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d'), 0x321ea6aa, 0x321ebbc5),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'BOUND_MOTION_TYPE_LIST'), 0x321eaa32, 0x321eaa99),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'TRIGGER_EFFECTSET_LIST'), 0x321eb0a7, 0x321eb295),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'ANIMTIME_ON_START_GAME'), 0x321eb79d, 0x321eb8dd),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'ANIMTIME_ON_START_SQUARE'), 0x321eb8ed, 0x321eba2d),
    (('EFFECT%d', 'MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'ANIMTIME_ON_START_UNITVIEWER'), 0x321eba3d, 0x321ebb80),
    (('EFFECT%d', 'MESH_TRIGGER_DATA'), 0x321ebbfc, 0x321ebf61),
    (('EFFECT%d', 'APPLY_DAMAGE_EFFECT_VELOCITY'), 0x321ebf87, 0x321ec01e),
    (('CAMERA_SHAKE%d',), 0x321ec0ee, 0x321edd1c),
    (('SOUND%d',), 0x321edd39, 0x321ee2a0),
]
# ---- runtime citations, by hand (Phase 4a on). The parse-time columns above say whether March
# reads a key; this says what June does with it at run time, and where. One entry per
# (scope root, sub-path, key); check_runtime() makes sure each names a real row. Verdicts:
# new / semantics-differ / same / unsupported / deferred / 4b@VA (only a Phase-4b reader) /
# not-creation (no reader on the creation path) / outside-effectset (read only by another
# subsystem, named in the branch). state: done / stage N (owned, not yet written) / deferred /
# - (nothing to write). The full spec is JUNE2017_EFFECT_REBUILD_PLAN.md, Phases 4a and 4b.
# Phase 4b replaced every '4b@VA' placeholder with a real verdict; check_runtime() fails on any
# that comes back, and reports every row still owned by a stage.
RT_4A = '4a'
RT_4B = '4b'
RT_4AB = '4a+4b'
# Phase 7a (DamageEffect per-frame: CEffect::OnFrameMove 31f88790, FollowLine 31f70ee0) adds
# 'not-per-frame': neither of those reads the key; its reader belongs to a later phase.
RT_7A = '7a'
RT_7B = '7b'
RT_7C = '7c'
RT_7D = '7d'
RT_7E = '7e'
RUNTIME = [
    # (root, rel, key, phase, state, verdict, June branch, March site)
    ('ES.EFFECT', (), 'ZOFFSET_USE_UNIT_IS_RIGHT', RT_4A, 'done', 'new',
     '321d9820@321d9b36-321d9c2a: game-unit Z offset subtracted while facing left', 'SetEffectSetOffset'),
    ('ES.EFFECT', (), 'POSITION_OFFSET_X', RT_4A, 'done', 'semantics-differ',
     '321d9820@321d9847-321d9889: facing unit = trace target when valid; @321d98a3-321d98c2: game-unit target-mesh branch unreachable in range (JBE)', 'SetEffectSetOffset'),
    ('ES.EFFECT', (), 'POSITION_OFFSET_Z', RT_4A, 'done', 'semantics-differ',
     '321d9820@321d9847-321d9889 and @321d98a3-321d98c2, as POSITION_OFFSET_X', 'SetEffectSetOffset'),
    ('ES.EFFECT', (), 'POSITION_OFFSET_Y', RT_4A, 'done', 'same',
     '321d9820@321d9b0b-321d9b23: added as is; the facing change does not reach Y', 'SetEffectSetOffset'),
    ('ES.EFFECT', (), 'BONE_TRACE_TARGET_MESH', RT_4AB, 'done', 'semantics-differ',
     '321ee600@321f1f76-321f20f4 (damage) / 321f21f7-321f2414 (mesh): every one pushed onto a target-mesh list; 321dcd30@321dcd9d: with INDEX -1 it traces no mesh itself. 4b: 321e4020@321e4140-321e419d deletes the set only when every entry\'s stored handle is -1; 321e0080@321e0170-321e021c hides an effect whose entry is gone (not for BTTM with INDEX < 0)', 'OnFrameMove_CreateEffect, GetEffectPosition, OnFrameMove_DeleteEffect, OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'BONE_TRACE_TARGET_MESH_INDEX', RT_4AB, 'done', 'new',
     'position 321dcd30@321dcd9d-321dce07 -> 321dcac0; rotation 321df7e0@321df80c-321df860; offset 321d9820@321d9889-321d9896 / 321d9e6a-321d9ea6. 4b: the same index rule in 321e0080@321e0170-321e01e9 (hide), GetCombineMatrix 321dc550@321dc5e0-321dc6a5 and REVERSE_Y @321e1547-321e1562', 'GetEffectPosition, GetEffectRotation, SetEffectSetOffset, OnFrameMove_UpdateEffect, GetCombineMatrix'),
    ('ES.EFFECT', (), 'TRACE_USER_TARGET_FORCE', RT_4AB, 'done', 'same',
     '321dcac0, 321df7e0@321df871, 321d9820@321d98e0: skips the target mesh, as March. 4b: 321e0080@321e01f6 (no hide), 321dc550@321dc5f8, REVERSE_Y @321e154e, as March', 'the same three, OnFrameMove_UpdateEffect, GetCombineMatrix'),
    ('ES.EFFECT', (), 'APPLY_UNIT_ROTATION', RT_4A, 'done', 'same',
     '321df7e0@321df860: as March, plus the target-mesh index', 'GetEffectRotation'),
    ('ES.EFFECT', (), 'APPLY_UNIT_ROTATION_MIRROR', RT_4A, 'done', 'same',
     '321df7e0@321df8f9-321df98c: as March, plus the target-mesh index', 'GetEffectRotation'),
    ('ES.EFFECT', (), 'LIMIT_DISTANCE', RT_4A, '-', 'same', '321ee600@321eedad-321eeddf', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'START_WHEN_TRACE_TARGET_DEAD', RT_4A, '-', 'same', '321ee600@321ee7e4-321eec83', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'USE_SUB_ATTACK_LIST_SET', RT_4A, '-', 'same', '321ee600@321f219f-321f21f2', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'TRACE_MATRIX', RT_4AB, 'done', 'semantics-differ', '321ee600, e.g. @321ef4f5-321ef58f: scale / unit scale / bone-trace scale (same). 4b: 321e0080@321e0fa4-321e146d skips it while the stop-tracing flag is set and adds the UI-major particle; GetCombineMatrix 321dc550 adds the index rule and the square-unit / pet frames', 'OnFrameMove_CreateEffect, OnFrameMove_UpdateEffect, GetCombineMatrix'),
    ('ES.EFFECT', (), 'USE_IS_RIGHT_WHEN_CREATE', RT_4A, 'done', 'new',
     '321ee600@321ef6b5-321ef6d5: CEffect +0x135 (= m_bFollowLineRight) = the facing snapshot taken by 321df230@321df51c-321df576', 'damage case, EffectSetInstance ctor'),
    ('ES.EFFECT', (), 'TRACE_MATRIX_IN_FIELD', RT_4AB, 'done', 'new',
     '321ee600 mesh @321ef8ee / 321efe84 / 321f03b6 / 321f088a, particle @321f0ce3 / 321f11da / 321f1738 / 321f1c12: BONE_NAME frame (owner: the mesh own / the set skin anim; else the target mesh, looked up in the UI-major player) -> SetUseDXMatrix + POSITION_OFFSET, or the bone position. 4b: per frame for UI-major particles only, 321e0080@321e0d1a-321e0f9b', 'mesh / particle cases, OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'APPLY_DAMAGE_EFFECT_VELOCITY', RT_4A, 'done', 'new',
     '321ee600@321ef68e-321ef6a0 -> 321db1b0 tag 1: main-effect m_vVelocity (mesh +0x534), add if [4]==0 else multiply; kept on the mesh (31c8a9b0) and re-applied by its velocity event (31c8a1b0 -> 31c89d60)', 'damage case, CXMeshEvent_Velocity'),
    ('ES.EFFECT', (), 'MESH_TRIGGER_DATA_LIST', RT_4AB, 'done', 'new',
     '321ee600@321f2130-321f2166: EffectInstance copies the list; mesh +0x194=0 (restore-after-trigger off; its reader is 31c87b80@31c87d0e, so 4b stage 3 stores it). 4b: 321e0080@321e02fc-321e0409 -> 321dfc40@321dfce0-321dffc4', 'mesh case, OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'SET_OVER_UI', RT_4A, 'done', 'semantics-differ',
     '321ee600@321f0f49-321f0f63 (x4): m_bOverUI (+0x170) and m_Layer (+0x164, from CKTDGObject ctor 31d28a20) = 500 XDL_DIRECTING', 'particle cases'),
    ('ES.EFFECT', (), 'GROUP_NAME', RT_4AB, 'done', 'new',
     '321ee600@321eee45-321eeeb7, loop 321eeec0-321ef0ce / 321f2550-321f257c, helper 321dd590 (-> 321dcc20): particles only; one particle per <base>_GROUPCHILD_<n> bone until one is missing (on the target mesh too, @321ef069); child flag / bone / BONE_NAME-frame offset kept on the EffectInstance (@321f2483-321f24f1) for 4b; the particle TRACE_MATRIX_IN_FIELD takes the child bone (@321f0d10). 4b: 321e0080@321e0825-321e0986 moves a child through GetCombineMatrix (BONE_NAME) or GetEffectPositionByBoneName. No June or March data file uses the key', 'OnFrameMove_CreateEffect, GetEffectPositionByBoneName, OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'NOT_ALLOW_MEMO', RT_4A, 'done', 'unsupported',
     '321dddb0@321dde5f-321dde89: scale x memo scale (EffectData +0x108) unless set; the memo scale comes only from APPLIED_SKILL_ID (unsupported); logged once per templet', 'GetEffectScale'),
    ('ES.EFFECT', (), 'APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION', RT_4A, 'done', 'unsupported',
     '321ee600@321f1ec8-321f1ee4: the effect value only when it is 1, else the instance +0x1bc -> damage +0xb32 / mesh 31c83620 / particle +0x1b0; logged once per templet', 'OnFrameMove_CreateEffect (log)'),
    ('ES.EFFECT', (), 'FOLLOW_UNIT_ROTATION', RT_4B, 'done', 'new',
     '321e0080@321e159a-321e1b05 (inside the trace block): GetUnitRotation -> mesh rotation 31c1a3a0 (damage main effect, meshes); particles 6/8 add it to their own rotation (31c1a030)', 'OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'FOLLOW_UNIT_ROTATION_CPARTICLE', RT_4B, 'done', 'new',
     '321e0080@321e1acc-321e1b00: with FOLLOW_UNIT_ROTATION, the same rotation on the particle\'s child particles (31cf1df0)', 'OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'REVERSE_Y', RT_4B, 'done', 'same',
     '321e0080@321e146d-321e159a: type 8 with a billboard; target-mesh facing when the index rule gives a live entry and !TRACE_USER_TARGET_FORCE, else the EffectInstance unit (vfunc 0x170): as March plus the index rule', 'OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'MESH_TRIGGER_DATA', RT_4B, 'done', 'new',
     '321e0080@321e02fc-321e0409 -> 321dfc40@321dffd0-321e0033: only with an empty list; the mesh restores the previous anim when the motion ends (31c87b80@31c87cf2-31c87d48)', 'OnFrameMove_UpdateEffect, CXMeshInstance'),
    ('ES.EFFECT', (), 'FOLLOW_UNIT_VIEWER_SCALE', RT_4A, '-', 'outside-effectset', 'read only by the unit viewer: 329585f0@32958a11, @329590c7; no reader in 0x321d0000-0x32208000 except the parser', '-'),
    ('ES.EFFECT', (), 'USE_EFFECT_SCALE_IN_UNIT_VIEWER', RT_4A, '-', 'outside-effectset', 'read only by the unit viewer: 32957980@32957e8e; no reader in 0x321d0000-0x32208000 except the parser', '-'),
    ('ES.templet', (), 'OVERLAP', RT_4A, 'done', 'new',
     '321d9f30 from the factory 321e1bf0@321e1c68-321e1c77: no new instance while one of the templet lives with the same trace target', '_CreateEffectSetInstance'),
    ('ES.templet', (), 'STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE', RT_4A, 'deferred', 'new',
     '321da070 via unit 3228a120 over unit+0x3e90 on state change; the writer of that list is not located', 'unit side'),
    ('ES.templet', (), 'SYNC_SHOW_BY_CREATE_UNIT', RT_4B, 'done', 'new',
     '321e0080@321e077d-321e07e9, per effect: the owner\'s GetShowObject() against the set\'s show flag -> SetShowObject on the whole set (321de4f0); at creation particles take the set flag (321ee600@321f251d)', 'OnFrameMove_UpdateEffect, creation'),
    ('ES.templet', (), 'APPLIED_SKILL_ID', RT_4A, 'done', 'unsupported',
     '321e1e60@321e1f51-321e1f95 (user owners only) -> 31fdbbb0 (user +0x867c memo) -> EffectSetData +0x60 -> instance +0x150 (321df230); logged once per templet', 'PlayEffectSet (log)'),
    ('ES.templet', (), 'APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION', RT_4A, 'done', 'unsupported',
     '321e1bf0@321e1dd7-321e1e0c -> instance +0x1bc; when 0, 322d4d10 derives it from the game options for every set; logged once per templet', '_CreateEffectSetInstance (log)'),
    ('ES.templet', (), 'EVENT_TIMER_TYPE', RT_4A, '-', 'same', '321ee600@321ee885-321ee96d', 'OnFrameMove_CreateEffect'),
    ('ES.templet', (), 'AFFECTED_BY_UNIT_SCALE', RT_4A, '-', 'same',
     'no creation-path reader in June; March\'s is compiled out (#ifndef FIX_EFFECT_SCALE_BY_UNIT_SCALE)', 'PlayEffectSet'),
    ('ES.templet', (), 'DELETE_EFFECTSET_AT_TRACE_UNIT_DIE', RT_4B, 'done', 'new',
     '321e4020@321e41a2-321e4231: the templet flag or the instance\'s two trace-unit flags; deleted when the trace target is null or its HP is not above 0', 'OnFrameMove_DeleteEffect'),
    ('ES.templet', (), 'DELETE_SHAKE_COUNT', RT_4B, '-', 'same',
     '321e4020@321e42ec-321e43ee: limit = templet + instance +0xcc (no writer in March, 0); a user trace target\'s facing flip adds one; deleted at the limit', 'OnFrameMove_DeleteEffect'),
    # existing keys the creation helpers consume the same way as March
    ('ES.EFFECT', (), 'NAME', RT_4A, '-', 'same',
     '321ee600@321ef3e8 (damage; likewise @321ef6f8, 321efcb9, 321f018f): HYPER_NAME if the instance is hyper and it is non-empty, else NAME', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'HYPER_NAME', RT_4A, '-', 'same', '321ee600@321ef3e8 etc., as NAME', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'EFFECT_TYPE', RT_4A, '-', 'same',
     '321ee600@321ef212-321ef222 (creation switch), @321f1eeb-321f1efe (EffectInstance switch)', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'START_TIME', RT_4A, '-', 'same', '321ee600@321ee7e4-321eec83: START_TIME + delay against the timer', 'OnFrameMove_CreateEffect'),
    ('ES.EFFECT', (), 'BONE_NAME', RT_4A, '-', 'same',
     '321dcd30@321dcedf-321dcf11 (owner), @321dd054-321dd0fe (trace target): _HEAD_BONE, else GetBonePos with the head-bone fallback for the owner', 'GetEffectPosition'),
    ('ES.EFFECT', (), 'WEAPON_BONE_NAME', RT_4A, '-', 'same', '321dcd30@321dd302-321dd31f, @321dd3ef-321dd41d', 'GetEffectPosition'),
    ('ES.EFFECT', (), 'WEAPON_BONE_WEAPON_INDEX', RT_4A, '-', 'same', '321dcd30@321dd31f, @321dd41d', 'GetEffectPosition'),
    ('ES.EFFECT', (), 'WEAPON_BONE_MODEL_INDEX', RT_4A, '-', 'same', '321dcd30@321dd319, @321dd417', 'GetEffectPosition'),
    ('ES.EFFECT', (), 'RIDING_PET_BONE_NAME', RT_4A, '-', 'same', '321dcd30@321dcf79 (owner branch only)', 'GetEffectPosition'),
    ('ES.EFFECT', (), 'USE_LAND_POSITION', RT_4A, '-', 'same', '321dcd30@321dcf11, 321dcfc4, 321dd086, 321dd34a: land pos else pos, per holder', 'GetEffectPosition'),
    ('ES.EFFECT', (), 'ROTATE_X', RT_4A, '-', 'same', '321df7e0@321df7f0-321df7fe', 'GetEffectRotation'),
    ('ES.EFFECT', (), 'ROTATE_Y', RT_4A, '-', 'same', '321df7e0@321df7f0-321df7fe', 'GetEffectRotation'),
    ('ES.EFFECT', (), 'ROTATE_Z', RT_4A, '-', 'same', '321df7e0@321df7f0-321df7fe', 'GetEffectRotation'),
    ('ES.EFFECT', (), 'APPLY_INSTANCE_SCALE', RT_4A, '-', 'same', '321dddb0@321dde35-321dde3a: instance scale when damage-effect scale, or this and not ET_DAMAGE_EFFECT', 'GetEffectScale'),
    ('ES.EFFECT', (), 'TRACE', RT_4B, '-', 'same', '321e0080@321e07fc-321e081f: untraced or dead effects are not moved, as March', 'OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'TRACE_MORE', RT_4B, '-', 'same', '321e0080@321e0805; particles SetPosition( pos, TRACE_MORE, TRACE_MORE ) @321e0cb7-321e0cca, as March', 'OnFrameMove_UpdateEffect'),
    ('ES.EFFECT', (), 'PASSIVE_EFFECT', RT_4B, '-', 'same', '321e4020@321e47c5-321e47d7: an alive effect keeps the set alive unless passive, as March', 'OnFrameMove_DeleteEffect'),
    ('ES.EFFECT', (), 'TRACE_MATRIX_EXCLUDE_SCALE', RT_4A, '-', 'not-creation',
     'parse time only: sets TRACE_MATRIX (321e48e0@321e760b-321e7610); no runtime reader in the EffectSet region (the +0xc8 in 321e4020@321e4316 is the instance\'s delete-shake count)', '-'),
    ('ES.templet', (), 'NAME', RT_4A, '-', 'same', 'GetEffectSetTemplet 321da420, the first call of PlayEffectSet 321e1e60', 'PlayEffectSet'),
    ('ES.templet', (), 'EFFECT%d', RT_4A, '-', 'same', 'container: each entry is one pass of 321ee600\'s per-effect loop', 'OnFrameMove_CreateEffect'),
    ('ES.templet', (), 'CAMERA_SHAKE%d', RT_4B, '-', 'same', '321dbcc0: every CAMERA_SHAKE%d entry, not yet done, is timed, distance-checked and started, as March', 'OnFrameMove_StartCameraShake'),
    ('ES.templet', (), 'SOUND%d', RT_4B, 'done', 'new',
     '321e0080@321e07ee-321e07f7 -> 321dee80, per effect instance (none created: nothing plays); per-instance copy, played entries dropped unless LOOP; the dtor stops the rest (321e3a80 -> 321da200)', 'OnFrameMove_UpdateEffect, dtor'),
] + [('ES.SOUND', (), k, RT_4B, 'done', 'new', b, 'OnFrameMove_UpdateEffect') for k, b in (
    ('EVENT_TIME', '321dee80@321def5e-321defaf: USE_GLOBAL_TIME against the elapsed time, else the set skin anim EventTimer (31d28e10)'),
    ('FILE_NAME', '321dee80@321def0c-321df0a5: opened and played at the pet / owner / square-unit position'),
    ('RATE', '321dee80@321def0c-321def1a: rand() % 100 < RATE, re-rolled every call until it plays'),
    ('USE_GLOBAL_TIME', '321dee80@321def5e: elapsed time instead of the anim EventTimer'),
    ('LOOP', '321dee80@321df0a8-321df0cd: re-armed on m_bAnimEnd when not USE_GLOBAL_TIME; without LOOP a played entry is dropped'))] + [
    ('ES.CAMERA_SHAKE', (), k, RT_4B, '-', 'same', b, 'OnFrameMove_StartCameraShake') for k, b in (
    ('START_TIME', '321dbcc0@321dbda3-321dc05a: START_TIME + delay against the custom timer or the pet / riding-pet / set skin anim time'),
    ('LIMIT_DISTANCE', '321dbcc0@321dc0b2-321dc160: squared distance to GetMyUnit(); -1 ignores it; with no my-unit only -1 fires'),
    ('SHAKE_TYPE', '321dbcc0@321dc16b: switch 1-9 onto the nine CKTDGCamera calls (31d014c0 = Up, 31d01490 = Down, by March\'s member order)'),
    ('ONE_DIR_SPEED', '321dbcc0@321dc18c-321dc2fb: types 1, 2, 5, 6'), ('ONE_DIR_ACCEL', '321dbcc0@321dc18c-321dc2fb: types 1, 2, 5, 6'),
    ('TWO_DIR_GAP', '321dbcc0@321dc210-321dc386: types 3, 4, 7, 8'), ('TWO_DIR_TIME', '321dbcc0@321dc210-321dc386: types 3, 4, 7, 8'),
    ('TWO_DIR_TIME_GAP', '321dbcc0@321dc210-321dc235, 321dc31f-321dc344: types 3, 7'),
    ('FIXED_LOOK_VEC', '321dbcc0@321dc3a7-321dc3fe: type 9'), ('EYE_SHAKE_COUNT', '321dbcc0@321dc3a7-321dc3fe: type 9'),
    ('EYE_MOVE_SPEED_MIN', '321dbcc0@321dc3a7-321dc3fe: type 9'), ('EYE_MOVE_SPEED_MAX', '321dbcc0@321dc3a7-321dc3fe: type 9'),
    ('EYE_MOVE_RANGE_MIN', '321dbcc0@321dc3a7-321dc3fe: type 9'), ('EYE_MOVE_RANGE_MAX', '321dbcc0@321dc3a7-321dc3fe: type 9'),
    ('LOOK_AT_SHAKE_COUNT', '321dbcc0@321dc3a7-321dc3fe: type 9'), ('LOOK_AT_MOVE_SPEED_MIN', '321dbcc0@321dc3a7-321dc3fe: type 9'),
    ('LOOK_AT_MOVE_SPEED_MAX', '321dbcc0@321dc3a7-321dc3fe: type 9'), ('LOOK_AT_MOVE_RANGE_MIN', '321dbcc0@321dc3a7-321dc3fe: type 9'),
    ('LOOK_AT_MOVE_RANGE_MAX', '321dbcc0@321dc3a7-321dc3fe: type 9'))] + [
    ('ES.EFFECT', ('MESH_TRIGGER_DATA',), k, RT_4B, 'done', 'new', b, 'OnFrameMove_UpdateEffect') for k, b in (
    ('TRIGGER_TIME', '321dfc40@321dffd8-321e0009: >= 0 and reached by the set skin anim time; not again while the mesh +0x634 is set'),
    ('MOTION_NAME', '321dfc40@321e000b-321e002e: played through 31c89ab0 at the mesh default speed (+0x3d4)'),
    ('PLAY_TYPE', '321dfc40@321e0017: passed to 31c89ab0'))] + [
    ('ES.EFFECT', ('MESH_TRIGGER_DATA_LIST',), 'TRIGGER_DATA%d', RT_4B, 'done', 'new',
     'container: 321dfc40@321dfce0-321dffc4 walks the EffectInstance copy, 0x98 bytes an entry; fired-once entries (+0x72) are skipped', 'OnFrameMove_UpdateEffect')] + [
    ('ES.EFFECT', ('MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d'), k, RT_4B, st, v, b, ms) for k, st, v, b, ms in (
    ('BOUND_MOTION_TYPE_LIST', 'done', 'new', '321dfc40@321dfd02-321dfdde: the owner\'s emotion (user +0x89d8 / square +0x608, normalised by 3264ca10) must be in the list', 'OnFrameMove_UpdateEffect'),
    ('TRIGGER_TIME', 'done', 'new', '321dfc40@321dfde0-321dfe6f: armed (a new anim, or the play count changed near the anim end), then TRIGGER_TIME reached', 'OnFrameMove_UpdateEffect'),
    ('MOTION_NAME', 'done', 'new', '321dfc40@321dfed2-321dfef8: played through 31c89ab0 at speed 1', 'OnFrameMove_UpdateEffect'),
    ('USE_MESH_TRANSITION', 'done', 'new', '321dfc40@321dfede: passed to 31c89ab0', 'OnFrameMove_UpdateEffect'),
    ('TRIGGER_ONCE', 'done', 'new', '321dfc40@321dff20-321dff2a: marks the entry fired-once (+0x72)', 'OnFrameMove_UpdateEffect'),
    ('STOP_TRACING_ON_TRIGGER', 'done', 'new', '321dfc40@321dff2e-321dff3c: sets the instance stop-tracing flag, which stops TRACE_MATRIX (321e0080@321e0fb5)', 'OnFrameMove_UpdateEffect'),
    ('TRIGGER_EFFECTSET_LIST', 'done', 'new', '321dfc40@321dff40-321dff83: each name through PlayEffectSetByMeshPlayer (321df9c0) on this mesh, type 2 always', 'OnFrameMove_UpdateEffect'),
    ('EFFECT_SET_TOGGLE_VISIBILITY_TIMER', 'done', 'new', '321dfc40@321dff89-321dff96 arms it; 321e0080@321e0105-321e0140 counts down and flips the set show flag (321de4f0)', 'OnFrameMove_UpdateEffect'),
    ('BLOCK_TRIGGER', 'done', 'new', '321dfc40@321dff99-321dffb1: blocks the mesh (+0x635); a blocked mesh runs no trigger (@321dfc8b)', 'OnFrameMove_UpdateEffect, CXMeshInstance'),
    ('BLOCK_TRIGGER_TIME', 'done', 'new', '321dfc40@321dff9f-321dffb1 -> mesh +0x638, counted down by 31c87b80@31c87bc4-31c87c20', 'CXMeshInstance'),
    ('ANIMTIME_ON_START_GAME', 'done', 'new', 'sub-table: 321dfc40@321dfe75-321dfe91 on a game-unit match', 'OnFrameMove_UpdateEffect'),
    ('ANIMTIME_ON_START_SQUARE', 'done', 'new', '321dfc40@321dfe93-321dfea9 on a square-unit match: TIME is the start time while its first byte is set; ONCE clears that byte', 'OnFrameMove_UpdateEffect'),
    ('ANIMTIME_ON_START_UNITVIEWER', '-', 'outside-effectset', '321dfc40@321dfeab-321dfeb8 runs only with no instance, i.e. from the unit viewer 329585f0@32958b12', '-'))] + [
    ('ES.EFFECT', ('MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'ANIMTIME_ON_START_GAME'), k, RT_4B, 'done', 'new', b, 'OnFrameMove_UpdateEffect') for k, b in (
    ('TIME', '321dfc40@321dfec5-321dfec8: the start time of the motion while the first byte (+0x81) is set, else 0'),
    ('ONCE', '321dfc40@321dfec2-321dfecf: first byte = !ONCE after the first use'))] + [
    ('ES.EFFECT', ('MESH_TRIGGER_DATA_LIST', 'TRIGGER_DATA%d', 'TRIGGER_EFFECTSET_LIST', '[]'), 'EFFECT_SET_NAME', RT_4B, 'done', 'new',
     '321dfc40@321dff4f-321dff78: PlayEffectSetByMeshPlayer( name, mesh handle, 2 )', 'OnFrameMove_UpdateEffect')] + [
    ('ES.EFFECT', (), k, RT_4A, '-', 'same', '321dddb0@321dddc7-321dde10: SCALE if x != 0; SCALE_*_ON_LEFT when not right', 'GetEffectScale')
     for k in ('SCALE_X', 'SCALE_Y', 'SCALE_Z', 'SCALE_X_ON_LEFT', 'SCALE_Y_ON_LEFT', 'SCALE_Z_ON_LEFT')] + [
    ('DE.top', (), 'TRACE_OWNER_BONE', RT_7A, 'done', 'new',
     '31f88790@31f88943-31f889e9: every frame the main effect moves to that bone of the owner (else g_pX2Game->GetMyUnit(), CX2Game slot 0x98): GetBonePos bool form (unit slot 0x80), else GetPos( false ) (slot 0x138)', 'CEffect::OnFrameMove'),
    ('DE.top', (), 'TRACE_OWNER_BONE_ONLY_ROTATE', RT_7A, 'done', 'unsupported',
     '31f88790@31f889e9-31f88a8b: only a GUT_USER owner (+0x264) of unit type 11 = UT_ROSE (31e4d0e0); rotation = ( own x, GetYRotateDegree (slot 0x158), user +0x9630 ), the string is never a bone name. Rose absent: logged once per templet', 'CEffect::OnFrameMove (log)'),
    ('DE.top', (), 'USE_OWNER_ATTACK_SPEED', RT_7A, 'done', 'new',
     '31f88790@31f8a00b-31f8a164: once (cache +0x85c/+0x860, ctor -1.0), GUT_USER owner only; f = GetAnimSpeedValue (June ResetAnimSpeed 31ffe190@31ffe1e3); f > 1: start * (2 - f), clamped 0; f < 1: end * (2 - f), the original end if negative (x87 emulated)', 'CEffect::OnFrameMove'),
    ('DE.top', (), 'USE_GLOBAL_TIME', RT_7A, 'done', 'same',
     '31f88790@31f8a166-31f8a201: March test against the global time (mesh +0x3e0) or the anim time, on the USE_OWNER_ATTACK_SPEED window; June DamageCheck 3262f5a0 takes three more args (0, -1, -1): Phase 9', 'CEffect::OnFrameMove'),
    ('DE.top', (), 'REFRESH_LAND_POS', RT_7A, 'done', 'semantics-differ',
     '31f88790@31f8a816-31f8a956: as March; @31f8a95c-31f8aa21: the crash-land update is skipped when the probe returns y == -1.0 (emulated). June probe 31c7bb00 has one more leading bool arg (1): line map, not ported', 'CEffect::OnFrameMove'),
    ('DE.top', (), 'FOLLOW_LINE_WITH_CUSTOM_ROTATE', RT_7A, 'done', 'new',
     '31f70ee0@31f711b8-31f7121d: the line angle keeps the main effect x/z rotation (m_vRotate +0x5b4/+0x5bc) instead of 0', 'FollowLine'),
    ('DE.top', (), 'FOLLOW_LINE_GRAVITY', RT_7A, 'deferred', 'new',
     '31f70ee0@31f71413-31f717e5 done: with FOLLOW_ON_LINE and no line below, snap to the nearest point of every enabled non-LT_WALL/LT_MONSTER_ROAD line, +100 y, else m_BeforePos. @31f71359-31f713c6 DEFERRED (Phase 10): within 30 of the line June adds 20 to y and clears the mesh move-enable flag (mesh +0x633, no March member)', 'FollowLine'),
    ('DE.top', (), 'IGNORE_MI_HUGE', RT_7A, 'deferred', 'new',
     '31f7aab0@31f7c795-31f7c83e (ctor, not per frame): when false, 31f6fdc0( s, s, s ) with s = +0x7d0 (m_DamageData +0x5d0), copied to +0xb24..; no writer of that field found (SetDamageDataFromLUA 32632390 never stores +0x5d0)', 'CEffect::CEffect (comment)'),
    ('DE.top', (), 'APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION', RT_7A, 'done', 'unsupported',
     '31f88790@31f88928-31f88943: when != 1, handed to the main effect every frame (CXMeshInstance 31c83620); effect-alpha option absent: logged once per templet when set', 'CEffect::OnFrameMove (log)'),
    ('DE.top', (), 'DAMAGE_DATA_CHANGE_TIME', RT_7A, 'done', 'semantics-differ',
     '31f88790@31f8a394-31f8a412: array entry [counter +0x8f8] against the skin EventTimer while the counter is inside the array and the NEXT chain (+0xd8) -> SetDamageDataNext 31f799c0 by index, counter++, CLEAR_HIT_UNIT_ON_DAMAGE_DATA_CHANGE; dormant on scalar data', 'CEffect::OnFrameMove, SetDamageDataNextByChainIndex'),
    ('DE.top', (), 'DAMAGE_DATA_CHANGE_TIME_GLOBAL_TIME', RT_7A, 'done', 'semantics-differ',
     '31f88790@31f8a412-31f8a490: the same with EventTimerGlobal (31c82d60) and the same counter, after the anim array', 'CEffect::OnFrameMove, SetDamageDataNextByChainIndex'),
    # REACT_ANIMSPEED / USE_ADVANCE_COLLISIOIN_CHECK were 7a 'not-per-frame' rows; Phase 7d owns them now.
    ('DE.top', (), 'REACT_ANIMSPEED', RT_7D, '-', 'same',
     'ctor 31f7aab0@31f7de2d stores +0xab8 (no per-frame reader, 7a); AttackResult 31f85910@31f86ad5-31f86afc: > 0 with a main mesh sets its anim speed (mesh +0x3d4) after the reActResult switch, as March SKILL_CASH_10_TEST (ON)', 'AttackResult (existing)'),
    ('DE.top', (), 'USE_ADVANCE_COLLISIOIN_CHECK', RT_7D, 'deferred', 'new',
     'ctor 31f7aab0@31f7c4d3 stores +0xb31; EffectToUnit 3262b6e0@3262bc1c loads it into a local at the head of the inlined CollisionDataCheck (main-effect attack list x defender collision set); its one real read @3262e976 picks a segment-style branch in the per-sphere test (3262e981-3262ec5d, 31c7b640). The slot is reused as the miss flag @3262f1ad/@3262f1c0. Phase 9 (collision)', '-')] + [
    # Phase 7b (lock-on: OnFrameMove_LockOnData 31f83aa0, VectorLockOn 31f73a10, the load-time tail of
    # LoadLockOnData 31f71930, the picks 322dadd0 / 322d7670 / 322daa10, the EffectToUnit list fill
    # 3262b6e0). 'log: Phase 11' = no deployed LOCK_ON_DATA carries the key, so its JUNE_FX CE_LOCKON
    # line is checked on June's own data, as Phases 2 and 3 did.
    ('DE.LOCK_ON_DATA', (), k, RT_7B, st, v, b, ms) for k, st, v, b, ms in (
    ('LOCK_ON_TYPE', 'done', 'semantics-differ', '31f83aa0 jump table 31f858e4 (8 cases): 1-3, 5, 7 as March; 4 LOT_UID_VECTOR re-targets through the seeded pick 322dadd0 (index (seed % 30 + 1) % count over owner-range-filtered candidates, not March GetLockOnTarget\'s walk); 6 see NOT_TARGET_PRE_LOCK_ON_TARGET; 8 LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE is June-only (enum appended = 8; log: Phase 11, March Enum.lua lacks it)', 'OnFrameMove_LockOnData, JunePickLockOnTargetBySeed'),
    ('CURVE_SPEED', 'done', 'same', '31f73a10@31f73cf8: now += (rel - now) * CURVE_SPEED * m_fElapsedTime, as March', 'VectorLockOn'),
    ('IN_DISTANCE_TO_DIE_EFFECT', 'done', 'same', '31f73a10@31f73a6c-31f73a9a: as March (handle -1 inside the distance)', 'VectorLockOn'),
    ('TIME_UNLOCK_FROM_START', 'done', 'semantics-differ', '31f83aa0@31f83b70-31f83bcf: counts down and returns, as March; new: the frame it runs out copies the effect position into m_vecSaveOwnerPos (June +0x994, the ONLY_FRONT origin)', 'OnFrameMove_LockOnData'),
    ('TIME_UNLOCK_AFTER_START', 'done', 'same', '31f83aa0@31f83bd4-31f83c07: as March (UPGRADE_RAVEN m_fUnLockTime)', 'OnFrameMove_LockOnData'),
    ('TARGET_RANGE', 'done', 'same', '31f83aa0@31f85384-31f853a6 / @31f85549-31f8556b: scales the died / no-target range of case 6 only, as March', 'OnFrameMove_LockOnData'),
    ('NOT_TARGET_PRE_LOCK_ON_TARGET', 'done', 'semantics-differ', 'int NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE (March TRUE = 1 = CLEAR). Case 6 picks through 322d7670 with the per-effect list (June +0x960) when non-zero; EffectToUnit 3262b6e0@3262f389/@3262f4b9 inserts every unit the effect hits; on a miss CLEAR empties it, SAVE_ONE when > 1, SAVE_HIT_UNIT also skips its last pick (+0x9d8). Types 2-4 steer ahead on a miss (31f8497c). Replaces March\'s game-wide pick list (BALANCE_DEADLY_CHASER)', 'OnFrameMove_LockOnData, JuneGetLockOnNearstTargetInRange, JuneOnLockOnHitUnit'),
    ('DUNGEON_TARGET_RANGE_MIN_MAX', 'done', 'semantics-differ', '31f83aa0@31f851b0-31f851d4 (and the other case 6 / 8 picks): the in-range pick range outside XS_PVP_GAME; March hardcodes 150/750 (the June default)', 'JuneGetLockOnTargetRange'),
    ('PVP_TARGET_RANGE_MIN_MAX', 'done', 'semantics-differ', 'the same in XS_PVP_GAME (June state 9 selects +0x978); March hardcodes 150/300 (the June default)', 'JuneGetLockOnTargetRange'),
    ('UNLOCK_START_AFTER_ATTACK', 'done', 'new', '31f83aa0@31f83c0d-31f83c26: no case runs until m_DamageTimeNow (+0x850) reaches it, then -1 (log: Phase 11)', 'OnFrameMove_LockOnData'),
    ('RETARGET_COOLTIME_AFTER_ATTACK', 'done', 'new', 'set into +0x984 by the hit-count re-targets of cases 6 / 8 (@31f851c6, @31f846a4); 31f83aa0@31f83c2c-31f83c45 counts it down and runs no case meanwhile (log: Phase 11)', 'OnFrameMove_LockOnData'),
    ('ONLY_FRONT', 'done', 'same', '31f73a10@31f73aad-31f73b72: as March (FINALITY_SKILL_SYSTEM); its origin is now also refreshed by TIME_UNLOCK_FROM_START', 'VectorLockOn'),
    ('NOT_FIND_TARGET_MOVE_STOP', 'done', 'same', '31f83aa0@31f84e6f-31f84ec9 / @31f85628-31f85686 (case 6) and the same in case 8: as March (BALANCE_PATCH_20131107)', 'OnFrameMove_LockOnData'),
    ('Y_DIR_INIT', 'done', 'new', '31f73a10@31f73d77-31f73db5: once, the new heading degree y *= -1.0 (fmul QWORD 33436c90) (log: Phase 11)', 'VectorLockOn'),
    ('LOCK_ON_OFFSET_Y_POS', 'done', 'new', '31f83aa0@31f84644-31f8465c: added to the case-8 aim point y (binary-only key)', 'OnFrameMove_LockOnData'),
    ('LOCK_ON_TARGET_BONE_NAME', 'done', 'semantics-differ', '31f71930@31f73109 reads it into the one member case 7 uses (31f856fe); March never assigned m_wstrTargetBoneName, so LOT_UID_BONE_POS always fell back to GetPos (log: Phase 11)', 'LoadLockOnDataBlock'),
    ('RANDOM_SEED', 'done', 'semantics-differ', '31f71930@31f7355c-31f73647: one member (+0x948) that the re-target picks read (@31f84106, @31f84489); Phase 6b\'s separate member now feeds m_iRandomSeed at load (log: Phase 11)', 'LoadLockOnDataBlock'),
    ('INIT_TARGET', 'done', 'new', '31f71930@31f73732-31f737e2: with a live owner, an immediate seeded pick (322dadd0, out bool starts true) at load (log: Phase 11)', 'LoadLockOnDataBlock'),
    ('NOT_LOCKON_INVINCIBLE_TARGET', 'done', 'new', '31f83aa0@31f8407a-31f840a9 via 31f63d80: case 4 also re-targets when the live target is invincible (log: Phase 11)', 'OnFrameMove_LockOnData'),
    ('AT_RAID_FIELD', 'done', 'new', '31f71930@31f7323f-31f73364: XS_BATTLE_FIELD when 32481f80( 32482650() ) (field type 2..4), re-expressed as GetIsBossRaidCurrentField -- that mapping is a candidate', 'JuneSelectLockOnRangeFromOwner'),
    ('LOCK_ON_RANGE_FROM_OWNER', 'done', 'new', 'selected at load (31f71930@31f73130-31f73540, into +0x9d0; without the table 1000 in a non-raid battle field, else 99999); 31f647c0 skips the whole VectorLockOn and drops candidates of 322dadd0 / 322daa10 whose aim point (slot 0x3e4) is farther from the owner', 'JuneSelectLockOnRangeFromOwner, JuneIsOutOfLockOnRangeFromOwner'))] + [
    ('DE.LOCK_ON_DATA', ('LOCK_ON_RANGE_FROM_OWNER',), k, RT_7B, 'done', v, b, 'JuneSelectLockOnRangeFromOwner') for k, v, b in (
    ('AT_DUNGEON', 'new', '31f71930@31f7315d-31f734d8: any state but XS_PVP_GAME / XS_BATTLE_FIELD (June\'s XS_PVE_GAME row, 5000 and no read, has no March state)'),
    ('AT_FIELD', 'new', '31f71930@31f73369-31f73433: XS_BATTLE_FIELD, not a raid field'),
    ('AT_PVP', 'new', '31f71930@31f73438-31f734d8: XS_PVP_GAME'))] + [
    # Phase 7c (the CREATE_DAMAGE_EFFECT loop inside CEffect::OnFrameMove 31f88790, the die-condition
    # evaluator 31f765b0 / 31f76010). Phase 7c wrote these into the generated matrix by hand; Phase 7d
    # moved them here so a census rerun keeps them. INHERIT_DAMAGE is Phase 7d's (below).
    ('DE.CREATE_DAMAGE_EFFECT', (), k, RT_7C, 'done', v, b, 'OnFrameMove (CREATE_DAMAGE_EFFECT loop)') for k, v, b in (
    ('APPLY_ROTATED_OFFSET', 'same', 'existing bApplyRotatedOffset branch, always compiled'),
    ('APPLY_ROTATE_ORIGIN', 'same', 'existing, always compiled'),
    ('AUTO_DIE', 'same', 'existing, ADD_GAME_STAGE_DELETE_DAMAGEEFFECT ON'),
    ('INHERIT_BUFF_FACTOR', 'same', 'existing, ADDITIONAL_MEMO ON'),
    ('INHERIT_LIFE_TIME', 'new', '31f89dc1-31f89dfc: the parent main effect\'s GetNowLifeTime() onto the child main effect'),
    ('LOOP_ADD_POS_X', 'new', '31f89b9d-31f89c1f: world space, count * addPos from the base position every spawn'),
    ('LOOP_ADD_POS_Y', 'new', 'the same formula as X'),
    ('LOOP_ADD_POS_Z', 'new', 'the same formula as X / Y (binary-only key)'),
    ('LOOP_END_COUNT', 'new', '31f89f15-31f89f27: uiCreateCount >= uiLoopEndCount deletes the entry'),
    ('LOOP_END_TIME', 'new', '31f8932f-31f89342: mesh GetNowLifeTime() >= fLoopEndTime expires the entry'),
    ('LOOP_INTERVAL', 'new', '31f893c3-31f893f7: fEventTime + fLoopInterval * uiCreateCount'),
    ('SET_DIE_WHEN_MASTER_EFFECT_DIE', 'new', '31f89e77-31f89e95: the child handle onto the parent\'s m_vecDieWithMasterChildEffects; destroyed non-silently by DestroyInstanceHandle (31f83830@31f838c5 -> 31f886c0); a parent that runs out of life leaves them'),
    ('SET_GROUP_ID', 'same', 'existing, SERV_RAVEN_VETERAN_COMMANDER ON'))] + [
    ('DE.DIE_CHECK_CONDITION', (), 'AND_OPERATION', RT_7C, 'done', 'same',
     '31f765b0@31f76602: JuneEvaluateDieCheckCondition ANDs its entries', 'JuneEvaluateDieCheckCondition'),
    ('DE.DIE_CHECK_CONDITION', (), 'OR_OPERATION', RT_7C, 'done', 'same',
     '31f765b0@31f76609-31f76614: ORed together, then ANDed into the result only when non-empty', 'JuneEvaluateDieCheckCondition')] + [
    ('DE.DIE_CHECK_CONDITION', ('AND_OPERATION', 'CONDITION_%d'), k, RT_7C, 'done', v, b, 'JuneEvaluateDieCheckCondition') for k, v, b in (
    ('CHECK_DAMAGE_TIME_VALUE', 'same', '31f760cc: compared to m_DamageTimeNow as (int)value (DEDCT_DAMAGE_TIME)'),
    ('DAMAGE_EFFECT_DIE_CONDITION_TYPE', 'same', 'DEDCT_DAMAGE_TIME = 0 -> 31f760cc, DEDCT_LOCK_ON_TARGET = 1 -> 31f76025'),
    ('IS_EXIST', 'new', '31f76025-31f760b2: against whether m_LockOnData\'s unit / NPC UID still resolves (GetUserUnitByUID / GetNPCUnitByUID; DEDCT_LOCK_ON_TARGET)'),
    ('IS_OVER', 'new', '31f760d4-31f760f3: selects > vs < against CHECK_DAMAGE_TIME_VALUE'))] + [
    # Phase 7d (hit response: AttackResult 31f85910, DamageReact 31f6f600 -- March's line for line, no
    # key --, the hit / die spawners 31f810c0 / 31f82210 behind 31f831e0 / 31f83400, the INHERIT_CRITICAL_*
    # helper 31f8af50, the child-state copy 31f8fdb0, EffectToUnit 3262b6e0). No deployed DamageEffect.lua
    # carries RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT, INHERIT_CRITICAL_*, INHERIT_DAMAGE or INHERIT_LIFE_TIME
    # (packed Sep 27 file, constant scan); REACT_ANIMSPEED x2, ONLY_RELATIONSHIP x1 and hit / die SCALE do.
    ('DE.top', (), 'RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT', RT_7D, 'done', 'new',
     '31f85910@31f881b6-31f881fe: after m_DamageTimeNow++ (+0x850), before the DAMAGE_TIME die test, rate >= 0 (0 restarts the clock): main effect now-life (mesh +0x448) = max-life (+0x44c) * rate; -1 skips. Out-of-line copy 31f75fc0 has no caller', 'AttackResult'),
    ('DE.top', (), 'ONLY_RELATIONSHIP', RT_7D, 'done', 'semantics-differ',
     '3262b6e0@3262ba35-3262ba49: a defender that is not GUT_USER (+0x264 != 0) is skipped outright, where March static_casts any unit to CX2GUUser; the relationship-UID test @3262ba4f-3262ba93 is March\'s (ADDED_RELATIONSHIP_SYSTEM ON)', 'CX2DamageManager::EffectToUnit'),
    ('DE.top', (), 'INHERIT_CRITICAL_RATE', RT_7D, 'done', 'new',
     '31f8af50@31f8af58-31f8af6d: child DamageData CRITICAL_RATE (+0x510, 32632390@3263c197) += the parent\'s, right after each hit (31f810c0@31f81987), die (31f82210@31f82f36) and touch-land (31f81ba0@31f81ffd) child spawn; not the CREATE_DAMAGE_EFFECT loop or RT_CREATE_DAMAGE_EFFECT. Touch-land runtime is not ported', 'CreateHitDamageEffect, CreateDieDamageEffect'),
    ('DE.top', (), 'INHERIT_CRITICAL_DAMAGE_RATE', RT_7D, 'done', 'new',
     '31f8af50@31f8af73-31f8af88: the same for APPLY_CRITICAL_DAMAGE_PERCENT (+0x59c, 32632390@3263e629)', 'CreateHitDamageEffect, CreateDieDamageEffect'),
    ('DE.CREATE_DAMAGE_EFFECT', (), 'INHERIT_DAMAGE', RT_7D, 'done', 'new',
     '7c: the flag onto the child @31f89cf5-31f89cfb. 7d: its readers are not DamageReact / AttackResult. 31f8fdb0@31f8feb1-31f8fecc (called @31f89d8b, after the flag is set): child damage PHYSIC / MAGIC (+0x38 / +0x3c, 32632390@326357ec / @3263588d) = the parent\'s; SetDamageDataNext 31f799c0@31f79b54-31f79b75 keeps it across a DAMAGE_DATA_NEXT change. Only this loop sets the child flag', 'OnFrameMove (CREATE_DAMAGE_EFFECT loop), SetDamageDataNextByChainIndex'),
    ('DE.HIT_DAMAGE_EFFECT', (), 'INHERIT_LIFE_TIME', RT_7D, 'done', 'new',
     '31f810c0@31f819bd-31f819fe: the child main effect takes the parent\'s now-life, as the loop does; the die spawner does the same (31f82210@31f82f74-31f82fb5, from LoadDieDamageEffect\'s read, which has no matrix row)', 'CreateHitDamageEffect, CreateDieDamageEffect'),
    ('DE.HIT_DAMAGE_EFFECT', (), 'INHERIT_BUFF_FACTOR', RT_7D, '-', 'same',
     '31f810c0 (entry +0x80): the parent buff-factor list (+0x240) copied when non-empty, as March ADDITIONAL_MEMO', 'CreateHitDamageEffect (existing)'),
    ('DE.DIE_DAMAGE_EFFECT', (), 'INHERIT_BUFF_FACTOR', RT_7D, '-', 'same',
     '31f82210 (entry +0x80): as the hit spawner, as March', 'CreateDieDamageEffect (existing)')] + [
    ('DE.HIT_DAMAGE_EFFECT', (), k, RT_7D, 'done', 'semantics-differ',
     '31f810c0@31f81a4f-31f81a90: child 31f6fdc0 (June SetScale) with this SCALE (entry +0x5c..+0x64) times the parent\'s hit damage-effect scale (+0xa5c..); March parsed the entry scale and applied only the parent\'s', 'CreateHitDamageEffect')
    for k in ('SCALE_X', 'SCALE_Y', 'SCALE_Z')] + [
    ('DE.DIE_DAMAGE_EFFECT', (), k, RT_7D, 'done', 'semantics-differ',
     '31f82210@31f82fb5-31f83010: the same with the parent\'s die damage-effect scale (+0xa68..)', 'CreateDieDamageEffect')
    for k in ('SCALE_X', 'SCALE_Y', 'SCALE_Z')] + [
    # Phase 7e (the three June-only sub-tables' runtime; identified in Phase 7d). HIT_EFFECT_SET and
    # DIE_CREATE_NPC transcribed in 7e. TOUCH_LAND_DAMAGE_EFFECT's gate and placement were settled
    # (x87-emulated, m_BeforePos-confirmed) but two CXMeshInstance offsets (+0x478, +0x54c) it needs
    # did not resolve to a named March member, and the spawner (31f81ba0) has blocks
    # CreateHitDamageEffect's shape does not cover (invoke-rate check, damage-rate multiply,
    # force-down-rate, SetScaleByUnit, SetMaxLifeTime; see funcmap.tsv's 31f81ba0 row) -- deferred
    # rather than shipped with a raw-offset gate or an incomplete spawner. No deployed data uses any
    # of the three sub-tables; the exit for the two done ones is a clean build matching the cited
    # branches.
    ('DE.top', (), 'TOUCH_LAND_DAMAGE_EFFECT%d', RT_7E, 'deferred', 'new',
     'gate OnFrameMove @31f8a2a4-31f8a394 (right after the die-check, before DAMAGE_DATA_CHANGE_TIME) settled: x87-emulated (june2017_re/x87test.py), mesh(+0x47c)<0 && mesh(+0x550)<0 && m_vVelocity.y(+0x538)>0, all strict -- the third is the OPPOSITE polarity from the earlier unemulated guess (this correction stands even though the port is deferred). +0x47c is read by two functions (31c87b80 @31c87ef1/31c8817a) not otherwise identified; +0x54c/+0x550/+0x554 is a plain per-frame copy of m_vVelocity written by 31c87b80 @31c87cac-31c87cc4 -- looks like a June-only member March\'s CXMeshInstance does not have at all, not a rename of an existing one. Position = m_BeforePos (+0x10c, confirmed by writer scan) with APPLY_ROTATE_ORIGIN (m_AngleDegree, CEffect +0x118) settled too. Spawner 31f81ba0 diffed against CreateHitDamageEffect: shares the position/rotate-origin/APPLY_ROTATED_OFFSET shape, but ALSO has an invoke-rate skip (@31f81cd4-31f81d24, FLD [+0x720]; FCOMPP; TEST AH,5; JNP), a damage-rate multiply (@31f82076-31f820ad, child +0x238/+0x23c *= [+0x724]), a force-down-rate branch matching CreateHitDamageEffect\'s #ifdef INHERIT_FORCEDOWNRATE arm rather than the #else one copied here, plus SetScaleByUnit/+0xacd-copy/SetMaxLifeTime -- none of which the shipped code has. Handed to Phase 10 (KTDGXMeshPlayer templet keys), which already owns CXMeshInstance member identification -- see its section\'s handoff paragraph.', '-'),
    ('DE.top', (), 'DIE_CREATE_NPC%d', RT_7E, 'done', 'new',
     'templet list +0xc8 (loader 31f66230@31f662a7); an E8-caller scan confirms all six June call sites of 31f70330 pair it immediately before 31f83400 (= March\'s own SetDieDamageEffect, which already calls CreateDieDamageEffect); March calls SetDieDamageEffect from exactly 5 active sites (DestroyInstanceHandle, DestroyInstance(CEffect*&), and OnFrameMove x3), covering 5 of June\'s 6 -- the 6th, the LockOnData 31f75e20 prelude, is a gap left by Phase 7b (LockOnData never calls SetDieDamageEffect at all), not this phase\'s to close -> create-NPC request 31f8b010 per entry -> CX2Game::CreateNPCReq (X2Game.h:587, EGS_NPC_UNIT_CREATE_REQ, answered offline); argument order settled by disassembling 31f8b010 and tracking ESP through its pushes, corroborated by 3 exact enum-value matches (TN_MONSTER==2, TN_NONE==-1, NCT_NONE==0) and an existing March call in the same shape (X2GUChung.cpp:17128)', 'CX2DamageEffect::CEffect::JuneDieCreateNpc'),
    ('DE.HIT_EFFECT_SET', (), 'HIT_EFFECT_SET%d', RT_7E, 'done', 'new',
     'templet list +0xb8 (loader 31f64c80@31f64d10); 31f70130 from AttackResult 31f85910@31f87b1a, after SetHitDamageEffect, inside March\'s own bCreateHitEffect==true branch', 'CX2DamageEffect::CEffect::JuneSetHitEffectSet'),
    ('DE.HIT_EFFECT_SET', (), 'EFFECT_SET_NAME', RT_7E, 'done', 'new',
     '31f70130: PlayEffectSet( entry +0x4, owner (EDI, confirmed by register provenance), defender (EBP), false, m_fPowerRate ), all trailing args at March\'s own defaults (fLifeTime=-1, scale=(1,1,1), bCustomPosition=false); entry +0x0 EVENT_TIME is parsed but never read', 'CX2DamageEffect::CEffect::JuneSetHitEffectSet'),
    ('DE.DIE_CREATE_NPC', (), 'NPC_ID', RT_7E, 'done', 'new',
     '31f70330: 31f8b010( isLocal gate, entry +0x4=unitID, level=0 const, bActive=true, CEffect position +0x1f4..+0x1fc=m_vMainEffectPosition, bRight, fDelayTime=0, bNoDrop=true, iKeyCode=-1, eTeam=owner GetTeam() (owner +0x278 XOR value, confirmed via TN_MONSTER==2), eAIType=NAT_ALLY, iAllyUID=owner GetUnitUID(), bElite=false, eAllyTeam=TN_NONE, eCreateMonsterType=NCT_NONE ) per entry', 'CX2DamageEffect::CEffect::JuneDieCreateNpc'),
    ('DE.DIE_CREATE_NPC', (), 'EVENT_TIME', RT_7E, 'done', '-',
     '31f70330 does not read entry +0x0; confirmed by disassembly, not just the decompile -- parsed-not-used, no delay implemented', 'CX2DamageEffect::CEffect::JuneDieCreateNpc')] + [
    ('DE.TOUCH_LAND_DAMAGE_EFFECT', (), k, RT_7E, 'deferred', 'new',
     '31f81ba0 (the touch-land spawner): its position/rotate-origin/APPLY_ROTATED_OFFSET shape was diffed against CreateHitDamageEffect and settled, but the spawner is missing blocks CreateHitDamageEffect does not have (invoke-rate, damage-rate multiply, force-down-rate, SetScaleByUnit, SetMaxLifeTime) and the gate needs two unidentified CXMeshInstance offsets -- deferred as a whole rather than ported partially. Handed to Phase 10.', '-')
    for k in ('DAMAGE_EFFECT_NAME', 'APPLY_ROTATE', 'PASSIVE_PARTICLE_X', 'PASSIVE_PARTICLE_Y', 'PASSIVE_PARTICLE_Z')]
# the EffectSet scopes whose March-read rows must all have a RUNTIME entry, at every sub-path
# (check_runtime). ES.Description is EFFECT_TOOL-only (COMPILED_OUT): the game never reads it.
RUNTIME_SCOPES = ('ES.templet', 'ES.EFFECT', 'ES.CAMERA_SHAKE', 'ES.SOUND')
RUNTIME_BY_KEY = dict(((r[0], r[1], r[2]), r) for r in RUNTIME)

# Phase 8a: hand-verified parse semantics for DE.DAMAGE_DATA rows March already reads, where the
# mechanical getter check above cannot settle the default (register-sourced defaults, no decrypt
# site, callee reads). Read with june2017_re/sdd_walk.py (32632390) and the decompile of 32630d20;
# the reasoning is in README.md, "Hand-verified (Phase 8a)". (root, rel, key) -> verdict. A verdict
# replaces the mechanical one only for rows it names; check_hand_semantics() fails on a stale entry.
_ED = ('EXTRA_DAMAGE',)
HAND_SEMANTICS = {
    # Phase 8c: the absent-system keys whose default is a register (plan rule 5 parse sites)
    ('DE.DAMAGE_DATA', (), 'APPLIED_SKILL_ID'): 'same (32632390@3263256a: int, default EBX, zeroed @3263254f)',
    ('DE.DAMAGE_DATA', (), 'NATURAL_FORCE_TYPE'): 'same (32632390@326354db: int, default EDI=-1 = NFT_NONE, set @3263532a and not rewritten before the store)',
    ('DE.DAMAGE_DATA', (), 'NUMERICAL_DATA_INDEX'): 'same (32632390@3263fc86: int into a stack local, default EBX, zeroed @3263fc6b)',
    ('DE.DAMAGE_DATA', (), 'MELEE_ATTACK'):'same (32632390@3263305e: bool getter, result ignored, so a missing key leaves the member as it was = March default-to-itself)',
    ('DE.DAMAGE_DATA', (), 'DYNAMO_PARTICLE_RATE_TYPE'): 'same (32632390@3263532a-32635340: int, default EDI=-1 = DPRT_NONE)',
    ('DE.DAMAGE_DATA', (), 'HIT_ADD_FORCE_POWER'): 'same (32632390@3263de28: int, default EBX, zeroed @3263de06)',
    ('DE.DAMAGE_DATA', _ED, 'LEVEL'): 'same (32632390@32636851: int, default EDI=-1 set @326367a9)',
    ('DE.DAMAGE_DATA', _ED, 'ADD_LEVEL'): 'same (32632390@326369b2: int, default EBX, zeroed @32636997)',
    ('DE.DAMAGE_DATA', _ED, 'EXTRA_DAMAGE_TYPE'): 'differ(post: 32632390@32636204-32636214 remaps EDT_MANA_DAMAGE_WITH_NO_DISPLAY 67 -> EDT_MANA_DAMAGE 16 and sets a no-display flag; default EBX=0 = EDT_NONE is the same; fixed under SERV_IRUHADEV_JUNE_DAMAGEDATA)',
    ('DE.DAMAGE_DATA', (), 'BUFF_FACTOR'): 'same (32630910: BeginTable, ParsingBuffFactor(lua, false), SetGameUnitBuffFactor(attacker), push_back = March ParsingBuffFactor; nested keys not compared)',
    ('DE.DAMAGE_DATA', (), 'BUFF_FACTOR_RELATIVE_SKILL_ID'): 'same (32630d20@32631a60: user default = vtable 0x524, a now-state skill-ID getter (GetNowStateSkillID); @32631edc: NPC default 0; level rule and learn-check identical)',
    ('DE.DAMAGE_DATA', (), 'BUFF_FACTOR_RELATIVE_MASTER_UNIT'): 'same (32630d20@326320df: bool, default false; master lookup and level-0 skip identical)',
    ('DE.DAMAGE_DATA', (), 'ATTACKER_GET'): 'same (32632390@32635b14-32635cf1: HP/MP floats default 0 into protected members; March attackerGet the same)',
    ('DE.DAMAGE_DATA', (), 'ENABLE_ATTACKBOX'): 'same (32632390@326379bc-32637a91: strings from index 1, SetEnableAttackBox(name, true) when the attacker is live)',
    ('DE.DAMAGE_DATA', (), 'DISABLE_ATTACKBOX'): 'same (32632390@32637a96-32637b71: as ENABLE_ATTACKBOX with false)',
}

COMPILED_OUT = {'CX2EffectSet::AddEffectSet_Description_LUA': '#ifdef EFFECT_TOOL, X2EffectSet.cpp:3380'}
LEVEL_ROOTS = {'ES.templet', 'ES.EFFECT', 'ES.CAMERA_SHAKE', 'ES.SOUND', 'DE.DAMAGE_DATA'}


def open_calls(B, f):
    """-> [(call va, opener, table name)] for every BeginTable call in f whose name is the
    plaintext pushed just before it (within 0x40 bytes: the SOUND re-test @321ee2a0 pushes
    its name 0x23 bytes ahead)."""
    from capstone import CS_GRP_CALL
    from capstone.x86 import X86_OP_IMM
    out, last = [], None
    for x in X.disasm(f, X.func_end(B.starts, f)):
        if x.mnemonic == 'push' and x.operands and x.operands[0].type == X86_OP_IMM:
            s = R.plain_at(B.d, x.operands[0].imm & 0xffffffff)
            if s and R.IDENT.match(s) and '%' not in s:
                last = (s, x.address)
        if x.group(CS_GRP_CALL) and x.operands and x.operands[0].type == X86_OP_IMM:
            t = x.operands[0].imm & 0xffffffff
            if t in (LUA_OPEN_N, LUA_OPEN, LUA_CLOSE):
                out.append((x.address, t, last[0] if (last and t != LUA_CLOSE and x.address - last[1] < 0x40) else None))
                if t != LUA_CLOSE:
                    last = None
    return out


def derive_levels(B, f):
    """Mechanical level map (valid for 32632390, whose only loops have no early EndTable)."""
    stack, ranges, popped = [], [], None
    end = X.func_end(B.starts, f)
    for va, t, name in open_calls(B, f):
        if t == LUA_CLOSE:
            if stack:
                n, st = stack.pop()
                ranges.append((tuple(x for x, _ in stack) + (n,), st, va))
                popped = (n, va)
            continue
        if name is None:
            continue
        name += '%d' if t == LUA_OPEN_N else ''
        if popped and popped[0] == name and va - popped[1] < 0x40:
            continue                  # loop-bottom re-test of the table just closed
        stack.append((name, va))
    for i, (n, st) in enumerate(stack):
        ranges.append((tuple(x for x, _ in stack[:i + 1]), st, end))
    return ranges


def check_levels(B):
    """Every ES_LEVELS range must start at a BeginTable on its own name (and a numbered
    range must end at the re-test on the same name)."""
    calls = dict((va, (t, n)) for va, t, n in open_calls(B, ES_PARSER))
    bad = []
    for path, a, b in ES_LEVELS:
        name = path[-1]
        base, numbered = name.replace('%d', ''), name.endswith('%d')
        want_t = LUA_OPEN_N if numbered else LUA_OPEN
        if calls.get(a) != (want_t, base):
            bad.append('%s start %08x is %s' % ('/'.join(path), a, calls.get(a)))
        end_ok = calls.get(b) == (want_t, base) if numbered else (calls.get(b, (None,))[0] == LUA_CLOSE)
        if not end_ok:
            bad.append('%s end %08x is %s' % ('/'.join(path), b, calls.get(b)))
    return bad

# Systems the plan expects to be absent. The census searches for them; the verdict is data.
# Systems the plan expects to be absent. Terms are whole identifiers (word-bounded regex):
# a substring search calls a system "present" on an unrelated enum member such as
# BTI_BUFF_NATURAL_FORCE (a buff templet) or SI_P_APT_NASOD_ARMOR_MODE (a skill).
SYSTEMS = [
    ('Rose weapon', r'ROSE_WEAPON', [r'ROSE_WEAPON_\w+', r'\w*RoseWeapon\w*', r'CX2GURose\w*', r'UC_ROSE\w*'],
     'no Rose class (`CX2GURose*`), unit class, enum or `ROSE_WEAPON_*` identifier in the client or `KncWX2Server/Common`; '
     '`CX2Unit::UNIT_TYPE` ends at `UT_ADD` (X2Unit.h), and every other `ROSE` stem hit is a skill or NPC name (Phase 8c)'),
    ('Natural force', r'NATURAL_FORCE', [r'NATURAL_FORCE_TYPE', r'NFT_\w+', r'm_\w*NaturalForce\w*', r'\w*NATURAL_FORCE_VALUE\w*'],
     'Rena natural-force code exists, but only under `ADD_RENA_SYSTEM`, which is off under `US_SERVICE`'),
    ('Skill memo', r'MEMO', [r'APPLIED_MEMO_ID', r'IsEqippedMemo_?', r'BUFF_FACTOR_ID_MEMO', r'BUFF_FACTOR_RELATIVE_MEMO_ID',
                             r'DAMAGE_DATA(_NEXT)?_MEMO\d*', r'NOT_ALLOW_MEMO', r'APPLIED_SKILL_ID_NOT_AFFECT_MEMO'],
     'compiled in: `ADD_MEMO_1ST_CLASS` is ON under `US_SERVICE` (March reads memo keys June does not; see *March reads it, June does not*)'),
    ('Numerical skill data', r'NUMERICAL_DATA', [r'NUMERICAL_DATA_\w+', r'\w*NumericalData\w*'],
     'no identifier anywhere (a plan-listed absent system)'),
    ('HP-rate damage', r'HP_RATE_DAMAGE', [r'HP_RATE_DAMAGE_\w+', r'\w*HpRateDamage\w*'],
     'no identifier anywhere, and no `HP_RATE_DAMAGE_TYPE` enum (a plan-listed absent system)'),
    ('Applied skill ID', r'^APPLIED_SKILL_ID$', [r'APPLIED_SKILL_ID', r'\w*AppliedSkillI[dD]\w*'],
     'no March identifier for the key (a plan-listed absent system); the search is by the key\'s own name. June\'s two '
     'DamageData readers, `ParsingBuffFactorID`\'s `_ULTIMATE` gate (`31fcf3e0`, unidentified, Phase 8b) and the '
     'numerical-data override (`32632390@3263fe41-32640054`), have no March counterpart. `DamageData::m_eUsedSkillID` '
     '(`SERV_ARA_CHANGE_CLASS_SECOND`) is a C++-set skill ID, not this key (Phase 8c)'),
    ('Nasod armor combo', r'NASOD_ARMOR_COMBO', [r'CAN_ACTIVATE_NASOD_ARMOR_COMBO', r'\w*NasodArmorCombo\w*'],
     'compiled in: Add\'s Nasod-armor combo states (`X2GUAdd`) under `SERV_9TH_NEW_CHARACTER`, which is ON'),
    ('Combination point', r'COMBINATION_POINT', [r'\w*COMBINATION_POINT\w*', r'\w*CombinationPoint\w*'],
     'no March identifier for the key (a plan-listed absent system); the search is by the key\'s own name'),
    ('Effect-alpha option', r'EFFECT_ALPHA_CONDITION', [r'\w*EFFECT_ALPHA_CONDITION\w*', r'\w*EffectAlphaCondition\w*',
                                                         r'AEACBO_\w+', r'\w*MY_TEAM_EFFECT\w*', r'\w*ANOTHER_TEAM_EFFECT\w*',
                                                         r'\w*OtherTeamEffect\w*'],
     'no per-owner effect-alpha game option: `X2GameOption` has only the effect-detail level (`m_eEffect`/`m_bEffect`, '
     'X2GameOption.h:48-52) and `m_bFlashEffect`. June\'s enum (`AEACBO_MY_EFFECT` / `_MY_TEAM_EFFECT` / '
     '`_ANOTHER_TEAM_EFFECT`) names an option this client never had. Honouring the key needs that option (UI + save): '
     '**a decision for the user**, not a transcription'),
]


def system_re(terms):
    return re.compile(r'(?<![A-Za-z0-9_])(?:%s)(?![A-Za-z0-9_])' % '|'.join(terms))

# June getter functions, pinned by the default-store shape after `call; test al,al; jne`
# at the sites surveyed in Phase 1 (fldz/fld1/fld [c] -> float; mov byte [dst],al|imm ->
# bool; mov dword [dst],imm -> int; push <L"">, assign -> string).
GETTERS = {0x32fea370: 'float', 0x32fe9e40: 'bool', 0x32fe9fd0: 'int', 0x32fea210: 'int',
           0x32feae90: 'string'}


# ------------------------------------------------------------------ June data
def walk(path):
    """reconcile.source_keys, plus enum references on the value side:
    -> keys [(templet, path, key, is_tab)], enums [(templet, path, key, TABLE, MEMBER)]."""
    toks = R.tokens(open(path, encoding='utf-8', errors='replace').read())
    keys = R.source_keys(path)
    enums = []
    stack = []
    templet = None
    for i, (k, v) in enumerate(toks):
        prev = toks[i - 1] if i else ('', '')
        prev2 = toks[i - 2] if i > 1 else ('', '')
        if v == '{' and k == 'op':
            if prev[1] == '=' and prev2[0] == 'name':
                name = prev2[1]
            elif prev[1] == '(' and prev2[0] == 'name' and i > 2 and toks[i - 3][1] == ':':
                name = 'call:' + prev2[1]
            else:
                name = '[]'
            if not [s for s in stack if s[0] == 'tab']:
                templet = name
            stack.append(('tab', name))
            continue
        if v == '(' and k == 'op':
            stack.append(('par', None))
            continue
        if v in ('}', ')') and k == 'op':
            stack.pop()
            continue
        if (k == 'name' and i + 2 < len(toks) and toks[i + 1][1] == '.' and toks[i + 2][0] == 'name'
                and prev[1] in ('=', '{', ',') and re.match(r'^[A-Z][A-Z0-9_]*$', v)):
            tabs = [s for s in stack if s[0] == 'tab']
            owner = prev2[1] if prev[1] == '=' and prev2[0] == 'name' else (tabs[-1][1] if tabs else None)
            p = tuple(R.norm(s[1]) for s in tabs[1:])
            if prev[1] != '=' and p:
                p = p[:-1]     # a list value `KEY = { T.M, ... }`: owner KEY lives one level up
            enums.append((templet, p, owner, v, toks[i + 2][1]))
    return keys, enums


def load_data():
    data, enums, bad = {}, {}, []
    for stem, fam in FILE_FAM.items():
        src = os.path.join(R.LUA, stem + '.src.lua')
        keys, en = walk(src)
        s_set = set(k for _, _, k, _ in keys)
        b_set = set(R.bytecode_keys(os.path.join(R.LUA, stem + '.lua')))
        if s_set != b_set:
            bad.append('%s: source-only %s, bytecode-only %s' % (stem, sorted(s_set - b_set)[:8], sorted(b_set - s_set)[:8]))
        data[stem], enums[stem] = keys, en
    if bad:
        sys.exit('source/bytecode key sets differ:\n  ' + '\n  '.join(bad))
    return data, enums


def root_of(fam, path):
    """-> (root id, sub-path below the root table) for a data path in a family."""
    if fam != 'DE' and fam != 'ES' or not path:
        return FAM_TOP[fam], tuple(path)
    for rid, rfam, pat, *_ in ROOTS:
        if rfam == fam and pat and re.fullmatch(pat, path[0]):
            return rid, tuple(path[1:])
    return None, tuple(path)


# ------------------------------------------------------------------ June binary
class June:
    def __init__(self):
        self.B = R.Binary()
        B = self.B
        self.esf = R.es_family(B)
        self.de_all = sorted(set(f for r in ROOTS if r[1] == 'DE' for f in r[3]))
        self.E = R.Explainer(B, self.de_all + self.esf)
        self.rows = collections.defaultdict(list)   # (func, plaintext) -> [use_va]
        for line in open(os.path.join(HERE, 'xorstrings.tsv'), encoding='utf-8').read().split('\n')[1:]:
            c = line.split('\t')
            if len(c) >= 10 and c[6] == 'ok':
                self.rows[(int(c[1], 16), c[5])].append(int(c[0], 16))
        self._scope = {}
        self._explained = {}
        self._levels = {}
        self._idx = None

    def readers(self, rid):
        r = ROOT[rid][3]
        return self.esf if r is None else r

    def scope(self, rid):
        """Everything reconcile.main computes per scope, for one root's readers."""
        readers = tuple(self.readers(rid))
        if readers in self._scope:
            return self._scope[readers]
        B = self.B
        keys, ult = set(), set()
        for f in readers:
            keys |= B.fn(f)['keys']
            for base, site, t in B.ultimate_bases(f):
                ult.add((base, site, t))
        keys |= set(b + '_ULTIMATE' for b, _, _ in ult)
        reach = self.E.closure(list(readers))
        callee_names = {}
        for f in sorted(reach - set(readers)):
            for kk in B.fn(f)['keys']:
                callee_names.setdefault(kk, f)
        complete = True
        for f in readers:
            fnx = X.Func(f, X.func_end(B.starts, f))
            rows = fnx.buffers()
            okn = sum(r['status'] == 'ok' for r in rows)
            if okn != len(rows) or len(fnx.guards) != okn + fnx.statics:
                complete = False
        s = {'readers': readers, 'keys': keys, 'ult': ult, 'm': tmatcher(keys), 'reach': reach,
             'callee_names': callee_names, 'cm': tmatcher(list(callee_names)), 'complete': complete}
        self._scope[readers] = s
        return s

    # ---- table levels inside one parser function
    def level_map(self, f):
        """-> [(path, start, end)] for a function whose sub-table levels are separated by VA,
        or None. 321e48e0 is the hand table ES_LEVELS (verified against the binary in
        check_levels); 32632390 is derived from its BeginTable/EndTable calls."""
        if f in self._levels:
            return self._levels[f]
        if f == ES_PARSER:
            lv = list(ES_LEVELS)
        elif f == DD_PARSER:
            lv = derive_levels(self.B, f)
        else:
            lv = None
        self._levels[f] = lv
        return lv

    def index_lists(self):
        """Levels at which June reads integer-keyed entries: every BeginTable(int) call
        (32fe99b0: lua_pushnumber(i); lua_gettable(-2); lua_type == LUA_TTABLE) in a
        level-mapped parser, as (parser, level path). The CEffect ctor never calls it."""
        if self._idx is None:
            self._idx = set()
            for par in (ES_PARSER, DD_PARSER):
                for va, t in self.B.fn(par)['calls']:
                    if t == LUA_OPEN_I:
                        self._idx.add((par, self.level_of(par, va)))
        return self._idx

    def index_read(self, fam, rid, path):
        """Is a data path containing '[]' read by index? Parent = the path before its last '[]'."""
        i = len(path) - 1 - list(reversed(path)).index('[]')
        parent = tuple(path[:i])
        if fam == 'ES':
            return (ES_PARSER, parent) in self.index_lists()
        if rid == 'DE.DAMAGE_DATA':
            return (DD_PARSER, parent[1:]) in self.index_lists()
        return False

    def level_of(self, f, va):
        lv = self.level_map(f)
        best = ()
        for path, a, b in lv:
            if a <= va < b and len(path) > len(best):
                best = path
        return best

    def mapped_sites(self, rid, hit, sites):
        """(func, va) sites -> [(site, level path)] for sites in a level-mapped function; a
        callee's sites are placed at its call site in the parser. Sites elsewhere -> None."""
        out = []
        for f, va in sites:
            par = ES_PARSER if f in self.esf else (DD_PARSER if f in (DD_PARSER, 0x32630d20) else None)
            if par is None or self.level_map(par) is None:
                out.append(((f, va), None))
                continue
            vas = [va] if (f == par and va) else []
            if f == par and not va:
                base = hit.replace('%d', '')
                vas = [pv for pv, s in self.B.fn(f)['plain'] if s == base]
            if f != par:
                vas = self.call_sites(par, f)
            for v in vas:
                out.append(((f, va), self.level_of(par, v)))
            if not vas:
                out.append(((f, va), None))
        return out

    def call_sites(self, par, f):
        """VAs in `par` of calls that reach f (directly, or through one intermediate callee)."""
        direct = [va for va, t in self.B.fn(par)['calls'] if t == f]
        if direct:
            return direct
        return [va for va, t in self.B.fn(par)['calls'] if t != par and any(tt == f for _, tt in self.B.fn(t)['calls'])]

    def classify(self, rid, key, paths):
        """-> (read?, row key, category, evidence, sites [(func, va)])."""
        s = self.scope(rid)
        hit = s['m'](key)
        if hit:
            sites = [(f, va) for f in s['readers'] for va in self.rows.get((f, hit), [])]
            how = 'decrypted' if sites else 'plaintext/template'
            if not sites:
                sites = [(f, None) for f in s['readers'] if hit in self.B.fn(f)['keys']][:3]
            elif rid in LEVEL_ROOTS:
                # A name can be both a decrypted key somewhere and a plaintext table name
                # here (DAMAGE: decrypted inside EXTRA_DAMAGE, pushed as a table name for
                # BeginTable("DAMAGE") @32635766). Level-check both kinds of site.
                base = hit.replace('%d', '')
                sites += [(f, None) for f in s['readers'] if f in (ES_PARSER, DD_PARSER)
                          and any(x == base for _, x in self.B.fn(f)['plain'])]
            if rid in LEVEL_ROOTS and paths and not (rid == 'DE.DAMAGE_DATA' and all(p[1:2] == ('BUFF_FACTOR',) for p in paths)):
                lv = self.mapped_sites(rid, hit, sites)
                # ES levels are absolute from the templet table; DamageData levels are
                # relative to the DAMAGE_DATA* table SetDamageDataFromLUA was handed. An
                # index-read list entry ('[]') is at its list's level.
                wants = set(tuple(c for c in (p[1:] if rid == 'DE.DAMAGE_DATA' else p) if c != '[]') for p in paths)
                keep = [st for st, l in lv if l is None or l in wants]
                if not keep:
                    got = sorted(set('/'.join(l) or '(top)' for _, l in lv))
                    return False, R.norm(key), 'other-level', (
                        'June reads `%s` only at level %s of %s (%s); not at `%s`' % (
                            hit, ', '.join(got), ', '.join('%08x' % f for f in sorted(set(f for (f, _), _ in lv))),
                            ', '.join('%08x' % v for (_, v), _ in lv[:3] if v), ', '.join(sorted('/'.join(w) or '(top)' for w in wants)))), []
                sites = list(dict.fromkeys(keep))
            return True, hit, 'direct', '%s in %s' % (how, ', '.join('%08x' % f for f, _ in sites[:3])), sites
        ck = (s['readers'], key)
        if ck not in self._explained:
            self._explained[ck] = self.E.explain(key, s['keys'], s['reach'] if s['readers'] else None)
        cat, why = self._explained[ck]
        if cat == 'other-func':
            # A key decrypted only by another sub-table's loader is read inside the table
            # that loader opens, not at this level (SetDamageData decrypts HIT_GAP inside
            # DAMAGE_DATA; a top-level HIT_GAP is not read). Table-name matches (template /
            # plaintext referenced) are this level's own keys naming a sub-table: they stay.
            foreign = set(f for r in ROOTS if r[0] != rid and r[1] == 'DE' and r[3] for f in r[3]) - set(s['readers'])
            namers = re.findall(r'([0-9a-f]{8}) \(([^)]*)\)', why)
            ok = [(f, k) for f, k in namers if not (k == 'decrypted' and int(f, 16) in foreign)]
            if namers and not ok:
                cat = 'dead-here'
                why = ('decrypted only by %s, loader(s) of other sub-tables, inside the table they open; '
                       'not read at this level' % ', '.join(f for f, _ in namers))
        if cat == 'name-collision':
            for pth in sorted(paths, key=lambda p: (-len(p), p)):
                for c in reversed(pth):
                    t = s['cm'](c)
                    if t:
                        cat = 'nested-subtable'
                        why = 'under `%s`, opened by callee %08x; %s' % ('/'.join(pth), s['callee_names'][t], why)
                        break
                if cat != 'name-collision':
                    break
        if cat == 'name-collision' and s['complete'] and s['readers']:
            cat = 'dead-here'
            why = 'every guard of the readers accounted for and no callee names it; ' + why
        tm = re.search(r'template (\S+?)\)', why)
        row = tm.group(1) if tm else key
        if cat in ('other-func', 'nested-subtable'):
            fs = [int(x, 16) for x in re.findall(r'\b([0-9a-f]{8})\b', why)][:3]
            return True, row, cat, why, [(f, (self.rows.get((f, key)) or [None])[0]) for f in fs]
        return False, row, cat, why, []


def tmatcher(keys):
    """Like reconcile.matcher, but returns the template itself (START_PARTICLE%d)."""
    exact = set(k for k in keys if '%' not in k)
    pats = [(k, re.compile('^' + re.escape(k).replace('%d', r'\d+').replace('%s', r'.+') + '$'))
            for k in sorted(keys) if '%' in k and not k.startswith('%')]

    def m(key):
        if key in exact:
            return key
        if '%' in key and key in keys:
            return key
        for k, p in pats:
            if p.match(key):
                return k
        return None
    return m


def getter_site(J, f, use):
    """Type, default and post-processing of the getter call that consumes a decrypted key."""
    from capstone import CS_GRP_CALL
    from capstone.x86 import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
    B = J.B
    end = X.func_end(B.starts, f)
    ins = list(X.disasm(use, min(end, use + 0x900)))
    ci = next((i for i, x in enumerate(ins) if x.group(CS_GRP_CALL)), None)
    if ci is None:
        return {'type': None, 'default': None, 'why': 'no call after the decrypt'}
    call = ins[ci]
    tgt = call.operands[0].imm & 0xffffffff if call.operands and call.operands[0].type == X86_OP_IMM else None
    res = {'call': call.address, 'getter': tgt, 'type': GETTERS.get(tgt), 'default': None, 'post': None}
    if tgt not in GETTERS:
        res['why'] = 'first call %s is not a known getter' % ('%08x' % tgt if tgt else 'indirect')
        return res
    # zero register: the terminator byte store (highest stack slot) before the call
    zero = None
    best = -1
    for x in ins[:ci]:
        if x.mnemonic == 'mov' and len(x.operands) == 2 and x.operands[0].type == X86_OP_MEM and x.operands[0].size == 1 \
                and x.operands[1].type == X86_OP_REG and x.operands[0].mem.disp > best:
            best, zero = x.operands[0].mem.disp, x.reg_name(x.operands[1].reg)
    rest = ins[ci + 1:ci + 12]
    mn = [x.mnemonic for x in rest]
    if mn[:3] == ['movzx', 'neg', 'sbb'] or mn[:2] == ['neg', 'sbb'] or mn[:4] == ['movzx', 'mov', 'neg', 'sbb']:
        res['type'] = 'enum' if res['type'] == 'int' else res['type']
        res['default'] = 0
        res['join'] = rest[4].address if len(rest) > 4 else None
        post_scan(res, ins, ci + 5)
        return res
    if len(rest) < 3 or mn[0] != 'test' or mn[1] != 'jne':
        res['why'] = 'non-standard shape after the call: %s' % ' '.join(mn[:4])
        return res
    join = rest[1].operands[0].imm & 0xffffffff
    body = [x for x in rest[2:] if x.address < join]
    typ = res['type']
    d = None
    if typ == 'float':
        if body and body[0].mnemonic == 'fldz':
            d = 0.0
        elif body and body[0].mnemonic == 'fld1':
            d = 1.0
        elif body and body[0].mnemonic == 'fld' and body[0].operands[0].type == X86_OP_MEM and body[0].operands[0].mem.base == 0:
            d = read_float(B.d, body[0].operands[0].mem.disp & 0xffffffff)
        if d is not None and not (len(body) > 1 and body[1].mnemonic == 'fstp'):
            d = None
    elif typ in ('bool', 'int'):
        if body and body[0].mnemonic == 'mov' and len(body[0].operands) == 2:
            src = body[0].operands[1]
            if src.type == X86_OP_IMM:
                d = src.imm if typ == 'int' else int(bool(src.imm & 0xff))
                if typ == 'int' and d >= 0x80000000:
                    d -= 1 << 32
            elif src.type == X86_OP_REG:
                r = body[0].reg_name(src.reg)
                if r == 'al' or (zero and r[-1] == zero[-1] and r[-2:-1] == zero[-2:-1]):
                    d = 0     # AL is 0 on the getter-failed path; `zero` is the terminator register
    elif typ == 'string':
        pushes = [x for x in body if x.mnemonic == 'push' and x.operands[0].type == X86_OP_IMM]
        for x in pushes:
            s = read_wide(B.d, x.operands[0].imm & 0xffffffff)
            if s is not None:
                d = s
                break
    if d is None:
        res['why'] = 'default store not recognised: %s' % '; '.join('%s %s' % (x.mnemonic, x.op_str) for x in body[:3]) if body else 'no default store (value kept)'
        if not body:
            res['default'] = 'kept'
    else:
        res['default'] = d
    res['join'] = join
    k = next((i for i, x in enumerate(ins) if x.address >= join), len(ins))
    post_scan(res, ins, k)
    return res


# Scaling / conversion only. Comparisons (cmp, fcom, fucom, test) are conditions, not a
# change of units, and are left to the owning phase's read of the site.
ARITH = {'fmul', 'fmulp', 'fdiv', 'fdivp', 'fdivr', 'fdivrp', 'fadd', 'faddp', 'fsub', 'fsubp', 'fsubr', 'fsubrp',
         'imul', 'idiv', 'mulss', 'divss', 'addss', 'subss', 'fild', 'cvtsi2ss'}


def post_scan(res, ins, k):
    """Arithmetic between the getter's join point and the next guard / decrypt / call."""
    from capstone import CS_GRP_CALL
    hits = []
    for x in ins[k:k + 14]:
        if x.group(CS_GRP_CALL) or (x.mnemonic == 'test' and '[0x33' in x.op_str) or \
                (x.mnemonic == 'mov' and re.match(r'\w+, (byte|word|dword) ptr \[0x33', x.op_str)):
            break
        if x.mnemonic in ARITH:
            hits.append('%s@%08x' % (x.mnemonic, x.address))
    res['post'] = ' '.join(hits[:4]) or None


def read_float(d, va):
    if not (X.TEXT_VA <= va < X.TEXT_VA + X.TEXT_RAWSIZE):
        return None
    o = X.raw(va)
    return round(struct.unpack('<f', d[o:o + 4])[0], 6)


def read_wide(d, va):
    if not (X.TEXT_VA <= va < X.TEXT_VA + X.TEXT_RAWSIZE):
        return None
    o = X.raw(va)
    s = d[o:o + 200]
    e = 0
    while e + 1 < len(s) and s[e:e + 2] != b'\x00\x00':
        e += 2
    try:
        t = s[:e].decode('utf-16le')
    except UnicodeDecodeError:
        return None
    return t if all(32 <= ord(c) < 127 for c in t) else None


# ------------------------------------------------------------------ March side
class RawSource:
    """X2Lib/ and KTDXLIB/ sources as written (not preprocessed): leads only. Our own
    X2Lib/Offline/ emulator is excluded -- it is not the studio parser."""
    QUOTED = re.compile(r'"([A-Za-z_][A-Za-z0-9_%]*)"')
    IDENTS = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')

    def __init__(self, subs=('X2Lib', 'KTDXLIB')):
        self.files = []
        self.quoted = {}                        # literal -> [first 'file:line', count]
        self.idents = collections.Counter()     # bare identifier occurrences
        for sub in subs:
            for dp, dns, fns in os.walk(os.path.join(TRUNK, sub)):
                dns[:] = [d for d in dns if d not in ('US_SERVICE', 'US_INTERNAL', 'Offline')]
                for fn in sorted(fns):
                    if fn.endswith(('.cpp', '.h')):
                        p = os.path.join(dp, fn)
                        rel = os.path.relpath(p, TRUNK).replace('\\', '/')
                        txt = open(p, 'rb').read().decode('latin1')
                        self.files.append((rel, txt))
                        for m in self.QUOTED.finditer(txt):
                            e = self.quoted.setdefault(m.group(1), ['%s:%d' % (rel, txt.count('\n', 0, m.start()) + 1), 0])
                            e[1] += 1
                            self.idents[m.group(1)] -= 1     # quoted, so not a bare identifier
                        self.idents.update(self.IDENTS.findall(txt))
        self._cond = {}

    def cond_stack(self, rel, txt, i):
        """Innermost enclosing #if/#ifdef/#ifndef flag name at offset i (None at top level)."""
        st = self.cond_all(rel, txt, i)
        return st[-1].lstrip('!') if st else None

    def in_june_rebuild(self, rel, txt, i):
        """True inside the #if branch of any SERV_IRUHADEV_JUNE_* block: the rebuild's own
        code. Plan rule 5 makes a phase parse, store and log an unsupported key there, so
        such a hit is the key's parse site, not evidence that the key's system exists. The
        #else branch is the studio's original code and still counts."""
        return any(f.startswith(JUNE_FLAG_PREFIX) for f in self.cond_all(rel, txt, i))

    def cond_all(self, rel, txt, i):
        """Every enclosing #if/#ifdef/#ifndef flag name at offset i, outermost first. A
        flag whose #else/#elif branch holds offset i is prefixed with '!'."""
        if rel not in self._cond:
            starts, stacks, stack = [], [], []
            o = 0
            for line in txt.split('\n'):
                s = line.strip()
                m = re.match(r'#\s*(if|ifdef|ifndef|elif|else|endif)\b\s*(.*)', s)
                starts.append(o)
                if m:
                    d, rest = m.group(1), m.group(2)
                    if d in ('if', 'ifdef', 'ifndef'):
                        fm = re.match(r'(?:defined\s*\(?\s*)?!?\s*(\w+)', rest)
                        stack.append(fm.group(1) if fm else '?')
                    elif d in ('else', 'elif') and stack and not stack[-1].startswith('!'):
                        stack[-1] = '!' + stack[-1]
                    elif d == 'endif' and stack:
                        stack.pop()
                stacks.append(tuple(stack))
                o += len(line) + 1
            self._cond[rel] = (starts, stacks)
        starts, stacks = self._cond[rel]
        return stacks[bisect.bisect_right(starts, i) - 1]

    def rel_of(self, path):
        """Absolute source path (as in a #line directive) -> (rel, txt) of a loaded file, or None."""
        if not hasattr(self, '_byrel'):
            self._byrel = dict((rel.lower(), (rel, txt)) for rel, txt in self.files)
        if not os.path.isabs(path):
            path = os.path.join(TRUNK, 'X2Lib', path)     # cl /P runs in X2Lib/ (preprocess)
        try:
            rel = os.path.relpath(os.path.normpath(path), TRUNK).replace('\\', '/').lower()
        except ValueError:
            return None
        return self._byrel.get(rel)


class March:
    def __init__(self, tus, raw):
        self.tu = dict((k, TU(k, v)) for k, v in tus.items())
        self.reads = {}   # (tu, func) -> [(path, key, line, off)]
        self.rawsrc = raw
        self.raw = raw.files
        self.compiled = collections.defaultdict(list)   # key -> [(tu, line)] over whole TUs
        for name, tu in self.tu.items():
            for m in STR.finditer(tu.text):
                if not m.group(1) and KEYLIKE.match(m.group(2)):
                    self.compiled[m.group(2)].append((name, tu.line_at(m.start())))
        self._enum_vals = {}
        self._vt = {}

    def func_reads(self, tu, func):
        if (tu, func) not in self.reads:
            t = self.tu[tu]
            out = []
            for b, e in t.functions(func):
                for path, key, off in march_reads(t, b, e):
                    out.append((path, key, t.line_at(off), off))
            self.reads[(tu, func)] = out
        return self.reads[(tu, func)]

    def find(self, rid, rel, key, rawkeys):
        """March reads of this row: [(tu, line, func, off)] at the matching sub-table path."""
        fam = ROOT[rid][1]
        hits = []
        want_keys = set([key]) | set(rawkeys)
        if rid == 'DE.DAMAGE_DATA' and rel and rel[0] == 'BUFF_FACTOR':
            for tu in BUFF_TUS + [DM_CPP]:
                t = self.tu[tu]
                for m in STR.finditer(t.text):
                    if not m.group(1) and m.group(2) in want_keys:
                        hits.append((tu, t.line_at(m.start()), '(buff parser, path-insensitive)', m.start()))
            return hits[:3]
        for tu, func, prefix in ROOT[rid][4]:
            want = tuple(prefix) + tuple(rel)
            for path, k, line, off in self.func_reads(tu, func):
                if not path_match(path, want):
                    continue
                if k in want_keys or (('%d' in k) and re.fullmatch(re.escape(k).replace('%d', r'\d+'), key)) or \
                        ('%d' in key and k == key):
                    hits.append((tu, line, func.split('::')[-1], off))
        return hits

    def leads(self, key):
        """Compiled-in literal anywhere in the parser TUs; quoted literal in raw source (first
        site, count); bare identifier count. For KEY%d, the stem and the %d template."""
        stem = key.replace('%d', '')
        comp = self.compiled.get(key) or self.compiled.get(stem) or []
        rq = self.rawsrc.quoted.get(key) or self.rawsrc.quoted.get(stem)
        rawq = [rq[0]] if rq else []
        return comp, rawq, (rq[1] if rq else 0), max(0, self.rawsrc.idents.get(stem, 0))

    def stmt(self, tu, off):
        return getvalue_stmt(self.tu[tu].text, off)

    def var_type(self, tu, var, enum):
        if enum:
            return 'enum'
        v = var.strip()
        if re.search(r'\.(x|y|z|w)$', v):
            return 'float'
        m = re.search(r'([A-Za-z_]\w*)\s*(\[[^\]]*\])?$', v)
        if not m:
            return None
        name = m.group(1)
        if (tu, name) in self._vt:
            return self._vt[(tu, name)]
        self._vt[(tu, name)] = self._var_type(tu, name)
        return self._vt[(tu, name)]

    DECL_TAIL = re.compile(r'\s*(?:\[[^\]]*\]\s*)?[;=,)]')
    DECL_HEAD = re.compile(r'([A-Za-z_][\w:]*(?:<[^;{}()<>]*(?:<[^;{}()<>]*>[^;{}()<>]*)*>)?)\s*[*&]?\s*$')

    def _var_type(self, tu, name):
        """Declared type of a member/local `name`: find() each occurrence and parse only
        the text just before it (a regex over the whole 20 MB TU backtracks for minutes)."""
        text = self.tu[tu].all_text
        types = collections.Counter()
        n = len(name)
        i = text.find(name)
        while i != -1:
            j = i + n
            if (i == 0 or not (text[i - 1].isalnum() or text[i - 1] == '_')) and                     not (j < len(text) and (text[j].isalnum() or text[j] == '_')) and self.DECL_TAIL.match(text, j):
                pre = text[max(0, i - 120):i]
                if pre and pre[-1] in ' 	*&':
                    dm = self.DECL_HEAD.search(pre)
                    if dm:
                        ty = dm.group(1)
                        if ty not in ('return', 'else', 'case', 'goto', 'delete', 'new', 'typedef', 'struct',
                                      'class', 'enum', 'const', 'sizeof'):
                            types[norm_type(ty)] += 1
            i = text.find(name, i + 1)
        if not types:
            return None
        return types.most_common(1)[0][0]

    def enum_value(self, tu, ident):
        key = (tu, ident)
        if key in self._enum_vals:
            return self._enum_vals[key]
        text = self.tu[tu].all_text
        val = None
        for em in re.finditer(r'\benum\b[^{;]*\{([^{}]*)\}', text):
            body = em.group(1)
            if not re.search(r'\b' + re.escape(ident) + r'\b', body):
                continue
            cur = -1
            names = {}
            for part in body.split(','):
                part = part.strip()
                if not part:
                    continue
                pm = re.match(r'([A-Za-z_]\w*)\s*(?:=\s*(.+))?$', part, re.S)
                if not pm:
                    cur = None
                    continue
                if pm.group(2) is not None:
                    ex = pm.group(2).strip()
                    try:
                        cur = int(ex, 0)
                    except ValueError:
                        cur = names.get(ex)
                elif cur is not None:
                    cur += 1
                names[pm.group(1)] = cur
            val = names.get(ident)
            if val is not None:
                break
        self._enum_vals[key] = val
        return val


def path_match(path, want):
    if len(path) != len(want):
        return False
    return all(a == b or a == '*' or b == '*' for a, b in zip(path, want))


def norm_type(t):
    t = t.split('::')[-1]
    if t in ('float', 'FLOAT', 'double'):
        return 'float'
    if t in ('bool', 'BOOL'):
        return 'bool'
    if t in ('int', 'UINT', 'DWORD', 'short', 'long', 'char', 'unsigned', 'USHORT', 'BYTE', 'WORD', 'UidType', '__int64', 'INT'):
        return 'int'
    if 'wstring' in t or t in ('WCHAR', 'wchar_t', 'string'):
        return 'string'
    if t.startswith('D3DXVECTOR') or t.startswith('D3DXCOLOR'):
        return 'vector'
    return 'other:' + t


def march_default(M, tu, d):
    """-> (kind, value) for a March default expression."""
    if d is None:
        return None, None
    s = d.strip()
    if s in ('true', 'TRUE'):
        return 'bool', 1
    if s in ('false', 'FALSE'):
        return 'bool', 0
    if s.startswith('L"'):
        m = STR.fullmatch(s)
        return 'string', m.group(2) if m else None
    n = re.fullmatch(r'\(?\s*(-?\s*\d+\.?\d*(?:[eE][-+]?\d+)?)[fF]?\s*\)?', s)
    if n:
        v = n.group(1).replace(' ', '')
        return ('float' if ('.' in v or s.lower().endswith('f')) else 'int'), float(v)
    idm = re.fullmatch(r'(?:[A-Za-z_]\w*::)*([A-Za-z_]\w*)', s)
    if idm:
        v = M.enum_value(tu, idm.group(1))
        return ('enum', v) if v is not None else ('expr', s)
    return 'expr', s


def compare(j, mstmt, mtype, mdef):
    """-> semantics verdict string."""
    if j.get('type') is None:
        return 'unchecked(June: %s)' % j.get('why', '?')
    if mstmt is None:
        return 'unchecked(March: key literal not in a GetValue statement)'
    jt = j['type']
    fam = {'float': 'float', 'bool': 'bool', 'int': 'int', 'enum': 'int', 'string': 'string'}
    mt = mtype
    if mt and mt.startswith('other:'):
        mt = 'enum' if mstmt.get('enum') else None
    if mt == 'vector':
        mt = None
    diffs = []
    if mt and fam.get(jt) and fam.get(mt, mt) != fam[jt]:
        diffs.append('type: June %s (getter %08x) vs March %s' % (jt, j['getter'], mtype))
    kind, val = mdef
    jd = j.get('default')
    if jd is None:
        dv = 'unchecked-default(June: %s)' % j.get('why', '?')
    elif kind in (None, 'expr'):
        if jd == 'kept' and mstmt.get('default') and strip_paren(mstmt['default']) == strip_paren(mstmt['var']):
            dv = None
        else:
            dv = 'unchecked-default(March default `%s`)' % (mstmt.get('default') or '?')
    elif jd == 'kept':
        diffs.append('default: June keeps the value, March `%s`' % mstmt.get('default'))
        dv = None
    else:
        if isinstance(jd, str) or isinstance(val, str):
            same = str(jd) == str(val)
        else:
            same = abs(float(jd) - float(val)) < 1e-4
        if not same:
            diffs.append('default: June %s, March %s (`%s`)' % (fmt(jd), fmt(val), mstmt.get('default')))
        dv = None
    if diffs:
        return 'differ(%s)' % '; '.join(diffs)
    if dv:
        return 'unchecked(%s)' % dv
    return 'same'


def fmt(v):
    if isinstance(v, float) and v == int(v):
        return str(int(v))
    return repr(v) if isinstance(v, str) else str(v)


# ------------------------------------------------------------------ enums
def parse_enum_lua(path):
    out = {}
    if not os.path.exists(path):
        return out
    cur = None
    for line in open(path, encoding='utf-8-sig', errors='replace'):
        m = re.match(r'^([A-Za-z_]\w*)\s*=\s*\{', line)
        if m:
            cur = m.group(1)
            out.setdefault(cur, {})
            continue
        if cur:
            mm = re.match(r'^\s*([A-Za-z_]\w*)\s*=\s*(-?\d+)', line)
            if mm:
                out[cur][mm.group(1)] = int(mm.group(2))
            if line.startswith('}'):
                cur = None
    return out


# ------------------------------------------------------------------ gating flags
def system_hits(raw):
    """-> {system: (Counter file -> hits, Counter enclosing flag -> hits, Counter file -> rule-5 hits)}
    over raw sources. Hits inside a SERV_IRUHADEV_JUNE_* block are counted apart (see
    RawSource.in_june_rebuild) and are not evidence."""
    out = {}
    code = [(rel, txt, blank_comments(txt)) for rel, txt in raw.files]
    for name, keypat, terms, note in SYSTEMS:
        files, gates, rule5 = collections.Counter(), collections.Counter(), collections.Counter()
        pat = system_re(terms)
        for rel, txt, ctxt in code:
            for m in pat.finditer(ctxt):     # a comment naming a key is not the key's system
                if raw.in_june_rebuild(rel, txt, m.start()):
                    rule5[rel] += 1
                    continue
                files[rel] += 1
                g = raw.cond_stack(rel, txt, m.start())
                if g:
                    gates[g] += 1
        out[name] = (files, gates, rule5)
    return out


def blank_comments(txt):
    """txt with // and /* */ comment bodies replaced by spaces (newlines kept), so offsets
    and line numbers still match txt. String literals are skipped, so "//" inside one
    stays."""
    def sub(m):
        s = m.group(0)
        return s if s[0] in '"\'' else re.sub(r'[^\n]', ' ', s)
    return _COMMENT_OR_STRING.sub(sub, txt)


_COMMENT_OR_STRING = re.compile(r'"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'|//[^\n]*|/\*.*?(?:\*/|\Z)', re.S)


def tu_origins(T):
    """-> (offsets, [(path, line)]) for every #line directive of a preprocessed TU: the
    source position of any offset in T.all_text is the last directive before it plus the
    newlines in between."""
    if not hasattr(T, '_origins'):
        offs, org = [], []
        for m in re.finditer(r'^#line (\d+) "(.*)"\r?$', T.all_text, re.M):
            offs.append(m.end() + 1)
            org.append((m.group(2).replace('\\\\', '\\'), int(m.group(1))))
        T._origins = (offs, org)
    return T._origins


def system_evidence(M, hits, raw):
    """Compiled hits per parser TU, minus the ones whose source line sits in a
    SERV_IRUHADEV_JUNE_* block of `raw` (the rebuild's own rule-5 parse sites)."""
    ev = []
    for name, keypat, terms, note in SYSTEMS:
        comp = collections.Counter()
        pat = system_re(terms)
        for tu, T in M.tu.items():
            offs, org = tu_origins(T)
            n = 0
            for m in pat.finditer(T.all_text):
                k = bisect.bisect_right(offs, m.start()) - 1
                if k >= 0:
                    src = raw.rel_of(org[k][0])
                    if src:
                        line = org[k][1] + T.all_text.count('\n', offs[k], m.start())
                        rel, txt = src
                        pos = line_offset(txt, line)
                        if pos is not None and raw.in_june_rebuild(rel, txt, pos):
                            continue
                n += 1
            if n:
                comp[tu] += n
        ev.append({'name': name, 'keypat': keypat, 'terms': terms, 'note': note, 'compiled': comp, 'raw': hits[name][0],
                   'gates': hits[name][1], 'rule5': hits[name][2]})
    return ev


_LINE_OFFS = {}


def line_offset(txt, line):
    """Offset of 1-based `line` in txt, or None past the end."""
    key = id(txt)
    if key not in _LINE_OFFS:
        starts, o = [0], 0
        for s in txt.split('\n'):
            o += len(s) + 1
            starts.append(o)
        _LINE_OFFS[key] = starts
    starts = _LINE_OFFS[key]
    return starts[line - 1] if 0 < line <= len(starts) - 1 else None


def verdict(e, flag_state):
    """present / gated-off / absent, from compiled hits in the parser TUs vs raw source."""
    if sum(e['compiled'].values()):
        return 'present'
    if sum(e['raw'].values()):
        on = [g for g in e['gates'] if flag_state.get(g)]
        off = [g for g in e['gates'] if flag_state.get(g) is False]
        unknown = [g for g in e['gates'] if flag_state.get(g) is None]
        if off and not on:
            return 'gated-off'
        if on and not unknown:
            return 'present-elsewhere'     # compiled in (every enclosing flag ON), outside the parser TUs
        return 'source-only'
    return 'absent'


# ------------------------------------------------------------------ main
def build(reuse_pp):
    data, enums = load_data()
    J = June()
    raw = RawSource()
    sysraw = RawSource(('X2Lib', 'KTDXLIB', os.path.join('KncWX2Server', 'Common')))
    hits = system_hits(sysraw)
    flags = set(g for _, gates, _ in hits.values() for g in gates)
    tus, flag_state = preprocess(reuse_pp, flags)
    M = March(tus, raw)
    sysev = system_evidence(M, hits, sysraw)
    sysv = dict((e['name'], verdict(e, flag_state)) for e in sysev)

    # ---- entries -> rows (keyed by the June template when one matches)
    groups = collections.OrderedDict()   # (fam, rid, rel, rawkey) -> info
    code_keys = collections.defaultdict(collections.Counter)
    for stem, fam in FILE_FAM.items():
        for templet, path, key, is_tab in data[stem]:
            if path == ('<code>',):
                code_keys[fam][key] += 1
                continue
            if '[]' in path and not J.index_read(fam, root_of(fam, path)[0], path):
                rid, rel = ('%s.positional' % fam), tuple(path)
            else:
                rid, rel = root_of(fam, path)
                if rid is None:
                    rid, rel = '%s.unrooted' % fam, tuple(path)
            g = groups.setdefault((fam, rid, rel, key), {'uses': 0, 'files': collections.Counter(), 'templets': set(),
                                                         'example': '%s:%s' % (stem, templet), 'paths': set(), 'is_tab': False})
            g['uses'] += 1
            g['files'][stem] += 1
            g['templets'].add(templet)
            g['paths'].add(path)
            g['is_tab'] |= is_tab

    rows = collections.OrderedDict()
    for (fam, rid, rel, key), g in groups.items():
        if rid.endswith('.positional') or rid.endswith('.unrooted'):
            read, rkey, cat, why, sites = False, key, 'positional' if rid.endswith('.positional') else 'unrooted', '', []
            if cat == 'positional':
                why = ('integer-keyed entry (`{ {...}, ... }`); every June loader opens its sub-tables by '
                       'name (KLuaManager::BeginTable formats "%s%d" @31c7b9be; the rest take plaintext names)')
            else:
                why = 'no scope root for sub-table `%s`' % path_s(rel)
        else:
            read, rkey, cat, why, sites = J.classify(rid, key, g['paths'])
        if not read:
            rkey = R.norm(key)
        r = rows.setdefault((fam, rid, rel, rkey), {
            'fam': fam, 'root': rid, 'rel': rel, 'key': rkey, 'raw': set(), 'uses': 0, 'files': collections.Counter(),
            'templets': set(), 'example': g['example'], 'paths': set(), 'june_read': read, 'cat': cat, 'why': why,
            'sites': sites, 'is_tab': False})
        r['raw'].add(key)
        r['uses'] += g['uses']
        r['files'].update(g['files'])
        r['templets'] |= g['templets']
        r['paths'] |= g['paths']
        r['is_tab'] |= g['is_tab']
        if read and not r['june_read']:
            r.update(june_read=True, cat=cat, why=why, sites=sites)

    # ---- parent-not-opened: a key under a sub-table June never opens is not read either.
    # A row is opened if any of its data paths has every enclosing table read by June.
    def blocker(fam, p):
        """-> None if June opens every table on data path p, else (name, reason)."""
        if not p:
            return None
        top = FAM_TOP[fam]
        rid = root_of(fam, p[:1])[0] if fam in ('DE', 'ES') else top
        chain = [(top, (), p[0])] + [(rid, tuple(p[1:i]), p[i]) for i in range(1, len(p))]
        for crid, crel, comp in chain:
            if comp == '[]':
                if J.index_read(fam, rid, p[:p.index('[]') + 1] if '[]' in p else p):
                    continue       # an entry of a list June reads by index (BeginTable(int) 32fe99b0)
                return comp, 'positional entry'
            if crid is None:
                return None     # unrooted below an opened table: the row stays `unrooted`
            pr = find_row(rows, fam, crid, crel, comp)
            if pr is None:
                return comp, 'no row for the table name'
            if not pr['june_read']:
                return pr['key'], pr['cat']
        return None

    for r in rows.values():
        if not r['paths']:
            continue
        bl = [blocker(r['fam'], p) for p in sorted(r['paths'])]
        if any(b is None for b in bl):
            continue
        name, why = bl[0]
        if r['cat'] == 'positional':
            continue
        r.update(june_read=False, cat='parent-not-opened',
                 why='under `%s`, which June does not open (%s)%s' % (name, why, ('; ' + r['why']) if not r['june_read'] and r['why'] else ''))

    # ---- binary-only rows: June reader keys no in-scope data uses
    data_keys = collections.defaultdict(set)
    for (fam, rid, rel, rkey), r in rows.items():
        data_keys[FAM_GROUP(fam)] |= r['raw'] | set([rkey])
        for p in r['paths']:
            data_keys[FAM_GROUP(fam)] |= set(p)
    for rid, fam, pat, readers, mfuncs, phase, subsys in ROOTS:
        if fam not in ('DE', 'ES', 'DD', 'DS'):
            continue
        rds = J.readers(rid)
        for f in rds:
            fkeys = J.B.fn(f)['keys']
            for k in sorted(fkeys):
                if not re.match(r'^[A-Z][A-Z0-9_]*(%d)?[A-Z0-9_]*$', k) or k + '%d' in fkeys:
                    continue
                dk = data_keys[FAM_GROUP(fam)]
                if k in dk or ('%d' in k and any(re.fullmatch(re.escape(k).replace('%d', r'\d+'), x) for x in dk)):
                    continue
                vas = J.rows.get((f, k), [])
                if rid.startswith('ES.') and rid != 'ES.templet':
                    continue    # ES readers are shared; attribute once, from the templet root
                # place the key at the sub-table level June reads it at
                target, rel = rid, ()
                if rid in LEVEL_ROOTS:
                    lvs = [l for _, l in J.mapped_sites(rid, k, [(f, v) for v in vas] or [(f, None)]) if l is not None]
                    lv = min(lvs, key=len) if lvs else ()
                    if fam == 'ES':
                        top = {'EFFECT%d': 'ES.EFFECT', 'CAMERA_SHAKE%d': 'ES.CAMERA_SHAKE', 'SOUND%d': 'ES.SOUND'}
                        target, rel = (top[lv[0]], tuple(lv[1:])) if lv else ('ES.templet', ())
                    else:
                        rel = tuple(lv)
                keyt = (fam, target, rel, k)
                if keyt in rows:
                    continue
                rows[keyt] = {'fam': keyt[0], 'root': target, 'rel': rel, 'key': k, 'raw': set([k]), 'uses': 0,
                              'files': collections.Counter(), 'templets': set(), 'example': '', 'paths': set(),
                              'june_read': True, 'cat': 'binary-only',
                              'why': '%s in %08x%s' % ('decrypted' if vas else 'plaintext', f, (' @' + ','.join('%08x' % v for v in vas[:3])) if vas else ''),
                              'sites': [(f, vas[0] if vas else None)], 'is_tab': False, 'binonly': True}

    # ---- March side, status, phase, semantics
    for r in rows.values():
        rid = r['root']
        r['march'] = []
        if rid in ROOT:
            r['march'] = M.find(rid, r['rel'], r['key'], r['raw'])
        comp, rawq, nraw, bare = M.leads(r['key'])
        r['leads'] = {'compiled': comp[:3], 'ncompiled': len(comp), 'rawq': rawq[:3], 'nrawq': nraw, 'bare': bare}
        r['system'] = None
        for name, keypat, terms, note in SYSTEMS:
            if re.search(keypat, r['key']) and sysv[name] in ('absent', 'gated-off'):
                r['system'] = '%s (%s)' % (name, sysv[name])
                break
        r['phase'] = phase_of(r)
        if not r['june_read']:
            r['status'] = 'june-ignores'
        elif r['march'] and r['system']:
            # absent system that March now parses, stores and logs (plan rule 5): that is
            # the row's final state, and the March column shows the parse site
            r['status'] = 'unsupported'
        elif r['march']:
            r['status'] = 'implemented'
        elif r.get('binonly'):
            r['status'] = 'binary-only'
        elif r['system']:
            r['status'] = 'unsupported'
        else:
            r['status'] = 'to-implement'
        r['sem'] = None
        r['jsite'] = None
        if r['march'] and r['status'] in ('implemented', 'unsupported'):
            r['sem'], r['jsite'], r['msite'] = semantics(J, M, r)
            hand = HAND_SEMANTICS.get((r['root'], r['rel'], r['key']))
            if hand:
                r['sem'] = hand

    # ---- enums
    june_enum = parse_enum_lua(JUNE_ENUM)
    march_enum = {}
    for p in MARCH_ENUMS:
        for t, members in parse_enum_lua(p).items():
            march_enum.setdefault(t, {}).update(members)
    enum_rows = collections.OrderedDict()
    for stem, fam in FILE_FAM.items():
        for templet, path, owner, T, Mm in enums[stem]:
            if T in ('TRUE', 'FALSE', 'NULL'):
                continue
            rid, rel = root_of(fam, path) if '[]' not in path else ('%s.positional' % fam, path)
            e = enum_rows.setdefault((T, Mm), {'uses': 0, 'keys': collections.Counter(), 'rows': set()})
            e['uses'] += 1
            e['keys'][owner] += 1
            rr = find_row(rows, fam, rid, rel, owner) if rid and owner else None
            if rr is not None:
                e['rows'].add((rr['root'], rr['key'], rr['status'], rr['phase']))
    return rows, code_keys, sysev, sysv, flag_state, enum_rows, june_enum, march_enum, J, M


def FAM_GROUP(fam):
    return 'ES' if fam in ('ES', 'ESD') else fam


def find_row(rows, fam, rid, rel, key):
    for k in (key, R.norm(key)):
        r = rows.get((fam, rid, tuple(rel), k))
        if r is not None:
            return r
    for (f, ri, re_, k), r in rows.items():
        if f == fam and ri == rid and re_ == tuple(rel) and '%d' in k and re.fullmatch(re.escape(k).replace('%d', r'\d+'), key):
            return r
    return None


def path_s(p):
    return '/'.join(p) if p else '(top)'


def phase_of(r):
    rid = r['root']
    if rid not in ROOT:
        return '-'
    ph = ROOT[rid][5]
    # A top-level key that names a sub-loader's table (TOUCH_LAND_DAMAGE_EFFECT%d, EFFECT%d)
    # is that loader's entry point: its phase, not the top level's. DAMAGE_DATA* stay with
    # the CEffect ctor (Phase 5 records what its SetDamageData calls pass).
    if rid in ('DE.top', 'ES.templet') and not r['rel']:
        fam = ROOT[rid][1]
        for sid, sfam, pat, _, _, sph, _ in ROOTS:
            if sfam == fam and pat and sid != 'DE.DAMAGE_DATA' and re.fullmatch(pat, r['key']):
                return sph
    if rid == 'DE.DAMAGE_DATA':
        if r['rel'] and r['rel'][0] == 'BUFF_FACTOR':
            return 'unassigned'
        # Phase 8c: an absent-system key stays 8c's once March parses it (plan rule 5), so the
        # system test comes before the March-site one, which would otherwise relabel it 8a.
        if r['system']:
            return '8c'
        return '8a' if r['march'] else '8b'
    return ph


def semantics(J, M, r):
    """Compare June's getter at the decrypt site with March's expanded GetValue."""
    jsites = [(f, va) for f, va in r['sites'] if va]
    msite = r['march'][0] if r['march'] else None
    # Only like-for-like reads are compared: a scope reader's own decrypt site against the
    # March read at the same path. A callee that names the key (other-func / nested) may
    # read a same-named key of another table with another getter (BUFF_RELATION_TYPE is
    # int in one buff parser and float in another), and the buff rows are path-insensitive.
    if r['root'] == 'DE.DAMAGE_DATA' and r['rel'][:1] == ('BUFF_FACTOR',):
        return 'unchecked(buff parser row: March matched path-insensitively)', None, msite
    if r['cat'] not in ('direct', 'binary-only'):
        return 'unchecked(June reads it in callee %s, not a scope reader)' % (
            ', '.join('%08x' % f for f, _ in r['sites'][:2]) or '?'), None, msite
    if not jsites:
        return ('n/a (sub-table name)' if r['is_tab'] or r['key'].endswith('%d') else
                'unchecked(June: no decrypt site, %s)' % r['cat']), None, msite
    js = [getter_site(J, f, va) for f, va in jsites]
    if msite is None:
        return 'unchecked(no March site)', js[0], None
    tu, line, func, off = msite
    st = M.stmt(tu, off)
    if st is None:
        if r['is_tab']:
            return 'n/a (sub-table name)', js[0], msite
        return 'unchecked(March: `%s` at %s:%d is not a GetValue key)' % (r['key'], tu, line), js[0], msite
    mtype = M.var_type(tu, st['var'], st.get('enum'))
    mdef = march_default(M, tu, st.get('default'))
    verdicts = sorted(set(compare(j, st, mtype, mdef) for j in js))
    v = verdicts[0] if len(verdicts) == 1 else ' | '.join(verdicts)
    posts = [j['post'] for j in js if j.get('post')]
    if posts and v == 'same':
        v = 'unchecked(post-processing %s)' % posts[0]   # June may scale the value after reading it
    return v, js[0], msite


# ------------------------------------------------------------------ output
def write(rows, code_keys, sysev, sysv, flag_state, enum_rows, june_enum, march_enum, checks):
    L = []
    w = L.append
    st = collections.Counter(r['status'] for r in rows.values())
    w('# June2017 effect-engine key matrix')
    w('')
    w('Generated by `python june2017_re/census.py`. **Do not edit by hand**: rerun it. This is the')
    w('finish line of every later phase of `JUNE2017_EFFECT_REBUILD_PLAN.md`. A parser phase is done')
    w('when its rows leave `to-implement`; `python june2017_re/census.py --summary` prints the counts.')
    w('')
    w('## How to read it')
    w('')
    w('- **One row is one key in one sub-table**: family, scope root, sub-path under the root, key.')
    w('  Numbered keys are folded to the June template (`START_PARTICLE%d`), and the same name in two')
    w('  sub-tables is two rows.')
    w('- **June side**: `xorstrings.tsv` (ok rows only) and the plaintext sub-table names each reader')
    w('  references, through the same `reconcile.Binary` / `reconcile.Explainer` machinery that builds')
    w('  `june2017_re/reconcile.md`. `direct` means a scope reader decrypts or references it;')
    w('  `other-func`/`nested-subtable` means a callee on the reader\'s path does.')
    w('- **March side**: X2Lib\'s parser TUs (%s) preprocessed with `cl /P` under the X2Lib' % ', '.join('`%s`' % t for t in TUS))
    w('  `US_SERVICE` defines plus `UNICODE`. A read counts only if it survives the preprocessor, and')
    w('  only inside the scope\'s March function at the same sub-table path (tracked through')
    w('  `BeginTable`/`EndTable`). Stringized keys (`CASE_BEHAVIOR_FACTOR` → `STRINGIZEA2(BBT_STUN)`)')
    w('  arrive as literals. The "leads" columns (quoted literal in raw source, bare identifier count)')
    w('  are leads only: a raw hit that is not in the preprocessed text is compiled out.')
    w('- **Status**, exactly one per row:')
    w('  - `implemented`: June reads it and March reads it at the same path.')
    w('  - `to-implement`: June reads it and March does not. `phase` is the owner.')
    w('  - `unsupported`: as to-implement, but it belongs to a system this build lacks (see')
    w('    *Absent systems*). Parse, store, log once; no behaviour (plan rule 5).')
    w('  - `binary-only`: a June reader asks for it and no in-scope June data uses it. Still')
    w('    implemented under plan rule 3; `phase` is the owner.')
    w('  - `june-ignores`: the data uses it and June never reads it (typo, dead data, removed,')
    w('    positional entry, or under a sub-table June never opens). Nothing to implement.')
    w('    **This fifth status is not in the plan\'s list of four**: without it, June\'s own dead')
    w('    data would have to be mislabelled `to-implement`.')
    w('- **Phases** follow the plan: EffectSet templet 2; `EFFECT%d` and its sub-blocks 3; CEffect top')
    w('  level 5; create/die/hit damage effects and die conditions 6a; the other sub-loaders 6b;')
    w('  DamageData 8a (keys March reads: semantics), 8b (new, system present), 8c (absent system).')
    w('  `unassigned`: no phase of the plan owns it (nested `BUFF_FACTOR`, `Elsword_DamageData.lua`,')
    w('  `DamageSound`). See *Open questions*.')
    w('- **semantics** (implemented rows with a decrypt site): June\'s getter call is read at the')
    w('  decrypt site (`call getter; test al,al; jne; <default store>`; getter types in `GETTERS`) and')
    w('  compared with March\'s expanded `GetValue` statement. `same`, `differ(...)`, or `unchecked(...)`')
    w('  when either side is not in the standard shape. `post:` lists arithmetic June does on the')
    w('  value right after reading it: a lead for the owning phase, not a verdict.')
    w('')
    w('## Checks')
    w('')
    for ok, text in checks:
        w('- %s %s' % ('PASS' if ok else '**FAIL**', text))
    w('')
    w('## Summary')
    w('')
    w('| status | rows |')
    w('|---|---|')
    for s in ('implemented', 'to-implement', 'unsupported', 'binary-only', 'june-ignores'):
        w('| %s | %d |' % (s, st[s]))
    w('')
    ph = collections.Counter((r['phase'], r['status']) for r in rows.values() if r['status'] in ('to-implement', 'unsupported', 'binary-only'))
    phases = sorted(set(p for p, _ in ph), key=phase_key)
    w('Open work per phase (`to-implement` / `unsupported` / `binary-only`):')
    w('')
    w('| phase | to-implement | unsupported | binary-only |')
    w('|---|---|---|---|')
    for p in phases:
        w('| %s | %d | %d | %d |' % (p, ph[(p, 'to-implement')], ph[(p, 'unsupported')], ph[(p, 'binary-only')]))
    semd = [r for r in rows.values() if r['sem'] and r['sem'].startswith('differ')]
    semu = [r for r in rows.values() if r['sem'] and r['sem'].startswith('unchecked')]
    sems = [r for r in rows.values() if r['sem'] == 'same' or (r['sem'] or '').startswith('same (')]   # 'same (...)': HAND_SEMANTICS
    nparsed = st['implemented'] + len([r for r in rows.values() if r['status'] == 'unsupported' and r['march']])
    w('')
    w('Semantics of the rows March parses (implemented, plus unsupported rows parsed under plan rule 5): '
      '%d same, %d **semantics-differ**, %d unchecked, %d n/a.' % (
        len(sems), len(semd), len(semu), nparsed - len(sems) - len(semd) - len(semu)))
    w('')
    # ---- semantics-differ
    w('## semantics-differ')
    w('')
    w('Keys March already reads whose June read differs in type or default. Owners: DamageData → 8a,')
    w('everything else → the parser phase in the `phase` column.')
    w('')
    w('| phase | scope | key | verdict | June site | March site |')
    w('|---|---|---|---|---|---|')
    for r in sorted(semd, key=lambda r: (phase_key(r['phase']), r['root'], r['rel'], r['key'])):
        w('| %s | %s | `%s` | %s | %s | %s |' % (r['phase'], scope_s(r), r['key'], r['sem'].replace('|', '/'), jsite_s(r), msite_s(r)))
    w('')
    w('### March reads it, June does not')
    w('')
    w('Rows June never reads (`june-ignores`) at a path where March does read them under `US_SERVICE`.')
    w('June dropped these, so March behaviour here is not June\'s. The owning phase decides: keep')
    w('(a local feature) or put it behind the June flag.')
    w('')
    mr = sorted([r for r in rows.values() if r['status'] == 'june-ignores' and r['march']],
                key=lambda r: (phase_key(r['phase']), r['root'], r['rel'], r['key']))
    if mr:
        w('| phase | scope | key | why June ignores it | March site |')
        w('|---|---|---|---|---|')
        for r in mr:
            w('| %s | %s | `%s` | %s | %s |' % (r['phase'], scope_s(r), r['key'], cell(r['why'][:180]), march_s(r)))
    else:
        w('None.')
    w('')
    postu = [r for r in rows.values() if (r['sem'] or '').startswith('unchecked(post-processing')]
    w('### Scaled after reading')
    w('')
    if postu:
        w('Implemented rows whose June read is followed by arithmetic before the next key. Their')
        w('verdict is `unchecked` until the owning phase reads the site:')
        w('')
        for r in sorted(postu, key=lambda r: (phase_key(r['phase']), r['key'])):
            w('- %s %s `%s`: %s' % (r['phase'], scope_s(r), r['key'], r['sem']))
    else:
        w('None: no implemented row\'s June read is followed by scaling arithmetic before the next key.')
    w('')
    # ---- runtime citations
    w('## Runtime (Phase 4a on)')
    w('')
    w('What June does with a key at run time, and where. The table is written by hand (`RUNTIME` in')
    w('`census.py`) from the June disassembly. It is kept apart from the parse-time `semantics` column, so')
    w('a getter verdict and a behaviour verdict never overwrite each other. `state` is `done`')
    w('(transcribed), `stage N` (owned, not yet written), `deferred`, or `-` (nothing to write).')
    w('`outside-effectset` means only another subsystem reads the key; the branch names it.')
    w('The full spec is in the plan, Phases 4a and 4b.')
    w('')
    rc = collections.Counter(e[5].split('@')[0] for e in RUNTIME)
    w('%d keys cited: %s.' % (len(RUNTIME), ', '.join('%s %d' % kv for kv in sorted(rc.items()))))
    w('')
    w('| phase | scope | key | runtime verdict | state | June branch | March site |')
    w('|---|---|---|---|---|---|---|')
    for e in sorted(RUNTIME, key=lambda e: (phase_key(e[3]), e[0], e[1], e[2])):
        w('| %s | `%s` %s | `%s` | %s | %s | %s | %s |' % (e[3], e[0], path_s(e[1]), e[2], e[5], e[4], cell(e[6]), cell(e[7])))
    w('')
    # ---- per scope
    order = [r[0] for r in ROOTS] + sorted(set(r['root'] for r in rows.values()) - set(ROOT))
    by_scope = collections.defaultdict(list)
    for r in rows.values():
        by_scope[(r['root'], r['rel'])].append(r)
    w('## Rows by subsystem and scope')
    w('')
    cur_sub = None
    for (rid, rel) in sorted(by_scope, key=lambda k: (order.index(k[0]) if k[0] in order else 999, len(k[1]), k[1])):
        rs = by_scope[(rid, rel)]
        sub = ROOT[rid][6] if rid in ROOT else 'Outside the scope roots'
        if rid == 'DE.DAMAGE_DATA' and rel and rel[0] == 'BUFF_FACTOR':
            sub = 'BuffFactor (nested in DAMAGE_DATA)'
        if sub != cur_sub:
            w('### %s' % sub)
            w('')
            cur_sub = sub
        c = collections.Counter(r['status'] for r in rs)
        rdr = ''
        if rid in ROOT:
            jr = J_READERS_S(rid)
            mf = ', '.join('`%s`' % f.split('::')[-1] for _, f, _ in ROOT[rid][4][:3]) + (' …' if len(ROOT[rid][4]) > 3 else '')
            rdr = ' June: %s. March: %s.' % (jr, mf or 'none')
        w('#### `%s` %s' % (rid, path_s(rel)))
        w('')
        w('%d rows: %s.%s' % (len(rs), ', '.join('%s %d' % kv for kv in sorted(c.items())), rdr))
        w('')
        w('| key | status | phase | uses | June | March | semantics | leads / note |')
        w('|---|---|---|---|---|---|---|---|')
        for r in sorted(rs, key=lambda r: (STATUS_ORDER.index(r['status']), r['key'])):
            w('| `%s` | %s | %s | %s | %s | %s | %s | %s |' % (
                r['key'], r['status'] + ('(%s)' % r['system'] if r['status'] == 'unsupported' else '') +
                ('(%s)' % r['cat'] if r['status'] == 'june-ignores' else ''),
                r['phase'] if r['status'] in ('to-implement', 'unsupported', 'binary-only') or (r['sem'] or '').startswith('differ') else '',
                r['uses'] or '', cell(june_s(r)), cell(march_s(r)), cell(sem_s(r)), cell(note_s(r))))
        w('')
    # ---- code-built
    w('## Code-built keys')
    w('')
    w('Keys assigned in code (`slotN.KEY = ...` on a deep-copied templet) have no table path. Each is')
    w('covered by the rows for the same name in declared tables:')
    w('')
    for fam, c in sorted(code_keys.items()):
        for k, n in sorted(c.items()):
            cov = sorted(set('%s %s' % (r['root'], r['status']) for r in rows.values() if r['fam'] in (fam,) and (k in r['raw'] or r['key'] == R.norm(k) or k == r['key'])))
            w('- %s `%s` (%d): %s' % (fam, k, n, '; '.join(cov) if cov else '**no declared row**'))
    w('')
    # ---- absent systems
    w('## Absent systems')
    w('')
    w('Searched in the preprocessed parser TUs (compiled in under `US_SERVICE`), in raw `X2Lib/`,')
    w('`KTDXLIB/` and `KncWX2Server/Common/` (packets, shared enums) sources, and for the `#if` flag that encloses each raw hit (resolved by a `cl /P`')
    w('probe that includes X2Lib\'s `stdafx.h`). Terms are whole identifiers, listed below the table.')
    w('')
    w('- `absent`: no identifier anywhere in `X2Lib/` or `KTDXLIB/`.')
    w('- `gated-off`: code exists, but every enclosing flag is off under `US_SERVICE`.')
    w('- `present`: compiled into one of the parser TUs.')
    w('- `present-elsewhere`: compiled in (every enclosing flag ON), outside the parser TUs. The')
    w('  system exists, so its keys are ordinary work, not `unsupported`.')
    w('- `source-only`: code exists, but the enclosing flags disagree or the probe could not settle one.')
    w('')
    w('Only `absent` and `gated-off` make a key `unsupported` (plan Phase 8c: confirm the system is')
    w('absent before logging it as such). Hits inside a `%s*` block are the rebuild\'s own' % JUNE_FLAG_PREFIX)
    w('plan-rule-5 parse sites (parse, store, log): they are counted in the last column and are not')
    w('evidence. An unsupported row that March parses that way stays `unsupported`, with its March site.')
    w('')
    w('| system | keys (pattern) | verdict | evidence | compiled hits (parser TUs) | raw hits | enclosing flags | rule-5 hits (not evidence) |')
    w('|---|---|---|---|---|---|---|---|')
    for e in sysev:
        comp = ', '.join('%s %d' % kv for kv in e['compiled'].most_common(3)) or '0'
        raw = '%d in %d files (%s)' % (sum(e['raw'].values()), len(e['raw']), ', '.join('`%s`' % f for f, _ in e['raw'].most_common(2))) if e['raw'] else '0'
        gates = ', '.join('`%s` %s' % (g, {True: 'ON', False: 'off', None: '?'}[flag_state.get(g)]) for g, _ in e['gates'].most_common(5)) or '-'
        r5 = '%d in %d files (%s)' % (sum(e['rule5'].values()), len(e['rule5']), ', '.join('`%s`' % f for f, _ in e['rule5'].most_common(2))) if e['rule5'] else '0'
        w('| %s | `%s` | **%s** | %s | %s | %s | %s | %s |' % (e['name'], e['keypat'], sysv[e['name']], e['note'], comp, raw, gates, r5))
    w('')
    w('Search terms: %s.' % '; '.join('%s: %s' % (e['name'], ', '.join('`%s`' % t for t in e['terms'])) for e in sysev))
    w('')
    ur = [r for r in rows.values() if r['status'] == 'unsupported']
    w('Unsupported rows (%d): %s.' % (len(ur), ', '.join('`%s` (%s, %s)' % (r['key'], scope_s(r), r['system']) for r in sorted(ur, key=lambda r: r['key'])) or 'none'))
    w('')
    # ---- enums
    w('## Enum census')
    w('')
    w('Every `TABLE.MEMBER` value the in-scope June data uses, against June\'s `Enum.lua` and')
    w('`march2014/data036/Enum.src.lua` + `DungeonEnum.src.lua`. `reader` is the phase that owns the')
    w('key(s) using it: an enum member with no reader is not added (Phase 11 owns the data change).')
    w('')
    missing = [(k, e) for k, e in enum_rows.items() if k[1] not in march_enum.get(k[0], {})]
    differ = [(k, e) for k, e in enum_rows.items() if k[1] in march_enum.get(k[0], {}) and k[1] in june_enum.get(k[0], {})
              and march_enum[k[0]][k[1]] != june_enum[k[0]][k[1]]]
    tables = sorted(set(k[0] for k in enum_rows))
    w('%d members of %d tables used; %d missing from March; %d whose value differs between June and March.' % (
        len(enum_rows), len(tables), len(missing), len(differ)))
    w('')
    w('| table | member | uses | June `Enum.lua` | March | keys using it | reader (key status, phase) |')
    w('|---|---|---|---|---|---|---|')
    for (T, Mm), e in sorted(enum_rows.items()):
        jv = june_enum.get(T, {}).get(Mm)
        mv = march_enum.get(T, {}).get(Mm)
        mt = 'table missing' if T not in march_enum else ('**missing**' if mv is None else str(mv))
        jt = 'table missing' if T not in june_enum else ('missing' if jv is None else str(jv))
        if mv is not None and jv is not None and mv != jv:
            mt = '**%s (June %s)**' % (mv, jv)
        if mv is not None and jv is not None and mv == jv:
            continue_row = True
        else:
            continue_row = False
        if continue_row:
            continue
        rd = '; '.join(sorted(set('%s %s %s' % (k, s, p) for (_, k, s, p) in e['rows'])))[:160] or '-'
        w('| `%s` | `%s` | %d | %s | %s | %s | %s |' % (T, Mm, e['uses'], jt, mt, ', '.join('`%s`' % k for k, _ in e['keys'].most_common(3)), rd))
    w('')
    same = sum(1 for (T, Mm) in enum_rows if march_enum.get(T, {}).get(Mm) is not None and march_enum[T][Mm] == june_enum.get(T, {}).get(Mm))
    w('(%d members present in both with the same value are omitted.)' % same)
    w('')
    w('## Open questions for the user')
    w('')
    w('No phase of the plan owns these scopes. Their rows are `unassigned` rather than folded into')
    w('a neighbouring phase:')
    w('')
    OPENS = ('to-implement', 'unsupported', 'binary-only')
    unas = collections.OrderedDict()
    for r in rows.values():
        if r['phase'] == 'unassigned':
            k = 'nested `DAMAGE_DATA/BUFF_FACTOR/...` (the buff parser)' if r['root'] == 'DE.DAMAGE_DATA' else (
                '`%s` (%s)' % (r['root'], ROOT[r['root']][6]))
            c = unas.setdefault(k, collections.Counter())
            c[r['status']] += 1
    notes = {'InsertDamageData': "March has no reader for `Elsword_DamageData.lua` at all; June's reader 32621080 is a candidate",
             'DamageSound': "March reads it (`ParseDamageSoundBlock`); June's reader 32610ff0 is a candidate",
             'ES.Description': 'March reads it only under `#ifdef EFFECT_TOOL` (X2EffectSet.cpp:3380), compiled out of the client'}
    for k, c in unas.items():
        nid = next((n for n in notes if ('`%s`' % n) in k), None)
        w('- %s: %d open (%s)%s.' % (k, sum(c[x] for x in OPENS), ', '.join('%s %d' % kv for kv in sorted(c.items())),
                                     ('; ' + notes[nid]) if nid else ''))
    w('')
    open(MATRIX, 'w', encoding='utf-8', newline='\n').write('\n'.join(L))


STATUS_ORDER = ['to-implement', 'unsupported', 'binary-only', 'implemented', 'june-ignores']
_J = None


def J_READERS_S(rid):
    r = _J.readers(rid)
    return ', '.join('`%08x`' % f for f in r[:4]) + (' …' if len(r) > 4 else '')


def phase_key(p):
    m = re.match(r'(\d+)([a-z]?)', p or '')
    return (int(m.group(1)), m.group(2)) if m else (99, p or '')


def scope_s(r):
    return '`%s` %s' % (r['root'], path_s(r['rel']))


def cell(s):
    return (s or '').replace('|', '/').replace('\n', ' ')


def june_s(r):
    if r['status'] == 'june-ignores':
        return r['why'][:160]
    s = r['cat']
    if r['sites']:
        s += ' ' + ', '.join('%08x%s' % (f, ('@%08x' % va) if va else '') for f, va in r['sites'][:2])
    return s


def march_s(r):
    if not r['march']:
        return ''
    return ', '.join('%s:%d' % (tu.replace('.cpp', ''), ln) for tu, ln, _, _ in r['march'][:2])


def sem_s(r):
    if not r['sem']:
        return ''
    s = r['sem']
    if r['jsite'] and r['jsite'].get('post'):
        s += ' post: ' + r['jsite']['post']
    if r['jsite'] and r['jsite'].get('type'):
        s += ' [June %s = %s]' % (r['jsite']['type'], fmt(r['jsite'].get('default')))
    return s


def note_s(r):
    out = []
    if r['status'] in ('to-implement', 'unsupported', 'binary-only'):
        ld = r['leads']
        if ld['ncompiled']:
            out.append('compiled elsewhere: ' + ', '.join('%s:%d' % (t.replace('.cpp', ''), l) for t, l in ld['compiled'][:2]))
        if ld['nrawq']:
            out.append('raw literal %d (%s)' % (ld['nrawq'], ', '.join(ld['rawq'][:2])))
        if ld['bare']:
            out.append('bare id %d' % ld['bare'])
    if r['status'] == 'june-ignores' and r['march']:
        out.append('**March reads it, June does not**')
    if r.get('binonly') and r['status'] == 'implemented':
        out.append('binary-only: no June data uses it')
    if len(r['raw']) > 1 and '%d' in r['key']:
        out.append('%d names' % len(r['raw']))
    if r['example'] and r['status'] in ('to-implement', 'unsupported'):
        out.append('e.g. ' + r['example'])
    rt = RUNTIME_BY_KEY.get((r['root'], r['rel'], r['key']))
    if rt:
        out.append('runtime %s: %s (%s)' % (rt[3], rt[5], rt[4]))
    return '; '.join(out)


def jsite_s(r):
    j = r['jsite']
    if not j:
        return ''
    return '%08x → %s %s, default %s%s' % (j.get('call', 0), '%08x' % j['getter'] if j.get('getter') else '?', j.get('type'),
                                          fmt(j.get('default')), (' post ' + j['post']) if j.get('post') else '')


def msite_s(r):
    m = r.get('msite')
    return '%s:%d' % (m[0], m[1]) if m else ''


def write_tsv(rows):
    cols = ['family', 'root', 'subpath', 'key', 'status', 'phase', 'system', 'june_cat', 'june_sites', 'march_sites',
            'semantics', 'uses', 'files', 'names']
    out = ['\t'.join(cols)]
    for r in rows.values():
        out.append('\t'.join(str(x) for x in [
            r['fam'], r['root'], path_s(r['rel']), r['key'], r['status'], r['phase'], r['system'] or '', r['cat'],
            ' '.join('%08x@%s' % (f, ('%08x' % va) if va else '-') for f, va in r['sites'][:4]),
            ' '.join('%s:%d' % (tu, ln) for tu, ln, _, _ in r['march'][:4]), (r['sem'] or '').replace('\t', ' '),
            r['uses'], ' '.join('%s:%d' % kv for kv in sorted(r['files'].items())), ' '.join(sorted(r['raw']))]))
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')


def main():
    global _J
    ap = argparse.ArgumentParser()
    ap.add_argument('--summary', action='store_true')
    ap.add_argument('--reuse-pp', action='store_true')
    a = ap.parse_args()
    rows, code_keys, sysev, sysv, flag_state, enum_rows, june_enum, march_enum, J, M = build(a.reuse_pp)
    _J = J
    st = collections.Counter(r['status'] for r in rows.values())
    unres = [r for r in rows.values() if r['status'] == 'june-ignores' and r['cat'] in ('name-collision', 'plain-only', 'decoder-miss?', 'unrooted')]
    nophase = [r for r in rows.values() if r['status'] in ('to-implement', 'unsupported', 'binary-only') and r['phase'] in ('-', None)]
    uncovered = [(f, k) for f, c in code_keys.items() for k in c
                 if not any(r['fam'] == f and (k in r['raw'] or r['key'] == R.norm(k)) for r in rows.values())]
    checks = [
        (not unres, 'every `june-ignores` row has a certain reason (no `name-collision` / `plain-only` / `decoder-miss?` / unrooted sub-table)%s'
         % ('' if not unres else ': ' + ', '.join('`%s` %s (%s)' % (r['key'], scope_s(r), r['cat']) for r in unres[:12]))),
        (not nophase, 'every open row has an owning phase%s' % ('' if not nophase else ': ' + ', '.join('`%s`' % r['key'] for r in nophase[:12]))),
        (not uncovered, 'every code-built key is covered by a declared row%s' % ('' if not uncovered else ': ' + ', '.join('%s %s' % u for u in uncovered))),
        (True, 'every March function in the scope table was found in its preprocessed TU, except the declared '
               'compiled-out ones: %s' % '; '.join('`%s` (%s)' % kv for kv in sorted(COMPILED_OUT.items()))),
        (all(s in STATUS_ORDER for s in st), 'every row has exactly one status'),
    ]
    missing_funcs = [(t, f) for rid in ROOT for t, f, _ in ROOT[rid][4] if not M.tu[t].functions(f) and f not in COMPILED_OUT]
    back_in = [f for f in COMPILED_OUT if any(M.tu[t].functions(f) for rid in ROOT for t, ff, _ in ROOT[rid][4] if ff == f)]
    if missing_funcs or back_in:
        checks[3] = (False, 'March functions not found: %s; declared compiled-out but present: %s' % (
            ', '.join('%s %s' % mf for mf in missing_funcs) or 'none', ', '.join(back_in) or 'none'))
    rowkeys = set((r['root'], r['rel'], r['key']) for r in rows.values())
    bad_rt = ['`%s` %s `%s`' % (e[0], path_s(e[1]), e[2]) for e in RUNTIME if (e[0], e[1], e[2]) not in rowkeys]
    checks.append((not bad_rt, 'every hand-written `RUNTIME` citation names a row of the matrix%s'
                   % ('' if not bad_rt else ': ' + ', '.join(bad_rt))))
    semrows = set((r['root'], r['rel'], r['key']) for r in rows.values() if r['sem'] is not None)
    bad_hs = ['`%s` %s `%s`' % (k[0], path_s(k[1]), k[2]) for k in HAND_SEMANTICS if k not in semrows]
    checks.append((not bad_hs, 'every hand-verified `HAND_SEMANTICS` verdict (Phase 8a) names a March-read row%s'
                   % ('' if not bad_hs else ': ' + ', '.join(bad_hs))))
    no_rt = ['`%s` %s `%s`' % (r['root'], path_s(r['rel']), r['key']) for r in rows.values()
             if r['root'] in RUNTIME_SCOPES and r['march']
             and r['status'] in ('implemented', 'unsupported', 'binary-only')
             and (r['root'], r['rel'], r['key']) not in RUNTIME_BY_KEY]
    checks.append((not no_rt, 'every key March reads in %s, at every sub-path, has a runtime citation (Phase 4b exit; `ES.Description` is EFFECT_TOOL-only)%s'
                   % (', '.join('`%s`' % s for s in RUNTIME_SCOPES), '' if not no_rt else ': ' + ', '.join(sorted(no_rt)))))
    ph_rt = ['`%s`' % e[2] for e in RUNTIME if '@' in e[5]]
    checks.append((not ph_rt, 'no runtime citation is a phase placeholder (`4b@VA`)%s' % ('' if not ph_rt else ': ' + ', '.join(ph_rt))))
    open_rt = collections.Counter(e[4] for e in RUNTIME if e[4].startswith('stage'))
    checks.append((not open_rt, 'every runtime citation is transcribed: no `stage N` state left (Phase 4b done)%s'
                   % ('' if not open_rt else ': ' + ', '.join('%s %d' % kv for kv in sorted(open_rt.items())))))
    bad_lv = check_levels(J.B)
    checks.append((not bad_lv, 'every hand-written `ES_LEVELS` range of 321e48e0 starts at a BeginTable on its own name '
                   'and ends at its re-test / EndTable%s' % ('' if not bad_lv else ': ' + '; '.join(bad_lv))))
    open8c = ['`%s`' % r['key'] for r in rows.values() if r['root'] == 'DE.DAMAGE_DATA' and r['phase'] == '8c' and not r['march']]
    checks.append((not open8c, 'every phase-8c `DE.DAMAGE_DATA` row (absent or gated-off system) has a March parse site: '
                   'parsed, stored and logged under plan rule 5 (Phase 8c exit)%s' % ('' if not open8c else ': ' + ', '.join(sorted(open8c)))))
    ph = collections.Counter((r['phase'], r['status']) for r in rows.values())
    print('status: '+ ', '.join('%s %d' % (s, st[s]) for s in STATUS_ORDER))
    print('phase   to-implement unsupported binary-only implemented june-ignores')
    for p in sorted(set(p for p, _ in ph), key=phase_key):
        print('%-8s %12d %11d %11d %11d %12d' % (p, ph[(p, 'to-implement')], ph[(p, 'unsupported')], ph[(p, 'binary-only')],
                                                  ph[(p, 'implemented')], ph[(p, 'june-ignores')]))
    semd = sum(1 for r in rows.values() if (r['sem'] or '').startswith('differ'))
    print('semantics-differ: %d' % semd)
    for ok, text in checks:
        print(('PASS ' if ok else 'FAIL ') + text[:300])
    if not a.summary:
        write(rows, code_keys, sysev, sysv, flag_state, enum_rows, june_enum, march_enum, checks)
        write_tsv(rows)
        print('wrote %s and %s' % (os.path.relpath(MATRIX, TRUNK), os.path.relpath(TSV, TRUNK)))
    if not all(ok for ok, _ in checks):
        sys.exit(1)


if __name__ == '__main__':
    main()
