#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "TexGen.h"

bool TexGen::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, -1.0f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
	float               fZn = -0.5f;
	float               fZf = 0.5f;

    
	// Projection matrix
	Matrix = OrthoMatrix(1.0f, 1.0f, fZn, fZf);
//	m_mProjectionMatrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
		return false;

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;
    
    if (m_d3dcaps.MaxVertexBlendMatrices >= 2)
    {
        Matrix = ScaleMatrix(0.5f, 1.f, 1.f);
        if (!SetTransform(D3DTRANSFORMSTATE_WORLD1,&Matrix))
            return false;
    }

//	Matrix = ViewMatrix(from, at, up);
    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    return true;
}

bool TexGen::SetDefaultRenderStates(void) {
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)TRUE);
#endif
    SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, (DWORD)FALSE);
    SetRenderState(D3DRENDERSTATE_LOCALVIEWER, (DWORD)FALSE);
	SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)TEXF_LINEAR);
	return true;
}

bool TexGen::SetDefaultLights(void) {
    D3DLIGHT8   Light;
	D3DVECTOR	vLightPos = cD3DVECTOR(0.f, 0.f, 500.f);
	D3DVECTOR	vLightDir = cD3DVECTOR(0.f, -0.707f, 0.707f);
    HRESULT     hr;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));

	Light.Type           = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r		= 0.7f;
	Light.Diffuse.g		= 0.7f;
	Light.Diffuse.b		= 0.7f;
	Light.Diffuse.a		= 1.0f;
	Light.Specular.r	= 0.0f;
	Light.Specular.g	= 0.0f;
	Light.Specular.b	= 1.0f;
	Light.Specular.a	= 1.0f;
	Light.Position		= vLightPos;
	Light.Direction		= vLightDir;
	Light.Range         = D3DLIGHT_RANGE_MAX;
	Light.Attenuation0  = 1.f;
	Light.Attenuation1  = 0.f;
	Light.Attenuation2  = 0.f;
    Light.Theta			= pi/50;
    Light.Phi			= pi/20;
//	m_pLight = CreateLight();
//	if (NULL == m_pLight)
//		return false;
//
//	m_pLight->SetLight(&Light);
//	if (!SetLight(0,m_pLight))
//		return false;
	
    hr = m_pDevice->SetLight(0, &Light);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}


bool TexGen::SetDefaultLightStates(void) {
	SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(90,90,90));
    return true;
}

bool TexGen::SetDefaultMaterials(void) {
    D3DMATERIAL8        Material;
    HRESULT             hr;

//    m_pMaterial = CreateMaterial();

//    if (NULL == m_pMaterial)
//        return false;

    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
//    Material.dwSize=sizeof(D3DMATERIAL);
    Material.Diffuse.r  = 1.0f;
    Material.Diffuse.g  = 1.0f;
    Material.Diffuse.b  = 1.0f;
    Material.Diffuse.a  = 1.0f;

    Material.Ambient.r  = 1.0f;
    Material.Ambient.g  = 1.0f;
    Material.Ambient.b  = 1.0f;
    Material.Ambient.a  = 1.0f;

    Material.Specular.r = 1.0f;
    Material.Specular.g = 1.0f;
    Material.Specular.b = 1.0f;
    Material.Specular.a = 1.0f;

    Material.Emissive.r = 0.0f;
    Material.Emissive.g = 0.0f;
    Material.Emissive.b = 0.0f;
    Material.Emissive.a = 0.0f;

    Material.Power      = 20.0f;
//    Material.dwRampSize = 32;
//	Material.hTexture	= 0;

//    if (!m_pMaterial->SetMaterial(&Material))
//        return false;

    // Now that the material data has been set,
    // re-set the material so the changes get picked up.

//    if (!SetMaterial(m_pMaterial))
//        return false;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}

