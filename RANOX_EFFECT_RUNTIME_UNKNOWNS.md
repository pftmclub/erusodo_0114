# Ranox effect runtime — untraced code paths

Companion to `RANOX_EFFECT_ENGINE_GAPS.md`. That document is scoped to **Lua data
keys with no C++ parse site**. This one is scoped to **runtime code paths whose
behaviour has not been traced** — the functions that decide whether a Ranox
monster's projectile actually hits something, where reading the call site was not
enough to predict the outcome.

Written after the 2026-09-23 session in which the mesh-templet merge
(`GameMajorXMeshPlayer.txt`, +75 templets) and the particle bank were packed, and
the reported symptom changed from *"nothing is thrown"* to **"the projectile
launches and renders perfectly, but its hitbox hits nothing."** Visual restoration
is done; hit detection is the open problem.

Nothing here is a fix. It is a list of what is *known*, what is *ruled out*, and
what is *unknown*, so the next session starts from evidence rather than from the
same two dead ends.

---

## 1. The hit-detection chain, as traced

This is not documented anywhere else in the repo. For a `DamageEffect` whose
`MAIN_EFFECT_NAME` resolves to an XMeshPlayer templet, the hitbox is **not**
declared in the `.txt` templet and **not** in the `.lua` block. It comes from the
`.X` mesh file's own frame names.

| Step | Site | What happens |
|---|---|---|
| Hit test | [X2DamageManager.cpp:1740](X2Lib/X2DamageManager.cpp#L1740) | `CollisionDataCheck( pAttackerMainEffect->GetXSkinAnim()->GetAttackDataList(), pDefender->GetCollisionListSet(), ... )` |
| Attack list, per instance | [KTDGXSkinAnim.cpp:556-568](KTDXLIB/KTDGXSkinAnim.cpp#L556) | copies each `CollisionData` from the device mesh, calls `LinkCollisionDataFrame` |
| Attack list, per device mesh | [KTDXDeviceXSkinMesh.cpp:770-878](KTDXLIB/KTDXDeviceXSkinMesh.cpp#L770) | built by **frame-name string parsing** |

The frame-name grammar, read directly from the parser:

- `ATTACK_LINE_START<n>_<name>` + matching `ATTACK_LINE_END<n>` → `CT_LINE`
  ([:770-829](KTDXLIB/KTDXDeviceXSkinMesh.cpp#L770))
- `ATTACK_SPHERE<n>_<name>` → `CT_SPHERE` ([:831-877](KTDXLIB/KTDXDeviceXSkinMesh.cpp#L831))
- `<name>` is optional — it is whatever follows the first `_` after the digits, and
  is empty when there is no `_`. It lands in `CollisionData::m_Name`.
- `m_fRadius` is taken from `pFrame->fBoundingRadius`, computed by
  `D3DXFrameCalculateBoundingSphere` over that frame's geometry.

**Consequence worth internalising:** a projectile's damage volume is authored by the
artist, inside the mesh, as a specially-named frame. No amount of `.lua` or `.txt`
editing can add one.

## 2. Ruled out — do not re-investigate these

Both of these were live hypotheses during the 2026-09-23 session and both are dead.
They are recorded here because each one is individually plausible and will be
re-invented otherwise.

### 2a. "The recovered templets lack an `_AttackBox` companion" — **FALSE**

The theory: working projectiles pair an invisible `DummyAttackBox_*.X` hit volume
with the visual, e.g. `MAG_STONE_Attack_A_Fire_AttackBox`
([GameMajorXMeshPlayer.txt:64549](march2014/data036/GameMajorXMeshPlayer.txt#L64549)),
while `MAG_CHARGER_Attack_A` ([:64524](march2014/data036/GameMajorXMeshPlayer.txt#L64524))
points straight at the visual mesh `Arme_FireballS.X` with no companion — so it has
no hitbox.

**Refuted by reading the mesh.** `Arme_FireballS.X` contains
`ATTACK_SPHERE1_FireballS` (and `COLLISION_SPHERE1_FireballS`). It has a perfectly
good attack sphere of its own. Verified frames:

| Mesh | Attack/collision frames |
|---|---|
| `DummyAttackBox_50x50x50.x` | `ATTACK_SPHERE1` |
| `Arme_FireballS.X` | `ATTACK_SPHERE1_FireballS`, `COLLISION_SPHERE1_FireballS` |
| `ALUMINUM_Weapon_Mesh01.X` | `ATTACK_SPHERE01` |

All three parse cleanly under the §1 grammar. The templet is not missing a hitbox.

### 2b. "`LinkCollisionDataFrame` is failing" — **FALSE (as a hard failure)**

That is the only *logged* failure in the whole chain: it raises `KEM_ERROR432`
([KTDGXSkinAnim.cpp:563](KTDXLIB/KTDGXSkinAnim.cpp#L563)). `ErrorLog.txt` after a
Ranox play-test contains **zero** occurrences of it. Codes actually present:
`10142` ×22, `36` ×16, `429` ×10, `428` ×10, `73` ×1, `10001` ×1.

**Caveat that keeps this from being a full clearance:** the 2013-09-13 comment at
[KTDGXSkinAnim.cpp:546-548](KTDXLIB/KTDGXSkinAnim.cpp#L546) records that the
`SAFE_DELETE`/`continue` was deliberately removed to stop a crash, so a datum that
*fails* to link is logged and then **kept in the list anyway**. Absence of 432
proves the link did not hard-fail; it does not prove the frame pointer is correct.

## 3. Tooling — reading `.X` meshes (`strings` gives false negatives)

The client's `.X` files are **MSZip-compressed**, header `xof 0303bzip0032` — note
`bzip` in the magic is a misnomer, the payload is raw deflate, not bzip2. A plain
`strings` scan returns **nothing** on `DummyAttackBox_50x50x50.x`, a mesh that is
known-positive by construction. That false negative is the same trap
`RANOX_EFFECT_ENGINE_GAPS.md` §1 documents for source-string searches; apply the
same discipline (always run a known-positive control).

Container layout: 16-byte header, 4-byte total uncompressed size, then repeating
blocks of `[2B uncompressed size][2B compressed size]['CK'][raw deflate]`.

```python
import zlib
def decomp(p):
    d = open(p, 'rb').read()
    if d[:4] != b'xof ':      return None
    if b'bzip' not in d[:16]: return d          # already plain text
    out, off = b'', 20                          # 16 header + 4 total-size
    while off + 4 <= len(d):
        cs = int.from_bytes(d[off+2:off+4], 'little')
        if cs == 0: break
        blk = d[off+4:off+4+cs]
        if blk[:2] != b'CK': break
        out += zlib.decompress(blk[2:], -15)    # skip 'CK', raw deflate
        off += 4 + cs
    return out
```

Extracted mesh archives live at
`237311/24965799/kom_contents/data0NN_kom/` (current build) and
`237311/14JUN17/data/data0NN_kom/` (the June2017 reference).

## 4. Open unknowns

Ordered by expected payoff. Each names the exact anchor and the exact question, so
none of them needs re-deriving.

### 4.1 `.txt` templet keys have never been audited against the parser — **top item**

`RANOX_EFFECT_ENGINE_GAPS.md` §1 ran exactly this audit for the **Lua** formats
(`DamageEffect` / `EffectSet`) and found 5 genuine unread keys. **The equivalent
audit for the XMeshPlayer `.txt` templet format has not been done at all**, and the
75 templets just merged came from June2017 into a March2014 parser.

Keys visible on the recovered Ranox templets that need a confirmed parse site in
`KTDGXMeshPlayer.cpp`: `UseLand`, `CrashLand`, `Gravity`, `ElasticCoeff`,
`ElasticCoeff_X` (note it takes a `random(a, b)` expression), `NOT_CHECK_LAND_TIME`,
`LifeType`, `LightPos`, `DrawCount`.

Why this is first: `MAG_CHARGER_Attack_A` sets `UseLand = TRUE`, `CrashLand = TRUE`
and `Gravity = XYZ(0, -2000, 0)`. If `CrashLand`/`UseLand` semantics differ between
the two engine versions, a gravity-arced projectile could be dying on ground contact
before it ever reaches the target — which matches *"launches perfectly, hits
nothing"* exactly, without requiring anything to be wrong with the hitbox at all.

Use a bare-identifier search, not a quoted-literal search (§3's lesson and
`RANOX_EFFECT_ENGINE_GAPS.md` §1's).

### 4.2 `CollisionDataCheck` / `CheckCollisionGuard` bodies — never read

> **Answered (Phase 9, 2026-09-29): no `m_Name` filter in either engine.**
> - **March.** Every `CollisionDataCheck` overload in `KTDXCollision.h` tests only `m_bEnable`, the
>   type, the points and `GetScaleRadius()`. `CheckCollisionGuard` (`:2107`) compiles its `#else`
>   branch: guard seen → impact = guard point, `CT_GUARD`; else the hit point.
> - **June.** `EffectToUnit` `FUN_3262b6e0` inlines the check at `@3262bc1a-3262eebd`. It copies each
>   defender datum's `m_Name` into a damage-manager string at `@3262c181-3262c18d`, and nothing
>   in the function reads that string. Its guard helper, `FUN_32640770`, is March's `#else` branch
>   exactly.
> - **What does differ** (now ported under `SERV_IRUHADEV_JUNE_COLLISION`):
>   - sphere×line and line×sphere use the closest point on the segment, clamped to its ends;
>   - their impact point is halfway between the sphere centre and that point;
>   - `USE_ADVANCE_COLLISIOIN_CHECK` moves the sphere×sphere impact point.
>
>   Sphere×sphere hit/miss, line×line (March's `LineToLine` inlined branch for branch) and the
>   guard rule are unchanged.
> - **The same code is in June's UnitToUnit check.** `FUN_32611970` uses the same helpers
>   (`31c7b560`, `326406b0`, `32640770`), so June changed the shared header. Only `EffectToUnit`
>   is switched over here. The other March callers (`UnitToUnit`, `UnitToEffect`, `MeshToUnit`,
>   `CollisionDataToUnit`) keep March semantics: a known gap.
> - **The discriminator below is answered by the port's diagnostics.** `JUNE_FX HIT … TEST` lines
>   log the fireball's datum centre next to its mesh position.

Only signatures have been read: [KTDXCollision.h:457](KTDXLIB/KTDXCollision.h#L457),
[:703](KTDXLIB/KTDXCollision.h#L703), [:949](KTDXLIB/KTDXCollision.h#L949),
[:1332](KTDXLIB/KTDXCollision.h#L1332); `CheckCollisionGuard` itself is at
[:2107](KTDXLIB/KTDXCollision.h#L2107) and is unread.

Specific question: **does it filter on `CollisionData::m_Name`?** Under §1's
grammar the fireball's sphere yields `m_Name = L"FireballS"` while every
known-working dummy box yields `m_Name = L""`. If names are matched or required
empty, that alone explains the symptom.

Cheap discriminator before reading any disassembly: Arme's own player-side fireball
uses the same mesh. **If a player Arme fireball lands damage in this build,
`m_Name` is ruled out** and the difference is elsewhere.

### 4.3 Does `m_fRadius` scale with the instance's `Size`?

> **Answered: yes, in both engines, the same way.**
> - **The formula.** Radius = `m_fRadius × m_fRadiusScale × scale`. The scale is the X length of
>   the datum's combine matrix, or the Y length when `m_bUseRadiusScaleY` is set.
> - **June.** `FUN_3262b6e0` reads `+0x90 × +0x8c × (+0x94 ? y : x)` of `FUN_326406b0( +0x5c )`, and
>   `326406b0` is `GetDecomposeScaleXY`.
> - **March.** `GetScaleRadius()` does the same under `X2OPTIMIZE_DAMAGEEFFECT_COLLISION` and
>   `EVE_ELECTRA`, both ON by `flagprobe.py`.
> - **What that means.** The combine matrix is frame × world, so the instance's `Size` is in it.
>   The tested radius is the unscaled device radius times `Size`.
> - **Caveat, in both engines.** An unlinked datum (`m_pCombineMatrix == NULL`) reads an
>   uninitialised scale. The Phase 9 `TEST` line prints `linked=`.

`m_fRadius` is set once per *device* mesh from `pFrame->fBoundingRadius`
([KTDXDeviceXSkinMesh.cpp:873](KTDXLIB/KTDXDeviceXSkinMesh.cpp#L873)), i.e. shared
across every instance. The templet independently sets
`Size = XYZ(0.600, 0.600, 0.600)` on the rendered instance. Unknown whether the
collision test applies instance scale to the radius, or tests the unscaled device
radius. A rendered-small / tested-large mismatch (or vice versa) would desync the
visual from the hitbox.

### 4.4 The silent zero-radius drop

> **Answered: June drops it the same way, silently. It does not apply to the fireball.**
> - **June has the same parse.** June's `KTDXDeviceXSkinMesh` frame parser is `FUN_31d4f8b0`,
>   which references every `ATTACK_SPHERE` / `ATTACK_LINE_START/END` / `COLLISION_*` literal
>   (`0x33453248`…).
> - **The compare.** Its `ATTACK_SPHERE` copy (`@31d5183b-31d5184d`) is
>   `fld radius; fldz; fucompp; test ah,0x44; jp create`. Emulated with `x87test.py`
>   (`make_func(0x44, True)`), it falls through to the drop for `+0.0` and `-0.0`. It creates the
>   datum for anything else, including NaN. That is March's `== 0.0f` test.
> - **No log in either engine.**
> - **`Arme_FireballS.X`'s frame is not empty.** Decompressed and tokenised as binary `.X`,
>   `ATTACK_SPHERE1_FireballS` has a child frame holding a 24-vertex box, 50×50×50. So its bounding
>   radius is about 43, not 0.

[KTDXDeviceXSkinMesh.cpp:851-862](KTDXLIB/KTDXDeviceXSkinMesh.cpp#L851): if
`fBoundingRadius == 0.0f`, the function `return`s and the attack datum is **never
added**, with no log line of any kind. This is the one failure in the chain that is
completely invisible — §2b's `KEM_ERROR432` does not cover it.

Now cheaply checkable thanks to §3: decompress the mesh and inspect the
`ATTACK_SPHERE*` frame's actual geometry. An attack frame that is a named-but-empty
frame (no mesh data under it) yields radius 0.

### 4.5 `FIX_EFFECTTOUNIT01` — defined or not?

> **Answered: defined** (`flagprobe.py`, US_SERVICE: ON). This is also the only way `:1926`
> compiles, since `pAttackerMainEffect` is declared inside that `#ifdef`.
>
> June has the same guard, `@3262bbfd-3262bc14`: `GetMainEffect` (`31f6eb30`) NULL, or its
> `+0x3c0` skin anim NULL, skips the unit.

At [X2DamageManager.cpp:1733-1737](X2Lib/X2DamageManager.cpp#L1733), if this macro
is defined, a NULL `GetXSkinAnim()` silently `continue`s past the **entire** hit
test. Its definition state under `US_SERVICE` has not been established.

Resolve with `cl /EP` on a probe, per the standing project rule — **not** by reading
nested `#ifdef`s. (`RANOX_EFFECT_ENGINE_GAPS.md` §1 is the cautionary tale.)

### 4.6 The effect→NPC direction is unlocated

> **Answered: the premise was wrong. `EffectToUnit` is the effect→NPC path too.**
> - **Every unit is a defender.** Both engines loop over every unit: March `GetUnitNum/GetUnit`,
>   June game vtable `0xf8`/`0xb0` `@3262b7a7-3262b7e4`. Users and NPCs alike.
> - **The `CX2GUUser` cast is only a filter.** It sits inside `ADDED_RELATIONSHIP_SYSTEM`
>   (`ONLY_RELATIONSHIP`) and `LOCK_ON_USER_ONLY_ON`.
> - **The per-unit filters match March.** June runs a team/ally pre-filter, `FUN_32622990`, then
>   the dead / not-live and already-hit checks (`@3262bb55-3262bbf6`).
> - **Not ported.** `FUN_32622990`'s extra PvP/field conditions, and the `+0x91e4` early-outs at
>   the top of June's `DamageCheck` `3262f5a0`. Both are recorded here and no data exercises them.
> - **The neighbouring paths.** `UnitToEffect` (`32622280` → `326413c0`, Phase 7d) is units
>   hitting effects. `CollisionDataCheckFromUnitToUnit` is melee.

The function traced at `X2DamageManager.cpp:1690-1741` casts `pDefender` to
`CX2GUUser` ([:1699](X2Lib/X2DamageManager.cpp#L1699)) — it is the
**monster-projectile-hits-player** path. That is the right path for the Mag Charger
fireball.

It is **not** the path for the symptoms where a *player* must be hit by nothing, or
where thrown weapons must damage monsters. `CollisionDataCheckFromUnitToUnit`
([:3256](X2Lib/X2DamageManager.cpp#L3256)) and the `UnitToEffectCB` lambda
([:1936](X2Lib/X2DamageManager.cpp#L1936)) are the neighbouring paths; neither has
been traced.

### 4.7 `ATTACK_TIME_START` / `ATTACK_TIME_END` units

> **Answered: seconds, window `[start, end)`, the same in both engines.**
> - **Where it's evaluated.** June `CEffect::OnFrameMove` `31f88790@31f8a166-31f8a201` tests
>   `start <= t < end`, then calls `DamageCheck`. That is March `X2DamageEffect.cpp:3077/3086`.
> - **What `t` is.** The skin-anim time (mesh `+0x3c0` → `+0x214`), or the mesh global time
>   (`+0x3e0`) under `USE_GLOBAL_TIME`. March's anim time is `m_fAnimCurrTime = td.Position`
>   (`KTDGXSkinAnim.cpp:1107`), the D3DX track position, in seconds.
> - **Defaults.** 0 and 100.0 (`fld [334355b8]`), i.e. effectively unbounded.
> - **So `SMITH_STAIN … ATTACK_TIME_END = 1` means one second** of global time (it sets
>   `USE_GLOBAL_TIME`). Mag Charger's fireball uses the default window, so it can hit for its
>   whole 2-second life.

Parsed into `m_AttackTime.x/.y` at
[X2DamageEffect.cpp:1564-1565](X2Lib/X2DamageEffect.cpp#L1564), defaults `0.0f` and
`100.0f`. Unknown whether these are **seconds** or a **normalized 0–1 animation
ratio**, and unknown where the window is evaluated. A default of `100.0f` reads like
seconds (i.e. "effectively unbounded"), but `MAG_CHARGER_Attack_A` ships
`DAMAGE_TIME = 9999` and no explicit attack window, while
`DamageEffect_SMITH_STAIN_SpecialAttackA_Black_Hole` ships `ATTACK_TIME_END = 1` —
which is either "1 second" or "100% of the animation" and means very different
things.

### 4.8 `USE_CANNONBALL` has no monster equivalent

> **Answered: user-only in June too; no monster equivalent exists.**
> - The ctor stores the key at CEffect `+0xae8` (`31f7aab0@31f7c5e3`).
> - A displacement scan of `31f50000-31fa0000` finds one reader, the ctor itself at
>   `@31f7cae4-31f7cb35`. It is gated on a live owner whose `+0x264` (`m_GameUnitType`) is 0, i.e.
>   `GUT_USER`, as March `:1813` is.
> - Monster projectiles are simply not expected to need it.

Parsed at [X2DamageEffect.cpp:1496](X2Lib/X2DamageEffect.cpp#L1496), used at exactly
one site, [:1528](X2Lib/X2DamageEffect.cpp#L1528), gated on
`GetGameUnitType() == CX2GameUnit::GUT_USER`. Monster-owned effects can therefore
never take that branch. Unknown whether a monster-side equivalent exists, or whether
monster projectiles are simply expected not to need it.

## 5. A framing correction

The working theory going into this was *"2014 engine running 2017 data, so behaviour
fields silently fall back to defaults."* **The one case traced end-to-end weakens
it:** `MAG_CHARGER_Attack_A` is a June2017 *templet* pointing at `Arme_FireballS.X`,
a mesh that ships in **this build's own** `data002_kom`, carrying a valid attack
sphere that parses cleanly under this build's parser. Nothing about that chain is
version-mismatched.

The version-gap theory remains live for the **templet key format** specifically
(§4.1) — that is a real, unaudited surface. It should not be assumed for assets, and
it should not be the default explanation for any new symptom until §4.1 and §4.2
have been checked.

## 6. Recommended order

1. **§4.2's cheap discriminator** — does a *player* Arme fireball deal damage? One
   play-test, no code. Splits the search space in half.
2. **§4.1** — audit the `.txt` templet keys against `KTDGXMeshPlayer.cpp`. Pure
   reading, no build, and it is the audit that already paid off once.
3. **§4.5** — one `cl /EP` probe.
4. **§4.4** — decompress the mesh, check the attack frame's geometry.
5. **§4.2 proper** — read `CollisionDataCheck` / `CheckCollisionGuard`.

Steps 1–4 are all cheap and none requires a rebuild. Only step 5 is real work.

## 7. Related documents

- `RANOX_EFFECT_ENGINE_GAPS.md` — Lua keys with no parse site; enum gaps; the
  constraint that June2017's `x2.exe` is Themida-packed and cannot be string-anchored
  (§6 there, which is why §4 here leans on the March sources and the mesh files
  instead).
- `LANOX_DEFECT_PHASES.md`, `RANOX_PORT_PLAN.md` — the port phases that produced the
  current state.
- `MODS.md` — local changes and how to revert them.
