-- The HP-ratio shape: a division stored once and compared with <= in two
-- separate later ifs (not an elseif; that variant is kenta.lua's).

function hp_two_ifs(npc)
	local r = npc:GetNowHP() / npc:GetMaxHP()
	if r <= 0.6 then
		npc:Do(60)
	end
	if r <= 0.2 then
		npc:Do(20)
	end
end

function hp_two_ifs_and(npc)
	local r = npc:GetNowHP() / npc:GetMaxHP()
	if npc:GetFlag(1) == false and r <= 0.6 then
		npc:Do(60)
	end
	if npc:GetFlag(2) == false and r <= 0.2 then
		npc:Do(20)
	end
end
