/*************************************************************************************************\
Camera.cpp			: Implementation of the Camera component.
Creation Date		: 2/18/2002 8:46:26 PM
Copyright Notice	: Copyright (c) Microsoft Corporation
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

#include "std.h"

#include "camera.h"
#include <stdio.h>
#include "utilities.h"
#include "XBInput.h"
#include "globals.h"
#include "File.h"

#define ASE_CAMERAHEADER	"*CAMERAOBJECT"
#define ASE_CAMERAPOS		"*TM_POS"
#define ASE_CAMERAMAT		"*TM_ROW%ld"
#define ASE_CAMERANEAR		"*CAMERA_NEAR"
#define ASE_CAMERAFAR		"*CAMERA_FAR"
#define ASE_CAMERAFOV		"*CAMERA_FOV"

Camera::Camera()
{
    memset( &m_PositionAnimation, 0, sizeof( m_PositionAnimation ) );
    memset( &m_LookAtAnimation, 0, sizeof( m_LookAtAnimation ) );
}
Camera::~Camera()
{
    if ( m_PositionAnimation.m_pPos )
    {
        delete [] m_PositionAnimation.m_pPos;
    }

    if ( m_LookAtAnimation.m_pPos )
    {
        delete [] m_LookAtAnimation.m_pPos;
    }

    if ( m_PositionAnimation.m_pQuat )
    {
        delete [] m_PositionAnimation.m_pQuat;
    }

    if ( m_LookAtAnimation.m_pQuat )
    {
        delete [] m_LookAtAnimation.m_pQuat;
    }

}

HRESULT Camera::FrameMove( float frameLen )
{
    m_curTime+= frameLen;
       // are we animating?
    if ( m_PositionAnimation.m_numFrames )
    {
        float totalLen = (m_PositionAnimation.m_numFrames/m_PositionAnimation.m_frameRate );
        while ( m_curTime > totalLen  )
            m_curTime -= totalLen;

        DWORD curFrame = (DWORD)(m_curTime * (float)m_PositionAnimation.m_frameRate);
  
        // build current animation position & rotation info
        if ( m_PositionAnimation.m_pPos )
            m_vPosition = m_PositionAnimation.m_pPos[curFrame];

        // TO DO: return different hr if anim is over
    }

    // are we animating?
    if ( m_LookAtAnimation.m_numFrames )
    {
        float totalLen = (m_LookAtAnimation.m_numFrames/m_LookAtAnimation.m_frameRate );
        while ( m_curTime > totalLen  )
            m_curTime -= totalLen;

        DWORD curFrame = (DWORD)(m_curTime * (float)m_PositionAnimation.m_frameRate);

        // build current animation LookAt & rotation info
        m_vLookAt = m_LookAtAnimation.m_pPos[curFrame];
  
        // TO DO: return different hr if anim is over
    }
	
	return S_OK;
}

void Camera::SetUp(IDirect3DDevice8* pDev )
{
	float degreeFOV = (float)m_fFov * 180.0f / 3.14159f;

	float widthFOV  = degreeFOV;
	float heightFOV = (degreeFOV / 4.0f) * 3.3f;

	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	float fov_horiz = (float)((widthFOV) / 180.0) * 3.14159f;
	float fov_vert  = (float)((heightFOV) / 180.0) * 3.14159f;
	float far_plane = 10000.0f;
	float near_plane = 1.0f;

	float w = (float)(1/tan(fov_horiz * 0.5));
	float h = (float)(1/tan(fov_vert * 0.5));
	float Q = far_plane / (far_plane - near_plane);

	matProj._11 = w;	//0.999996;
	matProj._22 = h;	//0.999996;
	matProj._33 = -Q;		
	matProj._43 = -Q * near_plane;
	matProj._34 = -1.0f;
	matProj._44 = 0.0f;

 	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtRH( &viewMatrix, &m_vPosition, &m_vLookAt, &m_vUp );
	pDev->SetTransform( D3DTS_VIEW, &viewMatrix );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).

	//float aspectRatio = 640.0f / 480.0f;
	//D3DXMATRIX matProj;
	//D3DXMatrixPerspectiveFovRH( &matProj, m_fFov, aspectRatio, m_fNearClip, m_fFarClip );
	pDev->SetTransform( D3DTS_PROJECTION, &matProj );
}

float yRotation = 0.f; // ditch this, for debugging only
float xRotation = 0.f;
void Camera::Update(XBGAMEPAD* pPad, float elapsedTime)
{
	D3DXMATRIX rot;
	D3DXMatrixRotationY( &rot, yRotation );
	if ( pPad->bAnalogButtons[XINPUT_GAMEPAD_A] )
		yRotation += .001f;
	else if ( pPad->bAnalogButtons[XINPUT_GAMEPAD_B] )
		yRotation -= .001f;

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &rot );
}

long Camera::Load( const char* ASEBuffer, TG_Animation* pAnimHeader )
{
    memset( &m_PositionAnimation, 0, sizeof( m_PositionAnimation ) );
    memset( &m_LookAtAnimation, 0, sizeof( m_LookAtAnimation ) );
    m_curTime = 0.f;


	long retVal = -1;
	char* pHeader = strstr( ASEBuffer, ASE_CAMERAHEADER );
	if ( pHeader )
	{

        char* pNodeName = strstr( pHeader, "*NODE_NAME " );
        char NodeName[64];
        if ( pNodeName )
        {
            GetNameData (pNodeName + strlen( "*NODE_NAME " ), NodeName);

        }

        CharUpperA( NodeName );

		pHeader = strstr( pHeader, ASE_CAMERAPOS );
		pHeader += strlen( ASE_CAMERAPOS ) + 1;
		m_vPosition.x = GetFloat( &pHeader );
		m_vPosition.y = GetFloat( &pHeader );
		m_vPosition.z = GetFloat( &pHeader );
        TG_Shape::LoadAnimationFromASE( (char*)ASEBuffer, NodeName, m_vPosition, pAnimHeader, &m_PositionAnimation );

        strcat( NodeName, ".TARGET" );


		pHeader = strstr( pHeader, ASE_CAMERAPOS );
		pHeader += strlen( ASE_CAMERAPOS ) + 1;
		m_vLookAt.x = GetFloat( &pHeader );
		m_vLookAt.y = GetFloat( &pHeader );
		m_vLookAt.z = GetFloat( &pHeader );

		m_vUp.x = 0.0;
		m_vUp.y = 0.0;
		m_vUp.z = 1.0;

        TG_Shape::LoadAnimationFromASE( (char*)ASEBuffer, NodeName, m_vLookAt, pAnimHeader, &m_LookAtAnimation );


		pHeader = strstr( pHeader, ASE_CAMERANEAR );
		pHeader += strlen( ASE_CAMERANEAR ) +1;

		m_fNearClip = GetFloat( &pHeader );
		if ( m_fNearClip == 0.f )
			m_fNearClip = 1.f;

		pHeader = strstr( pHeader, ASE_CAMERAFAR );
		pHeader += strlen( ASE_CAMERAFAR ) +1;

		m_fFarClip = GetFloat( &pHeader );

		pHeader = strstr( pHeader, ASE_CAMERAFOV );
		pHeader += strlen( ASE_CAMERAFOV ) + 1;

		m_fFov = GetFloat( &pHeader );
	}

	return retVal;

	
}

void Camera::SaveBinary( File* file )
{
    file->write( (BYTE*)&m_vPosition, sizeof ( D3DXVECTOR3 ) );
    file->write( (BYTE*)&m_vLookAt, sizeof ( D3DXVECTOR3 ) );
    file->write( (BYTE*)&m_vUp, sizeof ( D3DXVECTOR3 ) );

    file->writeFloat( m_fFov );
    file->writeFloat( m_fFarClip );
    file->writeFloat( m_fNearClip);

    file->write( (BYTE*)&m_PositionAnimation, sizeof( m_PositionAnimation ) );
    file->write( (BYTE*)&m_LookAtAnimation, sizeof( m_LookAtAnimation ) );

    if ( m_PositionAnimation.m_pPos )
    {
        file->write( (BYTE*)m_PositionAnimation.m_pPos, m_PositionAnimation.m_numFrames * sizeof( D3DXVECTOR3 ) );
    }

    if ( m_PositionAnimation.m_pQuat )
    {
        file->write( (BYTE*)m_PositionAnimation.m_pQuat, m_PositionAnimation.m_numFrames * sizeof( D3DXQUATERNION ) );
    }

    if ( m_LookAtAnimation.m_pPos )
    {
        file->write( (BYTE*)m_LookAtAnimation.m_pPos, m_LookAtAnimation.m_numFrames * sizeof( D3DXVECTOR3 ) );
    }

    if ( m_LookAtAnimation.m_pQuat )
    {
        file->write( (BYTE*)m_LookAtAnimation.m_pQuat, m_LookAtAnimation.m_numFrames * sizeof( D3DXQUATERNION ) );
    }
}

void Camera::LoadBinary( File* file )
{

    file->read( (BYTE*)&m_vPosition, sizeof ( D3DXVECTOR3 ) );
    file->read( (BYTE*)&m_vLookAt, sizeof ( D3DXVECTOR3 ) );
    file->read( (BYTE*)&m_vUp, sizeof ( D3DXVECTOR3 ) );

    m_fFov =        file->readFloat(  );
    m_fFarClip =    file->readFloat(  );
    m_fNearClip =   file->readFloat(  );

    // hack, but far clip is too close in max sometimes
    m_fFarClip = 10000;

    m_curTime = 0.f;
    file->read( (BYTE*)&m_PositionAnimation, sizeof( m_PositionAnimation ) );
    file->read( (BYTE*)&m_LookAtAnimation, sizeof( m_LookAtAnimation ) );

    if ( m_PositionAnimation.m_pPos )
    {
        m_PositionAnimation.m_pPos = new D3DXVECTOR3[m_PositionAnimation.m_numFrames];
        file->read( (BYTE*)m_PositionAnimation.m_pPos, m_PositionAnimation.m_numFrames * sizeof( D3DXVECTOR3 ) );

    }

    if ( m_PositionAnimation.m_pQuat )
    {
        m_PositionAnimation.m_pQuat = new D3DXQUATERNION[m_PositionAnimation.m_numFrames];
        file->read( (BYTE*)m_PositionAnimation.m_pQuat, m_PositionAnimation.m_numFrames * sizeof( D3DXQUATERNION ) );

    }

    if ( m_LookAtAnimation.m_pPos )
    {
        m_LookAtAnimation.m_pPos = new D3DXVECTOR3[m_LookAtAnimation.m_numFrames];
        file->read( (BYTE*)m_LookAtAnimation.m_pPos, m_LookAtAnimation.m_numFrames * sizeof( D3DXVECTOR3 ) );

    }

    if ( m_LookAtAnimation.m_pQuat )
    {
        m_LookAtAnimation.m_pQuat = new D3DXQUATERNION[m_LookAtAnimation.m_numFrames];
        file->read( (BYTE*)m_LookAtAnimation.m_pQuat, m_LookAtAnimation.m_numFrames * sizeof( D3DXQUATERNION ) );

    }


}

