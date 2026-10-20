# June character Aisha (Stage 5 pilot) - Phase CH1j2 - The five untranscribed callbacks

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1j2` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1j review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert until the final CH1) |
| Review | `kickoffs/pilot-char-revamp/CH1j2-review.md` (the phase writes it) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1j - Done (2026-10-21, Opus)" "CH1e - Done (2026-10-20, Sonnet 5.5)" "CH1f - Done (2026-10-20, Sonnet 5.5)"
grep -a "32cbca20\|32cc3040\|32ce7170\|32cc42a0\|32ce2cc0" june2017_re/char_arme/ch0_state_skill.tsv june2017_re/funcmap.tsv
```

## Task

Transcribe five June class callbacks as inert `June_*` members, the CH1e / CH1f way (one-line comments with the June address, June's constants,
`JUNE_ARME_*` key macros, `m_pXSkinAnimFuture` for the skin):

| June | State (registered by `June_InitState`) | Slot | Today |
|---|---|---|---|
| `32cbca20` | `AVSI_HIGH_MAGICIAN_COMBO_X2` | EventProcess | stand-in: March's `AVSI_HIGH_MAGICIAN_COMBO_X2_EventProcess` |
| `32cc3040` | `AVSI_AHM_DASH_JUMP_ZXX_FINISH` | FrameMove | empty |
| `32ce7170` | `AVSI_AEM_DASH_JUMP_X3` | FrameMove | empty |
| `32cc42a0` | `AVSI_AVP_DASH_ATTACK_X2` | EventProcess | empty |
| `32ce2cc0` | `AVSI_AVP_DASH_ATTACK_X2` | FrameMove | empty |

None has a Ghidra function: read them with `python june2017_re/funcdis.py --func <va>` (its bounds can merge a neighbour; stop at the first `ret`).
Then point the registration in `June_InitState` at the new members, replacing the stand-in and the three empty-slot comment lines. Edit with
`bytepatch.py`; the file is CP949. Add funcmap rows (`Char CH1j2`, PORTED) and flip the five `ROUTED` rows. A June quirk gets a JUNE_BUGS row.
Finish as usual: flagcheck, `build.py --deploy`, Done note, STATUS row, the CH1j2 review kickoff.
