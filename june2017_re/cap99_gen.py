#!/usr/bin/env python3
"""cap99_gen.py - Atlas Phase A2 (level cap 80 -> 99): StatTable / NpcExpTable rows 81-99,
the EXP tables for the save migration, and the proofs behind all three.

    python june2017_re/cap99_gen.py stat    [--write]   fit every StatTable curve, prove 1-80, extrapolate 81-99
    python june2017_re/cap99_gen.py npc     [--write]   NpcExpTable 81-99 anchored to June's ExpTable; kills/level report
    python june2017_re/cap99_gen.py exp     [--header]  March/June ExpTable check; --header writes X2OfflineCap99Exp.h
    python june2017_re/cap99_gen.py all     [--write]   everything above

Without --write / --header nothing is written; the proofs print either way. Rerunnable: --write first strips
every line it generated before ("-- CAP99" tag) and restores every ReserveMemory it raised, so a second run
produces the same bytes.

Sources (all read-only):
  KncWX2Server/ServerResource/US/StatTable.lua      studio, UTF-8 BOM + CRLF, no newline at EOF
  KncWX2Server/ServerResource/US/NpcExpTable.lua    studio, UTF-8 BOM + CRLF, tabs inside the parentheses
  tools/data/march_exptable.tsv                     March data036 ExpTable.lua (shipped bytecode, T0 decompile)
  june_src_fixed/data036_kom/ExpTable.src.lua       June data036 ExpTable.lua

The invention of StatTable 81-99 and NpcExpTable 81-99 is user-authorised (ATLAS_PORT_PHASES.md,
"User decisions (2026-10-03)" item 4). Everything else here is read out of a cited file.
"""
import math
import os
import re
import sys
from collections import defaultdict
from fractions import Fraction as F

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
STAT_LUA = os.path.join(TRUNK, 'KncWX2Server', 'ServerResource', 'US', 'StatTable.lua')
NPC_LUA = os.path.join(TRUNK, 'KncWX2Server', 'ServerResource', 'US', 'NpcExpTable.lua')
MARCH_TSV = os.path.join(TRUNK, 'tools', 'data', 'march_exptable.tsv')
JUNE_TSV = os.path.join(TRUNK, 'tools', 'data', 'june_exptable.tsv')
JUNE_SRC = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom', 'ExpTable.src.lua')
HEADER = os.path.join(TRUNK, 'X2Lib', 'Offline', 'X2OfflineCap99Exp.h')

OLD_CAP, NEW_CAP = 80, 99
TAG = '-- CAP99'
STATS = ['AtkPhysic', 'AtkMagic', 'DefPhysic', 'DefMagic', 'HP']
HP_SCALE = 2            # HP moves in .5 steps: fit HP*2 as an integer


# --------------------------------------------------------------------------- EXP tables

def march_exp():
    """level -> TOTAL_EXP, March data036 ExpTable.lua (tools/data/march_exptable.tsv)."""
    t = {}
    for line in open(MARCH_TSV):
        if line.strip() and not line.startswith('#'):
            lv, tot = map(int, line.split())
            t[lv] = tot
    return t


def june_exp():
    """level -> (NEED_EXP, TOTAL_EXP), June data036 ExpTable.lua (june_src_fixed)."""
    src = open(JUNE_SRC, 'rb').read().decode('utf-8-sig')
    t = {}
    for blk in re.findall(r'AddExpTable_LUA\(\{(.*?)\}\)', src, re.S):
        kv = dict((k, int(v)) for k, v in re.findall(r'(\w+)\s*=\s*(-?\d+)', blk))
        t[kv['LEVEL']] = (kv['NEED_EXP'], kv['TOTAL_EXP'])
    return t


def need(total, lv):
    """NEED(L) = TOTAL(L) - TOTAL(L-1): the EXP that takes a character from L-1 to L
    (CX2EXPTable::GetLevel returns L-1 while exp < TOTAL(L), X2EXPTable.cpp)."""
    return total[lv] - total[lv - 1]


def cmd_exp(header):
    m = march_exp()
    j = june_exp()
    jt = dict((k, v[1]) for k, v in j.items())
    ok = True
    for name, tab, top in (('March', m, OLD_CAP), ('June', jt, 100)):
        lv = sorted(tab)
        if lv != list(range(1, top + 1)) or tab[1] != 0:
            print('EXP  FAIL %s levels %s..%s (%d rows), TOTAL(1)=%s' % (name, lv[0], lv[-1], len(lv), tab.get(1)))
            ok = False
        if any(tab[l] <= tab[l - 1] for l in lv[1:]):
            print('EXP  FAIL %s TOTAL_EXP not strictly increasing' % name)
            ok = False
    bad = [l for l in range(2, 101) if j[l][0] != need(jt, l)]
    if bad:
        print('EXP  FAIL June NEED_EXP != TOTAL(L)-TOTAL(L-1) at %s' % bad)
        ok = False
    print('EXP  March 1-%d TOTAL(80)=%d | June 1-100 TOTAL(80)=%d TOTAL(99)=%d TOTAL(100)=%d | NEED(L)=TOTAL(L)-TOTAL(L-1) %s'
          % (OLD_CAP, m[80], jt[80], jt[99], jt[100], 'holds' if not bad else 'FAILS'))
    if header and ok:
        write_header(m, jt)
        write_june_tsv(jt)
    return ok


def write_june_tsv(jt):
    lines = ['# level\ttotal_exp  -- June 2017 ExpTable.lua (data036; june_src_fixed/data036_kom/ExpTable.src.lua),'
             ' adopted whole by Atlas A2; the unit.exp a level-N character starts at']
    lines += ['%d\t%d' % (l, jt[l]) for l in sorted(jt)]
    data = ('\n'.join(lines) + '\n').encode('ascii')
    write_if_changed(JUNE_TSV, data)


def write_header(m, jt):
    def arr(tab, top):
        rows = []
        for i in range(0, top + 1, 5):
            rows.append('\t' + ' '.join('%d,' % (tab[l] if l in tab else 0) for l in range(i, min(i + 5, top + 1))))
        return '\n'.join(rows)
    text = '''#ifdef SERV_IRUHADEV_LEVEL_CAP_99
#pragma once

//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-10-16
// Description: Atlas Phase A2 - the two EXP tables the one-way EXP save migration
//              (CX2OfflineDB::Migrate, schema v16) converts between. GENERATED by
//              june2017_re/cap99_gen.py exp --header - do not edit by hand.
//
//              The migration runs from CX2OfflineDB::Open, before CX2StateLoading
//              calls CX2Data::LoadEXPTable, and after this phase the mounted
//              ExpTable.lua is June's - so March's table exists nowhere at run
//              time and both are carried here as cited constants:
//
//                MARCH  data036 ExpTable.lua as shipped in March 2014
//                       (tools/data/march_exptable.tsv; shipped bytecode, T0 decompile)
//                JUNE   data036 ExpTable.lua, June 2017
//                       (june_src_fixed/data036_kom/ExpTable.src.lua)
//
//              Index = level. TOTAL(L) is the cumulative EXP that reaches level L
//              (CX2EXPTable::GetLevel); NEED(L) = TOTAL(L) - TOTAL(L-1). Slot 0 is unused.
//////////////////////////////////////////////////////////////////////////

namespace X2OfflineCap99Exp
{
	enum
	{
		MARCH_MAX_LEVEL	= %d,
		JUNE_MAX_LEVEL	= %d,
	};

	const int MARCH_TOTAL_EXP[ MARCH_MAX_LEVEL + 1 ] =
	{
%s
	};

	const int JUNE_TOTAL_EXP[ JUNE_MAX_LEVEL + 1 ] =
	{
%s
	};
}

#endif SERV_IRUHADEV_LEVEL_CAP_99
''' % (OLD_CAP, 100, arr(m, OLD_CAP), arr(jt, 100))
    write_if_changed(HEADER, text.replace('\n', '\r\n').encode('ascii'))


def write_if_changed(path, data):
    old = open(path, 'rb').read() if os.path.exists(path) else None
    if old == data:
        print('WRITE unchanged %s' % os.path.relpath(path, TRUNK))
        return
    open(path, 'wb').write(data)
    print('WRITE %s (%d bytes)' % (os.path.relpath(path, TRUNK), len(data)))


# --------------------------------------------------------------------------- StatTable

ROW_RX = re.compile(r'^(--\s*)?StatTable:SetUnitStat\(\s*(\d+)\s*,\s*(\d+)\s*,\s*\{(.*?)\}\s*\)')


def parse_stat(text):
    """(live, commented): class -> level -> {stat: Fraction}."""
    live, dead = defaultdict(dict), defaultdict(dict)
    for line in text.split('\r\n'):
        if TAG in line:
            continue
        mo = ROW_RX.match(line.strip())
        if mo:
            kv = dict((k, F(v)) for k, v in re.findall(r'(\w+)\s*=\s*([\d.]+)', mo.group(4)))
            (dead if mo.group(1) else live)[int(mo.group(2))][int(mo.group(3))] = kv
    return live, dead


def feasible(pts):
    """Every (a, r) with floor(a + r*L) == v for all (L, v) in pts: the open interval of r, or None."""
    lo, hi = None, None
    for i in range(len(pts)):
        li, vi = pts[i]
        for k in range(i + 1, len(pts)):
            lk, vk = pts[k]
            d = lk - li
            a, b = F(vk - vi - 1, d), F(vk - vi + 1, d)
            lo = a if lo is None or a > lo else lo
            hi = b if hi is None or b < hi else hi
    return lo, hi


def offset_range(pts, r):
    alo = max(v - r * l for l, v in pts)
    ahi = min(v + 1 - r * l for l, v in pts)
    return alo, ahi


def pick_rate(pts):
    """The shortest decimal r (fewest digits, then smallest) for which some a reproduces pts exactly
    under floor(a + r*L). The studio's rates are short decimals (9.9, 8.55, 1.98, 1500 HP ...)."""
    if len(pts) == 1:
        return F(0), F(pts[0][1])
    lo, hi = feasible(pts)
    if lo is None or lo >= hi:
        return None
    for digits in range(0, 9):
        q = 10 ** digits
        n = math.floor(lo * q)
        while F(n, q) <= hi:
            r = F(n, q)
            alo, ahi = offset_range(pts, r)
            if alo < ahi:
                # The middle of the feasible intercepts, not alo (A2 review, user 2026-10-17): at alo one
                # level sits exactly on an integer, so an extrapolated value just under the next integer
                # floors 1 low (class 110/111 AtkMagic L92 vs the studio rows). Rows 1-80 are unchanged.
                return r, (alo + ahi) / 2
            n += 1
    return None


def segment(vals):
    """Greedy segmentation of level -> int value, from the top level down: each segment is the longest
    run ending at its top level that one floor(a + r*L) reproduces exactly. Returns [(lo, hi, r, a)], top first."""
    top, bot = max(vals), min(vals)
    for st in (bot, bot + 1):           # the common shapes first: one line, or level 1 on its own
        f = pick_rate([(l, vals[l]) for l in range(st, top + 1)])
        if f is not None:
            segs = [(st, top, f[0], f[1])]
            if st > bot:
                segs.append((bot, bot, F(0), F(vals[bot])))
            return segs
    segs = []
    end = top
    while end >= bot:
        st = end
        while st - 1 in vals and pick_rate([(l, vals[l]) for l in range(st - 1, end + 1)]) is not None:
            st -= 1
        r, a = pick_rate([(l, vals[l]) for l in range(st, end + 1)])
        segs.append((st, end, r, a))
        end = st - 1
    return segs


def model(segs, lv):
    for st, en, r, a in segs:
        if st <= lv <= en:
            return math.floor(a + r * lv)
    st, en, r, a = segs[0]          # past the top: the last segment, extended
    return math.floor(a + r * lv)


def fmt_rate(r, q):
    x = r / q
    return str(x) if x.denominator == 1 else ('%.6g' % float(x))


def fmt_num(x):
    return str(int(x)) if x == int(x) else ('%.1f' % float(x))


def fit_all(live):
    """class -> stat -> (segs, generated {81..99: Fraction})."""
    out = {}
    for c in live:
        out[c] = {}
        for s in STATS:
            # HP moves in .5 steps, so it is fitted doubled - unless the class stores it truncated to
            # whole numbers (class 115 does), in which case the truncation is the curve to continue.
            whole = all(live[c][l][s].denominator == 1 for l in range(1, OLD_CAP + 1))
            q = HP_SCALE if s == 'HP' and not whole else 1
            vals = dict((l, int(live[c][l][s] * q)) for l in range(1, OLD_CAP + 1))
            segs = segment(vals)
            gen = dict((l, F(model(segs, l), q)) for l in range(OLD_CAP + 1, NEW_CAP + 1))
            out[c][s] = (segs, gen, vals, q)
    return out


def cmd_stat(write):
    raw = open(STAT_LUA, 'rb').read()
    assert raw.startswith(b'\xef\xbb\xbf'), 'StatTable.lua lost its BOM'
    text = raw[3:].decode('utf-8')
    live, dead = parse_stat(text)
    classes = sorted(live)
    bad_shape = [c for c in classes if sorted(live[c]) != list(range(1, OLD_CAP + 1))]
    print('STAT %d classes with live rows, %d rows; levels 1-80 contiguous for all: %s; commented-out rows: %s'
          % (len(classes), sum(len(live[c]) for c in classes), not bad_shape,
             dict((c, '%d-%d' % (min(dead[c]), max(dead[c]))) for c in sorted(dead))))
    fits = fit_all(live)

    # Proof 1: the model reproduces every row 1-80 exactly.
    worst, shapes, l1off, rates = 0, defaultdict(int), 0, defaultdict(int)
    for c in classes:
        for s in STATS:
            segs, gen, vals, q = fits[c][s]
            res = max(abs(model(segs, l) - vals[l]) for l in vals)
            worst = max(worst, res)
            shapes[len(segs)] += 1
            if len(segs) > 1 and segs[-1][0] == segs[-1][1] == 1:
                l1off += 1
            top = segs[0]
            rates[(top[0], top[1])] += 1
    print('STAT PROOF 1 (reproduce 1-80): max residual %d over %d class x stat curves' % (worst, len(classes) * len(STATS)))
    print('STAT   segments per curve: %s; level 1 off-curve (own segment): %d' % (dict(shapes), l1off))
    print('STAT   top segment spans: %s' % dict(sorted(rates.items(), key=lambda kv: -kv[1])))

    # Proof 2: the studio's own commented-out rows 81-99 (classes 110, 111) against the extrapolation.
    # Independent of proof 1: those rows are not an input to the fit.
    total, agree, worst2 = 0, 0, 0
    for c in sorted(dead):
        for s in STATS:
            diffs = [(l, float(fits[c][s][1][l] - dead[c][l][s])) for l in range(OLD_CAP + 1, NEW_CAP + 1)]
            nz = [(l, d) for l, d in diffs if d]
            total += len(diffs)
            agree += len(diffs) - len(nz)
            worst2 = max([worst2] + [abs(d) for l, d in nz])
            top = fits[c][s][0][0]
            print('STAT PROOF 2 class %d %-9s rate %-9s (fit over %d-%d): %s'
                  % (c, s, fmt_rate(top[2], fits[c][s][3]), top[0], top[1],
                     'matches the studio rows 81-99' if not nz else 'differs %s' % nz))
    print('STAT PROOF 2 extrapolation vs studio rows: %d/%d values equal, max |diff| %g' % (agree, total, worst2))
    print('STAT   classes %s ship the studio rows themselves (uncommented copies); the other %d classes ship the extrapolation'
          % (sorted(c for c in dead if sorted(dead[c]) == list(range(OLD_CAP + 1, NEW_CAP + 1))),
             len([c for c in classes if not (c in dead and sorted(dead[c]) == list(range(OLD_CAP + 1, NEW_CAP + 1)))])))
    if write:
        if worst != 0:
            print('STAT refusing to write: proof 1 failed')
            return False
        write_stat(raw, text, live, dead, fits)
    return worst == 0


def write_stat(raw, text, live, dead, fits):
    lines = text.split('\r\n')
    # undo an earlier run: drop generated rows, restore raised ReserveMemory
    kept = []
    for line in lines:
        if line.startswith('StatTable:SetUnitStat') and TAG in line:
            continue
        mo = re.match(r'^(StatTable:ReserveMemory\(\s*\d+\s*,\s*)%d(\s*\).*?) %s: was %d$' % (NEW_CAP, TAG, OLD_CAP), line)
        if mo:
            line = mo.group(1) + str(OLD_CAP) + mo.group(2)
        kept.append(line)
    lines = kept
    out = []
    raised = 0
    for line in lines:
        mo = re.match(r'^(StatTable:ReserveMemory\(\s*\d+\s*,\s*)%d(\s*\).*)$' % OLD_CAP, line)
        if mo:
            line = '%s%d%s %s: was %d' % (mo.group(1), NEW_CAP, mo.group(2), TAG, OLD_CAP)
            raised += 1
        out.append(line)
        rm = ROW_RX.match(line)
        if rm and not rm.group(1) and int(rm.group(3)) == OLD_CAP:
            c = int(rm.group(2))
            studio = c in dead and sorted(dead[c]) == list(range(OLD_CAP + 1, NEW_CAP + 1))
            for l in range(OLD_CAP + 1, NEW_CAP + 1):
                v = dict((s, (dead[c][l] if studio else dict((k, fits[c][k][1][l]) for k in STATS))[s]) for s in STATS)
                note = ('studio row (the commented-out row %d,%d below, uncommented)' % (c, l) if studio
                        else 'generated (june2017_re/cap99_gen.py)')
                out.append('StatTable:SetUnitStat( %d, %d, { AtkPhysic = %s, AtkMagic = %s, DefPhysic = %s, DefMagic = %s,'
                           ' HP = %s } ) %s %s'
                           % (c, l, fmt_num(v['AtkPhysic']), fmt_num(v['AtkMagic']), fmt_num(v['DefPhysic']),
                              fmt_num(v['DefMagic']), fmt_num(v['HP']), TAG, note))
    data = b'\xef\xbb\xbf' + '\r\n'.join(out).encode('utf-8')
    print('STAT ReserveMemory raised to %d: %d lines; rows added: %d' % (NEW_CAP, raised, (NEW_CAP - OLD_CAP) * len(live)))
    write_if_changed(STAT_LUA, data)


# --------------------------------------------------------------------------- NpcExpTable

NPC_RX = re.compile(r'^DropTable:AddNpcExpTable\(\t(\d+)\t,\t(\d+)\t\)')
K_FIT = range(70, OLD_CAP + 1)


def parse_npc(text):
    t = {}
    for line in text.split('\r\n'):
        if TAG in line:
            continue
        mo = NPC_RX.match(line)
        if mo:
            t[int(mo.group(1))] = int(mo.group(2))
    return t


def kills(total, npc, lv):
    """Kills of level-L monsters to go from L to L+1: NEED(L+1) / npcExp(L). One convention, fit and report."""
    if lv + 1 not in total or lv not in npc:
        return None
    return need(total, lv + 1) / float(npc[lv])


def cmd_npc(write):
    raw = open(NPC_LUA, 'rb').read()
    assert raw.startswith(b'\xef\xbb\xbf'), 'NpcExpTable.lua lost its BOM'
    text = raw[3:].decode('utf-8')
    npc = parse_npc(text)
    assert sorted(npc) == list(range(1, OLD_CAP + 1)), 'NpcExpTable.lua is not levels 1-80'
    m = march_exp()
    jt = dict((k, v[1]) for k, v in june_exp().items())
    # k(L) over 70-80 with June's NEED and March's npc rows, least-squares line, extrapolated
    xs = list(K_FIT)
    ks = [kills(jt, npc, l) for l in xs]
    n = len(xs)
    mx, my = sum(xs) / float(n), sum(ks) / float(n)
    slope = sum((x - mx) * (y - my) for x, y in zip(xs, ks)) / sum((x - mx) ** 2 for x in xs)
    icpt = my - slope * mx
    gen = {}
    for l in range(OLD_CAP + 1, NEW_CAP + 1):
        k = icpt + slope * l
        assert k > 0, 'extrapolated kills-per-level went non-positive at %d' % l
        gen[l] = int(math.floor(need(jt, l + 1) / k + 0.5))
    full = dict(npc)
    full.update(gen)
    mono = all(full[l] > full[l - 1] for l in range(2, NEW_CAP + 1))
    print('NPC  k(L) = NEED_june(L+1) / npcExp(L), fitted over %d-%d: k = %.4f + %.5f*L  (k70=%.2f k80=%.2f)'
          % (xs[0], xs[-1], icpt, slope, ks[0], ks[-1]))
    print('NPC  rows 81-99: %s' % ' '.join('%d:%d' % (l, gen[l]) for l in sorted(gen)))
    print('NPC  npcExp strictly increasing 1-99: %s' % mono)
    print('NPC  kills per level on own-level monsters (raw table, before the offline EXP boost):')
    print('NPC    L    old (March ExpTable + March npc)   new (June ExpTable + npc 1-99)')
    for l in (50, 70, 80, 90):
        old = kills(m, npc, l)
        new = kills(jt, full, l)
        print('NPC   %3d   %s   %s' % (l, ('%8.1f' % old) if old else '     n/a (cap 80)', '%8.1f' % new))
    if write:
        if not mono:
            print('NPC  refusing to write: not monotonic')
            return False
        write_npc(raw, text, gen)
    return mono


def write_npc(raw, text, gen):
    lines = [l for l in text.split('\r\n') if TAG not in l]
    # the file ends in CRLF, so the split leaves one empty string at the end
    assert lines[-1] == '', 'NpcExpTable.lua no longer ends in CRLF'
    body = lines[:-1]
    last = max(i for i, l in enumerate(body) if NPC_RX.match(l))
    add = ['DropTable:AddNpcExpTable(\t%d\t,\t%d\t)\t%s generated (june2017_re/cap99_gen.py)' % (l, gen[l], TAG)
           for l in sorted(gen)]
    body = body[:last + 1] + add + body[last + 1:]
    data = b'\xef\xbb\xbf' + ('\r\n'.join(body) + '\r\n').encode('utf-8')
    write_if_changed(NPC_LUA, data)


# --------------------------------------------------------------------------- main

def main(argv):
    if not argv or argv[0] not in ('stat', 'npc', 'exp', 'all'):
        print(__doc__)
        return 2
    write = '--write' in argv
    ok = True
    if argv[0] in ('exp', 'all'):
        ok = cmd_exp('--header' in argv or (argv[0] == 'all' and write)) and ok
    if argv[0] in ('stat', 'all'):
        ok = cmd_stat(write) and ok
    if argv[0] in ('npc', 'all'):
        ok = cmd_npc(write) and ok
    print('RESULT %s' % ('PASS' if ok else 'FAIL'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
