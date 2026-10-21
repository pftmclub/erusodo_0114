# June character Aisha (Stage 5 pilot) - Phase CH4d - Memorize: HUD, dialog statics, tree UI, decide ACK

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4d` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH4c review; not alongside CH4b / CH4c (same class files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` |
| Log tag | `[JUNE_CHAR]` (`ARME MEMO`) |
| Review | `kickoffs/pilot-char-revamp/CH4d-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH4 split (2026-10-22, Opus)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ah. The 10-slot dialog's class statics and the key-config rows (from June skill system S6)" "7aj. Tree-UI class pieces June keyed to one class (from June skill system S7b)" "7ak. Skill page decide ACK class pieces (from June skill system S7d)"
grep -a "^322deb80\|^322f4740\|^322cc030\|^32ce1770\|^32ce1650\|^328e9160" june2017_re/funcmap.tsv
```
`CH4.md` ("From the CH1h review", "From CH1i") has the memorize notes.

## Task

Map each June piece in Ghidra before porting it, then port whole:
- the CX2Game memorize-slot HUD `322deb80` (replaces the no-op `June_GameRefreshMemoRise`; callers `328e0ff0`, `32ce1770`, `32ccf4e0`, `32ce16d3`,
  `32ce1900`, `32ceca70`, `31f5ea00`) and its per-frame pass `322f4740` (the `+0xb38` timer; `June_SetGameFloatB38` `322cc030` becomes real;
  0x718's other callers `322cf810` / `322e1bb3`);
- the 10-slot dialog's memorize statics (7ah: `Static_Memorize_Skill_Slot_TypeA1-3` / `B1-3`, `Memorize_Slot1-3_TypeA/B`, `UseMemorizedSkillHotKey_A/B`);
- the tree UI (7aj): slot data type 5 `328d9ac0` (and 6 `328e0f10` if Aisha's), `SESAPT_MEMORIZE` picture `328e0460` case 2, mouse-in 29059
  `328ec719`, the memorize tooltip line in `328e9160` `@328e9710` (needs the templet `+0x170` field), class helper `32ce1650` from `328dfaa0`;
- the decide-page ACK (7ak): `32ce1770( &true )` @`3220f4dc` (CH1i ported the sweep).

Done gate: memorize stores and casts a skill as June does, the HUD slots show the stored skills. Delegate the mechanical reads, builds and log
reads to subagents. Edit CP949 files with `bytepatch.py` only.
