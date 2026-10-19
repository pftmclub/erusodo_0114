"""Build KTDXLIB/sfjpeg.lib: Scaleform's libjpeg 8d under private names, plus the three libgfx members that call it.

Why: the client links CxImage's libjpeg 6b (KTDXLIB/jpeg.lib, #pragma in KTDX.h). Scaleform's prebuilt libgfx.lib was built against
libjpeg 8d (JPEG_LIB_VERSION 80, a bigger jpeg_decompress_struct) and calls the same global names, so every JPEG inside a .swf
(DefineBitsJPEG2/3) hit 6b's "Wrong JPEG library version" and crashed the loader (june-gfx G3: Elysion7.swf). Both copies cannot be
linked under one name, and 6b cannot be dropped (CxImage still uses it). So the 8d copy is renamed.

What it does (same-length byte edit, nothing else changes):
  * S = every external symbol DEFINED in the SDK's libjpeg.lib whose name starts with "_j" (jpeg_*, jinit_*, jcopy_*, ...).
  * in all 46 libjpeg objects and in the libgfx.lib members that mention a name of S (JPEG_ImageReader / _Writer / _Common) the leading
    "_j" of each S name becomes "_J". C++ names, string literals and CRT names are untouched.
  * lib.exe packs the patched objects into KTDXLIB/sfjpeg.lib. KTDXApp.cpp / KTDGFXRuntime.cpp / KTDGGFXManager.cpp put
    #pragma comment( lib, "sfjpeg.lib" ) before libgfx, so the linker takes the three JPEG classes from sfjpeg.lib (calling _Jpeg_*) and never pulls
    libgfx's own copies (which would call the client's 6b).

    python tools/sfjpeg.py            # writes KTDXLIB/sfjpeg.lib
    python tools/sfjpeg.py --check    # prints the numbers, writes nothing
"""
import os
import struct
import subprocess
import sys
import tempfile

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SDK_LIB = os.path.join(TRUNK, "Libs", "ExternalLib", "Scaleform", "Lib", "Win32", "Msvc10", "Release")
LIB_EXE = r"D:\Program Files\VS\Microsoft Visual Studio 10.0\VC\bin\lib.exe"
OUT = os.path.join(TRUNK, "KTDXLIB", "sfjpeg.lib")
GFX_MEMBERS = ("JPEG_ImageReader", "JPEG_ImageWriter", "JPEG_ImageCommon")


def read_archive(path):
    d = open(path, "rb").read()
    assert d[:8] == b"!<arch>\n", path
    pos, raw, longnames = 8, [], None
    while pos < len(d):
        hdr = d[pos:pos + 60]
        name = hdr[:16].decode("latin1").rstrip()
        size = int(hdr[48:58])
        body = d[pos + 60:pos + 60 + size]
        raw.append((name, body))
        if name == "//":
            longnames = body
        pos += 60 + size + (size & 1)
    out = []
    for name, body in raw:
        if name in ("/", "//"):
            continue
        if name.startswith("/") and name[1:].isdigit():
            off = int(name[1:])
            end = off
            while end < len(longnames) and longnames[end] not in (0, 10):
                end += 1
            name = longnames[off:end].decode("latin1")
        else:
            name = name.rstrip("/")
        out.append((name, body))
    return out


def coff_symbols(data):
    """[(name bytes, offset of the name's first byte in data, storage class, section)]"""
    if len(data) < 20:
        return []
    machine, nsec, _ts, symptr, nsym = struct.unpack_from("<HHIII", data, 0)
    if machine != 0x14C or nsym == 0:
        return []
    strtab = symptr + nsym * 18
    out, i = [], 0
    while i < nsym:
        e = symptr + i * 18
        n8 = data[e:e + 8]
        _value, sec, _typ, sc, naux = struct.unpack_from("<IhHBB", data, e + 8)
        if n8[:4] == b"\0\0\0\0":
            off = strtab + struct.unpack_from("<I", n8, 4)[0]
            end = data.index(b"\0", off)
            out.append((data[off:end], off, sc, sec))
        else:
            out.append((n8.rstrip(b"\0"), e, sc, sec))
        i += 1 + naux
    return out


def patch(data, names):
    """_j... -> _J... for the symbols in names; returns (new data, count)"""
    buf = bytearray(data)
    n = 0
    for name, off, _sc, _sec in coff_symbols(data):
        if name in names:
            assert buf[off:off + 2] == b"_j"
            buf[off + 1] = ord("J")
            n += 1
    return bytes(buf), n


def main():
    check = "--check" in sys.argv
    jpeg = read_archive(os.path.join(SDK_LIB, "libjpeg.lib"))
    names = set()
    for _n, body in jpeg:
        for name, _off, sc, sec in coff_symbols(body):
            if sc == 2 and sec > 0 and name.startswith(b"_j"):
                names.add(name)
    objs = []
    for n, body in jpeg:
        if coff_symbols(body):
            new, c = patch(body, names)
            objs.append(("jpeg8_" + os.path.basename(n.replace("\\", "/")), new, c))
    for n, body in read_archive(os.path.join(SDK_LIB, "libgfx.lib")):
        base = os.path.basename(n.replace("\\", "/"))
        if any(base.startswith(m) for m in GFX_MEMBERS):
            new, c = patch(body, names)
            assert c > 0, base
            objs.append(("gfx_" + base, new, c))
    print("S: %d names, %d objects (%d from libjpeg, %d from libgfx), %d symbol entries renamed" % (
        len(names), len(objs), len(jpeg), sum(1 for o in objs if o[0].startswith("gfx_")), sum(o[2] for o in objs)))
    if check:
        return 0
    with tempfile.TemporaryDirectory() as tmp:
        paths = []
        for n, body, _c in objs:
            p = os.path.join(tmp, n)
            open(p, "wb").write(body)
            paths.append(p)
        if os.path.exists(OUT):
            os.remove(OUT)
        env = dict(os.environ)
        vs = os.path.dirname(os.path.dirname(os.path.dirname(LIB_EXE)))		# lib.exe needs mspdb100.dll from Common7\IDE
        env["PATH"] = os.path.dirname(LIB_EXE) + os.pathsep + os.path.join(vs, "Common7", "IDE") + os.pathsep + env.get("PATH", "")
        r = subprocess.run([LIB_EXE, "/nologo", "/OUT:" + OUT] + paths, capture_output=True, text=True, env=env)
        print(r.stdout.strip(), r.stderr.strip())
        if r.returncode != 0:
            return 1
    print("wrote", OUT, os.path.getsize(OUT), "B")
    return 0


if __name__ == "__main__":
    sys.exit(main())
