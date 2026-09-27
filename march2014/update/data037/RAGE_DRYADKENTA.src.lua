INIT_SYSTEM = {
	UNIT_HEIGHT = 350,
	UNIT_SCALE = 1.2,
	UNIT_WIDTH = 225,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"BURSTING_BLADE_flame.dds",
		"aisha_active_heavyPress_wave.dds",
		"Smoke.dds",
		"ColorBallFire.dds",
		"DRYAD_Fire01.dds",
		"GroundShockWave02.dds",
		"SDK_AIRWAVE_T01.dds",
		"elsword_active_finalStrike_particle.dds",
		"Arme_Critical.dds",
		"Arme_Damage_Center.dds",
		"Arme_Damage_Ring.dds",
		"Arme_Ring2.dds",
		"star2.dds",
		"VERITABLE_PANDEMONIUM_T02.dds",
		"waldo_trock_specialAttackB_cry_waveLine.dds",
		"SlashLight.dds",
		"Star.dds",
		"WhitePointSmall.dds"
	},
	READY_XMESH = {
		"RAGE_DRYADKENTA_Hair1.Y",
		"RAGE_DRYADKENTA_Hair2.Y",
		"RAGE_DRYADKENTA_Tail.Y",
		"Lire_Uplight01.Y",
		"IRON_ELTRION_In_Move_Out_Laser_B.Y"
	},
	READY_XSKIN_MESH = {
		"DummyAttackBox_50x50x50.x",
		"NUI_CUTTYSARK_Dying_Landing_Effect_Rock.X",
		"SA_AEM_ELEMENTAL_STORM_FIRE01.X",
		"Bat_StandUpAttack.X",
		"Lire_Triple_Property_Fire01.X",
		"aisha_SA_SCREWDRIVER_circle.X"
	},
	READY_SOUND = {
		"Kenta_ArrowFire1.ogg",
		"Kenta_AttackA_Ready1.ogg",
		"Kenta_AttackB_Ready1.ogg",
		"Kenta_Impact1.ogg",
		"Kenta_Special_Ready1.ogg",
		"Kenta_Voice_AttackA01.ogg",
		"Kenta_Voice_AttackB01.ogg",
		"Kenta_Voice_AttackC01.ogg",
		"Kenta_Voice_DamageA01.ogg",
		"Kenta_Voice_DamageB01.ogg",
		"Kenta_Voice_Dying01.ogg",
		"Kenta_Voice_Start01.ogg",
		"Kenta_Voice_Wait01.ogg",
		"Kenta_Voice_Win01.ogg"
	}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_RAGE_DRYADKENTA.x"
}
INIT_PHYSIC = {
	RUN_SPEED = 850,
	RELOAD_ACCEL = 2000,
	DASH_JUMP_SPEED = 2300,
	WALK_SPEED = 450,
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
	BOSS_GAGE_FACE_TEX_PIECE = "NUI_DRYADKENTA",
	BOSS_GAGE_FACE_TEX = "DLG_BossState.tga",
	BOSS_NAME_TEX = "HQ_BOSS_NAME_DRYADKENTA.dds",
	HEAD_BONE_NAME = "Bip02_Head",
	SHADOW_SIZE = 200,
	SMALL_HP_BAR_YELLOW = "Small_HP_bar_Yellow.TGA",
	MP_CHARGE_RATE = 130,
	MAX_HYPER_MODE_TIME = 30,
	EXCLAMATION_MARK_SEQ = "ExclamationMarkNPC",
	DIE_FLY = 0,
	QUESTION_MARK_SEQ = "QuestionMarkNPC",
	USE_SLASH_TRACE = FALSE,
	HITTED_TYPE = HITTED_TYPE.HTD_MEAT,
	FALL_DOWN = TRUE,
	IMMUNITY_TIME_STOP = TRUE
}
INIT_STATE = {
	{
		STATE_NAME = "RAGE_DRYADKENTA_START",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_START_STATE_START"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_WAIT"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_WALK"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DASH"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DASH_END"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_START",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_STATE_START"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_STATE_START",
		STATE_COOL_TIME = 14.5
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_END"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_READY",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_READY_STATE_START"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_A_ATTACK_FIRE"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_SPECIAL_ATTACK_READY",
		STATE_COOL_TIME = 20
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE",
		LUA_FRAME_MOVE_FUNC = "RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_FRAME_MOVE"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_EMOTION",
		LUA_FRAME_MOVE_FUNC = "RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_EMOTION_FRAME_MOVE"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_B_READY_START",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_B_READY_STATE_START"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_B_READY",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_B_READY_STATE_START",
		STATE_COOL_TIME = 14.5
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_B_WAIT",
		LUA_FRAME_MOVE_FUNC = "RAGE_DRYADKENTA_ATTACK_B_WAIT_FRAME_MOVE",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_B_WAIT_STATE_START"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_ATTACK_B",
		LUA_STATE_START_FUNC = "RAGE_DRYADKENTA_ATTACK_B_STATE_START"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_JUMP_UP"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_JUMP_DOWN"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_JUMP_UP_DIR"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_JUMP_DOWN_DIR"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_JUMP_DOWN_LANDING"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_FRONT"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_DOWN_FRONT"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_STANDUP_FRONT"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_BACK"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_DOWN_BACK"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_STANDUP_BACK"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR_DOWN"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR_DOWN_LANDING"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR_FLY_FRONT"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR_FLY_BACK"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR_UP"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DAMAGE_AIR_FALL"
	},
	{
		STATE_NAME = "RAGE_DRYADKENTA_DYING"
	},
	START_STATE = "RAGE_DRYADKENTA_START",
	BIG_DAMAGE_LAND_BACK = "RAGE_DRYADKENTA_DAMAGE_BACK",
	SMALL_DAMAGE_AIR = "RAGE_DRYADKENTA_DAMAGE_AIR_FLY_FRONT",
	DAMAGE_REVENGE = "",
	DOWN_DAMAGE_AIR_LANDING = "RAGE_DRYADKENTA_DAMAGE_AIR_DOWN_LANDING",
	DOWN_DAMAGE_LAND_FRONT = "RAGE_DRYADKENTA_DAMAGE_DOWN_FRONT",
	DYING_SKY = "RAGE_DRYADKENTA_DYING",
	DOWN_DAMAGE_AIR = "RAGE_DRYADKENTA_DAMAGE_AIR_DOWN",
	DOWN_DAMAGE_LAND_BACK = "RAGE_DRYADKENTA_DAMAGE_DOWN_BACK",
	SMALL_DAMAGE_LAND_FRONT = "RAGE_DRYADKENTA_DAMAGE_FRONT",
	FLY_DAMAGE_BACK = "RAGE_DRYADKENTA_DAMAGE_AIR_UP",
	FLY_DAMAGE_FRONT = "RAGE_DRYADKENTA_DAMAGE_AIR_UP",
	WAIT_STATE = "RAGE_DRYADKENTA_WAIT",
	BIG_DAMAGE_LAND_FRONT = "RAGE_DRYADKENTA_DAMAGE_FRONT",
	SMALL_DAMAGE_LAND_BACK = "RAGE_DRYADKENTA_DAMAGE_BACK",
	BIG_DAMAGE_AIR = "RAGE_DRYADKENTA_DAMAGE_AIR_FLY_BACK",
	DYING_LAND_BACK = "RAGE_DRYADKENTA_DYING",
	UP_DAMAGE = "RAGE_DRYADKENTA_DAMAGE_AIR_UP",
	COMMON_FRAME_FUNC = "RAGE_DRYADKENTA_COMMON_FRAMEMOVE",
	DYING_LAND_FRONT = "RAGE_DRYADKENTA_DYING",
	DAMAGE_EXTRA_STATES = {
		"RAGE_DRYADKENTA_DAMAGE_FRONT",
		"RAGE_DRYADKENTA_DAMAGE_DOWN_FRONT",
		"RAGE_DRYADKENTA_DAMAGE_STANDUP_FRONT",
		"RAGE_DRYADKENTA_DAMAGE_BACK",
		"RAGE_DRYADKENTA_DAMAGE_DOWN_BACK",
		"RAGE_DRYADKENTA_DAMAGE_STANDUP_BACK",
		"RAGE_DRYADKENTA_DAMAGE_AIR",
		"RAGE_DRYADKENTA_DAMAGE_AIR_DOWN",
		"RAGE_DRYADKENTA_DAMAGE_AIR_DOWN_LANDING",
		"RAGE_DRYADKENTA_DAMAGE_AIR_FLY_FRONT",
		"RAGE_DRYADKENTA_DAMAGE_AIR_FLY_BACK",
		"RAGE_DRYADKENTA_DAMAGE_AIR_UP",
		"RAGE_DRYADKENTA_DAMAGE_AIR_FALL"
	}
}
INIT_AI = {
	TARGET = {
		TARGET_NEAR_RANGE = 1000,
		TARGET_LOST_RANGE = 9999,
		TARGET_SUCCESS_RATE = 100,
		PRESERVE_LAST_TARGET_RATE = 100,
		TARGET_INTERVAL = 2,
		TARGET_RANGE = 9999,
		ATTACK_TARGET_RATE = 100,
		TARGET_PRIORITY = TARGET_PRIORITY.TP_LOW_HP_FIRST
	},
	CHASE_MOVE = {
		UP_JUMP_RATE = 0,
		DOWN_DOWN_RATE = 0,
		WALK_INTERVAL = 1,
		JUMP_INTERVAL = 1000,
		UP_DOWN_RATE = 0,
		MOVE_GAP = 160,
		DOWN_JUMP_RATE = 0,
		DEST_GAP = 500,
		DIR_CHANGE_INTERVAL = 2,
		FAR_WALK_RATE = 100,
		MOVE_SPLIT_RANGE = 400,
		NEAR_WALK_RATE = 100
	},
	PATROL_MOVE = {
		PATROL_COOL_TIME = 2,
		PATROL_RANGE = 300,
		PATROL_BEGIN_RATE = 100,
		ONLY_THIS_LINE_GROUP = TRUE
	},
	ESCAPE_MOVE = {
		UP_DOWN_RATE = 0,
		DOWN_DOWN_RATE = 0,
		UP_JUMP_RATE = 0,
		JUMP_INTERVAL = 1000,
		DOWN_JUMP_RATE = 0,
		FAR_WALK_RATE = 100,
		ESCAPE_GAP = 600,
		WALK_INTERVAL = 2,
		MOVE_SPLIT_RANGE = 400,
		NEAR_WALK_RATE = 100
	}
}
RAGE_DRYADKENTA_START = {
	ANIM_NAME = "WaitStart",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	SOUND_PLAY0 = {
		0.035,
		"Kenta_Voice_Start01.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_START",
			"CT_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_START"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_ATTACK_B_READY_START",
			"CT_RAGE_DRYADKENTA_ATTACK_B_READY_START"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_Wait",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	},
	CT_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_START = {
		RATE = 50,
		ANIM_EVENT_TIMER = 3.1
	},
	CT_RAGE_DRYADKENTA_ATTACK_B_READY_START = {
		RATE = 100,
		ANIM_EVENT_TIMER = 3.1
	}
}
RAGE_DRYADKENTA_WAIT = {
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "Wait",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = TRUE,
	SOUND_PLAY0 = {
		0.035,
		"Kenta_Voice_Wait01.ogg",
		35
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_FALSE_DOWN,
			"RAGE_DRYADKENTA_JUMP_DOWN"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH",
			"CT_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_ATTACK_B_READY",
			"CT_RAGE_DRYADKENTA_ATTACK_B_READY"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_SPECIAL_ATTACK_READY",
			"CT_RAGE_DRYADKENTA_SPECIAL_ATTACK_READY"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_WALK,
			"RAGE_DRYADKENTA_WALK"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_DASH,
			"RAGE_DRYADKENTA_DASH"
		}
	},
	CT_RAGE_DRYADKENTA_SPECIAL_ATTACK_READY = {
		RATE = 100,
		DISTANCE_TO_TARGET_NEAR = 1500,
		MY_HP_LESS_THAN_PERCENT = 50
	},
	CT_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH = {
		RATE = 50,
		DISTANCE_TO_TARGET_NEAR = 1500
	},
	CT_RAGE_DRYADKENTA_ATTACK_B_READY = {
		RATE = 100,
		DISTANCE_TO_TARGET_NEAR = 1500
	}
}
RAGE_DRYADKENTA_WALK = {
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "Walk",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_FALSE_DOWN,
			"RAGE_DRYADKENTA_JUMP_DOWN"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH",
			"CT_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_ATTACK_B_READY",
			"CT_RAGE_DRYADKENTA_ATTACK_B_READY"
		},
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"RAGE_DRYADKENTA_SPECIAL_ATTACK_READY",
			"CT_RAGE_DRYADKENTA_SPECIAL_ATTACK_READY"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_WAIT,
			"RAGE_DRYADKENTA_WAIT"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_DASH,
			"RAGE_DRYADKENTA_DASH"
		}
	},
	CT_RAGE_DRYADKENTA_SPECIAL_ATTACK_READY = {
		RATE = 100,
		DISTANCE_TO_TARGET_NEAR = 1500,
		MY_HP_LESS_THAN_PERCENT = 50
	},
	CT_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH = {
		RATE = 50,
		DISTANCE_TO_TARGET_NEAR = 1500
	},
	CT_RAGE_DRYADKENTA_ATTACK_B_READY = {
		RATE = 100,
		DISTANCE_TO_TARGET_NEAR = 1500
	}
}
RAGE_DRYADKENTA_DASH = {
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "Dash",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_FALSE_DOWN,
			"RAGE_DRYADKENTA_JUMP_DOWN_DIR"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_WAIT,
			"RAGE_DRYADKENTA_DASH_END"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_WALK,
			"RAGE_DRYADKENTA_WALK"
		}
	}
}
RAGE_DRYADKENTA_DASH_END = {
	PASSIVE_SPEED_X = 0,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DashEnd",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_FALSE_DOWN,
			"RAGE_DRYADKENTA_JUMP_DOWN_DIR"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_WAIT,
			"RAGE_DRYADKENTA_WAIT"
		},
		{
			STATE_CHANGE_TYPE.SCT_AI_WALK,
			"RAGE_DRYADKENTA_WALK"
		}
	}
}

function RAGE_DRYADKENTA_START_STATE_START(slot0, slot1, slot2)
	slot2:SetInt_LUA(0, 2.5)
	slot2:SetVector_LUA(0, D3DXVECTOR3(1, 2, 3))
	slot2:SetVector_LUA(1, D3DXVECTOR3(4, 5, 6))
	slot2:SetFlag_LUA(1, false)
	slot2:SetFlag_LUA(2, true)
end

function RAGE_DRYADKENTA_ATTACK_B_READY_STATE_START(slot0, slot1, slot2)
	slot3 = 1

	if slot1:GetLineMap() ~= nil and slot1:GetLineMap():GetStartPosition(slot2:GetVector_LUA(0).x).y == slot2:GetLandPosition_LUA().y then
		slot3 = 2
	end

	slot2:SetInt_LUA(1, slot3)
end

function RAGE_DRYADKENTA_ATTACK_B_WAIT_STATE_START(slot0, slot1, slot2)
	slot2:SetShowGage(false)
	slot2:SetShow(false)
	slot2:SetPositionForce(D3DXVECTOR3(0, -5000, 0))
	slot2:SetTimerRestart(0)
	slot2:SetFlag_LUA(0, true)
	slot2:SetVector_LUA(4, slot2:GetPos())
end

function RAGE_DRYADKENTA_ATTACK_B_WAIT_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:GetFlag_LUA(0) == false then
		return
	end

	if slot2:GetInt_LUA(0) - 2 < slot2:GetTimerElapsedTime(0) then
		slot4 = slot2:GetVector_LUA(4)

		if slot1:GetLineMap() ~= nil then
			slot5 = slot2:GetVector_LUA(0)

			if slot2:GetInt_LUA(1) == 2 then
				slot5 = slot2:GetVector_LUA(1)
			end

			-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): elstools' decompile inlined
			-- this call straight into the first branch's condition and dropped
			-- the assignment, leaving the second branch read an undeclared
			-- slot6 (always false -- the z-axis mark position was dead code).
			-- Confirmed against the real June2017 bytecode via
			-- june2017_re/ljd_jun2017.py (this function's instr 44-46: TGETS +
			-- CALL + MODVN into register 6, read back unmodified by both branches
			-- below).
			slot6 = slot2:GetRandVal() % 3
			slot7 = slot5.x

			if slot6 == 1 then
				slot7 = slot5.y
			elseif slot6 == 2 then
				slot7 = slot5.z
			end

			slot4 = slot1:GetLineMap():GetStartPosition(slot7)
		else
			-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): same class of bug -- the
			-- decompile read `slot5:GetPos()` here before this branch ever
			-- assigns slot5 (it only holds a value inside the `GetLineMap() ~=
			-- nil` branch above). Bytecode (instr 64-80) calls
			-- GetNearestUserUnit_LUA once into register 5 and reuses it for both
			-- GetPos() and GetLandPosition_LUA(). Restructured as an explicit
			-- else/if since Lua's elseif can't hold a statement ahead of its own
			-- condition.
			slot5 = slot1:GetNearestUserUnit_LUA(slot2:GetPos())

			if slot5 ~= nil then
				slot4 = slot5:GetPos()
				slot4.y = slot5:GetLandPosition_LUA().y
			end
		end

		-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): this is the actual "jumps out
		-- of the map and never comes back" softlock. The decompile printed
		-- `if slot1:GetEffectSet() ~= nil then slot5:PlayEffectSetWithCustomPos_LUA(...)`,
		-- reusing slot5's stale value from the branches above (a position
		-- vector, not an effect set) instead of the call result -- bytecode
		-- (instr 82-84) stores GetEffectSet() into register 5 and checks THAT
		-- register; it never calls GetEffectSet() a second time. Calling
		-- PlayEffectSetWithCustomPos_LUA on a vector errors and aborts this
		-- function before slot2:SetVector_LUA(4, slot4) below ever runs, so
		-- slot4 (register 4) keeps whatever ATTACK_B_WAIT_STATE_START seeded it
		-- with: slot2:GetPos() taken right after SetPositionForce(0, -5000, 0),
		-- i.e. the off-map mark position itself. ATTACK_B_STATE_START then calls
		-- SetPositionForce(slot2:GetVector_LUA(4)) and puts Kenta right back at
		-- (0, -5000, 0) -- she never returns from the Attack B "jump".
		slot5 = slot1:GetEffectSet()

		if slot5 ~= nil then
			slot5:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_ATTACK_B_Mark", slot2, slot4, slot2:GetRotateDegree())
		end

		slot2:SetVector_LUA(4, slot4)
		slot2:SetFlag_LUA(0, false)
	end
end

function CF_RAGE_DRYADKENTA_ATTACK_B(slot0, slot1, slot2)
	if slot2:GetInt_LUA(0) < slot2:GetTimerElapsedTime(0) then
		return true
	end

	return false
end

function RAGE_DRYADKENTA_ATTACK_B_STATE_START(slot0, slot1, slot2)
	slot2:SetShowGage(true)
	slot2:SetShow(true)
	slot2:SetPositionForce(slot2:GetVector_LUA(4))
end

RAGE_DRYADKENTA_ATTACK_B_READY = {
	ANIM_NAME = "AttackB_Ready",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = TRUE,
	SUPER_ARMOR = TRUE,
	NO_FALL_DOWN_AT_STATE = TRUE,
	SOUND_PLAY0 = {
		0.34,
		"Kenta_AttackB_Ready1.ogg"
	},
	SOUND_PLAY1 = {
		0.001,
		"Kenta_Voice_AttackB01.ogg",
		35
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_ATTACK_B_WAIT"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_AttackB_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT
		}
	}
}
RAGE_DRYADKENTA_ATTACK_B_WAIT = {
	ANIM_NAME = "AttackB_Fire",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	NO_FALL_DOWN_AT_STATE = TRUE,
	INVINCIBLE = {
		0,
		99
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"RAGE_DRYADKENTA_ATTACK_B",
			"CF_RAGE_DRYADKENTA_ATTACK_B"
		}
	}
}
RAGE_DRYADKENTA_ATTACK_B = {
	ANIM_NAME = "AttackB_Fire",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	NO_FALL_DOWN_AT_STATE = TRUE,
	SUPER_ARMOR_TIME0 = {
		0,
		0.312
	},
	SOUND_PLAY0 = {
		0.035,
		"Kenta_Impact1.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_AttackB_Fire",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT
		}
	}
}
RAGE_DRYADKENTA_ATTACK_B_READY_START = {
	ANIM_NAME = "AttackB_Ready",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = TRUE,
	VIEW_TARGET = TRUE,
	SUPER_ARMOR = TRUE,
	NO_FALL_DOWN_AT_STATE = TRUE,
	SOUND_PLAY0 = {
		0.34,
		"Kenta_AttackB_Ready1.ogg"
	},
	SOUND_PLAY1 = {
		0.001,
		"Kenta_Voice_AttackB01.ogg",
		35
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_ATTACK_B_WAIT"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_AttackB_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT
		}
	}
}
RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH = {
	ANIM_NAME = "Dash",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED,
	ALLOW_DIR_CHANGE = FALSE,
	VIEW_TARGET = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_END",
			"CF_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH"
		}
	}
}
RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_START = {
	ANIM_NAME = "Dash",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED,
	ALLOW_DIR_CHANGE = FALSE,
	VIEW_TARGET = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_END",
			"CF_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH"
		}
	}
}

function CF_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH(slot0, slot1, slot2)
	if slot1:GetDist_LUA(slot2:GetPos(), slot2:GetVector_LUA(2)) < 200 then
		return true
	end

	return false
end

-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): the same dropped-assignment bug
-- appears in this function's very first line too, pre-existing since Phase 4
-- -- `slot2:GetLastTouchLineIndex()` was never stored, leaving `slot3 == 1`
-- etc. below it read an undeclared global instead of the same call's result.
-- Confirmed against the real June2017 bytecode via june2017_re/ljd_jun2017.py
-- (this function's instr 1-3: GetLastTouchLineIndex() stored to register 3
-- before any of the four comparisons run).
function RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_STATE_START(slot0, slot1, slot2)
	slot3 = slot2:GetLastTouchLineIndex()

	if slot3 == 0 or slot3 == 1 or slot3 == 4 or slot3 == 5 then
		slot2:StateChange_LUA("RAGE_DRYADKENTA_WAIT", true)
	end

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): June2017's fallback to
	-- slot2:GetStoredGUUser() when there's no current target is omitted --
	-- that method isn't bound in this March2014 client (checked X2Lib/KTDXLIB,
	-- not found; a missing method call aborts the whole script, unlike a
	-- missing enum member). slot4 simply stays nil in the no-target case, so
	-- the reorientation block below (slot4 ~= nil) is skipped instead of
	-- falling back to a stale stored target -- same as every other state here
	-- that just checks GetTargetUser() directly.
	--
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): the omission above accidentally
	-- also dropped the *primary* assignment -- this used to be
	-- `if slot2:GetTargetUser() == nil then end`, an empty-bodied statement
	-- that called GetTargetUser() and threw the result away, leaving slot4
	-- undefined even when a target exists. That fed straight into
	-- slot2:SetVector_LUA(2, slot5) below with slot5 *also* never assigned
	-- (same decompiler bug: the call was inlined into the `if` condition
	-- instead of stored) -- SetVector_LUA(2, nil) is this dash's target
	-- position, and CF_RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH only ends the dash
	-- once she's within 200 of it, so a nil target let her dash indefinitely
	-- until she ran off the map. This state is forced at 60%/20% HP by
	-- RAGE_DRYADKENTA_COMMON_FRAMEMOVE, so it's the more common of the two
	-- "jump out of the map" routes in this file. Confirmed against the real
	-- June2017 bytecode via june2017_re/ljd_jun2017.py (this function's instr
	-- 17-19 assign slot4 = GetTargetUser(); instr 40-42 assign
	-- slot5 = GetLineGroupStartPos(), both unconditionally, before either is
	-- read).
	slot4 = slot2:GetTargetUser()

	if slot4 ~= nil then
		slot2:SetIsRight(not slot2:IsRightTarget_LUA(slot2, slot4:GetPos()))
	end

	slot5 = slot2:GetLineGroupStartPos()

	if slot2:IsRightTarget_LUA(slot2, slot5) == slot2:GetIsRight() then
		slot2:SetVector_LUA(2, slot5)
	else
		slot2:SetVector_LUA(2, slot2:GetLineGroupEndPos())
	end
end

RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_END = {
	ANIM_NAME = "DashEnd",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_READY"
		}
	}
}
RAGE_DRYADKENTA_ATTACK_A_ATTACK_READY = {
	ANIM_NAME = "SpecialAttackA_Ready",
	ANIM_SPEED = 0.75,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.035,
		"Kenta_AttackA_Ready1.ogg"
	},
	SOUND_PLAY1 = {
		0.001,
		"Kenta_Voice_AttackA01.ogg",
		30
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_ATTACK_A_ATTACK_FIRE"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_SpecialAttackA_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT
		}
	}
}

function RAGE_DRYADKENTA_ATTACK_A_ATTACK_READY_STATE_START(slot0, slot1, slot2)
	slot2:SetIsRight(not slot2:GetIsRight())
end

RAGE_DRYADKENTA_ATTACK_A_ATTACK_FIRE = {
	ANIM_NAME = "SpacialAttackA_Fire",
	ANIM_SPEED = 1,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.001,
		"Kenta_ArrowFire1.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackA_Fire",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT
		}
	}
}
RAGE_DRYADKENTA_SPECIAL_ATTACK_READY = {
	ANIM_SPEED = 0.75,
	ANIM_NAME = "AttackA_Ready",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	VIEW_TARGET = TRUE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.001,
		"Kenta_Special_Ready1.ogg"
	},
	SOUND_PLAY1 = {
		0.1,
		"Kenta_Voice_AttackC01.ogg"
	},
	ALARM = {
		DELAY = 0,
		DISAPPEAR_TIME = 7.5,
		DANGER = TRUE,
		ALARM_MESSAGE = STR_ID_32368,
		REPEAT = TRUE,
		ALARM_COLOR_TYPE = ALARM_COLOR_TYPE.ACT_ORANGE
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYAD_Attack_A_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT
		}
	}
}
RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE = {
	ANIM_NAME = "AttackA_Fire",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.001,
		"Kenta_ArrowFire1.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_EMOTION"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_NUI_RAGE_DRYADKENTA_AttackA_Fire",
			0
		}
	}
}

-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): every `slot3:GetStartPosition(...)`
-- call in this function reads slot3 before it's ever assigned -- the
-- decompile inlined `slot1:GetLineMap()` straight into each `if ... ~= nil`
-- condition instead of storing it first, same class of bug as
-- ATTACK_B_WAIT_FRAME_MOVE. Since slot3 is a bare global (this file never
-- declares `local`), it held whatever the last function to touch slot3 left
-- behind -- not a hard crash location-for-location, but never the line map
-- either, so these effect calls errored instead of playing. Confirmed against
-- the real June2017 bytecode via june2017_re/ljd_jun2017.py (this function's
-- instr 8-9 and 38-39: GetLineMap() stored to register 3 before GetEffectSet()
-- is even called).
function RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:AnimEventTimer_LUA(1.2) then
		slot3 = slot1:GetLineMap()
		slot4 = slot1:GetEffectSet()

		if slot3 ~= nil and slot4 ~= nil then
			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(8), slot2:GetRotateDegree())
		end
	end

	if slot2:AnimEventTimer_LUA(1.35) then
		slot3 = slot1:GetLineMap()
		slot4 = slot1:GetEffectSet()

		if slot3 ~= nil and slot4 ~= nil then
			slot5 = slot2:GetRotateDegree()

			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(7), slot5)
			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(9), slot5)
		end
	end
end

RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_EMOTION = {
	ANIM_NAME = "AttackA_Combo",
	ANIM_SPEED = 0.75,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	}
}

-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): same slot3-never-assigned bug as
-- RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_FRAME_MOVE just above -- confirmed
-- against the real June2017 bytecode via june2017_re/ljd_jun2017.py (this
-- function's instr 8-9 and 50-51: GetLineMap() stored to register 3 before
-- GetEffectSet() is even called).
function RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_EMOTION_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:AnimEventTimer_LUA(0.3) then
		slot3 = slot1:GetLineMap()
		slot4 = slot1:GetEffectSet()

		if slot3 ~= nil and slot4 ~= nil then
			slot5 = slot2:GetRotateDegree()

			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(11), slot5)
			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(12), slot5)
		end
	end

	if slot2:AnimEventTimer_LUA(1.2) then
		slot3 = slot1:GetLineMap()
		slot4 = slot1:GetEffectSet()

		if slot3 ~= nil and slot4 ~= nil then
			slot5 = slot2:GetRotateDegree()

			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(10), slot5)
			slot4:PlayEffectSetWithCustomPos_LUA("EffectSet_NUI_RAGE_DRYADKENTA_SpacialAttackB_Fire", slot2, slot3:GetStartPosition(13), slot5)
		end
	end
end

RAGE_DRYADKENTA_JUMP_UP = {
	ANIM_NAME = "JumpUp",
	ADD_POS_Y = 45,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SPEED_Y = INIT_PHYSIC.JUMP_SPEED,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_NEGATIVE_Y_SPEED,
			"RAGE_DRYADKENTA_JUMP_DOWN"
		}
	}
}
RAGE_DRYADKENTA_JUMP_UP_DIR = {
	ANIM_NAME = "JumpUp",
	ADD_POS_Y = 45,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED,
	SPEED_Y = INIT_PHYSIC.JUMP_SPEED,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_NEGATIVE_Y_SPEED,
			"RAGE_DRYADKENTA_JUMP_DOWN_DIR"
		}
	}
}
RAGE_DRYADKENTA_JUMP_DOWN = {
	ANIM_NAME = "JumpDown",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_TRUE,
			"RAGE_DRYADKENTA_JUMP_DOWN_LANDING"
		}
	}
}
RAGE_DRYADKENTA_JUMP_DOWN_DIR = {
	ANIM_NAME = "JumpDown",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_TRUE,
			"RAGE_DRYADKENTA_JUMP_DOWN_LANDING"
		}
	}
}
RAGE_DRYADKENTA_JUMP_DOWN_LANDING = {
	ANIM_NAME = "JumpDownLanding",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_FRONT = {
	ANIM_NAME = "DamageFront",
	EVENT_INTERVAL_TIME0 = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SOUND_PLAY0 = {
		0.001,
		"Kenta_Voice_DamageA01.ogg",
		30
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_DOWN_FRONT = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageDownFront",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	DEFENCE = {
		0,
		100,
		70
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_STANDUP_FRONT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_STANDUP_FRONT = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageStandUpFront",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	DEFENCE = {
		0,
		100,
		70
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_FALSE_DOWN,
			"RAGE_DRYADKENTA_JUMP_DOWN"
		},
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_BACK = {
	ANIM_NAME = "DamageBack",
	EVENT_INTERVAL_TIME0 = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SOUND_PLAY0 = {
		0.001,
		"Kenta_Voice_DamageB01.ogg",
		30
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_DOWN_BACK = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageDownBack",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	DEFENCE = {
		0,
		100,
		70
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_STANDUP_BACK"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_STANDUP_BACK = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageStandUpBack",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	DEFENCE = {
		0,
		100,
		70
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_FALSE_DOWN,
			"RAGE_DRYADKENTA_JUMP_DOWN"
		},
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_WAIT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR = {
	ANIM_NAME = "DamageAir",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_AIR_DOWN"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR_DOWN = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageAirDown",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_TRUE,
			"RAGE_DRYADKENTA_DAMAGE_AIR_DOWN_LANDING"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR_DOWN_LANDING = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageAirDownLanding",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	DEFENCE = {
		0,
		100,
		70
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_STANDUP_FRONT"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR_FLY_FRONT = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageAirFlyFront",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_AIR_FALL"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR_FLY_BACK = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageAirFlyBack",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_AIR_FALL"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR_UP = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageAirUp",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"RAGE_DRYADKENTA_DAMAGE_AIR_FALL"
		}
	}
}
RAGE_DRYADKENTA_DAMAGE_AIR_FALL = {
	ANIM_SPEED = 1.25,
	EVENT_INTERVAL_TIME0 = 0,
	ANIM_NAME = "DamageAirFall",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_FOOT_ON_LINE_TRUE,
			"RAGE_DRYADKENTA_DAMAGE_AIR_DOWN_LANDING"
		}
	}
}
RAGE_DRYADKENTA_DYING = {
	ANIM_NAME = "DamageAirDownLanding",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = TRUE,
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE,
	SOUND_PLAY0 = {
		0.001,
		"Kenta_Voice_Dying01.ogg"
	}
}

function RAGE_DRYADKENTA_COMMON_FRAMEMOVE(slot0, slot1, slot2)
	if slot2:GetNowStateID() == slot2:GetStateID_LUA("RAGE_DRYADKENTA_WAIT") or slot2:GetNowStateID() == slot2:GetStateID_LUA("RAGE_DRYADKENTA_DAMAGE_FRONT") or slot2:GetNowStateID() == slot2:GetStateID_LUA("RAGE_DRYADKENTA_DAMAGE_BACK") then
		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): both slot2:InitStateCoolTime(...)
		-- calls below are omitted -- that method isn't bound in this March2014
		-- client (checked X2Lib/KTDXLIB, not found; a missing method call
		-- aborts the whole script, unlike a missing enum member). The
		-- StateChange_LUA/SetFlag_LUA calls that follow are kept, so the
		-- dash-attack phase transition still triggers at the same HP/cooldown
		-- thresholds -- only whatever cooldown-priming InitStateCoolTime did
		-- for the target state is missing.
		--
		-- SERV_IRUHADEV_RANOX_PORT (2026-09-27): the HP-ratio division below
		-- was never stored to slot3 in the decompile -- it was inlined straight
		-- into this first condition's text, leaving the elseif's `slot3 <= 0.2`
		-- read an undeclared global. `nil <= 0.2` raises a Lua error (unlike
		-- `==`, comparisons aren't nil-safe), so once the 60% phase has fired
		-- once (flag 1 true) this errored on every WAIT/DAMAGE_FRONT/DAMAGE_BACK
		-- frame and the 20% phase never triggered. Confirmed against the real
		-- June2017 bytecode via june2017_re/ljd_jun2017.py (this function's
		-- instr 28-34: GetNowHP()/GetMaxHP() stored to register 3, still live
		-- and unmodified at the elseif's own comparison).
		slot3 = slot2:GetNowHP() / slot2:GetMaxHP()

		if slot2:GetFlag_LUA(1) == false and slot3 <= 0.6 then
			slot2:StateChange_LUA("RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH", true)
			slot2:SetFlag_LUA(1, true)
			slot2:SetFlag_LUA(2, false)
		elseif slot2:GetFlag_LUA(2) == false and slot3 <= 0.2 then
			slot2:StateChange_LUA("RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH", true)
			slot2:SetFlag_LUA(2, true)
		end
	end
end

function MovePos(slot0, slot1, slot2)
	slot0.x = slot0.x + slot2 * slot1.x
	slot0.y = slot0.y + slot2 * slot1.y
	slot0.z = slot0.z + slot2 * slot1.z

	return slot0
end
