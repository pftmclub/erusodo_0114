PVE_AREA_TYPE = {
	PAT_BASE_HAMEL = 1,
	PAT_LINE_BOTTOM = 5,
	PAT_END = 6,
	PAT_LINE_MID = 4,
	PAT_BASE_VELDER = 2,
	PAT_NONE = 0,
	PAT_LINE_TOP = 3
}

g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_NONE, 1)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_E, 3.5)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_D, 4)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_C, 4.5)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_B, 5)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_A, 5.5)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_S, 6)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_SS, 6.5)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_SSS, 7)
g_pData:AddPveReferData_LUA(PVE_RANK.PVE_RANK_X, 7.5)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_NONE, 0)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_E, 0)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_D, 0)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_C, 0)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_B, 0)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_A, 0)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_S, 1)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_SS, 3)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_SSS, 5)
g_pData:AddPveReferDataForMP_LUA(PVE_RANK.PVE_RANK_X, 6)
g_pData:SetSendPveGameSyncPacketTimer_LUA(10)
g_pData:SetPveBalanceBuffGiveScore_LUA(18)
g_pData:SetPveGameTimeStopDistance_LUA(1)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(47, PVE_AREA_TYPE.PAT_LINE_MID)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(48, PVE_AREA_TYPE.PAT_LINE_MID)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(82, PVE_AREA_TYPE.PAT_LINE_MID)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(93, PVE_AREA_TYPE.PAT_LINE_MID)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(4, PVE_AREA_TYPE.PAT_LINE_TOP)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(75, PVE_AREA_TYPE.PAT_LINE_TOP)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(60, PVE_AREA_TYPE.PAT_LINE_TOP)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(10, PVE_AREA_TYPE.PAT_LINE_TOP)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(2, PVE_AREA_TYPE.PAT_LINE_BOTTOM)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(66, PVE_AREA_TYPE.PAT_LINE_BOTTOM)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(55, PVE_AREA_TYPE.PAT_LINE_BOTTOM)
g_pData:SetPveAreaTypeByPortalLineMap_LUA(6, PVE_AREA_TYPE.PAT_LINE_BOTTOM)
