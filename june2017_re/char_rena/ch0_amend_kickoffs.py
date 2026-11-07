"""char-rena CH0 item 11: fill each kickoff's "Amended by CH0" section from the CH0 tsvs, and write the kickoffs CH0 adds.
    python june2017_re/char_rena/ch0_amend_kickoffs.py [--dry]
Copy of char_raven/ch0_amend_kickoffs.py. Per phase: functions (ch0_class_map.tsv col 7) with inside / new, states (ch0_state_skill.tsv col 8),
skills (part 2 col 6), checklist rows (ch0_checklist_rows.tsv col 5), natural-force rows (ch0_nf.tsv col 8), callees CH0 routed to an Opus
helper phase (HELPER), plus the hand notes in NOTES. Replaces the placeholder paragraph only; a filled section is left alone.
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-rena')
PLACE = '(CH0 fills this: filters with real column numbers, addresses, counts, any split.)'
DATE = '2026-11-07'
CM = 'june2017_re/char_rena/ch0_class_map.tsv'


def tsv(name):
    p = os.path.join(HERE, name)
    if not os.path.exists(p):
        return []
    return [l.rstrip('\r\n').split('\t') for l in open(p, encoding='utf-8', errors='replace') if l.strip() and not l.startswith('#')]


cm = [r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn']
ss_all = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8', errors='replace').read().split('\n')
cut = next(i for i, l in enumerate(ss_all) if l.startswith('# part 2'))
states = [l.split('\t') for l in ss_all[:cut] if l and not l.startswith('#') and not l.startswith('state\t')]
skills = [l.split('\t') for l in ss_all[cut:] if l and not l.startswith('#') and not l.startswith('skill_id')]
ck = [r for r in tsv('ch0_checklist_rows.tsv') if r and r[0] != 'section']
nf = [r for r in tsv('ch0_nf.tsv') if r and not r[0].startswith('march_file')]
ui = {r[0]: r for r in tsv('ch0_unmapped_identified.tsv') if r and r[0] != 'callee'}

# CH0's needs-Opus callees: helper phase -> (the Sonnet phase it precedes, callees); filled from ch0_unmapped_identified.tsv in main()
HELPER = collections.OrderedDict()
OWN = {}
OPUS = {'CH1m', 'CH1n', 'CH1o', 'CH1p', 'CH1q', 'CH1q2', 'CH1', 'CH2d0', 'CH3c0', 'CH3d', 'CH4a', 'CH4b'}

SPLITS = {   # new kickoff: (copied from, scope title, depends)
    'CH1d2': ('CH1d', 'Class core: dash and jump combos, base jump / dash attacks (CH0 split of CH1d)', 'CH1d review'),
    'CH1f2': ('CH1f', 'Class core: Grand Archer line (`LGA` / `RGA`) (CH0 split of CH1f)', 'CH1f review'),
    'CH1g2': ('CH1g', 'Class core: Night Watcher combos, dash XZ / XZZ, start / win / lose (CH0 split of CH1g)', 'CH1g review'),
    'CH1g3': ('CH1g', 'Class core: Night Watcher accel dash and dash-jump states (CH0 split of CH1g)', 'CH1g2 review'),
    'CH1q2': ('CH1q', 'Natural force, engine side: gauge data / UI, gauge manager, emulator, June engine callers (CH0 split of CH1q)', 'CH1q review'),
}
TITLES = {'CH1d': 'Class core: Rena ground combos (`LESI_COMBO*`)', 'CH1f': 'Class core: Sniping Ranger line (`SNIPING`, `SIEGE`, `LSR` / `RSR`)',
          'CH1g': 'Class core: Trapping Ranger line (`RTR`)'}

NOTES = {
 'CH0b': ["Stage list, motion answer, named items, closure and icons: CH0 Done note, **Assets** paragraph (`ch0b_stage_list.tsv`, `ch0_motion_question.tsv`, `ch1_held_motions.tsv`)."],
 'CH1a': ["`LESI_WALK` Start `32c6df30` and `LESI_DASH` Start `32c6e120` are **CH1q's**: March's `WalkStart` / `DashStart` exist only under `ADD_RENA_SYSTEM` (`.cpp:5856` / `:6168`; flag-off `LESI_DASH` uses `CX2GUUser::DashStart`). Declare nothing; CH1m registers them. "
          "`LESI_DASH` is the per-arm row: Night Watcher (114) registers StartFuture `32cb4330>32c6e0c0` / FrameMoveFuture `32cb4350>32c6e0f0`, every other class the base `329cc580` / `329cae40` (`32ca9dc0 @32caa647`)."],
 'CH1b': ["The four `LESI_P_LE_AGILE_MOVEMENT_*` states (ids 242-245) are here: InitState registers them with March's `StandUpRolling*` callbacks (`.cpp:1816-1838`)."],
 'CH1d2': ["The 7 base jump / dash attacks (`LESI_DOUBLE_JUMP_ATTACK_Z` / `_LANDING`, `LESI_DASH_ATTACK_X`, `LESI_JUMP_ATTACK_Z` / `_X`, `LESI_DASH_JUMP_ATTACK_Z` / `_LANDING`) are here. "
           "`LESI_DASH_JUMP_COMBO_X3` Start `32c6b280` is CH1q's (March `DashJumpComboX3StateStart` only under `ADD_RENA_SYSTEM`)."],
 'CH1f': ["`LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX` Start / FrameMove / EventProcess (`32c6ac20` / `32c8b2c0` / `32c6e590`) are CH1q's (March: only under `ADD_RENA_SYSTEM`); its FrameMoveFuture `32c83c90` is June-only and stays here (`new`)."],
 'CH1f2': ["`LESI_LGA_ZZXX` Start `32c6c130` is CH1q's (March `LESI_LGA_ZZXX_StateStart` only under `ADD_RENA_SYSTEM`, `.cpp:16891`). 7ac: `LESI_RGA_GUNGNIR_READY` CanDown call `@32ca111a`."],
 'CH1g2': ["`LESI_RNW_DASH_XZZ_FULL_CHARGE_END` End `32ca0e60` spawns `NUI_RENA_TARGETING_GUNGNIR` (2083) through `CreateNPCReq` `@32ca11f2` (fDelay 0.3f): write the call with June's arguments (`ch0_traps.tsv` row 4); CH2d0 owns the spawn rule. "
           "A `LESI_RNW_WIN2` roll sets byte `+0x9714` = 1 (InitState `32cadc90 @~209`): CH1m's, noted for the win state."],
 'CH1h': ["DamageReact `32c6da50` (uncarved), Lose `32c834f0`, HyperModeBuffEffectStart `32c9b3d0` (uncarved; the transcended return is CH3d's, 7ao), CreateMajor / MinorParticleArray `32ca4e10` / `32ca5010` (uncarved; alpha sites `32c893e5` in `32c89310`, `32c89535` in `32c89460`) and InitComponent `32c895b0` (6 alpha sites, 7c; GT_PVP NF reset `@32c895f4` is CH1q's line under `_NF`). "
          "`FriendshipNatureData` virtuals `32cb3500` / `32cb4200` / `32cb4270` (vtable `3351666c`; its COL reads `TimeLimited`; March `X2GULire_ElvenRanger.h:265`, live). "
          "Init `32c883d0` (0x40), Set/UnSetSpecificValueByBuffTempletID (0x24c / 0x250), AdjustDamageDataBeforeDamageReact (0x300), AttackResultByType (0x4e0), CreateGageData (0xd0), Check / DeleteDamageRelateSkillData (0x420 / 0x424) are **CH1q's**: March declares them only under `ADD_RENA_SYSTEM` (`.h:536-567`)."],
 'CH1i': ["0x740 `32c852f0` (calls base `32009780`) stays here; 0x744 `32c89230` writes the NF gauge (`32a317d0`, `32a36df0`) and is CH1q's. ShownSkillCoolTime 0x6a8 `32c7bed0` (7ap)."],
 'CH1j': ["CommonStateStart `32ca81b0` reads `BOW_SKILL_STATE` (flag `+0x981d`, @32ca82c2), `CREATE_TRAP_BLADE` and `DAMAGEDATA_CHANGE_TIME*`; CommonStateEnd `32ca5060` reads `+0x981d` @32ca5078; GetActualCoolTime (CH4b) reads it @32c7be7b. "
          "CommonFrameMove `32ca8bf0` and CommonFrameMoveFuture `32c87990` hold NF lines (cannon-ball count sync, `UpdateNaturalForceEffect` inline): write them under `_NF`. "
          "June passes a skill-templet byte to `31fe28f0` in CommonStateStart (@32ca898c) where March derives an NF count from MP."],
 'CH1k': ["The ctor `32ca2b60` (vtable store `32ca2bc4`; caller 3230f289, the unit factory) and `32cb5ee0` (member init @32ca3c13). SpecialAttackEventProcess `32c6a970` (uncarved) with parts `32cac200` (7, l.262) / `32ca50b0` / `32c89a20`; 0x810 `32c87ac0`, 0x824 `32c83320` (7av, the Agile Movement animation rate), 0x834 `32c86af0`; plus `ReservedSkillStart` (Rules)."],
 'CH1m': ["Registrars: InitStateCommon `32ca9dc0` and InitState `32cadc90` (all line states inline; no line registrar functions; InitPassiveSkillState registers no state). "
          "`charport_states.py init 32ca9dc0 32cadc90 3202b110` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **202 / 202**, one per-arm row (`LESI_DASH`, other arm read by hand). START / WIN / LOSE rows 9 / 11 / 12 are std::string rolls; a `LESI_RNW_WIN2` roll sets byte `+0x9714`. "
          "InitState's tail: vslot 0x690 (InitPassiveSkillState), 0x684( 0 ), `31fd8f00`, `32c96d60`, then `32c85200( 1 )` (SetNaturalForceBuff) when the gauge count > 0 (CH1q's line). "
          "State ids: `ch0_state_enum_names.tsv` (218 registered, 89 shared names all with a June id different from the C++ value: register by June's id)."],
 'CH1n': ["`32cb06d0` has three switches (skill type: byte tbl `32cb3004` / jump tbl `32cb2ff4`; base-id chain with the 6-case hyper switch, jump tbl `32cb31e0`; skill-id switch: byte tbl `32cb30fc` / jump tbl `32cb3010`): `ch0_skill_listing_rows.tsv` 157 rows, 111 ids with events; 19 ids (106, 23400-23417; types 1 / 8) leave at the type switch. The default branch plays `LESI_SI_LE_POWER_ATTACK1` (CH2a)."],
 'CH1o': ["Five class functions called from base code (31fef532 / 31fedfec / 31fca1be / 3229a79c): `32c83580` (+ `32c6dbd0`), `32c6dd10`, `32c6ddc0`, `32c85b20`: read each (Raven's `32c3d1c0` precedent) and give it a home. Plus `SetSkillLevelStateData` (§7 l.286, March `.cpp:22990`)."],
 'CH1p': ["CH0 found no June-only key for this phase: `BOW_SKILL_STATE` is CH1j + CH4b, `IMMUNE_TO_SUPER_ARMOR_DOWN` is routed to backlog Stage 2 (per-state immunity lists, reader `31fdaaa0`). Scope: 7ag and the hooks 7h / 7l / 7o / 7p / 7x as the plan says."],
 'CH1q': ["**Map: `june2017_re/char_rena/ch0_nf.tsv`** (225 rows, col 8 owner: 99 CH1q, 27 CH1q2, 24 absent in June; the rest are 2014 combo-revamp state-flow arms owned by the state phases). "
          "Class functions (27, `awk -F'\\t' '$7==\"CH1q\"'` on the class map): every June row whose March function exists only under `ADD_RENA_SYSTEM` goes inside that function (col 4 names it), June's body under `_LIVE`; the one-file probe defines `_LIVE`, `_NF` and `ADD_RENA_SYSTEM`. "
          "June differs: NATURAL_FORCE_VALUE_PER_HIT 3.0 (@32ca73fd; March 6), no buff threshold of 3 (SetNaturalForceBuff `32c85200` stacks BFI 355 per point), ConsumeNaturalForce `32c9f7a0` also restores MP (n x float +0x97e0), UpNaturalForce `32cb6290` adds 1 more when a roll beats the protected float +0x6c50, AdjustDamageDataBeforeDamageReact is empty. "
          "GetNaturalForceCount / IsEmpty / UpNaturalForce are `__forceinline` in March but out of line in June (engine callers in CH1q2): drop the inline. "
          "Flipped arms: `X2DamageEffect.cpp:5217` would drop `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`'s child sequence (June `31f85910 @31f860f2-31f8614d` does the first-attack copy AND the children): keep both; the other arms (`X2DamageManager` `m_NaturalForceType` / `m_iNaturalForceType` DD+0x56c, `X2GUUser.h:1410`, `X2GUUser.cpp:10893`, `X2GUNPC.cpp:11404`) are equivalent; pick the flag-on arm. "
          "The studio \"NF:\" overlay (`X2GUUser.cpp:29719`) and fill key (`X2Game.cpp:5855`) are absent in June: keep them out of the build. `FriendshipNatureData` is not NF (CH1h)."],
 'CH1q2': ["Gauge data `CX2RenaGageData` (vtable `3349b244`, size 0x12c; ctor `32498a30`, copy ctor `32a37e00`, dtor `32498e00`; Update `32a34f20`, InitWhenGameIsOver `32a34f80`, GetMyPlayStatus `32a32500`, SetMyPlayStatus `32a34fa0`, Clone `32a36060`, Copy `32a34fe0`); "
           "gauge UI `CX2RenaMyGageUI` (vtables `3353363c` / `33533624`; ctor `32e64760`, dtor `32e725a0`, InitUI `32e6e190` (`DLG_Rena_NaturalForce.lua`), OnFrameMove `32e71970`, UpdateNaturalForceUI `32e5ec10`), created by `CX2GageManager::CreateMyGageUI` `3248e290` case `3248e531` for classes 3 / 12 / 13 / 24 / 102 / 103 / 114. "
           "Engine callers: `ApplyHyperModeBuff` `32298590 @322985ec` (count x 15.0 to vslot 0xf0, then UpNaturalForce( 5 ); checklist l.959), `CX2GUNPC::AttackResultByType` `32773ee0` NF branch `@3277452b-3277464b`, the F7 training key `322ed400` (Rena: UpNaturalForce( 1 ) `@322ed57b`). "
           "Packet: June keeps March's `KGamePlayStatus` (`m_cCharAbilType` = `CAC_NATURAL_FORCE` 5, `m_iCharAbilCount`): the emulator's Lire case (`X2OfflineStatTable.cpp:289-313`, today `CAC_NONE`) and whether the count survives a relog are decided here (no save column today)."],
 'CH2a': ["`LESI_SI_LE_POWER_ATTACK1` (the skill switch default) is here."],
 'CH2b': ["`LESI_A_LE_RISING_FALCON_LANDING` (id 227, registered by InitState) is here with Rising Falcon; its FrameMove `32cad390` reads `DAMAGE_DATA_LAST` (@32cad4d9)."],
 'CH2d0': ["**Map: `june2017_re/char_rena/ch0_traps.tsv`.** June's live spawns: CreateEvoke `32c99690` (`NUI_EVOKE_TRAP_LIRE` 1020 @32c99a1a, eTeam -1 / eAllyTeam = team: swapped against the others), CreateAngerOfElfTrap `32c9e250` (1420 @32c9e42e), Night Watcher's Gungnir (2083 @32ca11f2, written by CH1g2). "
            "June dropped `NUI_EVOKE_TRAP_LIRE_MEMO` (1025) and `NUI_RENA_ANGER_OF_ELF_DELAY` (1421), and its Entangle NPC spawn (`32c95790`, dead). "
            "Summon-info setters `32c85900` / `32c86340` / `32c863a0` / `32c87860` / `32c878f0` (called from CX2Game summon code 3230bbbd-3230c8b6) + `32cb5f60`; the Evoke trap scripts `Lire_Evoke_trap.lua` / `_Memo.lua` (l.291, `USE_OWNERS_ENCHANT`) through `luabind.py check`."],
 'CH3b': ["Entangle 23213 (`RNSI_A_RGA_ENTANGLE`, Grand Archer in June) builds `DAMAGE_EFFECT_LENA_TRAPPING_RANGER_ENTANGLE` / `_MEMO` (`32c9e7b0`): no NPC. 7u: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` -> June's `..._Guided_Arrow` spelling, `DamageEffect_LGA_Arc_Shot` dropped."],
 'CH3c': ["`LESI_SA_RNW_FURIOUS_ENGAGE` FrameMove `32caca80` reads `DAMAGE_DATA_THIRD` / `DAMAGE_DATA_LAST`. Depends on CH3c0 (the Night Watcher engine hook)."],
 'CH3d': ["7ao: HyperModeBuffEffectStart `32c9b3d0` (transcended return) and the three `HQ_Cutin_R{WS,GA,NW}_Transcendence` rows."],
 'CH4a': ["Delegate registrations are in InitPassiveSkillState `32ca78b0` (thunks pushed @32ca7d17 / @32ca7e83 / @32ca8111 / @32ca8169; checklist 7am names `32ca7860`, which is Win): A `32ca2180` (also NPC `3275a490`), `32ca2390`, `32ca28b0`, `32c86790` (+ `32cb5430`, `32cb57b0`, `32cb5970`, `32cb4290`), stubs `32c6dac0`, `32c7af80`, `32cb5dd0`, `32cb66e0`-`32cb6700`. "
          "**Passive ids:** `ch0_passive_ids.tsv` (72 live March `SI_*` reads; the June-id column is a title match, verify each): D8 `SI_P_LCR_SHARP_KICK` 3400 -> `RNSI_P_RCR_SHARP_KICK` 23400 and `SI_P_LSR_POWERFUL_BOWSTRING` 3405 -> `RNSI_P_RGA_POWERFUL_BOWSTRING` 23405 (4 live sites each)."],
 'CH4b': ["MemoConditionRena vtable `33516854` (TD `3373d730`): slot 1 `32c6d4d0` (decide). GetActualCoolTime 0x6a4 `32c7be00` reads the `BOW_SKILL_STATE` flag `+0x981d`. SetDamageExceptionProcess `32cad510`."],
 'CH5': ["`ch0_unittemplet_rena.tsv`: every class adds `m_fLobbyRotationY` (30 / 30 / 25 / 1 / 30 / 20 / 10 for 3 / 12 / 13 / 24 / 102 / 103 / 114); 102 / 103 / 114 add `m_HyperCutIn` `HQ_Cutin_RWS/RGA/RNW_Transcendence` and change `name` `STR_ID_616` -> `STR_ID_4783` / `_4784` / `_17429` (\"Wind Sneaker\", \"Grand Archer \" with a trailing space, \"Night Watcher\": all in `general.ess` and `StringID_def`)."],
}


def phase_block(p):
    out = []
    fn = [r for r in cm if len(r) > 6 and r[6] == p]
    if fn:
        c = collections.Counter(r[4] for r in fn)
        out.append('- **Functions: %d** (`awk -F\'\\t\' \'$7=="%s"\' %s`; %s).' % (len(fn), p, CM, ', '.join('%s %d' % kv for kv in sorted(c.items()))))
        out.append('  June addresses: ' + ' '.join('`%s`%s' % (r[0], '' if r[4] == 'inside' else '(%s %s)' % (r[4], r[5])) for r in fn) + '.')
    st = [r for r in states if len(r) > 7 and r[7] == p]
    if st:
        out.append('- **States: %d** (`awk -F\'\\t\' \'$8=="%s"\' june2017_re/char_rena/ch0_state_skill.tsv`): %s.' % (len(st), p, ', '.join('`%s`' % r[0] for r in st)))
    sk = [r for r in skills if len(r) > 5 and r[5] == p]
    if sk:
        out.append('- **Skills: %d** (part 2 of `ch0_state_skill.tsv`, col 6): %s.' % (len(sk), ', '.join('%s %s' % (r[0], r[1]) for r in sk)))
    rows = [r for r in ck if len(r) > 4 and re.search(r'(^|[\s,/+])%s($|[\s,/+(])' % re.escape(p), r[4])]
    if rows:
        out.append('- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): ' + '; '.join('%s %s %s' % (r[0], r[1], r[2][:70]) for r in rows) + '.')
    n = [r for r in nf if len(r) > 7 and r[7] == p]
    if n and p not in ('CH1q', 'CH1q2'):
        out.append('- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `%s`): %d; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June\'s NF lines go under `_NF`.' % (p, len(n)))
    for hp, (before, cs) in HELPER.items():
        if hp == p:
            out.append('- **Callees to map** (`ch0_unmapped_identified.tsv`): ' + '; '.join('`%s` %s (%s)' % (c, ui[c][2][:80], ui[c][3]) for c in cs) + '.')
        elif before == p:
            out.append('- **Depends on CH0\'s Opus helper phase %s** (%s).' % (hp, ', '.join('`%s`' % c for c in cs)))
    if p in OWN:
        out.append('- **Callees this Opus phase maps itself** (`ch0_unmapped_identified.tsv`): ' + '; '.join('`%s` %s' % (c, ui[c][2][:80]) for c in OWN[p]) + '.')
    for t in NOTES.get(p, []):
        out.append('- ' + t)
    return out


def helper_kickoff(hp, before, cs, title, task):
    return '\n'.join([
        '# June character Rena (Stage 7) - Phase %s - %s (Opus)' % (hp, title), '',
        '| | |', '|---|---|',
        '| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena %s` |' % hp,
        '| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |',
        '| Depends on | the review of the phase before %s; one at a time with the other `X2GULire_ElvenRanger.*` / `X2GUUser.*` phases |' % before,
        '| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |',
        '| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |',
        '| Review | `kickoffs/char-rena/%s-review.md` (Opus 5.5 high; the phase writes it when done) |' % hp, '',
        '## Read first (and nothing else from the plan doc unless you need it)', '', '```sh',
        'python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "Phase CH0 - Rena map (read-only)" "CH0 - Done (2026-11-07)"',
        "awk -F'\\t' '$7==\"%s\"' %s" % (hp, CM),
        "awk -F'\\t' '$1 ~ /^(%s)/' june2017_re/char_rena/ch0_unmapped_identified.tsv" % '|'.join(cs) if cs and all(c in ui for c in cs) else '',
        '```', '', '## Task', '', task, ''] +
        ['- **`%s`** %s; CH0 grade: %s; callers %s' % (c, ui[c][2], ui[c][3], ui[c][8] if len(ui[c]) > 8 else '') for c in cs if c in ui] + [
        '',
        'You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. '
        'Done gate: every address above has a funcmap row (`Char Rena %s`) and a March body (or a recorded reason it needs none); the build passes; `%s`\'s kickoff names the March symbols; '
        '`junediverge.py --check` and `enumscan.py --check` PASS.' % (hp, before), '',
        '## Amended by CH0', '', PLACE, ''])


CH3C0_TASK = ('CH0 found a Night Watcher engine hook no doc maps: base `CX2GUUser` `31fcd700` calls Rena\'s `32ca6cd0` only when the unit class is 114, from '
              'three engine sites (`32037afb` in user code, `322739d9` in the damage path, `3279bbd8` in NPC code). Identify what the hook does (CH0 guess: Night '
              'Watcher\'s delayed-firing marks; verify), give each piece a March home (the base hook in `CX2GUUser` under the owning shared flag, the class '
              'functions under `_CORE` with plain names), and port the whole chain: `32ca6cd0` and the class functions only it reaches (the class map rows with '
              'phase `CH3c0`). CH3c (Night Watcher skills) depends on it.')


def main():
    dry = '--dry' in sys.argv
    for c, r in ui.items():
        if len(r) > 6 and r[5] == 'y':
            ph = r[6].split()[0] if r[6] else '?'
            if ph in OPUS:      # an Opus phase maps its own callees (Raven's OWN)
                OWN.setdefault(ph, []).append(c)
            else:
                HELPER.setdefault(ph + '0', (ph, []))[1].append(c)
    for p, (base, scope, dep) in SPLITS.items():
        dst = os.path.join(K, p + '.md')
        if os.path.exists(dst):
            continue
        s = open(os.path.join(K, base + '.md'), encoding='utf-8').read()
        s = s.replace('Phase %s - ' % base, 'Phase %s - ' % p, 1).replace('char-rena %s`' % base, 'char-rena %s`' % p)
        s = s.replace('"%s"' % base, '"%s"' % p).replace('%s-review.md' % base, '%s-review.md' % p).replace("'$7==\"%s\"'" % base, "'$7==\"%s\"'" % p)
        s = s.replace('the class map gives phase `%s`' % base, 'the class map gives phase `%s`' % p)
        s = re.sub(r'^(# .*? - Phase %s - ).*$' % re.escape(p), r'\g<1>' + scope, s, count=1, flags=re.M)
        s = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>' + dep + '. One at a time with the other `X2GULire_ElvenRanger.*` phases', s, count=1)
        if not dry:
            open(dst, 'w', encoding='utf-8', newline='\n').write(s)
        print('new', p)
    for p, t in TITLES.items():
        path = os.path.join(K, p + '.md')
        s = open(path, encoding='utf-8').read()
        s2 = re.sub(r'^(# .*? - Phase %s - ).*$' % re.escape(p), r'\g<1>' + t, s, count=1, flags=re.M)
        if s2 != s and not dry:
            open(path, 'w', encoding='utf-8', newline='\n').write(s2)
    hk = [('CH3c0', 'CH3c', [], 'Night Watcher engine hook (`31fcd700` -> `32ca6cd0`)', CH3C0_TASK)]
    for hp, (before, cs) in HELPER.items():
        hk.append((hp, before, cs, 'Helpers: engine callees the Sonnet phase %s needs' % before,
                   'CH0 found callees outside Rena\'s class range that no doc maps and that `%s` would stop on ("this needs Opus"). Map each in Ghidra, '
                   'name it, give it a March home (an existing March function when June\'s namesake does the same; else a new one under the flag, by the owning '
                   'class\'s pattern, no `June` in the name), and port its whole body (June\'s numbers and quirks verbatim, a `JUNE_BUGS.md` row each). Check '
                   'first whether an earlier port already holds it (`funcmap.tsv`, `junediverge.py --list`).' % before))
    for hp, before, cs, title, task in hk:
        dst = os.path.join(K, hp + '.md')
        if not os.path.exists(dst):
            if not dry:
                open(dst, 'w', encoding='utf-8', newline='\n').write(helper_kickoff(hp, before, cs, title, task))
            print('new', hp)
    for f in sorted(os.listdir(K)):
        if not f.endswith('.md') or f.endswith('-review.md') or f in ('CH0.md',):
            continue
        p = f[:-3]
        path = os.path.join(K, f)
        s = open(path, encoding='utf-8').read()
        s2 = s.replace('   # column as Raven\'s; CH0\'s amendment corrects it', '   # column 7 = phase (CH0 checked)')
        if PLACE in s2:
            body = phase_block(p) or ['- Nothing in the CH0 maps carries this phase\'s id; the plan section is the scope.']
            s2 = s2.replace('## Amended by CH0\n\n' + PLACE, '## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(body))
        if s2 != s:
            if not dry:
                open(path, 'w', encoding='utf-8', newline='\n').write(s2)
            print('amended', p)


if __name__ == '__main__':
    main()
