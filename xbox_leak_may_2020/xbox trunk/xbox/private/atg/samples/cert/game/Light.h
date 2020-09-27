//-----------------------------------------------------------------------------
// File: Light.h
//
// Desc: Lighting object
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_LIGHT_H
#define TECH_CERT_GAME_LIGHT_H

#include "Common.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <vector>
#pragma warning( pop )




//-----------------------------------------------------------------------------
// Global types
//-----------------------------------------------------------------------------
class Light;
typedef std::vector< Light* > LightList;




//-----------------------------------------------------------------------------
// Name: class Light
// Desc: Light object
//-----------------------------------------------------------------------------
class Light
{
    D3DXVECTOR3              m_vPosition;         // position
    D3DXQUATERNION           m_qOrientation;      // orientation
    D3DXVECTOR3              m_vDiffuseColor;     // diffuse intensity
    D3DXVECTOR3              m_vSpecularColor;    // specular intensity
    D3DXVECTOR3              m_vFalloffScale;     // falloff scale
    D3DXVECTOR3              m_vFalloffOffset;    // falloff offset
    LPDIRECT3DVOLUMETEXTURE8 m_pFalloffTexture;   // falloff volume
    mutable D3DXMATRIX       m_matLightTransform; // cached light transform
    mutable BOOL             m_bTransformValid;                             

public:

    // Create a point light
    Light( FLOAT fRange );

    // Create a spot light
    Light( FLOAT fRange, FLOAT fInnerAngle, FLOAT fOuterAngle );

    ~Light();

    // Move the light around
    VOID SetPosition(const D3DXVECTOR3& vPosition);
    VOID SetOrientation(const D3DXQUATERNION& qOrientation);

    const D3DXVECTOR3&    GetPosition() const;
    const D3DXQUATERNION& GetOrientation() const;

    // Get the transform into light space volume
    const D3DXMATRIX& GetWorldToLightTransform() const;

    // Get the falloff volume texture for the light
    const LPDIRECT3DVOLUMETEXTURE8 GetLightFalloffVolume() const;

private:

    // Disabled
    Light();

    static LPDIRECT3DVOLUMETEXTURE8 CreateSphericalFalloffTexture( INT, INT, INT );
    static LPDIRECT3DVOLUMETEXTURE8 CreateSpotlightFalloffTexture( INT, INT, INT, FLOAT );

};




#endif // TECH_CERT_GAME_LIGHT_H
