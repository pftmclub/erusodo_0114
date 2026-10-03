# June 2017 client — what is in each `dataNNN_kom` folder

**Scope.** `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\` — the June 2017 client. This guide was built **from file names only** (a census of every file name under each folder: extension mix plus the most common name prefixes). No file headers, sizes or contents were opened, so every description is an inference from naming. Where a guess is shaky it says so.

Counts are approximate file counts (each folder also holds one `_manifest.json`, included in the counts). The census covered 266 folders and about 226,000 files.

## Quick lookup — "I want…"

| I'm looking for… | Go to |
|---|---|
| Client Lua (UI dialogs, dungeon/stage/monster/NPC scripts, config) | [Scripts](#1-scripts-lua) — `data034_kom`–`data040_kom` (+ `data001_kom`, `data217_kom`), `datai014_kom`, `datai015_kom`, `datai016_kom`, `datai030_kom` |
| A character's equipment/avatar meshes (hair, upbody, weapon, face…) | [Class meshes](#3-per-class-avatar--equipment-meshes) |
| A character's skill-effect meshes/textures | [Class effects](#4-per-class-skill-effect-assets) |
| A character's emotion/base motions | [Emotion motions](#5-per-class-emotion--base-motions) |
| A character's voice lines | [Voices](#12-sound--voices) |
| Monster / boss meshes and motions | [Monsters](#7-monsters-npcs-pets-riding) |
| Map/dungeon objects and textures for a region | [Maps](#6-maps--stage-geometry-by-region) |
| Shop / inventory item icons | [Item icons](#9-item--shop-icons-hq_shop_) |
| Loading screens | [Loading screens](#10-loading-screens) |
| UI textures (`DLG_*`) | [UI textures](#8-ui-textures-dlg_) |
| Sound effects for a skill / monster / ambience | [Sound](#11-sound--effects-and-ambience) |
| Shaders, fonts | [Engine](#2-engine--shaders-fonts-render-resources) |

## File-type legend (inferred from names)

| Ext | Likely meaning |
|---|---|
| `.lua` / `.luac` | client script; `.luac` and `.std.luac` are compiled; `.src.lua` is a source copy |
| `.x` / `.y` | 3D mesh. `.Y` dominates map/prop/effect folders, `.X` dominates character/monster folders (exact difference not determined from names) |
| `.xet` | companion to a mesh, always appears next to `.x` (extra per-mesh data; role not determined from names) |
| `.tga` / `.dds` | textures. `.tga` is older content, `.dds` is the newer "HQ_" content |
| `.tet` | companion to `.tga` in UI folders (`DLG_*.TET` next to `DLG_*.tga`), likely a UI texture-coordinate set |
| `.gfx` / `.swf` | Scaleform/Flash UI (`data194`, `data195`, `data152`, `eventscene/`) |
| `.ogg` / `.wav` | audio (`.wav` only in `data070`) |
| `.fx` / `.bmp` | shaders / toon-shading ramp bitmaps |

## Naming vocabulary you will see

- **Internal unit names ≠ display names:** `Arme` = Aisha, `Lire` = Rena (also `Rana`/`Lena` in some older names), `EL` = Elesis, `Lu`/`Ciel` = the two halves of Luciel (folders for both exist, plus a few `LuCiel` names).
- **Prefixes:** `NUI_` = monster/NPC unit mesh; `Motion_` = animation mesh; `Mesh_` = equipment mesh; `HQ_` = high-quality DDS texture; `DLG_` = UI dialog; `UI_NPC_` = NPC bust/portrait; `PVP_HERO_` = PvP hero NPCs; `AMB_` = ambience sound; `GFX_ID_####` = sound effect keyed by numeric ID.
- **Skill-code prefixes** in effect folders: three-letter job codes like `AEM`, `SI_SA`, `CDC`, `CTT`, `EBS`, `EEP`, `RVK`, `LDN`, `ALP`. Those are job/skill abbreviations — I did not decode them.
- **Region/variant tags:** `_INT` (international variant), `_JP`, `_CN`, `_EU`, `_US`, `_Event`, `_Cash` appear all over; `_Sorted` on `.X`/`.Y` is a mesh-sort variant.
- **Regions (world map):** Ruben, Elder, Besma, Peita (spelled `Feita` in some UI names), Altera, Velder (`Belder` in typos), Hamel, Sander, Ranox, Elysion, plus the `Time` event dungeon and Erda Island.

---

## 1. Scripts (Lua)

| Folder | ≈Files | What the names say |
|---|---:|---|
| `data034_kom` | 1,168 | **UI dialog scripts** — `DLG_*.lua` (windows, popups, map windows, class-change controls, wedding, emulate, user list, etc.), a few `Class_Change_Guide_*`. |
| `data035_kom` | 369 | **Dungeon definitions** — `Dungeon_<Region>_<Name>_<Difficulty>.lua` (Normal/Hard/Expert), tutorial/training dungeons, event dungeons, `DungeonData.lua`. |
| `data036_kom` | 387 | **Misc core scripts & data** — `GameMajorParticle##.txt` (68), `CodeScriptEnum_*.lua`, `PVP_Map_List`, `ShopItem.std.luac`, `TutorialGuideInfo`, `BattleFieldData.src.lua`, `Emblem_Manager`, `AccountQuest`, `DefaultGameOptions`, per-class script files (`Lire_ElvenRanger.lua`), plus a few `.xml` / `.ess` / `.kim`. A grab bag of "everything that isn't UI or stage." |
| `data037_kom` | 3,361 | **NPC / monster behaviour scripts** — `NUI_PVE_*`, `PET_*`, `GOD_*`, `LOW_*`, `RAID_*`, `SECRET_*`, `CSM_*`, `Event_*`, `Treasure_*`, `ELYSION_*`. The largest script set; one file per monster/boss/pet/summon. |
| `data038_kom` | 3 | `SQUARE_Unit.lua`, `SQUARE_GM_Ariel.lua` — square (lobby) unit/GM NPC. |
| `data039_kom` | 118 | **NPC UI scripts** — `UI_<NPCNAME>.lua` (Ariel, Adams, Emirate, Ignia, Luichel, …). |
| `data040_kom` | 1,745 | **Stage / line-map scripts** — `<Region>_<Place>_Stage#_LineMap.lua`, `Arcade_*`, `Event_*_Stage#`, `Field_*`, `*_Pvpmap_*`. One per stage. |
| `data217_kom` | 173 | **More UI dialog scripts** — `DLG_*.lua` including newer entries (`DLG_Class_Change_<Class>_<Job>.lua`, `DLG_Map_Local_*`, April-Fools 2017 UI). |
| `data001_kom` | 9 | A handful of event UI: `DLG_UI_Arbor_Day_Event*`, `DLG_UI_PPoru_Letter`, `DLG_Guide_Skill_Popup` (lua + tga + tet). |
| `datai014_kom` | 216 | International-series `DLG_*.lua` UI scripts, class change, terms-and-conditions. |
| `datai015_kom` | 55 | **Per-region configuration** — `Config_<REGION>[_TEST/_SERVICE].lua` (US, EU, BR, CN, JP, TH, TW…), `CodeScriptEnum_<REGION>.lua`, `AddCubePackageData`, `CharCustomData_INT`, `CashShopCategory`, `CustomEventScript`, `Movie*`. **First place to look for region switches.** |
| `datai016_kom` | 391 | International-series NPC/event scripts — `EVENT_*`, `PET_*`, `PVP_*`, `CSM_*`, `Dungeon_NPC_*`. Mirrors `data037` for event content. |
| `datai030_kom` | 8 | Event dungeon definitions (`Dungeon_Event_*`, Halloween, Treasure House, Burned Forest Secret). |

## 2. Engine — shaders, fonts, render resources

| Folder | ≈Files | Contents |
|---|---:|---|
| `data041_kom` | 8 | **Shaders** — `XRenderer.fx`, `GCSkin.fx`, `RenderState.fx`, `XEnvBoxRenderer.fx`, `XHeatShimmerRenderer.fx`, `XPostEffect.fx`, `XRenderer_X2TOOL.fx`. |
| `data042_kom` | 22 | **Toon-shading ramps** — `Cartoon*.bmp` (Normal, Damage, Fire, Invincible, Petrified, RimLight_*). |
| `data045_kom` | 39 | Misc engine textures — slash traces, lens flare, shadow blobs, weapon-intensify glows, KOG logo, `REC_GRAY`. |
| `data046_kom` | 32 | **World overlay layers** — rain/snow/dust/blend `.tga` (`WorldLayer_*`, `Secret-blend`, `Kindling_*`). |
| `data196_kom` | 27 | **Fonts** — `.ttf` / `.ttc` / `.otf` (KozGoPro, NanumBarunGothic, Gulim, FZLTHK, STXIHEI, DFT, wenquanyi…). |

## 3. Per-class avatar / equipment meshes

Roughly 4,000–6,000 files each; one `Mesh_<Class>_…` family per class (Cash, Ed (elite), Event, Set, AC = accessory, Hair, Face, Hand, Foot, Upbody, Lowbody, Weapon) with the `.tga` / `.xet` textures. `hyper`, `mash`, `mseh` entries are typo variants of the same names.

| Class | Main folder | Also in the international series |
|---|---|---|
| Aisha (`Arme`) | `data015_kom` | `datai004_kom` |
| Elsword | `data017_kom` | — |
| Eve | `data018_kom` | `datai010_kom` |
| Rena (`Lire`) | `data019_kom` | `datai012_kom` |
| Raven | `data020_kom` | `datai011_kom` |
| Chung | `data107_kom` | `datai006_kom` |
| Ara | `data131_kom` | `datai005_kom` |
| Elesis (`EL`) | `data143_kom` | `datai008_kom`, `datai009_kom` |
| Add | `data148_kom` | `datai025_kom` |
| Lu | `data158_kom` | `datai028_kom` |
| Ciel | `data159_kom` | `datai029_kom` |
| Rose | `data185_kom` | `datai032_kom` (small) |
| Ain | `data211_kom` | — |

Related:

- `data016_kom` (3,345) — **shared/common accessories**: `Mesh_Common_AC_*`, `Mesh_Common_Cash_AC_*`, `Elite_AC`, `INT_AC` (face/arm/upbody/lowbody accessories usable by any class), plus a few strays from other classes.
- `datai007_kom` (1,411) — international-series common AC (same naming, with 8–9 digit numeric IDs).
- `data021_kom` (101) — **SD (chibi) body meshes** for every class (`Mesh_<Class>_SD_*`).
- `data033_kom` (59) — `Add_Center_*` — Add-centre (boss/hero-style) character models and motions (`GLITER_HAMMER_BOSS`, `HERO_BALAK/EDAN/Proto`, `LIZARDMAN_OLDER_BLACK`).
- `data128_kom` (110) — **PvP hero NPC meshes** (`PVP_HERO_*`) and the `Npc_<Class>_NN` meshes, plus guardian/apple items.
- `data224_kom` (32) — **PvP hero motions** (`Motion_PVP_HERO_<NAME>`).

## 4. Per-class skill-effect assets

Small-to-medium folders (200–450 files) holding the skill FX meshes, projectile traces, and textures, prefixed with that class's job/skill codes.

| Class | Folder |
|---|---|
| Aisha | `data116_kom` (also `data122_kom` — small set of blade/magic-square FX: windblade, stormblade, armageddon) |
| Chung | `data117_kom` |
| Elsword | `data118_kom` |
| Eve | `data119_kom` (contains one stray `.lua`) |
| Raven | `data120_kom` |
| Rena | `data121_kom` |
| Ara | `data132_kom` |
| Elesis | `data142_kom` (`EffectSet_*`, `SA_*`, `SDK`, `SCA`) |
| Add | `data147_kom` (`ADD_*`, `ADE`, `ATT`, `ADM`, `ADB`) |
| Lu / Ciel | `data157_kom` (`LD*`, `LC*`, `LL*` families, `Roo_*`) |
| Rose | `data184_kom` (`Gunner_*`, `RCR`, `ROM`, `RVK`, `RSR`, `RFY`, `RHB`, `EZ8`) |
| Ain | `data212_kom` (`AIN_*`, `ABH`, `ALW`, `AAP`, `AEE`, `ALE`) |

### 4b. Monster / boss skill-effect assets (by region)

| Folder | ≈Files | Contents |
|---|---:|---|
| `data123_kom` | 36 | Peita boss/summon FX — `Summon_Boss_*`, `Peita_Stage6_Boss_Effect*`, `Peita_Offerings_*`, Cuttysark death effects, ice-impact laser, ice upper stone. |
| `data124_kom` | 50 | Velder monster FX — Spriggan, Vanguard, Dark Elf boss, Unohound, Alchemyst, Cannon Demon, Kenaz, dark arrow. |
| `data125_kom` | 147 | Hamel monster FX — Taranvash, Cloe, Conrad (Queen's Throne), Shadow Stinger, trap vortex, plus assorted `SM_*`/`IS_*`/`NG_*` effect pieces. |

## 5. Per-class emotion / base motions

Animation `.x` files only (`Motion_<Class>_Emotion_*`, `…_Cash##`, event/collab emotes like `GFRIEND`, `BTS`, `2017Maid`, `April_Fools_Day`, `Larva`, `SeeU`, `BbaBbaBba`, plus the class's base `Motion_<CLASS>.x` and `Motion_CSM_*` job motions).

| Class | Folder |
|---|---|
| Elsword | `data199_kom` |
| Aisha | `data200_kom` |
| Rena | `data201_kom` |
| Raven | `data202_kom` |
| Eve | `data203_kom` |
| Chung | `data204_kom` |
| Ara | `data205_kom` |
| Elesis | `data206_kom` |
| Add | `data207_kom` |
| Lu | `data208_kom` |
| Ain | `data209_kom` |
| Ciel | `data218_kom` |
| Rose | `data190_kom` (59); `data189_kom` (7) holds Rose's mechanic/skill motions `HA_ROM_MECAVOLT_MX`, `SA_RG_RX78_LAND_RUNNER` |
| — | `data219_kom`, `data220_kom`, `data221_kom` — a single `dummy2xx.X` each (placeholders) |

## 6. Maps / stage geometry by region

World-geometry `.Y` objects with `.dds` textures; stage-numbered names (`<Region>_<Place>_S#_…`). Region folders mostly contain field, village, dungeon and PvP-map pieces for that region.

| Region / theme | Folder(s) |
|---|---|
| Ruben + El Forest (Elder Tree, training centre, Arcade, tutorial stage) | `data024_kom` (1,294), `data025_kom` (691) |
| Elder (Benders cave, Wally castle, Druid forest, Arcade) | `data026_kom` (1,315), `data027_kom` (773) |
| Besma (lake, mines, air ship, Dragon Nest, PvP) | `data028_kom` (659), `data029_kom` (1,447) |
| Peita/Feita (church, offerings, spiral corridor) | `data030_kom` (942) |
| Altera (Nasod factory, plains, canon fort, parasite) | `data031_kom` (718), `data032_kom` (1,102) |
| Velder (capital, bridge, Winter) | `data101_kom` (1,097) |
| Hamel (waterway, magmanta cave, frozen water hall, Resiam, ancient waterway) | `data110_kom` (2,690) |
| Sander (Garpai Rock, dry Sander, Antiquity) | `data135_kom` (1,627) |
| Ranox (fire temple, geyser, Final Hall, Ashes village, gateway) | `data154_kom` (1,350) |
| Elysion (Atlas City, Soless, Alert City, Adrian, memory) | `data180_kom` (1,870) |
| Event / PvP / guild / boss-raid / defence / red-dragon / square / Christmas / highschool / "Space of Thought" | `data023_kom` (2,366) — the catch-all map grab bag |
| Imire waiting room (floating rocks, moon) | `data099_kom` (25) |
| Tutorial stages — one `Tutorial_<Class>_Stage0_*` set per class | `data164_kom` (496) |
| `Photo_*` photo-backdrop scenes (numbered `Photo_30000_1`, `Photo_RESIAM`, `Photo_Peita_*`) | `data193_kom` (294) |
| "Time" event dungeon (`Time1/2/3_Stage0_*`, per-class variants `_raven_`, `_ara_`, `_eve_`) | `data226_kom` (423) |

## 7. Monsters, NPCs, pets, riding

| Folder | ≈Files | Contents |
|---|---:|---|
| `data002_kom` | 2,893 | **General grab bag**: pet and riding skill effects (`Pet_*`, `Riding_*`), cash emotion/sit meshes, boss effect meshes (Nasod King, Behemoth Soul, Eltrion, Hellputt), Soless, atlas diorama, `11Gang_Lighting`. If you can't find an effect anywhere, check here. |
| `data003_kom` | 60 | Small misc: `Motion_Cubic*`, `Raid_Eltrion_Box`, fire machine, `DLG_Game_Page_01.X`, collision-box textures. |
| `data004_kom` | 359 | **Dropped-item visuals** — `DropItem_*`, `Item_Drop_<part>_<class>`, `Item_Coin_*`, `Item_Plate`, upgrade rock, cards. |
| `data005_kom` | 297 | Monsters (`NUI_*`) — Ruben-era set: pporu/bboru, monkeys, Benders, tree knights, giant pporu, chest monster, mushroom king. |
| `data006_kom` | 348 | Monsters — Elder-era: soldiers, Wolleetroopers, thieves, bboru. |
| `data007_kom` | 336 | Monsters — Besma-era: lizardmen, Nasod, Besma, Wally. |
| `data008_kom` | 214 | Monsters — Peita/Gliter era: Peita, Gliter, summons, "deffence" (defence dungeon) monsters. |
| `data009_kom` | 272 | Monsters — Altera era: Nasod, Altera, airship. |
| `data010_kom` / `data011_kom` | 38 / 20 | Small leftover Nasod / Wally / parasite / Sleshu sets. |
| `data012_kom` | 1,087 | **Event, God (`GOD_*`), Eltrion, field and Halloween monsters** (plus Pporu William, Christmas-square decorations, Valentine items). Big event-monster set. |
| `data013_kom` | 441 | **Village NPCs** (`NPC_*`, 240 files; Camilla, Elysion, Ruben villages). `data013_kom_ori` (440) is a backup copy of the same folder; `data013_kom` additionally has one `.bak` file. |
| `data014_kom` | 988 | **NPC dialogue portraits** — `HQ_UI_NPC_<NAME>_<Mood>_##.tga` (Smile, Sad, Wait, Surprise, Serious…) plus `UI_NPC_*.x` busts. |
| `data043_kom` | 147 | **Boss name plates** — `HQ_BOSS_NAME_<BOSS>.dds` and a few boss effect textures. |
| `data100_kom` | 434 | Velder-era monsters: Gliter, Nephilim, Dark Elf, Unohound, Bloody, Giant Unohound. |
| `data111_kom` | 442 | Hamel-era monsters: Ancient (crab, magmanta), Brutal, Cloe, Hellputt, Shadow, Piranha, protect-guard statues. |
| `data136_kom` | 613 | Sander-era monsters: Trock, Karuso, Bomber Lord, Mad Behemoth Soul, tentacles. |
| `data153_kom` | 348 | Ranox-era monsters: Fire Devil, Geyser, Mullock, Smith, Mount (Firebird, Mag Charger), Amethyst, Rage. |
| `data181_kom` | 393 | Elysion-era monsters/NPCs (Maya, Soless, Nasod). |
| `data106_kom` | 809 | **Pets** — `Pet_*`, `PETIT_*`, `PUI_*`, Bbeuddi tree map, teddy bear; pet textures and motions. |
| `datai003_kom` | 285 | International-series pet meshes (`Pet_*`, Bboru). |
| `data140_kom` | 175 | **Riding** — `Riding_*` (Bone dragon, Scooter, Bumblebee, Scorpion, Hellhound, Phantom Steed, Rickshaw, God Beast…) and motions. |
| `data165_kom` | 53 | Tutorial NPC/object motions (`Motion_TUTORIAL_*`, `EVENT_TUTORIAL_*`, victor, per-class tutorial objects). |
| `data225_kom` | 44 | "Time" dungeon motions (`Motion_Time_<Class>`, `TIME_BOSS_ANGEL`, layers). |
| `datai001_kom` | 54 | International-series misc effect/item meshes: pet, `FieldBoss_*`, `DungeonUse_*`, Bunny Arrow, accessory, head markers. |
| `datai002_kom` | 142 | International-series misc motions, dropitems, UI items, wait motions. |
| `datai024_kom` | 47 | International-series NPC / shop / UI meshes. |
| `datai026_kom` / `datai027_kom` | 14 / 167 | International NPC motions / mixed class and PvP motions. |

### 7b. Event content (cross-cutting)

Event assets are scattered in several folders by design: `data012_kom` (event monsters), `data004_kom` (event drops), `data002_kom` (event emotes), `data023_kom` (event maps), `data001_kom` (event UI), `datai016_kom` / `datai030_kom` (event scripts), `datai022_kom` (event UI incl. gashapon/gacha, birthday cube, treasure boxes), `datai020_kom` / `datai021_kom` (event buttons and class-change event images). `data078_kom` also holds `Event_*` / `Mon_Event_*` sound lines.

## 8. UI textures (`DLG_*`)

Texture sheets (`.tga` + `.tet`, or `.dds`). `DLG_*` is the dialog/UI prefix.

| Folder | ≈Files | Contents |
|---|---:|---|
| `data051_kom` | 62 | Backgrounds — character-select/unit backgrounds per class, PvP lobby, studio, training. |
| `data052_kom` | 408 | **Class-change guide images** — `DLG_CC_<CLASS>_##`. |
| `data151_kom` | 243 | **More class-change guide images** — `DLG_CC_<CLASS>_1ST/2ND_#` (same family as `data052`), plus common-texture and `DLG_UI_Text_##` sheets. |
| `data053_kom`, `data054_kom`, `data055_kom`, `data056_kom` | 81 / 111 / 69 / 111 | **Common UI texture sheets** — `DLG_Common_New_Texture##`, `DLG_UI_Common_Texture_*`, `DLG_Common_Texture##`, `DLG_UI_Button##`, `DLG_INT_Button##`. |
| `data057_kom` | 963 | **Big UI sheet set** — `DLG_UI_Common_Texture##(_NEW)`, storycut, cash-shop window, **field-map images** (`DLG_UI_FIELD_MAP_<Region>_##_##`), town maps, clouds. |
| `data063_kom` | 578 | **Misc UI** — cursors, buttons, emblems, **skill icons** (`DLG_SKILL_<CLASS>_Active/Passive_##`, riding, transcendence), dungeon-result, boss-raid UI. |
| `data064_kom` | 223 | **In-game guide images** — `DLG_UI_Guide*` (attribute upgrade, custom, Erda, event, guild, Henir, market, marriage, pet, professional job, PvP, search item). |
| `data065_kom` | 115 | Region artwork — `DLG_New_Region_*`, `DLG_Background_<Region>`, PvP image. |
| `data066_kom` | 17 | Map gate/village markers (`DLG_UI_MAP_Gate##`, `DLG_UI_MAP_Village##`). |
| `data068_kom` | 148 | Misc UI: channel icons, EXP gauge, messenger, guild-PvP UI, mouse-point, loading with Elsword. |
| `data215_kom` | 360 | Newer-UI set: boss state, `DLG_CU_<CLASS>_#_#` (class-unit art), common textures, town maps, `POPUP_TR_*` popups. |
| `data216_kom` | 67 | Village/rest-area UI, first-loading UI, ad boards. |
| `datai020_kom` | 184 | International-series: class-change item UI, common buttons (`DLG_INT_Button##_<Class>`). |
| `datai021_kom` | 435 | International-series: event class-name art, photobook, buff/INT common textures, Elesis event. |
| `datai022_kom` | 341 | International-series: birthday cube, treasure-box art, EU events (SheathKnight), gashapon/gacha. |
| `data047_kom` | 54 | **Minimap icons** — `MiniMap_*`, `Minimap_<Region>_Gate`. |
| `data048_kom` | 725 | Misc HQ icons — story-quest pictures, debuff icons (`Stat_*`), decals (`DM_*`), rank/combo marks, dice, `Quest_Complete`. |
| `data049_kom` | 210 | Result-screen art — `HQ_PveScore_#`, `HQ_Combo_#`, `HQ_Round`, `HQ_EVENT#_#`, achievements, PC marks. |
| `data050_kom` | 232 | Present/standing images — `HQ_Present_<Class>_Prol#_#`, `HQ_Standing_*`, 2017 Fool's Day start. |
| `data194_kom` | 132 | **Scaleform UI** — `.gfx`, `.swf`, `.dds` atlases (`Component_Library_*`, cash shop, battle PvP result, bingo, job-professional, `fonts_EU.swf`). |
| `data195_kom` | 280 | Scaleform/HQ UI `.gfx` + `.dds`. |
| `data152_kom` | 1,429 | **Cut-in art** — `HQ_CutIn_<Class>_CASH_##` `.gfx`/`.dds`, `CUT_IN_TALK_*`. |
| `data044_kom` | 482 | Cut-in/class-clear images — `HQ_CutIn_*`, `DDS_<Class>_<Job>_Clear_*`. |
| `datai019_kom` | 312 | International-series cut-in talk art (`HQ_CutIn_Talk_L/R_*`), cherry/jewelry/minimap, item firecracker. |
| `data022_kom` | 848 | **Title icons** — `TITLE_####.tga` and `UnitTitle####.xet`; also a few `HQ_Shop_Item_*` strays. |
| `datai013_kom` | 571 | International-series titles (same format as `data022`). |

## 9. Item / shop icons (`HQ_Shop_*`)

Almost entirely `.dds` icon files named `HQ_Shop_<Class>_<Category><number>.dds` or `HQ_Shop_Item_<itemID>.dds`. These are inventory and cash-shop icons.

| Folder | ≈Files | Notes |
|---|---:|---|
| `data067_kom` | 13,561 | Largest set; earlier class/item icons. |
| `data130_kom` | 7,292 | Mid-era set; includes Lu/Ciel and set icons. |
| `data191_kom` | 10,023 | Late set; Ara, Ciel, Chung, Elsword and "Common_INT_AC" icons. |
| `data192_kom` | 9,506 | Late set; Eve, Lire, Raven, set-gear icons. |
| `data198_kom` | 5,369 | Latest set; Ain/Ara/Ciel/Lu/Rose cash icons, onepiece icons. |
| `datai023_kom` | 10,251 | International series; `_JP_` and `_INT_` icons. |
| `datai031_kom` | 2,259 | International series, supplement. |

The same item ID can appear in several folders; when searching for one, check them all. The numbers in `HQ_Shop_Item_####` look like item IDs, but I did not check that against the item table.

## 10. Loading screens

`HQ_Loading*` textures. One small folder per region/area.

| Folder | Region |
|---|---|
| `data058_kom` | Ruben / El Tree |
| `data059_kom` | Elder (incl. 2016 Xmas, Wally Land) |
| `data060_kom` | Besma (air ship, dragon nest, lake night, secret, Bestera) |
| `data061_kom` | Peita/Feita |
| `data062_kom` | Altera |
| `data103_kom` | Velder |
| `data115_kom` | Hamel |
| `data126_kom` | Defence / witch Pporu house / Altera rest / training room / tutorials |
| `data139_kom` | Sander |
| `data156_kom` | Ranox |
| `data183_kom` | Elysion pits (Apocalypse, Atlas, Maya, Sky Crossroads, Suveillan) |

## 11. Sound — effects and ambience

| Folder | ≈Files | Contents |
|---|---:|---|
| `data069_kom` | 1,102 | **Player skill/attack SFX** — `<Class>_<Skill>_*.ogg` for Aisha, Ara, Chung, Elsword, Eve, Lena/Rena, Raven. |
| `data070_kom` | 433 | **Generic combat SFX** — hit, landing, swing, laser, drone, electric charge, fire; the only folder with `.wav` files. |
| `data071_kom` | 214 | Altera: Nasod, cannon fort, alarms, cutscene. |
| `data072_kom` | 86 | King Nasod, Bestera, Altera cutscene, Add-centre boss. |
| `data073_kom` | 122 | Besma: lizard monsters, armadillo king, airship, carts. |
| `data074_kom` | 184 | Wally, Nasod, airship, ancient (bone dragon) monsters. |
| `data075_kom` | 131 | Elder: ambience, bats, ents, thieves. |
| `data076_kom` | 193 | Peita: golem, glitter, plants, durahan, amethyst, mana. |
| `data077_kom` | 99 | Ruben/early: monkey king, giant/ancient Pporu, Benders, agit, chickens. |
| `data078_kom` | 649 | Bosses, boss raid, event monsters, event cutscenes, field ambience, voice-pack, April-Fools and emotion SFX. |
| `data085_kom` | 96 | **UI sounds** — achievement, skill tree, item upgrade, strengthen, level-up. |
| `data098_kom` | 99 | Peita boss/ambient (summons, Cuttysark, gargoyle, obelisk). |
| `data102_kom` | 222 | Velder-era monsters (Gliter, Unohound, Coacktrigle, alchemyst). |
| `data105_kom` | 111 | Velder ambience, cutscene, Spriggan, giants, traps. |
| `data114_kom` | 192 | Hamel monsters (shadow, coral, ancient Nasod, Cloe, Taranvash). |
| `data127_kom` | 81 | Hamel ambience and traps, Magmanta. |
| `data138_kom` | 137 | Sander ambience and monsters. |
| `data146_kom` | 81 | Field-boss (Iron Eltrion, Mad Behemoth Soul) and field ambience. |
| `data155_kom` | 286 | Ranox/Erda monsters and ambience (Magi, Iplitan, SKA). |
| `data182_kom` / `data197_kom` | 445 / 233 | Elysion monsters, ambience, Soless boss phases. |
| `data222_kom` | 43 | "Space of Thought" / Time dungeon ambience and Time boss. |
| `data086_kom`, `data087_kom`, `data089_kom`, `data090_kom`, `data091_kom`, `data112_kom`, `data113_kom`, `data137_kom`, `data166_kom` | 3–17 | **Village ambience**, one folder per village: `data086_kom` Altera, `data087_kom` Besma, `data089_kom` dungeon-gate ambience, `data090_kom` Elder, `data091_kom` Ruben, `data112_kom` Velder, `data113_kom` Hamel, `data137_kom` Sander, `data166_kom` generic (forest, computer room, robotic). `data088_kom` is miscellaneous village sounds (bank, accessory, lute, portal). |
| `data129_kom` | 273 | **Pet sounds** (`Pet_<Name>_<Action>`). |
| `data141_kom` | 431 | **Riding-pet sounds** (`RidingPet_*`). |
| `data167_kom`, `data168_kom`, `data169_kom`, `data170_kom`, `data171_kom`, `data172_kom`, `data173_kom`, `data174_kom`, `data175_kom`, `data176_kom`, `data177_kom`, `data178_kom`, `data179_kom`, `data188_kom`, `data214_kom` | 63–3,953 | **Numbered effect sounds** — `GFX_ID_####[_INT].ogg`; the number is a sound-effect ID, not a name, so these can only be found by ID. 15 folders, ≈15,800 files total. `data179` is the biggest. |
| `datai017_kom` / `datai018_kom` | 6 / 79 | International: balloon/pet-whistle/treasure-box sounds; pet and riding-pet sounds. |
| `music/` (outside kom folders) | 228 | Background music — `Music_*`, `Field_*`, `Li_*`, `Tutorial_*`, `Village_*`, `DungeonGate_*`, lobby, Halloween, character-select. |

## 12. Sound — voices

| Voice | Folder(s) |
|---|---|
| Aisha | `data092_kom` |
| Elsword | `data093_kom` |
| Eve | `data094_kom` |
| Rena | `data095_kom` |
| Raven (+ clone voice) | `data097_kom` |
| Chung | `data109_kom` |
| Ara | `data134_kom` |
| Elesis | `data145_kom` |
| Add | `data150_kom` |
| Lu | `data162_kom` |
| Ciel | `data163_kom` |
| Rose (+ Zero) | `data187_kom` |
| Ain | `data210_kom` |
| Extra per-class lines (Ain, Rose, Add, Lu, Luciel, Ciel, Elesis; some `rx`) | `data223_kom` |
| Village NPC voices (`<NPC>_<Village>_<Shop>_Wait/Bye_Talk`) | `data096_kom` |
| Monster voices (`*Voice_*`, `Herbaon`, `Karis`, `Soles`, `Eris`, `Pang`) | `data104_kom` |

Tutorial narration (`Tuto_<Class>_##`) is split into one folder per class: Aisha `data079_kom`, Elsword `data080_kom`, Eve `data082_kom`, Rena/Elder `data083_kom`, Raven `data084_kom`, Chung `data108_kom`, Ara `data133_kom`, Elesis `data144_kom`, Add `data149_kom`, Lu `data160_kom`, Ciel `data161_kom`, Rose `data186_kom`, Ain `data213_kom`; `data081_kom` is mixed (older Benders/Thief/Lena scenes, glass-break SFX).

## 13. Folders outside the `dataNNN_kom` pattern

| Item | What it is |
|---|---|
| `data013_kom_ori/` | Backup copy of `data013_kom` (NPC meshes) — one `.bak` fewer. |
| `eventscene/` | 2,385 `.swf` files — **event-scene / story-quest cutscene Flash**: `quest*` (2,285), tutorials, per-NPC and per-region scenes. |
| `movie/` | 63 `.avi` clear/create movies (`movie_<region>_clear`, `movie_create_<class>_base`). |
| `music/` | See §11. |
| `ui/` | `arrow.x`, `DXUTControls.dds`, `DXUTShared.fx` — DXUT leftovers. |
| `log/` | 6 `.htm` logs. |
| `xigncode/` | Anti-cheat module (`.xem`, one `.exe`, one `.log`). Not game content. |
| `ScreenCapture/` | Empty. |

Loose files in `data\` beside the folders: the packed `dataNNN.kom` archives themselves (plus `data001_packed.kom`, `data023_packed.kom` — rebuilt variants), `x2.exe`, `unpacked_x2.exe`, `x2.fx`, runtime DLLs, `elswordus.ini`/`elswordkr.ini`/`elswordustest.ini`, `dlg_first_loading*.tga` and similar boot-time UI images.

## Things the names suggest but I did not verify

- Whether the `datai###` ("international") series is loaded in the US build, or whether it overrides the same-named files in the main series. Names are consistent with a JP/INT overlay (`_JP_`, `_INT_`), but nothing here proves the load order.
- Which of the `.x`/`.y`/`.xet`/`.tet` pairs are required by which loader — the legend above is pairing-based, not format-based.
- Exact class coverage of the three-letter skill codes in §4.
- `data223_kom`'s purpose beyond "extra per-class voice lines"; its names (`ain`, `rose`, `add`, `luciel`, `ciel`, `elesis`, `lu`) are class names with no usage hint.
