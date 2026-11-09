#!/usr/bin/env python3
"""skillicons.py - skill icons / skill-tree pictures whose texture or piece the March koms lack.

June's skill templet (NewSkillTempletVer2) names, for every skill, an icon texture + piece and a skill-tree image texture +
piece. When the texture is not in kom_contents, or its .TET has no such piece, the skill shows an empty icon in game.
That is how Aisha's transcendence passives (DLG_SKILL_Passive_12) and Eve's (DLG_SKILL_Passive_13) went missing.

  python tools/skillicons.py check 25000-25999            one id range (a class's skill ids), exit 1 on any miss
  python tools/skillicons.py check 22000-22999 25000-25999  several ranges
  python tools/skillicons.py check all                      every skill (other classes' misses are their ports')

Each miss row: id, name, icon / image, texture, piece, NO_TEXTURE / NO_TET / NO_PIECE. The last line lists the files to
stage from June (`python tools/kom.py stage-june <names>`; NO_PIECE means June's same-name sheet replaces March's).

A piece June's own sheet lacks ( the staged file is byte-identical to June's ) is listed in tools/data/skillicons_june_gaps.tsv
( skill id, icon / image, piece, reason ); the check prints it as KNOWN_JUNE_GAP and does not count it as a miss.
"""
import argparse, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KOMS = r"F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents"
SRC = os.path.join(ROOT, "june_src_fixed", "data036_kom", "NewSkillTempletVer2.src.lua")
KINDS = (("icon", "m_wstrIconTextureName", "m_wstrIconTexturePieceName"),
         ("image", "m_wstrImageTextureName", "m_wstrImageTexturePieceName"))


def kom_index():
    idx = {}
    for d in os.listdir(KOMS):
        p = os.path.join(KOMS, d)
        if d.endswith("_kom") and os.path.isdir(p):
            for f in os.listdir(p):
                idx.setdefault(f.lower(), os.path.join(p, f))
    return idx


def tet_pieces(path):
    d = open(path, "rb").read()
    t = d.decode("utf-16-le", "replace") if (d[:2] == b"\xff\xfe" or b"\x00" in d[:200]) else d.decode("cp949", "replace")
    return set(x.upper() for x in re.findall(r"[A-Za-z0-9_]+", t))


def templets():
    txt = open(SRC, encoding="utf-8-sig", errors="replace").read()
    for b in txt.split("g_pSkillTree:AddSkillTemplet_LUA(")[1:]:
        m = re.search(r"m_eID\s*=\s*(\d+)", b)
        if not m:
            continue
        name = re.search(r'm_wstrName\s*=\s*"([^"]*)"', b)
        refs = []
        for kind, tk, pk in KINDS:
            t, p = re.search(tk + r'\s*=\s*"([^"]*)"', b), re.search(pk + r'\s*=\s*"([^"]*)"', b)
            if t and t.group(1):
                refs.append((kind, t.group(1), p.group(1) if p else ""))
        yield int(m.group(1)), (name.group(1) if name else ""), refs


def june_gaps():
    gaps = set()
    path = os.path.join(ROOT, "tools", "data", "skillicons_june_gaps.tsv")
    if os.path.exists(path):
        for ln in open(path, encoding="ascii"):
            f = ln.rstrip("\n").split("\t")
            if len(f) >= 3 and f[0].isdigit():
                gaps.add((int(f[0]), f[1], f[2].upper()))
    return gaps


def in_ranges(sid, ranges):
    return ranges is None or any(a <= sid <= b for a, b in ranges)


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("check")
    c.add_argument("ranges", nargs="+", help="A-B id ranges, or 'all'")
    a = ap.parse_args()
    ranges = None
    if a.ranges != ["all"]:
        ranges = []
        for r in a.ranges:
            lo, _, hi = r.partition("-")
            ranges.append((int(lo), int(hi or lo)))
    idx, tets, misses, n = kom_index(), {}, [], 0
    gaps, known = june_gaps(), []
    for sid, name, refs in templets():
        if not in_ranges(sid, ranges):
            continue
        n += 1
        for kind, tex, piece in refs:
            if tex.lower() not in idx:
                misses.append((sid, name, kind, tex, piece, "NO_TEXTURE"))
                continue
            tet = os.path.splitext(tex)[0].lower() + ".tet"
            if tet not in tets:
                tets[tet] = tet_pieces(idx[tet]) if tet in idx else None
            if tets[tet] is None:
                misses.append((sid, name, kind, tex, piece, "NO_TET"))
            elif piece.upper() not in tets[tet]:
                if (sid, kind, piece.upper()) in gaps:
                    known.append((sid, name, kind, tex, piece, "KNOWN_JUNE_GAP"))
                else:
                    misses.append((sid, name, kind, tex, piece, "NO_PIECE"))
    for m in misses:
        print("\t".join(str(x) for x in m))
    files = sorted(set(f for m in misses for f in (m[3], os.path.splitext(m[3])[0] + ".tet")))
    for m in known:
        print("\t".join(str(x) for x in m))
    print("skills %d, misses %d, known June gaps %d, sheets %d" % (n, len(misses), len(known), len(files) // 2))
    if misses:
        print("stage: python tools/kom.py stage-june " + " ".join(files))
        print("RESULT: FAIL")
        return 1
    print("RESULT: PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
