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
