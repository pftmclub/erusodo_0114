STAGE_LIST = {
	DUNGEON_MAP = {
		STAGE0 = {
			SUB_STAGE0 = {
				80,
				187
			}
		},
		STAGE1 = {
			SUB_STAGE0 = {
				140,
				127
			},
			SUB_STAGE1 = {
				180,
				127
			},
			SUB_STAGE2 = {
				220,
				127
			},
			SUB_STAGE3 = {
				220,
				127
			}
		},
		STAGE2 = {
			SUB_STAGE0 = {
				280,
				127
			}
		},
		STAGE3 = {
			SUB_STAGE0 = {
				340,
				187
			},
			SUB_STAGE1 = {
				380,
				187
			}
		},
		STAGE4 = {
			SUB_STAGE0 = {
				440,
				217,
				TRUE
			}
		}
	},
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_ASHESVILLAGE_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
			NPC_UNIT_ID.NUI_SMITH_WORKER,
			NPC_UNIT_ID.NUI_SMITH_RUNNER,
			NPC_UNIT_ID.NUI_SMITH_SCIENCE,
			NPC_UNIT_ID.NUI_SMITH_MACHINE,
			NPC_UNIT_ID.NUI_RANOX_ASHESVILLAGE_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_RANOX_ASHESVILLAGE_GATE
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
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RANOX_ASHESVILLAGE_GATE,
					START_POS = {
						10
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE1 = {
		WORLD_ID = WORLD_ID.WI_RANOX_ASHESVILLAGE_STAGE1,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
			NPC_UNIT_ID.NUI_SMITH_WORKER,
			NPC_UNIT_ID.NUI_SMITH_RUNNER,
			NPC_UNIT_ID.NUI_SMITH_SCIENCE,
			NPC_UNIT_ID.NUI_SMITH_MACHINE,
			NPC_UNIT_ID.NUI_SMITH_CART,
			NPC_UNIT_ID.NUI_CHEST,
			NPC_UNIT_ID.NUI_RANOX_ASHESVILLAGE_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 10,
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
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						3
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER,
					START_POS = {
						10
					},
					ACTIVE = TRUE
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
					LINE_INDEX = 10,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 41,
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
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 4,
			START_LINE_SET = 3,
			END_LINE_SET = 5,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 41,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 51,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					1,
					3,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						34
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						35
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						37
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						27
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						28
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE3 = {
			MAIN_LINE_SET = 6,
			START_LINE_SET = 5,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 51,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_TIME,
				CLEAR_TIME = 7.5,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					2,
					0,
					100
				}
			},
			NPC_GROUP = {},
			TRIGGER_LIST = {}
		}
	},
	STAGE2 = {
		WORLD_ID = WORLD_ID.WI_RANOX_ASHESVILLAGE_STAGE2,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
			NPC_UNIT_ID.NUI_SMITH_WORKER,
			NPC_UNIT_ID.NUI_SMITH_RUNNER,
			NPC_UNIT_ID.NUI_SMITH_SCIENCE,
			NPC_UNIT_ID.NUI_SMITH_MACHINE,
			NPC_UNIT_ID.NUI_SMITH_ALUMINUM,
			NPC_UNIT_ID.NUI_SMITH_REGIEON_POINT
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					3,
					0,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_MACHINE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_REGIEON_POINT,
					START_POS = {
						1
					},
					ACTIVE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_REGIEON_POINT,
					START_POS = {
						2
					},
					ACTIVE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_REGIEON_POINT,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_REGIEON_POINT,
					START_POS = {
						4
					},
					ACTIVE = TRUE,
					IS_RIGHT = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_REGIEON_POINT,
					START_POS = {
						5
					},
					ACTIVE = TRUE,
					IS_RIGHT = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						9
					},
					ACTIVE = TRUE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						11
					},
					ACTIVE = TRUE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						13
					},
					ACTIVE = TRUE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						14
					},
					ACTIVE = TRUE,
					NO_DROP = TRUE
				},
				{
					MONSTER_GRADE = 2,
					BOSS_GAUGE_HP_LINES = 20,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_ALUMINUM,
					START_POS = {
						6
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE
				}
			},
			CREATED_NPC_DROP_TIMES = {
				{
					DROP_TIMES = 50,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER
				},
				{
					DROP_TIMES = 50,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				},
				{
					DROP_TIMES = 50,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_ASHESVILLAGE_STAGE3,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
			NPC_UNIT_ID.NUI_SMITH_WORKER,
			NPC_UNIT_ID.NUI_SMITH_RUNNER,
			NPC_UNIT_ID.NUI_SMITH_SCIENCE,
			NPC_UNIT_ID.NUI_SMITH_MACHINE,
			NPC_UNIT_ID.NUI_SMITH_ALUMINUM,
			NPC_UNIT_ID.NUI_SMITH_CART,
			NPC_UNIT_ID.NUI_RANOX_ASHESVILLAGE_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 4,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					3,
					1,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						1
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						3
					},
					ACTIVE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						5
					},
					ACTIVE = TRUE,
					IS_RIGHT = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_CART,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 3,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 4,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 6,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 7,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 8,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 9,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					KEY_CODE = 10,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						6
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
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 4,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_RANOX_ASHESVILLAG_SWITCH_GATE
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					4,
					0,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_RUNNER,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_WORKER,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_SCIENCE,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					MONSTER_GRADE = 2,
					BOSS_GAUGE_HP_LINES = 20,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_ALUMINUM,
					START_POS = {
						17
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_RANOX_ASHESVILLAG_SWITCH_GATE,
					START_POS = {
						20
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE4 = {
		WORLD_ID = WORLD_ID.WI_RANOX_ASHESVILLAGE_STAGEBOSS,
		READY_NPC = {
			NPC_UNIT_ID.NUI_SMITH_LITTLEWORKER,
			NPC_UNIT_ID.NUI_SMITH_WORKER,
			NPC_UNIT_ID.NUI_SMITH_RUNNER,
			NPC_UNIT_ID.NUI_SMITH_STAIN
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_SMITH_STAIN
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
					BOSS_GAUGE_HP_LINES = 40,
					NPC_ID = NPC_UNIT_ID.NUI_SMITH_STAIN,
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
