#!/usr/bin/env python3
"""Read-only decoder for item.kim (the preprocessed item templet table, 'KIM ' v0x1201).

Two record layouts exist under the same header version, told apart by stride:
  188 B  March 2014  (X2Lib/X2Item_Preprocessing.inl KItemFormatTemplet)
  228 B  June 2017   (mapped in JUNE_ITEM_KIM_PHASES.md "Phase K0 - Done";
                      June writer 325bcdc0, accessors 328670f0..32868060)
Anything else is an error.

  python tools/kim.py info   <kim>
  python tools/kim.py dump   <kim> <id> [<id> ...]
  python tools/kim.py diff   <kimA> <kimB> --ids <file|list> | --common [N]
  python tools/kim.py census <kim> --field <name> [--top N]   (--list-fields)
  python tools/kim.py assets <kim> --ids <file|list> | --all

Stdlib only. Never writes anything.
"""
import argparse
import collections
import os
import struct
import sys

MAGIC = 0x204D494B  # 'KIM '
VERSION = 0x1201
HDR = 16

# ---------------------------------------------------------------- layouts
# (name, offset, kind)  kind: I u32, i s32, f float, h s16, H u16, B u8,
#                             s string offset, P pool offset
def _common_head():
    return [
        ("id", 0, "I"), ("flags", 4, "I"), ("flags2", 8, "I"),
        ("item_level", 12, "i"), ("quantity", 16, "i"), ("repair_ed", 20, "f"),
        ("repair_vp", 24, "i"), ("price", 28, "i"), ("price_pvp_point", 32, "i"),
        ("set_id", 36, "i"), ("add_max_mp", 40, "f"),
    ]


def _tail(base):
    names = ["texture_change_xet", "ani_xet", "ani_name", "shop_image", "drop_viewer",
             "desc_in_skill_note"]
    out = [(n, base + 4 * k, "s") for k, n in enumerate(names)]
    pools = ["stat", "buff_factors", "special_abilities", "socket_options", "attached_particles",
             "attached_meshes", "extra", "random_socket_options", "stat_rel_lv"]
    b = base + 24
    out += [("ofs_" + n, b + 4 * k, "P") for k, n in enumerate(pools)]
    b += 36
    out += [("endurance", b, "h"), ("endurance_dmg_min", b + 2, "h"),
            ("endurance_dmg_max", b + 4, "h"), ("summon_npc", b + 6, "H")]
    bytes_ = ["buy_pvp_rank", "use_level", "unit_type", "unit_class", "buff_id", "eqip_position",
              "cool_time", "max_seal_count", "max_attrib_enchant", "attribute_level",
              "n_frame_offsets", "n_render_scales", "n_render_rotates", "n_common_model",
              "n_common_xet", "n_slash_traces"]
    out += [(n, b + 8 + k, "B") for k, n in enumerate(bytes_)]
    return out


def _layout_march():
    f = _common_head()
    f += [("name", 44, "s"), ("desc", 48, "s"), ("desc_in_shop", 52, "s")]
    f += [("attach_frame_%d" % k, 56 + 4 * k, "s") for k in range(6)]
    f += [("model_name_%d" % k, 80 + 4 * k, "s") for k in range(6)]
    f += _tail(104)
    return dict(stride=188, models=6, fields=f, stat_floats=5, stat_rel=5, era="March")


def _layout_june():
    f = _common_head()
    f += [("damage_range", 44, "f"), ("item_id_for_minor_unit", 48, "i"),
          ("item_equipment_group", 52, "i")]
    f += [("name", 56, "s"), ("desc", 60, "s"), ("desc_in_shop", 64, "s")]
    f += [("attach_frame_%d" % k, 68 + 4 * k, "s") for k in range(8)]
    f += [("expired_date", 100, "s"), ("pre_expired_date", 104, "s")]
    f += [("model_name_%d" % k, 108 + 4 * k, "s") for k in range(8)]
    f += _tail(140)
    f += [("desc_in_shop_type", 224, "B")]
    return dict(stride=228, models=8, fields=f, stat_floats=7, stat_rel=6, era="June")


LAYOUTS = {188: _layout_march(), 228: _layout_june()}

# Bit fields packed in flags / flags2. hide_set_desc moved: March flags bit 7, June flags2 bit 1.
FLAG_BITS = [("fashion", 0), ("vested", 1), ("can_enchant", 2), ("can_use_inventory", 3),
             ("no_equip", 4), ("is_pc_bang", 5), ("can_hyper_mode", 6)]
FLAG_NIBBLES = [("use_type", 8), ("use_condition", 12), ("item_type", 16), ("item_grade", 20),
                ("period_type", 24), ("shop_price_type", 28)]

# Extra-table keys. March: CX2Unit::UNIT_TYPE 1..9. June: 1..13 from the name table @337358fc
# (accessor 32867660 maps UNIT_TYPE 10 -> 10 Lu / 11 Ciel by its bool arg, 11 -> 12, 12 -> 13).
EXTRA_KEYS = {1: "Elsword", 2: "Aisha", 3: "Rena", 4: "Raven", 5: "Eve", 6: "Chung", 7: "Ara",
              8: "Elesis", 9: "Add", 10: "Lu", 11: "Ciel", 12: "Rose", 13: "Ain"}
SLASH_KEYS = {0: "SLASH_TRACE", 1: "SLASH_TRACE_TIP", 2: "HYPER_SLASH_TRACE",
              3: "HYPER_SLASH_TRACE_TIP"}

# March limits for census out-of-range reports (cited from the March source).
MARCH_LIMITS = {
    "item_type": (0, 13, "IT_NUM = 14 (X2Item_Preprocessing.inl ITEM_TYPE)"),
    "item_grade": (0, 6, "IG_NUM = 7 with SERV_IRUHADEV_JUNE_ENCHANT_PARITY (IG_LEGEND = 6)"),
    "use_type": (0, 3, "UT_NUM = 4"),
    "use_condition": (0, 3, "UC_NUM = 4"),
    "period_type": (0, 2, "PT_NUM = 3"),
    "shop_price_type": (0, 2, "SPT_NUM = 3"),
    "unit_type": (0, 9, "CX2Unit::UT_END = 10 (X2Unit.h)"),
    "unit_class": (0, 122, "UC_ADD_LUNATIC_PSYKER = 122 is the last UNIT_CLASS (X2Unit.h)"),
    "special_ability_type": (0, 39, "SAT_END = 40"),
    "extra_key": (1, 9, "Extra keys are UNIT_TYPE 1..9 in March"),
    "hide_set_desc_flags_bit7": (0, 1, "March reads flags bit 7"),
}


# ---------------------------------------------------------------- reader
class Kim:
    def __init__(self, path):
        self.path = path
        with open(path, "rb") as fh:
            self.b = fh.read()
        b = self.b
        if len(b) < HDR:
            raise SystemExit("%s: shorter than the 16-byte header" % path)
        magic, ver, self.n, self.nsets = struct.unpack_from("<4I", b, 0)
        if magic != MAGIC or ver != VERSION:
            raise SystemExit("%s: not a KIM 0x1201 file (magic %08x version %x)" % (path, magic, ver))
        self.ids_ofs = HDR
        self.rec_ofs = HDR + 4 * self.n
        self.stride = self._detect_stride()
        self.L = LAYOUTS[self.stride]
        self.setid_ofs = self.rec_ofs + self.stride * self.n
        self.setrec_ofs = self.setid_ofs + 4 * self.nsets
        self._index = None

    def _detect_stride(self):
        b, n = self.b, self.n
        ids = struct.unpack_from("<%dI" % n, b, HDR) if n else ()
        ok = []
        for stride in (188, 228):
            end = HDR + n * (4 + stride) + self.nsets * (4 + 20)
            if end > len(b):
                continue
            probe = sorted({0, 1, n // 3, n // 2, (2 * n) // 3, n - 1} & set(range(n)))
            if all(struct.unpack_from("<I", b, HDR + 4 * n + stride * k)[0] == ids[k] for k in probe):
                ok.append(stride)
        if len(ok) != 1:
            raise SystemExit("%s: record stride is not 188 (March) or 228 (June): candidates %s, "
                             "%d items, %d sets, %d bytes" % (self.path, ok, n, self.nsets, len(self.b)))
        return ok[0]

    # -- primitives
    def u32(self, o):
        return struct.unpack_from("<I", self.b, o)[0]

    def wstr(self, ofs):
        if not ofs:
            return ""
        if ofs + 2 > len(self.b):
            return "<bad ofs %d>" % ofs
        ln = struct.unpack_from("<H", self.b, ofs)[0]
        if ln == 0:
            return ""
        raw = self.b[ofs + 2: ofs + 2 + 2 * ln]
        return raw.decode("utf-16-le", "replace").split("\0", 1)[0]

    def index(self):
        if self._index is None:
            ids = struct.unpack_from("<%dI" % self.n, self.b, HDR)
            self._index = {v: k for k, v in enumerate(ids)}
        return self._index

    def ids(self):
        return list(struct.unpack_from("<%dI" % self.n, self.b, HDR))

    def rec_base(self, item_id):
        k = self.index().get(item_id)
        return None if k is None else self.rec_ofs + self.stride * k

    # -- record
    def raw(self, item_id):
        base = self.rec_base(item_id)
        if base is None:
            return None
        r = collections.OrderedDict()
        for name, off, kind in self.L["fields"]:
            if kind in "sP":
                r[name] = self.u32(base + off)
            else:
                r[name] = struct.unpack_from("<" + kind, self.b, base + off)[0]
        return r

    def record(self, item_id):
        r = self.raw(item_id)
        if r is None:
            return None
        out = collections.OrderedDict()
        out["id"] = r["id"]
        fl, fl2 = r["flags"], r["flags2"]
        out["flags"] = "0x%08x" % fl
        out["flags2"] = "0x%08x" % fl2
        for n, bit in FLAG_BITS:
            out[n] = (fl >> bit) & 1
        if self.stride == 188:
            out["hide_set_desc"] = (fl >> 7) & 1
            out["pvp_item"] = fl2 & 1
        else:
            out["hide_set_desc"] = (fl2 >> 1) & 1
            out["pvp_item"] = fl2 & 1
            out["is_for_minor_unit"] = (fl2 >> 2) & 1
            out["flags_bit7"] = (fl >> 7) & 1
        for n, pos in FLAG_NIBBLES:
            out[n] = (fl >> pos) & 0xF
        for name, off, kind in self.L["fields"]:
            if name in ("id", "flags", "flags2"):
                continue
            v = r[name]
            if kind == "s":
                out[name] = self.wstr(v)
            elif kind == "P":
                continue
            elif kind == "f":
                out[name] = round(v, 6)
            else:
                out[name] = v
        out.update(self.pools(r))
        return out

    # -- pools
    def pools(self, r):
        b, P = self.b, collections.OrderedDict()
        o = r["ofs_stat"]
        n = self.L["stat_floats"]
        if o:
            v = struct.unpack_from("<%df" % n, b, o)
            if n == 5:
                P["stat"] = dict(hp=v[0], atk_physic=v[1], atk_magic=v[2], def_physic=v[3], def_magic=v[4])
            else:
                P["stat"] = dict(hp=v[0], atk_physic=v[1], atk_magic=v[2], slot3=v[3], slot4=v[4],
                                 def_physic=v[5], def_magic=v[6])
        else:
            P["stat"] = None
        o = r["ofs_stat_rel_lv"]
        n = self.L["stat_rel"]
        if o:
            v = struct.unpack_from("<%dB" % n, b, o)
            d = dict(hp=v[0], atk_physic=v[1], atk_magic=v[2], def_physic=v[3], def_magic=v[4])
            if n == 6:
                d["all_stat"] = v[5]
            P["stat_rel_lv"] = d
        else:
            P["stat_rel_lv"] = None
        o = r["ofs_buff_factors"]
        if o:
            cnt = struct.unpack_from("<H", b, o)[0]
            P["buff_factors"] = list(struct.unpack_from("<%dH" % cnt, b, o + 2))
        else:
            P["buff_factors"] = []
        o = r["ofs_special_abilities"]
        sa = []
        if o:
            cnt = self.u32(o)
            for k in range(cnt):
                t, cool, v1, v2, v3, so = struct.unpack_from("<Ii3iI", b, o + 4 + 24 * k)
                sa.append(dict(type=t, cool_time=cool, value1=v1, value2=v2, value3=v3,
                               string1=self.wstr(so) if so else "Group 0"))
        P["special_abilities"] = sa
        for key in ("socket_options", "random_socket_options"):
            o = r["ofs_" + key]
            if o:
                cnt = self.u32(o)
                P[key] = list(struct.unpack_from("<%di" % cnt, b, o + 4))
            else:
                P[key] = []
        for key in ("attached_particles", "attached_meshes"):
            o = r["ofs_" + key]
            lst = []
            if o:
                cnt = self.u32(o)
                for k in range(cnt):
                    nm, bone = struct.unpack_from("<II", b, o + 4 + 8 * k)
                    e = dict(name=self.wstr(nm & 0x7FFFFFFF), bone=self.wstr(bone))
                    if key == "attached_meshes":
                        e["jiggle"] = nm >> 31
                    lst.append(e)
            P[key] = lst
        P["extra"] = self.extra(r)
        return P

    def extra(self, r):
        o = r["ofs_extra"]
        groups = [("frame_offset", "n_frame_offsets", "v0"), ("render_scale", "n_render_scales", "v1"),
                  ("render_rotate", "n_render_rotates", "v0"), ("common_model", "n_common_model", "s"),
                  ("common_xet", "n_common_xet", "s"), ("slash_trace", "n_slash_traces", "c")]
        out = collections.OrderedDict()
        k = 0
        for gname, cname, kind in groups:
            cnt = r[cname]
            lst = []
            for _ in range(cnt):
                if not o:
                    break
                key, val = struct.unpack_from("<II", self.b, o + 8 * k)
                k += 1
                if kind == "s":
                    v = self.wstr(val)
                elif kind == "c":
                    v = "0x%08x" % val
                elif val:
                    v = [round(x, 4) for x in struct.unpack_from("<3f", self.b, val)]
                else:
                    v = [1.0, 1.0, 1.0] if kind == "v1" else [0.0, 0.0, 0.0]
                names = SLASH_KEYS if kind == "c" else EXTRA_KEYS
                lst.append((key, names.get(key, "?%d" % key), v))
            if lst:
                out[gname] = lst
        return out

    # -- sets
    def set_record(self, set_id):
        lo, hi = 0, self.nsets
        while lo < hi:
            mid = (lo + hi) // 2
            if self.u32(self.setid_ofs + 4 * mid) < set_id:
                lo = mid + 1
            else:
                hi = mid
        if lo >= self.nsets or self.u32(self.setid_ofs + 4 * lo) != set_id:
            return None
        sid, maxlv, oname, oitems, oparts = struct.unpack_from("<IiIII", self.b, self.setrec_ofs + 20 * lo)
        items = []
        if oitems:
            cnt = self.u32(oitems)
            items = list(struct.unpack_from("<%dI" % cnt, self.b, oitems + 4))
        parts = []
        if oparts:
            cnt = self.u32(oparts)
            parts = [struct.unpack_from("<Ii", self.b, oparts + 4 + 8 * k) for k in range(cnt)]
        return dict(set_id=sid, max_level=maxlv, name=self.wstr(oname), items=items,
                    need_parts_and_option=parts)


# ---------------------------------------------------------------- helpers
def parse_ids(spec):
    if spec is None:
        return []
    if os.path.isfile(spec):
        out = []
        with open(spec, encoding="utf-8", errors="replace") as fh:
            for line in fh:
                tok = line.strip().split("\t")[0].split(",")[0].strip()
                if tok.isdigit():
                    out.append(int(tok))
        return out
    return [int(t) for t in spec.replace(",", " ").split() if t.strip().isdigit()]


def flat(rec, prefix=""):
    """Flatten a decoded record into name -> comparable scalar/str."""
    out = {}
    for k, v in rec.items():
        key = prefix + k
        if isinstance(v, dict):
            out.update(flat(v, key + "."))
        elif isinstance(v, (list, tuple)):
            out[key] = repr(v)
        elif v is None:
            out[key] = "None"
        else:
            out[key] = v
    return out


def norm_for_diff(kim, rec):
    """Field names are era-neutral already; June-only fields (stat.slot3, attach_frame_6, ...) simply
    have no counterpart in a March record and drop out of the per-field comparison."""
    return flat(rec)


def out(s=""):
    sys.stdout.write(s + "\n")


# ---------------------------------------------------------------- commands
def cmd_info(a):
    k = Kim(a.kim)
    out("%s: %s layout, stride %d, %d items, %d sets, %d bytes" % (
        a.kim, k.L["era"], k.stride, k.n, k.nsets, len(k.b)))
    ids = k.ids()
    if ids:
        out("ids %d .. %d" % (ids[0], ids[-1]))


def cmd_dump(a):
    k = Kim(a.kim)
    out("# %s: %s layout, stride %d" % (a.kim, k.L["era"], k.stride))
    for item_id in a.ids:
        rec = k.record(item_id)
        if rec is None:
            out("%d: not in this kim" % item_id)
            continue
        out("== %d" % item_id)
        for key, v in rec.items():
            if isinstance(v, dict) and key == "extra":
                for g, lst in v.items():
                    for kk, nm, vv in lst:
                        out("  extra.%s[%d %s] = %s" % (g, kk, nm, vv))
            elif isinstance(v, list) and v and isinstance(v[0], dict):
                for i, e in enumerate(v):
                    out("  %s[%d] = %s" % (key, i, e))
            elif v in ("", 0, [], None) and not a.all and key not in ("id",):
                continue
            else:
                out("  %s = %s" % (key, v))
        if rec.get("set_id"):
            s = k.set_record(rec["set_id"])
            out("  set -> %s" % (s if s else "set id %d not in the set table" % rec["set_id"]))


def cmd_diff(a):
    ka, kb = Kim(a.kimA), Kim(a.kimB)
    if a.common is not None:
        common = sorted(set(ka.ids()) & set(kb.ids()))
        if a.common and a.common < len(common):
            step = len(common) / float(a.common)
            common = [common[int(i * step)] for i in range(a.common)]
        ids = common
    else:
        ids = parse_ids(a.ids)
    out("# A %s (%s), B %s (%s); %d ids" % (a.kimA, ka.L["era"], a.kimB, kb.L["era"], len(ids)))
    mism = collections.Counter()
    seen = collections.Counter()
    missing = [0, 0]
    shown = 0
    for i in ids:
        ra, rb = ka.record(i), kb.record(i)
        if ra is None or rb is None:
            missing[0 if ra is None else 1] += 1
            if not a.summary:
                out("%d: %s" % (i, "missing in A" if ra is None else "missing in B"))
            continue
        fa, fb = norm_for_diff(ka, ra), norm_for_diff(kb, rb)
        for key in sorted(set(fa) & set(fb)):
            if key in ("flags", "flags2"):
                continue
            seen[key] += 1
            if fa[key] != fb[key]:
                mism[key] += 1
                if not a.summary and shown < a.limit:
                    out("%d %s: A=%r B=%r" % (i, key, fa[key], fb[key]))
                    shown += 1
        only = sorted(set(fa) ^ set(fb))
        if only and not a.summary and shown < a.limit and a.show_only:
            out("%d only in one side: %s" % (i, only))
    out("# missing in A %d, missing in B %d" % tuple(missing))
    out("# per-field mismatches (fields both layouts have):")
    for key in sorted(seen):
        out("%-40s %7d / %d" % (key, mism[key], seen[key]))


def census_values(k, field):
    """Yield (id, value) for a census field, including derived and pool fields."""
    for item_id in k.ids():
        r = k.raw(item_id)
        fl, fl2 = r["flags"], r["flags2"]
        if field in r:
            yield item_id, r[field]
        elif field in dict(FLAG_NIBBLES):
            yield item_id, (fl >> dict(FLAG_NIBBLES)[field]) & 0xF
        elif field in dict(FLAG_BITS):
            yield item_id, (fl >> dict(FLAG_BITS)[field]) & 1
        elif field == "flags_bit7" or field == "hide_set_desc_flags_bit7":
            yield item_id, (fl >> 7) & 1
        elif field.startswith("flags2_bit"):
            yield item_id, (fl2 >> int(field[10:])) & 1
        elif field == "special_ability_type":
            o = r["ofs_special_abilities"]
            if o:
                for j in range(k.u32(o)):
                    yield item_id, k.u32(o + 4 + 24 * j)
        elif field in ("extra_key", "slash_key"):
            o = r["ofs_extra"]
            cnt = sum(r[c] for c in ("n_frame_offsets", "n_render_scales", "n_render_rotates",
                                     "n_common_model", "n_common_xet"))
            if not o:
                continue
            if field == "extra_key":
                for j in range(cnt):
                    yield item_id, k.u32(o + 8 * j)
            else:
                for j in range(r["n_slash_traces"]):
                    yield item_id, k.u32(o + 8 * (cnt + j))
        elif field.startswith("stat."):
            o = r["ofs_stat"]
            if o:
                names = (["hp", "atk_physic", "atk_magic", "def_physic", "def_magic"]
                         if k.stride == 188 else
                         ["hp", "atk_physic", "atk_magic", "slot3", "slot4", "def_physic", "def_magic"])
                sub = field[5:]
                if sub in names:
                    yield item_id, struct.unpack_from("<f", k.b, o + 4 * names.index(sub))[0]
        elif field == "stat_rel_lv.all_stat":
            o = r["ofs_stat_rel_lv"]
            if o and k.stride == 228:
                yield item_id, k.b[o + 5]
        elif field == "n_buff_factors":
            o = r["ofs_buff_factors"]
            yield item_id, struct.unpack_from("<H", k.b, o)[0] if o else 0
        else:
            raise SystemExit("unknown field %r (use --list-fields)" % field)


def census_fields(k):
    names = [n for n, _, _ in k.L["fields"]]
    names += [n for n, _ in FLAG_BITS] + [n for n, _ in FLAG_NIBBLES]
    names += ["flags_bit7", "flags2_bit0", "flags2_bit1", "flags2_bit2", "flags2_bit3",
              "special_ability_type", "extra_key", "slash_key", "n_buff_factors",
              "stat.hp", "stat.atk_physic", "stat.atk_magic", "stat.def_physic", "stat.def_magic"]
    if k.stride == 228:
        names += ["stat.slot3", "stat.slot4", "stat_rel_lv.all_stat"]
    return names


def cmd_census(a):
    k = Kim(a.kim)
    if a.list_fields:
        for n in census_fields(k):
            out(n)
        return
    if not a.field:
        raise SystemExit("--field is required (or --list-fields)")
    hist = collections.Counter()
    example = {}
    for item_id, v in census_values(k, a.field):
        if isinstance(v, float):
            v = round(v, 4)
        hist[v] += 1
        example.setdefault(v, item_id)
    out("# %s (%s, stride %d): field %s, %d values, %d distinct" % (
        a.kim, k.L["era"], k.stride, a.field, sum(hist.values()), len(hist)))
    rows = sorted(hist.items(), key=lambda kv: (-kv[1], str(kv[0])))
    if not a.by_count:
        try:
            rows = sorted(hist.items())
        except TypeError:
            pass
    for v, c in rows[: a.top]:
        out("%-24s %8d   e.g. %d" % (v, c, example[v]))
    if len(rows) > a.top:
        out("... %d more distinct values (--top)" % (len(rows) - a.top))
    lim = MARCH_LIMITS.get(a.field)
    if lim:
        lo, hi, why = lim
        bad = {v: c for v, c in hist.items() if not (lo <= v <= hi)}
        out("# out of March range [%d..%d] (%s): %d value(s), %d record(s)" % (
            lo, hi, why, len(bad), sum(bad.values())))
        for v in sorted(bad):
            out("  %-22s %8d   e.g. %d" % (v, bad[v], example[v]))


ASSET_STRINGS = ["texture_change_xet", "ani_xet", "shop_image", "drop_viewer"]


def cmd_assets(a):
    k = Kim(a.kim)
    ids = k.ids() if a.all else parse_ids(a.ids)
    out("id\tkind\tname")
    for item_id in ids:
        rec = k.record(item_id)
        if rec is None:
            out("%d\tMISSING\t" % item_id)
            continue
        rows = []
        for m in range(k.L["models"]):
            rows.append(("model", rec.get("model_name_%d" % m, "")))
        for key in ASSET_STRINGS:
            rows.append((key, rec.get(key, "")))
        rows.append(("ani_name", rec.get("ani_name", "")))
        for e in rec["attached_particles"]:
            rows.append(("particle", e["name"]))
        for e in rec["attached_meshes"]:
            rows.append(("mesh", e["name"]))
        for g in ("common_model", "common_xet"):
            for kk, nm, v in rec["extra"].get(g, []):
                rows.append(("%s[%s]" % (g, nm), v))
        for kind, name in rows:
            if name:
                out("%d\t%s\t%s" % (item_id, kind, name))


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd")
    s = sub.add_parser("info"); s.add_argument("kim")
    s = sub.add_parser("dump"); s.add_argument("kim"); s.add_argument("ids", nargs="+", type=int)
    s.add_argument("--all", action="store_true", help="also print empty/zero fields")
    s = sub.add_parser("diff"); s.add_argument("kimA"); s.add_argument("kimB")
    g = s.add_mutually_exclusive_group(required=True)
    g.add_argument("--ids", help="file (first column) or comma/space list")
    g.add_argument("--common", type=int, nargs="?", const=0,
                   help="every id in both files, or an even sample of N")
    s.add_argument("--summary", action="store_true", help="per-field counts only")
    s.add_argument("--limit", type=int, default=200, help="max detail lines")
    s.add_argument("--show-only", action="store_true", help="list fields only one layout has")
    s = sub.add_parser("census"); s.add_argument("kim"); s.add_argument("--field")
    s.add_argument("--top", type=int, default=60); s.add_argument("--by-count", action="store_true")
    s.add_argument("--list-fields", action="store_true")
    s = sub.add_parser("assets"); s.add_argument("kim")
    g = s.add_mutually_exclusive_group(required=True)
    g.add_argument("--ids"); g.add_argument("--all", action="store_true")
    a = p.parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    fn = {"info": cmd_info, "dump": cmd_dump, "diff": cmd_diff, "census": cmd_census,
          "assets": cmd_assets}.get(a.cmd)
    if not fn:
        p.print_help()
        return 2
    fn(a)
    return 0


if __name__ == "__main__":
    sys.exit(main())
