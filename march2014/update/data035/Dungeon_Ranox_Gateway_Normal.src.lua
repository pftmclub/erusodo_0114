STAGE_LIST = {
	DUNGEON_MAP = {
		STAGE0 = {
			SUB_STAGE0 = {
				300,
				180
			}
		},
		STAGE1 = {
			SUB_STAGE0 = {
				200,
				120
			},
			SUB_STAGE1 = {
				160,
				80
			},
			SUB_STAGE2 = {
				200,
				40
			}
		},
		STAGE2 = {
			SUB_STAGE0 = {
				260,
				0
			},
			SUB_STAGE1 = {
				300,
				0
			},
			SUB_STAGE2 = {
				300,
				0
			}
		},
		STAGE3 = {
			SUB_STAGE0 = {
				300,
				-41,
				TRUE
			}
		}
	},
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_GATEWAY_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
			NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
			NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
			NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
			NPC_UNIT_ID.NUI_MOUNT_GATE,
			NPC_UNIT_ID.NUI_SAMETEAM_IGNIA
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_MOUNT_GATE
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
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 4.375,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.875,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_GATE,
					START_POS = {
						11
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE,
					NO_DROP = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SAMETEAM_IGNIA,
					START_POS = {
						1
					},
					ACTIVE = TRUE,
					ALLY_NPC = TRUE,
					TEAM = TEAM_NUM.TN_RED
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE1 = {
		WORLD_ID = WORLD_ID.WI_RANOX_GATEWAY_STAGE1,
		READY_NPC = {
			NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
			NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
			NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
			NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
			NPC_UNIT_ID.NUI_SAMETEAM_IGNIA
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 56,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
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
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 4.375,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.875,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						33
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SAMETEAM_IGNIA,
					START_POS = {
						1
					},
					ACTIVE = TRUE,
					ALLY_NPC = TRUE,
					TEAM = TEAM_NUM.TN_RED
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 2,
			START_LINE_SET = 1,
			END_LINE_SET = 3,
			GO_TYPE = GO_TYPE.GT_LEFT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 56,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
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
					2,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 4.375,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.875,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						18
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
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
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 10,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					2,
					0,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 4.375,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.875,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						28
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE2 = {
		WORLD_ID = WORLD_ID.WI_RANOX_GATEWAY_STAGE2,
		READY_NPC = {
			NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
			NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
			NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
			NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
			NPC_UNIT_ID.NUI_MOUNT_POINT_STAGE_BOSS,
			NPC_UNIT_ID.NUI_SAMETEAM_IGNIA
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 11,
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
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 4.375,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.875,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						2
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_SAMETEAM_IGNIA,
					START_POS = {
						1
					},
					ACTIVE = TRUE,
					ALLY_NPC = TRUE,
					TEAM = TEAM_NUM.TN_RED
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
					LINE_INDEX = 11,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
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
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_NOM
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 4.375,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.875,
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_FIREBIRD
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_CHARGER,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_MAG_STONE,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_MOUNT_POINT_STAGE_BOSS,
					START_POS = {
						10
					},
					ACTIVE = FALSE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_GATEWAY_STAGEBOSS,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_BOSS,
			NPC_UNIT_ID.NUI_MOUNT_POINT_STAGE_BOSS,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA,
			NPC_UNIT_ID.NUI_EVENT_GATEWAY_BOSS
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA
				},
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_GAME,
					0,
					0,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 3.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER
				}
			},
			NPC_GROUP = {
				{
					MONSTER_GRADE = 1,
					BOSS_GAUGE_HP_LINES = 35,
					SHOW_BOSS_NAME_DELAY_TIME = 7,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_EVENT_GATEWAY_BOSS,
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
