INIT_SYSTEM = {
	UNIT_SCALE = 1,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"SKA_flame.dds",
		"SKA_flame2.dds",
		"Sudden_Buster_P03.dds"
	},
	READY_SOUND = {
		"EventCutScene06_Ranox_AMB01.ogg",
		"EventCutScene_Ranox06_Flap01.ogg",
		"EventCutScene_Ranox06_Flap02.ogg",
		"EventCutScene_Ranox06_Impact01.ogg",
		"EventCutScene_Ranox06_Magic01.ogg",
		"EventCutScene_Ranox06_Quake01.ogg",
		"EventCutScene_Ranox06_SkaVoice01.ogg",
		"EventCutScene_Ranox06_SkaVoice02.ogg",
		"EventCutScene_Ranox06_Spell01.ogg",
		"EventCutScene_Ranox06_Whoosh01.ogg"
	},
	READY_XMESH = {
		"ADD_Ground_Explosion_02.Y",
		"AEM_M_lightning01.Y"
	},
	READY_XSKIN_MESH = {
		"Bust_Wolf_M03.X",
		"Mega_Slash_Trace_Dummy.x",
		"Summon_SKA_Effect_Rock.X"
	}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_EVENT_FINALHALL_BOSS.x"
}
INIT_PHYSIC = {
	RUN_SPEED = 0,
	RELOAD_ACCEL = 2000,
	DASH_JUMP_SPEED = 0,
	WALK_SPEED = 0,
	G_ACCEL = 4000,
	MAX_G_SPEED = -2000,
	JUMP_SPEED = 0
}
INIT_COMPONENT = {
	MAX_HP = 30000,
	MP_CHANGE_RATE = 1,
	MP_CHARGE_RATE = 130,
	MAX_HYPER_MODE_TIME = 30,
	HYPER_MODE_COUNT = 0,
	USE_SLASH_TRACE = FALSE,
	HITTED_TYPE = HITTED_TYPE.HTD_STONE,
	NOT_CULL = TRUE
}
INIT_STATE = {
	{
		STATE_NAME = "EVENT_FINALHALL_BOSS_START",
		LUA_FRAME_MOVE_FUNC = "EVENT_FINALHALL_BOSS_FRAME_MOVE",
		LUA_STATE_START_FUNC = "EVENT_FINALHALL_BOSS_START_START"
	},
	{
		STATE_NAME = "EVENT_FINALHALL_BOSS_DYING",
		LUA_STATE_START_FUNC = "EVENT_FINALHALL_BOSS_DYING_START_START"
	},
	START_STATE = "EVENT_FINALHALL_BOSS_START",
	BIG_DAMAGE_LAND_BACK = "EVENT_FINALHALL_BOSS_START",
	SMALL_DAMAGE_AIR = "EVENT_FINALHALL_BOSS_START",
	DOWN_DAMAGE_LAND_FRONT = "EVENT_FINALHALL_BOSS_START",
	DAMAGE_REVENGE = "",
	DOWN_DAMAGE_AIR_LANDING = "EVENT_FINALHALL_BOSS_START",
	DOWN_DAMAGE_AIR = "EVENT_FINALHALL_BOSS_START",
	DYING_SKY = "EVENT_FINALHALL_BOSS_DYING",
	SMALL_DAMAGE_LAND_FRONT = "EVENT_FINALHALL_BOSS_START",
	DOWN_DAMAGE_LAND_BACK = "EVENT_FINALHALL_BOSS_START",
	FLY_DAMAGE_BACK = "EVENT_FINALHALL_BOSS_START",
	FLY_DAMAGE_FRONT = "EVENT_FINALHALL_BOSS_START",
	WAIT_STATE = "EVENT_FINALHALL_BOSS_DYING",
	BIG_DAMAGE_LAND_FRONT = "EVENT_FINALHALL_BOSS_START",
	SMALL_DAMAGE_LAND_BACK = "EVENT_FINALHALL_BOSS_START",
	BIG_DAMAGE_AIR = "EVENT_FINALHALL_BOSS_START",
	DYING_LAND_BACK = "EVENT_FINALHALL_BOSS_DYING",
	UP_DAMAGE = "EVENT_FINALHALL_BOSS_START",
	DYING_LAND_FRONT = "EVENT_FINALHALL_BOSS_DYING"
}
INIT_AI = {
	NO_BRAIN = TRUE
}
EVENT_FINALHALL_BOSS_START = {
	ANIM_NAME = "Start",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	RIGHT = TRUE,
	IMMADIATE_PACKET_SEND = TRUE,
	FOCUS_CAMERA_AT_CURRENT_STATE = TRUE,
	STOP_AT_CURRENT_STATE = TRUE,
	SOUND_PLAY0 = {
		0.01,
		"EventCutScene_Ranox06_AMB01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY1 = {
		0.8,
		"EventCutScene_Ranox06_Quake01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY2 = {
		5.9,
		"EventCutScene_Ranox06_Whoosh01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY3 = {
		6.1,
		"EventCutScene_Ranox06_Spell01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY4 = {
		9.2,
		"EventCutScene_Ranox06_Flap01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY5 = {
		9.2,
		"EventCutScene_Ranox06_SkaVoice01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY6 = {
		10.5,
		"EventCutScene_Ranox06_Impact01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY7 = {
		10.6,
		"EventCutScene_Ranox06_Magic01.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY8 = {
		12.3,
		"EventCutScene_Ranox06_Flap02.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	SOUND_PLAY9 = {
		13.7,
		"EventCutScene_Ranox06_SkaVoice02.ogg",
		100,
		FALSE,
		99999,
		FALSE
	},
	DIRECTION_STATE = TRUE,
	CAMERA = {
		SUB_CAMERA_TIMING = {
			20
		},
		SUB_CAMERA0 = {
			LOOK_PARTS = "Dummy_Camera_Target",
			EYE_PARTS = "Dummy_Camera01",
			CAMERA_TYPE = CAMERA_TYPE.CT_PARTS_LOOK_DIRECT,
			LOOK_TYPE = LOOK_TYPE.LT_BONE,
			EYE_TYPE = EYE_TYPE.ET_BONE
		}
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"EVENT_FINALHALL_BOSS_DYING"
		}
	},
	EFFECT_SET_LIST = {
		"EffectSet_Ranox_FinalHall_Stage_Boss_Direct",
		0
	}
}

function EVENT_FINALHALL_BOSS_START_START(slot0, slot1, slot2)
	slot2:SetPositionForce(D3DXVECTOR3(0, 0, 0))
end

function EVENT_FINALHALL_BOSS_FRAME_MOVE(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): all three SetMovieDescriptionData(...)
	-- calls below are omitted -- that method isn't bound in this March2014
	-- client (checked X2Lib/KTDXLIB, not found; calling it would raise a Lua
	-- error each time this frame-move fires). This was cutscene subtitle text (STR_ID_34174/
	-- 34175/34176) timed to the boss-intro animation; the cutscene itself
	-- still plays via ANIM_NAME/EFFECT_SET_LIST above, just without the
	-- on-screen caption text.
	if slot2:AnimEventTimer_LUA(4) then
	elseif slot2:AnimEventTimer_LUA(9) then
	elseif slot2:AnimEventTimer_LUA(11) then
	end
end

EVENT_FINALHALL_BOSS_DYING = {
	ANIM_NAME = "Dying",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}

function EVENT_FINALHALL_BOSS_DYING_START_START(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
	-- assignment and inlined the call into the condition; the port carried
	-- the bug verbatim. Bytecode-confirmed: decompile_verdicts.tsv,
	-- EVENT_FINALHALL_BOSS.src.lua::EVENT_FINALHALL_BOSS_DYING_START_START,
	-- dropped-assignment, seed 1, POISON_USE slot3 call (128-seed harness).
	slot3 = slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_FIRE_DEVIL_SKA)

	if slot3 ~= nil then
		slot3:StateChange_LUA("FIRE_DEVIL_SKA_START")
	end

	slot2:SetPositionForce(D3DXVECTOR3(0, -5000, 0))
end
