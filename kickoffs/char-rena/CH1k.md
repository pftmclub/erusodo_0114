# June character Rena (Stage 7) - Phase CH1k - Class core: skill-use overrides and ReservedSkillStart

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1k` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1j review. One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1k-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1k - Done" "CH1k - Review"
awk -F'\t' '$7=="CH1k"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1k` (skill-use overrides from 0x7f0 (the seven `CX2GUUser` skill-use helpers are called, never copied), SpecialAttackEventProcess `32c6a970` (uncarved) with its parts `32cac200` / `32ca50b0` / `32c89a20`, 0x810 / 0x824 / 0x834 (uncarved), and **`ReservedSkillStart`**) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GULire_ElvenRanger.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A March block under `ADD_RENA_SYSTEM` inside a function you transcribe stays as it is (dead) until CH1q: the June body's natural-force lines go under
  `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (an `#ifdef` inside the `_LIVE` body), never under the studio macro.
- A class-range callee (`0x32c6a8d0`-`0x32cb6c6d`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py rena <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GULire_ElvenRanger.*` are CP949 (`.cpp` mixed endings, mostly LF; `.h` mostly LF):
  `bytepatch.py` only; check each `#ifdef` branch with `cl /P` (`ADD_RENA_SYSTEM`, `SERV_RENA_NIGHT_WATCHER`, `SERV_TRAPPING_RANGER_TEST`, ...).
- `ReservedSkillStart` (user, 2026-11-05): `SpecialAttackCooldown( skillID, m_fReservedSkillMp )` then `SkillMemoMpRefund( ... )`, and the base hook
  chain in `X2GUUser.h` / `.cpp` widened with Rena's `_LIVE` (copy Raven's). CH1 final's play-test casts each skill twice.
- Done gate: every class-map row with phase `CH1k` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

## Amended by CH0 (2026-11-07)

- **Functions: 21** (`awk -F'\t' '$7=="CH1k"' june2017_re/char_rena/ch0_class_map.tsv`; inside 8, new 13).
  June addresses: `32c6a970` `32c6d480`(new ?) `32c6d700`(new ?) `32c6d760`(new ?) `32c6d790`(new ?) `32c83320`(new ?) `32c85a60`(new IsNotCommonMoveState) `32c85c00`(new ?) `32c86af0` `32c87ac0` `32c89a20` `32c9ac00`(new ?) `32c9af70`(new ?) `32ca2af0`(new ?) `32ca2b60` `32ca50b0` `32ca5160` `32cac200` `32cb5b70`(new ?) `32cb5cb0`(new ?) `32cb5ee0`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L262 quick-slot special-item prefix at the top of SpecialAttackEventProcess; §7 L263 dev free-skill gate around the cool-time / MP / hyper-stone block of t; §7 L253 class skill-use function sets the cooldown then calls ApplySkillCoolTi; §7 L265 riding-key skill wait after CheckSkillUsePVPOfficial in the 510 lookup; §7 L262 every class's vslot 510 lookup (the Rena-named half: Phase 7d put the ; §7ai L862 JuneShowSkillNameStateString callers; §7al L898 new CX2GUUser hooks for every class: 0x654, 0x850, 0x860, 512, UpdateE; §7ap L987 June slots CH1o2 gave a base: Lire 0x6a8 32c7bed0 (skill use 32cac5a0); §7av L1045 class state animation rate, user vslot 0x824: Lire's inline SI_P_LCR_A.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1k`): 1; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- The ctor `32ca2b60` (vtable store `32ca2bc4`; caller 3230f289, the unit factory) and `32cb5ee0` (member init @32ca3c13). SpecialAttackEventProcess `32c6a970` (uncarved) with parts `32cac200` (7, l.262) / `32ca50b0` / `32c89a20`; 0x810 `32c87ac0`, 0x824 `32c83320` (7av, the Agile Movement animation rate), 0x834 `32c86af0`; plus `ReservedSkillStart` (Rules).
