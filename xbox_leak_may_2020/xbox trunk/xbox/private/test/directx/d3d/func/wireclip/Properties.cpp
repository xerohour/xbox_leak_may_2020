#include "d3dlocus.h"
#include "cd3dtest.h"
#include "dxlegacy.h"
#include "3dmath.h"
#include "Clipping.h"


bool WireClip::ResolveTestNumber()
{
    DWORD   num = m_dwCurrentTestNumber - 1;

    // Rotation
    m_dwCurrentRotation = num % m_dwRotations;
    num /= m_dwRotations;

    // Z clipping
#ifdef TESTTYPE_UCLIP
    m_dwCurrentZClip = 0;
#else
    m_dwCurrentZClip = (num & 0x01);
#endif

    return true;
}

bool WireClip::SetWorldMatrix(void) {
	double		dRotation;
	float		cosine, sine;
	D3DMATRIX	Matrix;
	double		Pi = 3.1415926535898;

	// World transformations
	dRotation = ( (2.) / (double)m_dwRotations ) * (double)m_dwCurrentRotation * Pi;
	Matrix = IdentityMatrix();
	cosine = (float) cos(dRotation);
	sine = (float) sin(dRotation);
	Matrix._11 = cosine;
	Matrix._22 = cosine;
	Matrix._12 = -sine;
	Matrix._21 = sine;

    if (m_dwCurrentZClip) {
		D3DMATRIX	ZMatrix;
		ZMatrix = RotateXMatrix(0.12f);
		Matrix = MatrixMult(ZMatrix, Matrix);
	}
    SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix);

    HRESULT hr = GetLastError();
	if (hr != D3D_OK)
	{
		WriteToLog("%s: SetWorldMatrix() returned an error on test %d\n", m_szTestName, m_dwCurrentTestNumber);
		return false;
	}
    return true;		
}

bool WireClip::SetProjectionMatrix(void) {
	D3DMATRIX	Matrix;
	float		fZn=0.5f;
	float		fZf=1.5f;

/*
	// Projection matrix for reference
	Matrix = IdentityMatrix();
	Matrix._11 = 2.0f / 2.f * (float)m_pDisplay->GetWidth() / (float)m_pDisplay->GetHeight();
	Matrix._22 = 2.0f / 2.f;
	Matrix._33 = 1.0f / (fZf - fZn);
	Matrix._43 = -fZn / (fZf - fZn);

//	Matrix = ProjectionMatrix(2.f*atan(1.), ((float)m_pRefTarget->m_dwWidth / (float)m_pRefTarget->m_dwHeight), fZn, fZf);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix,CD3D_REF))
        goto end;
*/
	// Projection matrix for source
#ifndef TESTTYPE_UCLIP
    if (m_dwCurrentZClip) {
		fZn = 0.95f;	
		fZf = 1.05f;	
	}
	Matrix = IdentityMatrix();
	Matrix._11 = 2.0f / 1.f * (float)m_pDisplay->GetWidth() / (float)m_pDisplay->GetHeight();
	Matrix._22 = 2.0f / 1.f;
	Matrix._33 = 1.0f / (fZf - fZn);
	Matrix._43 = -fZn / (fZf - fZn);
//	Matrix = ProjectionMatrix(2.f*atan(0.5), ((float)m_pRefTarget->m_dwWidth / (float)m_pRefTarget->m_dwHeight), fZn, fZf);
#endif
	SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix,CD3D_SRC);

//end:
    HRESULT hr = GetLastError();
	if (hr != D3D_OK)
	{
		WriteToLog("%s: SetProjectionMatrix() returned an error on test %d\n", m_szTestName, m_dwCurrentTestNumber);
		return false;
	}
		
	return true;
}

bool WireClip::SetVertices(void) {
	DWORD	i;
    BYTE    *p = m_pRenderVertices;
    static const DWORD	dwColorList[] = {	RGBA_MAKE(255,0,0,255),
				            				RGBA_MAKE(0,255,0,255),
							            	RGBA_MAKE(0,0,255,255)
							            };

    for (i = 0; i < m_dwVertices; i++)
    {
        // Vertex coordinates
        if (m_dwCurrentFVF & D3DFVF_XYZ)
        {
            ((float*)p)[0] = m_pInputVertices[i].x;
            ((float*)p)[1] = m_pInputVertices[i].y;
            ((float*)p)[2] = m_pInputVertices[i].z;
            
            // Change z value for fog test when we do not have z clipping
            if (m_dwCurrentProperties == CP_FOG && m_dwCurrentZClip == 0)
                ((float*)p)[2] = float(i) / float(m_dwVertices) - 0.5f;
            
            p += 3 * sizeof(float);
        }

        // Normal
        if (m_dwCurrentFVF & D3DFVF_NORMAL)
        {
            ((float*)p)[0] = m_pInputVertices[i].nx;
            ((float*)p)[1] = m_pInputVertices[i].ny;
            ((float*)p)[2] = m_pInputVertices[i].nz;
            p += 3 * sizeof(float);
        }

        // Diffuse color
        if (m_dwCurrentFVF & D3DFVF_DIFFUSE)
        {
            *(DWORD*)p = dwColorList [i % (sizeof(dwColorList)/sizeof(DWORD))];
            // Add alpha for 
            if (m_dwCurrentProperties == CP_ALPHA)
            {
				int alpha = i * 255 / m_dwVertices;
                *(DWORD*)p = (DWORD)((*(DWORD*)p) & 0x00ffffff) + (DWORD)(alpha << 24);
            }
            p += sizeof(DWORD);
        }

        // Specular color and fog
        if (m_dwCurrentFVF & D3DFVF_SPECULAR)
        {
            if (m_dwCurrentProperties == CP_SPECULAR)
            {
				int color = i * 255 / m_dwVertices;
                *(DWORD*)p = RGB_MAKE(color, color, color);
            }
            else
            {
				int fog = i * 255 / m_dwVertices;
                *(DWORD*)p = RGBA_MAKE(0, 0, 0, fog);
            }
            p += sizeof(DWORD);
        }

        DWORD   dwTexCount = (m_dwCurrentFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
        for (int j = 0; j < (int)dwTexCount; j++)
        {
            ((float*)p)[0] = m_pInputVertices[i].x * 0.5f + 0.5f;
            ((float*)p)[1] = m_pInputVertices[i].y * 0.5f + 0.5f;
            p += 2 * sizeof(float);
        }
    }

	return true;
}


bool WireClip::SetProperties(void) 
{
	D3DMATERIAL8	Material;
    D3DLIGHT8		Light;
    DWORD           dwTexCount = (m_dwCurrentFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    HRESULT         hr;
	int				i;
		
	// Clear previous settings
	for (i = m_dwMaxTextures - 1; i >= 0; i--) 
	{
		SetTextureStageState(i,D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
	    SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,(DWORD)i);
	    SetTextureStageState(i,D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
	    SetTextureStageState(i,D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
	    SetTextureStageState(i,D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR);
	    SetTextureStageState(i,D3DTSS_MAGFILTER, (DWORD)TEXF_LINEAR);
		SetTexture(i, 0);
	}

	SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_GOURAUD);
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, (DWORD)D3DFOG_NONE);
#endif // !UNDER_XBOX

	// Default values for material
    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
    Material.Diffuse.r  = 0.9f;
    Material.Diffuse.g  = 0.9f;
    Material.Diffuse.b  = 0.9f;
    Material.Diffuse.a  = 1.0f;

    Material.Ambient.r  = 0.3f;
    Material.Ambient.g  = 0.3f;
    Material.Ambient.b  = 0.3f;
    Material.Ambient.a  = 1.0f;

    Material.Specular.r = 0.0f;
    Material.Specular.g = 0.0f;
    Material.Specular.b = 0.0f;
    Material.Specular.a = 0.0f;

    Material.Power      = 20.0f;

	// Default values for the light source
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
    Light.Direction.z     = -2.0f;
    Light.Range           = D3DLIGHT_RANGE_MAX;
    Light.Attenuation0    = 0.f;
    Light.Attenuation1    = 0.f;
    Light.Attenuation2    = 0.f;

	switch(m_dwCurrentProperties) 
	{
	case CP_GOURAUD:
		// Gouraud shading
        m_pszCurrentProperties = "Gouraud shading";
		break;
    case CP_FLAT:
        // Flat shading
		SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT);
        m_pszCurrentProperties = "Flat shading";
        break;
    case CP_SPECULAR:
        // Specular lighting
		SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)TRUE);
	    Material.Specular.r = 1.0f;
		Material.Specular.g = 1.0f;
		Material.Specular.b = 0.0f;
	    // Put the light source to the new location to receive
		// specular highlights
		Light.Direction.z     = 0.0f;
        m_pszCurrentProperties = "Specular";
        break;
    case CP_ALPHA:
		// Alpha blending
		SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
		SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
		Material.Diffuse.a = 0.5f;
		Material.Ambient.a = 0.5f;
		Material.Specular.a = 0.5f;
        m_pszCurrentProperties = "Alpha blending";
		break;
    case CP_FOG:
		// Vertex fog
		SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD)TRUE);
		SetRenderState(D3DRENDERSTATE_FOGCOLOR,  RGBA_MAKE(0,0,128,255));
#ifndef UNDER_XBOX
		SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, (DWORD)D3DFOG_LINEAR);
		SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD)D3DFOG_NONE);
#else
		SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD)D3DFOG_LINEAR);
#endif // UNDER_XBOX
		SetRenderState(D3DRENDERSTATE_FOGSTART, F2DW(0.5f));
		SetRenderState(D3DRENDERSTATE_FOGEND, F2DW(1.5f));
        m_pszCurrentProperties = "Fog";
		break;
    case CP_TEXTURE:
    case CP_MULTITEX:
		for (i = 0; i < (int)dwTexCount; i++)
		{
			SetTexture(i, m_pTextureArray[i]);
			SetTextureStageState(i, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			if (i) 
			{
				SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
				SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
			}
			else
			{
				SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
				SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
			}
		}
        m_pszCurrentProperties = "Texture";
        break;
    }

    if (m_dwCurrentFVF & D3DFVF_DIFFUSE)
	{
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
	}
	else
	{
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
	    // New data for the light source
        hr = m_pDevice->SetLight(0, &Light);
        if (FAILED(hr)) {
            return false;
        }
        hr = m_pDevice->LightEnable(0, TRUE);
        if (FAILED(hr)) {
            return false;
        }

	    // Set new data for the material
        hr = m_pDevice->SetMaterial(&Material);
        if (FAILED(hr)) {
            return false;
        }
	}

/*
	DWORD	dwResult;
	if (m_dwVersion > 0x0600)
	{
		SetLastError(m_pSrcDevice7->ValidateDevice(&dwResult));
	}
*/

	hr = GetLastError();
	if ( FAILED(hr) )
	{
		WriteToLog("%s: SetProperties() returned an error on test %d\n", m_szTestName, m_dwCurrentTestNumber);
		return false;
	}
		
	return true;
}

bool WireClip::SetUserClipPlanes(void)
{
#ifndef UNDER_XBOX
#ifdef TESTTYPE_UCLIP
    D3DMATRIX           Matrix;
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, 1.0f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);

    float    aspect = float(m_pDisplay->GetHeight()) / float(m_pDisplay->GetWidth());
    float    minX[4] = {-1.f,  0.f,  0.f, 0.5f * aspect};
    float    maxX[4] = { 1.f,  0.f,  0.f, 0.495f * aspect};
    float    minY[4] = { 0.f,  1.f,  0.f, 0.5f};
    float    maxY[4] = { 0.f, -1.f,  0.f, 0.5f};
//  float    minZ[4] = { 0.f,  0.f, -1.f, 0.05f};
//  float    maxZ[4] = { 0.f,  0.f,  1.f, 0.05f};

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    SetClipPlane(0, minX, CD3D_SRC);
    SetClipPlane(1, maxX, CD3D_SRC);
    SetClipPlane(2, minY, CD3D_SRC);
    SetClipPlane(3, maxY, CD3D_SRC);
//  SetClipPlane(4, minZ, CD3D_SRC);
//  SetClipPlane(5, maxZ, CD3D_SRC);

	Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    if (m_dwCurrentZClip)
        return SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, (DWORD)0x3f);
    else
        return SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, (DWORD)0x0f);
#else
    return true;
#endif
#else
    return true;
#endif // UNDER_XBOX
}
