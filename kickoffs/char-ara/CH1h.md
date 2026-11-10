# June character Ara (Stage 7) - Phase CH1h - Class core: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH1h` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), char-rena Z, the previous CH1 sub-phase. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own: the CH1k review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1a - Done" "CH1a - Review"   # the worked example; pull the matching Rena sub-phase too
awk -F'\t' '$7=="CH1h"' june2017_re/char_ara/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Transcribe every function the class map gives phase `CH1h` (lifecycle, render and reaction virtuals; DamageReact `32bafa30`; ctor `32bc6bf0` / dtor `32bc79f0` offset map (start `june2017_re/char_ara/ch1_offsets.tsv` from Rena's base rows); Init `32bc7e90` and the other uncarved ones below 0x600 from the listing; InitComponent `32bb0da0` with its alpha sites (7c), the 7 `INIT_COMPONENT` force-power / full-hyper keys' reads under `_FP`, the tutorial force power 3 (checklist l.293) and the CEffect `+0xb18` writer `32bb0bd6` (l.312)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GUAra` function under `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c`
  of `X2GUAra.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_ARA_CORE`, compiled in every build, referenced by nothing yet. Leave
  `InitState*` alone (CH1m registers).
- Force-power lines that call a June piece CH1q has not written go under `SERV_IRUHADEV_JUNE_CHAR_ARA_FP`, Specter / tail lines under
  `SERV_IRUHADEV_JUNE_CHAR_ARA_SPECTER` (an `#ifdef` inside the `_LIVE` body); March's live force-power calls stay March's until CH1q.
- A class-range callee (`0x32b90b70`-`0x32bd86e0`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is
  called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py ara <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GUAra.*` are CP949 (mostly CRLF): `bytepatch.py` only; check each
  `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with this phase is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
  with `_LIVE` undefined (one-file probe build, no full build: no play-test) and the one-file probe passes with it defined; `junediverge.py --check`
  and `enumscan.py --check` on the class files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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

- **Functions: 36** (`awk -F'\t' '$7=="CH1h"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 32, new 4).
  June addresses: `32b90b90` `32b90bc0` `32b90d20` `32b96640` `32b968f0` `32b96940` `32b96980` `32b969c0` `32b96a30` `32b96b10`(new ?) `32ba6830` `32ba6c20` `32ba6d00` `32ba6e50` `32ba8150` `32ba8180` `32bab160`(new MarkSkillFPGainHit) `32bab1d0`(new ?) `32bab890` `32babc60` `32babdf0` `32badd50` `32bae120` `32baec10` `32bafa30` `32bb0da0` `32bb2280` `32bc6bf0` `32bc79f0` `32bc7e90` `32bc8030` `32bceca0` `32bd5e60`(new IsBuffHyperModeActive) `32bd85d0` `32bd85e0` `32bd8680`.
- **Checklist rows: 5** (`awk -F'\t' '$5 ~ /(^| )CH1h( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L184 INIT_COMPONENT keys FORCE_POWER_COUNT_AT_DUNGEON_START, _AT_; L237 buff hyper count readers / writers: 32ba6cc0 (vslot 39), 32b; L293 tutorial flag in CX2GUAra::InitComponent 32bb0e2c: tutorial ; L664 CUSTOM_RENDER_STATE class users: Ara_Martial_Artist among th; L852-L855 "pass June's skip / adjust / level / creator at every ported.
- **Force-power rows: 11** (`awk -F'\t' '$7 ~ /(^| )CH1h( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 4, june-only 7).
- **Specter / tail rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH1h( |$)/' june2017_re/char_ara/ch0_specter.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **Identified callees first needed here: 16** (`awk -F'\t' '$8 ~ /(^| )CH1h( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `31c20770` `31f8b7e0` `31f8b860` `321f3fe0` `32281440` `322a9e80` `32697df0` `326df6b0` `326df830` `32a31750` `32a31770` `32a31790` `32a31a00` `32a31a20` `32a32640` `32a32720`.
- 35 functions. Overrides named by CH0: 0xd0 `32bc19a0` = `CreateGageData` moved to CH1q; 0x29c `32bd32f0` = `GetReduceDebuffDurationRate` moved to CH4a; 0x300 `32ba47f0` (CEffect hook, skill 27411 power rate) CH4b; 0x328 `32ba6440` (float hook, passives 27417 / 27420) CH4a; 0x318 `32bc8be0` (berserk regen hook -> `32bc5bc0`) CH1q; 0x2fc `32bab1d0` (my-unit-only call `32bab160( arg+0x5c4, arg+0x5e8 )`) stays here, name unsettled: name it from the body.
- InitComponent `32bb0da0` reads the 7 June-only `INIT_COMPONENT` keys into: `+0x9610` / `+0x9611` (FP count at dungeon / PvP start, bytes), `+0x96fc` (default 9), `+0x9704`, `+0x9808` (vector<float>), `+0x9720` (vector<int>), `+0x9730` (`ch0_fp.tsv`): the reads are written here under `_FP`; their readers are CH1q's. Max FP is the constant 10 in Init `32bc7e90` / ReInit `32bc8030` (March: `min(level/10, 6) + 4`): under `_FP`.
- `AppendMajorParticleToDeviceList` `32b96640` (March has it) is here. `AppendDeviceToDeviceList` `32bd86b0` / `AppendComponentToDeviceList` `32bd86e0` are empty in June and March: nothing to port. `32bd86c0` is unreferenced (dead).
- June-only record vector at Ara `+0x9858` (`{USHORT, int, vector<12-byte T>}`; STL helpers `321f3fe0` / `32ed3950` / `32a79680`): declare the member and record type here.

## Amended by the CH1g review (2026-11-10)

- CH1g added three `_CORE` members to `X2GUAra.h` with ctor values in the initializer list: `m_hComboXXdownXZBlackHole` (`+0x9734`, `INVALID_EFFECTSET_HANDLE`), `m_iComboXXdownXZLoopCount` (`+0x9738`, 0), `m_fBuffHyperModeDamageRate` (`+0x9744`, 0). The ctor `32bc6bf0` transcription places them in the offset map and confirms June's ctor values; `+0x9744` is the buff hyper rate CH1q writes (L237).
- Effect-set handle lists are `CX2GameUnit` members: June `+0x3eb0` = `m_vecEffectSetToDeleteOnDamageReact`, `+0x3ec0` = `m_vecEffectSetToDeleteOnStateEnd` (now named in `ch1_offsets.tsv`, so `charport_adis.py` labels them). The CH1g review fixed nine pushes that had the wrong list; read the `lea ecx, [esi + 0x3e?0]` before each push.
