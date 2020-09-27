#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Lighting.h"

bool Lighting::ResolveTest7(UINT uTestNumber)
{
    int test = uTestNumber - 1;
    int tmp = 2/3;
    float               k = 2.f;

    // *************** Scene ************************************
    D3DVECTOR           from;
    D3DVECTOR           at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR           up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float               fZn;
    float               fZf;
    if (test & 0x01)
    {
        from = cD3DVECTOR(0.0f, 0.0f, -200.0f * k);
        fZn = 1.f * k;
        fZf= 300.f * k;

        m_SceneProperties.WorldMatrix = ScaleMatrix(k, k, k);
        m_SceneProperties.ViewMatrix = ViewMatrix(from, at, up);
        m_SceneProperties.ProjectionMatrix = IdentityMatrix();
        m_SceneProperties.ProjectionMatrix._11 = 2.0f / (320.f * k);
        m_SceneProperties.ProjectionMatrix._22 = 2.0f / (280.f * k);
        m_SceneProperties.ProjectionMatrix._33 = 1.0f / (fZf - fZn);
        m_SceneProperties.ProjectionMatrix._43 = -fZn / (fZf - fZn);
    }
    else
    {
        from = cD3DVECTOR(0.0f, 0.0f, -200.0f);
        fZn = 1.f;
        fZf = 300.f;

        m_SceneProperties.WorldMatrix = IdentityMatrix();
        m_SceneProperties.ViewMatrix = ViewMatrix(from, at, up);
        m_SceneProperties.ProjectionMatrix = IdentityMatrix();
        m_SceneProperties.ProjectionMatrix._11 = 2.0f / (320.f);
        m_SceneProperties.ProjectionMatrix._22 = 2.0f / (280.f);
        m_SceneProperties.ProjectionMatrix._33 = 1.0f / (fZf - fZn);
        m_SceneProperties.ProjectionMatrix._43 = -fZn / (fZf - fZn);
    }

    // *************** Model *************************************

    // Switch normalize normals every other frame
    m_ModelProperties.bNormalizeNormals = (test & 0x02) ? true : false;

    // Color & Alpha, combinations: 4, skip: 1
    tmp = (test & 0x03);
    m_ModelProperties.bBlue =       (tmp == 0 || tmp == 3) ? true : false;
    m_ModelProperties.bGreen =      (tmp == 0 || tmp == 2) ? true : false;
    m_ModelProperties.bRed =        (tmp == 0 || tmp == 1) ? true : false;
    m_ModelProperties.bAlpha =      (tmp == 0)             ? true : false;
    test >>= 2;

    // Ambient & Emissive, combinations: 2, skip: 4
    m_ModelProperties.bAmbient =    (test & 0x01)           ? false : true;
    m_ModelProperties.bEmissive =   ( !(test & 0x01) )      ? false : true;
    test >>= 1;

        // Global ambient, combinations: 2, skip: 8
    if (test & 0x01)
    {
        m_ModelProperties.dcvAmbientColor.r = 0.5f;
        m_ModelProperties.dcvAmbientColor.g = 0.5f;
        m_ModelProperties.dcvAmbientColor.b = 0.5f;
    }
    else
    {
        m_ModelProperties.dcvAmbientColor.r = 0.f;
        m_ModelProperties.dcvAmbientColor.g = 0.f;
        m_ModelProperties.dcvAmbientColor.b = 0.f;
    }
    test >>= 1;

    // Ambient source, combinations: 3, skip 16
    tmp = test % 3;
    switch(tmp)
    {
    case 0:     m_ModelProperties.AmbientSource = D3DMCS_MATERIAL;      break;
    case 1:     m_ModelProperties.AmbientSource = D3DMCS_COLOR1;        break;
    case 2:     m_ModelProperties.AmbientSource = D3DMCS_COLOR2;        break;
    }
    test /= 3;

    // Diffuse source, combinations: 3, skip: 48
    tmp = test % 3;
    switch(tmp)
    {
    case 0:     m_ModelProperties.DiffuseSource = D3DMCS_MATERIAL;      break;
    case 1:     m_ModelProperties.DiffuseSource = D3DMCS_COLOR1;        break;
    case 2:     m_ModelProperties.DiffuseSource = D3DMCS_COLOR2;        break;
    }
    test /= 3;

    // Specular source, combinations: 3, skip: 144
    tmp = test % 3;
    switch(tmp)
    {
    case 0:     m_ModelProperties.SpecularSource = D3DMCS_MATERIAL;     break;
    case 1:     m_ModelProperties.SpecularSource = D3DMCS_COLOR1;       break;
    case 2:     m_ModelProperties.SpecularSource = D3DMCS_COLOR2;       break;
    }
    test /= 3;

    // Emissive source, combinations: 3, skip: 432
    tmp = test % 3;
    switch(tmp)
    {
    case 0:     m_ModelProperties.EmissiveSource = D3DMCS_MATERIAL;     break;
    case 1:     m_ModelProperties.EmissiveSource = D3DMCS_COLOR1;       break;
    case 2:     m_ModelProperties.EmissiveSource = D3DMCS_COLOR2;       break;
    }
    test /= 3;

    // Color vertex is based on source material settings
    m_ModelProperties.FogFactorSource = D3DMCS_MATERIAL;
    if (m_ModelProperties.AmbientSource != D3DMCS_MATERIAL ||
        m_ModelProperties.DiffuseSource != D3DMCS_MATERIAL ||
        m_ModelProperties.SpecularSource != D3DMCS_MATERIAL ||
        m_ModelProperties.EmissiveSource != D3DMCS_MATERIAL)
    {
        m_ModelProperties.bColorVertex = true;
    }
    else
    {
        m_ModelProperties.bColorVertex = false;
    }

    // Diffuse, specular and localviewer, combinations: 5, skip: 1296, local viewer skip: 2592
    //  N | L | S | D
    // ---+---+---+---
    //  0 | 0 | 0 | 1
    //  1 | 0 | 1 | 0
    //  2 | 0 | 1 | 1
    //  3 | 1 | 1 | 0
    //  4 | 1 | 1 | 1
    tmp = (test % 5);
    m_ModelProperties.bDiffuse =        (tmp & 0x01) ? false : true;
    m_ModelProperties.bSpecular =       (tmp != 0) ? true : false;
    m_ModelProperties.bLocalViewer =    (tmp > 2) ? true : false;
    test /= 5;


    // Switching light, combinations: 15, skip: 6480
    tmp = (test % 15) + 1;
    m_LightProperties[0].bLightActive = (tmp & 0x03) ? true : false;
    m_LightProperties[1].bLightActive = (tmp & 0x0c) ? true : false;
    test /= 15;

    // First light
    if (m_LightProperties[0].bLightActive)
    {
        switch(tmp & 0x03)      {
        case 1:         m_LightProperties[0].dwLightType = D3DLIGHT_DIRECTIONAL;        break;
        case 2:         m_LightProperties[0].dwLightType = D3DLIGHT_POINT;                      break;
        case 3:         m_LightProperties[0].dwLightType = D3DLIGHT_SPOT;                       break;
        }

        m_LightProperties[0].dvLightPosition = cD3DVECTOR(0.f, 200.f, -200.f);
        m_LightProperties[0].dvLightDirection = Normalize(cD3DVECTOR(0.f, -1.f, 1.f));
        m_LightProperties[0].dvRange = D3DLIGHT_RANGE_MAX;
        m_LightProperties[0].dvFalloff = 1.5f;
        m_LightProperties[0].dvAttenuation0 = 1.f;
        m_LightProperties[0].dvAttenuation1 = 0.f;
        m_LightProperties[0].dvAttenuation2 = 0.f;
        m_LightProperties[0].dvTheta = 0.01f;
        m_LightProperties[0].dvPhi = 1.0f;
    }


    // Second light
    if (m_LightProperties[1].bLightActive)
    {
        switch((tmp >> 2) & 0x03)       {
        case 1:         m_LightProperties[1].dwLightType = D3DLIGHT_DIRECTIONAL;        break;
        case 2:         m_LightProperties[1].dwLightType = D3DLIGHT_POINT;                      break;
        case 3:         m_LightProperties[1].dwLightType = D3DLIGHT_SPOT;                       break;
        }

        m_LightProperties[1].dvLightPosition = cD3DVECTOR(200.f, 200.f, -400.f);
        m_LightProperties[1].dvLightDirection = Normalize(cD3DVECTOR(-1.f, -1.f, 2.f));
        m_LightProperties[1].dvRange = D3DLIGHT_RANGE_MAX;
        m_LightProperties[1].dvFalloff = 1.5f;
        m_LightProperties[1].dvAttenuation0 = 0.5f;
        m_LightProperties[1].dvAttenuation1 = 0.001f;
        m_LightProperties[1].dvAttenuation2 = 0.f;
        m_LightProperties[1].dvTheta = 0.02f;
        m_LightProperties[1].dvPhi = 0.6f;
    }

    if (test > 0)
    {
        WriteToLog("LightTest: Real number of tests is %d\n", uTestNumber);
    }

    return true;
}

bool Lighting::ValidateTest7(int iTest)
{
    // Check device capabilities
    // Number of active lights
    if (m_LightProperties[0].bLightActive &&
        m_LightProperties[1].bLightActive &&
        m_d3dcaps.MaxActiveLights == 0)
    {
        WriteToLog("LightTest7: Device does not support multiple active lights\n");
        SkipTests(291600 - iTest);
        return false;
    }

    // Directional light2
    if (m_LightProperties[1].bLightActive &&
        m_LightProperties[1].dwLightType == D3DLIGHT_DIRECTIONAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
    {
        WriteToLog("LightTest7: Device does not support directional lights\n");
        SkipTests(77760 - (iTest-58321)%77760);
        return false;
    }
    // Point light2
    if (m_LightProperties[1].bLightActive &&
        m_LightProperties[1].dwLightType == D3DLIGHT_POINT &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
    {
        WriteToLog("LightTest7: Device does not support point lights\n");
        SkipTests(77760 - (iTest-58321)%77760);
        return false;
    }
    // Spot light2
    if (m_LightProperties[1].bLightActive &&
        m_LightProperties[1].dwLightType == D3DLIGHT_SPOT &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
    {
        WriteToLog("LightTest7: Device does not support spot lights\n");
        SkipTests(77760 - (iTest-58321)%77760);
        return false;
    }

    // Directional light1
    if (m_LightProperties[0].bLightActive &&
        m_LightProperties[0].dwLightType == D3DLIGHT_DIRECTIONAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
    {
        WriteToLog("LightTest7: Device does not support directional lights\n");
        SkipTests(19440 - (iTest-1)%19440);
        return false;
    }
    // Point light1
    if (m_LightProperties[0].bLightActive &&
        m_LightProperties[0].dwLightType == D3DLIGHT_POINT &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
    {
        WriteToLog("LightTest7: Device does not support point lights\n");
        SkipTests(19440 - (iTest-1)%19440);
        return false;
    }
    // Spot light1
    if (m_LightProperties[0].bLightActive &&
        m_LightProperties[0].dwLightType == D3DLIGHT_SPOT &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
    {
        WriteToLog("LightTest7: Device does not support spot lights\n");
        SkipTests(19440 - (iTest-1)%19440);
        return false;
    }

    // Local viewer
    if (m_ModelProperties.bSpecular &&
        m_ModelProperties.bLocalViewer &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_LOCALVIEWER))
    {
        SkipTests(2592);
        return false;
    }

    // Alpha source
    if (m_ModelProperties.AlphaSource != D3DMCS_MATERIAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        SkipTests(1296 - (iTest-1)%1296);
        return false;
    }

    // Emissive source
    if (m_ModelProperties.EmissiveSource != D3DMCS_MATERIAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        SkipTests(432 - (iTest-1)%432);
        return false;
    }

    // Specular source
    if (m_ModelProperties.SpecularSource != D3DMCS_MATERIAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        SkipTests(144 - (iTest-1)%144);
        return false;
    }

    // Diffuse source
    if (m_ModelProperties.DiffuseSource != D3DMCS_MATERIAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        SkipTests(48 - (iTest-1)%48);
        return false;
    }

    // Ambient source
    if (m_ModelProperties.AmbientSource != D3DMCS_MATERIAL &&
        NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        SkipTests(16 - (iTest-1)%16);
        return false;
    }

    return true;
}
/*
bool Lighting::ResolveTest6(UINT uTestNumber)
{
    int test = uTestNumber - 1;
    int tmp;

//      return SetInitialProperties();

    // *************** Scene ************************************
    D3DVECTOR   from(0.0f, 0.0f, -200.0f);
    D3DVECTOR   at(0.0f, 0.0f, 0.0f);
    D3DVECTOR   up(0.0f, 1.0f, 0.0f);
    float       fZn= 1.f;
    float       fZf= 300.f;
    float                           k;

    //  Scale matrix, combinations: 2, skip: 1
    if (test % 0x01)
        k = 2.f;
    else
        k = 1.f;
    test >>= 1;

    m_SceneProperties.WorldMatrix = ScaleMatrix(k, k, 1.f);
    m_SceneProperties.ViewMatrix = ViewMatrix(from, at, up);
    m_SceneProperties.ProjectionMatrix = IdentityMatrix();
    m_SceneProperties.ProjectionMatrix._11 = 2.0f / 320.f / k;
    m_SceneProperties.ProjectionMatrix._22 = 2.0f / 280.f / k;
    m_SceneProperties.ProjectionMatrix._33 = 1.0f / (fZf - fZn);
    m_SceneProperties.ProjectionMatrix._43 = -fZn / (fZf - fZn);

    // *************** Model *************************************

    // Color & Alpha, combinations: 4, skip: 2
    tmp = (test & 0x03);
    m_ModelProperties.bBlue =       (tmp == 0 || tmp == 3) ? true : false;
    m_ModelProperties.bGreen =      (tmp == 0 || tmp == 2) ? true : false;
    m_ModelProperties.bRed =        (tmp == 0 || tmp == 1) ? true : false;
    m_ModelProperties.bAlpha =      (tmp == 0)             ? true : false;
    test >>= 2;

        // Ambient & Emissive, combinations: 4, skip: 8
    m_ModelProperties.bAmbient =    (test & 0x01)   ? false : true;
    m_ModelProperties.bEmissive =   (test & 0x02)   ? false : true;
    test >>= 2;

        // Global ambient, combinations: 2, skip: 32
    if (test & 0x01)
    {
        m_ModelProperties.dcvAmbientColor.r = 0.5f;
        m_ModelProperties.dcvAmbientColor.g = 0.5f;
        m_ModelProperties.dcvAmbientColor.b = 0.5f;
    }
    else
    {
        m_ModelProperties.dcvAmbientColor.r = 0.f;
        m_ModelProperties.dcvAmbientColor.g = 0.f;
        m_ModelProperties.dcvAmbientColor.b = 0.f;
    }
    test >>= 1;

    // Switching light, combinations: 80, skip: 64
    tmp = (test % 9) + 1;
    m_LightProperties[0].bLightActive = (tmp) ? true : false;
    test /= 9;

    // First light
    if (m_LightProperties[0].bLightActive)
    {
        if (tmp > 4)
            m_LightProperties[0].bUseLight2 = true;

        switch(tmp - ((tmp > 4) ? 4 : 0))       
        {
        case 1:         m_LightProperties[0].dwLightType = D3DLIGHT_DIRECTIONAL;        break;
        case 2:         m_LightProperties[0].dwLightType = D3DLIGHT_POINT;                      break;
        case 3:         m_LightProperties[0].dwLightType = D3DLIGHT_PARALLELPOINT;      break;
        case 4:         m_LightProperties[0].dwLightType = D3DLIGHT_SPOT;                       break;
        }

        m_LightProperties[0].dvLightPosition = D3DVECTOR(0.f, 200.f, -200.f);
        m_LightProperties[0].dvLightDirection = Normalize(D3DVECTOR(0.f, -1.f, 1.f));
        m_LightProperties[0].dvRange = D3DLIGHT_RANGE_MAX;
        m_LightProperties[0].dvFalloff = 1.5f;
        m_LightProperties[0].dvAttenuation0 = 1.f;
        m_LightProperties[0].dvAttenuation1 = 0.f;
        m_LightProperties[0].dvAttenuation2 = 0.f;
        m_LightProperties[0].dvTheta = 0.01f;
        m_LightProperties[0].dvPhi = 1.0f;

    }

    tmp = test % 9;
    m_LightProperties[1].bLightActive = (tmp) ? true : false;
    // First light
    if (m_LightProperties[1].bLightActive)
    {
        if (tmp > 4)
            m_LightProperties[1].bUseLight2 = true;

        switch(tmp - ((tmp > 4) ? 4 : 0)) 
        {
        case 1:         m_LightProperties[1].dwLightType = D3DLIGHT_DIRECTIONAL;        break;
        case 2:         m_LightProperties[1].dwLightType = D3DLIGHT_POINT;                      break;
        case 3:         m_LightProperties[1].dwLightType = D3DLIGHT_PARALLELPOINT;      break;
        case 4:         m_LightProperties[1].dwLightType = D3DLIGHT_SPOT;                       break;
        }

        m_LightProperties[1].dvLightPosition = D3DVECTOR(200.f, 200.f, -400.f);
        m_LightProperties[1].dvLightDirection = Normalize(D3DVECTOR(-1.f, -1.f, 2.f));
        m_LightProperties[1].dvFalloff = 1.5f;
        m_LightProperties[1].dvAttenuation0 = 0.5f;
        m_LightProperties[1].dvAttenuation1 = 0.001f;
        m_LightProperties[1].dvAttenuation2 = 0.f;
        m_LightProperties[1].dvTheta = 0.02f;
        m_LightProperties[1].dvPhi = 0.6f;

    }


    return true;
}
*/


bool Lighting::SetInitialProperties()
{
    // *************** Scene ************************************
    D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, -200.0f);
    D3DVECTOR           at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR           up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float               fZn= 1.f;
    float               fZf= 300.f;

    m_SceneProperties.WorldMatrix = ScaleMatrix(2.f, 2.f, 1.f);
    m_SceneProperties.ViewMatrix = ViewMatrix(from, at, up);
    m_SceneProperties.ProjectionMatrix = IdentityMatrix();
    m_SceneProperties.ProjectionMatrix._11 = 2.0f / 320.f / 2.f;
    m_SceneProperties.ProjectionMatrix._22 = 2.0f / 280 / 2.f;
    m_SceneProperties.ProjectionMatrix._33 = 1.0f / (fZf - fZn);
    m_SceneProperties.ProjectionMatrix._43 = -fZn / (fZf - fZn);

    // *************** Model *************************************
    m_ModelProperties.bDiffuse =    true;
    m_ModelProperties.bSpecular =   true;
    m_ModelProperties.bAmbient =    true;
    m_ModelProperties.bEmissive =   true;
    m_ModelProperties.bAlpha =              true;
    m_ModelProperties.bBlue =               true;
    m_ModelProperties.bGreen =              true;
    m_ModelProperties.bRed =                true;
    m_ModelProperties.dcvAmbientColor.r = 0.5f;
    m_ModelProperties.dcvAmbientColor.g = 0.5f;
    m_ModelProperties.dcvAmbientColor.b = 0.5f;
    m_ModelProperties.bLocalViewer = true;
    m_ModelProperties.AmbientSource = D3DMCS_MATERIAL;
    m_ModelProperties.DiffuseSource = D3DMCS_MATERIAL;
    m_ModelProperties.SpecularSource = D3DMCS_MATERIAL;
    m_ModelProperties.EmissiveSource = D3DMCS_MATERIAL;
    m_ModelProperties.AlphaSource = D3DMCS_MATERIAL;
    m_ModelProperties.FogFactorSource = D3DMCS_MATERIAL;
    m_ModelProperties.bColorVertex = false;

    // First light
    m_LightProperties[0].bLightActive = true;
    m_LightProperties[0].dwLightType = D3DLIGHT_SPOT;
    m_LightProperties[0].dvLightPosition = cD3DVECTOR(0.f, 200.f, -200.f);
    m_LightProperties[0].dvLightDirection = Normalize(cD3DVECTOR(0.f, -1.f, 1.f));
    m_LightProperties[0].dvRange = D3DLIGHT_RANGE_MAX;
    m_LightProperties[0].dvFalloff = 1.5f;
    m_LightProperties[0].dvAttenuation0 = 1.f;
    m_LightProperties[0].dvAttenuation1 = 0.f;
    m_LightProperties[0].dvAttenuation2 = 0.f;
    m_LightProperties[0].dvTheta = 0.01f;
    m_LightProperties[0].dvPhi = 1.0f;
    m_LightProperties[0].bUseLight2 = false;

    // Second light
    m_LightProperties[1].bLightActive = true;
    m_LightProperties[1].dwLightType = D3DLIGHT_POINT;
    m_LightProperties[1].dvLightPosition = cD3DVECTOR(200.f, 200.f, -400.f);
    m_LightProperties[1].dvLightDirection = Normalize(cD3DVECTOR(-1.f, -1.f, 2.f));
    m_LightProperties[1].dvRange = D3DLIGHT_RANGE_MAX;
    m_LightProperties[1].dvFalloff = 1.5f;
    m_LightProperties[1].dvAttenuation0 = 0.5f;
    m_LightProperties[1].dvAttenuation1 = 0.001f;
    m_LightProperties[1].dvAttenuation2 = 0.f;
    m_LightProperties[1].dvTheta = 0.02f;
    m_LightProperties[1].dvPhi = 0.6f;
    m_LightProperties[1].bUseLight2 = true;

    return true;
}



bool Lighting::SetSceneProperties(void)
{
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD, &m_SceneProperties.WorldMatrix))
    {
        WriteToLog("LightTest: Error setting world matrix\n");
        return false;
    }
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW, &m_SceneProperties.ViewMatrix))
    {
        WriteToLog("LightTest: Error setting view matrix\n");
        return false;
    }
    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION, &m_SceneProperties.ProjectionMatrix))
    {
        WriteToLog("LightTest: Error setting projection matrix\n");
        return false;
    }
    return true;
}


bool Lighting::SetModelProperties(void)
{
    D3DMATERIAL8        Material;

    ZeroMemory(&Material, sizeof(Material));

    if (m_ModelProperties.bAmbient)
    {
        if (m_ModelProperties.bRed)     Material.Ambient.r  = 0.5f;
        if (m_ModelProperties.bGreen)   Material.Ambient.g  = 0.5f;
        if (m_ModelProperties.bBlue)    Material.Ambient.b  = 0.5f;
                                        Material.Ambient.a  = 1.0f;
        }

    if (m_ModelProperties.bDiffuse)
    {
        if (m_ModelProperties.bRed)     Material.Diffuse.r  = 1.0f;
        if (m_ModelProperties.bGreen)   Material.Diffuse.g  = 1.0f;
        if (m_ModelProperties.bBlue)    Material.Diffuse.b  = 1.0f;
    }

    if (m_ModelProperties.bAlpha)       Material.Diffuse.a  = 0.5f;
    else                                Material.Diffuse.a  = 1.0f;

    if (m_ModelProperties.bSpecular)
    {
        if (m_ModelProperties.bRed)     Material.Specular.r = 1.0f;
        if (m_ModelProperties.bGreen)   Material.Specular.g = 1.0f;
        if (m_ModelProperties.bBlue)    Material.Specular.b = 1.0f;
                                        Material.Specular.a = 1.0f;
    }

    if (m_ModelProperties.bEmissive)
    {
        if (m_ModelProperties.bRed)     Material.Emissive.r = 0.2f;
        if (m_ModelProperties.bGreen)   Material.Emissive.g = 0.2f;
        if (m_ModelProperties.bBlue)    Material.Emissive.b = 0.2f;
                                        Material.Emissive.a = 1.0f;
    }

    Material.Power      = 9.5f;

    memcpy(&m_ModelProperties.Material, &Material, sizeof(m_ModelProperties.Material));

//    if (!m_pMaterial->SetMaterial(&Material))
//        return false;
//    if (!SetMaterial(m_pMaterial))
//        return false;
    if (FAILED(m_pDevice->SetMaterial(&Material))) {
        return false;
    }

    if (m_dwVersion > 0x0600)
    {
        if (!SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE, (DWORD)m_ModelProperties.AmbientSource))
        {
            WriteToLog("LightTest: Error setting material ambient color source\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, (DWORD)m_ModelProperties.DiffuseSource))
        {
            WriteToLog("LightTest: Error setting material diffuse color source\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE, (DWORD)m_ModelProperties.SpecularSource))
        {
            WriteToLog("LightTest: Error setting material specular color source\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, (DWORD)m_ModelProperties.EmissiveSource))
        {
            WriteToLog("LightTest: Error setting material ambient color source\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_COLORVERTEX, (DWORD)m_ModelProperties.bColorVertex))
        {
            WriteToLog("LightTest: Error switching COLORVERTEX renderstate\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_LOCALVIEWER, (DWORD)m_ModelProperties.bLocalViewer))
        {
            WriteToLog("LightTest: Error switching LOCALVIEWER renderstate\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_AMBIENT, D3DRGB(m_ModelProperties.dcvAmbientColor.r,m_ModelProperties.dcvAmbientColor.g,m_ModelProperties.dcvAmbientColor.b)))
        {
            WriteToLog("LightTest: Error setting ambient color\n");
            return false;
        }

        if (!SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, (DWORD)m_ModelProperties.bNormalizeNormals))
        {
            WriteToLog("LightTest: Error switching NORMALIZENORMALS renderstate\n");
            return false;
        }
    }
/*
    else
    {
        if (!SetLightState(D3DLIGHTSTATE_COLORVERTEX, (DWORD)m_ModelProperties.bColorVertex))
        {
            WriteToLog("LightTest: Error switching COLORVERTEX renderstate\n");
            return false;
        }

        if (!SetLightState(D3DLIGHTSTATE_AMBIENT, D3DRGB(m_ModelProperties.dcvAmbientColor.r,m_ModelProperties.dcvAmbientColor.g,m_ModelProperties.dcvAmbientColor.b)))
        {
            WriteToLog("LightTest: Error setting ambient color\n");
            return false;
        }
    }
*/
    if (!SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)m_ModelProperties.bSpecular))
    {
        WriteToLog("LightTest: Error switching SPECULARENABLE renderstate\n");
        return false;
    }

    return true;
}

bool Lighting::SetLightProperties(void)
{
//    D3DLIGHT        light1;
//    D3DLIGHT2       light2;
    D3DLIGHT8       light8;
    HRESULT         hr;

    // First light
    for (int i = 0; i < LT_LIGHTS; i++)
    {
        if (m_dwVersion > 0x0600)
        {
            if (!LightEnable(i, m_LightProperties[i].bLightActive))
            {
                WriteToLog("LightTest: Can't enable light %d\n", i);
                return false;
            }

            if (m_LightProperties[i].bLightActive)
            {
                ZeroMemory(&light8, sizeof(light8));
                light8.Type = m_LightProperties[i].dwLightType;
                light8.Position = m_LightProperties[i].dvLightPosition;
                light8.Direction = m_LightProperties[i].dvLightDirection;
                light8.Range = m_LightProperties[i].dvRange;
                light8.Falloff = m_LightProperties[i].dvFalloff;
                light8.Attenuation0 = m_LightProperties[i].dvAttenuation0;
                light8.Attenuation1 = m_LightProperties[i].dvAttenuation1;
                light8.Attenuation2 = m_LightProperties[i].dvAttenuation2;
                light8.Theta = m_LightProperties[i].dvTheta;
                light8.Phi = m_LightProperties[i].dvPhi;

                if (i == 0)
                {
                    if (m_ModelProperties.bAmbient)
                    {
                        if (m_ModelProperties.bRed)     light8.Ambient.r = 0.2f;
                        if (m_ModelProperties.bGreen)   light8.Ambient.g = 0.2f;
                        if (m_ModelProperties.bBlue)    light8.Ambient.b = 0.2f;
                    }

                    if (m_ModelProperties.bDiffuse)
                    {
                        if (m_ModelProperties.bRed)     light8.Diffuse.r = 0.8f;
                        if (m_ModelProperties.bGreen)   light8.Diffuse.g = 0.8f;
                        if (m_ModelProperties.bBlue)    light8.Diffuse.b = 0.8f;
                    }

                    if (m_ModelProperties.bSpecular)
                    {
                        if (m_ModelProperties.bRed)     light8.Specular.r = 0.5f;
                        if (m_ModelProperties.bGreen)   light8.Specular.g = 0.5f;
                        if (m_ModelProperties.bBlue)    light8.Specular.b = 0.5f;
                    }
                }
                else
                {
                    if (m_ModelProperties.bAmbient)
                    {
                        if (m_ModelProperties.bRed)     light8.Ambient.r = 0.15f;
                        if (m_ModelProperties.bGreen)   light8.Ambient.g = 0.15f;
                        if (m_ModelProperties.bBlue)    light8.Ambient.b = 0.15f;
                    }

                    if (m_ModelProperties.bDiffuse)
                    {
                        if (m_ModelProperties.bRed)     light8.Diffuse.r = 0.7f;
                        if (m_ModelProperties.bGreen)   light8.Diffuse.g = 0.7f;
                        if (m_ModelProperties.bBlue)    light8.Diffuse.b = 0.7f;
                    }

                    if (m_ModelProperties.bSpecular)
                    {
                        if (m_ModelProperties.bRed)     light8.Specular.r = 0.2f;
                        if (m_ModelProperties.bGreen)   light8.Specular.g = 0.2f;
                        if (m_ModelProperties.bBlue)    light8.Specular.b = 0.2f;
                    }
                }

                m_LightProperties[i].dcvAmbient = light8.Ambient;
                m_LightProperties[i].dcvDiffuse = light8.Diffuse;
                m_LightProperties[i].dcvSpecular = light8.Specular;

                hr = m_pDevice->SetLight(i, &light8);
                if (FAILED(hr)) {
                    WriteToLog("LightTest: Error setting light properties\n");
                    return false;
                }

                hr = m_pDevice->LightEnable(i, TRUE);
                if (FAILED(hr)) {
                    return false;
                }
            }
        }
/*
        else if (m_LightProperties[i].bUseLight2 || !m_LightProperties[i].bLightActive)
        {
                ZeroMemory(&light2, sizeof(light2));
                light2.dwSize = sizeof(light2);
                light2.dltType = m_LightProperties[i].dwLightType;
                light2.dvPosition = m_LightProperties[i].dvLightPosition;
                light2.dvDirection = m_LightProperties[i].dvLightDirection;
                light2.dvRange = m_LightProperties[i].dvRange;
                light2.dvFalloff = m_LightProperties[i].dvFalloff;
                light2.dvAttenuation0 = m_LightProperties[i].dvAttenuation0;
                light2.dvAttenuation1 = m_LightProperties[i].dvAttenuation1;
                light2.dvAttenuation2 = m_LightProperties[i].dvAttenuation2;
                light2.dvTheta = m_LightProperties[i].dvTheta;
                light2.dvPhi = m_LightProperties[i].dvPhi;
                light2.dwFlags = (m_LightProperties[i].bLightActive) ? D3DLIGHT_ACTIVE : 0;

                if (i == 0)
                {
                        light2.dcvColor.r = 0.8f;
                        light2.dcvColor.g = 0.8f;
                        light2.dcvColor.b = 0.8f;
                }
                else
                {
                        light2.dcvColor.r = 0.5f;
                        light2.dcvColor.g = 0.5f;
                        light2.dcvColor.b = 0.5f;
                }

                if (!m_ModelProperties.bSpecular)
                        light2.dwFlags |= D3DLIGHT_NO_SPECULAR;

                m_LightProperties[i].dcvDiffuse = light2.dcvColor;
                m_LightProperties[i].dcvSpecular = light2.dcvColor;

                if (m_pLight[i]->m_pSrcLight)
                {
                        hr = m_pLight[i]->m_pSrcLight->SetLight((D3DLIGHT*)&light2);
                        if ( FAILED(hr) )
                        {
                                WriteToLog("LightTest: Error setting light properties\n");
                                return false;
                        }
                }
                else
                {
                        WriteToLog("LightTest: Error in light initialization\n");
                        return false;
                }
        }
        else
        {
            ZeroMemory(&light1, sizeof(light1));
            light1.dwSize = sizeof(light1);
            light1.dltType = m_LightProperties[i].dwLightType;
            light1.dvPosition = m_LightProperties[i].dvLightPosition;
            light1.dvDirection = m_LightProperties[i].dvLightDirection;
            light1.dvRange = m_LightProperties[i].dvRange;
            light1.dvFalloff = m_LightProperties[i].dvFalloff;
            light1.dvAttenuation0 = m_LightProperties[i].dvAttenuation0;
            light1.dvAttenuation1 = m_LightProperties[i].dvAttenuation1;
            light1.dvAttenuation2 = m_LightProperties[i].dvAttenuation2;
            light1.dvTheta = m_LightProperties[i].dvTheta;
            light1.dvPhi = m_LightProperties[i].dvPhi;

            if (i == 0)
            {
                light1.dcvColor.r = 0.8f;
                light1.dcvColor.g = 0.8f;
                light1.dcvColor.b = 0.8f;
            }
            else
            {
                light1.dcvColor.r = 0.5f;
                light1.dcvColor.g = 0.5f;
                light1.dcvColor.b = 0.5f;
            }

            m_LightProperties[i].dcvDiffuse = light1.dcvColor;
            m_LightProperties[i].dcvSpecular = light1.dcvColor;

            if (m_pLight[i]->m_pSrcLight)
            {
                hr = m_pLight[i]->m_pSrcLight->SetLight(&light1);
                if ( FAILED(hr) )
                {
                    WriteToLog("LightTest: Error setting light properties\n");
                    return false;
                }
            }
            else
            {
                WriteToLog("LightTest: Error in light initialization\n");
                return false;
            }
        }
*/
    }

    return true;
}
