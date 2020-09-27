#include "..\..\inc\d3dlocus.h"
#include "..\..\inc\dxlegacy.h"
#include "..\..\inc\cd3dtest.h"
#include "..\..\inc\3dmath.h"
#include "ball.h"

bool Ball::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, 1.0f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = 0.1f;
	float               fZf = 2.f;

    
	// Projection matrix
	Matrix = OrthoMatrix(2.f, 2.f, fZn, fZf);
//	Matrix = OrthoMatrix(2.f, 2.f * ((float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth()), fZn, fZf);
//	Matrix = OrthoMatrix(2.f * ((float)m_pDisplay->GetWidth() / (float)m_pDisplay->GetHeight()), 2.f, fZn, fZf);
//	m_mProjectionMatrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
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

bool Ball::SetDefaultMaterials(void) {
    D3DMATERIAL8         Material;
    HRESULT              hr;

    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
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

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}


bool Ball::SetDefaultLights(void) {
    D3DLIGHT8   Light;
	D3DVECTOR	vLightPos = cD3DVECTOR(0.f, 0.f, 500.f);
	D3DVECTOR	vLightDir = cD3DVECTOR(0.f, -0.707f, -0.707f);
    HRESULT     hr;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));

	Light.Type              = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r		    = 0.7f;
	Light.Diffuse.g		    = 0.7f;
	Light.Diffuse.b		    = 0.7f;
	Light.Diffuse.a		    = 1.0f;
	Light.Specular.r		= 0.0f;
	Light.Specular.g		= 0.0f;
	Light.Specular.b		= 1.0f;
	Light.Specular.a		= 1.0f;
	Light.Position		    = vLightPos;
	Light.Direction	    	= vLightDir;
	Light.Range             = D3DLIGHT_RANGE_MAX;
	Light.Attenuation0      = 0.f;
	Light.Attenuation1      = 0.f;
	Light.Attenuation2      = 0.f;
    Light.Theta			    = pi/50;
    Light.Phi				= pi/20;

    hr = m_pDevice->SetLight(0, &Light);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}


bool Ball::SetDefaultLightStates(void) {
	SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(90,90,90));
	SetRenderState(D3DRENDERSTATE_LOCALVIEWER,(DWORD)TRUE);
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)TRUE);
#endif // !UNDER_XBOX
    return true;
}


bool Ball::SetDefaultRenderStates(void) {
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)TRUE);
#endif // !UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, (DWORD)TRUE);
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
    if (KeySet("WIREFRAME"))
        SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	return true;
}
