INIT_SYSTEM = {
	UNIT_HEIGHT = 225,
	UNIT_SCALE = 1.1,
	UNIT_WIDTH = 110,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"RingAlpha.dds",
		"Colorballgray.dds",
		"Fire_Flower01.dds",
		"WhitePoint.dds",
		"BURSTING_BLADE_flame.dds",
		"Arme_Critical.dds",
		"Arme_Damage_Center.dds",
		"Arme_Damage_Ring.dds",
		"Arme_Ring2.dds",
		"star2.dds"
	},
	READY_XMESH = {
		"Up_Light01_Mesh.Y"
	},
	READY_XSKIN_MESH = {
		"Lire_SI_SA_Gungnir_Mesh05.X",
		"elsword_SA_rageCutter_spwanPoint.X",
		"BH_Combo_XZ_Fire_DUMMY01.X",
		"SA_BLAZE_STEP_MEMO_FIRE.X",
		"HummingWind_AttackBox.X"
	},
	READY_SOUND = {
		"Ignia_Appear01.ogg",
		"Berserker_attack01.ogg",
		"Ignia_Disappear01.ogg",
		"Ignia_FireOra01.ogg",
		"Ignia_Meteor01.ogg",
		"Ignia_Specialattack_ready01.ogg",
		"Ignia_Spin01.ogg",
		"Ignia_Spin02.ogg",
		"Ignia_Swing01.ogg",
		"Ignia_Swing02.ogg",
		"Ignia_Swing03.ogg",
		"Ignia_Voice_AttackA01.ogg",
		"Ignia_Voice_AttackA02.ogg",
		"Ignia_Voice_AttackA03.ogg",
		"Ignia_Voice_AttackA04.ogg",
		"Ignia_Voice_AttackB01.ogg",
		"Ignia_Voice_AttackB02.ogg",
		"Ignia_Voice_Damage01.ogg",
		"Ignia_Voice_Damage02.ogg",
		"Ignia_Voice_Damage03.ogg",
		"Ignia_Voice_Damage04.ogg",
		"Ignia_Voice_Dying01.ogg"
	}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_NPC_FIRE_DEVIL_IGNIA.x"
}
INIT_PHYSIC = {
	RUN_SPEED = 800,
	RELOAD_ACCEL = 2000,
	DASH_JUMP_SPEED = 2300,
	WALK_SPEED = 400,
	G_ACCEL = 4000,
	MAX_G_SPEED = -2000,
	JUMP_SPEED = 1500
}
INIT_COMPONENT = {
	SHADOW_FILE_NAME = "shadow.dds",
	MP_CHANGE_RATE = 1,
	HYPER_MODE_COUNT = 0,
	SMALL_HP_BAR_RED = "Small_HP_bar_Red.TGA",
	MAX_HYPER_MODE_TIME = 30,
	SMALL_HP_BAR_BLUE = "Small_HP_bar_Blue.TGA",
	SHADOW_SIZE = 200,
	MP_CHARGE_RATE = 130,
	SMALL_HP_BAR_YELLOW = "Small_HP_bar_Yellow.TGA",
	USE_SLASH_TRACE = FALSE,
	HITTED_TYPE = HITTED_TYPE.HTD_MEAT,
	FALL_DOWN = FALSE,
	IMMUNITY_TIME_STOP = TRUE,
	DIE_FLY = FALSE,
	NOT_AFFECT_BLACKHOLE = TRUE,
	REMAINING_WHEN_SUB_STAGE_CLEAR = TRUE
}
INIT_STATE = {
	{
		STATE_NAME = "SAMETEAM_IGNIA_START",
		LUA_STATE_START_FUNC = "SAMETEAM_IGNIA_START_STATE_START"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_WAIT"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_EVENT_0"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_A_READY",
		STATE_COOL_TIME = 0
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_A_FIRE",
		LUA_STATE_END_FUNC = "ATTACK_STATE_END"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_B_READY",
		STATE_COOL_TIME = 0
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_B_FIRE",
		LUA_FRAME_MOVE_FUNC = "SAMETEAM_IGNIA_ATTACK_B_FIRE_FRAME_MOVE",
		LUA_STATE_END_FUNC = "ATTACK_STATE_END"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_C_READY",
		STATE_COOL_TIME = 0,
		LUA_STATE_END_FUNC = "SAMETEAM_IGNIA_ATTACK_C_READY_STATE_END"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_C_NOT_SHOW",
		LUA_FRAME_MOVE_FUNC = "SAMETEAM_IGNIA_ATTACK_C_NOT_SHOW_FRAME_MOVE"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_C_FIRE",
		LUA_STATE_END_FUNC = "ATTACK_STATE_END"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_D_READY",
		STATE_COOL_TIME = 0
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_ATTACK_D_FIRE",
		LUA_STATE_START_FUNC = "SAMETEAM_IGNIA_ATTACK_D_FIRE_STATE_START",
		LUA_STATE_END_FUNC = "ATTACK_STATE_END"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_SPECIAL_ATTACK_A_READY",
		STATE_COOL_TIME = 0
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_SPECIAL_ATTACK_A_START"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_SPECIAL_ATTACK_A_LOOP"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_SPECIAL_ATTACK_A_END",
		LUA_STATE_END_FUNC = "ATTACK_STATE_END"
	},
	{
		STATE_NAME = "SAMETEAM_IGNIA_DYING"
	},
	WAIT_STATE = "SAMETEAM_IGNIA_WAIT",
	DYING_SKY = "SAMETEAM_IGNIA_DYING",
	DYING_LAND_BACK = "SAMETEAM_IGNIA_DYING",
	COMMON_FRAME_FUNC = "SAMETEAM_IGNIA_COMMON_FRAME_MOVE",
	DYING_LAND_FRONT = "SAMETEAM_IGNIA_DYING",
	START_STATE = "SAMETEAM_IGNIA_START"
}
INIT_AI = {
	NO_BRAIN = TRUE
}

function SAMETEAM_IGNIA_START_STATE_START(slot0, slot1, slot2)
	slot2:SetTimerRestart(0)
	slot2:SetTimerRestart(1)
	slot2:SetTimerRestart(2)

	if slot1:GetNowStageIndex() == 1 then
		slot2:SetInt_LUA(1, 1)
		slot2:StateChange_LUA("SAMETEAM_IGNIA_ATTACK_D_READY", true)
	elseif slot1:GetNowStageIndex() == 2 then
		slot2:SetInt_LUA(2, 1)
	end
end

SAMETEAM_IGNIA_START = {
	PASSIVE_SPEED_X = 0,
	G_ACCEL = 0,
	ANIM_NAME = "Wait",
	DOWN_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	RIGHT = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	}
}
SAMETEAM_IGNIA_EVENT_0 = {
	G_ACCEL = 0,
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "Talk",
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	TALK_BOX = {
		{
			RATE = 100,
			MESSAGE = STR_ID_33679
		}
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	}
}
SAMETEAM_IGNIA_WAIT = {
	DOWN_ACCEL = 0,
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "Wait",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	RIGHT = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE
}
SAMETEAM_IGNIA_ATTACK_A_READY = {
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "AttackA_Ready",
	DOWN_ACCEL = 0,
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.2,
		"Ignia_Swing01.ogg"
	},
	SOUND_PLAY1 = {
		0.2,
		"Ignia_Voice_AttackA02.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_ATTACK_A_FIRE"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_FIRE_DEVIL_IGNIA_AttackA_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}
SAMETEAM_IGNIA_ATTACK_A_FIRE = {
	PASSIVE_SPEED_Y = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "AttackA_Fire",
	G_ACCEL = 0,
	ANIM_SPEED = 1.25,
	PASSIVE_SPEED_X = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.13,
		"Ignia_Swing02.ogg"
	},
	SOUND_PLAY1 = {
		0.75,
		"Ignia_Spin01.ogg"
	},
	SOUND_PLAY2 = {
		1.2,
		"Ignia_Swing02.ogg"
	},
	SOUND_PLAY3 = {
		0.13,
		"Ignia_Voice_AttackA04.ogg"
	},
	SOUND_PLAY4 = {
		1.1,
		"Ignia_Voice_AttackA03.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	},
	ATTACK_TIME0 = {
		0.155,
		0.239
	},
	ATTACK_TIME1 = {
		1.227,
		1.255
	},
	DAMAGE_DATA = {
		HIT_GAP = 0.5,
		DAMAGE_TYPE = DAMAGE_TYPE.DT_PHYSIC,
		HIT_TYPE = HIT_TYPE.HT_SWORD_HIT,
		REACT_TYPE = REACT_TYPE.RT_SMALL_DAMAGE,
		DAMAGE = {
			PHYSIC = 1
		},
		RE_ATTACK = TRUE
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_FIRE_DEVIL_IGNIA_AttackA_Fire",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}

function SAMETEAM_IGNIA_ATTACK_B_FIRE_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:AnimEventTimer_LUA(0.296) and slot1:GetEffectSet() ~= nil then
		slot4 = slot2:GetRotateDegree()

		if slot2:GetIsRight() == false then
			slot4.y = slot4.y - 180
		end

		slot3:PlayEffectSet_LUA("EffecSet_Same_IGNIA_Attack_B_Fire", slot2)
	end
end

SAMETEAM_IGNIA_ATTACK_B_READY = {
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "AttackB_Ready",
	DOWN_ACCEL = 0,
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.035,
		"Ignia_Meteor01.ogg"
	},
	SOUND_PLAY1 = {
		0.035,
		"Ignia_Voice_AttackB02.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_ATTACK_B_FIRE"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffecSet_IGNIA_Attack_B_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}
SAMETEAM_IGNIA_ATTACK_B_FIRE = {
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "AttackB_Fire",
	DOWN_ACCEL = 0,
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	}
}

function SAMETEAM_IGNIA_ATTACK_C_READY_STATE_END(slot0, slot1, slot2)
	slot2:SetTimerRestart(3)

	slot3 = D3DXVECTOR3(0.5, 0, 0)
	slot3.y = slot3.x - 0.3

	slot2:SetVector_LUA(0, slot3)
end

function SAMETEAM_IGNIA_ATTACK_C_NOT_SHOW_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:GetVector_LUA(0).y < slot2:GetTimerElapsedTime(3) then
		slot5 = slot2:GetPos()

		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 8 (GateWay) -- GetHostGameUnit
		-- isn't bound in this March2014 client (same gap Phase 5/7 already found;
		-- checked X2Lib/KTDXLIB, not found -- calling it would abort this file's
		-- load). Omitted; slot5 simply stays at slot2:GetPos() (already assigned
		-- above) instead of being repositioned relative to the room host.

		if slot1:GetEffectSet() ~= nil then
			slot6:PlayEffectSet_LUA("EffectSet_IGNIA_ATTACK_C_Fire", slot2)
		end

		slot3.y = 9999

		slot2:SetVector_LUA(0, slot3)
	elseif slot3.x < slot2:GetTimerElapsedTime(3) then
		slot2:StateChange_LUA("SAMETEAM_IGNIA_ATTACK_C_FIRE", true)

		slot3.x = 9999

		slot2:SetVector_LUA(0, slot3)
	end
end

SAMETEAM_IGNIA_ATTACK_C_READY = {
	ANIM_NAME = "AttackC_Ready",
	PASSIVE_SPEED_X = 0,
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	ANIM_SPEED = 1.25,
	DOWN_ACCEL = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SOUND_PLAY0 = {
		0.038,
		"Ignia_Disappear01.ogg"
	},
	SOUND_PLAY1 = {
		0.35,
		"Ignia_Voice_AttackA04.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_ATTACK_C_NOT_SHOW"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_IGNIA_ATTACK_C_Ready",
			0
		}
	}
}
SAMETEAM_IGNIA_ATTACK_C_NOT_SHOW = {
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "AttackC_Fire",
	G_ACCEL = 0,
	DOWN_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	NO_FALL_DOWN_AT_STATE = TRUE
}
SAMETEAM_IGNIA_ATTACK_C_FIRE = {
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "AttackC_Fire",
	DOWN_ACCEL = 0,
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	SHOW = TRUE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.001,
		"Ignia_Spin02.ogg"
	},
	SOUND_PLAY1 = {
		0.001,
		"Ignia_Voice_AttackA04.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	}
}

function SAMETEAM_IGNIA_ATTACK_D_FIRE_STATE_START(slot0, slot1, slot2)
	slot7 = ALARM_COLOR_TYPE.ACT_MAGENTA

	slot1:ShowDangerAlarm_LUA(0, 7.5, slot7, false, STR_ID_33671, true, TEAM_NUM.TN_RED)

	for slot7 = 0, slot1:GetUserUnitListSize() - 1 do
		if slot1:GetUserUnit(slot7) ~= nil then
			if slot2:GetInt_LUA(1) == 0 then
				slot8:SetBuffFactorToGameUnitByBuffFactorID_LUA(BUFF_FACTOR_ID.BFI_BUFF_IGNIA_HP_MP)
				slot8:SetBuffFactorToGameUnitByBuffFactorID_LUA(BUFF_FACTOR_ID.BFI_SET_SKILL_COOLTIME_ONCE_TO_MIN)
			else
				slot8:SetBuffFactorToGameUnitByBuffFactorID_LUA(BUFF_FACTOR_ID.BFI_BUFF_IGNIA_ATTACK_SUPERARMOR)
			end

			if slot1:GetEffectSet() ~= nil then
				slot9:PlayEffectSet_LUA("EffecSet_IGNIA_Attack_B_Ready", slot8)
			end
		end
	end

	slot2:SetInt_LUA(1, slot2:GetInt_LUA(1) + 1)
end

SAMETEAM_IGNIA_ATTACK_D_READY = {
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "AttackD_Ready",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.35,
		"Ignia_Spin02.ogg"
	},
	SOUND_PLAY1 = {
		0.001,
		"Ignia_Voice_AttackA01.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_ATTACK_D_FIRE"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_FIRE_DEVIL_IGNIA_AttackD_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}
SAMETEAM_IGNIA_ATTACK_D_FIRE = {
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "AttackD_Fire",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffectSet_FIRE_DEVIL_IGNIA_AttackD_Fire",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}
SAMETEAM_IGNIA_SPECIAL_ATTACK_A_READY = {
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "SpecialAttackA_Ready",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.001,
		"Ignia_Specialattack_ready01.ogg"
	},
	SOUND_PLAY1 = {
		0.2,
		"Ignia_Voice_AttackB01.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_SPECIAL_ATTACK_A_START"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffecSet_IGNIA_SpecialAttack_A_Ready",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}
SAMETEAM_IGNIA_SPECIAL_ATTACK_A_START = {
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "SpecialAttackA_FireStart",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	SOUND_PLAY0 = {
		0.01,
		"Ignia_Voice_AttackA04.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_SPECIAL_ATTACK_A_LOOP"
		}
	},
	EFFECT_SET_LIST_EX = {}
}
SAMETEAM_IGNIA_SPECIAL_ATTACK_A_LOOP = {
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "SpecialAttackA_FireLoop",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_SPECIAL_ATTACK_A_END"
		}
	},
	EFFECT_SET_LIST_EX = {
		{
			"EffecSet_SAME_IGNIA_SpecialAttack_A_Fire",
			0,
			DELETE_CONDITION_EFFECT_SET.DCES_DAMAGE_REACT,
			DELETE_CONDITION_EFFECT_SET.DCES_DIE
		}
	}
}
SAMETEAM_IGNIA_SPECIAL_ATTACK_A_END = {
	PASSIVE_SPEED_X = 0,
	DOWN_ACCEL = 0,
	ANIM_NAME = "SpecialAttackA_FireEnd",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SAMETEAM_IGNIA_WAIT"
		}
	}
}
SAMETEAM_IGNIA_DYING = {
	DOWN_ACCEL = 0,
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "Wait",
	G_ACCEL = 0,
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = TRUE,
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}

function ATTACK_STATE_END(slot0, slot1, slot2)
	slot2:SetFlag_LUA(0, false)
end

function SAMETEAM_IGNIA_COMMON_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:GetFlag_LUA(0) == true then
		return
	end

	if slot2:GetTimerElapsedTime(2) > 0.5 then
		if slot2:GetInt_LUA(1) == 2 then
			if slot1:GetNowSubStageIndex() == 2 then
				slot2:SetInt_LUA(1, 0)
				slot2:StateChange_LUA("SAMETEAM_IGNIA_ATTACK_D_READY", true)
				slot2:SetFlag_LUA(0, true)

				return
			end
		elseif slot2:GetInt_LUA(2) == 1 and slot1:GetNowSubStageIndex() == 2 then
			slot2:StateChange_LUA("SAMETEAM_IGNIA_EVENT_0", true)
			slot2:SetFlag_LUA(0, true)
			slot2:SetInt_LUA(2, 0)

			return
		end

		slot2:SetTimerRestart(2)
	end

	if slot2:GetTimerElapsedTime(0) > 2 then
		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): same GetHostGameUnit gap as above --
		-- the whole "if host exists and is far away, retreat to attack-C-ready"
		-- check can't be evaluated, so it's omitted and this block now always
		-- falls through to the SetTimerRestart(0) below (the same path June2017's
		-- own code takes when the condition is false), rather than never
		-- re-checking host distance.

		slot2:SetTimerRestart(0)
	end

	if slot2:GetTimerElapsedTime(1) > 7.5 and (slot1:GetNearestNPCUnitPos_LUA(slot2:GetPos(), 0, 1500).x ~= 0 or slot4.y ~= 0 or slot4.z ~= 0) then
		slot5 = slot1:GetDist_LUA(slot4, slot3)
		slot7 = slot2:GetRotateDegree()

		if slot2:IsRightTarget_LUA(slot2, slot4) == slot2:GetIsRight() then
			slot7.y = slot7.y + 180
		end

		slot2:SetIsRight(slot6)

		if slot5 < 200 then
			slot2:StateChange_LUA("SAMETEAM_IGNIA_ATTACK_A_READY", true)
		elseif slot2:GetRandVal() > 75 then
			slot2:StateChange_LUA("SAMETEAM_IGNIA_SPECIAL_ATTACK_A_READY", true)
		else
			slot2:StateChange_LUA("SAMETEAM_IGNIA_ATTACK_B_READY", true)
		end

		slot2:SetFlag_LUA(0, true)
		slot2:SetTimerRestart(1)
	end
end
