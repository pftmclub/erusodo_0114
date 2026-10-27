# June character Eve (Stage 7) - Phase CH1r - June's LoadEffectSetFromScript (EFFECT_SET_LIST_FINAL and the other June lists)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1r` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1 done (play-test finding, 2026-10-27) |
| Flag | new `SERV_IRUHADEV_JUNE_EFFECT_SET_LIST_VARIANTS` (engine, `CX2GUUser`; one flag, every class) |
| Log tag | `[JUNE_CHAR]` (`EVE CORE`) |
| Review | `kickoffs/char-eve/CH1r-review.md` (Opus 5.5 high; the phase writes it) |

## Why

CH1 play-test (user, 2026-10-27): Code: Nemesis / Exotic Gear dash-jump Z-X ( `ENSI_EEG_DASH_JUMP_ZX` / `_ZZX` ) shows no Queen's Spear on the ground. June puts that effect set ( `EffectSet_SA_ECE_Dashjump_ZX` ) in `EFFECT_SET_LIST_FINAL`, and March's `CX2GUUser::LoadEffectSetFromScript` reads none of June's list variants (`X2GUUser.cpp` comment at the S3f signature; checklist 7h row, `JUNE2017_CHARACTER_PORT_CHECKLIST.md` l.411). CH0's key scan matched `EFFECT_SET_LIST_FINAL` against the `_FINAL_ULTIMATE` text in that comment, so the key never reached `ch0_keys.tsv` as June-only.

Eve: `EFFECT_SET_LIST_FINAL` x5 (`ENSI_EEG_DASH_JUMP_ZX`, `_ZZX`, `ENSI_SA_EEG_IRONSCRAPS`, `_HYPER`, `ENSI_SA_ENS_ASSUALT_SPEAR`), `_FINAL_ULTIMATE` x5. Aisha (live since her CH1): `EFFECT_SET_LIST_ULTIMATE_EX` x4, also unread today.

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1 - Done (2026-10-27)"
```
Then the checklist row (l.411, `32024ff0` list variants) and the `32024ff0` rows of `june2017_re/funcmap.tsv` (C9h-4 / C9h-4d already ported the delete-key tail; S3f added the `bJuneUltimate_` argument, JUNE_BUGS 332).

## Task

1. Decompile June `32024ff0` (`CX2GUUser::LoadEffectSetFromScript`) whole and diff it against March's (`X2GUUser.cpp`, the S3f signature). Port the June body whole (no partial update): every list variant June reads (`_EX_TRANS_1` / `_TRANS_2` / `_EX_NO_TRANS`, `_ULTIMATE_EX( _APPLIED_SKILL )`, `_EX_APPLIED_SKILL`, `_FINAL` / `_FINAL_ULTIMATE` via `32020ec0`) with June's selection order and conditions (memo entry `31fdbbb0`, transcendence, applied-skill level `31fceab0`, the ultimate argument). A condition whose system is not in March gets June's absent-value default under its own stand-in, labelled; never invent.
2. March's original stays in `#else`. Name every caller's arguments (Eve, Aisha and the base `CommonStateStart` `@3205ad87`).
3. Play-test hand-off: EveNemesis (Code: Nemesis) dash-jump Z-X shows the spear and hits; Iron Scraps / Assault Spear if equipped; Aisha's `ULTIMATE_EX` states unchanged or June's (name them).
4. Done note, checklist row l.411 marked `(Eve: CH1r)`, funcmap rows, JUNE_BUGS for any June quirk kept.
