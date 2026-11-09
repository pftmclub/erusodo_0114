# June character Rena (Stage 7) - Phase CH1q - Natural force (ADD_RENA_SYSTEM)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1q` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1p review (Open decision 1 answered 2026-11-07: (a)). One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*`, `X2Gage*.*`, `X2DamageManager.*`, `X2DamageEffect.cpp`, `X2EffectSet.*`, `X2GUNPC.*`, `CommonPacket.h` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (+ `_NF_DEBUG`; defined only with `_LIVE`) |
| Log tag | none (nothing runs until `_LIVE`); `[JUNE_CHAR]` (`RENA NF`) from CH1 final |
| Review | `kickoffs/char-rena/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Open decisions (for the user)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1q - Done" "CH1q - Review"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1q - Done (2026-10-27)" "CH1q - Review (2026-10-27)"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and `june2017_re/char_rena/ch0_nf.tsv`. Raven's CH1q (rage)
and Eve's Nasod Core are the worked examples.

## Task

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

- **Functions: 27** (`awk -F'\t' '$7=="CH1q"' june2017_re/char_rena/ch0_class_map.tsv`; inside 25, new 2).
  June addresses: `32c6ac20` `32c6b280` `32c6c130` `32c6d4a0` `32c6df30` `32c6e120` `32c6e590` `32c7fdf0` `32c85200` `32c85260` `32c85290` `32c858c0` `32c86740` `32c87780` `32c883d0` `32c89200` `32c89230`(new ?) `32c8b2c0` `32c9f7a0` `32c9f830` `32c9fa50` `32ca7060` `32ca7130` `32cb50c0` `32cb50e0` `32cb5d10`(new ?) `32cb6290`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §2 L118 "NATURAL_FORCE_TYPE x77 in Lire_ElvenRanger, ""unsupported (parsed, lo; §3 L149 "Natural force: ADD_RENA_SYSTEM is off under US_SERVICE; ""a decision ; §7 L258 death-prevention pre-apply: gauge snapshot overrides (vslot 0x740 / 0x; §7an L278 Spec A / Spec B readers (l.278): Rena 32ca7130, 32cb6290 (spec A), 32c.
- **Map: `june2017_re/char_rena/ch0_nf.tsv`** (225 rows, col 8 owner: 99 CH1q, 27 CH1q2, 24 absent in June; the rest are 2014 combo-revamp state-flow arms owned by the state phases). Class functions (27, `awk -F'\t' '$7=="CH1q"'` on the class map): every June row whose March function exists only under `ADD_RENA_SYSTEM` goes inside that function (col 4 names it), June's body under `_LIVE`; the one-file probe defines `_LIVE`, `_NF` and `ADD_RENA_SYSTEM`. June differs: NATURAL_FORCE_VALUE_PER_HIT 3.0 (@32ca73fd; March 6), no buff threshold of 3 (SetNaturalForceBuff `32c85200` stacks BFI 355 per point), ConsumeNaturalForce `32c9f7a0` also restores MP (n x float +0x97e0), UpNaturalForce `32cb6290` adds 1 more when a roll beats the protected float +0x6c50, AdjustDamageDataBeforeDamageReact is empty. GetNaturalForceCount / IsEmpty / UpNaturalForce are `__forceinline` in March but out of line in June (engine callers in CH1q2): drop the inline. Flipped arms: `X2DamageEffect.cpp:5217` would drop `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`'s child sequence (June `31f85910 @31f860f2-31f8614d` does the first-attack copy AND the children): keep both; the other arms (`X2DamageManager` `m_NaturalForceType` / `m_iNaturalForceType` DD+0x56c, `X2GUUser.h:1410`, `X2GUUser.cpp:10893`, `X2GUNPC.cpp:11404`) are equivalent; pick the flag-on arm. The studio "NF:" overlay (`X2GUUser.cpp:29719`) and fill key (`X2Game.cpp:5855`) are absent in June: keep them out of the build. `FriendshipNatureData` is not NF (CH1h).

- **Written by CH1h under `_NF` (2026-11-09), do not repeat:** the destructor's natural force lines ( charge 0, `SetNaturalForceBuff( false )`, stop the NF effect set ), ReInit's natural force block ( charge 0, `m_CannonBallCount` sync, `SetNaturalForceBuff( 1 <= count )`, `SetNaturalForceEffect`, foot attack box x1.3, save-state skill id, relate-data reset ) and InitComponent's PvP `SetNaturalForceCount( 0 )` ( `@32c895f4` ). CH1q checks `SetNaturalForceBuff` itself ( June `32c85200` stacks BFI 355 per point, so the bool argument only means "count >= 1" ).

- **Amended by CH1k (2026-11-09).** Two June constructor stores have no member yet: `+0x973c` = 7 ( int, @32ca38f0 ) and the protected float `+0x9768` = 5.5 ( triple, @32ca3a5f ), both inside the natural-force group `+0x9738`-`+0x9768`. Declare them when the group's readers are ported.
