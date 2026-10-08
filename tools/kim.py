#!/usr/bin/env python3
"""Read-only decoder for item.kim (the preprocessed item templet table, 'KIM ' v0x1201).

Two record layouts exist under the same header version, told apart by stride:
  188 B  March 2014  (X2Lib/X2Item_Preprocessing.inl KItemFormatTemplet)
  228 B  June 2017   (mapped in JUNE_ITEM_KIM_PHASES.md "Phase K0 - Done";
                      June writer 325bcdc0, accessors 328670f0..32868060)
Anything else is an error.

  python tools/kim.py info   [kim]
  python tools/kim.py dump   [kim] <id> [<id> ...]
  python tools/kim.py find   [kim] [--name TEXT] [--re RX] [--where FIELD<op>VALUE ...] [--ids LIST]
                                   [--cols a,b,c] [--sort FIELD [--desc]] [--limit N] [--count|--tsv|--json]
  python tools/kim.py set    [kim] <set_id ...> | <words from the set name>
  python tools/kim.py census [kim] --field <name> [--top N]   (--list-fields)
  python tools/kim.py assets [kim] --ids <file|list> | --all
  python tools/kim.py diff   <kimA> <kimB> --ids <file|list> | --common [N]
  python tools/kim.py kims   (list the aliases below)

<kim> is a path or an alias: staged (default: kom_contents data036_kom, June's since K2), june, march,
march_patched. Fields are one namespace for find --where/--cols/--sort and census --field
(census --list-fields). Enum fields take names (item_type=IT_WEAPON, unit_class=elesis_knight); a value
with no sourced name prints as a number. Pool fields (special_ability_type, buff_factor,
socket_option, ...) match if ANY element matches; != and !~ mean NO element matches.

Stdlib only. Never writes anything.
"""
import argparse
import collections
import hashlib
import json
import math
import os
import re
import struct
import sys

# Named kims; any command taking a <kim> also takes one of these. Single-kim commands default to "staged".
GAME_ROOT = "F:/elsword stuff/elsword_2014/els_2014/237311"
KIM_ALIASES = collections.OrderedDict([
    ("staged", ("24965799/kom_contents/data036_kom/item.kim",
                "what /elstools pack puts into data036.kom (June's since K2)")),
    ("june", ("14JUN17/data/data036_kom/item.kim", "June 2017 original")),
    ("march", ("24965799/march_data/data036/item.kim", "March 2014 original")),
    ("march_patched", ("24965799/march_data/data036/item.kim.march_patched",
                       "March + buff Phase 13 patch (the pre-K2 staged kim)")),
])


def resolve_kim(spec):
    if spec in KIM_ALIASES:
        return GAME_ROOT + "/" + KIM_ALIASES[spec][0]
    return spec


def is_kim_spec(tok):
    return tok in KIM_ALIASES or (os.path.isfile(tok) and not tok.isdigit())


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

# ---------------------------------------------------------------- enum names
# Labels only; values never get guessed. A value with no sourced name prints as its bare number.
def _seq(names, start=0):
    return {start + k: n for k, n in enumerate(names)}


# X2Lib/X2Item_Preprocessing.inl. IT_CUSTOM = 14 and IG_LEGEND = 6 are behind
# SERV_IRUHADEV_JUNE_ITEM_KIM / SERV_IRUHADEV_JUNE_ENCHANT_PARITY there (June values).
ENUM_ITEM_TYPE = _seq(["IT_NONE", "IT_WEAPON", "IT_DEFENCE", "IT_ACCESSORY", "IT_SKILL", "IT_QICK_SLOT",
                       "IT_MATERIAL", "IT_SPECIAL", "IT_QUEST", "IT_OUTLAY", "IT_ETC", "IT_SKILL_MEMO",
                       "IT_PET", "IT_RIDING", "IT_CUSTOM"])
ENUM_ITEM_GRADE = _seq(["IG_NONE", "IG_UNIQUE", "IG_ELITE", "IG_RARE", "IG_NORMAL", "IG_LOW", "IG_LEGEND"])
ENUM_USE_TYPE = _seq(["UT_NONE", "UT_SKIN", "UT_ATTACH_ANIM", "UT_ATTACH_NORMAL"])
ENUM_USE_CONDITION = _seq(["UC_NONE", "UC_ANYONE", "UC_ONE_UNIT", "UC_ONE_CLASS"])
ENUM_PERIOD_TYPE = _seq(["PT_INFINITY", "PT_ENDURANCE", "PT_QUANTITY"])
ENUM_SHOP_PRICE_TYPE = _seq(["SPT_NONE", "SPT_CASH", "SPT_GP"])
# SAT 0..39 from the .inl; 41..52 June-only from june_item_kim/k1_enum_names.tsv. 40 has no source.
ENUM_SAT = _seq(["SAT_NONE", "SAT_HP_UP", "SAT_MP_UP", "SAT_REMOVE_CURSE", "SAT_REMOVE_SLOW",
                 "SAT_REMOVE_FIRE", "SAT_REMOVE_POISON", "SAT_RECIPE", "SAT_SUPPLEMENT_ITEM",
                 "SAT_HYPER_MODE", "SAT_SOUL_GAGE_UP", "SAT_SHOW_OPPONENT_MP",
                 "SAT_UP_MP_AT_ATTACK_OR_DAMAGE", "SAT_HP_PERCENT_UP", "SAT_TEAM_HP_UP", "SAT_TEAM_MP_UP",
                 "SAT_PEPPER_RUN", "SAT_TRANSFORM_UNIT_SCALE", "SAT_SPECIAL_SKILL", "SAT_MP_PERCENT_UP",
                 "SAT_POWER_RATE_UP", "SAT_MOVE_SPEED_UP", "SAT_JUMP_SPEED_UP", "SAT_MP_REGENERATION",
                 "SAT_USE_HYPER", "SAT_PHYSIC_ATTACK_UP", "SAT_MAGIC_ATTACK_UP", "SAT_PHYSIC_DEFENCE_UP",
                 "SAT_MAGIC_DEFENCE_UP", "SAT_SUPERARMOR", "SAT_SUMMON_SPIRIT", "SAT_REMOVE_FROZEN",
                 "SAT_TRANSFORM_MONSTER", "SAT_WAY_OF_SWORD_GAUGE_UP", "SAT_SUMMON_MONSTER",
                 "SAT_FORCE_CRITICAL_MAX", "SAT_ENABLE_ATTACK_MONSTER", "SAT_ARA_FORCE_POWER_PERCENT_UP",
                 "SAT_RETURN_TO_BASE_AT_PVE", "SAT_RIDINGPET_STAMINA_PERCENT_UP"])
ENUM_SAT.update(_seq(["SAT_ADD_DP_PERCENT_UP", "SAT_CURR_LEVEL_EXP_PERCENT_UP", "SAT_STEAL_AMUSEMENT_ITEM",
                      "SAT_PVP_EQUIP_ENCHANT_LEVEL_7", "SAT_PVP_EQUIP_ENCHANT_LEVEL_8",
                      "SAT_PVP_EQUIP_ENCHANT_LEVEL_9", "SAT_PVP_EQUIP_ENCHANT_LEVEL_10",
                      "SAT_PVP_EQUIP_ENCHANT_LEVEL_11", "SAT_PVP_EQUIP_ENCHANT_LEVEL_12",
                      "SAT_PVP_EQUIP_ENCHANT_LEVEL_13", "SAT_IMMEDIATELY_EXCHANGE_TO_ED",
                      "SAT_ELIANODE_PIECE_EQUIP_TYPE"], 41))
# X2Lib/X2Unit.h UNIT_TYPE 0..9. June 10..12 per accessor 32867660 (see EXTRA_KEYS): 10 Lu/Ciel, 11 Rose, 12 Ain.
ENUM_UNIT_TYPE = _seq(["UT_NONE", "UT_ELSWORD", "UT_ARME", "UT_LIRE", "UT_RAVEN", "UT_EVE", "UT_CHUNG",
                       "UT_ARA", "UT_ELESIS", "UT_ADD"])
ENUM_UNIT_TYPE_JUNE = {10: "UT_LU_CIEL", 11: "UT_ROSE", 12: "UT_AIN"}
# X2Lib/X2Unit_PreHeader.h EQIP_POSITION; 38 needs SERV_NEW_ONE_PIECE_AVATAR_SLOT, on for US (EP_END = 39,
# JUNE_ITEM_KIM_PHASES.md K1). June 26/27 are EP_AC_MINIME/EP_AC_EARRING (K1), March's unused TEMP2/3.
ENUM_EQIP = _seq(["EP_NONE", "EP_QUICK_SLOT", "EP_WEAPON_HAND", "EP_WEAPON_TEMP1", "EP_WEAPON_TEMP2",
                  "EP_WEAPON_TEMP3", "EP_DEFENCE_HAIR", "EP_DEFENCE_FACE", "EP_DEFENCE_BODY", "EP_DEFENCE_LEG",
                  "EP_DEFENCE_HAND", "EP_DEFENCE_FOOT", "EP_DEFENCE_TEMP1", "EP_DEFENCE_TEMP2",
                  "EP_DEFENCE_TEMP3", "EP_AC_TITLE", "EP_AC_HAIR", "EP_AC_FACE1", "EP_AC_FACE2", "EP_AC_FACE3",
                  "EP_AC_BODY", "EP_AC_LEG", "EP_AC_ARM", "EP_AC_RING", "EP_AC_NECKLESS", "EP_AC_WEAPON",
                  "EP_AC_TEMP2", "EP_AC_TEMP3", "EP_AC_TEMP4", "EP_AC_TEMP5", "EP_SKILL_1", "EP_SKILL_2",
                  "EP_SKILL_3", "EP_SKILL_TEMP1", "EP_SKILL_TEMP2", "EP_SKILL_TEMP3", "EP_RAVEN_LEFT_ARM",
                  "EP_WEAPON_SECOND", "EP_ONEPIECE_FASHION"])
ENUM_EQIP_JUNE = {26: "EP_AC_MINIME", 27: "EP_AC_EARRING"}


def _unit_classes():
    """CX2Unit::UNIT_CLASS 1..122 (March, resolved) from tools/createchar.py; June's 123+ stay numeric."""
    try:
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        import createchar
        return {k: "UC_" + v for k, v in createchar.CLASSES.items()}
    except Exception:
        return {}


_ENUM_CACHE = {}


def enum_table(field, era):
    """name table for an enum-valued field in this era, or None."""
    key = (field, era)
    if key in _ENUM_CACHE:
        return _ENUM_CACHE[key]
    t = None
    if field == "item_type":
        t = ENUM_ITEM_TYPE
    elif field == "item_grade":
        t = ENUM_ITEM_GRADE
    elif field == "use_type":
        t = ENUM_USE_TYPE
    elif field == "use_condition":
        t = ENUM_USE_CONDITION
    elif field == "period_type":
        t = ENUM_PERIOD_TYPE
    elif field == "shop_price_type":
        t = ENUM_SHOP_PRICE_TYPE
    elif field in ("special_ability_type", "special_abilities.type"):
        t = ENUM_SAT
    elif field == "unit_type":
        t = dict(ENUM_UNIT_TYPE)
        if era == "June":
            t.update(ENUM_UNIT_TYPE_JUNE)
    elif field == "unit_class":
        t = dict(_unit_classes())
        t[0] = "UC_NONE"
    elif field == "eqip_position":
        t = dict(ENUM_EQIP)
        if era == "June":
            t.update(ENUM_EQIP_JUNE)
    _ENUM_CACHE[key] = t
    return t


def enum_label(field, value, era):
    t = enum_table(field, era)
    if t is None or not isinstance(value, int):
        return None
    return t.get(value)


def enum_parse(field, text, era):
    """'IT_WEAPON' / 'weapon' / '1' -> 1 for enum fields; None if the field has no table or no match."""
    t = enum_table(field, era)
    if t is None:
        return None
    want = text.strip().upper()
    hits = [v for v, n in t.items() if n == want or n.split("_", 1)[-1] == want]
    if len(hits) == 1:
        return hits[0]
    if len(hits) > 1:
        raise SystemExit("%s=%s is ambiguous: %s" % (field, text, ", ".join(t[h] for h in hits)))
    return None


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
        path = resolve_kim(path)
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
        self.kinds = {n: kind for n, _, kind in self.L["fields"]}
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
    def _rec_struct(self):
        """One Struct for the whole record (fields in offset order, gaps padded); same values as
        unpacking field by field, several times faster over a full-table scan."""
        if "_struct" not in self.L:
            fmt, pos, names = "<", 0, []
            for name, off, kind in sorted(self.L["fields"], key=lambda f: f[1]):
                fmt += "x" * (off - pos) + ("I" if kind in "sP" else kind)
                pos = off + struct.calcsize("<" + ("I" if kind in "sP" else kind))
                names.append(name)
            order = [n for n, _, _ in self.L["fields"]]
            self.L["_struct"] = (struct.Struct(fmt), [names.index(n) for n in order], order)
        return self.L["_struct"]

    def raw(self, item_id):
        base = self.rec_base(item_id)
        if base is None:
            return None
        st, idx, order = self._rec_struct()
        vals = st.unpack_from(self.b, base)
        return collections.OrderedDict((n, vals[i]) for n, i in zip(order, idx))

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
        k.path, k.L["era"], k.stride, k.n, k.nsets, len(k.b)))
    ids = k.ids()
    if ids:
        out("ids %d .. %d" % (ids[0], ids[-1]))
    out("md5 %s" % hashlib.md5(k.b).hexdigest())


def cmd_dump(a):
    k = Kim(a.kim)
    out("# %s: %s layout, stride %d" % (k.path, k.L["era"], k.stride))
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
                lab = enum_label(key, v, k.L["era"])
                out("  %s = %s%s" % (key, v, "  (%s)" % lab if lab else ""))
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


STAT_NAMES = {188: ["hp", "atk_physic", "atk_magic", "def_physic", "def_magic"],
              228: ["hp", "atk_physic", "atk_magic", "slot3", "slot4", "def_physic", "def_magic"]}
STAT_REL_NAMES = ["hp", "atk_physic", "atk_magic", "def_physic", "def_magic", "all_stat"]
# Derived names that are aliases of a flag bit, per layout (see Kim.record).
DERIVED_BITS = {
    188: {"hide_set_desc": ("flags", 7), "pvp_item": ("flags2", 0)},
    228: {"hide_set_desc": ("flags2", 1), "pvp_item": ("flags2", 0), "is_for_minor_unit": ("flags2", 2),
          "pvp_equip": ("flags", 7)},
}


def field_values(k, r, field, decode_strings=True):
    """Every value of `field` in raw record r, as a list (pool fields can hold several, or none).
    The one field namespace shared by `census --field` and `find --where` / `--cols`."""
    fl, fl2 = r["flags"], r["flags2"]
    if field in r:
        if decode_strings and k.kinds[field] == "s":
            return [k.wstr(r[field])]
        return [r[field]]
    if field in dict(FLAG_NIBBLES):
        return [(fl >> dict(FLAG_NIBBLES)[field]) & 0xF]
    if field in dict(FLAG_BITS):
        return [(fl >> dict(FLAG_BITS)[field]) & 1]
    if field in DERIVED_BITS[k.stride]:
        word, bit = DERIVED_BITS[k.stride][field]
        return [((fl if word == "flags" else fl2) >> bit) & 1]
    if field == "flags_bit7" or field == "hide_set_desc_flags_bit7":
        return [(fl >> 7) & 1]
    if field.startswith("flags2_bit"):
        return [(fl2 >> int(field[10:])) & 1]
    if field == "special_ability_type":
        o = r["ofs_special_abilities"]
        return [k.u32(o + 4 + 24 * j) for j in range(k.u32(o))] if o else []
    if field in ("extra_key", "slash_key"):
        o = r["ofs_extra"]
        cnt = sum(r[c] for c in ("n_frame_offsets", "n_render_scales", "n_render_rotates",
                                 "n_common_model", "n_common_xet"))
        if not o:
            return []
        if field == "extra_key":
            return [k.u32(o + 8 * j) for j in range(cnt)]
        return [k.u32(o + 8 * (cnt + j)) for j in range(r["n_slash_traces"])]
    if field.startswith("stat."):
        o, names, sub = r["ofs_stat"], STAT_NAMES[k.stride], field[5:]
        if sub not in names:
            raise SystemExit("unknown field %r in the %s layout (use --list-fields)" % (field, k.L["era"]))
        return [struct.unpack_from("<f", k.b, o + 4 * names.index(sub))[0]] if o else []
    if field.startswith("stat_rel_lv."):
        o, names, sub = r["ofs_stat_rel_lv"], STAT_REL_NAMES[:k.L["stat_rel"]], field[12:]
        if sub not in names:
            raise SystemExit("unknown field %r in the %s layout (use --list-fields)" % (field, k.L["era"]))
        return [k.b[o + names.index(sub)]] if o else []
    if field in ("n_buff_factors", "buff_factor"):
        o = r["ofs_buff_factors"]
        cnt = struct.unpack_from("<H", k.b, o)[0] if o else 0
        if field == "n_buff_factors":
            return [cnt]
        return list(struct.unpack_from("<%dH" % cnt, k.b, o + 2)) if cnt else []
    if field in ("socket_option", "random_socket_option"):
        o = r["ofs_" + field + "s"]
        return list(struct.unpack_from("<%di" % k.u32(o), k.b, o + 4)) if o else []
    if field in ("attached_particle", "attached_mesh"):
        o = r[{"attached_particle": "ofs_attached_particles", "attached_mesh": "ofs_attached_meshes"}[field]]
        if not o:
            return []
        return [k.wstr(k.u32(o + 4 + 8 * j) & 0x7FFFFFFF) for j in range(k.u32(o))]
    if field == "set_name":
        s = k.set_record(r["set_id"]) if r["set_id"] else None
        return [s["name"]] if s else []
    raise SystemExit("unknown field %r (use --list-fields)" % field)


def census_values(k, field):
    """Yield (id, value) for a census field, including derived and pool fields."""
    for item_id in k.ids():
        for v in field_values(k, k.raw(item_id), field, decode_strings=True):
            yield item_id, v


def census_fields(k):
    names = [n for n, _, _ in k.L["fields"]]
    names += [n for n, _ in FLAG_BITS] + [n for n, _ in FLAG_NIBBLES]
    names += sorted(DERIVED_BITS[k.stride])
    names += ["flags_bit7", "flags2_bit0", "flags2_bit1", "flags2_bit2", "flags2_bit3",
              "special_ability_type", "extra_key", "slash_key", "n_buff_factors", "buff_factor",
              "socket_option", "random_socket_option", "attached_particle", "attached_mesh", "set_name"]
    names += ["stat." + n for n in STAT_NAMES[k.stride]]
    names += ["stat_rel_lv." + n for n in STAT_REL_NAMES[:k.L["stat_rel"]]]
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
        k.path, k.L["era"], k.stride, a.field, sum(hist.values()), len(hist)))
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


WHERE_RE = re.compile(r"^\s*([\w.]+)\s*(!=|!~|<=|>=|=|~|<|>)\s*(.*?)\s*$")
DEFAULT_COLS = "id,name,item_type,item_grade,unit_class,eqip_position,use_level"


def parse_where(k, expr):
    m = WHERE_RE.match(expr)
    if not m:
        raise SystemExit("bad --where %r: want FIELD OP VALUE, OP one of = != < <= > >= ~ !~" % expr)
    field, op, text = m.groups()
    if field not in census_fields(k):
        raise SystemExit("unknown field %r in the %s layout (kim.py census --list-fields)" % (field, k.L["era"]))
    num = enum_parse(field, text, k.L["era"]) if op not in ("~", "!~") else None
    if num is None:
        try:
            num = int(text, 0)
        except ValueError:
            try:
                num = float(text)
            except ValueError:
                num = None
    if num is None and enum_table(field, k.L["era"]) is not None and op not in ("~", "!~"):
        raise SystemExit("%s=%s: not a number or a known %s name" % (field, text, field))
    return field, op, text, num


def _cmp(v, op, text, num):
    if isinstance(v, str) or num is None or op in ("~", "!~"):
        a, b = (v if isinstance(v, str) else str(v)).lower(), text.lower()
        if op in ("~", "!~"):
            return b in a
        if op in ("=", "!="):
            return a == b
    else:
        a, b = (round(v, 4) if isinstance(v, float) else v), num
        if op in ("=", "!="):
            return a == b
    return {"<": a < b, "<=": a <= b, ">": a > b, ">=": a >= b}[op]


def where_ok(vals, cond):
    """Multi-valued fields match if ANY element matches; != and !~ mean NO element matches."""
    field, op, text, num = cond
    if op in ("!=", "!~"):
        return not any(_cmp(v, op, text, num) for v in vals)
    return any(_cmp(v, op, text, num) for v in vals)


def fmt_cell(k, field, vals, raw=False, width=0):
    parts = []
    for v in vals:
        lab = None if raw else enum_label(field, v, k.L["era"])
        if lab:
            parts.append(lab)
        elif isinstance(v, float):
            f = round(v, 4) if math.isfinite(v) else v
            parts.append("%d" % f if math.isfinite(f) and f == int(f) else repr(f))
        else:
            parts.append(str(v).replace("\r", " ").replace("\n", " ").replace("\t", " "))
    s = ",".join(parts)
    if width and len(s) > width:
        s = s[: width - 3] + "..."
    return s


def cmd_find(a):
    k = Kim(a.kim)
    conds = [parse_where(k, w) for w in a.where]
    name_re = re.compile(a.re, re.I) if a.re else None
    name_sub = a.name.lower() if a.name else None
    cols = [c.strip() for c in a.cols.split(",") if c.strip()]
    for c in cols:
        if c not in census_fields(k):
            raise SystemExit("unknown column %r in the %s layout (kim.py census --list-fields)" % (c, k.L["era"]))
    ids = parse_ids(a.ids) if a.ids else k.ids()
    hits, missing = [], []
    for item_id in ids:
        r = k.raw(item_id)
        if r is None:
            missing.append(item_id)
            continue
        if name_sub or name_re:
            nm = k.wstr(r["name"])
            if name_sub and name_sub not in nm.lower():
                continue
            if name_re and not name_re.search(nm):
                continue
        if all(where_ok(field_values(k, r, c[0]), c) for c in conds):
            hits.append(item_id)
    if a.count:
        out(str(len(hits)))
        return
    if a.sort:
        key, rev = (a.sort[1:], True) if a.sort.startswith("-") else (a.sort, a.desc)
        if key not in census_fields(k):
            raise SystemExit("unknown --sort field %r" % key)

        def sk(i):
            v = field_values(k, k.raw(i), key)
            return (0, v[0]) if v else (1, 0)
        hits.sort(key=sk, reverse=rev)
    shown = hits if not a.limit else hits[: a.limit]
    rows = []
    for i in shown:
        r = k.raw(i)
        rows.append([field_values(k, r, c) for c in cols])
    if a.json:
        doc = []
        for row in rows:
            d = collections.OrderedDict()
            for c, vals in zip(cols, row):
                d[c] = vals[0] if len(vals) == 1 else (None if not vals else vals)
                labs = [enum_label(c, v, k.L["era"]) for v in vals]
                if any(labs):
                    d[c + "_name"] = labs[0] if len(labs) == 1 else labs
            doc.append(d)
        out(json.dumps(doc, ensure_ascii=False, indent=1))
    elif a.tsv:
        out("\t".join(cols))
        for row in rows:
            out("\t".join(fmt_cell(k, c, v, a.raw) for c, v in zip(cols, row)))
    else:
        cells = [[fmt_cell(k, c, v, a.raw, a.width) for c, v in zip(cols, row)] for row in rows]
        w = [max([len(c)] + [len(r[j]) for r in cells]) for j, c in enumerate(cols)]
        out("  ".join(c.ljust(w[j]) for j, c in enumerate(cols)).rstrip())
        for r in cells:
            out("  ".join(x.ljust(w[j]) for j, x in enumerate(r)).rstrip())
    for i in missing:
        sys.stderr.write("%d: not in this kim\n" % i)
    summary = "# %d matched of %d scanned, showing %d%s (%s layout, %s)" % (
        len(hits), len(ids), len(shown), "" if len(shown) == len(hits) else " (--limit 0 for all)",
        k.L["era"], k.path)
    if a.json or a.tsv:
        sys.stderr.write(summary + "\n")  # keep stdout machine-readable
    else:
        out(summary)


def cmd_set(a):
    k = Kim(a.kim)
    set_ids = list(struct.unpack_from("<%dI" % k.nsets, k.b, k.setid_ofs)) if k.nsets else []
    want = [int(t) for t in a.query if t.isdigit()]
    words = " ".join(t for t in a.query if not t.isdigit()).lower()
    if words:
        want += [s for s in set_ids if words in k.set_record(s)["name"].lower()]
    if not want:
        raise SystemExit("give set ids and/or words from a set name")
    for n, sid in enumerate(want):
        if a.limit and n >= a.limit:
            out("... %d more sets (--limit 0 for all)" % (len(want) - n))
            break
        s = k.set_record(sid)
        if s is None:
            out("set %d: not in this kim" % sid)
            continue
        out("== set %d  %s  max_level=%d" % (sid, s["name"], s["max_level"]))
        for parts, opt in s["need_parts_and_option"]:
            out("  %d parts -> socket option %d" % (parts, opt))
        for i in s["items"]:
            r = k.raw(i)
            if r is None:
                out("  %d  (not in the item table)" % i)
                continue
            out("  %d  %s  [%s, %s]" % (i, k.wstr(r["name"]),
                                       fmt_cell(k, "eqip_position", field_values(k, r, "eqip_position")),
                                       fmt_cell(k, "unit_class", field_values(k, r, "unit_class"))))


def cmd_kims(a):
    for name, (rel, what) in KIM_ALIASES.items():
        path = resolve_kim(name)
        if not os.path.isfile(path):
            out("%-14s MISSING  %s" % (name, path))
            continue
        k = Kim(path)
        out("%-14s %-5s %6d items %5d sets  %s\n%14s %s" % (name, k.L["era"], k.n, k.nsets, what, "", path))


SINGLE_KIM_CMDS = ("info", "dump", "census", "assets", "find", "set")


def default_kim(argv):
    """Let single-kim commands omit <kim>: insert 'staged' when the next token is not a kim."""
    if argv and argv[0] in SINGLE_KIM_CMDS:
        nxt = argv[1] if len(argv) > 1 else None
        if nxt is None or not is_kim_spec(nxt):
            if nxt and (nxt.lower().endswith(".kim") or "/" in nxt or "\\" in nxt):
                raise SystemExit("%s: no such kim file (aliases: %s)" % (nxt, ", ".join(KIM_ALIASES)))
            argv = [argv[0], "staged"] + argv[1:]
    return argv


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
    s = sub.add_parser("find"); s.add_argument("kim")
    s.add_argument("--name", help="case-insensitive substring of the item name")
    s.add_argument("--re", help="regex on the item name (case-insensitive)")
    s.add_argument("--where", action="append", default=[], metavar="FIELD<op>VALUE",
                   help="repeatable, all must hold; op = != < <= > >= ~ !~")
    s.add_argument("--ids", help="only these ids: file (first column) or comma/space list")
    s.add_argument("--cols", default=DEFAULT_COLS, help="columns (default %s)" % DEFAULT_COLS)
    s.add_argument("--sort", help="sort by FIELD (--sort=-FIELD or --desc for descending)")
    s.add_argument("--desc", action="store_true", help="with --sort: descending")
    s.add_argument("--limit", type=int, default=50, help="max rows shown (0 = all; default 50)")
    s.add_argument("--width", type=int, default=48, help="truncate table cells (0 = never)")
    s.add_argument("--raw", action="store_true", help="numbers, no enum names")
    g = s.add_mutually_exclusive_group()
    g.add_argument("--count", action="store_true", help="print only the match count")
    g.add_argument("--tsv", action="store_true")
    g.add_argument("--json", action="store_true")
    s = sub.add_parser("set"); s.add_argument("kim"); s.add_argument("query", nargs="+")
    s.add_argument("--limit", type=int, default=20, help="max sets shown (0 = all)")
    sub.add_parser("kims")
    argv = default_kim(list(sys.argv[1:] if argv is None else argv))
    a = p.parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    fn = {"info": cmd_info, "dump": cmd_dump, "diff": cmd_diff, "census": cmd_census,
          "assets": cmd_assets, "find": cmd_find, "set": cmd_set, "kims": cmd_kims}.get(a.cmd)
    if not fn:
        p.print_help()
        return 2
    fn(a)
    return 0


if __name__ == "__main__":
    sys.exit(main())
