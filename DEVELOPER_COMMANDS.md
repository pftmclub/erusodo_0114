# Developer / GM slash commands (March 2014 client)

The in-game developer console accepts `/command args`. This is the full set the **March 2014** client ships: **108 commands**, plus **78 aliases** (mostly Hangul-keyboard typos of the English name).

**Provenance.** Command list, argument counts and aliases are parsed from `GameEditCommandList.lua` (decompiled from `237311/24965799/kom_contents/data036_kom/`). Whether each command can actually run was checked against the C++ bindings in the 2026-10-03 `US_SERVICE` build (`X2/US_SERVICE/x2.exe`, identical to the deployed `X2_offline.exe`). The offline column comes from a static trace of the packets each command sends, checked against `X2Lib/Offline/`. **None of this was play-tested.** June-only commands (122 of them) are deliberately left out.

## Read this first

- **`/destroy N` deletes every item in inventory tab N from your real save.** The emulator handles the delete packet, so there is no undo. Back up `els_db.sql` first.
- **`/crash` crashes the client on purpose.**
- **Most admin commands do nothing offline.** The emulator drops every `EGS_ADMIN_*` packet, so those commands run, get no answer, and have no effect. See the *Offline* column.
- **A dropped command ends in a popup.** The client waits for an answer that never comes, then shows a server-timeout OK popup. Dismiss it. I found no disconnect path in the handler, but this was not play-tested.
- **10 commands cannot run in this build at all** because the C++ they call was compiled out. They fail with the generic invalid-command message.

## Turning the console on

1. The account needs auth level **2 or higher** to open the console and **3 (dev) is the one to use**. Level 3 also allows raw Lua lines and the hotkeys. Level 10 (light operator) passes the numeric `>=` checks, but `/vp` and `/cs` contain an explicit light-operator refusal behind `LIGHT_OPERATOR_ACCOUNT`, whose state in this build was not checked.
2. Offline, the level comes from the save. With the game **closed**, back up `els_db.sql`, then:

   ```sql
   INSERT OR REPLACE INTO settings( name, value ) VALUES( 'auth_level', 3 );
   ```

   (`settings` is `name TEXT PRIMARY KEY, value INTEGER`.) Valid values are 0, 1, 2, 3 and 10; anything else is logged and treated as 0.
3. Start the game and log in again. The level is read on every login. Press **END** to open the console.

Side effects of raising the level, so they do not surprise you: at level 2+ the client sends an `EGS_ADMIN_SERVER_STATUS_REQ` every 60 seconds, which the emulator logs as `IGNORED`; and at level 3 dev-only UI switches on, for example the GM debug box in the enchant window, which the offline server honours as sent and uses to force the enchant to succeed.

| Level | Name | What it allows |
|---|---|---|
| 0 | normal | nothing |
| 1 | special | cannot open the console (the command gate is looser than the END-key gate) |
| 2 | operator | opens the console (END) and runs slash commands |
| 3 | dev | all of the above, raw Lua lines, hotkeys |
| 10 | light operator | passes `>=` checks; some commands refuse it |

## How arguments are parsed

`/name a b c` is turned into the Lua call `Slash_name(a,b,c)` and run with the full interpreter. That has consequences:

- **Command names are case-sensitive** (`/cm` and `/CM` are different commands).
- Numbers and `true` / `false` are written bare. **Strings must be in double quotes** (`/cm "BEE"`, `/l "Nick" 50 0`, `/k "Nick"`). A bare word is a Lua global, which is nil, and the command fails.
- **Exception:** `n`, `na`, `msc`, `pa`, `obs`, `afd`, `couple` take the **whole rest of the line as one string**, so no quotes. Whether each of these is in that group depends on build flags that were not individually checked.
- A failed command, an unknown name or a Lua error all print the same generic invalid-command message.
- Commands marked *In game only* print `게임중에만 실행 가능합니다` ("only runnable in a game") when typed anywhere else. Spawn and lookup commands can also print `잘못된 몬스터 이름입니다` (bad monster name) or `나의 유닛을 찾을 수 없습니다` (your unit was not found).
- The Hangul aliases only work if what you type reaches the console as the same characters the Lua file holds, so they depend on a Korean IME and on the text-conversion flag of the build (`CHEAT_WCHART_TO_UTF8`), which was not checked. The English names always work.
- Up / Down arrows walk the command history.

## Offline column legend

| Status | Meaning |
|---|---|
| No packet found | the trace found no server packet for it, so it should run entirely client-side. This is the *absence* of evidence of a packet, not proof; commands marked in-game only need to be used inside a dungeon or field |
| Handled | the emulator answers the packet it sends; it acts on the real save |
| Dropped | every packet it sends is ignored by the emulator (logged `IGNORED`); nothing happens. The client registers an ACK wait first (`AddServerPacket`). When it expires it posts `XGM_SERVER_PACKET_TIMEOUT`, which `X2State` answers with a server-timeout **OK popup**; no disconnect or quit path was found in that handler. Dismiss it. Not play-tested |
| Unhandled | its packet has no emulator case and no ignore rule; logged `*** UNHANDLED ***` |
| Unclear | the trace reached several packets or a mix; read `offline_packets.log` after trying it |
| Not compiled | the C++ method it needs is not in the exe |

The trace follows calls two levels deep from each command and drops one position-sync packet that nearly everything reaches through a shared helper. It is a static approximation. After trying any command, `grep` its packet in `offline_packets.log` to settle it.

## Notices, accounts and admin

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/a` |  | `/a "UserID" level` | Set an account's auth level. | Dropped by emulator (`EGS_ADMIN_CHANGE_AUTH_LEVEL_REQ`) |
| `/al` |  | `/al level` | List the users that hold a given auth level. | Dropped by emulator (`EGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ`) |
| `/channel` | `/초무ㅜ디` | `/channel code` | Set the publisher "channeling" code. | Dropped by emulator (`EGS_ADMIN_CHANNELING_CODE_CHEAT_NOT`) |
| `/dev` |  | `/dev true\|false` | Toggle developer-cheat-code mode (client side). | No packet found (local) |
| `/dg` |  | `/dg guildUID` | Delete a guild by UID. | Dropped by emulator (`EGS_ADMIN_DELETE_GUILD_ADD_NOT`) |
| `/em` | `/드` | `/em true\|false` | ED-monitoring mode on or off. | Dropped by emulator (`EGS_ADMIN_ED_MONITORING_CHEAT_NOT`) |
| `/gsp` | `/ㅎ네` | `/gsp n` | Give guild skill points. | Dropped by emulator (`EGS_ADMIN_GET_GUILD_SKILL_POINT_REQ`) |
| `/guest` |  | `/guest true\|false` | Guest/trial-account mode on or off. | Dropped by emulator (`EGS_ADMIN_GUEST_USER_MODE_NOT`) |
| `/i` |  | `/i` | Toggle GM invisibility. | Emulator does not handle `EGS_TOGGLE_INVISIBLE_REQ` |
| `/k` | `/ㅏ` | `/k "Nick"` | Kick a user by character nickname. | Dropped by emulator (`EGS_ADMIN_KICK_USER_REQ`) |
| `/kid` |  | `/kid "UserID"` | Kick a user by account ID. | Dropped by emulator (`EGS_ADMIN_KICK_USER_REQ`) |
| `/n` | `/ㅜ`, `/공지` | `/n message` | Server notice to the current server group. The whole rest of the line is the message, **no quotes**. | Emulator does not handle `EGS_NOTIFY_MSG_REQ` |
| `/na` | `/ㅜㅁ` | `/na message` | Same notice, sent to **all** server groups. Rest of the line, no quotes. | Emulator does not handle `EGS_NOTIFY_MSG_REQ` |
| `/shop` | `/놰ㅔ` | `/shop period` | Proxy-merchant (personal shop agency) period cheat. | Dropped by emulator (`EGS_ADMIN_PSHOP_AGENCY_CHEAT_REQ`) |
| `/spc` | `/넻` | `/spc enable [type]` | Switch PC-bang (internet-cafe) status on or off for the account; optional PC-bang type. | No packet found (local) |
| `/stdb` |  | `/stdb` | Send the DB statistics log. | Emulator does not handle `EGS_SEND_STATISTICS_DBLOG_REQ` |
| `/stloc` |  | `/stloc` | Send the location statistics log. | Emulator does not handle `EGS_SEND_STATISTICS_LOCLOG_REQ` |
| `/usered` | `/ㅕㄴㄷㄱㄷㅇ` | `/usered "Nick"` | Show a user's ED (ED-monitoring cheat). | Dropped by emulator (`EGS_ADMIN_SHOW_ED_CHEAT_REQ`) |

## Character, items and economy

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/ad` | `/ㅁㅇ` | `/ad n` | Add a flat developer damage value to your attacks (`SetDevAddDamage`). | No packet found (local) |
| `/b` | `/ㅠ` | `/b itemID quantity period` | Buy any item by ID. `period` is the duration for timed items (June's help text says -1 removes the auto-delete date, in-house only; not re-verified for March). Offline, whether the emulator accepts an arbitrary item ID through that packet was not tested. | Emulator handles `EGS_BUY_ED_ITEM_REQ` |
| `/bel` | `/ㅠ디` | `/bel level` | Buy the "super equipment" for a given level (`BuySuperEqipLevel_LUA`). | Emulator handles `EGS_BUY_ED_ITEM_REQ` |
| `/cs` | `/ㅊㄴ` | `/cs n` | Change spirit. | Dropped by emulator (`EGS_ADMIN_CHANGE_SPIRIT_REQ`) |
| `/csd` | `/ㅊㄴㅇ` | `/csd a b c d e` | Change the cash-skill-point expiry date (5 numbers; presumably year month day hour minute, by analogy with June's `/SkillExpandDate`). | Dropped by emulator (`EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ`) |
| `/destroy` |  | `/destroy N` | **DESTRUCTIVE.** Sends a delete request for **every item** in inventory tab N (1 equip, 2 accessory, 3 quick slot, 4 material, 5 quest, 6 special, 7 avatar, 8 pet). Offline this deletes from `els_db.sql` for real. | Emulator handles `EGS_DELETE_ITEM_REQ` |
| `/e` | `/ㄷ`, `/돈` | `/e "Nick" amount` | Change a character's ED. **March takes the nickname first.** Whether `amount` is absolute or a delta is decided by the server code, which is not in this tree. | Dropped by emulator (`EGS_ADMIN_CHANGE_ED_REQ`) |
| `/l` | `/ㅣ` | `/l "Nick" level exp` | Set a character's level and EXP. **March takes the nickname first** (June did not). | Dropped by emulator (`EGS_ADMIN_MODIFY_UNIT_LEVEL_REQ`) |
| `/pf` |  | `/pf n` | Add party-fever points. | Dropped by emulator (`EGS_ADMIN_INCREASE_PARTY_FEVER_POINT_NOT`) |
| `/t` | `/ㅅ` | `/t titleID [x]` | Grant a title (`GetTitleCheat`, 2 args; the second is not documented anywhere in this tree). | Dropped by emulator (`EGS_ADMIN_GET_TITLE_CHEAT_REQ`) |
| `/uc` | `/ㅕㅊ` | `/uc classID` | Change the character's unit class. | Dropped by emulator (`EGS_ADMIN_CHANGE_UNIT_CLASS_REQ`) |
| `/uc2` | `/전직` | `/uc2 "ClassName"` | Class change by **Korean class name** (50 are accepted, e.g. `"소드나이트"`, `"아이샤"`, `"레나"`). It maps the name to a class and then makes the same request as `/uc`. | Dropped by emulator (`EGS_ADMIN_CHANGE_UNIT_CLASS_REQ`) |
| `/vp` | `/페` | `/vp rating rPoint aPoint` | Change PvP rating / R-point / A-point. **March takes 3 arguments** (June took 4). | Dropped by emulator (`EGS_ADMIN_CHANGE_VSPOINT_REQ`) |
| `/wsp` | `/ㅈ네` | `/wsp n` | Set Elsword's Way-of-Sword points. | No packet found (local) |

## Skills

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/fsk` | `/ㄹ나`, `/스킬` | `/fsk` | Fill your skill tree to the maximum. | Dropped by emulator (`EGS_ADMIN_CHEAT_GET_ALL_SKILL_REQ`) |
| `/fskl` | `/ㄹ나ㅣ` | `/fskl` | Fill your skill tree up to what your level allows. | Dropped by emulator (`EGS_ADMIN_AUTO_GET_ALL_SKILL_REQ`) |
| `/isk` | `/ㅑ나`, `/스킬초기화` | `/isk` | Reset your skill tree. | Dropped by emulator (`EGS_ADMIN_INIT_SKILL_TREE_REQ`) |
| `/skl` | `/나ㅣ` | `/skl skillID level` | Set one skill's level. The client rejects IDs that are not your class's and levels outside 1..max. | Dropped by emulator (`EGS_ADMIN_GET_SKILL_REQ`) |
| `/sp` | `/스킬포인트` | `/sp n` | Set your skill points. | Dropped by emulator (`EGS_ADMIN_CHANGE_SKILL_POINT_REQ`) |

## Monsters and NPCs

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/ai` | `/먀` | `/ai true\|false` | Enable or disable AI for all NPCs. *In game only.* | No packet found (local) |
| `/ak` | `/마` | `/ak` | Kill all NPCs. *In game only.* | No packet found (local) |
| `/cm` | `/츠` | `/cm "NAME" attrib1 attrib2 sa1 sa2 level` | Spawn a monster through an admin request. `NAME` is the `NUI_` enum suffix (any case). In one game type it forwards to `/CM`. *In game only.* | Dropped by emulator (`EGS_ADMIN_NPC_UNIT_CREATE_REQ`) |
| `/CM` |  | `/CM "NAME" count level` | Spawn monsters locally at your position. `count` is capped at 50 and the loop is inclusive, so **you get count+1** (default 1 gives 2). *In game only.* | No packet found (local) |
| `/cmp` | `/츠ㅔ` | `/cmp "NAME" x y z` | Spawn one monster at x,y,z (at your position if x is 0). *In game only.* | No packet found (local) |
| `/cmt` |  | `/cmt "NAME" level team` | Spawn an **ally** monster. *In game only.* | No packet found (local) |
| `/monhp` |  | `/monhp n` | Set the HP of every living NPC to n. *In game only.* | No packet found (local) |
| `/monhpR` |  | `/monhpR percent` | Set every living NPC's HP to percent of its max. *In game only.* | No packet found (local) |
| `/ms` |  | `/ms "NAME" scale time` | Rescale the nearest monster of that type for `time`. *In game only.* | No packet found (local) |
| `/msc` | `/ㅁㄴㅊ` | `/msc state` | Change the state of the last monster you created. Rest of the line, no quotes. | No packet found (local) |
| `/nmp` |  | `/nmp` | Fill all NPCs' MP. *In game only.* | No packet found (local) |
| `/shownpc` | `/ShowNpc` | `/shownpc true\|false` | Show or hide NPCs. | No packet found (local) |

## Your unit (in-game)

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/canon` |  | `/canon` | Add a Chung cannon ball. *In game only.* | No packet found (local) |
| `/dp` | `/에` | `/dp n` | Set Add's Dynamo Point. *In game only.* | No packet found (local) |
| `/energy` |  | `/energy` | Add to the Ara gauge. *In game only.* | No packet found (local) |
| `/invin` |  | `/invin 0\|1` | **0 turns invincibility + no-MP-cost ON, 1 turns it OFF** (inverted from what you would guess). *In game only.* | No packet found (local) |
| `/mhp` | `/ㅡㅗㅔ`, `/체력` | `/mhp n` | Set your own HP. *In game only.* | No packet found (local) |
| `/muc` | `/ㅡㅕㅊ` | `/muc n` | Set Add's Mutation Count. *In game only.* | No packet found (local) |
| `/nocooltime` |  | `/nocooltime` | Remove skill cooldowns. There is no command to turn it back off. | No packet found (local) |
| `/Scanon` |  | `/Scanon` | Add a Chung extra cannon ball. *In game only.* | No packet found (local) |
| `/UnitScale` | `/unitscale`, `/ㅕㅜㅑㅅㄴㅊ믿`, `/us`, `/ㅕㄴ` | `/UnitScale scale time` | Rescale your own unit for `time`. | No packet found (local) |
| `/wakeup` |  | `/wakeup` | Add one awakening (hyper-mode) count. *In game only.* | No packet found (local) |

## Camera and display

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/EffectLog` | `/이펙트로그`, `/EFFECTLOG`, `/effectlog` | `/EffectLog level` | Effect log level: 0 off, 1 damage-effect and effect-set log, 2 also particle/mesh log (not implemented). | No packet found (local) |
| `/fc` | `/ㄹㅊ` | `/fc true\|false` | Free camera on/off. | No packet found (local) |
| `/fov` |  | `/fov n` | Set the projection field of view. | No packet found (local) |
| `/kfc` |  | `/kfc true\|false` | Free camera plus world-camera edit mode. | No packet found (local) |
| `/linemap` | `/라인맵` | `/linemap` | Toggle drawing the line map (in-game only in March; June also handled villages). | No packet found (local) |
| `/mc` |  | `/mc true\|false` | Manual camera position on/off (in-game only). *In game only.* | No packet found (local) |
| `/sdm` |  | `/sdm true\|false` | Show/hide **all** dialogs (the whole UI). | No packet found (local) |
| `/sf` | `/ㄴㄹ` | `/sf speed time` | Slow-motion: speed factor and duration. | No packet found (local) |
| `/suv` |  | `/suv true\|false` | Show/hide the unit-viewer UI. | No packet found (local) |

## Warp and stage

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/fwarp` | `/ㄹㅈㅁ게` | `/fwarp n` | Warp to a battlefield by index; **n must be below 27**. `/fwarp 0` prints the table: Ruben 1, Elder 2-3, Bethma 4-6, Altera 7-9, Feita 10-12, Velder 13-17, Hamel 18-22, Sander 23-24. | Emulator handles `EGS_FRAME_AVERAGE_REQ`, `EGS_JOIN_BATTLE_FIELD_REQ` |
| `/stage` | `/ㄴㅅㅁㅎㄷ` | `/stage n` | Jump to stage n (sub-stage 0) inside a dungeon. *In game only.* | No packet found (not traced) |
| `/warp` |  | `/warp n` | Warp to a village by index; **n must be below 15**, otherwise "range not allowed". No table is printed in March; June's own table for 0-14 is Ruben 0, Elder 1-2, Bethma 3-4, Altera 5-6, Feita 7-8, Velder 9-10, Hamel 11-12, Sander 13-14. | Emulator handles `EGS_STATE_CHANGE_FIELD_REQ` |

## Pets and riding

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/pa` | `/ㅔㅁ` | `/pa motion` | Play a pet motion by name. Rest of the line, no quotes. | No packet found (not traced) |
| `/pet` | `/ㅔㄷㅅ` | `/pet type value` | Set a pet stat. Type per June's help text: 0 evolution step, 1 fullness, 2 intimacy, 3 extroversion, 4 sensitivity. | Dropped by emulator (`EGS_ADMIN_CHANGE_PET_INFO_NOT`) |
| `/rps` | `/ㄱㅔㄴ`, `/라이딩스테미너` | `/rps n` | Fully recharge riding stamina. | No packet found (local) |

## Boss field and battlefield danger

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/afd` | `/ㅁㄹㅇ` | `/afd n` | Add to the battlefield danger value, then re-read it. Rest of the line, no quotes. *In game only.* | Mixed (`EGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_REQ`=ignored, `EGS_BATTLE_FIELD_DANGEROUS_VALUE_NOT`=unhandled); read `offline_packets.log` |
| `/CloseRaidGate` | `/crg`, `/CRG` | `/CloseRaidGate` | Close the boss-field gate. *In game only.* | No packet found (local) |
| `/danger` | `/ㅇ뭏ㄷㄱ` | `/danger` | Show the current field danger value. | Emulator does not handle `EGS_BATTLE_FIELD_DANGEROUS_VALUE_NOT` |
| `/HelpField` | `/helpfield` | `/HelpField` | Print a short Korean help list for the field-danger commands. | No packet found (local) |
| `/OpenRaidGate` | `/org`, `/ORG` | `/OpenRaidGate` | Open the boss-field gate in the current field. *In game only.* | No packet found (local) |
| `/SetTD` | `/settd` | `/SetTD n` | Add to the gate's danger value, then re-read it. *In game only.* | Dropped by emulator (`EGS_ADMIN_GET_TOTAL_DANGEROUS_VALUE_REQ`, `EGS_ADMIN_SET_TOTAL_DANGEROUS_VALUE_REQ`) |
| `/TotalDanger` | `/totaldanger` | `/TotalDanger` | Show the danger value needed to open the boss-field gate. *In game only.* | Dropped by emulator (`EGS_ADMIN_GET_TOTAL_DANGEROUS_VALUE_REQ`) |

## Hero PvP and observer

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/obs` | `/ㅐㅠㄴ` | `/obs message` | Hero-match notice. Rest of the line, no quotes. | Dropped by emulator (`EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ`) |
| `/obsdel` | `/ㅐㅠㄴㅇ디` | `/obsdel a b` | Remove a user from a hero-match group (also parsed by the console itself). | Not compiled in: Handler_EGS_ADMIN_REMOVE_HERO_PVP_USER_REQ is not in the exe |
| `/obsmode` | `/ㅐㅠ느ㅐㅇㄷ` | `/obsmode n` | Observer mode. | Dropped by emulator (`EGS_ADMIN_OBSERVER_MODE_REQ`) |
| `/obsw` | `/ㅐㅠㄵ` | `/obsw serverGroup Nick message` | Whisper a hero-match notice to one user. The console parses this command itself, and the hero-match UI in the client builds it with an **unquoted** nickname. | Dropped by emulator (`EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ`) |

## Henir, rankings, events, daily quests

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/buff` | `/ㅠㅕㄹㄹ` | `/buff n` | Henir buff on your own unit, **dungeon only**: 0 ends it, 1 fire, 2 water, 3 nature, 4 wind (further values exist; they depend on `ADD_HENIR_BUFF`). June later redefined `/buff` as a buff-script reload; March has only this one. | No packet found (local) |
| `/Cobo` |  | `/Cobo a b` | Cobo event item give (event cheat). | Not compiled in: SetCoboEventItemGet (SERV_EVENT_COBO_DUNGEON_AND_FIELD off) |
| `/couple` | `/채ㅕㅔㅣㄷ` | `/couple n` | Add days to the couple (relationship) timer. Rest of the line, no quotes. | Dropped by emulator (`EGS_ADMIN_CHANGE_COUPLE_DATE_REQ`) |
| `/dhn` |  | `/dhn type "Nick"` | Delete a Henir ranking entry by nickname. | Dropped by emulator (`EGS_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT`) |
| `/dhr` |  | `/dhr type rank` | Delete a Henir ranking entry by rank. **March takes 2 arguments** (June took 3). | Dropped by emulator (`EGS_ADMIN_DELETE_HENIR_RANKING_RANK_NOT`) |
| `/dhu` |  | `/dhu type unitUID` | Delete a Henir ranking entry by unit UID. | Dropped by emulator (`EGS_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT`) |
| `/henir` | `/ㅗ두ㅑㄱ` | `/henir n` | Set the Henir reward count. | Dropped by emulator (`EGS_ADMIN_HENIR_REWARD_CHEAT_NOT`) |
| `/m` |  | `/m n` | Set the event power multiplier. *In game only.* | Not compiled in: SetMultiplyer (SERV_EVENT_CHECK_POWER off) |
| `/quest` |  | `/quest` | Refresh the daily quests. | No packet found (local) |
| `/randomcube` |  | `/randomcube a b c` | Developer log of opening a random cube: item, key item, count. | Not compiled in: RandomCubeOpen (SERV_DEVELOPER_RANDOM_OPEN_ITEM_LOG off) |
| `/VCount` |  | `/VCount n` | Valentine dungeon item count (event cheat). | Not compiled in: SetValenTineCountCheat (SERV_EVENT_VALENTINE_DUNGEON_GIVE_ITEM off) |

## Diagnostics and misc

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/crash` | `/ㅊㄱㅁ노` | `/crash` | **Deliberately crashes the client** (`IntentionalCrash`). | No packet found (local) |
| `/forcehost` |  | `/forcehost 0\|1` | Force this client to be the host (1) or release (0). | Not compiled in: ForceHost_LUA (ACTIVE_KOG_GAME_PERFORMANCE_CHECK off) |
| `/lag` |  | `/lag time type` | Simulate network lag. | Not compiled in: OccurLag_LUA (ACTIVE_KOG_GAME_PERFORMANCE_CHECK off) |
| `/lp` | `/ㅣㅔ` | `/lp` | Load every character picture (a texture load, not a leveling command). | No packet found (local) |
| `/ping` |  | `/ping n` | Set the average ping used by host selection. | No packet found (local) |
| `/sd` | `/ㄴㅇ` | `/sd a b` | Apply self-damage. | Not compiled in: SetSelfDamage (CHEAT_SELF_DAMAGE off) |
| `/sld` | `/닝` | `/sld` | Write the list of currently loaded devices to a file. | No packet found (local) |
| `/switchconnect` |  | `/switchconnect 0\|1\|2` | Connection mode: 0 default, 1 force P2P, 2 force relay. | Not compiled in: SwitchConnect_LUA (ACTIVE_KOG_GAME_PERFORMANCE_CHECK off) |
| `/tf` |  | `/tf "NAME"` | Transform into a monster (or back to a user for an unknown name). *In game only.* | Not compiled in: TransformIntoMonster/TransformIntoUser are commented out in X2GUUser.h and X2Main.cpp |
| `/uuu` | `/ㅕㅕㅕ` | `/uuu` | Clear all UI units. | No packet found (local) |

## No-ops

| Command | Aliases | Usage | What it does | Offline |
|---|---|---|---|---|
| `/h` | `/ㅗ` | `/h` | Empty function in March. | Does nothing |
| `/q` |  | `/q a b` | Empty function in March (June's `/q` sent a quest cheat). | Does nothing |

## Hotkeys

| Key | Needs | Effect |
|---|---|---|
| END | auth 2+ | open or close the console |
| Up / Down | console open | walk command history |
| `;` | auth 2+, in a village | class-change cheat dialog |

The training-room dummy spawns (double-press 1-4) and the boss-raid keys need auth 3 and depend on build flags that were not checked; the spawns use an `EGS_ADMIN_` packet, so they would be dropped offline anyway.

## What is not here

The in-game help list (`/h`) is empty: March's Lua never registers help text and `/h` itself is an empty function, so this document is the only reference. Commands that exist only in the June 2017 client are left out on purpose.
