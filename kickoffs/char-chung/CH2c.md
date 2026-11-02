# June character Chung (Stage 7) - Phase CH2c - Skills: Shooting Guardian (21)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH2c` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2b review and CH3b0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH2c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2c` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 21 has that class 6 does not; siege mode (`CSI_SI_A_SIEGE_SHELLING_*`, `SIEGE_ANGLE` if CH0 placed it here). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai
BFI arguments, any 7b block CH0 put here, the numerical-only tables of `plan_june_side.md` §3); its effect closure (a name found only now is staged
byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check
the templet swap for every term that reads the id (the Aisha CH3b review defect). An id with no June `SKILL_ID` name stays a raw id with a
one-line comment (plan Rules). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Chung: CH2c)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence), what June's motion and effect look like.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-10-31)

- **Functions: 43** (`awk -F'\t' '$7=="CH2c"' june2017_re/char_chung/ch0_class_map.tsv`; inside 33, new 10).
  June addresses: `32bd9a20` `32bd9a80`(new CSI_SI_SA_CSG_DUAL_BUSTER_StateStartFuture) `32bd9d10` `32bd9dd0` `32bd9de0`(new CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_StateStartFuture) `32bd9e50`(new CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_StateStart) `32bda130`(new CSI_SI_A_CDC_TRICK_SHOT_Init) `32bda1a0` `32bda2e0` `32bdb340` `32bdb810`(new CSI_SI_SA_CSG_DEADLY_SHOT_Init) `32be22c0` `32be2300` `32be2460` `32be2db0` `32be3e80` `32be3eb0` `32beacc0`(new CSI_SI_SA_CSG_DEADLY_SHOT_EventProcess) `32bf42c0` `32bfbe80` `32bfdd70`(new CSI_SI_A_CDC_TRICK_SHOT_FrameMove) `32bfe4d0`(new CSI_SI_A_CDC_TRICK_SHOT_EventProcess) `32bfe730` `32bfea90` `32bfeb30` `32bff350` `32bff370` `32bff3d0`(new CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStartFuture) `32bff430` `32bff4f0` `32bff550` `32bff5b0` `32bff610` `32bff670` `32bff6d0` `32bff730` `32bff790` `32bff7f0` `32c084e0` `32c0b160`(new CSI_SI_SA_CSG_DEADLY_SHOT_FrameMove) `32c0e150` `32c0e190` `32c15340`.
- **States: 20** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_SI_SA_CSG_DEADLY_SHOT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_END`, `CSI_SI_SA_CSG_DUAL_BUSTER`, `CSI_SI_SA_CSG_DUAL_BUSTER_HYPER`, `CSI_SI_SA_CSG_DUAL_BUSTER_LANDING`, `CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME`, `CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_HYPER`, `CSI_SI_A_CDC_TRICK_SHOT`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 26011 RNSI_SA_CSG_DUAL_BUSTER, 26012 RNSI_SA_CDC_SHARPSHOOTER_SYNDROME, 26013 RNSI_SA_CDC_DEADLY_SHOT, 26014 RNSI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA, 26032 RNSI_SA_CSG_DUAL_BUSTER_UPGRADE, 26035 RNSI_SA_CDC_SHARPSHOOTER_SYNDROME_UPGRADE, 26049 RNSI_SA_CDC_DEADLY_SHOT_UPGRADE, 26050 RNSI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA_UPGRADE, 26208 RNSI_A_CSG_HEAD_SHOT, 26209 RNSI_A_CDC_TRICK_SHOT, 26223 RNSI_A_CSG_HEAD_SHOT_UPGRADE, 26232 RNSI_A_CDC_TRICK_SHOT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7b L325-326 skill-memo entry 31fdbbb0 / class callers of 31fdbcc0; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L637 March-only effect DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell (; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim; §7ai L835 pass June's skip / adjust / level / creator at every ported BFI call.
- **CH3b0 (summons) runs before this phase:** Quantum Ballista's READY FrameMove `32bfe730` spawns `NUI_TARGETING_CHUNG` / `_NOT_SHOW_OTHER_PLAYER` (also Artillery Strike's). 7ai L835 (BFI 452 at `32bfea69`) is in that function: here.
- Trick Shot `32bfdd70` replaces March's `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell` caller (7u).

## Amended by CH1l (2026-11-01)

- State 336 (the upgraded Dual Buster's second registration of table `CSI_SI_SA_CSG_DUAL_BUSTER`) is the `_CORE` enum member `CSI_SI_SA_CSG_DUAL_BUSTER_UPGRADE` (CH1l; CH1m may rename it). `RNSI_SA_CSG_DUAL_BUSTER` 26011 is registered; `RNSI_SA_CSG_DUAL_BUSTER_UPGRADE` 26032 is still yours.

## Amended by CH1p (2026-11-02)

- `32bfeb80` is June's `ArtilleryStrikeControl_EventProcess` (March has the function; class map row moved here from CH3b, since its only caller is your
  `32bff350` `@32bff353`). CH1p converted only its key test under `_LIVE`: the key-pressed value is slot + 1 (`32bff328`: A S D C, F `+0x872a`, Q W E R,
  T `+0x872e`), read through `SkillSlotKey::IsKeyReleased`. June also gates a held key on `HaveThisBuff( BTI_BUFF_ARTILLERY_STRIKE_SCOPE )` (418,
  `@32bfeded`); without it, it takes the not-held path. That gate and the rest of the body are yours, written whole inside March's function.

## Amended by CH2a (2026-11-02)

- **Already written (do not redo):** `CX2GUChung::CanConsumeCannonBallForShot( needed, use, bSecondRate, float* out )` (June `32c0a230`) and `PlayCannonShotEffectSet( name, needed, use, power,
  bSecondRate, fallbackName )` (`32c11410`, returns the `CX2EffectSet::Handle`), both under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL`. Your callers `32c0e190` / `32c0f6d0` / `32c0fc30` use them by these
  names; read the Done note ("CH2a - Done") for the argument order and the extra-ball rules.
- **Siege Shelling (`CSI_SI_A_SIEGE_SHELLING_*`, 26201) is CH2a's:** its bodies, `CreateCannonBulletDamageEffect( CBT_SIEGE / CBT_SIEGE_FINISH )` calls and `UpdateSpineAimRotation` calls are in. The
  Shooting Guardian's own states (Mobile Siege, Shooting Star ...) stay yours. `CSI_SI_A_SIEGE_SHELLING_StateStart` (June `32bda6c0`) is the shared `CommonStateStart` wrapper.
- `PlayEffectSet` of an empty name plays nothing (stand-in in `PlayCannonShotEffectSet`).
- **Amended by CH3b0 (2026-11-02):** Quantum Ballista's READY FrameMove (`32bfe730`) spawns 1419 (`m_eArtilleryStrikeType == CAST_QUANTUM_BALLISTA`) or 863; the owner link and stats are CH3b0's (`CX2Game::SetUserSummonedNPCInfo` cases under `_SKILL`, shared `CX2GUUser::SetSummonHelperInfo` = June `322ece70`). CH3b0 could not play-test (no state for the summon skill yet), so this phase's hand-off carries its check: each spawn logs `[JUNE_CHAR] SUMMON npc <id> uid <uid> skill <skill> lv <n> flags 0x<f> ...` (read with `python tools/logs.py --grep "JUNE_CHAR] SUMMON"`). Expect `npc 1419 ... skill 26014 ... flags 0x0` (or 26050 upgraded).
