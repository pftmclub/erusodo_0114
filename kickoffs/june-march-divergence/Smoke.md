# One function per name - Phase S - The plan-end smoke play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence S` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`), "Phase S - The plan-end smoke play-test" |
| Depends on | D2 done (the deployed exe carries every C and D1 flag and the gate) |
| Flag | none; a fix goes under the flag of the phase that broke it |
| Log tag | none (the C and D1 phases added none); read `ErrorLog.txt`, `offline_server.log`, `offline_packets.log` |
| Review | `D2-review.md` (it depends on this phase) |

## Why this phase exists

The user ruled on 2026-10-29 that no C or D1 phase plays the game: one smoke runs at the end of the plan. Every Done note left its checks as a
"Smoke (plan end)" line. This phase turns them into one play-test hand-off, has the user play it, reads the logs, and fixes what broke.
Nothing is built here unless a fix needs it.

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules"
grep -n -i "smoke" JUNE_MARCH_DIVERGENCE_PHASES.md      # every Done note's check; the seed list below may be stale
python tools/exestr.py --help
```

Memory rules that apply: a play-test hand-off is copy-ready (character + job, skill + slot, steps, expected, failure; a console line is the FULL
line); items come from `/additem <itemID> [qty]` (never the shop, never `/b`); no skill in transcend slots 4 / 9 before transcendence; a "check
the log" means this phase's own lines only; never launch the game.

## Task

1. **Prove the exe is the right one.** `build.py --deploy` is D2's; confirm `DEPLOY OK` there is newer than the last code commit
   (`git log -1 --format=%cd -- X2Lib KTDXLIB X2`) and run `python tools/exestr.py` for one string per big flag family (see each Done note's
   `EXESTR` line). If stale, build once (`python tools/build.py --deploy`, background).
2. **Build the hand-off.** Re-grep the "Smoke" lines; merge them into one run of about 30 minutes, grouped so the user changes character as
   few times as possible. Seed (2026-10-29, check each against its Done note, and drop what D1-3g / D1-2c / D1-3f's old MP sites took with them; ported characters only):
   - **Ported characters only (user, 2026-10-29): Aisha and Eve.** No Elsword, Chung, Ara, Add or any other unported class. Every check that the
     Done notes wrote for an unported class is dropped from the hand-off and listed in the Done note as "not covered" (the `stand-in: <class> not
     ported` guards stay untested until that class is ported). Keep an item only if Aisha or Eve can show it; re-derive its steps for them.
   - **Aisha, a dungeon:** dash combo Z-Z-Z with memo 5 gives no super armour; cool times after a skill use; B-slot skills; end a hyper mode early;
     finish the dungeon (result invincibility); die: body fades and hides, no Sacrifice of Hero heal; a +11 staff shows the lightning glow, a +13
     adds the sparks, none while riding or during an avatar emotion; a wind / light / curse weapon shows the hand sparks; a triple-attribute weapon
     glows; a hyper-active skill's cool time with item 60008065 is cut only in a dungeon; amusement-item pickups (press / missile / water balloon).
   - **Eve, a dungeon:** B-slot skills; glow on both weapons; hand sparks; a Pandemonium Fear holder takes a hit; non-hyper skill cool time with
     Cheat Code: Electronic learned; amusement-item pickups; a mana-drain hit shows no number; die as for Aisha.
   - **Either, riding pet:** in a village and a dungeon the rider sits on the saddle (not at the map origin) and every riding state plays `Ride_N`.
   - **Dungeon / camera (either):** a skill with `SUB_CAMERA_TIMING` plays its sub-cameras; a boss last-kill focus is not pulled back; a death with
     a party / NPC killer logs `GAME     unitUID=... died`; an NPC holding a freeze takes no state change; a party bot's passives apply.
   - **UI / quests (either):** dialogs hidden (the hide key): no HP+ / MP+ text; local map at map detail LOW keeps forced layers; Henir icon tooltip;
     key-binding slot for an unnamed key shows string 39162; quest list greys from +8 levels (list and text form); quick-quest window with 2+
     ongoing quests (titles, sub-quest lines, Complete button; a story quest's title is red); quest window all tabs, a quest detail, Remind /
     Skip Epic; skill tree; dungeon result; character info; the glow in the character window (U); quest NPC faces (story and transcend quest NPC,
     Edel); quest items in the shared bank count toward a collect quest.
   Save setup: use `/x2-db`, `/x2-create-char` and the `/additem` lines in the hand-off; back up `els_db.sql` first (`tools/elsdb.py`).
3. **User plays; then check.** `python tools/logs.py` for `EXCEPTION`, `UNHANDLED` and `IGNORED` rows in `offline_packets.log`, `ErrorLog.txt` lines
   from this session, and the `died` log line. Anything that appears is classified: a divergence flag broke it / pre-existing / not this plan.
4. **Fix.** A failure traced to a plan flag (`git log -S<FLAG> --oneline`) is fixed under that flag, the March `#else` untouched, then one build.
   A pre-existing or out-of-plan failure gets a bullet in the owning backlog stage (scope rule), never a fix here.
5. **Record.** A "Phase S Done" note in the plan doc: each check pass / fail / not reached, the fixes with commits, the routed items. STATUS row.
   Commit with explicit pathspecs. D2 review reads this note.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md). No `June` prefix (user, 2026-10-28).
