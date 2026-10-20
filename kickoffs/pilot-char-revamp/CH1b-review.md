# June character Aisha (Stage 5 pilot) - Phase CH1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1b review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1b - Done"
awk -F'\t' '$6=="CH1b"' june2017_re/char_arme/ch0_class_map.tsv      # the 38 June functions
grep -a "Char CH1b" june2017_re/funcmap.tsv                            # the rows the phase wrote (38 ported + 3 stand-ins)
```

The `June_*` members of CH1b are in the second block (`//{{ ... Phase CH1b`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1a's in the header; two new members of
`ARME_VIOLEMAGE_STATE_ID` (`AVSI_TELEPORT_STAND_UP_START_UPGRADE`, `AVSI_TELEPORT_STAND_UP_ATTACK_UPGRADE`) sit after `AVSI_TRANSFORMED`.

## Task

Review the transcription against June, function by function. **Delegate the mundane reads** (x2-phase, *When the phase is Opus*): the Ghidra dumps of the 38 functions go to the scratchpad, you read
the parts below. What the phase could get wrong:

- **Branch structure behind the decompiler.** `32cc6520` / `32cc66a0` / `32cc6820` (the `LAB_` gotos: the `[4]` / `[1]` / `[2]` / `[3]` flag order and which state follows each), `32ccc5a0` (the two XOR-protected
  times it tests, read here as `m_EnchantShock` then `m_Frozen`: confirm the offsets `+0x2480` / `+0x1730` against `CX2GameUnit`'s ExtraDamagePack layout), `32cd5360`, `32ce12c0` / `32ccd4a0`. Use `june2017_re/funcdis.py --func <va>`.
- **`32cd5360`**: the centre sequence of Air Blast now comes from the Major particle system (`322ceaa0`); confirm which handle member `param_1[0x260a]` / the second argument is, and that the ring stays on `322ceab0`.
- **Compare directions and constants** (`June_*` uses `charport_consts.py CH1b`): the `0.26 / 0.4` window of `32cc6ca0`, `0.66` of the three event processes, `0.28 < t < 0.7` of `32cd5360`, the mana chance table 0.25 / 0.5 / 1.0.
- **The decisions the phase could not make** (all need Opus):
  1. `32011d20` / `31fd66b0` / `32011f50` (June-only awakening effect-set family, unit `+0x9578` map, handle `+0x9f4`): file-local stand-ins in `X2GUArme_VioletMage.cpp` with June's empty-map default.
     Decide the owner (CH1, or its own plan under the scope rule; the family has no March member, funcmap row `32296cb0`).
  2. `32cb8b60` (vslot `0x60c`) and `32ccda40` (vslot `0x608`) are Aisha's own virtuals (CH1i); `June_Vslot60c` / `June_Vslot608` are empty until then.
  3. `June_IsTeleportUpgradeAcquired` (`32cc5ae0`) reads `GetUserSkillTemplet( 22216, false ) != NULL` as `IsSkillAcquired 328914b0( id, 0 )`; the earlier phase ported it that way (`JuneIsSkillAcquired`). Confirm the unit path (`+0x867c` -> CX2Unit -> user skill tree `+0x494`).
  4. The two `_UPGRADE` state ids (242 / 243) are enum members only; CH2a registers the states and CH1 decides whether March's numbering is kept (`AVSI_*` are symbolic here).
  5. `JUNE_BUGS.md` row 371 (June registers the Die FrameMove for the riding attack states): confirm against the registrar `32ce77b0` that it is not a ch0 mapping error.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1b commit>^..<CH1b commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1b -- Review" into the plan doc under the Done note, update the CH1b row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1c.md` still matches, and commit with explicit pathspecs.
