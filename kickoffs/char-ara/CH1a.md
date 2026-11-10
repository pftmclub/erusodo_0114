# June character Ara (Stage 7) - Phase CH1a - Class core: movement and slot-head states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH1a` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), char-rena Z, the previous CH1 sub-phase. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own: the CH1b review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1a - Done" "CH1a - Review"   # the worked example; pull the matching Rena sub-phase too
awk -F'\t' '$7=="CH1a"' june2017_re/char_ara/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Transcribe every function the class map gives phase `CH1a` (wait (2) + wait-state-change, walk, jump (7), double jump (3), throw item, common buff, hyper (2) + hyper wait, charge, airial dash (`ASI_AIRIAL_DASH`, `APPLY_MOVEMENT_SPEED`, checklist l.516-521)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c`
  of `X2GUAra.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE`, compiled in every build, referenced by nothing yet. Leave
  `InitState*` alone (CH1m registers).
- Force-power lines that call a June piece CH1q has not written go under `SERV_IRUHADEV_JUNE_CHAR_ARA_FP`, Specter / tail lines under
  `SERV_IRUHADEV_JUNE_CHAR_ARA_SPECTER` (an `#ifdef` inside the `_LIVE` body); March's live force-power calls stay March's until CH1q.
- A class-range callee (`0x32b90b70`-`0x32bd86e0`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is
  called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py ara <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GUAra.*` are CP949 (mostly CRLF): `bytepatch.py` only; check each
  `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with this phase is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
  with `_LIVE` undefined (one-file probe build, no full build: no play-test) and the one-file probe passes with it defined; `junediverge.py --check`
  and `enumscan.py --check` on the class files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 30** (`awk -F'\t' '$7=="CH1a"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 30).
  June addresses: `32b90e10` `32b90e40` `32b90e60` `32b90f20` `32b90f80` `32b90fb0` `32b90fd0` `32b91160` `32b96d80` `32b96db0` `32b97880` `32b97900` `32b979f0` `32b9a0b0` `32b9a1e0` `32b9a240` `32ba6fd0` `32ba73c0` `32ba7820` `32ba79b0` `32ba7a40` `32baa1c0` `32baa1f0` `32babee0` `32bb2d40` `32bb2ff0` `32bb3290` `32bb34d0` `32bb4200` `32bb43a0`.
- **States: 23** (`awk -F'\t' '$8=="CH1a"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_WAIT_STATE_CHANGE`, `ASI_WAIT`, `ASI_HYPER_WAIT`, `ASI_WALK`, `ASI_JUMP_READY`, `ASI_JUMP_UP`, `ASI_JUMP_DOWN`, `ASI_JUMP_LANDING`, `ASI_HYPER_MODE`, `ASI_ASD_HYPER_MODE`, `ASI_AYR_HYPER_MODE`, `ASI_AA_HYPER_MODE`, `ASI_BERSERK_MODE`, `ASI_ASD_BERSERK_MODE`, `ASI_AYR_BERSERK_MODE`, `ASI_AA_BERSERK_MODE`, `ASI_COMMON_BUFF`, `ASI_THROW_ITEM`, `ASI_AIRIAL_DASH`, `ASI_DOUBLE_JUMP_FRONT`, `ASI_DOUBLE_JUMP_UP`, `ASI_JUMP_ATTACK_Z`, `ASI_JUMP_ATTACK_X`.
- **Skills: 1** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 900 SI_ETC_WS_COMMON_LOVE.
- **Checklist rows: 4** (`awk -F'\t' '$5 ~ /(^| )CH1a( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S; L428 USI_WAIT_STATE_CHANGE registered in every class InitState; L516-L521 Ara's APPLY_MOVEMENT_SPEED = TRUE in ASI_AIRIAL_DASH (Ara_Ma; L713-L745 an id of a listed type with no case plays the class's first .
- **Force-power rows: 5** (`awk -F'\t' '$7 ~ /(^| )CH1a( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: unchecked 5).
- Hyper and berserk tables: the four berserk tables (`ASI_BERSERK_MODE` and the ASD / AYR / AA variants) are CH1a like the hyper tables; their registration is CH1m. `ASI_JUMP_ATTACK_Z` / `_X` are CH1a by the `JUMP*` prefix.
- Force-power lines in these states (`FP_CANCEL`, `ADD_FORCE_POWER`, the shared anim-end helper `32babba0`) go under `_FP` inside the `_LIVE` body: CH1q writes the helpers (`ch0_fp.tsv`).
