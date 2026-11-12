# June character Elesis (Stage 7) - Phase CH1k - Class core: skill-use overrides and ReservedSkillStart

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elesis CH1k` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), the previous CH1 sub-phase (the char-ara Z gate was dropped, user 2026-11-12). One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elesis/CH1k-review.md` (Opus 5.5 high; the phase writes it when done); covers CH1h-CH1k and reads every `listing-only` function of those phases against the raw listing |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1k - Done" "CH1k - Review"   # the worked example (Ara's matching sub-phase)
awk -F'\t' '$7=="CH1k"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Transcribe every function the class map gives phase `CH1k` (SpecialAttackEventProcess 0x7f4 `32b4ee20` (uncarved), CanUseSpecialAttack 0x7f8 `32b84a50` (its `CheckSummonSpecialAttack` `3202daf0` call under `_SUMMON`), 0x7fc `32b52590`, 0x800 `32b73e60`, NoStateChangeActionFrameMove 0x80c `32b818b0` with Endurance Power `32b80c60` / `32b80840`, SpecialAttackNoStageChange 0x810 `32b72550`, IsValideSkillRidingOn 0x818 `32b4ef10`, IsNotCommonMoveState 0x828 `32b70290`, ShowActiveSkillCutInAndLightByScript 0x834 `32b70d10` -> `32b70af0`; the seven `CX2GUUser` skill-use helpers and their fields are called, never copied; and **`CX2GUEL::ReservedSkillStart`** (plan Rules: March has no override; `SpecialAttackCooldown( skillID, m_fReservedSkillMp )` then `SkillMemoMpRefund( ... )`, the base hook chain in `X2GUUser.h` / `.cpp` widened with her `_LIVE` flag, as Ara's)) from Ghidra and the raw listing, **whole**. The plan's "Transcription rules
(CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GUEL` function under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE`, March's body in `#else`, `#endif SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE`. Add `_LIVE` to
  `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of `X2GUEL.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` (the build's own
  flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*` alone
  (CH1m registers).
- **`listing-only` rows** (no Ghidra function): read with `python june2017_re/charport_adis.py elesis <va>` (Ghidra stays read-only). If the
  listing does not make a function's control flow plain, say "this needs Opus" for that function, list it in the Done note and go on with the rest.
- Way of the Sword lines that call a June piece CH1q has not written go under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_WOS`, summon lines under `SERV_IRUHADEV_JUNE_CHAR_ELESIS_SUMMON`, blood-hook lines under
  `SERV_IRUHADEV_JUNE_CHAR_ELESIS_BLOOD` (an `#ifdef` inside the `_LIVE` body); March's live Way of the Sword calls stay March's until CH1q.
- A class-range callee (`0x32b4e770`-`0x32b90b6f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee
  is called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GUEL.cpp` / `X2GUEl.h` are CP949 with mixed CRLF / LF: `bytepatch.py`
  only, each inserted line with the EOL of its neighbours; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with this phase is written (the Done note lists them with June addresses, `inside` / `new` and `carved` /
  `listing-only`); the build passes with `_LIVE` undefined (one-file probe build, no full build: no play-test) and the one-file probe passes with
  it defined; `junediverge.py --check` and `enumscan.py --check` on the class files PASS. Nothing to play-test: the hand-off says so and names the
  next phase.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).
