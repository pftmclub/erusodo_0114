# June effect completion - C3q review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3q review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3q done (its Done note and commit) and its regression play-test |
| Flag | `SERV_IRUHADEV_JUNE_NPC_EFFECT_SET_LIST` |
| Log tag | none (no reachable data, no `_DEBUG` flag; see the Done note) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3q - Done"
grep -n "(C3q)" june2017_re/funcmap.tsv
grep -n "^| 163 " JUNE_BUGS.md
```

## Task

No play-test can reach the new code (the 10 `PVP_HERO_*` scripts cannot spawn in March), so this review is the only check of it.
Compare the commit line by line against June (Ghidra read-only, `june2017_re/disasm_range.py`, `funcdis.py`, `statedata_keys.py`):

- **The FINAL block** of `32784c00 @32787775-32787d09`:
  - the key format (`sprintf_s( buf, 0xff, "%s%d", "EFFECT_SET", n )` from n = 0, `@327877d4` and `@32787c78`);
  - the per-entry reset `32079980` (name L"", START_TIME 0, CONSUME_MP -1, conditions `{ DCES_NONE }`);
  - the miss defaults (`32fea370` START_TIME miss 0 `@32787a8c`, CONSUME_MP miss `334380f0` = -1 `@32787b95`);
  - the empty-name exit `@32787984` -> `@32787caf`: it closes `EFFECT_SETn` and then FINAL;
  - `DELETE_CONDITION` (`33476588`): `3221db80` clears the seed, then ints from index 1 (`32fea6e0`);
  - the push `@32787c5a` without a start-time filter;
  - **EndTable balance on every path in the port** (a slip would corrupt every later key of that state and no play-test shows it).
- **Order and sort:** LIST, EX, FINAL appended to `+0x1e4`, then `327c8b30` (stable, compare on `+0x1c`).
- **The gate** in `32776e10 @32776f10-32776f39`: `+0x30 <= 0` (or NaN) plays, otherwise `vslot 0xe4( +0x30, 0 ) == 1`; a failure jumps to the cursor
  increment `@327770b5`. The port is `if ( c > 0.f && false == FlushMp( c ) ) continue;`.
- **`3208dc70` = `CX2GameUnit::FlushMp`**: gage slot `0x2c` `32a31910` = March `CX2GageData::FlushMp`; the `IsMyUnit()` argument and `322b33d0`'s
  June-only accounting tail never matter for an NPC (`32756c10` = `xor al, al`).
- **The struct:** `m_fJuneConsumeMP` is `-1` in all three `ADD_TO_EFFECT_SET_LIST_EX_DELETE_CONDITION` ctors; user-side entries keep -1, so the
  user player is unchanged.
- **The rest of `32776e10` = March:** flag bits `0xe / 0xf / 0x10 / 0x11 / 0x15` against March's four delete lists and HYPER, and the
  `PlayEffectSet` arguments (`321e1e60`, trailing 1.0 = its default).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_NPC_EFFECT_SET_LIST --range <C3q commit>^..HEAD`.
Fix clear transcription errors under the same flag; hand design decisions back to the user.
