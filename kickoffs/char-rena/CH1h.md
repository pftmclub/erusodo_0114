# June character Rena (Stage 7) - Phase CH1h - Class core: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1h` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1g3 review (CH0 split). One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1h-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1h - Done" "CH1h - Review"
awk -F'\t' '$7=="CH1h"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1h` (overrides below vslot 0x600 (lifecycle, render, reaction virtuals; DamageReact `32c6da50` uncarved; ctor `32ca2b60` / dtor `32ca42f0` offset map; InitComponent `32c895b0` with 7c's alpha sites; the uncarved ones read from the listing)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- 7c: the 8 alpha-tag sites (`c10a_alpha_sites.tsv` l.206-213: 6 in InitComponent `32c895b0`, `32c893e5`, `32c89535`) are written here.
- Done gate: every class-map row with phase `CH1h` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 33** (`awk -F'\t' '$7=="CH1h"' june2017_re/char_rena/ch0_class_map.tsv`; inside 18, new 15).
  June addresses: `32c6a8d0` `32c6d870`(new ?) `32c6da50` `32c6db70` `32c7fa40` `32c834f0` `32c835c0` `32c849f0` `32c85d50`(new InitDevice) `32c870c0` `32c89270`(new ?) `32c89310`(new ?) `32c89460`(new ?) `32c895b0`(new InitComponent) `32c95670`(new ?) `32c99a40` `32c9b320`(new ?) `32c9b3d0` `32c9e130`(new ?) `32ca42f0` `32ca4a40` `32ca4e10` `32ca5010` `32ca7860` `32ca9ca0`(new ?) `32cb3200`(new ?) `32cb34e0`(new ?) `32cb3500`(new ?) `32cb4200`(new ?) `32cb4270`(new ?) `32cb6a20` `32cb6a30` `32cb6c50`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7c L357 Effect-alpha tags: 8 sites 32c89310, 32c89460, 32c895b0 (32296da0 hand; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call; §7ao L969 transcend: Rena hyper effect start 32c9b3d0 (vslot 49) returns at once.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1h`): 3; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- DamageReact `32c6da50` (uncarved), Lose `32c834f0`, HyperModeBuffEffectStart `32c9b3d0` (uncarved; the transcended return is CH3d's, 7ao), CreateMajor / MinorParticleArray `32ca4e10` / `32ca5010` (uncarved; alpha sites `32c893e5` in `32c89310`, `32c89535` in `32c89460`) and InitComponent `32c895b0` (6 alpha sites, 7c; GT_PVP NF reset `@32c895f4` is CH1q's line under `_NF`). `FriendshipNatureData` virtuals `32cb3500` / `32cb4200` / `32cb4270` (vtable `3351666c`; its COL reads `TimeLimited`; March `X2GULire_ElvenRanger.h:265`, live). Init `32c883d0` (0x40), Set/UnSetSpecificValueByBuffTempletID (0x24c / 0x250), AdjustDamageDataBeforeDamageReact (0x300), AttackResultByType (0x4e0), CreateGageData (0xd0), Check / DeleteDamageRelateSkillData (0x420 / 0x424) are **CH1q's**: March declares them only under `ADD_RENA_SYSTEM` (`.h:536-567`).
