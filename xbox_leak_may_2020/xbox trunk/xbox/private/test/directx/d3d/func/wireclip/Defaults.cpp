#include "d3dlocus.h"
#include "cd3dtest.h"
#include "dxlegacy.h"
#include "3dmath.h"
#include "Clipping.h"

bool WireClip::SetupViewport() {

#ifdef TESTTYPE_UCLIP
    return SetViewport(0,0,m_pDisplay->GetWidth(),m_pDisplay->GetHeight());
#endif
    bool    fSrcResult;
    bool    fRefResult;

    fSrcResult = SetViewport(m_pDisplay->GetWidth()/4, m_pDisplay->GetHeight()/4,
                             m_pDisplay->GetWidth()/2, m_pDisplay->GetHeight()/2,
                             0.f, 1.f, CD3D_SRC);

    return fSrcResult;

/*
	HRESULT		hrLastError = 0;

    D3DVIEWPORT2 ViewDataSrc;
    D3DVIEWPORT2 ViewDataRef;

    // Setup the viewports to encompass the entire render targets

    ZeroMemory(&ViewDataSrc,sizeof(D3DVIEWPORT2));
    ViewDataSrc.dwSize=sizeof(D3DVIEWPORT2);
    ViewDataSrc.dwX				= m_pSrcTarget->m_dwWidth / 4;
	ViewDataSrc.dwY				= m_pSrcTarget->m_dwHeight / 4;
    ViewDataSrc.dwWidth			= m_pSrcTarget->m_dwWidth / 2;  // - 2*m_dwGuardSize;
    ViewDataSrc.dwHeight		= m_pSrcTarget->m_dwHeight / 2; // - 2*m_dwGuardSize;
    ViewDataSrc.dvClipX			= -1.0f;
    ViewDataSrc.dvClipY			=  1.0f;
    ViewDataSrc.dvClipWidth		=  2.0f;
    ViewDataSrc.dvClipHeight	=  2.0f;
    ViewDataSrc.dvMinZ			=  0.0f;
    ViewDataSrc.dvMaxZ			=  1.0f;

	if (NULL != m_pRefTarget) {
		ZeroMemory(&ViewDataRef,sizeof(D3DVIEWPORT2));
		ViewDataRef.dwSize=sizeof(D3DVIEWPORT2);
		ViewDataRef.dwX            = 0;
		ViewDataRef.dwY            = 0;
		ViewDataRef.dwWidth        = m_pRefTarget->m_dwWidth;
		ViewDataRef.dwHeight       = m_pRefTarget->m_dwHeight;
		ViewDataRef.dvClipX        = -1.0f;
		ViewDataRef.dvClipY        =  1.0f;
		ViewDataRef.dvClipWidth    =  2.0f;
		ViewDataRef.dvClipHeight   =  2.0f;
		ViewDataRef.dvMinZ         =  0.0f;
		ViewDataRef.dvMaxZ         =  1.0f;
	}

    if (m_dwVersion >= 0x0700) {
        D3DVIEWPORT7    Data7;

        Data7.dwX = ViewDataSrc.dwX;
        Data7.dwY = ViewDataSrc.dwY;
        Data7.dwWidth = ViewDataSrc.dwWidth;
        Data7.dwHeight = ViewDataSrc.dwHeight;
        Data7.dvMinZ = ViewDataSrc.dvMinZ;
        Data7.dvMaxZ = ViewDataSrc.dvMaxZ;
        hrLastError = m_pSrcDevice7->SetViewport(&Data7);
        if (FAILED(hrLastError))
		{
            SetLastError(hrLastError);
			return false;
		}

        if (NULL != m_pRefTarget) {
			Data7.dwX = ViewDataRef.dwX;
			Data7.dwY = ViewDataRef.dwY;
			Data7.dwWidth = ViewDataRef.dwWidth;
			Data7.dwHeight = ViewDataRef.dwHeight;
			Data7.dvMinZ = ViewDataRef.dvMinZ;
			Data7.dvMaxZ = ViewDataRef.dvMaxZ;
            hrLastError = m_pRefDevice7->SetViewport(&Data7);
			if (FAILED(hrLastError))
			{
				SetLastError(hrLastError);
				return false;
			}
		}
    }
    else {
        if (NULL != m_pD3D3)
            hrLastError = m_pSrcViewport3->SetViewport2(&ViewDataSrc);
		if (FAILED(hrLastError))
		{
            SetLastError(hrLastError);
			return false;
		}

        if (NULL == m_pRefTarget)
            return true;

        if (NULL != m_pD3D3)
            hrLastError = m_pRefViewport3->SetViewport2(&ViewDataRef);
        if (FAILED(hrLastError))
		{
            SetLastError(hrLastError);
			return false;
		}
    }

    return true;
*/
}



bool WireClip::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DXVECTOR3         from(0.0f, 0.0f, 1.0f); 
    D3DXVECTOR3	        at(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3	        up(0.0f, 1.0f, 0.0f);
    float               fZn=0.5f;
	float               fZf=1.5f;

    
	// Setup an orthogonal projection

/*
	// Projection matrix for reference
    Matrix = IdentityMatrix();
	Matrix._11 = 2.0f / 3.f * (float)m_pRefTarget->m_dwWidth / (float)m_pRefTarget->m_dwHeight;
	Matrix._22 = 2.0f / 3.f;
	Matrix._33 = 1.0f / (fZf - fZn);
	Matrix._43 = -fZn / (fZf - fZn);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix,CD3D_REF))
		return false;
*/

	// Projection matrix for source
#ifndef TESTTYPE_UCLIP
    Matrix = IdentityMatrix();
	Matrix._11 = 2.0f / 1.5f * (float)m_pDisplay->GetWidth() / (float)m_pDisplay->GetHeight();
	Matrix._22 = 2.0f / 1.5f;
	Matrix._33 = 1.0f / (fZf - fZn);
	Matrix._43 = -fZn / (fZf - fZn);
#endif
	if (!SetTransform(D3DTS_PROJECTION,&Matrix,CD3D_SRC))
		return false;

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;

//	Matrix = IdentityMatrix();
	Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    return true;
}

bool WireClip::SetDefaultMaterials(void) {
    D3DMATERIAL8        Material;
    HRESULT             hr;

    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
    Material.Diffuse.r  = 0.0f;
    Material.Diffuse.g  = 0.0f;
    Material.Diffuse.b  = 0.9f;
    Material.Diffuse.a  = 1.0f;

    Material.Ambient.r  = 0.0f;
    Material.Ambient.g  = 0.0f;
    Material.Ambient.b  = 0.3f;
    Material.Ambient.a  = 1.0f;

    Material.Specular.r = 0.0f;
    Material.Specular.g = 0.0f;
    Material.Specular.b = 0.0f;
    Material.Specular.a = 0.0f;

    Material.Power      = 20.0f;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}

bool WireClip::SetDefaultLightStates(void) {
	SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(25,25,25));
	SetRenderState(D3DRENDERSTATE_LOCALVIEWER,(DWORD)FALSE);
    return true;
}

bool WireClip::SetDefaultRenderStates(void) {
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)TRUE);
#endif // !UNDER_XBOX
    SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);

#ifdef TESTTYPE_WIREFRAME
    SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
#endif

	return true;
}

bool WireClip::SetDefaultLights(void) {
    D3DLIGHT8   Light;
    HRESULT     hr;

    ZeroMemory(&Light,sizeof(D3DLIGHT8));

    Light.Type           = D3DLIGHT_DIRECTIONAL;
    Light.Diffuse.r        = 1.0f;
    Light.Diffuse.g        = 1.0f;
    Light.Diffuse.b        = 1.0f;
    Light.Diffuse.a        = 1.0f;
    Light.Position.x      = 0.0f;
    Light.Position.y      = 0.0f;
    Light.Position.z      = 0.0f;
    Light.Direction.x     = 0.0f;
    Light.Direction.y     = -1.0f;
    Light.Direction.z     = 0.0f;
    Light.Range           = D3DLIGHT_RANGE_MAX;
    Light.Attenuation0    = 0.f;
    Light.Attenuation1    = 0.f;
    Light.Attenuation2    = 0.f;

    hr = m_pDevice->SetLight(0, &Light);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}

