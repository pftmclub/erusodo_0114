INIT_SYSTEM = {
	UNIT_SCALE = 1,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"Magi_P01.dds",
		"WhiteImage.dds",
		"secretVelder_alchemystBoss_magicAttackA_dust.dds",
		"MagicSquareLight01_Poision.tga"
	},
	READY_XSKIN_MESH = {
		"DummyAttackBox_50x50x50.X"
	},
	READY_XMESH = {
		"Dark_Drabaki_Dead_01.Y"
	},
	READY_SOUND = {
		"Ranox_SD_Boss_Scene.ogg"
	}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_SECRET_SCENE_1_1.x"
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
		STATE_NAME = "SECRET_SCENE_1_1_START",
		LUA_FRAME_MOVE_FUNC = "SECRET_SCENE_1_1_FRAME_MOVE",
		LUA_STATE_START_FUNC = "SECRET_SCENE_1_1_START_START",
		LUA_STATE_END_FUNC = "SECRET_SCENE_1_1_START_END_FUNC"
	},
	{
		STATE_NAME = "SECRET_SCENE_1_1_DYING"
	},
	START_STATE = "SECRET_SCENE_1_1_START",
	BIG_DAMAGE_LAND_BACK = "SECRET_SCENE_1_1_START",
	SMALL_DAMAGE_AIR = "SECRET_SCENE_1_1_START",
	DOWN_DAMAGE_LAND_FRONT = "SECRET_SCENE_1_1_START",
	DAMAGE_REVENGE = "",
	DOWN_DAMAGE_AIR_LANDING = "SECRET_SCENE_1_1_START",
	DOWN_DAMAGE_AIR = "SECRET_SCENE_1_1_START",
	DYING_SKY = "SECRET_SCENE_1_1_DYING",
	SMALL_DAMAGE_LAND_FRONT = "SECRET_SCENE_1_1_START",
	DOWN_DAMAGE_LAND_BACK = "SECRET_SCENE_1_1_START",
	FLY_DAMAGE_BACK = "SECRET_SCENE_1_1_START",
	FLY_DAMAGE_FRONT = "SECRET_SCENE_1_1_START",
	WAIT_STATE = "SECRET_SCENE_1_1_DYING",
	BIG_DAMAGE_LAND_FRONT = "SECRET_SCENE_1_1_START",
	SMALL_DAMAGE_LAND_BACK = "SECRET_SCENE_1_1_START",
	BIG_DAMAGE_AIR = "SECRET_SCENE_1_1_START",
	DYING_LAND_BACK = "SECRET_SCENE_1_1_DYING",
	UP_DAMAGE = "SECRET_SCENE_1_1_START",
	DYING_LAND_FRONT = "SECRET_SCENE_1_1_DYING"
}
INIT_AI = {
	NO_BRAIN = TRUE
}
SECRET_SCENE_1_1_START = {
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
	IMMADIATE_PACKET_SEND = TRUE,
	FOCUS_CAMERA_AT_CURRENT_STATE = TRUE,
	STOP_AT_CURRENT_STATE = TRUE,
	DIRECTION_STATE = TRUE,
	CAMERA = {
		SUB_CAMERA_TIMING = {
			30
		},
		SUB_CAMERA0 = {
			LOOK_PARTS = "Dummy_Camera_Target",
			EYE_PARTS = "Dummy_Camera01",
			CAMERA_TYPE = CAMERA_TYPE.CT_PARTS_LOOK_DIRECT,
			LOOK_TYPE = LOOK_TYPE.LT_BONE,
			EYE_TYPE = EYE_TYPE.ET_BONE
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_Ranox_Secret_Scene_BossSt_Start",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_STATE_END
		}
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SECRET_SCENE_1_1_DYING"
		}
	},
	SOUND_PLAY0 = {
		0.01,
		"Ranox_SD_Boss_Scene.ogg",
		100,
		FALSE,
		99999,
		FALSE
	}
}

function SECRET_SCENE_1_1_START_START(slot0, slot1, slot2)
	slot2:SetPositionForce(D3DXVECTOR3(0, 0, 0))
	slot2:SetAddRotateDegree_LUA(0, -slot2:GetYRotateDegree(), 0)
end

function SECRET_SCENE_1_1_FRAME_MOVE(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- the
	-- same cutscene-subtitle gap first found in Phase 5 (EVENT_FINALHALL_BOSS.lua),
	-- recurring in every "_BOSS"/cutscene event file since. SetMovieDescriptionData
	-- does not exist anywhere in this repo's C++. The AnimEventTimer_LUA(2)
	-- condition is kept (the timer call may have a side effect independent
	-- of its return value) but its body (the caption-text call) is emptied,
	-- same disposition as its prior occurrences.
	if slot2:AnimEventTimer_LUA(2) then
	end
end

function SECRET_SCENE_1_1_START_END_FUNC(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
	-- assignment (read again by StateChange_LUA below) and inlined the call
	-- into the condition; the port carried the bug verbatim. Same drop
	-- happened in the elseif branch, which also merged its `slot3 ~= nil`
	-- check into an `and` -- restored to the bytecode's own nested if so
	-- slot3 has somewhere to live. Bytecode-confirmed: decompile_verdicts.tsv,
	-- SECRET_SCENE_1_1.src.lua::SECRET_SCENE_1_1_START_END_FUNC,
	-- dropped-assignment, seed 4, POISON_READ slot3@L148 (128-seed harness).
	if slot1:GetNowStageIndex() == 4 then
		slot3 = slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_SECRET_MAGI_BOSS)

		if slot3 ~= nil then
			slot3:StateChange_LUA("SECRET_MAGI_WAITSTART_READY", true)
		end
	elseif slot1:GetNowStageIndex() == 8 then
		slot3 = slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_SECRET_EXTRA_MAGI_BOSS)

		if slot3 ~= nil then
			slot3:StateChange_LUA("SECRET_MAGI_WAITSTART_READY", true)
		end
	end
end

SECRET_SCENE_1_1_DYING = {
	ANIM_NAME = "Dying",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	SHOW = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}
