# June character Elsword (Stage 7) - Phase CH1q - Way of the Sword

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1q` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time: `X2GUElsword_SwordMan.*`, `X2GageData.*`, `X2GageUI.*`, `X2GageManager.*`, `X2DamageManager.*`, `X2DamageEffect.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (+ `_WOS_DEBUG`) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1q - Done (2026-11-02)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

Port June's Way of the Sword from `ch0_wos.tsv`: June's bodies inside March's WoS functions (class `:409-529` / `:7182-7480`,
`CX2ElswordGageData`, `CX2ElswordMyGageUI`, `X2GageManager`, `X2DamageManager::CalcWayOfSwordPoint` / `CalcVigorMpUpOnAttack` and the const
table, the `X2DamageEffect.cpp:5505-5519` caller, the `m_CannonBallCount` sync), June-only pieces under plain names. Shared pieces are ported
whole (no partial updates); Elesis's class-side code stays char-elesis's, and where June's shared body would break her unported March class,
guard it inside the June body with a one-line `stand-in: <class> not ported` comment. `WayOfSword.lua` / `DLG_Elsword_WayOfSword.lua`: `luabind.py
check`, adopted with CH1 final (`--dry-run` here). The vslot 0x740 snapshot override `32d058a0` if CH0 put it here. Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS`, sub-tag
`WOS`; a temporary `_WOS_DEBUG` for CH1 final's play-test lines.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Nothing runs until `_LIVE`: no play-test; the hand-off names the next phase.

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

## Amended by CH0 (2026-11-03)

- **Functions: 21** (`awk -F'\t' '$7=="CH1q"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 19, new 2).
  June addresses: `32cf36b0` `32cf36c0` `32cf50f0` `32cf5100` `32cf5130` `32cf5140` `32cf63c0` `32d058a0`(new StoreGageSnapshot) `32d058d0`(new RestoreGageSnapshot) `32d07810` `32d08ad0` `32d08b00` `32d0c960` `32d0cd60` `32d0cf70` `32d0d240` `32d0eeb0` `32d0f0c0` `32d0f300` `32d18cd0` `32d1b810`.
- **States: 2** (`awk -F'\t' '$8=="CH1q"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_SWORD_WAIT`, `ESSI_DESTRUCTION_RAGE`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 class keys WAY_OF_SWORD_TYPE x102, PHANTOM_BLADE_DAMAGE_DATA_NAME / _A; §7 L256 death-prevention pre-apply: gauge snapshot overrides, Crest of Time (v; §7c L352 29 alpha-tag sites 32d0d660 .. 32d1ecc0; §7an L930 AttackResult Way-of-Sword MP factor (32012b50 @32012d98, @32012ee3): 1.
- **Callees this Opus phase maps itself:** `326106c0` CX2DamageManager::CalcWayOfSwordPointOnDamageReact( float, int iWayOfSwordState ; `32715040` none in March: socket-option speed soft cap ( float -> float, three linear segme.
- **Split by CH0** (about 60 functions): CH1q = the class side (21 class-map functions: gauge get / set, vigor and destruction start / end, `WayOfSwordStateChange`, `DoStateDestructionRage`, the Sword Wait / Destruction Rage states, `GetFinalDamage_WayOfSword` 0x74 (June adds a PvP-type branch), `CreateGageData` 0xd0, the Crest of Time pair 0x740 / 0x744 `StoreGageSnapshot` / `RestoreGageSnapshot`, which save only the point at +0x92b8) + the damage-manager math (loader `3261aee0`, `CalcWayOfSwordPoint` `326105c0`, `CalcWayOfSwordPointOnDamageReact` `326106c0` (replace row; remove its allow row), `CalcVigorMpUpOnAttack` `32610790`, `CalcVigorMpUpOnDamageReact` `326107e0`, `GetDestRageWSPGain` `32622600`, Elesis's `32622550` / `32622660`), the damage-effect block (`31f85910` @31f86bd6-31f86eeb), the AttackResult MP factor (`32012b50` @32012d98 / @32012ee3, checklist L930) and the sync field (+0x7ec0 / +0x8160). Gauge data / UI / manager are **CH1q2**.
- Map: `june2017_re/char_elsword/ch0_wos.tsv` (111 rows: 76 paired, 15 not located, 4 absent in June, 6 June-only; row `32d05960` is **not** WoS, it is `IsInJobLine`). Liveness by `cl /EP`: `ELSWORD_WAY_OF_SWORD` and `SPECIAL_ABILITY_WAY_OF_SWORD` live. `WayOfSword.lua`: June 42 keys vs March 28 (3 values differ: `Const_DestRageWSPGain` 1500 -> 2250, `SkillTypeConst_Dest/VigorCommand` 96 -> 67.2); June's loader reads only the 28, the 14 June-only keys have no reader found: settle before adopting. Unsure (CH0): whether June's `GetActualMPConsume` still applies Vigor x0.7 (CH4b reads it).

## Amended by CH1a review (2026-11-04)

- SwordWait EventProcess `32d0f0c0` tests `oneZ` alone, with no re-attack gate (as Wait / Walk / JumpLanding / DashEnd in CH1a).

## Amended by CH1h (2026-11-04)

- CH1h's bodies call, by March name and unchanged: `End_SwordmanVigor` / `End_SwordmanDestruction` (ReInit, Die), `Start_SwordmanVigor` / `Start_SwordmanDestruction` (InitEffect), `SetWayOfSwordPoint` / `SetWayOfSwordState` / `GetWayOfSwordState`, `DoStateDestructionRage` (`32d0f300`) and `CX2DamageManager::CalcWayOfSwordPointOnDamageReact` (`326106c0`). ReInit also nudges the point by +-1 and ends the mode outside PvP (June, kept).
