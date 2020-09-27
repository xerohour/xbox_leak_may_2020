//-----------------------------------------------------------------------------
// File: D3DUtil.cpp
//
// Desc: Shortcut macros and functions for using DX objects
//
//
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------

#define STRICT

#include <stdio.h>
#ifdef XBOX
#include <xtl.h>
#include <xgraphics.h>
#else
#include <windows.h>
#endif

#include "D3DUtil.h"
#include "DXUtil.h"

//-----------------------------------------------------------------------------
// Name: D3DUtil_InitMaterial()
// Desc: Initializes a D3DMATERIAL8 structure, setting the diffuse and ambient
//       colors. It does not set emissive or specular colors.
//-----------------------------------------------------------------------------
VOID D3DUtil_InitMaterial( D3DMATERIAL8& mtrl, FLOAT r, FLOAT g, FLOAT b,
                           FLOAT a )
{
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = r;
    mtrl.Diffuse.g = mtrl.Ambient.g = g;
    mtrl.Diffuse.b = mtrl.Ambient.b = b;
    mtrl.Diffuse.a = mtrl.Ambient.a = a;
}




//-----------------------------------------------------------------------------
// Name: D3DUtil_InitLight()
// Desc: Initializes a D3DLIGHT structure, setting the light position. The
//       diffuse color is set to white, specular and ambient left as black.
//-----------------------------------------------------------------------------
VOID D3DUtil_InitLight( D3DLIGHT8& light, D3DLIGHTTYPE ltType,
                        FLOAT x, FLOAT y, FLOAT z )
{
    ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type        = ltType;
    light.Diffuse.r   = 1.0f;
    light.Diffuse.g   = 1.0f;
    light.Diffuse.b   = 1.0f;
    light.Position.x   = light.Direction.x = x;
    light.Position.y   = light.Direction.y = y;
    light.Position.z   = light.Direction.z = z;
    light.Range        = 1000.0f;
}




//-----------------------------------------------------------------------------
// Name: D3DUtil_CreateTexture()
// Desc: Helper function to create a texture. It checks the root path first,
//       then tries the DXSDK media path (as specified in the system registry).
//-----------------------------------------------------------------------------
HRESULT D3DUtil_CreateTexture( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strTexture,
                               LPDIRECT3DTEXTURE8* ppTexture, D3DFORMAT d3dFormat )
{
	// Get the path to the texture
    CHAR strPath[512];
	DXUtil_FindMediaFile( strPath, strTexture );

	// Create the texture using D3DX
    return D3DXCreateTextureFromFileExA(pd3dDevice, strPath, 
		        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, d3dFormat, 
				D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
			    D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, ppTexture );
}


#if 0

//-----------------------------------------------------------------------------
// Name: D3DUtil_SetColorKey()
// Desc: Changes all texels matching the colorkey to transparent, black.
//-----------------------------------------------------------------------------
HRESULT D3DUtil_SetColorKey( LPDIRECT3DTEXTURE8 pTexture, DWORD dwColorKey )
{
    // Get colorkey's red, green, and blue components
    DWORD r = ((dwColorKey&0x00ff0000)>>16);
    DWORD g = ((dwColorKey&0x0000ff00)>>8);
    DWORD b = ((dwColorKey&0x000000ff)>>0);

    // Put the colorkey in the texture's native format
    D3DSURFACE_DESC d3dsd;    
    pTexture->GetLevelDesc( 0, &d3dsd );

    if( d3dsd.Format == D3DFMT_A4R4G4B4 )
        dwColorKey = 0xf000 + ((r>>4)<<8) + ((g>>4)<<4) + (b>>4);
    else if( d3dsd.Format == D3DFMT_A1R5G5B5 )
        dwColorKey = 0x8000 + ((r>>3)<<10) + ((g>>3)<<5) + (b>>3);
    else if( d3dsd.Format != D3DFMT_A8R8G8B8 )
        return E_FAIL;
    // Lock the texture
    D3DLOCKED_RECT  d3dlr;
#ifdef XBOX
    if( FAILED( pTexture->LockRect( 0, &d3dlr, 0, D3DLOCK_RAWDATA ) ) )
        return E_FAIL;

    PVOID pBits = Swizzler_GetUnswizzledBits(&d3dlr, &d3dsd);
    if (pBits == NULL)
    {
        pTexture->UnlockRect(0);
        return E_FAIL;
    }
#else
    if( FAILED( pTexture->LockRect( 0, &d3dlr, 0, 0 ) ) )
        return E_FAIL;
#endif

    // Scan through each pixel, looking for the colorkey to replace
    for( DWORD y=0; y<d3dsd.Height; y++ )
    {
        for( DWORD x=0; x<d3dsd.Width; x++ )
        {
            if( d3dsd.Format==D3DFMT_A8R8G8B8 )
            {
                // Handle 32-bit formats
#ifdef XBOX
                if( ((DWORD*)pBits)[d3dsd.Width*y+x] == dwColorKey )
                    ((DWORD*)pBits)[d3dsd.Width*y+x] = 0x00000000;
#else
                if( ((DWORD*)d3dlr.pBits)[d3dsd.Width*y+x] == dwColorKey )
                    ((DWORD*)d3dlr.pBits)[d3dsd.Width*y+x] = 0x00000000;
#endif
            }
            else
            {
                // Handle 16-bit formats
#ifdef XBOX
                if( ((WORD*)pBits)[d3dsd.Width*y+x] == dwColorKey )
                    ((WORD*)pBits)[d3dsd.Width*y+x] = 0x0000;
#else
                if( ((WORD*)d3dlr.pBits)[d3dsd.Width*y+x] == dwColorKey )
                    ((WORD*)d3dlr.pBits)[d3dsd.Width*y+x] = 0x0000;
#endif
            }
        }
    }

#ifdef XBOX
    Swizzler_SwizzleFromBits(pBits, &d3dlr, &d3dsd);
#endif

    // Unlock the texture and return OK.
    pTexture->UnlockRect(0);
    return S_OK;
}

#endif 0



//-----------------------------------------------------------------------------
// Name: D3DUtil_CreateVertexShader()
// Desc: Assembles and creates a file-based vertex shader
//-----------------------------------------------------------------------------
HRESULT D3DUtil_CreateVertexShader( LPDIRECT3DDEVICE8 pd3dDevice, 
								    WCHAR* strFilename, DWORD* pdwVertexDecl,
									DWORD* pdwVertexShader )
{
#if 1
    return E_NOTIMPL;
#else

    LPD3DXBUFFER pCode;
    WCHAR        strPath[512];
    HRESULT      hr;

	// Get the path to the vertex shader file
	DXUtil_FindMediaFile( strPath, strFilename );

	// Assemble the vertex shader file
    if( FAILED( hr = D3DXAssembleVertexShaderFromFile( strPath, &pCode, NULL ) ) )
		return hr;

	// Create the vertex shader
    hr = pd3dDevice->CreateVertexShader( pdwVertexDecl, 
		                                 (DWORD*)pCode->GetBufferPointer(),
                                         pdwVertexShader, 0 );
    pCode->Release();
	return hr;
#endif
}




//-----------------------------------------------------------------------------
// Name: D3DUtil_GetCubeMapViewMatrix()
// Desc: Returns a view matrix for rendering to a face of a cubemap.
//-----------------------------------------------------------------------------
D3DXMATRIX D3DUtil_GetCubeMapViewMatrix( DWORD dwFace )
{
    D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    switch( dwFace )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
    return matView;
}


#if 0

//-----------------------------------------------------------------------------
// Name: D3DUtil_GetRotationFromCursor()
// Desc: Returns a quaternion for the rotation implied by the window's cursor
//       position.
//-----------------------------------------------------------------------------
D3DXQUATERNION D3DUtil_GetRotationFromCursor( HWND hWnd,
                                              FLOAT fTrackBallRadius )
{
    POINT pt;
    RECT  rc;
    GetCursorPos( &pt );
    GetClientRect( hWnd, &rc );
    ScreenToClient( hWnd, &pt );
    FLOAT sx = ( ( ( 2.0f * pt.x ) / (rc.right-rc.left) ) - 1 );
    FLOAT sy = ( ( ( 2.0f * pt.y ) / (rc.bottom-rc.top) ) - 1 );
    FLOAT sz;

    if( sx == 0.0f && sy == 0.0f )
        return D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 1.0f );

    FLOAT d1 = 0.0f;
    FLOAT d2 = sqrtf( sx*sx + sy*sy );

    if( d2 < fTrackBallRadius * 0.70710678118654752440 ) // Inside sphere
        sz = sqrtf( fTrackBallRadius*fTrackBallRadius - d2*d2 );
    else                                                 // On hyperbola
        sz = (fTrackBallRadius*fTrackBallRadius) / (2.0f*d2);

    // Get two points on trackball's sphere
    D3DXVECTOR3 p1( sx, sy, sz );
    D3DXVECTOR3 p2( 0.0f, 0.0f, fTrackBallRadius );

    // Get axis of rotation, which is cross product of p1 and p2
    D3DXVECTOR3 vAxis;
    D3DXVec3Cross( &vAxis, &p1, &p2);

    // Calculate angle for the rotation about that axis
    FLOAT t = D3DXVec3Length( &(p2-p1) ) / ( 2.0f*fTrackBallRadius );
    if( t > +1.0f) t = +1.0f;
    if( t < -1.0f) t = -1.0f;
    FLOAT fAngle = 2.0f * asinf( t );

    // Convert axis to Quaternion
    D3DXQUATERNION quat;
    D3DXQuaternionRotationAxis( &quat, &vAxis, fAngle );
    return quat;
}

#endif 0


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DArcBall::CD3DArcBall()
{
    D3DXQuaternionIdentity( &m_qDown );
    D3DXQuaternionIdentity( &m_qNow );
    D3DXMatrixIdentity( &m_matRotation );
    D3DXMatrixIdentity( &m_matRotationDelta );
    D3DXMatrixIdentity( &m_matTranslation );
    D3DXMatrixIdentity( &m_matTranslationDelta );
    m_bDrag = FALSE;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DArcBall::SetWindow( int iWidth, int iHeight, float fRadius )
{
    // Set ArcBall info
    m_iWidth  = iWidth;
    m_iHeight = iHeight;
    m_fRadius = fRadius;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
D3DXVECTOR3 CD3DArcBall::ScreenToVector( int sx, int sy )
{
    // Scale to screen
    FLOAT x   =  (sx - m_iWidth/2)  / (m_fRadius*m_iWidth/2);
    FLOAT y   = -(sy - m_iHeight/2) / (m_fRadius*m_iHeight/2);
    FLOAT z   = 0.0f;
    FLOAT mag = x*x + y*y;

    if( mag > 1.0f )
    {
        FLOAT scale = 1.0f/sqrtf(mag);
        x *= scale;
        y *= scale;
    }
    else
        z = sqrtf( 1.0f - mag );

    // Return vector
    return D3DXVECTOR3( x, y, z );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DArcBall::SetRadius( FLOAT fRadius )
{
    m_fRadius2 = fRadius;
}

#if 0

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
LRESULT CD3DArcBall::HandleMouseMessages( HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam )
{
    static int         iCurMouseX;      // Saved mouse position
    static int         iCurMouseY;
    static D3DXVECTOR3 m_vDown;         // Button down quaternion
    static D3DXVECTOR3 m_vCur;          // Current quaternion

    // Current mouse position
    int iMouseX = LOWORD(lParam);
    int iMouseY = HIWORD(lParam);

    switch( uMsg )
    {
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // Store off the position of the cursor when the button is pressed
            iCurMouseX = iMouseX;
            iCurMouseY = iMouseY;
            return TRUE;

        case WM_LBUTTONDOWN:
            // Start drag mode
            m_bDrag = TRUE;
            m_vDown = ScreenToVector( iMouseX, iMouseY );
            return TRUE;

        case WM_LBUTTONUP:
            // End drag mode
            m_bDrag = FALSE;
            m_qDown = m_qNow;
            return TRUE;

        case WM_MOUSEMOVE:
            // Drag object
            if( MK_LBUTTON&wParam )
            {
                if( m_bDrag )
                {
                    D3DXVECTOR3 vPart;
                    D3DXVECTOR3 vCur = ScreenToVector( iMouseX, iMouseY );
                    D3DXVec3Cross( &vPart, &m_vDown, &vCur );
                    m_qNow = m_qDown * D3DXQUATERNION( vPart.x, vPart.y, vPart.z,
                                                       D3DXVec3Dot( &m_vDown, &vCur ) );
                }

                D3DXQUATERNION qConj;
                D3DXQuaternionConjugate( &qConj, &m_qNow );

                D3DXMatrixRotationQuaternion( &m_matRotationDelta, &qConj );
                D3DXMatrixTranspose( &m_matRotationDelta, &m_matRotationDelta );
                D3DXMatrixMultiply( &m_matRotation, &m_matRotation, &m_matRotationDelta );

                D3DXQuaternionIdentity( &m_qDown );
                D3DXQuaternionIdentity( &m_qNow );
                m_vDown = ScreenToVector( iMouseX, iMouseY );
                m_bDrag = TRUE;
            }
            else if( (MK_RBUTTON&wParam) || (MK_MBUTTON&wParam) )
            {
                // Normalize based on size of window and bounding sphere radius
                FLOAT fDeltaX = ( iCurMouseX-iMouseX ) * m_fRadius2 / m_iWidth;
                FLOAT fDeltaY = ( iCurMouseY-iMouseY ) * m_fRadius2 / m_iHeight;

                if( wParam & MK_RBUTTON )
                {
                    D3DXMatrixTranslation( &m_matTranslationDelta, -2*fDeltaX, 2*fDeltaY, 0.0f );
                    D3DXMatrixMultiply( &m_matTranslation, &m_matTranslation, &m_matTranslationDelta );
                }
                else  // wParam & MK_MBUTTON
                {
                    D3DXMatrixTranslation( &m_matTranslationDelta, 0.0f, 0.0f, 5*fDeltaY );
                    D3DXMatrixMultiply( &m_matTranslation, &m_matTranslation, &m_matTranslationDelta );
                }

                // Store mouse coordinate
                iCurMouseX = iMouseX;
                iCurMouseY = iMouseY;
            }
            return TRUE;
    }

    return FALSE;
}

#endif 0


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DCamera::CD3DCamera()
{
    // Set attributes for the view matrix
    SetViewParams( D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,1.0f),
                   D3DXVECTOR3(0.0f,1.0f,0.0f) );

    // Set attributes for the projection matrix
    SetProjParams( D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DCamera::SetViewParams( D3DXVECTOR3 &vEyePt, D3DXVECTOR3& vLookatPt,
                                D3DXVECTOR3& vUpVec )
{
    // Set attributes for the view matrix
    m_vEyePt    = vEyePt;
    m_vLookatPt = vLookatPt;
    m_vUpVec    = vUpVec;
    D3DXVec3Normalize( &m_vView, &(m_vLookatPt - m_vEyePt) );
    D3DXVec3Cross( &m_vCross, &m_vView, &m_vUpVec );

    D3DXMatrixLookAtLH( &m_matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
    D3DXMatrixInverse( &m_matBillboard, NULL, &m_matView );
    m_matBillboard._41 = 0.0f;
    m_matBillboard._42 = 0.0f;
    m_matBillboard._43 = 0.0f;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DCamera::SetProjParams( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane,
                                FLOAT fFarPlane )
{
    // Set attributes for the projection matrix
    m_fFOV        = fFOV;
    m_fAspect     = fAspect;
    m_fNearPlane  = fNearPlane;
    m_fFarPlane   = fFarPlane;

    D3DXMatrixPerspectiveFovLH( &m_matProj, fFOV, fAspect, fNearPlane, fFarPlane );
}




