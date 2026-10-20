# June character Aisha (Stage 5 pilot) - Phase CH1m - Pressed-index space, hooks and script keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1m` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1l review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`; behaviour changes behind `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` |
| Log tag | none (inert until the final CH1) |
| Review | `kickoffs/pilot-char-revamp/CH1m-review.md` (the phase writes it) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ag. Pressed-index space, the fifth slot and the A5 / B5 states (from June skill system S5)" "7l. Super-armour and bounding hooks that only class code drives (from June effect completion C3b)" "7o. Fall-speed limit data and the minor unit's physics (from June effect completion C3g)" "7p. Hit-stop class sites and the time-stop immune state (from June effect completion C3e-3)" "7x. Combo hits must carry June's AT_COMMAND tag (from Atlas E10)"
grep -a "ch1\|CH1" june2017_re/char_arme/ch0_keys.tsv
```
Then `kickoffs/pilot-char-revamp/CH1.md` items 4 and 5 and its "From CH1e" note (`SPECIAL_OPTION_VALUE`).

## Task

1. **`CH1.md` item 4 (7ag).** Convert the class decode (`:5232-5257`, `:5072`, the consumer `:16610`; re-find the lines, they have moved) to June's
   A 0-4 / B 5-9, with June's 4 (F) / 9 (T) presses: the slot-5 gap at `X2GUUser.cpp:25500-25510`, inert until Stage 6. `CommonSpecialAttackEventProcess`
   is shared by every class. Decide how Aisha alone gets June's space, and write the design call into the Done note.
2. **`CH1.md` item 5.** The bounding and wait-state hooks are registered since CH1j. Run C3d-3's review legs for 7l. Then the 7 June-only script
   keys of `ch0_keys.tsv`: confirm each against March's preprocessed reader, and port the missing reader or route it. `SPECIAL_OPTION_VALUE` gets
   parsed in `CommonStateStart` and the `June_GetSpecialOptionValue` stand-in points at the user list. Also cover `SET_LIMIT_MAX_G_SPEED` (7o),
   the 12 `TIME_STOP_IMMUNE_STATE` states (7p) and `AT_COMMAND` (7x).
