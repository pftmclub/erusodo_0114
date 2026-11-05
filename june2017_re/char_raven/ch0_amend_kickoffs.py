"""char-raven CH0 item 11: fill each kickoff's "Amended by CH0" section from the CH0 tsvs, and write the kickoffs CH0 adds.
    python june2017_re/char_raven/ch0_amend_kickoffs.py [--dry]
Per phase: functions (ch0_class_map.tsv col 7) with inside / new, states (ch0_state_skill.tsv col 8), skills (part 2 col 6),
checklist rows (ch0_checklist_rows.tsv col 5), callees CH0 routed to an Opus helper phase (HELPER), plus the hand notes in NOTES.
Replaces the placeholder paragraph only; a kickoff whose section is already filled is left alone.
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-raven')
PLACE = '(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)'
DATE = '2026-11-05'


def tsv(name):
    return [l.rstrip('\r\n').split('\t') for l in open(os.path.join(HERE, name), encoding='utf-8', errors='replace')
            if l.strip() and not l.startswith('#')]


cm = [r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn']
ss_all = open(os.path.join(HERE, 'ch0_state_skill.tsv'), encoding='utf-8', errors='replace').read().split('\n')
cut = next(i for i, l in enumerate(ss_all) if l.startswith('# part 2'))
states = [l.split('\t') for l in ss_all[:cut] if l and not l.startswith('#') and not l.startswith('state\t')]
skills = [l.split('\t') for l in ss_all[cut:] if l and not l.startswith('#') and not l.startswith('skill_id')]
ck = [r for r in tsv('ch0_checklist_rows.tsv') if r[0] != 'section']
ui = {r[0]: r for r in tsv('ch0_unmapped_identified.tsv') if r[0] != 'callee'}

# CH0's needs-Opus callees: helper phase -> (the Sonnet phase it precedes, callees)
HELPER = collections.OrderedDict([
 ('CH2c0', ('CH2c', ['31fd9f40'])),
])  # FILLED_AFTER_UNMAPPED

OWN = {}

NOTES = {
 'CH0b': [
  "**Stage list** `june2017_re/char_raven/ch0b_stage_list.tsv`: 4,825 rows (texture 3,267, mesh 1,249, sound 224, motion 63, other 22), no Lua. "
  "CH0 dry run: `STAGE-JUNE DRY RUN: 35 kom(s), 4825 staged (4490 new, 335 replaced, 0 identical), 0 refused`. **One new kom: data202** (June's number, as Eve's data203 / Chung's data204): the 53 June-only emotion motions + `Motion_Standard_Raven.X`.",
  "**Held for CH1 final:** `Motion_Raven_OT.x` only (`ch1_held_motions.tsv`, data033). March's live script and class code still play `OT_ComboX3Down` / `X4Down` / `X5Down` and `OT_SP1a_ChargedBolt`, which June's OT file renames (`_New`, `Raven_OT_SP1a_ChargedBolt`). "
  "The other 9 differing class motions (data033) drop no name and go now (`ch0_motion_question.tsv`). `Motion_Raven.x` is already June's (R12b).",
  "**Lua is not staged** (June `0x82` bytecode fails the pack gate): adopt June-only `DLG_1ST_Class_Change_Guide_Raven.lua` and `DLG_2ND_Class_Change_Guide_Raven.lua` with `luasrc.py adopt-june` (bytecode, `--dry-run` first; `luabind.py check` each). "
  "`DLG_Transcendental_Change_Guide_Raven.lua` and `DLG_Class_Change_Guide_Raven.lua` are already June's bytes. `POPUP_TR_B_MASTER` / `_R_FIST` / `_V_COMMANDER` (.tga + .TET, data215) are in the list.",
  "**Skill icons:** `skillicons.py check 106 900 24000-24058 24200-24239 24400-24422 24600-24605` = 30 misses in `DLG_SKILL_RavenActive_04` (22), `_05` (4, sheet missing: 24057 / 24058), `DLG_Raven_SKILL_ScreenShot_03` (2), `DLG_SKILL_Passive_12` (2: 24417 / 24421); all four sheets + .tet are in the list: re-run after staging. "
  "Effect closure: 68 missing names = 49 sounds (in the list) + 19 June itself does not define (`ch0_effect_closure.tsv`). Save: no unit of 4 / 16 / 17 / 25 / 107 / 115 exists; create them.",
 ],
 'CH1a': ["The three `RSI_SUPER_DASH*` tables are dead data in June (no registration): nothing for `SUPER` here. `RSI_COMMON_BUFF` is here (skill type 12)."],
 'CH1b': ["Parrying moved to **CH1q** with the Nasod core (its states call DoParrying / RestoreNasodBall). Die front / back Start `32c55ee0` / `32c55fe0` call `ClearNasodBall` `32c4f750` (CH1q: declare it, `body in CH1q`) and `32c52480` (clears the handle list +0x9638, here; also called by the dtor and CommonFrameMove). "
          "`RSI_TRANSFORMED` and `RSI_FROZEN` are dead data."],
 'CH1c': ["Only 2 class functions: the riding / pepper / dance states use `CX2GUUser` callbacks. `RSI_RIDING_JUMP_READY` is dead data; `RSI_RIDING_DAMAGE_BACK` is registered with no script table."],
 'CH1d': ["7h: `32c2b360` is `RSI_COMBO_ZZXX` EventProcess (`UPDOWN_CANCEL_AFTER`, +0x8ca4)."],
 'CH1f': ["7h: `RRF_COMBO_ZZZX` holds the second `UPDOWN_CANCEL_AFTER`. Over Taker: March's `RSI_OVER_TAKER_COMBO_XXdownX*` are March-only; June's `XXfrontX` family plays the `_New` names (`Motion_Raven_OT.x` arrives with CH1 final)."],
 'CH1g': ["`32c5fd90` (Weapon Taker X-X-X loop helper) copies a DamageData by value: `31e9cb70` / `31e9bb70` are `DamageData`'s copy ctor / dtor (funcmap): write a plain local copy. "
          "The WT combo states call `CreateOverHeatObject` `32c4dfe0` (CH1g2: declare it, `body in CH1g2`)."],
 'CH1g2': ["**Scope changed by CH0:** the 9 `RVC_DASH*` combo tables (30 functions) + the overheat infrastructure `CreateOverHeatObject` `32c4dfe0`, `DeleteOverHeatObject` `32c4e3c0`, `_ShowOverheatIcon` `32c4add0` (`ch0_overheat.tsv`; March `_SetOverheat` is inlined into Create's 13 callers). "
           "The `RVC_SI_*` tables are skill states and follow their skills: Bleeding Slicer (24208 / 24232, Sword Taker) **CH2b**; Ignition Crow Napalm / Incineration and Deadly Raid (24022-24024, 24053-24055) **CH3c2**."],
 'CH1h': ["0x9c / 0xa0 (`32c69f60` / `32c68ff0`, uncarved) are March's inline `m_bHyperAura` pair (`X2GURaven.h:483-484`, byte +0x97c8): stay here. 0x73c / 0x748 are CH4a. "
          "HyperModeBuffEffectStart / End `32c4e7c0` / `32c50fb0` and AttackResultByType `32c56be0` (+ June-only arc helper `32c54960`) hold rage lines: call CH1q's functions (declared). The transcended return in `32c4e7c0` (7ao) is CH3d's to check. "
          "The dtor `32c54c10` calls ClearNasodBall. June ReInit `32c409b0` has no overheat cleanup (March's is dropped)."],
 'CH1j': ["CommonStateStart 0x7c4 `32c43380` (uncarved) holds the `BURNING_NASOD_HAND_POSSIBLE` read @32c43911 and the +0x4108 = 2 write @32c43ac4: write it whole. The write only bites with June's 46 `AT_COMMAND` tags and 24401 learned, so it pairs with CH4a's setter (checklist 7b). "
          "CommonFrameMove `32c55240` calls `DoFrameMoveNasodBall` (CH1q) and `DeleteOverHeatObject` (CH1g2): declare them. March's class-side hyper ball block (`:5321-5363`) and the `SA_OVERHEAT_HP` re-read (`:5446`) are absent in June."],
 'CH1k': ["The ctor `32c3ece0`. SpecialAttackEventProcess `32c25af0` (uncarved) with parts `32c58c30` / `32c25ba0` / `32c43bf0` (`32c43bf0` keeps March's rage += MP / 3 in hyper, `:5831-5838`). Skill gate: no skill while parrying except Shadow Step (`:5653-5658`). Plus `ReservedSkillStart` (Rules)."],
 'CH1m': ["Registrars: InitStateCommon **`32c5d660`** (called from InitState `32c66fe0` before base `3202b110`), the class switch's three line registrars `32c511c0` (16 / 106), `32c51b70` (17 / 107), `32c63670` (25 / 115), and InitPassiveSkillState `32c57660` (6 Shadow Step states; the function is CH4a's). "
          "`charport_states.py init 32c5d660 32c66fe0 3202b110 32c511c0 32c51b70 32c63670 32c57660` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **178 / 178**, no hand rows (START / WIN / LOSE rows 9 / 11 / 12 and the hyper row read through the call reader). "
          "State ids: one C++ enum `RAVEN_STATE_ID` holds every prefix; 206 live June tables have no C++ member (raw id + comment, as Eve) and June's registered ids differ from the C++ values for all 109 shared names: register by June's id (`ch0_state_enum_names.tsv`). 26 March-only states dropped."],
 'CH1n': ["`32c641a0` has three switches (skill type 0xd-0x10, the 6-case hyper switch 0xc / 0x6019-0x601d, the skill-id switch 0x5dc1-0x5eaf): `ch0_skill_listing_rows.tsv` (listing emulator) = 181 rows for 126 ids; 23 ids (106, 24400-24415, 24417-24422, types 1 / 8) leave at the type switch."],
 'CH1o': ["9 triage functions: 8 unsettled callback targets (3 reached only through the `MemoConditionRaven` vtable `33511404`) and `32c3d1c0` (called from base `31fef53b`; AppendEnchantParticle). CanParryingState / AddRageGuage moved to CH1q."],
 'CH1p': ["No June-only key is owned here: the 7 `BBT_*` / `BFT_*` keys are engine buff sub-table keys, the camera pair is Stage 9's, `IMMUNITY_EDT_LIST_AT_THIS_STATE` backlog Stage 2's (`ch0_keys.tsv`). The 6 march-only-site keys drop no March input."],
 'CH1q': ["**Map: `june2017_re/char_raven/ch0_rage.tsv`** (79 rows, col 7 owner: 59 CH1q; 55 paired incl. 6 June-only, 7 absent in June, 17 `?` to locate). No split: 15 class functions + the gauge UI + the engine sites.",
          "Class: CanParryingState `32c3e030`, AddRageGuage `32c41170` (x (+0x6c50 + 1.0), reads 24419), DoParrying `32c491a0`, Clear / Create / Restore NasodBall `32c4f750` / `32c4f7e0` / `32c4fd80` (+ `32c4ff70`, `32c33b80`), ChangePowerRate `32c4ff90`, Attack `32c50060`, DoFrameMove `32c50330`, the 4 parrying state functions. "
          "Specialization B readers (7an) sit inside DoParrying @32c49410 and DoFrameMove @32c507da / @32c50b15 / @32c50c3d: roll on +0x6d04, skip the rage cost, `clock()` stamp +0x9778.",
          "Engine: DamageReact hooks (`X2GUUser.cpp:10229-10270`, `:10757-10790`, rage fill `:12000-12011`, parry state change `:13499-13530` = June `31feabd0`, SetHold `:37186-37200` = `31fd29c0`), `DTT_RAVEN_RAGE_ATTACK` / `UPGRADE_RAVEN` blocks of `X2DamageManager.cpp` / `X2DamageEffect.cpp`. "
          "June syncs the ball level in the class int +0x7ec0 (no `m_CannonBallCount`). `CX2RavenMyGageUI` vtable `3353348c` (ctor `32e6d890`, dtor `32e72790`, InitWakeOrb `32e6d6a0`, OnFrameMove `32e718a0`, UpdateHyperOrb `32e6d900`, 7aq); its creation site (`X2GageManager.cpp:167-176`) is unlocated."],
 'CH1': ["Copy the held `Motion_Raven_OT.x` with the script (`kom.py stage-june june2017_re/char_raven/ch1_held_motions.tsv`, then pack and ask for data033). Adopt `RAVEN_FIGHTER.lua`. "
         "Its key typos `ANUM_SPEED` and `STOP_TIME_ATK` (8 tables) are in March's script too (no reader either side): `JUNE_BUGS.md` rows on adoption."],
 'CH2a': ["The 5 `RSI_SHADOW_*` states June registers in InitPassiveSkillState are here (their registration lines are CH4a's function). §7 L256: none of March's three power-rate assigners survives; June's `RSI_A_RWT_FLAME_SWORD_FLY` Start `32c5d3f0` does `+0x7cc8 *= 1.2` (skill 24201)."],
 'CH2b': ["Bleeding Slicer (24208 / 24232 `RNSI_A_RST_BLEEDING_SLICER`, tables `RVC_SI_A_BLEEDING_SLICER*`) moved here from CH1g2 by CH0."],
 'CH2c': ["Megadrill Break (24203) calls June-only `31fd9f40` (left-hand bone scale, CH2c0) and reads `IGNORE_DEFENCE_RATE` (engine reader ported). `RSI_SA_ROT_CHARGED_BOLT_BLOOD` is dead data in June: the bare `OT_SP1a_ChargedBolt` it names is never played, so no `JUNE_BUGS.md` row."],
 'CH2d': ["`RSI_A_RWT_BARRAGE_ATTACK_AIR` FrameMove `32c4b9e0` does `+0x7cc8 *= 1.2` (24212). `RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT` is here; `_ShowOverheatIcon` is CH1g2's."],
 'CH3a': ["`32c3e450` (called by `RSI_SA_RST_SHOCKWAVE_FRONT` FrameMove `32c3e680`) is here by CH0's hand rule."],
 'CH3c': ["**Split by CH0:** CH3c = Veteran Commander skills except Ignition Crow and Deadly Raid, which are **CH3c2** (45 functions with their overheat states)."],
 'CH3c2': ["Scope (CH0 split of CH3c): Ignition Crow Napalm 24022 / 24053, Incineration 24023 / 24054, Deadly Raid 24024 / 24055: the `RVC_SI_SA_*` tables incl. the `*_OVERHEAT` variants, helpers `32c37470` / `32c5af90`. "
            "`RVC_SI_SA_IGNITION_CROW*_OVERHEAT_END_UPGRADE` are dead data. Overheat mode itself (24415) is CH4c's; CH1g2 provides `CreateOverHeatObject`."],
 'CH3d': ["7ao: 24419 in `HyperModeStart` `31ffd490` and the transcended return of `32c4e7c0` (CH1h writes the body)."],
 'CH4a': ["**Passive ids:** `ch0_passive_ids.tsv`: 32 live March `SI_*` reads whose June id differs (e.g. `SI_P_RRF_EXPLOSIVE_NASOD_HAND` 4410 -> 24402, `SI_P_RVC_OVERHEAT_MODE` 4418 -> 24415, `SI_A_RF_SHADOW_STEP` 4205 -> 24200); 20 more renamed under another class tag (`SI_P_RRF_BURNING_NASOD_HAND` 4408 -> `RNSI_P_ROT_BURNING_NASOD_HAND` 24401). "
          "Burning Nasod Hand: the 24401 setter here pairs with CH1j's +0x4108 write. 0x73c `32c3c7f0` / 0x748 `32c3c820` (uncarved) are here (7an)."],
 'CH4c': ["**Map: `june2017_re/char_raven/ch0_overheat.tsv`** (50 rows, col 7 owner: 24 CH4c). June reads `SA_OVERHEAT_HP` (slot 125) / `_PVP` (145) only in InitPassiveSkillState (24415 @32c58307); March's CommonFrameMove re-read has no twin. "
          "`_CanOverheat` / `_BurnOverheatHP` are not located; the extra-damage pack is 37 in June vs March's 35; the VC skill-slot icon swap is June `328e0aa0`. "
          "Left arm: June's reader `329657d0` is inside `3295dfa0` (june-engine R14c ports it whole); arm hiding `3201c3a0` is already ported; CH4c locates the viewer and skill-state readers."],
 'CH5': ["`ch0_unittemplet_raven.tsv`: 5 rows differ only by `m_fLobbyRotationY`; 106 / 107 / 115 add `m_HyperCutIn` `HQ_Cutin_RBM/RRF/RVC_Transcendence`; 115's name `STR_ID_617` -> `STR_ID_17431` (\"Veteran Commander\", present in `general.ess` and `StringID_def`). The cut-in `.gfx` are already June's bytes."],
}

NEW_KICKOFFS = {
 'CH3c2': ('CH3c', 'Skills: Veteran Commander Ignition Crow and Deadly Raid (with their overheat states)', 'CH3c review'),
}


def phase_block(p):
    out = []
    fn = [r for r in cm if r[6] == p]
    if fn:
        c = collections.Counter(r[4] for r in fn)
        out.append('- **Functions: %d** (`awk -F\'\\t\' \'$7=="%s"\' june2017_re/char_raven/ch0_class_map.tsv`; %s).' %
                   (len(fn), p, ', '.join('%s %d' % kv for kv in sorted(c.items()))))
        out.append('  June addresses: ' + ' '.join('`%s`%s' % (r[0], '' if r[4] == 'inside' else '(%s %s)' % (r[4], r[5])) for r in fn) + '.')
    st = [r for r in states if len(r) > 7 and r[7] == p]
    if st:
        out.append('- **States: %d** (`awk -F\'\\t\' \'$8=="%s"\' june2017_re/char_raven/ch0_state_skill.tsv`): %s.' %
                   (len(st), p, ', '.join('`%s`' % r[0] for r in st)))
    sk = [r for r in skills if len(r) > 5 and r[5] == p]
    if sk:
        out.append('- **Skills: %d** (part 2 of `ch0_state_skill.tsv`, col 6): %s.' % (len(sk), ', '.join('%s %s' % (r[0], r[1]) for r in sk)))
    rows = [r for r in ck if len(r) > 4 and re.search(r'(^|\s)%s(\s|$)' % re.escape(p), r[4])]
    if rows:
        out.append('- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): ' + '; '.join('%s %s %s' % (r[0], r[1], r[2][:70]) for r in rows) + '.')
    for hp, (before, cs) in HELPER.items():
        if hp == p:
            out.append('- **Callees to map** (`ch0_unmapped_identified.tsv`): ' + '; '.join(
                '`%s` %s (%s; callers %s)' % (c, ui[c][2][:80], ui[c][3], ui[c][8][:40]) for c in cs) + '.')
        elif before == p:
            out.append('- **Depends on CH0\'s Opus helper phase %s** (%s).' % (hp, ', '.join('`%s`' % c for c in cs)))
    if p in OWN:
        out.append('- **Callees this Opus phase maps itself:** ' + '; '.join('`%s` %s' % (c, ui[c][2][:80]) for c in OWN[p]) + '.')
    for n in NOTES.get(p, []):
        out.append('- ' + n)
    return out


def helper_kickoff(hp, before, cs):
    return '\n'.join([
        '# June character Raven (Stage 7) - Phase %s - Helpers: engine callees the Sonnet phase %s needs (Opus)' % (hp, before), '',
        '| | |', '|---|---|',
        '| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven %s` |' % hp,
        '| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |',
        '| Depends on | the review of the phase before %s; one at a time with the other `X2GURaven.*` / `X2GUUser.*` phases |' % before,
        '| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |',
        '| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |',
        '| Review | `kickoffs/char-raven/%s-review.md` (Opus 5.5 high; the phase writes it when done) |' % hp, '',
        '## Read first (and nothing else from the plan doc unless you need it)', '', '```sh',
        'python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "Phase CH0 - Raven map (read-only)" "CH0 - Done (2026-11-05)"',
        "awk -F'\\t' '$1 ~ /^(%s)/' june2017_re/char_raven/ch0_unmapped_identified.tsv june2017_re/char_raven/ch0_unmapped_callees.tsv" % '|'.join(cs),
        '```', '', '## Task', '',
        'CH0 found callees outside Raven\'s class range that no doc maps and that the Sonnet phase `%s` would stop on ("this needs Opus"). '
        'Map each in Ghidra, name it, give it a March home (an existing March function when June\'s namesake does the same; else a new one under the flag, '
        'by the owning class\'s pattern, no `June` in the name), and port its whole body (June\'s numbers and quirks verbatim, a `JUNE_BUGS.md` row each). '
        'A funcmap row per function (`Char Raven %s`) so `%s` calls it by its March name. Check first whether Aisha\'s, Eve\'s or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).' % (before, hp, before),
        ''] + ['- **`%s`** %s; CH0 grade: %s; callers %s; evidence: %s' % (c, ui[c][2], ui[c][3], ui[c][8], ui[c][4][:160]) for c in cs] + [
        '',
        'You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. '
        'Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `%s`\'s kickoff names the March symbols; '
        '`junediverge.py --check` and `enumscan.py --check` PASS.' % before, ''])


def main():
    dry = '--dry' in sys.argv
    # new split kickoffs: a copy of the base kickoff with the phase id and scope swapped
    for p, (base, scope, dep) in NEW_KICKOFFS.items():
        dst = os.path.join(K, p + '.md')
        if os.path.exists(dst):
            continue
        s = open(os.path.join(K, base + '.md'), encoding='utf-8').read()
        s = s.replace('Phase %s - ' % base, 'Phase %s - ' % p, 1).replace('char-raven %s`' % base, 'char-raven %s`' % p)
        s = s.replace('"%s"' % base, '"%s"' % p).replace('/%s/' % base, '/%s/' % p).replace('%s-review.md' % base, '%s-review.md' % p)
        s = re.sub(r'^(# .*? - Phase %s - ).*$' % re.escape(p), r'\g<1>' + scope, s, count=1, flags=re.M)
        s = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>' + dep + '; one at a time with the other `X2GURaven.*` phases', s, count=1)
        s = s.replace('the class map gives phase `%s`' % base, 'the class map gives phase `%s`' % p)
        if not dry:
            open(dst, 'w', encoding='utf-8', newline='\n').write(s)
        print('new', p)
    for hp, (before, cs) in HELPER.items():
        dst = os.path.join(K, hp + '.md')
        if not os.path.exists(dst):
            if not dry:
                open(dst, 'w', encoding='utf-8', newline='\n').write(helper_kickoff(hp, before, cs))
            print('new', hp)
    for f in sorted(os.listdir(K)):
        if not f.endswith('.md') or f.endswith('-review.md'):
            continue
        p = f[:-3]
        path = os.path.join(K, f)
        s = open(path, encoding='utf-8').read()
        s2 = s.replace("'$6==\"", "'$7==\"").replace('      # CH0 amends the column number if it differs', '')
        if PLACE in s2:
            body = phase_block(p) or ['- Nothing in the CH0 maps carries this phase\'s id; the plan section is the scope.']
            s2 = s2.replace('## Amended by CH0\n\n' + PLACE, '## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(body))
        elif (p in NEW_KICKOFFS or p in ('CH0b', 'CH1')) and '## Amended by CH0 (' not in s2:   # Raven's CH0b / CH1 kickoffs have no placeholder
            s2 = s2.rstrip('\n') + '\n\n## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(phase_block(p)) + '\n'
        if s2 != s:
            if not dry:
                open(path, 'w', encoding='utf-8', newline='\n').write(s2)
            print('amended', p)


if __name__ == '__main__':
    main()
