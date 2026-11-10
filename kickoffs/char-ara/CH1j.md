# June character Ara (Stage 7) - Phase CH1j - Class core: Common* overrides

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH1j` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), char-rena Z, the previous CH1 sub-phase. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own: the CH1k review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1a - Done" "CH1a - Review"   # the worked example; pull the matching Rena sub-phase too
awk -F'\t' '$7=="CH1j"' june2017_re/char_ara/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Transcribe every function the class map gives phase `CH1j` (Common* overrides 0x7c0-0x7e8 (CommonStateStartFuture `32bc8380`, CommonStateStart `32bd2a40`, CommonFrameMoveFuture `32bb0bf0`, CommonFrameMove `32bce2e0`, CommonStateEndFuture `32b90be0` / CommonStateEnd `32b90bf0` uncarved, CommonRender_Prepare `32b90c00`) and the helpers only they reach (`32bd6de0` from `32bb0bf0`, funcmap l.5818)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

- **Functions: 19** (`awk -F'\t' '$7=="CH1j"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 9, new 10).
  June addresses: `32b90be0` `32b90bf0` `32b90c00` `32b95a00`(new IsSkillAffectedByHellGateOpen) `32b95b00`(new IsSkillAffectedByLoadOfAsura) `32b95b30`(new IsSkillAffectedByFoxSmile) `32ba7b80`(new GetPassiveSkillRate) `32ba7cf0`(new UpdateOpenMindBuffByForcePower) `32ba7da0`(new UpdateValorDevotionTimer) `32ba7ec0`(new GetNowSkillPassiveRate) `32baa0d0` `32bb0bf0` `32bc13c0`(new CreateTranscendenceDamageEffectByTimer) `32bc5ea0`(new StopKillEnemyEffectSet) `32bc6aa0`(new CleanupSkillFPGainTable) `32bc8380` `32bce2e0` `32bd2a40` `32bd7280`.
- **Checklist rows: 1** (`awk -F'\t' '$5 ~ /(^| )CH1j( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L184 class keys ADD_FORCE_POWER x9, USE_FORCE_POWER x26, FP_CANCE.
- **Force-power rows: 4** (`awk -F'\t' '$7 ~ /(^| )CH1j( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 2, june-only 1, same 1).
- **Specter / tail rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH1j( |$)/' june2017_re/char_ara/ch0_specter.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **Identified callees first needed here: 4** (`awk -F'\t' '$8 ~ /(^| )CH1j( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `3207ff00` `32e87810` `32feae90` `33048830`.
