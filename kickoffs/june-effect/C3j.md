# June effect completion - C3j - June's NPC bounding-state fallbacks (trace first, then a user decision)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3j` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3d-3 review |
| Flag | `SERV_IRUHADEV_JUNE_BOUNDING_STATE` if the user says port; a new `SERV_IRUHADEV_JUNE_FIX_*` only if they choose to deviate |
| Log tag | `JUNE_FX C3j` |
| Review | `kickoffs/june-effect/C3j-review.md` if code lands |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-3 - Review"
grep -n "(C3d-3 review)" june2017_re/funcmap.tsv
```

## Task

June's NPC `InitState` gives the three bounding ids fallbacks that C3d-3 did not port (`327992a2-327992fd`, verified against March's enum by
`enumprobe.py`):

- DOWN = type 0x21, else `m_CommonState.m_DownDamageAir` (`+0x4098`);
- LANDCRASH = type 0x22, else type 0x13 (`STATE_TYPE_DOWN_DAMAGE_AIR_LANDING`), else `m_DownDamageAir`;
- UP = type 0x23, else `m_UpDamage` (`+0x8690`).

No June NPC script names the three types, so in June every NPC reacts to RT_BOUNDING through these fallbacks. March leaves them 0, and the
hit is refused.

**The reason this was not ported in the review:** `CX2GUNPC::CommonStateStart` calls `3275a070` unconditionally (`@327af3aa`). It sets
`nowSpeed = m_vJuneBoundingBackSpeed` whenever `nowState == UP`. With the fallback, `UP == m_UpDamage`, and June's RT_UP case
(`32769b72-32769bb3`) writes `nowSpeed.y` *before* `StateChange_Reaction_FIRST( m_UpDamage )`. Read literally, June's every NPC
up-launch is overwritten with the last bounding back speed (zero before any bounding hit). The only exception is a state that sets its
own `SPEED_X/Y` later in `CommonStateStart`, and only 663 of 2,376 June NPC up-damage states carry `SPEED_Y`. That is implausible for a
shipped game, so first find what the reading misses:

1. Is `+0x8454` (decrypted, `movzx` low byte) `nowState`, or another id (reserved, future)? Check its writers and March's `syncData`.
2. Is `+0x40f0/+0x40f4` written anywhere else? The review's operand sweep found only the two RT_BOUNDING stores; also check `rep stos` or
   memset in the `CX2GameUnit` ctor.
3. Does anything after `CommonStateStart` re-apply the reaction's `nowSpeed.y`? The NPC DamageReact `327b1f30` calls DamageReactStateChange `32769240` at `@327b3253` (corrected by C3i: `32769060`, called at `@327b52cf`, is a separate one-argument function).
4. The two virtuals `IsBoundingUp` (`3275a010`, vslot 0x2f4) and `IsBoundingLandCrash` (`3275a030`, vslot 0x2f8, also tests `+0x8404`)
   have one caller each, both in class code (`32b34726`, `32951511`): name the classes, add them to checklist 7l, and port the base
   virtuals (engine side). `32759fd0` (any bounding state) has no caller.

Then hand the user the verdict. Either June really does this, and porting it verbatim changes every March monster's launch (ask: port
verbatim, or port with a `SERV_IRUHADEV_JUNE_FIX_*`), or the trace shows why it doesn't, and the fallbacks port as a plain transcription
under `SERV_IRUHADEV_JUNE_BOUNDING_STATE`.

Current state, latent: without the fallbacks an RT_BOUNDING hit on an NPC still writes `nowSpeed.y` and `m_vJuneBoundingBackSpeed`, then
changes no state (`StateChange_Reaction_FIRST` refuses `state <= 0`). It matches neither June nor March; nothing reaches it yet (no shipped
DamageEffect uses RT_BOUNDING).
