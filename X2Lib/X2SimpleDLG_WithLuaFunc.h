//{{ Iruha : 2026-10-15 // June effect C3z-4s: June 2017's CX2SimpleDLG_WithLuaFunc ( implementation in X2Game.cpp )
#ifdef SERV_IRUHADEV_JUNE_SIMPLE_DLG
#pragma once

/** @class : CX2SimpleDLG_WithLuaFunc
	@brief : June 2017 ( 0x14 B, vtable 334b3438 ): one dialog file that a dungeon script creates on the game
			 ( CX2Game +0xba8, CreateSimpleDLG 322def10 ) and drives through 12 Lua methods ( binder 31d95080
			 @31e42615..@31e43fde ). Users: Ranox Secret Hell ( SECRET_SCENE_DUMMY.lua ), Elysion Soless / Herbaon.
			 Layout: +0x4/+0x8 the dialog handle, +0xc the Lua number, +0x10 use-external-condition, +0x11 the
			 skill-UI type it was last placed for.
*/
class CX2SimpleDLG_WithLuaFunc
{
public:
	CX2SimpleDLG_WithLuaFunc( const WCHAR* wszFileName_ );		// June 326b24a0
	virtual ~CX2SimpleDLG_WithLuaFunc();							// June 326b1320

	void	UpdateExternalCondition();								// June 326b18c0

	void	SetShow_LUA( bool bShow_ );								// June 326b1380
	void	SetNumberInNumberStatic_LUA( const char* pStaticName_, int iNumber_, bool bShowTensZero_ );	// June 326b1bb0
	void	SetNumberInGageStatic_LUA( const char* pStaticName_, int iNumber_ );	// June 326b1d40
	void	SetNumber_LUA( int iNumber_ ) { m_iNumber = iNumber_; }	// June 31e4d8a0
	int		GetNumber_LUA() { return m_iNumber; }					// June 32e4ea40
	void	CrashAndColorChange_LUA();								// June 326b1a10
	void	CrashAndColorChangeCustom_LUA( float fRed_, float fGreen_, float fBlue_, float fAlpha_, float fTime_,
				float fMinX_, float fMinY_, float fMaxX_, float fMaxY_ );	// June 326b1400
	void	SetPercentValueInGageStatic_LUA( const char* pStaticName_, int iPictureIndex_, const char* pTexKey_, float fPercent_ );	// June 326b1e40
	void	SetShowStatic_LUA( const char* pStaticName_, bool bShow_ );	// June 326b1f80
	void	SetNumberInNumberStatic_MoreParam_LUA( const char* pStaticName_, int iNumber_, bool bShowTensZero_, bool bShowWhenZero_ );	// June 326b2070
	void	SetUseExternalCondition_LUA( bool bUse_ );				// June 326b1a90
	void	SetNumberInNumberStatic_MoreDigit_LUA( const char* pStaticName_, int iNumber_, int iDigit_, bool bShowZero_ );	// June 326b2170

	static void	RegisterLuaBind( lua_State* L );

private:
	void	CreateDLG( const WCHAR* wszFileName_ );					// June 326b2340
	CKTDGUIStatic*	GetStatic( const char* pStaticName_ );

	static void	SetNumberInGageStatic( CKTDGUIStatic* pStatic_, int iNumber_ );	// June 326b13c0
	static void	SetNumberInNumberStatic_MoreParam( CKTDGUIStatic* pStatic_, int iNumber_, bool bShowTensZero_, bool bShowWhenZero_ );	// June 326b1940
	static void	SetPercentValueInGageStatic( CKTDGUIStatic* pStatic_, int iPictureIndex_, const wstring& wstrTexKey_, float fPercent_, bool bHorizontal_ );	// June 326b1720

	CKTDGUIDialogType	m_pDLG;							/// June +0x4 / +0x8
	int					m_iNumber;						/// June +0xc
	bool				m_bUseExternalCondition;		/// June +0x10
	bool				m_bPlacedForSkillUITypeA;		/// June +0x11 ( ctor 1 )
};
#endif SERV_IRUHADEV_JUNE_SIMPLE_DLG
//}}
