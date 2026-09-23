# Ranox effect/damage engine gaps

Companion to the plan that restored Ranox monster effects and damage
(`march2014/data036/EffectSet.src.lua` + `DamageEffect.src.lua` merge, Phase A;
`SERV_IRUHADEV_RANOX_EFFECT_DEBUG`, Phase B; `SERV_IRUHADEV_RANOX_ATTACH_SKIN_MESH`,
Phase C). Phases A-C restore everything this build's March-2014 engine is *capable* of.
This document is Phase D: a standing inventory of what it is **not** capable of — the
fields June2017's data sets that this client's C++ never reads — as a target list for
reverse-engineering the June2017 client binary later, and as a record of what was
deliberately left unimplemented and why. It is a document, not code, and sits next to
`RANOX_PORT_PLAN.md` for the same reason.

Nothing below blocks Phase A. Every gap listed here degrades to a parser default or a
disclosed visual approximation; none of them causes a Lua load failure or a missing
damage number for content this port ships.

## 1. Damage-effect keys with no parse site (5, across 2 blocks)

Across all 92 recovered `DamageEffect` blocks and all 196 recovered `EffectSet` blocks,
these are the only keys June's data sets that have no corresponding read anywhere in
`X2Lib`/`KTDXLIB`. The effect-set format itself has **zero** unread keys — it is fully
implemented in this build.

| Key | Block(s) that use it |
|---|---|
| `LOOP_ADD_POS_X` | `DAMAGE_EFFECT_Trap_Fire_Big` |
| `LOOP_ADD_POS_Y` | `DAMAGE_EFFECT_Trap_Fire_Big` |
| `SET_DIE_WHEN_MASTER_EFFECT_DIE` | `DAMAGE_EFFECT_Trap_Fire_Big` |
| `NUMERICAL_DATA_INDEX` | `DamageEffect_SA_RHB_Lockon_Stratos_Attack` |
| `APPLIED_SKILL_ID` | `DamageEffect_SA_RHB_Lockon_Stratos_Attack` |

Two blocks affected total, both degrading to whatever the surrounding parser default is
for an unread field (position/lifetime nuance only) rather than failing to load.

**This number was 11 in an earlier draft, and the correction matters as a method note.**
The first pass searched for each candidate key as a *quoted string literal* in
`X2Lib`/`KTDXLIB` and flagged six buff keys — `BFT_TIME`, `BBT_CHANGE_NOW_HP_PER_SECOND`,
`BBT_FROZEN`, `BBT_SUPER_ARMOR`, `BBT_STUN`, `BFT_HITTED_COUNT` — as unimplemented. They
are not: the Lua key name for those is generated from the C++ enum identifier by a
stringizing macro (`CASE_BEHAVIOR_FACTOR`, `X2BuffFactor.cpp:9-22`), so the literal never
appears in source as text. March's buff-factor system implements every one of them (cases
at `X2BuffFactor.cpp:110,168,371`; templets in `X2BuffBehaviorTemplet.cpp:51,78,96,98` and
`X2BuffFinalizerTemplet.cpp:49,51`). Re-tested by bare-identifier and stem search, the five
above are the only genuine gaps. Re-tested against `march2014/data036/Enum.src.lua`, every
enum token `DAMAGE_EFFECT_MAG_CHARGER_Dying` needs — `BUFF_BEHAVIOR_TYPE`,
`BUFF_FINALIZER_TYPE`, `BUFF_CHANGE_TYPE`, `BUFF_RELATION_TYPE`, `BUFF_DURATION_TIME_TYPE`,
`BTI_DEBUFF_FIRE`, `BCT_RELATION_VALUE`, `BRT_MIX_ATTACK`, `BDTT_NORMAL_TIME` — resolves.

**So Phase A fixes the Mag Charger completely, burn-over-time included.** A zero-hit
string search is not proof of absence in this codebase — stringizing macros are used
throughout `X2Lib`. Confirm with a bare-identifier search before calling anything a gap;
this is the fourth time on this project a plausible parser produced a confident wrong
verdict (see `MARCH_2014_MIGRATION.md`, `OFFLINE_MODE_PLAN.md` for the earlier three).

## 2. Enum tables June defines and March does not (5)

`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `DAMAGE_EFFECT_DIE_CONDITION_TYPE`,
`HP_RATE_DAMAGE_TYPE`, `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`.

None of these is referenced by anything Ranox needs (that's what makes Phase A's
merge-not-replace approach viable at all — see "Why not replace March's files with June's
outright?" in the plan). Each names a whole engine feature this build lacks entirely, not
a missing constant. Two more whole tables *were* needed by Ranox and *were* added, already
landed in an earlier port phase — see `march2014/data036/Enum.src.lua:9212-9245`
(`DELETE_CONDITION_SOUND` and `SKIN_MESH_RENDER_TYPE`, the latter used by Phase C above).
That comment block also already noted that neither `SOUND_PLAY_EX` nor
`SKIN_MESH_RENDERTYPE` (the *keys* that index into those tables, as opposed to the tables
themselves) had any C++ parse site — Phase C above closes that gap for
`SKIN_MESH_RENDERTYPE` specifically; `SOUND_PLAY_EX` remains unparsed and unneeded by
anything Ranox ships.

## 3. Enum members June defines and March does not (413 across 12 tables)

253 `SKILL_ID`, 73 `BUFF_FACTOR_ID`, 37 `DAMAGE_TRIGGER_TYPE`, 19 `HIT_TYPE`, the remaining
31 spread across 8 smaller tables. All Rose/Ain-era content, out of scope for a Ranox fix
and not added — adding them would require also adding the (also Rose/Ain-era) C++ that
gives them meaning, which is a content port an order of magnitude larger than this one.

Of these 413, only 5 members are actually referenced by the 288 Ranox blocks this port
recovers, and those 5 were resolved individually rather than by adding the member:

| Token | June value | Affected block(s) | Resolution |
|---|---|---|---|
| `DAMAGE_EFFECT_POSITION_TYPE.DEPT_BONE_POS` | *absent in June too* | `DamageEffect_MULMAI_Attack_A` | nil → `DEPT_BONE` default. June's own `Enum.lua` doesn't define this member either, so this is not a degrade relative to the reference client — June takes the identical default |
| `HIT_TYPE.HT_MAGIC_GLOVE_SLASH` | 54 | 2 `SECRET_BOSS_MAGI` blocks | nil → default hit reaction |
| `HIT_TYPE.HT_SOUL_BIG_HIT` | 60 | 1 `SECRET_BOSS_MAGI` block | nil → default (60 is past March's `HIT_TYPE` max of 54) |
| `BUFF_TEMPLET_ID.BTI_DEBUFF_LAVA_BURN` | 789 | `DamageEffect_SECRET_BOSS_LAVA_ATTACK_Dot_Damage` | nil → no debuff applied; base damage still lands |
| `SKILL_ID.RNSI_SA_RHB_LOCKON_STRATOS` | 31012 | `DamageEffect_SA_RHB_Lockon_Stratos_Attack` | nil → default (31012 is far past March's max of 7218) |

None of the five was added to `Enum.lua`: none has a C++ counterpart in this build (zero
hits across `X2Lib`/`KTDXLIB`/`KncWX2Server/Common` for all five), and two of the values
sit outside March's C++ enum ranges — feeding an out-of-range integer into a C++ enum is
worse than the parser's own default. The affected blocks were transcribed verbatim with
the field left `nil`, per this project's "transcribe, don't tidy" convention.

## 4. `ATTACH_SKIN_MESH` / `SKIN_MESH_RENDER_TYPE.SMRT_UV_ANIM` — what Phase C infers vs. knows

Phase C (`SERV_IRUHADEV_RANOX_ATTACH_SKIN_MESH`, `X2GUNPC.cpp`/`X2SubEquip.h/.cpp`) is a
best-inference implementation, not a confirmed reproduction of June2017's own behavior —
the June `x2.exe` cannot be inspected directly (see §6). What's known vs. inferred:

**Known directly from the Lua data** (`march2014/update/data037/FIRE_DEVIL_FIGHTER.src.lua`
and 15 siblings across the whole `FIRE_DEVIL_*`/`SECRET_*` fighter+berserker family): the
block is `ATTACH_SKIN_MESH0 = { VALUE = 0.01, ATTACH_MESH_NAME = "...", SKIN_MESH_RENDERTYPE
= SKIN_MESH_RENDER_TYPE.SMRT_UV_ANIM }`. No `ATTACH_BONE_NAME`, no offset, no scale, no
index beyond `0` (no NPC in this port uses `ATTACH_SKIN_MESH1` or higher).

**Inferred — the block is a skinned whole-body shell, not a rigid bone attachment.**
Evidence: it carries no `ATTACH_BONE_NAME` (the rigid `ATTACH_MESH` path requires one to
locate a `MultiAnimFrame` via `GetCloneFrame`); the referenced mesh is named `..._Body.x`
(a whole second body, not a prop); a second body shell rendering over the first matches the
reported symptom (Demon Fighter's missing glow) exactly. Phase C therefore anchors the
sub-equip to the unit's **root frame** (`GetCloneFrameRoot()`) rather than any specific
bone — a choice with no direct evidence for or against, since nothing in the Lua names a
bone. The unexplained `VALUE = 0.01` is **not accounted for by Phase C at all** — it is
read by nothing, and no hypothesis for it survived (candidates considered: a UV-scroll
speed multiplier — too small by two orders of magnitude against the `1.0f`/frame-time
convention used elsewhere; an alpha/opacity value — no alpha field exists on
`CX2SubEquip`'s render param path Phase C uses; a scale — `ATTACH_MESH`'s own `ATTACH_SCALE`
field defaults to `1.f`, and `0.01` would make the shell invisible-small, contradicting the
symptom). It is left unread; if the June binary settles what it means, revisit.

**Inferred — the engine mechanism reused for the glow is the enchanted-weapon glow path.**
`SMRT_UV_ANIM` has no `CKTDGXRenderer::RENDER_TYPE` member — the engine's only
UV-scrolling facility is `RenderParam::texOffsetStage2.y` advanced per frame, which is how
`CX2Eqip.cpp:412-419` renders enchanted-weapon glow via `RT_LIGHT_FLOW_LOCAL_Z`. Phase C
wires `CX2SubEquip::OnFrameMove` to the same mechanism, with the same fallback scroll speed
(`1.0f`, taken verbatim from `X2Eqip.cpp:414`'s own `#else` branch — not invented). **One
real limitation, disclosed rather than papered over**: `CX2Eqip` renders the glow as a
*second*, additive pass over a copy of the render param (`m_EnchantRenderParam`), keeping
the base mesh's normal cel-shaded look underneath. `CX2SubEquip` has only one render param
and one draw call — Phase C's `RT_LIGHT_FLOW_LOCAL_Z` assignment therefore **replaces** the
whole-body shell's normal shading rather than overlaying it. Whether that reads as "a body
glow" (right) or "a flat scrolling texture instead of a shaded body" (wrong) is something
only a play-test can answer. If it looks wrong, per the plan's own instruction: leave the
attachment rendering with the default `RT_REAL_COLOR` cel-shading technique and drop only
the `SetUVAnimGlow` call — the mesh will still be attached and visible, which is the
majority of the reported symptom (currently the mesh isn't attached at all), just without
the scroll. Do not spend further guesswork tuning the single-pass approximation; a second
render pass on `CX2SubEquip` (mirroring `CX2Eqip`'s `m_EnchantRenderParam` duplication) is
the correct fix but is new engine surface, not a Ranox-scoped change.

## 5. Data that exists in neither client

Not RE targets — no amount of binary analysis on June2017's `x2.exe` produces these, since
they were never shipped as data by either client:

- **5 unrecoverable effect sets**: `EffectSet_FIRE_DEVIL_BERSERKER_FIRETEMPLE_Attack{A,B}_Ready`,
  `EffectSet_Geyser_Mulsayrena_SpecialAttackA_Fire`, `EffectSet_NUI_GEYSER_ALUMINUM_Attack_A_Ready`,
  `EffectSet_SECRET_MAGI_DashEnd`. All five are wind-up/telegraph states whose owning NPC
  states carry their own `DAMAGE_DATA` block, so these lose visuals only, not damage. They
  stay silent — Phase B's diagnostic will name them every time they're requested, and that
  is correct behavior, not a bug to chase.
- **1 unrecoverable particle sequence**: `SA_AEM_ELEMENTAL_STORM_FX_M01`, referenced by one
  damage effect that keeps all its other visuals.

Listing these here alongside the genuine engine gaps above is deliberate: it prevents
someone later mistaking a missing-asset problem (nothing to reverse-engineer, nothing to
implement, just absent) for a missing-engine-feature problem (something a binary dump could
theoretically recover).

## 6. Constraint on RE-ing the June2017 client: `x2.exe` is packed

**Read this before spending time on string-anchor analysis of the June2017 executable.**
Re-verified directly against `14JUN17/data/x2.exe` (17,876,512 bytes) this session, not just
carried from an earlier claim: its PE has 6 sections, of which the first and fourth have no
name at all and two more are `rhwpapaz` and `cadidffj` — not a plausible compiler/linker
output, and consistent with Themida/WinLicense packing. A byte-level scan of the full file,
in both ASCII and UTF-16LE, finds **zero** hits for:

- Every key name in §1 above (`LOOP_ADD_POS_X`, `NUMERICAL_DATA_INDEX`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`, `APPLIED_SKILL_ID`)
- Control strings that must be present in an unpacked build (`POSITION_TYPE`,
  `DamageEffect`, `EFFECT_SET_LIST`)
- Even `d3d9.dll`, which every unpacked DirectX 9 client link-imports by name

The string-anchor technique that made the rest of this document actionable (grep a
candidate name, read the surrounding disassembly) **does not work on the file as shipped**.
Recovering any of §1's parse sites, §4's `VALUE = 0.01` meaning, or a ground-truth
`ATTACH_SKIN_MESH` rendering path requires a runtime memory dump of the unpacked, running
process with a rebuilt import address table first — a separate, larger undertaking than
anything in this document. Recorded here so it's known going in, not discovered after
picking this list back up.

## Verification

Phase B's diagnostic (`SERV_IRUHADEV_RANOX_EFFECT_DEBUG`, grep `[RANOX_EFFECT_MISSING]` in
`offline_server.log`) is the live check that this document's §5 list is complete and
nothing else is silently missing: after a play-test, every name it logs should be one of
the 5 effect sets in §5, or a genuinely new gap this document needs to grow to cover — never
one of the 196 Phase A is supposed to have restored.
