#ifndef CAMERA_H
#define CAMERA_H

#include "TGL.h"

struct XBGAMEPAD;
class File;
class Camera
{

public:

	void SetUp(IDirect3DDevice8* pDev );
	long Load( const char* ASEBuffer, TG_Animation* pAnimHeader );
	void Update(XBGAMEPAD* pPad, float elapsedTime);
    void SaveBinary( File* file );
    void LoadBinary( File* file );
    
    HRESULT FrameMove( float frameLen );

    ~Camera();
    Camera();



private:

	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vLookAt;
	D3DXVECTOR3		m_vUp;

	float			m_fFov;
	float			m_fNearClip;
	float			m_fFarClip;

    TG_Animation    m_PositionAnimation;
    TG_Animation    m_LookAtAnimation;

    float           m_curTime;


};

#endif
