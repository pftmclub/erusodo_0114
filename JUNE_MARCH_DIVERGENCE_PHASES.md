# March behaviour inside June code: replace divergent March functions at the definition

Backlog Stage 35 (user, 2026-10-28). Plan key `june-march-divergence`, branch `main`, no log tag (D1 may add one), flags
`SERV_IRUHADEV_JUNE_DIVERGE_<NAME>` (one per replaced function), kickoffs `kickoffs/june-march-divergence/`, table
`tools/data/june_march_divergent.tsv` (D0 writes it). **Runs after the Eve port is complete (user, 2026-10-28)**: D0 waits for char-eve Z.

## Context

The user (2026-10-28): "i just dont want the march related issue keep happening on june's logic or work".

**The failure.** A June body is transcribed calling March functions by name. Where the June namesake is a stub or behaves differently,
the port keeps March's behaviour. It compiles, it reads right, and the play-test passes; no error ever points at it.
- char-eve CH4a review (`b86fc6d6`): `June_InitPassiveSkillState` called March's `GetEqippedSkillMemo( SMI_EVE_MEMO12 )`. June's `31fca9e0`
  always answers false; March's answers true when the memo is equipped, or under `_IN_HOUSE_` with frame scale on.
- Aisha CH2a review (`f4f26c25`): June's PvP-or-PvE flag `322cc1a0`. Aisha CH3c review (`012ca1f0`): June's `322ece70` power-rate flags.
- The same kind, other reviews: `04448382` (GT_PVP-only tests -> `GetIsNowPVPCorrectionGame`), `3785fd55` (March's case 682 on skill 5008),
  `01e83fcf` (summon setup = June's `322ece70` flags), `214dec90` (March's state 0x82 is a class state), `51837124`, `c3010acc`, `9c4e7307`.

**The approach (user, 2026-10-28): replace the function, not every call.** If March's `GetEqippedSkillMemo` *is* June's stub, every June
body that calls it by name is right with no extra rule. This is how the studio shipped its own updates: the function changed in place
under a per-update flag and callers kept the name (March's `GetEqippedSkillMemo`, `X2GUUser.cpp:6765-6781`, sits whole inside
`#ifdef SERV_SKILL_NOTE`; June's `31fca9e0` is that function after a later update turned it off). Our `SERV_IRUHADEV_` rule is the same
pattern. The `June_*` side-by-side copies are where the slip comes from: a copy calls the old namesake.

**Today's tree (planning, 2026-10-28):**
- One live June-side memo call: `X2GUArme_VioletMage.cpp:27702`, `June_AVSI_ABM_DASH_COMBO_Z2a_FrameMove` (June `32cdc0f0`), under
  `JUNE_CHAR_ARME_CORE`: `GetEqippedSkillMemo( SMI_AISHA_MEMO5 )` adds super armor that June never gives. Every other call (438 grep hits,
  15 files) is March code or a `#else`. Earlier ports wrote the stub as `const bool bJuneMemo = false` (VioletMage 29873; Eve 45995,
  48258, 53294, 56616) or dropped the branch (Eve 54084, 54418).
- GT_PVP-only tests where June calls `322cc1a0`: VioletMage 34837 / 35032 / 35720, `X2GUUser.cpp:29462`. Their "March has no GT_PVE"
  comments are stale: `X2Game.h:72` registers `GT_PVE = 4`, and `X2Game.h:884` has `GetIsNowPVPCorrectionGame()` (June `322cc1a0`).
  A file-local `June_IsPvpOrPveGame` sits at VioletMage 37061.

**Seed candidates** (`june2017_re/funcmap.tsv` lines; D0 rules each):

| Line | June | March name | June does |
|---|---|---|---|
| 1182 | `31fca9e0` | `GetEqippedSkillMemo` | returns 0 unconditionally (also 1195, 1196, 4346, 4407, ...) |
| 3545 / 3264 | `31fd4450` | MemoCondition decide | `xor al,al; ret 4`: `m_bApplyMemo = false` |
| 4491 | `328df840` | memorize-sign test (vslot 0x714) | always false for Aisha |
| 985 | `327089e0` | `GetAttribEnchantRequireED` | always true; March fails count > 3 / > 2 |
| 946 | `32001570` | `CanApplySocketDataByGameType` | GT_PVE rejects all |
| 1129 | `32630d20` | `ParsingBuffFactorID` | `_ULTIMATE_PVP` in PvP / PvE, no found test |
| 1166 | `323644a0` | `GetIncreaseSkillLevelByBuff` | 0 under GT_PVE |
| 945 | `3204f190` | `SumSocketOption` | GT_PVE admits only 35318 / 35319 |
| 2281 | `322d0350` (+3) | StopOther / StopAll / Stop2* | GT_PVE team skip |
| 2434 / 2490 | `327ef160` | `ShowBossNameAtStart` | game type 2 or 4, FORCE_CINEMATIC_STATE gate |
| 1161 | `326c9030` | `Handler_EGS_UPDATE_BUFF_INFO_IN_ROOM_NOT` | BFI 154 skip in type 3 / 4 |
| 504 | `32d4ce60` | BuffFactor SetGameUnit BBT 80-84 | x0.5 under GT_PVE |
| 2489 | `320462a0` | DamageReact (Rune Slayer reflect) | magic damage 0, no reflect roll |
| 1495 | `3204d440` | EraseDebuffAndAddMP | MP tail unconditional, no erase count |
| 1699 | `3208ddc0` | `CheckAndUpdateAdditionalStat` | `ret 4` in every CX2GU* vtable (no-op) |
| 2221 | `31d411c0` | `CKTDXDeviceSound::Update3DPosition` | error-log rewrite |
| 3006 | `31d43ef0` | `GetStrByLineBreakColor` | EU word-wrap branch, unconditional |
| 2386 / 2393 / 3030 | `327ac9d0` / `327cea70` / `327f6e40` | NPC CommonStateStart / UpdateUnitPointer / OnFrameRender_Prepare | "differs from March" (routed) |
| 328 / 1473 / 1474 | `32494560` / `3201aff0` / `32000320` | buff-factor / reduced-damage helpers | GT_PVE terms (Stage 33's map) |
| 2069 / 2923 / 160 | `3231f510`, `31e4c550` | base-class virtual stubs | constant answers |
| idiom | `322cc1a0` | `GT_PVP == GetGameType()` | PvP **or** PvE (types 1 / 4) |
| idiom | `322ece70` | March's stat / power split | flag word picks the split |

## Rules

- Branch `main`; commit with explicit pathspecs. One flag per replaced function in `KTDXLIB/Always.h`, March's body in `#else`, one-line
  comments citing the June address. X2Lib / KTDXLIB sources are CP949: `bytepatch.py` only.
- **June removed it, so it is removed here (user, 2026-10-28).** Replacing a function changes its unported March callers too (about 350
  sites in Elsword, Rena, Raven, Chung, Ara, Elesis, Add). That is the point of the port, not a question: a March feature that goes away
  because June removed it goes away, unless the user says to keep it. A row is `site` instead of `replace` only when replacing would
  crash, leave a script unloadable, or need a June system that is not ported yet (name it).
- **Replace = June's whole body** (no partial June updates). June quirks verbatim with a `JUNE_BUGS.md` row.
- **Stage 33 owns stat / damage formulas.** A candidate inside its map (`3208ddc0`, `3201aff0`, `32000320`, `32494560`, ...) gets verdict
  `stage33` and stays there; this plan does not touch it.
- No hardcoded enum literals (`enumscan.py --check`); named `_CONST_<CLASS>_JUNE_<phase>_` namespaces, never `namespace { }`.
- **Table maintenance (from D2 on, every plan):** a review that finds a new divergent March function adds a row to
  `tools/data/june_march_divergent.tsv` in the same commit. The default fix is `replace` at the definition; a `site` row is the exception.
- Drift (a June branch found here that is not a divergent namesake) goes by the backlog scope rule, never into this plan.

## Starting context

Tools only: `python tools/build.py --deploy` (background), `flagcheck.py`, `bytepatch.py`, `docsection.py`, `enumscan.py`, `logs.py`,
`elsdb.py`. June: Ghidra (`.claude/skills/ghidra`), `june2017_re/funcmap.tsv`, `june2017_re/funcdis.py` for bodies with no Ghidra
function. March: resolve which branch is live with `cl /P` (`SERV_SKILL_NOTE`, `ADDITIONAL_MEMO`, ...), never by reading `#ifdef`s.
Region walker to copy for D2: `enumscan.scan_file` (`tools/enumscan.py:437-666`: `#ifdef` / `#ifndef` / `#if defined` frames,
`DEFINED_JUNE` from `Always.h`, function-name tracking at :558-574). Allow-list shape: `luabind.py`'s `known()` (`tools/luabind.py:41-48`).

## Phase D0 - The table (Opus 5.5, high)

**Run with:** `/x2-phase june-march-divergence D0`. **Depends on:** char-eve Z. Read-only plus the tsv; no code, no build.
Delegate greps, caller counts and `cl /P` probes to Sonnet subagents; keep every June-vs-March reading.
1. Write `tools/data/june_march_divergent.tsv` (header `# name	kind	june_va	verdict	june_does	march_does	write_instead	march_callers	breaks_if_replaced	source`).
   `kind` = `func` / `idiom`; `verdict` = `replace` / `site` / `out` / `stage33`.
2. Rule every Context seed plus the functions of the ten fix commits against the June listing. A March name whose June body is the same
   is `out`. Re-grep the funcmap for `stub|always|unconditionally|differs from March` rows added since 2026-10-28.
3. Per `func` row: March callers (ported June code vs unported March code), the live March branch (`cl /P`), and `replace` unless
   replacing crashes, breaks a script or needs an unported June system.
4. Order the `replace` rows for D1 (`GetEqippedSkillMemo` first) and write them into `D1.md`; list the `idiom` / `site` rows for D2.
**Gate:** every seed has a verdict and a source; the D1 / D2 kickoffs carry the final lists; Done note <= 60 lines.

## Phase D1 - Replace at the definition (Opus 5.5, high)

**Depends on:** D0. **Flags:** `SERV_IRUHADEV_JUNE_DIVERGE_<NAME>`, one per row.
- Open by re-reading 2-3 of D0's rulings in Ghidra (D0 has no separate review).
- Per `replace` row: March's definition becomes June's whole body under its flag, March's in `#else`, `MODS.md` flag + revert line.
  First: `GetEqippedSkillMemo` (`X2GUUser.cpp:6766`) -> `return false;` (June `31fca9e0`). That alone fixes `X2GUArme_VioletMage.cpp:27702`;
  the call site is not edited. The existing `bJuneMemo` locals stay.
- `JUNE_BUGS.md` rows where June's answer is a bug (memo effects never applied: extend 445 / 447 or add rows).
- Hand-off: one check per visible change, on an unported class (an equipped memo on Elsword no longer changes its skill) and on a ported
  one (Aisha's Z2a dash combo with memo 5: no super armor).
**Gate:** every `replace` row landed or moved to `site` with the reason; flagcheck PASS per flag; DEPLOY OK; play-test lines.

## Phase D1 - Review (Opus 5.5, high)

`/x2-phase june-march-divergence D1 review`: each replacement against its June listing, the `#else` bodies intact, `flagcheck --range` per
flag, an unported-caller spot check (one March call site per row, does it still compile into the right branch), the play-test logs.

## Phase D2 - The idiom gate and the sweep (Sonnet 5.5, high)

**Depends on:** D1 review. **Flags:** fixes go under the flag that owns the code being fixed (`SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`, ...).
1. `tools/junediverge.py` checks only `idiom` and `site` rows:
   - `--check <files>`: a hit is a table pattern inside June code = a `June*` / `June_*` body, or the ON branch of a defined
     `SERV_IRUHADEV_JUNE_*` (the `#else` of an `#ifndef`); never March's `#else`, comments or strings.
   - allow-list `tools/data/june_diverge_allow.tsv` (`# file	function	name	reason`, keyed by function, not line);
   - output `<file>:<line>  <function>  <name>  -> <write_instead>`, then `RESULT: PASS` / `RESULT: FAIL, N hit(s)`; exit 1 on FAIL;
   - `--list` prints each row's name and what to write instead;
   - `--selftest` on pinned revisions: `git show d4e8edc6:X2Lib/X2GUArme_VioletMage.cpp` hits the GT_PVP idiom at the 34837 site;
     `X2GuEve.cpp` 3936 / 3952 (`JUNE_CHAR_EVE_PASSIVE` `#else`) do not hit; an `#ifndef` fixture hits in its `#else`.
2. Sweep `X2Lib/*.cpp X2Lib/Offline/*.cpp KTDXLIB/*.cpp`: each hit is fixed under its owning flag (GT_PVP-only -> `GetIsNowPVPCorrectionGame()`
   where June is `322cc1a0`) or allow-listed with the June address that shows June tests type 1 only. Opus rules any unclear hit.
3. Wiring: x2-phase "Start" step 5 (`junediverge.py --list` before transcribing a June body), x2-phase "Finish" step 1 (`--check <files>`
   must PASS, beside luabind / skillicons, with the maintenance rule), the Stage 7 template (`JUNE_CHARACTER_AISHA_PHASES.md`, "Template for
   Stage 7"), the open character plans' Rules (char-eve if still open), `tools/README.md`.
**Gate:** `--selftest` PASS; `--check` over the sweep set PASS; DEPLOY OK if code changed.

## Phase D2 - Review and close (Opus 5.5, high)

`/x2-phase june-march-divergence D2 review`: selftest, every sweep verdict against its June address, the wiring lines, `flagcheck --range`
per touched flag. Close: `MODS.md` section (flags, revert), backlog Stage 35 closing note, STATUS. **The plan closes here.**

**Standing rule (user, 2026-10-28: "after this plan complete, make it as a rule so this won't happen again").** The close writes it as a
`CLAUDE.md` rule section (`## Rule: a divergent March function is replaced at its definition (user, 2026-10-28)`), a line in
`JUNE_PORT_BACKLOG_ORDER.md` "Standing rules", a line in `kickoffs/TEMPLATE.md`, and a memory file + `MEMORY.md` pointer. Wording, one
point per line:
- A June body calls March functions by name only if June's namesake does the same thing; `python tools/junediverge.py --list` before
  transcribing, `--check <files>` must PASS before committing.
- A March function whose June namesake is a stub or differs is replaced at its definition (June's whole body under
  `SERV_IRUHADEV_JUNE_DIVERGE_<NAME>`, March's in `#else`), never patched call by call. Unported callers follow June.
- A review that finds a new one adds the `june_march_divergent.tsv` row and the replacement in the same commit.
