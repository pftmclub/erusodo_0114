# June character Elsword (Stage 7) - Phase CH1j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1j review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1j - Done (2026-11-04)"
awk -F'\t' '$7=="CH1j"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 13 CH1j functions against June (`python june2017_re/charport_adis.py elsword --phase CH1j`; the listing of `32d1b2b0` runs on into its jump table at `32d1b5b0` / `32d1b5b8`). The risk sits in these:
- **CommonFrameMove `32d1b2b0` Windmill switch.** The state is `( +0x7cbc protected - +0x7cec ) / 100`, then `- 0x32` indexes the byte table at `32d1b5b8` (0x8e entries). The Done note reads 0 at indices 0-19 and 140-141 only (ids 50-69 = `USI_SPECIAL_ATTACK_1` ... `USI_SPECIAL_ATTACK_SLOTB_HYPER_5` with `GUSI_END` = 5, and 190 / 191 = `ESSI_SA_ESK_WINDMILL` / `_END`). Re-read the 142 table bytes with a script and confirm no other index is 0; confirm index 0 of the 2-entry table (`32d1b5b0`) is the shock / freeze branch (`+0x2480` EnchantShock, `+0x1730` Frozen, either above 0 stops the effects) and index 1 the plain stop. Also the sheath block, the handle `+0x95a4` / `+0x95a8` / sound `+0x95ac`, and the tail order (WayOfSwordStateChange, DestructionRageFrameMove, weapon-effect refresh at `+0x41f8` / vslot 0x28c, five ticks, `+0x9788`).
- **The three cool-time pair names.** `+0x96f8` / `+0x9704` (21410), `+0x9740` / `+0x9734` (21407), `+0x9758` / `+0x974c` (21411) and `+0x9788` (21415) were assigned from the skill block of `InitPassiveSkillState` `32d1c560` that initialises each, not read from a registration. Check the pairing in the listing (init of each pair right before the `GetSkillLevel` of its skill; ability 0x94 stored into the cool member) and rename in the header if wrong. Check the tick rule too: `0 < cool` and not `cool < elapsed`, then `elapsed += m_fElapsedTime` (the compare operands of `fcompp` after the two `fld`).
- **DestructionRageFrameMove `32d0b650`** (the class map said `new`; March has it): early return on `IsSuperArmor`, then the whole body only while `0 < +0x9618`; the protected bool `+0x7ff4` set, `+0x8040` cleared, countdown clamped.
- **CommonRender_Prepare `32cf3230`**: `+0xc14` is read as `m_RenderParam.cartoonTexType` (CTT_SKYBLUE 11 / CTT_SCARLET 12; renderType and outline colour not written). Confirm `+0xc14` against the March layout of `CKTDGXRenderer::RenderParam` (renderType first) or another June class that writes `+0xc10`.
- **CommonStateStart `32cf2e90`** (the two keys, defaults 0, then the flag at `+0x960f` = `m_bFirstAttackSuccess`); **CommonFrameMoveFuture `32d0b4d0`**; the three plain `jmp` thunks.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h` (then `--range <commit>^..<commit>`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1j - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1k.md`, commit with explicit pathspecs.
