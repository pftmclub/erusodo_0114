# June character Chung (Stage 7) - Phase CH1l review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` / `_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_ENUM_NAMES`, `SERV_IRUHADEV_JUNE_DIVERGE_SCALE_HEAD` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1l - Done (2026-11-01)"
grep -a "Char Chung CH1l:" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH1l --out <scratch>     # June listings, 16 rows (2 are n/a)
```

## Task

Check each CH1l transcription against June in Ghidra (read-only). In order of risk:
- **`SetEquippedSkillLevel` `32bdbe50`:** the 18 blocks in June's order, each skill id against `Enum.src.lua` (`RNSI_*` values were compile-probed, the
  *state* ids are mapped by name from `ch0_reg.tsv` / `ch0_skill.tsv`: re-check the state-id -> name map, above all `0x111`-`0x113`, `0x12a` / `0x12b`
  (Guard ready / ready memo), `0x141` / `0x142`, `0x150` (336), `0x151` / `0x152`). The Guard block: the level, the `SA_DAMAGED_REL` percent, the new
  `SA_MP_CONSUMPTION` write into the guard object (+0x14, default -4 from `32c24400`), the early return on a missing unit. Is `SetEquippedSkillLevelStateData` (March's
  per-class wrapper) really what vslot 0x7a8 `32028190` does for Chung?
- **`ReplaceWeapon` `32bef140`** (no `GetItem()` test; `+0x194` / `+0x198` = `GetEnchantLevel()` / `GetItemTemplet()`), **`SetShowViewList` `32bf0a40`**
  (the `+0x93da` stand-in: confirm it is the only difference and that Stage 17 lists it).
- **The five `_CORE` functions:** `IsInJobLine` (class ids), `GetIncreaseSoulRateOnDamageReact` (`SA_SOUL_ABS` = June 170, skill 26415; name), the two gauge snapshot
  functions (members `+0x92c0` / `+0x92c4` belong to the `CX2GUUser` snapshot; is leaving out the base call safe until the Crest of Time port?), `UpdateSpineAimRotation`
  (the other bone axis against `ApplySpineAimRotation`, x87 compare idioms).
- **`ScaleHead` `32bf0480`** under `_LIVE`: the bone choice (`+0x8fc0` && !hyper), the 31fd64a0 body-0 copy.
- The `= March` rows: spot-check three. State `336`: is `CSI_SI_SA_CSG_DUAL_BUSTER_UPGRADE` the right home, or should CH1m name it differently?

Hygiene: `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES JUNE_DIVERGE_SCALE_HEAD --range <CH1l commit>^..<CH1l commit>`,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.*`, both one-file probes. Write "CH1l - Review" into the plan doc, update the STATUS row,
check `kickoffs/char-chung/CH1l2.md` and commit with explicit pathspecs.
