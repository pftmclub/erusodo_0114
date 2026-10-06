#pragma once

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈
// 스킬ID, 스킬레벨업 수치
typedef std::pair< CX2SkillTree::SKILL_ID, int> SkillLevelUpIDAndNum;
#endif // UPGRADE_SKILL_SYSTEM_2013

class CX2SocketItem
{
	public:
		enum SOCKET_DATA_TYPE
		{
			SDT_NONE			= 0,
			SDT_WEAPON_NORMAL,
			SDT_WEAPON_RARE,
			SDT_DEFENCE_NORMAL,
			SDT_DEFENCE_RARE,
			SDT_SET_ITEM,
			SDT_ACCESSORY,
			SDT_TITLE,
			SDT_WEAPON_HARMONY,
			SDT_DEFENCE_HARMONY,
			SDT_WEAPON_MYSTERY,
			SDT_DEFENCE_MYSTERY,
			SDT_WEAPON_SHINE,
			SDT_DEFENCE_SHINE,
			SDT_WEAPON_BATTLE,
			SDT_DEFENCE_BATTLE,
			SDT_WEAPON_INCREASE,
			SDT_DEFENCE_INCREASE,
			SDT_WEAPON_HIT,
			SDT_DEFENCE_HIT,
			SDT_WEAPON_ATK_PHYSIC,
			SDT_DEFENCE_ATK_PHYSIC,
			SDT_WEAPON_ATK_MAGIC,
			SDT_DEFENCE_ATK_MAGIC,
			SDT_WEAPON_DEF_PHYSIC,
			SDT_DEFENCE_DEF_PHYSIC,
			SDT_WEAPON_DEF_MAGIC,
			SDT_DEFENCE_DEF_MAGIC,
			SDT_WEAPON_HYPER_TIME,
			SDT_DEFENCE_HYPER_TIME,
			SDT_WEAPON_INCREASE_HP,
			SDT_DEFENCE_INCREASE_HP,
			SDT_WEAPON_HYPER_CHARGE,
			SDT_DEFENCE_HYPER_CHARGE,
			SDT_WEAPON_SPEED,
			SDT_DEFENCE_SPEED,
			SDT_WEAPON_CRITICAL,
			SDT_DEFENCE_CRITICAL,
			SDT_WEAPON_MOVE_SPEED,
			SDT_DEFENCE_MOVE_SPEED,
			SDT_WEAPON_STRIKE,
			SDT_DEFENCE_STRIKE,
			SDT_WEAPON_AVOID,
			SDT_DEFENCE_AVOID,
			SDT_WEAPON_JUMP_SPEED,
			SDT_DEFENCE_JUMP_SPEED,
//{{ 2011. 07. 27 김민성   소켓 아이템 추가
			SDT_WEAPON_ADD_DAMAGE,
			SDT_DEFENCE_ADD_DAMAGE,
			SDT_WEAPON_ADD_DAMAGE_REDUCE,
			SDT_DEFENCE_ADD_DAMAGE_REDUCE,
//}}
//{{ 2011. 08. 30	최육사	듀얼 마법석 개편
//#ifdef SERV_NEW_DUAL_MAGIC_STONE
			SDT_WEAPON_DESTROY,
			SDT_DEFENCE_DESTROY,
			SDT_WEAPON_EXPLOSION,
			SDT_DEFENCE_EXPLOSION,
			SDT_WEAPON_BURN,
			SDT_DEFENCE_BURN,
			SDT_WEAPON_QUICK,
			SDT_DEFENCE_QUICK,
			SDT_WEAPON_FURY,
			SDT_DEFENCE_FURY,
//#endif SERV_NEW_DUAL_MAGIC_STONE
//}}
//{{ 2012. 07. 09	김민성       현자의 마법석
//#ifdef SERV_DUNGEON_CLEAR_PAYMENT_STONE_EVENT
			SDT_WEAPON_SAGE,
			SDT_DEFENCE_SAGE,
//#endif SERV_DUNGEON_CLEAR_PAYMENT_STONE_EVENT
//}}

//{{ 2012. 09. 19	박세훈	2012 대전 시즌2
//#ifdef SERV_2012_PVP_SEASON2
			SDT_WEAPON_HYPER_TIME_PVP,			/// 대련용 마법석 : 무기 각성 지속 시간
			SDT_DEFENCE_HYPER_TIME_PVP,			/// 대련용 마법석 : 방어구 각성 지속 시간
			SDT_WEAPON_INCREASE_HP_PVP,			/// 대련용 마법석 : 무기 체력
			SDT_DEFENCE_INCREASE_HP_PVP,		/// 대련용 마법석 : 방어구 체력
			SDT_WEAPON_HYPER_CHARGE_PVP,		/// 대련용 마법석 : 무기 각성 충전 속도
			SDT_DEFENCE_HYPER_CHARGE_PVP,		/// 대련용 마법석 : 방어구 각성 충전 속도
			SDT_WEAPON_SPEED_PVP,				/// 대련용 마법석 : 무기 동작속도
			SDT_DEFENCE_SPEED_PVP,				/// 대련용 마법석 : 방어구 동작속도
			SDT_WEAPON_CRITICAL_PVP,			/// 대련용 마법석 : 무기 크리티컬
			SDT_DEFENCE_CRITICAL_PVP,			/// 대련용 마법석 : 방어구 크리티컬
			SDT_WEAPON_MOVE_SPEED_PVP,			/// 대련용 마법석 : 무기 이동력
			SDT_DEFENCE_MOVE_SPEED_PVP,			/// 대련용 마법석 : 방어구 이동력
			SDT_WEAPON_STRIKE_PVP,				/// 대련용 마법석 : 무기 명중
			SDT_DEFENCE_STRIKE_PVP,				/// 대련용 마법석 : 방어구 명중
			SDT_WEAPON_AVOID_PVP,				/// 대련용 마법석 : 무기 회피
			SDT_DEFENCE_AVOID_PVP,				/// 대련용 마법석 : 방어구 회피
			SDT_WEAPON_JUMP_SPEED_PVP,			/// 대련용 마법석 : 무기 점프력
			SDT_DEFENCE_JUMP_SPEED_PVP,			/// 대련용 마법석 : 방어구 점프력
			SDT_WEAPON_ADD_DAMAGE_PVP,			/// 대련용 마법석 : 무기 추가 데미지
			SDT_DEFENCE_ADD_DAMAGE_PVP,			/// 대련용 마법석 : 방어구 추가 데미지
			SDT_WEAPON_ADD_DAMAGE_REDUCE_PVP,	/// 대련용 마법석 : 무기 데미지 감소
			SDT_DEFENCE_ADD_DAMAGE_REDUCE_PVP,	/// 대련용 마법석 : 방어구 데미지 감소
			SDT_WEAPON_DESTROY_PVP,				/// 대련용 마법석 : 무기 파괴
			SDT_DEFENCE_DESTROY_PVP,			/// 대련용 마법석 : 방어구 파괴
			SDT_WEAPON_EXPLOSION_PVP,			/// 대련용 마법석 : 무기 폭발
			SDT_DEFENCE_EXPLOSION_PVP,			/// 대련용 마법석 : 방어구 폭발
			SDT_WEAPON_BURN_PVP,				/// 대련용 마법석 : 무기 연소
			SDT_DEFENCE_BURN_PVP,				/// 대련용 마법석 : 방어구 연소
			SDT_WEAPON_QUICK_PVP,				/// 대련용 마법석 : 무기 신속
			SDT_DEFENCE_QUICK_PVP,				/// 대련용 마법석 : 방어구 신속
			SDT_WEAPON_FURY_PVP,				/// 대련용 마법석 : 무기 격노
			SDT_DEFENCE_FURY_PVP,				/// 대련용 마법석 : 방어구 격노
			SDT_PVP_TAG_POINT,
			SDT_WEAPON_SPEED_2P5_FIXED,				/// 고정 효과 마법석 2.5% : 무기 속도 
			SDT_DEFFENCE_SPEED_2P5_FIXED,			/// 고정 효과 마법석 2.5% : 방어구 속도 
			SDT_WEAPON_SPEED_5P_FIXED,				/// 고정 효과 마법석 5% : 무기 속도 
			SDT_DEFFENCE_SPEED_5P_FIXED,			/// 고정 효과 마법석 5% : 방어구 속도 
			SDT_WEAPON_CRITICAL_2P5_FIXED,			/// 고정 효과 마법석 2.5% : 무기 크리티컬 
			SDT_DEFFENCE_CRITICAL_2P5_FIXED,		/// 고정 효과 마법석 2.5% : 방어구 크리티컬 
			SDT_WEAPON_CRITICAL_5P_FIXED,			/// 고정 효과 마법석 5% : 무기 크리티컬 
			SDT_DEFFENCE_CRITICAL_5P_FIXED,			/// 고정 효과 마법석 5% : 방어구 크리티컬 
			SDT_WEAPON_AVOID_2P5_FIXED,				/// 고정 효과 마법석 2.5% : 무기 회피 
			SDT_DEFFECNE_AVOID_2P5_FIXED,			/// 고정 효과 마법석 2.5% : 방어구 회피 
			SDT_WEAPON_AVOID_5P_FIXED,				/// 고정 효과 마법석 5% : 무기 회피 
			SDT_DEFFENCE_AVOID_5P_FIXED,			/// 고정 효과 마법석 5% : 방어구 회피
			SDT_WEAPON_ADD_DAMAGE_2P5_FIXED,		/// 고정 효과 마법석 2.5% : 무기 추가 데미지 
			SDT_DEFFENCE_ADD_DAMAGE_2P5_FIXED,		/// 고정 효과 마법석 2.5% : 방어구 추가 데미지 
			SDT_WEAPON_ADD_DAMAGE_5P_FIXED,			/// 고정 효과 마법석 5% : 무기 추가 데미지 
			SDT_DEFFENCE_ADD_DAMAGE_5P_FIXED,		/// 고정 효과 마법석 5% : 방어구 추가 데미지 
			SDT_WEAPON_DAMAGE_REDUCE_2P5_FIXED,		/// 고정 효과 마법석 2.5% : 무기 데미지 감소 
			SDT_DEFFECNE_DAMAGE_REDUCE_2P5_FIXED,	/// 고정 효과 마법석 2.5% : 방어구 데미지 감소 
			SDT_WEAPON_DAMAGE_REDUCE_5P_FIXED,		/// 고정 효과 마법석 5% : 무기 데미지 감소 
			SDT_DEFFENCE_DAMAGE_REDUCE_5P_FIXED,	/// 고정 효과 마법석 5% : 방어구 데미지 감소 
			SDT_WEAPON_HYPER_CHARGE_5P_FIXED,		/// 고정 효과 마법석 5% : 무기 각성 충전 
			SDT_DEFFENCE_HYPER_CHARGE_5P_FIXED,		/// 고정 효과 마법석 5% : 방어구 각성 충전 
			SDT_WEAPON_HYPER_CHARGE_10P_FIXED,		/// 고정 효과 마법석 10% : 무기 각성 충전 
			SDT_DEFFENCE_HYPER_CHARGE_10P_FIXED,	/// 고정 효과 마법석 10% : 방어구 각성 충전

			SDT_SKILL_LV_GROUP,						/// 직업에 따라 다른 스킬을 부여하기 위한 스킬 그룹

//#endif SERV_2012_PVP_SEASON2
//}}
			SDT_QUEST_CLEAR_COUNT 					= 120, /// 해외팀 머지 작업. 2013-09-03 김창한
			SDT_ITEM_EXCHANGE_COUNT 				= 121, /// 해외팀 머지 작업. 2013-09-03 김창한
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// June buff port Phase 16-data: the 273 members June's Enum.lua has after 121, at June's values
			/// (june_src_fixed/data036_kom/Enum.src.lua; none collides with March). Only the Lua data names them:
			/// March's code switches on SDT_SKILL_LV_GROUP and the three grow-up types alone, so these take its default paths.
			SDT_WEAPON_RESONANT                             = 122,
			SDT_WEAPON_MANA                                 = 123,
			SDT_WEAPON_HEAVYBLOW                            = 124,
			SDT_WEAPON_AMPLIFICATION                        = 125,
			SDT_DEFENCE_AMPLIFICATION                       = 126,
			SDT_WEAPON_ABSORB                               = 127,
			SDT_DEFENCE_ABSORB                              = 128,
			SDT_WEAPON_RESIST                               = 129,
			SDT_DEFENCE_RESIST                              = 130,
			SDT_WEAPON_DESTRUCTION                          = 131,
			SDT_DEFENCE_DESTRUCTION                         = 132,
			SDT_WEAPON_GALE                                 = 133,
			SDT_DEFENCE_GALE                                = 134,
			SDT_WEAPON_SHINE_AMPLIFICATION                  = 135,
			SDT_DEFENCE_SHINE_AMPLIFICATION                 = 136,
			SDT_WEAPON_SHINE_ABSORB                         = 137,
			SDT_DEFENCE_SHINE_ABSORB                        = 138,
			SDT_WEAPON_SHINE_RESIST                         = 139,
			SDT_DEFENCE_SHINE_RESIST                        = 140,
			SDT_WEAPON_SHINE_DESTRUCTION                    = 141,
			SDT_DEFENCE_SHINE_DESTRUCTION                   = 142,
			SDT_WEAPON_SHINE_GALE                           = 143,
			SDT_DEFENCE_SHINE_GALE                          = 144,
			SDT_WEAPON_ADD_DAMAGE_6P_FIXED                  = 150,
			SDT_DEFFENCE_ADD_DAMAGE_3P_FIXED                = 151,
			SDT_WEAPON_ADD_DAMAGE_7P_FIXED                  = 152,
			SDT_DEFFENCE_ADD_DAMAGE_3P5_FIXED               = 153,
			SDT_WEAPON_ADD_DAMAGE_8P_FIXED                  = 154,
			SDT_DEFFENCE_ADD_DAMAGE_4P_FIXED                = 155,
			SDT_WEAPON_CRITICAL_6P_FIXED                    = 156,
			SDT_DEFFENCE_CRITICAL_3P_FIXED                  = 157,
			SDT_WEAPON_CRITICAL_7P_FIXED                    = 158,
			SDT_DEFFENCE_CRITICAL_3P5_FIXED                 = 159,
			SDT_WEAPON_CRITICAL_8P_FIXED                    = 160,
			SDT_DEFFENCE_CRITICAL_4P_FIXED                  = 161,
			SDT_WEAPON_SPEED_6P_FIXED                       = 162,
			SDT_DEFFENCE_SPEED_3P_FIXED                     = 163,
			SDT_WEAPON_SPEED_7P_FIXED                       = 164,
			SDT_DEFFENCE_SPEED_3P5_FIXED                    = 165,
			SDT_WEAPON_SPEED_8P_FIXED                       = 166,
			SDT_DEFFENCE_SPEED_4P_FIXED                     = 167,
			SDT_WEAPON_DAMAGE_REDUCE_6P_FIXED               = 168,
			SDT_DEFFENCE_DAMAGE_REDUCE_3P_FIXED             = 169,
			SDT_WEAPON_DAMAGE_REDUCE_7P_FIXED               = 170,
			SDT_DEFFENCE_DAMAGE_REDUCE_3P5_FIXED            = 171,
			SDT_WEAPON_DAMAGE_REDUCE_8P_FIXED               = 172,
			SDT_DEFFENCE_DAMAGE_REDUCE_4P_FIXED             = 173,
			SDT_FIX_SKILL_DAMAGE_ACTIVE                     = 174,
			SDT_FIX_SKILL_DAMAGE_SPECIAL_ACTIVE_GRADE_1     = 175,
			SDT_FIX_SKILL_DAMAGE_SPECIAL_ACTIVE_GRADE_2     = 176,
			SDT_FIX_SKILL_DAMAGE_SPECIAL_ACTIVE_GRADE_3     = 177,
			SDT_FIX_SKILL_DAMAGE_HYPER_ACTIVE               = 178,
			SDT_FIX_SKILL_DAMAGE_SPECIAL_ACTIVE_GRADE_ALL   = 179,
			SDT_WEAPON_INCREASE_ATTACK_MP                   = 180,
			SDT_DEFENCE_INCREASE_ATTACK_MP                  = 181,
			SDT_WEAPON_INCREASE_ATTACKED_MP                 = 182,
			SDT_DEFENCE_INCREASE_ATTACKED_MP                = 183,
			SDT_WEAPON_SPECIALIZATION_A                     = 184,
			SDT_DEFENCE_SPECIALIZATION_A                    = 185,
			SDT_WEAPON_SPECIALIZATION_B                     = 186,
			SDT_DEFENCE_SPECIALIZATION_B                    = 187,
			SDT_WEAPON_MAXIMIZE                             = 188,
			SDT_DEFENCE_MAXIMIZE                            = 189,
			SDT_WEAPON_TRANSCEND                            = 190,
			SDT_DEFENCE_TRANSCEND                           = 191,
			SDT_WEAPON_SHINE_TRANSCEND                      = 192,
			SDT_DEFENCE_SHINE_TRANSCEND                     = 193,
			SDT_WEAPON_AMPLIFICATION_PVP                    = 194,
			SDT_DEFENCE_AMPLIFICATION_PVP                   = 195,
			SDT_WEAPON_ABSORB_PVP                           = 196,
			SDT_DEFENCE_ABSORB_PVP                          = 197,
			SDT_WEAPON_RESIST_PVP                           = 198,
			SDT_DEFENCE_RESIST_PVP                          = 199,
			SDT_WEAPON_DESTRUCTION_PVP                      = 200,
			SDT_DEFENCE_DESTRUCTION_PVP                     = 201,
			SDT_WEAPON_GALE_PVP                             = 202,
			SDT_DEFENCE_GALE_PVP                            = 203,
			SDT_WEAPON_SHINE_AMPLIFICATION_PVP              = 204,
			SDT_DEFENCE_SHINE_AMPLIFICATION_PVP             = 205,
			SDT_WEAPON_SHINE_ABSORB_PVP                     = 206,
			SDT_DEFENCE_SHINE_ABSORB_PVP                    = 207,
			SDT_WEAPON_SHINE_RESIST_PVP                     = 208,
			SDT_DEFENCE_SHINE_RESIST_PVP                    = 209,
			SDT_WEAPON_SHINE_DESTRUCTION_PVP                = 210,
			SDT_DEFENCE_SHINE_DESTRUCTION_PVP               = 211,
			SDT_WEAPON_SHINE_GALE_PVP                       = 212,
			SDT_DEFENCE_SHINE_GALE_PVP                      = 213,
			SDT_WEAPON_INCREASE_ATTACK_MP_PVP               = 214,
			SDT_DEFENCE_INCREASE_ATTACK_MP_PVP              = 215,
			SDT_WEAPON_INCREASE_ATTACKED_MP_PVP             = 216,
			SDT_DEFENCE_INCREASE_ATTACKED_MP_PVP            = 217,
			SDT_WEAPON_SPECIALIZATION_A_PVP                 = 218,
			SDT_DEFENCE_SPECIALIZATION_A_PVP                = 219,
			SDT_WEAPON_SPECIALIZATION_B_PVP                 = 220,
			SDT_DEFENCE_SPECIALIZATION_B_PVP                = 221,
			SDT_WEAPON_MAXIMIZE_PVP                         = 222,
			SDT_DEFENCE_MAXIMIZE_PVP                        = 223,
			SDT_WEAPON_TRANSCEND_PVP                        = 224,
			SDT_DEFENCE_TRANSCEND_PVP                       = 225,
			SDT_WEAPON_SHINE_TRANSCEND_PVP                  = 226,
			SDT_DEFENCE_SHINE_TRANSCEND_PVP                 = 227,
			SDT_WEAPON_ADD_DAMAGE_9P_FIXED                  = 228,
			SDT_WEAPON_ADD_DAMAGE_10P_FIXED                 = 229,
			SDT_WEAPON_ADD_DAMAGE_11P_FIXED                 = 230,
			SDT_WEAPON_ADD_DAMAGE_12P_FIXED                 = 231,
			SDT_DEFFENCE_ADD_DAMAGE_4P5_FIXED               = 232,
			SDT_DEFFENCE_ADD_DAMAGE_5P5_FIXED               = 234,
			SDT_DEFFENCE_ADD_DAMAGE_6P_FIXED                = 235,
			SDT_WEAPON_CRITICAL_9P_FIXED                    = 236,
			SDT_WEAPON_CRITICAL_10P_FIXED                   = 237,
			SDT_WEAPON_CRITICAL_11P_FIXED                   = 238,
			SDT_WEAPON_CRITICAL_12P_FIXED                   = 239,
			SDT_DEFFENCE_CRITICAL_4P5_FIXED                 = 240,
			SDT_DEFFENCE_CRITICAL_5P5_FIXED                 = 242,
			SDT_DEFFENCE_CRITICAL_6P_FIXED                  = 243,
			SDT_WEAPON_SPEED_9P_FIXED                       = 244,
			SDT_WEAPON_SPEED_10P_FIXED                      = 245,
			SDT_WEAPON_SPEED_11P_FIXED                      = 246,
			SDT_WEAPON_SPEED_12P_FIXED                      = 247,
			SDT_DEFFENCE_SPEED_4P5_FIXED                    = 248,
			SDT_DEFFENCE_SPEED_5P5_FIXED                    = 250,
			SDT_DEFFENCE_SPEED_6P_FIXED                     = 251,
			SDT_WEAPON_DAMAGE_REDUCE_9P_FIXED               = 252,
			SDT_WEAPON_DAMAGE_REDUCE_10P_FIXED              = 253,
			SDT_WEAPON_DAMAGE_REDUCE_11P_FIXED              = 254,
			SDT_WEAPON_DAMAGE_REDUCE_12P_FIXED              = 255,
			SDT_DEFFENCE_DAMAGE_REDUCE_4P5_FIXED            = 256,
			SDT_DEFFENCE_DAMAGE_REDUCE_5P5_FIXED            = 258,
			SDT_DEFFENCE_DAMAGE_REDUCE_6P_FIXED             = 259,
			SDT_WEAPON_HYPER_CHARGE_9P_FIXED                = 260,
			SDT_WEAPON_HYPER_CHARGE_10P5_FIXED              = 261,
			SDT_WEAPON_HYPER_CHARGE_12P_FIXED               = 262,
			SDT_WEAPON_HYPER_CHARGE_13P5_FIXED              = 263,
			SDT_WEAPON_HYPER_CHARGE_15P_FIXED               = 264,
			SDT_WEAPON_HYPER_CHARGE_16P5_FIXED              = 265,
			SDT_WEAPON_HYPER_CHARGE_18P_FIXED               = 266,
			SDT_DEFENCE_HYPER_CHARGE_6P_FIXED               = 267,
			SDT_DEFENCE_HYPER_CHARGE_7P_FIXED               = 268,
			SDT_DEFENCE_HYPER_CHARGE_8P_FIXED               = 269,
			SDT_DEFENCE_HYPER_CHARGE_9P_FIXED               = 270,
			SDT_DEFENCE_HYPER_CHARGE_10P_FIXED              = 271,
			SDT_DEFENCE_HYPER_CHARGE_11P_FIXED              = 272,
			SDT_DEFENCE_HYPER_CHARGE_12P_FIXED              = 273,
			SDT_WEAPON_HYPER_TIME_9P_FIXED                  = 274,
			SDT_WEAPON_HYPER_TIME_10P5_FIXED                = 275,
			SDT_WEAPON_HYPER_TIME_12P_FIXED                 = 276,
			SDT_WEAPON_HYPER_TIME_13P5_FIXED                = 277,
			SDT_WEAPON_HYPER_TIME_15P_FIXED                 = 278,
			SDT_WEAPON_HYPER_TIME_16P5_FIXED                = 279,
			SDT_WEAPON_HYPER_TIME_18P_FIXED                 = 280,
			SDT_DEFENCE_HYPER_TIME_4P5_FIXED                = 281,
			SDT_DEFENCE_HYPER_TIME_5P25_FIXED               = 282,
			SDT_DEFENCE_HYPER_TIME_6P_FIXED                 = 283,
			SDT_DEFENCE_HYPER_TIME_6P75_FIXED               = 284,
			SDT_DEFENCE_HYPER_TIME_7P5_FIXED                = 285,
			SDT_DEFENCE_HYPER_TIME_8P25_FIXED               = 286,
			SDT_DEFENCE_HYPER_TIME_9P_FIXED                 = 287,
			SDT_WEAPON_TWISTED_SAGE                         = 288,
			SDT_DEFENCE_TWISTED_SAGE                        = 289,
			SDT_WEAPON_SAGE_PVP                             = 290,
			SDT_DEFENCE_SAGE_PVP                            = 291,
			SDT_WEAPON_MAXIMIZE_6P_FIXED                    = 292,
			SDT_WEAPON_MAXIMIZE_7P_FIXED                    = 293,
			SDT_WEAPON_MAXIMIZE_8P_FIXED                    = 294,
			SDT_WEAPON_MAXIMIZE_9P_FIXED                    = 295,
			SDT_WEAPON_MAXIMIZE_10P_FIXED                   = 296,
			SDT_WEAPON_MAXIMIZE_11P_FIXED                   = 297,
			SDT_WEAPON_MAXIMIZE_12P_FIXED                   = 298,
			SDT_DEFFENCE_MAXIMIZE_3P_FIXED                  = 299,
			SDT_DEFFENCE_MAXIMIZE_3P5_FIXED                 = 300,
			SDT_DEFFENCE_MAXIMIZE_4P_FIXED                  = 301,
			SDT_DEFFENCE_MAXIMIZE_4P5_FIXED                 = 302,
			SDT_DEFFENCE_MAXIMIZE_5P_FIXED                  = 303,
			SDT_DEFFENCE_MAXIMIZE_5P5_FIXED                 = 304,
			SDT_DEFFENCE_MAXIMIZE_6P_FIXED                  = 305,
			SDT_WEAPON_SPECIALIZATION_A_6P_FIXED            = 306,
			SDT_WEAPON_SPECIALIZATION_A_7P_FIXED            = 307,
			SDT_WEAPON_SPECIALIZATION_A_8P_FIXED            = 308,
			SDT_WEAPON_SPECIALIZATION_A_9P_FIXED            = 309,
			SDT_WEAPON_SPECIALIZATION_A_10P_FIXED           = 310,
			SDT_WEAPON_SPECIALIZATION_A_11P_FIXED           = 311,
			SDT_WEAPON_SPECIALIZATION_A_12P_FIXED           = 312,
			SDT_DEFFENCE_SPECIALIZATION_A_3P_FIXED          = 313,
			SDT_DEFFENCE_SPECIALIZATION_A_3P5_FIXED         = 314,
			SDT_DEFFENCE_SPECIALIZATION_A_4P_FIXED          = 315,
			SDT_DEFFENCE_SPECIALIZATION_A_4P5_FIXED         = 316,
			SDT_DEFFENCE_SPECIALIZATION_A_5P_FIXED          = 317,
			SDT_DEFFENCE_SPECIALIZATION_A_5P5_FIXED         = 318,
			SDT_DEFFENCE_SPECIALIZATION_A_6P_FIXED          = 319,
			SDT_WEAPON_SPECIALIZATION_B_6P_FIXED            = 320,
			SDT_WEAPON_SPECIALIZATION_B_7P_FIXED            = 321,
			SDT_WEAPON_SPECIALIZATION_B_8P_FIXED            = 322,
			SDT_WEAPON_SPECIALIZATION_B_9P_FIXED            = 323,
			SDT_WEAPON_SPECIALIZATION_B_10P_FIXED           = 324,
			SDT_WEAPON_SPECIALIZATION_B_11P_FIXED           = 325,
			SDT_WEAPON_SPECIALIZATION_B_12P_FIXED           = 326,
			SDT_DEFFENCE_SPECIALIZATION_B_3P_FIXED          = 327,
			SDT_DEFFENCE_SPECIALIZATION_B_3P5_FIXED         = 328,
			SDT_DEFFENCE_SPECIALIZATION_B_4P_FIXED          = 329,
			SDT_DEFFENCE_SPECIALIZATION_B_4P5_FIXED         = 330,
			SDT_DEFFENCE_SPECIALIZATION_B_5P_FIXED          = 331,
			SDT_DEFFENCE_SPECIALIZATION_B_5P5_FIXED         = 332,
			SDT_DEFFENCE_SPECIALIZATION_B_6P_FIXED          = 333,
			SDT_WEAPON_ORIGIN                               = 334,
			SDT_DEFENCE_ORIGIN                              = 335,
			SDT_WEAPON_GROWTH                               = 336,
			SDT_DEFENCE_GROWTH                              = 337,
			SDT_WEAPON_HUNTING                              = 338,
			SDT_DEFENCE_HUNTING                             = 339,
			SDT_WEAPON_COUNTER                              = 340,
			SDT_DEFENCE_COUNTER                             = 341,
			SDT_WEAPON_SHINE_ORIGIN                         = 342,
			SDT_DEFENCE_SHINE_ORIGIN                        = 343,
			SDT_WEAPON_SHINE_GROWTH                         = 344,
			SDT_DEFENCE_SHINE_GROWTH                        = 345,
			SDT_WEAPON_SHINE_HUNTING                        = 346,
			SDT_DEFENCE_SHINE_HUNTING                       = 347,
			SDT_WEAPON_SHINE_COUNTER                        = 348,
			SDT_DEFENCE_SHINE_COUNTER                       = 349,
			SDT_WEAPON_ADD_MAXIMIZE_6P_FIXED                = 350,
			SDT_DEFFENCE_ADD_MAXIMIZE_3P_FIXED              = 351,
			SDT_WEAPON_ADD_MAXIMIZE_7P_FIXED                = 352,
			SDT_DEFFENCE_ADD_MAXIMIZE_3P5_FIXED             = 353,
			SDT_WEAPON_ADD_MAXIMIZE_8P_FIXED                = 354,
			SDT_DEFFENCE_ADD_MAXIMIZE_4P_FIXED              = 355,
			SDT_WEAPON_ADD_MOVE_SPEED_6P_FIXED              = 356,
			SDT_DEFFENCE_ADD_MOVE_SPEED_3P_FIXED            = 357,
			SDT_WEAPON_ADD_MOVE_SPEED_7P_FIXED              = 358,
			SDT_DEFFENCE_ADD_MOVE_SPEED_3P5_FIXED           = 359,
			SDT_WEAPON_ADD_MOVE_SPEED_8P_FIXED              = 360,
			SDT_DEFFENCE_ADD_MOVE_SPEED_4P_FIXED            = 361,
			SDT_WEAPON_ADD_HP_6P_FIXED                      = 362,
			SDT_DEFFENCE_ADD_HP_3P_FIXED                    = 363,
			SDT_WEAPON_ADD_HP_7P_FIXED                      = 364,
			SDT_DEFFENCE_ADD_HP_3P5_FIXED                   = 365,
			SDT_WEAPON_ADD_HP_8P_FIXED                      = 366,
			SDT_DEFFENCE_ADD_HP_4P_FIXED                    = 367,
			SDT_WEAPON_ADD_DAMAGE_6P_FIXED_PVP              = 368,
			SDT_DEFFENCE_ADD_DAMAGE_3P_FIXED_PVP            = 369,
			SDT_WEAPON_ADD_DAMAGE_7P_FIXED_PVP              = 370,
			SDT_DEFFENCE_ADD_DAMAGE_3P5_FIXED_PVP           = 371,
			SDT_WEAPON_ADD_DAMAGE_8P_FIXED_PVP              = 372,
			SDT_DEFFENCE_ADD_DAMAGE_4P_FIXED_PVP            = 373,
			SDT_WEAPON_CRITICAL_6P_FIXED_PVP                = 374,
			SDT_DEFFENCE_CRITICAL_3P_FIXED_PVP              = 375,
			SDT_WEAPON_CRITICAL_7P_FIXED_PVP                = 376,
			SDT_DEFFENCE_CRITICAL_3P5_FIXED_PVP             = 377,
			SDT_WEAPON_CRITICAL_8P_FIXED_PVP                = 378,
			SDT_DEFFENCE_CRITICAL_4P_FIXED_PVP              = 379,
			SDT_WEAPON_SPEED_6P_FIXED_PVP                   = 380,
			SDT_DEFFENCE_SPEED_3P_FIXED_PVP                 = 381,
			SDT_WEAPON_SPEED_7P_FIXED_PVP                   = 382,
			SDT_DEFFENCE_SPEED_3P5_FIXED_PVP                = 383,
			SDT_WEAPON_SPEED_8P_FIXED_PVP                   = 384,
			SDT_DEFFENCE_SPEED_4P_FIXED_PVP                 = 385,
			SDT_WEAPON_DAMAGE_REDUCE_6P_FIXED_PVP           = 386,
			SDT_DEFFENCE_DAMAGE_REDUCE_3P_FIXED_PVP         = 387,
			SDT_WEAPON_DAMAGE_REDUCE_7P_FIXED_PVP           = 388,
			SDT_DEFFENCE_DAMAGE_REDUCE_3P5_FIXED_PVP        = 389,
			SDT_WEAPON_DAMAGE_REDUCE_8P_FIXED_PVP           = 390,
			SDT_DEFFENCE_DAMAGE_REDUCE_4P_FIXED_PVP         = 391,
			SDT_WEAPON_ADD_MAXIMIZE_6P_FIXED_PVP            = 392,
			SDT_DEFFENCE_ADD_MAXIMIZE_3P_FIXED_PVP          = 393,
			SDT_WEAPON_ADD_MAXIMIZE_7P_FIXED_PVP            = 394,
			SDT_DEFFENCE_ADD_MAXIMIZE_3P5_FIXED_PVP         = 395,
			SDT_WEAPON_ADD_MAXIMIZE_8P_FIXED_PVP            = 396,
			SDT_DEFFENCE_ADD_MAXIMIZE_4P_FIXED_PVP          = 397,
			SDT_WEAPON_ADD_HP_6P_FIXED_PVP                  = 398,
			SDT_DEFFENCE_ADD_HP_3P_FIXED_PVP                = 399,
			SDT_WEAPON_ADD_HP_7P_FIXED_PVP                  = 400,
			SDT_DEFFENCE_ADD_HP_3P5_FIXED_PVP               = 401,
			SDT_WEAPON_ADD_HP_8P_FIXED_PVP                  = 402,
			SDT_DEFFENCE_ADD_HP_4P_FIXED_PVP                = 403,
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
		};

		enum MAGIC_STONE_ITEM_ID
		{
			MSI_NONE = 0,
			MSI_MAGIC_STONE				= 109945,
			MSI_RARE_MAGIC_STONE		= 109955,
			//{{ 2010. 04. 22  최육사	소켓 개편
			MSI_HARMONY_MAGIC_STONE		= 132110,
			MSI_MYSTERY_MAGIC_STONE		= 132111,
			MSI_SHINE_MAGIC_STONE		= 132112,
			MSI_BATTLE_MAGIC_STONE		= 132113,
			MSI_INCREASE_MAGIC_STONE	= 132114,
			MSI_HIT_MAGIC_STONE			= 132115,
			//}}
		};

		enum SOCKET_COUNT
		{
			SC_NONE           = -1,

			SC_WEAPON_LOW     = 0,
			SC_WEAPON_NORMAL  = 1,
			SC_WEAPON_RARE    = 2,
			SC_WEAPON_ELITE   = 3,
			SC_WEAPON_UNIQUE  = 4,

			SC_DEFENCE_LOW    = 0,
			SC_DEFENCE_NORMAL = 1,
			SC_DEFENCE_RARE   = 1,
			SC_DEFENCE_ELITE  = 2,
			SC_DEFENCE_UNIQUE = 3,
		};

		//{{ kimhc // 2010.3.31 // 비밀던전 작업(셋트아이템효과)
#ifdef SERV_SECRET_HELL
		enum SOCKET_ITEM_EFFECT_EX				// 2010년 05월 비던 패치 이후부터 추가된 소켓 효과 관련 ENUM값
		{
			SIEE_NONE							= 0,	/// 추가효과 없음
			SIEE_STAT_UP,						/// 상태이상에 걸리면 X% 확률로 모든 방어력이 Y초간 Z% 향상 되는 효과
			SIEE_UP_MANA_RECOVERY_VALUE,		/// 상태이상에 걸리면 마나회복속도가 X초간 Y씩 증가한다.
			SIEE_CURE_DEBUFF,					/// 피격시 x % 확률로 자신에게 걸린 상태이상 y개를 해제한다. (z초안에 다시 발동 안함)
			SIEE_DEBUFF_ATTACKER,				/// 피격시 자신을 타격한 대상을 X초간 Y% 확률로 Z 상태로만든다.
			SIEE_BONE_SHIELD,					/// 피격시 x%확률로 피해를 흡수하는 뼈방패를 생성 하여 타격의 z%흡수(w초 경과시 사라짐)				
			SIEE_ATTACK_SPEED_UP,				/// 공격 성공 시 x% 확률로 애니메이션속도(공격속도)를 Y% 증가 시켜줌

			//{{ kimhc // 2010.12.29 // 하멜 마을 추가시 들어가는 셋트 아이템 효과
			SIEE_ICE_NOVA,						/// 피격시 ICE_NOVA 이펙트 셋 생성
			//}} kimhc // 2010.12.29 // 하멜 마을 추가시 들어가는 셋트 아이템 효과
#ifdef ADD_SOCKET_FOR_SECRET_SET
			SIEE_POISON_BURST,					/// 피격 시 Poison 세트 효과
			SIEE_ABSORB_HP,						/// 피격 시 HP 흡수 세트 효과
#endif
#ifdef HAMEL_SECRET_DUNGEON // 김태환
			SIEE_ABSORB_HP_NO_REACT,			/// 피격 시 HP 흡수 세트 효과 ( 경직 제거 )
			SIEE_ACTIVE_BUFF,					/// 피격 시 버프 적용 효과
#endif // HAMEL_SECRET_DUNGEON
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// June buff port Phase 16-data: June's SIEE 12-20 (Enum.lua values). June files all of them (31fe4730);
			/// March has no ProcessSocketEffectEx case for any (SIEE 15's aura: 16-step3b).
			SIEE_PLAY_EFFECTSET                             = 12,
			SIEE_BUFF_WHEN_SUCCESS_ATTACK                   = 13,
			SIEE_PLAY_EFFECTSET_WHEN_SUCCESS_ATTACK         = 14,
			SIEE_ALWAYS_PLAY_EFFECT_SET                     = 15,
			SIEE_PLAY_EFFECTSET_TO_VICTIM_WHEN_SUCCESS_ATTACK = 16,
			SIEE_BUFF_PERIODICALLY                          = 17,
			SIEE_PLAY_EFFECTSET_WHEN_DIE                    = 18,
			SIEE_RETURN_FROM_DEATH                          = 19,
			SIEE_DOWN                                       = 20,
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY

		};

		enum STAT_UP_TYPE	// 상태이상에 걸릴때 % UP 될 수 있는 스탯의 종류
		{
			SUT_NONE			= 0,
			SUT_PHYSIC_ATT,			// 물공
			SUT_MAGIC_ATT,			// 마공
			SUT_PHYSIC_DEF,			// 물방
			SUT_MAGIC_DEF,			// 마방
			SUT_BOTH_ATT,			// 물공, 마공
			SUT_BOTH_DEF,			// 물방, 마방
			SUT_ALL,				// HP를 제외한 모든 스탯
		};
#endif SERV_SECRET_HELL

#ifdef HAMEL_SECRET_DUNGEON // 김태환
		enum POSSIBLE_USE_PLACE			/// 사용 가능한 위치
		{
			PUP_NONE							= -1,
			PUP_ALL								= 0,	/// 모든곳
			PUP_DUNGEON_ONLY					= 1,	/// 던전 전용
			PUP_PVP_ONLY						= 2,	/// 대전 전용
			PUP_END								= 3,
		};

		enum HYPER_MODE_EFFECTIVE_TYPE	/// 각성시 사용할 효과 타입
		{
			HMET_NONE									= -1,
			HMET_USE_BUFF								= 0,	/// 버프 사용
			HMET_ERASE_DEBUFF_AND_ADD_MP				= 1,	/// (define ADJUST_SECRET_ITEM_OPTION) 디버프 삭제, 삭제 갯수에 관계없이 mp 회복으로 수정 <- 디버프 삭제 및 삭제 개수당 MP 회복
			HMET_ADD_HP_IMMEDIATELY						= 2,	/// HP 즉시 회복
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			HMET_FEVER									= 3,	/// June (Enum.lua); no awakening handler in March (not ported)
			HMET_END									= 4,	/// June
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
			HMET_END									= 3,
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
		};

		struct InfoByUseHyperMode
		{
			InfoByUseHyperMode()
			{
				m_ePossibleUsePlace			= PUP_ALL;
				m_eHyperModeEffectiveType	= HMET_NONE;
				m_fEffectiveValue			= 0.f;
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				m_fJuneCoolTime				= 0.f;
				m_iJuneSocketID				= 0;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
#ifdef ADJUST_SECRET_ITEM_OPTION //김창한
				m_fCoolTime					= 0.f;
#endif //ADJUST_SECRET_ITEM_OPTION
			}

			POSSIBLE_USE_PLACE				m_ePossibleUsePlace;		/// 사용 가능한 지역
			HYPER_MODE_EFFECTIVE_TYPE		m_eHyperModeEffectiveType;	/// 각성시 사용할 효과 타입
			CX2BuffFactorPtr				m_pBuffFactorPtr;			/// 사용할 버프
			float							m_fEffectiveValue;			/// 적용 값
#ifdef ADJUST_SECRET_ITEM_OPTION //김창한
			float							m_fCoolTime;				/// 적용 후 재사용 가능할때까지의 시간
#endif //ADJUST_SECRET_ITEM_OPTION
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// June buff port 16-step3b6: June's entry is 24 bytes (SumSocketOption 3204f190 @3204fc22..fca6): place, type,
			/// buff, value, then the socket's m_fIntervalTime (+0x10, unconditional in June; US has no ADJUST_SECRET_ITEM_OPTION)
			/// and its socket ID (+0x14), the key of the per-socket cool-time store (+0x8b98) ApplyBuffByUseHyperMode uses.
			float							m_fJuneCoolTime;
			int								m_iJuneSocketID;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
		};
		typedef std::pair<POSSIBLE_USE_PLACE, CX2BuffFactorPtr> BuffInfoByUseHyperMode;
#endif // HAMEL_SECRET_DUNGEON

			//}} kimhc // 2010.3.31 // 비밀던전 작업(셋트아이템효과)
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		struct KItemStatRelLVData
		{
			// CX2SocketItem 클래스에서 CX2Item클래스에 접근 할 수 없어서 작성 한 구조체
			float m_fBaseHPRelLV;
			float m_fAtkPhysicRelLV;
			float m_fAtkMagicRelLV;
			float m_fDefPhysicRelLV;
			float m_fDefMagicRelLV;
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// June buff port 16-step3b7 item 6: June's sixth float m_fAllStatRelLV (socket templet +0x268, loader 32726850 @32727838,
			/// aggregate add 3207cd70 @3207d3fc), added to every stat's level term by CalculateRealStatData (328d68c0)
			float m_fAllStatRelLV;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY

			KItemStatRelLVData()
			{
				Init();
			}

			inline bool IsValideCheck() const
			{
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				/// June 328d6f60: the sum of all six (its order AtkPhysic + BaseHP + ..., the same value) == 0 -> the templet's base stats
				return 0.f != (m_fBaseHPRelLV + m_fAtkPhysicRelLV + m_fAtkMagicRelLV + m_fDefPhysicRelLV + m_fDefMagicRelLV + m_fAllStatRelLV);
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
				return 0.f != (m_fBaseHPRelLV + m_fAtkPhysicRelLV + m_fAtkMagicRelLV + m_fDefPhysicRelLV + m_fDefMagicRelLV);
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			}

			void AddStat( const KItemStatRelLVData& sStatRelLVData_)
			{
				this->m_fBaseHPRelLV += sStatRelLVData_.m_fBaseHPRelLV;
				this->m_fAtkPhysicRelLV += sStatRelLVData_.m_fAtkPhysicRelLV;
				this->m_fAtkMagicRelLV += sStatRelLVData_.m_fAtkMagicRelLV;
				this->m_fDefPhysicRelLV += sStatRelLVData_.m_fDefPhysicRelLV;
				this->m_fDefMagicRelLV += sStatRelLVData_.m_fDefMagicRelLV;
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				this->m_fAllStatRelLV += sStatRelLVData_.m_fAllStatRelLV;	/// June 3207cd70 @3207d3fc (ConvertAndAdd) and 328d6f60's sum
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			}

			void Init()
			{
				m_fBaseHPRelLV = 0.f;
				m_fAtkPhysicRelLV = 0.f;
				m_fAtkMagicRelLV = 0.f;
				m_fDefPhysicRelLV = 0.f;
				m_fDefMagicRelLV = 0.f;
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				m_fAllStatRelLV = 0.f;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			}

			bool operator < ( const KItemStatRelLVData& rhs ) const 
			{
				if( m_fBaseHPRelLV < rhs.m_fBaseHPRelLV )
					return true;
				else if( m_fBaseHPRelLV > rhs.m_fBaseHPRelLV )
					return false;

				if( m_fAtkPhysicRelLV < rhs.m_fAtkPhysicRelLV )
					return true;
				else if( m_fAtkPhysicRelLV > rhs.m_fAtkPhysicRelLV )
					return false;

				if( m_fAtkMagicRelLV < rhs.m_fAtkMagicRelLV )
					return true;
				else if( m_fAtkMagicRelLV > rhs.m_fAtkMagicRelLV )
					return false;

				if( m_fDefPhysicRelLV < rhs.m_fDefPhysicRelLV )
					return true;
				else if( m_fDefPhysicRelLV > rhs.m_fDefPhysicRelLV )
					return false;

				if( m_fDefMagicRelLV < rhs.m_fDefMagicRelLV )
					return true;
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				/// June's cache key comparator 328d8880 compares all six floats in order, the sixth last
				else if( m_fDefMagicRelLV > rhs.m_fDefMagicRelLV )
					return false;

				if( m_fAllStatRelLV < rhs.m_fAllStatRelLV )
					return true;
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY

				return false;
			}
		};


#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈
		struct SkillLvUpSocketOption
		{
			CX2Unit::UNIT_CLASS m_eUnitClass;
			CX2SkillTree::SKILL_ID m_eSkillID;
			int m_iSkillLvUPNum;

			SkillLvUpSocketOption( CX2Unit::UNIT_CLASS eUnitClass_, CX2SkillTree::SKILL_ID eSkillID_, int iSkillLvUPNum_ ):
			m_eUnitClass(eUnitClass_), m_eSkillID(eSkillID_), m_iSkillLvUPNum(iSkillLvUPNum_)
			{}

		};
#endif // UPGRADE_SKILL_SYSTEM_2013

#endif // SERV_NEW_ITEM_SYSTEM_2013_05

		struct SocketData
		{
		public:
//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
			//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

			static const float		MAX_HYPERMODETIME;
			static const float 		MAX_REPAIR_PRICE_DISCOUNT;
			static const float		MAX_SUPER_ARMOR_RATE;
			static const float		MAX_SUPER_ARMOR_TIME;
			static const float		MAX_DUBLE_ATTACK_RATE;
			
		public:
			int						m_ID;
			SOCKET_DATA_TYPE		m_Type;							//옵션타입
			CX2Stat::Stat			m_Stat;							//능력치증가
			float					m_fMoveSpeed;					//달리기속도증가(%)(최대 30%)
			float					m_fJumpSpeed;					//점프속도증가(%)(최대 30%)

			float 					m_fAnimationSpeedRate;			//[0,100]타격, 피격, 피격후 회복 동작시 애니메이션 속도 향상

			float					m_fHyperModeChargeRate;			//소울게이지차징속도 증가(%)(최대 64.2%)
			float					m_fHyperModeTime;				//각성시간증가(40%)
			float					m_fRepairPriceSale;				//내구도수리비감소(%)(최대90%)
			
			CX2BuffFactorPtr							m_ptrBuffFactor;
			//vector<CX2BuffFactorPtr>					m_vecBuffFactorPtr;
			CX2DamageManager::ExtraDamageData			m_SocketExtraDamage;				// 중복안되는 extra damage
			vector<CX2DamageManager::ExtraDamageData>	m_MultipleSocketExtraDamage;		// 중복 되는, 즉, 여러개 동시에 적용될 수 있는 extra damage
			
			
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05 // 오현빈
			// 공용화 아이템에 대해, 장착중인 유저의 직업에 따라 다른 스킬을 보여 주기 위해 그룹 추가
			vector<int> m_vecSkillLevelUpGroupID;		//스킬레벨그룹 ID
			vector< CX2SocketItem::SkillLevelUpIDAndNum > m_vecSkillLevelUpID;		//스킬레벨
#else
			vector<CX2SkillTree::SKILL_ID> m_vecSkillLevelUpID;		//스킬레벨
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

			float					m_fDamageReduce;
			float					m_fPercentDamageReduce;

			std::wstring			m_DescEvadeByMonsterAttack;		// 특정몬스터에 의한 데미지 감소의 아이템 툴팁을 스크립트에서 직접 작성하는 경우에 사용됨
			set< int >				m_setMonsterIDDamageReduced;	//한쌍으로 움직인다. //특정 몬스터 리스트 
			float					m_fDamageReduceByMonsterAttack; //특정 몬스터들로 부터 감소될 데미지
			float					m_fPercentDRByMonsterAttack;	//특정몬스터에게 데미지 감소될 확률

			float					m_fEvadePercent;				// [0,1] 회피율

			float					m_fAntiEvadePercent;			// [0,1] 명중율


			float					m_fPercentCritical;				//크리티컬 터지는 확률
			float					m_fCritical;					//크리티컬 터졌을 때 몇배?


			//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
			float					m_fAdditionalDefenceValue;	/// 최종적인 데미지에 계산될 데미지 감소 수치 (이 수치를 가지고 % 연산을 함)
			float					m_fAdditionalAttackValue;	/// 최종적인 데미지에 계산될 추가 데미지
			//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

			vector<D3DXVECTOR2>		m_vecHpUpByAttack;				// 공격을 했을 때 증가하는 HP %, x: 발동확률, y: 증가하는 HP %
			vector<D3DXVECTOR2>		m_vecHpUpByAttacked;			// 공격을 당했을 때 증가하는 HP % x: 발동확률, y: 증가하는 HP %

			//{{ kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정
			vector<D3DXVECTOR2>		m_vecMpUpByAttack;				// 공격을 했을 때 증가하는 MP양, x: 발동확률, y: 증가하는 MP양

#ifdef ADJUST_SECRET_ITEM_OPTION //김창한
			vector<D3DXVECTOR4>		m_vecMpUpByAttacked;			// 공격을 당했을 때 증가하는 MP양 x: 발동확률, y: 증가하는 MP양, z: 제한 쿨타임, w: 현재 쿨타임.
#else //ADJUST_SECRET_ITEM_OPTION
			vector<D3DXVECTOR2>		m_vecMpUpByAttacked;			// 공격을 당했을 때 증가하는 MP양 x: 발동확률, y: 증가하는 MP양
#endif //ADJUST_SECRET_ITEM_OPTION

			//}} kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정

			//{{ 2011.09.05 / 이지헌 / 대만/홍콩 / 마법의 목걸이 개편 / 병합 : 강정훈
#ifdef MAGIC_NEC_FIX
			float					m_fMPMultiplyByAll;				// 엠피 회수량 증가치( 마목 효과 )
#endif
			//}}

			float					m_fDamageUpByAMADS;				//m_fDamageUpByAttackMonsterAtDonwState 다운상태의 몬스터 공격시 데미지 증가량, 몇배인가

			CX2DamageManager::EXTRA_DAMAGE_TYPE		m_ResistExtraDamageType;		//특수 데미지 저항 타입
			float									m_fResistExtraDamage;			//몇퍼센트 감소시키는가?

			CX2DamageManager::EXTRA_DAMAGE_TYPE		m_ImmuneExtraDamageType;		//특수데미지 완전 이뮨 타입
			float									m_fPercentImmuneExtraDamage;	//몇퍼센트로 이뮨 될지..


			int						m_AllSkillLevelUp;
	
			set<int>				m_setMonsterIDForDamageUp;
			float					m_fDamageUpPerAtSpecificMonster;	//특정 몬스터에게 데미지 증가

			set<int>				m_setMonsterIDForEvade;
			float					m_fEvadePercentByMonsterAttack;

			float					m_fSpeedUpManaGather;

			float					m_fIncreaseEDPercent;
			float					m_fIncreaseDCBExpPercent;			// dungeonClearBonusExp

			bool					m_bDungeonOnly;
			bool					m_bPVPOnly;
			bool					m_bOverlap;

			float					m_fPercentSuperArmor;				// [0,1] 피격시 일정확율로 슈퍼아머
			float					m_fTimeSuperArmor;					// 슈퍼아머 시간

			int						m_aiResistEnchant[ CX2EnchantItem::ET_END ];
#ifdef PET_AURA_SKILL
			float					m_afAttackEnchant[ CX2EnchantItem::ET_END ];
#endif

//{{ kimhc // 2010.4.1 // 비밀던전 작업(셋트아이템효과)
#ifdef SERV_SECRET_HELL
			SOCKET_ITEM_EFFECT_EX	m_eTypeSocketItemEffectEx;	// 적용될 추가 소켓 효과			
			STAT_UP_TYPE			m_eStatUpType;			// 증가 시킬 스탯 타입
			float					m_fDurationTime;		// 지속시간
			float					m_fProbability;			// 발생확률
			float					m_fIncOrDecRate;		// 증가 or 감소 율
			float					m_fIncOrDecVal;			// 증가 or 감소 량			
			int						m_iCount;				// 갯수 (해제할 상태이상 or 5번 피격 당하면 사라짐 등에 사용함)
			float					m_fIntervalTime;		// 발생될수있는 시간 간격(발동된 후에 Inverval 시간 만큼은 다시 발동 될 수 없음)
			vector<wstring>			m_vecEffectName;		// 해당 효과에 쓰일 이펙트 명
#endif SERV_SECRET_HELL
//}} kimhc // 2010.4.1 // 비밀던전 작업(셋트아이템효과)

#ifdef SUPER_SOCKET_IN_TITLE
			float					m_fPercentSuperArmorDungeon;		/// 던전일 경우 최고 값 확률
			float					m_fTimeSuperArmorDungeon;			/// 던전일 경우 최고 값 시간
			float					m_fPercentSuperArmorPVP;			/// 대전일 경우 최고 값 확률
			float					m_fTimeSuperArmorPVP;				/// 대전일 경우 최고 값 시간
#endif

#ifdef BUFF_TEMPLET_SYSTEM 
			int						m_iAddMPValue;						// MP 증가량
			int						m_iMPChangeValue;					// MP 자연 회복량
			float					m_fHPRelativeChangeRate;			// 10초당 HP 회복
			float					m_fSkillDamageUpRate;				// 스킬 공격 증가 데미지값
			float					m_fAllAttackEnchantRate;			// 속성 발동 확률 % 증가
#endif BUFF_TEMPLET_SYSTEM

			int						m_iSummonNpcId;			// 소환할 npc id
			float					m_fSummonNpcRate;		// 타격시 소환확율
			float					m_fSummonNpcCoolTime;	// 소켓 쿨타임

#ifdef PVP_SEASON2_SOCKET
			bool					m_bIsLevelLinkedStat;	/// 레벨 적용 아이템 여부
			int						m_iAddLevel;			/// 추가 적용 레벨
#endif
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
			KItemStatRelLVData		m_kStatRelLVData;	/// 스탯 별 추가 레벨
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
			// POWER_RATE_TYPE 상수 값을 곱해서 증가시켜주는 스킬 레벨업 소켓 옵션
			int						m_iAllSkillLevelUpEx;
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE

#ifdef HAMEL_SECRET_DUNGEON // 김태환
			CX2BuffFactorPtr			m_ptrBuffFactorCustomUse;		/// 특별한 목적으로 사용되는 버프 펙터 저장 컨테이너
																		/// ( 타격시 특정 디버프 적용, 피격시 특정 버프 적용, 각성시 특정 버프 적용 )
			HYPER_MODE_EFFECTIVE_TYPE	m_eHyperModeEffectiveType;		/// 각성시 사용하는 효과 설정값
			vector<InfoByUseHyperMode>	m_vecInfoByUseHyperMode;		/// 각성시 사용하는 버프 설정 저장 컨테이너
			float						m_fEffectiveValue;				/// 효과 값
#endif // HAMEL_SECRET_DUNGEON

#ifdef SERV_GROW_UP_SOCKET
			map<int, vector<int>>	m_mapGrowUpSocketID;		// 성장 소켓 그룹
#endif //SERV_GROW_UP_SOCKET

#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// June buff port Phase 16: the SocketDataCommon fields June's loader 32726850 fills and March lacks.
			/// The comments give June's offset (the same in the templet and in the per-unit aggregate).
			struct JuneConditionalBuff				/// June +0x29c entry (0x10 bytes), pushed @32735bc5 (3207b5d0)
			{
				UINT	m_uiBuffFactorID;				/// +0 BUFF_FACTOR_ID
				int		m_iBuffLevel;					/// +4 the second m_sBuffLevel
				bool	m_bKillAbsoluteMonster;			/// +8
				bool	m_bAlwaysPlayEffectSet;			/// +9 m_eTypeSetItemEffectEx == 15 (SIEE_ALWAYS_PLAY_EFFECT_SET)
				int		m_iUseSkillCategory;			/// +0xc the key m_fIncreaseDmgEnemyHPFew_HpRate, read as an int: June uses it as
														/// the skill category (32070510) the entry fires on (CommonStateStart @32060c9c)
			};
			int		m_iDungeonOnlyType;					/// +0x18c DUNGEON_ONLY_TYPE (a DUNGEON_TYPE, 0 = any)
			float	m_fCriticalAfflicted;				/// +0xc4
			float	m_fAdditionalDamageAfflicted;		/// +0xc8 (78's term +0x5194)
			float	m_fAdditionalDamage;				/// +0xcc (78's term +0x5248)
			float	m_fAntiCriticalRate;				/// +0xd0
			float	m_fAntiAddDamageRate;				/// +0xd4 (78's term +0x5ac8)
			float	m_fAdditionalAttackedDamageValue;	/// +0xe0
			float	m_fMPIncByAttack;					/// +0x124 = m_fMPMultiplyByAll + m_fMPIncByAttack_ReturnRate (@32732977)
			float	m_fMPIncByAttacked;					/// +0x12c = m_fMPMultiplyByAll + m_fMPIncByAttacked_ReturnRate (@32732be4)
			float	m_fCharacterSpecializationA;		/// +0x134
			float	m_fCharacterSpecializationB;		/// +0x138
			float	m_fDamageMaximizeRate;				/// +0x13c (BBT 92's option seed)
			float	m_fBonusTitleMissionCountingRate;	/// +0x298 (loaded; no client reader found, not summed by 3207cd70)
			vector<JuneConditionalBuff>	m_vecConditionalBuff;	/// +0x29c
			float	m_fAtkPhysicPiercingRate;			/// +0x2ac
			float	m_fAtkMagicPiercingRate;			/// +0x2b0
			float	m_fHPRecoveryRate;					/// +0x2b4
			float	m_fIncreaseItemDropRate;			/// +0x2b8 (no client reader in June: server-side)
			float	m_fIncreaseDmgRateToBoss;			/// +0x2bc
			float	m_fIncreaseDefRateToBoss;			/// +0x2c0
			float	m_fDecreaseActiveSkillCooltime;		/// +0x2c4
			float	m_fDecrease1stSA_SkillCooltime;		/// +0x2c8
			float	m_fDecrease2ndSA_SkillCooltime;		/// +0x2cc
			float	m_fDecrease3rdSA_SkillCooltime;		/// +0x2d0
			float	m_fIncreaseDmgEnemyHPFew_DmgRate;	/// +0x2d4
			float	m_fIncreaseDefMyHPFew_DefRate;		/// +0x2d8
			float	m_fDecreaseDebuffTimeRate;			/// +0x2dc
			float	m_fDecrease_MPCostRate;				/// +0x2e0
			float	m_fIncreaseDmgRate_MyHpFew;			/// +0x2e4
			float	m_fAllSkillCooltimeDecreaseRate;	/// +0x2e8
			float	m_fAllSkillDamageIncreaseRate;		/// +0x2ec
			bool	m_bIsRankLinkedStat;				/// +0x294 (June 3204f190's rank branch: scaled copy 320925f0, see SumSocketOption)
			bool	m_bIsPVERankLinkedStat;				/// +0x58
			/// 16-step3b5: June's skill-group power sockets. The loader (32726850 @3272e4b5..3272eb3e) fills the
			/// first three on the socket templet; the per-unit aggregate gets only the map, from the equipped-slot
			/// pass in SumSocketOption (31fe14f0). ConvertAndAdd (32080670 / 3207cd70) carries none of them.
			std::map<int, float>	m_mapJuneSkillAdditionalPowerRate;	/// +0x48 { skill, rate }: templet = the "Group N" members
																		/// (or skill IDs), first insert wins; aggregate = by equipped skill
			vector<int>		m_vecJuneAdditionalPowerSkillType;		/// +0x98 m_SkillType (327150b0: 1..6 -> June skill type 6, 8, 14, 15, 16, 13)
			vector<int>		m_vecJuneAdditionalPowerRateType;		/// +0xa8 m_PowerRate (32715110: 1..6 -> power-rate type 0..5)
			float			m_fJuneSkillAdditionalPowerRate;		/// +0xb8 m_fSkillAdditionalPowerRate (0 unless the group string parsed)

			void InitJuneSocketParity()					/// the ctor's and ReInit's (320922a0) zeroing of the fields above
			{
				m_iDungeonOnlyType = 0;
				m_fCriticalAfflicted = m_fAdditionalDamageAfflicted = m_fAdditionalDamage = 0.f;
				m_fAntiCriticalRate = m_fAntiAddDamageRate = m_fAdditionalAttackedDamageValue = 0.f;
				m_fMPIncByAttack = m_fMPIncByAttacked = 0.f;
				m_fCharacterSpecializationA = m_fCharacterSpecializationB = m_fDamageMaximizeRate = 0.f;
				m_fBonusTitleMissionCountingRate = 0.f;
				m_vecConditionalBuff.clear();
				m_fAtkPhysicPiercingRate = m_fAtkMagicPiercingRate = m_fHPRecoveryRate = m_fIncreaseItemDropRate = 0.f;
				m_fIncreaseDmgRateToBoss = m_fIncreaseDefRateToBoss = 0.f;
				m_fDecreaseActiveSkillCooltime = m_fDecrease1stSA_SkillCooltime = m_fDecrease2ndSA_SkillCooltime = m_fDecrease3rdSA_SkillCooltime = 0.f;
				m_fIncreaseDmgEnemyHPFew_DmgRate = m_fIncreaseDefMyHPFew_DefRate = m_fDecreaseDebuffTimeRate = 0.f;
				m_fDecrease_MPCostRate = m_fIncreaseDmgRate_MyHpFew = m_fAllSkillCooltimeDecreaseRate = m_fAllSkillDamageIncreaseRate = 0.f;
				m_bIsRankLinkedStat = m_bIsPVERankLinkedStat = false;
				m_mapJuneSkillAdditionalPowerRate.clear();
				m_vecJuneAdditionalPowerSkillType.clear();
				m_vecJuneAdditionalPowerRateType.clear();
				m_fJuneSkillAdditionalPowerRate = 0.f;
			}

			/// June 320925f0 (buff port Phase 16-step3b, item 8): the scaled copy SumSocketOption makes of a rank-linked
			/// socket. Called on a default-constructed SocketData: it takes the stat block and these fields from rhs_,
			/// each times fRate_, and copies the +0x29c entries unscaled; every other field keeps its default (June's
			/// copy drops e.g. m_fPercentDamageReduce while scaling m_fDamageReduce, JUNE_BUGS #50). The stat block is
			/// March's 11 floats of June's 13 (June's range P/M at indices 9/10 are not in March's Stat).
			void SetJuneRankLinkedScaled( const SocketData& rhs_, const float fRate_ )
			{
				m_Stat.m_fBaseHP	= rhs_.m_Stat.m_fBaseHP * fRate_;
				m_Stat.m_fAtkPhysic	= rhs_.m_Stat.m_fAtkPhysic * fRate_;
				m_Stat.m_fAtkMagic	= rhs_.m_Stat.m_fAtkMagic * fRate_;
				m_Stat.m_fDefPhysic	= rhs_.m_Stat.m_fDefPhysic * fRate_;
				m_Stat.m_fDefMagic	= rhs_.m_Stat.m_fDefMagic * fRate_;
				m_Stat.m_ExtraStat.m_fIncreaseHPRate		= rhs_.m_Stat.m_ExtraStat.m_fIncreaseHPRate * fRate_;
#ifdef PET_AURA_SKILL
				m_Stat.m_ExtraStat.m_fIncreaseMPRate		= rhs_.m_Stat.m_ExtraStat.m_fIncreaseMPRate * fRate_;
				m_Stat.m_ExtraStat.m_fIncreaseAtkPhysicRate	= rhs_.m_Stat.m_ExtraStat.m_fIncreaseAtkPhysicRate * fRate_;
				m_Stat.m_ExtraStat.m_fIncreaseAtkMagicRate	= rhs_.m_Stat.m_ExtraStat.m_fIncreaseAtkMagicRate * fRate_;
				m_Stat.m_ExtraStat.m_fIncreaseDefPhysicRate	= rhs_.m_Stat.m_ExtraStat.m_fIncreaseDefPhysicRate * fRate_;
				m_Stat.m_ExtraStat.m_fIncreaseDefMagicRate	= rhs_.m_Stat.m_ExtraStat.m_fIncreaseDefMagicRate * fRate_;
#endif PET_AURA_SKILL
				m_fAntiCriticalRate					= rhs_.m_fAntiCriticalRate * fRate_;					/// +0xd0
				m_fAntiAddDamageRate				= rhs_.m_fAntiAddDamageRate * fRate_;					/// +0xd4
				m_fAdditionalAttackValue			= rhs_.m_fAdditionalAttackValue * fRate_;				/// +0xdc
				m_fAdditionalDefenceValue			= rhs_.m_fAdditionalDefenceValue * fRate_;				/// +0xd8
				m_fAdditionalAttackedDamageValue	= rhs_.m_fAdditionalAttackedDamageValue * fRate_;		/// +0xe0
				m_fAnimationSpeedRate				= rhs_.m_fAnimationSpeedRate * fRate_;					/// +0x14
				m_fDamageReduce						= rhs_.m_fDamageReduce * fRate_;						/// +0x5c
				m_fHyperModeChargeRate				= rhs_.m_fHyperModeChargeRate * fRate_;					/// +0x18
				m_fHyperModeTime					= rhs_.m_fHyperModeTime * fRate_;						/// +0x1c
				m_fJumpSpeed						= rhs_.m_fJumpSpeed * fRate_;							/// +0x10
				m_fMoveSpeed						= rhs_.m_fMoveSpeed * fRate_;							/// +0xc
				m_fPercentCritical					= rhs_.m_fPercentCritical * fRate_;						/// +0xbc
				m_fMPIncByAttack					= rhs_.m_fMPIncByAttack * fRate_;						/// +0x124
				m_fMPIncByAttacked					= rhs_.m_fMPIncByAttacked * fRate_;						/// +0x12c
				m_fCharacterSpecializationA			= rhs_.m_fCharacterSpecializationA * fRate_;			/// +0x134
				m_fCharacterSpecializationB			= rhs_.m_fCharacterSpecializationB * fRate_;			/// +0x138
				m_fDamageMaximizeRate				= rhs_.m_fDamageMaximizeRate * fRate_;					/// +0x13c
				m_vecConditionalBuff				= rhs_.m_vecConditionalBuff;							/// +0x29c, entries copied (3207b5d0)
				m_fAtkPhysicPiercingRate			= rhs_.m_fAtkPhysicPiercingRate * fRate_;				/// +0x2ac..+0x2ec
				m_fAtkMagicPiercingRate				= rhs_.m_fAtkMagicPiercingRate * fRate_;
				m_fHPRecoveryRate					= rhs_.m_fHPRecoveryRate * fRate_;
				m_fIncreaseItemDropRate				= rhs_.m_fIncreaseItemDropRate * fRate_;
				m_fIncreaseDmgRateToBoss			= rhs_.m_fIncreaseDmgRateToBoss * fRate_;
				m_fIncreaseDefRateToBoss			= rhs_.m_fIncreaseDefRateToBoss * fRate_;
				m_fDecreaseActiveSkillCooltime		= rhs_.m_fDecreaseActiveSkillCooltime * fRate_;
				m_fDecrease1stSA_SkillCooltime		= rhs_.m_fDecrease1stSA_SkillCooltime * fRate_;
				m_fDecrease2ndSA_SkillCooltime		= rhs_.m_fDecrease2ndSA_SkillCooltime * fRate_;
				m_fDecrease3rdSA_SkillCooltime		= rhs_.m_fDecrease3rdSA_SkillCooltime * fRate_;
				m_fIncreaseDmgEnemyHPFew_DmgRate	= rhs_.m_fIncreaseDmgEnemyHPFew_DmgRate * fRate_;
				m_fIncreaseDefMyHPFew_DefRate		= rhs_.m_fIncreaseDefMyHPFew_DefRate * fRate_;
				m_fDecreaseDebuffTimeRate			= rhs_.m_fDecreaseDebuffTimeRate * fRate_;
				m_fDecrease_MPCostRate				= rhs_.m_fDecrease_MPCostRate * fRate_;
				m_fIncreaseDmgRate_MyHpFew			= rhs_.m_fIncreaseDmgRate_MyHpFew * fRate_;
				m_fAllSkillCooltimeDecreaseRate		= rhs_.m_fAllSkillCooltimeDecreaseRate * fRate_;
				m_fAllSkillDamageIncreaseRate		= rhs_.m_fAllSkillDamageIncreaseRate * fRate_;
			}
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			SocketData()
			{
				m_ID								= 0;
				m_Type								= SDT_NONE;//zz
				m_fMoveSpeed						= 0.0f;
				m_fJumpSpeed						= 0.0f;

				m_fAnimationSpeedRate				= 0.0f;
				m_fHyperModeChargeRate				= 0.0f;
				m_fHyperModeTime					= 0.0f;
				m_fRepairPriceSale					= 0.0f;

				m_fDamageReduce						= 0.0f;
				m_fPercentDamageReduce				= 0.0f;

				m_DescEvadeByMonsterAttack			= L"";//zz
				m_fDamageReduceByMonsterAttack		= 0.0f;//zz
				m_fPercentDRByMonsterAttack			= 0.0f;//zz

				m_fEvadePercent						= 0.0f;

				m_fAntiEvadePercent					= 0.0f;



				m_fPercentCritical					= 0.0f;	
				m_fCritical							= 0.0f;		

				//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
				m_fAdditionalDefenceValue	= 0.0f;	// 최종적인 데미지에 계산될 데미지 감소 수치 (이 수치를 가지고 % 연산을 함)
				m_fAdditionalAttackValue	= 0.0f;	// 최종적인 데미지에 계산될 추가 데미지
				//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

		//{{ kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정
		//}} kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정

				//{{ 2011.09.05 / 이지헌 / 대만/홍콩 / 마법의 목걸이 개편 / 병합 : 강정훈
#ifdef MAGIC_NEC_FIX
				m_fMPMultiplyByAll					= 0.0f;
#endif
				//}}
				

				m_fDamageUpByAMADS					= 0.0f;			

				m_ResistExtraDamageType				= CX2DamageManager::EDT_NONE;//zz
				m_fResistExtraDamage				= 0.0f;//zz

				m_ImmuneExtraDamageType				= CX2DamageManager::EDT_NONE;//zz			
				m_fPercentImmuneExtraDamage			= 0.0f;		//zz

				m_AllSkillLevelUp					= 0;
				m_fDamageUpPerAtSpecificMonster		= 0.0f;//zz
				
				m_fEvadePercentByMonsterAttack		= 0.0f;//zz


				m_fSpeedUpManaGather				= 0.0f;

				m_fIncreaseEDPercent				= 0.0f;
				m_fIncreaseDCBExpPercent			= 0.0f;

				m_bDungeonOnly						= false;//zz
				m_bPVPOnly							= false;
				m_bOverlap							= false;//zz

				m_fPercentSuperArmor				= 0.0f;
				m_fTimeSuperArmor					= 0.0f;

				for( int i=0; i< (int)CX2EnchantItem::ET_END ; i++ )
				{
					m_aiResistEnchant[i] = 0;
#ifdef PET_AURA_SKILL
					m_afAttackEnchant[i] = 0.f;
#endif
				}

				//{{ kimhc // 2010.4.1 // 비밀던전 작업(셋트아이템효과)
#ifdef SERV_SECRET_HELL
				m_eTypeSocketItemEffectEx			= SIEE_NONE;	// 적용될 추가 셋트아이템 효과
				m_eStatUpType				= SUT_NONE;	// 증가 시킬 스탯 타입
				m_fDurationTime						= 0.0f;			// 지속시간
				m_fProbability						= 0.0f;			// 발생확률
				m_fIncOrDecRate						= 0.0f;			// 증가 or 감소 율
				m_fIncOrDecVal						= 0.0f;			// 증가 or 감소 량			
				m_iCount							= 0;			// 갯수 (해제할 상태이상 or 5번 피격 당하면 사라짐 등에 사용함)
				m_fIntervalTime						= 0.0f;			// 발생될수있는 시간 간격(발동된 후에 Inverval 시간 만큼은 다시 발동 될 수 없음)
#endif SERV_SECRET_HELL
				//}} kimhc // 2010.4.1 // 비밀던전 작업(셋트아이템효과)

#ifdef SUPER_SOCKET_IN_TITLE
				m_fPercentSuperArmorDungeon			= 0.0f;
				m_fTimeSuperArmorDungeon			= 0.0f;
				m_fPercentSuperArmorPVP				= 0.0f;
				m_fTimeSuperArmorPVP				= 0.0f;
#endif

#ifdef BUFF_TEMPLET_SYSTEM
				m_iAddMPValue						= 0;			// MP 증가량
				m_iMPChangeValue					= 0;			// MP 자연 회복량
				m_fHPRelativeChangeRate				= 0.0f;			// 10초당 HP 회복
				m_fSkillDamageUpRate				= 0.0f;			// 스킬 공격 증가 데미지값
				m_fAllAttackEnchantRate				= 1.0f;			// 속성 발동 확률 % 증가
#endif BUFF_TEMPLET_SYSTEM

				m_iSummonNpcId						= 0;			// 소환할 npc id
				m_fSummonNpcRate					= 0.f;		// 타격시 소환확율
				m_fSummonNpcCoolTime				= 0.f;

#ifdef PVP_SEASON2_SOCKET
				m_bIsLevelLinkedStat	= false;
				m_iAddLevel				= 0;
#endif

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
				m_iAllSkillLevelUpEx = 0;
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE

#ifdef HAMEL_SECRET_DUNGEON // 김태환
				m_eHyperModeEffectiveType	= HMET_NONE;
				m_fEffectiveValue			= 0.f;
#endif // HAMEL_SECRET_DUNGEON
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				InitJuneSocketParity();
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			}

			//{{ // kimhc // 실시간 엘소드 중 실시간 아바타 및 장비 교체 중 소켓 옵션
#ifdef REAL_TIME_ELSWORD
			void ReInit()
			{
				m_Stat.Init();

				m_fMoveSpeed				= 0.0f;
				m_fJumpSpeed				= 0.0f;

				m_fAnimationSpeedRate		= 0.0f;
				m_fHyperModeChargeRate		= 0.0f;
				m_fHyperModeTime			= 0.0f;
				m_fRepairPriceSale			= 0.0f;

				m_fDamageReduce				= 0.0f;
				m_fPercentDamageReduce		= 0.0f;

				m_fEvadePercent				= 0.0f;

				m_fAntiEvadePercent			= 0.0f;

				m_fPercentCritical			= 0.0f;
				m_fCritical					= 0.0f;

				//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
				m_fAdditionalDefenceValue	= 0.0f;	// 최종적인 데미지에 계산될 데미지 감소 수치 (이 수치를 가지고 % 연산을 함)
				m_fAdditionalAttackValue	= 0.0f;	// 최종적인 데미지에 계산될 추가 데미지
				//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

				//{{ kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정
				m_vecHpUpByAttack.resize( 0  );
				m_vecHpUpByAttacked.resize( 0 );

				m_vecMpUpByAttack.resize( 0 );				// 공격을 했을 때 증가하는 MP양, x: 발동확률, y: 증가하는 MP양
				m_vecMpUpByAttacked.resize( 0 );			// 공격을 당했을 때 증가하는 MP양 x: 발동확률, y: 증가하는 MP양
				//}} kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정

				//{{ 2011.09.05 / 이지헌 / 대만/홍콩 / 마법의 목걸이 개편 / 병합 : 강정훈
#ifdef MAGIC_NEC_FIX
				m_fMPMultiplyByAll			= 0.0f;
#endif
				//}}

				m_fDamageUpByAMADS			= 0.0f;

				m_fSpeedUpManaGather		= 0.0f;
				m_fIncreaseEDPercent		= 0.0f;
				m_fIncreaseDCBExpPercent	= 0.0f;

				m_fPercentSuperArmor		= 0.0f;
				m_fTimeSuperArmor			= 0.0f;

				m_AllSkillLevelUp			= 0;

				m_vecSkillLevelUpID.resize( 0 );

				m_SocketExtraDamage.Init();
				m_MultipleSocketExtraDamage.resize( 0 );

				for( int i=0; i< (int)CX2EnchantItem::ET_END ; i++ )
				{
					m_aiResistEnchant[i] = 0;
#ifdef PET_AURA_SKILL
					m_afAttackEnchant[i] = 0.f;
#endif
				}

				//{{ kimhc // 2010.4.1 // 비밀던전 작업(셋트아이템효과)
#ifdef SERV_SECRET_HELL
				m_eTypeSocketItemEffectEx			= SIEE_NONE;	// 적용될 추가 셋트아이템 효과
				m_eStatUpType				= SUT_NONE;	// 증가 시킬 스탯 타입
				m_fDurationTime						= 0.0f;			// 지속시간
				m_fProbability						= 0.0f;			// 발생확률
				m_fIncOrDecRate						= 0.0f;			// 증가 or 감소 율
				m_fIncOrDecVal						= 0.0f;			// 증가 or 감소 량			
				m_iCount							= 0;			// 갯수 (해제할 상태이상 or 5번 피격 당하면 사라짐 등에 사용함)
				m_fIntervalTime						= 0.0f;			// 발생될수있는 시간 간격(발동된 후에 Inverval 시간 만큼은 다시 발동 될 수 없음)
				m_vecEffectName.resize(0);
#endif SERV_SECRET_HELL
				//}} kimhc // 2010.4.1 // 비밀던전 작업(셋트아이템효과)

#ifdef SUPER_SOCKET_IN_TITLE
				m_fPercentSuperArmorDungeon			= 0.0f;
				m_fTimeSuperArmorDungeon			= 0.0f;
				m_fPercentSuperArmorPVP				= 0.0f;
				m_fTimeSuperArmorPVP				= 0.0f;
#endif

#ifdef BUFF_TEMPLET_SYSTEM
				m_iAddMPValue						= 0;			// MP 증가량
				m_iMPChangeValue					= 0;			// MP 자연 회복량
				m_fHPRelativeChangeRate				= 0.0f;			// 10초당 HP 회복
				m_fSkillDamageUpRate				= 0.0f;			// 스킬 공격 증가 데미지값
				m_fAllAttackEnchantRate				= 1.0f;			// 속성 발동 확률 % 증가
#endif BUFF_TEMPLET_SYSTEM

				m_iSummonNpcId						= 0;			// 소환할 npc id
				m_fSummonNpcRate					= 0.f;		// 타격시 소환확율
				m_fSummonNpcCoolTime				= 0.f;

#ifdef PVP_SEASON2_SOCKET
				m_bIsLevelLinkedStat	= false;
				m_iAddLevel				= 0;
#endif

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				m_kStatRelLVData.Init();	/// 스탯 별 추가 레벨
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
				m_iAllSkillLevelUpEx = 0;
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE

#ifdef HAMEL_SECRET_DUNGEON // 김태환
				m_vecInfoByUseHyperMode.clear();	/// 각성시 사용하는 효과 설정 저장 컨테이너
#endif // HAMEL_SECRET_DUNGEON
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				InitJuneSocketParity();
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			}
#endif REAL_TIME_ELSWORD
			//}} // kimhc // 실시간 엘소드 중 실시간 아바타 및 장비 교체 중 소켓 옵션

			void Add( const SocketData& rhs )
			{
				m_Stat.AddStat( rhs.m_Stat, true );			// socket option의 stat의 extra_stat 정보는 default로 포함해서 계산하도록 한다.


				m_fMoveSpeed += rhs.m_fMoveSpeed;
				m_fJumpSpeed += rhs.m_fJumpSpeed;

				m_fAnimationSpeedRate += rhs.m_fAnimationSpeedRate;
				m_fHyperModeChargeRate += rhs.m_fHyperModeChargeRate;
				m_fHyperModeTime += rhs.m_fHyperModeTime;
				m_fRepairPriceSale += rhs.m_fRepairPriceSale;
				
			
				m_fDamageReduce += rhs.m_fDamageReduce;
				m_fPercentDamageReduce += rhs.m_fPercentDamageReduce;

				m_fEvadePercent += rhs.m_fEvadePercent;

				m_fAntiEvadePercent += rhs.m_fAntiEvadePercent;


				m_fPercentCritical += rhs.m_fPercentCritical;
				m_fCritical += rhs.m_fCritical;

				//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
				m_fAdditionalDefenceValue = rhs.m_fAdditionalDefenceValue;	// 최종적인 데미지에 계산될 데미지 감소 수치 (이 수치를 가지고 % 연산을 함)
				m_fAdditionalDefenceValue = rhs.m_fAdditionalAttackValue;	// 최종적인 데미지에 계산될 추가 데미지
				// 최종적인 데미지에 계산될 데미지 감소 수치 (이 수치를 가지고 % 연산을 함)
				//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

				if ( false == rhs.m_vecHpUpByAttack.empty() )
					m_vecHpUpByAttack.insert( m_vecHpUpByAttack.end(), rhs.m_vecHpUpByAttack.begin(), rhs.m_vecHpUpByAttack.end() );

				if ( false == rhs.m_vecHpUpByAttacked.empty() )
					m_vecHpUpByAttacked.insert( m_vecHpUpByAttacked.end(), rhs.m_vecHpUpByAttacked.begin(), rhs.m_vecHpUpByAttacked.end() );

				//{{ kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정
				if ( false == rhs.m_vecMpUpByAttack.empty() )
					m_vecMpUpByAttack.insert( m_vecMpUpByAttack.end(), rhs.m_vecMpUpByAttack.begin(), rhs.m_vecMpUpByAttack.end() );

				if ( false == rhs.m_vecMpUpByAttacked.empty() )
					m_vecMpUpByAttacked.insert( m_vecMpUpByAttacked.end(), rhs.m_vecMpUpByAttacked.begin(), rhs.m_vecMpUpByAttacked.end() );
				//}} kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정

				//{{ 2011.09.05 / 이지헌 / 대만/홍콩 / 마법의 목걸이 개편 / 병합 : 강정훈
#ifdef MAGIC_NEC_FIX
				m_fMPMultiplyByAll += rhs.m_fMPMultiplyByAll;
#endif
				//}}

#ifdef BUFF_TEMPLET_SYSTEM
				m_iAddMPValue					+= rhs.m_iAddMPValue;					// MP 증가량
				m_iMPChangeValue				+= rhs.m_iMPChangeValue;				// MP 자연 회복량
				m_fHPRelativeChangeRate			+= rhs.m_fHPRelativeChangeRate;			// 10초당 HP 회복
				m_fSkillDamageUpRate			+= rhs.m_fSkillDamageUpRate;			// 스킬 공격 증가 데미지값
				m_fAllAttackEnchantRate			*= rhs.m_fAllAttackEnchantRate;			// 속성 발동 확률 % 증가
#endif BUFF_TEMPLET_SYSTEM

				m_fDamageUpByAMADS += rhs.m_fDamageUpByAMADS;

				m_fSpeedUpManaGather += rhs.m_fSpeedUpManaGather;
				m_fIncreaseEDPercent += rhs.m_fIncreaseEDPercent;
				m_fIncreaseDCBExpPercent += rhs.m_fIncreaseDCBExpPercent;

				m_fPercentSuperArmor += rhs.m_fPercentSuperArmor;
				m_fTimeSuperArmor += rhs.m_fTimeSuperArmor;
					
				m_AllSkillLevelUp += rhs.m_AllSkillLevelUp;

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				m_kStatRelLVData.AddStat( rhs.m_kStatRelLVData );	/// 스탯 별 추가 레벨
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
				// POWER_RATE_TYPE 상수 값을 곱해서 증가시켜주는 스킬 레벨업 소켓 옵션
				m_iAllSkillLevelUpEx += rhs.m_iAllSkillLevelUpEx;
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
			}

			//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05 // 오현빈
			wstring GetSocketDesc( const int iSocketLevel_, bool bCompact = false, CX2Unit::UNIT_CLASS eUnitClass_ = CX2Unit::UC_NONE ) const;
	#else
			wstring GetSocketDesc( const int iSocketLevel_, bool bCompact = false ) const;
	#endif // SERV_NEW_ITEM_SYSTEM_2013_05
			//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

//{{ kimhc // 2010.4.9 // 비밀던전 작업(셋트아이템효과)
#ifdef SERV_SECRET_HELL
			wstring GetSocketDescEx() const;			// SOCKET_ITEM_EFFECT_EX가 NONE이 아닌 타입의 디스크립션
#endif SERV_SECRET_HELL
//}} kimhc // 2010.4.9 // 비밀던전 작업(셋트아이템효과)

			// @bExcludeInternalLimit: UI에 표시되는 부분에는 제한이 없으나 내부적으로 제한이 있는 경우에, 제한수치 계산을 하지 않기 위해
			void LimitMaximum( bool bExcludeInternalLimit = false )
			{
				m_Stat.LimitMaximum();

//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
				//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

				// 다른 수치의 제한은 SocketOptionForm.lua에서 수행
				if( m_fHyperModeTime > CX2SocketItem::SocketData::MAX_HYPERMODETIME )
					m_fHyperModeTime = CX2SocketItem::SocketData::MAX_HYPERMODETIME;				

				if( m_fPercentSuperArmor > CX2SocketItem::SocketData::MAX_SUPER_ARMOR_RATE )
					m_fPercentSuperArmor = CX2SocketItem::SocketData::MAX_SUPER_ARMOR_RATE;
				
				if( m_fTimeSuperArmor > CX2SocketItem::SocketData::MAX_SUPER_ARMOR_TIME )
					m_fTimeSuperArmor = CX2SocketItem::SocketData::MAX_SUPER_ARMOR_TIME;

			}

			//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
			// 정수로 변환 시 반올림을 위해서 0.5f를 더해준다.
			int GetLinearCriticalValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fPercentCritical * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearCriticalValue", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearMoveSpeedValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fMoveSpeed * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearMoveSpeedValue", iSocketLevel_ ) + 0.5f );
			}
	
			int GetLinearJumpSpeedValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fJumpSpeed * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearJumpSpeedValue", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearAnimationSpeedValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fAnimationSpeedRate * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearAnimationSpeedValue", iSocketLevel_ ) + 0.5f );
			}
		
			int GetLinearAntiEvadeValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fAntiEvadePercent * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearAntiEvadeValue", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearEvadeValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fEvadePercent * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearEvadeValue", iSocketLevel_ ) + 0.5f );
			}
			
			int GetHpIncrementValue( const int iSocketLevel_ ) const
			{
				return lua_tinker::call<int>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateHpIncrementValue", iSocketLevel_ );
			}

			int GetLinearHyperGageChargeSpeedValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fHyperModeChargeRate * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearHyperGageChargeSpeedValue", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearAdditionalDefenceValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fAdditionalDefenceValue * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearAdditionalDefenceValue", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearAdditionalAttackValue( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fAdditionalAttackValue * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearAdditionalAttackValue", iSocketLevel_ ) + 0.5f );
			}
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
			/// June 3207cd70's three new linear terms (strings in June's SocketOptionForm.lua, lines 91-120)
			int GetLinearMpIncValueByAttack( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fMPIncByAttack * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearMpIncValueByAttack", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearMpIncValueByAttacked( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fMPIncByAttacked * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearMpIncValueByAttacked", iSocketLevel_ ) + 0.5f );
			}

			int GetLinearDamageMaximizeRate( const int iSocketLevel_ ) const
			{
				return static_cast<int>( m_fDamageMaximizeRate * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateLinearDamageMaximizeRate", iSocketLevel_ ) + 0.5f );
			}

			/// June 32073860 (A) / 320739e0 (B): the unit type picks CalculateLinear<Class>Specialization{A,B}Value.
			/// June's types 10-12 (Luciel, Rose, Ain) have no March UNIT_TYPE; any other type returns 0, as in June.
			static const char* GetJuneSpecializationCurveName( const CX2Unit::UNIT_TYPE eUnitType_, const bool bA_ )
			{
				switch ( eUnitType_ )
				{
				case CX2Unit::UT_ELSWORD:	return bA_ ? "CalculateLinearElswordSpecializationAValue" : "CalculateLinearElswordSpecializationBValue";
				case CX2Unit::UT_ARME:		return bA_ ? "CalculateLinearArmeSpecializationAValue" : "CalculateLinearArmeSpecializationBValue";
				case CX2Unit::UT_LIRE:		return bA_ ? "CalculateLinearLireSpecializationAValue" : "CalculateLinearLireSpecializationBValue";
				case CX2Unit::UT_RAVEN:		return bA_ ? "CalculateLinearRavenSpecializationAValue" : "CalculateLinearRavenSpecializationBValue";
				case CX2Unit::UT_EVE:		return bA_ ? "CalculateLinearEveSpecializationAValue" : "CalculateLinearEveSpecializationBValue";
				case CX2Unit::UT_CHUNG:		return bA_ ? "CalculateLinearChungSpecializationAValue" : "CalculateLinearChungSpecializationBValue";
				case CX2Unit::UT_ARA:		return bA_ ? "CalculateLinearAraSpecializationAValue" : "CalculateLinearAraSpecializationBValue";
				case CX2Unit::UT_ELESIS:	return bA_ ? "CalculateLinearElesisSpecializationAValue" : "CalculateLinearElesisSpecializationBValue";
				case CX2Unit::UT_ADD:		return bA_ ? "CalculateLinearAddSpecializationAValue" : "CalculateLinearAddSpecializationBValue";
				default:					return NULL;
				}
			}

			int GetLinearCharacterSpecializationAValue( const int iSocketLevel_, const CX2Unit::UNIT_TYPE eUnitType_ ) const
			{
				const char* pszCurve = GetJuneSpecializationCurveName( eUnitType_, true );
				if ( NULL == pszCurve )
					return 0;
				return static_cast<int>( m_fCharacterSpecializationA * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), pszCurve, iSocketLevel_ ) + 0.5f );
			}

			int GetLinearCharacterSpecializationBValue( const int iSocketLevel_, const CX2Unit::UNIT_TYPE eUnitType_ ) const
			{
				const char* pszCurve = GetJuneSpecializationCurveName( eUnitType_, false );
				if ( NULL == pszCurve )
					return 0;
				return static_cast<int>( m_fCharacterSpecializationB * lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), pszCurve, iSocketLevel_ ) + 0.5f );
			}
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY

	#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			void ConvertAndAdd( const SocketData& rhs_, const int iSocketLevel_, const CX2Unit::UNIT_TYPE eUnitType_ )
	#else // SERV_9TH_NEW_CHARACTER
			void ConvertAndAdd( const SocketData& rhs_, const int iSocketLevel_ )
	#endif // SERV_9TH_NEW_CHARACTER
			{

				// 2011-07-07 현재까지는 Stat중에서 m_fIncreaseHPRate 만 옵션 수치화에 영향을 받는다
				m_Stat.ConvertAndAddStat( rhs_.m_Stat, GetHpIncrementValue( iSocketLevel_ ) );			// socket option의 stat의 extra_stat 정보는 default로 포함해서 계산하도록 한다.


				m_fMoveSpeed += rhs_.GetLinearMoveSpeedValue( iSocketLevel_ );
				m_fJumpSpeed += rhs_.GetLinearJumpSpeedValue( iSocketLevel_ );

				m_fAnimationSpeedRate += rhs_.GetLinearAnimationSpeedValue( iSocketLevel_ );
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				/// June 3207cd70 has no 0.5 reform for Add on hyper charge / hyper time (checklist section 7)
				m_fHyperModeChargeRate += rhs_.GetLinearHyperGageChargeSpeedValue( iSocketLevel_ );
				m_fHyperModeTime += rhs_.m_fHyperModeTime;
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY


	#ifdef SERV_9TH_NEW_CHARACTER // 김태환
				/// 애드일 땐, 절반만 적용하자
				const float fReformValue = ( CX2Unit::UT_ADD == eUnitType_ ) ? 0.5f : 1.f;

				m_fHyperModeChargeRate	+= ( rhs_.GetLinearHyperGageChargeSpeedValue( iSocketLevel_ ) * fReformValue );
				m_fHyperModeTime		+= rhs_.m_fHyperModeTime * fReformValue;
	#else // SERV_9TH_NEW_CHARACTER
				m_fHyperModeChargeRate += rhs_.GetLinearHyperGageChargeSpeedValue( iSocketLevel_ );
				m_fHyperModeTime += rhs_.m_fHyperModeTime;
	#endif // SERV_9TH_NEW_CHARACTER
				
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
				m_fRepairPriceSale += rhs_.m_fRepairPriceSale;
				
			
				m_fDamageReduce += rhs_.m_fDamageReduce;
				m_fPercentDamageReduce += rhs_.m_fPercentDamageReduce;

#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				/// June 3207cd70 adds neither evade nor anti-evade (June's loader has neither key)
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
				m_fEvadePercent += rhs_.GetLinearEvadeValue( iSocketLevel_ );

				m_fAntiEvadePercent += rhs_.GetLinearAntiEvadeValue( iSocketLevel_ );
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY


				m_fPercentCritical += rhs_.GetLinearCriticalValue( iSocketLevel_ );
				m_fCritical += rhs_.m_fCritical;

				m_fAdditionalDefenceValue += rhs_.GetLinearAdditionalDefenceValue( iSocketLevel_ );
				m_fAdditionalAttackValue += rhs_.GetLinearAdditionalAttackValue( iSocketLevel_ );

				if ( false == rhs_.m_vecHpUpByAttack.empty() )
					m_vecHpUpByAttack.insert( m_vecHpUpByAttack.end(), rhs_.m_vecHpUpByAttack.begin(), rhs_.m_vecHpUpByAttack.end() );

				if ( false == rhs_.m_vecHpUpByAttacked.empty() )
					m_vecHpUpByAttacked.insert( m_vecHpUpByAttacked.end(), rhs_.m_vecHpUpByAttacked.begin(), rhs_.m_vecHpUpByAttacked.end() );

				//{{ kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정
				if ( false == rhs_.m_vecMpUpByAttack.empty() )
					m_vecMpUpByAttack.insert( m_vecMpUpByAttack.end(), rhs_.m_vecMpUpByAttack.begin(), rhs_.m_vecMpUpByAttack.end() );

				if ( false == rhs_.m_vecMpUpByAttacked.empty() )
					m_vecMpUpByAttacked.insert( m_vecMpUpByAttacked.end(), rhs_.m_vecMpUpByAttacked.begin(), rhs_.m_vecMpUpByAttacked.end() );
				//}} kimhc // 2011-05-25 // SetOption, TitleOption에만 적용 되는 option 모든 itemOption에 적용 되도록 수정

				m_fDamageUpByAMADS += rhs_.m_fDamageUpByAMADS;

				m_fSpeedUpManaGather += rhs_.m_fSpeedUpManaGather;
				m_fIncreaseEDPercent += rhs_.m_fIncreaseEDPercent;
				m_fIncreaseDCBExpPercent += rhs_.m_fIncreaseDCBExpPercent;

#ifdef SUPER_SOCKET_IN_TITLE
				if ( true == rhs_.m_bDungeonOnly )
				{
					m_fPercentSuperArmorDungeon = max( m_fPercentSuperArmorDungeon, rhs_.m_fPercentSuperArmor );
					m_fTimeSuperArmorDungeon = max( m_fTimeSuperArmorDungeon, rhs_.m_fTimeSuperArmor );
				}
				else if ( true == rhs_.m_bPVPOnly )
				{
					m_fPercentSuperArmorPVP = max( m_fPercentSuperArmorPVP, rhs_.m_fPercentSuperArmor );
					m_fTimeSuperArmorPVP = max( m_fTimeSuperArmorPVP, rhs_.m_fTimeSuperArmor );
				}
				else
				{
					m_fPercentSuperArmorDungeon = max( m_fPercentSuperArmorDungeon, rhs_.m_fPercentSuperArmor );
					m_fTimeSuperArmorDungeon = max( m_fTimeSuperArmorDungeon, rhs_.m_fTimeSuperArmor );

					m_fPercentSuperArmorPVP = max( m_fPercentSuperArmorPVP, rhs_.m_fPercentSuperArmor );
	m_fTimeSuperArmorPVP = max( m_fTimeSuperArmorPVP, rhs_.m_fTimeSuperArmor );
				}
				
#else
				m_fPercentSuperArmor += rhs_.m_fPercentSuperArmor;
				m_fTimeSuperArmor += rhs_.m_fTimeSuperArmor;
#endif
					
				m_AllSkillLevelUp += rhs_.m_AllSkillLevelUp;

				//{{ 2011.09.05 / 이지헌 / 대만/홍콩 / 마법의 목걸이 개편 / 병합 : 강정훈
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				/// June 3207cd70 has no plain add of m_fMPMultiplyByAll: the loader already summed it into the MP pair
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
#ifdef MAGIC_NEC_FIX
				m_fMPMultiplyByAll += rhs_.m_fMPMultiplyByAll;
#endif
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
				//}}
#ifdef BUFF_TEMPLET_SYSTEM
				m_iAddMPValue					+= rhs_.m_iAddMPValue;					// MP 증가량
				m_iMPChangeValue				+= rhs_.m_iMPChangeValue;				// MP 자연 회복량
				m_fHPRelativeChangeRate			+= rhs_.m_fHPRelativeChangeRate;		// 10초당 HP 회복
				m_fSkillDamageUpRate			+= rhs_.m_fSkillDamageUpRate;			// 스킬 공격 증가 데미지값
				m_fAllAttackEnchantRate			*= rhs_.m_fAllAttackEnchantRate;		// 속성 발동 확률 % 증가
#endif BUFF_TEMPLET_SYSTEM

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				m_kStatRelLVData.AddStat( rhs_.m_kStatRelLVData );	/// 스탯 별 추가 레벨
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
				// POWER_RATE_TYPE 상수 값을 곱해서 증가시켜주는 스킬 레벨업 소켓 옵션
				m_iAllSkillLevelUpEx += rhs_.m_iAllSkillLevelUpEx;
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
				/// June 3207cd70: the MP pair and 92's maximize are linear, the class specializations go through
				/// 32073860 / 320739e0, the rest are plain adds; the +0x29c list is appended
				if ( CX2SocketItem::IsJuneSocketOptionFormLoaded() )
				{
				m_fMPIncByAttack				+= rhs_.GetLinearMpIncValueByAttack( iSocketLevel_ );
				m_fMPIncByAttacked				+= rhs_.GetLinearMpIncValueByAttacked( iSocketLevel_ );
				m_fDamageMaximizeRate			+= rhs_.GetLinearDamageMaximizeRate( iSocketLevel_ );
				m_fCharacterSpecializationA		+= rhs_.GetLinearCharacterSpecializationAValue( iSocketLevel_, eUnitType_ );
				m_fCharacterSpecializationB		+= rhs_.GetLinearCharacterSpecializationBValue( iSocketLevel_, eUnitType_ );
				}

				m_fCriticalAfflicted				+= rhs_.m_fCriticalAfflicted;
				m_fAdditionalDamageAfflicted		+= rhs_.m_fAdditionalDamageAfflicted;
				m_fAdditionalDamage					+= rhs_.m_fAdditionalDamage;
				m_fAntiCriticalRate					+= rhs_.m_fAntiCriticalRate;
				m_fAntiAddDamageRate				+= rhs_.m_fAntiAddDamageRate;
				m_fAdditionalAttackedDamageValue	+= rhs_.m_fAdditionalAttackedDamageValue;

				m_fAtkPhysicPiercingRate			+= rhs_.m_fAtkPhysicPiercingRate;
				m_fAtkMagicPiercingRate				+= rhs_.m_fAtkMagicPiercingRate;
				m_fHPRecoveryRate					+= rhs_.m_fHPRecoveryRate;
				m_fIncreaseItemDropRate				+= rhs_.m_fIncreaseItemDropRate;
				m_fIncreaseDmgRateToBoss			+= rhs_.m_fIncreaseDmgRateToBoss;
				m_fIncreaseDefRateToBoss			+= rhs_.m_fIncreaseDefRateToBoss;
				m_fDecreaseActiveSkillCooltime		+= rhs_.m_fDecreaseActiveSkillCooltime;
				m_fDecrease1stSA_SkillCooltime		+= rhs_.m_fDecrease1stSA_SkillCooltime;
				m_fDecrease2ndSA_SkillCooltime		+= rhs_.m_fDecrease2ndSA_SkillCooltime;
				m_fDecrease3rdSA_SkillCooltime		+= rhs_.m_fDecrease3rdSA_SkillCooltime;
				m_fIncreaseDmgEnemyHPFew_DmgRate	+= rhs_.m_fIncreaseDmgEnemyHPFew_DmgRate;
				m_fIncreaseDefMyHPFew_DefRate		+= rhs_.m_fIncreaseDefMyHPFew_DefRate;
				m_fDecreaseDebuffTimeRate			+= rhs_.m_fDecreaseDebuffTimeRate;
				m_fDecrease_MPCostRate				+= rhs_.m_fDecrease_MPCostRate;
				m_fIncreaseDmgRate_MyHpFew			+= rhs_.m_fIncreaseDmgRate_MyHpFew;
				m_fAllSkillCooltimeDecreaseRate		+= rhs_.m_fAllSkillCooltimeDecreaseRate;
				m_fAllSkillDamageIncreaseRate		+= rhs_.m_fAllSkillDamageIncreaseRate;

				if ( false == rhs_.m_vecConditionalBuff.empty() )
					m_vecConditionalBuff.insert( m_vecConditionalBuff.end(), rhs_.m_vecConditionalBuff.begin(), rhs_.m_vecConditionalBuff.end() );
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
			}
			//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
#ifdef VIEW_SPECIAL_OPTIONDATA	
			/** @function : LevelLinkedStatAdd
				@brief : 기본스탯의 옵션수치화 적용
				@param : 적용시킬 스탯 객체(rhs_), 소켓레벨(iSocketLevel_)
			*/
			void LevelLinkedStatAdd( const SocketData& rhs_, const int iSocketLevel_ )
			{
				m_Stat.m_fBaseHP += rhs_.m_Stat.m_fBaseHP * iSocketLevel_;
				m_Stat.m_fAtkMagic += rhs_.m_Stat.m_fAtkMagic * iSocketLevel_;
				m_Stat.m_fAtkPhysic += rhs_.m_Stat.m_fAtkPhysic * iSocketLevel_;
				m_Stat.m_fDefMagic += rhs_.m_Stat.m_fDefMagic * iSocketLevel_;
				m_Stat.m_fDefPhysic += rhs_.m_Stat.m_fDefPhysic * iSocketLevel_;
			}
#endif //VIEW_SPECIAL_OPTIONDATA
		};
	public:
		CX2SocketItem(void);
		~CX2SocketItem(void);

		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		void			OpenScriptFile( const WCHAR* pFileName, const WCHAR* pFormulaFileName_ );

		static float GetFinalCriticalPercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalCriticalPercent", fSumValue_, iUserLevel_ );
		}

		static float GetFinalMoveSpeedPercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalMoveSpeedPercent", fSumValue_, iUserLevel_ );
		}

		static float GetFinalJumpSpeedPercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalJumpSpeedPercent", fSumValue_, iUserLevel_ );
		}

		static float GetFinalAnimationSpeedPercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalAnimationSpeedPercent", fSumValue_, iUserLevel_ );
		}

		static float GetFinalAnitEvadePercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalAnitEvadePercent", fSumValue_, iUserLevel_ );
		}

		static float GetFinalEvadePercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalEvadePercent", fSumValue_, iUserLevel_ );
		}
		
		static float GetFinalHyperGageChargeSpeedPercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalHyperGageChargeSpeedPercent", fSumValue_, iUserLevel_ );
		}
		
		static float GetFinalAdditionalDefencePercent( const float fSumValue_, const int iUserLevel_ )
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalAdditionalDefencePercent", fSumValue_, iUserLevel_ );
		}

		static float GetFinalAdditionalAttackValue( const float fSumValue_, const int iUserLevel_ )
		{
			// fPower_ 은 각 스테이트의 공격배율*파워레이트 이다.
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalAdditionalAttackValue", fSumValue_, iUserLevel_ );
		}

		// 1레벨 보정 던전 용
		static float GetFinalAdditionalAttackValueInDamageFreeGame( const float fSumValue_, const int iUserLevel_ )
		{
			// fPower_ 은 각 스테이트의 공격배율*파워레이트 이다.
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalAdditionalAttackValueInDamageFreeGame", fSumValue_, iUserLevel_ );
		}
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
		/// June buff port Phase 16: the option-data curves June's refresh (32290950) adds. Every curve is called
		/// ( value, level ); June also passes the unit type, which only the class specializations read.
		static float GetFinalMpIncPercentByAttack( const float fSumValue_, const int iUserLevel_ )		/// June 322a99e0 (members 24/25)
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalMpIncPercentByAttack", fSumValue_, iUserLevel_ );
		}

		static float GetFinalMpIncPercentByAttacked( const float fSumValue_, const int iUserLevel_ )		/// June 32073ba0 (member 26)
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalMpIncPercentByAttacked", fSumValue_, iUserLevel_ );
		}

		static float GetFinalDamageMaximizeRate( const float fSumValue_, const int iUserLevel_ )		/// June 322a9e30 -> 320723b0 (BBT 92)
		{
			return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalculateFinalDamageMaximizeRate", fSumValue_, iUserLevel_ );
		}

		/// True when the loaded SocketOptionForm.lua defines this global function (March's file lacks June's curves)
		static bool IsLuaCurveLoaded( const char* pszName_ )
		{
			lua_State* L = g_pKTDXApp->GetLuaBinder()->GetLuaState();
			lua_getfield( L, LUA_GLOBALSINDEX, pszName_ );
			const bool bFunction = ( 0 != lua_isfunction( L, -1 ) );
			lua_pop( L, 1 );
			return bFunction;
		}

		/// True when June's SocketOptionForm.lua is loaded (CalculateFinalMpIncPercentByAttack stands for June's curve set).
		/// Checked once; when false, one [JUNE_BUFF] line names the file, and the June-only curve calls are skipped (the
		/// socket MP gain, class specializations and 92's maximize then stay 0) rather than failing one by one in Lua.
		static bool IsJuneSocketOptionFormLoaded();

		/// June 322a9a10 (A) / 322a9c20 (B): the unit type (types 1..12) picks CalculateFinal<Class>Specialization{A,B}Percent;
		/// any other type (an NPC passes 0) returns 0 (@322a9be0). Luciel, Rose and Ain have no March UNIT_TYPE.
		struct JuneSpecializationFinal
		{
			CX2Unit::UNIT_TYPE	m_eUnitType;
			bool				m_bA;

			JuneSpecializationFinal( const CX2Unit::UNIT_TYPE eUnitType_, const bool bA_ ) : m_eUnitType( eUnitType_ ), m_bA( bA_ ) {}

			float operator()( const float fSumValue_, const int iUserLevel_ ) const
			{
				const char* pszCurve = NULL;
				switch ( m_eUnitType )
				{
				case CX2Unit::UT_ELSWORD:	pszCurve = m_bA ? "CalculateFinalElswordSpecializationAPercent" : "CalculateFinalElswordSpecializationBPercent";	break;
				case CX2Unit::UT_ARME:		pszCurve = m_bA ? "CalculateFinalArmeSpecializationAPercent" : "CalculateFinalArmeSpecializationBPercent";			break;
				case CX2Unit::UT_LIRE:		pszCurve = m_bA ? "CalculateFinalLireSpecializationAPercent" : "CalculateFinalLireSpecializationBPercent";			break;
				case CX2Unit::UT_RAVEN:		pszCurve = m_bA ? "CalculateFinalRavenSpecializationAPercent" : "CalculateFinalRavenSpecializationBPercent";		break;
				case CX2Unit::UT_EVE:		pszCurve = m_bA ? "CalculateFinalEveSpecializationAPercent" : "CalculateFinalEveSpecializationBPercent";			break;
				case CX2Unit::UT_CHUNG:		pszCurve = m_bA ? "CalculateFinalChungSpecializationAPercent" : "CalculateFinalChungSpecializationBPercent";		break;
				case CX2Unit::UT_ARA:		pszCurve = m_bA ? "CalculateFinalAraSpecializationAPercent" : "CalculateFinalAraSpecializationBPercent";			break;
				case CX2Unit::UT_ELESIS:	pszCurve = m_bA ? "CalculateFinalElesisSpecializationAPercent" : "CalculateFinalElesisSpecializationBPercent";		break;
				case CX2Unit::UT_ADD:		pszCurve = m_bA ? "CalculateFinalAddSpecializationAPercent" : "CalculateFinalAddSpecializationBPercent";			break;
				default:					return 0.f;
				}
				return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), pszCurve, fSumValue_, iUserLevel_ );
			}
		};
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
#ifdef ELSWORD_WAY_OF_SWORD 
		// 옵션 점감선
		static float GetOptionRateCorrection( const float fOptionRate_ );
#endif ELSWORD_WAY_OF_SWORD
		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

		const SocketData*   GetSocketData( int key ) const;
		
		bool			AddSocketData_LUA();
#ifdef LUA_TRANS_DEVIDE
		bool			AddSocketDataTrans_LUA();
#endif LUA_TRANS_DEVIDE

#ifdef SERV_CASH_ITEM_SOCKET_OPTION
		void			AddSocketGroupDataForCashAvatar_LUA();		/// 캐시아바타에서 선택할 수 있는 옵션 그룹 파싱
		int				GetSocketIdListForCashAvatar( const int iGroupID_, OUT vector<int>& vecSocketGroupData_, const int iItemType_ );
#endif // SERV_CASH_ITEM_SOCKET_OPTION

		bool			GetIsPossibleSocketItemByOnlyItemType( int itemID );

		//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
#ifdef PRINT_INGAMEINFO_TO_EXCEL
		void PrintOptionInfo_ToExcel(KProtectedType<int> iLevel);
#endif PRINT_INGAMEINFO_TO_EXCEL
		//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		int	 GetSocketPrefixStrID( int iSocketID_ ) const;
		void GetSocketPrefix( IN int iSocketID_, OUT wstring& wstrItemDesc_ ) const;
#ifdef EU_NEW_ITEM_SYSTEM_PREFIX_TO_POSTFIX
		void GetSocketPostfix( IN int iSocketID_, OUT wstring& wstrItemDesc_ ) const;
#endif // EU_NEW_ITEM_SYSTEM_PREFIX_TO_POSTFIX
		void AddSkillLevelUpSocketGroup_LUA( int iGroupID_, CX2Unit::UNIT_CLASS eUnicClass_, int iSkillID_
			#ifdef UPGRADE_SKILL_SYSTEM_2013// 오현빈
			, int iSkillLvUP_ 
			#endif // UPGRADE_SKILL_SYSTEM_2013
			);

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈 
		bool GetSkillIDByUnicClassAndGropID( OUT vector<SkillLevelUpIDAndNum>& vecSkillLevelUPIDAndNumList_, IN CX2Unit::UNIT_CLASS eUnitClass_, IN int iGroupID_ ) const;

		void GetSkillLevelIncreaseValue( IN const SocketData* pSocketData, IN const CX2SkillTree::SKILL_ID eSkillID, OUT int& iIncreaseSkillLevelBySocket, IN CX2Unit::UNIT_CLASS eUnitClass_) const;
#else
		CX2SkillTree::SKILL_ID GetSkillIDByUnicClassAndGropID( CX2Unit::UNIT_CLASS eUnitClass_, int iGroupID_ ) const;
#endif //UPGRADE_SKILL_SYSTEM_2013
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
		/// June 32715680 (buff port 16-step3b5): every member's skill ID of group iGroupID_, any unit class. Clears the vector first.
		bool GetJuneGroupSkillIDs( IN const int iGroupID_, OUT vector<int>& vecSkillID_ ) const;
		void JuneLogSkillGroupPowerCensus() const;	/// one load-time line after OptionItemData.lua (16-step3b5)
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
		const CX2SocketItem::KItemStatRelLVData& GetStatRelLVDataBySocktID( int iSocketID_ ) const;

		static KItemStatRelLVData	ms_KDummyItemStatRelLVData;
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SERV_GROW_UP_SOCKET
		bool			SetGrowUpSocketData( IN const wstring& wstrGrowUpSocketID_, OUT map<int, vector<int>>& mapGrowUpSocketID );
		// 성장 소켓 정보 가져오기
		bool			GetGrowUpSocketData(IN const vector<int>& vecOldSocketOption_, OUT vector<int>& vecNewSocketOption_ );
#endif //SERV_GROW_UP_SOCKET

	private:
#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        typedef std::map<int,SocketData>    SocketDataMap;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        typedef std::map<int,SocketData*>   SocketDataMap;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
		SocketDataMap                       m_mapSocketOptionPool;

#ifdef SERV_CASH_ITEM_SOCKET_OPTION
		typedef std::map<int, vector<int>> MapSocketGroupDataForCashAvartar;
		MapSocketGroupDataForCashAvartar m_mapSocketGroupDataForCashAvartar;		/// 캐시아바타에서 선택할 수 있는 소켓옵션 그룹
#endif // SERV_CASH_ITEM_SOCKET_OPTION

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		// 소켓 ID 별 접두어 String ID // map<SocketID , StringID >
		std::map< int, int >			m_mapSocketDescStrID;

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈
		// 그룹ID, < 유닛클래스, 스킬ID, 스킬레벨업 수치>
		typedef std::map< int, std::vector< SkillLvUpSocketOption >> mapUnitClassSkillGroup;
		mapUnitClassSkillGroup m_mapSkillUpSocketGroup;
#else
		// <유닛클래스, vcetor< 그룹ID, 스킬ID > //  
		typedef	std::pair<int, CX2SkillTree::SKILL_ID> pairSkillGroup;
		typedef std::map< CX2Unit::UNIT_CLASS, std::vector< pairSkillGroup >> mapUnitClassSkillGroup;
		mapUnitClassSkillGroup m_mapSkillUpSocketGroup;
#endif //UPGRADE_SKILL_SYSTEM_2013 

#endif //SERV_NEW_ITEM_SYSTEM_2013_05
};

