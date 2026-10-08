# June effect completion - C9h-4c - UPDOWN_CANCEL_AFTER and June's RidingAttackZEventProcess (C9h-3b's `+0x620` arm)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-4c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4 (identify), C9h-3b (the `+0x620` arm). Not at the same time as C9h-4a / C9h-4d (same `X2GUUser.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_UPDOWN_CANCEL` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX UPDOWN_CANCEL` |
| Review | `kickoffs/june-effect/C9h-4c-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4 - Done" "C9h-3b - Done"
grep -n "Effect C9h-4\|^32031e60\|^320347d0" june2017_re/funcmap.tsv
```

March: `X2Lib/X2GUUser.cpp` CommonStateStartFuture's cancel-afters (:14489) and CommonStateStart's (:15429), `RidingAttackZEventProcess`
(:38374 region), the `RIDING_*_CANCEL_AFTER` macros (`X2GUUser.h:190`), and C9h-3b's comment in `CX2GUUser::DoFrameMoveFuture`.

## Task

1. **The member and its two parses.**
   - `UPDOWN_CANCEL_AFTER` goes into a new float with miss value 9999.0. June reads it in CommonStateStartFuture (`@32056597`) and in
     CommonStateStart (`@3205f6f8`), after `USE_COMMON_CANCEL_AFTER` and before `SKILL_CANCEL_AFTER_FLY`.
   - **`USE_COMMON_CANCEL_AFTER`** (`@320563b0`) is the June-only key next to it, in 10 June scripts. Nobody has mapped it. Identify its
     member and consumer here. Port it with this flag if it is engine; if only class code reads it, add a row to checklist 7h.
2. **June's `RidingAttackZEventProcess`** (`32031e60`), the whole function. Its order and keys are in its funcmap row. Its tests are
   March's: time >= 0, and time below the riding pet's anim time. What is new is the UPDOWN branch after WALK and the `3202f460` callee.
   Port it under this flag:
   - It replaces March's `RidingAttackZEventProcess` for state `RIDING_ATTACK_Z`.
   - It is also what June's `DoFrameMoveFuture` runs on the riding pet's ONGROUND arm (`320347d0 @32034872`). Replace C9h-3b's
     fall-through comment there with that call.
   - Its skill cancel calls June's `RidingPetSpecialAttackEventProcess` rewrite `3202f460( 1 )`. That rewrite belongs to backlog Stage 17
     (transformer state sets and the target search). Call March's `RidingPetSpecialAttackEventProcess` with the condition June's `1` maps
     to, and leave a comment naming `3202f460`.
3. **Class readers stay out.** About 25 Elsword functions (`32cf8230..32d06c50`), one Raven function (`32c2b360`) and one Lire function
   (`32cb9ee0`) read `+0x8ca4` in their own EventProcess. Checklist 7h lists them; the character ports bring them.

**Reach.** The key is only in June-only riding pets (Lord_Of_Inferno, Lord_Of_Inferno_2P, Perkisas) and in June's `RAVEN_FIGHTER`. The
riding pet's write of it comes in C9h-4e. The phase is inert on March data, so the play-test is a riding-pet regression pass: Z attack,
then cancel by walk, dash and skill.

**Done gate:** a `JUNE_FX UPDOWN_CANCEL` line per RIDING_ATTACK_Z entry while `_DEBUG` is on (the four cancel-after values), and the
strings are in the deployed exe.
