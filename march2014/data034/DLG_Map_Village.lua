Village = {
	[VILLAGE_MAP_ID.VMI_RUBEN] = {
		DLGBack = "DLG_Map_Village_Ruben_Back.lua",
		DLGFront = "DLG_Map_Village_Ruben_Front.lua",
		NAME = STR_ID_1112,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_RUBEN,
		HouseIDList = {
			HOUSE_ID.HI_ANNE_HOUSE,
			HOUSE_ID.HI_HAGERS_HOUSE,
			HOUSE_ID.HI_LOW_TRAINNING_ZONE,
			HOUSE_ID.HI_POSTBOX
		},
		WORLD_ID = WORLD_ID.WI_VILLAGE_RUBEN,
		CAMERA = {
			1500,
			230,
			300
		},
		START_POS = {
			{
				StartPosX = 655.438,
				StartPosY = 151.576,
				StartPosId = 1,
				StartPosZ = -682.745,
				IsSummon = TRUE
			},
			{
				StartPosX = 5454.83,
				StartPosY = 123.391,
				StartPosId = 2,
				StartPosZ = -488.52,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 5454.83,
				StartPosY = 123.391,
				StartPosId = 50000,
				StartPosZ = -488.52,
				IsSummon = TRUE
			},
			{
				StartPosX = 3744.78,
				StartPosY = 58.1,
				StartPosId = 3,
				StartPosZ = -1.66,
				IsWarp = TRUE
			},
			{
				StartPosX = 5643.992,
				StartPosY = 123.391,
				StartPosId = 4,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -616.398,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RUBEN_FIELD_01
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_GATE_RUBEN] = {
		DLGBack = "DLG_Map_Village_Ruben_Back.lua",
		DLGFront = "DLG_Map_Village_Ruben_Front.lua",
		NAME = STR_ID_1113,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_RUBEN,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_BILLBOARD
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_GATE_RUBEN,
		CAMERA = {
			1500,
			150,
			280
		},
		START_POS = {
			{
				StartPosX = -3485.31,
				StartPosY = 0,
				StartPosId = 10,
				StartPosZ = -427.52,
				IsSummon = TRUE
			},
			{
				StartPosX = 3557.85,
				StartPosY = 0,
				StartPosId = 11,
				StartPosZ = -439.96,
				IsSummon = TRUE
			},
			{
				StartPosX = -3798.921,
				StartPosY = 0,
				StartPosId = 12,
				StartPosZ = -481.299,
				LinkPos = {
					2
				}
			},
			{
				StartPosX = 3798.921,
				StartPosY = 0,
				StartPosId = 13,
				StartPosZ = -481.299,
				LinkPos = {
					20
				}
			},
			{
				StartPosX = -1751.86,
				StartPosY = -130.28,
				StartPosId = 14,
				StartPosZ = 0,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_ELDER] = {
		DLGBack = "DLG_Map_Village_Elder_Back.lua",
		DLGFront = "DLG_Map_Village_Elder_Front.lua",
		NAME = STR_ID_1114,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ELDER,
		HouseIDList = {
			HOUSE_ID.HI_ECHO_ALCHEMIST_HOUSE,
			HOUSE_ID.HI_HOFMANN_HOUSE,
			HOUSE_ID.HI_LUICHEL_HOUSE,
			HOUSE_ID.HI_LENPAD_HOUSE,
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		WORLD_ID = WORLD_ID.WI_VILLAGE_ELDER,
		CAMERA = {
			1500,
			230,
			300
		},
		START_POS = {
			{
				StartPosX = -8041,
				StartPosY = 1400,
				StartPosId = 20,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 7870,
				StartPosY = 1400,
				StartPosId = 21,
				StartPosZ = 0,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -8499,
				StartPosY = 1400,
				StartPosId = 22,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 0,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RUBEN_FIELD_01
			},
			{
				StartPosX = 8521,
				StartPosY = 1400,
				StartPosId = 23,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 0,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_01
			},
			{
				StartPosX = -314.1,
				StartPosY = 372.38,
				StartPosId = 26,
				StartPosZ = -483.65,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_GATE_ELDER] = {
		DLGBack = "DLG_Map_Village_Elder_Back.lua",
		DLGFront = "DLG_Map_Village_Elder_Front.lua",
		NAME = STR_ID_1115,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ELDER,
		WORLD_ID = WORLD_ID.WI_DUNGEON_GATE_ELDER,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_BILLBOARD
		},
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 400,
				StartPosY = 1241,
				StartPosId = 30,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 18850,
				StartPosY = 1000,
				StartPosId = 31,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 0,
				StartPosY = 1241,
				StartPosId = 32,
				StartPosZ = 0,
				LinkPos = {
					21
				}
			},
			{
				StartPosX = 19250,
				StartPosY = 1000,
				StartPosId = 33,
				StartPosZ = 0,
				LinkPos = {
					40
				}
			},
			{
				StartPosX = 3617.75,
				StartPosY = 1000,
				StartPosId = 34,
				StartPosZ = 0,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BESMA] = {
		DLGBack = "DLG_Map_Village_Besma_Back.lua",
		DLGFront = "DLG_Map_Village_Besma_Front.lua",
		NAME = STR_ID_1116,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_BESMA,
		HouseIDList = {
			HOUSE_ID.HI_CHACHABUK_HOUSE,
			HOUSE_ID.HI_RICHANG_HOUSE,
			HOUSE_ID.HI_STELLA_HOUSE,
			HOUSE_ID.HI_TOMA_HOUSE,
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		WORLD_ID = WORLD_ID.WI_VILLAGE_BESMA,
		CAMERA = {
			1500,
			230,
			300
		},
		START_POS = {
			{
				StartPosX = -901.18,
				StartPosY = 5022.37,
				StartPosId = 40,
				StartPosZ = -3430,
				IsSummon = TRUE
			},
			{
				StartPosX = 10349.5,
				StartPosY = 5505.35,
				StartPosId = 41,
				StartPosZ = -3430,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -1475.44,
				StartPosY = 5021.27,
				StartPosId = 42,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -3430,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_02
			},
			{
				StartPosX = 10888.6,
				StartPosY = 5505.35,
				StartPosId = 43,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -3430.23,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_01
			},
			{
				StartPosX = 3699.12,
				StartPosY = 5024.24,
				StartPosId = 46,
				StartPosZ = -3430.23,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_GATE_BESMA] = {
		DLGBack = "DLG_Map_Village_Elder_Back.lua",
		DLGFront = "DLG_Map_Village_Elder_Front.lua",
		NAME = STR_ID_1117,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_BESMA,
		WORLD_ID = WORLD_ID.WI_DUNGEON_GATE_BESMA,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_BILLBOARD
		},
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 853.747,
				StartPosY = 1000,
				StartPosId = 50,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 7942.95,
				StartPosY = 60,
				StartPosId = 51,
				StartPosZ = -1100,
				IsSummon = TRUE
			},
			{
				StartPosX = 9812.35,
				StartPosY = 1610,
				StartPosId = 52,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 28.2639,
				StartPosY = 1000,
				StartPosId = 53,
				StartPosZ = 0,
				LinkPos = {
					41
				}
			},
			{
				StartPosX = 8626,
				StartPosY = 50,
				StartPosId = 54,
				StartPosZ = -1100,
				LinkPos = {
					70
				}
			},
			{
				StartPosX = 10380,
				StartPosY = 1660,
				StartPosId = 55,
				StartPosZ = 0,
				LinkPos = {
					60
				}
			},
			{
				StartPosX = 2326.64,
				StartPosY = 1000,
				StartPosId = 56,
				StartPosZ = 0,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_ALTERA] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1118,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ALTERA_ISLAND,
		HouseIDList = {
			HOUSE_ID.HI_ADEL_HOUSE,
			HOUSE_ID.HI_AMOS_ALCHEMIST_HOUSE,
			HOUSE_ID.HI_AGATHA_EQUIP_SHOP_HOUSE,
			HOUSE_ID.HI_AIDA_ACCESSORY_HOUSE,
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		WORLD_ID = WORLD_ID.WI_VILLAGE_ALTERA,
		CAMERA = {
			1500,
			250,
			300
		},
		START_POS = {
			{
				StartPosX = 23229.75,
				StartPosY = 960,
				StartPosId = 60,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 35078.5,
				StartPosY = 960,
				StartPosId = 61,
				StartPosZ = 0,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 22783,
				StartPosY = 960,
				StartPosId = 62,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 0,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_03
			},
			{
				StartPosX = 35520,
				StartPosY = 960,
				StartPosId = 63,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 0,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_01
			},
			{
				StartPosX = 27986.67,
				StartPosY = 240,
				StartPosId = 66,
				StartPosZ = -427,
				IsWarp = TRUE
			},
			{
				StartPosX = 36174,
				StartPosY = 409,
				StartPosId = 67,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -193,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_03
			},
			{
				StartPosX = 35812.22,
				StartPosY = 409,
				StartPosId = 68,
				StartPosZ = -193,
				IsSummon = TRUE,
				IsRight = FALSE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_PEITA] = {
		NAME = STR_ID_1119,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_PEITA,
		HouseIDList = {
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_ALLEGRO_PEITA_HOUSE,
			HOUSE_ID.HI_LENTO_PEITA_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		WORLD_ID = WORLD_ID.WI_VILLAGE_PEITA,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -13869.24,
				StartPosY = 1457,
				StartPosId = 70,
				StartPosZ = -1574,
				IsSummon = TRUE
			},
			{
				StartPosX = -1914,
				StartPosY = 1137,
				StartPosId = 71,
				StartPosZ = -1476,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -14120,
				StartPosY = 1457,
				StartPosId = 72,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -1574,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_01
			},
			{
				StartPosX = -7562.446,
				StartPosY = 437.599,
				StartPosId = 73,
				StartPosZ = -3705.317,
				IsWarp = TRUE
			},
			{
				StartPosX = -1584,
				StartPosY = 1137,
				StartPosId = 74,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -1350,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_02
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_GATE_ALTERA] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1120,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ALTERA_ISLAND,
		WORLD_ID = WORLD_ID.WI_DUNGEON_GATE_ALTERA,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_BILLBOARD
		},
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 515.295,
				StartPosY = 1000,
				StartPosId = 80,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 9402.67,
				StartPosY = 1000,
				StartPosId = 81,
				StartPosZ = 0,
				IsSummon = TRUE
			},
			{
				StartPosX = 0.001,
				StartPosY = 1000,
				StartPosId = 82,
				StartPosZ = 0,
				LinkPos = {
					61
				}
			},
			{
				StartPosX = 2400,
				StartPosY = 1000,
				StartPosId = 83,
				StartPosZ = 0,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_VELDER] = {
		NAME = STR_ID_4622,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_VELDER,
		WORLD_ID = WORLD_ID.WI_VILLAGE_VELDER,
		HouseIDList = {
			HOUSE_ID.HI_GRAIL,
			HOUSE_ID.HI_NOEL,
			HOUSE_ID.HI_VANESSA,
			HOUSE_ID.HI_PRAUS,
			HOUSE_ID.HI_HANNA,
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		CAMERA = {
			1500,
			250,
			250
		},
		START_POS = {
			{
				StartPosX = -4115.85,
				StartPosY = 475,
				StartPosId = 90,
				StartPosZ = 80.3739,
				IsSummon = TRUE
			},
			{
				StartPosX = 6885.72,
				StartPosY = 467,
				StartPosId = 91,
				StartPosZ = -567.6,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -4551.87,
				StartPosY = 475,
				StartPosId = 92,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 142.971,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_03
			},
			{
				StartPosX = 7379,
				StartPosY = 467,
				StartPosId = 93,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -641,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_01
			},
			{
				StartPosX = 459.2,
				StartPosY = 259,
				StartPosId = 95,
				StartPosZ = -237,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_GATE_VELDER] = {
		NAME = STR_ID_4623,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_VELDER,
		WORLD_ID = WORLD_ID.WI_DUNGEON_GATE_VELDER,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_BILLBOARD
		},
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -3461.98,
				StartPosY = 10,
				StartPosId = 100,
				StartPosZ = -601,
				IsSummon = TRUE
			},
			{
				StartPosX = -4000,
				StartPosY = 10,
				StartPosId = 102,
				StartPosZ = -601,
				LinkPos = {
					91
				}
			},
			{
				StartPosX = 3140,
				StartPosY = 10,
				StartPosId = 103,
				StartPosZ = -601,
				IsSummon = TRUE
			},
			{
				StartPosX = 3500,
				StartPosY = 10,
				StartPosId = 104,
				StartPosZ = -601,
				LinkGateDungeon = DUNGEON_ID.DI_BATTLE_SHIP_VELDER
			},
			{
				StartPosX = -1468.23,
				StartPosY = 10,
				StartPosId = 105,
				StartPosZ = -601,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_HAMEL] = {
		NAME = STR_ID_10232,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_HAMEL,
		WORLD_ID = WORLD_ID.WI_VILLAGE_HAMEL,
		HouseIDList = {
			HOUSE_ID.HI_HORATIO,
			HOUSE_ID.HI_LUCY,
			HOUSE_ID.HI_DAISY,
			HOUSE_ID.HI_PENENSIO,
			HOUSE_ID.HI_DENKA,
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		CAMERA = {
			1500,
			230,
			300
		},
		START_POS = {
			{
				StartPosX = -7889.64,
				StartPosY = 585,
				StartPosId = 110,
				StartPosZ = 1461,
				IsSummon = TRUE
			},
			{
				StartPosX = 7568.44,
				StartPosY = 585,
				StartPosId = 111,
				StartPosZ = 1291,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -8242,
				StartPosY = 585,
				StartPosId = 112,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 1461,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_SHIP_STAGE
			},
			{
				StartPosX = 8000,
				StartPosY = 585,
				StartPosId = 113,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 1291,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_01
			},
			{
				StartPosX = 731.6,
				StartPosY = 527.72,
				StartPosId = 115,
				StartPosZ = 88.21,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_GATE_HAMEL] = {
		NAME = STR_ID_10233,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_HAMEL,
		WORLD_ID = WORLD_ID.WI_DUNGEON_GATE_HAMEL,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_BILLBOARD
		},
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -11318.8,
				StartPosY = 5084,
				StartPosId = 121,
				StartPosZ = -252,
				IsSummon = TRUE
			},
			{
				StartPosX = -11531,
				StartPosY = 5084,
				StartPosId = 123,
				StartPosZ = -252,
				LinkPos = {
					111
				}
			},
			{
				StartPosX = -8094,
				StartPosY = 4844,
				StartPosId = 125,
				StartPosZ = -252,
				IsWarp = TRUE
			},
			{
				StartPosX = -2369.61,
				StartPosY = 5084,
				StartPosId = 126,
				StartPosZ = -252,
				LinkPos = {
					130
				}
			},
			{
				StartPosX = -2640.42,
				StartPosY = 5084,
				StartPosId = 127,
				StartPosZ = -252,
				IsSummon = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_SANDER] = {
		NAME = STR_ID_22048,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_SANDER,
		WORLD_ID = WORLD_ID.WI_VILLAGE_SANDER,
		HouseIDList = {
			HOUSE_ID.HI_ROSEANG,
			HOUSE_ID.HI_EMIRATE,
			HOUSE_ID.HI_VAPOR,
			HOUSE_ID.HI_DAPPAR,
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK
		},
		CAMERA = {
			1500,
			230,
			300
		},
		START_POS = {
			{
				StartPosX = -10468.64,
				StartPosY = 1000,
				StartPosId = 130,
				StartPosZ = -8229,
				IsSummon = TRUE
			},
			{
				StartPosX = 10372,
				StartPosY = 1000,
				StartPosId = 131,
				StartPosZ = -8229.71,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -10717.74,
				StartPosY = 1000,
				StartPosId = 132,
				BattleFieldStartLineIndex = 3,
				StartPosZ = -8229.71,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_04
			},
			{
				StartPosX = 10717.74,
				StartPosY = 1000,
				StartPosId = 133,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -8229.71,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_01
			},
			{
				StartPosX = -1592.8,
				StartPosY = 1000,
				StartPosId = 135,
				StartPosZ = -8469,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_RANOX] = {
		NAME = STR_ID_32562,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_RANOX,
		WORLD_ID = WORLD_ID.WI_VILLAGE_RANOX,
		HouseIDList = {
			HOUSE_ID.HI_CAMILLA_PVP_HOUSE,
			HOUSE_ID.HI_ARIEL_EVENT_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_PRIVATE_BANK,
			HOUSE_ID.HI_EDEL,
			HOUSE_ID.HI_STEEL,
			HOUSE_ID.HI_FACEOFF,
			HOUSE_ID.HI_SYOTA,
			HOUSE_ID.HI_IGNIA,
			HOUSE_ID.HI_GUILD_MARK,
			HOUSE_ID.HI_STORY_QUEST,
			HOUSE_ID.HI_TRANSCEND_QUEST,
			HOUSE_ID.HI_ADAMS_HOUSE,
			HOUSE_ID.HI_SPA_SIGN_RANOX,
			HOUSE_ID.HI_MU,
			HOUSE_ID.HI_EVENT_GUILD_EDAN_VILAGE,
			HOUSE_ID.HI_EVENT_BLESS_OF_SOLACE
		},
		CAMERA = {
			1500,
			230,
			300
		},
		START_POS = {
			{
				StartPosX = -6848,
				StartPosY = 2460,
				StartPosId = 140,
				StartPosZ = 2617,
				IsSummon = TRUE
			},
			{
				StartPosX = 6995,
				StartPosY = 2460,
				StartPosId = 141,
				StartPosZ = 2617,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -7400,
				StartPosY = 2460,
				StartPosId = 142,
				BattleFieldStartLineIndex = 177,
				StartPosZ = 2617,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_04
			},
			{
				StartPosX = 7400,
				StartPosY = 2460,
				StartPosId = 143,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 2617,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_01
			},
			{
				StartPosX = -59,
				StartPosY = 880,
				StartPosId = 144,
				StartPosZ = -3093.42,
				IsWarp = TRUE
			},
			{
				StartPosX = -4106,
				StartPosY = 790,
				StartPosId = 146,
				OtherPlaceStartLineIndex = 1,
				StartPosZ = 280,
				IsSpaEnter = TRUE,
				OtherPlaceID = VILLAGE_MAP_ID.VMI_SPA_RANOX
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_RUBEN] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1121,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_RUBEN,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_RUBEN,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 10001,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EL_FOREST_WEST_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 960,
						StartPosX = 4912
					},
					{
						StartPosZ = 0,
						StartPosY = 960,
						StartPosX = 5342
					},
					{
						StartPosZ = 0,
						StartPosY = 1200,
						StartPosX = 4657
					},
					{
						StartPosZ = 0,
						StartPosY = 1200,
						StartPosX = 5597
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_RUBEN_EL_TREE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -580,
						StartPosY = 240,
						StartPosX = -1402
					},
					{
						StartPosZ = -580,
						StartPosY = 60,
						StartPosX = -972
					},
					{
						StartPosZ = -580,
						StartPosY = 100,
						StartPosX = -84
					},
					{
						StartPosZ = -580,
						StartPosY = 100,
						StartPosX = 61
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_RUBEN_RUIN_OF_ELF_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 142,
						StartPosX = -220
					},
					{
						StartPosZ = 0,
						StartPosY = 382,
						StartPosX = -880
					},
					{
						StartPosZ = 0,
						StartPosY = 142,
						StartPosX = 436
					},
					{
						StartPosZ = 0,
						StartPosY = 142,
						StartPosX = 778
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EL_FOREST_MAIN_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 340,
						StartPosX = 1473
					},
					{
						StartPosZ = 365,
						StartPosY = 100,
						StartPosX = 2218
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1143
					},
					{
						StartPosZ = 547,
						StartPosY = 100,
						StartPosX = 2396
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_RUBEN_SWAMP_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 546,
						StartPosY = 411,
						StartPosX = -132
					},
					{
						StartPosZ = 546,
						StartPosY = 171,
						StartPosX = 245
					},
					{
						StartPosZ = 639,
						StartPosY = 411,
						StartPosX = 826
					},
					{
						StartPosZ = 696,
						StartPosY = 411,
						StartPosX = 1028
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_NOVICE_FOREST_STAGE2,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 750
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1740
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 984
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1515
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EL_FOREST_EXTRA_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 1900,
						StartPosX = 4446
					},
					{
						StartPosZ = 0,
						StartPosY = 1420,
						StartPosX = 3422
					},
					{
						StartPosZ = 0,
						StartPosY = 1420,
						StartPosX = 4500
					},
					{
						StartPosZ = 0,
						StartPosY = 1660,
						StartPosX = 3100
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EL_FOREST_NORTH_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2647
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 3347
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2297
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 3697
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EL_FOREST_EXTRA_CAVE_STAGE5,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1843
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 2143
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1693
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 2293
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_TRAINING_CENTER_FREESTAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 700
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1000
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 400
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_CORE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 249,
						StartPosY = 1682,
						StartPosX = 1965
					},
					{
						StartPosZ = 270,
						StartPosY = 1682,
						StartPosX = 2507
					},
					{
						StartPosZ = 296,
						StartPosY = 1442,
						StartPosX = 1500
					},
					{
						StartPosZ = 350,
						StartPosY = 1442,
						StartPosX = 2953
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_ELDER] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1122,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ELDER,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_ELDER,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 10002,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_DRUID_FOREST_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 224,
						StartPosX = 2321
					},
					{
						StartPosZ = 0,
						StartPosY = 174,
						StartPosX = 2071
					},
					{
						StartPosZ = 0,
						StartPosY = 122,
						StartPosX = 1814
					},
					{
						StartPosZ = 0,
						StartPosY = 76,
						StartPosX = 1581
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_NATURE_CAVE_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1320
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1660
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1150
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1830
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_BENDERS_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 232,
						StartPosY = 24,
						StartPosX = 1807
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1301
					},
					{
						StartPosZ = 0,
						StartPosY = 580,
						StartPosX = 1297
					},
					{
						StartPosZ = 470,
						StartPosY = 154,
						StartPosX = 2147
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_BELOW_PASSAGE_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 126,
						StartPosX = 1733
					},
					{
						StartPosZ = 0,
						StartPosY = 127,
						StartPosX = 2463
					},
					{
						StartPosZ = 0,
						StartPosY = 380,
						StartPosX = 1668
					},
					{
						StartPosZ = 0,
						StartPosY = 380,
						StartPosX = 2527
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_WALLY_CASTLE_ROOF_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -900
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 900
					},
					{
						StartPosZ = 0,
						StartPosY = 480,
						StartPosX = -900
					},
					{
						StartPosZ = 0,
						StartPosY = 480,
						StartPosX = 900
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_WALLY_CASTLE_CENTER_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = -300
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = 300
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = -600
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = 600
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_WALLY_CASTLE_LAB_STAGE5,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 204,
						StartPosX = 8975
					},
					{
						StartPosZ = 0,
						StartPosY = 204,
						StartPosX = 9575
					},
					{
						StartPosZ = 0,
						StartPosY = 449,
						StartPosX = 8650
					},
					{
						StartPosZ = 0,
						StartPosY = 449,
						StartPosX = 9900
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_TRAINING_CENTER_FREESTAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 700
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1000
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 400
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_CORE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 249,
						StartPosY = 1682,
						StartPosX = 1965
					},
					{
						StartPosZ = 270,
						StartPosY = 1682,
						StartPosX = 2507
					},
					{
						StartPosZ = 296,
						StartPosY = 1442,
						StartPosX = 1500
					},
					{
						StartPosZ = 350,
						StartPosY = 1442,
						StartPosX = 2953
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ARCADE_NEW_STAGE_REST,
				USER_START_POS = {
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -460
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 460
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_TREE_DAY_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 10,
						StartPosY = 0,
						StartPosX = 1361
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1073
					},
					{
						StartPosZ = 136,
						StartPosY = 0,
						StartPosX = 1867
					},
					{
						StartPosZ = 391,
						StartPosY = 0,
						StartPosX = 2137
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_SECRET_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -504,
						StartPosY = 480,
						StartPosX = -1260.62
					},
					{
						StartPosZ = -504,
						StartPosY = 480,
						StartPosX = -1044
					},
					{
						StartPosZ = -504,
						StartPosY = 0,
						StartPosX = -165.992
					},
					{
						StartPosZ = -504,
						StartPosY = 0,
						StartPosX = 329.121
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_HELL_EXTRA_STAGE5,
				USER_START_POS = {
					{
						StartPosZ = -504,
						StartPosY = 480,
						StartPosX = -1260.62
					},
					{
						StartPosZ = -504,
						StartPosY = 480,
						StartPosX = -1044
					},
					{
						StartPosZ = -504,
						StartPosY = 0,
						StartPosX = -165.992
					},
					{
						StartPosZ = -504,
						StartPosY = 0,
						StartPosX = 329.121
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ELDER_SECRET_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -504,
						StartPosY = 480,
						StartPosX = -1260.62
					},
					{
						StartPosZ = -504,
						StartPosY = 480,
						StartPosX = -1044
					},
					{
						StartPosZ = -504,
						StartPosY = 0,
						StartPosX = -165.992
					},
					{
						StartPosZ = -504,
						StartPosY = 0,
						StartPosX = 329.121
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_HALLOWEEN_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 480,
						StartPosX = 640.894
					},
					{
						StartPosZ = 0,
						StartPosY = 480,
						StartPosX = 841.114
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1169.68
					},
					{
						StartPosZ = 0,
						StartPosY = 720,
						StartPosX = 1713.89
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_NORMAL_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_HARD_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_EXPERT_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 298
					},
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 646
					},
					{
						StartPosZ = -5062,
						StartPosY = 4418,
						StartPosX = 1090
					},
					{
						StartPosZ = -4960,
						StartPosY = 4418,
						StartPosX = 1423
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_BESMA] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1123,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_BESMA,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_BESMA,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 10003,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_DRAGON_ROAD_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 900
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1600
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 450
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 2050
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_NEW_BESMA_DRAGON_ROAD_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 725.035
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 317.919
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1670.77
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 2033.05
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_LAKE_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 520,
						StartPosY = 360,
						StartPosX = 1350
					},
					{
						StartPosZ = 520,
						StartPosY = 360,
						StartPosX = 1950
					},
					{
						StartPosZ = 410,
						StartPosY = 360,
						StartPosX = 967
					},
					{
						StartPosZ = 408,
						StartPosY = 360,
						StartPosX = 2335
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_LAKE_NIGHT_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 520,
						StartPosY = 360,
						StartPosX = 1350
					},
					{
						StartPosZ = 520,
						StartPosY = 360,
						StartPosX = 1950
					},
					{
						StartPosZ = 410,
						StartPosY = 360,
						StartPosX = 967
					},
					{
						StartPosZ = 408,
						StartPosY = 360,
						StartPosX = 2335
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_DRAGON_ROAD_CANYON_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 900
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1600
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 450
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 2050
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_LAKE_CANYON_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 520,
						StartPosY = 360,
						StartPosX = 1350
					},
					{
						StartPosZ = 520,
						StartPosY = 360,
						StartPosX = 1950
					},
					{
						StartPosZ = 410,
						StartPosY = 360,
						StartPosX = 967
					},
					{
						StartPosZ = 408,
						StartPosY = 360,
						StartPosX = 2335
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_DRAGON_NEST_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 1625
					},
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 2225
					},
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 1325
					},
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 2525
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_MINE_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = -1350
					},
					{
						StartPosZ = -187,
						StartPosY = 480,
						StartPosX = -1675
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = -1750
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = -940
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_MINE2_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = 752
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = 1160
					},
					{
						StartPosZ = -187,
						StartPosY = 480,
						StartPosX = 1160
					},
					{
						StartPosZ = -187,
						StartPosY = 0,
						StartPosX = 280
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_AIRSHIP_STAGE5,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 133,
						StartPosX = 2250
					},
					{
						StartPosZ = 0,
						StartPosY = 133,
						StartPosX = 2750
					},
					{
						StartPosZ = 0,
						StartPosY = 33,
						StartPosX = 1800
					},
					{
						StartPosZ = 0,
						StartPosY = 33,
						StartPosX = 3200
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_SECRET_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 1625
					},
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 2225
					},
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 1325
					},
					{
						StartPosZ = 519,
						StartPosY = 480,
						StartPosX = 2525
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_TRAINING_CENTER_FREESTAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 700
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1000
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 400
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_CORE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 249,
						StartPosY = 1682,
						StartPosX = 1965
					},
					{
						StartPosZ = 270,
						StartPosY = 1682,
						StartPosX = 2507
					},
					{
						StartPosZ = 296,
						StartPosY = 1442,
						StartPosX = 1500
					},
					{
						StartPosZ = 350,
						StartPosY = 1442,
						StartPosX = 2953
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ARCADE_NEW_STAGE_REST,
				USER_START_POS = {
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -460
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 460
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_TREE_DAY_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 10,
						StartPosY = 0,
						StartPosX = 1361
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1073
					},
					{
						StartPosZ = 136,
						StartPosY = 0,
						StartPosX = 1867
					},
					{
						StartPosZ = 391,
						StartPosY = 0,
						StartPosX = 2137
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_HELL_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 225,
						StartPosY = 720,
						StartPosX = 1013.6
					},
					{
						StartPosZ = 225,
						StartPosY = 479,
						StartPosX = 1402
					},
					{
						StartPosZ = 225,
						StartPosY = 479,
						StartPosX = 2388.8
					},
					{
						StartPosZ = 225,
						StartPosY = 720,
						StartPosX = 2694.8
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_HELL_EXTRA_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 225,
						StartPosY = 720,
						StartPosX = 1013.6
					},
					{
						StartPosZ = 225,
						StartPosY = 479,
						StartPosX = 1402
					},
					{
						StartPosZ = 225,
						StartPosY = 479,
						StartPosX = 2388.8
					},
					{
						StartPosZ = 225,
						StartPosY = 720,
						StartPosX = 2694.8
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_BESMA_HELL_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 225,
						StartPosY = 720,
						StartPosX = 1013.6
					},
					{
						StartPosZ = 225,
						StartPosY = 479,
						StartPosX = 1402
					},
					{
						StartPosZ = 225,
						StartPosY = 479,
						StartPosX = 2388.8
					},
					{
						StartPosZ = 225,
						StartPosY = 720,
						StartPosX = 2694.8
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_NORMAL_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_HARD_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_EXPERT_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 298
					},
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 646
					},
					{
						StartPosZ = -5062,
						StartPosY = 4418,
						StartPosX = 1090
					},
					{
						StartPosZ = -4960,
						StartPosY = 4418,
						StartPosX = 1423
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_ALTERA] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1124,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ALTERA_ISLAND,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_ALTERA,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 10004,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_BATTLE_AIR_SHIP_STAGE6,
				USER_START_POS = {
					{
						StartPosZ = -455,
						StartPosY = 3293,
						StartPosX = 2090
					},
					{
						StartPosZ = -455,
						StartPosY = 3293,
						StartPosX = 2950
					},
					{
						StartPosZ = -455,
						StartPosY = 3293,
						StartPosX = 1800
					},
					{
						StartPosZ = -455,
						StartPosY = 3533,
						StartPosX = 3186
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_PLAIN_RECYCLE_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2300
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2900
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2000
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 3200
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_PLAIN_RECYCLE_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1400
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2000
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1100
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_BELOW_TUNNEL_STAGE4,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1775
					},
					{
						StartPosZ = 0,
						StartPosY = 300,
						StartPosX = 2000
					},
					{
						StartPosZ = 0,
						StartPosY = 520,
						StartPosX = 1800
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2200
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_PLAIN_STAGE3,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2300
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2900
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 2000
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 3200
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_NASOD_FACTORY_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1165
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1825
					},
					{
						StartPosZ = 0,
						StartPosY = 380,
						StartPosX = 870
					},
					{
						StartPosZ = 0,
						StartPosY = 380,
						StartPosX = 2120
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_CORE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 249,
						StartPosY = 1682,
						StartPosX = 1965
					},
					{
						StartPosZ = 270,
						StartPosY = 1682,
						StartPosX = 2507
					},
					{
						StartPosZ = 296,
						StartPosY = 1442,
						StartPosX = 1500
					},
					{
						StartPosZ = 350,
						StartPosY = 1442,
						StartPosX = 2953
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_SECRET_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 1700
					},
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 2100
					},
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 1500
					},
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 2300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_HELL_EXTRA_STAGE5,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 1700
					},
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 2100
					},
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 1500
					},
					{
						StartPosZ = 0,
						StartPosY = 60,
						StartPosX = 2300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_TRAINING_CENTER_FREESTAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 700
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1000
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 400
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ALTERA_CORE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 249,
						StartPosY = 1682,
						StartPosX = 1965
					},
					{
						StartPosZ = 270,
						StartPosY = 1682,
						StartPosX = 2507
					},
					{
						StartPosZ = 296,
						StartPosY = 1442,
						StartPosX = 1500
					},
					{
						StartPosZ = 350,
						StartPosY = 1442,
						StartPosX = 2953
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ARCADE_NEW_STAGE_REST,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 700
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1000
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 400
					},
					{
						StartPosZ = 0,
						StartPosY = 240,
						StartPosX = 1300
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_TREE_DAY_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 10,
						StartPosY = 0,
						StartPosX = 1361
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1073
					},
					{
						StartPosZ = 136,
						StartPosY = 0,
						StartPosX = 1867
					},
					{
						StartPosZ = 391,
						StartPosY = 0,
						StartPosX = 2137
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_NORMAL_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_HARD_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_EXPERT_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 298
					},
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 646
					},
					{
						StartPosZ = -5062,
						StartPosY = 4418,
						StartPosX = 1090
					},
					{
						StartPosZ = -4960,
						StartPosY = 4418,
						StartPosX = 1423
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_PEITA] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_1125,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_PEITA,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_PEITA,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 10005,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_PEITA_OFFERINGS_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -281,
						StartPosY = 658,
						StartPosX = -2145
					},
					{
						StartPosZ = -281,
						StartPosY = 658,
						StartPosX = -1745
					},
					{
						StartPosZ = -284,
						StartPosY = 898,
						StartPosX = -2445
					},
					{
						StartPosZ = -278,
						StartPosY = 898,
						StartPosX = -1445
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_PEITA_SPIRAL_CORRIDOR_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -455,
						StartPosY = 1797,
						StartPosX = -2250
					},
					{
						StartPosZ = -455,
						StartPosY = 1797,
						StartPosX = -1750
					},
					{
						StartPosZ = -455,
						StartPosY = 1797,
						StartPosX = -2500
					},
					{
						StartPosZ = -455,
						StartPosY = 1797,
						StartPosX = -1500
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_PEITA_CHAPEL_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -275.047
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 277.607
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -547.22
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 521.071
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_PEITA_UNDER_GARDEN_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -1322,
						StartPosY = 1002,
						StartPosX = -260.603
					},
					{
						StartPosZ = -1322,
						StartPosY = 1002,
						StartPosX = 370.825
					},
					{
						StartPosZ = -1322,
						StartPosY = 762,
						StartPosX = -259.82
					},
					{
						StartPosZ = -1322,
						StartPosY = 762,
						StartPosX = 409.707
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_PEITA_TOWER_HEART_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 1671,
						StartPosX = -489
					},
					{
						StartPosZ = 0,
						StartPosY = 1671,
						StartPosX = -228
					},
					{
						StartPosZ = 0,
						StartPosY = 1671,
						StartPosX = 228
					},
					{
						StartPosZ = 0,
						StartPosY = 1671,
						StartPosX = 489
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_PEITA_OFFERINGS_ALTER_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 668,
						StartPosY = 4743,
						StartPosX = -489
					},
					{
						StartPosZ = 668,
						StartPosY = 4743,
						StartPosX = -228
					},
					{
						StartPosZ = 668,
						StartPosY = 4743,
						StartPosX = 228
					},
					{
						StartPosZ = 668,
						StartPosY = 4743,
						StartPosX = 489
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ARCADE_NEW_STAGE_REST,
				USER_START_POS = {
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -460
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 460
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_TREE_DAY_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 10,
						StartPosY = 0,
						StartPosX = 1361
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1073
					},
					{
						StartPosZ = 136,
						StartPosY = 0,
						StartPosX = 1867
					},
					{
						StartPosZ = 391,
						StartPosY = 0,
						StartPosX = 2137
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_NORMAL_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_HARD_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_EXPERT_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 298
					},
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 646
					},
					{
						StartPosZ = -5062,
						StartPosY = 4418,
						StartPosX = 1090
					},
					{
						StartPosZ = -4960,
						StartPosY = 4418,
						StartPosX = 1423
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_VELDER] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_4624,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_VELDER,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_VELDER,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 87,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_THIRD_DWELLING_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -1095,
						StartPosY = 0,
						StartPosX = 100
					},
					{
						StartPosZ = -1095,
						StartPosY = 0,
						StartPosX = 900
					},
					{
						StartPosZ = -1095,
						StartPosY = 240,
						StartPosX = 1420
					},
					{
						StartPosZ = -1095,
						StartPosY = 480,
						StartPosX = 1762.82
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_BRIDGE_HOPE_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = -5516,
						StartPosY = 1337,
						StartPosX = 4780
					},
					{
						StartPosZ = -5516,
						StartPosY = 1097,
						StartPosX = 4444
					},
					{
						StartPosZ = -5516,
						StartPosY = 1097,
						StartPosX = 5432
					},
					{
						StartPosZ = -5516,
						StartPosY = 1097,
						StartPosX = 5814
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_PALACE_ENTRANCE_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = -600
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = -376
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 133
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 407
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_BRIDGE_BURNING_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5516,
						StartPosY = 1577,
						StartPosX = 4469.18
					},
					{
						StartPosZ = -5516,
						StartPosY = 1577,
						StartPosX = 4399.55
					},
					{
						StartPosZ = -5516,
						StartPosY = 1577,
						StartPosX = 5428
					},
					{
						StartPosZ = -5516,
						StartPosY = 1577,
						StartPosX = 5723
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_MARKET_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 3343.59,
						StartPosY = 200,
						StartPosX = -384.499
					},
					{
						StartPosZ = 3343.33,
						StartPosY = 200,
						StartPosX = -69.0972
					},
					{
						StartPosZ = 3343.52,
						StartPosY = 200,
						StartPosX = 454.948
					},
					{
						StartPosZ = 3343.03,
						StartPosY = 200,
						StartPosX = 697.027
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_GATE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 1200,
						StartPosX = 2542
					},
					{
						StartPosZ = 0,
						StartPosY = 1200,
						StartPosX = 2992.76
					},
					{
						StartPosZ = 0,
						StartPosY = 1200,
						StartPosX = 3374.23
					},
					{
						StartPosZ = 0,
						StartPosY = 1200,
						StartPosX = 2147.22
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ARCADE_NEW_STAGE_REST,
				USER_START_POS = {
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -460
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 460
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_TREE_DAY_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 10,
						StartPosY = 0,
						StartPosX = 1361
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1073
					},
					{
						StartPosZ = 136,
						StartPosY = 0,
						StartPosX = 1867
					},
					{
						StartPosZ = 391,
						StartPosY = 0,
						StartPosX = 2137
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_NORMAL_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_HARD_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_EXPERT_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 298
					},
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 646
					},
					{
						StartPosZ = -5062,
						StartPosY = 4418,
						StartPosX = 1090
					},
					{
						StartPosZ = -4960,
						StartPosY = 4418,
						StartPosX = 1423
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_SECRET_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 1134
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 1610
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 621
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 268
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_VELDER_SECRET_EXTRA_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 1134
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 1610
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 621
					},
					{
						StartPosZ = 3171,
						StartPosY = 18,
						StartPosX = 268
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_HAMEL] = {
		DLGBack = "DLG_Map_Village_Altera_Back.lua",
		DLGFront = "DLG_Map_Village_Altera_Front.lua",
		NAME = STR_ID_13414,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_HAMEL,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE
		},
		WORLD_ID = WORLD_ID.WI_DUNGEON_LOUNGE_HAMEL,
		CAMERA = {
			1500,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 1287.78,
				StartPosY = 1000,
				StartPosId = 87,
				StartPosZ = 0,
				IsSummon = TRUE
			}
		},
		DUNGEON_LOUNGE_START_POS = {
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_RESIAM_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -2276,
						StartPosY = 205,
						StartPosX = -592
					},
					{
						StartPosZ = -2276,
						StartPosY = 205,
						StartPosX = -270
					},
					{
						StartPosZ = -2276,
						StartPosY = 205,
						StartPosX = 346
					},
					{
						StartPosZ = -2276,
						StartPosY = 205,
						StartPosX = 767
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_RESIAM_UNDERWATER_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -17,
						StartPosY = 451,
						StartPosX = 3291
					},
					{
						StartPosZ = -17,
						StartPosY = 451,
						StartPosX = 3552
					},
					{
						StartPosZ = -17,
						StartPosY = 451,
						StartPosX = 4064
					},
					{
						StartPosZ = -17,
						StartPosY = 451,
						StartPosX = 4325
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_WATERWAY_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -630
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -328
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 370
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 631
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_WATERWAY_CORE_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -261
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = -81
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 131
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 294
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_CAVE_OF_MAGMANTA_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -2251,
						StartPosY = 357,
						StartPosX = 599
					},
					{
						StartPosZ = -2251,
						StartPosY = 357,
						StartPosX = 924
					},
					{
						StartPosZ = -2251,
						StartPosY = 357,
						StartPosX = -563
					},
					{
						StartPosZ = -2251,
						StartPosY = 357,
						StartPosX = -893
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_FROZEN_WATER_TEMPLE_B0SS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -4277,
						StartPosY = 5740,
						StartPosX = -407
					},
					{
						StartPosZ = -4277,
						StartPosY = 5740,
						StartPosX = -659
					},
					{
						StartPosZ = -4277,
						StartPosY = 5740,
						StartPosX = 285
					},
					{
						StartPosZ = -4277,
						StartPosY = 5740,
						StartPosX = 549
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_HAMEL_FROZEN_WATER_HALL_BOSS_STAGE,
				USER_START_POS = {
					{
						StartPosZ = 40,
						StartPosY = 770,
						StartPosX = -440
					},
					{
						StartPosZ = 40,
						StartPosY = 770,
						StartPosX = -914
					},
					{
						StartPosZ = 40,
						StartPosY = 770,
						StartPosX = 662
					},
					{
						StartPosZ = 40,
						StartPosY = 770,
						StartPosX = 952
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_ARCADE_NEW_STAGE_REST,
				USER_START_POS = {
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 230
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = -460
					},
					{
						StartPosZ = -2,
						StartPosY = 240,
						StartPosX = 460
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_NORMAL_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_HARD_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -436
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = 246
					},
					{
						StartPosZ = -5126,
						StartPosY = 3697,
						StartPosX = -74
					},
					{
						StartPosZ = -5126,
						StartPosY = 3937,
						StartPosX = 19
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_DEFENCE_DUNGEON_EXPERT_STAGE,
				USER_START_POS = {
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 298
					},
					{
						StartPosZ = -5126,
						StartPosY = 4418,
						StartPosX = 646
					},
					{
						StartPosZ = -5062,
						StartPosY = 4418,
						StartPosX = 1090
					},
					{
						StartPosZ = -4960,
						StartPosY = 4418,
						StartPosX = 1423
					}
				}
			},
			{
				START_POS_WORLD_ID = WORLD_ID.WI_EVENT_TREE_DAY_STAGE_BOSS,
				USER_START_POS = {
					{
						StartPosZ = 10,
						StartPosY = 0,
						StartPosX = 1361
					},
					{
						StartPosZ = 0,
						StartPosY = 0,
						StartPosX = 1073
					},
					{
						StartPosZ = 136,
						StartPosY = 0,
						StartPosX = 1867
					},
					{
						StartPosZ = 391,
						StartPosY = 0,
						StartPosX = 2137
					}
				}
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_REST_00] = {
		NAME = STR_ID_19988,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ELDER,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_ELDER_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 13576.75,
				StartPosY = 2420,
				StartPosId = 300,
				StartPosZ = -4752,
				IsSummon = TRUE
			},
			{
				StartPosX = 13291,
				StartPosY = 2420,
				StartPosId = 301,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -4752,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_01
			},
			{
				StartPosX = 22512.43,
				StartPosY = 2420,
				StartPosId = 302,
				StartPosZ = -4752,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 22885,
				StartPosY = 2420,
				StartPosId = 303,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -4752,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_02
			},
			{
				StartPosX = 16177.383,
				StartPosY = 2445.425,
				StartPosId = 304,
				StartPosZ = -4752.352,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_REST_00] = {
		NAME = STR_ID_19991,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_BESMA,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_BESMA_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -2476,
				StartPosY = 1620,
				StartPosId = 310,
				StartPosZ = 7230,
				IsSummon = TRUE
			},
			{
				StartPosX = -3022,
				StartPosY = 1620,
				StartPosId = 311,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 7230,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_01
			},
			{
				StartPosX = 4051,
				StartPosY = 1860,
				StartPosId = 312,
				StartPosZ = 6895,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 4240,
				StartPosY = 1860,
				StartPosId = 313,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 6629,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_02
			},
			{
				StartPosX = 3974,
				StartPosY = 2100,
				StartPosId = 314,
				StartPosZ = 7230,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 4610,
				StartPosY = 2100,
				StartPosId = 315,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 7230,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_03
			},
			{
				StartPosX = 493.568,
				StartPosY = 2100,
				StartPosId = 316,
				StartPosZ = 7489.997,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_REST_00] = {
		NAME = STR_ID_19995,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_ALTERA_ISLAND,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_ALTERA_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 2612,
				StartPosY = 5617,
				StartPosId = 320,
				StartPosZ = -18672,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 2926,
				StartPosY = 5617,
				StartPosId = 321,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -18371,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_01
			},
			{
				StartPosX = -6438,
				StartPosY = 5617,
				StartPosId = 322,
				StartPosZ = -18300,
				IsSummon = TRUE
			},
			{
				StartPosX = -6725,
				StartPosY = 5617,
				StartPosId = 323,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -17961,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_02
			},
			{
				StartPosX = -510.316,
				StartPosY = 5377.844,
				StartPosId = 324,
				StartPosZ = -20579.938,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_REST_00] = {
		NAME = STR_ID_20002,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_VELDER,
		HouseIDList = {
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_POSTBOX,
			HOUSE_ID.HI_BILLBOARD
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_VELDER_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -12829.76,
				StartPosY = 900,
				StartPosId = 330,
				StartPosZ = 1262,
				IsSummon = TRUE
			},
			{
				StartPosX = -13452,
				StartPosY = 900,
				StartPosId = 331,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 1262,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_01
			},
			{
				StartPosX = -4465.22,
				StartPosY = 1333,
				StartPosId = 332,
				StartPosZ = 3475.49,
				IsSummon = TRUE
			},
			{
				StartPosX = -4465.22,
				StartPosY = 1376,
				StartPosId = 333,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 3772.49,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_02
			},
			{
				StartPosX = -3173.22,
				StartPosY = 900,
				StartPosId = 334,
				StartPosZ = 1262.49,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = -2510,
				StartPosY = 900,
				StartPosId = 335,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 1262,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_03
			},
			{
				StartPosX = -9722,
				StartPosY = 660,
				StartPosId = 336,
				StartPosZ = 1262,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_REST_00] = {
		NAME = STR_ID_20008,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_HAMEL,
		HouseIDList = {
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_POSTBOX
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_HAMEL_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = 48524.76,
				StartPosY = 4500,
				StartPosId = 340,
				StartPosZ = 482,
				IsSummon = TRUE
			},
			{
				StartPosX = 48157,
				StartPosY = 4500,
				StartPosId = 341,
				BattleFieldStartLineIndex = 2,
				StartPosZ = 482,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_02
			},
			{
				StartPosX = 57412,
				StartPosY = 4500,
				StartPosId = 342,
				StartPosZ = 123.59,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 58196,
				StartPosY = 4740,
				StartPosId = 343,
				BattleFieldStartLineIndex = 1,
				StartPosZ = 733,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_04
			},
			{
				StartPosX = 57749.22,
				StartPosY = 4740,
				StartPosId = 344,
				StartPosZ = 733.49,
				IsSummon = TRUE,
				IsRight = FALSE
			},
			{
				StartPosX = 57735,
				StartPosY = 4500,
				StartPosId = 345,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -236,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_03
			},
			{
				StartPosX = 54035.949,
				StartPosY = 4350,
				StartPosId = 346,
				StartPosZ = -52.499,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_REST_00] = {
		NAME = STR_ID_23093,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_SANDER,
		HouseIDList = {
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_POSTBOX
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_SANDER_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -4888,
				StartPosY = 22594,
				StartPosId = 350,
				StartPosZ = -59506,
				IsSummon = TRUE
			},
			{
				StartPosX = -5035,
				StartPosY = 22594,
				StartPosId = 351,
				BattleFieldStartLineIndex = 2,
				StartPosZ = -59506,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_02
			},
			{
				StartPosX = 3671,
				StartPosY = 22624,
				StartPosId = 352,
				StartPosZ = -59506,
				IsSummon = TRUE
			},
			{
				StartPosX = 4044,
				StartPosY = 22624,
				StartPosId = 353,
				BattleFieldStartLineIndex = 1,
				StartPosZ = -59506,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_03
			},
			{
				StartPosX = 624,
				StartPosY = 22400,
				StartPosId = 356,
				StartPosZ = -59506,
				IsWarp = TRUE
			}
		}
	},
	[VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_REST_00] = {
		NAME = STR_ID_33882,
		BaseLocalStateID = LOCAL_MAP_ID.LMI_RANOX,
		HouseIDList = {
			HOUSE_ID.HI_GLAVE_HOUSE,
			HOUSE_ID.HI_HELLEN_SHOP_HOUSE,
			HOUSE_ID.HI_BILLBOARD,
			HOUSE_ID.HI_POSTBOX
		},
		WORLD_ID = WORLD_ID.WI_BATTLE_FIELD_RANOX_FIELD_REST_00,
		CAMERA = {
			1800,
			160,
			300
		},
		START_POS = {
			{
				StartPosX = -4529.9,
				StartPosY = 22165,
				StartPosId = 360,
				StartPosZ = -26773.28,
				IsSummon = TRUE
			},
			{
				StartPosX = -5310.91,
				StartPosY = 22165,
				StartPosId = 361,
				BattleFieldStartLineIndex = 32,
				StartPosZ = -26988.11,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_02
			},
			{
				StartPosX = 3676.28,
				StartPosY = 22624,
				StartPosId = 363,
				StartPosZ = -25558.85,
				IsSummon = TRUE
			},
			{
				StartPosX = 4219,
				StartPosY = 22624,
				StartPosId = 364,
				BattleFieldStartLineIndex = 22,
				StartPosZ = -25287,
				BattleFieldId = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_03
			},
			{
				StartPosX = -412.69,
				StartPosY = 22400,
				StartPosId = 362,
				StartPosZ = -26448.16,
				IsWarp = TRUE
			}
		}
	},
}
