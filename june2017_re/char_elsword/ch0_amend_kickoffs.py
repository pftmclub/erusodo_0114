"""char-elsword CH0 item 11: fill each kickoff's "Amended by CH0" section from the CH0 tsvs, and write the kickoffs CH0 adds.
    python june2017_re/char_elsword/ch0_amend_kickoffs.py [--dry]
Per phase: functions (ch0_class_map.tsv col 7) with inside / new, states (ch0_state_skill.tsv col 8), skills (part 2 col 6),
checklist rows (ch0_checklist_rows.tsv col 5), callees CH0 routed to an Opus helper phase (HELPER), plus the hand notes in NOTES.
Replaces the placeholder paragraph only; a kickoff whose section is already filled is left alone.
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
T = os.path.normpath(os.path.join(HERE, '..', '..'))
K = os.path.join(T, 'kickoffs', 'char-elsword')
PLACE = '(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)'
DATE = '2026-11-03'


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

# CH0's grouping of the 26 needs-Opus callees: helper phase -> callees (the Opus phases CH1m / CH1n / CH4a map their own)
HELPER = collections.OrderedDict([
])  # FILLED_AFTER_UNMAPPED

OWN = {'CH1o': ['322d2680', '31f76580'], 'CH1q': ['326106c0', '32715040']}

NOTES = {
 'CH0b': [
  "**Stage list** `june2017_re/char_elsword/ch0b_stage_list.tsv`: 4,848 rows (4,465 missing, 383 march-copy; texture 3,305, mesh 1,249, sound 202, motion 65, other 27), no Lua. "
  "CH0 dry run: `STAGE-JUNE DRY RUN: 36 kom(s), 4848 staged (4465 new, 383 replaced, 0 identical), 0 refused` (largest data017 2,226, data191 433, data130 423, data192 289). No new kom number.",
  "**data199:** March's `data199.kom` holds 3 files (`Motion_CSM_EIS_BC.x`, `_ELK_IC.x`, `_ERS_OG.x`), byte-identical to June's; June's data199 adds 70 `Motion_Elsword*` + `Motion_Standard_Elsword.X`. "
  "`stage-june` routes the 54 June-only motions (53 emotions + Standard) to **data199** (June's own number; the packed kom is a superset of March's, nothing dropped) and the 11 differing class motions to **data033**, where March ships them.",
  "**Motion question answered: nothing is lost.** Only `SI_A_EIS_SPIRAL_STING` leaves `Motion_Elsword_IS.x`; June moved it to `Motion_Elsword_TK.x`, which the Infinity Sword templet row loads as `m_GameMotion1` in both March and June. "
  "So **no kom is held for CH1 final**: data033 and data199 are copied here with the rest (`ch0_motion_question.tsv`, 82 rows).",
  "**Skill icons:** `skillicons.py check 106 900 21000-21060 21200-21236 21400-21420 21600-21605` = 17 misses in `DLG_SKILL_ElswordActive_04`, `_05`, `DLG_Elsword_SKILL_ScreenShot_04` (all in the list). "
  "16 clear once staged; **21402 `SI_P_ELK_WIND_SLICER`**'s picture names a piece June's own `ScreenShot_04` .TET lacks: June's gap, a `JUNE_BUGS.md` row (as Chung's 488), not a stage miss.",
  "`POPUP_TR_L_KNIGHT` / `_R_SLAYER` / `_I_SWORD` (.tga + .TET) are in the list (data215). The transcendence cut-ins `HQ_Cutin_ELK/ERS/EIS_Transcendence` are already June's bytes (data195). "
  "`Motion_EVENT_ELSWORD_DP1.x` is an event NPC motion (june-dungeons), not staged. Save: no Sword Knight (10) unit exists; create it and a lv 99 Sword Man.",
 ],
 'CH1a': ["`ESSI_LEAP` (id 287, ungated) is here by CH0's hand rule. Its StartFuture `32d119b0` calls `CX2Game::GetNearestNPCUnitUID` (June `322d2680` differs: a NULL gage counts as alive): call the March name; the replace row is allow-listed until CH1o ports June's body."],
 'CH1b': ["Die front / back call `End_SwordmanVigor` / `End_SwordmanDestruction` (CH1q): declare them with `// June <va>: body in CH1q`. Checklist L281: `32d076f0` is the Revenge reaction state (here)."],
 'CH1c': ["Only 2 class functions: the 34 riding / pepper / dance / `MACHINE` states use `CX2GUUser` callbacks (Chung's CH1n2 already replaced the base ones). `ESSI_THROW_WOODEN_PIECE` is a dead table in June (no registration)."],
 'CH1d': ["**Split by CH0:** CH1d = ground combos `ESSI_COMBO_*` and `ESSI_JUMP_ATTACK_X/Z` (28); the dash and dash-jump combos are **CH1d2** (28)."],
 'CH1d2': ["Scope (CH0 split of CH1d): `ESSI_DASH_ATTACK_X`, `ESSI_DASH_COMBO_*`, `ESSI_DASH_JUMP_COMBO_*` (incl. `_LANDING`), and the shared `ESSI_KNIGHT_DASH_COMBO_Z2/Z3` / `ESSI_DASH_JUMP_COMBO_X2` (gated for several job lines; CH0 hand rule). 7h: `32cf8230` is here."],
 'CH1g': ["**Split by CH0:** CH1g = Sheath Knight line `ESSI_ETK_*` (31); Infinity Sword `ESSI_EIS_*` and the Compact Counter guard attack are **CH1g2** (16)."],
 'CH1g2': ["Scope (CH0 split of CH1g): `ESSI_EIS_*` states and `ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK` (6 callbacks). June registers the latter on id 162 when **21216 `RNSI_A_EIS_COMPACT_COUNTER`** (class 112 only) is learned; March gates on `SI_P_ETK_COMPACT_COUNTER` (the registration is CH1m's)."],
 'CH1h': ["0x24c / 0x250 (`32d1f690` / `32d0b870`) stay here: they switch on buff ids 3 / 163-165 / 19 and set class fields (Chung sent his to CH4a; Elsword's read no passive).",
          "ReInit, DamageReact, InitComponent, InitEffect hold Way of the Sword parts: call CH1q's functions (declared, `body in CH1q`). DamageReact calls `CX2DamageManager::CalcWayOfSwordPointOnDamageReact` by its March name (June `326106c0` differs; allow-listed until CH1q).",
          "Slot names (`ch0_vtable_slots.tsv`, `ch0_unmapped_identified.tsv`): 0xbc `GetFinalDamageToBeChangedByActiveSkill` (inside), 0x4ac `GetSpecialItemUsableExtraStateID` (returns 0xce `ESSI_SWORD_WAIT`), 0x508 / 0x50c empty overrides (called from `GetFinalDamage` 32284d43 / 32284d62), 0x510 applies passive 0x53a9's buff after `ProcessSocketEffectEx` (DamageReact @320478c9). `32d270f0` / `32d27100` are compiler this-adjust thunks (n/a)."],
 'CH1i': ["0x670 `32d05960` is `IsInJobLine` (Chung's name, `X2GUChung.h:660`): line 1 = classes 10 / 100, 2 = 11 / 101, 3 = 22 / 112. The Crest of Time pair 0x740 / 0x744 is CH1q."],
 'CH1j': ["CommonFrameMove `32d1b2b0` calls `WayOfSwordStateChange` `32d0eeb0` (CH1q: declare it). CommonFrameMoveFuture `32d0b4d0` writes the sync field from `GetWayOfSwordPoint`."],
 'CH1k': ["The ctor `32d09490` (caller outside the class) and `InitializeElswordMinorParticleArray` `32cf5660` (called inside the ctor @32d0b07b). 0x840 `32d00fe0` (`?`): called from DamageReact @320478dd, applies skill 0x539c's buff when `GetApplyWeaponExtraDamageType` != 0. 0x850 `AfterImageFrameMove`."],
 'CH1m': ["InitStateCommon is **one** function, `32d1f8f0` (ids 7-286, no class gate); InitState `32d23ff0` is carved and holds the class gates (`*(+0x867c)+0x38`; ranges in `ch0_build_state_skill.py` GATES), the START / WIN / LOSE roll and id 130. `ch0c_listing_check.py`: **180 / 180** rows; no hand rows.",
          "Id 162: June registers `ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK` when 21216 `RNSI_A_EIS_COMPACT_COUNTER` is learned, else `ESSI_REVENGE_ATTACK` (March gates on `SI_P_ETK_COMPACT_COUNTER`). "
          "7 names registered with no table (`ESSI_ELK_XXXXXX`, `ESSI_RIDING_DAMAGE_BACK`, 5 `USI_EVENT_SNIPER/PROTON_BOMB_*`); `ESSI_RIDING_DAMAGE_FRONT` is defined twice in June's script (a JUNE_BUGS row); 27 March-only states dropped (26 `*_HYPER`, `ESSI_SI1_ES_MEGA_SLASH`); 45 dead tables (`ch0_state_skill.tsv` totals)."],
 'CH1n': ["`ESSI_SUMMON_MAGIC_SPEAR` (id 286) registers only `CX2GUUser` callbacks (`31ffba50` / `31fcbd10`). The skill switch read from the listing: 143 rows for 127 ids (`ch0_skill_listing_rows.tsv`; 22 passives produce no state)."],
 'CH1o': ["No class-map function carries CH1o after CH0's triage (the 9 unsettled helpers went to CH1h / CH1k / CH4a / CH4c / n/a). It owns two engine callees: replace `CX2Game::GetNearestNPCUnitUID` with June's `322d2680` (tsv row added by CH0; remove its allow row) and write June-only `31f76580` (main-effect scale ( mode, x ) -> vector (x,0,0): zeroes y / z, a `JUNE_BUGS.md` candidate) that CH2d's `32d16c00` calls."],
 'CH1p': ["7h: 24 functions (not 25; `[reg+0x8ca4]` scan, 48 sites) are written by their own phases (`ch0_func_rows.tsv`); CH1p owns the shared cancel rule. No June-only key is owned here (the camera pair is Stage 9's, `IMMUNITY_EDT_LIST_AT_THIS_STATE` backlog Stage 2)."],
 'CH1q': ["**Split by CH0** (about 60 functions): CH1q = the class side (21 class-map functions: gauge get / set, vigor and destruction start / end, `WayOfSwordStateChange`, `DoStateDestructionRage`, the Sword Wait / Destruction Rage states, `GetFinalDamage_WayOfSword` 0x74 (June adds a PvP-type branch), `CreateGageData` 0xd0, the Crest of Time pair 0x740 / 0x744 `StoreGageSnapshot` / `RestoreGageSnapshot`, which save only the point at +0x92b8) "
          "+ the damage-manager math (loader `3261aee0`, `CalcWayOfSwordPoint` `326105c0`, `CalcWayOfSwordPointOnDamageReact` `326106c0` (replace row; remove its allow row), `CalcVigorMpUpOnAttack` `32610790`, `CalcVigorMpUpOnDamageReact` `326107e0`, `GetDestRageWSPGain` `32622600`, Elesis's `32622550` / `32622660`), the damage-effect block (`31f85910` @31f86bd6-31f86eeb), the AttackResult MP factor (`32012b50` @32012d98 / @32012ee3, checklist L930) and the sync field (+0x7ec0 / +0x8160). Gauge data / UI / manager are **CH1q2**.",
          "Map: `june2017_re/char_elsword/ch0_wos.tsv` (111 rows: 76 paired, 15 not located, 4 absent in June, 6 June-only; row `32d05960` is **not** WoS, it is `IsInJobLine`). Liveness by `cl /EP`: `ELSWORD_WAY_OF_SWORD` and `SPECIAL_ABILITY_WAY_OF_SWORD` live. "
          "`WayOfSword.lua`: June 42 keys vs March 28 (3 values differ: `Const_DestRageWSPGain` 1500 -> 2250, `SkillTypeConst_Dest/VigorCommand` 96 -> 67.2); June's loader reads only the 28, the 14 June-only keys have no reader found: settle before adopting. Unsure (CH0): whether June's `GetActualMPConsume` still applies Vigor x0.7 (CH4b reads it)."],
 'CH1q2': ["Scope (CH0 split of CH1q): `CX2ElswordGageData` (vtable `3349b0c4`, size 0x11c, +0x110 changed / +0x114 state / +0x118 point; Update `32a34890`, InitWhenGameIsOver `32a348d0` (June no longer resets the state), ToPacket `32a323e0`, FromPacket `32a348e0`, Clone `32a35e60`, Copy `32a34930` (no longer forces the changed flag), SetState `32a36310`, copy ctor `32a37930`); "
           "`CX2ElswordMyGageUI` (vtable `3349aba4`: InitUI `32e6d830`, OnFrameMove `32e71850`, `UpdateUIElswordWSP` `32e67df0`, `UIFrameMoveElswordWSP` `32e60f00`, Stop `32e675c0`, PlayDest / PlayVigor `32e67770` / `32e67ab0`, SetShow `32e5e180`, SetShowWspParticle `32e5e300`); the gauge-manager creation switch `3248e290`; "
           "`CX2Game::UpdateElswordSkillSlotEffect` `322cd5c0` (checklist L827); the class-switch gauge randomiser `322ed400` (June-only). `DLG_Elsword_WayOfSword.lua` is unchanged from March (`luabind.py check` PASS); adopt with CH1 final. Elesis's own copies (vtables `334fa61c`, `3349b184`, `3349ac34`) stay char-elesis's."],
 'CH1': ["No motion kom is held for this phase: CH0 found no lost animation (CH0b copies data033 / data199). Adopt `Elsword_SwordMan.lua`, `WayOfSword.lua` (see CH1q on its 14 unread keys) and `DLG_Elsword_WayOfSword.lua`."],
 'CH2a': ["The two `c1a` zero rows are **not** Sword Man's: 21006 Mortal Blow (class 10) is CH2b, 21013 Wind Blade (class 11) is CH2c (the plan text said CH2a)."],
 'CH2b': ["`c1a` zero row: **21006 `RNSI_SA_ESK_MORTAL_BLOW`** (`c1a_numerical_blocks.tsv` l.731, `DamageEffect_ESK_Mortal_Blow_Ultimate_Fire_Ground` INDEX 2): verbatim + `JUNE_BUGS.md` row. Checklist L124 / L125 (`APPLY_LEARNED` keys in `ESSI_A_ESK_ARMOR_BREAK`) are here."],
 'CH2c': ["`c1a` zero row: **21013 `RNSI_SA_EMK_WIND_BLADE`** (l.1274, `DamageEffect_SA_EMK_WindBlade_Ultimate_Projectile` INDEX 2): verbatim + `JUNE_BUGS.md` row."],
 'CH2d': ["`32d16c00` (Crescent Cut) calls June-only `31f76580`: CH1o writes it; call it by CH1o's name. Alpha tags (7c): `32d14100` x9 (Sword Fall FrameMove)."],
 'CH3a': ["March-only templet `LORD_KNIGHT_SAND_STORM` at `X2GUElsword_SwordMan.cpp:15450` (7u: take June's caller, drop the name)."],
 'CH3b': ["March-only `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER` / Sword Enchant site `X2GUElsword_SwordMan.cpp:15268` (7u)."],
 'CH4a': ["`LightingStepFrameMove` `32d08ba0` (delegate registered @32d1d410): June's dash threshold is 0.5 s, March's 1.0 s. The hit-list clear `32d270a0` is here. Enum names: 4 of 127 ids are in the C++ enum by name, 28 lookalikes, 95 none; 64 in the live core `Enum.lua` (`ch0_enum_names.tsv`)."],
 'CH4b': ["Read whether June's `GetActualMPConsume` `32cf4b70` still applies the Vigor x0.7 MP rule (CH0's WoS read found no WoS access; unproven)."],
 'CH4c': ["`MemoConditionElsword` vtable `3352286c`: dtor `32c69d80`, decide `32cf4d00` (calls the always-false base `31fd4450`, as Eve's `32968490`), parse thunk `32cf4d20` -> `31fc8cf0`; tsv row `MemoCondition` (replace)."],
 'CH5': ["`STR_ID_4641` \"Lord Knight\", `4642` \"Rune Slayer\", `17425` \"Infinity Sword\" all exist (`StringID_def.lua`, `general.ess`). Row diff: `ch0_unittemplet_elsword.tsv` (178 key lines)."],
}

NEW_KICKOFFS = {
 'CH1q2': ('CH1q', 'Way of the Sword: gauge data, gauge UI, gauge manager, skill-slot effect', 'CH1q review'),
 'CH1d2': ('CH1d', 'Class core: Sword Man dash and dash-jump combos (shared by every line)', 'CH1d review'),
 'CH1g2': ('CH1g', 'Class core: Infinity Sword line (ESSI_EIS*, Compact Counter guard attack)', 'CH1g review'),
}


def phase_block(p):
    out = []
    fn = [r for r in cm if r[6] == p]
    if fn:
        c = collections.Counter(r[4] for r in fn)
        out.append('- **Functions: %d** (`awk -F\'\\t\' \'$7=="%s"\' june2017_re/char_elsword/ch0_class_map.tsv`; %s).' %
                   (len(fn), p, ', '.join('%s %d' % kv for kv in sorted(c.items()))))
        out.append('  June addresses: ' + ' '.join('`%s`%s' % (r[0], '' if r[4] == 'inside' else '(%s %s)' % (r[4], r[5])) for r in fn) + '.')
    st = [r for r in states if len(r) > 7 and r[7] == p]
    if st:
        out.append('- **States: %d** (`awk -F\'\\t\' \'$8=="%s"\' june2017_re/char_elsword/ch0_state_skill.tsv`): %s.' %
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
        '# June character Elsword (Stage 7) - Phase %s - Helpers: engine callees the Sonnet phase %s needs (Opus)' % (hp, before), '',
        '| | |', '|---|---|',
        '| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword %s` |' % hp,
        '| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |',
        '| Depends on | the review of the phase before %s; one at a time with the other `X2GUElsword_SwordMan.*` / `X2GUUser.*` phases |' % before,
        '| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |',
        '| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |',
        '| Review | `kickoffs/char-elsword/%s-review.md` (Opus 5.5 high; the phase writes it when done) |' % hp, '',
        '## Read first (and nothing else from the plan doc unless you need it)', '', '```sh',
        'python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "CH0 - Done (2026-11-03)"',
        "awk -F'\\t' '$1 ~ /^(%s)/' june2017_re/char_elsword/ch0_unmapped_identified.tsv june2017_re/char_elsword/ch0_unmapped_callees.tsv" % '|'.join(cs),
        '```', '', '## Task', '',
        'CH0 found callees outside Elsword\'s class range that no doc maps and that the Sonnet phase `%s` would stop on ("this needs Opus"). '
        'Map each in Ghidra, name it, give it a March home (an existing March function when June\'s namesake does the same; else a new one under the flag, '
        'by the owning class\'s pattern, no `June` in the name), and port its whole body (June\'s numbers and quirks verbatim, a `JUNE_BUGS.md` row each). '
        'A funcmap row per function (`Char Elsword %s`) so `%s` calls it by its March name. Check first whether Aisha\'s, Eve\'s or a Stage 2-3 port already holds it (`funcmap.tsv`, `junediverge.py --list`).' % (before, hp, before),
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
        s = s.replace('Phase %s - ' % base, 'Phase %s - ' % p, 1).replace('char-elsword %s`' % base, 'char-elsword %s`' % p)
        s = s.replace('"%s"' % base, '"%s"' % p).replace('/%s/' % base, '/%s/' % p).replace('%s-review.md' % base, '%s-review.md' % p)
        s = re.sub(r'^(# .*? - Phase %s - ).*$' % re.escape(p), r'\g<1>' + scope, s, count=1, flags=re.M)
        s = re.sub(r'(\| Depends on \| )[^\n]*', r'\g<1>' + dep + '; one at a time with the other `X2GUElsword_SwordMan.*` phases', s, count=1)
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
        elif p in NEW_KICKOFFS and '## Amended by CH0 (' not in s2:
            s2 = s2.rstrip('\n') + '\n\n## Amended by CH0 (%s)\n\n' % DATE + '\n'.join(phase_block(p)) + '\n'
        if s2 != s:
            if not dry:
                open(path, 'w', encoding='utf-8', newline='\n').write(s2)
            print('amended', p)


if __name__ == '__main__':
    main()
