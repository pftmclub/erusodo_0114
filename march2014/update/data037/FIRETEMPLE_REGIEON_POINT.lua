INIT_SYSTEM = {
	UNIT_HEIGHT = 10,
	UNIT_WIDTH = 10
}
INIT_DEVICE = {
	READY_TEXTURE = {},
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
		STATE_NAME = "FIRETEMPLE_REGIEON_POINT_SUMMON",
		LUA_FRAME_MOVE_FUNC = "FIRETEMPLE_REGIEON_POINT_SUMMON_FRAME_MOVE",
		LUA_STATE_START_FUNC = "FIRETEMPLE_REGIEON_POINT_SUMMON_START",
		LUA_STATE_END_FUNC = "FIRETEMPLE_REGIEON_POINT_SUMMON_START"
	},
	{
		STATE_NAME = "FIRETEMPLE_REGIEON_POINT_DYING"
	},
	DYING_LAND_BACK = "FIRETEMPLE_REGIEON_POINT_DYING",
	DYING_SKY = "FIRETEMPLE_REGIEON_POINT_DYING",
	DYING_LAND_FRONT = "FIRETEMPLE_REGIEON_POINT_DYING",
	REVENGE_ATTACK = "",
	START_STATE = "FIRETEMPLE_REGIEON_POINT_SUMMON"
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
FIRETEMPLE_REGIEON_POINT_SUMMON = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	}
}

function FIRETEMPLE_REGIEON_POINT_SUMMON_START(slot0, slot1, slot2)
	slot3 = slot2:GetPos()
	slot3.x = 0
	slot3.y = -5000
	slot3.z = 0

	slot2:SetPositionForce(slot3)
	slot2:SetFlag_LUA(0, false)
	slot2:SetFlag_LUA(1, false)
	slot2:SetFlag_LUA(2, false)
end

function FIRETEMPLE_REGIEON_POINT_SUMMON_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:GetFlag_LUA(1) == false then
		slot3 = 0
		slot4 = slot1:GetNPCUnitListSize()

		if slot2:GetFlag_LUA(2) == false then
			if slot1:LiveActiveNPCNum() > 0 then
				slot2:SetFlag_LUA(2, true)
			end
		elseif slot1:LiveActiveNPCNum() == 0 then
			for slot9 = 0, slot1:GetUserUnitListSize() - 1 do
				if slot1:GetUserUnit(slot9) ~= nil then
					slot10:CreateTeleportEffect(slot10:GetLandPosition_LUA(), slot10:GetRotateDegree())
				end
			end

			slot2:SetFlag_LUA(1, true)
		end
	end

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): this whole block is omitted --
	-- slot1:GetIsGoNextPacketSended_LUA() and slot1:Send_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ()
	-- are both unbound in this March2014 client (checked X2Lib/KTDXLIB, not
	-- found under either name). Checked the underlying feature, not just the
	-- name: EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ IS a real, already-wired packet
	-- in this repo -- CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ
	-- (X2Lib/X2DungeonGame.cpp) sends it, and the offline emulator answers it
	-- (X2Lib/Offline/Handlers_Room.cpp). But that C++ method takes two int
	-- args and fires automatically from CX2DungeonGame's own per-frame update
	-- (X2DungeonGame.cpp ~line 237, gated on m_bGoNextPacketSended and a
	-- TouchStartLineMapSet() line-touch check) -- it was never exposed to Lua
	-- under this zero-arg "Send_..." name, so this is a real binding gap, not
	-- a misnamed lookup. Confirmed this doesn't strand FinalHall progression:
	-- every SUB_STAGE this NPC appears in (STAGE3 in all three difficulties)
	-- clears via the ordinary CLEAR_COND0 = CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC
	-- + CLEAR_TYPE.CT_SUB_STAGE mechanism, which is engine-driven and
	-- independent of this NPC's own Lua -- this block was an additional,
	-- redundant "teleport players to the next line immediately once my
	-- summons die" convenience the NPC itself performed on top of that,
	-- not the actual gate. Losing it means players walk to the next
	-- sub-stage's start line themselves instead of being auto-teleported
	-- there. Flagged for the repo/engine integration phase group in
	-- RANOX_PORT_PLAN.md as a candidate small wrapper binding (call the
	-- existing two-int-arg C++ method with the dungeon's own before/after
	-- sub-stage indices) if this convenience is wanted back.
	--[[
	if slot1:GetIsGoNextPacketSended_LUA() == false and slot2:GetFlag_LUA(0) == false and slot1:Send_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ() == true then
		if slot1:GetLineMap() ~= nil then
			slot5 = 31
			slot6 = 7

			if slot1:GetNowSubStageIndex() == 2 then
				slot5 = 35
				slot6 = 16
			elseif slot7 == 3 then
				slot5 = 39
				slot6 = 13
			end

			for slot12 = 0, slot1:GetUserUnitListSize() - 1 do
				if slot1:GetUserUnit(slot12) ~= nil then
					slot13:SetPositionOnLine(slot4:GetStartPosition(slot5), slot6)
					slot13:CreateTeleportEffect(slot13:GetLandPosition_LUA(), slot13:GetRotateDegree())

					if slot7 == 1 and slot12 == 2 then
						slot6 = 9
					end

					slot5 = slot5 + 1
				end
			end
		else
			slot5 = D3DXVECTOR3(-3254.2, 500, -12894)
			slot6 = 7

			if slot1:GetNowSubStageIndex() == 2 then
				slot5 = D3DXVECTOR3(11791, 500, -3658.86)
				slot6 = 16
			elseif slot7 == 3 then
				slot5 = D3DXVECTOR3(-833.982, 2711, -534)
				slot6 = 13
			end

			for slot12 = 0, slot1:GetUserUnitListSize() - 1 do
				if slot1:GetUserUnit(slot12) ~= nil then
					slot13:SetPositionOnLine(slot5, slot6)
					slot13:CreateTeleportEffect(slot13:GetLandPosition_LUA(), slot13:GetRotateDegree())
				end
			end
		end

		slot2:SetFlag_LUA(0, true)
	end
	]]
end

FIRETEMPLE_REGIEON_POINT_DYING = {
	ANIM_NAME = "Wait",
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
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}
