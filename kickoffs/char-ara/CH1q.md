# June character Ara (Stage 7) - Phase CH1q - Force power and berserk

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1q` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1p review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_FP` (defined only with `_LIVE`) (+ `..._FP_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA FP`) where the phase logs; otherwise none |
| Review | `kickoffs/char-ara/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1q - Done" "CH1q - Review"
awk -F'\t' '$7=="CH1q"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Force power and berserk from `ch0_fp.tsv`: June's whole body inside every March function that differs, March's in `#else`. The known pieces:
`SetForcePower` `32ba6ef0` with the `+0x6d04` spend conversion (checklist l.257 / l.943), `UpForcePower`, FP cancel and the byte `+0x9621` (funcmap
l.1083), `IsFullHyperMode` / `SetFullHyperMode` (vslots 0x9c `32ba6cc0` / 0xa0 `32b96a00`; rows in `june_march_divergent.tsv` left to this port: replace
them), hyper effect start / end `32bb22d0` / `32bb2740` with the buff hyper count (`CX2GameUnit::GetHyperModeCountByBuff`, l.236-237, and its readers),
`IsHyperModeState` `32bd85f0` (l.603 / l.613: also picks the berserk sound set), `GetHypetMpdeStateID` `32ba46c0`, `ChangeMaxCharacterAddResource`
`32baafc0` (l.257), the gauge snapshot 0x740 `32ba4840` / 0x744 `32ba7b50` as `StoreGageSnapshot` / `RestoreGageSnapshot` (l.258; the Crest bases stay
unported, as Rena's), the 7 `INIT_COMPONENT` keys' consumers, `CX2AraGageData` / `CX2AraMyGageUI` against June's vtables, and the emulator's
`CAC_FORCE_POWER` answer (`X2OfflineStatTable.cpp:306`). Split as CH0 says (CH1q3 for the gauge UI if over ~45 functions). Sub-tag `FP`; the debug
lines stay until this phase's review. Full build only if this phase hands off a play-test (it normally does not: CH1 final tests it).

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 25** (`awk -F'\t' '$7=="CH1q"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 11, new 14).
  June addresses: `32b95c70`(new ?) `32b96a00` `32ba46c0` `32ba4840`(new StoreGageSnapshot) `32ba6cc0` `32ba6ef0` `32ba6f80` `32ba7b50`(new RestoreGageSnapshot) `32baad30` `32baae50`(new UpForcePowerByHyperModeBuffCount) `32baafc0`(new ChangeMaxCharacterAddResource) `32bab210` `32bab5c0`(new UpForcePowerByReadySkillGain) `32bab780`(new UpForcePowerBySkillHit) `32babba0`(new StateChangeOrFPCancel) `32bb22d0` `32bb2740` `32bb2b40` `32bc19a0` `32bc5bc0`(new GainFullHyperKillReward) `32bc8be0`(new ?) `32bc9a60`(new ProcessFullHyperHitReward) `32bd5e80`(new ?) `32bd6c80`(new ?) `32bd85f0`(new IsHyperModeState).
- **Checklist rows: 12** (`awk -F'\t' '$5 ~ /(^| )CH1q( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L184 class keys ADD_FORCE_POWER x9, USE_FORCE_POWER x26, FP_CANCE; L236 Ara hyper effect start / end 32bb22d0 / 32bb2740 (vslots 49 ; L237 buff hyper count readers / writers: 32ba6cc0 (vslot 39), 32b; L257 ChangeMaxCharacterAddResource Ara 32baafc0 (vslot 201 +0x324; L258 Crest of Time snapshot: vslot 0x740 per-class gauge override; L278 spec A / spec B readers: Ara 32bc9a60 (A), 32ba6ef0 (B); L293 tutorial flag in CX2GUAra::InitComponent 32bb0e2c: tutorial ; L603 user vslot 0x51c override 32bd85f0: now state 0x17 or 0x9b (; L613 sound sets: Ara / Luciel JuneIsHyperModeState also picks the; L943 "GetCharacterSpecializationB() readers: Ara 32ba6f31 (""the ; L959 hyper-effect start returns at once for a transcended unit; L978 transcend Ara: 32bb22d0 hyper effect start (vslot 49) branch.
- **Force-power rows: 56** (`awk -F'\t' '$7 ~ /(^| )CH1q( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: absent-in-june 2, differs 24, june-only 14, same 13, unchecked 3).
- **Specter / tail rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH1q( |$)/' june2017_re/char_ara/ch0_specter.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **CH-N (class path) rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH1q( |$)/' june2017_re/char_ara/ch0_classn.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **Identified callees first needed here: 3** (`awk -F'\t' '$8 ~ /(^| )CH1q( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `32498550` `3295bdd0` `32a37970`.
- `ch0_fp.tsv` (114 rows; relation: differs 56, unchecked 27, june-only 15, same 14, absent 2) is this phase's list: about 28 class functions + the gauge side (data 7, UI 5, manager 1, emulator) = about 44: **no split** (gauge rows are mostly `same`). The class map carries 23 CH1q functions (hand rows in `ch0_hand_phases.tsv`).
- Facts: force power lives in the gauge (`[this+0xc04]`: now +0x114, max +0x120, berserk byte +0x110, changed +0x111, FP-changed +0x112; getters `32a31a00` / `32a31a20`); `SetForcePower` `32ba6ef0` converts a spend into MP: (now - new) x 20.0 x spec B (`+0x6d04`, March's `GetCharacterSpecializationB`), my unit only; `IsFullHyperMode` 0x9c `32ba6cc0` (`+0x9744` > 0 ? buff hyper count `+0x6844` > 0 : gage berserk byte); `SetFullHyperMode` 0xa0 `32b96a00` (March's two gage writes + `+0x9700` = 0); `+0x9621` = FP-cancel probe (36 writers, readers `32b90c20` / `32bcfaa0`); June folds March's 22 inline "anim end: FP cancel or StateChange" blocks into `32babba0`; `UpForcePower` is inlined at about 7 sites with the `+0x9644` overflow-to-MP; `UseForcePower` `32bb2b40` merges March's `UseForcePower` + `ConsumeForcePower`; `CForcePowerSystem` is write-only in March and absent in June; berserk per-hit roll `32bc9a60` (FULL_HYPER_MODE_ADVANTAGE_*), regen `32bc5bc0` via the 0x318 hook `32bc8be0`; `ApplyHyperModeBuff` 0xc0 `32bab210` tail `32baae50` (GAIN_FORCEPOWER_USEDHYPERCOUNT / GAIN_MP_BY_FPCONVERSION_HYPERMODE); `CreateGageData` 0xd0 `32bc19a0`.
- Gauge: June `CX2AraGageData` vtable `3349b124` = March's six overrides (packet path sets max 10, `UpdateDataFromGameUnit` skips the changed byte); `CX2AraMyGageUI` vtable `3353351c` = March's. Gauge create for 34 / 124 is CHNa's. Emulator `CAC_FORCE_POWER` (3) matches June.
- Check: berserk portrait refresh `32e69060` compares gage +0x111 with +0x110, which `SetFullHyperMode` writes both to 1: confirm, and a `JUNE_BUGS.md` row if June never refreshes.
