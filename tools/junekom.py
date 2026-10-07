#!/usr/bin/env python3
"""Read June 2017 .kom archives (V.0.3 and V.0.4) byte-exactly. Read-only on the archives.

    python tools/junekom.py list data002                      # name, size, csize, algo, MappedID, index, seed
    python tools/junekom.py extract data002 --out <dir>       # -> <dir>/data002_kom/<Name>
    python tools/junekom.py extract --names A.dds B.X --out <dir>   # search every June kom for these names
    python tools/junekom.py extract --all --out <June data> --overwrite --backup <June data>\\_extract_backup
                                                              # rewrite June's own *_kom extracts where they differ
                                                              # (identical files are skipped; each replaced file is
                                                              # copied to the backup first; June's tree refuses
                                                              # anything without --overwrite --backup)
    python tools/junekom.py verify                            # all 258 June koms (run in the background)
    python tools/junekom.py verify data101 --compare <root>   # also diff against <root>/data101_kom/<Name>
    python tools/junekom.py verify --sample 3                 # 3 entries per (kom, algo, csize%8) class
    python tools/junekom.py selftest                          # Blowfish vector + one known June entry

Why: elstools cuts the last 1-7 bytes of every V.0.4 algo-2 entry whose CompressedSize % 8 != 0 (June
Blowfish-decrypts only len & ~7 and stores the tail in plain text). This reader implements June's own
decode, as settled in JUNE_ASSET_EXTRACT_PHASES.md "Phase X0 - Done" (Ghidra addresses are there and in
the `XKOM X0` rows of june2017_re/funcmap.tsv):

  container  0x48-byte header: 0x34 count, 0x3C FileTime sum, 0x40 adler32(stored manifest), 0x44 manifest
             size; manifest at 0x48; payloads back to back in manifest order; last offset == file size.
             V.0.4 manifest is XORed with a 12-byte key, table[size % 200] (june2017_re/v04_manifest_keys.tsv).
  algo 0     plain zlib.
  algo 2     V.0.4: MappedID rotated right by Size & 7 -> index (june2017_re/v04_seed_table.tsv) -> Blowfish
             key SHA-1("%u" % seed); standard ECB decrypt over len & ~7, tail kept as stored.
             V.0.3 (or no index): key-C XOR-CRC, PLAIN seed step, no trailer.
  algo 3     key-C XOR-CRC, NOT-form seed step, 4 regenerated trailer bytes, inflate from offset 4.
             Same in both versions (V.0.4 algo-3 entries carry a MappedID that is not used).

`verify` decodes every entry and requires: inflate reaches end of stream, no trailing bytes, zlib's own
adler32 passes, output size == manifest Size. Per-entry Checksum (adler32 of the stored payload) is never
read by June; it is counted separately as evidence, not part of the verdict.

Exit codes: 0 ok, 1 failures found, 2 usage. Paths: JUNE_DATA env var overrides the June data dir.
Python 3, standard library only.
"""
import argparse, collections, faulthandler, hashlib, os, random, re, shutil, struct, sys, time, zlib
import xml.etree.ElementTree as ET

for _s in (sys.stdout, sys.stderr):  # the Windows console defaults to cp1252
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
JUNE_DATA = os.environ.get("JUNE_DATA", r"F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data")
JUNE_ROOT = os.path.dirname(os.path.normpath(JUNE_DATA))  # ...\14JUN17
KEYS_TSV = os.path.join(TRUNK, "june2017_re", "v04_manifest_keys.tsv")
SEEDS_TSV = os.path.join(TRUNK, "june2017_re", "v04_seed_table.tsv")
OUT_DIR = os.path.join(TRUNK, ".build", "junekom")
KOM_RE = re.compile(r"^datai?\d{3}\.kom$", re.I)
M32 = 0xFFFFFFFF

# --------------------------------------------------------------------------- tables

def _load_tsv(path, header):
    rows, seen = [], False
    for line in open(path, encoding="utf-8"):
        line = line.rstrip("\r\n")
        if not seen:  # leading '#' comment lines, then the header row (a MappedID may start with '#')
            seen = line.startswith(header)
            continue
        if line:
            rows.append(line.split("\t"))
    if not seen:
        raise SystemExit("%s: header %r not found" % (path, header))
    return rows

def load_manifest_keys():
    keys = {int(r[0]): bytes.fromhex(r[2]) for r in _load_tsv(KEYS_TSV, "index\t")}
    assert len(keys) == 200 and all(len(k) == 12 for k in keys.values()), "v04_manifest_keys.tsv"
    return keys

def load_seed_table():
    """{MappedID: index}, {index: (seed, sha1 key)}; the seed/key pairing is re-derived and checked."""
    by_id, by_idx = {}, {}
    for mid, _sva, idx, _tva, dwords, seed, key in _load_tsv(SEEDS_TSV, "mapped_id\t"):
        idx, seed = int(idx), int(seed)
        assert sum(int(d, 16) for d in dwords.split()) & M32 == seed, "seed sum %s" % mid
        assert hashlib.sha1(str(seed).encode()).hexdigest() == key, "sha1 %s" % mid
        by_id[mid] = idx
        by_idx[idx] = (seed, bytes.fromhex(key))
    assert len(by_id) == 300 and sorted(by_idx) == list(range(1, 301)), "v04_seed_table.tsv"
    return by_id, by_idx

def _pi_words(n):
    """First n 32-bit words of pi's fractional part (the standard Blowfish P/S init), via Machin."""
    bits = n * 32 + 64
    one = 1 << bits
    def arctan_inv(x):
        total, term, k, x2 = 0, one // x, 1, x * x
        while term:
            total += term // k if (k // 2) % 2 == 0 else -(term // k)
            term //= x2
            k += 2
        return total
    pi = 16 * arctan_inv(5) - 4 * arctan_inv(239)
    frac = (pi - 3 * one) >> 64
    return [(frac >> (32 * (n - 1 - i))) & M32 for i in range(n)]

CRC_T = []
for _i in range(256):
    _c = _i
    for _ in range(8):
        _c = (_c >> 1) ^ 0xEDB88320 if _c & 1 else _c >> 1
    CRC_T.append(_c)
CRC_REV = [0] * 256  # June's CRC32_GetTable()+0x400: index of the entry whose top byte is b
for _i, _t in enumerate(CRC_T):
    CRC_REV[_t >> 24] = _i
CRC_LO = bytes(t & 0xFF for t in CRC_T)
CRC_HI = [(t & 0xFFFFFF00) | i for i, t in enumerate(CRC_T)]
KEY_C = bytes.fromhex("fde108ec0d0902dcde4c8c6416627de49716ffdc")

_bf_init = None
_tables = None

def tables():
    global _tables
    if _tables is None:
        _tables = (load_manifest_keys(),) + load_seed_table()
    return _tables

# --------------------------------------------------------------------------- Blowfish (standard, ECB decrypt)

def _bf_initial():
    global _bf_init
    if _bf_init is None:
        w = _pi_words(18 + 1024)
        assert w[0] == 0x243F6A88 and w[18] == 0xD1310BA6, "pi digits"
        _bf_init = (w[:18], w[18:])
    return _bf_init

def _bf_schedule(key):
    P0, S0 = _bf_initial()
    P, S = list(P0), list(S0)
    j = 0
    for i in range(18):
        d = 0
        for _ in range(4):
            d = (d << 8) | key[j]
            j = (j + 1) % len(key)
        P[i] ^= d
    def enc(l, r):
        for i in range(16):
            l ^= P[i]
            r ^= ((((S[l >> 24] + S[256 + ((l >> 16) & 255)]) ^ S[512 + ((l >> 8) & 255)]) + S[768 + (l & 255)]) & M32)
            l, r = r, l
        l, r = r, l
        return l ^ P[17], r ^ P[16]
    l = r = 0
    for i in range(0, 18, 2):
        l, r = enc(l, r); P[i], P[i + 1] = l, r
    for i in range(0, 1024, 2):
        l, r = enc(l, r); S[i], S[i + 1] = l, r
    return P, S, enc

def bf_decryptor(key):
    """Return dec(words) -> words: ECB decrypt of a flat list of big-endian 32-bit words, rounds unrolled."""
    P, S, _ = _bf_schedule(key)
    src = ["def dec(w):", " out = [0] * len(w)", " for i in range(0, len(w), 2):", "  a = w[i]; b = w[i + 1]"]
    F = "((((S0[{x} >> 24] + S1[({x} >> 16) & 255]) ^ S2[({x} >> 8) & 255]) + S3[{x} & 255]) & 4294967295)"
    for k in range(16):  # loop form: l ^= P[i]; r ^= F(l); swap, i = 17..2; unrolled without the swaps
        if k % 2 == 0:
            src.append("  a ^= %d; b ^= %s" % (P[17 - k], F.format(x="a")))
        else:
            src.append("  b ^= %d; a ^= %s" % (P[17 - k], F.format(x="b")))
    src += ["  out[i] = b ^ %d; out[i + 1] = a ^ %d" % (P[0], P[1]), " return out"]
    ns = {"S0": tuple(S[0:256]), "S1": tuple(S[256:512]), "S2": tuple(S[512:768]), "S3": tuple(S[768:1024])}
    exec("\n".join(src), ns)
    return ns["dec"]

_bf_cache = {}

def bf_decode(raw, idx):
    """June 31c47a00 -> 31d26270 mode 0: decrypt len & ~7 bytes; the 1-7 tail bytes stay as stored."""
    dec = _bf_cache.get(idx)
    if dec is None:
        dec = _bf_cache[idx] = bf_decryptor(tables()[2][idx][1])
    n = len(raw) & ~7
    if not n:
        return bytes(raw)
    fmt = ">%dI" % (n // 4)
    return struct.pack(fmt, *dec(struct.unpack_from(fmt, raw))) + raw[n:]

# --------------------------------------------------------------------------- key-C XOR-CRC (algo 3, V.0.3 algo 2)

def keyc_decode(name, raw, algo3):
    """algo3=True: June 31c310e0 (NOT-form seed loop, trailer, output from offset 4).
    algo3=False: June 31c47a00 index-0 path = March XORCRCDecrypt (plain seed loop, no trailer)."""
    seed = bytearray()
    for ch in name[:260]:  # std::tolower(wchar_t, "C" locale): ASCII only; high byte appended when nonzero
        c = ord(ch.lower()) if ch.isascii() else ord(ch)
        seed.append(c & 0xFF)
        if c >> 8:
            seed.append((c >> 8) & 0xFF)
    s = len(raw)
    for _ in range(4):
        if not s:
            break
        seed.append(s & 0xFF); s >>= 8
    T, KC = CRC_T, KEY_C
    crc, k = M32, 0
    for b in seed:
        ks = KC[k] ^ (crc & 0xFF); k = (k + 1) % 20
        if algo3:
            ck = (b ^ 0xFF) ^ ks
            crc = (crc >> 8) ^ ((T[ck] & 0xFFFFFF00) | ck)
        else:
            crc = (crc >> 8) ^ T[b ^ ks]
    crc_seed = crc
    n = len(raw)
    kstream = (KC[k:] + KC[:k]) * (n // 20 + 1)
    out = bytearray(n)
    LO, HI = CRC_LO, CRC_HI
    for i in range(n):
        ks = kstream[i] ^ (crc & 0xFF)
        ck = raw[i] ^ 0xFF ^ ks
        out[i] = LO[ck] ^ ks
        crc = HI[ck] ^ (crc >> 8)
    k = (k + n) % 20
    if not (algo3 and n >= 4):
        return bytes(out)
    R = CRC_REV  # trailer: 31c3156e..31c31691
    k1 = R[crc >> 24]; s1 = ((HI[k1] ^ crc) << 8) & M32
    k2 = R[s1 >> 24]; s2 = ((HI[k2] ^ s1) << 8) & M32
    k3 = R[s2 >> 24]; s3 = ((HI[k3] ^ s2) << 8) & M32
    k4 = R[s3 >> 24]
    t = bytearray(4)
    s4 = KC[k] ^ (crc_seed & 0xFF); k = (k + 1) % 20
    t[0] = (T[k4] & 0xFF) ^ s4
    k5 = HI[k4] ^ (crc_seed >> 8)
    s5 = KC[k] ^ (k5 & 0xFF); k = (k + 1) % 20
    t[1] = (T[k3] & 0xFF) ^ s5
    k6 = HI[k3] ^ (k5 >> 8)
    s6 = KC[k] ^ (k6 & 0xFF); k = (k + 1) % 20
    t[2] = (T[k2] & 0xFF) ^ s6
    k7 = HI[k2] ^ (k6 >> 8)
    s7 = KC[k] ^ (k7 & 0xFF)
    t[3] = (T[k1] & 0xFF) ^ s7
    return bytes(out[4:]) + bytes(t)

# --------------------------------------------------------------------------- container

class Kom:
    """Parsed header + manifest. entries: dicts name/size/csize/algo/mid/idx/checksum/filetime/offset."""

    def __init__(self, path):
        self.path = path
        self.stem = os.path.splitext(os.path.basename(path))[0]
        self.file_size = os.path.getsize(path)
        with open(path, "rb") as f:
            h = f.read(0x48)
            self.count, self.unk38, self.ft_sum, self.adler, self.msize = struct.unpack_from("<5I", h, 0x34)
            stored = f.read(self.msize)
        self.version = h[:0x34].split(b"\0")[0].decode("latin1")
        self.v04 = "V.0.4" in self.version
        if not (self.v04 or "V.0.3" in self.version):
            raise ValueError("%s: unknown version %r" % (path, self.version))
        self.problems = []
        if zlib.adler32(stored) & M32 != self.adler:
            self.problems.append("manifest adler32 %08x != header %08x" % (zlib.adler32(stored) & M32, self.adler))
        m = stored
        if self.v04:
            key = tables()[0][self.msize % 200]
            m = bytes(c ^ key[i % 12] for i, c in enumerate(stored))
        root = ET.fromstring(m.split(b"\0")[0])
        by_id = tables()[1] if self.v04 else {}
        self.entries, off, ft = [], 0x48 + self.msize, 0
        for f in root.iter("File"):
            a = f.attrib
            size, csize = int(a["Size"]), int(a["CompressedSize"])
            mid = a.get("MappedID", "")
            idx = 0
            if self.v04 and mid:  # 31c4d350 @31c4db44: rotate right by Size & 7, then look up
                r = size & 7
                idx = by_id.get(mid[-r:] + mid[:-r] if r else mid, 0)
            ft += int(a.get("FileTime", "0"), 16)
            self.entries.append(dict(name=a["Name"], size=size, csize=csize, algo=int(a["Algorithm"]), mid=mid,
                                     idx=idx, checksum=a.get("Checksum", ""), offset=off))
            off += csize
        if len(self.entries) != self.count:
            self.problems.append("entry count %d != header %d" % (len(self.entries), self.count))
        if ft & M32 != self.ft_sum:
            self.problems.append("FileTime sum %08x != header %08x" % (ft & M32, self.ft_sum))
        if off != self.file_size:
            self.problems.append("last offset %d != file size %d" % (off, self.file_size))

def rule_of(e, v04):
    if e["algo"] == 0:
        return "zlib"
    if e["algo"] == 3:
        return "keyc3"
    if e["algo"] == 2:
        return "blowfish" if e["idx"] else "keyc2"
    return "algo%d?" % e["algo"]

def decode(e, raw, v04):
    """-> (bytes or None, error string or ''). Strict: end of stream, no trailing bytes, size == manifest."""
    rule = rule_of(e, v04)
    try:
        if rule == "zlib":
            z = raw
        elif rule == "keyc3":
            z = keyc_decode(e["name"], raw, True)
        elif rule == "blowfish":
            z = bf_decode(raw, e["idx"])
        elif rule == "keyc2":
            z = keyc_decode(e["name"], raw, False)
        else:
            return None, "unknown algorithm %d" % e["algo"]
        d = zlib.decompressobj()
        out = d.decompress(z) + d.flush()
    except zlib.error as ex:
        return None, "%s: zlib %s" % (rule, ex)
    if not d.eof:
        return None, "%s: stream not finished (%d B out)" % (rule, len(out))
    if d.unused_data:
        return None, "%s: %d trailing bytes after the stream" % (rule, len(d.unused_data))
    if len(out) != e["size"]:
        return None, "%s: %d B, manifest says %d" % (rule, len(out), e["size"])
    return out, ""

def resolve_kom(arg):
    if os.path.isfile(arg):
        return os.path.abspath(arg)
    s = arg.lower()
    s = s[:-4] if s.endswith(".kom") else s
    s = s[:-4] if s.endswith("_kom") else s
    m = re.fullmatch(r"(datai?)?(\d+)", s)
    if m:
        s = "%s%03d" % (m.group(1) or "data", int(m.group(2)))
    p = os.path.join(JUNE_DATA, s + ".kom")
    if not os.path.isfile(p):
        raise SystemExit("no such kom: %s (%s)" % (arg, p))
    return p

def all_koms():
    names = sorted(os.listdir(JUNE_DATA))
    koms = [os.path.join(JUNE_DATA, n) for n in names if KOM_RE.match(n)]
    skipped = [n for n in names if n.lower().endswith(".kom") and not KOM_RE.match(n)]
    return koms, skipped

def _long(path):
    """Windows: lift MAX_PATH for deep --out dirs (the \\\\?\\ prefix needs an absolute, backslashed path)."""
    path = os.path.abspath(path)
    return "\\\\?\\" + path if os.name == "nt" and len(path) > 240 and not path.startswith("\\\\") else path

def _under(path, root):
    path, root = os.path.normcase(os.path.abspath(path)), os.path.normcase(os.path.abspath(root))
    return path == root or path.startswith(root.rstrip("\\/") + os.sep)

# --------------------------------------------------------------------------- list / extract

def cmd_list(args):
    k = Kom(resolve_kom(args.kom))
    print("# %s  %s  %d entries  manifest %d B  header checks: %s" % (
        os.path.basename(k.path), k.version, k.count, k.msize, "; ".join(k.problems) or "ok"))
    print("name\tsize\tcsize\talgo\trule\tmapped_id\tindex\tseed")
    seeds = tables()[2]
    for e in k.entries:
        print("%s\t%d\t%d\t%d\t%s\t%s\t%d\t%s" % (e["name"], e["size"], e["csize"], e["algo"], rule_of(e, k.v04),
                                                  e["mid"], e["idx"], seeds[e["idx"]][0] if e["idx"] else ""))
    return 0

def cmd_extract(args):
    want = set(n.lower() for n in (args.names or []))
    if args.names_file:
        want |= set(l.strip().lower() for l in open(args.names_file, encoding="utf-8") if l.strip())
    koms = [resolve_kom(a) for a in args.koms] if args.koms else all_koms()[0]
    if not args.koms and not want and not args.all:
        raise SystemExit("give a kom, --names / --names-file, or --all (refusing to extract all 258 koms implicitly)")
    if _under(args.out, JUNE_ROOT) and not (args.overwrite and args.backup):
        raise SystemExit("--out is inside %s: June's tree is only written with --overwrite --backup <dir>" % JUNE_ROOT)
    if args.backup and os.path.normcase(os.path.abspath(args.backup)) == os.path.normcase(os.path.abspath(args.out)):
        raise SystemExit("--backup must not be --out (e.g. <June data>\\_extract_backup)")
    t0 = time.time()
    tasks = []
    for p in koms:
        k = Kom(p)
        ents = [e for e in k.entries if not want or e["name"].lower() in want]
        for chunk in _chunks(ents, args.chunk_mb << 20):
            tasks.append((p, k.stem, k.v04, chunk, args.out, args.overwrite, args.backup, args.verbose))
    tasks.sort(key=lambda t: -sum(_cost(e) for e in t[3]))
    st, found = collections.Counter(), set()
    def consume(res):
        c, msgs, names = res
        st.update(c); found.update(names)
        for m in msgs:
            print(m)
        sys.stdout.flush()
    if args.jobs <= 1 or len(tasks) <= 1:
        for t in tasks:
            consume(_extract_chunk(t))
    else:
        import multiprocessing
        with multiprocessing.Pool(args.jobs, initializer=_worker_init) as pool:
            for res in pool.imap_unordered(_extract_chunk, tasks):
                consume(res)
    bad = st["failed"]
    missing = sorted(want - found)
    for n in missing:
        print("NOT FOUND in any given kom: %s" % n)
    print("EXTRACT %s: new %d, replaced %d, identical %d, refused %d, failed %d, not found %d, %.0f s" % (
        "OK" if not bad and not missing and not st["refused"] else "INCOMPLETE",
        st["new"], st["replaced"], st["identical"], st["refused"], bad, len(missing), time.time() - t0))
    return 1 if bad or missing or st["refused"] else 0

def _cost(e):
    return e["csize"] * (1 if e["algo"] == 0 else 25)  # crude: cipher bytes cost ~25x an inflate

def _chunks(ents, chunk_cost):
    cur, cost = [], 0
    for e in ents:
        if cur and cost + _cost(e) > chunk_cost:
            yield cur
            cur, cost = [], 0
        cur.append(e); cost += _cost(e)
    if cur:
        yield cur

def _extract_chunk(task):
    """Worker: decode, compare, back up and write one slice of a kom. Each (kom, name) path is owned by one task."""
    path, stem, v04, ents, out_root, overwrite, backup, verbose = task
    st, msgs, names = collections.Counter(), [], []
    with open(path, "rb") as f:
        for e in ents:
            f.seek(e["offset"]); raw = f.read(e["csize"])
            out, err = decode(e, raw, v04)
            names.append(e["name"].lower())
            if out is None:
                msgs.append("FAIL %s/%s: %s" % (stem, e["name"], err)); st["failed"] += 1
                continue
            dst = _long(os.path.join(out_root, stem + "_kom", e["name"]))
            if os.path.isfile(dst):
                old = open(dst, "rb").read()
                if old == out:
                    st["identical"] += 1; continue
                if not overwrite:
                    msgs.append("DIFFERS (left alone, no --overwrite): %s (%d B on disk, %d B decoded)" % (dst, len(old), len(out)))
                    st["refused"] += 1; continue
                if backup:
                    bk = _long(os.path.join(backup, stem + "_kom", e["name"]))
                    if os.path.isfile(bk) and open(bk, "rb").read() != old:
                        msgs.append("REFUSED: backup %s exists with other content" % bk); st["refused"] += 1; continue
                    os.makedirs(os.path.dirname(bk), exist_ok=True)
                    if not os.path.isfile(bk):
                        shutil.copy2(dst, bk)
                st["replaced"] += 1
            else:
                st["new"] += 1
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            tmp = dst + ".part"
            with open(tmp, "wb") as f2:
                f2.write(out)
            os.replace(tmp, dst)
            if verbose:
                msgs.append("%s/%s %d B" % (stem, e["name"], len(out)))
    return st, msgs, names

# --------------------------------------------------------------------------- verify

def _worker_init():
    faulthandler.enable()

def _verify_chunk(task):
    """Worker: decode a slice of one kom's entries. Returns counters + rows (failures, compare diffs)."""
    path, stem, v04, ents, compare = task
    c = collections.Counter()
    fails, diffs = [], []
    with open(path, "rb") as f:
        for e in ents:
            f.seek(e["offset"]); raw = f.read(e["csize"])
            rule = rule_of(e, v04)
            out, err = decode(e, raw, v04)
            ver = "V4" if v04 else "V3"
            c[("n", ver, e["algo"], rule)] += 1
            c["bytes"] += e["csize"]
            if e["checksum"]:
                ok = "%08x" % (zlib.adler32(raw) & M32) == e["checksum"].lower().rjust(8, "0")
                c[("cksum", ver, e["algo"], "match" if ok else "mismatch")] += 1
            if out is None:
                c["fail"] += 1
                fails.append((stem, e["name"], e["algo"], rule, e["csize"], e["csize"] % 8, err))
                continue
            c["ok"] += 1
            if compare:
                xp = os.path.join(compare, stem + "_kom", e["name"])
                if not os.path.isfile(xp):
                    verdict, xs = "missing", ""
                else:
                    x = open(xp, "rb").read(); xs = len(x)
                    if x == out:
                        verdict = "same"
                    elif len(x) < len(out) and out.startswith(x):
                        verdict = "short-prefix"
                    else:
                        verdict = "different"
                c[("cmp", verdict)] += 1
                if verdict != "same":
                    diffs.append((stem, e["name"], e["algo"], rule, e["size"], xs, verdict))
    return path, c, fails, diffs

def _tasks(koms, compare, sample, chunk_bytes):
    tasks, per_kom, header = [], collections.Counter(), {}
    rnd = random.Random(7)
    for p in koms:
        k = Kom(p)
        header[p] = k
        ents = k.entries
        if sample:
            by = collections.defaultdict(list)
            for e in ents:
                by[(e["algo"], e["csize"] % 8)].append(e)
            ents = [e for lst in by.values() for e in rnd.sample(lst, min(sample, len(lst)))]
        cur, cost = [], 0
        for e in ents:
            w = e["csize"] * (1 if e["algo"] == 0 else 25)  # crude: cipher bytes cost ~25x an inflate
            if cur and cost + w > chunk_bytes:
                tasks.append((cost, (p, k.stem, k.v04, cur, compare))); per_kom[p] += 1
                cur, cost = [], 0
            cur.append(e); cost += w
        if cur or not tasks or tasks[-1][1][0] != p:
            tasks.append((cost, (p, k.stem, k.v04, cur, compare))); per_kom[p] += 1
    tasks.sort(key=lambda t: -t[0])
    return [t[1] for t in tasks], per_kom, header

def cmd_verify(args):
    import multiprocessing
    if args.koms:
        koms, skipped = [resolve_kom(a) for a in args.koms], []
    else:
        koms, skipped = all_koms()
    os.makedirs(OUT_DIR, exist_ok=True)
    stamp = time.strftime("%Y%m%d_%H%M%S")
    report = args.report or os.path.join(OUT_DIR, "verify_%s.tsv" % stamp)
    fail_path = os.path.splitext(report)[0] + "_failures.tsv"
    diff_path = os.path.splitext(report)[0] + "_compare.tsv"
    t0 = time.time()
    tasks, per_kom, header = _tasks(koms, args.compare, args.sample, args.chunk_mb << 20)
    print("verify: %d koms, %d tasks, %d jobs%s%s" % (len(koms), len(tasks), args.jobs,
          ", sample %d per class" % args.sample if args.sample else "", ", compare %s" % args.compare if args.compare else ""))
    if skipped:
        print("not June archives (name does not match dataNNN.kom / dataiNNN.kom), skipped: %s" % ", ".join(skipped))
    hdr_bad = [(os.path.basename(p), k.problems) for p, k in header.items() if k.problems]
    for n, pr in hdr_bad:
        print("HEADER %s: %s" % (n, "; ".join(pr)))
    total = collections.Counter()
    kom_c = collections.defaultdict(collections.Counter)
    left = dict(per_kom)
    rf = open(report, "w", encoding="utf-8", newline="\n")
    rf.write("kom\tversion\tentries\tchecked\tok\tfail\tstored_bytes\theader\tcompare\n"); rf.flush()
    ff = open(fail_path, "w", encoding="utf-8", newline="\n")
    ff.write("kom\tname\talgo\trule\tcsize\tcsize_mod8\terror\n")
    df = open(diff_path, "w", encoding="utf-8", newline="\n") if args.compare else None
    if df:
        df.write("kom\tname\talgo\trule\tsize\textract_size\tverdict\n")
    done_koms = 0
    def consume(res):
        nonlocal done_koms
        p, c, fails, diffs = res
        kom_c[p].update(c); total.update(c)
        for r in fails:
            ff.write("\t".join(map(str, r)) + "\n"); ff.flush()
            print("FAIL %s/%s algo %s %s: %s" % (r[0], r[1], r[2], r[3], r[6]))
        if df:
            for r in diffs:
                df.write("\t".join(map(str, r)) + "\n")
            df.flush()
        left[p] -= 1
        if not left[p]:
            k, kc = header[p], kom_c[p]
            cmp_s = ",".join("%s=%d" % (key[1], v) for key, v in sorted(kc.items(), key=str) if key[0:1] == ("cmp",)) if df else ""
            rf.write("%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%s\n" % (k.stem, k.version, k.count, kc["ok"] + kc["fail"], kc["ok"],
                     kc["fail"], kc["bytes"], "; ".join(k.problems) or "ok", cmp_s))
            rf.flush()
            done_koms += 1
            if args.progress and (done_koms % 10 == 0 or done_koms == len(koms)):
                print("  %d/%d koms, %d entries ok, %d fail, %.0f s" % (done_koms, len(koms), total["ok"], total["fail"], time.time() - t0))
                sys.stdout.flush()
    if args.jobs <= 1:
        for t in tasks:
            consume(_verify_chunk(t))
    else:
        with multiprocessing.Pool(args.jobs, initializer=_worker_init) as pool:
            for res in pool.imap_unordered(_verify_chunk, tasks):
                consume(res)
    rf.close(); ff.close()
    if df:
        df.close()
    print("\nversion algo rule      entries")
    for key in sorted(k for k in total if k[0] == "n"):
        print("  %s     %d    %-9s %d" % (key[1], key[2], key[3], total[key]))
    print("per-entry Checksum (adler32 of the stored payload; June never reads it):")
    for key in sorted(k for k in total if k[0] == "cksum"):
        print("  %s algo %d %-8s %d" % (key[1], key[2], key[3], total[key]))
    if args.compare:
        print("compare against %s: %s" % (args.compare, ", ".join(
            "%s %d" % (k[1], v) for k, v in sorted(total.items(), key=str) if k[0] == "cmp")))
        print("  rows: %s" % diff_path)
    ok = not total["fail"] and not hdr_bad
    print("VERIFY %s: %d koms, %d entries ok, %d failed, %d header problems, %.1f GB stored, %.0f s" % (
        "OK" if ok else "FAILED", len(koms), total["ok"], total["fail"], len(hdr_bad), total["bytes"] / 1e9, time.time() - t0))
    print("  report: %s" % report)
    if total["fail"]:
        print("  failures: %s" % fail_path)
    return 0 if ok else 1

# --------------------------------------------------------------------------- selftest

def cmd_selftest(args):
    P, S, enc = _bf_schedule(bytes(8))
    assert enc(0, 0) == (0x4EF99745, 0x6198DD78), "Blowfish test vector (key 0x00*8)"
    assert bf_decryptor(bytes(8))([0x4EF99745, 0x6198DD78]) == [0, 0], "Blowfish decrypt test vector"
    _, _, enc2 = _bf_schedule(bytes.fromhex("FEDCBA9876543210"))
    assert enc2(0x01234567, 0x89ABCDEF) == (0x0ACEAB0F, 0xC6A0A28D), "Blowfish test vector (key FEDCBA98...)"
    print("Blowfish: standard test vectors pass (P/S regenerated from pi)")
    k = Kom(resolve_kom("data002"))
    e = next(e for e in k.entries if e["name"] == "CHUNG_GIGANTIC_IMPACT_Bottom_Broken_SP.dds")
    with open(k.path, "rb") as f:
        f.seek(e["offset"]); raw = f.read(e["csize"])
    out, err = decode(e, raw, k.v04)
    assert out is not None, err
    assert len(out) == 22000 and "%08x" % (zlib.adler32(out) & M32) == "8e4a4e09", "CHUNG entry"
    print("CHUNG_GIGANTIC_IMPACT_Bottom_Broken_SP.dds: csize %d (%%8 = %d) -> %d B, adler32 8e4a4e09 (X0 gate)" % (
        e["csize"], e["csize"] % 8, len(out)))
    t = time.time(); n = 0
    for e in sorted((e for e in k.entries if rule_of(e, True) == "blowfish"), key=lambda e: -e["csize"])[:3]:
        with open(k.path, "rb") as f:
            f.seek(e["offset"]); raw = f.read(e["csize"])
        assert decode(e, raw, True)[0] is not None
        n += e["csize"]
    print("Blowfish throughput: %.2f MB/s (one process, %d KB)" % (n / 1e6 / (time.time() - t), n // 1024))
    print("SELFTEST OK")
    return 0

def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0], formatter_class=argparse.RawDescriptionHelpFormatter,
                                 epilog=__doc__.split("\n\n", 1)[1])
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("list"); s.add_argument("kom")
    s = sub.add_parser("extract")
    s.add_argument("koms", nargs="*", help="koms to read (default: every June kom; then --names is required)")
    s.add_argument("--names", nargs="+"); s.add_argument("--names-file")
    s.add_argument("--all", action="store_true", help="every entry of every June kom (needed when no kom/names are given)")
    s.add_argument("--out", required=True, help="writes <out>/<stem>_kom/<Name>")
    s.add_argument("--overwrite", action="store_true", help="replace an existing file whose bytes differ")
    s.add_argument("--backup", help="copy each replaced file to <backup>/<stem>_kom/<Name> first")
    s.add_argument("-v", "--verbose", action="store_true")
    s.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 2) - 1))
    s.add_argument("--chunk-mb", type=int, default=64, help="work unit size (cost-weighted MB)")
    s = sub.add_parser("verify")
    s.add_argument("koms", nargs="*")
    s.add_argument("--sample", type=int, default=0, help="N entries per (algo, csize%%8) class per kom")
    s.add_argument("--compare", help="extract root holding <stem>_kom/ dirs (e.g. June's data dir)")
    s.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 2) - 1))
    s.add_argument("--chunk-mb", type=int, default=64, help="work unit size (cost-weighted MB)")
    s.add_argument("--report", help="per-kom TSV (default .build/junekom/verify_<time>.tsv)")
    s.add_argument("--progress", action="store_true")
    sub.add_parser("selftest")
    args = ap.parse_args()
    return {"list": cmd_list, "extract": cmd_extract, "verify": cmd_verify, "selftest": cmd_selftest}[args.cmd](args)

if __name__ == "__main__":
    sys.exit(main())
