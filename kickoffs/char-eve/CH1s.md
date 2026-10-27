# June character Eve (Stage 7) - Phase CH1s - The third-awakening portrait ball does not show

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1s` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1r (both edit Eve / `CX2GUUser` code; one at a time) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` (CH1q's), plus a temporary `SERV_IRUHADEV_JUNE_CHAR_EVE_DEBUG` if a diagnostic is needed (retired in the review) |
| Log tag | `[JUNE_CHAR]` (`EVE NCORE`) |
| Review | `kickoffs/char-eve/CH1s-review.md` (the phase writes it) |

## Why

CH1 play-test (user, 2026-10-27): the third-awakening circle on Eve's portrait (top left) never shows. CH1q moved it to `CX2EveMyGageUI::June_UpdateHyperBall` (June `32e6a210`, `X2GageUI.cpp`), which creates `Eve_PowerUp_UI_01` (in `data036 GameMajorParticle25.txt`) on `g_pData->GetGameMajorParticle()` while `m_pGageData->GetHyperModeCount() == 3`. The test used `g_pX2Game:GetMyUnit():SetHyperModeCount( 3 )` and then awakened.

Already checked in CH1 (do not redo): `CX2MyGageUI::OnFrameMove( float )` is the live overload (`X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`, `KTDX.h:342`), so the override runs; `CX2GageManager` builds `CX2EveMyGageUI` for all 7 Eve classes under `ADD_EVE_SYSTEM_2014`; the particle name exists in data036.

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1q - Done (2026-10-27)" "CH1q - Review (2026-10-27)" "CH1 - Done (2026-10-27)"
```

## Task

1. **What is June's gage data `+0xf4`?** `32e6a210` tests `*(char*)( gageData + 0xf4 ) == 3`. Find its writers in June (user vslot `0xa8` = `3208eef0`, no Ghidra function there yet: read the bytes; the gage-data setters; the awakening start `@32564`-equivalent where March sets the count to 0 and records `m_iHyperModeCountUsed`). If it is the orb count, the ball shows while 3 orbs are held (March's 2014 rule); if it is the awakening level used, it shows during a 3-orb awakening. Port whatever June writes there.
2. **Why it never shows now.** Check `32e6a210`'s first gate (`DAT_3372c0fc` observer, `FUN_31aad340` / `31aad500`, a `type_info` compare: which object must exist), the particle system `322ceaa0` (`g_pData+0x84` per the CH1q review: is that March's `GetGameMajorParticle()`, and does that system load `GameMajorParticle25.txt`?), and the CreateSequenceHandle arguments (`31aada50`: position from `DAT_337676e8 +0x1b8 / +0x1bc` x constants `33534178` / `33438090`). If reading cannot settle it, add a throttled `_DEBUG` log line (handle, count, show) and hand over one play-test.
3. Fix under `_NCORE` (or the flag that owns the cause), Done note, JUNE_BUGS row if June's rule surprises, hand-off: EveSeraph, paste `g_pX2Game:GetMyUnit():SetHyperModeCount( 3 )`, look at the portrait before and during awakening.
