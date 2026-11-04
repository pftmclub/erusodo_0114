# June character Elsword (Stage 7) - Phase CH1q2 - Way of the Sword: gauge data, gauge UI, gauge manager, skill-slot effect

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1q2` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1q review; one at a time with the other `X2GUElsword_SwordMan.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (+ `_WOS_DEBUG`) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-elsword/CH1q2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1q - Done (2026-11-02)"
```
Then CH0's Done note, the earlier CH1 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_elsword/ch0_*.tsv`.
Chung's matching sub-phase is the worked example.

## Task

Port June's Way of the Sword from `ch0_wos.tsv`: June's bodies inside March's WoS functions (class `:409-529` / `:7182-7480`,
`CX2ElswordGageData`, `CX2ElswordMyGageUI`, `X2GageManager`, `X2DamageManager::CalcWayOfSwordPoint` / `CalcVigorMpUpOnAttack` and the const
table, the `X2DamageEffect.cpp:5505-5519` caller, the `m_CannonBallCount` sync), June-only pieces under plain names. Shared pieces are ported
whole (no partial updates); Elesis's class-side code stays char-elesis's, and where June's shared body would break her unported March class,
guard it inside the June body with a one-line `stand-in: <class> not ported` comment. `WayOfSword.lua` / `DLG_Elsword_WayOfSword.lua`: `luabind.py
check`, adopted with CH1 final (`--dry-run` here). The vslot 0x740 snapshot override `32d058a0` if CH0 put it here. Flag `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS`, sub-tag
`WOS`; a temporary `_WOS_DEBUG` for CH1 final's play-test lines.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Nothing runs until `_LIVE`: no play-test; the hand-off names the next phase.

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

## Amended by CH0 (2026-11-03)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ah L827 Static_EL_SKILL_B(_B) / _R(_B) Elsword way of sword; CX2Game::UpdateEl.
- Scope (CH0 split of CH1q): `CX2ElswordGageData` (vtable `3349b0c4`, size 0x11c, +0x110 changed / +0x114 state / +0x118 point; Update `32a34890`, InitWhenGameIsOver `32a348d0` (June no longer resets the state), ToPacket `32a323e0`, FromPacket `32a348e0`, Clone `32a35e60`, Copy `32a34930` (no longer forces the changed flag), SetState `32a36310`, copy ctor `32a37930`); `CX2ElswordMyGageUI` (vtable `3349aba4`: InitUI `32e6d830`, OnFrameMove `32e71850`, `UpdateUIElswordWSP` `32e67df0`, `UIFrameMoveElswordWSP` `32e60f00`, Stop `32e675c0`, PlayDest / PlayVigor `32e67770` / `32e67ab0`, SetShow `32e5e180`, SetShowWspParticle `32e5e300`); the gauge-manager creation switch `3248e290`; `CX2Game::UpdateElswordSkillSlotEffect` `322cd5c0` (checklist L827); the class-switch gauge randomiser `322ed400` (June-only). `DLG_Elsword_WayOfSword.lua` is unchanged from March (`luabind.py check` PASS); adopt with CH1 final. Elesis's own copies (vtables `334fa61c`, `3349b184`, `3349ac34`) stay char-elesis's.

## Amended by CH1h (2026-11-04)

- CH1h's InitEffect refreshes the gauge: `CX2GageManager::GetInstance()->GetMyGageUI()`, then June vslot 0x5c `32e681d0` (sets the gauge data `+0x110` changed flag, tail-calls `UpdateUIElswordWSP` `32e67df0`). CH1h wrote it as `SetChangedWayOfSwordState( true )` plus `static_cast<CX2ElswordMyGageUI*>( ... )->UpdateUIElswordWSP()`; make the gauge UI virtual (or keep this) when the UI is ported.

## Amended by CH1q (2026-11-04)

- The gauge UI and the skill-slot effect are CH1q2's alone; CH1q changed no gauge class. The skill-slot MP requirement with Vigor x0.7
  (`X2Game.cpp:14855-14881`, `ch0_wos.tsv` row 92, not located in June by CH0) goes with `UpdateElswordSkillSlotEffect` here: read whether June keeps it.
- Flag: CH1q added `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_WOS` (`//#define`, `_LIVE` `#error`s without it) and `_WOS_DEBUG` (defined); put the gauge bodies
  under `_WOS` too, and their play-test lines inside `_WOS` code.
