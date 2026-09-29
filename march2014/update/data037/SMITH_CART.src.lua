INIT_SYSTEM = {
	UNIT_HEIGHT = 200,
	UNIT_SCALE = 1,
	UNIT_WIDTH = 100,
	UNIT_LAYER = X2_LAYER.XL_UNIT_0
}
INIT_DEVICE = {
	READY_TEXTURE = {
		"Smoke_Machine03.tga"
	},
	READY_SOUND = {
		"Nasod_Railer_Walk.ogg",
		"LittleWorker_Voice_DamageA01.ogg"
	},
	READY_XMESH = {},
	READY_XSKIN_MESH = {}
}
INIT_MOTION = {
	MOTION_FILE_NAME = "Motion_SMITH_CART.X"
}
INIT_PHYSIC = {
	RUN_SPEED = 500,
	RELOAD_ACCEL = 2000,
	DASH_JUMP_SPEED = 1800,
	WALK_SPEED = 250,
	G_ACCEL = 4000,
	MAX_G_SPEED = -2000,
	JUMP_SPEED = 1500
}
INIT_COMPONENT = {
	SHADOW_FILE_NAME = "shadow.dds",
	MP_CHANGE_RATE = 1,
	MAX_HP = 4500,
	SMALL_HP_BAR_BLUE = "Small_HP_bar_Blue.TGA",
	SMALL_HP_BAR_RED = "Small_HP_bar_Red.TGA",
	DEFAULT_ANIM_SPEED = 0.5,
	HYPER_MODE_COUNT = 0,
	DIE_FLY = 0,
	SHADOW_SIZE = 200,
	SMALL_HP_BAR_YELLOW = "Small_HP_bar_Yellow.TGA",
	MP_CHARGE_RATE = 130,
	MAX_HYPER_MODE_TIME = 30,
	EXCLAMATION_MARK_SEQ = "ExclamationMarkNPC",
	QUESTION_MARK_SEQ = "QuestionMarkNPC",
	USE_SLASH_TRACE = FALSE,
	HITTED_TYPE = HITTED_TYPE.HTD_WOOD,
	FALL_DOWN = FALSE,
	DAMAGE_DOWN = FALSE
}
INIT_STATE = {
	{
		STATE_NAME = "SMITH_CART_WAIT"
	},
	{
		STATE_NAME = "SMITH_CART_TURN",
		LUA_STATE_START_FUNC = "SMITH_CART_TURN_STATE_START"
	},
	{
		STATE_NAME = "SMITH_CART_DASH_ATTACK",
		LUA_FRAME_MOVE_FUNC = "SMITH_CART_DASH_ATTACK_FRAME_MOVE",
		LUA_STATE_END_FUNC = "SMITH_CART_DASH_ATTACK_STATE_END"
	},
	{
		STATE_NAME = "SMITH_CART_DASH_ATTACK_END"
	},
	{
		STATE_NAME = "SMITH_CART_DAMAGE",
		LUA_FRAME_MOVE_FUNC = "SMITH_CART_DAMAGE_FRAME_MOVE"
	},
	{
		STATE_NAME = "SMITH_CART_DYING_LAND"
	},
	DYING_SKY = "SMITH_CART_DYING_LAND",
	DOWN_DAMAGE_LAND_FRONT = "SMITH_CART_DAMAGE",
	SMALL_DAMAGE_LAND_FRONT = "SMITH_CART_DAMAGE",
	SMALL_DAMAGE_AIR = "SMITH_CART_DAMAGE",
	DAMAGE_REVENGE = "SMITH_CART_DAMAGE",
	FLY_DAMAGE_BACK = "SMITH_CART_DAMAGE",
	UP_DAMAGE = "SMITH_CART_DAMAGE",
	START_STATE = "SMITH_CART_WAIT",
	DOWN_DAMAGE_LAND_BACK = "SMITH_CART_DAMAGE",
	BIG_DAMAGE_LAND_BACK = "SMITH_CART_DAMAGE",
	FLY_DAMAGE_FRONT = "SMITH_CART_DAMAGE",
	REVENGE_ATTACK = "",
	BIG_DAMAGE_LAND_FRONT = "SMITH_CART_DAMAGE",
	SMALL_DAMAGE_LAND_BACK = "SMITH_CART_DAMAGE",
	BIG_DAMAGE_AIR = "SMITH_CART_DAMAGE",
	DYING_LAND_BACK = "SMITH_CART_DYING_LAND",
	DOWN_DAMAGE_AIR = "SMITH_CART_DAMAGE",
	DYING_LAND_FRONT = "SMITH_CART_DYING_LAND"
}
INIT_AI = {
	TARGET = {
		TARGET_NEAR_RANGE = 1500,
		TARGET_LOST_RANGE = 5000,
		TARGET_SUCCESS_RATE = 100,
		PRESERVE_LAST_TARGET_RATE = 0,
		TARGET_INTERVAL = 1,
		TARGET_RANGE = 1500,
		ATTACK_TARGET_RATE = 100,
		TARGET_PRIORITY = TARGET_PRIORITY.TP_LOW_HP_FIRST
	},
	CHASE_MOVE = {
		UP_JUMP_RATE = 0,
		DOWN_DOWN_RATE = 0,
		WALK_INTERVAL = 3,
		JUMP_INTERVAL = 5,
		UP_DOWN_RATE = 0,
		MOVE_GAP = 160,
		DOWN_JUMP_RATE = 0,
		DEST_GAP = 150,
		DIR_CHANGE_INTERVAL = 9999,
		LINE_END_RANGE = 80,
		FAR_WALK_RATE = 0,
		MOVE_SPLIT_RANGE = 600,
		NEAR_WALK_RATE = 0
	},
	PATROL_MOVE = {
		PATROL_COOL_TIME = 2,
		PATROL_RANGE = 200,
		PATROL_BEGIN_RATE = 0,
		ONLY_THIS_LINE_GROUP = TRUE
	}
}
SMITH_CART_WAIT = {
	EVENT_INTERVAL_TIME0 = 0.8,
	ANIM_NAME = "Walk",
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = TRUE,
	LAND_CONNECT = TRUE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SMITH_CART_DASH_ATTACK"
		}
	},
	CT_SMITH_CART_DASH_ATTACK = {
		EVENT_INTERVAL_ID = 0,
		RATE = 100
	}
}
SMITH_CART_TURN = {
	ANIM_NAME = "Turn",
	ANIM_SPEED = 1.5,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = TRUE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	SUPER_ARMOR = TRUE,
	FLIP_DIR_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE,
	NEVER_MOVE = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SMITH_CART_WAIT"
		}
	}
}

function SMITH_CART_TURN_STATE_START(slot0, slot1, slot2)
	slot2:SetFlag_LUA(0, false)
end

SMITH_CART_DASH_ATTACK = {
	PASSIVE_SPEED_X = 400,
	ANIM_NAME = "Dash",
	ANIM_SPEED = 1.25,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP,
	TRANSITION = FALSE,
	LAND_CONNECT = TRUE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"SMITH_CART_DASH_ATTACK_END",
			"CF_SMITH_CART_DASH_ATTACK_END"
		}
	},
	ATTACK_TIME0 = {
		0.01,
		100
	},
	DAMAGE_DATA = {
		CAMERA_CRASH_GAP = 5,
		BACK_SPEED_Y = 0,
		CAMERA_CRASH_TIME = 0.2,
		HIT_GAP = 0.5,
		BACK_SPEED_X = 400,
		DAMAGE_TYPE = DAMAGE_TYPE.DT_PHYSIC,
		HIT_TYPE = HIT_TYPE.HT_SWORD_SLASH,
		REACT_TYPE = REACT_TYPE.RT_DOWN,
		DAMAGE = {
			PHYSIC = 0.5
		},
		RE_ATTACK = TRUE
	}
}
SMITH_CART_DASH_ATTACK_END = {
	ANIM_NAME = "DashEnd",
	ANIM_SPEED = 1.5,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = TRUE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	NEVER_MOVE = TRUE,
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SMITH_CART_TURN"
		}
	}
}
SMITH_CART_DAMAGE = {
	PASSIVE_SPEED_X = 0,
	ANIM_SPEED = 0.75,
	ANIM_NAME = "Damage",
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = TRUE,
	CAN_PUSH_UNIT = TRUE,
	CAN_PASS_UNIT = FALSE,
	ALLOW_DIR_CHANGE = FALSE,
	IMMADIATE_PACKET_SEND = TRUE,
	SUPER_ARMOR = TRUE,
	NEVER_MOVE = TRUE,
	SOUND_PLAY0 = {
		0.035,
		"LittleWorker_Voice_DamageA01.ogg"
	},
	EVENT_PROCESS = {
		{
			STATE_CHANGE_TYPE.SCT_CONDITION_FUNCTION,
			"SMITH_CART_TURN",
			"CF_SMITH_CART_TURN"
		},
		{
			STATE_CHANGE_TYPE.SCT_MOTION_END,
			"SMITH_CART_DASH_ATTACK"
		}
	}
}
SMITH_CART_DYING_LAND = {
	PASSIVE_SPEED_X = 0,
	ANIM_NAME = "Damage",
	PASSIVE_SPEED_Y = 0,
	PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_ONE_WAIT,
	TRANSITION = FALSE,
	LAND_CONNECT = TRUE,
	INVINCIBLE = {
		0,
		100
	},
	CAN_PUSH_UNIT = FALSE,
	CAN_PASS_UNIT = TRUE,
	DYING_END = TRUE,
	IMMADIATE_PACKET_SEND = TRUE
}

function CF_SMITH_CART_DASH_ATTACK_END(slot0, slot1, slot2)
	if slot2:GetStateTime() < 0.1 then
		return false
	end

	-- SERV_IRUHADEV_RANOX_PORT (2026-09-29): elstools' decompile dropped this
	-- assignment (read again by the second `slot3 == false` check below) and
	-- inlined the call into the first condition only; the port carried the
	-- bug verbatim. Bytecode-confirmed: decompile_verdicts.tsv,
	-- SMITH_CART.src.lua::CF_SMITH_CART_DASH_ATTACK_END, dropped-assignment,
	-- seed 2, POISON_READ slot3@L284 (128-seed harness).
	slot3 = slot2:GetIsRight()
	slot4 = slot2:GetLineGroupStartPos()
	slot6 = false

	if slot3 == true and slot2:GetDistanceFrom(slot2:GetLineGroupEndPos()) < 280 then
		slot6 = true
	end

	if slot3 == false and slot2:GetDistanceFrom(slot4) < 280 then
		slot6 = true
	end

	slot2:SetFlag_LUA(0, slot6)

	return slot6
end

function CF_SMITH_CART_TURN(slot0, slot1, slot2)
	return slot2:GetFlag_LUA(0)
end

function SMITH_CART_DASH_ATTACK_FRAME_MOVE(slot0, slot1, slot2)
	pMinorParticle = slot1:GetMinorParticle()

	if slot2:AnimTimer_LUA(0.01) then
		vRotateDegree = slot2:GetRotateDegree()
		vPos = slot2:GetPos()
		vLandPos = slot2:GetLandPosition_LUA()
		bIsRight = slot2:GetIsRight()
		vDirVector = slot2:GetDirVector()
		vZVector = slot2:GetZVector()

		if bIsRight == true then
			vPos = MovePos(vPos, vDirVector, 330)
		else
			vPos = MovePos(vPos, vDirVector, -330)
		end

		vPos = MovePos(vPos, vZVector, -80)
		pParticle = slot1:CreateApplyAlphaParticleSequence_LUA(false, slot2, "Stone_LizardMan_High_WaitStart", vPos, D3DXVECTOR2(-1, -1), D3DXVECTOR2(-1, -1))

		if pParticle ~= nil then
			pParticle:SetAddRotate(vRotateDegree)
			pParticle:SetAxisAngle(vRotateDegree)
			pParticle:SetLandPosition(vLandPos.y)
		end
	end

	if slot2:AnimTimer_LUA(0.25) then
		vRotateDegree = slot2:GetRotateDegree()
		vPos = slot2:GetPos()
		vLandPos = slot2:GetLandPosition_LUA()
		bIsRight = slot2:GetIsRight()
		vDirVector = slot2:GetDirVector()
		vZVector = slot2:GetZVector()

		if bIsRight == true then
			vPos = MovePos(vPos, vDirVector, 330)
		else
			vPos = MovePos(vPos, vDirVector, -330)
		end

		vPos = MovePos(vPos, vZVector, -80)
		pParticle = slot1:CreateApplyAlphaParticleSequence_LUA(false, slot2, "Stone_LizardMan_High_WaitStart", vPos, D3DXVECTOR2(-1, -1), D3DXVECTOR2(-1, -1))

		if pParticle ~= nil then
			pParticle:SetAddRotate(vRotateDegree)
			pParticle:SetAxisAngle(vRotateDegree)
			pParticle:SetLandPosition(vLandPos.y)
		end
	end

	if slot2:AnimTimer_LUA(0.133) then
		slot2:PlaySound_LUA("Nasod_Railer_Walk.ogg")

		vRotateDegree = slot2:GetRotateDegree()
		vBonePos = slot2:GetBonePos_LUA("Dummy1_Bag")
		bIsRight = slot2:GetIsRight()
		vDirVector = slot2:GetDirVector()

		if bIsRight == true then
			vBonePos = MovePos(vBonePos, vDirVector, -20)
		else
			vBonePos = MovePos(vBonePos, vDirVector, 20)
		end

		pParticle = slot1:CreateApplyAlphaParticleSequence_LUA(false, slot2, "Smoke_Nasod_MinerThin_Attack", vBonePos, D3DXVECTOR2(-1, -1), D3DXVECTOR2(-1, -1))

		if pParticle ~= nil then
			pParticle:SetAddRotate(vRotateDegree)
			pParticle:SetAxisAngle(vRotateDegree)
			slot2:SetMinorParticle_LUA(0, pParticle:GetHandle())
		end
	end

	pParticle = slot2:GetMinorParticle_LUA(0)

	if pParticle ~= nil then
		if pMinorParticle:IsLiveInstance(pParticle) == true then
			vBonePos = slot2:GetBonePos_LUA("Dummy1_Bag")

			pParticle:SetPosition(vBonePos)
		else
			slot2:ClearMinorParticle_LUA(0)
		end
	end
end

function SMITH_CART_DASH_ATTACK_STATE_END(slot0, slot1, slot2)
	pMinorParticle = slot1:GetMinorParticle()
	pParticle = slot2:GetMinorParticle_LUA(0)

	if pParticle ~= nil then
		pMinorParticle:DestroyInstance(pParticle)
		slot2:ClearMinorParticle_LUA(0)
	end
end

function SMITH_CART_DAMAGE_FRAME_MOVE(slot0, slot1, slot2)
	if slot2:AnimEventTimer_LUA(0.047) then
		pMinorParticle = slot1:GetMinorParticle()

		pMinorParticle:GameUnitCreateSequence_LUA(slot2, "StepSmoke", slot2:GetLandPosition_LUA(), D3DXVECTOR2(100, 100), D3DXVECTOR2(5, -1))
	end
end

function MovePos(slot0, slot1, slot2)
	slot0.x = slot0.x + slot2 * slot1.x
	slot0.y = slot0.y + slot2 * slot1.y
	slot0.z = slot0.z + slot2 * slot1.z

	return slot0
end
