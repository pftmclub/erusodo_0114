STAGE_LIST = {
	DUNGEON_MAP = {
		STAGE0 = {
			SUB_STAGE0 = {
				80,
				145
			},
			SUB_STAGE1 = {
				110,
				105
			},
			SUB_STAGE2 = {
				140,
				65
			}
		},
		STAGE1 = {
			SUB_STAGE0 = {
				210,
				65
			},
			SUB_STAGE1 = {
				250,
				65
			},
			SUB_STAGE2 = {
				290,
				105
			}
		},
		STAGE2 = {
			SUB_STAGE0 = {
				360,
				105
			}
		},
		STAGE3 = {
			SUB_STAGE0 = {
				420,
				105,
				TRUE
			}
		}
	},
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_GEYSER_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
			NPC_UNIT_ID.NUI_GEYSER_WORKER,
			NPC_UNIT_ID.NUI_GEYSER_RUNNER,
			NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
			NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
			NPC_UNIT_ID.NUI_GEYSER_MULLOCK_CALL,
			NPC_UNIT_ID.NUI_GEYSER_MULMAI,
			NPC_UNIT_ID.NUI_GEYSER_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 22,
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
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER,
					START_POS = {
						13
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
					LINE_INDEX = 22,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 25,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					0,
					2,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						28
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						29
					},
					ACTIVE = TRUE
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
					LINE_INDEX = 25,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GEYSER_GATE
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
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						32
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						33
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						34
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						35
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						37
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						38
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						39
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						40
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						41
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						42
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_GATE,
					START_POS = {
						43
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
		WORLD_ID = WORLD_ID.WI_RANOX_GEYSER_STAGE1,
		READY_NPC = {
			NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
			NPC_UNIT_ID.NUI_GEYSER_WORKER,
			NPC_UNIT_ID.NUI_GEYSER_RUNNER,
			NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
			NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
			NPC_UNIT_ID.NUI_GEYSER_MULLOCK_CALL,
			NPC_UNIT_ID.NUI_GEYSER_MULMAI,
			NPC_UNIT_ID.NUI_GEYSER_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 20,
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
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						40
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						13
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
					LINE_INDEX = 20,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 45,
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
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						19
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						42
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						41
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						28
					},
					ACTIVE = TRUE
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
					LINE_INDEX = 45,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GEYSER_GATE
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
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						32
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						33
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
					START_POS = {
						34
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						35
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						37
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_GATE,
					START_POS = {
						38
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
		WORLD_ID = WORLD_ID.WI_RANOX_GEYSER_STAGE2,
		READY_NPC = {
			NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER,
			NPC_UNIT_ID.NUI_GEYSER_WORKER,
			NPC_UNIT_ID.NUI_GEYSER_RUNNER,
			NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
			NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
			NPC_UNIT_ID.NUI_GEYSER_MULLOCK_CALL,
			NPC_UNIT_ID.NUI_GEYSER_MULMAI,
			NPC_UNIT_ID.NUI_GEYSER_ALUMINUM,
			NPC_UNIT_ID.NUI_GEYSER_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GEYSER_GATE
				},
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
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_LITTLEWORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_WORKER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULLOCK,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						14
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_SCIENCE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULMAI,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_RUNNER,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_GATE,
					START_POS = {
						13
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_GEYSER_STAGEBOSS,
		READY_NPC = {
			NPC_UNIT_ID.NUI_GEYSER_MULSAYRENA,
			NPC_UNIT_ID.NUI_GEYSER_MULSAYRENA_CALL,
			NPC_UNIT_ID.NUI_EVENT_GAYSER_BOSS
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_GEYSER_MULSAYRENA
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
					SHOW_BOSS_NAME_DELAY_TIME = 17,
					NPC_ID = NPC_UNIT_ID.NUI_GEYSER_MULSAYRENA,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_EVENT_GAYSER_BOSS,
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
