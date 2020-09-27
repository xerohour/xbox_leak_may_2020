#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "LightBall.h"

bool LightBall::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, -100.0f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = 1.f;
	float               fZf = 1000.f;

    
	// Projection matrix
	m_mProjectionMatrix = OrthoMatrix(80.f, 80.f, fZn, fZf);
//	m_mProjectionMatrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&m_mProjectionMatrix))
		return false;

    m_mModelMatrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&m_mModelMatrix))
        return false;

	m_mViewMatrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&m_mViewMatrix))
        return false;

    return true;
}

bool LightBall::SetDefaultRenderStates(void) {
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)TRUE);
#endif
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);
    if (m_dwVersion >= 0x0700)
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
	SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR);
	SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)TEXF_POINT);
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
	return true;
}
