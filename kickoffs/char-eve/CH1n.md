# June character Eve (Stage 7) - Phase CH1n - Base callbacks and the slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1n` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (undefined until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-eve/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1 split (2026-10-21, user)"
```
Then CH0's Done note, the earlier CH1 Done notes / reviews (their routed gaps), and the `CH1n` rows of `june2017_re/char_eve/ch0_*.tsv`. Aisha's matching sub-phase (CH1j / CH1k / CH1l / CH1m) is the worked example.

## Task

The CX2GUUser base callbacks Eve registers that CH1m left on stand-ins (Aisha's CH1k ported several into CX2GUUser: check `JUNE_CHARACTER_AISHA_PHASES.md` "CH1k - Done (2026-10-21, Opus)" first and reuse). `SetEquippedSkillFuncBySkillSlotIndex` `329c8a00` head: bounds, the two slot tables, the 7i rule (June's `31fd2af0` snapshot call, as Aisha handled it), the upgrade-id switch, and the dev free-skill gate `JuneIsDevUnlimitedSkillUse()` around June's cool-time / MP / hyper check (standing rule). Under `_LIVE`.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GuEve.*` / `X2GUUser.*` are CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit (the `SKILL_SLOT_10` / `UPGRADE_SKILL_SYSTEM_2013` pairs). Done gate: the build passes with `_LIVE` undefined, and a `cl /P` probe with it defined compiles the new path; no play-test.

## Amended by CH0 (2026-10-26)

- **Functions: 2** (`awk -F'\t' '$6=="CH1n"' june2017_re/char_eve/ch0_class_map.tsv`; helper 1, override 1).
  June addresses: `329c8a00` `329cc2c0`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L257 (dev free-skill gate in the skill-use check); §7b L318-319 (31fdbbb0 / 31fdbcc0 memo readers); §7i L413-428 (SetEquippedSkillFuncBySkillSlotIndex drops 31fd2af0, sets +0).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `32007910` user (candidate).
- **`329c8a00` shape:** bounds 0..4; slot tables `334c9e00` / `334c9e14` (normal A / B) and `334c9e28` / `334c9e3c` (hyper A / B); `31fd2af0` (7i: dropped at the port), `+0x11 = 1`. Then a transcendence-memo switch on `31fdbcc0( id )` (25033 / 25070, 25034 / 25071, 25035-36 / 25072-73, 25217 / 25234), a type switch (5 = `ENSI_SI1_COMMON_BUFF`; 6 / 7 / 13-16 with id > 25600 = the hyper actives, `== 25600` Lunatic Scud; 12 + id 900 = `ENSI_THROW_ITEM`), then the id switch. **The `default:` plays `ENSI_A_EAT_FAENA`**: Fayner (25224 / 25225) has no case and arrives there. Epilogue label `LAB_329caa72`.
- **Extra states a case registers itself** (`ch0_skill.tsv` column 10): 205 / 206 Burst Spear landing, 211-213 Sweep Rolling, 242 Reason Pride action; 239-241 Electronic Move take every callback of states 219-221 through `329cc2c0` (Aisha's `June_CopyStateCallbacks`: reuse it).
- **Dev free-skill gate:** in the vslot 510 copy `329b5af0` (CH1l3 transcribes it, CH1n wires the gate).
