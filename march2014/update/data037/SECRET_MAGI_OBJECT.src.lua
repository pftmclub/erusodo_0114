INIT_SYSTEM = {
	UNIT_HEIGHT = 200,
	UNIT_SCALE = 1,
	UNIT_WIDTH = 200,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"Magi_P01.dds",
		"secretVelder_alchemystBoss_magicAttackA_dust.dds",
		"SKA_flame7.dds"
	},
	READY_XMESH = {},
	READY_XSKIN_MESH = {},
	READY_SOUND = {
		"Magi_Object_Start01.ogg",
		"Magi_Object_Start02_C.ogg",
		"Magi_Object_Start03_C.ogg",
		"Chainball_Loop01.ogg",
		"Magi_Object_Fever_Start01.ogg",
		"Magi_Object_Fever_Start02_C.ogg",
		"Magi_Object_Fever_Start03_C.ogg",
		"Chainball_Damage01.ogg"
	}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_SECRET_MAGI_OBJECT.x"
}
INIT_PHYSIC = {
	RUN_SPEED = 300,
	RELOAD_ACCEL = 2000,
	DASH_JUMP_SPEED = 2300,
	WALK_SPEED = 300,
	G_ACCEL = 4000,
	MAX_G_SPEED = -2000,
	JUMP_SPEED = 1500
}
INIT_COMPONENT = {
	SHADOW_FILE_NAME = "shadow.dds",
	MP_CHANGE_RATE = 1,
	SMALL_HP_BAR_BLUE = "Small_HP_bar_Blue.TGA",
	HYPER_MODE_COUNT = 0,
	SMALL_HP_BAR_RED = "Small_HP_bar_Red.TGA",
	DIE_FLY = 0,
	HEAD_BONE_NAME = "Bip01_Head",
	SHADOW_SIZE = 200,
	SMALL_HP_BAR_YELLOW = "Small_HP_bar_Yellow.TGA",
	MP_CHARGE_RATE = 130,
	MAX_HYPER_MODE_TIME = 30,
	EXCLAMATION_MARK_SEQ = "ExclamationMarkNPC",
	QUESTION_MARK_SEQ = "QuestionMarkNPC",
	USE_SLASH_TRACE = FALSE,
	NOT_AFFECT_BLACKHOLE = TRUE,
	HITTED_TYPE = HITTED_TYPE.HTD_DEFAULT,
	FALL_DOWN = TRUE
}
INIT_STATE = {
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_START",
		LUA_STATE_END_FUNC = "SECRET_MAGI_OBJECT_START_END_FUNC"
	},
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_WAIT"
	},
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_FEVER_READY"
	},
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_FEVER"
	},
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_FEVER_END"
	},
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_DAMAGE"
	},
	{
		STATE_NAME = "SECRET_MAGI_OBJECT_DYING",
		LUA_STATE_START_FUNC = "SECRET_MAGI_OBJECT_DYING_STATE_START"
	},
	START_STATE = "SECRET_MAGI_OBJECT_START",
	BIG_DAMAGE_LAND_BACK = "SECRET_MAGI_OBJECT_DAMAGE",
	SMALL_DAMAGE_AIR = "SECRET_MAGI_OBJECT_DAMAGE",
	DAMAGE_REVENGE = "",
	DOWN_DAMAGE_AIR_LANDING = "SECRET_MAGI_OBJECT_DAMAGE",
	DOWN_DAMAGE_LAND_FRONT = "SECRET_MAGI_OBJECT_DAMAGE",
	DYING_SKY = "SECRET_MAGI_OBJECT_DYING",
	DOWN_DAMAGE_AIR = "SECRET_MAGI_OBJECT_DAMAGE",
	DOWN_DAMAGE_LAND_BACK = "SECRET_MAGI_OBJECT_DAMAGE",
	SMALL_DAMAGE_LAND_FRONT = "SECRET_MAGI_OBJECT_DAMAGE",
	FLY_DAMAGE_BACK = "SECRET_MAGI_OBJECT_DAMAGE",
	FLY_DAMAGE_FRONT = "SECRET_MAGI_OBJECT_DAMAGE",
	WAIT_STATE = "SECRET_MAGI_OBJECT_WAIT",
	BIG_DAMAGE_LAND_FRONT = "SECRET_MAGI_OBJECT_DAMAGE",
	SMALL_DAMAGE_LAND_BACK = "SECRET_MAGI_OBJECT_DAMAGE",
	BIG_DAMAGE_AIR = "SECRET_MAGI_OBJECT_DAMAGE",
	DYING_LAND_BACK = "SECRET_MAGI_OBJECT_DYING",
	UP_DAMAGE = "SECRET_MAGI_OBJECT_DAMAGE",
	COMMON_FRAME_FUNC = "SECRET_MAGI_OBJECT_COMMON_FUNC",
	DYING_LAND_FRONT = "SECRET_MAGI_OBJECT_DYING",
	DAMAGE_EXTRA_STATES = {
		"SECRET_MAGI_OBJECT_DAMAGE"
	}
}
INIT_AI = {
	NO_BRAIN = TRUE
}
SECRET_MAGI_OBJECT_START = {
	ANIM_SPEED = 1.5,
	ANIM_NAME = "Start",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	NEVER_MOVE = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SECRET_MAGI_OBJECT_WAIT"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_Ranox_Secret_Magi_Wait",
			1,
			DELETE_CONDITION_EFFECT_SET.DCES_CUSTOM_STATE
		}
	},
	SOUND_PLAY0 = {
		0.01,
		"Magi_Object_Start01.ogg"
	},
	SOUND_PLAY1 = {
		0.01,
		"Magi_Object_Start02_C.ogg"
	},
	SOUND_PLAY2 = {
		0.01,
		"Magi_Object_Start03_C.ogg"
	}
}

function SECRET_MAGI_OBJECT_START_END_FUNC(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- the
	-- removed block's gate, GetSimpleDLG() ~= nil, and its counter/gauge
	-- update calls (SetNumberInNumberStatic_LUA/SetNumberInGageStatic_LUA)
	-- are all absent from this repo's C++ (see SECRET_SCENE_DUMMY.lua's
	-- CreateSimpleDLG note for the full finding). Same "no substitute
	-- counter without inventing one" situation that note documents: this
	-- block's slot3:GetNumber_LUA()/SetNumber_LUA() read/write the
	-- fever-gauge dialog's own number-static widget as the live magi count
	-- -- with no dialog, there is nothing to increment. Omitted whole; the
	-- flag/timer/sound side effects below are untouched.

	slot2:SetFlag_LUA(0, true)
	slot2:SetTimerRestart(0)
	slot2:PlaySound_LUA("Chainball_Loop01.ogg")
end

SECRET_MAGI_OBJECT_WAIT = {
	ANIM_NAME = "Wait",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	NEVER_MOVE = TRUE,
	STOP_LOOP_SOUND = TRUE
}
SECRET_MAGI_OBJECT_FEVER_READY = {
	ANIM_NAME = "Fever_Up",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SECRET_MAGI_OBJECT_FEVER"
		}
	},
	SOUND_PLAY0 = {
		0.01,
		"Magi_Object_Fever_Start01.ogg"
	},
	SOUND_PLAY1 = {
		0.01,
		"Magi_Object_Fever_Start02_C.ogg"
	},
	SOUND_PLAY2 = {
		0.01,
		"Magi_Object_Fever_Start03_C.ogg"
	}
}
SECRET_MAGI_OBJECT_FEVER = {
	ANIM_NAME = "Wait",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = FALSE,
	SHOW = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	CUSTOM_STATE_DELETE_EFFECT_SET = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"SECRET_MAGI_OBJECT_FEVER_END",
			"CT_SECRET_MAGI_OBJECT_FEVER_END"
		}
	},
	CT_SECRET_MAGI_OBJECT_FEVER_END = {
		STATE_TIME_OVER = 12
	}
}
SECRET_MAGI_OBJECT_FEVER_END = {
	ANIM_NAME = "Wait",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	SHOW = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SECRET_MAGI_OBJECT_DYING"
		}
	},
	CUSTOM_STATE_DELETE_EFFECT_SET = TRUE
}
SECRET_MAGI_OBJECT_DAMAGE = {
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "Damage",
	ANIM_SPEED = 1.5,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	NEVER_MOVE = TRUE,
	SOUND_PLAY0 = {
		0.01,
		"Chainball_Damage01.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SECRET_MAGI_OBJECT_WAIT"
		}
	}
}
SECRET_MAGI_OBJECT_DYING = {
	DYING_SPEED = 1.6,
	ANIM_NAME = "Dying",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE,
	CUSTOM_STATE_DELETE_EFFECT_SET = TRUE
}

function SECRET_MAGI_OBJECT_DYING_STATE_START(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- same
	-- GetSimpleDLG/counter gap as SECRET_MAGI_OBJECT_START_END_FUNC above
	-- (decrement instead of increment). Omitted whole; the sound/flag side
	-- effects below are untouched.

	slot2:StopSound_LUA("Chainball_Loop01.ogg")
	slot2:SetFlag_LUA(0, false)
end

function SECRET_MAGI_OBJECT_COMMON_FUNC(slot0, slot1, slot2)
	if slot2:GetFlag_LUA(0) == true and slot2:GetTimerElapsedTime(0) > 15 then
		slot2:PlaySound_LUA("Chainball_Loop01.ogg")
		slot2:SetFlag_LUA(0, true)
		slot2:SetTimerRestart(0)
	end
end
