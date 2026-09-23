#!/bin/sh
# Rebuild the gitignored files in june2017_re/ from a running Ghidra instance.
# Needs: Ghidra open on els_jun_2017.gpr with the GhidraMCP HTTP server on 127.0.0.1:8080.
# Usage: ./regen.sh [extra_address ...]   (run from june2017_re/)
#
# Region survey (functions >= 1500 bytes) from funcs.txt:
#   python -c "import re;fs=sorted((int(m.group(2),16),m.group(1)) for m in (re.match(r'(\S+) at ([0-9a-fA-F]+)',l) for l in open('funcs.txt',encoding='latin1')) if m);[print('%08x %6d %s'%(a,fs[i+1][0]-a,n)) for i,(a,n) in enumerate(fs[:-1]) if 0x31f50000<=a<0x31f90000 and fs[i+1][0]-a>=1500]"

G="${GHIDRA:-http://127.0.0.1:8080}"
# Every curl below quotes --noproxy '*'. Unquoted, the shell globs it and Ghidra is never reached.

cd "$(dirname "$0")" || exit 1
curl -s --noproxy '*' -m 10 "$G/methods?limit=1" >/dev/null || { echo "Ghidra server not reachable at $G" >&2; exit 1; }

curl -s --noproxy '*' -m 600 "$G/list_functions" > funcs.txt
curl -s --noproxy '*' -m 600 -G "$G/methods" --data-urlencode "limit=100000" > methods.txt
echo "funcs.txt: $(wc -l < funcs.txt) lines"

mkdir -p ghidra
for a in \
    0x31f7aab0 0x321e48e0 0x321ee600 0x321f28f0 0x321e4020 0x321e0080 0x321dbcc0 \
    0x32632390 0x31f6ac60 0x31f68f30 0x31f66700 0x31f66e60 0x31f71930 0x31f73f30 \
    0x31f74a80 0x31f76a90 0x31f6ceb0 0x31f637e0 0x31f64200 0x31f7fc60 0x31f7ff60 \
    0x31f85910 0x31f83aa0 0x31c895d0 0x31c8d140 "$@"
do
    curl -s --noproxy '*' -m 300 -G "$G/disassemble_function" --data-urlencode "address=$a" > "ghidra/dis_$a.txt"
    echo "ghidra/dis_$a.txt: $(wc -l < "ghidra/dis_$a.txt") lines"
done

# Decompiled June lua. Copy first: decompile-batch writes siblings next to its input, and
# the 14JUN17 directory should stay pristine.
JUNE="F:/elsword stuff/elsword_2014/els_2014/237311/14JUN17/data/data036_kom"
mkdir -p june_lua
for f in "$JUNE"/DamageEffect*.lua "$JUNE"/EffectSet*.lua "$JUNE"/UIEffectSet*.lua \
         "$JUNE"/UnitViewerEffectSetTable.lua "$JUNE"/Elsword_DamageData.lua \
         "$JUNE"/DamageSound.lua "$JUNE"/Enum.lua
do
    case "$f" in *.src.lua) continue ;; esac
    cp "$f" june_lua/
    ../elstools/elstools.exe decompile-batch -i "june_lua/$(basename "$f")"
done
echo "june_lua: $(ls june_lua/*.src.lua 2>/dev/null | wc -l) decompiled files"
