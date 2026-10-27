# June character Eve (Stage 7) - Phase CH1q review - Nasod Core

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1q review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH1q done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` (defined only with `_LIVE`; it defines `ADD_EVE_SYSTEM_2014` / `FIX_ADD_EVE_SYSTEM_2014`) |
| Log tag | `[JUNE_CHAR]` (`EVE NCORE`), seen from CH1 final |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1q - Done (2026-10-27)"
```
Then `june2017_re/char_eve/ch1q_nasod_core_outcome.tsv` (every map row with its outcome) and the `(CH1q)` rows of `june2017_re/funcmap.tsv`.

## Task

Review CH1q's commit against June: the June bodies of `3297ee20`, `329b4060`, `32975740`, `329b5230`, `329ac4a0`, `329acea0`, `32968470`, `329cf280` and the gauge UI `32e64790` / `32e71990` / `32e6a210` / `32e5eca0` / `32e725e0`. Things this phase could get wrong:
- the spec B roll's sense and order ( `!( B > GetRandomFloat(0) )`, the roll before `IsMyUnit` in `329b5230 @329b5547` ) and that `+0x6c50` / `+0x6d04` are `GetCharacterSpecializationA` / `B` results;
- `32975740`: `GetSkillTemplet( id )` for June's `( id, 1 )`, level 1, ability `0x17`;
- the hyper ball: owner NULL, `( 72, 80 ) x resolution`, over-UI, show only while `GetShow()`, destroyed when the count is not 3; `+0xf4` is the gage data's hyper count;
- `AttackManeuverCore`: the MP block's place ( inside the effect-set / damage-effect test, outside the mesh test ), `(int)` truncation, `pos.y + 50`;
- the March sites that `ADD` / `FIX` turn on for every class ( `X2GUUser.cpp` DamageReact / SetHold / RenderName, `X2GUNPC.cpp`, `X2DamageManager.h` `DTT_END` now 6, `X2GageUI` orb removals ): any of them reachable by a non-Eve unit;
- the parked CH1b / CH1k / CH1l2 / CH1l3 / CH1l4 blocks now live, and the three parrying registrations in `June_InitStateCommon` ( re-run `ch1m_regcheck.py` after dropping ids 224-226 from `ROUTED`: `0 problems` ).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_NCORE --range <CH1q commit>^..<CH1q commit>` (the flag is undefined without `_LIVE`, so a "not #defined" FAIL is by design); `enumscan --check` on the four files; `/Zs` of `X2GuEve.cpp`, `X2GUUser.cpp`, `X2GageUI.cpp`, `X2GageManager.cpp`, `X2GUNPC.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` and without.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1q - Review" into the plan doc, update the CH1q STATUS.md row, check `kickoffs/char-eve/CH1.md` still matches, and commit.
