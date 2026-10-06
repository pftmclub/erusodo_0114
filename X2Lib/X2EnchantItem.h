#pragma once

class CX2GameUnit;
class CX2GUUser;
class CX2EnchantItem
{
	public:
		enum ENCHANT_TYPE
		{
			ET_NONE = 0,

			ET_PREFIX_START,
			ET_BLAZE		= ET_PREFIX_START,	// RED		, BLAZE		, prefix
			ET_WATER,							// BLUE		, FROZEN	, prefix
			ET_NATURE,							// GREEN	, POISON	, prefix
			ET_PREFIX_END	= ET_NATURE,

			ET_SUFFIX_START,
			ET_WIND			= ET_SUFFIX_START,	//			, PIERCING	, postfix
			ET_LIGHT,							//			, SHOCK		, postfix
			ET_DARK,							//			, SNATCH	, postfix
			ET_SUFFIX_END	= ET_DARK,

			ET_RANDOM,
			ET_END,
		};

		enum ENCHANT_SLOT_ID
		{
			ESI_SLOT_1 = 0,
			ESI_SLOT_2,
			ESI_SLOT_3,
		};

		
		enum ATTRIB_ITEM_ID
		{
			ATI_UNKNOWN			= 130047, // 무속성
			ATI_RED				= 130048, // 불
			ATI_BLUE			= 130049, // 물
			ATI_GREEN			= 130051, // 자연
			ATI_WIND			= 130050, // 바람
			ATI_LIGHT			= 130053, // 빛
			ATI_DARK			= 130052, // 암흑

			ATI_IDENTIFY_STONE	= 130054, // 감정석
			ATI_UNKNOWN_STONE	= 130055, // 복구석

#ifdef FINALITY_SKILL_SYSTEM //JHKang
			ATI_HYPER_SKILL_STONE	= 130208, /// 궁극기 사용에 소모되는 엘의 정수
#endif //FINALITY_SKILL_SYSTEM
		};


		enum ATTRIB_COUNT_TYPE
		{
			ACT_NONE = 0,
			ACT_SINGLE,
			ACT_DUAL,
			ACT_TRIPLE,
		};


		enum ATTRIB_ENCHANT_TYPE
		{
			AET_NONE = 0,

			AET_SINGLE_WEAPON,
			AET_DUAL_WEAPON,
			AET_ARMOR,

			AET_TRIPLE_WEAPON,
			AET_DUAL_ARMOR,
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
			/// June buff port Phase 15: June's 4th weapon / 3rd armour attribute types (Enum.lua ATTRIB_ENCHANT_TYPE)
			AET_QUADRA_WEAPON	= 6,
			AET_TRIPLE_ARMOR	= 7,
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
		};

		enum ENCHANT_ARMOR_RESIST
		{
			EAR_VALUE			= 75,
			EAR_MASTER_VALUE	= 110,
			EAR_VALUE_FOR_NPC	= 150,
			EAR_MAX_VALUE		= 500,
		};



		struct ItemEnchantedAttribute
		{
			CX2EnchantItem::ENCHANT_TYPE m_aEnchantedType[3];

			ItemEnchantedAttribute()
			{
				Init();
			}

			void Init()
			{
				m_aEnchantedType[0] = CX2EnchantItem::ET_NONE;
				m_aEnchantedType[1] = CX2EnchantItem::ET_NONE;
				m_aEnchantedType[2] = CX2EnchantItem::ET_NONE;
			}
		};




		struct EnchantData
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE m_ExtraDamageType;
			
			float m_fRate;					//요것만 float로 하고

			int m_Time;						//지속시간

#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
			float m_FirstDamagePercent;		/// June buff port Phase 15: float in June (32fea370; June data has 37.5, 112.5)
			float m_SecondDamagePercent;
			float m_ThirdDamagePercent;
#else SERV_IRUHADEV_JUNE_ENCHANT_PARITY
			int m_FirstDamagePercent;		//이것들은 정수로
			int m_SecondDamagePercent;
			int m_ThirdDamagePercent;
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

			int m_FirstSlowPercent;
			int m_SecondSlowPercent;
			int m_ThirdSlowPercent;


			int m_FirstDefenceDebuffPercent;
			int m_SecondDefenceDebuffPercent;
			int m_ThirdDefenceDebuffPercent;


			float m_fTimeStun;

			int m_HPDrainPercent;
			int m_MPDrain;
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
			/// June buff port Phase 15: June EnchantData +0x3c..+0x70 (loader 32708f00)
			float m_fAttribReiterationRate;		/// +0x3c, read by the accumulation modifier 32272d90
			float m_fBlazeMpDrain;				/// +0x40, blaze tick MP drain
			float m_fPiercingDamageFactor;		/// +0x44, GetFinalDamage piercing term
			int m_iShockDebuffTime;				/// +0x48, shock debuff (BFI 1000) time
			float m_fShockDebuffDamageFactor;	/// +0x4c, shock debuff factor
			float m_arrMovementSpeedDecrease[3];	/// +0x50, per accumulation 1..3 (Lua int list)
			float m_arrJumpSpeedDecrease[3];		/// +0x5c
			float m_arrAttackSpeedDecrease[3];	/// +0x68
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

			EnchantData()
			{
				m_ExtraDamageType = CX2DamageManager::EDT_NONE;

				m_fRate					= 0;

				m_Time					= 0;

				m_FirstDamagePercent	= 0;
				m_SecondDamagePercent	= 0;
				m_ThirdDamagePercent	= 0;

				m_FirstSlowPercent		= 0;
				m_SecondSlowPercent		= 0;
				m_ThirdSlowPercent		= 0;

				m_FirstDefenceDebuffPercent		= 0;
				m_SecondDefenceDebuffPercent	= 0;
				m_ThirdDefenceDebuffPercent		= 0;


				m_fTimeStun				= 0.0f;

				m_HPDrainPercent		= 0;
				m_MPDrain				= 0;
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
				/// June's default zeroes only March's 15 dwords; the port zeroes the June tail too (a missing list would be garbage in June)
				m_fAttribReiterationRate	= 0.0f;
				m_fBlazeMpDrain				= 0.0f;
				m_fPiercingDamageFactor		= 0.0f;
				m_iShockDebuffTime			= 0;
				m_fShockDebuffDamageFactor	= 0.0f;
				for ( int iJune = 0; iJune < 3; iJune++ )
				{
					m_arrMovementSpeedDecrease[iJune]	= 0.0f;
					m_arrJumpSpeedDecrease[iJune]		= 0.0f;
					m_arrAttackSpeedDecrease[iJune]		= 0.0f;
				}
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY
			}
		};

	


	public:
		CX2EnchantItem(void);
		virtual ~CX2EnchantItem(void);

		bool			OpenScriptFile( WCHAR* pFileName );
		const EnchantData*	GetEnchantData( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType ) const; // 2008.10.30 수정 - 김태완 
		const EnchantData*	GetNPCEnchantData( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType ) const;
		bool			AddEnchantData_LUA();
		//{{ 2008. 9. 19  최육사	추가
		bool			AddEnchantRequire_LUA( int iAttribEnchantType, int iCharLv );
		//}}

		// 속성강화 요구사항 함수
		bool	GetAttribEnchantRequireMagicStoneCount( bool bWeapon, int iCurrEnchantedCount, int iEqipLevel, int iItemGrade, int& iRequireCount );
		bool	GetAttribEnchantRequireED( bool bWeapon, int iCurrEnchantedCount, int iEqipLevel, int iItemGrade, int& iRequireED );
		

		CX2DamageManager::EXTRA_DAMAGE_TYPE GetExtraDamageType( CX2EnchantItem::ENCHANT_TYPE eEnchantedType );
		CX2DamageManager::EXTRA_DAMAGE_TYPE GetExtraDamageType( const std::vector< CX2EnchantItem::ENCHANT_TYPE >& vecAttribEnchant );
		CX2DamageManager::EXTRA_DAMAGE_TYPE GetExtraDamageType( const CX2EnchantItem::ItemEnchantedAttribute& enchantedAttribute );
		CX2DamageManager::EXTRA_DAMAGE_TYPE GetExtraDamageType( CX2EnchantItem::ItemEnchantedAttribute currEnchantedAttribute, CX2EnchantItem::ENCHANT_TYPE newEnchantType );



	
		static int GetEnchantResistValue( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType );

		int	GetItemID( ENCHANT_TYPE enchantType );

		static bool CanEnchantAttribute( const CX2EnchantItem::ItemEnchantedAttribute& currEnchantedAttribute, CX2EnchantItem::ENCHANT_TYPE newEnchantType );
		
		CX2DamageManager::EXTRA_DAMAGE_TYPE GetBaseExtraDamageType( CX2DamageManager::EXTRA_DAMAGE_TYPE eEDT );
		static int DecomposeEnchantExtraDamage( CX2DamageManager::EXTRA_DAMAGE_TYPE eComposedEDT, CX2DamageManager::EXTRA_DAMAGE_TYPE& eDecomposedEDT1, CX2DamageManager::EXTRA_DAMAGE_TYPE& eDecomposedEDT2 );

		bool ProcessEnchant( CX2GameUnit* pDefender, CX2DamageManager::DamageData* pDamageData, float fRandValue1, float fRandValue2 );
#ifdef SERV_IRUHADEV_JUNE_ENCHANT_PARITY
		/// June 32708c50: false only in a PvP game on a free channel; there the enchant damage skips ENCHANT_STAT_SCALE
		static bool JuneIsEnchantStatScaleOn();
#endif SERV_IRUHADEV_JUNE_ENCHANT_PARITY

		//{{ JHKang // 강정훈 // 2010. 10. 6 // 자동 줄나누기 두 번 실행 방지
#ifdef FIX_TOOLTIP
		wstring GetToolTip( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, CX2Item* pItem, bool bShowName = true, bool bTotal = false );
#else
		wstring GetToolTip( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, CX2Item* pItem, bool bShowName = true );
#endif FIX_TOOLTIP
		//}} JHKang // 강정훈 // 2010. 10. 6 // 자동 줄나누기 두 번 실행 방지
		wstring GetToolTipForPure( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, CX2Item* pItem );
		

		wstring GetEnchantResistName( CX2DamageManager::EXTRA_DAMAGE_TYPE enchantedCombination, bool bTwoLine = false );
		wstring GetEnchantResistToolTip( CX2DamageManager::EXTRA_DAMAGE_TYPE enchantedCombination, bool bTwoLine = false );


		static const wstring GetSlashTraceTextureName( CX2Unit::UNIT_TYPE eUnitTYpe, CX2DamageManager::EXTRA_DAMAGE_TYPE eExtraDamageType );


	protected:		

		


		int GetAttribEnchantType( bool bWeapon, int iCurrEnchantedCount );
		int		GetRequireEDFactor( int iItemGrade );

#ifdef  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        typedef std::map< CX2DamageManager::EXTRA_DAMAGE_TYPE, EnchantData >    EnchantDataMap;
#else   X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        typedef std::map< CX2DamageManager::EXTRA_DAMAGE_TYPE, EnchantData* >    EnchantDataMap;
#endif  X2OPTIMIZE_REMOVE_UNNECESSARY_PTR
        EnchantDataMap      m_mapEnchantData;
        EnchantDataMap      m_mapEnchantDataForNPC;

		//{{ 2008. 9. 19  최육사	추가
		typedef std::map< std::pair< int, int >, std::map< int, int > > MapEnchantRequire;
		MapEnchantRequire										 m_mapEnchantRequireInfo;
		//}}		
};



