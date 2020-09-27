/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    light.cpp

Author:

    Matt Bronder

Description:

    Direct3D lighting routines.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// Light functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     SetDirectionalLight
//
// Description:
//
//     Set a directional light using the given light data.
//
// Arguments:
//
//     CDevice8* pDevice            - Pointer to the device object
//
//     DWORD dwIndex                - Index of the light to set
//
//     D3DXVECTOR3 vDir             - Direction of the light
//
//     D3DCOLOR c (Optional)        - Color of the light
//
// Return Value:
//
//     A pointer to the created IDirect3DLight object on success,
//     NULL on failure.
//
//******************************************************************************
BOOL SetDirectionalLight(CDevice8* pDevice, DWORD dwIndex, D3DXVECTOR3 vDir, D3DCOLOR c) {

    return SetLight(pDevice, dwIndex, D3DLIGHT_DIRECTIONAL, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
                       vDir, c, c, c, D3DLIGHT_RANGE_MAX, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

//******************************************************************************
//
// Function:
//
//     SetDirectionalLight
//
// Description:
//
//     Set a directional light using the given light data.
//
// Arguments:
//
//     D3DLIGHT8* plight            - Pointer to the light to set
//
//     D3DXVECTOR3 vDir             - Direction of the light
//
//     D3DCOLOR c (Optional)        - Color of the light
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
void SetDirectionalLight(D3DLIGHT8* plight, D3DXVECTOR3 vDir, D3DCOLOR c) {

    SetLight(plight, D3DLIGHT_DIRECTIONAL, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
                       vDir, c, c, c, D3DLIGHT_RANGE_MAX, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f);
}

//******************************************************************************
//
// Function:
//
//     SetPointLight
//
// Description:
//
//     Set a point light using the given light data.
//
// Arguments:
//
//     CDevice8* pDevice            - Pointer to the device object
//
//     DWORD dwIndex                - Index of the light to set
//
//     D3DXVECTOR3 vPos             - Position of the light
//
//     D3DCOLOR c (Optional)        - Color of the light
//
//     float fRange (Optional)      - Range of the light
//
//     float fAttenuation0          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation1          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation2          - Change in the light's intensity
//              (Optional)
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetPointLight(CDevice8* pDevice, DWORD dwIndex, D3DXVECTOR3 vPos, 
                                 D3DCOLOR c, float fRange, 
                                 float fAttenuation0, float fAttenuation1, 
                                 float fAttenuation2)
{
    return SetLight(pDevice, dwIndex, D3DLIGHT_POINT, vPos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), c, c, c,
                       fRange, 1.0f, fAttenuation0, fAttenuation1, fAttenuation2, 
                       0.0f, 0.0f);
}

//******************************************************************************
//
// Function:
//
//     SetPointLight
//
// Description:
//
//     Set a point light using the given light data.
//
// Arguments:
//
//     CDevice8* pDevice            - Pointer to the device object
//
//     D3DXVECTOR3 vPos             - Position of the light
//
//     D3DCOLOR c (Optional)        - Color of the light
//
//     float fRange (Optional)      - Range of the light
//
//     float fAttenuation0          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation1          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation2          - Change in the light's intensity
//              (Optional)
//
// Return Value:
//
//     None.
//
//******************************************************************************
void SetPointLight(D3DLIGHT8* plight, D3DXVECTOR3 vPos, 
                                 D3DCOLOR c, float fRange, 
                                 float fAttenuation0, float fAttenuation1, 
                                 float fAttenuation2)
{
    SetLight(plight, D3DLIGHT_POINT, vPos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), c, c, c,
                       fRange, 1.0f, fAttenuation0, fAttenuation1, fAttenuation2, 
                       0.0f, 0.0f);
}

//******************************************************************************
//
// Function:
//
//     SetSpotLight
//
// Description:
//
//     Set a spot light using the given light data.
//
// Arguments:
//
//     CDevice8* pDevice            - Pointer to the device object
//
//     DWORD dwIndex                - Index of the light to set
//
//     D3DXVECTOR3 vPos             - Position of the light
//
//     D3DXVECTOR3 vDir             - Direction of the light
//
//     D3DCOLOR c (Optional)        - Color of the light
//
//     float fRange (Optional)      - Range of the light
//
//     float fFalloff (Optional)    - Falloff of a spotlight's penumbra
//
//     float fAttenuation0          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation1          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation2          - Change in the light's intensity
//              (Optional)
//
//     float fTheta (Optional)      - Angle of a spotlight's umbra
//
//     float fPhi (Optional)        - Angle of a spotlight's penumbra
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetSpotLight(CDevice8* pDevice, DWORD dwIndex, D3DXVECTOR3 vPos, 
                                D3DXVECTOR3 vDir, D3DCOLOR c, float fRange, 
                                float fFalloff, float fAttenuation0, 
                                float fAttenuation1, float fAttenuation2, 
                                float fTheta, float fPhi)
{
    return SetLight(pDevice, dwIndex, D3DLIGHT_SPOT, vPos, vDir, c, c, c,
                       fRange, fFalloff, fAttenuation0, fAttenuation1, 
                       fAttenuation2, fTheta, fPhi);
}

//******************************************************************************
//
// Function:
//
//     SetSpotLight
//
// Description:
//
//     Set a spot light using the given light data.
//
// Arguments:
//
//     D3DLIGHT8* plight            - Pointer to the light to set
//
//     D3DXVECTOR3 vPos             - Position of the light
//
//     D3DXVECTOR3 vDir             - Direction of the light
//
//     D3DCOLOR c (Optional)        - Color of the light
//
//     float fRange (Optional)      - Range of the light
//
//     float fFalloff (Optional)    - Falloff of a spotlight's penumbra
//
//     float fAttenuation0          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation1          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation2          - Change in the light's intensity
//              (Optional)
//
//     float fTheta (Optional)      - Angle of a spotlight's umbra
//
//     float fPhi (Optional)        - Angle of a spotlight's penumbra
//
// Return Value:
//
//     None.
//
//******************************************************************************
void SetSpotLight(D3DLIGHT8* plight, D3DXVECTOR3 vPos, 
                                D3DXVECTOR3 vDir, D3DCOLOR c, float fRange, 
                                float fFalloff, float fAttenuation0, 
                                float fAttenuation1, float fAttenuation2, 
                                float fTheta, float fPhi)
{
    SetLight(plight, D3DLIGHT_SPOT, vPos, vDir, c, c, c,
                       fRange, fFalloff, fAttenuation0, fAttenuation1, 
                       fAttenuation2, fTheta, fPhi);
}

//******************************************************************************
//
// Function:
//
//     SetLight
//
// Description:
//
//     Set a light using the given light data.
//
// Arguments:
//
//     CDevice8* pDevice            - Pointer to the device object
//
//     DWORD dwIndex                - Index of the light to set
//
//     D3DLIGHTTYPE d3dlt           - Type of light to Set
//
//     D3DXVECTOR3 vPos             - Position of the light
//
//     D3DXVECTOR3 vDir             - Direction of the light
//
//     D3DCOLOR cDiffuse (Optional) - Diffuse color of the light
//
//     D3DCOLOR cSpecular(Optional) - Specular color of the light
//
//     D3DCOLOR cAmbient (Optional) - Ambient color of the light
//
//     float fRange (Optional)      - Range of the light
//
//     float fFalloff (Optional)    - Falloff of a spotlight's penumbra
//
//     float fAttenuation0          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation1          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation2          - Change in the light's intensity
//              (Optional)
//
//     float fTheta (Optional)      - Angle of a spotlight's umbra
//
//     float fPhi (Optional)        - Angle of a spotlight's penumbra
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetLight(CDevice8* pDevice, DWORD dwIndex, D3DLIGHTTYPE d3dlt,
                             D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, 
                             D3DCOLOR cDiffuse, D3DCOLOR cSpecular,
                             D3DCOLOR cAmbient,
                             float fRange, float fFalloff, 
                             float fAttenuation0, float fAttenuation1, 
                             float fAttenuation2, float fTheta, 
                             float fPhi)
{
    D3DLIGHT8   light;
    HRESULT     hr;

    // Parameter validation
    if (!pDevice) {
        return NULL;
    }

    // Initialize the light
    SetLight(&light, d3dlt, vPos, vDir, cDiffuse, cSpecular, cAmbient,
                fRange, fFalloff, fAttenuation0, fAttenuation1,
                fAttenuation2, fTheta, fPhi);

    // Set the light
    hr = pDevice->SetLight(dwIndex, &light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    // Turn on the light
    hr = pDevice->LightEnable(dwIndex, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     SetLight
//
// Description:
//
//     Set a light using the given light data.
//
// Arguments:
//
//     D3DLIGHT8* plight            - Pointer to the light to set
//
//     D3DLIGHTTYPE d3dlt           - Type of light to Set
//
//     D3DXVECTOR3 vPos             - Position of the light
//
//     D3DXVECTOR3 vDir             - Direction of the light
//
//     D3DCOLOR cDiffuse (Optional) - Diffuse color of the light
//
//     D3DCOLOR cSpecular(Optional) - Specular color of the light
//
//     D3DCOLOR cAmbient (Optional) - Ambient color of the light
//
//     float fRange (Optional)      - Range of the light
//
//     float fFalloff (Optional)    - Falloff of a spotlight's penumbra
//
//     float fAttenuation0          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation1          - Change in the light's intensity
//              (Optional)
//
//     float fAttenuation2          - Change in the light's intensity
//              (Optional)
//
//     float fTheta (Optional)      - Angle of a spotlight's umbra
//
//     float fPhi (Optional)        - Angle of a spotlight's penumbra
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
void SetLight(D3DLIGHT8* plight, D3DLIGHTTYPE d3dlt,
                             D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, 
                             D3DCOLOR cDiffuse, D3DCOLOR cSpecular,
                             D3DCOLOR cAmbient,
                             float fRange, float fFalloff, 
                             float fAttenuation0, float fAttenuation1, 
                             float fAttenuation2, float fTheta, 
                             float fPhi)
{
    if (plight) {

        // Initialize the light data
        memset(plight, 0, sizeof(D3DLIGHT8));
        plight->Type = d3dlt;
        plight->Diffuse.r = (float)RGBA_GETRED(cDiffuse) / 255.0f;
        plight->Diffuse.g = (float)RGBA_GETGREEN(cDiffuse) / 255.0f;
        plight->Diffuse.b = (float)RGBA_GETBLUE(cDiffuse) / 255.0f;
        plight->Diffuse.a = (float)RGBA_GETALPHA(cDiffuse) / 255.0f;
        plight->Specular.r = (float)RGBA_GETRED(cSpecular) / 255.0f;
        plight->Specular.g = (float)RGBA_GETGREEN(cSpecular) / 255.0f;
        plight->Specular.b = (float)RGBA_GETBLUE(cSpecular) / 255.0f;
        plight->Specular.a = (float)RGBA_GETALPHA(cSpecular) / 255.0f;
        plight->Ambient.r = (float)RGBA_GETRED(cAmbient) / 255.0f;
        plight->Ambient.g = (float)RGBA_GETGREEN(cAmbient) / 255.0f;
        plight->Ambient.b = (float)RGBA_GETBLUE(cAmbient) / 255.0f;
        plight->Ambient.a = (float)RGBA_GETALPHA(cAmbient) / 255.0f;
        plight->Position = vPos;
        plight->Direction = vDir;
        plight->Range = fRange;
        plight->Falloff = fFalloff;
        plight->Attenuation0 = fAttenuation0;
        plight->Attenuation1 = fAttenuation1;
        plight->Attenuation2 = fAttenuation2;
        plight->Theta = fTheta;
        plight->Phi = fPhi;
    }
}

//******************************************************************************
// Material functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     SetMaterial
//
// Description:
//
//     Initialize a material with the given color information.
//
// Arguments:
//
//     D3DMATERIAL8* pmaterial          - Pointer to the material
//
//     D3DCOLOR cDiffuse (Optional)     - Diffuse component of the material
//
//     D3DCOLOR cAmbient (Optional)     - Ambient component of the material
//
//     D3DCOLOR cSpecular (Optional)    - Specular component of the material
//
//     D3DCOLOR cEmissive (Optional)    - Emissive component of the material
//
//     float fPower (Optional)          - Sharpness of the specular highlights
//
// Return Value:
//
//     None.
//
//******************************************************************************
void SetMaterial(D3DMATERIAL8* pmaterial,
                       D3DCOLOR cDiffuse, D3DCOLOR cAmbient,
                       D3DCOLOR cSpecular, D3DCOLOR cEmissive,
                       float fPower) 
{
    if (pmaterial) {

        // Intialize the material
        memset(pmaterial, 0, sizeof(D3DMATERIAL8));
        pmaterial->Ambient.r = (float)RGBA_GETRED(cAmbient) / 255.0f;
        pmaterial->Ambient.g = (float)RGBA_GETGREEN(cAmbient) / 255.0f;
        pmaterial->Ambient.b = (float)RGBA_GETBLUE(cAmbient) / 255.0f;
        pmaterial->Ambient.a = (float)RGBA_GETALPHA(cAmbient) / 255.0f;
        pmaterial->Diffuse.r = (float)RGBA_GETRED(cDiffuse) / 255.0f;
        pmaterial->Diffuse.g = (float)RGBA_GETGREEN(cDiffuse) / 255.0f;
        pmaterial->Diffuse.b = (float)RGBA_GETBLUE(cDiffuse) / 255.0f;
        pmaterial->Diffuse.a = (float)RGBA_GETALPHA(cDiffuse) / 255.0f;
        pmaterial->Specular.r = (float)RGBA_GETRED(cSpecular) / 255.0f;
        pmaterial->Specular.g = (float)RGBA_GETGREEN(cSpecular) / 255.0f;
        pmaterial->Specular.b = (float)RGBA_GETBLUE(cSpecular) / 255.0f;
        pmaterial->Specular.a = (float)RGBA_GETALPHA(cSpecular) / 255.0f;
        pmaterial->Emissive.r = (float)RGBA_GETRED(cEmissive) / 255.0f;
        pmaterial->Emissive.g = (float)RGBA_GETGREEN(cEmissive) / 255.0f;
        pmaterial->Emissive.b = (float)RGBA_GETBLUE(cEmissive) / 255.0f;
        pmaterial->Emissive.a = (float)RGBA_GETALPHA(cEmissive) / 255.0f;
        pmaterial->Power = fPower;
    }
}

//******************************************************************************
//
// Function:
//
//     SetMaterial
//
// Description:
//
//     Initialize a material with the given color information.
//
// Arguments:
//
//     D3DMATERIAL8* pmaterial          - Pointer to the material
//
//     D3DXVECTOR4 vDiffuse (Optional)  - Diffuse component of the material
//
//     D3DXVECTOR4 vAmbient (Optional)  - Ambient component of the material
//
//     D3DXVECTOR4 vSpecular (Optional) - Specular component of the material
//
//     D3DXVECTOR4 vEmissive (Optional) - Emissive component of the material
//
//     float fPower (Optional)          - Sharpness of the specular highlights
//
// Return Value:
//
//     None.
//
//******************************************************************************
void SetMaterial(D3DMATERIAL8* pmaterial,
                       D3DXVECTOR4 vDiffuse, D3DXVECTOR4 vAmbient,
                       D3DXVECTOR4 vSpecular, D3DXVECTOR4 vEmissive,
                       float fPower) 
{
    if (pmaterial) {

        // Intialize the material
        memset(pmaterial, 0, sizeof(D3DMATERIAL8));
        pmaterial->Ambient.r = vAmbient.x;
        pmaterial->Ambient.g = vAmbient.y;
        pmaterial->Ambient.b = vAmbient.z;
        pmaterial->Ambient.a = vAmbient.w;
        pmaterial->Diffuse.r = vDiffuse.x;
        pmaterial->Diffuse.g = vDiffuse.y;
        pmaterial->Diffuse.b = vDiffuse.z;
        pmaterial->Diffuse.a = vDiffuse.w;
        pmaterial->Specular.r = vSpecular.x;
        pmaterial->Specular.g = vSpecular.y;
        pmaterial->Specular.b = vSpecular.z;
        pmaterial->Specular.a = vSpecular.w;
        pmaterial->Emissive.r = vEmissive.x;
        pmaterial->Emissive.g = vEmissive.y;
        pmaterial->Emissive.b = vEmissive.z;
        pmaterial->Emissive.a = vEmissive.w;
        pmaterial->Power = fPower;
    }
}

//******************************************************************************
//
// Function:
//
//     SetMaterial
//
// Description:
//
//     Set a material in the device.
//
// Arguments:
//
//     CDevice8* pDevice                - Pointer to the device object
//
//     D3DCOLOR cDiffuse (Optional)     - Diffuse component of the material
//
//     D3DCOLOR cAmbient (Optional)     - Ambient component of the material
//
//     D3DCOLOR cSpecular (Optional)    - Specular component of the material
//
//     D3DCOLOR cEmissive (Optional)    - Emissive component of the material
//
//     float fPower (Optional)          - Sharpness of the specular highlights
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetMaterial(CDevice8* pDevice,
                       D3DCOLOR cDiffuse, D3DCOLOR cAmbient,
                       D3DCOLOR cSpecular, D3DCOLOR cEmissive,
                       float fPower) 
{
    D3DMATERIAL8    material;
    HRESULT         hr;

    if (!pDevice) {
        return FALSE;
    }

    // Intialize the material
    SetMaterial(&material, cDiffuse, cAmbient, cSpecular, cEmissive, fPower);

    // Set the material
    hr = pDevice->SetMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     SetMaterial
//
// Description:
//
//     Set a material in the device.
//
// Arguments:
//
//     CDevice8* pDevice                - Pointer to the device object
//
//     D3DXVECTOR4 vDiffuse (Optional)  - Diffuse component of the material
//
//     D3DXVECTOR4 vAmbient (Optional)  - Ambient component of the material
//
//     D3DXVECTOR4 vSpecular (Optional) - Specular component of the material
//
//     D3DXVECTOR4 vEmissive (Optional) - Emissive component of the material
//
//     float fPower (Optional)          - Sharpness of the specular highlights
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetMaterial(CDevice8* pDevice,
                       D3DXVECTOR4 vDiffuse, D3DXVECTOR4 vAmbient,
                       D3DXVECTOR4 vSpecular, D3DXVECTOR4 vEmissive,
                       float fPower)
{
    D3DMATERIAL8    material;
    HRESULT         hr;

    if (!pDevice) {
        return FALSE;
    }

    // Intialize the material
    SetMaterial(&material, vDiffuse, vAmbient, vSpecular, vEmissive, fPower);

    // Set the material
    hr = pDevice->SetMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return FALSE;
    }

    return TRUE;
}

#ifdef UNDER_XBOX
//******************************************************************************
//
// Function:
//
//     SetBackMaterial
//
// Description:
//
//     Set a back material in the device.
//
// Arguments:
//
//     CDevice8* pDevice                - Pointer to the device object
//
//     D3DCOLOR cDiffuse (Optional)     - Diffuse component of the material
//
//     D3DCOLOR cAmbient (Optional)     - Ambient component of the material
//
//     D3DCOLOR cSpecular (Optional)    - Specular component of the material
//
//     D3DCOLOR cEmissive (Optional)    - Emissive component of the material
//
//     float fPower (Optional)          - Sharpness of the specular highlights
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetBackMaterial(CDevice8* pDevice,
                       D3DCOLOR cDiffuse, D3DCOLOR cAmbient,
                       D3DCOLOR cSpecular, D3DCOLOR cEmissive,
                       float fPower) 
{
    D3DMATERIAL8    material;
    HRESULT         hr;

    if (!pDevice) {
        return FALSE;
    }

    // Intialize the material
    SetMaterial(&material, cDiffuse, cAmbient, cSpecular, cEmissive, fPower);

    // Set the material
    hr = pDevice->SetBackMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     SetBackMaterial
//
// Description:
//
//     Set a back material in the device.
//
// Arguments:
//
//     CDevice8* pDevice                - Pointer to the device object
//
//     D3DXVECTOR4 vDiffuse (Optional)  - Diffuse component of the material
//
//     D3DXVECTOR4 vAmbient (Optional)  - Ambient component of the material
//
//     D3DXVECTOR4 vSpecular (Optional) - Specular component of the material
//
//     D3DXVECTOR4 vEmissive (Optional) - Emissive component of the material
//
//     float fPower (Optional)          - Sharpness of the specular highlights
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetBackMaterial(CDevice8* pDevice,
                       D3DXVECTOR4 vDiffuse, D3DXVECTOR4 vAmbient,
                       D3DXVECTOR4 vSpecular, D3DXVECTOR4 vEmissive,
                       float fPower)
{
    D3DMATERIAL8    material;
    HRESULT         hr;

    if (!pDevice) {
        return FALSE;
    }

    // Intialize the material
    SetMaterial(&material, vDiffuse, vAmbient, vSpecular, vEmissive, fPower);

    // Set the material
    hr = pDevice->SetBackMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return FALSE;
    }

    return TRUE;
}
#endif // UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     SetMaterialTranslucency
//
// Description:
//
//     Set the alpha channel information on a material.
//
// Arguments:
//
//     D3DMATERIAL8* pmaterial              - Material to make translucent
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
// Return Value:
//
//     None.
//
//******************************************************************************
void SetMaterialTranslucency(D3DMATERIAL8* pmaterial, float fAlpha) {

    if (pmaterial) {

        // Set the alpha information
        pmaterial->Ambient.a = fAlpha;
        pmaterial->Diffuse.a = fAlpha;
        pmaterial->Specular.a = fAlpha;
        pmaterial->Emissive.a = fAlpha;
    }
}

//******************************************************************************
//
// Function:
//
//     SetMaterialTranslucency
//
// Description:
//
//     Set the alpha channel information on a material.
//
// Arguments:
//
//     CDevice8* pDevice                    - Device containing the material to
//                                            make translucent
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetMaterialTranslucency(CDevice8* pDevice, float fAlpha) {

    D3DMATERIAL8 material;
    HRESULT      hr;

    if (!pDevice) {
        return FALSE;
    }

    // Get the material data
    memset(&material, 0, sizeof(D3DMATERIAL8));
    hr = pDevice->GetMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetMaterial"))) {
        return FALSE;
    }

    // Set the alpha information
    SetMaterialTranslucency(&material, fAlpha);

    // Set the material
    hr = pDevice->SetMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetMaterial"))) {
        return FALSE;
    }

    return TRUE;
}

#ifdef UNDER_XBOX
//******************************************************************************
//
// Function:
//
//     SetMaterialTranslucency
//
// Description:
//
//     Set the alpha channel information on a back material.
//
// Arguments:
//
//     CDevice8* pDevice                    - Device containing the material to
//                                            make translucent
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetBackMaterialTranslucency(CDevice8* pDevice, float fAlpha) {

    D3DMATERIAL8 material;
    HRESULT      hr;

    if (!pDevice) {
        return FALSE;
    }

    // Get the material data
    memset(&material, 0, sizeof(D3DMATERIAL8));
    hr = pDevice->GetBackMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetMaterial"))) {
        return FALSE;
    }

    // Set the alpha information
    SetMaterialTranslucency(&material, fAlpha);

    // Set the material
    hr = pDevice->SetBackMaterial(&material);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetBackMaterial"))) {
        return FALSE;
    }

    return TRUE;
}
#endif // UNDER_XBOX

// ##TODO: Incorporate settings for z, w, and range based fog

#ifndef UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     SetVertexFog
//
// Description:
//
//     Set the appropriate render states to initialize vertex fog in a
//     scene.
//
// Arguments:
//
//     CDevice8* pDevice                - Pointer to the device object
//
//     D3DCOLOR c                       - Fog color
//
//     float fStart                     - Distance at which the fog will begin
//
//     float fEnd                       - Distance at which the fog will end
//
//     float fDensity                   - Density of the fog
//
//     D3DFOGMODE d3dfm                 - Fog intensification
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetVertexFog(CDevice8* pDevice, D3DCOLOR c, float fStart, 
                  float fEnd, float fDensity, D3DFOGMODE d3dfm)
{
    D3DCAPS8 d3dcaps;
    HRESULT hr;

    if (!pDevice) {
        return FALSE;
    }

    hr = pDevice->GetDeviceCaps(&d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return FALSE;
    }

    if (d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX) {

        hr = pDevice->SetRenderState(D3DRS_FOGCOLOR, (DWORD)c);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGSTART, *((LPDWORD)&fStart));
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGEND, *((LPDWORD)&fEnd));
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGDENSITY, *((LPDWORD)&fDensity));
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, (DWORD)d3dfm);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        return TRUE;
    }

    else {
        return FALSE;
    }
}

#endif // !UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     SetPixelFog
//
// Description:
//
//     Set the appropriate render states to initialize pixel fog in a
//     scene.
//
// Arguments:
//
//     CDevice8* pDevice                - Pointer to the device object
//
//     D3DCOLOR c                       - Fog color
//
//     float fStart                     - Distance at which the fog will begin
//
//     float fEnd                       - Distance at which the fog will end
//
//     float fDensity                   - Density of the fog
//
//     D3DFOGMODE d3dfm                 - Fog intensification
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetPixelFog(CDevice8* pDevice, D3DCOLOR c, float fStart, 
                  float fEnd, float fDensity, D3DFOGMODE d3dfm)
{
    D3DCAPS8 d3dcaps;
    HRESULT hr;

    if (!pDevice) {
        return FALSE;
    }

    hr = pDevice->GetDeviceCaps(&d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return FALSE;
    }

    if (d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) {

        hr = pDevice->SetRenderState(D3DRS_FOGCOLOR, (DWORD)c);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGSTART, *((LPDWORD)&fStart));
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGEND, *((LPDWORD)&fEnd));
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGDENSITY, *((LPDWORD)&fDensity));
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        hr = pDevice->SetRenderState(D3DRS_FOGTABLEMODE, (DWORD)d3dfm);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            return FALSE;
        }

        return TRUE;
    }

    else {
        return FALSE;
    }
}
