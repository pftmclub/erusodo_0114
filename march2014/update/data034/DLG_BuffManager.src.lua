g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_NORMAL_1)
g_pUIDialog:SetPos(720, 2)
g_pUIDialog:AddDummyPos(D3DXVECTOR3(993, 26, 0))
g_pUIDialog:AddDummyPos(D3DXVECTOR3(993, 2, 0))

g_pStaticOver = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticOver)
g_pStaticOver:SetName("MouseOver")
