# June census V0 - function inventory

Program checked first: imagebase `0x31aa0000` (segments `.text 31aa1000-337b9fff` ... `.SCY`), `unpacked_x2.exe` = 43,801,600 B (matches
`june2017_re/README.md`). Ghidra was read-only: no rename, comment or retype. Reproduce: `python -I june2017_re/census/v0_export.py`.

## Counts (`functions.tsv`, 55,556 rows)

| | functions | share |
|---|---|---|
| total | 55,556 | 100% |
| with a source file | 358 (54 distinct files) | 0.64% |
| with a class | 890 (87 distinct names) | 1.60% |
| with both | 276 | 0.50% |
| with either | 972 | 1.75% |
| with neither | 54,584 | 98.25% |
| Ghidra name not `FUN_xxxxxxxx` | 1,673 | 3.0% |

Total equals Ghidra's own count: `list_functions` = `methods` = 55,556 lines, no duplicate `june_va`. The plan text says 55,555: that
was one short (a `wc -l` on output with no trailing newline).

Time: whole export, including 57 + 873 `xrefs_to` calls, took 10 s over HTTP. No Ghidra script is needed.

## Method

1. `list_functions` (name + start VA); `size_approx` = gap to the next function start (approximate, includes padding and data).
2. Source file: strings holding a `X:\...\Name.cpp|.h|.c` path (ascii `strings` listing plus wide strings from `data`), `xrefs_to` each,
   the containing function (greatest start <= xref address) gets the file. 57 path strings were found.
3. Class: strings of the form `Class::Method` (`CX2EffectSet::OnFrameMove_CreateEffect`), same xref step; 873 strings used.
4. `evidence` = the string addresses used (`path@` / `str@`), at most four.

## Blind spots (V1 must not read the 98% as "library code")

- The binary has almost no RTTI: Ghidra holds 2 `vftable` labels and 1,113 `.?A` type names, nearly all `lua_tinker` template
  names. No vtable / class recovery is possible from the HTTP listings, so a class is known only where a log string names it.
- Only ~100 `__FILEW__` paths exist; most gameplay functions carry no string, so they have neither column.
- A class or file string seen in a function says the function uses it, not that the function belongs to that class (a helper
  that logs another class's name is tagged with it). Multiple values are joined with `;`.
- `ClassTraits` / `Instances` / `InstanceTraits` / `Classes` / `KSession` (the top class names) are Scaleform / networking library
  code; `Session.h` (200) and `libxml/*.c` are library files. The studio-code file hits are X2Lib / KTDXLIB / X2 paths only.
- 1,673 non-`FUN_` Ghidra names come from imports, thunks and project work, not from a census of the studio's classes.
- V1 has to attribute the rest by other evidence: `funcmap.tsv` joins, call-graph neighbours of attributed functions, address
  ranges (the studio's classes sit in contiguous link-order blocks), and the March tree's own function list.
