# June skill system core - Phase S7l - The rest of June's emblem manager

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7l` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7i (and its review) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_*` |
| Log tag | `[JUNE_SKILL] EMBLEM` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7l-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7i - Done (2026-10-19, Opus 5.5 high)" "Phase S7i -- Review (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7i` and `S7l`; `june2017_re/xorstrings.tsv` rows of `32449000` (the parser's 17 keys).

## Task

Added by S7i (2026-10-19, unowned-branch rule). S7i ported only what emblem 17 needs; June's `CX2EmblemManager` differs from March's in more places, and since
S7i data036 ships June's `Emblem_Manager.lua` (20 emblems, `EXPAND_TEXTURE_FILE_NAME` on 6 of them). Port each remaining June function whole (rule 1):

1. **Parser `32449000`**: `EXPAND_TEXTURE_FILE_NAME` ( `{ name, x, y }` rows -> entry `+0x74` list, stride 0x28, `3244f6f0` ); June reads no `OPEN_TIME_SETTING`
   (March's `ADD_2013_CHARACTER_ADD_EVENT` block and `SetOpenTimeEventEmblem` go to `#else`); compare the rest key by key.
2. **`32448ce0`** (called after `PlayEmblemEffect`): the expand-texture particles ( `+0x44` handle list; the one-shot flag at row `+0x24` erases the row ) and June's
   second sound pick (it re-clears `+0x24` / `+0x20` and picks again: keep verbatim, JUNE_BUGS row).
3. **`OnFrameMove` `3244c8c0` whole**: the `+0x44` follow loop (camera-relative positions, `g_pKTDXApp +0x1fc +0x1c0..+0x1c8`, double `334355b0`), the hide-dialog
   gate `31d03920`, June's state switch on `g_pMain +0x8c` (cases 0xb / 0x17 with the boss-raid field mark-shown `g_pMain +0x16a`, 0xe / 0xf, 0x15 with `32482650`)
   against March's `XS_VILLAGE_MAP` / `XS_DUNGEON_GAME` / `XS_BATTLE_FIELD` switch (map June's state ids to March's first).
4. **`InitEmbelmPlayInfo` `324485d0` whole** (the `+0x44` handles, the wait constant `33436700`) and `CheckNextPlayingEmblem` `32447d40` (its result when every
   row is shown).
5. **`PlayEmblemOtherProcess` `3244c4b0`, the other cases**: 5 (S3e's buff, plus the vtable `+0x21c( 0x9a, ... )` call), 7 (chat `0x7d9f` with two names, sound),
   8 ( `3269f140` / `3269e8f0` ), 0x12 `EI_NEW_TITLE_INFO_NOTICE` ( `3244c2b0`, `32447b50` / `32448420` / `3244bfb0`, map `+0x78`; same dialog as 17 ), 0x14
   `EI_ACHIEVEMENT_COMPLETE_NOTICE` ( `DLG_Emblem_Achievement_Notice.lua`, `3244e520`, `+0x94` ). A case whose system March lacks (titles notice, achievements) is
   noted with its owner, not invented.
6. **The `PlayEmblem` callers** ( `32447d70` xrefs: `31ef9bc0`, `31efd850`, `31f06000`, `32161070`, `31f0bdd0`, `32701960`, `326c9530`, `326c9030`, `31ef7ad0`,
   `32600190`, `3269f940` ) against March's ( `X2BossRaidManager`, `X2PremiumBuffManager`, `X2State` ): name each, port the ones whose March counterpart exists.

**Play-test:** whatever the ported cases reach offline (the boss-raid and burning emblems need their server events; say which cannot be shown).
