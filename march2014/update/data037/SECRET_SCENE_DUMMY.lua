g_iSummonStartCondition_S1Sub1 = 4
g_iMagiStartPosIndex_S1Sub1 = 54
g_arrSummonNpcID_S1Sub1 = {
	NPC_UNIT_ID.NUI_SECRET_SUMMON_MAG_NOM,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_FIGHTER,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_FIREBIRD,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_SNIKER
}
g_arrMonsterStartPosIndex_S1Sub1 = {
	19,
	20,
	21,
	22,
	41,
	23,
	37
}
g_iSummonNpcNum_S1Sub1 = 7
g_iSummonStartConditionB_S1Sub2 = 7
g_iMagiBStartPosIndex_S1Sub2 = 56
g_iSummonStartConditionC_S1Sub2 = 4
g_iMagiCStartPosIndex_S1Sub2 = 57
g_arrSummonNpcID_S1Sub2 = {
	NPC_UNIT_ID.NUI_SECRET_SUMMON_MAG_NOM,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_FIGHTER,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_FIREBIRD,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_SNIKER
}
g_arrMonsterStartPosIndex_S1Sub2 = {
	25,
	30,
	24,
	26,
	31,
	27,
	28,
	29
}
g_iSummonNpcNum_S1Sub2 = 8
g_iSummonWaveCount = 4
g_iSummomWaveGapTime = 3
g_iSummonLimitMonsterNum = 4
g_arrSummonNpcID_S2 = {
	NPC_UNIT_ID.NUI_SECRET_SUMMON_MAG_NOM,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_FIGHTER,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_FIREBIRD,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_SNIKER,
	NPC_UNIT_ID.NUI_SECRET_SUMMON_BERSERKER
}
g_arrMonsterStartPosIndex_S2 = {
	16,
	17,
	18,
	20,
	21,
	22,
	13,
	14,
	15,
	26,
	19,
	25,
	24
}
g_iSummonNpcNum_S2 = 7
g_iSummomMagiGapTime = 2.5
g_iSummomMagiMaxNum = 5
g_arrSummonPosOrigin = {
	13,
	15,
	26,
	17,
	20,
	25,
	23,
	21,
	16,
	19,
	24
}
g_iFeverTime = 10

function TableCopy(t)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- same
	-- reconstruction, same evidence and reasoning as SECRET_MAGI_BOSS.lua's
	-- own TableCopy (see that file for the full note) -- this file's own call
	-- site, `g_arrSummonPos = TableCopy(g_arrSummonPosOrigin)` two lines below,
	-- confirms the same 1-arg shallow-copy-and-return signature.
	local copy = {}
	for k, v in pairs(t) do
		copy[k] = v
	end
	return copy
end

g_arrSummonPos = TableCopy(g_arrSummonPosOrigin)
INIT_SYSTEM = {
	UNIT_HEIGHT = 150,
	UNIT_WIDTH = 50
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
	SHOW_ON_MINIMAP = FALSE,
	NOT_CHECK_INDICATOR = TRUE
}
INIT_STATE = {
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_START",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_START_STATE_START"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_SUMMON_TYPE1",
		LUA_FRAME_MOVE_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE1_FRAME_MOVE",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE1_STATE_START"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_SUMMON_TYPE2",
		LUA_FRAME_MOVE_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE2_FRAME_MOVE",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE2_STATE_START"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_SUMMON_TYPE3",
		LUA_FRAME_MOVE_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_FRAME_MOVE",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_STATE_START"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_READY",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_READY_START_FUNC"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_STATE_START",
		LUA_STATE_END_FUNC = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_STATE_END"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_END"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_DYING_READY",
		LUA_STATE_END_FUNC = "SECRET_SCENE_DUMMY_DYING_READY_END_FUNC"
	},
	{
		STATE_NAME = "SECRET_SCENE_DUMMY_DYING",
		LUA_STATE_START_FUNC = "SECRET_SCENE_DUMMY_DYING_STATE_START"
	},
	DYING_LAND_BACK = "SECRET_SCENE_DUMMY_DYING",
	DYING_SKY = "SECRET_SCENE_DUMMY_DYING",
	COMMON_FRAME_FUNC = "SECRET_SCENE_DUMMY_COMMON_FRAME_FUNC",
	DYING_LAND_FRONT = "SECRET_SCENE_DUMMY_DYING",
	REVENGE_ATTACK = "",
	START_STATE = "SECRET_SCENE_DUMMY_START"
}
INIT_AI = {
	NO_BRAIN = TRUE
}
SECRET_SCENE_DUMMY_START = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	EVENT_PROCESS = {}
}

function SECRET_SCENE_DUMMY_START_STATE_START(slot0, slot1, slot2)
	slot2:SetFlag_LUA(0, false)
	slot2:SetFlag_LUA(1, false)
	slot2:SetFlag_LUA(5, false)
	slot2:SetInt_LUA(4, 0)
	slot2:SetInt_LUA(5, g_iSummonWaveCount)
	slot2:SetInt_LUA(6, g_iSummomWaveGapTime)
	slot2:SetInt_LUA(7, g_iSummonLimitMonsterNum)
	slot1:SetInt_LUA(0, 0)

	if slot1:GetNowStageIndex() == 1 or slot1:GetNowStageIndex() == 5 then
		if slot1:GetNowSubStageIndex() == 1 then
			slot2:StateChange_LUA("SECRET_SCENE_DUMMY_SUMMON_TYPE1", true)
		elseif slot1:GetNowSubStageIndex() == 2 then
			slot2:StateChange_LUA("SECRET_SCENE_DUMMY_SUMMON_TYPE2", true)
		end
	elseif slot1:GetNowStageIndex() == 2 or slot1:GetNowStageIndex() == 6 then
		slot2:StateChange_LUA("SECRET_SCENE_DUMMY_SUMMON_TYPE3", true)

		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
		-- CreateSimpleDLG and SetShow_LUA do not exist anywhere in this
		-- repo's C++ (checked X2Lib/KTDXLIB exhaustively) -- this whole
		-- "SimpleDLG" NPC-script-to-UI-dialog mechanism is genuinely absent
		-- from this build, not just unbound under another name (a plain
		-- "SimpleDLG"/"SetNumberInGageStatic"/"SetNumberInNumberStatic"
		-- grep across both trees returns zero hits). Omitted; this dungeon's
		-- fever-gauge dialog (DLG_UI_RANOX_SECRET_FEVER_GAGE.lua, ported this
		-- phase to march2014/update/data034/) is never opened, so none of
		-- this file's other GetSimpleDLG()-gated blocks below can ever run
		-- either -- see each for its own specific effect.
	end
end

SECRET_SCENE_DUMMY_SUMMON_TYPE1 = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"SECRET_SCENE_DUMMY_DYING_READY",
			"CF_SECRET_SCENE_DUMMY_DYING_READY"
		}
	}
}

function CF_SECRET_SCENE_DUMMY_DYING_READY(slot0, slot1, slot2)
	return slot2:GetFlag_LUA(5)
end

function SECRET_SCENE_DUMMY_SUMMON_TYPE1_STATE_START(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
	-- SetActiveTrigger_LUA does not exist anywhere in this repo's C++
	-- (checked X2Lib/KTDXLIB exhaustively). Omitted; this stage's LineMap
	-- trigger index 4 is never armed by this call.
	print("trigger on 4")
end

function SECRET_SCENE_DUMMY_SUMMON_TYPE1_FRAME_MOVE(slot0, slot1, slot2)
	if slot1:IsHost() == true and slot2:GetNowHP() > 0 and slot1:LiveKeyCodeNPCNum(1) <= g_iSummonStartCondition_S1Sub1 then
		slot3 = slot1:GetLineMap():GetStartPosition(g_iMagiStartPosIndex_S1Sub1)

		if slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_A) ~= nil then
			-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
			-- GetNowStateName does not exist anywhere in this repo's C++
			-- (checked X2Lib/KTDXLIB exhaustively). Omitted; when a
			-- NUI_SECRET_MAGI_SCENE_A object already exists, this branch now
			-- does nothing instead of starting its scene action -- the
			-- sibling else-branch's normal random-pos summon is untouched.
		else
			if slot2:GetFlag_LUA(5) == false then
				SummonMonstersRandomPos(slot1, slot2, g_arrSummonNpcID_S1Sub1, g_arrMonsterStartPosIndex_S1Sub1, g_iSummonNpcNum_S1Sub1)
			end

			slot2:SetFlag_LUA(5, true)
		end
	end
end

SECRET_SCENE_DUMMY_SUMMON_TYPE2 = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"SECRET_SCENE_DUMMY_DYING_READY",
			"CF_SECRET_SCENE_DUMMY_DYING_READY"
		}
	}
}

function SECRET_SCENE_DUMMY_SUMMON_TYPE2_STATE_START(slot0, slot1, slot2)
end

function SECRET_SCENE_DUMMY_SUMMON_TYPE2_FRAME_MOVE(slot0, slot1, slot2)
	if slot1:IsHost() == true and slot2:GetNowHP() > 0 then
		if slot1:LiveKeyCodeNPCNum(1) <= g_iSummonStartConditionB_S1Sub2 and slot2:GetFlag_LUA(0) == false then
			if slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_B) ~= nil then
				-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret
				-- Hell) -- GetNowStateName does not exist anywhere in this
				-- repo's C++ (same gap as SCENE_A above). Omitted; when a
				-- NUI_SECRET_MAGI_SCENE_B object already exists, this branch
				-- now does nothing instead of starting its scene action.
			else
				slot2:SetFlag_LUA(0, true)
				SummonMonstersRandomPos(slot1, slot2, g_arrSummonNpcID_S1Sub2, g_arrMonsterStartPosIndex_S1Sub2, g_iSummonNpcNum_S1Sub2)
			end
		end

		if slot1:LiveKeyCodeNPCNum(1) <= g_iSummonStartConditionC_S1Sub2 and slot2:GetFlag_LUA(1) == false and slot2:GetFlag_LUA(0) == true then
			if slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_C) ~= nil then
				-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret
				-- Hell) -- GetNowStateName does not exist anywhere in this
				-- repo's C++ (same gap as SCENE_A above). Omitted; when a
				-- NUI_SECRET_MAGI_SCENE_C object already exists, this branch
				-- now does nothing instead of starting its scene action.
			else
				slot2:SetFlag_LUA(1, true)
				SummonMonstersRandomPos(slot1, slot2, g_arrSummonNpcID_S1Sub2, g_arrMonsterStartPosIndex_S1Sub2, g_iSummonNpcNum_S1Sub2)
			end
		end

		if slot2:GetFlag_LUA(0) == true and slot2:GetFlag_LUA(1) == true then
			slot2:SetFlag_LUA(5, true)
		end
	end
end

SECRET_SCENE_DUMMY_SUMMON_TYPE3 = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	ALARM = {
		DELAY = 2,
		DISAPPEAR_TIME = 5,
		DANGER = FALSE,
		ALARM_MESSAGE = STR_ID_40005,
		REPEAT = FALSE,
		ALARM_COLOR_TYPE = ALARM_COLOR_TYPE.ACT_ORANGE
	}
}

function SECRET_SCENE_DUMMY_SUMMON_TYPE3_STATE_START(slot0, slot1, slot2)
	if slot1:IsHost() == true then
		slot2:SetFlag_LUA(0, false)
		slot2:SetFlag_LUA(1, false)
		slot2:SetInt_LUA(0, 0)
		slot2:SetInt_LUA(1, 0)
		slot2:SetTimerRestart(0)
		slot2:SetTimerRestart(1)

		g_arrSummonPos = TableCopy(g_arrSummonPosOrigin)

		print("Init!!!")
	end

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
	-- SetActiveTrigger_LUA does not exist anywhere in this repo's C++
	-- (checked X2Lib/KTDXLIB exhaustively). Omitted; this stage's LineMap
	-- trigger index 2 is never armed by this call.

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- the
	-- removed block reset the fever-gauge dialog's counter/gauge widgets to
	-- 0 via GetSimpleDLG/SetNumberInNumberStatic_LUA/SetNumberInGageStatic_LUA,
	-- none of which exist in this repo's C++ (see the CreateSimpleDLG note
	-- above, ~line 210). Omitted; pure UI-display reset, no gameplay state.

	slot1:GetWorld():SetCameraDistance(1000, 0, 0, 0)
end

function SECRET_SCENE_DUMMY_SUMMON_TYPE3_FRAME_MOVE(slot0, slot1, slot2)
	if slot1:IsHost() == true then
		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
		-- SUBSTANTIAL, DOCUMENTED-BEHAVIOR-LOSS OMISSION, not a cosmetic one.
		-- The removed condition's own gate, GetSimpleDLG() ~= nil, does not
		-- exist in this repo's C++ (see the CreateSimpleDLG note above,
		-- ~line 210) -- and this dungeon's whole "count to 5" mechanism has
		-- no separate state variable; the live count is read directly off
		-- the fever-gauge dialog's own number-static widget
		-- (slot3:GetNumber_LUA(), set elsewhere in this file via the equally
		-- absent SetNumberInNumberStatic_LUA). With no dialog, there is no
		-- substitute counter to gate on without inventing a new mechanism,
		-- which this port does not do. GetDyingStart (used inside the
		-- removed block) is also absent from this repo's C++, independently.
		-- Omitted whole: the magi-boss group-object spawn
		-- (CreateNPCReq_LUA(NUI_SECRET_MAGI_OBJECT_GROUB, ...)), the
		-- transition into SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_READY, and the
		-- per-magi-object FEVER_READY state transition never fire. The fever
		-- buff sequence this whole state chain builds toward
		-- (SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_STATE_START/_END further down
		-- this file) is therefore currently unreachable in-game. Repo/engine
		-- integration phase group candidate: binding a substitute counter
		-- (or resolving the SimpleDLG gap itself) would restore this.

		SummonMonsterFrameMoveWithMaxCount(slot1, slot2, NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT, g_iSummomMagiGapTime, g_iSummomMagiMaxNum)
	end
end

SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_READY = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF",
			"CF_SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF"
		}
	}
}

function SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_READY_START_FUNC(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- pure
	-- UI-display update (fever-gauge widget), same GetSimpleDLG/
	-- SetNumberInNumberStatic_LUA/SetNumberInGageStatic_LUA gap as the
	-- CreateSimpleDLG note above (~line 210). Omitted; no gameplay state.
end

function CF_SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
	-- GetNowStateName does not exist anywhere in this repo's C++ (same gap
	-- as SCENE_A/B/C above). This condition function is this state's
	-- SCT_CONDITION_TABLE early-exit check (see
	-- SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF's EVENT_PROCESS above); the same
	-- table's STATE_TIME_OVER = g_iFeverTime still ends the fever buff on
	-- its own timer regardless, so always returning false here (never
	-- early-exiting via the group object's state) is the safe default --
	-- the buff simply always runs its full duration instead of ending early
	-- if the group object leaves its FEVER_LOOP state first.
	return false
end

SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	ALARM = {
		DELAY = 0,
		DISAPPEAR_TIME = 3,
		DANGER = TRUE,
		ALARM_MESSAGE = STR_ID_40006,
		REPEAT = TRUE,
		ALARM_COLOR_TYPE = ALARM_COLOR_TYPE.ACT_RED
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_END",
			"CT_SECRET_SCENE_DUMMY_SUMMON_TYPE3"
		}
	},
	CT_SECRET_SCENE_DUMMY_SUMMON_TYPE3 = {
		STATE_TIME_OVER = g_iFeverTime
	}
}

function SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_STATE_START(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
	-- FadeOriginWorldColor_LUA does not exist anywhere in this repo's C++
	-- (checked X2Lib/KTDXLIB exhaustively). Omitted; the fever buff's
	-- world-tint fade-in never plays, the buff itself (below) is untouched.

	if slot1:IsHost() == true then
		SetBuffAllNpcAndMagiStageChange(slot1, slot2, BUFF_FACTOR_ID.BFI_BUFF_RANOX_SECRET_FEVER)
	end
end

function SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_STATE_END(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- same
	-- FadeOriginWorldColor_LUA gap as BUFF_STATE_START above. Omitted; the
	-- fever buff's world-tint fade-out never plays, the NPC state-change
	-- and buff-erase logic below is untouched.

	if slot1:IsHost() == true then
		for slot7 = 0, slot1:GetNPCUnitListSize() - 1 do
			if slot1:GetNPCUnit(slot7) ~= nil then
				if slot8:IsUnitID(NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT) == true then
					slot8:StateChange_LUA("SECRET_MAGI_OBJECT_FEVER_END", true)
				end

				if slot8:IsUnitID(NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT_GROUB) == true then
					slot8:StateChange_LUA("SECRET_MAGI_OBJECT_GROUB_FEVER_END", true)
				end

				if slot8:HaveThisBuff(BUFF_TEMPLET_ID.BTI_BUFF_RANOX_SECRET_FEVER) == true then
					slot8:EraseBuffTempletFromGameUnit_LUA(BUFF_TEMPLET_ID.BTI_BUFF_RANOX_SECRET_FEVER)
				end
			end
		end
	end

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- pure
	-- UI-display reset (fever-gauge widget), same GetSimpleDLG/
	-- SetNumberInNumberStatic_LUA/SetNumberInGageStatic_LUA gap as the
	-- CreateSimpleDLG note above (~line 210). Omitted; no gameplay state.
end

SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_END = {
	ANIM_NAME = "Wait",
	SHOW = FALSE,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = FALSE,
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	NEVER_MOVE = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"SECRET_SCENE_DUMMY_SUMMON_TYPE3",
			"CF_SECRET_SCENE_DUMMY_SUMMON_TYPE3"
		}
	}
}

function CF_SECRET_SCENE_DUMMY_SUMMON_TYPE3(slot0, slot1, slot2)
	if slot1:GetNPCUnitByType(NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT) == nil then
		return true
	end

	return false
end

SECRET_SCENE_DUMMY_DYING_READY = {
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
	IMMADIATE_PACKET_SEND = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_TABLE,
			"SECRET_SCENE_DUMMY_DYING",
			"CT_SECRET_SCENE_DUMMY_DYING"
		}
	},
	CT_SECRET_SCENE_DUMMY_DYING = {
		STATE_TIME_OVER = 5
	}
}

function SECRET_SCENE_DUMMY_DYING_READY_END_FUNC(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
	-- decompiler mislabeling, corrected, not a gameplay decision: the
	-- vendored ljd decompiler emitted "GetNowHP(0)" here, but GetNowHP is a
	-- real, already-used, zero-argument getter in this same file
	-- (SECRET_SCENE_DUMMY_SUMMON_TYPE1_FRAME_MOVE's
	-- "slot2:GetNowHP() > 0") -- passing it a phantom argument whose return
	-- is then discarded (a bare statement, no assignment) would make this
	-- "_READY_END_FUNC" handler, called right before the state machine
	-- enters SECRET_SCENE_DUMMY_DYING below, do nothing at all. SetNowHP(0)
	-- is the verbatim call this file's own SECRET_SCENE_DUMMY_SUMMON_TYPE3
	-- branch already uses for the same "finish killing this NPC" purpose
	-- (both GetNowHP and SetNowHP are real, distinct, bound methods --
	-- X2Lib/X2Main.cpp's class_def<CX2GameUnit> block). Using the form
	-- consistent with the rest of this file, not "tidying" studio data.
	slot2:SetNowHP(0)
end

SECRET_SCENE_DUMMY_DYING = {
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

function SECRET_SCENE_DUMMY_DYING_STATE_START(slot0, slot1, slot2)
	if slot1:GetNowStageIndex() == 2 or slot1:GetNowStageIndex() == 6 then
		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) --
		-- SetActiveTrigger_LUA does not exist anywhere in this repo's C++
		-- (checked X2Lib/KTDXLIB exhaustively). Omitted; this stage's
		-- LineMap trigger index 1 is never armed by this call.

		-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- the
		-- removed block hid the fever-gauge dialog via GetSimpleDLG/
		-- SetShow_LUA, neither of which exists in this repo's C++ (see the
		-- CreateSimpleDLG note above, ~line 210). Omitted; pure UI-display,
		-- no gameplay state.
	end
end

function SECRET_SCENE_DUMMY_COMMON_FRAME_FUNC(slot0, slot1, slot2)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- the
	-- removed block's gate, GetSimpleDLG() ~= nil, and its body,
	-- CrashAndColorChange_LUA, are both absent from this repo's C++ (see
	-- the CreateSimpleDLG note above, ~line 210). Omitted; pure UI-display
	-- warning-flash cue, no gameplay state -- the real per-frame logic
	-- below (host check, stage-index guard, summon-cap gate) is untouched.

	if slot1:IsHost() == true and slot2:GetNowHP() > 0 then
		if slot1:GetNowStageIndex() ~= 2 and slot1:GetNowStageIndex() ~= 6 then
			return false
		end

		if slot2:GetInt_LUA(4) < slot2:GetInt_LUA(5) then
			if slot2:GetInt_LUA(7) < slot1:LiveActiveNPCNum() then
				return false
			end

			if slot2:GetTimerElapsedTime(1) < slot2:GetInt_LUA(6) then
				return false
			end

			print("Monster summon!!!")
			SummonMonstersRandomPos(slot1, slot2, g_arrSummonNpcID_S2, g_arrMonsterStartPosIndex_S2, g_iSummonNpcNum_S2)
			slot2:SetInt_LUA(4, slot3 + 1)
			slot2:SetTimerRestart(1)
		elseif slot1:LiveActiveNPCNum() < 2 then
			SECRET_SCENE_DUMMY_SUMMON_TYPE3_BUFF_STATE_END(slot0, slot1, slot2)
			slot2:SetNowHP(0)
		end
	end
end

function SummonMonsterFrameMoveWithMaxCount(slot0, slot1, slot2, slot3, slot4)
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 9 (Secret Hell) -- BEHAVIOR
	-- DELETION, not a reconstruction -- unlike TableCopy, this function's real
	-- logic could not be recovered and was not guessed at. Same decompiler
	-- limitation as TableCopy (ljd/ast/builder.py:375's iterator-warp assert,
	-- tripped by a USETV instruction, not an ITER-family opcode -- confirmed
	-- by instrumenting the assert directly), but this function's body is real,
	-- non-trivial gameplay logic (a periodic summon-cap gate), not a universal
	-- utility -- reconstructing it from its name alone would be inventing
	-- dungeon behavior, which this port does not do. Parameter names are
	-- inferred from this function's one real call site (below in this same
	-- file): SummonMonsterFrameMoveWithMaxCount(slot1, slot2,
	-- NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT, g_iSummomMagiGapTime, g_iSummomMagiMaxNum)
	-- -- so slot2/slot3/slot4 are almost certainly an NPC_UNIT_ID to summon, a
	-- gap-time, and a max-count. Left as a no-op (rather than omitting the
	-- function or the file) so the call site doesn't raise "attempt to call a
	-- nil value" and abort this NPC's whole script -- same disposition
	-- Phases 6-8 gave an unbound C++ method's call site, extended here to a
	-- whole function body. Effect: the periodic magi-minion summon wave this
	-- gate was meant to control never fires. Needs either a decompiler fix
	-- (repo/engine phase group candidate, though this is a tooling gap, not an
	-- engine gap) or manual reverse engineering as follow-up.
end

function SetBuffAllNpcAndMagiStageChange(slot0, slot1, slot2)
	for slot7 = 0, slot0:GetNPCUnitListSize() - 1 do
		if slot0:GetNPCUnit(slot7) ~= nil and slot8:IsUnitID(NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY) == false and slot8:IsUnitID(NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT) == false and slot8:IsUnitID(NPC_UNIT_ID.NUI_SECRET_MAGI_OBJECT_GROUB) == false then
			slot8:SetBuffFactorToGameUnitByBuffFactorID_LUA(slot2)
		end
	end
end

function EraseBuffAllNpc(slot0, slot1, slot2)
	for slot7 = 0, slot0:GetNPCUnitListSize() - 1 do
		if slot0:GetNPCUnit(slot7) ~= nil and slot8:HaveThisBuff(slot2) == true then
			slot8:EraseBuffTempletFromGameUnit_LUA(slot2)
		end
	end
end

function SummonMonstersRandomPos(slot0, slot1, slot2, slot3, slot4)
	for slot9 = 0, slot4 - 1 do
		if #TableCopy(slot3) == 0 then
			slot5 = TableCopy(slot3)
		end

		slot11 = slot2[slot1:GetRandVal(slot9) % #slot2 + 1]
		slot13 = slot0:GetLineMap():GetStartPosition(slot5[slot1:GetRandVal() % #slot5 + 1])

		if #slot5 >= 1 then
			table.remove(slot5, slot10)
		end

		slot1:CreateNPCReq_LUA(slot11, slot1:GetHardLevel(), true, slot13, false, 0, false)
		print("== SummonMonstersRandomPos Success Info Start ==")
		print(slot12)
		print(slot11)
		print("== SummonMonstersRandomPos Success Info End ==")
	end
end
