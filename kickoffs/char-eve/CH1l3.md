# June character Eve (Stage 7) - Phase CH1l3 - Class core transcription: the skill-use overrides from 0x7f0 and their helpers (split from CH1l by CH0)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1l3` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l2 review. One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1l3-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1l3"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1l3"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1l3/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1l3` (SpecialAttackEventProcess and slots 510-512, NoStateChangeActionFrameMove, SpecialAttackNoStageChange, 0x828, ShowActiveSkillCutInAndLightByScript, JuneSummonedNPCInfo, and the helpers they reach first) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1l3` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 26** (`awk -F'\t' '$6=="CH1l3"' june2017_re/char_eve/ch0_class_map.tsv`; helper 17, override 9).
  June addresses: `32968db0` `32969300` `329693e0` `32972480` `32972a70` `32976f30` `3297a6d0` `3297a800` `3297baa0` `3297c570` `32981130` `3298d8d0` `329919a0` `32994fd0` `329978d0` `3299ba70` `3299ebd0` `3299f390` `329a0ae0` `329a7e70` `329b3cc0` `329b4b40` `329b5af0` `329b6810` `329cc370` `329cc390`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L272 (spec A readers); §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7ai L806-819 (BFI call arguments, 3 s skill-name string); §7an L890 (GetCharacterSpecializationB reader 329b407b).
- **7c alpha-tag sites inside these functions: 1** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed); `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `31c19f50` CKTDGParticleSystem::CParticleEventSequence::SetAxisAngle (KTDGParticleSystem.h:1385) (confirmed); `32290490` CX2GameUnit::UpDownCrashCameraNoReset (X2GameUnit.cpp:13104) (confirmed); `31e56810` KObserverPtr<T>::reset / ~KObserverPtr (KTDXLIB/KObserverPtr.h) (confirmed); `31edf560` user (candidate); `321e3100` CX2EffectSet::PlayEffectSetWithCustomPos_LUA( const char*, CX2GameUnit*, D3DXVECTOR3 vPos, D3DXVECTOR3 vRot ) (X2EffectSet.cpp:5804) (confirmed); `31cdd340` CKTDXDeviceManager::IsPlaying( const WCHAR* ) (KTDXDeviceManager.cpp:1095) (confirmed); `31fce8b0` user (candidate); `31f8afa0` CX2World::FadeWorldColor( D3DXCOLOR, float ) (X2World.h:975) (confirmed); `31f8afe0` CX2World::GetOriginColor (X2World.h:1066) (confirmed).
- `CanUseSpecialAttack` `329b5af0` (vslot 510) holds the dev free-skill gate site (CH1n wires it) and calls `31fce8b0` (mapped by CH1j20).

## Amended by CH1j20 (2026-10-26)

- `31fce8b0` is `CX2GUUser::JuneIsEquippedSkillKeyReleased( m_InputData, eSkillID )` (`X2GUUser.h`, flag `_CORE`; funcmap `Char Eve CH1j20`): June's call passes `unit +0x86da` and the skill id. Call it; do not re-transcribe it.
