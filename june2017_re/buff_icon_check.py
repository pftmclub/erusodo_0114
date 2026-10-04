"""Phase 12 buff-icon check (JUNE_BUFF_PORT_PHASES.md). Resolves every BUFF_ICON_TEXTURE_KEY in June's
BattleFieldBuffTemplet(+_INT) against the .TET named by BUFF_ICON_TEXTURE_NAME.
`python buff_icon_check.py june` checks June's own atlases; `march` checks kom_contents (after the Phase 12
staging that is the June atlases; the pre-adoption March files are in kom_contents_backup_june_buff_phase12/).
TET layout: "TET ", u32 version, u32 count, count*u32 key offsets; key = u16 length + UTF-16LE; after the
offset table, 40-byte records of 10 floats (l,t,r,b pixels first). Keys are upper-cased by the client."""
import re, struct, sys, collections
R = r"F:/elsword stuff/elsword_2014/els_2014/"
T = R + "ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/"
J = R + "237311/14JUN17/data/"
M = R + "237311/24965799/kom_contents/"

def tet_keys(path):
    b = open(path, "rb").read()
    assert b[:4] == b"TET ", path
    n = struct.unpack_from("<I", b, 8)[0]
    offs = struct.unpack_from("<%dI" % n, b, 12)
    keys = []
    for o in offs:
        l = struct.unpack_from("<H", b, o)[0]
        keys.append(b[o + 2:o + 2 + 2 * l].decode("utf-16-le"))
    return keys

def templet_icons(path):
    """yield (templet_id_or_name, tex_name, key) per BUFF_ICON_TEXTURE_KEY"""
    txt = open(path, encoding="utf-8", errors="replace").read()
    # templets are top-level g_pBuffManager:AddBuffTemplet_LUA{ ... } style blocks: split on BUFF_TEMPLET_ID
    blocks = txt.split("AddBuffTemplet_LUA(")[1:]
    out = []
    for b in blocks:
        m = re.search(r"BUFF_TEMPLET_ID\s*=\s*BUFF_TEMPLET_ID\.([A-Za-z_0-9]+)", b)
        if not m:
            m = re.search(r"BUFF_TEMPLET_ID\s*=\s*([A-Za-z_0-9.]+)", b)
        k = re.search(r'BUFF_ICON_TEXTURE_KEY\s*=\s*"([^"]*)"', b)
        nm = re.search(r'BUFF_ICON_TEXTURE_NAME\s*=\s*"([^"]*)"', b)
        if k:
            out.append((m.group(1), nm.group(1) if nm else None, k.group(1)))
    return out

if __name__ == "__main__":
    which = sys.argv[1]  # "june" or "march"
    atlas = {}
    if which == "june":
        atlas["DLG_UI_Common_Texture_Buff.tga"] = J + "data215_kom/DLG_UI_Common_Texture_Buff.TET"
        atlas["DLG_UI_Common_Texture_Buff_INT.tga"] = J + "datai021_kom/DLG_UI_Common_Texture_Buff_INT.TET"
        atlas["DLG_INT_Texture01.tga"] = J + "datai021_kom/DLG_INT_Texture01.TET"
        atlas["DLG_INT_Button02.tga"] = J + "data056_kom/DLG_INT_Button02.TET"
    else:
        atlas["DLG_UI_Common_Texture_Buff.tga"] = M + "data057_kom/DLG_UI_Common_Texture_Buff.TET"
        atlas["DLG_INT_Texture01.tga"] = M + "datai021_kom/DLG_INT_Texture01.TET"
        atlas["DLG_INT_Button02.tga"] = M + "datai020_kom/DLG_INT_Button02.TET"
    ak = {}
    for k, v in atlas.items():
        try:
            ak[k] = tet_keys(v)
            print("atlas", k, len(ak[k]), "keys", "dups:", len(ak[k]) - len(set(ak[k])))
        except Exception as e:
            print("atlas", k, "MISSING", e)
    ents = []
    for f in ("BattleFieldBuffTemplet.src.lua", "BattleFieldBuffTemplet_INT.src.lua"):
        e = templet_icons(T + "june_src_fixed/data036_kom/" + f)
        print(f, len(e), "templets with icon key")
        ents += [(f,) + x for x in e]
    miss = []
    missci = []
    for f, tid, nm, key in ents:
        ks = ak.get(nm)
        if ks is None:
            miss.append((f, tid, nm, key, "NO ATLAS"))
        elif key.upper() not in [x.upper() for x in ks]:
            miss.append((f, tid, nm, key, "key absent"))
    print("unresolved (exact):", len(miss), " case-only mismatches:", len(missci))
    for m in miss:
        print("  ", m)
    for m in missci:
        print("  CI", m)
    print("distinct unresolved keys:", len(set((m[2], m[3]) for m in miss)))
