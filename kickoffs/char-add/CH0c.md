# June character Add (Stage 7) - Phase CH0c - Add on the creation screen (March classes 9 / 32 / 122)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH0c` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments); char-ara CHNa review (the creation state is shared: one character at a time); not Add's class code (it plays on his March kit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_CREATE` (+ `..._CREATE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD CREATE`) |
| Review | `kickoffs/char-add/CH0c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Context" "Rules" "Phase CH0c - Add on the creation screen (March classes)" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md "CH0 - Done"
cat june2017_re/char_add/ch0_create.tsv
cat kickoffs/char-luciel/NU4.md   # June's create-unit functions
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CHNa - Done"   # the creation-screen edits that landed before yours
```

## Task

Make Add creatable, the way June shows him (user, 2026-11-11: "Add right now doesn't have any entry"). Today `SERV_LIMIT_UNIT`'s US limit
(`_CONST_LIMIT_UNIT_::iLimitType = 8`, `ServerDefine_Global.h:1190`) hides `AddCreateUnitSlot` (`X2StateCreateUnit.cpp:132-135`); `SelectAdd()`
(`:2346`), the entry switch's `UT_ADD` arm (`:207-209`) and the class-9 beginner gear (`X2OfflineInventory.cpp:1236`, "unreachable") already exist.
- Lift the limit for `UT_ADD` at the client's use sites (`:106-136`, `:173-230` and every other `iLimitType` reader in `ch0_create.tsv`) under
  `_CREATE`, March's code in `#else`. Never edit `ServerDefine_Global.h`.
- June's `SelectAdd` `32555320` inside March's, and every June function CH0 mapped for his entry (portrait, name / description pictures, stand mesh
  and lobby animation, voice, background, the class-change info dialog), each inside March's namesake or new under `_CREATE`.
- June's `DLG_Unit_Create_ClassChange_Button_Add.lua` (`adopt-june`, `luabind.py check`): the 9 / 32 / 122 previews and `Movie_Create_Add_*`; the four
  June-only buttons harmless before CHNa (check `SelectUpperClass`; if a click is not harmless, hide them under `_CREATE` until CHNa). The slot row
  in `DLG_Unit_Create_Unit_Button_New.lua` only if June's differs (core-list rule; Lu/Ciel NU4 owns the file's other slots).
- The emulator's create path for class 9 (create-unit handler; `SeedBeginnerGear`'s row becomes reachable: fix its comment); `createchar.py` needs no
  change for 9 / 32 / 122.
- **Stat rows for 9 / 32 / 122** (Open decision 1, answered: derive) from CH0 item 12 into `KncWX2Server/ServerResource/US/StatTable.lua`, a labelled
  stand-in; name the file in the hand-off (the user XORs and packs it into data036); until then the loader's fallback, logged.
- Stage the creation-screen assets CH0 lists (byte-exact, pack, copy; CLAUDE.md 2026-11-10: no ask). Save a pre-CH0c ErrorLog first.
- Full build and deploy; play-test: open the creation screen, Add's slot shows, his preview, voice and class buttons (Psychic Tracer, Lunatic Psyker)
  play June's, create an Add (any free nickname), he is listed on the select screen with his beginner gear, enters the village and a dungeon, survives
  a relog; every other character's slot is unchanged. Hand-off: dev console first, per check where / paste / look for / if not.

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
