STAGE_LIST = {
	DUNGEON_MAP = {
		STAGE0 = {
			SUB_STAGE0 = {
				80,
				65
			}
		},
		STAGE1 = {
			SUB_STAGE0 = {
				110,
				145
			},
			SUB_STAGE1 = {
				150,
				145
			},
			SUB_STAGE2 = {
				190,
				145
			},
			SUB_STAGE3 = {
				230,
				145
			},
			SUB_STAGE4 = {
				230,
				145
			}
		},
		STAGE2 = {
			SUB_STAGE0 = {
				310,
				145
			},
			SUB_STAGE1 = {
				350,
				145
			}
		},
		STAGE3 = {
			SUB_STAGE0 = {
				430,
				205,
				TRUE
			}
		}
	},
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FIRETEMPLE_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
			NPC_UNIT_ID.NUI_FIRETEMPLE_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_FIRETEMPLE_GATE
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
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						13
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRETEMPLE_GATE,
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
	STAGE1 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FIRETEMPLE_STAGE1,
		EXCLUDE_RANDOM_POS = {
			8,
			9,
			33,
			34
		},
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER,
			NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 5,
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
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						35
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						37
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
					START_POS = {
						9
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
					LINE_INDEX = 5,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 16,
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
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						17
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
					LINE_INDEX = 16,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
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
					3,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE3 = {
			MAIN_LINE_SET = 6,
			START_LINE_SET = 5,
			END_LINE_SET = 7,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 36,
					AT_START = FALSE,
					LOOK_LEFT = TRUE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					1,
					4,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						28
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						32
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						38
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						39
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
					START_POS = {
						33
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
					START_POS = {
						34
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE4 = {
			MAIN_LINE_SET = 8,
			START_LINE_SET = 7,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 36,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_TIME,
				CLEAR_TIME = 3,
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
		WORLD_ID = WORLD_ID.WI_RANOX_FIRETEMPLE_STAGE2,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER,
			NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_JIN,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_IN,
			NPC_UNIT_ID.NUI_MOUNT_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 14,
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
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD_FIRETEMPLE,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						9
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
					LINE_INDEX = 14,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC,
				DELAY_TIME_CLEAR_SUB_STAGE = 2.5,
				PORTAL_LINE_INDEX = {
					24
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
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_FIRETEMPLE,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_FIRETEMPLE,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FIRETEMPLE_STAGEBOSS,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_IPLITAN
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_IPLITAN
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
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_IPLITAN,
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
