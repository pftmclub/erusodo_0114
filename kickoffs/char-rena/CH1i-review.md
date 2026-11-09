# June character Rena (Stage 7) - Phase CH1i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1i review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1i done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; `SetEquippedSkillLevel`, `SetSiegeModeEffectiveValue`, virtual `ShownSkillCoolTime`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; `StoreGageSnapshot`, `UpdateSpineAimRotation`, 10 enum members, 1 member), `..._NF` (undefined; 1 line), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (13 `RNSI_` ids) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1i - Done"
awk -F'\t' '$7=="CH1i"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1i - Review"
```

## Task

Review the 8 CH1i rows against June (`python june2017_re/charport_adis.py rena --phase CH1i --out <dir>`; `32c85970` runs into its own jump table, `32ca5310` is 6 instructions). Spot-check at least:
- **`SetEquippedSkillLevel` `32c886b0`:** every block's skill id and state ids against the listing: the tests are `cmp bl, 1` / `cmp ebp, id`, in the order 0x5aa7, 0x59e1, 0x5aad, 0x59e0, 0x5aa2, 0x5aac, 0x5ab0, the Siege loop ( 0x5aa1, 0x5ab5 ), three Hyper loops ( 0x5c30 / 0x5c33, 0x5c31 / 0x5c34, 0x5c32 / 0x5c35 ), then 0x59e3, 0x5aa4, 0x59e7, 0x5a13, 0x5a14. State ids ( `push 0xNN` ): 199; 201 / 202; 203 / 204; 227; 240 / 241; 167-171 + `32c881f0( level )`; 246 / 247; 248 / 249; 251; 216; 295; 283-294; 296-301; 302. **Check the two inferred ids: 167 = `LESI_SIEGE_READY` and 283 = `LESI_RGA_GUNGNIR_READY`** ( the only single gaps in the registered id run, next to the READY registrations; the Done note has the evidence ). Entangle: ability 0x1e = `SA_PROJECTILE_FIRE_COUNT` -> `m_MaxEntangleTrapCount` ( +0x95d4 ), ability 1 -> `m_fEntangleTrapAttackDuration` ( +0x95d0 ), memo 603012 = `SMI_RENA_MEMO12` ( `GetEqippedSkillMemo` is already June's `false` ); Anger of Elf: ability 0x1e -> +0x95f4.
- **`SetSiegeModeEffectiveValue` `32c881f0`:** `GetSkillLevel( 0x5ab5, 1, 0, 1 ) > 0` picks 23221 else 23201 ( the `GetSkillLevel( 0x5aa1 )` in the else arm is discarded ); abilities 0xf = `SA_DAMAGE_REL` x +0x9610 -> +0x95b8, 0x1b = `SA_MP_CONSUME_REL` -> +0x95ac ( March's `m_fDamageRelSiegeArrow` / `m_fMPConsumeRateSiegeArrow` by role: confirm they are those fields ).
- **`UpdateSpineAimRotation` `32c863d0`:** the all-zero test writes `GetRotateDegree()` into +0x97d4 only; the `eps < |y|` / `eps < |z|` tests, the yaw wrap and the bone call use the local aim; `+0x8a68` = local z; `m_afSiegeAngle[3]` is passed as is ( Chung multiplies by a distance ).
- **`ShownSkillCoolTime` `32c7bed0`** ( `fcompp` + `test ah, 0x41` + `jp` = returns 0 when `limit <= count` ) and **`StoreGageSnapshot` `32c852f0`** ( base call, then the gauge's now count at +0x114 into +0x92b4 ).
- **"= March" rows:** `InitEquippedActiveSkillState` ( jmp thunk ), `CanRidingState` ( jump table over `( state - 13 )`: the ten zero entries ), `HyperModeFrameMove` ( `CommonHyperModeFrameMove( 0.2, 0.75 )` ).
- **Names:** the 13 `RNSI_` ids against `Enum.src.lua` ( 23008 / 23009 / 23011 / 23059 / 23202 / 23204 / 23212 / 23600-23605 ), the 10 `LESI_` members ( 202, 283, 295-302 ) and that no earlier phase already owns a name for them.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1i commit>^..<CH1i commit>` ( FAIL only for the commented-out `#define` ); `python tools/flagcheck.py SERV_IRUHADEV_JUNE_ENUM_NAMES --range <CH1i commit>^..<CH1i commit>`; `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe ( `census.cl_cmd`, `/c /Zs /EHsc` ) of `X2GULire_ElvenRanger.cpp` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1i - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1j.md`, commit with explicit pathspecs.
