# June character Eve (Stage 7) - Phase CH1k - Class core transcription: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1k` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1j2 review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1k-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1k"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1k"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1k/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1k` (lifecycle, render and reaction virtuals; derive the constructor (`3299bd70`) / destructor offset map for the members the other groups use) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1k` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 39** (`awk -F'\t' '$6=="CH1k"' june2017_re/char_eve/ch0_class_map.tsv`; helper 15, override 24).
  June addresses: `32965dd0` `32965e10` `32965f80` `32968ea0` `32968ee0` `32969190` `32973d30` `32973e70` `329770d0` `32977130` `32977300` `32978420` `3297a940` `3297b4b0` `3297bb70` `3297fdd0` `32990da0` `32991c00` `32994710` `32994750` `32997c60` `3299bd70` `3299cdc0` `329a2910` `329a4680` `329aece0` `329af460` `329c3da0` `329c47e0` `329cc3b0` `329cd440` `329cd860` `329cda50` `329ce0a0` `329ce210` `329ce310` `329cf170` `329cf210` `329cf670`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L272 (spec A readers); §7c L323-343 (167 alpha-tag sites 32982623..329c84f8).
- **7c alpha-tag sites inside these functions: 1** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed); `31e920b0` BUFF_FACTOR::~BUFF_FACTOR (X2BuffFactor.h:652, implicit) (candidate); `31f63d10` CX2DamageEffect::CEffect::SetLaserGroupID( int ) (X2DamageEffect.cpp:10757) (confirmed); `320017d0` user (candidate); `32290350` CX2GameUnit::UpDownCrashCamera (X2GameUnit.cpp:13091) (confirmed); `32071d20` CX2GUUser::GetXSkinAnimPtr (X2GUUser.h:2084) (candidate); `32d5da80` CX2UnitSlashTraceManager::AddSlashTraceData (X2UnitSlashTraceManager.cpp:88) (confirmed); `31be3020` CKTDXApp::GetProj3DPos( const D3DXVECTOR3& ) (KTDXApp.cpp:3374) (confirmed); `31f8b1c0` KObserverPtr<CX2GameUnit> operator bool (GetObservable() != NULL) (candidate); `31fd7500` user (candidate); `32000a90` user (candidate); `322e65c0` CX2Game::JuneUpdateEveRecoveryUI (June-only CX2Game method, name unsettled) (candidate); `31fc8070` user (candidate); `31fc8cd0` CX2GUUser::MemoCondition::MemoCondition (June-only struct) (confirmed); `31fca500` CX2GUUser::InitializeGUUserMinorParticleArray (X2GUUser.cpp:4739, unrolled) (confirmed); `31fd5d70` CX2GUUser::HyperModeBuffEffectEnd (X2GUUser.cpp:40887) (confirmed); `31febf50` user (candidate); `31fef620` CX2GUUser::SetCommonMajorParticleByEnum (X2GUUser.cpp:4722) (confirmed); `31fefba0` CX2GUUser::DeleteGUUserMajorParticle (X2GUUser.cpp:4878) (confirmed); `320142c0` CX2GUUser::AttackResultPlayEffect (X2GUUser.cpp:14983) (confirmed); `3228cfc0` CX2GameUnit::Weapon::InitEnchantParticleForBow (X2GameUnit.cpp:10430) (confirmed); `322dee90` CX2Game June EveRecovery tooltip (June-only CX2Game method, name unsettled) (candidate); `32d417e0` user (candidate).
- **Constructor `3299bd70`** (called by CX2Game `3230f330` @3230f389) and its helpers `32990da0`, `329cc3b0`, `329cda50`: derive the member offset map the other groups use. The Nasod Core members (`+0x9b98..+0x9c44`) belong to CH1q; `ch0_nasod_core.tsv` names them.
- **June-only CX2Game calls:** `322e65c0` / `322dee90` (the Eve Recovery guide / tooltip statics, checklist 7ah) are called from `329aece0` / `329c3da0`: a bare declaration does not link (CH1c review), so leave each call out, put `// June <va>: call added in CH4c` where it was, and do not port them here (CH4c amended).
- `32994710` (vslot 0x4c) is the 7ao hyper effect start: whole here, its transcendence branch checked in CH3d.

## Amended by CH1c (2026-10-26)

- **`329ac330` is CH1k's** (`ENSI_RIDING_HYPER_MODE` FrameMove; `June_ENSI_RIDING_HYPER_MODE_FrameMove`): it is `32997c60( 0.0f, 0.1f, true )` (`this`, then the two floats, then the bool), June's `CommonHyperModeFrameMove`. CH1c could not leave the call to an undefined body without a link error. Transcribe `329ac330` with `32997c60` (class map row moved to CH1k). `32997c60` is not a bare March copy: June adds the awakening effect-set calls (`32011d20`, `31fd66b0`, `32011f50`: the pilot's stand-ins `June_AwakeningEffectBegin` / `SetPresent` / `Play`, `JUNE_CHARACTER_AISHA_PHASES.md` CH1b review), `31febf50` (candidate, `ch0_unmapped_identified.tsv`), `32290350` (`ch0_unmapped_callees.tsv`) and the 0x6347 skill-level test; the Aisha copy `June_CommonHyperModeFrameMove` (`32ce12c0`) is the comparison. If a callee stays unmapped, say "this needs Opus".

## Amended by CH1e (2026-10-26)

- The ctor zeroes four dwords at +0x9c20 .. +0x9c2c (`3299bd70 @3299c900`). CH1e declared +0x9c20 (`m_pJuneDashXFireBall`) and +0x9c24 (`m_pJuneDashXTargetUnit`) and set them NULL in the ctor; +0x9c28 / +0x9c2c are not read by CH1e (their owner finds them).
