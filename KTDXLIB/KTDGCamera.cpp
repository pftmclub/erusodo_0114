// KTDGCamera.cpp: implementation of the KTDGCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KTDGCamera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKTDGCamera::CKTDGCamera( LPDIRECT3DDEVICE9 pd3dDevice )
: m_fLeftCrashGap( 0.f )
, m_fLeftCrashSpeed( 0.f )
, m_fLeftCrashAccel( 0.f )
, m_fRightCrashGap( 0.f )
, m_fRightCrashSpeed( 0.f )
, m_fRightCrashAccel( 0.f )
, m_bLeftRightCrashPositive( false )
, m_fLeftRightCrashGap( 0.f )
, m_fLeftRightCrashTime( 0.f )
, m_fLeftRightCrashTimeGap( 0.f )
, m_fLeftRightCrashTimeGapInit( 0.f )

, m_fImpactForwardDistance( 0.f )
, m_fImpactForwardSpeed( 0.f )
, m_fImpactForwardBounceSpeed( 0.f )
, m_fImpactForwardDamping( 0.f )	
, m_fImpactForwardBounceRadian( 0.f )
, m_fImpactForwardCurrentBounceRange( 0.f )
, m_iImpactForwardMaxBounceCount( 0 ) 
, m_iImpactForwardCurrentBounceCount( 0 )
{
	ZeroMemory( &m_Camera, sizeof(CameraData) );
	D3DXMatrixIdentity(&m_Camera.m_ViewMatrix);

	m_fTrackTime		= 0;
	m_fTrackAtTime		= 0.0f;

	m_fDownCrashGap		= 0.0f;
	m_fDownCrashSpeed	= 0.0f;
	m_fDownCrashAccel	= 0.0f;

	m_fUpCrashGap		= 0.0f;
	m_fUpCrashSpeed		= 0.0f;
	m_fUpCrashAccel		= 0.0f;

	m_bUpDownCrashPositive	= false;
	m_fUpDownCrashGap		= 0.0f;
	m_fUpDownCrashTime		= 0.0f;
	m_fUpDownCrashTimeGap	= 0.0f;
	m_fUpDownCrashTimeGapInit	 = 0.f;

#ifdef KEYFRAME_CAMERA
	m_bFixedUpVec			= true;
#endif KEYFRAME_CAMERA

#ifdef TOGETHER_FESTIVAL_2012_AUTUMN
	m_fSmoothCameraCreshMaxTime	= 0.f;		/// 부드러운 카메라 크래시에 적용되는 시간
	m_fSmoothCameraCreshMaxGap	= 0.f;		/// 부드러운 카메라 크래시 강도
	m_bEnableSmoothCameraCresh	= false;	/// 부드러운 카메라 크래시 적용 여부
#endif TOGETHER_FESTIVAL_2012_AUTUMN
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
	// June ctor 31d03220: +0x68 roll ( @31d03251, and the 0x68-byte memset of CameraData ), +0x84 / +0x94 = 0, +0x90 = 0 ( @31d03364 )
	m_Camera.m_fRoll	= 0.f;
	m_fRollTrack		= 0.f;
	m_bLinearTrack		= false;
	m_fLinearTrackTime	= 0.f;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_bShakeLock	= false;	// June ctor 31d03383 ( word +0x19c = 0 )
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_MESH_HIDDEN
	m_bWorldMeshHidden = false;	// June ctor 31d03383 ( word +0x19c = 0 )
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_MESH_HIDDEN
}

CKTDGCamera::~CKTDGCamera()
{
}

void CKTDGCamera::Point( float XEye, float YEye, float ZEye, 
							float XAt, float YAt, float ZAt,
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
							float XUpVec, float YUpVec, float ZUpVec, float fRoll )	// June 31d01310: as March, plus the roll +0x68
#else SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
							float XUpVec, float YUpVec, float ZUpVec )
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
{
	m_Camera.m_Eye.x	= XEye;
	m_Camera.m_Eye.y	= YEye;
	m_Camera.m_Eye.z	= ZEye;

	m_Camera.m_LookAt.x = XAt;
	m_Camera.m_LookAt.y = YAt;
	m_Camera.m_LookAt.z = ZAt;

	m_Camera.m_UpVec.x	= XUpVec;
	m_Camera.m_UpVec.y	= YUpVec;
	m_Camera.m_UpVec.z	= ZUpVec;
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK

	m_Camera.m_fRoll	= fRoll;	// June 31d0134f, before the track times
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
	if( IsSamef( fRoll, 0.f ) == false )
		CamTrackLog( "Point", false, fRoll, 0.f );
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK

	m_fTrackTime = 0.0f;
	m_fTrackAtTime = 0.0f;
}
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK

// June 31d01640 ( 3 callers: 324baa7c, 328c43bc, 328c4466 -> Phase R13d ): Point, then the view is built and applied at once.
// No m_Track / roll-rate reset ( unlike UpdateViewTransform 31d01a30 ).
void CKTDGCamera::PointAndUpdateViewTransform( float XEye, float YEye, float ZEye,
							float XAt, float YAt, float ZAt,
							float XUpVec, float YUpVec, float ZUpVec, float fRoll )
{
	m_Camera.m_Eye.x	= XEye;
	m_Camera.m_Eye.y	= YEye;
	m_Camera.m_Eye.z	= ZEye;

	m_Camera.m_LookAt.x = XAt;
	m_Camera.m_LookAt.y = YAt;
	m_Camera.m_LookAt.z = ZAt;

	m_Camera.m_UpVec.x	= XUpVec;
	m_Camera.m_UpVec.y	= YUpVec;
	m_Camera.m_UpVec.z	= ZUpVec;

	m_Camera.m_fRoll	= fRoll;

	m_fTrackTime = 0.0f;
	m_fTrackAtTime = 0.0f;

	D3DXMatrixLookAtLH( &m_Camera.m_ViewMatrix, &m_Camera.m_Eye, &m_Camera.m_LookAt, &m_Camera.m_UpVec );	// @31d016c5
	ApplyViewRoll();
	g_pKTDXApp->SetViewTransform( &m_Camera.m_ViewMatrix );
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
	if( IsSamef( fRoll, 0.f ) == false )
		CamTrackLog( "PointAndUpdateViewTransform", false, fRoll, 0.f );
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
}

// June: after D3DXMatrixLookAtLH, when | roll - 0.0 | > FLT_EPSILON ( 0x334350e8 / 0x334490f8 ), view = view * RotationZ( roll )
// ( thunks 33086abc = D3DXMatrixRotationZ, 33086a26 = D3DXMatrixMultiply, resolved through the bound d3dx9_43 imports ).
// Inlined in June at 31d016ca ( 31d01640 ), 31d01ac1 ( 31d01a30 ) and 31d030fe ( 31d02e50 ); the only RotationZ users in the exe.
void CKTDGCamera::ApplyViewRoll()
{
	if( IsSamef( m_Camera.m_fRoll, 0.f ) == false )
	{
		D3DXMATRIX matRoll;
		D3DXMatrixRotationZ( &matRoll, m_Camera.m_fRoll );
		D3DXMatrixMultiply( &m_Camera.m_ViewMatrix, &m_Camera.m_ViewMatrix, &matRoll );
	}
}

#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
// One ErrorLog line per non-zero roll or linear-tracking request: the first 20, then at most one a second.
void CKTDGCamera::CamTrackLog( const char* pszFrom, bool bLinear, float fRollTo, float fTime )
{
	static int		s_iCount	= 0;
	static DWORD	s_dwLast	= 0;
	DWORD dwNow = ::GetTickCount();
	if( s_iCount >= 20 && dwNow - s_dwLast < 1000 )
		return;
	++s_iCount;
	s_dwLast = dwNow;
	KLOG( "ErrorLog.txt" ) << "[JUNE_ENGINE] CAMTRACK " << pszFrom << ": linear " << ( bLinear ? 1 : 0 ) << ", roll target " << fRollTo
		<< ", roll now " << m_Camera.m_fRoll << ", time " << fTime << ", look-at-depends-on-eye " << ( g_pKTDXApp->GetLookAtDependOnEye() ? 1 : 0 )
		<< " (#" << s_iCount << ")" << fileout;
}
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK

void CKTDGCamera::Move( float XPos, float YPos, float ZPos )
{
	m_Camera.m_Eye.x	= XPos;
	m_Camera.m_Eye.y	= YPos;
	m_Camera.m_Eye.z	= ZPos;
}

void CKTDGCamera::MoveRel( float XAdd, float YAdd, float ZAdd )
{
	m_Camera.m_Eye.x	+= XAdd;
	m_Camera.m_Eye.y	+= YAdd;
	m_Camera.m_Eye.z	+= ZAdd;
}

void CKTDGCamera::MoveRel( D3DXVECTOR3 vAdd )
{
	m_Camera.m_Eye += vAdd;
}

void CKTDGCamera::LookAt( float XAt, float YAt, float ZAt )
{
	m_Camera.m_LookAt.x = XAt;
	m_Camera.m_LookAt.y = YAt;
	m_Camera.m_LookAt.z = ZAt;
}

void CKTDGCamera::LookAtRel( float XAdd, float YAdd, float ZAdd )
{
	m_Camera.m_LookAt.x += XAdd;
	m_Camera.m_LookAt.y += YAdd;
	m_Camera.m_LookAt.z += ZAdd;
}

void CKTDGCamera::LookAtRel( D3DXVECTOR3 vAdd )
{
	m_Camera.m_LookAt += vAdd;
}

void CKTDGCamera::RotateX( float radian )
{
	float fDist = GetDistance( m_Camera.m_LookAt, m_Camera.m_Eye );
	D3DXVECTOR3 dirVec = m_Camera.m_LookAt - m_Camera.m_Eye;
	D3DXVec3Normalize( &dirVec, &dirVec );

	D3DXMATRIX matRot;
	D3DXMatrixIdentity( &matRot );

	D3DXMatrixRotationYawPitchRoll(&matRot, 0.0f, radian, 0.0f );

	D3DXVECTOR4 outVec;
	D3DXVec3Transform( &outVec, &dirVec, &matRot );

	dirVec.x = outVec.x;
	dirVec.y = outVec.y;
	dirVec.z = outVec.z;

	m_Camera.m_LookAt = m_Camera.m_Eye + dirVec * fDist;
}

void CKTDGCamera::RotateY( float radian )
{
	float x = m_Camera.m_LookAt.x - m_Camera.m_Eye.x;
	float y = m_Camera.m_LookAt.y - m_Camera.m_Eye.y;
	float z = m_Camera.m_LookAt.z - m_Camera.m_Eye.z;

	m_Camera.m_LookAt.x = (x * cos( -radian ) - z * sin( -radian )) + m_Camera.m_Eye.x;
	m_Camera.m_LookAt.z = (x * sin( -radian ) + z * cos( -radian )) + m_Camera.m_Eye.z;
}

void CKTDGCamera::RotateZ( float radian )
{

	float x = m_Camera.m_LookAt.x - m_Camera.m_Eye.x;
	float y = m_Camera.m_LookAt.y - m_Camera.m_Eye.y;
	float z = m_Camera.m_LookAt.z - m_Camera.m_Eye.z;

	m_Camera.m_LookAt.x = (x * cos( radian ) - y * sin( radian )) + m_Camera.m_Eye.x;
	m_Camera.m_LookAt.y = (x * sin( radian ) + y * cos( radian )) + m_Camera.m_Eye.y;
}

void CKTDGCamera::UpVec( float XAt, float YAt, float ZAt )
{
	m_Camera.m_UpVec.x = XAt;
	m_Camera.m_UpVec.y = YAt;
	m_Camera.m_UpVec.z = ZAt;
}

void CKTDGCamera::UpVecRel( float XAdd, float YAdd, float ZAdd )
{
	m_Camera.m_UpVec.x += XAdd;
	m_Camera.m_UpVec.y += YAdd;
	m_Camera.m_UpVec.z += ZAdd;
}

void CKTDGCamera::UpdateCamera( float fElapsedTime )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_MESH_HIDDEN
	m_bWorldMeshHidden = false;	// June 31d02e75: cleared every update, before the tracking
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_MESH_HIDDEN
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
	// June 31d02e50: the tracking steps by fTrackElapsed. With +0x90 set it is +0x94, which is then consumed ( @31d02e6d-31d02e88 ).
	// The shake / impact / up-side-down below still take the real fElapsedTime ( @31d03052, @31d0305b, @31d0307f ).
	float fTrackElapsed = fElapsedTime;
	if( m_bLinearTrack == true )
	{
		fTrackElapsed		= m_fLinearTrackTime;
		m_fLinearTrackTime	= 0.f;
	}

	if( IsSamef( m_fTrackTime, 0.f ) == false )
	{
		if( IsSamef( m_fTrackAtTime, 0.f ) == true )
		{
			MoveRel( m_Track.x * fTrackElapsed,
						m_Track.y * fTrackElapsed,
						m_Track.z * fTrackElapsed );

			LookAtRel( m_TrackAt.x * fTrackElapsed,
						m_TrackAt.y * fTrackElapsed,
						m_TrackAt.z * fTrackElapsed );
		}
		else
		{
			MoveRel( m_Track.x * fTrackElapsed,
				m_Track.y * fTrackElapsed,
				m_Track.z * fTrackElapsed );
		}

		m_fTrackTime -= fTrackElapsed;
		if( m_fTrackTime <= 0.0f )
			m_fTrackTime = 0.0f;

		m_Camera.m_fRoll += m_fRollTrack * fTrackElapsed;	// @31d02fbc: inside the tracking block, after the time step
	}

	if ( IsSamef(m_fTrackAtTime, 0.f) == false )
	{
		LookAtRel( m_TrackAt.x * fTrackElapsed,
			m_TrackAt.y * fTrackElapsed,
			m_TrackAt.z * fTrackElapsed );

		m_fTrackAtTime -= fTrackElapsed;
		if( m_fTrackAtTime <= 0.0f )
			m_fTrackAtTime = 0.0f;
	}
#else SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
	if( IsSamef( m_fTrackTime, 0.f ) == false && IsSamef( m_fTrackAtTime, 0.f ) == true )
	{
		MoveRel( m_Track.x * fElapsedTime, 
					m_Track.y * fElapsedTime, 
					m_Track.z * fElapsedTime );

		LookAtRel( m_TrackAt.x * fElapsedTime, 
					m_TrackAt.y * fElapsedTime, 
					m_TrackAt.z * fElapsedTime );
		
		m_fTrackTime -= fElapsedTime;
		if( m_fTrackTime <= 0.0f )
			m_fTrackTime = 0.0f;
	}
	else if ( IsSamef(m_fTrackTime, 0.f) == false )
	{
		MoveRel( m_Track.x * fElapsedTime, 
			m_Track.y * fElapsedTime, 
			m_Track.z * fElapsedTime );

		m_fTrackTime -= fElapsedTime;
		if( m_fTrackTime <= 0.0f )
			m_fTrackTime = 0.0f;
	}



	if ( IsSamef(m_fTrackAtTime, 0.f) == false )
	{
		LookAtRel( m_TrackAt.x * fElapsedTime, 
			m_TrackAt.y * fElapsedTime, 
			m_TrackAt.z * fElapsedTime );

		m_fTrackAtTime -= fElapsedTime;
		if( m_fTrackAtTime <= 0.0f )
			m_fTrackAtTime = 0.0f;
	}
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK

	// 카메라 흔들기 및 기타 조작
	UpdateCameraShake( fElapsedTime );
	UpdateCameraImpact( fElapsedTime );
	m_UpSideDownCameraData.OnFrameMove( fElapsedTime );
	if( m_UpSideDownCameraData.m_fTimeLeft > 0.f )
	{
		m_Camera.m_UpVec = D3DXVECTOR3( 0, -1, 0 );
	}
	else
	{
#ifdef KEYFRAME_CAMERA
		if(m_bFixedUpVec)
		{
			m_Camera.m_UpVec = D3DXVECTOR3( 0, 1, 0 );
		}
#else
		m_Camera.m_UpVec = D3DXVECTOR3( 0, 1, 0 );
#endif KEYFRAME_CAMERA
	}


	

    D3DXMatrixLookAtLH( &m_Camera.m_ViewMatrix, &m_Camera.m_Eye, &m_Camera.m_LookAt, &m_Camera.m_UpVec );
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
	ApplyViewRoll();	// June 31d02e50 @31d030fe
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
	g_pKTDXApp->SetViewTransform( &m_Camera.m_ViewMatrix );
}


#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
// June 31d01a30 ( ret 0xc ): as March, plus the roll rate +0x84 = 0, the roll +0x68 = fRoll_, and the roll after LookAtLH ( @31d01ac1 ).
// Both June callers ( 31fb061a SquareCameraUpdate, 31fb0a98 TFieldCameraUpdate ) pass 0.
void CKTDGCamera::UpdateViewTransform(const D3DXVECTOR3& vEye_, const D3DXVECTOR3& vLookAt_, float fRoll_)
{
    ASSERT( g_pKTDXApp );

	m_Track         = vEye_;
	m_TrackAt       = vLookAt_;
	m_fTrackTime    = 0.f;
	m_fTrackAtTime  = 0.f;
	m_fRollTrack    = 0.f;
	m_Camera.m_fRoll = fRoll_;

    m_Camera.m_Eye      = vEye_;
    m_Camera.m_LookAt   = vLookAt_;

    D3DXMatrixLookAtLH( &m_Camera.m_ViewMatrix, &m_Camera.m_Eye, &m_Camera.m_LookAt, &m_Camera.m_UpVec );
	ApplyViewRoll();
	g_pKTDXApp->SetViewTransform( &m_Camera.m_ViewMatrix );
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
	if( IsSamef( fRoll_, 0.f ) == false )
		CamTrackLog( "UpdateViewTransform", false, fRoll_, 0.f );
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
}//CKTDGCamera::UpdateViewTransform()
#else SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
void CKTDGCamera::UpdateViewTransform(const D3DXVECTOR3& vEye_, const D3DXVECTOR3& vLookAt_)
{
    ASSERT( g_pKTDXApp );

	m_Track         = vEye_;
	m_TrackAt       = vLookAt_;
	m_fTrackTime    = 0.f;
	m_fTrackAtTime  = 0.f;

    m_Camera.m_Eye      = vEye_;
    m_Camera.m_LookAt   = vLookAt_;

    D3DXMatrixLookAtLH( &m_Camera.m_ViewMatrix, &m_Camera.m_Eye, &m_Camera.m_LookAt, &m_Camera.m_UpVec );
	g_pKTDXApp->SetViewTransform( &m_Camera.m_ViewMatrix );
}//CKTDGCamera::UpdateViewTransform()
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK


#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
// June 31d01b70 ( ret 0x28 ). Differences from March:
//  - g_pKTDXApp +0x2d4 ( SetLookAtDependOnEye, Lua bind 31c154dc ) == 1: the target eye x / y become the look-at x / y ( @31d01b79-31d01b90 );
//  - threshold Time > 0.001 ( 0x33431564 ) instead of > 0; when it fails the track time is 0, not Time ( @31d01c3a );
//  - the 7th channel: roll rate +0x84 = ( fRollTo - roll ) / Time, or fRollTo itself when the threshold fails ( @31d01bf8 / @31d01c30 );
//  - bLinear goes to +0x90 ( @31d01c4e ).
void CKTDGCamera::SetTracking( float XTo, float YTo, float ZTo, float XAt, float YAt, float ZAt, float Time, float atTime, bool bLinear, float fRollTo )
{
	if( g_pKTDXApp->GetLookAtDependOnEye() == true )
	{
		XTo = XAt;
		YTo = YAt;
	}

	float fTrackTime = Time;
	if( Time > 0.001f )
	{
		m_Track.x = (XTo - m_Camera.m_Eye.x)/Time;
		m_Track.y = (YTo - m_Camera.m_Eye.y)/Time;
		m_Track.z = (ZTo - m_Camera.m_Eye.z)/Time;

		m_TrackAt.x = (XAt - m_Camera.m_LookAt.x)/Time;
		m_TrackAt.y = (YAt - m_Camera.m_LookAt.y)/Time;
		m_TrackAt.z = (ZAt - m_Camera.m_LookAt.z)/Time;

		m_fRollTrack = (fRollTo - m_Camera.m_fRoll)/Time;
	}
	else
	{
		m_Track.x = XTo;
		m_Track.y = YTo;
		m_Track.z = ZTo;

		m_TrackAt.x = XAt;
		m_TrackAt.y = YAt;
		m_TrackAt.z = ZAt;

		m_fRollTrack = fRollTo;
		fTrackTime = 0.f;
	}

	m_fTrackTime = fTrackTime;
	m_bLinearTrack = bLinear;
	m_fTrackAtTime = atTime;
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
	if( bLinear == true || IsSamef( fRollTo, 0.f ) == false || IsSamef( m_Camera.m_fRoll, 0.f ) == false || g_pKTDXApp->GetLookAtDependOnEye() == true )
		CamTrackLog( "SetTracking", bLinear, fRollTo, Time );
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK_DEBUG
}
#else SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK
void CKTDGCamera::SetTracking( float XTo, float YTo, float ZTo, float XAt, float YAt, float ZAt, float Time, float atTime )
{
    //{{ seojt // 2008-10-23, 18:53
    if ( Time > 0.f )
    {
	    m_Track.x = (XTo - m_Camera.m_Eye.x)/Time;
	    m_Track.y = (YTo - m_Camera.m_Eye.y)/Time;
	    m_Track.z = (ZTo - m_Camera.m_Eye.z)/Time;

	    m_TrackAt.x = (XAt - m_Camera.m_LookAt.x)/Time;
	    m_TrackAt.y = (YAt - m_Camera.m_LookAt.y)/Time;
	    m_TrackAt.z = (ZAt - m_Camera.m_LookAt.z)/Time;
    }
    else
    {
	    m_Track.x = XTo;
	    m_Track.y = YTo;
	    m_Track.z = ZTo;

	    m_TrackAt.x = XAt;
	    m_TrackAt.y = YAt;
	    m_TrackAt.z = ZAt;
    }//if.. else..
    //}} seojt // 2008-10-23, 18:53
	
	m_fTrackTime = Time;
	m_fTrackAtTime = atTime;
}
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_TRACK

void CKTDGCamera::PushCamera()
{
	m_CameraStack.push(m_Camera);
}

int CKTDGCamera::PopCamera()
{
	if( m_CameraStack.empty() != true )
	{
		m_Camera = m_CameraStack.top();
		m_CameraStack.pop();
	}

	return (int)m_CameraStack.size();
}

void CKTDGCamera::Reset()
{
	m_fTrackTime = 0.0f;
	m_fTrackAtTime = 0.0f;
	Point( 0,0,0,0,0,0 );
}


float CKTDGCamera::GetCameraDistance()
{
	return GetDistance(m_Camera.m_Eye, m_Camera.m_LookAt);
}


void CKTDGCamera::DownCrashCamera( float speed, float accel )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d01490
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fDownCrashGap			= 0.0f;
	m_fDownCrashSpeed		= speed;
	m_fDownCrashAccel		= accel;
}

void CKTDGCamera::UpCrashCamera( float speed, float accel )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d014c0
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fUpCrashGap		= 0.0f;
	m_fUpCrashSpeed		= speed;
	m_fUpCrashAccel		= accel;
}

void CKTDGCamera::UpDownCrashCamera( float fGap, float fTime )
{
	ShakeUpDown( fGap, fTime, 0.05f );
}

void CKTDGCamera::ShakeUpDown( float fGap /*= 10.f*/, float fTime /*= 0.2f*/, float fTimeGap /*= 0.05f */ )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d014f0; UpDownCrashCamera 31d01cd0 inlines this with the same test
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fUpDownCrashGap		= fGap;
	m_fUpDownCrashTime		= fTime;


	m_fUpDownCrashTimeGap	= fTimeGap;
	m_fUpDownCrashTimeGapInit = fTimeGap;

#ifdef TOGETHER_FESTIVAL_2012_AUTUMN
	m_bEnableSmoothCameraCresh	= false;
#endif TOGETHER_FESTIVAL_2012_AUTUMN
}

void CKTDGCamera::UpDownCrashCameraNoReset( float fGap, float fTime )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d01520
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fUpDownCrashGap		= fGap;
	m_fUpDownCrashTime		= fTime;
}




void CKTDGCamera::LeftCrashCamera( float speed, float accel )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d01540
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fLeftCrashGap			= 0.0f;
	m_fLeftCrashSpeed		= speed;
	m_fLeftCrashAccel		= accel;
}

void CKTDGCamera::RightCrashCamera( float speed, float accel )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d01570
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fRightCrashGap		= 0.0f;
	m_fRightCrashSpeed		= speed;
	m_fRightCrashAccel		= accel;
}

void CKTDGCamera::LeftRightCrashCamera( float fGap, float fTime )
{
	ShakeLeftRight( fGap, fTime, 0.05f );
}

// @fTimeGap: 방향을 반대로 바꾸는 시간 간격
void CKTDGCamera::ShakeLeftRight( float fGap /*= 10.f*/, float fTime /*= 0.2f*/, float fTimeGap /*= 0.05f */ )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d015a0; LeftRightCrashCamera 31d01d10 inlines this with the same test
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fLeftRightCrashGap		= fGap;
	m_fLeftRightCrashTime		= fTime;


	m_fLeftRightCrashTimeGapInit	= fTimeGap;
	m_fLeftRightCrashTimeGap		= fTimeGap;
}

void CKTDGCamera::LeftRightCrashCameraNoReset( float fGap, float fTime )
{
#ifdef SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	if( m_bShakeLock == true )	// June 31d015d0
		return;
#endif SERV_IRUHADEV_JUNE_RENDER_CAMERA_SHAKE_LOCK
	m_fLeftRightCrashGap		= fGap;
	m_fLeftRightCrashTime		= fTime;
}


void CKTDGCamera::UpdateCameraShake( float fElapsedTime )
{
	if( m_fDownCrashGap <= 0.0f && m_fDownCrashAccel != 0.0f )
	{
		m_fDownCrashGap += m_fDownCrashSpeed * fElapsedTime;
		m_fDownCrashSpeed += m_fDownCrashAccel * fElapsedTime;
		MoveRel( 0.0f, m_fDownCrashGap, 0.0f );
		LookAtRel( 0.0f, m_fDownCrashGap, 0.0f );
	}
	else
	{
		m_fDownCrashGap		= 0.0f;
		m_fDownCrashSpeed	= 0.0f;
		m_fDownCrashAccel	= 0.0f;
	}

	if( m_fUpCrashGap >= 0.0f && m_fUpCrashAccel != 0.0f )
	{
		m_fUpCrashGap += m_fUpCrashSpeed * fElapsedTime;
		m_fUpCrashSpeed += m_fUpCrashAccel * fElapsedTime;
		MoveRel( 0.0f, m_fUpCrashGap, 0.0f );
		LookAtRel( 0.0f, m_fUpCrashGap, 0.0f );
	}
	else
	{
		m_fUpCrashGap	= 0.0f;
		m_fUpCrashSpeed	= 0.0f;
		m_fUpCrashAccel	= 0.0f;
	}

	if( m_fUpDownCrashTime > 0.0f )
	{
		m_fUpDownCrashTime		-= fElapsedTime;
		m_fUpDownCrashTimeGap	-= fElapsedTime;
		if( m_fUpDownCrashTimeGap <= 0.0f )
		{
			m_fUpDownCrashTimeGap = m_fUpDownCrashTimeGapInit;

#ifdef TOGETHER_FESTIVAL_2012_AUTUMN
			ProcessUpDownCrashCameraSmooth();
#endif TOGETHER_FESTIVAL_2012_AUTUMN

			if( m_bUpDownCrashPositive == true )
			{
				m_bUpDownCrashPositive = false;
				MoveRel( 0.0f, m_fUpDownCrashGap, 0.0f );
				LookAtRel( 0.0f, m_fUpDownCrashGap, 0.0f );
			}
			else
			{
				m_bUpDownCrashPositive = true;
				MoveRel( 0.0f, -m_fUpDownCrashGap, 0.0f );
				LookAtRel( 0.0f, -m_fUpDownCrashGap, 0.0f );
			}
		}
	}
	else
	{
		m_fUpDownCrashTime		= 0.0f;
		m_fUpDownCrashTimeGap	= 0.0f;

#ifdef TOGETHER_FESTIVAL_2012_AUTUMN
		m_fSmoothCameraCreshMaxTime	= 0.f;
		m_fSmoothCameraCreshMaxGap	= 0.f;
		m_bEnableSmoothCameraCresh	= false;
#endif TOGETHER_FESTIVAL_2012_AUTUMN
	}








	if( m_fLeftCrashGap >= 0.0f && m_fLeftCrashAccel != 0.0f )
	{
		D3DXVECTOR3 vRightVec = GetRightVec();

		m_fLeftCrashGap		+= m_fLeftCrashSpeed * fElapsedTime;
		m_fLeftCrashSpeed	+= m_fLeftCrashAccel * fElapsedTime;
		MoveRel( -vRightVec * m_fLeftCrashGap );
		LookAtRel( -vRightVec * m_fLeftCrashGap );
	}
	else
	{
		m_fLeftCrashGap		= 0.0f;
		m_fLeftCrashSpeed	= 0.0f;
		m_fLeftCrashAccel	= 0.0f;
	}

	if( m_fRightCrashGap <= 0.0f && m_fRightCrashAccel != 0.0f )
	{
		D3DXVECTOR3 vRightVec = GetRightVec();

		m_fRightCrashGap	+= m_fRightCrashSpeed * fElapsedTime;
		m_fRightCrashSpeed	+= m_fRightCrashAccel * fElapsedTime;
		MoveRel( vRightVec * m_fRightCrashGap );
		LookAtRel( vRightVec * m_fRightCrashGap );
	}
	else
	{
		m_fRightCrashGap	= 0.0f;
		m_fRightCrashSpeed	= 0.0f;
		m_fRightCrashAccel	= 0.0f;
	}





	if( m_fLeftRightCrashTime > 0.0f )
	{
		m_fLeftRightCrashTime		-= fElapsedTime;
		m_fLeftRightCrashTimeGap	-= fElapsedTime;
		if( m_fLeftRightCrashTimeGap <= 0.0f )
		{
			D3DXVECTOR3 vRightVec = GetRightVec();

			m_fLeftRightCrashTimeGap = m_fLeftRightCrashTimeGapInit;
			if( m_bLeftRightCrashPositive == true )
			{
				m_bLeftRightCrashPositive = false;
				MoveRel( -vRightVec * m_fLeftRightCrashGap );
				LookAtRel( -vRightVec * m_fLeftRightCrashGap );
			}
			else
			{
				m_bLeftRightCrashPositive = true;
				MoveRel( vRightVec * m_fLeftRightCrashGap );
				LookAtRel( vRightVec * m_fLeftRightCrashGap );
			}
		}
	}
	else
	{
		m_fLeftRightCrashTime		= 0.0f;
		m_fLeftRightCrashTimeGap	= 0.0f;
	}




	if( m_RandomCameraShakeData.m_iEyeShakeCount > 0 || m_RandomCameraShakeData.m_iLookAtShakeCount > 0 )
	{
		if( m_RandomCameraShakeData.m_fEyeMoveTimeLeft >= 0.f )
		{
			m_RandomCameraShakeData.m_fEyeMoveTimeLeft -= fElapsedTime;
			if( m_RandomCameraShakeData.m_fEyeMoveTimeLeft < 0.f )
			{
				m_RandomCameraShakeData.m_iEyeShakeCount -= 1;

				D3DXVECTOR3 vUpVec( 0, 1, 0 );
				D3DXVECTOR3 vRightVec = GetRightVec();

				D3DXVECTOR3 vEye = GetEye();
				D3DXVECTOR3 vTargetDirFromEye = vUpVec * RandomFloat( -1.f, 1.f ) + vRightVec * RandomFloat( -1.f, 1.f );
				D3DXVec3Normalize( &vTargetDirFromEye, &vTargetDirFromEye );

				D3DXVECTOR3 vTargetPos = vTargetDirFromEye * m_RandomCameraShakeData.m_EyeMoveRange.GetRandomNumInRange();
				m_RandomCameraShakeData.m_vEyeMoveDir		= vTargetPos - m_RandomCameraShakeData.m_vEyeMove;
				float fDist			= D3DXVec3Length( &m_RandomCameraShakeData.m_vEyeMoveDir );
				if( 0.f != fDist )
					m_RandomCameraShakeData.m_vEyeMoveDir /= fDist;

				m_RandomCameraShakeData.m_fEyeMoveSpeed		= m_RandomCameraShakeData.m_EyeMoveSpeed.GetRandomNumInRange();
				m_RandomCameraShakeData.m_fEyeMoveTimeLeft	= fDist / m_RandomCameraShakeData.m_fEyeMoveSpeed;
			}

			m_RandomCameraShakeData.m_vEyeMove += m_RandomCameraShakeData.m_fEyeMoveSpeed * m_RandomCameraShakeData.m_vEyeMoveDir * fElapsedTime;
		}





		if( true == m_RandomCameraShakeData.m_bFixedLookVec )
		{
			m_RandomCameraShakeData.m_iLookAtShakeCount = 0; // m_iEyeShakeCount를 따른다
			m_RandomCameraShakeData.m_vLookAtMove = m_RandomCameraShakeData.m_vEyeMove;
		}
		else if( m_RandomCameraShakeData.m_fLookAtMoveTimeLeft >= 0.f )
		{
			m_RandomCameraShakeData.m_fLookAtMoveTimeLeft -= fElapsedTime;

			if( m_RandomCameraShakeData.m_fLookAtMoveTimeLeft < 0.f )
			{
				m_RandomCameraShakeData.m_iLookAtShakeCount -= 1;

				D3DXVECTOR3 vUpVec( 0, 1, 0 );
				D3DXVECTOR3 vRightVec = GetRightVec();

				D3DXVECTOR3 vLookAt = GetLookAt();
				D3DXVECTOR3 vTargetDirFromLookAt = vUpVec * RandomFloat( -1.f, 1.f ) + vRightVec * RandomFloat( -1.f, 1.f );
				D3DXVec3Normalize( &vTargetDirFromLookAt, &vTargetDirFromLookAt );

				D3DXVECTOR3 vTargetPos = vTargetDirFromLookAt * m_RandomCameraShakeData.m_LookAtMoveRange.GetRandomNumInRange();
				m_RandomCameraShakeData.m_vLookAtMoveDir	= vTargetPos - m_RandomCameraShakeData.m_vLookAtMove;
				float fDist			= D3DXVec3Length( &m_RandomCameraShakeData.m_vLookAtMoveDir );
				if( 0.f != fDist )
					m_RandomCameraShakeData.m_vLookAtMoveDir /= fDist;

				m_RandomCameraShakeData.m_fLookAtMoveSpeed		= m_RandomCameraShakeData.m_LookAtMoveSpeed.GetRandomNumInRange();
				m_RandomCameraShakeData.m_fLookAtMoveTimeLeft	= fDist / m_RandomCameraShakeData.m_fLookAtMoveSpeed;
			}

			m_RandomCameraShakeData.m_vLookAtMove += m_RandomCameraShakeData.m_fLookAtMoveSpeed * m_RandomCameraShakeData.m_vLookAtMoveDir * fElapsedTime;
		}
	
		

		if( m_RandomCameraShakeData.m_iEyeShakeCount <= 0 &&
			m_RandomCameraShakeData.m_iLookAtShakeCount <= 0 )
		{
			m_RandomCameraShakeData.m_vEyeMove		= D3DXVECTOR3(0, 0, 0);
			m_RandomCameraShakeData.m_vLookAtMove	= D3DXVECTOR3(0, 0, 0);
		}

		MoveRel( m_RandomCameraShakeData.m_vEyeMove );
		LookAtRel( m_RandomCameraShakeData.m_vLookAtMove );
	}

}




void CKTDGCamera::ShakeRandom( int iEyeShakeCount, float m_fEyeMoveSpeedMin, float m_fEyeMoveSpeedMax, float m_fEyeMoveRangeMin, float m_fEyeMoveRangeMax, 
									int iLookAtShakeCount, float m_fLookAtMoveSpeedMin, float m_fLookAtMoveSpeedMax, float m_fLookAtMoveRangeMin, float m_fLookAtMoveRangeMax, 
									bool bFixedLookVec )
{
	m_RandomCameraShakeData.m_bFixedLookVec = bFixedLookVec;


	m_RandomCameraShakeData.m_iEyeShakeCount = iEyeShakeCount;	
	m_RandomCameraShakeData.m_EyeMoveSpeed = CMinMax<float>( m_fEyeMoveSpeedMin, m_fEyeMoveSpeedMax );		
	m_RandomCameraShakeData.m_EyeMoveRange = CMinMax<float>( m_fEyeMoveRangeMin, m_fEyeMoveRangeMax );		

	m_RandomCameraShakeData.m_iLookAtShakeCount = iLookAtShakeCount;	
	m_RandomCameraShakeData.m_LookAtMoveSpeed = CMinMax<float>( m_fLookAtMoveSpeedMin, m_fLookAtMoveSpeedMax );		
	m_RandomCameraShakeData.m_LookAtMoveRange = CMinMax<float>( m_fLookAtMoveRangeMin, m_fLookAtMoveRangeMax );		


	m_RandomCameraShakeData.m_vEyeMove				= D3DXVECTOR3(0, 0, 0);
	m_RandomCameraShakeData.m_vEyeMoveDir			= D3DXVECTOR3(0, 0, 0);
	m_RandomCameraShakeData.m_fEyeMoveTimeLeft		= 0.f;
	m_RandomCameraShakeData.m_fEyeMoveSpeed			= 0.f;

	m_RandomCameraShakeData.m_vLookAtMove			= D3DXVECTOR3(0, 0, 0);
	m_RandomCameraShakeData.m_vLookAtMoveDir		= D3DXVECTOR3(0, 0, 0);
	m_RandomCameraShakeData.m_fLookAtMoveTimeLeft	= 0.f;
	m_RandomCameraShakeData.m_fLookAtMoveSpeed		= 0.f;
}





void CKTDGCamera::UpdateCameraImpact( float fElapsedTime )
{
	if( m_fImpactForwardBounceSpeed > 0.f )
	{
		if( m_fImpactForwardDistance > 0.f )
		{
			m_fImpactForwardCurrentBounceRange += m_fImpactForwardSpeed * fElapsedTime;
			if( m_fImpactForwardCurrentBounceRange > m_fImpactForwardDistance )
			{
				m_fImpactForwardCurrentBounceRange = m_fImpactForwardDistance;
				m_fImpactForwardDistance = 0.f;
			}
		}
		else
		{
			if( m_iImpactForwardCurrentBounceCount >= m_iImpactForwardMaxBounceCount ) 
			{
				m_iImpactForwardMaxBounceCount = 0;
				m_iImpactForwardCurrentBounceCount = 0;
				m_fImpactForwardBounceSpeed = 0.f;
				m_fImpactForwardBounceRadian = 0.f;
				m_fImpactForwardCurrentBounceRange = 0.f;
			}
			else
			{
				m_fImpactForwardBounceRadian += m_fImpactForwardBounceSpeed * D3DX_PI / 180.f;
				if( m_fImpactForwardBounceRadian >= D3DX_PI * 2.f )
				{
					m_fImpactForwardBounceRadian -= D3DX_PI * 2.f;
					m_iImpactForwardCurrentBounceCount += 1;
					m_fImpactForwardCurrentBounceRange *= m_fImpactForwardDamping;
				}
			}
		}	

		D3DXVECTOR3 vLookVec = GetLookVec();
		D3DXVec3Normalize( &vLookVec, &vLookVec );

		MoveRel( vLookVec * m_fImpactForwardCurrentBounceRange * cosf( m_fImpactForwardBounceRadian ) );
	}	
}




void CKTDGCamera::ImpactForward( float fDistance /*= 20.f*/, float fSpeed /*= 100.f*/, float fRetrieveAccel /*= 20.f*/, float fDampingRate /*= 0.6f*/, int iRetrieveBounceCount /*= 2 */ )
{
	m_fImpactForwardDistance = fDistance;
	m_fImpactForwardSpeed = fSpeed;
	m_fImpactForwardBounceSpeed = fRetrieveAccel;
	m_fImpactForwardDamping = fDampingRate;
	m_iImpactForwardMaxBounceCount = iRetrieveBounceCount;

	m_iImpactForwardCurrentBounceCount = 0;
}


void CKTDGCamera::UpSideDown( float fDuration /*= 3.f*/ )
{
	m_UpSideDownCameraData.m_fTimeLeft = fDuration;
}

#ifdef TOGETHER_FESTIVAL_2012_AUTUMN	/// 부드러운 카메라 크래시 ( 점점 강해졌다가 최대값 이후 점점 약해지는 카메라 크래시 )

void CKTDGCamera::UpDownCrashCameraSmooth( float fGap /*= 10.0f*/, float fTime /*= 0.2f*/ )	/// 시간, 강도 설정
{
	ShakeUpDown( fGap, fTime, 0.05f );

	m_bEnableSmoothCameraCresh	= true;
	m_fSmoothCameraCreshMaxTime	= fTime;
	m_fSmoothCameraCreshMaxGap	= fGap;
}

void CKTDGCamera::ProcessUpDownCrashCameraSmooth()		/// 시간에 따른 강도 연산
{
	if( false == m_bEnableSmoothCameraCresh )
		return;

	/// 현재 카메라 크래시 시간이 부드러운 카메라 크래시 최대 시간보다 크면 초기화
	if( m_fSmoothCameraCreshMaxTime <= m_fUpDownCrashTime )
	{
		InitUpDownCrashCameraSmooth();		/// 초기화
	}
	else	/// 카메라 크래시 강도 설정
	{
		float fCrashTime		= m_fUpDownCrashTime / m_fSmoothCameraCreshMaxTime;	/// 현재부드러운 카메라 진행 시간 연산 ( 0 ~ 1.f )
		float fChangeGapValue   = 0.f;	/// 카메라 크래시 강도 수정값

		if( 0.6f <= fCrashTime )		/// 강도 증가
			fChangeGapValue = ( 1.f - fCrashTime ) * 2.5f;	/// ( 0.5f ~ 0.f ) * 2.f
		else							/// 강도 감소
			fChangeGapValue = fCrashTime * 1.6f;				/// ( 0.f ~ 0.5f ) * 2.f

		m_fUpDownCrashGap = m_fSmoothCameraCreshMaxGap * fChangeGapValue;
	}
}

void CKTDGCamera::InitUpDownCrashCameraSmooth()		/// 초기화
{
	/// 카메라 크래시 정지
	m_fUpDownCrashTime			= 0.f;
	m_fUpDownCrashGap			= 0.f;

	/// 부드러운 카메라 크래시 초기화
	m_fSmoothCameraCreshMaxTime	= 0.f;
	m_fSmoothCameraCreshMaxGap	= 0.f;
	m_bEnableSmoothCameraCresh	= false;
}
#endif TOGETHER_FESTIVAL_2012_AUTUMN