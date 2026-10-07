# June effect completion - C9e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9e review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET` (viewer site also needs `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`) |
| Log tag | none: every new path is inert in March and logs nothing; the regression play-test reads `JUNE_FX` lines only for the C9c loads |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9e - Done" "C9d - Done"
```

Also read: `june2017_re/funcmap.tsv` rows tagged `Effect C9d` (now marked `PORTED (SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET`), and the phase's commit
(`git log --oneline --grep="C9e"`).

## Task: check the port against June in Ghidra (read-only), then the data

Hygiene is one command: `python tools/flagcheck.py JUNE_EMOTION_EFFECT_SET --range <C9e commit>^..HEAD`. Paste its `files:` / `in this flag's files:` /
`RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check, in this order (the first is the one a transcription slip would hide):
- **`32355a50`** (the helper in `X2Unit.h`): all 11 ids against the jump table and strings. The 11 names against June's `UIEffectSet.src.lua`
  (a name typo is a silent miss). Ids `0x1c-0x20` order: Keroro, Kururu, Giroro, Dororo, Tamama.
- **GUUser `OnFrameMove` switch** (`3206eaf0 @3206eb64-3206ed70`): decode the byte table `32070198` / targets `32070188` again and compare the three
  case lists with the port (stop: 1-3, 25-27, 54-59; keep: 28-32, 35, 38-41, 47, 48; default stop). The port takes June's `GetEmotionData == NULL`
  path; check that nothing in that path is skipped other than the `+0x93dc` / `+0x93ec` drains.
- **GUUser `PlayEmotion` placement**: the port puts the play right after `m_ePlayedEmotion = eEmotionId;`, i.e. inside March's
  `if( m_pXSkinAnim != NULL )`. June's `@32043a33` is before the C9c-1 stop `@32043b64`; check whether June's block is under a skin-anim test or
  not, and whether the sit-item block that precedes it in June changes the order. Move it under the same flag if June has it outside the test.
- **SquareUnit**: `StateStart` stop `@323fbcbd-323fbd77` (June gates it on the skin anim: the port does not; say whether that can matter once a
  set exists), `PlayEmotion` `@323ff8c5`, dtor `@323f7b36`, `RemoveUnit` `323af8f0 @323af9e6` -> `323f4640`. Identify `323f56d0`
  (called there next to `323f4640` and left out): if it touches effect handles, port it.
- **Viewer** `31fa52b0 @31fa5995-31fa59d2` and the `32959640 @32959680` NULL-skin test added to `JuneViewerFxPlay`.
- **Data**: `UIEffectSet.lua` = C9c-2b's 67 sets + 11 (compiled from `UIEffectSet.src.lua`, appended, none replaced); `UIMajorXMeshPlayer.txt` = March's
  + the 5 entries before the closing brace. 26 particles all resolve in `UIMajorParticle.txt`.

Fix clear transcription errors under the same flag, then `python tools/build.py --deploy` (background). Hand design decisions back to the user.
Write "C9e - Review" into the plan doc, update the C9e review row in `STATUS.md`, and commit with explicit pathspecs.

## Play-test (regression only)

Nothing new is reachable (March emotion ids end at 15). Pass = village emote, character-select emote and a dungeon emote, a village -> dungeon -> village
trip and a clean exit, with 0 EXCEPTION, no `X2_*.tmp` crash bundle, and the C9c load lines unchanged (`UNIT_VIEWER_FX_LOAD ... rows=18`).
