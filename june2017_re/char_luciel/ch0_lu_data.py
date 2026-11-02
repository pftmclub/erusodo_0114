r"""char-luciel CH0: shared June-side data for the CH0 builders (skill ids, templets, tree rows, script tables).

Sources (read-only): june_src_fixed/data036_kom/Enum.src.lua (SKILL_ID, UNIT_CLASS), NewSkillTempletVer2.src.lua (AddSkillTemplet_LUA,
AddSkillTreeTemplet_LUA), Luciel_Duo.src.lua / Luciel_Demonio.src.lua (state tables).
"""
import os, re

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
J = os.path.join(T, 'june_src_fixed', 'data036_kom')
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
LO, HI = 0x32ae4080, 0x32b4e770          # class range (inclusive start, exclusive end)
CLASSES = {200: 'DUO', 37: 'CHILIARCH', 38: 'ROYAL_GUARD', 39: 'DIABLURA', 127: 'DREAD_LORD', 128: 'NOBLESSE', 129: 'DEMONIO'}
CLS_ORDER = [200, 37, 38, 39, 127, 128, 129]


def rd(p):
    return open(p, encoding='utf-8', errors='replace').read()


def skill_names():
    en = rd(os.path.join(J, 'Enum.src.lua'))
    i = en.index('SKILL_ID = {')
    blk = en[i:en.index('\n}', i)]
    return {int(m.group(2)): m.group(1) for m in re.finditer(r'^\s*(\w+)\s*=\s*(\d+)', blk, re.M)}


def templets():
    """id -> (type, delete_by_upgrade, wstrName)"""
    out = {}
    for blk in rd(os.path.join(J, 'NewSkillTempletVer2.src.lua')).split('AddSkillTemplet_LUA(')[1:]:
        me = re.search(r'^\tm_eID = (\d+),', blk, re.M)
        ty = re.search(r'^\tm_eType = (\d+),', blk, re.M)
        de = re.search(r'^\tm_DeleteSkillIDByUpgrade = (\d+),', blk, re.M)
        nm = re.search(r'^\tm_wstrName = "(.*)",', blk, re.M)
        if me:
            out[int(me.group(1))] = (int(ty.group(1)) if ty else None, int(de.group(1)) if de else 0, nm.group(1) if nm else '')
    return out


def tree_rows():
    """[(unit_class_value, skill_id, row, col, master_level)] for the 7 Lu/Ciel classes, in file order"""
    en = rd(os.path.join(J, 'Enum.src.lua'))
    cv = {m.group(1): int(m.group(2)) for m in re.finditer(r'^\s*(UC_LUCIEL_\w+)\s*=\s*(\d+)', en, re.M)}
    rows = []
    for blk in rd(os.path.join(J, 'NewSkillTempletVer2.src.lua')).split('AddSkillTreeTemplet_LUA(')[1:]:
        c = re.search(r'm_eUnitClass = UNIT_CLASS\.(\w+)', blk)
        if not c or c.group(1) not in cv or cv[c.group(1)] not in CLASSES:
            continue
        g = lambda k: int(re.search(r'm_%s = (\d+)' % k, blk).group(1))
        rows.append((cv[c.group(1)], g('iSkillID'), g('iRowIndex'), g('iColumnIndex'), g('iMasterLevel')))
    return rows


def script_tables():
    """[(name, script, line)] for every top-level table of both June scripts"""
    out = []
    for f in ('Luciel_Duo', 'Luciel_Demonio'):
        src = rd(os.path.join(J, f + '.src.lua'))
        for m in re.finditer(r'^([A-Za-z_]\w*)\s*=\s*\{', src, re.M):
            out.append((m.group(1), f, src[:m.start()].count('\n') + 1))
    return out


def lu_skill_ids(tm):
    return sorted(i for i in tm if 10000 <= i <= 10999 or 30000 <= i <= 30999)
