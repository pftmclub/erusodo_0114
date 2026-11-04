# June character Elsword (Stage 7) - Phase CH1q2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1q2 review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1q2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (`//#define` until CH1 final), `_WOS_DEBUG` (defined; its lines sit in `_WOS` code), `_LIVE` (the InitEffect call site) |
| Log tag | none (nothing runs until `_LIVE` / `_WOS`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1q2 - Done (2026-11-04)"
```
Then the CH1q2 rows of `june2017_re/funcmap.tsv` (`grep "Char Elsword CH1q2"`) and of `tools/data/june_march_divergent.tsv` (`grep "CH1q2"`), and the
`gagedata` / `gageui` / `gagemanager` rows and rows 91 / 92 / 109 of `june2017_re/char_elsword/ch0_wos.tsv`.

## Task

Check CH1q2 against June (read-only in Ghidra and the exe bytes). In order of risk:
- **The skill-slot B test** (`322cd5c0 @322cd7eb-322cd856`): `3214f3d0( i )` with `i` the combined 0-9 index, the order of the four tests, key 0 then key 1 on
  my unit's skill tree (`g_pData +0x68 +0x90 +0x494`), both statics hidden with no break; is `IsTranscendenceSlotIndex( i )` with `bSlotB` the same set as
  `== 3` with `bSlotB`? The rest of the function (gate on types 1 / 8, observer test, 10 slots, `+0x160` WOS type, flicker 0.4 / 1.0 / 0.1) = March.
- **The refresh virtual**: slot 0x5c of the base `CX2MyGageUI` vtable `3353336c` is `31f3f070` (`ret`), Elsword `32e681d0`, Elesis `32e69e70`; InitEffect
  `32d0d540 @32d0d596-32d0d5ba` is inside the own-unit test, so `GetMyGageUI()` is Elsword's UI there. Is a base virtual right, and is CH1h's old
  stand-in fully gone?
- **Gauge data**: `InitWhenGameIsOver` `32a348d0` = base only (`32a33f90` is the same two instructions); `CopyGageData` `32a34930` really forces the
  changed flag (`@32a34973`, CH0's note was wrong); Update / ToPacket / FromPacket / Clone / SetState = March.
- **Gauge UI**: `InitUI` `32e6d830` order; `UpdateUIElswordWSP` `32e67df0` entry tests `+0x60` then `+0x5c`, the case-0 thresholds (`test ah,0x41` / `test ah,1`
  @32e67f5f / @32e67f7b), the final state test 0xe / 0xf (0xf = June `XS_GUILD_AGIT_GAME`, absent in March: comment only); `UIFrameMoveElswordWSP`,
  `SetShow`, `SetShowWspParticle`, `OnFrameMove`, `StopUIEffectElswordWS`, both Play functions (position = picture 4 centre, 0.5 at `3342ebc8`) = March.
- **The two "no code" calls**: `CreateMyGageUI` `3248e290` Elsword cases; the Vigor x0.7 in `UpdateSkillCoolTime` `322e07e0` (double 0.7 at `3344af88`
  against March's `0.7f`: one ulp at most, kept March's; agree or not).
- **Routing**: `322ed400` is the training-game F7 key, not a class-switch randomiser: its bullet in `JUNE_PORT_BACKLOG_ORDER.md` Stage 2; the Elesis
  findings in `kickoffs/june-backlog/stage7-plan.md`.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.

Hygiene: `flagcheck` on `JUNE_CHAR_ELSWORD_WOS --with _WOS_DEBUG _LIVE` and on `_WOS_DEBUG` with `--range <CH1q2 commit>^..<CH1q2 commit> --files` the six
code files, `git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` the four `.cpp` files, one-file probes of `X2GageData.cpp`,
`X2GageUI.cpp`, `X2Game.cpp`, `X2GUElsword_SwordMan.cpp` with no flag, with `_WOS`, and with `_LIVE` + `_WOS`. Write "CH1q2 - Review" into the plan doc,
update the STATUS rows (CH1 becomes ready once CH0b's play-test is in), check `kickoffs/char-elsword/CH1.md` and commit with explicit pathspecs.
