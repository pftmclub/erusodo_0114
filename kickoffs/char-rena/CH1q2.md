# June character Rena (Stage 7) - Phase CH1q2 - Natural force, engine side: gauge data / UI, gauge manager, emulator, June engine callers (CH0 split of CH1q)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1q2` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1q review. One at a time with the other `X2GULire_ElvenRanger.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (+ `_NF_DEBUG`; defined only with `_LIVE`) |
| Log tag | none (nothing runs until `_LIVE`); `[JUNE_CHAR]` (`RENA NF`) from CH1 final |
| Review | `kickoffs/char-rena/CH1q2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Open decisions (for the user)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1q - Done" "CH1q - Review"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1q - Done (2026-10-27)" "CH1q - Review (2026-10-27)"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and `june2017_re/char_rena/ch0_nf.tsv`. Raven's CH1q (rage)
and Eve's Nasod Core are the worked examples.

## Task

- **CH0 split of CH1q:** CH1q writes the class side (the 27 class functions, the `_NF` / `ADD_RENA_SYSTEM` definition, the flipped engine arms); this phase writes the engine side listed under "Amended by CH0" (gauge data / UI, gauge manager, emulator, packet, June engine callers). The bullets below are the whole natural-force scope; take only the engine-side ones.
- **Open decision 1 = (a)** (user, 2026-11-07): as Eve's Nasod Core, March's dormant `ADD_RENA_SYSTEM` code is the base, June's whole body
  wherever it differs.
- Port natural force from `ch0_nf.tsv`: `_NF` defines `ADD_RENA_SYSTEM` (only with `_LIVE`) with its `ServerDefine.h` second home
  (`CommonPacket.h` is reached before `Always.h`) and a `SECOND_HOMES` row in `tools/flagcheck.py`; then June's whole body inside every March NF function
  that differs: class tail `:23576-24004` (`CreateGageData`, `AttackResultByType`, `Init`, `IsHyperActiveSkill`), `X2GUUser.*`, the first-attack /
  relate-skill copies in `X2DamageManager` / `X2DamageEffect` / `X2EffectSet`, `X2GUNPC.*` incl. `AttackResultByType` `32773ee0`, `CX2RenaGageData` /
  `CX2RenaMyGageUI`, `X2GageManager`, `CAC_NATURAL_FORCE` and `KGamePlayStatus::GetNaturalForce()`.
- June-only pieces under plain names: `FriendshipNatureData` (RTTI `3373d6fc`), `32cb50c0` / `32cb6290( 5 )`, the `ApplyHyperModeBuff` Rena branch
  (`32298590 @322985ec`, checklist l.959).
- Resolve each June-port arm the flag flips to June's branch: `X2DamageManager.h:1386` / `:1663`, `.cpp:314` / `:2202` / `:3038` / `:3506`
  (`m_iNaturalForceType`, June `+0x56c`), `X2GUUser.h:1410`, `X2GUUser.cpp:10893`, `X2DamageEffect.cpp:5217` (`SERV_IRUHADEV_JUNE_SPAWN_INHERIT`'s child
  sequence), `X2GUNPC.cpp:11404`. Drop the studio "NF:" overlay (`X2GUUser.cpp:29719`) and dev fill key (`X2Game.cpp:5855`) if June has none.
- The emulator: `CAC_NATURAL_FORCE` for Lire in `X2OfflineStatTable.cpp:289-313`; whether June keeps NF across a relog decides a save column (no schema
  change unless June's packet carries it; snapshot `db_backup/` first). Sub-tag `NF`; a temporary `_NF_DEBUG` for CH1 final's play-test lines. Split if
  `ch0_nf.tsv` is over ~45 functions (CH1q2: gauge UI and emulator).

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

## Amended by CH0 (2026-11-07)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §3 L149 "Natural force: ADD_RENA_SYSTEM is off under US_SERVICE; ""a decision ; §7ao L959 ApplyHyperModeBuff 32298590 @322985ec: unit type 3 -> 32cb50c0 count x; §7aq JUNE_PORT_BACKLOG_ORDER.md:236 "JUNE_PORT_BACKLOG_ORDER.md:236 ""Rena 32cb6290( 1 )"" (training-game .
- Gauge data `CX2RenaGageData` (vtable `3349b244`, size 0x12c; ctor `32498a30`, copy ctor `32a37e00`, dtor `32498e00`; Update `32a34f20`, InitWhenGameIsOver `32a34f80`, GetMyPlayStatus `32a32500`, SetMyPlayStatus `32a34fa0`, Clone `32a36060`, Copy `32a34fe0`); gauge UI `CX2RenaMyGageUI` (vtables `3353363c` / `33533624`; ctor `32e64760`, dtor `32e725a0`, InitUI `32e6e190` (`DLG_Rena_NaturalForce.lua`), OnFrameMove `32e71970`, UpdateNaturalForceUI `32e5ec10`), created by `CX2GageManager::CreateMyGageUI` `3248e290` case `3248e531` for classes 3 / 12 / 13 / 24 / 102 / 103 / 114. Engine callers: `ApplyHyperModeBuff` `32298590 @322985ec` (count x 15.0 to vslot 0xf0, then UpNaturalForce( 5 ); checklist l.959), `CX2GUNPC::AttackResultByType` `32773ee0` NF branch `@3277452b-3277464b`, the F7 training key `322ed400` (Rena: UpNaturalForce( 1 ) `@322ed57b`). Packet: June keeps March's `KGamePlayStatus` (`m_cCharAbilType` = `CAC_NATURAL_FORCE` 5, `m_iCharAbilCount`): the emulator's Lire case (`X2OfflineStatTable.cpp:289-313`, today `CAC_NONE`) and whether the count survives a relog are decided here (no save column today).
