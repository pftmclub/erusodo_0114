# June character Elsword (Stage 7) - Phase CH1k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1k review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1k done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1k - Done (2026-11-04)"
awk -F'\t' '$7=="CH1k"' june2017_re/char_elsword/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1l3 - Done (2026-11-01)"   # Chung's lookup / use / cooldown, the worked example
```

## Task

Review the 15 CH1k functions against June (`python june2017_re/charport_adis.py elsword --phase CH1k`). The risk sits in these:
- **Lookup `32d1bd10`.** The check order (quick-slot wait, `CommonSpecialAttackEventProcess`, templet id equal to the slot's id else string 0x8bae, then `CheckSkillUseCondition`, `CheckSkillUsePVPOfficial`, `3202d140`, `3202d3c0`, riding-key wait, auto guard gate with skills 21200 / 21217). The MP chain: type 6 (`ST_ACTIVE`) subtracts `level x rate x MP` (`fild` of `+0x96a8`, `fmul` `+0x969c`, `fmul st(1)`, `fsubp`), Vigor (`GetWayOfSwordState()` = 2) x 0.7 when **templet `+0x160` = 2**: the port reads it as `m_iWOSType` (`WST_VIGOR_ACTIVE`); confirm `+0x160` is that member in June's `SkillTemplet` loader (a wrong member would apply the rule to the wrong skills). The dev gate around 226 / 2549 / 26119; the hyper stone test exempts only the training game (`g_pMain` `+0x8c` != 0x12).
- **Cooldown `32d0d8e0`.** `GetActualCoolTime` (vslot 0x6a4) gets `push 0; push 1; push edi` = (templet, **1**, 0), not the clamped level (Chung's `32bf5130` pushes `eax`): confirm JUNE_BUGS 524 and that the two `GetSkillLevel` calls are dead. Order of the tail: `SetSkillCoolTimeLeft`, `31fcd3d0`, `3235a710`. The finality stone uses `31fd0570` (training or photo zone).
- **0x7f4 `32cf3290`.** The use-condition values 3 / 5 (`ASUT_ANYTIME` / `ASUT_ANYTIME_EXCEPT_AIR`), the saved MP / slot / skill give-back, `+0x88bc`. **Use `32cf5760`:** `SpecialAttackNoStageChange` (vslot 0x810) true -> cooldown (0x800), memo refund (0x764), `+0x8eb0` = 0; false -> reserve (0x804) + `31fd32e0`; `+0x8ac4` = 1 in both.
- **NoStateChangeActionFrameMove `32d1ddf0`.** Cases 1 / 10 / 11 (read the `dec` / `sub 9` chain), `ShowSkillNameStateString( 21201 )` after cases 1 and 11 only. **ProcessEnduranceSkill `32d1dbd0`:** templet `+0x178` = `m_iDeleteSkillIDByUpgrade`, `KBuffFactor` `+0x20` = the buff templet id, vslot 0x23c (`SetSkillBuffFactors`) args (21420, 1, true) / (21420, 2, true), `GetSkillLevelValue( 21420 )`.
- **Constructor `32d09490`.** The values of the eleven cool-time floats (-1.0, `m_fBrutalSlayerElapsedTime` 0.0: read the `fld` before each `lea` and the `xor eax, [esp+0x58/0x60]` that ends its block), `+0x95bc` = `m_hDoubleSlash`, `+0x95c0` = `m_fMagicReflectRate`, and that nothing else in June's list differs from March's. The eight unnamed June-only members (`ch1_offsets.tsv`).
- **`AfterImageFrameMove` `32d08d70` / `IsNotCommonMoveState` `32d08b30`** (the two live under `_LIVE` because they override base virtuals): the compared state ids (13 = `USI_WAIT`, 206 = `ESSI_SWORD_WAIT`, 154 = `ESSI_DASH_JUMP_POWER_LANDING`), the future vs now state (`+0x7e6c` vs `+0x7cbc`).
- **Cut-in `32d09050`:** the table copy index (`+0x8ac8`, stride 0x30 x 3) and the two event cut-ins (`337aec60` / `337aec90` = `HQ_CutIn_Elsword_CASH_21` / `_22`, by analogy with Chung's; read the static initialiser if you can).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_LIVE --with JUNE_CHAR_ELSWORD_CORE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h` (then `--range <commit>^..<commit>`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1k - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1m.md`, commit with explicit pathspecs.
