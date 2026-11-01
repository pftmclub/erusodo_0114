# June character Chung (Stage 7) - Phase CH1l3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l3 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l3 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` / `_LIVE` (`//#define`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1l3 - Done (2026-11-01)"
grep -a "Char Chung CH1l3:" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH1l3
```

## Task

Check each row against June's listing (read-only). In order of risk:
- **`32c17830` lookup:** order of the checks, the string ids (0x8bae 226 2549 10225 26119), the `> 4` / `- 5` slot split into `GetCBConsumption`, the 0x6a8 call on the slot's cool time, `+0x97a2` (`m_bInStateMobileArmoredMode`) -> only Back Blast 26200, vslot 0x4f4 = future state id (26 / 28) -> only Guard 26207, `+0x8d78` = quick-slot wait.
- **`32bee490` / `32bf5130` / `32bd8790`:** the saved values restored for use condition 3 / 5 (`+0x98` of the templet), `+0x8ac0` vs `+0x88bc` (`m_eSpecialAttackKeyPressed` / `m_iNowSpecialAttack`), the state tables inlined in `32bee490` (`33507b00`, 0x14-byte steps = Normal A, Hyper A, Normal B, Hyper B) against the shared `StateChangeBySkillSlot`.
- **`32bed5d0`:** the jump table ids (26020 26025 26036 26060 26203), 26228, the three exits. **`32bed560`:** `+0x110` / `+0x11c` / `+0x128` = now / max / extra in `CX2ChungGageData`; the namesake proof (callers `32bedf60` / `32bedfc0` / `32bee020`).
- **`32bed8c0`:** range `26233 < id <= 26235` (JUNE_BUGS #500), vslot 0x524 vs the Future state data `+0x7cb0` -> `+0xc`. **`32bf0800`:** cut-in copy order, event 0x8b3 data, hyper flag arg.
- `ReservedSkillStart` hook plumbing in `X2GUUser.h` / `.cpp` (the case where only `_CHUNG_LIVE` is defined).

Hygiene: `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range <CH1l3 commit>^..<CH1l3 commit>`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.*`, a one-file probe of `X2GUChung.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Write "CH1l3 - Review" into the plan doc, update the STATUS rows (CH1l4 becomes ready), check `kickoffs/char-chung/CH1l4.md` and commit with explicit pathspecs.
