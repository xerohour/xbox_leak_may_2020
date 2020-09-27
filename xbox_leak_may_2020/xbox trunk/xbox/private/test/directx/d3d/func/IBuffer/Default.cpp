#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "IBuffer.h"

bool IBuffer::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
	D3DVECTOR           from; 
    D3DVECTOR	        at;
    D3DVECTOR	        up;
	float               fZn = 1.f;
	float               fZf = 3.f;

    from.x = 0.0f; from.y = 0.0f; from.z = -2.0f;
    at.x = 0.0f; at.y = 0.0f; at.z = 0.0f;
    up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	// Projection matrix
	Matrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	if (!SetTransform(D3DTS_PROJECTION,&Matrix))
		return false;

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTS_WORLD,&Matrix))
        return false;

	Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTS_VIEW,&Matrix))
        return false;

    return true;
}

bool IBuffer::SetDefaultMaterials(void) {
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


bool IBuffer::SetDefaultLights(void) {
    D3DLIGHT8   Light;
	D3DVECTOR	vLightPos;
	D3DVECTOR	vLightDir;
    HRESULT     hr;

    vLightPos.x = 0.f; vLightPos.y = 2.f; vLightPos.z = -2.f;
    vLightDir.x = 0.f; vLightDir.y = -0.707f; vLightDir.z = 0.707f;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));
	Light.Type           = D3DLIGHT_SPOT;
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
	Light.Range         = 1000.f;
	Light.Attenuation0  = 1.f;
	Light.Attenuation1  = 0.f;
	Light.Attenuation2  = 0.f;
    Light.Theta			= pi/10;
    Light.Phi			= pi/2;

    hr = m_pDevice->SetLight(0, &Light);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}


bool IBuffer::SetDefaultLightStates(void) {
    SetRenderState(D3DRS_AMBIENT, (DWORD)((90<<16)|(90<<8)|(90)) );
    SetRenderState(D3DRS_LOCALVIEWER,(DWORD)TRUE);
#ifndef UNDER_XBOX
    SetRenderState(D3DRS_CLIPPING, (DWORD)TRUE);
#endif
    return true;
}


bool IBuffer::SetDefaultRenderStates(void) {
#ifndef UNDER_XBOX
	SetRenderState(D3DRS_LASTPIXEL, (DWORD)TRUE);
#endif
	SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
	return true;
}
