# Atlas E6 - June dramatic messages: RE notes (working file)

June binary `unpacked_x2.exe`, imagebase 0x31aa0000. Opus RE, 2026-10-17.

## Data side (CX2Game)

| June | What | Notes |
|---|---|---|
| 322e9e00 | `CX2Game::LoadDramaticMessage( KLuaManager&, UidType npcUID, int stateID )` -> bool | thiscall, `RET 0x10`; this = g_pX2Game. Rows `DRAMATIC_MESSAGE%d` from 0 until a table is missing. Returns `count > 0` (SETG @322eb807) |
| 327ac9d0 @327af18c | caller: NPC state load | `this->+0x9060 = LoadDramaticMessage( this+0x2b4 (lua mgr), vslot 0x54 = GetUID(), state id (protected +0x8454) )`; sibling call before it = 323014b0 (ALARM_MESSAGE loader, not E6) |
| +0x8d8 | `vector<DramaticMessage> m_vecDramaticMessage[DMT_END]` (0x10 stride) | row = 0xb0 bytes |
| 322e61c0 | remove rows of ( uid, state ) where ( REPEAT or !played ) and !NO_STATE_MESSAGE | caller 3277ee10 |
| 322e6270 | remove every row of uid | caller 3279fb60 |
| 32306110 | per-frame player ( fElapsedTime ) | caller 32308c20 (CX2Game::OnFrameMove) @3230aa21, near the end, before 322cfe20 |
| 322d5bb0 | `Playing_Dramatic_Message_LUA( UidType uid, int type, int messageID, float startTime )` | `RET 0x14`; find row in vec[type] with uid and MESSAGE == id (3230eef0), set START_TIME (+0x14) |
| 32307a51 | ctor: `+0x48c = new CX2TalkBoxManager( 3 )` (dramatic) after `+0x488` (default) | |
| 32308613 | dtor: delete +0x48c | |
| 323095da | OnFrameMove: +0x48c->OnFrameMove after +0x488's | |
| 3230ad36 | base render 3230aa60: after the fade render (31ce94a0), `bX ? +0x48c->Render variant 31cda1a0 : 31cda140` (CKTDGObject OnFrameRender wrappers) | draws over the fade |

Row layout (0xb0): +0 index, +8/+0xc uid, +0x10 state, +0x14 START_TIME (0), +0x18 elapsed, +0x1c SHOW_TIME
(default _DAT_33436700), +0x20 REPEAT (1), +0x21 played, +0x24 TYPE, +0x28 IMAGE_FILE_NAME, +0x44/+0x48 IMAGE_OFFSET_POSITION
(default (_DAT_3344a510, 0)), +0x4c BACK_COLOR (1,1,1,1), +0x5c MESSAGE (-1), +0x60 MESSAGE_COLOR (0,0,0,1), +0x70/+0x74 POSITION
(0,0 -> per TYPE DAT_33733d74/78 (0) or 7c/80 (1), and +0x78 custom = 0), +0x78 custom-position (1 when given), +0x7c MAX_ROW_CHAR (16),
+0x80 USE_SPREAD (1), +0x84 SPREAD_TIME_GAP (_DAT_334395e0), +0x88 SOUND_FILE_NAME, +0xa4 NO_STATE_MESSAGE, +0xa5 ON_SKILL_SLOT_UI (1),
+0xa8 DISPLAY_TEAM_TYPE (-1).

Loader duplicate rule: an existing row with the same ( index, uid, state ) in vec[TYPE] is erased and re-added unless
REPEAT == 0 and played == 1 (then skipped).

## Display side (CX2TalkBoxManagerImpDramaticMessage, TBMT 3)

- RTTI `.?AVCX2TalkBoxManagerImpDramaticMessage@@`, vtable 3353c620, ctor 32f21840 (base ctor 32e939f0, font = 32257cf0( 2 ) -> +0x10),
  dtor 32f21ab0. Factory 32e904e0 case 3.
- June base Imp vtable 33535a14 has 10 slots: 0 dtor, 1 MsgProc, 2 OnFrameMove, 3 OnFrameRender, 4 InitTalkBox (new virtual),
  5 SeTOverTalkbox, 6 DrawBalloon, 7 SetBalloonTip (new virtual), 8 LineBreak (new virtual), 9 MouseMove.
  Dramatic overrides 4 (32f218c0), 7 (32f21380), 8 (32f21600).
- Dramatic InitTalkBox 32f218c0 = base 32e943d0 except box height = nRow * fontH (base: (fontH + 1) * nRow).
- Portrait: 32e92cf0 (called from base render after the text) creates UI particle `DramaticMessageImage_Left` / `_Right`
  (g_pData +0x7c), sets the texture to TalkBox +0xe4, stores the handle in +0x100 (destroyed on removal, 31cefbc0).
- TalkBox::Update 32e945c0: slide-up for uid -10 / -20 boxes (+0x104 moving, +0x10c target, +0x124 base y, step _DAT_334396b0).
- SetMove 32e90a30( bool, TalkBox* other ).

TalkBox (June 0x128) fields used: +8 uid, +0x14 bTraceUnit, +0x18 content, +0x34/+0x38 pos, +0x3c remain, +0x40 type, +0x44 font
article, +0x50/+0x54 size, +0x58/+0x5c scale, +0x68 tip lerp, +0x6c show, +0x70 text colour, +0x80 back colour, +0xa8 delay,
+0xdc max row char, +0xe0 max len (200), +0xe4 image name, +0x100 particle handle (-1), +0x104 moving, +0x108 x, +0x10c target y,
+0x110 custom pos, +0x111 use spread, +0x114 spread gap, +0x118/+0x11c image offset, +0x124 base y.
