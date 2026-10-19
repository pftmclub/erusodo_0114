# June character Aisha (Stage 5 pilot) - Phase CH0b - Every June Aisha asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH0b` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_ARME_ASSET` |
| Log tag | `[JUNE_CHAR]` (`ARME DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH0 - Aisha map (read-only)" "Phase CH0b - Every June Aisha asset, whole, first"
```
Then CH0's Done note and `june2017_re/char_arme/ch0_assets.tsv` (owner Aisha, kind not Lua).

## Task

Do "Phase CH0b". The user's priority: Aisha's assets all become June's at once, with no half-March, half-June character.
1. Save a pre-CH0b `ErrorLog.txt` reference.
2. Stage every listed file with `kom.py stage-june`, byte-exact.
3. Pack each touched kom, then ask before each copy.
4. Hand over one play-test: one Aisha per job tier, a costume and a weapon worn, three skills cast, village and dungeon.

A file the list missed (seen as a missing-file line in the play-test) is staged in this phase and added to `ch0_assets.tsv`. Never keep
March's copy to make something load: a reader that breaks gets a code fix, under the flag above.

## Amended by CH0

Written by CH0 (2026-10-20).

- **The list.** `june2017_re/char_arme/ch0b_stage_list.tsv` (5,688 files, columns `name june_kom placement_kom state chosen_by`;
  `kom.py stage-june` takes it as a TSV with a `name` column: `--dry-run --plan-out` first). It is the CH0b rows of `ch0_assets.tsv` (13,833 rows,
  routes a / b / c unioned) whose March state is `missing` (5,302) or `march-copy` (386): texture 4,026, mesh 1,331, sound 250, motion 64,
  other 17. 7,975 more CH0b rows are already June's bytes; 99 `dangling` rows (item.kim or a mesh names them, no kom holds them) stage nothing.
- **Koms.** 54 placement koms. Large: data015 2,370, data191 603, data130 414, data016 329, datai004 304, data198 289, datai023 259, data092 157,
  data067 119, data192 104, datai031 92, data131 81. Cross-kom moves: data200 -> data033 (the 10 base `Motion_Arme*`), data191 -> data130 7,
  data015 -> datai004 2, one each data016 -> data107, data130 -> data067, data192 -> data130. 54 June-only motions stay in data200.
  Ask the user once with the whole kom list rather than 54 times, and copy only what they approve.
- **Readers a June asset could break: none blocking.**
  - `Motion_Arme_EM.x`: June moves `EM_METEOR_SHOWER_Start` / `_Loop` / `_End` to `Motion_Arme_HM.x`. The EM class loads both (UnitTemplet
    `m_GameMotion1 = HM`), so March's script still finds them: **stage EM and HM in the same pack**. No other base motion loses a name.
  - `Motion_Arme_Common.x`: 34 of 76 bind-pose frames change (Bip01 y +8.6). June fixes a March outlier: June Common equals June and March
    `Motion_Arme.x`. Look at Aisha standing in the village in the play-test.
  - Same bones and frame counts in every base motion; animation sets only grow.
  - Hard-coded names in March's C++: only `elsword_SA_SwordBlastiong_slash.dds` is absent from June (it stays March's file, and the C++ that
    names it goes away with CH1-CH3); 4 literals are in no kom at all (`AVP_Hell_Drop_Light.x`, `Arme_ICEballS2.dds`, `Black_Light.tga`,
    `EM_Cyclone_Effect.x`): already dead.
  - 2 June meshes did not decompress for the texture scan (`Mesh_Arme_Cash_Upbody860_Sorted.x`, `Mesh_Arme_Cash_Hair1590_A_Sorted.x`): stage them
    anyway; their textures come by name route (a).
- **Particle and mesh-player definitions need nothing.** Aisha's entries live in the files every character shares (`GameMajorParticle*.txt`,
  `GameMinorParticle.txt`, `Game*XMeshPlayer.txt`, `UI*`), and all 75 of them in kom_contents are byte-identical to June's (only the four
  `_INT.txt` files in datai015 are March's own). The effect closure (`ch0_effect_closure.tsv`) agrees: every Aisha particle / mesh-player
  entry is June's.
- **Not staged here:** the 67 Lua rows (CH1, or owned elsewhere), 3 name false positives (`harmel`, `spearmesh`), and the 99 dangling rows.
- **Done-gate re-run:** for every `ch0b_stage_list.tsv` row, compare the kom_contents file with June's bytes (`tools/junekom.py`'s strict
  decode, the method CH0 used: equal = June's); 0 rows may stay `missing` or `march-copy`.
