r"""char-chung CH0: build ch0_vtable_all.tsv (all 544 slots of CX2GUChung 3350967c) and ch0_vtable_slots.tsv (the in-range overrides).

    python june2017_re/char_chung/ch0_build_vtable.py

Chung vtable 3350967c (544 slots 0x0-0x87c; the dwords after it are RTTI string bytes); base = CX2GUUser vtable 334781f4 (544 slots).
Overrides = entries in the Chung class range 32bd86f0-32c25a1f that differ from the base entry. Names: Eve / Aisha / Arme class maps
(same virtual), funcmap rows, X2GUChung.h / X2GUUser.h declarations; `?` when none settles it (a hint says what the body does).
"""
import os, re, struct

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
EXE = r'F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\unpacked_x2.exe'
B = 0x31aa0c00
LO, HI = 0x32bd86f0, 0x32c25a1f
VT, BASE = 0x3350967c, 0x334781f4
d = open(EXE, 'rb').read()


def vt(a, n=544):
    return [struct.unpack_from('<I', d, a - B + 4 * i)[0] for i in range(n)]


c, u = vt(VT), vt(BASE)
eve = {}
for l in open(os.path.join(HERE, '..', 'char_eve', 'ch0_vtable_all.tsv')).read().splitlines()[1:]:
    p = l.split('\t')
    eve[int(p[0], 16)] = p[1]
eve_name = {}
for l in open(os.path.join(HERE, '..', 'char_eve', 'ch0_vtable_slots.tsv'), encoding='utf-8').read().splitlines():
    p = l.split('\t')
    if l and l[0] != '#' and p[0] != 'slot_off':
        eve_name[int(p[0], 16)] = (p[4], p[2])

NAMES = {
    0x0: ('~CX2GUChung (scalar deleting dtor)', 'CONF: Eve/Aisha class map 0x0; X2GUChung.h:492'),
    0x4: ('OnFrameMove', 'CONF: Aisha class map 0x4; X2GUChung.h:773'),
    0x28: ('NotifyShowObjectChanged', 'HINT: base 31ff1950 then the SetShowViewList slot (0x63c) with the unit show flag (+0x172); X2GUChung.h:752 sits before NotifyDGManagerChanged:753'),
    0x30: ('NotifyDGManagerChanged', 'CONF: Aisha class map 0x30; X2GUChung.h:753'),
    0x3c: ('InitComponent', 'CONF: Eve/Aisha class map 0x3c; funcmap 32bf4900 (data ref 335096b8 = this slot)'),
    0x40: ('Init', 'CONF: funcmap 3204ea20 CX2GUUser::Init (June) is the base; X2GUChung.h:495'),
    0x44: ('ReInit', 'CONF: Eve/Aisha class map 0x44'),
    0x48: ('InitEffect', 'CONF: Eve/Aisha class map 0x48'),
    0x4c: ('DeleteMinorParticle', 'CONF: Eve/Aisha class map 0x4c'),
    0x60: ('DamageReact', 'CONF: Eve/Aisha class map 0x60'),
    0x90: ('Win', 'CONF: Eve/Aisha class map 0x90'),
    0x94: ('Lose', 'CONF: Eve/Aisha class map 0x94'),
    0x98: ('Draw', 'CONF: Eve/Aisha class map 0x98'),
    0x9c: ('IsFullHyperMode', 'CONF: Eve class map 0x9c; funcmap slot 39; X2GUChung.h:761'),
    0xa0: ('SetFullHyperMode', 'CONF: Eve class map 0xa0; X2GUChung.h:765'),
    0xa4: ('IsBerserkByChance', 'CONF: funcmap Effect C3t row (31e4c550 / 32c24b90): unit vslot 0xa4 berserk-by-chance, none in March; only CX2GUChung overrides it'),
    0xb8: ('?', 'HINT: class 0x75 (CTT) gate, float member at +0xc04 gage, then a call with (0,1): not identified; base 3208dc20'),
    0xc0: ('ApplyHyperModeBuff', 'CONF: funcmap 32298590 CX2GameUnit::ApplyHyperModeBuff (vslot 48); X2GUChung.h:777'),
    0xc4: ('HyperModeBuffEffectStart', 'CONF: Eve class map 0xc4; X2GUChung.h:778'),
    0xc8: ('HyperModeBuffEffectEnd', 'CONF: Eve class map 0xc8; X2GUChung.h:779'),
    0xd0: ('?', 'HINT: bool-returning; calls the own-unit slot 0x198 first, then a SEH-protected create (alloc 0x144): not identified; base 322984c0'),
    0x1b8: ('?', 'HINT: float getter; passive 0x672f (RNSI_P_CIP_OPEN_MIND) scales a protected float by the skill level value; base 3208e290'),
    0x1d8: ('?', 'HINT: float(float) hook: protected float + member +0x96bc when +0x9718 == 1; base 3208e5a0'),
    0x24c: ('SetSpecificValueByBuffTempletID', 'CONF: Eve class map 0x24c; X2GUChung.h:781'),
    0x250: ('UnSetSpecificValueByBuffTempletID', 'CONF: Eve class map 0x250; X2GUChung.h:782'),
    0x2b8: ('SetDamageExceptionProcess', 'CONF: Eve class map 0x2b8; funcmap 32bf28f0'),
    0x2e8: ('?', 'HINT: float(DamageData*) hook, returns 1.0 unless passive 0x6734 and attacker flag; base 3208dd70 returns 1.0'),
    0x2ec: ('?', 'HINT: float(DamageData*) hook, passive 0x65b8 and damage type 0x21 / 0x22; base 3208dd70 returns 1.0'),
    0x314: ('ApplyRenderParamOnEmptyRenderParamByBuff', 'CONF: Eve class map 0x314; funcmap 32bec310 (vslot 197); X2GUChung.h:783'),
    0x324: ('ChangeMaxCharacterAddResource', 'CONF: funcmap 32bf4880 (slot 201); X2GUChung.h:763'),
    0x3a8: ('ApplyRenderParam', 'CONF: Eve class map 0x3a8; X2GUChung.h:1773'),
    0x4b0: ('GetComboZStateID', 'CONF: Eve class map 0x4b0; X2GUChung.h:796'),
    0x4b4: ('GetComboXStateID', 'CONF: Eve class map 0x4b4 (CH1o); X2GUChung.h:797'),
    0x540: ('CreateMajorParticleArray', 'CONF: Eve/Aisha class map 0x540'),
    0x548: ('CreateMinorParticleArray', 'CONF: Eve/Aisha class map 0x548'),
    0x54c: ('InitEnchantWeaponEffectUnitSpecific', 'CONF: Eve/Aisha class map 0x54c'),
    0x5f0: ('COMMON_BUFF_FrameMove', 'CONF: Eve/Aisha class map 0x5f0'),
    0x600: ('HyperModeFrameMove', 'CONF: Eve/Aisha class map 0x600'),
    0x628: ('CanRidingState', 'CONF: Aisha class map 0x628; X2GUChung.h:481'),
    0x634: ('?', 'HINT: if not IsFullHyperMode: SetShowViewList(1) + 3201c3a0, then the UpdateEquipCollisionData base 31fd8c00 and three bone calls (Rfoot, Lfoot, Lhand2); base 31fc7a60'),
    0x63c: ('SetShowViewList', 'CONF: Aisha class map 0x63c; X2GUChung.h:755'),
    0x644: ('UpdateEquipViewListInfo', 'CONF: Aisha class map 0x644; X2GUChung.h:758'),
    0x66c: ('ScaleHead', 'CONF: funcmap 31fdc070 CX2GUUser::ScaleHead vslot 0x66c (head bone Bip01_Head / Bip01_Spine1 by the berserk flag)'),
    0x670: ('?', 'HINT: bool(int 1..3): class 0x1b / 0x75 (1), 0x15 / 0x6f (2): a per-class capability test; base 331a4950'),
    0x684: ('InitEquippedActiveSkillState', 'CONF: Eve/Aisha class map 0x684'),
    0x688: ('?', 'HINT: base 31fcd740, then gage +0x142 = 1 and a call with the gage value (+0x11c): follows InitEquippedActiveSkillState; base 31fcd740'),
    0x690: ('InitPassiveSkillState', 'CONF: funcmap 32c16be0; X2GUChung.h:504'),
    0x6a0: ('GetActualMPConsume', 'CONF: Eve class map 0x6a0; X2GUChung.h:523'),
    0x6a4: ('GetActualCoolTime', 'CONF: Eve class map 0x6a4; X2GUChung.h:1780'),
    0x6a8: ('?', 'HINT: float(float, int skillId): returns arg1 unless skill 0x65b7 with member +0x98d4 live; base 3208e190 (same shape as Eve 0x6a8)'),
    0x740: ('?', 'CONF: funcmap 32009780 CX2GUUser slot 0x740 Crest of Time snapshot (none in March); Chung adds its gauges'),
    0x744: ('?', 'CONF: funcmap 3204da60 CX2GUUser slot 0x744 Crest of Time restore (none in March); Chung restores its gauges'),
    0x748: ('?', 'HINT: float getter, passive 0x672f value 0xaa when 32bee5b0 true; base 3208e1b0'),
    0x774: ('?', 'HINT: base 31f3f070 is the empty CX2State stub; Chung body builds a 0x44 byte record with floats: not identified'),
    0x7a4: ('InitState', 'CONF: Eve/Aisha class map 0x7a4; X2GUChung.h:497'),
    0x7b4: ('SetEquippedSkillFuncBySkillSlotIndex', 'CONF: Eve/Aisha class map 0x7b4; X2GUChung.h:505'),
    0x7b8: ('SetEquippedSkillLevel', 'CONF: Eve/Aisha class map 0x7b8; X2GUChung.h:506'),
    0x7c0: ('CommonStateStartFuture', 'CONF: Eve/Aisha class map 0x7c0'),
    0x7c4: ('CommonStateStart', 'CONF: Eve/Aisha class map 0x7c4'),
    0x7c8: ('CommonFrameMoveFuture', 'CONF: Eve/Aisha class map 0x7c8'),
    0x7cc: ('CommonFrameMove', 'CONF: Eve/Aisha class map 0x7cc'),
    0x7d8: ('CommonStateEndFuture', 'CONF: Eve/Aisha class map 0x7d8'),
    0x7dc: ('CommonStateEnd', 'CONF: Eve/Aisha class map 0x7dc'),
    0x7e8: ('CommonRender_Prepare', 'CONF: Eve/Aisha class map 0x7e8'),
    0x7f4: ('SpecialAttackEventProcess', 'CONF: funcmap 32bd8790 (slot 509, vtable 33509e74 = Chung); X2GUChung.h:520'),
    0x7f8: ('SpecialAttackEventProcess part: skill lookup (slot 510)', 'CONF: funcmap 32bd8790 note: calls slot 510 32c17830 first'),
    0x7fc: ('SpecialAttackEventProcess part: skill use (slot 511)', 'CONF: Eve/Aisha class map 0x7fc'),
    0x800: ('SpecialAttackEventProcess part: per-class skill use / cooldown (slot 512)', 'CONF: Eve/Aisha class map 0x800'),
    0x80c: ('NoStateChangeActionFrameMove', 'CONF: Eve/Aisha class map 0x80c; X2GUChung.h:521'),
    0x810: ('SpecialAttackNoStageChange', 'CONF: Eve/Aisha class map 0x810; X2GUChung.h:749'),
    0x824: ('?', 'HINT: bool-ish; shares the stream 32bed840 (no int3 padding between them); damage-type test on 0x21 / 0x22 and passive 0x65b8; base 3208dd70'),
    0x828: ('?', 'HINT: base 31fd7640 true, then hit-count test (/100 == 0xa2) on +0x7cbc..: not identified (Aisha calls it an idle-timer reset)'),
    0x834: ('ShowActiveSkillCutInAndLightByScript', 'CONF: Eve/Aisha class map 0x834; X2GUChung.h:589'),
}
# a settled name taken from a sibling class map when the table above has a gap
for slot, (n, f) in eve_name.items():
    pass

hdr = {}
for fn in ('X2GUChung.h', 'X2GUUser.h', 'X2GameUnit.h'):
    path = os.path.join(T, 'X2Lib', fn)
    for i, l in enumerate(open(path, 'rb').read().decode('latin-1').split('\n'), 1):
        m = re.search(r'\bvirtual\b[^;{(]*?[~\w]+\s*\(', l)
        if m:
            nm = re.search(r'([~\w]+)\s*\(', l[l.index('virtual'):])
            if nm:
                hdr.setdefault((fn, nm.group(1)), (i, re.sub(r'\s+', ' ', l.strip().split('//')[0]).strip()))


def march_of(name):
    key = name.split(' (')[0]
    if name == 'HyperModeFrameMove':
        for i, l in enumerate(open(os.path.join(T, 'X2Lib', 'X2GUChung.h'), 'rb').read().decode('latin-1').split('\n'), 1):
            if re.match(r'\s*void HyperModeFrameMove\s*\(', l):
                return 'X2GUChung.h:%d %s (no virtual keyword; overrides the pure virtual X2GUUser.h:2335)' % (i, l.strip())
    for fn in ('X2GUChung.h', 'X2GUUser.h', 'X2GameUnit.h'):
        if (fn, key) in hdr:
            i, txt = hdr[(fn, key)]
            if fn == 'X2GUChung.h':
                return '%s:%d %s' % (fn, i, txt)
            return '- (CX2GUChung does not override it in March; %s:%d %s)' % (fn, i, txt)
    if name.startswith('SpecialAttackEventProcess part'):
        return '- (part of SpecialAttackEventProcess, X2GUChung.h:520)'
    if name == 'HyperModeFrameMove':
        for i, l in enumerate(open(os.path.join(T, 'X2Lib', 'X2GUChung.h'), 'rb').read().decode('latin-1').split('\n'), 1):
            if re.search(r'\bHyperModeFrameMove\s*\(', l):
                return 'X2GUChung.h:%d %s (no virtual keyword)' % (i, re.sub(r'\s+', ' ', l.strip().split('//')[0]))
    if name == 'ScaleHead':
        return '- (not in March Chung; CX2GUUser has no ScaleHead in March: June-only virtual)'
    if name == '?':
        return '- (not found; name unsettled)'
    return '- (not in X2GUChung.h / X2GUUser.h / X2GameUnit.h virtuals)'


allrows = ['slot_off\tchung_target\tin_range\teve_target']
rows = ['slot_off\tvt_entry\tfn\tbase_fn\tname\tsrc\teve_fn\teve_name\tmarch']
n_ov = 0
for i in range(544):
    s = i * 4
    inr = LO <= c[i] <= HI
    allrows.append('0x%x\t%08x\t%d\t%s' % (s, c[i], 1 if inr else 0, eve.get(s, '-')))
    if inr and c[i] != u[i]:
        n_ov += 1
        nm, src = NAMES.get(s, ('?', 'HINT: not settled'))
        en = eve_name.get(s, ('-', '-'))
        rows.append('0x%x\t%08x\t%08x\t%08x\t%s\t%s\t%s\t%s\t%s' % (s, VT + s, c[i], u[i], nm, src, en[1], en[0], march_of(nm)))
rows.append('# Chung vtable CX2GUChung %08x: 544 slots (0x0-0x87c); overrides (target in %08x-%08x and != base): %d' % (VT, LO, HI, n_ov))
rows.append('# base_fn = CX2GUUser vtable %08x; eve_fn / eve_name = the Eve class map entry of the same slot (char_eve/ch0_vtable_all.tsv)' % BASE)
open(os.path.join(HERE, 'ch0_vtable_all.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(allrows) + '\n')
open(os.path.join(HERE, 'ch0_vtable_slots.tsv'), 'w', newline='\n', encoding='utf-8').write('\n'.join(rows) + '\n')
print(n_ov, 'overrides;', sum(1 for r in rows[1:-2] if '\t?\t' in r), 'unnamed')
