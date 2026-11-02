# June character Lu/Ciel (Stage 8) - Phase NU0 - New-unit and two-body engine map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel NU0` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | - (read-only; beside CH0) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each NU / DU review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Context" "Planning evidence (2026-11-02)" "Phases" "Phase NU0 - New-unit and two-body engine map (read-only)" "Phase NU - New-character foundation (NU1-NU4)" "Phase DU - The two-body engine (DU1-DU5)"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 8: new characters (Lu/Ciel, Rose, Ain)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Phase CH-N - A June-only later class path (template; not run for Aisha)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do "Phase NU0" items 1-9. Ghidra is read-only. You are on Opus: subagents do the site greps, `cl /P` probes, the tsv joins and the
Ghidra dumps of the addresses the plan names; you keep June's verdict per site and the owner phase.

- The 60 per-type switches (item 2) are the bulk: record Rose's / Ain's June case in their column, but own only Lu/Ciel's.
- Item 4 covers Lu/Ciel's whole entry on the character-creation screen (user, 2026-11-02), not only the button: list every June function,
  dialog, texture, string, motion and sound it uses, and add the assets to CH0b's list.
- Item 3 decides NU2's size: count every member, serializer and cast, and say whether the layout change reaches `X2ServerProtocol`.
- Item 7 writes the DU4 migration shape (tables, columns, old -> new ids). The user approved June's numbering (2026-11-02).
- Fill the "Amended by NU0" sections of NU1-NU4, DU1-DU5, CHN and TU. Done note (at most ~60 lines) and funcmap rows `Char Luciel NU0`.

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
