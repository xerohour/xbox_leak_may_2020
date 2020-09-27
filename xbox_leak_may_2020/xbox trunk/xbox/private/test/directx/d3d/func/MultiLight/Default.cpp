#include "d3dlocus.h"
#include "dxlegacy.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "MultiLight.h"

bool MultiLight::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, -50.0f*m_fScale); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = 10.f * m_fScale;
	float               fZf = 1000.f * m_fScale;

    
	// Projection matrix
//	Matrix = OrthoMatrix(80.f, 80.f, fZn, fZf);
	Matrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
		return false;

    Matrix = MatrixMult(ScaleMatrix(m_fScale, m_fScale, m_fScale), RotateYMatrix(-1.2f));
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;

	Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    return true;
}

bool MultiLight::SetDefaultMaterials(void) {
    D3DMATERIAL8         Material;
    HRESULT              hr;

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

//    return true;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}


bool MultiLight::SetDefaultLights(void) {
	return true;
}


bool MultiLight::SetDefaultLightStates(void) {
    if (m_dwVersion >= 0x0700) 
	{
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(50,50,50));
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


bool MultiLight::SetDefaultRenderStates(void) {
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)TRUE);
#endif
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);
    SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, (DWORD)TRUE);
    SetRenderState(D3DRENDERSTATE_WRAP0, (DWORD)(D3DWRAP_U | D3DWRAP_V));
    if (m_dwVersion >= 0x0700) {
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
	}
	SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR);
	SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)TEXF_POINT);
//	SetTextureStageState(0, D3DTSS_ADDRESS, (DWORD)D3DTADDRESS_CLAMP);
	return true;
}
