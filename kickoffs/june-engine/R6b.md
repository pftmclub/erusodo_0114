# June engine remainder - Phase R6b - Equip mesh flags and the equip glow

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then type `/x2-phase june-engine R6b` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R6 and its review |
| Flag | `SERV_IRUHADEV_JUNE_EQUIP_MESH_RENDER` (+ `_DEBUG` if the phase logs) |
| Log tag | `[JUNE_ENGINE] EQRP` (`python tools/logs.py --tag JUNE_ENGINE`) |
| Review | `kickoffs/june-engine/R6b-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R6 -- Done" "Phase R6b - Equip mesh flags and the equip glow"
```

Then the funcmap rows tagged `(R6)` (`grep "(R6)" june2017_re/funcmap.tsv`).

## Task

R6 ported June's skin-anim draw. It now reads mesh flags whose June writers live outside the engine, and only their ctor defaults
reach it. Port the writers:

1. **CX2Eqip mesh-flag switch, June `32964ae0 @32965400-329654f9`.** Face ids set mesh `+0x8d0` (March `SetNoRenderable`). 202560 / 202610
   and the June ids `0x435b3`..`0x450c3` set mesh `+0x864` (alpha). March's `ARA_CHARACTER_BASE` `SetUseRenderParam` cases
   (`X2Eqip.cpp:1137-1148`, `:1160-1171`) are gone in June. Compare the whole June function with March's (`X2Eqip.cpp` around `:1100`,
   both the normal and the hyper model). Port June's switch whole (no partial updates).
2. **The equip glow `3295b970`** (caller `32963c80`, struct copy `3295a6c0`). It builds an equip RenderParam at `+0x278` (RT `0xf`,
   item checks `32809f40( 0x11..0x17 )`, light-flow impact decay), converts it to the 0xac struct and stores it in mesh `+0x980`,
   then sets `+0xa2c`. That feeds R6's RenderParam pass. Map `32963c80` first (what calls it, per frame or on equip), then port.
3. **CX2GUUser `31fd0880`** toggles skin anim `+0x851` on both anims when user `+0x9170` is set. Compare it with March's
   `X2GUUser.cpp:8371` / `:8381` (`SetHideNoRenderable`) and port the difference, if there is one.

Done gate: a phase log line of its own (`[JUNE_ENGINE] EQRP`) on an item June glows. R6's `_DEBUG` (`SMRT renderparam`) is off since
the R6 review; re-define it for one run only if the glow's RenderParam pass needs proving. Hand the user a play-test with an item from each branch (`/additem`).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

## From june-march-divergence C4 review (2026-10-29)

C4 merged June's mesh `+0xa2c` into March's `m_bUseRenderParam` (`GetUseRenderParam()`): it is the same field. Until this phase lands, March's
Ara cases in `CX2Eqip` (`X2Eqip.cpp`, items 41510 / 41640 / 41700 / 41736 / 41743, normal and hyper model) set that flag with no `+0x980` param, so
R6's RenderParam pass draws those weapons a second time with the default `RenderParamData` (RT 0, `RT_REAL_COLOR`, no blend): flat over the
cartoon pass. June's switch `32964ae0` has no Ara cases and `3295b970` writes the param and the flag together; port both in one go.
