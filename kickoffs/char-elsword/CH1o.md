# June character Elsword (Stage 7) - Phase CH1o - Virtuals and routed gaps

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1o` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time: `X2GUElsword_SwordMan.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1o - Done (2026-11-02)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

The virtuals under `_LIVE`; June-only and changed user virtuals Elsword overrides (CH0's class map, vtable `33522914`; berserk-by-chance
0xa4 is not overridden); the gaps the CH1a-CH1k reviews routed here. A user virtual whose base body June changed is replaced at its definition
(`june_march_divergent.tsv` row in the same commit).

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

## Amended by CH0 (2026-11-03)

- **Callees this Opus phase maps itself:** `322d2680` CX2Game::GetNearestNPCUnitUID( D3DXVECTOR3 pos, int npcUIDToExcept, float fMaxRa; `31f76580` none in March: CEffect main-effect scale modifier, ( int mode, float x ) -> 31f7.
- No class-map function carries CH1o after CH0's triage (the 9 unsettled helpers went to CH1h / CH1k / CH4a / CH4c / n/a). It owns two engine callees: replace `CX2Game::GetNearestNPCUnitUID` with June's `322d2680` (tsv row added by CH0; remove its allow row) and write June-only `31f76580` (main-effect scale ( mode, x ) -> vector (x,0,0): zeroes y / z, a `JUNE_BUGS.md` candidate) that CH2d's `32d16c00` calls.

## Amended by CH1h (2026-11-04)

- CH1h wrote the empty hooks `ChangeFinalDamageAsDefender` (vslot 0x508, `GetFinalDamage` @32284d43) and `ChangeFinalDamageAsAttacker` (0x50c, @32284d62) and `ApplyBuffToOpenMind` (0x510, `CX2GUUser::DamageReact` @320478c9), all `_CORE`, called by nothing: add the base virtuals and the call sites here (a user-attacker/defender test, two float buffers at the first two). `GetSpecialItemUsableExtraStateID` (`32d27130`) is already written under `_LIVE`.
- CH1h review: June's `ApplyBuffToOpenMind` `32d0c780` calls its empty base `31fc7ab0` (vslot 0x510) first; when you add the `CX2GUUser` virtual, add that base call at the top of Elsword's body too (`_CORE`).
