# June buff port - Phase 8 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 8 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 8 done (its Done note and commits `cb83083` and the second code commit it names) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER` (+ `JUNE_BUFF_DEBUG`) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 4" "Phase 8 -- Done"
```

The "Buff Phase 8" rows of `june2017_re/funcmap.tsv` hold the RE. `JUNE_BUGS.md` #15 and #16 are
new. `MODS.md` has the peer-build note.

## Task

Review Phase 8's two code commits against June. Check these first:

1. **The class bodies** (`X2BuffFinalizerTemplet.cpp`, the Phase 8 block): BFT 15/16 SetFactor,
   DidFinish, OnFrameMove, GetFactor (the NULL-unit early return), SetFactorFromPacket (no slot
   256); the aura's SetFactor / CheckAura (slot 9) / DidFinish (no reserve test) / GetFactor.
2. **The factor parse and level slice** (`X2BuffFactor.cpp`): 15/16 reuse
   `ParsingHitCountFinalizer` with `bIsSkill = true` (June's literal 1) and `PushFinalizerHitToLevel`
   retagged. Confirm against `32d44a80`, `32d44d60`, `32d48eb0`, `32d49060`.
3. **The input mapping.** June's actions 0x25/0x26 were mapped to `GA_LEFT`/`GA_RIGHT` from the
   order of June's `InputData` calls in `322f7170` (0x25, 0x26, 0x23 vs March's LEFT, RIGHT, UP).
   Confirm with one more June reader of those actions, and that `31c488c0` is the held
   (`GetPureActionState`) state.
4. **BFT 16's host.** June counts at the top of `CommonStateStart` for the reserved skill; the
   port reserves in `CountUsedSkill` (March's skill-use point, before MP) and counts in
   `CountReservedSkillUse`. Check: every class's skill use reaches `CountUsedSkill` (including
   Ara's two variants and Add/EL's `StateChangeSpecialAttack`); the 1 s expiry; nothing else in
   June's reserved branch has a March home.
5. **The sync.** Field flag width, bit `0x800000`, Serialize/Deserialize order (between this
   build's peers only), `eUnitUserSync_DataField_All` now includes the new bit (the first full
   sync needs it), XPT 27's value (27 = after `XPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL`), the
   observer's timer (`@32063857-938`) and that it runs only for units that are not mine.
6. **The four `+0x6dd0` bump sites** and the double end hook (#15).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_BUFF_NEW_FINALIZER --range cb83083^..HEAD --with JUNE_BUFF_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase 8 -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

## Phase 8's play-test lines (dev console, END key; once per launch, in this order)

Templets (BTI 9001-9004), then factors (ids 362-365: the first after March's 361 factor slots):

```
local m=g_pBuffTempletManager local function T(id,f) m:AddBuffTemplet_LUA({BUFF_TEMPLET_ID=id,IS_WORLD_BUFF=0,IS_DEBUF=0,ACCUMULATION_LIMIT=1,REGIST_TYPE=0,BUFF_ICON_TEXTURE_NAME="DLG_UI_Common_Texture_Buff.tga",BUFF_ICON_TEXTURE_KEY="Buff_Icon_Dodge",BEHAVIOR={COMBINATION={55}},FINALIZER=f}) end T(9001,{COMBINATION={1,15},BFT_TIME={BUFF_DURATION_TIME_TYPE=1},BFT_STICK_SHAKE={BUFF_USE_COUNT_TYPE=0}}) T(9002,{COMBINATION={1,16},BFT_TIME={BUFF_DURATION_TIME_TYPE=1},BFT_USE_SKILL_BY_SKILL_SLOTTYPE={BUFF_USE_COUNT_TYPE=0,SKILL_SLOT_TYPE=1}}) T(9003,{COMBINATION={1},BFT_TIME={BUFF_DURATION_TIME_TYPE=1,SHAKE_DECREASE_TIME_AT_ONCE=0.1,SHAKE_DECREASE_MAX_TIME=2}}) T(9004,{COMBINATION={14},BFT_AURA={}})
```
```
local m=g_pBuffTempletManager local function F(n,id,f) m:AddBuffFactor_LUA({BUFF_FACTOR_ID=n,BUFF_FACTOR={BUFF_TEMPLET_ID=id,RATE={1},IGNORE_REGIST_FOR_RATE=1,BEHAVIOR={COMBINATION={55}},FINALIZER=f}}) end F(362,9001,{COMBINATION={1,15},BFT_TIME={BUFF_DURATION_TIME_TYPE=1,NORMAL_TIME={30}},BFT_STICK_SHAKE={BUFF_USE_COUNT_TYPE=0,COUNT={4}}}) F(363,9002,{COMBINATION={1,16},BFT_TIME={BUFF_DURATION_TIME_TYPE=1,NORMAL_TIME={60}},BFT_USE_SKILL_BY_SKILL_SLOTTYPE={BUFF_USE_COUNT_TYPE=0,COUNT={2}}}) F(364,9003,{COMBINATION={1},BFT_TIME={BUFF_DURATION_TIME_TYPE=1,NORMAL_TIME={20}}}) F(365,9004,{COMBINATION={14},BFT_AURA={}})
```

Apply one: `g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 )` (363, 364, 365 likewise).
