# June character Eve (Stage 7) - Phase CH2a3 - Mega Electronball FrameMove (split from CH2a)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2a3` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2a review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`) |
| Review | `kickoffs/char-eve/CH2a3-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2a - Done (2026-10-27, Sonnet 5.5)"
```
Then the `Char Eve CH2a` rows of `june2017_re/funcmap.tsv`.

## Task

Skill ids 25207 `RNSI_A_EAT_MEGA_ELECTRONBALL` and 25219 `_UPGRADE` (states `ENSI_SI_A_EN_MEGA_ELECTRONBALL` / `_HYPER`). CH2a (Sonnet) stopped here: the FrameMove `329b1e20` (`@329b1e20-329b2e0f`, 1178 instructions, uncarved in Ghidra: use `june2017_re/funcdis.py` / an annotating disassembler; Ghidra's decompile of it has a broken frame) is a rewritten mechanic, not March's. Read so far: the 0.22 s `EveElectraSystem::June_CreatePlane( +0x99a8, 2.0, 240, 130 )`; the 0.23 s / 0.48 s release; MP cost by `+0x9788` rank (x1.5 / x1.25 and x1.25 / x1.125 on ability `0x20` / `0x1f`, `SA_PROJECTILE_SIZE` / `SA_PROJECTILE_RANGE_REL`), class 0x13 / 0x6d `+0x97b0` factor, `32039a50`, `32358970( 0xb )` x0.01, `FlushMp` (vslot 0xe4) with `JuneUpNowMpWithText` (`32278070`) on failure, Open Mind `634b` refund (`32007b10( 0x42 )`), `32007c40` (file-local `June_TryGetAcquiredSkillAbilityValue`) setting `+0x978d`, three alpha-tagged particles (`Ring_Eve_SI_A_EN_MEB01`, `Light_Eve_SI_A_EN_MEB02` / `03`), then `DamageEffect_A_EEG_MegaElectronBall` through `31f8ff00` pushed to a list (`31f8b860`) and a loop over a unit list built by vslot `0x77c` (`3299aee0`) with the effect set at the targets (+150 above), SetScale (`31f6fdc0`), `322cc1a0` x0.33, BFI `0x28c`. Unread: `@329b2aec-329b2e0f`.

Done already in CH2a (inert, `X2GuEve.cpp`): `June_ENSI_A_EN_MEGA_ELECTRONBALL_Init` (`32966910`) and `_Start` (`3296f9a0`). Equal to March (verified): StartFuture `32966960`, FrameMoveFuture `3296fa90`, EventProcess `3297cba0`, EndFuture `32966990`. To do: `329b1e20`, then the case in `June_SetEquippedSkillFuncHead` (Init `329cbb50`, StartFuture `329cbb60`, Start `329ccef0`, FMF `329ccf00`, FM `329cf650`, EP `329ce700`, EndFuture `329cbb70`; hyper table `ENSI_SI_A_EN_MEGA_ELECTRONBALL_HYPER` copies all seven including Init), with `June_LogEveSkill`. Alpha-tag sites in it: `329b231c` and five more (checklist 7c). `NUM`/damage: June level-1 `DamageEffect_A_EEG_MegaElectronBall` 3.23, `_Explosion` 11.02 (`plan_numerical_rows.tsv`). CP949: `bytepatch.py` only.
