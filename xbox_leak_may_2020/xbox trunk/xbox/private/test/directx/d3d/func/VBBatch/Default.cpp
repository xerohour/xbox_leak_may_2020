//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "VBBatch.h"

bool VBBatch::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, 1.5f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = 0.1f;
	float               fZf = 2.f;

    
	// Projection matrix
	Matrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
		return false;

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;

	Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    return true;
}

bool VBBatch::SetDefaultMaterials(void) {
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
//       return false;

    // Now that the material data has been set,
    // re-set the material so the changes get picked up.

//    if (!SetMaterial(m_pMaterial))
//        return false;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}


bool VBBatch::SetDefaultLights(void) {
    D3DLIGHT8   Light;
	D3DVECTOR	vLightPos = cD3DVECTOR(0.f, 0.f, -500.f);
	D3DVECTOR	vLightDir = cD3DVECTOR(0.f, 0.f, 1.f);
    HRESULT     hr;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));

	Light.Type           = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r		= 1.0f;
	Light.Diffuse.g		= 1.0f;
	Light.Diffuse.b		= 1.0f;
	Light.Diffuse.a		= 1.0f;
	Light.Position		= vLightPos;
	Light.Direction		= vLightDir;
	Light.Range           = D3DLIGHT_RANGE_MAX;
	Light.Attenuation0    = 1.f;
	Light.Attenuation1    = 0.f;
	Light.Attenuation2    = 0.f;
    Light.Theta			= pi/50;
    Light.Phi				= pi/20;
//	m_pLight = CreateLight();
//	if (NULL == m_pLight)
//		return false;

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


bool VBBatch::SetDefaultLightStates(void) {
    if (m_dwVersion >= 0x0700) 
	{
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(90,90,90));
		SetRenderState(D3DRENDERSTATE_LOCALVIEWER,(DWORD)TRUE);
#ifndef UNDER_XBOX
		SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)TRUE);
#endif
	}
	else 
	{
//	    SetLightState(D3DLIGHTSTATE_AMBIENT,RGB_MAKE(50,50,50));
	}
    return true;
}


bool VBBatch::SetDefaultRenderStates(void) {
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
    SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)((m_bSpecular) ? TRUE : FALSE));
    if (m_dwVersion >= 0x0700)
        SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)((m_bLighting) ? TRUE : FALSE));
	return true;
}
