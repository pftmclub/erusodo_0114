INIT_SYSTEM = {
	UNIT_HEIGHT = 50,
	UNIT_WIDTH = 50
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"DRYAD_Fire01.dds",
		"Dark_Drabaki_GroundCrack001.dds"
	},
	READY_XSKIN_MESH = {
		"DummyAttackBox_50x50x50.X"
	},
	READY_SOUND = {}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Dummy_Ani_Wait.x"
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
	HYPER_MODE_COUNT = 0,
	MAX_HYPER_MODE_TIME = 30,
	HEAD_BONE_NAME = "Bip01",
	USE_SLASH_TRACE = FALSE,
	SHOW_ON_MINIMAP = FALSE
}
INIT_STATE = {
	{
		STATE_NAME = "TRAP_FIRE_BIG_START",
		LUA_FRAME_MOVE_FUNC = "TRAP_FIRE_BIG_START_FRAME_MOVE",
		LUA_STATE_START_FUNC = "TRAP_FIRE_BIG_START_STATE_START"
	},
	{
		STATE_NAME = "TRAP_FIRE_BIG_DYING",
		LUA_STATE_START_FUNC = "TRAP_FIRE_BIG_DYING_STATE_START"
	},
	DYING_LAND_BACK = "TRAP_FIRE_BIG_DYING",
	DYING_SKY = "TRAP_FIRE_BIG_DYING",
	DYING_LAND_FRONT = "TRAP_FIRE_BIG_DYING",
	REVENGE_ATTACK = "",
	START_STATE = "TRAP_FIRE_BIG_START"
}
INIT_AI = {
	TARGET = {
		TARGET_NEAR_RANGE = 0,
		TARGET_LOST_RANGE = 0,
		TARGET_SUCCESS_RATE = 0,
		PRESERVE_LAST_TARGET_RATE = 0,
		TARGET_INTERVAL = 99999,
		TARGET_RANGE = 0,
		ATTACK_TARGET_RATE = 0,
		TARGET_PRIORITY = TARGET_PRIORITY.TP_LOW_HP_FIRST
	}
}
TRAP_FIRE_BIG_START = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	RIGHT = TRUE,
	INVINCIBLE = {
		0,
		9999
	}
}

function TRAP_FIRE_BIG_START_STATE_START(slot0, slot1, slot2)
	slot2:SetTimerRestart(0)
	slot2:SetInt_LUA(0, 5)

	if slot1:GetEffectSet() ~= nil and slot3:PlayEffectSet_LUA("EffectSet_Trap_Fire_Big", slot2) ~= nil then
		slot2:SetEffectSet_LUA(0, slot4)
	end

	slot2:SetFlag_LUA(0, true)
end

function TRAP_FIRE_BIG_START_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:GetInt_LUA(0) < slot2:GetTimerElapsedTime(0) then
		if slot1:GetEffectSet() ~= nil then
			if slot2:GetFlag_LUA(0) == true then
				slot3:StopEffectSet_LUA(slot2:GetEffectSet_LUA(0))
				slot2:ClearEffectSet_LUA(0)
				slot2:SetFlag_LUA(0, false)
			else
				if slot3:PlayEffectSet_LUA("EffectSet_Trap_Fire_Big", slot2) ~= nil then
					slot2:SetEffectSet_LUA(0, slot4)
				end

				slot2:SetFlag_LUA(0, true)
			end
		end

		slot2:SetTimerRestart(0)
	end
end

TRAP_FIRE_BIG_DYING = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	INVINCIBLE = {
		0,
		9999
	},
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}

function TRAP_FIRE_BIG_DYING_STATE_START(slot0, slot1, slot2)
	if slot1:GetEffectSet() ~= nil then
		slot3:StopEffectSet_LUA(slot2:GetEffectSet_LUA(0))
		slot2:ClearEffectSet_LUA(0)
	end
end
