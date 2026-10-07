# June effect completion - C9h-2a review - the pet / riding-pet viewer effect sets (viewer + engine)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-2a review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2a done (its Done note), regression play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_PET_VIEWER_FX` |
| Log tag | `JUNE_FX PET_VIEWER` (`python tools/logs.py --grep "JUNE_FX PET_VIEWER"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-2a - Done" "C9h - Done"
grep -n "Effect C9h" june2017_re/funcmap.tsv
```

## Task

Review the C9h-2a code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **Variant branch** (`X2NPCUnitViewerUI.cpp`, ResetPet, before `EndTable // INIT_MOTION`) vs `32e50de0 @32e517f1-32e51a6f`: the three tests
  (`iLv == 3` after the clamp, variant != 0, the templet's `+0x6c` vector size cast to `char` == 4), the `len > 3` / `len > 5` guards, the
  `substr` cut lengths (3 and 5), the literal order (variant 1 / 2 / else 3), and that the suffix helper's non-empty result replaces the literal.
  The destination of each assignment (motion string, XET string, the scratch string) was read from the `LEA ECX,[ESP+..]` offsets, not from the decompile.
- **`326cc990`**: pet 39 -> `H.xet`; pet 134 with variant 1 or 2 -> `2.xet`; else empty.
- **ResetRidingPet vs `32e51ea0 @32e52ad1-32e52ce0`**: no group-1 stop; the by-UI list is collected before the viewer list plays; each name goes through
  `PlayEffectSet( name, skin )` after both tables close, a valid handle is pushed.
- **`PlayEffectSet( name, skin )` vs `321e28d0`**: null templet / null skin -> invalid; `_CreateEffectSetInstance` args (no unit, no trace target, no square unit, no pet);
  the flag is set on the instance, not before the create call.
- **The three `+0xdd` consumers**: `GetEffectPosition @321dd0e0-321dd19e` (ESM_FIELD, no square unit; BONE_NAME then RIDING_PET_BONE_NAME; both empty still runs the
  transform; no `SetEffectSetOffset`), `GetEffectPositionByBoneName @321dd926` (non-empty name only), particle creation `@321f2522` (OverUI, `PT_2D_PLANE`,
  resolution convert). The three doubles (`3347e350` 512.0, `3347e348` 384.0, `33436c90` -1.0) were read out of `unpacked_x2.exe` by C9h-2a; no need to repeat.
- **Frame hooks**: `32e4f600` base update after the skin block, `32e4f820` base prepare on the shown path only (hint dropped), `32e4f8f0` base draw after the skin draw.
- **Handle stop / SetScale**: a copy per handle, the vector never erased; SetScale writes `m_vScale` and clears `m_bDamageEffectScale`.
- **The `+0xe0` branch** (`@321dd1a3`) is NOT ported: that is C9h-3.
- The two corrections C9h-2a made to the C9h kickoff and funcmap (base Draw call; the stop works on a copy): confirm them in the disassembly.

Hygiene is one command: `python tools/flagcheck.py JUNE_PET_VIEWER_FX --range <first C9h-2a commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row (Always.h) and the EOL-mismatch count.
Write "C9h-2a - Review" into the plan doc, update the STATUS row, and commit. The next owners are C9h-2b (Opus 5.5 high) and C9h-3;
check their kickoffs exist.
