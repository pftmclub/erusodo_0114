# June character Ara (Stage 7) - Phase CH1c - Class core: riding, pepper run, dance

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH1c` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), char-rena Z, the previous CH1 sub-phase. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own: the CH1d2 review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1a - Done" "CH1a - Review"   # the worked example; pull the matching Rena sub-phase too
awk -F'\t' '$7=="CH1c"' june2017_re/char_ara/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Transcribe every function the class map gives phase `CH1c` (riding (27 `ASI_RIDING*`), pepper run (5), dance (3)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

- **Functions: 3** (`awk -F'\t' '$7=="CH1c"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 3).
  June addresses: `32b90f30` `32bc0140` `32bc5ba0`.
- **States: 37** (`awk -F'\t' '$8=="CH1c"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_PEPPER_RUN_READY`, `ASI_PEPPER_RUN`, `ASI_PEPPER_RUN_END`, `ASI_PEPPER_RUN_JUMP_UP`, `ASI_PEPPER_RUN_JUMP_DOWN`, `ASI_AMUSEMENT_ITEM_PHOENIX_RUN`, `ASI_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `ASI_DANCE_Z`, `ASI_DANCE_X`, `ASI_DANCE_WAIT`, `ASI_RIDING_START`, `ASI_RIDING_ON`, `ASI_RIDING_OFF`, `ASI_RIDING_WAIT_HABIT`, `ASI_RIDING_WAIT`, `ASI_RIDING_WALK`, `ASI_RIDING_JUMP_UP`, `ASI_RIDING_JUMP_DOWN`, `ASI_RIDING_JUMP_LANDING`, `ASI_RIDING_DASH`, `ASI_RIDING_DASH_END`, `ASI_RIDING_DASH_JUMP`, `ASI_RIDING_DASH_JUMP_LANDING`, `ASI_RIDING_DAMAGE_FRONT`, `ASI_RIDING_DAMAGE_FRONT`, `ASI_RIDING_ATTACK_Z`, `ASI_RIDING_ATTACK_X`, `ASI_RIDING_JUMP_ATTACK_Z`, `ASI_RIDING_ATTACK_SPECIAL`, `ASI_RIDING_DIE`, `ASI_RIDING_SPECIAL_MOVE`, `ASI_RIDING_HYPER_MODE`, `ASI_RIDING_SPECIFIC`, `ASI_RIDING_ATTACK_X_READY`, `ASI_RIDING_ATTACK_X_READY_2`, `ASI_RIDING_ATTACK_X_END`, `ASI_RIDING_DAMAGE_BACK`.
- **Checklist rows: 1** (`awk -F'\t' '$5 ~ /(^| )CH1c( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L178 June-only members of tables March has (SKILL_ID names, ARA_S.
- June's script defines `ASI_RIDING_DAMAGE_FRONT` twice (the second wins) and June registers `ASI_RIDING_DAMAGE_BACK` with no table; `ASI_RIDING_JUMP_READY` is dead data. Keep verbatim; `JUNE_BUGS.md` row for the duplicate.
