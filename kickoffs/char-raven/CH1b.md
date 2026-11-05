# June character Raven (Stage 7) - Phase CH1b - Class core: reactions

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1b` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1a review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1b - Done" "CH1b - Review"
awk -F'\t' '$7=="CH1b"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1b` (damage (21 `RSI_DAMAGE*`), stand-up (8), die, start, win / lose blocks, wall, amusement, frozen, transformed, parrying (2) unless CH0 gives it to CH1q) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GURaven.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A class-range callee (`0x32c25a20`-`0x32c6a63d`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py raven <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (`*=` where June multiplies; March assigns in the Shadow Step / Shadow Buster
  states, checklist §7 l.256).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- `X2GURaven.*` are CP949 (`.cpp` mixed endings, mostly LF; `.h` mostly LF): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1b` is written (the Done note lists them with June addresses and `inside` / `new`); the build
  passes with `_LIVE` undefined and the one-file probe passes with it defined; `junediverge.py --check` and `enumscan.py --check` on the class
  files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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

## Amended by CH0 (2026-11-05)

- **Functions: 40** (`awk -F'\t' '$7=="CH1b"' june2017_re/char_raven/ch0_class_map.tsv`; inside 36, new 4).
  June addresses: `32c25c50` `32c25c70` `32c25db0` `32c25df0` `32c25e30` `32c25e70` `32c25eb0` `32c25ec0` `32c25f00` `32c25f10` `32c266d0` `32c266e0` `32c26720` `32c26730` `32c28960` `32c289c0` `32c28a00` `32c28a60` `32c28bc0` `32c28c10` `32c28d90` `32c28dd0` `32c28f50` `32c290d0`(new RSI_DAMAGE_STANDUP_FRONT_StateStart) `32c29130`(new RSI_DAMAGE_STANDUP_FRONT_StateStartFuture) `32c29190` `32c29210` `32c29290` `32c29310` `32c29390` `32c29410` `32c30f30` `32c30fb0` `32c33bd0` `32c52480`(new ?) `32c55ee0` `32c55fe0` `32c59550`(new ?) `32c5b6f0` `32c5b710`.
- **States: 70** (`awk -F'\t' '$8=="CH1b"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_RVC_START`, `RSI_RVC_WIN`, `RSI_RVC_WIN2`, `RSI_RVC_LOSE`, `RSI_RVC_LOSE2`, `RSI_DIE_FRONT`, `RSI_DIE_BACK`, `RSI_START`, `RSI_START_INTRUDE`, `RSI_WIN`, `RSI_WIN2`, `RSI_LOSE`, `RSI_LOSE2`, `RSI_PARRYING`, `RSI_PARRYING_SMALL`, `RSI_DAMAGE_GROGGY`, `RSI_DAMAGE_SMALL_FRONT`, `RSI_DAMAGE_SMALL_BACK`, `RSI_DAMAGE_BIG_FRONT`, `RSI_DAMAGE_BIG_BACK`, `RSI_DAMAGE_DOWN_FRONT`, `RSI_DAMAGE_DOWN_BACK`, `RSI_DAMAGE_STANDUP_FRONT`, `RSI_DAMAGE_STANDUP_BACK`, `RSI_DAMAGE_BOUNDING_DOWN`, `RSI_DAMAGE_BOUNDING_LANDCRASH`, `RSI_DAMAGE_BOUNDING_UP`, `RSI_DAMAGE_AIR_SMALL`, `RSI_DAMAGE_AIR_DOWN`, `RSI_DAMAGE_AIR_DOWN_INVINCIBLE`, `RSI_DAMAGE_AIR_DOWN_LANDING`, `RSI_DAMAGE_AIR_FALL`, `RSI_DAMAGE_AIR_UP`, `RSI_DAMAGE_AIR_FLY_FRONT`, `RSI_DAMAGE_AIR_FLY_BACK`, `RSI_DAMAGE_REVENGE`, `RSI_STANDUP_ROLLING_FRONT_FRONT`, `RSI_STANDUP_ROLLING_FRONT_BACK`, `RSI_STANDUP_ROLLING_BACK_FRONT`, `RSI_STANDUP_ROLLING_BACK_BACK`, `RSI_STANDUP_ATTACK_FRONT`, `RSI_STANDUP_ATTACK_BACK`, `RSI_RST_START`, `RSI_ROT_START`, `RSI_RBM_START`, `RSI_RRF_START`, `RSI_RST_WIN`, `RSI_ROT_WIN`, `RSI_RBM_WIN`, `RSI_RRF_WIN`, `RSI_RST_WIN2`, `RSI_ROT_WIN2`, `RSI_RBM_WIN2`, `RSI_RRF_WIN2`, `RSI_RST_LOSE`, `RSI_ROT_LOSE`, `RSI_RBM_LOSE`, `RSI_RRF_LOSE`, `RSI_RST_LOSE2`, `RSI_ROT_LOSE2`, `RSI_RBM_LOSE2`, `RSI_RRF_LOSE2`, `RSI_RWT_START`, `RSI_RWT_WIN`, `RSI_RWT_WIN2`, `RSI_RWT_LOSE`, `RSI_RWT_LOSE2`, `RSI_STANDUP_ATTACK_FRONT_NO_DOWN`, `RSI_STANDUP_ATTACK_BACK_NO_DOWN`, `RSI_WALL_LANDING`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID).
- Parrying moved to **CH1q** with the Nasod core (its states call DoParrying / RestoreNasodBall). Die front / back Start `32c55ee0` / `32c55fe0` call `ClearNasodBall` `32c4f750` (CH1q: declare it, `body in CH1q`) and `32c52480` (clears the handle list +0x9638, here; also called by the dtor and CommonFrameMove). `RSI_TRANSFORMED` and `RSI_FROZEN` are dead data.
