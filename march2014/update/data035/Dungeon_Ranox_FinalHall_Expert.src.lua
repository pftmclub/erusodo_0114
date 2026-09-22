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
				140,
				125
			},
			SUB_STAGE1 = {
				180,
				125
			},
			SUB_STAGE2 = {
				220,
				125
			}
		},
		STAGE2 = {
			SUB_STAGE0 = {
				280,
				185
			}
		},
		STAGE3 = {
			SUB_STAGE0 = {
				370,
				185
			},
			SUB_STAGE1 = {
				410,
				185
			},
			SUB_STAGE2 = {
				410,
				143
			}
		},
		STAGE4 = {
			SUB_STAGE0 = {
				410,
				101,
				TRUE
			}
		}
	},
	STAGE0 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FINALHALL_STAGE0,
		START_MOTION = TRUE,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC,
				DELAY_TIME_CLEAR_SUB_STAGE = 2.5,
				PORTAL_LINE_INDEX = {
					14
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
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_CHEST,
					START_POS = {
						10
					},
					ACTIVE = FALSE,
					GAGE_BAR = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE1 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FINALHALL_STAGE1,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_BOSS_FINALHALL,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER,
			NPC_UNIT_ID.NUI_FIRETEMPLE_GATE
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CURTAIN_GROUP = {
				{
					LINE_INDEX = 13,
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
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						53
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
					START_POS = {
						17
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
					LINE_INDEX = 13,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				},
				{
					LINE_INDEX = 50,
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
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						55
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						56
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						31
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						32
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						33
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						34
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						35
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						36
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						37
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
					LINE_INDEX = 50,
					AT_START = TRUE,
					LOOK_LEFT = FALSE
				}
			},
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_FIRETEMPLE_GATE
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
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						38
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						39
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						45
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						40
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						41
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						42
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						43
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
					START_POS = {
						44
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						46
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						47
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						48
					},
					ACTIVE = TRUE
				},
				{
					MONSTER_GRADE = 2,
					BOSS_GAUGE_HP_LINES = 20,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_BOSS_FINALHALL,
					START_POS = {
						49
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						50
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						51
					},
					ACTIVE = TRUE
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
	STAGE2 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FINALHALL_STAGE2,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL,
			NPC_UNIT_ID.NUI_EVENT_FINALHALL_BOSS2,
			NPC_UNIT_ID.NUI_EVENT_FINALHALL_BOSS3
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
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
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					MONSTER_GRADE = 2,
					BOSS_GAUGE_HP_LINES = 20,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						5
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						6
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_EVENT_FINALHALL_BOSS2,
					START_POS = {
						7
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_EVENT_FINALHALL_BOSS3,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		}
	},
	STAGE3 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FINALHALL_STAGE3,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_JIN_FINALHALL,
			NPC_UNIT_ID.NUI_FIRE_DEVIL_IN_FINALHALL,
			-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): June2017 itself references
			-- NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA_BOSS here, which is not defined
			-- anywhere in June2017's own Enum.lua either -- a dead/typo'd name
			-- in the source client, per RANOX_PORT_PLAN.md's Findings section.
			-- Resolved as NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL, which is defined
			-- and is almost certainly what was meant.
			NPC_UNIT_ID.NUI_FIRE_DEVIL_IGNIA_BOSS_FINALHALL,
			NPC_UNIT_ID.NUI_FIRETEMPLE_REGIEON_POINT
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = 0,
			GO_TYPE = GO_TYPE.GT_RIGHT,
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
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRETEMPLE_REGIEON_POINT,
					START_POS = {
						1
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						1
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						2
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						3
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						4
					},
					ACTIVE = TRUE
				},
				{
					MONSTER_GRADE = 2,
					BOSS_GAUGE_HP_LINES = 20,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_JIN_FINALHALL,
					START_POS = {
						5
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					MONSTER_GRADE = 2,
					BOSS_GAUGE_HP_LINES = 20,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_IN_FINALHALL,
					START_POS = {
						6
					},
					SHOW_BOSS_NAME = FALSE,
					HAVE_BOSS_GAGE = TRUE,
					FOCUS_CAMERA = FALSE,
					STOP_AT_START_STATE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						7
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						8
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						9
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						10
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE1 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = 0,
			END_LINE_SET = 0,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					3,
					2,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRETEMPLE_REGIEON_POINT,
					START_POS = {
						11
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						11
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						12
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						13
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						14
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						15
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						16
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						17
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT,
					START_POS = {
						18
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT,
					START_POS = {
						19
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						20
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						21
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT,
					START_POS = {
						22
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE2 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = 0,
			END_LINE_SET = 0,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_ALL_ACTIVE_NPC,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_SUB_STAGE,
					3,
					3,
					100
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.75,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_FIGHTER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 2,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_BERSERKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 1.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SNIKER_INCHANT
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL
				}
			},
			NPC_REWARD_RATE = {
				{
					REWARD_RATE = 0.5,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE
				}
			},
			NPC_GROUP = {
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRETEMPLE_REGIEON_POINT,
					START_POS = {
						15
					},
					ACTIVE = FALSE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_BALL,
					START_POS = {
						23
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						24
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						25
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						26
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						27
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						28
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						29
					},
					ACTIVE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_CHAIN_STONE,
					START_POS = {
						30
					},
					ACTIVE = TRUE
				}
			},
			TRIGGER_LIST = {}
		},
		SUB_STAGE3 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = 0,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_TIME,
				CLEAR_TIME = 4,
				NEXT_STAGE0 = {
					CLEAR_TYPE.CT_STAGE,
					4,
					0,
					100
				}
			},
			NPC_GROUP = {},
			TRIGGER_LIST = {}
		}
	},
	STAGE4 = {
		WORLD_ID = WORLD_ID.WI_RANOX_FINALHALL_STAGEBOSS,
		READY_NPC = {
			NPC_UNIT_ID.NUI_FIRE_DEVIL_SKA,
			NPC_UNIT_ID.NUI_EVENT_FINALHALL_BOSS
		},
		SUB_STAGE0 = {
			MAIN_LINE_SET = 0,
			START_LINE_SET = -1,
			END_LINE_SET = -1,
			GO_TYPE = GO_TYPE.GT_RIGHT,
			CLEAR_COND0 = {
				CLEAR_CONDITION.CC_KILL_NPC_TYPE,
				NPC_ID = {
					NPC_UNIT_ID.NUI_FIRE_DEVIL_SKA
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
					SHOW_BOSS_NAME_DELAY_TIME = 13,
					NPC_ID = NPC_UNIT_ID.NUI_FIRE_DEVIL_SKA,
					START_POS = {
						1
					},
					SHOW_BOSS_NAME = TRUE,
					HAVE_BOSS_GAGE = TRUE,
					STOP_AT_START_STATE = TRUE
				},
				{
					NPC_ID = NPC_UNIT_ID.NUI_EVENT_FINALHALL_BOSS,
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
