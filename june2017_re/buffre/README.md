# buffre: June 2017 buff-engine RE helpers (Buff port Phase 1)

Run from this directory. `gh.py` talks to the GhidraMCP HTTP server (read-only calls only)
and caches decompiles/disassemblies in `dec/` (gitignored).

| Script | What it does |
|---|---|
| `gh.py` | Ghidra client: `dec(addr)`, `dis(addr)`, `xto(addr)`, `get(endpoint, ...)` |
| `vt.py PATTERN OUT.json` | RTTI TypeDescriptor -> CompleteObjectLocator -> vftable for every class whose RTTI name contains PATTERN (e.g. `CX2Buff`, `CX2GU`). Stops each vtable at the next COL, because `.text` is merged with rdata here |
| `jt.py` | Sweeps `.text` for `jmp [reg*4+tbl]` jump tables and prints the bound/offset preamble (find a switch by its case range) |
| `swdump.py JMPVA BASE` | Dumps one jump-table switch: case value -> target, plus each target's `lea ecx`/`call`/member accesses. BASE = the `add eax,-N` before the bound (pass N) |
| `dispxref.py DISP...` | Finds every instruction whose memory displacement **or immediate** equals DISP. Keep EBP-based hits: EBP is a general register in this binary |
| `rawfn.py ADDR` | Capstone disassembly of a function Ghidra has not carved (stops at the last `ret`) |
| `compact.py ADDR` | Decompile with the XOR-string init noise stripped, prefixed with the decrypted keys (`xorstrings.tsv`) in use order |
| `casemap2.py ADDR ENUMTABLE` | Case -> call map for a decompiled switch, with June enum names |

Method notes (protected-float members at `M+0xa0`, the `+0x1a0` sub-object behind
`DamageData::optrAttackerGameUnit`, decompiler mislabelling hidden-ECX pushes) are in
`JUNE_BUFF_PORT_PHASES.md`, section "How to read June's stores".
