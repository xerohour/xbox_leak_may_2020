//-----------------------------------------------------------------------------
// File: Light.cpp
//
// Desc: Lighting object
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Light.h"
#include <XBApp.h>
#include "Globals.h"
#include "XBUtil.h"




//-----------------------------------------------------------------------------
// Name: Light()
// Desc: Constructor for point light
//-----------------------------------------------------------------------------
Light::Light( FLOAT fRange )
:
    m_vPosition         ( 0.0f, 0.0f, 0.0f ),
    m_qOrientation      ( 0.0f, 0.0f, 0.0f, 1.0f ),
    m_vDiffuseColor     (),
    m_vSpecularColor    (),
    m_vFalloffScale     (),
    m_vFalloffOffset    ( 0.5f, 0.5f, 0.5f ),
    m_pFalloffTexture   ( CreateSphericalFalloffTexture( 32, 32, 32 ) ),
    m_bTransformValid   ( FALSE ),
    m_matLightTransform ()
{
    FLOAT fScale = 1.0f / fRange;
    m_vFalloffScale = D3DXVECTOR3( fScale*0.5f, fScale*0.5f, fScale*0.5f );
}




//-----------------------------------------------------------------------------
// Name: Light()
// Desc: Constructor for spot light
//-----------------------------------------------------------------------------
Light::Light( FLOAT fRange, FLOAT fInnerAngle, FLOAT fOuterAngle )
:
    m_vPosition         ( 0.0f, 0.0f, 0.0f ),
    m_qOrientation      ( 0.0f, 0.0f, 0.0f, 1.0f ),
    m_vDiffuseColor     (),
    m_vSpecularColor    (),
    m_vFalloffScale     (),
    m_vFalloffOffset    ( 0.5f, 0.5f, 0.0f ),
    m_pFalloffTexture   ( NULL ),
    m_bTransformValid   ( FALSE ),
    m_matLightTransform ()
{
    FLOAT fTanInner = tanf( fInnerAngle );
    FLOAT fTanOuter = tanf( fOuterAngle );
    FLOAT fZScale = 1.0f / fRange;
    FLOAT fXYScale = 1.0f / (fRange * fTanOuter);

    m_vFalloffScale = D3DXVECTOR3( fXYScale*0.5f, fXYScale*0.5f, fZScale );
    m_pFalloffTexture = CreateSpotlightFalloffTexture( 32, 32, 64, 
                                                       fTanInner/fTanOuter );
}




//-----------------------------------------------------------------------------
// Name: ~Light()
// Desc: Destroy the light
//-----------------------------------------------------------------------------
Light::~Light()
{
    m_pFalloffTexture->Release();
}




//-----------------------------------------------------------------------------
// Name: SetPosition()
// Desc: Change position of light
//-----------------------------------------------------------------------------
VOID Light::SetPosition( const D3DXVECTOR3& vPosition )
{
    m_bTransformValid = FALSE;
    m_vPosition = vPosition;
}




//-----------------------------------------------------------------------------
// Name: SetOrientation()
// Desc: Change light orientation
//-----------------------------------------------------------------------------
VOID Light::SetOrientation( const D3DXQUATERNION& qOrientation )
{
    m_bTransformValid = FALSE;
    m_qOrientation = qOrientation;
}




//-----------------------------------------------------------------------------
// Name: GetPosition()
// Desc: Returns light position
//-----------------------------------------------------------------------------
const D3DXVECTOR3& Light::GetPosition() const
{
    return m_vPosition;
}




//-----------------------------------------------------------------------------
// Name: GetOrientation()
// Desc: Returns light orientation
//-----------------------------------------------------------------------------
const D3DXQUATERNION& Light::GetOrientation() const
{
    return m_qOrientation;
}




//-----------------------------------------------------------------------------
// Name: GetWorldToLightTransform()
// Desc: Get the transform into light space volume
//-----------------------------------------------------------------------------
const D3DXMATRIX& Light::GetWorldToLightTransform() const
{
    // If light transform needs to be (re)cached, do it now
    if( !m_bTransformValid )
    {
        // Compute world to light volume transform
        D3DXMATRIX matTranslation;
        D3DXMatrixTranslation( &matTranslation, -m_vPosition.x, 
                               -m_vPosition.y, -m_vPosition.z );

        D3DXQUATERNION qInverseOrientation;
        D3DXQuaternionInverse( &qInverseOrientation, &m_qOrientation );

        D3DXMATRIX matRotation;
        D3DXMatrixRotationQuaternion( &matRotation, &qInverseOrientation );

        D3DXMATRIX matScaling;
        D3DXMatrixScaling( &matScaling, m_vFalloffScale.x, m_vFalloffScale.y, 
                           m_vFalloffScale.z );

        // matTranslation * matRotation * matScaling;
        D3DXMatrixMultiply( &m_matLightTransform, &matTranslation,
                            &matRotation );
        D3DXMatrixMultiply( &m_matLightTransform, &m_matLightTransform,
                            &matScaling );

        // Offset by 0.5 to get center
        m_matLightTransform._41 += m_vFalloffOffset.x;
        m_matLightTransform._42 += m_vFalloffOffset.y;
        m_matLightTransform._43 += m_vFalloffOffset.z;

        m_bTransformValid = TRUE;
    }
    
    return m_matLightTransform;
}




//-----------------------------------------------------------------------------
// Name: GetLightFalloffVolume()
// Desc: Get the falloff volume texture for the light
//-----------------------------------------------------------------------------
const LPDIRECT3DVOLUMETEXTURE8 Light::GetLightFalloffVolume() const
{
    return m_pFalloffTexture;
}




//-----------------------------------------------------------------------------
// Name: CreateSpotlightFalloffTexture()
// Desc: Create spotlight texture. Caller is responsible for releasing texture.
//-----------------------------------------------------------------------------
LPDIRECT3DVOLUMETEXTURE8 Light::CreateSpotlightFalloffTexture( INT iWidth, 
                                    INT iHeight, INT iDepth, FLOAT fTanAngle )
{
    // Create a volume texture
    LPDIRECT3DVOLUMETEXTURE8 pTexture;
    HRESULT hr = g_pd3dDevice->CreateVolumeTexture( iWidth, iHeight, iDepth, 
                                                    1, 0, D3DFMT_A8R8G8B8, 
                                                    D3DPOOL_MANAGED,
                                                    &pTexture );
    assert( hr == D3D_OK );
    USED( hr );

    // Lock and fill the volume texture
    D3DVOLUME_DESC desc;
    D3DLOCKED_BOX lock;
    pTexture->GetLevelDesc( 0, &desc );
    pTexture->LockBox( 0, &lock, 0, 0 );
    DWORD* pBits = (DWORD*)lock.pBits;

    for( INT w = 0; w < iDepth; ++w )
    {
        FLOAT z = ( 1.0f * w ) / iDepth;

        for( INT v = 0; v < iHeight; ++v )
        {
            FLOAT y = ( 2.0f * v ) / iHeight - 1.0f + 1.0f/iHeight;

            for( INT u = 0; u < iWidth; ++u )
            {
                FLOAT x = ( 2.0f * u ) / iWidth - 1.0f + 1.0f/iWidth;

                FLOAT fZDiskFalloff = z * z * z;

                // Compute the intensity
                FLOAT fIntensity = 1.0f - fZDiskFalloff;

                FLOAT fXyDist = sqrtf( x*x + y*y );

                if( fXyDist < z )
                {
                    // Inside cone
                    fXyDist /= z;

                    // Linear falloff from inner cone to outer cone
                    if( fXyDist > fTanAngle )
                        fIntensity *= (1.0f - (fXyDist - fTanAngle) / 
                                      (1.0f - fTanAngle));
                }
                else
                {
                    // Outside cone
                    fIntensity = 0.0f;
                }
                    

                // Clamp to range 0 to 1
                if( fIntensity > 1.0f )
                    fIntensity = 1.0f;
                if( fIntensity < 0.0f )
                    fIntensity = 0.0f;

                // Write the texel
                DWORD red = (DWORD)(255 * fIntensity);
                DWORD green = (DWORD)(255 * fIntensity);
                DWORD blue = (DWORD)(255 * fIntensity);
                (*pBits++) = 0xff000000 + (red << 16) + (green << 8) + blue;
            }
        }
    }
    
    // Swizzle and unlock the texture
    XBUtil_SwizzleTexture3D( &lock, &desc );
    pTexture->UnlockBox( 0 );

    return pTexture;
}




//-----------------------------------------------------------------------------
// Name: CreateSpotlightFalloffTexture()
// Desc: Create spotlight texture. Caller is responsible for releasing texture.
//-----------------------------------------------------------------------------
LPDIRECT3DVOLUMETEXTURE8 Light::CreateSphericalFalloffTexture( INT iWidth, 
                                                               INT iHeight, 
                                                               INT iDepth )
{
    // Create a volume texture
    LPDIRECT3DVOLUMETEXTURE8 pTexture;
    HRESULT hr = g_pd3dDevice->CreateVolumeTexture( iWidth, iHeight, iDepth, 
                                                    1, 0, D3DFMT_A8R8G8B8, 
                                                    D3DPOOL_MANAGED,
                                                    &pTexture );
    assert( hr == D3D_OK );
    USED( hr );

    // Lock and fill the volume texture
    D3DVOLUME_DESC desc;
    D3DLOCKED_BOX lock;
    pTexture->GetLevelDesc( 0, &desc );
    pTexture->LockBox( 0, &lock, 0, 0 );
    DWORD* pBits = (DWORD*)lock.pBits;

    for( INT w = 0; w < iDepth; ++w )
    {
        FLOAT z = ( 2.0f * w ) / iDepth - 1.0f + 1.0f/iDepth;

        for( INT v = 0; v < iHeight; ++v )
        {
            FLOAT y = ( 2.0f * v ) / iHeight - 1.0f + 1.0f/iHeight;

            for( INT u = 0; u < iWidth; ++u )
            {
                FLOAT x = ( 2.0f * u ) / iWidth - 1.0f + 1.0f/iWidth;

                FLOAT fDist = x*x + y*y + z*z;

                // Compute the intensity
                FLOAT fIntensity = 1.0f - fDist;

                // Clamp to range 0-1
                if( fIntensity > 1.0f )
                    fIntensity = 1.0f;
                if( fIntensity < 0.0f )
                    fIntensity = 0.0f;

                // Write the texel
                DWORD red = (DWORD)(255 * fIntensity);
                DWORD green = (DWORD)(255 * fIntensity);
                DWORD blue = (DWORD)(255 * fIntensity);
                (*pBits++) = 0xff000000 + (red << 16) + (green << 8) + blue;
            }
        }
    }
    
    // Swizzle and unlock the texture
    XBUtil_SwizzleTexture3D( &lock, &desc );
    pTexture->UnlockBox( 0 );

    return pTexture;
}
