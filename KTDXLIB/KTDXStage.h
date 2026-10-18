#pragma once


class CKTDXDeviceHolder;
class CKTDXStage : public CKTDXDeviceHolder
{
	public:
		enum STAGE_STATE
		{
			SS_NULL = 0,
			SS_ACTIVE,
			SS_DEACTIVE,
			SS_READY_TO_DELETE,
		};

	public:
		CKTDXStage(){ m_eStageState = SS_ACTIVE; }
		virtual ~CKTDXStage(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime ){ return S_OK; }
		virtual HRESULT OnFrameRender(){ return S_OK; }
#ifdef SERV_IRUHADEV_JUNE_RENDER_FRAME
		/// June CKTDXStage vtable 33446fa8: 0x8 OnFrameRender ( 3231f510, empty ), then the pass slots CKTDXApp::OnFrameRender ( 31be3800 )
		/// runs over the stage chain after it. A March class that overrides only OnFrameRender keeps drawing everything there ( R4c / R4d split them ).
		virtual HRESULT OnFrameRenderAlpha( int /*iRenderPass_*/ ){ return S_OK; }		/// 0xc ( 330f9c30 ): alpha objects of the pass
		virtual HRESULT OnFrameRenderFrontUI( int /*iRenderPass_*/ ){ return S_OK; }		/// 0x10 ( 330f9c30 ): front UI of the pass
		virtual HRESULT OnFrameRenderAll()															/// 0x14 ( 31d28530; June has no caller )
		{
			OnFrameRender();
			OnFrameRenderAlpha( 0 );
			OnFrameRenderFrontUI( 0 );
			return S_OK;
		}
		virtual HRESULT OnFrameRenderAfter(){ return S_OK; }								/// 0x18 ( 3231f510 ): after the frame ( GFx )
#endif SERV_IRUHADEV_JUNE_RENDER_FRAME

		virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){ return false; }

		virtual HRESULT OnResetDevice(){ return S_OK; }
		virtual HRESULT OnLostDevice(){ return S_OK; }

		STAGE_STATE		GetStageState(){ return m_eStageState; }
		void			SetStageState( STAGE_STATE eStageState ){ m_eStageState = eStageState; }


		virtual void	SetLuaFrameMoveFunc_LUA( const char* pFuncName ) {}

		void			SetStageName( const WCHAR* wszStageName ) { m_StageName = wszStageName; }
		wstring			GetStageName() { return m_StageName; }

	protected:
		STAGE_STATE				m_eStageState;
		wstring					m_StageName;	// FieldFix: 윈도우 모드에서 X버튼 누르는 부분에 대한 처리 외에 딱히 사용하는 곳이 없어서 bool로 해도 될듯
};
