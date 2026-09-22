STAGE_LIST = {
	DUNGEON_MAP = {
		STAGE0 = {
			SUB_STAGE0 = {
				80,
				67
			},
			SUB_STAGE1 = {
				120,
				67
			}
		},
		STAGE1 = {
			SUB_STAGE0 = {
				180,
				67
			},
			SUB_STAGE1 = {
				220,
				67
			},
			SUB_STAGE2 = {
				260,
				67
			}
		},
		STAGE2 = {
			SUB_STAGE0 = {
				320,
				67
			},
			SUB_STAGE1 = {
				360,
				67
			}
		},
		STAGE3 = {
			SUB_STAGE0 = {
				420,
				67,
				TRUE
			}
		}
	},
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_BURNDEDFOREST_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_MAG_NOM,
			NPC_UNIT_ID.NUI_MAG_STONE,
			NPC_UNIT_ID.NUI_MAG_CHARGER,
			NPC_UNIT_ID.NUI_RAGE_BABYFIG,
			NPC_UNIT_ID.NUI_RANOX_BURNDEDFOREST_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 7,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					0,
					1,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 7,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_RANOX_BURNDEDFOREST_GATE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					1,
					0,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RANOX_BURNDEDFOREST_GATE,
					START_POS = {
						20
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE1 = {
		WORLD_ID = WORLD_ID.WI_RANOX_BURNDEDFOREST_STAGE1,
		READY_NPC = {
			NPC_UNIT_ID.NUI_MAG_NOM,
			NPC_UNIT_ID.NUI_MAG_STONE,
			NPC_UNIT_ID.NUI_MAG_CHARGER,
			NPC_UNIT_ID.NUI_RAGE_BABYFIG,
			NPC_UNIT_ID.NUI_RAGE_DRYAD,
			NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
			NPC_UNIT_ID.NUI_TRAP_FIRE_BIG
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 40,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					1,
					1,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						52
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						45
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						62
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						63
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						64
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						65
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = 3,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 40,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 67,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					1,
					2,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						56
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						28
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						32
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						33
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						34
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						35
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						53
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						54
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						55
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						66
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						67
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_SMALL,
					START_POS = {
						68
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 4,
			START_LINE_SET = 3,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 67,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_RANOX_BURNDEDFOREST_GATE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					2,
					0,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						37
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						57
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						60
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						61
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						38
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						39
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						58
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						40
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						41
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						42
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM,
					START_POS = {
						43
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						44
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						46
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						47
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						48
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER,
					START_POS = {
						49
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						51
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RANOX_BURNDEDFOREST_GATE,
					START_POS = {
						50
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_TRAP_FIRE_BIG,
					START_POS = {
						69
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE2 = {
		WORLD_ID = WORLD_ID.WI_RANOX_BURNDEDFOREST_STAGE2,
		READY_NPC = {
			NPC_UNIT_ID.NUI_RAGE_BABYFIG,
			NPC_UNIT_ID.NUI_RAGE_MOTHERFIG
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 12,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					2,
					1,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						1
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						2
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						3
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						4
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						5
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						6
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						7
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						8
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						9
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						10
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						11
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						12
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						13
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						14
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						16
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						17
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						18
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						19
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						20
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						21
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						22
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_BABYFIG,
					START_POS = {
						23
					},
					ACTIVE = TRUE,
					GAGE_BAR = FALSE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 12,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_TIME,
				CLEAR_TIME = 3,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					3,
					0,
					100
				}
			},
			NPC_GROUP = {},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_BURNDEDFOREST_STAGEBOSS,
		READY_NPC = {
			NPC_UNIT_ID.NUI_RAGE_DRYAD,
			NPC_UNIT_ID.NUI_RAGE_DRYADKENTA
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_RAGE_DRYADKENTA
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_GAME,
					0,
					0,
					100
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						2
					},
					ACTIVE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYAD,
					START_POS = {
						3
					},
					ACTIVE = TRUE,
					IS_RIGHT = FALSE
				},
				{
					MONSTER_GRADE = 1,
					BOSS_GAUGE_HP_LINES = 35,
					NPC_ID = NPC_UNIT_ID.NUI_RAGE_DRYADKENTA,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	}
}
