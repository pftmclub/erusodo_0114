# June effect completion - C9e - port June's per-emotion effect sets (`32355a50`), inert

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9e` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C9d (done 2026-10-07), and **C9c-2b committed** (its viewer player `32959640` and its edits to `X2UnitViewerUI.cpp`) |
| Flag | new `SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET` |
| Log tag | `JUNE_FX` (no new log line is needed: the path is inert) |
| Review | `kickoffs/june-effect/C9e-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9d - Done" "C9c-1 - Review"
```

Also read:
- `june2017_re/funcmap.tsv`: every row tagged `Effect C9d` (`grep "Effect C9d" june2017_re/funcmap.tsv`), plus `31fcf490`, `31fcf4d0`;
- March: `X2Lib/X2GUUser.cpp` `PlayEmotion` (:28870, C9c-1's `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE` block), `OnFrameMove` (C9c-1's
  stop after `CX2GameUnit::OnFrameMove`), ctor and dtor head; `X2Lib/X2SquareUnit.cpp` `StateStart` (:2170) and `PlayEmotion` (:3616);
  `X2Lib/X2UnitViewerUI.cpp` `PlayEmotion` (:6270 at `b6a5e1f`; the line moves once C9c-2b lands);
- `june_src_fixed/data036_kom/UIEffectSet.src.lua` at the 11 `NAME` lines (row `32355a50`); June's `UIMajorXMeshPlayer.txt` at
  `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\data036_kom\` for the 5 mesh players.

## Task

Everything goes behind the new flag, verbatim from June. Every path is **inert in March**, because March has no emotion id above 15.
Do not add June's `EMOTION_TYPE` entries, `EmotionData`, `IsAvatarEmotion` changes or any sit-item code: those belong to the emotion
system the user is placing (C9d's open question). Where June reads EmotionData, take June's NULL path and cite it.

1. **`32355a50` helper.** Add one function shared by all three classes (e.g. a static on `CX2Unit`), taking an `int`. It returns the
   11 names for June's raw ids, with June's enum names in comments (`0x1c` = `ET_EMOTION_AVATAR15`, ...), and `L""` otherwise.
2. **`CX2GUUser`, handle `+0x9434`.**
   - Member: init `INVALID_EFFECTSET_HANDLE` in the ctor (`@32041dca`).
   - **PlayEmotion:** immediately before C9c-1's block, the same place in June (`@32043a33`, just before `31fcf490`). If the name is
     non-empty and `g_pData`: stop the handle if valid, then `handle = GetUIEffectSet()->PlayEffectSet( name, this )` with defaults.
     Store it unconditionally (June stores -1 too) and do not tag it.
   - **OnFrameMove:** right after `CX2GameUnit::OnFrameMove`, before C9c-1's `+0x943c` stop. This is June's switch on `m_ePlayedEmotion`:
     - ids 1-3 and 25-27, 54-59: stop;
     - ids 28-32, 35, 38-41, 47, 48: keep;
     - every other id (0 included): stop. This is June's path when `GetEmotionData` returns NULL; skip the `+0x93dc`/`+0x93ec` drains.

     Write the case lists as June's numbers.
   - **Dtor:** stop it at the dtor head, before C9c-1's `+0x943c` stop (June `@3203af9f` precedes `@3203afb7`).
3. **`CX2SquareUnit`, handle `+0x7ac`.**
   - Member: init `INVALID` in the ctor (`323f8080`).
   - **PlayEmotion:** after the avatar-sound name switch and before `if( m_pAvatarEmotionSound != NULL )` (March `:3762`, June
     `@323ff8c5`, between the sound switch and the sound open). Play with `PlayEffectSet( name, CX2SquareUnitPtr this )` on
     `GetUIEffectSet()`. There is **no stop before the play** here: keep that, and add a `JUNE_BUGS.md` row if it can leak a handle.
     June widened the enclosing condition to its own avatar list (missing system); say so in a comment.
   - **StateStart:** at June's site (`@323fbcbd-323fbd77`), always stop `+0x7ac`. Leave out the `323f34a0` sit-item call; it belongs
     to the missing system.
   - **Dtor** (`@323f7b36`): stop it.
   - `323f4640`'s only caller `323af8f0` is unidentified. Port it only if you can name a March counterpart; otherwise record it in the
     Done note.
4. **`CX2UnitViewerUI::PlayEmotion`** (`31fa52b0 @31fa5995-31fa59d2`). After March's gates (`:6336-6347`), before
   `if( m_pXSkinAnim != NULL )`: if the name is non-empty, call C9c-2b's Play with `( GetXSkinAnim(), name, group 1 )`.
   - Group 1 is stopped by the viewer dtor (C9c-2b) and by June's sit-item block (missing system). Note that in the Done note.
   - Check what June's play does with a NULL skin anim, and keep that behaviour.
5. **Staging.**
   - Append the 11 templets verbatim to March's `UIEffectSet` (decompile, append, compile; never a wholesale replace). Do the same with
     the 5 mesh-player entries in `UIMajorXMeshPlayer.txt`.
   - Re-check that the particles resolve in the committed `UIMajorParticle.txt`.
   - The 5 `.x`/`.X` meshes ship with the emotion system. List them in the Done note; do not stage them.
   - Pack `data036`.

**Gate:**
- the load lines of `UIEffectSet` still pass, and `UIMajorXMeshPlayer` loads clean;
- an emote in the village, the character select and a dungeon runs with 0 EXCEPTION and exits cleanly;
- the new code is in the deployed exe. Prove it with `#pragma message` or strings, since nothing logs.
