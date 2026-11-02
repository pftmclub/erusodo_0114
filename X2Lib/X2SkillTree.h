#pragma once


/**
	@class CX2SkillTree
	@brief	
		- 캐릭터 Skill에 대한 정보를 처리
		- enum : SKILL_TYPE, ACTIVE_SKILL_USE_CONDITION, SKILL_ID, SKILL_ABILITY_TYPE, SKILL_MEMO_ID
		- struct : SkillTemplet, SkillTemplet, SealSkillItemInfo
	@date 2010/11/05
	@section MODIFYINFO 수정 정보
		- 2010/10/31: JHKang
			- Raven(Blade Master SKILL_ID 2519) : 긴급 탈출(Emergency Escape)
			- Elsword(Rune Slayer SKILL_ID 1019) : 루나 블레이드(Luna Blade)
		- 2010/11/
			- Eve(Code Empress SKILL_ID 3019) : 어설트 스피어(Assualt Spear)
			- Eve(Code Nemesis SKILL_ID 3020) : 아토믹 쉴드(Atomic Shield)
		- 2013/05/30: JHKang
			- 스킬 개편 관련, Aisha 스킬 아이디 추가
*/

class CX2SkillTree
{
public:

	enum SKILL_TYPE
	{
		ST_NONE = 0,
		ST_PASSIVE_PHYSIC_ATTACK,		// STAT 수치를 변경시켜주는 passive
		ST_PASSIVE_MAGIC_ATTACK,
		ST_PASSIVE_MAGIC_DEFENCE,
		ST_PASSIVE_PHYSIC_DEFENCE,
		ST_BUFF,						
		ST_ACTIVE,						// timestop이 없는 필살기
		ST_SPECIAL_ACTIVE,				// timestop이 있는 필살기
		ST_PASSIVE,						// STAT외에 기타 기능이 있는 passive 

//#ifdef GUILD_SKILL // oasis907 : 김상윤 [2009.11.25] // 본섭 위해 주석 해제
		// 길드 스킬
		ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE, 
		ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE,
		ST_GUILD_PASSIVE,
//#endif GUILD_SKILL

//#ifdef ADDED_RELATIONSHIP_SYSTEM
		ST_RELATIONSHIP_SKILL,
//#endif // ADDED_RELATIONSHIP_SYSTEM

#ifdef FINALITY_SKILL_SYSTEM //JHKang
		ST_HYPER_ACTIVE_SKILL,
#endif //FINALITY_SKILL_SYSTEM
#ifdef SERV_IRUHADEV_JUNE_SKILL_TYPE
		/// June Enum.lua SKILL_TYPE: the special active grades and the core skills ( explicit, as June numbers them )
		ST_SPECIAL_ACTIVE_GRADE_1 = 14,
		ST_SPECIAL_ACTIVE_GRADE_2 = 15,
		ST_SPECIAL_ACTIVE_GRADE_3 = 16,
		ST_CORE = 17,
		ST_CORE_PASSIVE = 18,
#endif SERV_IRUHADEV_JUNE_SKILL_TYPE
	};



	enum ACTIVE_SKILL_USE_CONDITION
	{
		ASUT_NONE,
		ASUT_GROUND,		// 땅위에서만 쓸 수 있는 필살기, skip_point에서만
		ASUT_AIR,			// 공중에서도 쓸 수 있는 필살기, skip_point에서만
		ASUT_ANYTIME,		// 아무때나 쓸 수 있는 필살기
		ASUT_AIR_ONLY,		// 공중에서만 쓸 수 있는 필살기, skip_point에서만
		ASUT_ANYTIME_EXCEPT_AIR, // 공중을 제외한 어디서든(피격 시 포함) 사용 할 수 있는 스킬
#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_CORE
		ASUT_DAMAGE,				/// June Enum.lua 6
		ASUT_DAMAGE_EXCEPT_AIR,		/// June Enum.lua 7: damage states, on the ground only ( 31fca9f0 )
#endif SERV_IRUHADEV_JUNE_CHAR_ARME_CORE
	};




	enum SKILL_SLOT_ENCHANT
	{
		SSE_NONE,
		SSE_OVERFLOWING_MANA,
		SSE_STRONG_POWER,

		SSE_END,
	};


	// note: 수정되면 xslskilltree.h에도 update
	enum SKILL_ID
	{
		SI_INVALID = -1,
		SI_NONE,

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		#pragma region 캐릭터 공통
		SI_P_COMMON_PHYSIC_ATTACK_BEGINNER = 100,		/// 기초 근력단련
		SI_P_COMMON_MAGIC_ATTACK_BEGINNER,				/// 기초 마법수련
		SI_P_COMMON_PHYSIC_ATTACK_INTERMEDIATE,			/// 중급 근력단련
		SI_P_COMMON_MAGIC_ATTACK_INTERMEDIATE,			/// 중급 마법수련
		SI_P_COMMON_GET_CHANCE,							/// 기회의 포착
		SI_P_COMMON_SUPPORT_AURA = 105,					/// 격려의 기운
		#pragma endregion 패시프
		
		#pragma region 캐릭터 공통
		SI_SA_COMMON_AURA_POWER_ACCEL = 200,			/// 힘의 문장
		SI_SA_COMMON_AURA_MAGIC_ACCEL,					/// 지혜의 문장
		SI_SA_COMMON_AURA_SHIELD_ACCEL,					/// 인내의 문장
		SI_SA_COMMON_AURA_SPEED_ACCEL,					/// 가속의 문장
		SI_SA_COMMON_AURA_EL_DEFENCE_ACCEL,				///	가호의 문장
		SI_SA_COMMON_AURA_CRITICAL_ACCEL = 205,			/// 승리의 문장
		SI_SA_COMMON_AURA_ADDATK_ACCEL,					/// 섬멸의 문장
		#pragma endregion 버프

		#pragma region 길드, 결혼
		SI_GP_COMMON_BALANCED_BODY					= 500,		/// 균형잡힌 육체
		SI_GP_COMMON_HARMONIC_SPIRIT				= 501,		/// 조화로운 영혼
		SI_GP_COMMON_CHEERING_OF_TEAM				= 502,		/// 동료의 응원
		SI_GP_COMMON_MASTER_OF_ALCHEMIST			= 503,		/// 연금술의 달인
		SI_GP_COMMON_CONCENTRATION_OF_SPIRIT		= 504,		/// 정신 집중
		SI_GP_COMMON_CONCENTRATION_MAGICAL_POWER	= 505,		/// 집중된 마력
		SI_GP_COMMON_VIOLENT						= 506,		/// 격노
		SI_GP_COMMON_LIBERATION_OF_ANGER			= 507,		/// 분노 해방
		// 여기까지 2트리
		SI_GP_COMMON_INFINITE_STRENGTH				= 508,		/// 끝없는 체력
		SI_GP_COMMON_CHANCE_TO_REVERSE				= 509,		/// 역전의 기회
		SI_GP_COMMON_SACRIFICE_OF_HERO				= 510,		/// 영웅의 희생
		SI_GP_COMMON_MIDAS							= 511,		/// 마이더스의 손
		
		SI_ETC_WS_COMMON_LOVE						= 900,		/// 커플 스킬 천상지애
		#pragma endregion 특수
		
		#pragma region Elsword
		SI_SA_ES_FATAL_FURY = 1000,						/// 페이탈 퓨리
		SI_SA_ES_FLAME_GEYSER,							/// 플레임 게이져
		SI_SA_ES_UNLIMITED_BLADE,						/// 언리미티드 블레이드
		SI_SA_ES_TRIPLE_GEYSER,							/// 트리플 게이져
		SI_SA_ESK_SPIRAL_BLAST,							/// 스파이럴 블래스트
		SI_SA_ESK_ARMAGEDON_BLADE = 1005,				/// 아마겟돈 블레이드
		SI_SA_ESK_DOUBLE_SLASH,							/// 더블 슬래시
		SI_SA_ELK_WINDMILL,								/// 윈드밀
		SI_SA_ELK_SAND_STORM,							/// 샌드 스톰
		SI_SA_EMK_RISING_SLASH,							/// 라이징 슬래시
		SI_SA_EMK_RISING_WAVE = 1010,					/// 라이징 웨이브
		SI_SA_EMK_SWORD_FIRE,							/// 소드 파이어
		SI_SA_EMK_PHOENIX_TALON,						/// 피닉스 탤런
		SI_SA_ERS_STORM_BLADE,							/// 스톰 블레이드
		SI_SA_ERS_LUNA_BLADE,							/// 루나 블레이드
		SI_SA_ETK_FINAL_STRIKE = 1015,					/// 파이널 스트라이크
		SI_SA_ETK_PHANTOM_SWORD,						/// 팬텀 소드
		SI_SA_EIS_MAELSTORM_RAGE,						/// 메일스톰 레이지
		SI_SA_EIS_SWORD_FALL,							/// 소드폴
		SI_SA_EIS_RAGE_CUTTER,							/// 레이지 커터
		#pragma endregion Special Active

		#pragma region Elsword
		SI_A_ES_MEGASLASH = 1200,						/// 메가 슬래시
		SI_A_ES_ASSAULT_SLASH,							/// 어설트 슬래시
		SI_A_ES_COUNTER,								/// 반격
		SI_A_ES_ENDURANCE,								/// 극기
		SI_A_ES_KICK,									/// 발차기
		SI_A_ES_ROLLING = 1205,							/// 구르기
		SI_A_ES_SWORD_WAVE,								/// 소드 웨이브
		SI_A_ES_DEFENCE,								/// 막기
		SI_A_ESK_AIR_SLASH,								/// 에어 슬래시
		SI_A_ESK_ARMOR_BREAK,							/// 아머 브레이크
		SI_A_ESK_WEAPON_BREAK = 1210,					/// 웨폰 브레이크
		SI_A_ELK_SONIC_BLADE,							/// 소닉 블레이드
		SI_A_ELK_ROLLING_SMASH,							/// 롤링 스매쉬
		SI_A_ELK_IMPACT_SMASH,							/// 임팩트 스매시
		SI_A_EMK_BIG_BURST,								/// 대폭살
		SI_A_EMK_WIND_BLADE = 1215,						/// 윈드 블레이드
		SI_A_ERS_SPLASH_EXPLOSION,						/// 스플래쉬 익스플로젼
		SI_A_ERS_SWORD_ENCHANT,							/// 소드 인챈트
		SI_A_ERS_CRITICAL_SWORD,						///	크리티컬 소드
		SI_A_ERS_RUNE_OF_FIRE,							/// 초열의 룬
		SI_A_ERS_RUNE_OF_ICE = 1220,					/// 한빙의 룬
		SI_A_ETK_CRESCENT_CUT,							/// 크레센트 컷
		SI_A_ETK_SWORD_SHIELD,							/// 소드 실드
		SI_A_ETK_HARSH_CHASER,							/// 하쉬 체이서
		SI_A_ETK_PIERCING_SWORD,						/// 피어싱 소드
		SI_A_EIS_SWORD_BLASTING = 1225,					/// 소드 블래스팅
		SI_A_EIS_MIRAGE_STING,							/// 미라지 스팅
		SI_A_EIS_FATAL_SLAP,							/// 페이탈 슬랩
		#pragma endregion Active

		#pragma region Elsword
		SI_P_ES_POWERFUL_VITAL = 1400,					/// 강인한 신체
		SI_P_ESK_GET_MANA_UP,							/// 활력
		SI_P_ESK_AUTO_GUARD,							/// 오토 가드
		SI_P_ELK_STRONG_BONE,							/// 근골 체질
		SI_P_ELK_REDUCE_PAIN,							/// 고통억제
		SI_P_ELK_INTENSIVE_ATTACK = 1405,				/// 강렬한 일격
		SI_P_ELK_INDURANCE_OF_REVENGE,					/// 역전의 인내
		SI_P_ES_POWERFUL_FIREBALL,						/// 파이어볼 강화
		SI_P_ERS_SPELL_CHAIN,							/// 마력의 사슬
		SI_P_ERS_MAGIC_RESISTANCE,						/// 마법저항 수련
		SI_P_ERS_RUNE_MASTERY = 1410,					/// 룬 마스터리
		SI_P_ETK_BRUTAL_SLAYER,							/// 잔혹한 학살자
		SI_P_ETK_HARSH_SLAYER,							/// 냉혹한 학살자
		SI_P_EIS_COMPACT_COUNTER,						/// 빈틈 없는 반격
		SI_P_EIS_MIND_OF_FIGHTER,						/// 승부사의 기질
		SI_P_EIS_LIGHTNING_STEP = 1415,					/// 전광 석화
		SI_P_EIS_CUTTING_SWORD,							/// 날카로운 검
		#pragma endregion Passive

		#pragma region Elsword
		SI_HA_ELK_GIGANTIC_SLASH = 1600,				/// 기간틱 슬래시
		SI_HA_ERS_SHINING_RUNE_BUSTER,					/// 샤이닝 룬버스터
		SI_HA_EIS_BLADE_RAIN,							/// 블레이드 레인
		#pragma endregion Finality Skill

		#pragma region Aisha
		SI_SA_AV_GUST_SCREW = 2000,						/// 거스트 스크류
		SI_SA_AV_LIGHTNING_BOLT,						/// 라이트닝 볼트
		SI_SA_AV_GUST_STORM,							/// 거스트 스톰
		SI_SA_AV_ICE_STORM,								/// 아이스 스톰
		SI_SA_AHM_MAGIC_MISSILE,						/// 매직 미사일
		SI_SA_AHM_BLIZZARDSHOWER = 2005,				/// 블리자드 샤워
		SI_SA_AHM_CHAIN_LIGHTNING,						/// 체인 라이트닝
		SI_SA_AHM_BLAZE_STEP,							/// 블레이즈 스탭
		SI_SA_AEM_METEOR_SHOWER,						/// 메테오 샤워
		SI_SA_AEM_LIGHTNING_SHOWER,						/// 라이트닝 샤워
		SI_SA_ADM_DARK_CLOUD = 2010,					/// 다크 클라우드
		SI_SA_ADM_PLASMA_CUTTER,						/// 플라즈마 커터
		SI_SA_ADM_DEATHFIELD,							/// 데스필드
		SI_SA_ADM_AGING,								/// 에이징
		SI_SA_AVP_PHANTOM_BREATHING_DARK_FALL,			/// 팬텀 브리딩 - 다크폴
		SI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE = 2015,	/// 팬텀 브리딩 - 다크 홀
		SI_SA_ABM_ENERGY_SPURT,							/// 에너지 스퍼트
		SI_SA_ABM_SUPER_NOVA,							/// 슈퍼 노바
		SI_SA_ABM_GUILLOTINE_PRESS,						/// 길로틴 프레스
		SI_SA_ABM_MAGICAL_MAKEUP,						/// 메지컬 메이크업
		SI_SA_ADW_SCREWDRIVER_TORNADO = 2020,			/// 스크류 드라이버 - 토네이도
		SI_SA_ADW_SCREWDRIVER_DRILLER,					/// 스크류 드라이버 - 드릴러
		#pragma endregion Special Active
		
		#pragma region Aisha
		SI_A_AV_CHAIN_FIRE_BALL = 2200,					/// 체인 파이어볼
		SI_A_AV_BINDING_CIRCLE,							/// 바인딩 서클
		SI_A_AV_TELEPORT,								/// 텔레포트
		SI_A_AV_MANA_SHIELD,							/// 마나 실드
		SI_A_AV_CIRCLE_FLAME,							/// 서클 플래임
		SI_A_AV_FIRE_ROAD = 2205,						/// 파이어 로드
		SI_A_AV_ICICLE_WAVE,							/// 아이시클 웨이브
		SI_A_AHM_CYCLONE,								/// 싸이클론
		SI_A_AHM_MEDITATION,							/// 명상
		SI_A_AEM_CHAIN_BURST,							/// 체인 버스트
		SI_A_AEM_STONE_WALL = 2210,						/// 돌벽
		SI_A_AEM_AQUA_SHOWER,							/// 아쿠아 샤워
		SI_A_AEM_SHINING_BODY,							/// 샤이닝 바디
		SI_A_ADM_HELL_STONE,							/// 헬스톤
		SI_A_ADM_HELL_DROP,								/// 헬 드롭
		SI_A_ADM_MANA_INTAKE = 2215,					/// 마나 흡수
		SI_A_AVP_SUMMON_BAT_TIME_BOMB,					/// 박쥐 소환 - 타임 밤
		SI_A_AVP_SUMMON_BAT_HEAVY_DOLL,					/// 쁘띠 앙고르 - 헤비 돌
		SI_A_AVP_SHADOW_BODY,							/// 쉐도우 바디
		SI_A_ABM_VITAL_DRAIN,							/// 바이탈 드레인
		SI_A_ABM_ENERGY_DRAIN = 2220,					/// 에너지 드레인
		SI_A_ADW_DISTORTION,							/// 공간 왜곡
		SI_A_ADW_ENERGETIC_BODY,						/// 에너제틱 바디
		SI_A_ADW_HEAVY_PRESS,							/// 헤비 프레스
		SI_A_ADW_IMPACT_HAMMER,							/// 임팩트 해머
		SI_A_ADW_WORM_HOLE = 2225,						/// 웜홀
		#pragma endregion Active

		#pragma region Aisha
		SI_P_AV_FIREBALL_UP = 2400,						/// 파이어볼 연마
		SI_P_AHM_FREEZING_POINT_RESEARCH,				/// 빙점 연구
		SI_P_AHM_UNLIMIT_MANA,							/// 끝없는 마력
		SI_P_AEM_ELEMENTAL_FRIENDSHIP,					/// 원소와의 친화
		SI_P_AEM_ENERGY_OF_THE_PLENTY,					/// 약화의 기운
		SI_P_AEM_ELEMENTAL_RESEARCH = 2405,				/// 원소학 연구
		SI_P_ADM_SHADOW_DATH,							/// 죽음의 그림자
		SI_P_ADM_SHADOW_OF_DISASTER,					/// 재앙의 그림자
		SI_P_AVP_LIMITED_MANA_MANAGEMENT,				/// 극한의 마나 운용
		SI_P_AVP_STRONG_MIND,							/// 어둠의 계약
		SI_P_AVP_SPRINTER = 2410,						/// 스프린터
		SI_P_AVP_MAGIC_AMPLIFICATION,					/// 마력 증폭
		SI_P_AVP_GRAVITY_BALL_RESEARCH,					/// 그라비티 볼 연구
		SI_P_ABM_FITNESS,								/// 신체 단련
		SI_P_ABM_MANAFLOW_OVERDRIVE,					/// 기혈 폭주
		SI_P_ADW_SPIRIT_ACCELERATION  = 2415,			/// 기력 가속
		SI_P_ADW_ADVANCED_TELEPORTATION,				/// 공간 도약
		SI_P_ADW_MAGICAL_STAFF,							/// 마력의 지팡이
		#pragma endregion Passive

		#pragma region Aisha
		SI_HA_AEM_ELEMENTAL_STORM = 2600,				/// 엘리멘탈 스톰
		SI_HA_AVP_ABYSS_ANGOR,							/// 어비스 앙고르
		SI_HA_ADW_FATE_SPACE,							/// 페이트 스페이스
		#pragma endregion Finality Skill

		#pragma region Lena
		SI_SA_LE_PERFECT_STORM = 3000,					/// 퍼팩트 스톰
		SI_SA_LE_MULTIPLE_STINGER,						/// 멀티플 스팅거
		SI_SA_LE_AERO_TORNADO,							/// 에어로 토네이도
		SI_SA_LE_PHOENIX_STRIKE,						/// 피닉스 스트라이크
		SI_SA_LCR_CRESCENT_KICK,						/// 크레센트 킥
		SI_SA_LCR_DIVE_KICK_BOMBING = 3005,				/// 다이브 킥 봄잉
		SI_SA_LCR_VIOLENT_ATTACK,						/// 바이올런트 어택
		SI_SA_LWS_SHARPFALL,							/// 샤프 폴
		SI_SA_LWS_AIRELINNA_NYMPH,						/// 아이레린나- 님프
		SI_SA_LWS_AIRELINNA_SYLPH,						/// 아이레린나 - 실프
		SI_SA_LSR_GUIDED_ARROW = 3010,					/// 가이드 애로우
		SI_SA_LSR_CRAZY_SHOT,							/// 크레이지 샷
		SI_SA_LSR_WINDWARD,								/// 윈드와드
		SI_SA_LSR_GUNGNIR,								/// 궁그닐
		SI_SA_LGA_AERO_STRAFE,							/// 에어로 스트레이프
		SI_SA_LTR_FATALITY = 3015,						///	페이탈리티
		SI_SA_LTR_TRAPPING_ARROW_BURST,					///	트래핑 애로우 - 버스트
		SI_SA_LTR_TRAPPING_ARROW_FUNGUS,				/// 트래핑 애로우 - 펑거스
		SI_SA_LTR_CALL_OF_RUIN,							///	콜 오브 루인
		SI_SA_LTR_KARMA,								///	카르마
		SI_SA_LNW_GLIDING_STRIKE = 3020,				///	글라이딩 스트라이크
		#pragma endregion Special Active

		#pragma region Lena
		SI_A_LE_RAIL_STINGER = 3200,					/// 레일 스팅거
		SI_A_LE_ASSAULT_KICK,							/// 어설트 킥
		SI_A_LE_BACKJUMP_SHOT,							/// 백덤블링 샷
		SI_A_LE_REFLEX_MAGIC,							/// 마법반사
		SI_A_LE_RISING_FALCON,							///	라이징 팔콘
		SI_A_LE_SIEGEMODE = 3205,						/// 시즈 모드
		SI_A_LCR_SPINNING_KICK,							/// 스피닝 킥
		SI_A_LCR_LOW_KICK,								/// 로우킥
		SI_A_LCR_MIDDLE_KICK,							/// 미들킥
		SI_A_LWS_NATURE_FORCE,							/// 자연의 힘
		SI_A_LWS_SLIDE_KICK = 3210,						/// 슬라이드 더블 킥
		SI_A_LWS_ASSAULT_IMPACT,						/// 어설트 임팩트
		SI_A_LWS_BACK_KICK,								/// 백킥
		SI_A_LWS_HIGH_KICK,								/// 하이킥
		SI_A_LSR_ENTANGLE,								/// 인탱글 - 스타이핑 레인저
		SI_A_LSR_SHOOTING_MAGNUM = 3215,				/// 슈팅 매그넘
		SI_A_LSR_HUMMING_WIND,							/// 허밍 윈드
		SI_A_LGA_FREEZING_ARROW,						/// 프리징 애로우
		SI_A_LGA_FREEZING_BOLT,							/// 프리징 볼트
		SI_A_LGA_ARC_SHOT,								/// 아크 샷
		SI_A_LGA_RAPID_SHOT = 3220,						/// 래피드 샷
		SI_A_LGA_WIND_BLAST,							/// 윈드 블래스트
		SI_A_LGA_STIGMA_ARROW,							/// 낙인의 사격
		SI_A_LTR_EVOKE,									///	이보크
		SI_A_LTR_ENTANGLE,								/// 구속의 씨앗
		SI_A_LTR_EXPLOSION_TRAP = 3225,					///	폭발의 덫
		SI_A_LNW_FURIOUS_ENGAGE,						///	퓨리어스 인게이지
		SI_A_LNW_DELAYED_FIRING,						///	지연의 신호탄
		SI_A_RNW_THORNS_TRAP,							/// 가시덩굴 덫
		SI_A_RNW_ROSEBUSH_TRAP,							/// 찔레 덫
		SI_A_RNW_SEED_OF_RUIN = 3230,					/// 진노의 씨앗
		SI_A_RNW_ANGER_OF_ELF,							/// 엘드랏실의 분노
		#pragma endregion Active

		#pragma region Lena
		SI_P_LCR_SHARP_KICK = 3400,						/// 날렵한 발차기
		SI_P_LCR_AGILE_MOVEMENT,						/// 재빠른 몸놀림
		SI_P_LCR_SOFTBODY,								/// 유연한몸놀림
		SI_P_LWS_POWERFUL_SHOT,							/// 힘 있는 사격
		SI_P_LWS_FRIENDSHIP_OF_NATURE,					/// 자연과의 친화
		SI_P_LSR_POWERFUL_BOWSTRING = 3405,				/// 강화된 보우스트링
		SI_P_LSR_HAWK_EYE,								/// 호크아이
		SI_P_LSR_SHARP_ARROW,							/// 다듬어진 화살
		SI_P_LGA_COMMUNE_OF_NATURE,						/// 자연의 감응
		SI_P_LGA_VIBRATION_SHOOTING,					/// 진동 사격
		SI_P_LTR_HUNTERS_ABILITY = 3410,				///	사냥꾼의 재능
		SI_P_LTR_SHARPEN_ARROW,							///	날카로운 화살
		SI_P_LNW_VITALPOINT_PIERCING,					///	급소 찌르기
		SI_P_LNW_PRIOR_PLANNED_BLOW,					///	계산된 일격
		#pragma endregion Passive

		#pragma region Lena
		SI_HA_LWS_SPIRAL_STRIKE = 3600,					/// 스파이럴 스트라이크
		SI_HA_LGA_CRYOTRON_BOLT,						/// 크리아오트론 볼트
		SI_HA_LNW_INNOCENT,								/// 이노센트
		#pragma endregion Finality Skill

		#pragma region Raven
		SI_SA_RF_POWER_ASSAULT = 4000,					/// 파워 어설트
		SI_SA_RF_CANNON_BLADE,							/// 캐논 블레이드
		SI_SA_RF_SEVEN_BURST,							/// 세븐 버스트
		SI_SA_RST_HYPER_SONIC_STAB,						/// 하이퍼 소닉스탭
		SI_SA_RST_SHOCKWAVE,							/// 쇼크 웨이브 - 강
		SI_SA_RST_SHOCKWAVE_FRONT = 4005,				/// 쇼크 웨이브 - 속
		SI_SA_RST_BLOODY_ACCEL,							/// 블러디 액셀
		SI_SA_RBM_FLYING_IMPACT,						/// 플라잉 임팩트
		SI_SA_RBM_GIGA_DRIVE_LIMITER,					/// 기가 드라이브- 리미터
		SI_SA_RBM_GIGA_DRIVE_SEISMIC_TREMOR,			/// 기가 드라이브 - 사이즈믹 트레몰
		SI_SA_ROT_ARC_ENEMY = 4010,						/// 아크 애너미
		SI_SA_ROT_GUARDIAN_STRIKE,						/// 가디언 스트라이크
		SI_SA_ROT_VALKYRIESJAVELIN,						/// 발키리스 자벨린
		SI_SA_ROT_NUCLEAR,								/// 뉴클리어
		SI_SA_RRF_WILD_CHARGE,							/// 와일드 차지
		SI_SA_RWT_BURSTING_BLADE = 4015,				/// 버스팅 블레이드
		SI_SA_RWT_REVOLVER_CANNON_ORDNANCE_PENETRATOR,	/// 리볼버 캐논 - OP탄
		SI_SA_RWT_REVOLVER_CANNON_HIGH_EXPLOSIVE_SHELL,	/// 리볼버 캐논 - HE탄
		SI_SA_RWT_HARPOON_SPEAR,						/// 하푼 스피어
		SI_SA_RWT_HELLFIRE_GATLING,						/// 헬파이어 개틀링
		SI_SA_RWT_GIGA_PROMINENCE = 4020,				/// 기가 프로미넌스
		SI_SA_RVC_DEADLY_RAID,							/// 데들리 레이드
		SI_SA_RVC_IGNITION_CROW_NAPALM,					/// 이그니션 크로우 - 네이팜
		SI_SA_RVC_IGNITION_CROW_INCINERATION,			/// 이그니션 크로우 - 인시너레이션
		#pragma endregion Special Active

		#pragma region Raven
		SI_A_RF_MAXIMUM_CANNON = 4200,					/// 맥시멈 캐논
		SI_A_RF_EARTH_BREAKER,							/// 어스 브레이크
		SI_A_RF_MEGADRILL_BREAK,						/// 메가 드릴 브레이크
		SI_A_RF_BURNING_RUSH,							/// 버닝러시
		SI_A_RF_GROUND_IMPACT,							/// 그라운드 임팩트
		SI_A_RF_SHADOW_STEP = 4205,						/// 섀도우스탭
		SI_A_RF_BREAKING_FIST,							/// 브레이킹 피스트
		SI_A_RST_BERSERKER_BLADE,						/// 버서커 블레이드
		SI_A_RST_CUT_TENDON,							/// 컷텐던
		SI_A_RBM_ONE_FLASH,								/// 일섬
		SI_A_RBM_SONIC_SLASH = 4210,					/// 소닉 슬래시
		SI_A_RBM_WOFL_FANG,								/// 울프팽
		SI_A_ROT_CHARGED_BOLT_HEDGHOG,					/// 차지드 볼트 - 헤지호그
		SI_A_ROT_CHARGED_BOLT_BLOOD,					/// 차지드 볼트 - 쏜즈 블러드
		SI_A_RRF_HELL_DIVE,								/// 헬 다이브
		SI_A_ROT_WEAPON_BREAK = 4215,					/// 무기 깨기
		SI_A_ROT_ARMOR_BREAK,							/// 방어구 깨기
		SI_A_RRF_X_CRASH,								/// 엑스 크래시
		SI_A_RRF_LIMIT_CRUSHER,							/// 리미트 크러셔
		SI_A_RWT_BARRAGE_ATTACK,						/// 연무 폭파
		SI_A_RWT_FLAME_SWORD = 4220,					/// 화염인
		SI_A_RVC_BLEEDING_SLICER,						/// 치명상
		SI_A_RVC_NAPALM_GRENADE,						/// 네이팜 그레네이드		
		#pragma endregion Active

		#pragma region Raven
		SI_P_RF_SHADOW_PIERCING = 4400,					/// 섀도우피어싱
		SI_P_RF_HARDEN_BODY,							/// 단련된신체
		SI_P_RF_INDUCE_DESIRE_WINNING,					/// 호승심유발
		SI_P_RST_BLADE_MASTERY,							/// 숙련된 검술
		SI_P_RST_EXQUISITE_SWORDMANSHIP,				/// 섬세한 검술
		SI_P_RBM_REVERSAL_SOLDIER = 4405,				/// 역전의용사
		SI_P_RBM_SHADOW_THRUST,							/// 쉐도우 쓰러스트
		SI_P_RBM_EMERGENCY_ESCAPE,						/// 긴급 탈출
		SI_P_RRF_BURNING_NASOD_HAND,					/// 타오르는 나소드 핸드
		SI_P_RRF_SPIRITUALIZED_FURY,					/// 승화된 분노
		SI_P_RRF_EXPLOSIVE_NASOD_HAND = 4410,			/// 폭발하는 나소드 핸드
		SI_P_RRF_SHADOW_PUNISHER,						/// 쉐도우 퍼니셔
		SI_P_RRF_CONFIDENCE_VICTORY,					/// 승리의자신감		
		SI_P_RWT_BEHAVIOR_OF_MERCENARY,					/// 용병의 처신술
		SI_P_RVC_SHADOW_BACK_SLIDE,						/// 섀도우 백 슬라이드
		SI_P_RVC_SURVIVAL_TECHNIQUE_OF_MERCENARY = 4415,/// 용병의 생존 전략
		SI_P_RVC_WINNABLE_TECHNIQUE_OF_MERCENARY,		/// 용병의 승리전략
		SI_P_RVC_AIR_SLICER,							/// 에어 슬라이서
		SI_P_RVC_OVERHEAT_MODE,							/// 과열모드 작동!
		#pragma endregion Passive

		#pragma region Raven
		SI_HA_RBM_EXTREM_BLADE = 4600,					/// 익스트림 블레이드
		SI_HA_RRF_INFERNAL_ARM,							/// 인페르날 암즈
		SI_HA_RVC_BURNING_BUSTER,						/// 버닝 버스터
		#pragma endregion Finality Skill

		#pragma region Eve
		SI_SA_EN_ILLUSION_STINGER = 5000,				/// 일루전 스팅어
		SI_SA_EN_DIMENSION_LINK_BLADER,					/// 디멘션 링크 - 블레이더
		SI_SA_EN_GENERATE_BLACKHOLE,					/// 제네레이터 블랙홀
		SI_SA_EN_DIMENSION_LINK_GUARDIAN,				/// 디멘션 링크 - 가디언
		SI_SA_EEG_EXPLOSION_IMPACT,						/// 익스플로전 임팩트
		SI_SA_EEG_QUEENS_THRONE = 5005,					/// 퀸스 쓰론
		SI_SA_EEG_JUNK_BREAK,							/// 정크 브레이크
		SI_SA_ENS_IRONSCRAPS,							/// 아이언 스크랩스
		SI_SA_ENS_ATOMIC_SHIELD,						/// 아토믹 쉴드
		SI_SA_ENS_ASSUALT_SPEAR_ANNIHILATOR,			/// 어설트 스피어 - 어나이얼레이터
		SI_SA_EAT_SONIC_WAVE = 5010,					/// 소닉 웨이브
		SI_SA_EAT_GENOCIDE_RIPPER,						/// 제노사이드 리퍼
		SI_SA_EAT_HEAVENS_FIST_PRESSURE,				/// 해븐즈 피스트 - 프레셔
		SI_SA_EAT_HEAVENS_FIST_SWEEPER,					/// 헤븐즈 피스트 - 스위퍼
		SI_SA_EEP_SPACE_WRENCH,							/// 스페이 스랜처
		SI_SA_EEP_ASSUALT_SPEAR_BURST = 5015,			/// 어설트 스피어 - 버스터
		SI_SA_EEL_PARTICLE_RAY,							/// 파티클 레이
		SI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE,			/// 스위프 롤링 - 오버차지
		SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN,				/// 스위프 롤링 - 트라이 발칸
		SI_SA_EEL_PHOTON_FLARE,							/// 포톤 플레어
		SI_SA_EEL_GIGA_STREAM = 5020,					/// 기가 스트림
		SI_SA_EEL_THOUSANDS_OF_STARS,					/// 사우전드 스타
		SI_SA_EBS_ENERGY_NEEDLES,						/// 에너지 니들스
		SI_SA_EBS_ENERGETIC_HEART,						/// 에너제틱 하트
		SI_SA_EEL_LINEAR_DIVIDER,						/// 리니어 디바이더
		#pragma endregion Special Active

		#pragma region Eve
		SI_A_EN_ILLUSION_STRIKE = 5200,					/// 일루전 스트라이크
		SI_A_EN_DIMENSION_LINK,							/// 디멘션 링크 - 센트리
		SI_A_EN_CLOAKING,								/// 클로킹
		SI_A_EN_CONVERSION,								/// 마력 전환
		SI_A_EN_MEGA_ELECTRONBALL,						/// 메가 일렉트론볼
		SI_A_EN_ILLUSION_RAY = 5205,					/// 일루전 레이
		SI_A_EN_TESLA_SHOCK,							/// 테슬라 쇼크
		SI_A_EN_PHOTON_BLINK,							/// 광자 점멸
		SI_A_EEG_HORNET_STING_EXPLOSION,				/// 호넷 스팅 - 익스프로전
		SI_A_EEG_HORNET_STING_SHAPED_CHARGE,			/// 호넷 스팅 - 쉐이프트 차지
		SI_A_EEG_METAL_DUST_AURA = 5210,				/// 메탈 더스트 아우라
		SI_A_ENS_ATOMIC_BLASTER_PLASMAGUN,				/// 아토믹 블래스터 - 플라즈마 건
		SI_A_ENS_ATOMIC_BLASTER_SONICGUN,				/// 아토믹 블래스터 - 소닉 건
		SI_A_ENS_SPEAR_BURST,							/// 스피어 버스트
		SI_A_EAT_SURFACE_CUTING,						/// 서피스 커팅
		SI_A_EAT_OBERON_GUARD = 5215,					/// 오베론 가드
		SI_A_EEP_SPIT_FIRE_FLUSH,						/// 스핏 파이어 - 플러시
		SI_A_EEP_SPIT_FIRE_GRENADE,						/// 스핏 파이어 - 그리네이드
		SI_A_EEP_ELECTRONIC_FIELD,						/// 일렉트로닉 필드
		SI_A_EEL_SPECTRO_EL_CRYSTAL,					/// 엘 분광 결정
		SI_A_EBS_TASER_PILUM = 5220,					/// 테이저 필라 (복수형 발음)
		SI_A_EBS_KUGELBLITZ,							/// 쿠글블릿츠
		#pragma endregion Active

		#pragma region Eve
		SI_P_EN_ELECTRONBALL_UP = 5400,					/// 일렉트론볼 연마
		SI_P_EN_QUEENS_CONTROL,							/// 여왕의 지배력
		SI_P_EN_QUEENS_POTENTIAL,						/// 여왕의 잠재력
		SI_P_EN_REFINED_STEP,							/// 우아한발걸음
		SI_P_EN_PHOTON_BOOSTER,							/// 광자 추진기
		SI_P_ENS_NASOD_WEAPON = 5405,					/// 나소드 무기 강화
		SI_P_ENS_ENERGY_OF_CONCENTRATION,				/// 집중의 기운
		SI_P_EEP_ELECTRONBALL_MASTER,					/// 일렉트론 볼마스터
		SI_P_EEP_CHARGING_BOOSTER,						/// 충전추진기
		SI_P_EEL_SPECTRUM_DYNAMICS,						/// 분광 역학
		SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR = 5410,		/// 강화 엘 에너지 반응로
		SI_P_EBS_HYPER_OPTICAL_RESEARCH,				/// 초광학 연구
		SI_P_EBS_HIGHPOWERED_ELECTRICS,					/// 고출력 전자회로
		SI_P_EBS_AMPLIFIED_EL_ENERGY_REACTOR,			/// 증폭 엘 에너지 반응로
		#pragma endregion Passive

		#pragma region Eve
		SI_HA_ENS_LUNATIC_SCUD = 5600,					/// 루나틱 스커드
		SI_HA_EEP_LINK_OVERCHARGE_ILLUSION,				/// 링크 오버차지 일루전
		SI_HA_EBS_PSYCHIC_ARTILLERRY,					/// 사이킥 아틸러리( 이레이저 캐논 )
		#pragma endregion Finality Skill

		#pragma region Chung
		SI_SA_CC_LUNATIC_BLOW = 6000,					/// 루나틱 블로우
		SI_SA_CC_SCARE_CHASE,							/// 스케어 체이스
		SI_SA_CC_GIGANTIC_IMPACT,						/// 기간틱 임팩트
		SI_SA_CC_ACELDAMA,								/// 아겔다마
		SI_SA_CFG_LUNATIC_FURY,							/// 루나틱 퓨리
		SI_SA_CFG_PANDEMONIUM_CHAOS = 6005,				/// 판데모니움 - 카오스
		SI_SA_CFG_PANDEMONIUM_FEAR,						/// 판데모니움 - 피어
		SI_SA_CFG_PAIN_OF_CALADBOLG,					/// 칼라볼그 페인
		SI_SA_CIP_IRON_HOWLING,							/// 아이언 하울링
		SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS,			/// 랜드 디몰리셔 - 헤비 암즈
		SI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE = 6010,	/// 랜드 디몰리셔 - 어스 퀘이크
		SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA,	/// 아틸러리 스트라이크 - 퀀텀 발리스타
		SI_SA_CSG_HEAVY_RAILGUN,						/// 헤비레일건
		SI_SA_CSG_SHOOTING_STAR,						/// 슈팅 스타
		SI_SA_CDC_COMET_CRASHER,						/// 카밋 크래셔
		SI_SA_CDC_SHARPSHOOTER_SYNDROME = 6015,			/// 샤프슈터 신트롬
		SI_SA_CHG_BIGBANG_STREAM,						/// 빅뱅 스트림
		SI_SA_CHG_CHAOS_CANNON,							/// 카오스 캐논
		SI_SA_CHG_CARPET_BOMBING,						/// 카펫 바밍
		SI_SA_CTT_ARTILLERY_STRIKE,						/// 아틸러리 스트라이크 - 미슬 샤워
		SI_SA_CTT_DREAD_CHASE = 6020,					/// 드레드 체이스
		SI_SA_CTT_TACTICAL_FIELD_INDURANCE,				/// 택티컬 필드 - 인듀런스
		SI_SA_CTT_TACTICAL_FIELD_RAID,					/// 택티컬 필드 - 레이드
		#pragma endregion Special Active

		#pragma region Chung
		SI_A_CC_BRUTAL_SWING = 6200,					/// 브루탈 스윙
		SI_A_CC_DETONATION,								/// 디토네이션
		SI_A_CC_IMPACT_DETONATION,						/// 임팩트 디토네이션
		SI_A_CC_RELOAD_CANNON,							/// 탄전
		SI_A_CC_AIMMING_SHOT,							/// 에이밍 샷
		SI_A_CC_SIEGE_SHELLING = 6205,					/// 고정 포격
		SI_A_CFG_STEEL_EDGE,							/// 스틸 엣지
		SI_A_CFG_GUARD,									/// 가드
		SI_A_CFG_LEAP_ATTACK,							/// 리프어택
		SI_A_CIP_SUDDEN_BURSTER,						/// 서든 버스터
		SI_A_CIP_ARTILLERY_NOVA = 6210,					/// 아틸러리 노바
		SI_A_CIP_BURST_WOLF,							/// 버스트 울프
		SI_A_CSG_DOUBLE_FIRE,							/// 더블 파이어
		SI_A_CSG_DUAL_BUSTER,							/// 듀얼 버스터
		SI_A_CSG_HEAD_SHOT,								/// 헤드샷
		SI_A_CSG_MAGNUM_SHOT = 6215,					/// 매그넘 샷
		SI_A_CDC_RUMBLE_SHOT,							/// 럼블샷
		SI_A_CDC_BULLET_BLITZ,							/// 불렛 블릿츠
		SI_A_CDC_GATLING_SHOT,							/// 개틀링 샷 ( 래피드 캐논 )
		SI_A_CDC_LEG_SHOT,								/// 레그샷
		SI_A_CHG_CANNON_STRIKE = 6220,					/// 캐논 스트라이크
		SI_A_CHG_ELASTIC_BOMB_GRENADE,					/// 일레스틱 밤
		SI_A_CHG_ELASTIC_BOMB_LAUNCHER,					/// 일레스틱 밤 - 그리네이드 런처
		SI_A_CHG_MARK_OF_COMMANDER,						/// 지휘관의 표식
		SI_A_CTT_WONDER_WALL,							/// 원더 월
		SI_A_CTT_AUTOMATIC_MORTAR = 6225,				/// 박격포
		SI_A_CTT_BOMBARD_SERVICE,						/// 지원 포격
		#pragma endregion Active

		#pragma region Chung
		SI_P_CFG_METABOLISM_BOOST = 6400,				/// 신진대사 촉진
		SI_P_CFG_GUARD_MASTERY,							/// 방어숙련
		SI_P_CFG_ELEMENTAL_DEFENCE_A,					/// 불과 물과 자연
		SI_P_CFG_ELEMENTAL_DEFENCE_B,					/// 바람과 해와 달
		SI_P_CIP_ADVANCED_METABOLISM_BOOST,				/// 신진대사 촉진 강화
		SI_P_CIP_IRON_WILL = 6405,						/// 굳건한 의지
		SI_P_CSG_REMODEL_MAGAZINE,						/// 탄창/약실 개조
		SI_P_CSG_REMODEL_RIFLE,							/// 강선 개조
		SI_P_CDC_TUSSLE_TECHNIQUE,						/// 난전의 기술
		SI_P_CDC_ACCURATE_MARKMANSHIP,					/// 정밀한 사격술
		SI_P_CDC_INTENSE_SHOWTIME = 6410,				/// 강렬한 쇼타임
		SI_P_CDC_GAS_PISTON_SYSTEM,						/// 가스 피스톤 시스템
		SI_P_CHG_RELOAD_MASTERY,						/// 리로드 마스터리
		SI_P_CHG_CONVERTED_WARHEAD,						/// 탄두 개조
		SI_P_CHG_CANNONEER_WITHSTANDING,				/// 포병의 버티기
		SI_P_CTT_MOBILE_ARMORED_MODE = 6415,			/// 이동 포격
		SI_P_CTT_REACTIVE_ARMOR,						/// 반응 장갑
		#pragma endregion Passive

		#pragma region Chung
		SI_HA_CIP_DOOM_STRIKER = 6600,					/// 둠 스트라이크
		SI_HA_CDC_OUTRAGE_STRIKE,						/// 아웃레이지 스트라이크
		SI_HA_CTT_SATELITE_RAIN,						/// 세틀라이트 레인
		#pragma endregion Finality Skill

		#pragma region Ara
		SI_SA_AM_PULLING_PIERCE = 7000,					/// 천의_빨래 널기
		SI_SA_AM_TEMPEST_DANCE,							/// 선풍_휘모리
		SI_SA_AM_FALLING_DRAGON,						/// 강룡추_구름 떨구기
		SI_SA_AM_SHADOW_KNOT,							/// 결영_그림자 매듭_귀살 3식
		SI_SA_ALH_MOONLIGHT_SLASH,						/// 용아 4식 - 달빛 베기
		SI_SA_ALH_PEERLESSNESS_SLASH = 7005,			/// 무쌍참
		SI_SA_ALH_WHITE_TIGER,							/// 맹호격
		SI_SA_ALH_SWALLOW_ASSAULT,						/// 비연
		SI_SA_ASD_SUPPRESSION,							/// 제압
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		SI_SA_ALD_FINGER_BULLET,						/// 나찰 1식 흡혼
		SI_SA_ALD_WOLF_SPEAR = 7010,					/// 낭아 4식 늑대 이빨
		SI_SA_ALD_ENERGY_WAVE,							/// 기공파
		SI_SA_AYR_SUPPRESSION_ENERGY,					/// 제압 : 기
		SI_SA_ALD_ENERGY_BEAD,							/// 나찰 2식 연환장
		SI_SA_ALD_HELL_OF_TORNADO,						/// 연환지옥
		SI_SA_ALD_PRISON_SPEAR = 7015,					/// 나찰 3식 창살감옥
		SI_SA_AYR_ENERGY_VOID,							/// 초진공장
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		#pragma endregion Special Active

		#pragma region Ara
		SI_A_AM_WIND_WEDGE = 7200,						/// 연풍설_바람 쐐기_낭아 2식
		SI_A_AM_SHADOW_RISING,							/// 벽력뢰_천둥 꽃망울
		SI_A_AM_TIGER_CLAW,								/// 호랑이_발톱_맹호 2식
		SI_A_AM_FORCE_SPEAR,							/// 기공창
		SI_A_AM_GAIN_FORCE,								/// 흡공
		SI_A_AM_QUICK_PIERCING = 7205,					/// 신속
		SI_A_AM_SHOULDER_BLOW,							/// 철산고
		SI_A_AM_QUICK_STAB,								/// 천격
		SI_A_ALH_ROCK_CRASH,							/// 맹호 1식 - 바위깨기
		SI_A_ALH_BREAKING,								/// 이화접목
		SI_A_ALH_DOUBLE_COLLISION = 7210,				/// 용아 3식 - 팔괘장
		SI_A_ASD_DRAGON_FALLING,						/// 낙화
		SI_A_ASD_TURBULENT_WAVE,						/// 맹호 4식 - 노도
		SI_A_ASD_LOW_BRANDISH,							/// 맹호 3식 - 지축
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		SI_A_ALD_WOLF_CLAW,								/// 낭아 3식 늑대 발톱
		SI_A_ALD_CONVERSION_HEALTH = 7215,				/// 이혈공
		SI_A_AYR_HIGH_SPEED,							/// 쾌속
		SI_A_ALD_CONVERSION_MANA,						/// 이기공
		SI_A_AYR_STEAL_SOUL,							/// 나찰 4식 탈혼
		SI_A_ALD_REFLECTION,							/// 반탄공
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		#pragma endregion Active

		#pragma region Ara
		SI_P_ALH_QUICK_STAND = 7400,					/// 재빠른 기상
		SI_P_ALH_VIGOR_ENERGIZE,						/// 기력 활성화
		SI_P_ALH_FILLED_POWER,							/// 충만한 힘
		SI_P_ASD_QUICKEN_GUARD,							/// 번득이는 기질
		SI_P_ASD_BRUTAL_PIERCING,						/// 창술의 극의
		SI_P_ASD_FULLMOON_SLASH = 7405,					/// 만월 베기
		SI_P_ASD_POWER_OF_TIGER,						/// 맹호의 힘
		SI_P_ASD_SELF_PROTECTION_FORTITUDE,				/// 호신강기
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		SI_P_AYR_ENERGY_BULLET_REINFORCE,				/// 기탄 강화
		SI_P_AYR_HOWLING,								/// 늑대의 울부짖음
		SI_P_AYR_RESURRECTION = 7410,					/// 회광반조
		SI_P_ALD_ENERGY_REINFORCE,						/// 기력 강화
		SI_P_AYR_HELL_GATE_OPEN,						/// 지옥 초래
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		#pragma endregion Passive

		#pragma region Ara
		SI_HA_ASD_THOUSANDS_BLOSSOMS = 7600,			/// 분기등천 - 천수화
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		SI_HA_AYR_ENERGY_CANNON,						/// 대규환지옥
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		#pragma endregion Finality Skill

#else //UPGRADE_SKILL_SYSTEM_2013
		// 패시브 필살기, 캐릭터 공통	1~299
		SI_P_COMMON_PHYSIC_ATTACK_BEGINNER = 1,			/// 기초 근력단련
		SI_P_COMMON_MAGIC_ATTACK_BEGINNER,				/// 기초 마법수련
		SI_P_COMMON_PHYSIC_DEFENCE_BEGINNER,			/// 기초 물리방어술
		SI_P_COMMON_MAGIC_DEFENCE_BEGINNER,				/// 기초 마법방어술
		SI_P_COMMON_PHYSIC_ATTACK_INTERMEDIATE,			/// 중급 근력단련
		SI_P_COMMON_MAGIC_ATTACK_INTERMEDIATE,			/// 중급 마법수련
		SI_P_COMMON_PHYSIC_DEFENCE_INTERMEDIATE,		/// 중급 물리방어술
		SI_P_COMMON_MAGIC_DEFENCE_INTERMEDIATE,			/// 중급 마법방어술

		SI_P_COMMON_GET_CHANCE,							/// 기회의 포착				// 구현완료.
		SI_P_COMMON_GET_MANA_UP,						/// 활력					// 구현완료.
		SI_P_COMMON_POWERFUL_VITAL,						/// 강인한 신체				// 서버쪽에서 구현완료.
		SI_P_COMMON_POWERFUL_SOUL,						/// 강인한 정신력			// 서버쪽에서 구현완료.
		SI_P_COMMON_SUPPORT_AURA,						/// 격려의 기운				// 구현완료.
		SI_P_COMMON_UNLIMIT_MANA,						/// 끝없는 마력				// 구현완료.
		SI_P_COMMON_SHADOW_DATH,						/// 죽음의 그림자			// 구현완료.
		SI_P_COMMON_PHYSIC_HARMONY,						/// 육체와 정신의 조화		// 서버쪽에서 구현완료.
		SI_P_COMMON_SPRINTER,							/// 스프린터				// 구현완료.
		SI_P_COMMON_SOFTBODY,							/// 유연한몸놀림			// 구현완료.
		SI_P_COMMON_FIGHTER_SOUL,						/// 격투가의혼				// 서버쪽에서 구현완료.
		SI_P_COMMON_CONFIDENCE_VICTORY,					/// 승리의자신감			// 구현완료.
		SI_P_COMMON_HARDEN_BODY,						/// 단련된신체				// 구현완료.
		SI_P_COMMON_REVERSAL_SOLDIER,					/// 역전의용사				// 구현완료.
		SI_P_COMMON_INDUCE_DESIRE_WINNING,				/// 호승심유발				// 구현완료.
		SI_P_COMMON_REFINED_STEP,						/// 우아한발걸음			// 구현완료.


		// 버프 필살기, 캐릭터 공통		300~599
		SI_SA_COMMON_POWER_ACCEL = 300,					/// 파워 엑셀레이터
		SI_SA_COMMON_MAGIC_ACCEL,						/// 매직 엑셀레이터
		SI_SA_COMMON_SHIELD_ACCEL,						/// 실드 엑셀레이터
		SI_SA_COMMON_AURA_POWER_ACCEL,					/// 오러 파워 엑셀레이터, 2013/05/30, 이름 변경 : 힘의 문장
		SI_SA_COMMON_AURA_MAGIC_ACCEL,					/// 오러 매직 엑셀레이터, 2013/05/30, 이름 변경 : 지혜의 문장
		SI_SA_COMMON_AURA_SHIELD_ACCEL,					/// 오러 실드 엑셀레이터, 2013/05/30, 이름 변경 : 인내의 문장
		SI_SA_COMMON_POWER_ADRENALIN,					/// 파워 아드레날린
		SI_SA_COMMON_MAGIC_ADRENALIN,					/// 매직 아드레날린
		SI_SA_COMMON_SHIELD_ADRENALIN,					/// 실드 아드레날린
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
		SI_SA_COMMON_AURA_SPEED_ACCEL			= 309,	// 오러 스피드 액셀러레이터, , 2013/05/30, 이름 변경 : 가속의 문장
#endif
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 공통 스킬 개편, 김종훈
		SI_SA_COMMON_AURA_EL_DEFENCE_ACCEL,				///	가호의 문장
		SI_SA_COMMON_AURA_CRITICAL_ACCEL,				/// 승리의 문장
		SI_SA_COMMON_AURA_ADDATK_ACCEL,					/// 섬멸의 문장
#endif // UPGRADE_SKILL_SYSTEM_2013 // 공통 스킬 개편, 김종훈


		// 액티브 필살기, 캐릭터 순서데로, 캐릭터 하나당 skill id의 범위는 500
		// elsword
		SI_SA_ES_MEGASLASH = 1000,						/// 메가 슬래시
		SI_SA_ES_ASSAULT_SLASH,							/// 어설트 슬래시
		SI_SA_ES_FATAL_FURY,							/// 페이탈 퓨리
		SI_SA_ES_FLAME_GEYSER,							/// 프레임 게이져
		SI_SA_ES_UNLIMITED_BLADE,						/// 언리미티드 블레이드
		SI_SA_ES_TRIPLE_GEYSER,							/// 트리플 게이져
		SI_A_ESK_AIR_SLASH,							/// 에어 슬래시
		SI_SA_ESK_SPIRAL_BLAST,							/// 스파이럴 블래스트
		SI_SA_ESK_ARMAGEDON_BLADE,						/// 아마겟돈 블레이드
		SI_A_EMK_WIND_BLADE,							/// 윈드 블레이드
		SI_SA_EMK_RISING_SLASH,							/// 라이징 슬래시
		SI_SA_ERS_STORM_BLADE,							/// 스톰 블레이드
		SI_SA_ELK_WINDMILL,								/// 윈드밀					// dmlee 작업중
		SI_SA_EMK_SWORD_FIRE,							/// 소드 파이어				// wonpok 구현완료. 
		SI_SA_ESK_DOUBLE_SLASH,							/// 더블 슬래시				// 상형. 구현완료
		SI_SA_EMK_PHOENIX_TALON,						/// 피닉스 탤런				// wonpok 구현완료
		SI_A_ELK_SONIC_BLADE,							/// 소닉 블레이드
		SI_A_ERS_SPLASH_EXPLOSION,						/// 스플래쉬 익스플로젼
		SI_SA_ELK_SAND_STORM,							/// 샌드 스톰				// oasis907
		SI_SA_ERS_LUNA_BLADE,							/// 루나 블레이드			// JHKang
#ifdef ELSWORD_SHEATH_KNIGHT
		SI_A_ETK_CRESCENT_CUT,							/// 크레센트 컷
		SI_SA_EIS_SWORD_FALL,							/// 소드폴	
		SI_SA_ETK_FINAL_STRIKE,							/// 파이널 스트라이크
		SI_SA_EIS_MAELSTORM_RAGE,						/// 메일스톰 레이지
		SI_SA_ETK_PHANTOM_SWORD,						/// 팬텀 소드
#endif ELSWORD_SHEATH_KNIGHT
#ifdef SERV_ELSWORD_INFINITY_SWORD
		SI_A_EIS_SWORD_BLASTING	= 1025,				//  소드 블래스팅
		SI_SA_EIS_RAGE_CUTTER,							//  레이지 커터
#endif //SERV_ELSWORD_INFINITY_SWORD
#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
		SI_SA_EMK_RISING_WAVE,					//  라이징 웨이브	
#endif //UPGRADE_SKILL_SYSTEM_2013

		// 액티브
		SI_A_ES_COUNTER = 1200,							/// 반격					// 엘소드 구현완료.
		SI_A_ES_ENDURANCE,								/// 극기					// 엘소드 구현완료.
		SI_A_ES_KICK,									/// 발차기					// 엘소드 구현완료.
		SI_A_ESK_ARMOR_BREAK,							/// 아머 브레이크			// dmlee 작업중, 2009-07-27
		SI_A_EMK_BIG_BURST,								/// 대폭살					// hoons 작업중(1차 완료하여 피드백 대기중)
		SI_A_ELK_ROLLING_SMASH,							/// 롤링 스매쉬
		SI_A_ERS_SWORD_ENCHANT,							/// 소드 인챈트
#ifdef ELSWORD_SHEATH_KNIGHT
		SI_A_ES_ROLLING,								/// 구르기
		SI_A_ETK_HARSH_CHASER,							/// 하쉬 체이서
#endif ELSWORD_SHEATH_KNIGHT
#ifdef SERV_ELSWORD_INFINITY_SWORD
		SI_A_EIS_MIRAGE_STING	= 1209,					// 미라지 스팅
#endif //SERV_ELSWORD_INFINITY_SWORD
#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
		SI_A_ELK_IMPACT_SMASH,							//  임팩트 스매시
		SI_A_ES_SWORD_WAVE,								//  소드 웨이브
		SI_A_ES_DEFENCE,								//  막기			
		SI_A_ERS_CRITICAL_SWORD,						//	크리티컬 소드
		SI_A_ERS_RUNE_OF_FIRE,							//  초열의 룬
		SI_A_ERS_RUNE_OF_ICE,							//  한빙의 룬
		SI_A_ETK_PIERCING_SWORD,						//  피어싱 소드
		SI_A_EIS_FATAL_SLAP,							//  페이탈 슬랩
		SI_A_ETK_SWORD_SHIELD,							//  소드 실드
		SI_A_ESK_WEAPON_BREAK,							//  웨폰 브레이크
#endif //UPGRADE_SKILL_SYSTEM_2013

		// 패시브
		SI_P_ES_COUNTER_UP	= 1400,						/// 반격 연마				// 엘소드 구현완료.
		SI_P_ES_POWERFUL_FIREBALL,						/// 강화된 파이어볼			// 엘소드 구현완료.
		SI_P_ELK_STRONG_BONE,							/// 근골 체질			
		SI_P_ERS_SPELL_CHAIN,							/// 마력의 사슬
		SI_P_ELK_REDUCE_PAIN,							/// 고통억제				// kimhc // 2010-11-15
		SI_P_ERS_MAGIC_RESISTANCE,						/// 마법저항 수련			// oasis907
#ifdef ELSWORD_SHEATH_KNIGHT
		SI_P_ETK_COMPACT_COUNTER,						/// 빈틈없는 반격
		SI_P_ETK_BRUTAL_SLAYER,							/// 잔혹한 학살자								
#endif ELSWORD_SHEATH_KNIGHT
#ifdef SERV_ELSWORD_INFINITY_SWORD
		SI_P_EIS_MIND_OF_FIGHTER	= 1408,				// 승부사의 기질
		SI_P_EIS_LIGHTNING_STEP,						// 전광 석화
#endif //SERV_ELSWORD_INFINITY_SWORD
#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
		SI_P_ESK_AUTO_GUARD,							//  오토 가드
		SI_P_ERS_RUNE_MASTERY,							//  룬 마스터리
		SI_P_ESK_INTENSIVE_ATTACK,						//  강렬한 일격
		SI_P_ELK_INDURANCE_OF_REVENGE,					//  역전의 인내
		SI_P_EIS_CUTTING_SWORD,							//  날카로운 검
#endif //UPGRADE_SKILL_SYSTEM_2013


		// aisha
		SI_SA_AV_CHAIN_FIRE_BALL = 1500,				/// 체인 파이어볼
		SI_SA_AV_BINDING_CIRCLE,						/// 바인딩 서클
		SI_SA_AV_GUST_SCREW,							/// 거스트 스크류
		SI_SA_AV_LIGHTNING_BOLT,						/// 라이트닝 볼트
		SI_SA_AV_GUST_STORM,							/// 거스트 스톰
		SI_SA_AV_METEO_CALL,							/// 메테오 콜
		SI_A_AEM_CHAIN_BURST,							/// 체인 버스트
		SI_SA_AHM_MAGIC_MISSILE,						/// 매직 미사일
		SI_SA_AHM_INFERNAL_WAVE,						/// 인퍼널 웨이브
		SI_SA_ADM_HELL_STONE,							/// 헬스톤
		SI_SA_ADM_DARK_CLOUD,							/// 다크 클라우드
		SI_SA_ADM_PLASMA_CUTTER,						/// 플라즈마 커터
		SI_SA_AHM_BLIZZARDSHOWER,						/// 블리자드 샤워			// dmlee 작업중	
		SI_SA_ADM_DEATHFIELD,							/// 데쓰필드				// dmlee 작업중
		SI_SA_AHM_BLAZE_STEP,							/// 블레이즈 스탭			// hoons. 작업완료.
		SI_SA_ADM_AGING,								/// 에이징					// wonpok 구현중
		SI_SA_AEM_CYCLONE,								/// 싸이클론
		SI_SA_AVP_HELL_DROP,							/// 헬 드롭
#ifdef NEW_SKILL_2010_11		
		SI_SA_AEM_METEOR_SHOWER					= 1518,
		SI_SA_AVP_PHANTOM_BREATHING_DARK_FALL				= 1519,
#endif NEW_SKILL_2010_11
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
		SI_SA_ABM_HEAVY_PRESS					= 1520,	// 헤비 프레스
		SI_SA_ABM_ENERGY_SPURT,							// 에너지 스퍼트
		SI_SA_ABM_SUPER_NOVA,							// 슈퍼 노바
		SI_SA_ABM_GUILLOTINE_PRESS,						// 길로틴 프레스
		SI_SA_ABM_MAGICAL_MAKEUP,						// 메지컬 메이크업
#endif
#ifdef SERV_ARME_DIMENSION_WITCH
		SI_SA_ADW_MORNING_STAR	= 1525,					// 모닝 스타
		SI_SA_ADW_SCREWDRIVER,							// 스크류 드라이버
#endif

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_SA_AV_ICE_STORM,						/// 아이스 스톰
		SI_SA_AHM_AQUA_SHOWER,					/// 아쿠아 샤워
		SI_SA_AHM_LIGHTNING_SHOWER,				/// 라이트닝 샤워
		SI_SA_AHM_CHAIN_LIGHTNING,				/// 체인 라이트닝
		SI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE,	/// 팬텀 브리딩 - 다크 홀
		SI_SA_ADW_SCREWDRIVER_DRILLER,			/// 스크류 드라이버 - 드릴러
#endif //UPGRADE_SKILL_SYSTEM_2013


		// 액티브
		SI_A_AV_TELEPORT = 1700,						/// 텔레포트				// 아이샤 구현완료. 
		SI_A_AV_MANA_SHIELD,							/// 마나 실드				// wonpok, 0527. 구현완료
		SI_A_AV_CIRCLE_FLAME,							/// 서클 플래임				// wonpok, 0525. 구현완료
		SI_A_AHM_MEDITATION,							/// 명상					// dmlee 작업중
		SI_A_ADM_MANA_INTAKE,							/// 마나 흡수				// wonpok 작업완료.
		SI_A_AEM_STONE_WALL,							/// 돌벽
		SI_A_AVP_SUMMON_BAT,							/// 박쥐 소환
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
		SI_A_AV_FIRE_ROAD,								// 파이어 로드
		SI_A_ABM_ENERGY_DRAIN,							// 에너지 드레인
#endif
#ifdef SERV_ARME_DIMENSION_WITCH
		SI_A_ADW_DISTORTION	= 1709,						// 공간 왜곡
#endif

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_A_AV_ICICLE_WAVE	= 1710,		/// 아이시클 웨이브
		SI_A_AVP_SUMMON_BAT_HEAVY,		/// 쁘띠 앙고르 - 헤비 돌
		SI_A_AHM_SHINING_BODY,			/// 샤이닝 바디
		SI_A_AVP_SHADOW_BODY,			/// 쉐도우 바디
		SI_A_ADW_ENERGETIC_BODY,		/// 에너제틱 바디
		SI_A_ABM_VITAL_DRAIN,			/// 바이탈 드레인
		SI_A_ADW_WORM_HOLE,				/// 웜홀
#endif //UPGRADE_SKILL_SYSTEM_2013


		// 패시브
		SI_P_AV_TELEPORT_UP = 1900,						/// 텔레포트 연마			// 아이샤 구현완료.
		SI_P_AV_FIREBALL_UP,							/// 파이어볼 연마			// 아이샤 구현완료.
		SI_P_AEM_ELEMENTAL_FRIENDSHIP,					/// 원소 친화
		SI_P_AVP_STRONG_MIND,							/// 강한 마음
		SI_P_AEM_ENERGY_OF_THE_PLENTY,					/// 풍요의 기운				// oasis907
#ifdef NEW_SKILL_2010_11
		SI_P_AVP_LIMITED_MANA_MANAGEMENT		= 1905,
#endif
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
		SI_P_ABM_FITNESS						= 1906,	// 신체 단련
		SI_P_ABM_MANAFLOW_OVERDRIVE,					// 기혈 폭주
		SI_P_ABM_SPIRIT_ACCELERATION,					// 기력 가속
#endif
#ifdef SERV_ARME_DIMENSION_WITCH
		SI_P_ADW_ADVANCED_TELEPORTATION	= 1909,			// 공간 도약
		SI_P_ADW_MAGICAL_STAFF,							// 마력의 지팡이
#endif

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_P_AHM_FREEZING_POINT_RESEARCH	= 1911,		/// 빙점 연구
		SI_P_ADM_SHADOW_OF_DISASTER,					/// 재앙의 그림자
		SI_P_AEM_ELEMENTAL_RESEARCH,					/// 원소학 연구
		SI_P_AVP_MAGIC_AMPLIFICATION,					/// 마력 증폭
		SI_P_AVP_GRAVITY_BALL_RESEARCH,					/// 그라비티 볼 연구
#endif //UPGRADE_SKILL_SYSTEM_2013


		// rena
		SI_SA_LE_RAIL_STINGER = 2000,					/// 레일 스팅거
		SI_SA_LE_ASSAULT_KICK,							/// 어설트 킥
		SI_SA_LE_PERFECT_STORM,							/// 퍼팩트 스톰
		SI_SA_LE_MULTIPLE_STINGER,						/// 멀티플 스팅거
		SI_SA_LE_AERO_TORNADO,							/// 에어로 토네이도
		SI_SA_LE_PHOENIX_STRIKE,						/// 피닉스 스트라이크
		SI_SA_LCR_SPINNING_KICK,						/// 스피닝 킥
		SI_SA_LCR_CRESCENT_KICK,						/// 크레센트 킥
		SI_SA_LCR_DIVE_KICK_BOMBING,					/// 다이브 킥 봄잉
		SI_SA_LSR_HUMMING_WIND,							/// 허밍 윈드
		SI_SA_LSR_GUIDED_ARROW,							/// 가이드 애로우
		SI_SA_LSR_CRAZY_SHOT,							/// 크레이지 샷
		SI_SA_LCR_SHARPFALL,							/// 샤프 폴				// dmlee 작업중
		SI_SA_LSR_WINDWARD,								/// 윈드와드			// dmlee 작업중
		SI_SA_LCR_VIOLENT_ATTACK,						/// 바이올런트 어택		// wonpok 구현완료
		SI_SA_LSR_GUNGNIR,								/// 궁그닐				// kimhc 구현완료
		SI_SA_LWS_SLIDE_KICK,							/// 슬라이드 킥
		SI_SA_LGA_FREEZING_ARROW,						/// 프리징 애로우
		SI_SA_LWS_AIRELINNA,							/// 아이레린나				// oasis907
#ifdef NEW_SKILL_2010_11
		SI_SA_LGA_AERO_STRAFE					= 2019,
#endif NEW_SKILL_2010_11

#ifdef	SERV_TRAPPING_RANGER_TEST
		SI_SA_RTR_EVOKE							= 2020,	///	이보크
		SI_SA_RTR_FATALITY						= 2021,	///	페이탈리티
		SI_SA_RTR_TRAPING_ARROW					= 2022,	///	트래핑 애로우
		SI_SA_RTR_CALL_OF_RUIN					= 2023,	///	콜 오브 루인
		SI_SA_RTR_KARMA							= 2024,	///	카르마
#endif

#ifdef SERV_RENA_NIGHT_WATCHER
		SI_SA_RNW_FURIOUS_ENGAGE				= 2025,	///	퓨리어스 인게이지
		SI_SA_RNW_GLIDING_STRIKE				= 2026,	///	글라이딩 스트라이크
#endif

#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
		SI_SA_RTR_TRAPPING_ARROW_FUNGUS,				/// 트래핑 애로우 - 펑거스
		SI_SA_LWS_AIRELINNA_SYLPH,						/// 아이레린나 - 실프
#endif //UPGRADE_SKILL_SYSTEM_2013


		// 액티브
		SI_A_LE_SIEGEMODE = 2200,						/// 시즈 모드				// 레나 구현완료.
		SI_A_LE_BACKJUMP_SHOT,							/// 백덤블링 샷				// wonpok, 0525. 구현완료
		SI_A_LE_REFLEX_MAGIC,							/// 마법반사
		SI_A_LCR_LOW_KICK,								/// 로우킥					// wonpok. 작업완료
		SI_A_LSR_ENTANGLE,								/// 인텡글					// dmlee 작업중
		SI_A_LWS_NATURE_FORCE,							/// 자연의 힘
		SI_A_LGA_STIGMA_ARROW,							/// 스티그마 애로우 

#ifdef	SERV_TRAPPING_RANGER_TEST
		SI_A_LE_RISING_FALCON					= 2207,	///	라이징 팔콘
		SI_A_RTR_EXPLOSION_TRAP					= 2208,	///	폭발의 덫
#endif

#ifdef SERV_RENA_NIGHT_WATCHER
		SI_A_RNW_START_OF_DELAYED_FIRING		= 2209,	///	지연의 신호탄
#endif

#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
		SI_A_LCR_MIDDLE_KICK,							/// 미들킥
		SI_A_LWS_BACK_KICK,								/// 백킥
		SI_A_LWS_HIGH_KICK,								/// 하이킥
		SI_A_LSR_SHOOTING_MAGNUM,						/// 슈팅 매그넘
		SI_A_LGA_ARC_SHOT,								/// 아크 샷
		SI_A_LGA_RAPID_SHOT,							/// 래피드 샷
		SI_A_LGA_FREEZING_BOLT,							/// 프리징 볼트
		SI_A_LGA_WIND_BLAST,							/// 윈드 블래스트
		SI_A_RNW_THORNS_TRAP,							/// 가시덩굴 덫
		SI_P_RNW_SEED_OF_RUIN,							/// 진노의 씨앗
		SI_A_RNW_ANGER_OF_ELF,							/// 엘드랏실의 분노
		SI_A_RNW_ROSEBUSH_TRAP,							/// 찔레 덫
		SI_A_LWS_ASSAULT_IMPACT,						/// 어설트 임팩트
#endif //UPGRADE_SKILL_SYSTEM_2013


		// 패시브
		SI_P_LE_SIEGEMODE_UP = 2400,					/// 시즈모드 연마			// 레나 구현완료.
		SI_P_LE_SHARP_KICK,								/// 날렵한 발차기			// 레나 구현완료.
		SI_P_LE_POWERFUL_BOWSTRING,						/// 강화된 보우스트링		// 레나 구현완료.
		SI_P_LE_HAWK_EYE,								/// 호크아이				// 레나 구현완료.
		SI_P_LE_SHARP_ARROW,							/// 다듬어진 화살			// 레나 구현완료.
		SI_P_LWS_POWERFUL_SHOT,							/// 파워풀 샷
		SI_P_LGA_COMMUNE_OF_NATURE,						/// 자연의 친화
		SI_P_LWS_FRIENDSHIP_OF_NATURE,					/// 자연과의 친화			// oasis907
		SI_P_LGA_VIBRATION_SHOOTING,					/// 진동 사격				// oasis907

#ifdef	SERV_TRAPPING_RANGER_TEST
		SI_P_RTR_HUNTERS_ABILITY				= 2409,	///	사냥꾼의 재능(패시브)
		SI_P_RTR_SHARPEN_ARROW					= 2410,	///	날카로운 화살(패시브)
		SI_P_RTR_VITALPOINT_PIERCING			= 2411,	///	급소 찌르기(패시브)
#endif

#ifdef SERV_RENA_NIGHT_WATCHER
		SI_P_RNW_PRIOR_PLANNED_BLOW				= 2412,	///	계산된 일격
		SI_P_RNW_GUARDIAN_OF_ELF,						/// 엘드랄실의 수호자
#endif


		// raven
		SI_SA_RF_MAXIMUM_CANNON = 2500,					/// 맥시멈 캐논
		SI_SA_RF_EARTH_BREAKER,							/// 어스 브레이크
		SI_SA_RF_POWER_ASSAULT,							/// 파워 어설트
		SI_SA_RF_CANNON_BLADE,							/// 캐논 블레이드
		SI_SA_RF_MAGNUM_BLASTER,						/// 매그넘 블래스터
		SI_SA_RF_SEVEN_BURST,							/// 세븐 버스트
		SI_SA_RST_BERSERKER_BLADE,						/// 버서커 블레이드
		SI_SA_RST_FLYING_IMPACT,						/// 플라잉 임팩트
		SI_SA_RST_HYPER_SONIC_STAB,						/// 하이퍼 소닉스탭
		SI_SA_ROT_CHARGED_BOLT,							/// 차지드 볼트
		SI_SA_ROT_ARC_ENEMY,							/// 아크 애너미
		SI_SA_ROT_GUARDIAN_STRIKE,						/// 가디언 스트라이크
		SI_SA_RST_SHOCKWAVE,							/// 쇼크 웨이브				// hoons. 작업중. (현재 1차 완료 피드백 대기중)
		SI_SA_ROT_VALKYRIESJAVELIN,						/// 발키리스 자벨린			// wonpok. 작업완료
		SI_SA_RST_BLOODY_ACCEL,							/// 블러디엑셀				// wonpok 구현중
		SI_SA_ROT_NUCLEAR,								/// 뉴클리어				// wonpok 구현중
		SI_SA_RBM_WOFL_FANG,							/// 울프팽
		SI_SA_RRF_X_CRASH,								/// 엑스 크래시

		SI_SA_RBM_GIGA_DRIVE,							/// 기가 드라이브			// kimhc // 2010-11-15
		SI_SA_RRF_WILD_CHARGE,							/// 와일드 차지				// kimhc // 2010-11-15

#ifdef RAVEN_WEAPON_TAKER
		SI_SA_RWT_BURSTING_BLADE,						// 버스팅 블레이드
		SI_SA_RWT_REVOLVER_CANNON,						// 리볼버 캐논
		SI_SA_RWT_HARPOON_SPEAR,						// 하푼 스피어
		SI_SA_RWT_HELLFIRE_GATLING,						// 헬파이어 개틀링
		SI_SA_RWT_GIGA_PROMINENCE,						// 기가 프로미넌스
#endif RAVEN_WEAPON_TAKER

#ifdef SERV_RAVEN_VETERAN_COMMANDER
		SI_SA_RVC_DEADLY_RAID = 2525,					// 데들리 레이드
		SI_SA_RVC_IGNITION_CROW,						// 이그니션 크로우
#endif SERV_RAVEN_VETERAN_COMMANDER

#ifdef UPGRADE_SKILL_SYSTEM_2013 // Raven 스킬 개편, 김종훈
		SI_SA_RWT_REVOLVER_CANNON_HE,						// 리볼버 캐논 HE		
		SI_SA_RVC_IGNITION_CROW_INCINERATION,				// 이그니션 크로우 인시너레이션
		SI_SA_RBM_GIGA_DRIVE_BURST,							// 기가 드라이브 - 폭
		SI_SA_ROT_CHARGED_BOLT_BLOOD,						// 차지드 볼트 - 쏜즈 블러드
		SI_SA_RST_SHOCKWAVE_FRONT,							// 쇼크 웨이브 - 속
#endif // UPGRADE_SKILL_SYSTEM_2013 // Raven 스킬 개편, 김종훈


		// 액티브
		SI_A_RF_BURNING_RUSH = 2700,					/// 버닝러시				// 레이븐 구현완료.
		SI_A_RF_SHADOW_STEP,							/// 섀도우스탭				// 레이븐 구현완료.
		SI_A_RST_CUT_TENDON,							/// 컷텐돈					// dmlee 작업중
		SI_A_ROT_WEAPON_BREAK,							/// 무기 깨기				// dmlee 작업중, 2009.07.28 
		SI_A_RBM_ONE_FLASH,								/// 일섬
		SI_A_RRF_GROUND_IMPACT,							/// 그라운드 임팩트
#ifdef RAVEN_WEAPON_TAKER
		SI_A_RF_BREAKING_FIST,							// 브레이킹 피스트
		SI_A_RWT_BARRAGE_ATTACK,						// 연무 폭파
#endif RAVEN_WEAPON_TAKER

#ifdef SERV_RAVEN_VETERAN_COMMANDER
		SI_A_RVC_BLEEDING_SLICER = 2708,				// 치명상
#endif SERV_RAVEN_VETERAN_COMMANDER

#ifdef UPGRADE_SKILL_SYSTEM_2013 // Raven 스킬 개편, 김종훈, 액티브
		SI_A_RRF_HELL_DIVE,								// 헬 다이브
		SI_A_RRF_LIMIT_CRUSHER,							// 리미트 크러셔
		SI_A_RWT_FLAME_SWORD,							// 화염인
		SI_A_RVC_NAPALM_GRENADE,						// 네이팜 그레네이드
		SI_A_ROT_ARMOR_BREAK,							// 엘소드 아머 브레이커 추가
		SI_A_RBM_SONIC_SLASH,							// 소닉 슬래시
#endif // UPGRADE_SKILL_SYSTEM_2013 // Raven 스킬 개편, 김종훈

		// 패시브
		SI_P_RF_THRUST = 2900,							/// 섀도우피어싱			// 레이븐 구현완료.
		SI_P_RBM_BLADE_MASTERY,							/// 숙련된 검술
		SI_P_RRF_BURNING_NASOD_HAND,					/// 타오르는 나소드 핸드
		SI_P_RBM_EMERGENCY_ESCAPE,						/// 긴급 탈출
		SI_P_RRF_SPIRITUALIZED_FURY,					/// 승화된 분노
#ifdef RAVEN_WEAPON_TAKER
		SI_P_RWT_AIR_SLICER,							// 에어 슬라이서
		SI_P_RWT_BEHAVIOR_OF_MERCENARY,					// 용병의 처신술
#endif RAVEN_WEAPON_TAKER

#ifdef SERV_RAVEN_VETERAN_COMMANDER
		SI_P_RVC_SHADOW_BACK_SLIDE = 2907,				// 섀도우 백 슬라이드
		SI_P_RVC_SURVIVAL_TECHNIQUE_OF_MERCENARY,		// 용병의 생존술
#endif SERV_RAVEN_VETERAN_COMMANDER

#ifdef UPGRADE_SKILL_SYSTEM_2013 // Raven 스킬 개편, 김종훈
		SI_P_RBM_EXQUISITE_SWORDMANSHIP,				// 섬세한 검술
		SI_P_RBM_SHADOW_THRUST,							// 쉐도우 쓰러스트
		SI_P_RRF_EXPLOSIVE_NASOD_HAND,					// 폭발하는 나소드 핸드
		SI_P_RRF_SHADOW_PUNISHER,						// 쉐도우 퍼니셔
		SI_P_RVC_WINNABLE_TECHNIQUE_OF_MERCENARY,		// 용병의 승리전략
		SI_P_RVC_OVERHEAT_MODE,							// 과열모드 작동!
#endif // UPGRADE_SKILL_SYSTEM_2013 // Raven 스킬 개편, 김종훈


		// eve
		// 스페셜 액티브
		SI_SA_EN_ILLUSION_STRIKE = 3000,				/// 일루전 스트라이크
		SI_SA_EN_DIMENSION_LINK,						/// 디멘션 링크
		SI_SA_EN_ILLUSION_STINGER,						/// 일루전 스팅어
		SI_SA_EN_DIMENSION_LINK_BLADER,					/// 디멘션링크 블레이더
		SI_SA_EN_GENERATE_BLACKHOLE,					/// 제네레이터 블랙홀
		SI_SA_EN_DIMENSION_LINK_GUARDIAN,				/// 디멘션링크 가디언
		SI_SA_EEG_HORNET_STING,							/// 호넷 스팅
		SI_SA_EEG_EXPLOSION_IMPACT,						/// 익스플로전 임팩트
		SI_SA_EEG_QUEENS_THRONE,						/// 퀸스 쓰론
		SI_SA_EAT_SURFACE_CUTING,						/// 서피스 커팅
		SI_SA_EAT_SONIC_WAVE,							/// 소닉 웨이브
		SI_SA_EAT_GENOCIDE_RIPPER,						/// 제노사이드 리퍼
		SI_SA_EEG_IRONSCRAPS,							/// 아이언 스크랩스			// hoons. 작업중. (현재 1차 완료 피드백 대기중)
		SI_SA_EAT_SPACE_WRENCH,							/// 스페이 스랜처			// kimhc, 작업중. (현재 1차 완료)
		SI_SA_EEG_JUNK_BREAK,							/// 정크 브레이크			// wonpok 구현완료
		SI_SA_EAT_HEAVENS_FIST,							/// 해븐즈 피스트			// wonpok 구현완료
		SI_SA_ENS_ATOMIC_BLASTER,						/// 아토믹 블래스터
		SI_SA_EEP_ELECTRONIC_FIELD,						/// 일렉트로닉 필드
		SI_SA_ENS_ATOMIC_SHIELD,						/// 아토믹 쉴드
		SI_SA_EEP_ASSUALT_SPEAR,						/// 어설트 스피어
#ifdef EVE_ELECTRA
		SI_SA_EEL_PARTICLE_RAY,							// 파티클 레이
		SI_SA_EEL_SWEEP_ROLLING,						// 스위프 롤링
		SI_SA_EEL_PHOTON_FLARE,							// 포톤 플레어
		SI_SA_EEL_GIGA_STREAM,							// 기가 스트림
		SI_SA_EEL_THOUSANDS_OF_STARS,					// 사우전드 스타
#endif EVE_ELECTRA
#ifdef SERV_EVE_BATTLE_SERAPH
		SI_SA_EBS_ENERGY_NEEDLES,						/// 에너지 니들스
		SI_SA_EBS_ENERGETIC_HEART,						/// 에너제틱 하트
#endif
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_SA_EN_ILLUSION_RAY,							/// 일루전 레이
		SI_SA_EEG_HORNET_STING_SHAPED_CHARGE,			/// 호넷 스팅 - 쉐이프트 차지
		SI_SA_ENS_ATOMIC_BLASTER_SONICGUN,				/// 아토믹 블래스터 - 플라즈마 건
		SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN,				/// 스위프 롤링 - 트라이 발칸
		SI_SA_EAT_HEAVENS_FIST_SWEEPER,					/// 헤븐즈 피스트 - 스위퍼
#endif //UPGRADE_SKILL_SYSTEM_2013
		// 액티브
		SI_A_EN_CLOAKING = 3200,						/// 클로킹					// 이브 구현 완료.
		SI_A_EN_CONVERSION,								/// 마력 전환				// 이브 구현 완료.
		SI_A_EN_MEGA_ELECTRONBALL,						/// 메가 일랙트론볼			// 이브 구현 완료.
		SI_A_EEG_METAL_DUST_AURA,						/// 메탈 더스트 아우라		// hoons. 작업중. (현재 1차 완료 피드백 대기중)
		SI_A_EAT_OBERON_GUARD,							/// 오베론가디안			// hoons. 작업중.
		SI_A_ENS_SPEAR_BURST,							/// 스피어 버스트
		SI_A_EEP_SPIT_FIRE,								/// 스핏 파이어
#ifdef EVE_ELECTRA
		SI_A_EVE_PHOTON_BLINK,							// 광자 점멸
		SI_A_EEL_SPECTRO_EL_CRYSTAL,					// 엘 분광 결정
#endif EVE_ELECTRA
#ifdef SERV_EVE_BATTLE_SERAPH
		SI_A_EBS_TASER_PILUM,							/// 테이저 필라 (복수형 발음)
#endif
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_A_EN_TESLA_SHOCK,							/// 테슬라 쇼크
		SI_A_EEP_SPIT_FIRE_GRENADE,						/// 스핏 파이어 - 그리네이드
		SI_A_EBS_KUGELBLITZ,							/// 쿠글블릿츠
#endif //UPGRADE_SKILL_SYSTEM_2013

		// 패시브
		SI_P_EN_CLOAKING_UP = 3400,						/// 클로킹 연마			// 이브 구현 완료.
		SI_P_EN_ELECTRONBALL_UP,						/// 일렉트론볼 연마		// 이브 구현 완료.
		SI_P_EN_QUEENS_CONTROL,							/// 여왕의 지배력		// 이브 구현 완료.
		SI_P_EN_QUEENS_POTENTIAL,						/// 여왕의 잠재력		// 이브 구현 완료.
		SI_P_ENS_NASOD_WEAPON,							/// 나소드 무기 강화
		SI_P_EEP_ELECTRONBALL_MASTER,					/// 일렉트론 볼마스터
		SI_P_ENS_ENERGY_OF_CONCENTRATION,				/// 집중의 기운			// oasis907
		SI_P_EEP_CHARGING_BOOSTER,						/// 충전추진기			// kimhc // 2010-11-
#ifdef EVE_ELECTRA
		SI_P_EEL_SPECTRUM_DYNAMICS,						// 분광 역학
		SI_P_EEL_PHOTON_BOOSTER,						// 광자 추진기
		SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR,			// 강화 엘 에너지 반응로
#endif EVE_ELECTRA
#ifdef SERV_EVE_BATTLE_SERAPH
		SI_P_EBS_HYPER_OPTICAL_RESEARCH,				/// 초광학 연구
		SI_P_EBS_HIGHPOWERED_ELECTRICS,					/// 고출력 전자회로 (공돌이 스킬, 납땜기 들고 와라!)
#endif
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_P_EBS_AMPLIFIED_EL_ENERGY_REACTOR,			/// 증폭 엘 에너지 반응로
#endif //UPGRADE_SKILL_SYSTEM_2013


		///////////////////////////// CHUNG //////////////////////////////////////////////////////////////
		//////////////////////////// 스페셜 액티브
		SI_SA_CHUNG_BRUTAL_SWING	= 4000,				/// 브루탈 스윙
		SI_SA_CHUNG_DETONATION,							/// 디토네이션
		SI_SA_CHUNG_LUNATIC_BLOW,						/// 루나틱 블로우
		SI_SA_CHUNG_SCARE_CHASE,						/// 스케어 체이스
		SI_SA_CHUNG_GIGANTIC_IMPACT,					/// 기간틱 임팩트
		SI_SA_CHUNG_ACELDAMA,							/// 아겔다마

		//{{ kimhc // 2011.1.8 // 청 1차 전직
		SI_SA_CFG_STEEL_EDGE,							/// 스틸엣지 (퓨리가디언)
		SI_SA_CFG_IRON_HOWLING,							/// 아이언 하울링 (퓨리가디언)
		SI_SA_CFG_LUNATIC_FURY,							/// 루나틱퓨리 (퓨리가디언)

		SI_SA_CSG_DUAL_BUSTER,							/// 듀얼버스터 (슈팅가디언)
		SI_SA_CSG_HEAVY_RAILGUN,						/// 헤비레일건 (슈팅가디언)
		SI_SA_CSG_SHOOTING_STAR,						/// 슈팅스타 (슈팅가디언)
		//}} kimhc // 2011.1.8 // 청 1차 전직

		//{{ kimhc // 2011-3-16 // 2011-04 에 패치될 청 캐시 스킬
		SI_SA_CFG_PANDEMONIUM,							/// 판데모니움 (퓨리가디언)
		SI_SA_CFG_PAIN_OF_CALADBOLG,					/// 칼라볼그페인 (퓨리가디언)

		SI_SA_CSG_COMET_CRASHER,						/// 카밋크래셔 (슈팅가디언)
		SI_SA_CSG_SHARPSHOOTER_SYNDROME,				/// 샤프슈터신트롬 (슈팅가디언)
		//}} kimhc // 2011-3-16 // 2011-04 에 패치될 청 캐시 스킬

		SI_SA_CIP_BURST_WOLF,							/// 버스트 울프 (아이언 팔라딘)
		SI_SA_CIP_LAND_DEMOLISHIER,						/// 랜드 디몰리셔 (아이언 팔라딘)

		//{{ JHKang / 강정훈 / 2011.6.21
		SI_SA_CDC_BULLET_BLITZ,							/// 불렛 블릿츠 (데들리 체이서)
		SI_SA_CDC_ARTILLERY_STRIKE,						/// 아틸러리 스트라이크 (데들리 체이서)
		//}}

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		SI_SA_CHG_WONDER_WALL			= 4020,			// 원더 월
		SI_SA_CHG_DREAD_CHASE,							// 드레드 체이스
		SI_SA_CHG_BIGBANG_STREAM,						// 빅뱅 스트림
		SI_SA_CHG_CHAOS_CANNON,							// 카오스 캐논
		SI_SA_CHG_CARPET_BOMBING		= 4024,			// 카펫 바밍
#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN

#ifdef SERV_CHUNG_TACTICAL_TROOPER
		SI_SA_CTT_CROSS_FIRE			= 4025,			// 크로스 파이어
		SI_SA_CTT_TACTICAL_FIELD		= 4026,			// 택티컬 필드
#endif //SERV_CHUNG_TACTICAL_TROOPER

		//#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈
		SI_SA_CFG_PANDEMONIUM_FEAR					= 4027,	// 판데모니움 - 피어
		SI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE		= 4028,	// 랜드 디몰리셔 - 어스 퀘이크
		SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA	= 4029,	// 아틸러리 스트라이크 - 퀀텀 발리스타
		SI_SA_CTT_TACTICAL_FIELD_RAID				= 4030,	// 택티컬 필드 - 레이드
		//#endif // UPGRADE_SKILL_SYSTEM_2013


		/////////////////////////////// 액티브
		SI_A_CHUNG_RELOAD_CANNON	= 4200,				/// 탄전
		SI_A_CHUNG_AIMMING_SHOT,						/// 에이밍 샷
		//{{ kimhc // 2011.1.10 // 청 1차 전직
		SI_A_CFG_GUARD,									/// 가드 (퓨리가디언)
		SI_A_CFG_LEAP_ATTACK,							/// 리프어택 (퓨리가디언)

		SI_A_CSG_HEAD_SHOT,								/// 헤드샷 (슈팅가디언)
		SI_A_CSG_RUMBLE_SHOT,							/// 럼블샷 (슈팅가디언)
		//}} kimhc // 2011.1.10 // 청 1차 전직
		SI_A_CIP_SUDDEN_BURSTER,						/// 서든 버스터 (아이언 팔라딘)
		//{{ JHKang / 강정훈 / 2011.6.21
		SI_A_CDC_LEG_SHOT,								/// 레그샷 (데들리 체이서)
		//}}
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		SI_A_CHUNG_SIEGE_SHELLING	= 4208,				// 고정 포격
		SI_A_CHG_ELASTIC_BOMB,							// 일레스틱 밤
		SI_A_CHG_MARK_OF_COMMANDER	= 4210,				// 지휘관의 표식
#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN

#ifdef SERV_CHUNG_TACTICAL_TROOPER
		SI_A_CTT_AUTOMATIC_MORTAR		= 4211,				// 박격포
#endif //SERV_CHUNG_TACTICAL_TROOPER

		//#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈
		SI_A_CHUNG_IMPACT_DETONATION	= 4212,			/// 임팩트 디토네이션
		SI_A_CIP_ARTILLERY_NOVA			= 4213,			/// 아틸러리 노바
		SI_A_CSG_DOUBLE_FIRE			= 4214,			/// 더블 파이어
		SI_A_CSG_MAGNUM_SHOT			= 4215,			/// 매그넘 샷
		SI_A_CDC_GATLING_SHOT			= 4216,			/// 개틀링 샷
		SI_A_CHG_ELASTIC_BOMB_LAUNCHER	= 4217,			/// 일레스틱 밤 - 그리네이드 런처
		SI_A_CTT_BOMBARD_SERVICE		= 4218,			/// 지원 포격
		//#endif // UPGRADE_SKILL_SYSTEM_2013

		//////////////////////////////// 패시브
		SI_P_CHUNG_RELOAD_CANNON	= 4400,				/// 숙련된 탄전	(더이상 사용하지 않음)
		//{{ kimhc // 2011.1.10 // 청 1차 전직
		SI_P_CFG_METABOLISM_BOOST,						/// 신진대사 촉진 (퓨리가디언)
		SI_P_CFG_GUARD_MASTERY,							/// 방어숙련 (퓨리가디언)
		SI_P_CFG_ELEMENTAL_DEFENCE_A,					/// 불과 물과 자연 (퓨리가디언)
		SI_P_CFG_ELEMENTAL_DEFENCE_B,					/// 바람과 해와 달 (퓨리가디언)

		SI_P_CSG_REMODEL_MAGAZINE,						/// 탄창/약실 개조 (슈팅가디언)
		SI_P_CSG_REMODEL_RIFLE,							/// 강선 개조 (슈팅가디언)
		SI_P_CSG_TUSSLE_TECHNIQUE,						/// 난전의 기술 (슈팅가디언)
		//}} kimhc // 2011.1.10 // 청 1차 전직

		SI_P_CIP_ADVANCED_METABOLISM_BOOST,				/// 신진대사 촉진 강화 (아이언 팔라딘)
		SI_P_CIP_IRON_WILL,								/// 굳건한 의지 (아이언 팔라딘)

		//{{ JHKang / 강정훈 / 2011.6.21
		SI_P_CDC_ACCURATE_MARKMANSHIP,					/// 정밀한 사격술 (데들리 체이서)
		SI_P_CDC_INTENSE_SHOWTIME,						/// 강렬한 쇼타임 (데들리 체이서)
		//}}

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		SI_P_CHUNG_ABILITY_CANNONEER	= 4412,			// 숙련된 포병의 능력
		SI_P_CHG_RELOAD_MASTERY,						// 리로드 마스터리
		SI_P_CHG_CONVERTED_WARHEAD,						// 탄두 개조
		SI_P_CHG_CANNONEER_WITHSTANDING	= 4415,			// 포병의 버티기
#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN

#ifdef SERV_CHUNG_TACTICAL_TROOPER
		SI_P_CTT_MOBILE_ARMORED_MODE	= 4416,			// 이동 포격
		SI_P_CTT_REACTIVE_ARMOR			= 4417,			// 반응 장갑
#endif //SERV_CHUNG_TACTICAL_TROOPER

		//#ifdef UPGRADE_SKILL_SYSTEM_2013 // 오현빈
		SI_P_CDC_GAS_PISTON_SYSTEM		= 4418,			// 가스 피스톤 시스템
		//#endif // UPGRADE_SKILL_SYSTEM_2013

#pragma region 아라
#ifdef ARA_CHARACTER_BASE
#pragma region 스페셜 액티브
		SI_SA_ARA_WIND_WEDGE			= 4500,		/// 연풍설_바람 쐐기_낭아 2식
		SI_SA_ARA_SHADOW_RISING_JUMP,				/// 벽력뢰_천둥 꽃망울
		SI_SA_ARA_PULLING_PIERCE,					/// 천의_빨래 널기
		SI_SA_ARA_TEMPEST_DANCE,					/// 선풍_휘모리
		SI_SA_ARA_FALLING_DRAGON,					/// 강룡추_구름 떨구기
		SI_SA_ARA_SHADOW_KNOT,						/// 결영_그림자 매듭_귀살 3식
#ifdef ARA_CHANGE_CLASS_FIRST
		SI_SA_ALH_DOUBLE_COLLISION,					/// 용아 3식 - 팔괘장
		SI_SA_ALH_MOONLIGHT_SLASH,					/// 용아 4식 - 달빛 베기
		SI_SA_ALH_PEERLESSNESS_SLASH,				/// 무쌍참
		SI_SA_ALH_WHITE_TIGER,						/// 맹호격
		SI_SA_ALH_SWALLOW_ASSAULT,					/// 비연
		SI_SA_ASD_TURBULENT_WAVE,					/// 맹호 4식 - 노도
		SI_SA_ASD_SUPPRESSION,						/// 제압
#endif
#pragma endregion 연풍설, 벽력뢰, 천의, 선풍, 결영, 강룡추 / 팔괘장, 달빛 베기, 무쌍참, 맹호격, 비연 / 노도, 제압

#pragma region 액티브
		SI_A_ARA_TIGER_CLAW				= 4700,		/// 호랑이_발톱_맹호 2식
		SI_A_ARA_FORCE_SPEAR,						/// 기공창
		SI_A_ARA_GAIN_FORCE,						/// 흡공
#ifdef ARA_CHANGE_CLASS_FIRST
		SI_A_ALH_ROCK_CRASH,						/// 맹호 1식 - 바위깨기
		SI_A_ALH_BREAKING,							/// 이화접목
		SI_A_ASD_LOW_BRANDISH,						/// 맹호 3식 - 지축
#endif
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_A_ARA_QUICK_PIERCING,					/// 신속
		SI_A_ARA_SHOULDER_BLOW,						/// 철산고
		SI_A_ASD_DRAGON_FALLING,					/// 용추
		SI_A_ARA_QUICK_STAB,						/// 천격
#endif //UPGRADE_SKILL_SYSTEM_2013
#pragma endregion 바위 가르기, 흡공 / 바위깨기, 이화접목 / 지축 / 신속, 철산고, 용추, 천격

#pragma region 패시브
		SI_P_ARA_FORCE_SPEAR_MASTERY	= 4900,		/// 기공창 숙련
#ifdef ARA_CHANGE_CLASS_FIRST
		SI_P_ALH_QUICK_STAND,						/// 재빠른 기상
		SI_P_ALH_VIGOR_ENERGIZE,					/// 기력 활성화
		SI_P_ALH_FILLED_POWER,						/// 충만한 힘
		SI_P_ASD_QUICKEN_GUARD,						/// 번득이는 기질
		SI_P_ASD_BRUTAL_PIERCING,					/// 창술의 극의
#endif
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		SI_P_ASD_FULLMOON_SLASH,					/// 만월 베기
		SI_P_ASD_POWER_OF_TIGER,					/// 맹호의 힘
		SI_P_ASD_SELF_PROTECTION_FORTITUDE,			/// 호신강기
#endif //UPGRADE_SKILL_SYSTEM_2013
#pragma endregion 기공창숙련 / 재빠른 기상, 기력 활성화, 충만한 힘 / 번득이는 기지, 창술의 극의 / 만월베기, 맹호의 힘, 호신강기
#endif
#pragma endregion 스킬



// oasis907 : 김상윤 [2009.11.25] // 본섭 파싱 위해 주석해제
//#ifdef GUILD_SKILL
		//{{ oasis907 : 김상윤 //// 2009-11-12 ////  길드 스킬 ID
		SI_GP_COMMON_BALANCED_BODY					= 5000,		/// 균형잡힌 육체
		SI_GP_COMMON_HARMONIC_SPIRIT				= 5010,		/// 조화로운 영혼
		SI_GP_COMMON_CHEERING_OF_TEAM				= 5020,		/// 동료의 응원
		SI_GP_COMMON_MASTER_OF_ALCHEMIST			= 5030,		/// 연금술의 달인
		SI_GP_COMMON_CONCENTRATION_OF_SPIRIT		= 5040,		/// 정신 집중
		SI_GP_COMMON_CONCENTRATION_MAGICAL_POWER	= 5050,		/// 집중된 마력
		SI_GP_COMMON_VIOLENT						= 5060,		/// 격노
		SI_GP_COMMON_LIBERATION_OF_ANGER			= 5070,		/// 분노 해방
		// 여기까지 2트리
		SI_GP_COMMON_INFINITE_STRENGTH				= 5080,		/// 끝없는 체력
		SI_GP_COMMON_CHANCE_TO_REVERSE				= 5090,		/// 역전의 기회
		SI_GP_COMMON_SACRIFICE_OF_HERO				= 5100,		/// 영웅의 희생
		SI_GP_COMMON_MIDAS							= 5110,		/// 마이더스의 손
		SI_GP_COMMON_SUPPORT_AURA					= 5120,		/// 기사 회생
		SI_GP_COMMON_IMPROVE_CHANCE					= 5130,		/// 기회 포착
		SI_GP_COMMON_ESCAPE_FROM_CRISIS				= 5140, 	/// 위기 탈출
		SI_GP_COMMON_INSTRUCTION_OF_WISEMAN			= 5150,		/// 현자의 가르침
//#endif GUILD_SKILL

//#ifdef ADDED_RELATIONSHIP_SYSTEM
		SI_ETC_WS_COMMON_LOVE						= 6001,		/// 커플 스킬 천상지애
//#endif // ADDED_RELATIONSHIP_SYSTEM
#endif //UPGRADE_SKILL_SYSTEM_2013

#pragma region 엘리시스
		SI_SA_EL_WILD_SHOCK							= 8000,		/// 와일드 쇼크
		SI_SA_EL_POWER_BURSTER						= 8001,		/// 파워 버스터
		SI_SA_EL_MEGA_BURSTER						= 8002,		/// 메가 버스터
		SI_SA_EL_UNLIMITED_BLADE					= 8003,		/// 언리미티드 블레이드

		SI_SA_SSK_SPIRAL_BLAST						= 8004,		/// 스파이럴 블래스트
		SI_SA_SSK_EXTINCTION						= 8005,		/// 세이버 - 익스팅션
		SI_SA_SSK_VICTORIOUS_SWORD					= 8006,		/// 승리의 검
		SI_SA_SSK_JUDGEMENT_DRIVE					= 8007,		/// 저지먼트 드라이브

		SI_SA_SPK_BURST_RISING						= 8008,		/// 버스트 라이징
		SI_SA_SPK_BURST_WAVE						= 8009,		/// 버스트 웨이브
		SI_SA_SPK_SWORDFIRE							= 8010,		/// 소드 파이어
		SI_SA_SPK_INFERNAL_BLADE					= 8011,		/// 인페르날 블레이드
		SI_SA_SPK_ETERNAL_FIRE						= 8012,		/// 이터널 파이어
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
		SI_SA_SGM_SONIC_BLADE						= 8013,		/// 소닉 블레이드
		SI_SA_SGM_CRITICAL_DIVE						= 8014,		/// 크리티컬 다이브
		SI_SA_SBH_SCALDIC_SWORD						= 8015,		/// 스칼딕 소드
		SI_SA_SBH_WALL_OF_PROMINENCE				= 8016,		/// 파이어 월
		SI_SA_SGM_JUGGERNAUT_BUSTER					= 8017,		/// 저거넛 버스터
		SI_SA_SGM_WAR_PRELUDE						= 8018,		/// 워 프렐류드
		SI_SA_SBH_BLAZE_WING						= 8019,		/// 블레이즈 윙	
		SI_SA_SBH_BLAZING_DANCE						= 8020,		/// 블레이징 댄스
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

		SI_A_EL_DODGE_AND_SLASH						= 8200,		/// 닷지&슬래시
		SI_A_EL_LEAP_CRASH							= 8201,		/// 도약
		SI_A_EL_RUSHING_SWORD						= 8202,		/// 러싱 소드
		SI_A_EL_KICK								= 8203,		/// 발차기
		SI_A_EL_MEGA_SLASH							= 8204,		/// 메가 슬래시
		SI_A_EL_SONIC_ASSAULT_STAB					= 8205,		/// 소닉 어설트 - 스탭
		SI_A_EL_SONIC_ASSAULT_STING					= 8206,		/// 소닉 어설트 - 스팅

		SI_A_EL_ENDURANCE_POWER						= 8207,		/// 극기 - 강

		SI_A_SSK_CROSS_SLASH						= 8208,		/// 크로스 슬래시
		SI_A_SSK_POWER_BREAK						= 8209,		/// 파워 브레이크
		SI_A_SSK_HEAVY_STUNNER						= 8210,		/// 헤비 스터너

		SI_A_SPK_SWORD_ERUPTION						= 8211,		/// 소드 이럽션
		SI_A_SPK_SCALDIC_SWORD						= 8212,		/// 불꽃의 검
		SI_A_SPK_BIG_BURST							= 8213,		/// 대폭살
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
		SI_A_SGM_PROVOKE							= 8214,		/// 도발
		SI_A_SGM_DRASTIC_CHARGE						= 8215,		/// 드래스틱 차지
		SI_A_SGM_SNAP_CRASHER						= 8216,		/// 스냅 크래셔
		SI_A_SBH_FLAME_SWORD						= 8217,		///	홍련의 검
		SI_A_SBH_FIREWORKS							= 8218,		/// 불놀이
		SI_A_SBH_SCALET_ROSE						= 8219,		/// 불꽃 장미
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

		SI_P_SSK_AGILITY_MASTERY					= 8400,		/// 순발력 향상
		SI_P_SSK_CHIVALRY							= 8401,		/// 기사의 숙명
		SI_P_SPK_CONCENTRATION_MASTERY				= 8402,		/// 집중력 향상
		SI_P_SPK_BODY_OF_FIRE						= 8403,		/// 불의 신체
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
		SI_P_SGM_ADVANCED_KNIGHT_MASTERY			= 8404,		/// 상급 기사 숙련
		SI_P_SGM_WALTZ_OF_SWORD						= 8405,		/// 검의 왈츠
		SI_P_SGM_FIRST_STRIKE						= 8406,		/// 선수필승
		SI_P_SBH_FIRE_BLOSSOMS						= 8407,		/// 불꽃 개화
		SI_P_SBH_UNEXTINGUISHABLE_FIRE				= 8408,		/// 꺼지지 않는 불꽃
		SI_P_SBH_STIGMA_OF_FIRE						= 8409,		/// 불꽃의 낙인	
		
		SI_HA_SGM_SWORD_OF_RELICS					= 8600,		/// 소드 오브 렐릭스
		SI_HA_SBH_ANCIENT_FIRE						= 8601,		/// 에인션트 파이어
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE	

#pragma endregion 스킬


#pragma region 애드
#ifdef SERV_9TH_NEW_CHARACTER // 김태환

		/// 하이퍼 액티브
		SI_SA_AN_VOID_BREAKER						= 9000,		/// 보이드 브레이커
		SI_SA_AN_PARTICLE_PRISM						= 9001,		/// 파티클 프리즘
		SI_SA_AN_ENERGY_BOOM						= 9002,		/// 에너지 붐
		SI_SA_AN_PARTICLE_ACCELERATOR				= 9003,		/// 파티클 액셀러레이터
		SI_SA_AN_NEUTRON_BOMB						= 9004,		/// 뉴트란 밤

		SI_SA_APT_PULSE_CANNON						= 9005,		/// 펄스 캐논
		SI_SA_APT_EMP_SHOCK							= 9006,		/// EMP 쇼크
		SI_SA_APT_QUAKE_BUSTER						= 9007,		/// 퀘이크 버스터
		SI_SA_APT_STASIS_FIELD						= 9008,		/// 스테이시스 필드
		SI_SA_APT_CONQUEROR							= 9009,		/// 퀀커러

	#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
		SI_SA_ALP_REVERSE_REACTOR					= 9010,		/// 리버스 리액터
		SI_SA_ALP_QUICKSILVER_ACCEL					= 9011,		/// 퀵실버 액셀
		SI_SA_ALP_QUICKSILVER_FRENZY				= 9012,		/// 퀵실버 프렌지
		SI_SA_ALP_DUST_ANATOMY						= 9013,		/// 더스트 아나토미
		SI_SA_ALP_PSYCHIC_STORM						= 9014,		/// 사이킥 스톰
	#endif //SERV_ADD_LUNATIC_PSYKER


		/// 액티브
		SI_A_AN_PULSE_BULLET						= 9200,		/// 입자탄
		SI_A_AN_MIND_BREAK							= 9201,		/// 정신 붕괴
		SI_A_AN_PHASE_SHIFT							= 9202,		/// 위상 변화

		SI_A_APT_MAGNETRON_DYNAMO					= 9203,		/// 다이너모 구성 - 마그네트론
		SI_A_APT_DESOLVER_DYNAMO					= 9204,		/// 다이너모 구성 - 디졸버
		SI_A_APT_PYLON_DYNAMO						= 9205,		/// 다이너모 구성 - 파일런

	#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
		SI_A_ALP_PSIONIC_BEAT						= 9206,		/// 사이오닉 비트
		SI_A_ALP_TWIRL_RUSH							= 9207,		/// 트월 러시
	#endif //SERV_ADD_LUNATIC_PSYKER


		/// 패시브
		SI_P_AN_CHARGED_IMPULSAR					= 9400,		/// 다이너모 구성 - 과충전기
		SI_P_AN_RESEARCH_DYNAMO_DISSOLUTION			= 9401,		/// 다이너모 연구 - 해체
		SI_P_AN_RESEARCH_DYNAMO_COMPOSITION			= 9402,		/// 다이너모 연구 - 구성
		SI_P_APT_NASOD_ARMOR_MODE					= 9403,		/// 나소드 아머 모드

		SI_P_APT_LIBRARY_OF_LIMITLESS				= 9404,		/// 무한의 도서관
		SI_P_APT_DUST_STORM							= 9405,		/// 미립자 폭풍

	#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
		SI_P_ALP_BODY_OF_TRANSCENDENCE				= 9406,		/// 초월자의 신체
		SI_P_ALP_POWER_EXCHANGER					= 9407,		/// 출력 교환기
		SI_P_ALP_FANTASY_TRACER						= 9408,		/// 환상 추적자
	#endif //SERV_ADD_LUNATIC_PSYKER


		/// 하이퍼 액티브 스킬
	#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
		SI_HA_ALP_DOOMS_DAY							= 9600,		/// 둠스 데이
	#endif //SERV_ADD_LUNATIC_PSYKER

#endif //SERV_9TH_NEW_CHARACTER
//{{ Iruha : 2026-10-26 // June enum names Phase N2 (JUNE_ENUM_NAMES_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names a June-port site spells (values from June's Enum.lua)
		SI_SA_SDK_BRUTAL_CUTTER                               = 8021,
		SI_SA_SCA_CRIMSON_BREAK                               = 8025,
		SI_SA_SCA_DREAD_WEAPON                                = 8026,
		RNSI_SA_AISHA_CHAIN_FIREBALL                          = 22000,
		RNSI_SA_AISHA_GUST_SCREW                              = 22001,
		RNSI_SA_AISHA_GUST_STORM                              = 22002,
		RNSI_SA_AISHA_LIGHTNING_BOLT                          = 22003,
		RNSI_SA_AISHA_METEOR_CALL                             = 22004,
		RNSI_SA_AISHA_BINDING_CIRCLE                          = 22005,
		RNSI_SA_AHM_MAGIC_MISSILE                             = 22006,
		RNSI_SA_AHM_METEOR_SHOWER                             = 22007,
		RNSI_SA_AHM_BLIZZARD_SHOWER                           = 22008,
		RNSI_SA_AHM_CHAIN_LIGHTNING                           = 22009,
		RNSI_SA_AEM_BLAZE_STEP                                = 22010,
		RNSI_SA_AEM_CYCLONE                                   = 22011,
		RNSI_SA_AEM_LIGHTNING_SHOWER                          = 22012,
		RNSI_SA_AEM_CHAIN_BURST                               = 22013,
		RNSI_SA_ADM_DARK_CLOUD                                = 22014,
		RNSI_SA_ADM_AGING                                     = 22015,
		RNSI_SA_ADM_DEATH_FIELD                               = 22016,
		RNSI_SA_ADM_HELL_STONE                                = 22017,
		RNSI_SA_AVP_PLASMA_CUTTER                             = 22018,
		RNSI_SA_AVP_PHANTOM_BREATHING                         = 22019,
		RNSI_SA_AVP_HELL_DROP                                 = 22020,
		RNSI_SA_AVP_DARK_BURST                                = 22021,
		RNSI_SA_ABM_MAGICAL_MAKEUP                            = 22022,
		RNSI_SA_ABM_SUPER_NOVA                                = 22023,
		RNSI_SA_ABM_GUILLOTINE_PRESS                          = 22024,
		RNSI_SA_ABM_ENERGY_SPURT                              = 22025,
		RNSI_SA_ADW_HEAVY_PRESS                               = 22026,
		RNSI_SA_ADW_DISTORTION                                = 22027,
		RNSI_SA_ADW_SCREW_DRIVER                              = 22028,
		RNSI_SA_ADW_IMPACT_HAMMER                             = 22029,
		RNSI_SA_AISHA_CHAIN_FIREBALL_UPGRADE                  = 22030,
		RNSI_SA_AISHA_GUST_SCREW_UPGRADE                      = 22031,
		RNSI_SA_AISHA_GUST_STORM_UPGRADE                      = 22032,
		RNSI_SA_AISHA_LIGHTNING_BOLT_UPGRADE                  = 22033,
		RNSI_SA_AISHA_METEOR_CALL_UPGRADE                     = 22034,
		RNSI_SA_AISHA_BINDING_CIRCLE_UPGRADE                  = 22035,
		RNSI_SA_AVP_HELL_DROP_ADD_TRANS                       = 22036,
		RNSI_SA_AHM_MAGIC_MISSILE_UPGRADE                     = 22038,
		RNSI_SA_ADM_DARK_CLOUD_UPGRADE                        = 22039,
		RNSI_SA_ABM_MAGICAL_MAKEUP_UPGRADE                    = 22040,
		RNSI_SA_AEM_CIRCLE_ELEMENTAL                          = 22041,
		RNSI_SA_AEM_ELEMENTAL_EDGE                            = 22042,
		RNSI_SA_AVP_SUMMON_BAT_MILLIONS                       = 22043,
		RNSI_SA_AVP_RUIN                                      = 22044,
		RNSI_SA_ADW_AIR_STUNNER                               = 22045,
		RNSI_SA_ADW_CRAZY_WORM                                = 22046,
		RNSI_SA_AEM_BLAZE_STEP_UPGRADE                        = 22050,
		RNSI_SA_AEM_CYCLONE_UPGRADE                           = 22051,
		RNSI_SA_AEM_LIGHTNING_SHOWER_UPGRADE                  = 22052,
		RNSI_SA_AEM_CHAIN_BURST_UPGRADE                       = 22053,
		RNSI_SA_AVP_PLASMA_CUTTER_UPGRADE                     = 22057,
		RNSI_SA_AVP_PHANTOM_BREATHING_UPGRADE                 = 22058,
		RNSI_SA_AVP_HELL_DROP_UPGRADE                         = 22059,
		RNSI_SA_AVP_DARK_BURST_UPGRADE                        = 22060,
		RNSI_SA_ABM_ENERGY_SPURT_UPGRADE                      = 22063,
		RNSI_SA_ADW_DISTORTION_UPGRADE                        = 22065,
		RNSI_SA_ADW_SCREW_DRIVER_UPGRADE                      = 22066,
		RNSI_SA_ADW_IMPACT_HAMMER_UPGRADE                     = 22067,
		RNSI_SA_AEM_ICICLE_SPEAR                              = 22068,
		RNSI_SA_AVP_ABYSS_FIELD                               = 22069,
		RNSI_SA_ADW_IMPACT_ZONE                               = 22070,
		RNSI_A_AISHA_TELEPORT                                 = 22200,
		RNSI_A_AHM_FIRE_ROAD                                  = 22201,
		RNSI_A_AHM_AURA_MAGIC_ACCEL                           = 22202,
		RNSI_A_AHM_MEDITATION                                 = 22203,
		RNSI_A_AEM_CIRCLE_FLAME                               = 22204,
		RNSI_A_AEM_ELEMENTAL_BODY                             = 22205,
		RNSI_A_ADM_DARK_ROAD                                  = 22206,
		RNSI_A_ADM_AURA_SPEED_ACCEL                           = 22207,
		RNSI_A_ADM_MANA_INTAKE                                = 22208,
		RNSI_A_AVP_SUMMON_BAT                                 = 22209,
		RNSI_A_AVP_SHADOW_BODY                                = 22210,
		RNSI_A_ABM_ENERGY_ROAD                                = 22211,
		RNSI_A_ABM_AURA_POWER_ACCEL                           = 22212,
		RNSI_A_ABM_ENERGY_DRAIN                               = 22213,
		RNSI_A_ADW_WORM_HOLE                                  = 22214,
		RNSI_A_ADW_ENERGETIC_BODY                             = 22215,
		RNSI_A_AISHA_TELEPORT_UPGRADE                         = 22216,
		RNSI_A_AVP_SUMMON_BAT_ADD_TRANS                       = 22217,
		RNSI_A_AHM_FIRE_ROAD_UPGRADE                          = 22218,
		RNSI_A_ADM_DARK_ROAD_UPGRADE                          = 22219,
		RNSI_A_ABM_ENERGY_ROAD_UPGRADE                        = 22220,
		RNSI_A_AHM_AURA_MAGIC_ACCEL_UPGRADE                   = 22221,
		RNSI_A_ADM_AURA_SPEED_ACCEL_UPGRADE                   = 22222,
		RNSI_A_ABM_AURA_POWER_ACCEL_UPGRADE                   = 22223,
		RNSI_A_AEM_FIRE_RING                                  = 22224,
		RNSI_A_AVP_DARK_RING                                  = 22225,
		RNSI_A_ADW_ENERGY_RING                                = 22226,
		RNSI_A_AEM_CIRCLE_FLAME_UPGRADE                       = 22228,
		RNSI_A_AVP_SUMMON_BAT_UPGRADE                         = 22231,
		RNSI_A_ADW_WORM_HOLE_UPGRADE                          = 22235,
		RNSI_P_AHM_FIREBALL_UP                                = 22400,
		RNSI_P_AEM_UNLIMIT_MANA                               = 22401,
		RNSI_P_AEM_ELEMENTAL_FRIENDSHIP                       = 22402,
		RNSI_P_AEM_ELEMENTAL_RESEARCH                         = 22403,
		RNSI_P_ADM_GRAVITYBALL_RESEARCH                       = 22404,
		RNSI_P_AVP_STRONG_MIND                                = 22405,
		RNSI_P_AVP_SHADOW_DATH                                = 22406,
		RNSI_P_AVP_LIMITED_MANA_MANAGEMENT                    = 22407,
		RNSI_P_ADW_MAGICAL_STAFF                              = 22409,
		RNSI_P_ADW_MANAFLOW_OVERDRIVE                         = 22410,
		RNSI_P_ADW_ADVANCED_TELEPORTATION                     = 22411,
		RNSI_P_AEM_OPEN_MIND                                  = 22412,
		RNSI_P_AEM_MAGICAL_BODY                               = 22413,
		RNSI_P_AVP_OPEN_MIND                                  = 22414,
		RNSI_P_AVP_DARK_DEAL                                  = 22415,
		RNSI_P_ADW_OPEN_MIND                                  = 22416,
		RNSI_P_ADW_TIME_LIMIT                                 = 22417,
		RNSI_HA_AEM_ELEMENTAL_STORM                           = 22600,
		RNSI_HA_AVP_ABYSS_ANGOR                               = 22601,
		RNSI_HA_ADW_FATE_SPACE                                = 22602,
		RNSI_HA_AEM_ELEMENTAL_STORM_UPGRADE                   = 22603,
		RNSI_HA_AVP_ABYSS_ANGOR_UPGRADE                       = 22604,
		RNSI_HA_ADW_FATE_SPACE_UPGRADE                        = 22605,
		RNSI_SA_RGA_GUNGNIR                                   = 23015,
		RNSI_A_RGA_ENTANGLE                                   = 23213,
		RNSI_A_RTR_ANGER_OF_ELF                               = 23216,
		RNSI_A_RTR_EVOKE                                      = 23218,
		RNSI_SA_EVE_DIMENSION_LINK_SENTRY                     = 25000,
		RNSI_SA_EEG_QUEENS_THRONE                             = 25003,
		RNSI_SA_ECN_ATOMIC_SHIELD                             = 25011,
		RNSI_SA_EAT_OBERON_GUARD                              = 25014,
		RNSI_SA_EAT_SURFACE_CUTING                            = 25015,
		RNSI_SA_ECE_ELECTRONIC_FIELD                          = 25020,
		RNSI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE                 = 25021,
		RNSI_SA_EEL_KUGELBLITZ                                = 25024,
		RNSI_SA_ECB_THOUSANDS_OF_STARS                        = 25028,
		RNSI_SA_ECB_ENERGETIC_HEART                           = 25029,
		RNSI_SA_EVE_DIMENSION_LINK_SENTRY_UPGRADE             = 25030,
		RNSI_SA_ECN_ATOMIC_SHIELD_UPGRADE                     = 25055,
		RNSI_SA_EAT_OBERON_GUARD_UPGRADE                      = 25056,
		RNSI_SA_EAT_SURFACE_CUTING_UPGRADE                    = 25057,
		RNSI_SA_ECE_ELECTRONIC_FIELD_UPGRADE                  = 25062,
		RNSI_A_COMMON_AURA_ADDATK_ACCEL                       = 25201,
		RNSI_A_EVE_SPECTRO_EL_CRYSTAL                         = 25202,
		RNSI_A_ECN_SPEAR_BURST                                = 25206,
		RNSI_A_EAT_MEGA_ELECTRONBALL                          = 25207,
		RNSI_A_ECB_TASER_PILUM                                = 25212,
		RNSI_A_COMMON_AURA_ADDATK_ACCEL_UPGRADE               = 25214,
		RNSI_A_EVE_SPECTRO_EL_CRYSTAL_UPGRADE                 = 25215,
		RNSI_A_EBS_ELECTRONIC_MOVE                            = 25223,
		RNSI_SA_ECN_PSYKER_KINESIS                            = 25235,
		RNSI_SA_ECE_REASON_PRIDE                              = 25236,
		RNSI_P_ECN_NASOD_WEAPON                               = 25401,
		RNSI_P_ECN_ENERGY_OF_CONCENTRATION                    = 25402,
		RNSI_P_ECE_GRACE_OF_QUEEN                             = 25408,
		RNSI_P_ENS_OPEN_MIND                                  = 25415,
		RNSI_P_ENS_CHEAT_CODE_DESTROY                         = 25416,
		RNSI_P_EBS_OPEN_MIND                                  = 25419,
		RNSI_HA_ECE_LINK_OVERCHARGE_ILLUSION                  = 25601,
		RNSI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA         = 26014,
		RNSI_SA_CTT_ARTILLERY_STRIKE_MISSILE_SHOWER           = 26025,
		RNSI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA_UPGRADE = 26050,
		RNSI_SA_CTT_ARTILLERY_STRIKE_MISSILE_SHOWER_UPGRADE   = 26060,
		RNSI_SA_CTT_HEAVY_FIELD                               = 26064,
		RNSI_A_CTT_AUTOMATIC_MORTAR                           = 26215,
		RNSI_A_CTT_AUTOMATIC_MORTAR_UPGRADE                   = 26238,
		RNSI_SA_SBH_WALL_OF_PROMINENCE                        = 28018,
		RNSI_SA_SCA_BLOOD_CUTTER                              = 28023,
		RNSI_SA_SBH_WALL_OF_PROMINENCE_UPGRADE                = 28054,
		RNSI_SA_SCA_BLOOD_CUTTER_UPGRADE                      = 28057,
		RNSI_SA_AN_PARTICLE_PRISM                             = 29003,
		RNSI_SA_AN_PARTICLE_PRISM_UPGRADE                     = 29029,
		RNSI_SA_ADE_MOONLIGHT_RHAPSODY                        = 29024,
		RNSI_HA_AMM_APOCALYPSE                                = 29601,
		RNSI_SA_LLD_SUMMON_DARKNESS                           = 30013,
		RNSI_SA_LCN_SHADOW_BOLT                               = 30016,
		RNSI_SA_LDN_SINISTER_MIND                             = 30048,
		RNSI_SA_LDN_GARDEN_OF_HAURES                          = 30053,
		RNSI_A_LUCIEL_AURA_PUNISHMENT_ACCEL                   = 30301,
		RNSI_A_LUCIEL_AURA_PUNISHMENT_ACCEL_UPGRADE           = 30302,
		RNSI_BUFF_PHANTOM_CIRCLE                              = 30303,
		RNSI_SA_ROSE_RX78_LAND_RUNNER                         = 31002,
		RNSI_SA_ROSE_RX78_LAND_RUNNER_UPGRADE                 = 31007,
		RNSI_SA_RHB_PROTON_BOMB                               = 31015,
		RNSI_SA_RFY_NEIL_THE_SNIPER                           = 31038,
		RNSI_SA_RMH_GALE_FORCE                                = 31044,
		RNSI_SA_RMH_EZ_8_COUNT_DOWN                           = 31045,
		RNSI_SA_RMH_SPARROW_FACTORY                           = 31047,
		RNSI_SA_RMH_GALE_FORCE_UPGRADE                        = 31053,
		RNSI_SA_RMH_EZ_8_COUNT_DOWN_UPGRADE                   = 31054,
		RNSI_SA_RSR_PHOTON_SHOWER                             = 31055,
		RNSI_SA_RFY_M18A1_CLAYMORE                            = 31059,
		RNSI_SA_RHB_PROTON_BOMB_UPGRADE                       = 31065,
		RNSI_SA_RFY_NEIL_THE_SNIPER_UPGRADE                   = 31082,
		RNSI_SA_RMH_SPARROW_FACTORY_UPGRADE                   = 31087,
		RNSI_SA_ROM_ARMORED_TEMPEST                           = 31096,
		RNSI_A_RMH_ALL_BOMB                                   = 31229,
		RNSI_A_ROM_G_EX_MAGNETICFORCE                         = 31232,
		RNSI_A_RMH_ALL_BOMB_UPGRADE                           = 31234,
		RNSI_A_ROM_G_EX_MAGNETICFORCE_UPGRADE                 = 31320,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-26 // June character port Phase CH1l3 (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Eve skill use spells (values from June's Enum.lua)
		RNSI_A_EVE_PHOTON_BLINK                               = 25200,
		RNSI_A_EVE_CONVERSION                                 = 25203,
		RNSI_A_EVE_PHOTON_BLINK_UPGRADE                       = 25213,
		RNSI_A_EVE_CONVERSION_UPGRADE                         = 25216,
		RNSI_A_EAT_MEGA_ELECTRONBALL_UPGRADE                  = 25219,
		RNSI_A_ECB_TASER_PILUM_UPGRADE                        = 25233,
		RNSI_SA_ECB_THOUSANDS_OF_STARS_UPGRADE                = 25068,
		RNSI_P_ECE_CHARGING_BOOSTER                           = 25409,
		RNSI_P_EBS_CHEAT_CODE_ELECTRONIC                      = 25420,
		RNSI_HA_ECE_LINK_OVERCHARGE_ILLUSION_UPGRADE          = 25604,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-26 // June character port Phase CH1l4 (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's CommonFrameMove helpers spell (values from June's Enum.lua)
		RNSI_P_ECB_HYPER_OPTICAL_RESEARCH                     = 25411,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-27 // June character port Phase CH1n (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's skill-slot head spells (values from June's Enum.lua)
		RNSI_SA_EEG_HORNET_STING_EXPLOSION_ADD_TRANS          = 25033,
		RNSI_SA_ECB_ENERGY_NEEDLES_ADD_TRANS                  = 25034,
		RNSI_SA_ECB_THOUSANDS_OF_STARS_ADD_TRANS_1            = 25035,
		RNSI_SA_ECB_THOUSANDS_OF_STARS_ADD_TRANS_2            = 25036,
		RNSI_SA_EEG_HORNET_STING_EXPLOSION_ADD_TRANS_UPGRADE  = 25070,
		RNSI_SA_ECB_ENERGY_NEEDLES_ADD_TRANS_UPGRADE          = 25071,
		RNSI_SA_ECB_THOUSANDS_OF_STARS_ADD_TRANS_1_UPGRADE    = 25072,
		RNSI_SA_ECB_THOUSANDS_OF_STARS_ADD_TRANS_2_UPGRADE    = 25073,
		RNSI_A_ECE_SPIT_FIRE_FLUSH_ADD_TRANS                  = 25217,
		RNSI_A_ECE_SPIT_FIRE_FLUSH_ADD_TRANS_UPGRADE          = 25234,
		RNSI_HA_ECN_LUNATIC_SCUD                              = 25600,
		RNSI_HA_ECB_PSYCHIC_ARTILLERRY                        = 25602,
		RNSI_HA_ECN_LUNATIC_SCUD_UPGRADE                      = 25603,
		RNSI_HA_ECB_PSYCHIC_ARTILLERRY_UPGRADE                = 25605,
		RNSI_A_EEL_ILLUSION_RAY                               = 25210,
		RNSI_A_EEL_ILLUSION_RAY_UPGRADE                       = 25220,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-27 // June character port Phase CH2a (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Nasod skill cases spell (values from June's Enum.lua)
		RNSI_SA_EVE_ILLUSION_STINGER                          = 25001,
		RNSI_SA_EVE_DIMENSION_LINK_GUARDIAN                   = 25002,
		RNSI_SA_EVE_ILLUSION_STINGER_UPGRADE                  = 25031,
		RNSI_SA_EVE_DIMENSION_LINK_GUARDIAN_UPGRADE           = 25032,
		RNSI_P_EEP_CHEAT_CODE_RULE_BREAK                      = 25418,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-27 // June character port Phase CH2b (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Architecture skill cases spell (values from June's Enum.lua)
		RNSI_SA_EAT_HEAVENS_FIST_PRESSURE                     = 25012,
		RNSI_SA_EAT_SONIC_WAVE                                = 25013,
		RNSI_SA_EAT_HEAVENS_FIST_SWEEPER                      = 25016,
		RNSI_SA_EAT_HEAVENS_FIST_PRESSURE_UPGRADE             = 25038,
		RNSI_SA_EAT_SONIC_WAVE_UPGRADE                        = 25041,
		RNSI_SA_EAT_HEAVENS_FIST_SWEEPER_UPGRADE              = 25058,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-27 // June character port Phase CH2c (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Electra skill cases spell (values from June's Enum.lua)
		RNSI_SA_EEL_PHOTON_FLARE                              = 25022,
		RNSI_SA_EEL_PARTICLE_RAY                              = 25023,
		RNSI_SA_EEL_PHOTON_FLARE_UPGRADE                      = 25042,
		RNSI_SA_EEL_PARTICLE_RAY_UPGRADE                      = 25063,
		RNSI_SA_EEL_KUGELBLITZ_UPGRADE                        = 25064,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-28 // June character port Phase CH2c2 (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Sweep Rolling and Giga Stream cases spell (values from June's Enum.lua)
		RNSI_SA_EEL_GIGA_STREAM                               = 25025,
		RNSI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE_UPGRADE         = 25039,
		RNSI_SA_EEL_GIGA_STREAM_UPGRADE                       = 25065,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-28 // June character port Phase CH3a (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Code: Nemesis skill cases spell (values from June's Enum.lua)
		RNSI_SA_ECN_JUNK_BREAK                                = 25008,
		RNSI_SA_ECN_ASSAULT_SPEAR_ANNIHILATOR                 = 25009,
		RNSI_SA_ECN_IRONSCRAPS                                = 25010,
		RNSI_SA_ENS_ILLUSIONIC_THORN                          = 25043,
		RNSI_SA_ENS_EXTREMING_CUTTING                         = 25044,
		RNSI_SA_ECN_JUNK_BREAK_UPGRADE                        = 25052,
		RNSI_SA_ECN_ASSAULT_SPEAR_ANNIHILATOR_UPGRADE         = 25053,
		RNSI_SA_ECN_IRONSCRAPS_UPGRADE                        = 25054,
		RNSI_A_ECN_TESLA_SHOCK                                = 25205,
		RNSI_A_ENS_SHOT_GUN                                   = 25221,
		RNSI_A_ECN_TESLA_SHOCK_UPGRADE                        = 25227,
		RNSI_A_ECN_SPEAR_BURST_UPGRADE                        = 25228,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-28 // June character port Phase CH3b (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Code: Empress skill cases spell (values from June's Enum.lua)
		RNSI_SA_ECE_GENOCIDE_RIPPER                           = 25017,
		RNSI_SA_ECE_ASSUALT_SPEAR_BURST                       = 25018,
		RNSI_SA_ECE_SPACE_WRENCH                              = 25019,
		RNSI_SA_EEP_TESLA_FLARE                               = 25045,
		RNSI_SA_EEP_ELECTRONIC_THUNDER                        = 25046,
		RNSI_SA_ECE_GENOCIDE_RIPPER_UPGRADE                   = 25059,
		RNSI_SA_ECE_ASSUALT_SPEAR_BURST_UPGRADE               = 25060,
		RNSI_SA_ECE_SPACE_WRENCH_UPGRADE                      = 25061,
		RNSI_A_ECE_TESLA_SHOCK                                = 25208,
		RNSI_A_ECE_SPIT_FIRE_FLUSH                            = 25209,
		RNSI_A_EEP_SUMMON_TO_MIND                             = 25222,
		RNSI_A_ECE_TESLA_SHOCK_UPGRADE                        = 25229,
		RNSI_A_ECE_SPIT_FIRE_FLUSH_UPGRADE                    = 25230,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-28 // June character port Phase CH3c (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Code: Battle Seraph skill cases spell (values from June's Enum.lua)
		RNSI_SA_ECB_LINEAR_DIVIDER                            = 25026,
		RNSI_SA_ECB_ENERGY_NEEDLES                            = 25027,
		RNSI_SA_EBS_NEEDLES_CUTTING                           = 25047,
		RNSI_SA_EBS_LASER_POLE                                = 25048,
		RNSI_SA_ECB_LINEAR_DIVIDER_UPGRADE                    = 25066,
		RNSI_SA_ECB_ENERGY_NEEDLES_UPGRADE                    = 25067,
		RNSI_SA_ECB_ENERGETIC_HEART_UPGRADE                   = 25069,
		RNSI_A_ECB_TESLA_SHOCK                                = 25211,
		RNSI_A_ECB_TESLA_SHOCK_UPGRADE                        = 25232,
		RNSI_SA_ECB_EXTREME_VIRTUAL                           = 25237,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-27 // June character port Phase CH2a2 (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's Exotic Gear skill cases spell (values from June's Enum.lua)
		RNSI_SA_EEG_EXPLOSION_IMPACT                          = 25004,
		RNSI_SA_EEG_ATOMIC_BLASTER_PLASMAGUN                  = 25005,
		RNSI_SA_EEG_HORNET_STING_EXPLOSION                    = 25006,
		RNSI_SA_EEG_GENERATE_BLACKHOLE                        = 25007,
		RNSI_SA_EEG_QUEENS_THRONE_UPGRADE                     = 25037,
		RNSI_SA_EEG_EXPLOSION_IMPACT_UPGRADE                  = 25040,
		RNSI_SA_EEG_ATOMIC_BLASTER_PLASMAGUN_UPGRADE          = 25049,
		RNSI_SA_EEG_HORNET_STING_EXPLOSION_UPGRADE            = 25050,
		RNSI_SA_EEG_GENERATE_BLACKHOLE_UPGRADE                = 25051,
		RNSI_A_EEG_METAL_DUST_AURA                            = 25204,
		RNSI_A_EEG_METAL_DUST_AURA_UPGRADE                    = 25218,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-28 // June character port Phase CH4a (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Eve's InitPassiveSkillState spells (values from June's Enum.lua)
		RNSI_P_EEG_REFINED_STEP                               = 25400,
		RNSI_P_ECN_QUEEN_OF_VENGEANCE                         = 25403,
		RNSI_P_ECN_QUEEN_OF_DESTROY                           = 25404,
		RNSI_P_EAT_QUEENS_POTENTIAL                           = 25405,
		RNSI_P_ECE_QUEENS_CONTROL                             = 25406,
		RNSI_P_ECE_ELECTRON_BOOSTER                           = 25407,
		RNSI_P_EEL_SPECTRUM_DYNAMICS                          = 25410,
		RNSI_P_ECB_PHOTON_BOOSTER                             = 25412,
		RNSI_P_ECB_HIGHPOWERED_ELECTRICS                      = 25413,
		RNSI_P_ECB_ADVANCED_EL_ENERGY_REACTOR                 = 25414,
		RNSI_P_EEP_OPEN_MIND                                  = 25417,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-30 // june-charinfo C3
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the character window's basic-stat skill bonuses read ( values from June's Enum.lua )
		RNSI_P_COMMON_HYPER_AURA                              = 106,
		RNSI_P_EMK_IF_PHYSICUP_MAGIC_GET                      = 21405,
		RNSI_P_ERS_RUNE_MASTERY                               = 21408,
		RNSI_P_ERS_MAGIC_RESISTANCE                           = 21409,
		RNSI_P_ABM_FITNESS                                    = 22408,
		RNSI_P_RF_PHYSIC_MAGIC_ATTACK                         = 24400,
		RNSI_P_RRF_SPIRITUALIZED_FURY                         = 24405,
		RNSI_P_CIP_IRON_WILL                                  = 26402,
		RNSI_P_CIP_OPEN_MIND                                  = 26415,
		RNSI_P_CDC_OPEN_MIND                                  = 26417,
		RNSI_P_RSR_OPEN_MIND                                  = 31417,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-31 // char-chung CH1g2
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Shooting Guardian Head Shot state reads ( values from June's Enum.lua )
		RNSI_A_CSG_HEAD_SHOT                                  = 26208,
		RNSI_A_CSG_HEAD_SHOT_TRANS                            = 26220,
		RNSI_A_CSG_HEAD_SHOT_TRANS_UPGRADE                    = 26225,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-01 // char-chung CH1j
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID name the Tactical Trooper mobile states read ( value from June's Enum.lua )
		RNSI_P_CTT_OPEN_MIND                                  = 26419,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-01 // char-chung CH1k
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Chung class core reads ( values from June's Enum.lua )
		RNSI_SA_CDC_EXCITING_CRASH                            = 26040,
		RNSI_P_CFG_METABOLISM_BOOST                           = 26400,
		RNSI_P_CTT_REACTIVE_ARMOR                             = 26414,
		RNSI_P_CTT_USEFUL_TACTIC                              = 26420,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-01 // char-chung CH1l
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Chung SetEquippedSkillLevel cases spell ( values from June's Enum.lua )
		RNSI_SA_CC_LUNATIC_BLOW                               = 26001,
		RNSI_SA_CFG_LUNATIC_FURY                              = 26006,
		RNSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE                = 26007,
		RNSI_SA_CIP_IRON_HOWLING                              = 26008,
		RNSI_SA_CSG_DUAL_BUSTER                               = 26011,
		RNSI_SA_CSG_SHOOTING_STAR                             = 26015,
		RNSI_SA_CDC_COMET_CRASHER                             = 26017,
		RNSI_SA_CTT_DREAD_CHASE                               = 26020,
		RNSI_SA_CC_IMPACT_DETONATION                          = 26021,
		RNSI_SA_CHG_CHAOS_CANNON                              = 26022,
		RNSI_A_CC_SIEGE_SHELLING                              = 26201,
		RNSI_A_CFG_LEAP_ATTACK                                = 26204,
		RNSI_A_CIP_ARTILLERY_NOVA                             = 26206,
		RNSI_A_CFG_GUARD                                      = 26207,
		RNSI_A_CDC_GATLING_SHOT                               = 26211,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-01 // char-chung CH1l2
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Chung CommonStateStart switch spells ( values from June's Enum.lua )
		RNSI_SA_CDC_DEADLY_SHOT                               = 26013,
		RNSI_SA_CDC_BULLET_BLITZ                              = 26018,
		RNSI_SA_CSG_DUAL_BUSTER_UPGRADE                       = 26032,
		RNSI_SA_CDC_SHOOTING_BIND                             = 26039,
		RNSI_SA_CDC_BURNING_PUNISHER                          = 26063,
		RNSI_A_CDC_TRICK_SHOT                                 = 26209,
		RNSI_A_CSG_HEAD_SHOT_UPGRADE                          = 26223,
		RNSI_A_CDC_STUN_SHOT                                  = 26227,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-01 // char-chung CH1l3
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Chung skill-use functions spell ( values from June's Enum.lua )
		RNSI_SA_CTT_DREAD_CHASE_UPGRADE                       = 26036,
		RNSI_SA_CHG_CANNON_STRIKE_UPGRADE                     = 26061,
		RNSI_A_CC_BACK_BLAST                                  = 26200,
		RNSI_A_CC_AIMMING_SHOT                                = 26203,
		RNSI_A_CTT_FATAL_CANNON                               = 26228,
		RNSI_A_CFG_GUARD_UPGRADE                              = 26231,
		RNSI_A_CDC_RUMBLE_SHOT_UPGRADE                        = 26233,
		RNSI_A_CDC_GATLING_SHOT_TRANS_UPGRADE                 = 26235,
		RNSI_P_CDC_INTENSE_SHOWTIME                           = 26407,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-02 // char-chung CH2a
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Iron Cannon skill bodies spell ( values from June's Enum.lua )
		RNSI_SA_CC_SCARE_CHASE                                = 26000,
		RNSI_SA_CC_ACELDAMA                                   = 26002,
		RNSI_A_CC_RELOAD_CANNON                               = 26202,
		RNSI_P_CDC_ENDLESS_CHASE                              = 26418,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-02 // char-chung CH2b
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names the Fury Guardian / Iron Paladin skill bodies spell ( values from June's Enum.lua )
		RNSI_SA_CIP_BURST_WOLF                                = 26003,
		RNSI_SA_CFG_PANDEMONIUM_CHAOS                         = 26004,
		RNSI_SA_CC_BRUTAL_SWING                               = 26005,
		RNSI_SA_CIP_BURST_WOLF_UPGRADE                        = 26031,
		RNSI_SA_CFG_PANDEMONIUM_CHAOS_UPGRADE                 = 26034,
		RNSI_SA_CC_BRUTAL_SWING_UPGRADE                       = 26043,
		RNSI_SA_CFG_LUNATIC_FURY_UPGRADE                      = 26044,
		RNSI_A_CIP_SUDDEN_BURSTER                             = 26205,
		RNSI_A_CFG_LEAP_ATTACK_UPGRADE                        = 26222,
		RNSI_A_CIP_SUDDEN_BURSTER_UPGRADE                     = 26229,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-01 // char-chung CH1l4
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID name Chung's Carpet Bombing shot spells ( value from June's Enum.lua )
		RNSI_SA_CHG_CARPET_BOMBING                            = 26023,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-02 // char-chung CH1n
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names Chung's skill-slot head spells ( values from June's Enum.lua )
		RNSI_SA_CSG_SHOOTING_STAR_TRANS                       = 26030,
		RNSI_SA_CSG_SHOOTING_STAR_TRANS_UPGRADE               = 26052,
		RNSI_A_CDC_GATLING_SHOT_TRANS                         = 26221,
		RNSI_HA_CIP_DOOM_STRIKE                               = 26600,
		RNSI_HA_CDC_OUTRAGE_STRIKE                            = 26601,
		RNSI_HA_CTT_SATELLITE_RAIN                            = 26602,
		RNSI_HA_CIP_DOOM_STRIKE_UPGRADE                       = 26603,
		RNSI_HA_CDC_OUTRAGE_STRIKE_UPGRADE                    = 26604,
		RNSI_HA_CTT_SATELLITE_RAIN_UPGRADE                    = 26605,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-02 // char-chung CH1n2
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID name HyperModeStart spells ( value from June's Enum.lua )
		RNSI_P_RRF_OPEN_MIND                                  = 24419,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-11-02 // char-chung CH1o
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID name InitRemodelMagazinePassiveSkill spells ( value from June's Enum.lua )
		RNSI_P_CSG_REMODEL_MAGAZINE                           = 26406,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
//{{ Iruha : 2026-10-29 // june-march-divergence D1-2
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
		// June SKILL_ID names CheckSummonSpecialAttack's Add summon skills spell (values from June's Enum.lua)
		RNSI_HA_AMM_APOCALYPSE_UPGRADE                        = 29604,
		RNSI_HA_ROM_MECAVOLT_MX                               = 31607,
		RNSI_HA_ROM_MECAVOLT_MX_UPGRADE                       = 31608,
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
#pragma endregion 스킬
	}; 



	enum SKILL_ABILITY_TYPE
	{
		SA_INVALID,															
		SA_EFFECTIVE_TIME,					/// 유지 시간												
		SA_EFFECTIVE_COUNT,					/// 유지 횟수
		SA_SKILL_ID,						/// 특정 스킬 지정
		SA_COMBO_ID,						/// 특정 콤보 지정
		SA_EFFECTIVE_RATE,					/// 발동 확률
		SA_ATK_REL,							/// 공격력(물리,마법) 스탯 증가
		SA_ATK_PHYSIC_REL,					/// 물리 공격력 스탯 증가
		SA_ATK_MAGIC_REL,					/// 마법 공격력 스탯 증가
		SA_DEF_PHYSIC_REL,					/// 물리 방어력 스탯 증가
		SA_DEF_MAGIC_REL,					/// 마법 방어력 스탯 증가
		SA_TEAM_ATK_PHYSIC_REL,				/// 물리 공격력 스탯 증가 (팀)
		SA_TEAM_ATK_MAGIC_REL,				/// 마법 공격력 스탯 증가 (팀)
		SA_TEAM_DEF_PHYSIC_REL,				/// 물리 방어력 스탯 증가 (팀)
		SA_TEAM_DEF_MAGIC_REL,				/// 마법 방어력 스탯 증가 (팀)
		SA_DAMAGE_REL,						/// 데미지 배율 변경
		SA_DAMAGED_REL,						/// 받는 데미지 변경
		SA_UNFIXED_DEFENSE_REL,				/// 방어도 무시
		SA_SUMMON_ATK_REL,					/// 소환수 공격력 증가
		SA_MAX_HP_ABS,						/// 최대 HP 증가 (수치)
		SA_MAX_HP_REL,						/// 최대 HP 증가 (%)
		SA_MAX_MP_ABS,						/// 최대 MP 증가 (수치)
		SA_MAX_MP_REL,						/// 최대 MP 증가 (%)
		SA_TELEPORT_RANGE_ABS,				/// 캐릭터 이동스킬 거리 증가
		SA_CHARGING_TIME,					/// 모으는 시간
		SA_HP_REL_TO_MP_ABS_1,				/// 마력전환 첫번째 param (HP의 %)
		SA_HP_REL_TO_MP_ABS_2,				/// 마력 전환 두번째 param (MP의 수치)
		SA_MP_CONSUME_REL,					/// 마나 소모 조정
		SA_MP_GAIN_ON_HIT_REL,				/// (타격시)마나 획득량 조정
		SA_SCOPE_ABS,						/// 스킬범위 조정 (반경,수치)
		SA_PROJECTILE_FIRE_COUNT,			/// 발사체 개수 조정
		SA_PROJECTILE_RANGE_REL,			/// 발사체 거리 조정
		SA_PROJECTILE_SIZE,					/// 발사체 사이즈 조정
		SA_PROJECTILE_PIERCING,				/// 발사체 힛트 수 (관통력) 조정
		SA_MOVE_SPEED,						/// 이동 속도 조정
		SA_JUMP_SPEED,						/// 점프 속도 조정
		SA_ATK_PHYSIC_TO_ATK_MAGIC,			/// 물리 공격력의 일정량 마법공격력
		SA_ATK_MAGIC_TO_ATK_PHYSIC,			/// 마법 공격력의 일정량 물리공격력
		SA_READY_TIME_REL,					/// 시즈모드 준비 시간 조정
		SA_FORCE_DOWN_REL,					/// 강제다운 수치 조정 (%)
		SA_FORCE_DOWN_MELEE_REL,			/// 근접공격 강제다운 수치 조정 (%)
		SA_FORCE_DOWN_RANGE_REL,			/// 발사체공격 강제다운 수치 조정 (%)
		SA_FORCE_DOWN_ABS,					/// 스킬타격시 강제다운 수치 조정 (수치)
		SA_MP_REGENERATION_ABS,				/// MP 회복 증가량
		SA_MP_INTAKE_ABS,					/// MP 강탈량
		SA_TARGET_ANI_SPEED,				/// 대상 애니 속도 조정
		SA_TARGET_MOVE_SPEED,				/// 대상 이동 속도 조정
		SA_TARGET_JUMP_SPEED,				/// 대상 점프 속도 조정
		SA_TARGET_ATK_REL,					/// 대상 공격력 조정
		SA_HP_INTAKE_REL_DAMAGE,			/// 준 데미지 대비 HP 흡수량(%)
		SA_ATK_CRITICAL_DAM_ABS,			/// 공격자가 주는 크리티컬 데미지 증가
		SA_ATK_CRITICAL_RATE_ABS,			/// 공격자가 주는 크리티컬 비율
		SA_DEF_CRITICAL_DAM_ABS,			/// 피격자가 받는 크리티컬 데미지 증가
		SA_DEF_CRITICAL_RATE_ABS,			/// 피격자가 받는 크리티컬 비율		// kimhc // 2011-04 에 패치될 청 캐시 스킬
		SA_BACK_SPEED_X,					/// 공격에 의한 x방향으로 밀려나는 값
		SA_ITEM_SPECIAL_ABILITY_REL,		/// HP/MP 아이템 습득시 회복량 증가(%)
		SA_MP_GAIN_GET_HIT_REL,				/// (피격시)마나 획득량 조정 (%) 
		SA_SOUL_GAIN_ON_HIT_REL,			/// (타격시)소울 획득량 조정 (%) 
		SA_SOUL_GAIN_GET_HIT_REL,			/// (피격시)소울 획득량 조정 (%) 
		SA_HP_GAIN_REL_MAX_HP,				/// 캐릭터의 MAX HP의 %에 해당하는 HP 회복
		SA_ED_GAIN_REL,						/// ED 습득량 증가
		SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL,	/// (물리 공격력 + 마법 공격력)의 %데미지
		SA_EXP_GAIN_REL,					/// EXP 습득량 증가
		SA_DAMAGE_ABSORB_REL_MAX_HP,		/// 캐릭터의 MAX HP의 %에 해당하는 데미지 흡수


		SA_STRONG_MIND_ACTIVE_REL,			/// ??
		SA_STRONG_MIND_SPECIAL_ACTIVE_REL,	/// ??
		SA_MP_RETAKE_ABS,					/// 마나 회복

		SA_RESIST_FIRE_ABS,					/// 불 저항
		SA_RESIST_WATER_ABS,				/// 물 저항
		SA_RESIST_GREEN_ABS,				/// 자연 저항
		SA_RESIST_WIND_ABS,					/// 바람 저항
		SA_RESIST_LIGHT_ABS,				/// 빛 저항
		SA_RESIST_DARK_ABS,					/// 어둠 저항

		SA_ATK_MAGIC_FIRST_REL,				/// 첫번째 마법
		SA_ATK_MAGIC_SECOND_REL,			/// 두번째 마법
		SA_ATK_MAGIC_THIRD_REL,				/// 세번째 마법

		SA_REMOTE_SPEED_ABS,				/// 속도 조절

		SA_DAMAGE_TYPE_CHANGE,				/// 데미지 형태 변경
		SA_DAMAGE_TIME,						/// 데미지 시간

//{{ 김상훈 : 2010.11.12
//#ifdef NEW_SKILL_2010_11
		SA_RECOVER_MP_ABS, 
		SA_TARGET_MANA,
//#endif NEW_SKILL_2010_11
//}} 김상훈 : 2010.11.12		
//#ifdef NEW_SKILL_2010_11
		SA_TARGET_MANA_REL,					/// 타겟 마나 제거 비율, JHKang
		SA_SIZE_ABS,						/// 레벨별 NPC 크기, (아토믹 쉴드 크기 값), JHKang
//#endif NEW_SKILL_2010_11
		SA_REFLECT_REL,						/// 마법 반사 확률 
		SA_EL_DEFENCE_ABS,					/// 모든 속성 저항
		SA_TARGET_HP_REL,					/// 대상 HP 퍼센트 회복
		SA_TARGET_HITRATE_REL,				/// EDT BLIND의 강도
		//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
		SA_CHARGE_CANNON_BALL,				/// 캐논볼 차지량
		//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011.1.14 // 청 1차 전직
		SA_FIRE_DEFENCE_ABS,				/// 스탯 창의 불
		SA_WATER_DEFENCE_ABS,				/// 스탯 창의 물
		SA_NATURE_DEFENCE_ABS,				/// 스탯 창의 자연(그린)
		SA_WIND_DEFENCE_ABS,				/// 스탯 창의 바람
		SA_LIGHT_DEFENCE_ABS,				/// 스탯 창의 빛
		SA_DARK_DEFENCE_ABS,				/// 스탯 창의 어둠

		SA_EDT_FIRE_RESIST_REL,				/// 화상 저항 배율
		SA_EDT_FROZEN_RESIST_REL,			/// 빙결 저항 배율
		SA_EDT_ICE_RESIST_REL,				/// 동상 저항 배율
		SA_EDT_POISON_RESIST_REL,			/// 독 저항 배율
		SA_EDT_Y_PRESSED_RESIST_REL,		/// 프레스 저항 배율
		SA_EDT_LEG_WOUND_RESIST_REL,		/// 상처 저항 배율
		SA_EDT_STUN_RESIST_REL,				/// 스턴 저항 배율
		SA_EDT_CURSE_RESIST_REL,			/// 저주 저항 배율

		SA_EXPAND_CB,						/// 캐논볼 확장 수
		SA_ANI_SPEED,						/// 애니메이션 속도 증가
		//}} kimhc // 2011.1.14 // 청 1차 전직

		//{{ kimhc // 2011-04-04 // 2011-04 에 패치될 청 캐시 스킬
		SA_ACCURACY_PERCENT_ABS,			/// 명중률
		//}} kimhc // 2011-04-04 // 2011-04 에 패치될 청 캐시 스킬
		SA_CRITICAL_EVASION_REL,
//#ifdef CHUNG_SECOND_CLASS_CHANGE
		SA_DECREASE_REL,					/// 감소 비율
//#endif
//#ifdef ELSWORD_SHEATH_KNIGHT
		SA_ATK_ADD_DAMAGE_FIRST_ABS,
		SA_ATK_ADD_DAMAGE_SECOND_ABS,
		SA_ATK_ADD_DAMAGE_THIRD_ABS,
		SA_ATK_ADD_DAMAGE_FOURTH_ABS,
		SA_ATK_ADD_DAMAGE_FIFTH_ABS,
		SA_TELEPORT_SPEED_REL,
		HP_CONSUME_REL,
		SA_MP_BURN_ABS,
//#endif ELSWORD_SHEATH_KNIGHT
//#ifdef	SERV_TRAPPING_RANGER_TEST	//	페이탈리티 데미지와 확률
		SA_SKILLDAMAGE_MULTIPLE_01,
		SA_SKILLDAMAGE_MULTIPLE_02,
		SA_SKILLDAMAGE_MULTIPLE_03,
		SA_SKILLDAMAGE_MULTIPLE_04,

		SA_SKILLDAMAGE_MULTIPLE_01_RATE,
		SA_SKILLDAMAGE_MULTIPLE_02_RATE,
		SA_SKILLDAMAGE_MULTIPLE_03_RATE,
		SA_SKILLDAMAGE_MULTIPLE_04_RATE,
//#endif	SERV_TRAPPING_RANGER_TEST
//#ifdef RAVEN_WEAPON_TAKER
		SA_EVASION_REL,
		SA_REDUCE_EDT_TIME_REL,
		SA_OVERHEAT_HP,
//#endif RAVEN_WEAPON_TAKER
//#ifdef EVE_ELECTRA
		SA_SCOPE_ABS_HYPER,
		SA_EFFECTIVE_TIME_HYPER,
		SA_AIR_MOVE_COUNT,
//#endif EVE_ELECTRA
//#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		SA_NOT_CANNONBALL_REL,
		SA_INSERT_CANNONBALL_REL,
		SA_BE_SHOT_CANNONBALL_REL,
		SA_CANNONBALL_DAMAGE_UP_REL,
		SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE,
		SA_SIEGE_SHELLING_REL,
		SA_EFFECTIVE_DAMAGED_ATTACK_BASE,
//#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN
//#ifdef SERV_CHUNG_TACTICAL_TROOPER
		SA_FIRST_HYPER_EFFECTIVE,
		SA_SECOND_HYPER_EFFECTIVE,
		SA_THIRD_HYPER_EFFECTIVE,
//#endif SERV_CHUNG_TACTICAL_TROOPER
//#ifdef BALANCE_CODE_NEMESIS_20121213
		SA_ATK_CRITICAL_RATE_ATK_BASE,			/// 공격력의 % 만큼 추가되는 크리티컬 비율
//#endif //BALANCE_CODE_NEMESIS_20121213
		SA_PROJECTILE_CURVE_SPEED_REL,			/// 커브 스피드를 조절 해주는 배율
		SA_DP_GAIN_REL,							/// DP 획득량 배율
		SA_DP_USE_REL,							/// DP 소모량 배율
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		/// June SA ids 143-202 ( june2017_re/s0a_templet_keys.tsv ); SA_END is June's name table size ( 203 )
		SA_MP_CONSUMPTION,							/// June 143
		SA_PROJECTILE_RANGE_REL_2,							/// June 144
		SA_OVERHEAT_HP_PVP,							/// June 145
		SA_DECREASE_REL_2,							/// June 146
		SA_WAY_OF_SWORD_POINT_MULTIPLIER,							/// June 147
		SA_COOLTIME_1,							/// June 148
		SA_COOLTIME_2,							/// June 149
		SA_COOLTIME_3,							/// June 150
		SA_MP_GAIN_ON_HIT_REL_2,							/// June 151
		SA_MP_GAIN_ON_HIT_REL_3,							/// June 152
		SA_USE_DP_REL_AT_FORMATION_MODE,							/// June 153
		SA_RECOVERY_VALUE_REL,							/// June 154
		SA_ADD_MP_VALUE_ABS,							/// June 155
		SA_ADD_DP_VALUE_ABS,							/// June 156
		SA_SCOPE_ABS_PVP,							/// June 157
		SA_SCOPE_ABS_Y,							/// June 158
		SA_SCOPE_ABS_Z,							/// June 159
		SA_MP_REGENERATION_ABS_PVP,							/// June 160
		SA_ATK_PHYSIC_REL_PVP,							/// June 161
		SA_ATK_MAGIC_REL_PVP,							/// June 162
		SA_DEF_PHYSIC_REL_PVP,							/// June 163
		SA_DEF_MAGIC_REL_PVP,							/// June 164
		SA_TEAM_ATK_PHYSIC_REL_PVP,							/// June 165
		SA_TEAM_ATK_MAGIC_REL_PVP,							/// June 166
		SA_TEAM_DEF_PHYSIC_REL_PVP,							/// June 167
		SA_TEAM_DEF_MAGIC_REL_PVP,							/// June 168
		SA_TIME_ABS,							/// June 169
		SA_SOUL_ABS,							/// June 170
		SA_MAX_HP_REL_PVP,							/// June 171
		SA_RECOVER_MP_ABS_2,							/// June 172
		SA_ULTIMATE_STRENGTHENED_SKILL_POWERRATE,							/// June 173
		SA_MAX_INCREASE_MOVEMENT_BY_MANA,							/// June 174
		SA_MIN_INCREASE_MOVEMENT_BY_MANA,							/// June 175
		SA_CHAIN_SKILL_ANI_SPEED_REL,							/// June 176
		SA_HIT_COUNT_1,							/// June 177
		SA_HIT_COUNT_2,							/// June 178
		SA_HIT_COUNT_3,							/// June 179
		SA_FORCE_DOWN_GAIN_REL,							/// June 180
		SA_MUTATION_CONSUME_MAX_COUNT,							/// June 181
		SA_DAMAGED_REL_PVP,							/// June 182
		SA_ECP_REL,							/// June 183
		SA_SUMMON_ALL_STAT_REL,							/// June 184
		SA_ADD_LEARN_SKILL_LIST_1,							/// June 185
		SA_ADD_LEARN_SKILL_LIST_2,							/// June 186
		SA_ADD_LEARN_SKILL_LIST_3,							/// June 187
		SA_FP_CANCEL_SKILL,							/// June 188
		SA_ATK_PHYSIC_INCREASE_BY_ADDITIONALATTACK,							/// June 189
		SA_NOT_EQUIP_WITH_SKILLID_SAME_TIME,							/// June 190
		SA_FP_UP_ABS,							/// June 191
		SA_FP_DOWN_ABS,							/// June 192
		SA_EFFECTIVE_VALUE_REL,							/// June 193
		SA_CRITICAL_REL,							/// June 194
		SA_CRITICAL_DAMAGE_REL,							/// June 195
		SA_MAXIMIZE_REL,							/// June 196
		SA_DEGREE_X,							/// June 197
		SA_DEGREE_Y,							/// June 198
		SA_DEGREE_Z,							/// June 199
		SA_DEGREE_MAX,							/// June 200
		SA_DFG_GAIN_REL,							/// June 201
		SA_DFG_CONSUME_REL,							/// June 202
		SA_END,
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		// XSLSkillTree.h에도 추가해주세요~
	};																		
																			

#ifdef SERV_SKILL_NOTE
	// 엘소드	601001 ~ 601999
	// 아이샤	602001 ~ 602999
	// 레나		603001 ~ 603999
	// 레이븐	604001 ~ 604999
	// 이브		605001 ~ 605999
	// 청		606001 ~ 606999
//{{ Iruha : 2026-10-26 // June character port Phase CH1l4 (JUNE_CHARACTER_EVE_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_ENUM_NAMES
	/// June's MEMO_ID ( Enum.lua ): the type of a skill memo row ( JuneSkillMemoEntry::m_iType )
	enum MEMO_ID
	{
		MI_NONE					= 0,
		MI_ENHANCE				= 1,
		MI_HUGE					= 2,
		MI_LIGHT				= 3,
		MI_HEAVY				= 4,
		MI_USEFUL				= 5,
		MI_MAXIMIZE				= 6,
		MI_BRILLIANT			= 7,
		MI_CRITICAL				= 8,
		MI_FASTEST				= 9,
		MI_REGENERATE_1			= 10,
		MI_REGENERATE_2			= 11,
		MI_BRUTAL				= 12,
		MI_WICKED				= 13,
		MI_TOUGH				= 14,
		MI_CHECK_AND_KILL_1		= 15,
		MI_CHECK_AND_KILL_2		= 16,
		MI_ABSORB				= 17,
		MI_PERSISTENT			= 18,
		MI_TRANSCENDENCE_1		= 19,
		MI_TRANSCENDENCE_2		= 20,
		MI_SAGE					= 21,
		MI_LONGSHOT				= 22,
		MI_END					= 23,
	};
#endif SERV_IRUHADEV_JUNE_ENUM_NAMES
//}}
	enum SKILL_MEMO_ID
	{
		SMI_NONE			= 0,

		SMI_ELSWORD_MEMO1	= 601001,	/// 남자는 물러설 때도 알아야한다.
		SMI_ELSWORD_MEMO2,				/// 넓은 보법에 대한 조언
		SMI_ELSWORD_MEMO3,				/// 끝없는 공격 그 이론과 실재
		SMI_ELSWORD_MEMO4,				/// 양적 향상에 대한 생각
		SMI_ELSWORD_MEMO5,				/// 위험 부담 제거 법
		SMI_ELSWORD_MEMO6,				/// 긴장된 호흡과 강화 유지법
		SMI_ELSWORD_MEMO7,				/// 발차기
		SMI_ELSWORD_MEMO8,				/// 용암분출 (플레임게이져)
		SMI_ELSWORD_MEMO9,				/// 정확하고 강한 타격 (스파이럴 블래스트)
		SMI_ELSWORD_MEMO10,				/// 영혼파괴 (윈드 블레이드)
		SMI_ELSWORD_MEMO11,				/// 갑옷붕괴 (아머 브레이크)
		SMI_ELSWORD_MEMO12,				/// 꺼지지 않는 불길 (대폭살)
#ifdef ADDITIONAL_MEMO
		SMI_ELSWORD_MEMO13	= 601013,	/// 궤뚫는 검 (하쉬 체이서)
		SMI_ELSWORD_MEMO14,				/// 베고 또 베고 (크레센트 컷)
		SMI_ELSWORD_MEMO15,				/// 칼의 비 (소드 폴)
#endif

		SMI_AISHA_MEMO1		= 602001,	/// 재벌 구이 방법(체인 파이어볼 편)
		SMI_AISHA_MEMO2,				/// 정신적 외상
		SMI_AISHA_MEMO3,				/// 최선형 미사일
		SMI_AISHA_MEMO4,				/// 독을 쓸 때에는 자비심 없이!
		SMI_AISHA_MEMO5,				/// 영혼 이동에 대한 메모
		SMI_AISHA_MEMO6,				/// 견고한 마나 실드에 대한 메모
		SMI_AISHA_MEMO7,				/// 써클플레임
		SMI_AISHA_MEMO8,				/// 200% 과충전 쇼크 (라이트닝 볼트)
		SMI_AISHA_MEMO9,				/// 열기폭발 (체인 버스트)
		SMI_AISHA_MEMO10,				/// 리드미컬한 회전 ( 헬스톤 )
		SMI_AISHA_MEMO11,				/// 자유로운 영혼 (메디테이션)
		SMI_AISHA_MEMO12,				/// 끝없는 갈망 (마나 인테이크)
#ifdef ADDITIONAL_MEMO
		SMI_AISHA_MEMO13	= 602013,	/// 파워 드레인 (에너지 드레인)
		SMI_AISHA_MEMO14,				/// 마력을 담은 망치질 (헤비 프레스)
		SMI_AISHA_MEMO15,				/// 에너지의 링 (에너지 스퍼트)
#endif

		SMI_RENA_MEMO1		= 603001,	/// 살상력을 높이는 관통력에 대한 연구
		SMI_RENA_MEMO2,					/// 증폭된 불사조와의 교감
		SMI_RENA_MEMO3,					/// 뼈속까지 스며드는 아픔
		SMI_RENA_MEMO4,					/// 언제까지 따라가니기만 할 것인가
		SMI_RENA_MEMO5,					/// 재 도약의 기회 (백덤블링 샷 편)
		SMI_RENA_MEMO6,					/// 재 도약의 기회 (리플렉트 킥 편)
		SMI_RENA_MEMO7,					/// 시즈모드
		SMI_RENA_MEMO8,					/// 처음은 살살3 ( 멀티플 스팅거)
		SMI_RENA_MEMO9,					/// 급습 ( 스피닝 킥 )
		SMI_RENA_MEMO10,				/// 공기 응축법 (허밍 윈드)
		SMI_RENA_MEMO11,				/// 명품 발차기 ( 미들킥 )	/// 2013년 스킬 트리 개편 이후, 로우킥에서 미들킥으로 변경
		SMI_RENA_MEMO12,				/// 덫 숙련 ( 인탱글 )
#ifdef ADDITIONAL_MEMO
		SMI_RENA_MEMO13		= 603013,	/// 폭발물 취급 주의(폭발의 덫)
		SMI_RENA_MEMO14,				/// 추격하는 정령 (이보크)
		SMI_RENA_MEMO15,				/// 보호된 엘드랏실의 가지 (트래핑 애로우)
#endif

		SMI_RAVEN_MEMO1		= 604001,	/// 고통스러운 진행 속도
		SMI_RAVEN_MEMO2,				/// 정신을 잃게 만드는 타격
		SMI_RAVEN_MEMO3,				/// 회전으로 인한 어지러움에 면역
		SMI_RAVEN_MEMO4,				/// 폭파
		SMI_RAVEN_MEMO5,				/// 신나는 달리기
		SMI_RAVEN_MEMO6,				/// 번개 같은 발걸음,
		SMI_RAVEN_MEMO7,				/// 새도우 피어싱
		SMI_RAVEN_MEMO8,				/// 새로운 보법 (파워 어설트)
		SMI_RAVEN_MEMO9,				/// 약점 간파 (버서커 블레이드)
		SMI_RAVEN_MEMO10,				/// 볼트 출력 강화 (차지드 볼트)
		SMI_RAVEN_MEMO11,				/// 아물지 않는 상처 (컷텐던)
		SMI_RAVEN_MEMO12,				/// 효과적인 무기 파괴 (웨폰 브레이크)
#ifdef ADDITIONAL_MEMO
		SMI_RAVEN_MEMO13	= 604013,	/// 농밀한 화약 (연무폭파)
		SMI_RAVEN_MEMO14,				/// 불붙은 칼의 춤 (버스팅 블레이드)
		SMI_RAVEN_MEMO15,				/// 패스트 리볼빙 (리볼버 캐논)
#endif

		SMI_EVE_MEMO1		= 605001,	/// 강한 흡입력
		SMI_EVE_MEMO2,					/// 연속공격을 위한 소환
		SMI_EVE_MEMO3,					/// 처음은 살살 (익스플로전 임팩트 편)
		SMI_EVE_MEMO4,					/// 처음은 살살 (소닉 웨이브 편)
		SMI_EVE_MEMO5,					/// 쓸쓸한 기다림
		SMI_EVE_MEMO6,					/// 속임수 계약
		SMI_EVE_MEMO7,					/// 메가 일렉트론볼
		SMI_EVE_MEMO8,					/// 변형 카오스 엔진 (일루전 스팅어)
		SMI_EVE_MEMO9,					/// 특제 나소드 고폭탄 (호넷 스팅)
		SMI_EVE_MEMO10,					/// 도움이 되기 위한 훈련 (서피스 커팅)
		SMI_EVE_MEMO11,					/// 짧고 굵게! (메탈 더스트 오라)
		SMI_EVE_MEMO12,					/// 오펜스 모드 전환! (오베론 가드)
#ifdef ADDITIONAL_MEMO
		SMI_EVE_MEMO13		= 605013,	/// 이중 자가 반응로 (엘 분광결정)
		SMI_EVE_MEMO14,					/// 고출력 전자 빔 (파티클 레이)
		SMI_EVE_MEMO15,					/// 반응 가속 (스위프 롤링)
#endif

		//{{ 2011-04 에 패치될 청 메모 스킬
#ifdef	CHUNG_MEMO_01					
		SMI_CHUNG_MEMO1		= 606001,	/// 탄약개조 (디토네이션)
		SMI_CHUNG_MEMO2,				/// 광폭발 (스케어체이스)
		SMI_CHUNG_MEMO3,				/// 기합포 (스틸엣지)
		SMI_CHUNG_MEMO4,				/// 진정한 느와르 액션 (듀얼버스터)
		SMI_CHUNG_MEMO5,				/// 우겨넣기 (리로드캐논)
		SMI_CHUNG_MEMO6,				/// 속성탄환 (에이밍샷)
		SMI_CHUNG_MEMO7,				/// 어스퀘이크 (리프어택)
		SMI_CHUNG_MEMO8,				/// REAL 헤드샷 (헤드샷)
#endif	CHUNG_MEMO_01					
		//}} 2011-04 에 패치될 청 메모 스킬
#ifdef ADDITIONAL_MEMO
		SMI_CHUNG_MEMO9		= 606009,	/// 푸시&푸시 (브루탈 스윙)
		SMI_CHUNG_MEMO10,				/// 태풍과 같은 함성 (아이언 하울링)
		SMI_CHUNG_MEMO11,				/// 자동 감응탄 (헤비 레일건)
		SMI_CHUNG_MEMO12,				/// 인내,인내,인내 (가드)
		SMI_CHUNG_MEMO13,				/// 연쇄반응탄 (럼블샷)
		SMI_CHUNG_MEMO14,				/// 수호하는 마음가짐 (원더 월)
		SMI_CHUNG_MEMO15,				/// 3단 분열 유도탄 (드레드 체이스)
		SMI_CHUNG_MEMO16,				/// 이중 투척 (일레스틱 밤)
		SMI_CHUNG_MEMO17,				/// 지원폭격 세리머니 (지휘관의 표식)
#endif

#ifdef ADD_ARA_MEMO	// 적용날짜: 2013-03-22

		SMI_ARA_MEMO1		= 607001,	/// 끝 없는 갈망 II (흡공)
		SMI_ARA_MEMO2		= 607006,	/// 흐름을 타는 창술 (맹호 2식 : 바람쐐기)
		SMI_ARA_MEMO3,					/// 영혼 꿰기 (기공창)
		SMI_ARA_MEMO4,					/// 초천공술 (용아 2식 : 빨래널기)
		SMI_ARA_MEMO5,					/// 비전 그림자 창술 (그림자매듭)
		SMI_ARA_MEMO6,					/// 바위로 바위 치기 (맹호 1식 : 바위깨기)
		SMI_ARA_MEMO7,					/// 물 흐르듯이 (이화접목)
		SMI_ARA_MEMO8,					/// 정신을 잃게 만드는 타격 II (용아 3식 : 팔괘장)
		SMI_ARA_MEMO9,					/// 진 달빛베기 (용아 4식 : 달빛베기)

#endif // ADD_ARA_MEMO

#ifdef ADD_MEMO_1ST_CLASS //김창한
		SMI_ELSWORD_MEMO16	= 601016,	/// 균열 파괴 ( 웨폰 브레이크 )
		SMI_ELSWORD_MEMO17,				/// 순간을 파고드는 일격 ( 더블 슬래시 )
		SMI_ELSWORD_MEMO18,				/// 강화된 검기의 응용법 ( 아마겟돈 블레이드 )
		SMI_ELSWORD_MEMO19,				/// 버스트 인챈트 ( 소드 인챈트 )
		SMI_ELSWORD_MEMO20,				/// 질적 향상에 대한 생각 1 (라이징 웨이브 )
		SMI_ELSWORD_MEMO21,				/// 작열하는 화염의 검 (소드 파이어 )
		SMI_ELSWORD_MEMO22,				/// 타고남은 재의 활용 (피닉스 탤런 )
		SMI_ELSWORD_MEMO23,				/// 톱날 칼날 ( 소드 실드 )
		SMI_ELSWORD_MEMO24,				/// 계산된 투척 ( 피어싱 소드 )
		SMI_ELSWORD_MEMO25,				/// 허공검법 ( 파이널 스트라이크 )
		SMI_ELSWORD_MEMO26,				/// 질적 향상에 대한 생각 2 ( 팬텀 소드 )

		SMI_AISHA_MEMO16	= 602016,	/// 돌풍의 부름 ( 사이클론 )
		SMI_AISHA_MEMO17,				/// 번개를 다루는 방법 ( 체인 라이트닝 )
		SMI_AISHA_MEMO18,				/// 분노? 분노! (블레이즈 스텝 )
		SMI_AISHA_MEMO19,				/// 차가운 마음가짐 ( 블리자드 샤워 )
		SMI_AISHA_MEMO20,				/// 사신의 미소 ( 데스 필드 )
		SMI_AISHA_MEMO21,				/// 주문하신 운석입니다 ( 헬 드롭 )
		SMI_AISHA_MEMO22,				/// 쇠퇴, 집념, 사념 ( 에이징 )
		SMI_AISHA_MEMO23,				/// 이온 플라즈마 ( 플라즈마 커터 )
		SMI_AISHA_MEMO24,				/// 몸무게의 정의 ( 길로틴 프레스 )
		SMI_AISHA_MEMO25,				/// 아름다운 화장법 ( 매지컬 메이크업 )
		SMI_AISHA_MEMO26,				/// 네 것도 내꺼-! ( 바이탈 드레인 )
		SMI_AISHA_MEMO27,				/// 추가 주문 ( 슈퍼 노바 )
		
		SMI_RENA_MEMO16		= 603016,	/// 고통에 대한 고찰 1 ( 로우 킥 )
		SMI_RENA_MEMO17,				/// 후폭풍 ( 다이브킥 봄잉 )
		SMI_RENA_MEMO18,				/// 집념의 증거 ( 바이올런트 어택 )
		SMI_RENA_MEMO19,				/// 속성 화살 ( 슈팅 매그넘 )
		SMI_RENA_MEMO20,				/// 마탄의 구체 ( 크레이지 샷 )
		SMI_RENA_MEMO21,				/// 폭발에 대한 정의 1 ( 궁그닐 )
		SMI_RENA_MEMO22,				/// 빈틈을 노리는 일격 ( 페이탈리티 )
		SMI_RENA_MEMO23,				/// 냉기독을 가진 씨앗 ( 구속의 씨앗 )
		SMI_RENA_MEMO24,				/// 날카로운 가시 1 ( 가시덩굴 덫 )
		SMI_RENA_MEMO25,				/// 날카로운 가시 2 ( 찔레 덫 )
		SMI_RENA_MEMO26,				/// 최상급 징벌 ( 콜 오브 루인 )
		SMI_RENA_MEMO27,				/// 신성한 기운의 집약 ( 카르마 )

		SMI_RAVEN_MEMO16	= 604016,	///	검기의 운용 1 ( 쇼크 웨이브 - 디바이더 )
		SMI_RAVEN_MEMO17,				/// 검기의 운용 2 ( 쇼크 웨이브 - 커터 )
		SMI_RAVEN_MEMO18,				/// 잘게썰기 ( 하이퍼 소닉 스탭 )
		SMI_RAVEN_MEMO19,				/// 피의 맹약 ( 블러디 액셀 )
		SMI_RAVEN_MEMO20,				/// 연쇄 폭발 ( 헬 다이브 )
		SMI_RAVEN_MEMO21,				/// 높은 곳에서의 낙하 ( 발키리스 자벨린 )
		SMI_RAVEN_MEMO22,				/// 완벽한 충격을 주는 방법 ( 아머 크래시 )
		SMI_RAVEN_MEMO23,				/// 강철의 가시 ( 차지드 볼트 - 블러디 쏜즈 )
		SMI_RAVEN_MEMO24,				/// 신성한 폭발 ( 가디언 스트라이크 )
		SMI_RAVEN_MEMO25,				/// 폭발에 대한 정의 ( 뉴클리어 )
		SMI_RAVEN_MEMO26,				/// 육참골단 ( 하푼 스피어 )
		SMI_RAVEN_MEMO27,				/// 화염의 잔상 ( 화염인 )
		SMI_RAVEN_MEMO28,				/// 버스트 리볼빙 ( 리볼버 캐논 - HE탄 )
		SMI_RAVEN_MEMO29,				/// 오버 차지 ( 헬파이어 개틀링 )
		SMI_RAVEN_MEMO30,				/// 폭발에 대한 정의 2 ( 기가 프로미넌스 )

		SMI_EVE_MEMO16		= 605016,	/// 특제 나소드 관통탄 ( 호넷 스팅 - 쉐이프트 차지 )
		SMI_EVE_MEMO17,					/// 자존감 강화 ( 퀸스 쓰론 )
		SMI_EVE_MEMO18,					/// 코보 믹서기 엔진 ( 정크 브레이크 )
		SMI_EVE_MEMO19,					/// 깊게 베기 ( 제노사이드 리퍼 )
		SMI_EVE_MEMO20,					/// 강한 충격을 전하는 방법 ( 헤븐즈 피스트 - 프레셔 )
		SMI_EVE_MEMO21,					/// 뺨때리기 ( 헤븐즈 피스트 - 스위퍼 )
		SMI_EVE_MEMO22,					/// 삼단 프리즘 ( 스위프 롤링 - 트라이 발칸 )
		SMI_EVE_MEMO23,					/// 코보 특제 섬광 ( 포톤 플레어 )
		SMI_EVE_MEMO24,					/// 고출력 입자 ( 리니어 디바이더 )
		SMI_EVE_MEMO25,					/// 풀 일렉트로닉 ( 기가 스트림 )

		SMI_CHUNG_MEMO18	= 606018,	/// 공포 그리고 외침 ( 판데모니움 - 피어 )
		SMI_CHUNG_MEMO19,				/// 혼돈 그리고 외침 ( 판데모니움 - 카오스 )
		SMI_CHUNG_MEMO20,				/// 묵직한 일격 ( 루나틱 퓨리 )
		SMI_CHUNG_MEMO21,				/// 향상된 외치기 ( 칼라드볼그 폐인 )
		SMI_CHUNG_MEMO22,				/// 향상된 조준법 ( 더블 파이어 )
		SMI_CHUNG_MEMO23,				/// 확산탄 제조법 ( 매그넘 샷 )
		SMI_CHUNG_MEMO24,				/// 코보 특제 유도탄 ( 슈팅 스타 )
		SMI_CHUNG_MEMO25,				/// 숙련된 포병의 유도 ( 아틸러리 스트라이크 - 퀀텀 발리스타 )
		SMI_CHUNG_MEMO26,				/// 인가되지 않은 폭탄 ( 빅뱅 스트림 )
		SMI_CHUNG_MEMO27,				/// 특제 수류탄 제조법 ( 일레스틱 밤 - 그리네이드 런처 )
		SMI_CHUNG_MEMO28,				/// 흔들림 없는 자세 ( 캐논 스트라이크 )
		SMI_CHUNG_MEMO29,				/// 포신 강화 ( 카오스 캐논 ) 
		SMI_CHUNG_MEMO30,				/// 추가 지원 ( 카펫 바밍 )

		SMI_ARA_MEMO10		= 607018,	/// 포용 ( 낙화)
		SMI_ARA_MEMO11,					/// 숨겨진 맹수의 발톱 ( 맹호 2식 : 호랑이 발톱 )
		SMI_ARA_MEMO12,					/// 맹호의 기상 ( 맹호격 )
		SMI_ARA_MEMO13,					/// 효율적인 기력 운용 ( 나찰 1식 : 흡혼 )
		SMI_ARA_MEMO14,					/// 깊게 할퀴기 ( 낭아 3식 : 늑대 발톱 )
		SMI_ARA_MEMO15,					/// 끝 없는 갈망 3 ( 이기공 )
		SMI_ARA_MEMO16,					/// 끝 없는 갈망 5 ( 이혈공 )
		SMI_ARA_MEMO17,					/// 늑대의 물어 뜯기 ( 낭아 4식 : 늑대 이빨 )
		SMI_ARA_MEMO18,					/// 구슬로 구슬치기 ( 연환지옥 )
		SMI_ARA_MEMO19,					/// 탈출 불가능! ( 나찰 3식 : 창살감옥 )
		SMI_ARA_MEMO20,					/// 패황의 기운 ( 기공파 )
		SMI_ARA_MEMO21,					/// 탄력적인 기의 운용 ( 반탄공 )

		SMI_ELESIS_MEMO1	= 608007,	/// 섬멸을 위한 준비 ( 크로스 슬래시 )
		SMI_ELESIS_MEMO2,				/// 빠르고 강한 공격1 ( 스파이럴 블래스트 )
		SMI_ELESIS_MEMO3,				/// 효율적인 타격 방법 ( 발차기 )
		SMI_ELESIS_MEMO4,				/// 더 아프게 때리는 방법 ( 헤비 스터너 )
		SMI_ELESIS_MEMO5,				/// 파괴 달인의 기술 ( 파워 브레이크 )
		SMI_ELESIS_MEMO6,				/// 유연하게 움직이는 방법 ( 세이버 - 익스팅션 )
		SMI_ELESIS_MEMO7,				/// 빠르고 강한 공격2 ( 심판의 검 )
		SMI_ELESIS_MEMO8,				/// 환검 ( 승리의 검 )
		SMI_ELESIS_MEMO9,				/// 명경지수 ( 극기 - 강 )
		SMI_ELESIS_MEMO10,				/// 강한 충격 ( 소드 이럽션 )
		SMI_ELESIS_MEMO11,				/// 효과적인 불 태우기 ( 대폭살 )
		SMI_ELESIS_MEMO12,				/// 신속한 검격 ( 버스트 라이징 )
		SMI_ELESIS_MEMO13,				///	강할 수록 좋지! ( 버스트 웨이브 )
		SMI_ELESIS_MEMO14,				/// 불의 증폭 ( 소드 파이어 )
		SMI_ELESIS_MEMO15,				/// 예리한 불의 칼날 ( 인페르날 블레이드 )
		SMI_ELESIS_MEMO16,				/// 폭염 개화 ( 이터널 파이어 )

#endif //ADD_MEMO_1ST_CLASS

		SMI_END,
	};
#endif
	
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

	enum SKILL_POWER_RATE_TYPE		/// 각 조건에 대한 데미지 배율 인덱스
	{
		SPRT_NONE			= -1,
		SPRT_BASIC_CLASS,			/// 기본 전직
		SPRT_FIRST_CLASS,			/// 1차전직
		SPRT_SECOND_CLASS,			/// 2차전직
		SPRT_SECOND_CLASS2,			/// 2차전직2 (후반 스킬) 
		SPRT_HYPER_SKILL,			/// 각성기
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		SPRT_HYPER_CLASS,           /// June 5: the hyper-class power-rate row
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		SPRT_END,
	};

#endif // UPGRADE_SKILL_SYSTEM_2013

	struct SkillTemplet
	{
		bool						m_bGuildSkill;					/// 길드 스킬 확인
		SKILL_TYPE					m_eType;						/// 스킬 타입
		SKILL_ID					m_eID;							/// 스킬 아이디
		
		//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
	#ifdef	NEW_CHARACTER_CHUNG
		USHORT						m_usCBConsumption;				/// 캐논볼 소모량
	#endif	NEW_CHARACTER_CHUNG
		//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

	#ifdef ARA_CHARACTER_BASE
		USHORT						m_usFPGainCount;				/// 기력 획득량
		USHORT						m_usFPConsumtion;				/// 기력 사용량
	#endif
	
		wstring						m_wstrName;						/// 스킬 이름
		wstring						m_wstrMainDesc;					/// 스킬 설명 ( 레벨과 관계없이 공통으로 표시되는 툴팁 내용 )

		bool						m_bBornSealed;					/// 봉인이 있는 스킬인지
		
		wstring						m_wstrIconName;					/// 스킬 아이콘 텍스처 이름
		wstring						m_wstrIconPieceName;			/// 스킬 아이콘 텍스처 Rect 이름
		ACTIVE_SKILL_USE_CONDITION	m_eActiveSkillUseCondtion;		/// 스킬 사용을 위한 상태
		bool						m_bShowSkillLevel;				/// 스킬 레벨 표시 여부
		
		vector<CX2BuffFactorPtr>	m_vecBuffFactorPtr;				/// 스킬 버프
		vector<UINT>				m_vecBuffFactorID;				/// 버프 팩터 ID, m_vecBuffFactorPtr와 같은 index로 접근 가능

#ifdef UPGRADE_SKILL_SYSTEM_2013
		vector<wstring>				m_vecEffectiveDesc;				/// 스킬 효과 설명 ( 레벨별로 다르게 표시되는 툴팁 내용  )
		wstring						m_wstrImageName;				/// 스킬 이미지 텍스처 이름
		wstring						m_wstrImagePieceName;			/// 스킬 이미지 텍스처 Rect 이름
		int							m_iRequireLearnSkillPoint;		/// 스킬 습득에 필요한 SP
		int							m_iRequireUpgradeSkillPoint;	/// 스킬 강화에 필요한 SP
		vector<CX2Stat::Stat>		m_vecStat;						/// 버프필살기의 경우 stat 증가치
		vector<float>				m_vecSkillCoolTime;				/// 쿨타임
		vector<float>				m_vecMPConsumption;				/// MP 소모량
		vector<float>				m_vecPowerRate;					/// 공격력 배율
		vector<int>					m_vecRequireCharacterLevel;		/// 스킬 습득 요구 레벨
		int							m_iWOSType;						/// 검의길 타입
		std::map< CX2SkillTree::SKILL_ABILITY_TYPE, vector<float> > m_mapSkillAbility;	/// 스킬 능력치
#else // UPGRADE_SKILL_SYSTEM_2013
		int							m_iSkillLevel;					/// 이 스킬템플릿의 스킬 레벨
		float						m_fSkillCoolTime;				/// 쿨타임
		float						m_fMPConsumption;				/// MP 소모량
		CX2Stat::Stat				m_Stat;							/// 버프필살기의 경우 stat 증가치
		std::map< SKILL_ABILITY_TYPE, float >	m_mapSkillAbility;	/// 스킬 능력치
		float						m_fPowerRate;					/// 공격력 비율
#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
		SKILL_POWER_RATE_TYPE		m_eSkillPowerRateType;
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
		SKILL_POWER_RATE_TYPE		m_ePowerRateType;	/// June templet +0x174 m_ePowerRate (32216320 @32217cbb). March reads the
															/// key but keeps it only under SKILL_LEVEL_UP_BY_POWER_RATE_TYPE (off here)
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		/// June 32216320 NUMERICAL_DATA<n> element ( 0x48 B in a vector at templet +0x17c ); rows are 16 B { value, pvp rate, tech point, hit add mp }
		struct NumericalRow
		{
			float   m_fValue;
			float   m_fPvpRate;
			float   m_fTechPoint;
			float   m_fHitAddMP;
		};
		struct NumericalData
		{
			wstring                     m_wstrName;         /// m_wstrName ( an SA_* name for the ability lookup )
			wstring                     m_wstrParamType;    /// m_ParamType, default L"i" ( unused by the damage override )
			vector<NumericalRow>    m_vecRow;
		};
		int                                 m_iDeleteSkillIDByUpgrade;  /// June +0x178 m_DeleteSkillIDByUpgrade: the base skill this one replaces once learned
		vector<NumericalData>           m_vecNumericalData;     /// June +0x17c
		vector< std::pair<wstring, wstring> >   m_vecExtraIconInfo;     /// June +0x88: ( m_wstrTextureName, m_wstrTexturePieceName ) of m_ExtraIconInfo<n>

		static void GetNumericalRow( const NumericalData& data_, int iLevel_, NumericalRow& rowOut_ );     /// 320716c0
		const NumericalData* FindNumericalData( const WCHAR* pwszName_ ) const;                                 /// 3221e930
#ifdef SERV_IRUHADEV_JUNE_SKILL_NUMERICAL
		void GetNumericalRowByIndex( int iIndex_, int iLevel_, NumericalRow& rowOut_ ) const;                  /// 320717e0
		void GetNumericalRowByName( const WCHAR* pwszName_, int iLevel_, NumericalRow& rowOut_ ) const;        /// 32078060
#endif SERV_IRUHADEV_JUNE_SKILL_NUMERICAL
		bool GetNumericalAbilityValue( SKILL_ABILITY_TYPE eType_, int iLevel_, bool bPvp_, float& fOut_ ) const;
		bool TryGetSkillAbilityValue( SKILL_ABILITY_TYPE eType_, int iLevel_, bool bPvp_, float& fOut_ ) const;     /// 32206fb0
		float GetSkillAbilityValue( SKILL_ABILITY_TYPE eType_, int iLevel_, bool bPvp_ ) const;                         /// 32206e50
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET

#ifdef UPGRADE_SKILL_SYSTEM_2013

		/** @function	: GetSkillStatValue
		@brief		: 스킬 레벨에 해당하는 습득 요구 레벨을 가져옴
		*/
		const int GetSkillRequireLevelValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecRequireCharacterLevel.size() ) > iIndex )
				return m_vecRequireCharacterLevel[iIndex]; 

			return 0;
		}

		/** @function	: GetSkillStatValue
		@brief		: 스킬 레벨에 해당하는 스텟 값을 가져옴
		*/
		const CX2Stat::Stat GetSkillStatValue( int iLevel ) const
		{
			CX2Stat::Stat stat;

			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecStat.size() ) > iIndex )
				stat.AddStat( m_vecStat[iIndex] ); 

			return stat;
		}

		/** @function	: GetSkillPowerRateValue
		@brief		: 스킬 레벨에 해당하는 데미지 배율 값을 가져옴
		*/
		const float GetSkillPowerRateValue( int iLevel ) const
		{
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
			/// June 32071720: an upgraded skill ( m_DeleteSkillIDByUpgrade != 0 ) takes its power rate from SA 0xad first
			if ( 0 != m_iDeleteSkillIDByUpgrade )
			{
				float fUltimatePowerRate = 0.f;
				if ( true == TryGetSkillAbilityValue( SA_ULTIMATE_STRENGTHENED_SKILL_POWERRATE, iLevel, false, fUltimatePowerRate ) )
					return fUltimatePowerRate;
			}
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecPowerRate.size() ) > iIndex )
				return m_vecPowerRate[iIndex];

			return 0.f;
		}

		/** @function	: GetSkillEffectiveDescValue
			@brief		: 스킬 타입에 해당하는 스킬 효과 툴팁 값을 가져옴
		*/
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_UI
		const wstring GetSkillEffectiveDescValue( int iLevel ) const;		// June 328ee280, defined in X2UISkillTreeNew2015.cpp
#else SERV_IRUHADEV_JUNE_SKILL_TREE_UI
		const wstring GetSkillEffectiveDescValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecEffectiveDesc.size() ) > iIndex )
			{
#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
				// 스킬 설명 확장이 필요한 경우의 예외 처리, 에너지 스퍼트의 @1 ( 현재 레벨 표기 ) 때문에 추가				
				wstring wstrEffectiveDesc = m_vecEffectiveDesc[iIndex];
				
				if ( true == GetSkillEffectiveDescExpand ( wstrEffectiveDesc, iLevel ) )
					return wstrEffectiveDesc;
				else
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편	
					return m_vecEffectiveDesc[iIndex];
			}
			return L"";
		}
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_UI

		/** @function	: GetSkillMPConsumption
			@brief		: 스킬 레벨에 해당하는 MP소비값을 가져옴
		*/
		const float GetSkillMPConsumptionValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecMPConsumption.size() ) > iIndex )
				return m_vecMPConsumption[iIndex];

			return 0.f;
		}

		/** @function	: GetSkillCoolTimeValue
			@brief		: 스킬 레벨에 해당하는 쿨타임 값을 가져옴
		*/
		const float GetSkillCoolTimeValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecSkillCoolTime.size() ) > iIndex )
				return m_vecSkillCoolTime[iIndex];

			return 0.f;
		}

		/** @function	: GetSkillAbilityValue
			@brief		: 스킬 어빌리티에 해당하는 값을 가져옴
		*/
		float GetSkillAbilityValue( CX2SkillTree::SKILL_ABILITY_TYPE eType, int iLevel ) const 
		{
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
			/// June 32206e50: a NUMERICAL_DATA block named after the ability wins over the +0x164 map
			{
				float fNumerical = 0.f;
				if ( true == GetNumericalAbilityValue( eType, iLevel, false, fNumerical ) )
					return fNumerical;
			}
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
			//ASSERT( false == m_mapSkillAbility.empty() );
			
			std::map< CX2SkillTree::SKILL_ABILITY_TYPE, vector<float> >::const_iterator it = m_mapSkillAbility.find( eType );
			//ASSERT( m_mapSkillAbility.end() != it );
			if( m_mapSkillAbility.end() != it )
			{
				vector<float> vecValue = it->second;

				const int iIndex = iLevel - 1;

				if ( 0 <= iIndex && static_cast<int>( vecValue.size() ) > iIndex )
					return vecValue[iIndex];
			}
				
			return 0.f;
		}

#else

		/** @function	: GetSkillAbilityValue
			@brief		: 스킬 타입에 해당하는 값을 가져옴
		*/
		float GetSkillAbilityValue( CX2SkillTree::SKILL_ABILITY_TYPE eType ) const 
		{
			//ASSERT( false == m_mapSkillAbility.empty() );
			
			std::map< SKILL_ABILITY_TYPE, float >::const_iterator it = m_mapSkillAbility.find( eType );
			//ASSERT( m_mapSkillAbility.end() != it );
			if( m_mapSkillAbility.end() == it )
				return 0.f;

			return it->second;
		}

#endif // UPGRADE_SKILL_SYSTEM_2013

		/** @function : GetCloneBuffFactorPtr
			@brief : 스킬이 가지고 있는 버프팩터의 클론을 얻어오는 함수
			@param : 얻으려고 하는 버프팩터의 인덱스
			@return : 스킬이 가지고 있는 버프팩터의 클론 또는 빈 스마트 포인터
		*/
		CX2BuffFactorPtr GetCloneBuffFactorPtr( const UINT uiIndex_ ) const
		{
			if ( uiIndex_ < m_vecBuffFactorPtr.size() && NULL != m_vecBuffFactorPtr[uiIndex_] )
				return m_vecBuffFactorPtr[uiIndex_]->GetClonePtr();
			else
				return CX2BuffFactorPtr();
		}

		/** @function	: SkillTemplet
			@brief		: 생성자, 속성 초기화
		*/

#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
		bool GetSkillEffectiveDescExpand ( OUT wstring & wstrSkillEffectiveDesc, int iLevel ) const;
#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편

#ifdef SERV_9TH_NEW_CHARACTER // 김태환
		/** @function	: GetSkillDPConsumptionValue
			@brief		: DP소비값을 가져옴
		*/
		const float GetSkillDPConsumptionValue() const { return static_cast<float>( m_usFPConsumtion ); }
#endif //SERV_9TH_NEW_CHARACTER

#ifdef REFORM_SKILL_NOTE_UI
		const WCHAR* GetIconTextureFileName()const{ return m_wstrIconName.c_str(); }
		const WCHAR* GetIconTextureKeyName()const{ return m_wstrIconPieceName.c_str(); }
#endif // REFORM_SKILL_NOTE_UI



		SkillTemplet() :
		m_bGuildSkill( false ),
		m_eType(ST_NONE ),
		m_eID(SI_NONE ),
	//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
	#ifdef	NEW_CHARACTER_CHUNG
		m_usCBConsumption( 0 ),
	#endif	NEW_CHARACTER_CHUNG
	//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
	#ifdef ARA_CHARACTER_BASE
		m_usFPGainCount( 0 ),
		m_usFPConsumtion( 0 ),
	#endif
		m_wstrName( L"" ),
		m_wstrMainDesc( L"" ),
		m_bBornSealed( false ),
		m_wstrIconName( L"" ),
		m_wstrIconPieceName( L"" ),
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		m_wstrImageName( L"" ),
		m_wstrImagePieceName( L"" ),
		m_iRequireLearnSkillPoint( 0 ),
		m_iRequireUpgradeSkillPoint( 0 ),
	#else // UPGRADE_SKILL_SYSTEM_2013
		m_iSkillLevel( 0 ),
		m_fSkillCoolTime( 0.f ),
		m_fMPConsumption( 0.f ),
		m_fPowerRate( 0.f ),
	#endif // UPGRADE_SKILL_SYSTEM_2013
		m_eActiveSkillUseCondtion( ASUT_NONE ),
		m_bShowSkillLevel( false )
	#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
		,m_eSkillPowerRateType(SPRT_NONE)
	#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
	#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
		,m_ePowerRateType(SPRT_NONE)
	#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
	#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		,m_iDeleteSkillIDByUpgrade( 0 )
	#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
		{
			m_vecBuffFactorPtr.clear();
			m_mapSkillAbility.clear();
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			m_vecStat.clear();
			m_vecSkillCoolTime.clear();
			m_vecMPConsumption.clear();
			m_vecPowerRate.clear();
			m_vecRequireCharacterLevel.clear();
			m_vecEffectiveDesc.clear();
	#endif // UPGRADE_SKILL_SYSTEM_2013
		}
	}; // struct SkillTemplet

#ifndef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

	struct SkillKey
	{
		SKILL_ID				m_eID;
		int						m_iSkillLevel;

		SkillKey() :
		m_eID( SI_NONE ),
			m_iSkillLevel( 0 )
		{
		}

		bool operator < ( const SkillKey& rhs ) const 
		{
			if( (int)m_eID < (int)rhs.m_eID )
				return true;
			else if( (int)m_eID > (int)rhs.m_eID )
				return false;

			if( m_iSkillLevel < rhs.m_iSkillLevel )
				return true;

			return false;
		}
	};

#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	typedef std::map<SKILL_ID, SkillTemplet*> SkillTempletMap;
#else // UPGRADE_SKILL_SYSTEM_2013
	typedef std::map<SkillKey, SkillTemplet*> SkillTempletMap;
#endif // UPGRADE_SKILL_SYSTEM_2013

	/// 스킬 트리 템플릿
	struct SkillTreeTemplet
	{
		int				m_iMaxSkillLevel;			/// 최대 스킬 레벨
		int				m_iTier;					/// 스킬트리에서 아이콘 위치, 세로로 몇 번째 row인지 0부터 시작
		int				m_iIndexInTier;				/// 스킬트리에서 아이콘 위치, 가로로 몇 번째 column인지 0부터 시작

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		int				m_iPrecedingSkill;			/// 선행 스킬 ID
		int				m_iFollowingSkill;			/// 스킬 되돌리기 할 때 필요
#else // UPGRADE_SKILL_SYSTEM_2013
		vector<int>		m_vecPrecedingSkill;		/// 선행 스킬 ID
		vector<int>		m_vecFollowingSkill;		/// 스킬 되돌리기 할 때 필요
#endif // UPGRADE_SKILL_SYSTEM_2013

		/** @function	: SkillTreeTemplet
			@brief		: 생성자, 속성 초기화
		*/
		SkillTreeTemplet()
		: m_iMaxSkillLevel( 0 )
		, m_iTier( 0 )
		, m_iIndexInTier( 0 )
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		, m_iPrecedingSkill( 0 )
		, m_iFollowingSkill( 0 )
#endif // UPGRADE_SKILL_SYSTEM_2013
		{}
	};
	typedef std::map< SKILL_ID, SkillTreeTemplet > SkillTreeTempletMap;		/// 스킬 아이디, 스킬 트리템플릿을 가지는 맵
	typedef std::map< int, SkillTreeTempletMap > UnitSkillTreeTempletMap;	/// < CX2Unit::UNIT_CLASS, SkillTreeTempletMap >

// oasis907 : 김상윤 [2009.12.2] // 본섭 파싱 위해 주석
//#ifdef GUILD_SKILL
	//{{ oasis907 : 김상윤 //// 2009-11-12 //// 
	typedef std::map< int, SkillTreeTempletMap > GuildSkillTreeTempletMap;	/// < CX2Unit::GUILD_CLASS, SkillTreeTempletMap >
	//}} oasis907 : 김상윤 //// 2009-11-12 //// 
//#endif GUILD_SKILL	


	//{{ 2009. 8. 5  최육사		봉인스킬정보
	struct SealSkillItemInfo
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		int 								m_iUnitClassType;	/// 사용 가능한 전직	CX2Unit::UNIT_CLASS_TYPE
		std::vector<CX2SkillTree::SKILL_ID>	m_vecSkillID;		/// 모든 전직의 봉인 스킬 아이디 저장 컨테이너

		SealSkillItemInfo()
		{
			m_iUnitClassType	= -1;
			m_vecSkillID.clear();
		}
#else // UPGRADE_SKILL_SYSTEM_2013
		int		m_iSkillID;
		char	m_cUnitClass;

		SealSkillItemInfo()
		{
			m_iSkillID = 0;
			m_cUnitClass = 0;
		}
#endif // UPGRADE_SKILL_SYSTEM_2013
	};
	//}}



public:
	static const WCHAR* GetAccelCoreParticleName( const SKILL_ID eAccelSkillID_ );

	CX2SkillTree();
	~CX2SkillTree();
	
	bool OpenScriptFile( const WCHAR* pFileName, const WCHAR* pSkillDataFileName );
	void OpenScriptFilePostProcess();
	bool AddSkillTemplet_LUA();
	bool AddSkillTreeTemplet_LUA();
#ifdef LUA_TRANS_DEVIDE
	bool AddSkillTempletTrans_LUA();
#endif LUA_TRANS_DEVIDE

	//{{ 2009. 8. 5  최육사		봉인 스킬 아이템 정보
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	bool AddSealSkillInfo_LUA();
#else // UPGRADE_SKILL_SYSTEM_2013
	bool AddSealSkillInfo_LUA( int iUnsealItemID, int iSkillID, char cUnitClass );
#endif // UPGRADE_SKILL_SYSTEM_2013
	//}}
	
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	const SkillTemplet* GetSkillTemplet( SKILL_ID eSkillID ) const;
#else // UPGRADE_SKILL_SYSTEM_2013
	const SkillTemplet* GetSkillTemplet( SKILL_ID eSkillID, int iSkillLevel ) const;
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	const SkillTempletMap& GetSkillTempletMap() const { return m_mapSkillTemplet; }
	const SkillTreeTempletMap& GetSkillTreeTempletMap( int eUnitClass );
#ifdef X2TOOL
	SkillTreeTempletMap& GetSkillTreeTempletMap4Tool( int eUnitClass ); 
#endif

	bool IsUnitTypeDefaultSkill( SKILL_ID eSkillID );

	const SkillTreeTemplet* GetSkillTreeTemplet( int iUnitClass, SKILL_ID eSkillID );

	// oasis907 : 김상윤 [2009.11.25] // 본섭 파싱 위해 이동
	bool AddGuildSkillTreeTemplet_LUA(); //{{ oasis907 : 김상윤 //// 2009-11-12 //// 
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	void    OpenScriptFilePostProcessLua() {}                           /// June binds the Lua name OpenScriptFilePostProcess to a bare RET ( 31f3f070 )
	int     GetUpgradedSkillID( int iSkillID_ ) const;                  /// June 32205c00: the "[Enhanced]" skill of a base skill, or -1
	bool    IsUpgradedSkill( SKILL_ID eSkillID_ ) const;                /// June 32206d00
	SKILL_ID GetUpgradeBaseSkillID( SKILL_ID eSkillID_ ) const;         /// June 32207210
	static const WCHAR* GetSkillAbilityName( int iType_ );              /// June g_SkillAbilityName
	static void LogShortArrayReach( int iSkillID_, int iKind_, int iLevel_, int iSize_ );
	void    SetSkillAbilityMap( KLuaManager* pLuaManager_, SKILL_ABILITY_TYPE eSkillAbility_, SkillTemplet* pSkillTemplet_, const char* pszKey_ );   /// 3220b740
	void    LogShortArray( int iSkillID_, int iKind_, int iSize_ );
	void    LogLoadSummary();
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET

	int GetMaxSkillLevel( int iUnitClass, SKILL_ID eSkillID );

	wstring GetSkillLevelTextureName( int iLevel );
	wstring GetSkillLevelTextureKey( int iLevel );

	//{{ 2009. 8. 5  최육사		봉인 스킬 아이템 정보
	bool IsUnsealSkillItemID( int iItemID ) { return ( m_mapSealSkillItemTemplet.find( iItemID ) != m_mapSealSkillItemTemplet.end() ); }

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	const int GetUnsealSkillItemRequireClass( const int iItemID ) const;

	const CX2SkillTree::SKILL_ID GetUnsealSkillItemInfo( const int iItemID ) const;
#else // UPGRADE_SKILL_SYSTEM_2013
	bool GetUnsealSkillItemInfo( int iItemID, int& iSkillID, char& cUnitClass );
#endif // UPGRADE_SKILL_SYSTEM_2013
	//}}


#ifdef GUILD_SKILL
	const SkillTreeTemplet* GetGuildSkillTreeTemplet( int iGuildClass, SKILL_ID eSkillID ); //{{ oasis907 : 김상윤 //// 2009-11-16 //// 

	int GetMaxGuildSkillLevel( int iGuildClass, SKILL_ID eSkillID );  //{{ oasis907 : 김상윤 //// 2009-11-16 //// 


	
	const SkillTreeTempletMap& GetGuildSkillTreeTempletMap( int eGuildClass ); //{{ oasis907 : 김상윤 //// 2009-11-12 //// 
#endif GUILD_SKILL

	//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
#ifdef PRINT_INGAMEINFO_TO_EXCEL
	void PrintSkillInfo_ToExcel();
#endif PRINT_INGAMEINFO_TO_EXCEL
	//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력

	//////////////////////////////////////////////////////////////////////////
	bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_CHANGE_SKILL_SLOT_REQ( int iSlotID, CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_CHANGE_SKILL_SLOT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#ifdef SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	/// June's slot-change queue: Handler_EGS_CHANGE_SKILL_SLOT_REQ only queues ( 3220e3b0 ), this sends ( 3220e5f0 ), and
	/// JuneFrameMoveSkillSlotQueue ( 322105c0, from CX2Data::OnFrameMove 31ee9f50 ) sends the front once per frame.
	bool SendChangeSkillSlotReq( int iSlotID, CX2SkillTree::SKILL_ID eSkillID );
	void FrameMoveSkillSlotQueue();
	/// June 32206d30: the class's tree skills with a required level equal to ( level / 5 ) * 5.
	void GetSkillListByLevel( int iUnitLevel, int iUnitClass, OUT std::vector< int >& vecSkillID_ ) const;
	/// the class's tree rows, or NULL ( GetSkillTreeTempletMap asserts and falls back to the first class ); the emulator's grant reads it
	const SkillTreeTempletMap* FindSkillTreeTempletMap( int iUnitClass ) const
	{
		UnitSkillTreeTempletMap::const_iterator mit = m_mapUnitSkillTreeTemplet.find( iUnitClass );
		return ( mit != m_mapUnitSkillTreeTemplet.end() ) ? &mit->second : NULL;
	}
	/// The block June's level-up handlers end with ( 323ae130 @323ae5c9, 32473f70 @32474a7f, 325346e0 @32534b1b ).
	void AutoEquipOnLevelUp();
	bool Handler_EGS_ACQUIRE_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
#ifdef SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM
	/// S7i: what opens at a level, by type ( 1 new skill, 2 a memo, 3 new upgraded skill, 4 next skill level ), June 3220d2b0
	typedef std::map< int, std::vector< const SkillTemplet* > > NewSkillOrMemoMap;
	void GetNewSkillOrMemoByLevel( int iLevel_, int iUnitClass_, OUT NewSkillOrMemoMap& mapNew_ ) const;
	static void AddNewSkillOrMemo( int iType_, const SkillTemplet* pTemplet_, IN OUT NewSkillOrMemoMap& mapNew_ );	/// June 3220b990
	/// June 31d885e0 ( a CX2Main method in June ): my unit's level-up feeds the emblem manager and plays emblem 17
	void ShowNewSkillOrMemoEmblem() const;
#endif SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	/// S7e: the data behind June's tree panels. JuneSkillCommandDesc is the tree UI's +0x3c block ( 0x50 bytes, reset by 328ee430 );
	/// a combo row is June's 0x34-byte record ( 328df4c0 / 3220d4d0 ); a command icon is one CommandIconData.lua row ( map +0xc4 ).
	struct DescGroup
	{
		int										m_iDescStringID;	/// DESCRIPTION_STRING_ID ( -1 when absent )
		std::vector< std::pair< int, int > >	m_vecSkill;			/// SKILL_ID read in pairs: { skill id, string id }
	};
	struct MajorCombo
	{
		std::string	m_strComboName;		/// MAJOR_COMBO row [1]: NORMAL_COMBO / DASH_COMBO / JUMP_COMBO / DASH_JUMP_COMBO ( "" when absent )
		int			m_iIndex;			/// [2]: the INDEX_n row of the class's ComboTree.lua table ( -1 when absent )
		int			m_iStringID;		/// [3]: the row's text ( -1 when absent )
	};
	struct SkillCommandDesc
	{
		DescGroup				m_kTranscendental;		/// +0x00 TRANSCENDENTAL_SKILL_DATA
		DescGroup				m_kSkill;				/// +0x14 SKILL_DATA ( the shown group; 328e42d0 copies +0x00 over it when transcended )
		int							m_iCommandDescStringID;	/// +0x28 COMMAND_DATA
		std::vector<MajorCombo>	m_vecMajorCombo;		/// +0x2c
		int							m_iMinorDescStringID;	/// +0x3c COMMAND_DATA_MINOR
		std::vector<MajorCombo>	m_vecMinorCombo;		/// +0x40
		SkillCommandDesc() { Reset(); }
		void Reset();				/// June 328ee430: the four ids to -1, the four vectors cleared
	};
	struct ComboRow
	{
		int					m_iComboType;	/// 32205870( table name ): 0 normal, 1 dash, 2 jump, 3 dash jump ( 0 for any other name )
		std::wstring		m_wstrMovie;	/// INDEX_n [1] ( 328df4c0 reads it; 3220d4d0 leaves it empty )
		std::vector<int>	m_vecIcon;		/// INDEX_n [2..]: COMBO_TREE_ICON values
		int					m_iStringID;	/// 3220d4d0: the MAJOR_COMBO row's text; 328df4c0: -1
		ComboRow() : m_iComboType( 0 ), m_iStringID( -1 ) {}
	};
	struct CommandIconData
	{
		std::wstring	m_wstrIconTextureName;		/// node +0x10 m_IconTextureName ( default DLG_UI_Common_Texture_NewSkill_02.TGA )
		std::wstring	m_wstrIconTetName;			/// +0x2c m_IconTetName ( a row without it is not added )
		std::wstring	m_wstrOutLineTextureName;	/// +0x48 m_OutLineTextureName ( default DLG_UI_Common_Texture_NewSkill_02.TGA )
		std::wstring	m_wstrOutLineTetName;		/// +0x64 m_OutLineTetName
		D3DXVECTOR3		m_vToolTipOutLineColor;		/// +0x80 m_ToolTipOutLineColor ( default 1, 1, 1 )
		float			m_fOffsetY;					/// +0x8c m_fOffsetY ( default 0 )
		std::vector< std::pair< int, int > >	m_vecToolTipString;	/// +0x90 m_ToolTipString rows: { unit type ( 0 when absent ), string id }
	};
	void LoadSkillCommandDescription( int iUnitClass_, bool bMinor_, SkillCommandDesc* pDesc_ );	/// 3220bdb0
	void LoadMajorComboRows( const SkillCommandDesc* pDesc_, const std::string& strClassName_, bool bMinor_,
		std::vector<ComboRow>& vecRow_ );																/// 3220d4d0
	static int GetComboType( const std::string& strTableName_ );		/// 32205870
	static int GetComboTitleStringID( int iComboType_ );				/// 32205610
	static std::string GetUnitClassName( int iUnitClass_ );			/// 32375e30 ( narrows 32375530's wide name )
	bool GetCommandIconTexture( int iIcon_, std::wstring& wstrTexture_, std::wstring& wstrKey_ ) const;		/// 32207110
	bool GetCommandOutLineTexture( int iIcon_, std::wstring& wstrTexture_, std::wstring& wstrKey_ ) const;	/// 32207190
	void AddCommandIconPicture( int iIcon_, float fX_, float fY_, CKTDGUIStatic* pStatic_ ) const;			/// 32207630
	void AddCommandIconData_LUA();		/// 3220f8d0
	void OpenCommandIconData();		/// 32207dd0 ( OpenScriptFile's tail @322092f2 )
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	/// S7c: June's memo ACKs ( CX2State handlers 31f1bdb0 / 31f1c1d0; March routes skill packets here from every state )
	bool Handler_EGS_ACTIVE_SKILL_MEMO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_DEACTIVE_SKILL_MEMO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	/// both ACKs then rebuild every equipped skill of my game unit ( 31fd84c0 = ChangeEquippedSkillState, slots 0-4 of bar A and B )
	static void RefreshEquippedSkillsForMemo();
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET




	bool Handler_EGS_CHANGE_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	#ifdef SKILL_PAGE_SYSTEM //JHKang
	bool Handler_EGS_GET_SKILL_REQ( USHORT usPageNumber_ = 0 );
	#else //SKILL_PAGE_SYSTEM
	bool Handler_EGS_GET_SKILL_REQ();
	#endif //SKILL_PAGE_SYSTEM
#else // UPGRADE_SKILL_SYSTEM_2013
	bool Handler_EGS_GET_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID );
#endif // UPGRADE_SKILL_SYSTEM_2013

	bool Handler_EGS_GET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	bool Handler_EGS_RESET_SKILL_REQ( CX2SkillTree::SKILL_ID eDelSkillID = CX2SkillTree::SI_NONE );
#else // UPGRADE_SKILL_SYSTEM_2013
	bool Handler_EGS_RESET_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID );
#endif // UPGRADE_SKILL_SYSTEM_2013
	bool Handler_EGS_RESET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_INIT_SKILL_TREE_REQ( UidType itemUID );
	bool Handler_EGS_INIT_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_EXPIRE_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_UPDATE_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_EXPAND_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_EXPIRE_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

#ifdef SERV_SKILL_SLOT_CHANGE_INVENTORY
	bool Handler_EGS_SKILL_SLOT_CHANGE_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif SERV_SKILL_SLOT_CHANGE_INVENTORY

	bool Handler_EGS_UNSEAL_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	void On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( KEGS_EXPIRE_CASH_SKILL_POINT_NOT kEvent );

#ifdef GUILD_SKILL
	//{{ oasis907 : 김상윤 //// 2009-11-12 //// 

	// oasis907 : 김상윤 [2009.11.27] // 길드 스킬에 액티브 스킬이 없는 이상 사용x
	/*
	bool Handler_EGS_CHANGE_GUILD_SKILL_SLOT_REQ( int iSlotID, CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_CHANGE_GUILD_SKILL_SLOT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_CHANGE_GUILD_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	*/


	bool Handler_EGS_GET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_GET_GUILD_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	bool Handler_EGS_GET_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : 김상윤 [2009.11.19] //

	bool Handler_EGS_RESET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_RESET_GUILD_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_RESET_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : 김상윤 [2009.11.19] //

	bool Handler_EGS_INIT_GUILD_SKILL_TREE_REQ( UidType itemUID );
	bool Handler_EGS_INIT_GUILD_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_INIT_GUILD_SKILL_TREE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : 김상윤 [2009.11.19] //

	bool Handler_EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : 김상윤 [2009.11.27] //
	
	void Hander_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ( const int iYear_, const int iMonth_, const int iDay_, const int iHour_, const int iMinute_ );
	void Handler_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	//}} oasis907 : 김상윤 //// 2009-11-12 //// 
#endif GUILD_SKILL

#ifdef SKILL_PAGE_SYSTEM //JHKang
	bool Handler_EGS_GET_NEXT_SKILL_PAGE_ED_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_EXPAND_SKILL_PAGE_REQ();
	bool Handler_EGS_EXPAND_SKILL_PAGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ( USHORT usPageNumber_ );
	bool Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif //SKILL_PAGE_SYSTEM

	//{{ kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
	// 스킬슬롯체인지 중인가? (SkillSlotChangeReq를 전송한 후 Ack를 아직 받지 못한 상태인가?)
	bool IsSkillSlotChanging() const { return m_bSkillSlotChanging; }
	void SetSkillSlotChanging(bool val) { m_bSkillSlotChanging = val; }
#endif	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
	//}} kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정

#ifdef ADDED_RELATIONSHIP_SYSTEM
	void UnequipRelationSkill();
#endif // ADDED_RELATIONSHIP_SYSTEM

#ifdef UPGRADE_SKILL_SYSTEM_2013
	/// 스킬 템플릿 내 값 설정 함수
	void SetSkillAbilityMap( KLuaManager* pLuaManager, SKILL_ABILITY_TYPE eSkillAbility, CX2SkillTree::SkillTemplet* pSkillTemplet );
	template< typename T >
#ifdef HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC // 김태환
	void SetSkillVlaue( KLuaManager& pLuaManager, vector<T>& vecValue, bool bAutoFill = true );
#else //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
	void SetSkillVlaue( KLuaManager& pLuaManager, vector<T>& vecValue );
#endif //HIDE_DO_NOT_EXIST_SKILL_EFFECTIVE_DESC
	void SetSkillStat( KLuaManager& pLuaManager, vector<CX2Stat::Stat>& vecStat );

	/// 스킬 습득 혹은 강화시 서버로 넘기기 위한 변동값 처리
	const std::map<int, KGetSkillInfo>& GetMapSkillInfo() const { return m_mapGetSkillInfo; }
	void SetMapSkillInfo(std::map<int, KGetSkillInfo> val) { m_mapGetSkillInfo = val; }
	void ClearMapSkillInfo() { m_mapGetSkillInfo.clear();
#ifdef SERV_SUB_QUEST_LEARN_NEW_SKILL
	m_vecNowLearnSkill.clear();
#endif SERV_SUB_QUEST_LEARN_NEW_SKILL	
	}

	/// 스킬 습득 혹은 강화시 서버로 넘기기 위한 변동값 저장 컨테이너의 값 수정 혹은 추가 함수
	void FixOrInsertMapSkillInfoValue( CX2SkillTree::SKILL_ID eSkillID );

	/// 현재까지 사용한 스킬 포인트
	const int GetUsedSPoint() const { return m_iUsedSPoint; }
	void SetUsedSPoint( int val ) { m_iUsedSPoint = val; }

	/// 현재까지 사용한 캐시 스킬 포인트
	const int GetUsedCSPoint() const { return m_iUsedCSPoint; }
	void SetUsedCSPoint( int val ) { m_iUsedCSPoint = val; }

	/// 직업별 데미지 배율 테이블 파싱 함수
	void AddSkillPowerRate_LUA();
	void SetPowerRateValue( SKILL_POWER_RATE_TYPE eSkillPowerRateType );
	void SetSkillPowerRate( IN SKILL_POWER_RATE_TYPE eSkillPowerRateType, OUT vector<float>& vecPowerRate );
	const std::map<SKILL_POWER_RATE_TYPE, std::vector<float>>& GetMapSkillPowerRateType() const { return m_mapSkillPowerRateType; }

	/// 직업별 기본 스킬 테이블 파싱 함수
	void AddDefaultSkill_LUA();
	const std::map<int, std::vector<int>>& GetMapDefaultSkill() const { return m_mapDefaultSkill; }

	/// 해당 스킬 아이디가 기본 스킬인지 여부
	bool isDefaultSkill( SKILL_ID eSkillID );

	/// 현재 케릭터의 스킬인지 검사
	bool IsMyClassSkillID( SKILL_ID eSkillID );

	/// 최대 제한 스킬 레벨값 반환
	const int GetMaxLimitedSkillLevel() { return MAX_LIMITED_SKILL_LEVEL; }

	/// 봉인 스킬 템플릿 반환
	const map<int, SealSkillItemInfo>& GetSealSkillItemTemplet() const { return m_mapSealSkillItemTemplet; }

	/// 레벨에 따른 총 스킬 포인트 반환
	const int GetCalcInitSkillPoint( const int iUnitLevel ) const;

	void SetSkillTemplet( SKILL_ID  eSkillID, SkillTemplet* pSkillTemplet )
	{
		if ( NULL != pSkillTemplet )
			m_mapSkillTemplet.insert( std::make_pair( eSkillID, pSkillTemplet ) );
	}
#else // UPGRADE_SKILL_SYSTEM_2013
	void SetSkillTemplet( SkillKey skillKey, SkillTemplet* pSkillTemplet)
	{
		if ( NULL != pSkillTemplet )
			m_mapSkillTemplet.insert( std::make_pair( skillKey, pSkillTemplet ) );
	}
#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
	UINT GetSkillLevelUpNumByPowerRateType( SKILL_POWER_RATE_TYPE eSkillPowerRateType_ );
#endif // SKILL_LEVEL_UP_BY_POWER_RATE_TYPE
private:
	SkillTempletMap			m_mapSkillTemplet;
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경

	std::map<SKILL_POWER_RATE_TYPE, std::vector<float>>	m_mapSkillPowerRateType;	/// 각 직업에 따른 데미지 배율 저장 컨테이너
	std::map<int, std::vector<int>>						m_mapDefaultSkill;			/// 각 직업에 따른 기본 스킬 저장 컨테이너
	std::map<int, KGetSkillInfo>						m_mapGetSkillInfo;			/// 스킬 습득 혹은 강화시 서버로 넘기기 위한 변동값 저장 컨테이너

	bool					m_bNowLearnSkill;				/// 현재 스킬창에서 스킬 레벨을 올리고 있는가
	int						m_iUsedSPoint;					/// 스킬 포인트 소비량 검사를 위한 임시 SP 저장값
	int						m_iUsedCSPoint;					/// 캐시 스킬 포인트 소비량 검사를 위한 임시 캐시 SP 저장값
#endif // UPGRADE_SKILL_SYSTEM_2013
	UnitSkillTreeTempletMap	m_mapUnitSkillTreeTemplet;
//#ifdef GUILD_SKILL // oasis907 : 김상윤 [2009.11.25] // 본섭 파싱 위해 주석 해제
	//{{ oasis907 : 김상윤 //// 2009-11-12 //// 
	GuildSkillTreeTempletMap m_mapGuildSkillTreeTemplet;
	//}} oasis907 : 김상윤 //// 2009-11-12 //// 
//#endif GUILD_SKILL

	//{{ 2009. 8. 5  최육사		봉인스킬정보
	map< int, SealSkillItemInfo > m_mapSealSkillItemTemplet;
	
	//}}
	

#ifdef GUILD_SKILL
	//{{ oasis907 : 김상윤 //// 2009-11-17 ////	임시
	SkillTreeTempletMap nulltemplet;
	//}} oasis907 : 김상윤 //// 2009-11-17 //// 
#endif GUILD_SKILL

	//{{ kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
	bool m_bSkillSlotChanging;			// 스킬슬롯체인지 중인가? (SkillSlotChangeReq를 전송한 후 Ack를 아직 받지 못한 상태인가?)
#endif	MODIFY_INFINITE_SKILL_BUG_WHEN_INTRUDE
#ifdef SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	std::deque< std::pair< int, int > >	m_deqSkillSlotChange;	/// June tree +0xd4: queued ( slot, skill ) changes
#endif SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	//}} kimhc // 2010.7.29 // 난입시 무한 스킬 버그 수정

#ifdef SERV_SUB_QUEST_LEARN_NEW_SKILL
	vector<int>	m_vecNowLearnSkill;
#endif SERV_SUB_QUEST_LEARN_NEW_SKILL
#ifdef SERV_IRUHADEV_JUNE_SKILL_TEMPLET
	struct LoadStats
	{
		int     m_iNumericalBlock;
		int     m_iExtraIcon;
		int     m_iEmptySASkipped;
		int     m_iDuplicate;
		int     m_iInHouseDropped;
		int     m_iShortCoolTime;
		int     m_iShortMP;
		int     m_iShortReqLevel;
		std::set<std::string>   m_setKeysSeen;

		LoadStats()
			: m_iNumericalBlock( 0 ), m_iExtraIcon( 0 ), m_iEmptySASkipped( 0 ), m_iDuplicate( 0 ), m_iInHouseDropped( 0 )
			, m_iShortCoolTime( 0 ), m_iShortMP( 0 ), m_iShortReqLevel( 0 )
		{}
	};

	std::map<int, int>  m_mapUpgradeSkill;      /// June CX2SkillTree +0x20: base skill id -> "[Enhanced]" skill id ( filled by m_DeleteSkillIDByUpgrade )
	LoadStats       mLoadStats;
	bool                m_bScratch;         /// a throw-away debug tree: no buff factor parse
#endif SERV_IRUHADEV_JUNE_SKILL_TEMPLET
#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
	std::map< int, CommandIconData >	m_mapCommandIcon;	/// June CX2SkillTree +0xc4: CommandIconData.lua by COMBO_TREE_ICON
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS
}; // class CX2SkillTree
