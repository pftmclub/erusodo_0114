r"""char-elsword CH0: write ch0_hand_names.tsv (fn, proposed_name, why) for every class-map row still `new?`.

    NO_HAND_NAMES=1 python june2017_re/char_elsword/ch0_build_class_map.py ; python june2017_re/char_elsword/ch0_make_hand_names.py ; python june2017_re/char_elsword/ch0_build_class_map.py

A name is proposed only where Chung's method settles it: the destructor body (inside) and the MemoConditionElsword / secondary vtable entries (RTTI names
read from the exe). Every other row stays `?` with a hint (callers, vslot, base function) for the transcribing phase to name by the class pattern.
"""
import os, re
HERE = os.path.dirname(os.path.abspath(__file__))
rows = {}
for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8').read().splitlines()[1:]:
    p = l.split('\t')
    if re.fullmatch(r'[0-9a-f]{8}', p[0]):
        rows[p[0]] = p
SPECIAL = {
    '32cf2e20': ('GetFinalDamageToBeChangedByActiveSkill', 'inside: X2GUElsword_SwordMan.h:323 GetFinalDamageToBeChangedByActiveSkill (vslot 0xbc; body multiplies by +0x96e4 m_fActiveSkillAddPowerRate)'),
    '32d058a0': ('StoreGageSnapshot', 'user vslot 0x740 (Chung X2GUChung.h:693 name): saves GetWayOfSwordPoint at +0x92b8'),
    '32d058d0': ('RestoreGageSnapshot', 'user vslot 0x744 (Chung X2GUChung.h:694 name): SetWayOfSwordPoint from +0x92b8, then base 3204da60'),
    '32d05960': ('IsInJobLine', 'user vslot 0x670 (Chung X2GUChung.h:660 name): line 1 = classes 10/100, 2 = 11/101, 3 = 22/112'),
    '32d08d70': ('AfterImageFrameMove', 'user vslot 0x850 (X2GUUser.h:3421): toggles the two after-images by now state 13 or 0xce'),
    '32d27130': ('GetSpecialItemUsableExtraStateID', 'user vslot 0x4ac (X2GUUser.h:3347): returns 0xce = ESSI_SWORD_WAIT'),
    '32cf55a0': ('AppendMajorParticleToDeviceList', 'inside: X2GUElsword_SwordMan.h:344 AppendMajorParticleToDeviceList (ch0_unmapped_identified CH1o triage)'),
    '32cf56a0': ('AppendMinorParticleToDeviceList', 'inside: X2GUElsword_SwordMan.h:370 AppendMinorParticleToDeviceList (ch0_unmapped_identified CH1o triage)'),
    '32cf5660': ('InitializeElswordMinorParticleArray', 'inside: X2GUElsword_SwordMan.h:367 InitializeElswordMinorParticleArray (ch0_unmapped_identified CH1o triage)'),
    '32d08ba0': ('LightingStepFrameMove', 'inside: X2GUElsword_SwordMan.h:688 LightingStepFrameMove (ch0_unmapped_identified CH1o triage)'),
    '32d1b810': ('CreateGageData', 'inside: X2GUElsword_SwordMan.h:1645 CreateGageData (Way of the Sword, ch0_wos.tsv)'),
    '32cf50f0': ('GetWayOfSwordState', 'inside: X2GUElsword_SwordMan.h:416 GetWayOfSwordState (Way of the Sword, ch0_wos.tsv)'),
    '32cf5100': ('SetWayOfSwordState', 'inside: X2GUElsword_SwordMan.h:417 SetWayOfSwordState (Way of the Sword, ch0_wos.tsv)'),
    '32cf5130': ('GetWayOfSwordPoint', 'inside: X2GUElsword_SwordMan.h:419 GetWayOfSwordPoint (Way of the Sword, ch0_wos.tsv)'),
    '32cf5140': ('SetWayOfSwordPoint', 'inside: X2GUElsword_SwordMan.h:420 SetWayOfSwordPoint (Way of the Sword, ch0_wos.tsv)'),
    '32d0c960': ('Start_SwordmanVigor', 'inside: X2GUElsword_SwordMan.h:423 Start_SwordmanVigor (Way of the Sword, ch0_wos.tsv)'),
    '32d0cd60': ('End_SwordmanVigor', 'inside: X2GUElsword_SwordMan.h:424 End_SwordmanVigor (Way of the Sword, ch0_wos.tsv)'),
    '32d0cf70': ('Start_SwordmanDestruction', 'inside: X2GUElsword_SwordMan.h:425 Start_SwordmanDestruction (Way of the Sword, ch0_wos.tsv)'),
    '32d0d240': ('End_SwordmanDestruction', 'inside: X2GUElsword_SwordMan.h:426 End_SwordmanDestruction (Way of the Sword, ch0_wos.tsv)'),
    '32d18cd0': ('GetFinalDamage_WayOfSword', 'inside: X2GUElsword_SwordMan.h:437 GetFinalDamage_WayOfSword (Way of the Sword, ch0_wos.tsv)'),
    '32d07810': ('PlayAnimationSwordWaitStart', 'inside: X2GUElsword_SwordMan.h:1582 PlayAnimationSwordWaitStart (Way of the Sword, ch0_wos.tsv)'),
    '32d0eeb0': ('WayOfSwordStateChange', 'inside: X2GUElsword_SwordMan.h:1584 WayOfSwordStateChange (Way of the Sword, ch0_wos.tsv)'),
    '32d0f300': ('DoStateDestructionRage', 'inside: X2GUElsword_SwordMan.h:1587 DoStateDestructionRage (Way of the Sword, ch0_wos.tsv)'),
    '32d09490': ('CX2GUElsword_SwordMan', 'inside: X2GUElsword_SwordMan.h constructor (vtable store 32d094e4, body 32d09490-32d0b4c5); caller 3230f189 is outside the class'),
    '32d1a5d0': ('~CX2GUElsword_SwordMan', 'inside: X2GUElsword_SwordMan.h:278 destructor body (ctor store 32d094e4 / dtor store 32d1a5fd; the scalar deleting dtor 32d27620 calls it, then the delete helper 32fe2300)'),
    '32cf4d00': ('?', 'MemoConditionElsword vtable 33522870 slot 0 (RTTI .?AVMemoConditionElsword@CX2GUElsword_SwordMan@@, March nested class); name by the memo-condition pattern (CH1h / CH4a decision)'),
    '32d270f0': ('?', 'entry of the one-slot vtable at 33522908 (RTTI CX2GUElsword_SwordMan, secondary base subobject); thunk-like, read it'),
    '32d27100': ('?', 'entry of the one-slot vtable at 33522900 (RTTI CX2GUElsword_SwordMan, secondary base subobject); thunk-like, read it'),
}
out = ['fn\tproposed_name\twhy']
for fn, p in sorted(rows.items()):
    if p[4] != 'new?':
        continue
    if fn in SPECIAL:
        out.append('%s\t%s\t%s' % ((fn,) + SPECIAL[fn]))
        continue
    if p[1].startswith('override'):
        out.append('%s\t?\tvirtual %s (base %s); not named by any sibling class map, funcmap or X2GUUser.h; read the body' % (fn, p[1].split()[1], p[7].replace('base ', '')))
        continue
    callers = re.findall(r'[0-9a-f]{8}', p[2])
    names = []
    for c in callers[:4]:
        r = rows.get(c)
        names.append('%s(%s)' % (c, (r[2].split(' / ')[0][:28] if r and r[1].startswith(('override', 'state-fn', 'registrar')) else r[1] if r else '?')))
    out.append('%s\t?\t%s%s' % (fn, p[2] if not callers else 'called by ' + ' '.join(names), ('; ' + p[7]) if p[7] and 'phase unsettled' not in p[7] else ''))
open(os.path.join(HERE, 'ch0_hand_names.tsv'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print(len(out) - 1, 'rows')
