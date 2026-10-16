# June effect completion - C3p - June's skip-the-start (ESC) system and its NPC key DIRECTION_STATE (identify the rest, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3p` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3o (found there). **Not with** a phase that edits the end of `CX2GUNPC::CommonStateStart` (C3f-1 item 2b) |
| Flag | `SERV_IRUHADEV_JUNE_SKIP_START` (+ `_DEBUG` for the play-test lines) |
| Log tag | `JUNE_FX C3p` |
| Review | `kickoffs/june-effect/C3p-review.md` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3o - Done" "C3p - port: June's skip-the-start (ESC) system and DIRECTION_STATE"
grep -n "(C3o)\|^322f11d0\|^322f4c60\|^324cf360" june2017_re/funcmap.tsv
```

## Task

**Visible:** 77 shipped NPC scripts (dungeon openings: `BES_START`, `Besma_Rock_Common_Start`, the event `*_START` scripts ...) set
`DIRECTION_STATE = TRUE` on their staging states. In June, ESC during such a state skips the opening. March has none of it.

The chain, all June-only (C3o mapped the NPC side; Buff Phase 7e the game side):
1. **Parser:** `32784c00 @3278bbc7`, key `DIRECTION_STATE` (XOR-built `@3278ba7b`), June state flag bit `0x4d`, a miss stores false.
2. **Arming:** `CX2GUNPC::CommonStateStart 327ac9d0 @327af25b-327af360`, at June's position after C3f-1's 2b block and before C3o's
   cinematic-UI store. With the bit and a live `g_pX2Game` (holder `0x3372c0fc`): if the list at game `+0xb44` is empty,
   `322d8010` resets the struct (`+0xb40` / `+0xb41` = 0, clear, then `g_pMain` `+0x90` -> `31ef7370( 0x1b, -1.0 )`); then
   `+0xb40` = 1 and push_back the NPC's `+0x270` (likely `GetUID()`: settle it with vslot `0xa8` = `GetNPCUnitByUID`).
3. **Consumers:** `322f4c60` (ESC, from the base CX2State key handler `31f1e1a0 @31f1e3a2`): armed and not sent -> a party of 2+
   (`g_pData +0x5c`, 0xa0-byte entries at `+0x28`) sends event `0x4ac` (`31e57100`), else `322f11d0`. `322f11d0`: every user's
   start-motion delay (`+0x92cc`, March `m_fStartMotionDelayTime`) to 0 unless -1; each listed NPC `StateChange( +0x4088 )` (June
   `+0x4088` = the NPC's WAIT_STATE id, InitState `@3279917e`); clear the struct; `31ef7370( 0x1b, -1.0 )`. `324cf360` answers `0x4ac`.

Settle before porting: what `CX2State +0x1ec / +0x1f0` (`31ef7370`, key `0x1b` = VK_ESCAPE?) is and who reads it; every reader of
game `+0xb40 / +0xb41 / +0xb44` (raw disp32 scan, not only a linear sweep); whether the offline party at `g_pData +0x5c` counts the
AI-party bots as members (if it does, ESC takes the `0x4ac` path and the emulator must answer it, offline rule); any UI June shows
while armed. Port the whole chain under one flag; a June-only path no other phase owns is planned, not asked about.
Play-test: a dungeon whose first stage opens with a `DIRECTION_STATE` NPC (name it and its dungeon id from `DungeonData`), ESC during
the opening, with exact steps and the `JUNE_FX C3p` lines to look for.
