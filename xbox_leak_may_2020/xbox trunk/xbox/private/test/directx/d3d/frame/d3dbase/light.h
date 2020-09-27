/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    light.h

Author:

    Matt Bronder

Description:

    Direct3D lighting routines.

*******************************************************************************/

#ifndef __LIGHT_H__
#define __LIGHT_H__

//##HACK: Remove this when d3d8 starts defining it again
#ifndef D3DLIGHT_RANGE_MAX
#define D3DLIGHT_RANGE_MAX          ((float)sqrt(FLT_MAX))
#endif

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                SetDirectionalLight(LPDIRECT3DDEVICE8 pDevice, DWORD dwIndex,
                                    D3DXVECTOR3 vDir, D3DCOLOR c = 0xFFFFFF);
void                SetDirectionalLight(D3DLIGHT8* plight, 
                                    D3DXVECTOR3 vDir, D3DCOLOR c = 0xFFFFFF);
BOOL                SetPointLight(LPDIRECT3DDEVICE8 pDevice, DWORD dwIndex,
                                    D3DXVECTOR3 vPos, D3DCOLOR c = 0xFFFFFF, 
                                    float fRange = D3DLIGHT_RANGE_MAX, 
                                    float fAttenuation0 = 1.0f, 
                                    float fAttenuation1 = 0.0f, 
                                    float fAttenuation2 = 0.0f);
void                SetPointLight(D3DLIGHT8* plight, 
                                    D3DXVECTOR3 vPos, D3DCOLOR c = 0xFFFFFF, 
                                    float fRange = D3DLIGHT_RANGE_MAX, 
                                    float fAttenuation0 = 1.0f, 
                                    float fAttenuation1 = 0.0f, 
                                    float fAttenuation2 = 0.0f);
BOOL                SetSpotLight(LPDIRECT3DDEVICE8 pDevice, DWORD dwIndex,
                                    D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, 
                                    D3DCOLOR c = 0xFFFFFF, 
                                    float fRange = D3DLIGHT_RANGE_MAX, 
                                    float fFalloff = 1.0f, 
                                    float fAttenuation0 = 1.0f, 
                                    float fAttenuation1 = 0.0f, 
                                    float fAttenuation2 = 0.0f, 
                                    float fTheta = 0.523598f, 
                                    float fPhi = 0.785397f);
void                SetSpotLight(D3DLIGHT8* plight, 
                                    D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, 
                                    D3DCOLOR c = 0xFFFFFF, 
                                    float fRange = D3DLIGHT_RANGE_MAX, 
                                    float fFalloff = 1.0f, 
                                    float fAttenuation0 = 1.0f, 
                                    float fAttenuation1 = 0.0f, 
                                    float fAttenuation2 = 0.0f, 
                                    float fTheta = 0.523598f, 
                                    float fPhi = 0.785397f);
BOOL                SetLight(LPDIRECT3DDEVICE8 pDevice, DWORD dwIndex,
                                    D3DLIGHTTYPE d3dlt,
                                    D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, 
                                    D3DCOLOR cDiffuse = 0xFFFFFFFF, 
                                    D3DCOLOR cSpecular = 0xFFFFFFFF,
                                    D3DCOLOR cAmbient = 0xFFFFFFFF,
                                    float fRange = D3DLIGHT_RANGE_MAX, 
                                    float fFalloff = 1.0f, 
                                    float fAttenuation0 = 1.0f, 
                                    float fAttenuation1 = 0.0f, 
                                    float fAttenuation2 = 0.0f, 
                                    float fTheta = 0.523598f, 
                                    float fPhi = 0.785397f);
void                SetLight(D3DLIGHT8* plight, 
                                    D3DLIGHTTYPE d3dlt,
                                    D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, 
                                    D3DCOLOR cDiffuse = 0xFFFFFFFF, 
                                    D3DCOLOR cSpecular = 0xFFFFFFFF,
                                    D3DCOLOR cAmbient = 0xFFFFFFFF,
                                    float fRange = D3DLIGHT_RANGE_MAX, 
                                    float fFalloff = 1.0f, 
                                    float fAttenuation0 = 1.0f, 
                                    float fAttenuation1 = 0.0f, 
                                    float fAttenuation2 = 0.0f, 
                                    float fTheta = 0.523598f, 
                                    float fPhi = 0.785397f);

void                SetMaterial(D3DMATERIAL8* pmaterial,
                                    D3DCOLOR cDiffuse = 0xFFFFFFFF, 
                                    D3DCOLOR cAmbient = 0xFF595959,
                                    D3DCOLOR cSpecular = 0xFF404040, 
                                    D3DCOLOR cEmissive = 0xFF000000,
                                    float fPower = 20.0f);
void                SetMaterial(D3DMATERIAL8* pmaterial,
                                    D3DXVECTOR4 vDiffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 
                                    D3DXVECTOR4 vAmbient = D3DXVECTOR4(0.35f, 0.35f, 0.35f, 1.0f),
                                    D3DXVECTOR4 vSpecular = D3DXVECTOR4(0.25f, 0.25f, 0.25f, 1.0f),
                                    D3DXVECTOR4 vEmissive = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f),
                                    float fPower = 20.0f);
BOOL                SetMaterial(LPDIRECT3DDEVICE8 pDevice,
                                    D3DCOLOR cDiffuse = 0xFFFFFFFF, 
                                    D3DCOLOR cAmbient = 0xFF595959,
                                    D3DCOLOR cSpecular = 0xFF404040,
                                    D3DCOLOR cEmissive = 0xFF000000,
                                    float fPower = 20.0f);
BOOL                SetMaterial(LPDIRECT3DDEVICE8 pDevice,
                                    D3DXVECTOR4 vDiffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 
                                    D3DXVECTOR4 vAmbient = D3DXVECTOR4(0.35f, 0.35f, 0.35f, 1.0f),
                                    D3DXVECTOR4 vSpecular = D3DXVECTOR4(0.25f, 0.25f, 0.25f, 1.0f),
                                    D3DXVECTOR4 vEmissive = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f),
                                    float fPower = 20.0f);
void                SetMaterialTranslucency(D3DMATERIAL8* pmaterial, float fAlpha);
BOOL                SetMaterialTranslucency(LPDIRECT3DDEVICE8 pDevice, float fAlpha);

#ifndef UNDER_XBOX
BOOL                SetVertexFog(LPDIRECT3DDEVICE8 pDevice, D3DCOLOR c, 
                                    float fStart, float fEnd, 
                                    float fDensity, D3DFOGMODE d3dfm = D3DFOG_LINEAR);
#endif // !UNDER_XBOX
BOOL                SetPixelFog(LPDIRECT3DDEVICE8 pDevice, D3DCOLOR c, 
                                    float fStart, float fEnd, 
                                    float fDensity, D3DFOGMODE d3dfm = D3DFOG_LINEAR);

#endif //__LIGHT_H__
