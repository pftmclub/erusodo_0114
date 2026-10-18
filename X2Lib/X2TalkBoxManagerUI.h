#pragma once

class CX2TalkBoxManagerUI : public CKTDGUIControl
{

	public:

		CX2TalkBoxManagerUI(void);
		virtual ~CX2TalkBoxManagerUI(void);

		HRESULT OnFrameMove( double fTime, float fElapsedTime );
#ifdef SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
		// June 32eec4a0 ( the imp's pass is R4h )
		HRESULT OnFrameRender( int iRenderPass_ = 0 );
#else SERV_IRUHADEV_JUNE_UI_CONTROL_PASS
		HRESULT OnFrameRender();
#endif SERV_IRUHADEV_JUNE_UI_CONTROL_PASS

		//void Push(TalkBox* talkBox) { Push( *talkBox ); }
		void Push( CX2TalkBoxManagerImp::TalkBox& talkBox );
		bool Delete(int unitUID, bool bTraceUnit = false, CX2GameUnit::GAME_UNIT_TYPE gameUnitType = CX2GameUnit::GUT_USER );
		void Clear();

		void SetPos( UidType unitUID, D3DXVECTOR2 pos, int boxType, bool bTraceUnit = false, CX2GameUnit::GAME_UNIT_TYPE gameUnitType = CX2GameUnit::GUT_USER );
		void SetPos( int talkBoxID, D3DXVECTOR2 pos, int boxType, bool bTraceUnit = false, CX2GameUnit::GAME_UNIT_TYPE gameUnitType = CX2GameUnit::GUT_USER );

		int GetMaxStrLen();

	protected:

		CX2TalkBoxManagerImp* m_pTalkBoxManagerImp;
};
