INIT_SYSTEM = {
	UNIT_SCALE = 1,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"GroundShockWave.dds",
		"SpreadLight02_Gray.dds",
		"soket_wind_stom_wind01.dds"
	},
	READY_SOUND = {
		"EventCutScene01_Ranox03.ogg"
	},
	READY_XMESH = {},
	READY_XSKIN_MESH = {
		"Lire_SI_SA_Gungnir_Mesh05.X"
	}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_Ranox_Geyser_Boss_Stage_Direct.x"
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
		STATE_NAME = "EVENT_GAYSER_BOSS_START",
		LUA_FRAME_MOVE_FUNC = "EVENT_GAYSER_BOSS_FRAME_MOVE",
		LUA_STATE_START_FUNC = "EVENT_GAYSER_BOSS_START_START"
	},
	{
		STATE_NAME = "EVENT_GAYSER_BOSS_DYING",
		LUA_STATE_START_FUNC = "EVENT_GAYSER_BOSS_DYING_START_START"
	},
	START_STATE = "EVENT_GAYSER_BOSS_START",
	BIG_DAMAGE_LAND_BACK = "EVENT_GAYSER_BOSS_START",
	SMALL_DAMAGE_AIR = "EVENT_GAYSER_BOSS_START",
	DOWN_DAMAGE_LAND_FRONT = "EVENT_GAYSER_BOSS_START",
	DAMAGE_REVENGE = "",
	DOWN_DAMAGE_AIR_LANDING = "EVENT_GAYSER_BOSS_START",
	DOWN_DAMAGE_AIR = "EVENT_GAYSER_BOSS_START",
	DYING_SKY = "EVENT_GAYSER_BOSS_DYING",
	SMALL_DAMAGE_LAND_FRONT = "EVENT_GAYSER_BOSS_START",
	DOWN_DAMAGE_LAND_BACK = "EVENT_GAYSER_BOSS_START",
	FLY_DAMAGE_BACK = "EVENT_GAYSER_BOSS_START",
	FLY_DAMAGE_FRONT = "EVENT_GAYSER_BOSS_START",
	WAIT_STATE = "EVENT_GAYSER_BOSS_DYING",
	BIG_DAMAGE_LAND_FRONT = "EVENT_GAYSER_BOSS_START",
	SMALL_DAMAGE_LAND_BACK = "EVENT_GAYSER_BOSS_START",
	BIG_DAMAGE_AIR = "EVENT_GAYSER_BOSS_START",
	DYING_LAND_BACK = "EVENT_GAYSER_BOSS_DYING",
	UP_DAMAGE = "EVENT_GAYSER_BOSS_START",
	DYING_LAND_FRONT = "EVENT_GAYSER_BOSS_DYING"
}
INIT_AI = {
	NO_BRAIN = TRUE
}
EVENT_GAYSER_BOSS_START = {
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
		0.48,
		"EventCutScene01_Ranox03.ogg",
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
			"EVENT_GAYSER_BOSS_DYING"
		}
	},
	EFFECT_SET_LIST = {
		"EffectSet_Ranox_Geyser_Boss_Stage_Direct",
		0
	}
}

function EVENT_GAYSER_BOSS_START_START(slot0, slot1, slot2)
	slot2:SetPositionForce(D3DXVECTOR3(0, 0, 0))
end

function EVENT_GAYSER_BOSS_FRAME_MOVE(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): all six SetMovieDescriptionData(...)
	-- calls below are omitted -- that method isn't bound in this March2014
	-- client (checked X2Lib/KTDXLIB, not found; calling it would raise a Lua
	-- error each time this frame-move fires). Same gap already documented on
	-- march2014/update/data037/EVENT_FINALHALL_BOSS.lua (Phase 5). This was
	-- cutscene subtitle text (STR_ID_33826/33827/33828/33829/33829/33830)
	-- timed to the boss-intro animation; the cutscene itself still plays via
	-- ANIM_NAME/EFFECT_SET_LIST above, just without the on-screen caption text.
	if slot2:AnimEventTimer_LUA(7.5) then
	elseif slot2:AnimEventTimer_LUA(9) then
	elseif slot2:AnimEventTimer_LUA(10.25) then
	elseif slot2:AnimEventTimer_LUA(12) then
	elseif slot2:AnimEventTimer_LUA(13) then
	elseif slot2:AnimEventTimer_LUA(13.25) then
	end
end

EVENT_GAYSER_BOSS_DYING = {
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

function EVENT_GAYSER_BOSS_DYING_START_START(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
	-- assignment and inlined the call into the condition; the port carried
	-- the bug verbatim. Bytecode-confirmed: decompile_verdicts.tsv,
	-- EVENT_GAYSER_BOSS.src.lua::EVENT_GAYSER_BOSS_DYING_START_START,
	-- dropped-assignment, seed 1, POISON_USE slot3 call (128-seed harness).
	slot3 = slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_GEYSER_MULSAYRENA)

	if slot3 ~= nil then
		slot3:StateChange_LUA("GEYSER_MULSAYRENA_START")
	end

	slot2:SetPositionForce(D3DXVECTOR3(0, -5000, 0))
end
