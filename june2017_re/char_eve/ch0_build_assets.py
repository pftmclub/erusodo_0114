"""Character-port CH0 item 5: every June file that is one character's, by three routes, unioned (read-only; writes only the tsvs).

    python june2017_re/char_eve/ch0_build_assets.py [eve]  [--out DIR] [--closure TSV]

Routes (columns `route`): a = by name over every June kom (a-abbr job tokens, a-short delimited short tokens, a-job full job names),
b = by reference from the character's data (class script literals, UnitTemplet rows, effect closure, textures inside meshes),
c = by reference from equipment (June item.kim items of her unit type / classes, plus the items her UnitTemplet rows name).
Output: ch0_assets.tsv (+ `# total` lines) and ch0b_stage_list.tsv (what `kom.py stage-june` takes, with its placement rules).
Reuses june2017_re/charport_closure.py (Data: June / March inventories, file_status) and tools/kim.py. A later character adds an entry
to CHARS (tokens, abbr, unit type / classes, Lua owners) and runs it with that key; run charport_closure.py for it first.
Never writes into a kom, kom_contents or the game dir.
"""
import argparse
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(TRUNK, 'june2017_re'))
sys.path.insert(0, os.path.join(TRUNK, 'tools'))
import charport_closure as CC   # noqa: E402
import fxdata as F              # noqa: E402
import junekom as JK            # noqa: E402
import kim as KIM               # noqa: E402
import kom as KOM               # noqa: E402

LONG_NAME = 59

TOK = re.compile(r'[A-Z]{2,}(?![a-z])|[A-Z][a-z]+|[a-z]+|\d+')


def toks(name):
    base = name.rsplit('.', 1)[0] if '.' in name else name
    return TOK.findall(base)


# --------------------------------------------------------------------------------------------- characters
# owner_rules: first match wins; (regex on the file name, owner, phase, note). Applied to every kind.
EVE_SKILL_LUA = ['Atomic_Shield_Eve', 'Guardian_Eve', 'Guardian_Sp1_Eve', 'Guardian_Sp1_Eve_Memo', 'Guardian_Sp2_Eve',
                 'Guardian_Sp2_Eve_Memo', 'Nasod_Watch_Eve', 'Nasod_Watch_Eve_Memo', 'Nasod_Element_Fire_Eve', 'Wally9th_Eve',
                 'SI_HA_FERDINAND', 'SI_SA_EEP_ELECTRONIC_FIELD', 'Eve_AT_Sp2a_SonicWave', 'SI_SA_AT_SURFACE_CUTING']
CHARS = {
    'eve': dict(
        owner='char-eve',
        script='Eve_Nasod', lo=0x32965dd0, hi=0x329cf86f,
        unit_type=5, unit_type_name='UT_EVE',
        unit_classes=[5, 18, 19, 26, 108, 109, 116],
        tokens=['eve'],                                    # whole token, any case ("Event" / "EVENT" / "Seven" are other tokens)
        abbr=['EEG', 'EAT', 'ENS', 'EEP', 'EPR', 'EBS', 'ECN', 'ECE', 'ECB'],     # whole token, upper case
        abbr_exclude=re.compile(r'(?i)(^|_)pet_|_EVENT_EAT_|^HQ_Shop_Pet'),
        short_re=re.compile(r'^(?:SI_(?:SA|A|HA)_|Effect_|EFFECT_)?(?:NS|EP|AT|EG|BS)_'),      # short tokens: only as a name prefix
        job_re=re.compile(r'(?i)nemesis|empress|seraph|electra|codearchitecture|codeexotic'),
        lua_script='Eve_Nasod',
        skill_lua=EVE_SKILL_LUA,
        motion_glob=re.compile(r'(?i)^Motion_EVE'),
        tutorial_re=re.compile(r'(?i)^tuto'),              # a token starting "tuto": Tutorial_Eve_*, Tuto_Eve_*, Tutotial_Eve_*, EveTutorial
        owner_rules=[
            (r'(?i)^DLG_(1ST|2ND)_Class_Change_Guide_Eve|^DLG_UI_JUMPING_SYSTEM_EVE_|^DLG_Class_Change_|^DLG_Unit_(Create|Select)_|'
             r'^DLG_PVP_Game_Damaged_Eve|^DLG_UI_JUMPING_EVENT_EVE', 'stage9-ui', '-', 'class-change / jumping / unit dialog'),
            (r'(?i)^DLG_Transcendental_Change_Guide_Eve', 'char-eve', 'CH3d', 'transcendence guide (Stage 6 pattern)'),
            (r'(?i)^(PET_|Pet_|HQ_Shop_Pet)', 'stage17-pet', '-', 'pet asset'),
            (r'(?i)^POPUP_TR_', 'stage6-transcendence', '-', 'transcendence job popup UI'),
        ],
    ),
}

FILE_EXT = ('.dds', '.tga', '.bmp', '.png', '.jpg', '.jpeg', '.x', '.y', '.xet', '.ksm', '.ogg', '.wav', '.mp3', '.tet', '.gfx',
            '.txt', '.avi', '.lua')
TEX = ('.dds', '.tga', '.bmp', '.png', '.jpg', '.jpeg', '.psd')
MESH = ('.x', '.y', '.xet', '.ksm')
SND = ('.ogg', '.wav', '.mp3')


def log(*a):
    sys.stderr.write(' '.join(str(x) for x in a) + '\n')
    sys.stderr.flush()


def ext(n):
    return os.path.splitext(n)[1].lower()


def kind_of(name):
    e = ext(name)
    if e == '.lua':
        return 'lua'
    if e == '.gfx':
        return 'gfx'
    if e in TEX:
        return 'texture'
    if e in SND:
        return 'sound'
    if e in ('.x',) and name.lower().startswith('motion_'):
        return 'motion'
    if e in MESH:
        return 'mesh'
    return 'other'


class Info(object):
    def __init__(self, name):
        self.name = name
        self.routes = set()
        self.why = []

    def add(self, route, why=''):
        self.routes.add(route)
        if why and why not in self.why and len(self.why) < 3:
            self.why.append(why)


ROUTE_ORDER = ['a', 'a-abbr', 'a-short', 'a-job', 'b', 'c']


def route_str(rs):
    return '+'.join(r for r in ROUTE_ORDER if r in rs)



# --------------------------------------------------------------------------------------------- motion question
# march_data/dataNNN holds the ORIGINAL March payloads of the Motion files, XOR-ed with a 12-byte key (found by xor-ing one with its
# kom_contents extract; every file gives the same key). kom_contents may already hold June's bytes (Motion_EVE_Common.x does).
MARCH_DATA = r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\march_data'
MARCH_XOR = bytes.fromhex('02aaf8c6dcab4726efbb0098')
KSM_STR = re.compile(rb'[\x03-\x50]\x00\x00\x00(?=[A-Za-z_])')


def ksm_names(d):
    """Length-prefixed identifier strings of a KSM motion file (bone, mesh and animation-set names)."""
    out = []
    for m in KSM_STR.finditer(d):
        ln = d[m.start()]
        s = d[m.start() + 4:m.start() + 4 + ln]
        if len(s) == ln and re.fullmatch(rb'[A-Za-z_][A-Za-z0-9_]*', s):
            out.append(s.decode())
    return out


def motion_question(C, out_dir):
    """Names a same-name June Eve motion file lacks vs March's original, and who plays them. Writes ch0_motion_question.tsv."""
    june = {}
    for kp in JK.all_koms()[0]:
        k = JK.Kom(kp)
        for e in k.entries:
            if C['motion_glob'].match(e['name']) and ext(e['name']) == '.x' and not e['name'].lower().startswith('motion_event'):
                with open(kp, 'rb') as f:
                    f.seek(e['offset'])
                    raw = f.read(e['csize'])
                d, err = JK.decode(e, raw, k.v04)
                if d is not None:
                    june[e['name']] = (d, k.stem)
    march = {}
    for dn in ('data033', 'data012'):
        dd = os.path.join(MARCH_DATA, dn)
        if os.path.isdir(dd):
            for f in os.listdir(dd):
                if C['motion_glob'].match(f) and not f.lower().startswith('motion_event'):
                    raw = open(os.path.join(dd, f), 'rb').read()
                    march[f] = raw if raw[:4] == b'KSM ' else bytes(c ^ MARCH_XOR[i % 12] for i, c in enumerate(raw))
    lua_m = open(os.path.join(MARCH_DATA, 'data036', C['lua_script'] + '.lua'), 'rb').read()
    lua_j = open(os.path.join(CC.SRC_DIR, C['lua_script'] + '.src.lua'), 'rb').read().decode('utf-8', 'replace').split('\n')
    srcs = []
    for dn in ('X2Lib', 'KTDXLIB'):
        for f in sorted(os.listdir(os.path.join(TRUNK, dn))):
            if f.lower().endswith(('.cpp', '.h', '.inl')):
                srcs.append((dn + '/' + f, open(os.path.join(TRUNK, dn, f), 'rb').read().decode('latin-1').split('\n')))

    def march_play(name):
        hits = []
        pos = [m.start() for m in re.finditer(re.escape(bytes([5 + len(name)]) + name.encode()), lua_m)]
        if pos:
            hits.append('%s.lua bytecode const x%d' % (C['lua_script'], len(pos)))
        for fn, lines in srcs:
            for i, l in enumerate(lines):
                if '"' + name + '"' in l:
                    hits.append('%s:%d' % (fn, i + 1))
                    break
        return '; '.join(hits[:4]) if hits else 'no'

    def june_play(name):
        ls = [str(i + 1) for i, l in enumerate(lua_j) if '"' + name + '"' in l]
        if not ls:
            return 'no'
        return '%s.src.lua:%s%s' % (C['lua_script'], ','.join(ls[:4]), ' (+%d)' % (len(ls) - 4) if len(ls) > 4 else '')

    anim_tab = open(os.path.join(CC.SRC_DIR, 'AnimationNameTable.src.lua'), 'rb').read().decode('utf-8', 'replace').split('\n')
    names_m = dict((f, set(ksm_names(d))) for f, d in march.items())
    names_j = dict((f, set(ksm_names(d[0]))) for f, d in june.items())
    rows, summary = [], []
    for f in sorted(march, key=str.lower):
        jf = next((x for x in june if x.lower() == f.lower()), None)
        if jf is None:
            continue
        nm, nj = names_m[f], names_j[jf]
        dropped, new = sorted(nm - nj), sorted(nj - nm)
        same_bytes = march[f] == june[jf][0]
        summary.append((f, len(march[f]), len(june[jf][0]), len(dropped), len(new), same_bytes))
        if not dropped:
            rows.append((f, '(no name dropped)', '-', '-', '-', '-', 'March %d B, June %d B; %d new names%s' % (
                len(march[f]), len(june[jf][0]), len(new), '; identical bytes' if same_bytes else '')))
        for a in dropped:
            land = sorted(x for x in june if x != jf and a in names_j[x])
            ci = [x for x in nj if x.lower() == a.lower()]
            if land:
                note = 'moved to June %s' % ','.join(land)
            elif ci:
                note = 'case rename -> June has %s (June plays it through AnimationNameTable.lua:%s)' % (ci[0], ','.join(
                    str(i + 1) for i, l in enumerate(anim_tab) if '"' + ci[0] + '"' in l) or 'no')
            else:
                note = 'not in any June Eve motion file'
            rows.append((f, a, 'yes', 'no', march_play(a), june_play(a) if not ci else june_play(ci[0]), note))
        for a in new:
            rows.append((f, a, 'no', 'yes', march_play(a), june_play(a), 'new in June'))
    with open(os.path.join(out_dir, 'ch0_motion_question.tsv'), 'w', encoding='utf-8', newline='\n') as fh:
        fh.write('file\tanimation\tin_march\tin_june\tplayed_by_march_code\tplayed_by_june_script\tnote\n')
        for r in rows:
            fh.write('\t'.join(r) + '\n')
        fh.write('# per file: March bytes, June bytes, dropped names, new names, identical\n')
        for sm in summary:
            fh.write('# %s\t%d\t%d\tdropped=%d\tnew=%d\t%s\n' % (sm[0], sm[1], sm[2], sm[3], sm[4], 'identical' if sm[5] else 'differs'))
    log('motion question rows', len(rows))
    for r in rows:
        if r[2] == 'yes':
            log('  dropped', r[0], r[1], r[4], '|', r[5], '|', r[6])

# --------------------------------------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('char', nargs='?', default='eve')
    ap.add_argument('--out', default=HERE)
    ap.add_argument('--closure', default=None, help='effect closure tsv (default: <out>/ch0_effect_closure.tsv)')
    ap.add_argument('--motion-only', action='store_true', help='only write ch0_motion_question.tsv')
    a = ap.parse_args()
    C = CHARS[a.char]
    owner = C['owner']
    closure_tsv = a.closure or os.path.join(a.out, 'ch0_effect_closure.tsv')
    if a.motion_only:
        motion_question(C, a.out)
        return

    D = CC.Data()
    june = D.jun.inv                                   # lower -> [Copy]
    names_all = {}
    for low, cs in june.items():
        names_all[low] = cs[0].name
    log('june names', len(names_all))

    # March: kom_contents (D.mpath/mkoms) plus live-only
    live_size = {}
    for kp in sorted(os.listdir(F.MAR_DATA)):
        if not re.match(r'(?i)^datai?\d{3}\.kom$', kp):
            continue
        k = JK.Kom(os.path.join(F.MAR_DATA, kp))
        for e in k.entries:
            live_size.setdefault(e['name'].lower(), {})[k.stem] = e['size']

    info = collections.OrderedDict()                   # lower -> Info

    def add(name, route, why=''):
        low = name.lower()
        if low not in info:
            info[low] = Info(names_all.get(low, name))
        info[low].add(route, why)

    # ---- route a
    tokset = set(C['tokens'])
    for low, nm in names_all.items():
        ts = toks(nm)
        tl = [t.lower() for t in ts]
        base = nm.rsplit('.', 1)[0] if '.' in nm else nm
        if any(t in tokset for t in tl):
            add(nm, 'a', 'token ' + '/'.join(t for t in ts if t.lower() in tokset))
            continue
        hit = [t for t in ts if t in C['abbr']]
        if hit and not C['abbr_exclude'].search(nm):
            add(nm, 'a-abbr', 'abbr token ' + hit[0])
            continue
        if C['short_re'].match(base + '_') and not nm.lower().startswith('dlg_'):
            add(nm, 'a-short', 'short token prefix')
            continue
        if C['job_re'].search(nm) and not any(t.lower() == 'event' for t in ts):
            add(nm, 'a-job', 'job name')
    log('route a rows', len(info))
    n_a = len(info)

    # ---- route b1: class script literals
    sp = os.path.join(CC.SRC_DIR, C['script'] + '.src.lua')
    states = CC.script_states(sp)
    script_files = set()
    for st, strs in states.items():
        for s in strs:
            if s and ext(s) in FILE_EXT and '%' not in s:
                script_files.add(s)
    for s in sorted(script_files):
        add(s, 'b', 'named in %s' % C['script'])
    for sl in C['skill_lua']:
        add(sl + '.lua', 'b', 'skill NPC / summon Lua named by the class code')
    # b2: UnitTemplet rows of this unit type
    ut = open(os.path.join(CC.SRC_DIR, 'UnitTemplet.src.lua'), 'rb').read().decode('utf-8', 'replace')
    rows = re.findall(r'AddUnitTemplet\(\{(.*?)\}\)', ut, re.S)
    ut_items = set()
    n_ut_rows = 0
    for r in rows:
        if 'UNIT_TYPE.%s' % C['unit_type_name'] not in r:
            continue
        n_ut_rows += 1
        for s in re.findall(r'"([^"]+)"', r):
            if ext(s) in FILE_EXT:
                add(s, 'b', 'UnitTemplet row')
        for m in re.finditer(r'(?:basic|promotion)\w*ItemID\s*=\s*(\d+)', r):
            ut_items.add(int(m.group(1)))
    log('UnitTemplet rows', n_ut_rows, 'items', len(ut_items))
    # b3: effect closure files
    closure_n = 0
    with open(closure_tsv, encoding='utf-8') as f:
        next(f)
        for line in f:
            if line.startswith('#'):
                continue
            p = line.rstrip('\n').split('\t')
            if len(p) < 4 or p[1] not in ('texture', 'mesh', 'sound', 'other'):
                continue
            if ext(p[2]) in FILE_EXT:
                add(p[2], 'b', 'effect closure')
                closure_n += 1
    log('closure file rows', closure_n)

    # ---- route c: item.kim
    k = KIM.Kim('june')
    item_ids = []
    for i in k.ids():
        r = k.raw(i)
        if r['unit_type'] == C['unit_type'] or r['unit_class'] in C['unit_classes'] or i in ut_items:
            item_ids.append(i)
    log('kim items', len(item_ids))
    part_roots = set()
    c_files = 0
    for i in item_ids:
        rec = k.record(i)
        for m in range(k.L['models']):
            v = rec.get('model_name_%d' % m, '')
            if v and ext(v) in FILE_EXT:
                add(v, 'c', 'item %d' % i)
                c_files += 1
        for key in ('texture_change_xet', 'ani_xet', 'shop_image'):
            v = rec.get(key, '')
            if v and ext(v) in FILE_EXT:
                add(v, 'c', 'item %d %s' % (i, key))
                c_files += 1
        for e in rec['attached_meshes']:
            if e['name'] and ext(e['name']) in FILE_EXT:
                add(e['name'], 'c', 'item %d attached mesh' % i)
        for e in rec['attached_particles']:
            if e['name']:
                part_roots.add(e['name'])
        for g in ('common_model', 'common_xet'):
            for kk, nm, v in rec['extra'].get(g, []):
                if v and ext(v) in FILE_EXT:
                    add(v, 'c', 'item %d %s' % (i, g))
    # attached particles are templet names: walk them with the closure rules
    n_part_files = 0
    seen = set()
    queue = collections.deque()
    for p in sorted(part_roots):
        for typ in D.templet_ns.get(p, []):
            queue.append((typ, p))
    while queue:
        typ, nm = queue.popleft()
        key = (typ, nm.lower() if typ == 'file' else nm)
        if key in seen:
            continue
        seen.add(key)
        if typ == 'file':
            add(nm, 'c', 'item attached particle')
            n_part_files += 1
        for ctyp, cname, _k in CC.children(D, typ, nm):
            queue.append((ctyp, cname))
    log('attached particle templets', len(part_roots), 'files', n_part_files)

    def exists(n):
        return n.lower() in names_all or n.lower() in D.mpath or n.lower() in live_size

    def repair(t):
        """A scanned name can start with junk bytes (a length prefix read as a letter, '(' ')' '+'): drop 1-2 leading chars if that finds a real file."""
        if exists(t):
            return t
        for cut in (1, 2):
            if len(t) > cut + 4 and exists(t[cut:]):
                return t[cut:]
        u = t.lstrip('()+-.~ ')
        if u != t and exists(u):
            return u
        return t

    # ---- b4: textures inside every mesh / xet collected so far (one pass over the union, then once over what that added)
    for _pass in range(2):
        added = 0
        for low in list(info):
            if ext(low) in MESH and not getattr(info[low], 'scanned', False):
                info[low].scanned = True
                for t in D.embedded(low):
                    t = repair(t)
                    tl = t.lower()
                    if tl not in info:
                        added += 1
                    add(t, 'b', 'texture named inside %s' % info[low].name)
        log('embedded pass', _pass, 'new', added)

    # ---- resolve
    ledger_lua = {}
    rec_path = os.path.join(TRUNK, 'june2017_re', 'june_npc_lua_port_20261009.tsv')
    port_rec = {}
    for line in open(rec_path, encoding='utf-8'):
        if line.startswith('#') or line.startswith('name\t'):
            continue
        p = line.rstrip('\n').split('\t')
        if len(p) > 2:
            port_rec[p[0].lower()] = p[2]
    core = set()
    cl = os.path.join(TRUNK, 'tools', 'data', 'lua_core_list.tsv')
    if os.path.isfile(cl):
        for line in open(cl, encoding='utf-8'):
            p = line.rstrip('\n').split('\t')
            if len(p) > 1:
                core.add(p[1].lower())
    ljm = None
    out_rows = []
    for low, inf in info.items():
        nm = inf.name
        cs = june.get(low, [])
        jkoms = sorted(set(c.kom for c in cs))
        jsizes = sorted(set(c.size for c in cs))
        jk = ','.join(jkoms) if jkoms else '-'
        js = '/'.join(str(s) for s in jsizes) if jsizes else '-'
        mk = list(D.mkoms.get(low, []))
        msz = None
        if low in D.mpath:
            msz = os.path.getsize(D.mpath[low])
        lv = live_size.get(low, {})
        for s in lv:
            if s not in mk:
                mk.append(s)
        if msz is None and lv:
            msz = sorted(set(lv.values()))[0]
        mkom = ','.join(sorted(mk)) if mk else '-'
        msize = str(msz) if msz is not None else '-'
        kind = kind_of(nm)
        note = []
        if kind == 'lua':
            if not cs:
                state = 'march-only' if mk else 'dangling'
            elif not mk:
                state = 'missing'
            else:
                jb = D.june_bytes(low)
                mp = D.mpath.get(low)
                state = 'march-copy'
                if jb is not None and mp:
                    mb = open(mp, 'rb').read()
                    cand = jb
                    if jb[:4] == b'\x1bKL\x82':
                        try:
                            if ljm is None:
                                sys.path.insert(0, os.path.join(TRUNK, 'tools'))
                                import luasrc
                                ljm = luasrc.ljconv_mod()
                            cand = ljm[0].transcode(jb, ljm[1])[0]
                        except Exception as e:      # noqa: BLE001
                            note.append('ljconv failed: %s' % str(e)[:60])
                            cand = None
                    if cand is not None and cand == mb:
                        state = 'june-bytes'
                if low in core:
                    note.append('core list (kept merged)')
        else:
            if not cs:
                state = 'march-only' if mk else 'dangling'
                note.append('named by %s but in no June kom' % '/'.join(inf.why[:1] or ['?']) if state == 'dangling' else
                            'March-only (no June counterpart)')
            else:
                state, n = D.file_status(low)
                if n:
                    note.append(n)
        # owner / phase
        ow, ph = owner, 'CH0b'
        oc = None
        for rx, o2, p2, n2 in C['owner_rules']:
            if re.search(rx, nm):
                ow, ph, oc = o2, p2, n2
                break
        if oc:
            note.append(oc)
        elif any(C['tutorial_re'].search(t) for t in toks(nm)) and any(t.lower() in tokset for t in toks(nm)):
            ow, ph = 'june-dungeons', '-'
            note.append('Eve tutorial dungeon asset, not the character')
        elif kind == 'lua':
            stem = nm.rsplit('.', 1)[0]
            if stem.lower() == C['lua_script'].lower():
                ph = 'CH1'
                note.append('class script')
            elif stem in C['skill_lua'] or stem.lower() in [s.lower() for s in C['skill_lua']]:
                if state == 'june-bytes' or low in port_rec:
                    ph = 'done-elsewhere'
                    note.append('skill NPC / summon Lua; already June (ljconv) per june_npc_lua_port_20261009.tsv' if low in port_rec else
                                'skill NPC / summon Lua; bytes == ljconv(June)')
                else:
                    ph = 'CH1'
                    note.append('skill NPC / summon Lua')
            else:
                if low in port_rec:
                    ow, ph = 'june-npc-lua-ljconv', 'done-elsewhere'
                    note.append("adopted by record june_npc_lua_port_20261009.tsv (%s)" % port_rec[low])
                elif state == 'june-bytes':
                    ow, ph = 'june-npc-lua-ljconv', 'done-elsewhere'
                    note.append('bytes == ljconv(June); no adopt record row')
                elif state == 'march-only':
                    ow, ph = 'june-npc-lua', '-'
                    note.append('March-only Lua (no June counterpart); not adopted')
                else:
                    ow, ph = 'june-npc-lua', '-'
                    note.append('NPC / event / shadow / bot Lua; not yet adopted (check lua ledger / adopt-june)')
        if kind == 'motion':
            note.append('motion; Emotion behaviour is Stage 10') if 'Emotion' in nm else None
        if state in ('dangling',):
            ph = '-'
            if ow == owner:
                ow = '-'
        if state == 'march-only' and ow == owner:
            ph = '-'
        if ow == owner and ph == 'CH0b' and state == 'june-bytes':
            note.append('bytes==June')
        out_rows.append(dict(file=nm, kind=kind, route=route_str(inf.routes), jk=jk, js=js, mk=mkom, ms=msize, state=state,
                             owner=ow, phase=ph, note='; '.join(x for x in ([w for w in inf.why[:1]] + note) if x), low=low))

    # ---- stage list (kom.py stage-june placement rules)
    live, staged = D.live, D.staged
    stage = []
    refused = []
    for r in out_rows:
        if not (r['owner'] == owner and r['phase'] == 'CH0b' and r['state'] in ('missing', 'march-copy')):
            continue
        low, nm = r['low'], r['file']
        where = dict(live.get(low, {}))
        for stem, sp_ in staged.get(low, {}).items():
            where.setdefault(stem, sp_)
        if len(nm) > LONG_NAME:
            refused.append((nm, 'name is %d characters (limit %d)' % (len(nm), LONG_NAME)))
            continue
        if len(where) > 1:
            refused.append((nm, 'March already has it in %d koms: %s' % (len(where), ','.join(sorted(where)))))
            continue
        if where:
            (stem, spelling), = where.items()
            origin = 'March'
        else:
            stems = sorted({c.kom for c in june[low]}, key=lambda s: (not os.path.isdir(os.path.join(KOM.KOM_ROOT, s + '_kom')), s))
            stem, origin = stems[0], 'June'
        stage.append((nm, r['jk'], stem, r['state'], origin))
    stage.sort(key=lambda x: x[0].lower())

    # ---- write
    out_rows.sort(key=lambda r: (r['kind'] != 'lua', r['file'].lower()) if False else r['file'].lower())
    cols = ['file', 'kind', 'route', 'june_kom', 'june_size', 'march_kom', 'march_size', 'march_state', 'owner', 'phase', 'note']
    keys = ['file', 'kind', 'route', 'jk', 'js', 'mk', 'ms', 'state', 'owner', 'phase', 'note']
    cnt = collections.Counter
    lines = ['\t'.join(cols)]
    for r in out_rows:
        lines.append('\t'.join(str(r[k_]).replace('\t', ' ') for k_ in keys))
    lines.append('# total rows: %d' % len(out_rows))
    lines.append('# total kind: ' + '; '.join('%s=%d' % kv for kv in cnt(r['kind'] for r in out_rows).most_common()))
    lines.append('# total march_state: ' + '; '.join('%s=%d' % kv for kv in cnt(r['state'] for r in out_rows).most_common()))
    lines.append('# total route: ' + '; '.join('%s=%d' % kv for kv in cnt(r['route'] for r in out_rows).most_common()))
    lines.append('# total owner/phase: ' + '; '.join('%s/%s=%d' % (k_[0], k_[1], v) for k_, v in
                                                     cnt((r['owner'], r['phase']) for r in out_rows).most_common()))
    own = [r for r in out_rows if r['owner'] == owner and r['phase'] == 'CH0b']
    lines.append('# total owner/phase CH0b by march_state: ' + '; '.join('%s=%d' % kv for kv in cnt(r['state'] for r in own).most_common()))
    lines.append('# total owner/phase CH0b by kind: ' + '; '.join('%s=%d' % kv for kv in cnt(r['kind'] for r in own).most_common()))
    lines.append('# total CH0b must stage (missing+march-copy, owner %s/CH0b, not refused): %d (missing %d, march-copy %d); '
                 'june-bytes (already June, nothing to stage): %d; refused by stage-june rules: %d' % (
                     owner, len(stage), sum(1 for s in stage if s[3] == 'missing'), sum(1 for s in stage if s[3] == 'march-copy'),
                     sum(1 for r in own if r['state'] == 'june-bytes'), len(refused)))
    for nm, why in refused:
        lines.append('#   refused: %s: %s' % (nm, why))
    kmap = cnt((s[1], s[2]) for s in stage)
    live_stems = set(st for d_ in live_size.values() for st in d_)
    new_koms = sorted(set(s[2] for s in stage if s[2] not in live_stems and not os.path.isdir(os.path.join(KOM.KOM_ROOT, s[2] + '_kom'))))
    lines.append('# placement koms with no live archive and no kom_contents dir yet (stage-june creates them; the pack / copy step needs the user): %d: %s' % (
        len(new_koms), ','.join(new_koms)))
    lines.append('# stage kom map (June source kom -> March placement kom: files): ' + '; '.join(
        '%s->%s:%d' % (j, m, n) for (j, m), n in sorted(kmap.items())))
    lines.append('# totals by kind of the stage list: ' + '; '.join('%s=%d' % kv for kv in cnt(kind_of(s[0]) for s in stage).most_common()))
    lines.append('# source counts: route a (name) %d first-seen names; UnitTemplet rows %d; class script file literals %d; closure file rows %d; '
                 'kim items %d (unit_type %d / classes %s / UnitTemplet items %d); attached particle templets %d' % (
                     n_a, n_ut_rows, len(script_files), closure_n, len(item_ids), C['unit_type'], C['unit_classes'], len(ut_items), len(part_roots)))
    # shared particle / mesh-player definition files
    defs = []
    for ns, fl in F.NS_FILES.items():
        for f_ in fl:
            if f_.startswith('<'):
                fl2 = ['GameMajorParticle%d.txt' % i for i in range(2, 80)]
            else:
                fl2 = [f_]
            for f2 in fl2:
                if f2.lower() in june:
                    defs.append((f2, D.file_status(f2.lower())))
    lines.append('# shared definition .txt in March vs June: ' + '; '.join('%s=%s' % (f_, s[0]) for f_, s in defs))
    with open(os.path.join(a.out, 'ch0_assets.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')
    with open(os.path.join(a.out, 'ch0b_stage_list.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('name\tjune_kom\tplacement_kom\tstate\tchosen_by\n')
        for s in stage:
            f.write('\t'.join(s) + '\n')
    log('rows', len(out_rows), 'stage', len(stage), 'refused', len(refused))
    motion_question(C, a.out)
    for l in lines[-14:]:
        log(l[:400])


if __name__ == '__main__':
    main()
