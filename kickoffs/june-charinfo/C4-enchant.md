# June character info window - Phase C4 add-on - The unit viewer's attribute-enchant family (June's handle lists)

Moved here from june-march-divergence D1-3g (user, 2026-10-29: drift, the viewer is C4's subject; nothing in it is a `replace` row of
`june_march_divergent.tsv`, and the game plays without it). It runs inside or right after C4, in the same session when C4 has room.

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-charinfo C4` and read this file with `C4.md` |
| Plan doc | `JUNE_CHARINFO_PHASES.md` (branch `main`); the Done notes below are in `JUNE_MARCH_DIVERGENCE_PHASES.md` |
| Depends on | C3 and C4's viewer work (same `X2UnitViewerUI` files; D1-3f's reform and equip-collision edits there must be committed first); june-march-divergence D1-3c / D1-3e are done |
| Flag | new `SERV_IRUHADEV_JUNE_CHARINFO_VIEW_ATTRIB_ENCHANT` (`#error` without `SERV_IRUHADEV_JUNE_DIVERGE_WEAPON_EFFECT_SHOW`, whose viewer `CanShowWeaponEffect` it calls) |
| Log tag | none (add one only if a check needs a log line) |
| Review | `kickoffs/june-charinfo/C5-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules" "D1-3c Done (2026-10-29, Opus 5.5; two \`SERV_IRUHADEV_JUNE_DIVERGE_*\` flags, March's body in every \`#else\`)" "D1-3e Done (2026-10-29, Opus 5.5; one site under D1-3c's \`WEAPON_EFFECT_SHOW\`; viewer family -> D1-3g, 263 -> june-engine R6c)"
```

and the `(Diverge D1-3e)` rows of `june2017_re/funcmap.tsv` (`grep "Diverge D1-3e" june2017_re/funcmap.tsv`): they map every June function below.

## Task

June replaced the viewer's seven scalar attribute-enchant handles (`m_hSeqWeapon_Fire` / `2` / `3`, `m_hSeqWeapon_Wind` / `2` / `3` / `4`) with two
handle vectors: the fire list (`+0xd90`) and the hand list (`+0xda0`). Every function that touches them changed with it, so port the family whole
(no partial updates), each June body into March's function under the flag, March's in `#else`:

| March (`X2UnitViewerUI.cpp`) | June |
|---|---|
| members, ctor (`:45`), dtor (`:188`) | `31fab020`, `31fab920`: two `vector<CParticleEventSequenceHandle>` (names follow the class: `m_vecSeqWeaponFire`, `m_vecSeqWeaponWind`) |
| `CreateEnchantedWeaponParticleAtWeapon` (`:5519`), `...AtHand` (`:5565`) | `31f9d1f0`, `31f9d3b0` (variadic count + names; push_back, emit 0) |
| `InitEnchantWeaponEffectForSword` (`:5363`), `...ForBow` (`:5445`), `InitEnchantedWeaponEffectAtHand` (`:5624`) | `31f9ea20`, `31f9eeb0`, `31f9d570`: compare every EDT case and name |
| `DeleteEnchantWeaponEffect` (`:5728`) | `31f9da10` (destroy both lists, clear) |
| `ProcessEnchantedWeaponEffectAtHand` (`:5746`) | `31f961a0` (per-type bone, a vector of emit rates; Chung on `Bip01_R_Hand` with five rates, Elesis with Elsword) |
| `ProcessEnchantWeaponEffectForEve` / `Ara` / `Add` / `Sword` / `Bow` (`:5895`-`:6117`) | `31f92de0` / `31f92ec0` / `31f92f70` / `31f9daf0` / `31f932e0` (hide test `CanShowWeaponEffect`, `31f91230`) |
| `OnFrameMove` dispatch (`:889`-`:954`), `SetUnit` (`:2705`) | `31fa8ce0`, `31faa6f0` (the enchant parts only) |
| `OnFrameRender_Draw` (`:1586`), `Render_DrawForAvartaScreenCapture` (`:1867`), `SetShowAttribEnchant` (`:6320`) | `31fa00b0`, `31f97960` (R4a owns its vslot 0x10 role: port only the enchant part), `31f91f90` (attribute part) |

- The June-only frame moves `31f93050` (Lu/Ciel), `31f93200` (Rose), `31f940b0` (Ain) and cases 10-12 go to checklist 7as (already listed by D1-3e).
- Read stack slots from the disassembly where the decompile mixes locals (the D1-3b lesson); constants with `june2017_re/charport_consts.py --fn <va>`.
- `MODS.md` entry, a `JUNE_BUGS.md` row per June bug kept, funcmap rows `(CharInfo S34)` (the 16 existing `(Diverge D1-3e)` rows map every June function above). Delegate bytepatch specs you write, builds and flagcheck
  runs to Sonnet subagents. Record it in C4's Done note, update the STATUS row, commit with explicit pathspecs. Split again if it fills the session.
- **Gate (added to C4's):** the character window (U) shows a wind / light / fire / curse weapon's attribute sparks on the model, and none while riding or during an
  avatar emotion; the hand sparks follow June's per-type bone.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md). No `June` prefix (user, 2026-10-28).

## Lessons from D1-3e (2026-10-29)

- D1-3c's guess that `+0xd90` was Add's own list was wrong: it is the viewer's fire list, read by every frame move. Check a member offset against
  every function that reads it (`grep "+ 0xd90\]"` over the viewer range's disassembly) before calling it June-only.
- Finish like any charinfo phase (x2-phase "Finish"): flagcheck, build, DEPLOY OK, Done note, play-test hand-off with the gate above.
