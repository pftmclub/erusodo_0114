# June character Eve (Stage 7) - Phase CH1n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1n review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1n done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (the head, `June_SetFaenaSkillFunc`), `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (the call-site pair and the id-switch `default:`), `SERV_IRUHADEV_JUNE_USER_COPY_STATE_CALLBACKS` (`CX2GUUser::June_CopyStateCallbacks`), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (14 RNSI names in `X2SkillTree.h`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1n - Done"
grep -a "(CH1n)" june2017_re/funcmap.tsv               # 3 rows
grep -a "^| 442 |" JUNE_BUGS.md
```

The code is the `//{{ ... CH1n` block at the end of `X2Lib/X2GuEve.cpp`, the two `_LIVE` pairs in `CX2GUEve::SetEquippedSkillFuncBySkillSlotIndex`, two declarations at the end of `X2Lib/X2GuEve.h`, the CH1n enum block in `X2Lib/X2SkillTree.h`, and the hoist (`X2GUUser.h` / `.cpp` by `DoStateEndStartFuture`, Aisha's `#ifndef` pair in `X2GUArme_VioletMage.h` / `.cpp`, the `Always.h` define).

## Task

Review against June. **Delegate the mundane reads**: fresh Ghidra decompile and listing of `329c8a00` and `329cc2c0` to the scratchpad, and the four slot tables `334c9e00` / `334c9e14` / `334c9e28` / `334c9e3c` from the exe (`june2017_re/charport_consts.py` `read_va`). You decide:

- **Head:** bounds before the batch; normal = `9e00` (A) / `9e28` (B), hyper = `9e14` / `9e3c` (CH0's note had them swapped); `31fd2af0` dropped; `+0x11` / `+0x10`; `31fca8e0( id, 1 )` = `GetEquippedActiveSkillTemplet( id, true )`.
- **Memo switch** (`@329c8b4a`, byte table `329caac8` / targets `329caab4`): the ten ids and their four bodies, each callback slot against the listing (Energy Needles' hyper Init = JUNE_BUGS 442; Hornet and Spit Fire set the hyper byte `+0x10`); any other memo id registers nothing.
- **Type switch:** 5 / 6 / 7 / 0xc / 0xd-0x10 names; the hyper-active split (`> 25600` switch, `== 25600`, else the id switch) as the C++ reorders it; Link Overcharge's hyper takes Start + FrameMove / EventProcess / End (`@329ca906` then `329c9be9`); Psychic Artillery, Lunatic Scud (EventProcess only); `> 25600` default = Faena; 900 = `SI_ETC_WS_COMMON_LOVE`.
- **Stand-ins:** every March member the head registers is the class map's March pair of the June target (`ch0_class_map.tsv` column 4); Faena's Init / FrameMove left to CH2b (kickoff amended); `COMMON_BUFF_FrameMove` is the virtual (vslot 0x5f0), switched by CH1o.
- **Call site:** under `_LIVE` the March head and type switch are in the `#else`; the id switch's `default:` (March's falls into Illusion Strike) calls `June_SetFaenaSkillFunc`; the March-only `_HYPER` cases stay March's until their skill phases.
- **Hoist:** `June_CopyStateCallbacks` equals `329cc2c0` (nine delegates, `+0x14..+0x58`); Aisha's call sites now resolve to the base.
- **Enum names:** the 14 values against `june_src_fixed/data036_kom/Enum.src.lua`; no March member shares a value.

Hygiene: `python tools/flagcheck.py <FLAG> --range <CH1n commit>^..<CH1n commit>` for `JUNE_CHAR_EVE_CORE`, `JUNE_USER_COPY_STATE_CALLBACKS`, `JUNE_ENUM_NAMES` (and `JUNE_CHAR_EVE_LIVE`, whose "not #defined" FAIL is by design until CH1 final); paste the `files:` / `in this flag's files:` / `RESULT:` lines. `enumscan --check X2GuEve.cpp X2GUUser.cpp`; `ch1m_regcheck.py` stays `0 problems`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1n - Review" into the plan doc under the Done note, update the CH1n row in `STATUS.md`, check that `kickoffs/char-eve/CH1o.md` still matches, and commit with explicit pathspecs.
