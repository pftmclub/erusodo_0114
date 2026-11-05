r"""char-raven CH0: write ch0_funcmap_new_rows.tsv and append it to june2017_re/funcmap.tsv (rows 'Char Raven CH0'); fixes the 32c43120 row key.
    python june2017_re/char_raven/ch0_funcmap_rows.py          (run once; refuses if the rows are already in funcmap)
"""
import os
HERE = os.path.dirname(os.path.abspath(__file__))
RE = os.path.normpath(os.path.join(HERE, '..'))
T = 'Char Raven CH0: '
new = [
    ('32c5d660', 'CX2GURaven::InitStateCommon', 'confirmed', 'called from InitState 32c66fe0 before base 3202b110; charport_states.py init + ch0c_listing_check 178 / 178 (char_raven/ch0_reg.tsv)'),
    ('32c511c0', 'CX2GURaven Sword Taker / Blade Master state registrar (none in March as a function)', 'confirmed', 'InitState 32c66fe0 class switch, classes 16 / 106; rows in char_raven/ch0_reg.tsv; CH1m'),
    ('32c51b70', 'CX2GURaven Over Taker / Reckless Fist state registrar (none in March as a function)', 'confirmed', 'InitState class switch, classes 17 / 107; CH1m'),
    ('32c63670', 'CX2GURaven Weapon Taker / Veteran Commander state registrar (none in March as a function)', 'confirmed', 'InitState class switch, classes 25 / 115; CH1m'),
    ('32c43380', 'CX2GURaven::CommonStateStart (vslot 0x7c4, uncarved)', 'confirmed', 'holds the BURNING_NASOD_HAND_POSSIBLE read @32c43911 and the +0x4108 = 2 write @32c43ac4 (the C8a row now keyed 32c43380 @32c43888-32c43ac4; 32c43120 is a separate dead helper); CH1j'),
    ('32c3e030', 'CX2GURaven::CanParryingState', 'candidate', 'March X2GURaven.cpp:13827; base callers 31fd29f6 / DamageReact 32047385; CH1q (char_raven/ch0_rage.tsv)'),
    ('32c41170', 'CX2GURaven::AddRageGuage', 'candidate', 'March X2GURaven.cpp:14440; base caller DamageReact 3204a674; June multiplies by (+0x6c50 + 1.0) and reads 24419 (0x5f63); CH1q'),
    ('32c491a0', 'CX2GURaven::DoParrying', 'candidate', 'March X2GURaven.cpp:13857; called by RSI_PARRYING Start 32c4a9f0; Specialization B reader @32c49410 (7an); CH1q'),
    ('32c4f750', 'CX2GURaven::ClearNasodBall', 'candidate', 'March X2GURaven.cpp:13662; CH1q'),
    ('32c4f7e0', 'CX2GURaven::CreateNasodBall', 'candidate', 'March X2GURaven.cpp:13686; RAVEN_NASOD_BALL damage effect, June adds EffectSet_BEED_UP; CH1q'),
    ('32c4fd80', 'CX2GURaven::RestoreNasodBall', 'candidate', 'March X2GURaven.cpp:13909; called by RSI_PARRYING End 32c50dd0; CH1q'),
    ('32c4ff90', 'CX2GURaven::ChangePowerRateNasodBall', 'candidate', 'March X2GURaven.cpp:14090; CH1q'),
    ('32c50060', 'CX2GURaven::AttackNasodBall', 'candidate', 'March X2GURaven.cpp:14132; CH1q'),
    ('32c50330', 'CX2GURaven::DoFrameMoveNasodBall', 'candidate', 'March X2GURaven.cpp:14210; caller CommonFrameMove 32c55240; ball level synced in class int +0x7ec0 (no m_CannonBallCount); Specialization B readers @32c507da / @32c50b15 / @32c50c3d; CH1q'),
    ('32c56be0', 'CX2GURaven::AttackResultByType (vslot 0x4e0)', 'candidate', 'March X2GURaven.cpp:13979 (rage fill from hits); CH1h'),
    ('32c54960', 'none in March: CX2GURaven Flying Impact arc helper', 'candidate', 'called by AttackResultByType 32c56be0; CH1h'),
    ('32c4e7c0', 'CX2GURaven::HyperModeBuffEffectStart (vslot 0xc4)', 'candidate', 'March X2GURaven.cpp:21541 (rage +40, hyper aura); the transcended return (7ao) is CH3d; CH1h'),
    ('32c50fb0', 'CX2GURaven::HyperModeBuffEffectEnd (vslot 0xc8)', 'candidate', 'March X2GURaven.cpp:21570; CH1h'),
    ('32c69f60', 'CX2GURaven::IsFullHyperMode (vslot 0x9c, uncarved)', 'confirmed', 'returns byte +0x97c8 = March m_bHyperAura (inline X2GURaven.h:483); CH1h'),
    ('32c68ff0', 'CX2GURaven::SetFullHyperMode (vslot 0xa0, uncarved)', 'confirmed', 'writes byte +0x97c8 (X2GURaven.h:484); CH1h'),
    ('32c3c7f0', 'none in March: CX2GURaven vslot 0x73c (1.0 + protected float +0x95b8, MP rate on damage react)', 'candidate', 'uncarved; only engine caller DamageReact; CH4a (7an)'),
    ('32c3c820', 'none in March: CX2GURaven vslot 0x748 GetIncreaseSoulRateOnDamageReact (1.0 + protected float +0x95c4)', 'candidate', 'uncarved; base hook ported by Chung CH4b; CH4a (7an)'),
    ('32c4dfe0', 'CX2GURaven::CreateOverHeatObject', 'candidate', 'March X2GURaven.cpp:21337; 13 callers (March _SetOverheat inlined); CH1g2 (char_raven/ch0_overheat.tsv)'),
    ('32c4e3c0', 'CX2GURaven::DeleteOverHeatObject', 'candidate', 'sole caller CommonFrameMove 32c55240; CH1g2'),
    ('32c4add0', 'CX2GURaven::_ShowOverheatIcon', 'candidate', 'Overlap_EDT_Emoticon_*_OverHeat / WeaponTaker_Overheat particles; callers RWT / RVC skill states; CH1g2'),
    ('32e6d890', 'CX2RavenMyGageUI::CX2RavenMyGageUI', 'candidate', 'RTTI vtable 3353348c; no direct caller found (the X2GageManager.cpp:167-176 creation site is unlocated); CH1q'),
    ('32e72790', 'CX2RavenMyGageUI::~CX2RavenMyGageUI', 'candidate', 'vtable 3353348c; CH1q'),
    ('32e6d6a0', 'CX2RavenMyGageUI::InitWakeOrb', 'candidate', 'March X2GageUI.cpp:3078-3141; CH1q'),
    ('32e718a0', 'CX2RavenMyGageUI::OnFrameMove', 'candidate', 'March X2GageUI.cpp; CH1q'),
    ('32e6d900', 'CX2RavenMyGageUI::UpdateHyperOrb', 'candidate', 'the only reader of raven_powrUp_UI_01 (7aq; March X2GageUI.cpp:3130-3175 at 100, 120); CH1q'),
]
FIX = {
    '31e9cb70': ('CX2DamageManager::DamageData::DamageData( const DamageData& ) (copy ctor)', 'confirmed',
                 'default member init, then 31e9c190 (body to 31e9cb6d, holds the DamageData copy row 31e9cad7); a local copy in 32c5fd90 (CH1g); C++: a by-value copy'),
    '31e9bb70': ('CX2DamageManager::DamageData::~DamageData', 'confirmed', 'destroys the +0x450 vector and the +0x608 per-unit list (row 31e9cad7); pairs with 31e9cb70'),
    '31fd9f40': ('none in March: CX2GUUser left-hand bone scale setter (+0x8e14..+0x8e1c times base +0x8e08..+0x8e10, Bip01_L_Hand)', 'candidate',
                 'decompile: stores the triple, multiplies by the base, GetCloneFrame( Bip01_L_Hand ) on the now skin anim (32071d80); callers Megadrill Break 32c38710 / 32c5ce40; Opus phase CH2c0'),
}
rows = [(a, b, c, T + d) for a, b, c, d in new]
for l in open(os.path.join(HERE, 'ch0_unmapped_identified.tsv'), encoding='utf-8').read().splitlines()[1:]:
    c = l.split('\t')
    sym, conf, ev = FIX.get(c[0], (c[2], c[3], c[4]))
    rows.append((c[0], sym, conf, T + ev))
open(os.path.join(HERE, 'ch0_funcmap_new_rows.tsv'), 'w', encoding='utf-8', newline='\n').write(
    'june_va\tmarch_symbol\tstatus\tevidence\n' + ''.join('\t'.join(r) + '\n' for r in rows))
p = os.path.join(RE, 'funcmap.tsv')
fm = open(p, encoding='utf-8', newline='').read()
if T in fm:
    raise SystemExit('funcmap already has Char Raven CH0 rows')
nl = '\r\n' if '\r\n' in fm[:4000] else '\n'
old = '32c43120 @32c43888-32c43ac4\t'
assert fm.count(old) == 1
fm = fm.replace(old, '32c43380 @32c43888-32c43ac4\t', 1)
if not fm.endswith(nl):
    fm += nl
fm += ''.join('\t'.join(r) + nl for r in rows)
open(p, 'w', encoding='utf-8', newline='').write(fm)
print('rows', len(rows))
