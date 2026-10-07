# June effect completion - C9g - the unit viewer's effect groups 1-4 and its group stops (identify)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9g` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9c-2b review (it found the gap) |
| Flag | none (no code; the port phase this one plans gets its own) |
| Log tag | none |
| Review | none (identify only); this phase writes the port kickoff `C9g-2.md` and its STATUS row, or routes the work to a backlog stage |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9c-2b - Review"
```

Also read the `june2017_re/funcmap.tsv` rows `31fa4980`, `31fa5c90 @groups`, `31fac190`, `31fae1a0` and `329594c0` (`grep -n "^31fa4980\|^31fa5c90\|^31fac190\|^31fae1a0\|^329594c0" june2017_re/funcmap.tsv`).

## Task

C9c-2b ported June's unit-viewer effect player and its group 0 (the `UnitViewerEffectSetTable.lua` rows). C9e plays group 1 (the per-emotion sets).
June also uses groups 1-4 for other viewer effects and stops them from places March does not have. No plan owns that (user rule 2026-10-07: an
unowned June branch gets its own phase and follows June). What the review already has:
- **`31fa4980`** (likely June's `DeleteEnchantWeaponEffect`): resets four 4-name string arrays and `+0x1174/+0x1176/+0x11c0/+0x11cc`, then stops
  groups **0..4**. Callers: `31fa89b0` (arg 1), `31faa150` (an equip refresh: `31fa89b0(2)`, then this when the `+0x1e0` unit is set),
  `32d7c7d0`, `32d7e755`. March's `DeleteEnchantWeaponEffect` (`X2UnitViewerUI.cpp:5525`) destroys the seven `m_hSeqWeapon_*` handles instead.
- **`PlayByMotionType` `31fa5c90`**: unless the motion type is 6, 7 or 8 (or 1-2 with `+0xf74` set), it stops `0/1/2` when `+0xf14 == 1`,
  else groups `0..4` (`31fac190`). `+0xf14` and `+0xf74` are unidentified.
- **`31fa5140` / `31fa5180`**: stop groups 1 and 2 and call `31fa4ec0( 4|5, emotion, 1|2 )` on the sit emotions (`+0xe5c` 1, 0x1a, 0x37, 0x3a).
  C9d put June's sit-item effects in backlog Stage 10 (the emotion system); check whether these belong there.

Identify, in Ghidra, read-only:
1. **What plays into groups 1-4**: every `32959640` caller with a group other than 0 (`xrefs_to 32959640`), the set names (from the four string
   arrays `31fa4980` resets), and what each group means (enchant attribute? upgrade? sit item?).
2. **The stop sites**: `31fa4980`'s callers mapped to March functions (is `31fa89b0` March's enchant-type change in `OnFrameMove`, `:874`?), and
   `+0xf14` / `+0xf74` in `PlayByMotionType`.
3. **March's side**: which March handles (`m_hSeqWeapon_*`, `m_hSeqUpgradeWeapon*`, emblem) June replaced, and what retiring them would lose.
4. **Reach**: which items, enchant attributes or emotions reach each group in March data and on the save (`sqlite3 els_db.sql`).

Done = funcmap rows confirmed or corrected (tag `Effect C9g`), the port list in the Done note, and either `kickoffs/june-effect/C9g-2.md` + its
STATUS row (the port: Sonnet 5.5 high, its own flag, the group stops in `PlayByMotionType` and the enchant reset beside C9c-2b's player), or a
routing line to backlog Stage 10 for the parts that are the emotion system. Hand a design question to the user only if copying June would break
March (for example retiring a working March enchant effect that June's data does not replace).
