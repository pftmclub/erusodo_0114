# June character Rena (Stage 7) - Phase CH2d0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH2d0 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2d0 done (its Done note and commit; no play-test: no trap is castable before CH2d) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP`, `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP_DEBUG` (**stays defined**: CH2d's play-test reads its lines, CH2d's review retires it) |
| Log tag | `[JUNE_CHAR]` (`RENA TRAP spawn`, `RENA TRAP linked`, `RENA TRAP removed`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH2d0 - Done (2026-11-10)"
grep -n "Rena: CH2d0" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH2d0" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH3b0 - Review"
```
Plus `june2017_re/char_rena/ch0_traps.tsv` (rows corrected by CH2d0).

## Task

Check CH2d0's code against June (`python june2017_re/charport_adis.py rena 32c99690 32c9e250 32c95790 32c86340`; Ghidra read-only for `3230c1a0`, `3230b7b0`, `322ece70`). This is an engine / shared-code phase (`X2Game.cpp`, `X2GUUser.*`): it keeps its own review. Spot-check at least:
- **Evoke `32c99690`.** The memo choice: `31fca8e0( vslot 0x524, 1 )` then `+0x178 != 0` -> `0x401`, else `0x3fc` (= `IsEquippedSkillUpgraded( GetNowStateSkillID() )`); the level compare at `@32c9986e` (`fldz` / `fcomp` / `test ah, 5; jp`) = 1 only while the gauge's hyper time > 0; the push order at `@32c9991d` / `@32c999d3` gives `eTeam -1`, `eAllyTeam` = team.
- **Anger of Elf `32c9e250`.** Key code = `_ftol( 32007b10( 1, vslot 0x524 ) )` and `SA_EFFECTIVE_TIME` == 1; `+0x9700` -> `0x58d`; the June scripts' `GetKeyCode() <= GetTimerElapsedTime( 0 )` (`june_src_fixed/data037_kom/AngerOfElfTrapRena.src.lua:172`). The key is a `char` in `KNPCUnitReq`: check the effective-time values in June's skill table fit (`< 128`).
- **`3230c1a0` cases.** Jump-table targets (`0x823 -> 3230cbdf`, `0x3fc / 0x401 -> 3230c6c4`, `0x58c / 0x58d -> 3230c868`, `0x1c9 -> 3230c3ff`); flags 1 / 1 / 0 / 0; the rate (`31fceab0( 0x5b70 ) > 0` -> `32007b10( 0xf, 0x5b70 )`) lands in `322ece70`'s power-rate product (where `SetSummonHelperInfo` multiplies `fRate_`); `+0x97b8` is `m_iGungnirTargetNPCUID` (its readers `32ca1276` .. `32ca77fd`).
- **Access change.** `CX2GUUser::GetLearnedSkillAbilityValue` is public under `_TRAP` (protected `#ifndef`); no other caller changes meaning.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP --range <CH2d0 commit>^..<CH2d0 commit> --with SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP_DEBUG` (then `git log -SSERV_IRUHADEV_JUNE_CHAR_RENA_TRAP <commit>..HEAD`); paste the `files:` / `in this flag's files:` / `RESULT:` lines; explain every OTHER row and deleted line. `python tools/junediverge.py --check-all` = `RESULT: PASS`.

Fix clear transcription errors under the same flag and prove them with a one-file `/Zs` probe (no play-test here: "full build skipped: no play-test"); hand design decisions back to the user. Write "CH2d0 - Review" into the plan doc, update the STATUS row (CH2d -> ready), check `kickoffs/char-rena/CH2d.md` (its "Amended by CH2d0" section), and commit.
