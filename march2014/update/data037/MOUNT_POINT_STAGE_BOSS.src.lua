INIT_SYSTEM = {
	UNIT_HEIGHT = 1,
	UNIT_WIDTH = 1
}
INIT_DEVICE = {
	READY_TEXTURE = {},
	READY_SOUND = {}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_GateWay_2stage_magma03.x",
	ADD_ROTATE_Y = 180
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
	DIE_FLY = 0,
	MAX_HYPER_MODE_TIME = 30,
	HEAD_BONE_NAME = "",
	USE_SLASH_TRACE = FALSE,
	SHOW_ON_MINIMAP = FALSE,
	FALL_DOWN = FALSE,
	NOT_AFFECT_BLACKHOLE = TRUE
}
INIT_STATE = {
	{
		STATE_NAME = "MOUNT_POINT_STAGE_BOSS_START",
		LUA_STATE_START_FUNC = "MOUNT_POINT_STAGE_BOSS_START_STATE_START"
	},
	{
		STATE_NAME = "MOUNT_POINT_STAGE_BOSS_DYING"
	},
	DYING_LAND_BACK = "MOUNT_POINT_STAGE_BOSS_DYING",
	DYING_SKY = "MOUNT_POINT_STAGE_BOSS_DYING",
	DYING_LAND_FRONT = "MOUNT_POINT_STAGE_BOSS_DYING",
	REVENGE_ATTACK = "",
	START_STATE = "MOUNT_POINT_STAGE_BOSS_START"
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

function MOUNT_POINT_STAGE_BOSS_START_STATE_START(slot0, slot1, slot2)
	slot2:SetPositionForce(D3DXVECTOR3(0, 0, 0))
end

MOUNT_POINT_STAGE_BOSS_START = {
	ANIM_NAME = "wait",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		1000
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"MOUNT_POINT_STAGE_BOSS_DYING",
			"CF_MOUNT_POINT_STAGE_BOSS_DYING"
		}
	}
}

function CF_MOUNT_POINT_STAGE_BOSS_DYING(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
	-- counter's initializer, then dropped slot9 too (read again by every
	-- comparison below) and inlined its call into the condition. A third
	-- effect of the same fold: the increment printed as `slot4 = 0 + 1`
	-- instead of `slot4 = slot4 + 1`, so the port never actually counted
	-- past 1. The port carried all three verbatim. Bytecode-confirmed:
	-- decompile_verdicts.tsv, MOUNT_POINT_STAGE_BOSS.src.lua::
	-- CF_MOUNT_POINT_STAGE_BOSS_DYING, dropped-assignment, seed 1,
	-- POISON_READ slot4@L91/slot9@L86 (128-seed harness).
	slot4 = 0

	for slot8 = 0, slot1:GetNPCUnitListSize() - 1 do
		slot9 = slot1:GetNPCUnit(slot8)

		if slot9 ~= nil and slot2:GetUID() ~= slot9:GetUID() and slot9:GetClassType() == NPC_CLASS_TYPE.NCT_BASIC and slot9:IsUnitID(NPC_UNIT_ID.NUI_SAMETEAM_IGNIA) == false and slot9:GetNowHP() > 0 then
			slot4 = slot4 + 1
		end
	end

	if slot4 == 0 then
		return true
	end

	return false
end

MOUNT_POINT_STAGE_BOSS_DYING = {
	ANIM_NAME = "move",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	INVINCIBLE = {
		0,
		1000
	},
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}
