"""june-census V2: split the unowned studio functions of coverage.tsv into groups and decide each one.

Groups: a named class (coverage.tsv `group`), a March object file (the function lies between two June functions anchored
to the same March .obj), or an address gap between two anchors of different .obj files. Anchors come from funcmap symbols
and class names resolved through March's X2/US_SERVICE/X2.map (symbol -> .obj). Session.obj / luajit anchors are skipped
(shared serializer and binding instantiations sit everywhere). Decisions come from the SYS / M / STR tables below.

    python -I june2017_re/census/v2_exclusions.py      -> june2017_re/census/exclusions.tsv (+ totals on stdout)
"""
import collections, csv, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.normpath(os.path.join(HERE, '..', '..'))
MAP = os.path.join(TRUNK, 'X2', 'US_SERVICE', 'X2.map')
COV = os.path.join(HERE, 'coverage.tsv')
FSTR = os.path.join(TRUNK, 'june2017_re', 'func_strings.tsv')
OUT = os.path.join(HERE, 'exclusions.tsv')

SYS = {  # system -> (decision, stage, reason)
    'char-shared': ('port', 'S7', 'shared CX2GUUser / CX2GameUnit code the class ports touch'),
    'char-march': ('port', 'S7', 'March character class code with no funcmap row yet (open ports cover it in CH0; closed ones re-sweep)'),
    'char-new': ('port', 'S8', 'new-character class code (Lu/Ciel, Rose, Ain)'),
    'gauge': ('port', 'S7', 'per-class gauge data (CX2*GageData), ported with its class'),
    'stat': ('port', 'S33', 'stat and damage calculation'),
    'buff': ('port', 'S0', 'buff templet / behaviour / finalizer / displayer code'),
    'effect': ('port', 'S0', 'effect set / damage effect code'),
    'skill': ('port', 'S3', 'skill tree, skill note, combo-chain code'),
    'world': ('port', 'S22', 'dungeon / world / NPC / battle-field game code'),
    'pet': ('port', 'S17', 'pet and riding-pet code'),
    'item': ('port', 'S16', 'item templet, inventory data, socket / enchant, drop code'),
    'ui': ('port', 'S9', 'UI windows, HUD gauges, menus, chat and talk boxes, cash-shop browsing'),
    'quest': ('port', 'S4', 'quest manager and quest UI'),
    'party': ('port', 'S28', 'party manager'),
    'scene': ('port', 'S27', 'event scenes, slide shots, GFX event scenes'),
    'result': ('port', 'S29', 'dungeon result screen'),
    'font': ('port', 'S25', 'fonts, word wrap, string table'),
    'engine': ('port', 'S2', 'KTDX / KTDG engine, device, particle, UI framework, app core'),
    'customize': ('port', 'S12', 'character customize'),
    'achieve': ('port', 'S31', 'achievements, titles and emblems'),
    'resonance': ('port', 'S30', 'resonance'),
    'friendship': ('port', 'S18', 'NPC friendship / field NPC'),
    'field': ('port', 'S38', 'village / field game core (CX2TFieldGame, CX2SquareUnit, field state); no stage owns it'),
    'event': ('port', 'S39', 'date-window game events (custom events, level-up event, random mission); no stage owns them'),
    'triage': ('port', 'S40', 'not attributable by address: identify the classes, then route to the owning stage or exclude'),
    'raid': ('port', 'S40', 'boss raid / world mission (user, 2026-11-09: keep; Stage 40 identifies them, then S22 or S39)'),
    'social': ('exclude', '-', 'wedding / mentor (tutor list) need a second real player (user, 2026-11-09)'),
    'pvp': ('exclude', '-', 'PvP only; the offline client has no PvP'),
    'net': ('exclude', '-', 'network layer, packet serializers, Lua binding glue: the emulator replaces the server; glue arrives with its feature'),
    'telemetry': ('exclude', '-', 'packet log / mail-out telemetry'),
    'anticheat': ('exclude', '-', 'anti-cheat / anti-tamper'),
    'devtool': ('exclude', '-', 'in-house dev console; a command is ported only when a play-test needs it'),
    'publisher': ('exclude', '-', 'publisher locale / server group / tournament service'),
}
M = [  # (regex on class or .obj name, system); first match wins
    (r'^(CX2)?GUUser$|^X2GUUser$|^(C)?X2GameUnit$|^X2UnitSlashTraceManager$|^KDYNAMIC_UNIT_USER_SYNC$', 'char-shared'),
    (r'GULuciel|GUAin|GURose', 'char-new'),
    (r'GUAdd|GUAra|GUEL$|GUArme|GURaven|GUChung|GuEve|GUEve|GUElsword|GULire|EveMyGageUI', 'char-march'),
    (r'GageData$', 'gauge'),
    (r'^(C)?X2(Unit|Stat|ItemStatCalculator|DamageManager|UnitManager)$', 'stat'),
    (r'Buff', 'buff'),
    (r'SkillTree|SkillNote|Combo', 'skill'),
    (r'PetManager|^(C)?X2Pet$|^x2pet$|PetAI|PetInfo|RidingPet', 'pet'),
    (r'^(C)?X2UIInventory$', 'ui'),
    (r'DamageEffect|EffectSet', 'effect'),
    (r'Item|Inventory$|Eqip|SubEquip|subequip|CubeOpen|^lottery$', 'item'),
    (r'Quest', 'quest'),
    (r'PartyManager', 'party'),
    (r'EventScene|SlideShot', 'scene'),
    (r'DungeonResult', 'result'),
    (r'Font|WordWrap|WordLine|StringTable', 'font'),
    (r'CharCustom|Customize', 'customize'),
    (r'Title|Emblem|Achievement', 'achieve'),
    (r'Resonance', 'resonance'),
    (r'TFieldNpc', 'friendship'),
    (r'TFieldGame|SquareUnit|StateField$|^x2tfieldgame$|^x2squareunit$', 'field'),
    (r'CustomEvent|LVUpEvent|RandomMission', 'event'),
    (r'Wedding', 'social'),
    (r'WorldMission|BossRaid', 'raid'),
    (r'Informer', 'ui'),
    (r'PVP|PvP', 'pvp'),
    (r'Session|TRUser|IOThread|IOCP|Iocp|^iocp$|SocketObject|Performer|ActorProxy|ServerProtocol|^(C)?X2Packet$|KLuaManager|KTDNUDP|netcommon|threadmanager|KSk', 'net'),
    (r'PacketLog|SMTP', 'telemetry'),
    (r'KNPGame|KProtect', 'anticheat'),
    (r'GameEdit', 'devtool'),
    (r'^(C)?X2(GUNPC|NPCAI|AllyNPCAI|GUNPCAI|SimplePathFinder|Game|DungeonGame|Dungeon|DungeonStage|DungeonSubStage|DungeonManager|World|WorldManager'
     r'|FieldMapBase|Camera|Room|LocationManager|BattleFieldManager|BattleFieldGame|DefaultBoneInfo)$|WorldObject|StateDungeonGame|StateBattleField'
     r'|StateTrainingGame|TrainingGameUI', 'world'),
    (r'^(C)?X2(UI|State|Gage|Chat|TalkBox|Notice|GameOption|PlayGuide|Cursor|StatusGage|SlotManager|KeyPad|WorldMapUI|LocalMapUI|MiniMapUI|PartyUI'
     r'|CashShop|NPCUnitViewerUI|BaseUnitViewerUI|UnitViewerUI)|^x2(uiunit|npcunitviewerui)$|^CLIK', 'ui'),
    (r'^(C)?KTD|^(C)?X2(Main|Data|InstanceData|MemoryHolder|SoundCloseManager|EXPTable)$|^CParticle', 'engine'),
]
STR = [  # decrypted-string hints (func_strings.tsv) for gaps; one distinct hint decides, several -> triage
    (r'Achievement|ResonanceManager', 'achieve'), (r'NPC_FRIEND', 'friendship'), (r'CustomEvent', 'event'),
    (r'TournamentSystem|LocaleInfo|SERVER_GROUP', 'publisher'), (r'g_pEventScene|Event_Scene|SlideShot', 'scene'),
    (r'CharCustom', 'customize'), (r'PVPResultTable', 'pvp'), (r'g_pTitleSystem', 'achieve'), (r'MEMORY BOMB', 'anticheat'),
    (r'g_pPetManager|PET_ID', 'pet'), (r'HYPER_GAGE|DP_RATE_BY', 'gauge'), (r'Book_Table|Dungeon_Loading', 'ui'),
    (r'g_pkExpTable|UIMajorParticle', 'engine'), (r'SECRET_SKILL', 'char-shared'), (r'HenirData', 'world'), (r'WorldTourInfoData', 'scene'), (r'TUTOR_REQUEST', 'social'),
]
OVR = {'gap:X2UIInventory..X2CashShop@0x321c6800': ('net', 'v2_sample: 10 / 10 packet serializers')}


def undecorate(s):
    if not s.startswith('?'):
        return None
    if s.startswith('??0') or s.startswith('??1'):
        body = s[3:].split('@@')[0].split('@')
        return '::'.join(reversed(body)) + '::' + ('~' if s[2] == '1' else '') + body[0]
    if s.startswith('??'):
        return None
    body = s[1:].split('@@')[0].split('@')
    return '::'.join(reversed(body[1:])) + ('::' if len(body) > 1 else '') + body[0]


def load_map():
    sym2obj, cls2obj = {}, collections.defaultdict(collections.Counter)
    for line in open(MAP, encoding='latin-1'):
        p = line.split()
        if len(p) < 4 or not p[0].startswith('0001:'):
            continue
        try:
            int(p[2], 16)
        except ValueError:
            continue
        if 'i' in p[3:-1]:
            continue  # inline / COMDAT copy: placed in whichever .obj used it first
        n = undecorate(p[1])
        if n:
            sym2obj.setdefault(n, p[-1])
            if '::' in n:
                cls2obj[n.rsplit('::', 1)[0]][p[-1]] += 1
    return sym2obj, {c: cnt.most_common(1)[0][0] for c, cnt in cls2obj.items()}


def sysof(name):
    for rx, s in M:
        if re.search(rx, name):
            return s
    return None


def main():
    sym2obj, c2o = load_map()
    rows = [r for r in csv.DictReader((l for l in open(COV, encoding='utf-8') if not l.startswith('#')), delimiter='\t')]
    for r in rows:
        r['va'] = int(r['june_va'], 16)
        r['sz'] = int(r['size'] or 0)
    rows.sort(key=lambda r: r['va'])
    fs = {}
    for l in open(FSTR, encoding='latin-1'):
        p = l.rstrip('\n').split('\t')
        if len(p) >= 3:
            fs[int(p[0], 16)] = p[2]
    anchors = []
    for i, r in enumerate(rows):
        o = None
        s = r['fm_symbol'].strip()
        if s:
            o = sym2obj.get(s.split('(')[0].strip())
            if not o:
                for m in re.finditer(r'([A-Za-z_][\w:]*)::~?\w+', s):
                    if m.group(1) in c2o:
                        o = c2o[m.group(1)]
                        break
        if not o and r['basis'] != 'block' and r['group'] in c2o:
            o = c2o[r['group']]
        if o and not (o.startswith('X2ServerProtocol:Session') or o.startswith('luajitLib:')):
            anchors.append((i, o))
    short = lambda o: o.split(':')[-1].replace('.obj', '')
    G = collections.OrderedDict()

    def add(key, kind, r):
        g = G.setdefault(key, dict(kind=kind, n=0, b=0, first=r['june_va'], last=r['june_va'], strs=[]))
        g['n'] += 1
        g['b'] += r['sz']
        g['last'] = r['june_va']
        if r['va'] in fs:
            g['strs'].append(fs[r['va']])
    seen = set()
    for (i1, o1), (i2, o2) in zip(anchors, anchors[1:]):
        for k in range(i1, i2):
            r = rows[k]
            if r['bucket'] != 'studio' or r['owner']:
                continue
            seen.add(k)
            if r['group'] and r['group'] != '(unattributed studio)':
                add('class:' + r['group'], 'class', r)
            elif k == i1 or o1 == o2:
                add('file:' + short(o1), 'file', r)
            else:
                add('gap:%s..%s@%s' % (short(o1), short(o2), rows[i1 + 1]['june_va']), 'gap', r)
    for k, r in enumerate(rows):
        if k not in seen and r['bucket'] == 'studio' and not r['owner']:
            add('edge:outside the first / last anchor', 'gap', r)
    out = []
    for key, g in G.items():
        strs = ' '.join(g['strs'])
        if g['kind'] == 'gap' and key.startswith('gap:'):
            a, b = key[4:].split('@')[0].split('..')
            sa, sb = sysof(a), sysof(b)
            hints = sorted({s for rx, s in STR if re.search(rx, strs)})
            if key in OVR:
                s, how = OVR[key]
            elif len(hints) == 1:
                s, how = hints[0], 'strings'
            elif hints:
                s, how = 'triage', 'strings name several systems: ' + ' / '.join(hints)
            elif sa and sa == sb:
                s, how = sa, 'both neighbours'
            elif sa and sb and SYS[sa][:2] == SYS[sb][:2]:
                s, how = sa, 'both neighbours, same stage (%s | %s)' % (sa, sb)
            elif sa and sb and SYS[sa][0] == SYS[sb][0] == 'exclude':
                s, how = sa, 'both neighbours excluded (%s | %s)' % (sa, sb)
            else:
                s, how = 'triage', 'neighbours differ (%s | %s)' % (sa, sb)
        elif g['kind'] == 'gap':
            s, how = 'triage', 'no anchor on one side'
        else:
            s, how = sysof(key.split(':', 1)[1]) or 'triage', 'name'
        d, st, why = SYS[s]
        hint = '; '.join(x[:60] for x in g['strs'][:3]).replace('\t', ' ')
        out.append((key, g['n'], d, why, st, g['b'], g['first'], g['last'], '%s: %s' % (s, how), hint))
    out.sort(key=lambda x: (x[4], -x[1]))
    with open(OUT, 'w', encoding='utf-8', newline='\n') as f:
        f.write('# june-census V2 (v2_exclusions.py): every unowned studio function of coverage.tsv in one group with a decision\n')
        f.write('group\tfunctions\tdecision\treason\tstage\tbytes\tfirst_va\tlast_va\tbasis\tstrings\n')
        for x in out:
            f.write('\t'.join(str(v) for v in x) + '\n')
    tot = collections.Counter()
    for x in out:
        tot[(x[2], x[4])] += x[1]
    n = sum(tot.values())
    want = sum(1 for r in rows if r['bucket'] == 'studio' and not r['owner'])
    for k, v in sorted(tot.items(), key=lambda kv: -kv[1]):
        print('%-8s %-4s %6d' % (k[0], k[1], v))
    same = sum(1 for (i1, o1), (i2, o2) in zip(anchors, anchors[1:]) if o1 == o2)
    print('groups %d, functions %d, unowned studio in coverage.tsv %d, anchors %d (neighbouring pairs in one .obj: %d of %d)'
          % (len(out), n, want, len(anchors), same, len(anchors) - 1))
    print('RESULT: %s' % ('PASS' if n == want else 'FAIL'))
    return 0 if n == want else 1


if __name__ == '__main__':
    sys.exit(main())
