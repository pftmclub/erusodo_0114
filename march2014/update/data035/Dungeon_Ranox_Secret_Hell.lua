STAGE_LIST = {
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_CHEST,
			NPC_UNIT_ID.NUI_SECRET_STAGE0_CHAIN_OBJECT,
			NPC_UNIT_ID.NUI_SECRET_STAGE0_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					0,
					1,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 40,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						2
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						4
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						6
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						10
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						9
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_STAGE0_CHAIN_OBJECT,
					START_POS = {
						18
					},
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_SECRET_STAGE0_GATE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					1,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 40,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						12
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						14
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						15
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						16
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						17
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_STAGE0_GATE,
					START_POS = {
						19
					},
					ACTIVE = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE1 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_STAGE1,
		STAGE_TYPE = STAGE_TYPE.ST_EXTRA_NPC,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_CHEST,
			NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN1,
			NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN2,
			NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
			NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
			NPC_UNIT_ID.NUI_SECRET_SCENE_1_0,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_A,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_B,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_C
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			EXTRA_NPC_START_POS = 47,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					1,
					1,
					100
				}
			},
			CLEAR_COND1 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GATE_OF_NIGHTMARE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					5,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 103,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						2
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						4
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						35
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						36
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						6
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						9
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						10
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						12
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						32
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						34
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						33
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN1,
					START_POS = {
						35
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN2,
					START_POS = {
						32
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						38
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						39
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						8
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						48
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						40
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						49
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_1_0,
					START_POS = {
						45
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = 3,
			EXTRA_NPC_START_POS = 46,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					1,
					2,
					100
				}
			},
			CLEAR_COND1 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GATE_OF_NIGHTMARE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					5,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 103,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 100,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						18
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						17
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						14
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						15
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						16
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
					START_POS = {
						21
					},
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_A,
					START_POS = {
						54
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 4,
			START_LINE_SET = 3,
			END_LINE_SET = -1,
			EXTRA_NPC_START_POS = 47,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC,
				DELAY_TIME_CLEAR_SUB_STAGE = 2,
				PORTAL_LINE_INDEX = {
					101
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					2,
					0,
					100
				}
			},
			CLEAR_COND1 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GATE_OF_NIGHTMARE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					5,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 100,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						25
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						30
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						24
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						26
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						31
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						27
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						28
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						29
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						50
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						51
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						52
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						53
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
					START_POS = {
						25
					},
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_B,
					START_POS = {
						56
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_C,
					START_POS = {
						57
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE2 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_STAGE2,
		STAGE_TYPE = STAGE_TYPE.ST_EXTRA_NPC,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_CHEST,
			NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
			NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			EXTRA_NPC_START_POS = 27,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					2,
					1,
					100
				}
			},
			CLEAR_COND1 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GATE_OF_NIGHTMARE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					5,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 28,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						6
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						4
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						2
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						12
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						9
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
					START_POS = {
						10
					}
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					3,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 28,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						17
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						14
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						15
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						26
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						20
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						19
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						24
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						21
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						23
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
					START_POS = {
						20
					},
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_STAGE3,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SUMMON,
			NPC_UNIT_ID.NUI_SECRET_JIN,
			NPC_UNIT_ID.NUI_SECRET_IN,
			NPC_UNIT_ID.NUI_CHEST
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					3,
					1,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 54,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						2
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						4
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						6
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						9
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						12
					}
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = 3,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					3,
					2,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 54,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				},
				{
					LINE_INDEX = 7,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						14
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						15
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						16
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						17
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						18
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						19
					}
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 4,
			START_LINE_SET = 3,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					4,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 7,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						22
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						23
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						24
					}
				},
				{
					BOSS_GAUGE_HP_LINES = 20,
					MONSTER_GRADE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_JIN,
					START_POS = {
						20
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					BOSS_GAUGE_HP_LINES = 20,
					MONSTER_GRADE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_IN,
					START_POS = {
						21
					},
					SHOW_BOSS_NAME = FALSE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE,
					IS_RIGHT = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE4 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_BOSS_STAGE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAGI_BOSS,
			NPC_UNIT_ID.NUI_SECRET_BOSS_LAVA,
			NPC_UNIT_ID.NUI_SECRET_SCENE_1_1
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_NONE,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_SECRET_MAGI_BOSS
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
					MONSTER_GRADE = 1,
					BOSS_GAUGE_HP_LINES = 35,
					SHOW_BOSS_NAME_DELAY_TIME = 11,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_BOSS,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_1_1,
					START_POS = {
						1
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BOSS_LAVA,
					START_POS = {
						1
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE5 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_EXTRA_STAGE1,
		STAGE_TYPE = STAGE_TYPE.ST_EXTRA,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_CHEST,
			NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN1,
			NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN2,
			NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
			NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
			NPC_UNIT_ID.NUI_SECRET_SCENE_1_0,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_A,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_B,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_C
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					5,
					1,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 103,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						2
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						4
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						35
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						36
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						6
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						9
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						10
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						12
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						32
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						34
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						33
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN1,
					START_POS = {
						35
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_STAGE1_CHAIN2,
					START_POS = {
						32
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						38
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						39
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						48
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						40
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						49
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_1_0,
					START_POS = {
						45
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = 3,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					5,
					2,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 103,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 100,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						18
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						17
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						14
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						15
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						16
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
					START_POS = {
						21
					},
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_A,
					START_POS = {
						54
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 4,
			START_LINE_SET = 3,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC,
				DELAY_TIME_CLEAR_SUB_STAGE = 2,
				PORTAL_LINE_INDEX = {
					101
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					6,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 100,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						25
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						30
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						24
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						26
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						31
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						27
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						28
					}
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						29
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						50
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						51
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						52
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
					START_POS = {
						53
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
					START_POS = {
						25
					},
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_B,
					START_POS = {
						56
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAGI_SCENE_C,
					START_POS = {
						57
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE6 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_EXTRA_STAGE2,
		STAGE_TYPE = STAGE_TYPE.ST_EXTRA,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_CHEST,
			NPC_UNIT_ID.NUI_SECRET_LAVA_DUMMY,
			NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					6,
					1,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 28,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						6
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						4
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						2
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						12
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						9
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
					START_POS = {
						10
					}
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					7,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 28,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						17
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						14
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						15
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						26
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER,
					START_POS = {
						20
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						19
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						24
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						21
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						23
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_DUMMY,
					START_POS = {
						20
					},
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE7 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_EXTRA_STAGE3,
		STAGE_TYPE = STAGE_TYPE.ST_EXTRA,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER,
			NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
			NPC_UNIT_ID.NUI_SECRET_MAGI_SUMMON,
			NPC_UNIT_ID.NUI_SECRET_JIN,
			NPC_UNIT_ID.NUI_SECRET_IN,
			NPC_UNIT_ID.NUI_CHEST
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					7,
					1,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 54,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						1
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER,
					START_POS = {
						2
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						3
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						4
					}
				},
				{
					MONSTER_GRADE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER,
					START_POS = {
						5
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						6
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						9
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						7
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						8
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						11
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						12
					}
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = 3,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					7,
					2,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 54,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				},
				{
					LINE_INDEX = 7,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM,
					START_POS = {
						13
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						14
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						15
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD,
					START_POS = {
						16
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						17
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						18
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER,
					START_POS = {
						19
					}
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 4,
			START_LINE_SET = 3,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					8,
					0,
					100
				}
			},
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 7,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_NOM
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_MAG_CHARGER
				},
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER
				},
				{
					REWARD_RATE = 2.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BERSERKER
				},
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SNIKER
				},
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIREBIRD
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_NOM
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_MAG_CHARGER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_FIGHTER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_SNIKER
				},
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_CRUEL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						22
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						23
					}
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_FIGHTER,
					START_POS = {
						24
					}
				},
				{
					BOSS_GAUGE_HP_LINES = 20,
					MONSTER_GRADE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_JIN,
					START_POS = {
						20
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					BOSS_GAUGE_HP_LINES = 20,
					MONSTER_GRADE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_IN,
					START_POS = {
						21
					},
					SHOW_BOSS_NAME = FALSE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE,
					IS_RIGHT = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE8 = {
		WORLD_ID = WORLD_ID.WI_RANOX_SECRET_EXTRA_BOSS_STAGE,
		STAGE_TYPE = STAGE_TYPE.ST_EXTRA,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SECRET_EXTRA_MAGI_BOSS,
			NPC_UNIT_ID.NUI_SECRET_BOSS_LAVA,
			NPC_UNIT_ID.NUI_SECRET_SCENE_1_1
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_NONE,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_SECRET_EXTRA_MAGI_BOSS
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
					MONSTER_GRADE = 1,
					BOSS_GAUGE_HP_LINES = 35,
					SHOW_BOSS_NAME_DELAY_TIME = 11,
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_EXTRA_MAGI_BOSS,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_SCENE_1_1,
					START_POS = {
						1
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SECRET_BOSS_LAVA,
					START_POS = {
						1
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	}
}
