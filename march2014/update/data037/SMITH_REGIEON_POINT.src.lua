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
		STATE_NAME = "SMITH_REGIEON_POINT_SUMMON",
		LUA_FRAME_MOVE_FUNC = "SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE",
		LUA_STATE_START_FUNC = "SMITH_REGIEON_POINT_SUMMON_START"
	},
	{
		STATE_NAME = "SMITH_REGIEON_POINT_DYING"
	},
	DYING_LAND_BACK = "SMITH_REGIEON_POINT_DYING",
	DYING_SKY = "SMITH_REGIEON_POINT_DYING",
	DYING_LAND_FRONT = "SMITH_REGIEON_POINT_DYING",
	REVENGE_ATTACK = "",
	START_STATE = "SMITH_REGIEON_POINT_SUMMON"
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
SMITH_REGIEON_POINT_SUMMON = {
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

function SMITH_REGIEON_POINT_SUMMON_START(slot0, slot1, slot2)
	slot1:SetInt_LUA(0, 10)
	slot1:SetInt_LUA(1, 10)
	slot1:SetInt_LUA(2, 10)
	slot1:SetInt_LUA(4, slot1:GetInt_LUA(0) + slot1:GetInt_LUA(1) + slot1:GetInt_LUA(2) + slot1:GetInt_LUA(3))
	slot2:SetTimerRestart(0)
	slot2:SetInt_LUA(4, 5)
	slot2:SetInt_LUA(5, 15)
	slot2:SetInt_LUA(0, NPC_UNIT_ID.NUI_SMITH_RUNNER)
	slot2:SetInt_LUA(1, NPC_UNIT_ID.NUI_SMITH_SCIENCE)
	slot2:SetInt_LUA(2, NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER)
end

function SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:GetInt_LUA(4) <= slot2:GetTimerElapsedTime(0) then
		if slot1:GetInt_LUA(4) <= 0 then
			slot2:StateChange_LUA("SMITH_REGIEON_POINT_DYING", true)

			return
		end

		if slot1:GetNPCUnitNumCheckTeam(slot2:GetTeam()) < slot2:GetInt_LUA(5) then
			-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
			-- assignment (read again by GetInt_LUA below, and by ConsumeNPCSummomLimitNum/
			-- CreateNPCReq_LUA/GetInt_LUA further down when this branch isn't taken) and
			-- inlined the call into the condition; the port carried the bug verbatim.
			-- Bytecode-confirmed: decompile_verdicts.tsv,
			-- SMITH_REGIEON_POINT.src.lua::SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE,
			-- dropped-assignment, seed 2, POISON_READ slot3@L111 (128-seed harness pass).
			slot3 = slot2:GetRandVal() % 3

			if slot1:GetInt_LUA(slot3) <= 0 then
				slot4 = false

				for slot8 = 0, 2 do
					if slot1:GetInt_LUA(slot8) > 0 then
						slot3 = slot8
						slot4 = true

						break
					end
				end

				if slot4 == false then
					slot2:StateChange_LUA("SMITH_REGIEON_POINT_DYING", true)

					return
				end
			end

			ConsumeNPCSummomLimitNum(slot1, slot3)
			slot2:CreateNPCReq_LUA(slot2:GetInt_LUA(slot3), slot2:GetHardLevel(), FALSE, slot2:GetLandPosition_LUA(), true, 0, false)
			slot2:SetTimerRestart(0)
		end
	end
end

function ConsumeNPCSummomLimitNum(slot0, slot1)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
	-- assignment (read again by SetInt_LUA below) and inlined the call into
	-- the condition; the port carried the bug verbatim. Bytecode-confirmed:
	-- decompile_verdicts.tsv, SMITH_REGIEON_POINT.src.lua::ConsumeNPCSummomLimitNum,
	-- dropped-assignment, seed 2, POISON_READ slot2@L123 (128-seed harness pass).
	slot2 = slot0:GetInt_LUA(slot1) - 1

	if slot2 < 0 then
		slot2 = 0
	end

	slot0:SetInt_LUA(slot1, slot2)

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): same class of bug on the second
	-- counter (decompile: another dropped slot3, read again by SetInt_LUA
	-- below). Also renamed slot3 -> slot9: this file has no `local`, and the
	-- caller SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE keeps its own slot3 (the
	-- NPC-slot index picked above the call) alive across this call and
	-- rereads it afterward. Confirmed by the harness: CLOBBER
	-- SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE ConsumeNPCSummomLimitNum slot3
	-- had/unset on several seeds before this rename; 0 after. Per the
	-- recorded decision (RANOX_LUA_DECOMPILE_FIX_PLAN.md, "the local slotN
	-- decision"), known clobbers are fixed case by case rather than
	-- adopting `local`; this rename is the case-by-case fix and stays
	-- within "no local".
	slot9 = slot0:GetInt_LUA(4) - 1

	if slot9 < 0 then
		slot9 = 0
	end

	slot0:SetInt_LUA(4, slot9)
end

SMITH_REGIEON_POINT_DYING = {
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
