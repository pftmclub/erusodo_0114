# June character Lu/Ciel (Stage 8) - Phase NU4 - Creation, starter items and the select screen

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel NU4` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | NU2 review, NU3 review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CREATE`, `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_STARTER_ITEMS` |
| Log tag | `[JUNE_CHAR]` (`LUCIEL CREATE`) (`python tools/logs.py --grep "JUNE_CHAR LUCIEL"`) |
| Review | kickoffs/char-luciel/NU4-review.md (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase NU - New-character foundation (NU1-NU4)" "Phase NU0 - New-unit and two-body engine map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the NU4 row of "Phase NU". Starter rows (user, 2026-11-02): 15 rows, gear 113300-113304 plus both promotion sets 41890-41894 and
41900-41904 (`june2017_re/char_luciel/plan_starter_items.tsv`), a labelled stand-in whose comment says the live shape is ten. The stat-row
fallback never borrows another class's rows; until the user answers CH0's stat question it is the synthetic curve, logged once per class.
`tools/createchar.py` learns the seven classes (`CLASSES`, `PARENT`, `PROMO`). **The creation screen's Lu/Ciel entry is part of the gate**
(user, 2026-11-02): the slot, portrait, name, description, preview model and motions, select voice and job-preview buttons, all as NU0 item 4
maps them from June. Hand-off: open the creation screen, check each of those, create a Lu/Ciel, and see it listed
with its items (`/x2-db`), **and do not enter the game** (the class arrives in CH1).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by NU0

NU0 (2026-11-02), `june2017_re/char_luciel/nu0_create.tsv` (47 rows) and `nu0_type_switches.tsv` rows `NU4` (9):
- **June functions:** SelectLuciel `325559f0` (Add `32555320`); UICustomEventProc `32558860` (uncarved; table `3255aabc`, index msg - 1:
  `SCUUCM_LUCIEL_BUTTON_UP` 36 and `_JUMPING` 50 both call SelectLuciel, 50 also sets `+0x71c` = 1; re-select table `3255ab8c`, type 10
  `@325593da`); SelectUpperClass `325519d0`; ChangeCreateUnitClassChangeInfoDlg `325514e0`; class-change dialog loader `325525a0`; info texture
  `325506e0`; stand mesh `32551e30` (select screen `32561130`); OnFrameMove `3254f180`; InitDummyUnits `3255b800`; PlaySoundCreateUnit `32550aa0`;
  create request `325584f0`; select background `3255ecf0`; PlaySoundSelect `3255f240`; voice-name builder `32376270` (prefix switch `32375d00`).
- **Entry:** June's ctor (`3255bd10` -> `32556f60`) has no per-type switch. March's `SERV_LIMIT_UNIT` switch (`X2StateCreateUnit.cpp:175-230`) is
  **live** by `cl /P` (the planning digest said dead): it needs the Lu/Ciel case.
- **Create request trap:** March's live `REFORM_ENTRY_POINT` branch sends `m_SelectUnitType` as `m_iClass` (`X2StateCreateUnit.cpp:2505`) and the
  emulator (`Handlers_Unit.cpp:330`) reads it as a class; June sends the class (`m_SelectUnitClass`, 200). Port June's request body.
  `InitDummyUnits` keeps the class in a signed `char` (NU2's list).
- **Button slot:** `LucielCreateUnitSlot` x 668 (308 + 45 * 8), y 558, `BT_LUCIEL_SELECT_N` / `_O` in `bt_01.TGA`, group 1, message 36, no sound.
  June's whole file re-orders every slot and adds Rose / Ain slots whose `SCUUCM_ROSE_BUTTON_UP` / `SCUUCM_AIN_BUTTON_UP` March's `Enum.lua` lacks:
  as the Rules say, core-list source with only Lu/Ciel's slot (and `SCUUCM_LUCIEL_BUTTON_UP` 36 / `_JUMPING` 50 in `Enum.lua` and the C++ enum).
- **Screen content:** name and description are pictures in `DLG_UI_Common_Texture_GUI_character_info_10.TET` (19 keys); background
  `DLG_Unit_Background_Luciel.tga`; stand `Dummy_Village_Board11` + `EffectSet_Luciel_Stand_Aura` (x + 25, z + 50); camera / scale / light as
  Add's; `PlayAnim( "LobbyDownLanding", 2, 0, 2 )` on both skins; voice `LucielVoice_Introduce.ogg`; job movies `Movie_Create_Luciel_{Base,LD,CN,LN}.avi`
  already loose in `data\movie`. Create voices `LucielVoice_Create01/02.ogg`, select `LucielVoice_Character_Select.ogg` / `_GameStart.ogg`.
- **Two bodies in June** on the create screen (`@32555fe8` viewer `+0xd7c`, minor rotation `31edf260( class, 1 )`) and the select screen
  (`32564400`, per-frame `3255e570`): NU4 shows Lu only, DU5 adds Ciel (plan).
- **Rotation:** June's `31edf260( unitClass, bMinor )` replaces March's `RotateYSelectCharacterType` (`X2Data.cpp:3490`; divergence row,
  verdict `stage8`, NU4 replaces for every class): templet `m_fLobbyRotationY` / `m_fMinorUnitLobbyRotationY` (NU3), constant switch when 0.
- **Voices:** June's builder `32376270` reads `SelectVoiceTable.lua` (data036, June-only; March has no loader) before its fallback names. Port the
  builder and its loader whole for the create / select sounds (`adopt-june` + `luabind.py check`), and say in the Done note whether any other class's
  voice changes.
- Assets: CH0b's NU0 list. Select-list guard (`X2StateServerSelect.cpp:2951` / `:11391`, STR_ID 707) is NU1's. First entry is TU's.
