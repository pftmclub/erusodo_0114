# June character Eve (Stage 7) - Phase CH1 - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1q review; CH0b done (its koms copied, or held for this phase if CH0 found a dropped animation March plays) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (defined here), `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | `[JUNE_CHAR]` (`EVE DATA`, `EVE CORE`, `EVE NCORE`) |
| Review | `kickoffs/char-eve/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
```
Then the CH1a-CH1q Done notes and reviews.

## Task

1. Define `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` in `Always.h`; `flagcheck.py JUNE_CHAR_EVE_LIVE`.
2. Adopt `Eve_Nasod.lua` (`luasrc.py adopt-june data036 Eve_Nasod.lua`, `--dry-run` first) and the skill NPC / summon scripts CH0 marked CH1 (planning list: `Atomic_Shield_Eve`, `Guardian_Eve`, `Guardian_Sp1_Eve`, `Guardian_Sp2_Eve`, `Nasod_Watch_Eve`, `Nasod_Element_Fire_Eve`, `Wally9th_Eve`, `SI_HA_FERDINAND`, `SI_SA_EEP_ELECTRONIC_FIELD`, and their `_Memo` files). A `[JUNE_CHAR] EVE DATA` line logs the script load. Re-run `charport_scan.py` and `charport_keys.py Eve_Nasod`: nothing new.
3. Build, pack, ask before each copy (with CH0b's held koms if any), then hand over the play-test: an Eve save per job tier; combos, dash, jump, riding, a bounding hit, damage, awakening with the Nasod Core; the `ErrorLog.txt` lines to compare against a pre-CH1 run.
- Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` on the wait-state-change table (7h).
- Mark the ported checklist rows `(Eve: CH1)`.
