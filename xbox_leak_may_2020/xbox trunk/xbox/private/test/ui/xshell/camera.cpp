#include "stdafx.h"
#include "camera.h"

int randIntBetween(int start, int finish)
{
	float randMax = RAND_MAX;
	float random  = rand() / randMax;

	int delta = finish - start;
	int final = (int)(random * delta) + start;
	return final;
}

float randFloatBetween(float start, float finish)
{
	float randMax = RAND_MAX;
	float random  = rand() / randMax;

	float delta = finish - start;
	float final = (random * delta) + start;
	return final;
}


// Constructor
Camera::Camera( void ) 
{
}

// Destructor
Camera::~Camera( void )
{
}

void Camera::update(IDirect3DDevice8* pd3dDevice)
{
	pd3dDevice->SetTransform( D3DTS_VIEW, &m_MatView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_MatProj );
}

void Camera::noise( IDirect3DDevice8* pd3dDevice )
{
	float randMax = RAND_MAX;

	for(int i = 0; i < 2; i++)
	{
		D3DXVECTOR3 CameraPos;
		if(i == 0)
		{
			CameraPos = m_CameraBase;
		}
		else if(i == 1)
		{
			CameraPos = m_CameraTarget;
		}
		
		if(m_iXframeCount[i] == m_iXframeDuration[i])
		{
			m_iXframeDuration[i] = randIntBetween(500, 800);  // new duration of frames for move
			m_PreviousPos[i].x = m_NewPos[i].x;
			
			float randx = randFloatBetween(-0.1f, 0.1f);
			m_NewPos[i].x = randx + CameraPos.x;
			m_iXframeCount[i] = 0;
		}
		if(m_iYframeCount[i] == m_iYframeDuration[i])
		{
			m_iYframeDuration[i] = randIntBetween(500, 800);
			m_PreviousPos[i].y = m_NewPos[i].y;

			float randy = randFloatBetween(-0.1f, 0.1f);
			m_NewPos[i].y = randy + CameraPos.y;
			m_iYframeCount[i] = 0;
		}
		if(m_iZframeCount[i] == m_iZframeDuration[i])
		{
			m_iZframeDuration[i] = randIntBetween(500, 800);
			m_PreviousPos[i].z = m_NewPos[i].z;
			
			float randz = randFloatBetween(-0.00f, 0.00f);
			m_NewPos[i].z = randz + CameraPos.z;
			m_iZframeCount[i] = 0;
		}

		m_fXinc[i] = (m_NewPos[i].x - m_PreviousPos[i].x);
		m_fYinc[i] = (m_NewPos[i].y - m_PreviousPos[i].y);
		m_fZinc[i] = (m_NewPos[i].z - m_PreviousPos[i].z);

		m_fXinc[i] *= (float)sin(((float)m_iXframeCount[i] / (float)m_iXframeDuration[i]) * 1.5708f);
		m_fYinc[i] *= (float)sin(((float)m_iYframeCount[i] / (float)m_iYframeDuration[i]) * 1.5708f);
		m_fZinc[i] *= (float)sin(((float)m_iZframeCount[i] / (float)m_iZframeDuration[i]) * 1.5708f);

		m_iXframeCount[i]++;
		m_iYframeCount[i]++;
		m_iZframeCount[i]++;
	}

	//	D3DXMATRIX translate;
	//	D3DXMatrixTranslation( &translate, m_PreviousPos.x + m_fXinc, m_PreviousPos.y + m_fYinc, m_PreviousPos.z + m_fZinc);
	//	m_MatView = translate * objectMatrix;

	D3DXVECTOR3 newBase   = D3DXVECTOR3(m_PreviousPos[0].x + m_fXinc[0], m_PreviousPos[0].y + m_fYinc[0], m_PreviousPos[0].z + m_fZinc[0]);
	D3DXVECTOR3 newTarget = D3DXVECTOR3(m_PreviousPos[1].x + m_fXinc[1], m_PreviousPos[1].y + m_fYinc[1], m_PreviousPos[1].z + m_fZinc[1]);
	D3DXMatrixLookAtRH(&m_MatView, &newBase, &newTarget, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	pd3dDevice->SetTransform( D3DTS_VIEW, &m_MatView );
}

// Sets the proper aspect ration for 4:3 or Widescreen mode depending upon the value
// passed in.  TRUE for Widescree
void Camera::SetWidescreen( BOOL bValue )
{
 	float fieldOfView = (3.14159f / 2.0f) * 0.89f;
    float aspect = 640.0f / 480.0f;

    // widescreen
    if( bValue )
        aspect *= 1.25f;

    D3DXMatrixPerspectiveFovRH( &m_MatProj, fieldOfView, aspect, 1.0f, 1000.0f );
}

void Camera::readCAM( char* pszFilename )
{
	FILE  *pfInput;
	DWORD totalObjects = 1;

	pfInput = fopen( pszFilename, "rb" );

    if( !pfInput )
    {
        XDBGERR( APP_TITLE_NAME_A, "Camera::readCAM():Failed to open the file - '%s'!!", pszFilename );

        return;
    }

	fread( &totalObjects, sizeof(DWORD), 1, pfInput );

	//
	// Write out camera information necessary for scene
	//
	fread( &m_CameraBase.x, sizeof(float), 1, pfInput );
	fread( &m_CameraBase.y, sizeof(float), 1, pfInput );
	fread( &m_CameraBase.z, sizeof(float), 1, pfInput );
	fread( &m_CameraTarget.x, sizeof(float), 1, pfInput );
	fread( &m_CameraTarget.y, sizeof(float), 1, pfInput );
	fread( &m_CameraTarget.z, sizeof(float), 1, pfInput );
	fread( &m_Axis.x, sizeof(float), 1, pfInput );
	fread( &m_Axis.y, sizeof(float), 1, pfInput );
	fread( &m_Axis.z, sizeof(float), 1, pfInput );
	fread( &m_Angle, sizeof(float), 1, pfInput );
	fread( &m_fFOV, sizeof(float), 1, pfInput );
	
    fclose( pfInput );

	//
	// Preform math necessary for 3D space placement
	//
    /*
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis( &q, &m_Axis, m_Angle );
	D3DXMatrixRotationQuaternion( &m_ObjectMatrix, &q );

	D3DXMATRIX translate;
	D3DXMatrixTranslation( &translate, m_CameraBase.x, m_CameraBase.y, m_CameraBase.z );
	m_MatView = translate * m_ObjectMatrix;
    */

    // Bring camera down a bit
    m_CameraBase.z -= 0.1f;
    m_CameraTarget.z -= 0.1f;

	m_CameraBase.x *= -1.0f;
	m_CameraBase.y *= -1.0f;
	m_CameraBase.z *= -1.0f;
	m_CameraTarget.x *= -1.0f;
	m_CameraTarget.y *= -1.0f;
	m_CameraTarget.z *= -1.0f;
	D3DXMatrixLookAtRH(&m_MatView, &m_CameraBase, &m_CameraTarget, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));

	//
	// Setup the Projection Matrix
	//
    SetWidescreen( FALSE ); // Default it to 4:3 mode
	
	//
	// Initalize noise paramaters
	//
    for( int i = 0; i < 2; i++ )
    {
	    m_iXframeCount[i]    = 0;
	    m_iXframeDuration[i] = 0;
	    m_iYframeCount[i]    = 0;
	    m_iYframeDuration[i] = 0;
	    m_iZframeCount[i]    = 0;
	    m_iZframeDuration[i] = 0;
    }

	m_PreviousPos[0]     = D3DXVECTOR3( m_CameraBase.x, m_CameraBase.y, m_CameraBase.z );
	m_NewPos[0]          = D3DXVECTOR3( m_CameraBase.x, m_CameraBase.y, m_CameraBase.z );
	m_PreviousPos[1]     = D3DXVECTOR3( m_CameraTarget.x, m_CameraTarget.y, m_CameraTarget.z );
	m_NewPos[1]          = D3DXVECTOR3( m_CameraTarget.x, m_CameraTarget.y, m_CameraTarget.z );
}
