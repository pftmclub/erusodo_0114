#pragma once

class CX2StateStartUp : public CX2State
{
public:
	CX2StateStartUp(void);
	virtual ~CX2StateStartUp(void);

protected:
	void LoadData();

#ifdef FIRST_SCREEN_TEST
	public:
#ifdef SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT
		virtual HRESULT OnFrameRenderFrontUI( int iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT
		virtual HRESULT OnFrameRender();
#endif SERV_IRUHADEV_JUNE_RENDER_STAGE_SPLIT

	private:
		CKTDXDeviceTexture*		m_pTexutureBG;
#endif // FIRST_SCREEN_TEST

};
