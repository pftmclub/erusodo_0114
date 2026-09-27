-- ljdiff self-test (python ljdiff.py --selftest). Plain source compiled with
-- luajit -b and traced as one side; ljdiff.py checks the events listed in
-- SELFTEST_EXPECT. Written the way ljd prints a stripped round trip: bare
-- slotN globals, no `local`.

-- nested-frame clobber, caller had its own value
function inner(slot0)
	slot1 = 7
	return slot1
end
function outer_had(slot0)
	slot1 = 5
	inner(slot0)
	return slot1
end
-- caller never wrote it: the callee's write masks what would be a poison read
function outer_unset(slot0)
	inner(slot0)
	return slot1
end

-- engine re-entry: RunCallback is listed synchronous in selftest/reentrant.tsv
function cb(slot0, slot1, slot2)
	slot3 = 9
end
function user(slot0, slot1, slot2)
	slot3 = 1
	slot2:RunCallback("cb")
	return slot3
end

-- an unwritten slotN
function poison()
	return slot9
end

-- static learner: x:M() == false  ->  ret IsReady boolean
function flagged(slot0, slot1, slot2)
	if slot2:IsReady() == false then
		return 1
	end
	return 2
end

-- compare attribution: the ISLE operand, not the newer GetOther result
function ordered(slot0, slot1, slot2)
	slot3 = slot2:GetLevel()
	slot4 = slot2:GetOther()
	if slot3 <= 3 then
		return slot4
	end
	return 0
end

-- seeded __eq between two nodes
function states(slot0, slot1, slot2)
	if slot2:GetNowStateID() == slot2:GetStateID_LUA("WAIT") then
		return "match"
	end
	return "none"
end

-- a function held in a main-chunk table, under a mixed key path
CALLBACKS = {
	{
		run = function(slot0)
			slot5 = 41
			return slot5 + 1
		end
	}
}

-- state-table re-entry (CX2SquareUnit::StateChange_LUA's shape): a global
-- table per state, FUNC_STATE_START/END naming the callbacks, called as
-- (g_pKTDXApp, g_pMain, game, this)
WALK = { FUNC_STATE_START = "start_walk" }
JUMP = { FUNC_STATE_START = "start_jump", FUNC_STATE_END = "end_jump" }
function start_walk(slot0, slot1, slot2, slot3)
	slot4 = 2
end
function start_jump(slot0, slot1, slot2, slot3)
end
function end_jump(slot0, slot1, slot2, slot3)
	slot3:Landed()
end
function square(slot0, slot1, slot2, slot3)
	slot4 = 1
	slot3:ChangeState("JUMP")
	slot3:ChangeState("WALK")
	slot3:ChangeState("NOT_A_STATE")
	slot3:Deferred("WALK")
	slot3:OpenDialog("DLG_Something.lua")
	return slot4
end

-- a table global nested deeper than fmt()'s depth-8 display cap (main_values)
DEEP = { a = { b = { c = { d = { e = { f = { g = { h = { i = { j = 1 } } } } } } } } } }

-- Phase 2b. A main-chunk temp (slot7) read by a function that never writes it
-- is SHADOW_READ; shadow_self writes slot8 before reading it, so it is not.
slot7 = 5
slot8 = 6
function shadow_main()
	return slot7
end
function shadow_self()
	slot8 = 1
	return slot8
end

-- the static must-define pass: must_ok writes before every read, must_both
-- writes on both arms, must_loop writes only on some iterations of a loop
function must_ok(slot0)
	slot1 = slot0:Get()
	if slot1 then
		slot2 = 1
	end
	return slot1
end
function must_both(slot0)
	if slot0:Get() then
		slot1 = 1
	else
		slot1 = 2
	end
	return slot1
end
function must_loop(slot0)
	for slot1 = 1, 3 do
		if slot0:Get() then
			slot2 = slot1
		end
	end
	return slot2
end

-- an engine table indexed with a nil key reads nil, as any Lua table does
-- (it was a harness error before Phase 2b: EVENT_SHADOW_MIRROR_FD.lua)
SHORT_LIST = { 1, 2 }
function nil_key()
	return ENGINE_ENUM[SHORT_LIST[9]]
end

-- two draws of one boolean method in one call are independent (before Phase
-- 2b's fmix32 they were always equal: FNV-1a's parity bit)
function two_flags(slot0)
	return slot0:IsReady(), slot0:IsReady()
end
