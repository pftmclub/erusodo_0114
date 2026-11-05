# June character Elsword (Stage 7) - Phase CH4a2 - Engine readers of Elsword's passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4a2` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4a review. Touches `X2GUUser.*` (state parse, CommonFrameMove): not beside CH4b / CH4c, which touch it too |
| Flag | new `SERV_IRUHADEV_JUNE_USER_CREATE_EFFECT_SET` (class-agnostic: Elesis writes the rate too) (+ `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE_DEBUG` if a log is needed) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE`) |
| Review | `kickoffs/char-elsword/CH4a2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "CH4a - Done (2026-11-05)"
```
Then checklist §7ao's Elsword row and `june2017_re/transcend/t0_readers.tsv` rows 11 and 14.

## Task (found by CH4a, 2026-11-05; plan subject: these are the only readers of Wind Slicer 21402, Fight God 21416 and Rune Slayer Open Mind 21417)

1. **The state key `CREATE_EFFECT_SET`** ( `{ name, time, y offset }` ): June's user state-data parser `32057b80` @`320606b7`-`32060736` reads it into user
   `+0x8e2c` ( wstring ), `+0x8e48` ( time ), `+0x8e4c` ( y offset ); find where June clears it per state ( `3203a620` @`3203b66f` writes `+0x8e2c` ). Elsword's
   class script uses it in many states ( `Elsword_SwordMan.src.lua` l.5448 ... `EffectSet_SI_P_ELK_WIND_SLICER` ); grep the other June class scripts.
2. **The play** in `CX2GUUser::CommonFrameMove` `32063650` @`32063a6c`-`32063d06`: name set, `GetRandomFloat() < +0x8e50 + bonus`, the event time reached
   ( `31e4c230` on the now anim ), then the effect set at the position + y offset. The bonus is `+0x8e50` again when the unit has BTI 18 ( `HaveThisBuff( 0x12 )` )
   and Fight God 21416 `RNSI_P_ELK_FIGHT_GOD` is learned ( `( id, 1, 0, 1 )` ), hyper-class and the unit transcended ( `+0x3c == 1` ).
   `m_fCreateEffectSetRate` ( June `+0x8e50` ) exists since CH4a ( `X2GUUser.h`, under `_PASSIVE` ): move it under the new flag.
3. **`32610cb0`** ( 21417 `RNSI_P_ERS_OPEN_MIND`: learned, hyper-class, transcended; no Ghidra function, body `32610cf0`-`32610d4f` ): find its caller and port the branch.
4. Who else writes `+0x8e50` ( Elesis `32b74500` @`32b7471b`, `32b89260` @`32b8930d` ): route to Elesis' Stage 7 port, do not port.

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps
(x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark checklist §2 L124 / §7ao's Elsword rows `(Elsword: CH4a2)`.
Done gate: Lord Knight's Wind Slicer effect set plays on his attacks at the logged rate; a transcended Lord Knight with Fight God plays it about twice as often.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
