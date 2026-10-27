# June character Eve (Stage 7) - Phase CH1p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1p review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1p done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (Eve, insert-only, undefined until CH1 final); `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (the Aisha Hell Drop fix, live) |
| Log tag | none (Eve's code runs only under `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1p - Done (2026-10-27)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ag. Pressed-index space, the fifth slot and the A5 / B5 states (from June skill system S5)"
grep -a -E "^(329a7f50|329b1e00|32cd9d00)" june2017_re/funcmap.tsv
```

## Task

Review against June. **Delegate the mundane reads**: raw listings (`disasm_range.py`) of `32981130` @329815e0-32981610, `32cd9d00` @32cd9d00-32cd9d80 with the jump table `32cda1b0` (9 dwords), `324d9550` @324d9676-324d97c5, `329b1e00`, `329a7f50`; a byte scan of the Eve range `32965dd0`-`329cf86f` for the `0x8ac0` displacement; and a `cl /P` of `X2GuEve.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (`census.cl_cmd`). You decide:

- **The sweep.** Is every LIVE-surviving reader of a slot id, the pressed index or `m_eSpecialAttackKeyPressed` in `X2GuEve.cpp` either June's 0-9, dead (no LIVE caller), or converted? The Done note says the March `GetSkillSlotData*` / `IsUpSkillSlot*` have no LIVE caller and `CommonEventProcess` / `InitInpuDataProcess` / `InitEquippedActiveSkillState` are in `#else`.
- **`JuneEveIsSkillKeyUp`.** Value - 1 = slot id; the F / T keys are `pureSkill5` / `pureSkillEx5`, and "no slot" is not "up" (March's semantics for an unmatched value). Are both converted conditions March's otherwise (the `m_fEffectStartTime >= 3.0f` term kept)?
- **Aisha fix.** The cases match June's table (value 1-4 = A S D C, 5 = up, 6-9 = Q W E R, else up), and `SKILL_SLOT_B1..B4` = 5-8 with `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` on. Are there other Aisha (or other LIVE class) readers of `m_eSpecialAttackKeyPressed` with March labels? (`X2GUArme_VioletMage.cpp:16829` is March's own Hell Drop: check it is dead under `ARME_LIVE`.)
- **Hooks.** `324d97a0-324d97be`'s gate (my unit, `+0x867c`, `+0xaf0`, type 5) is March's `DungeonStageLoadNot` UT_EVE block, at the same place; `329b1e00` = `June_Reset( false, &future )`, `329a7f50` = March's Taser reset.
- **7h / 7l / 7o / 7p / 7x and the keys.** The Done note's verdicts, and the `+0x9c60` consumers (the three inlined state copies use the CH1a helper in the port).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE --range <CH1p commit>^..<CH1p commit>` (the "not #defined" FAIL is by design until CH1 final) and the same for `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (9 deleted = the replaced switch and case lines); paste the `files:` / `in this flag's files:` / `RESULT:` lines. `enumscan --check` on the two code files; `/Zs` of `X2GuEve.cpp` with and without LIVE, and of `X2GUArme_VioletMage.cpp`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1p - Review" into the plan doc under the Done note, update the CH1p row in `STATUS.md`, check that `kickoffs/char-eve/CH1q.md` still matches, and commit with explicit pathspecs.
