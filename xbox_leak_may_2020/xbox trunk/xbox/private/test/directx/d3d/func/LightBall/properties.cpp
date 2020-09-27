#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "LightBall.h"

bool LightBall::ResolveTestNumber()
{
	m_dwCurrentTransform = m_dwCurrentTestNumber % m_dwSameScenarioFrames;
	m_dwCurrentScenario = m_dwCurrentTestNumber / m_dwSameScenarioFrames;

	// Use projected textures
	if (m_dwCurrentScenario & 0x01)
	{
		if (!CheckProjectedTextures())
			return false;
		m_bProjectedTexture = true;
	}
	else
	{
		m_bProjectedTexture = false;
	}

	// Bumpmapping
	if (m_dwCurrentScenario & 0x02)
	{
		if (NULL == m_pBumpTexture)
			return false;
		m_bBumpMapping = true;
	}
	else
	{
		m_bBumpMapping = false;
	}

	// Lumonance texture
	if (m_dwCurrentScenario & 0x04)
	{
		if (NULL == m_pLuminanceTexture)
			return false;
		m_bLuminanceTexture = true;
	}
	else
	{
		m_bLuminanceTexture = false;
	}

    if (m_dwCurrentScenario & 0x08)
    {
        return false; // Pure TnL
/*
        if (IID_IDirect3DTnLHalDevice == m_pAdapter->Devices[m_pMode->nSrcDevice].Guid || 
            IID_IDirect3DTnLRefDevice == m_pAdapter->Devices[m_pMode->nSrcDevice].Guid || 
            IID_IDirect3DTnLHalDevice == m_pAdapter->Devices[m_pMode->nRefDevice].Guid || 
            IID_IDirect3DTnLRefDevice == m_pAdapter->Devices[m_pMode->nRefDevice].Guid)
                return false;
        m_bUseVertexBuffer = true;
*/
    }
    else
    {
        m_bUseVertexBuffer = false;
    }

	return true;
}

bool LightBall::SetCurrentTransform()
{
	D3DMATRIX	TransformMatrix;
	float		fZn = 1.f;
	float		fZf = 1000.f;

	m_mRotationMatrix = IdentityMatrix();
	m_mModelMatrix = IdentityMatrix();
	TransformMatrix = RotateXMatrix(m_dwCurrentTransform / 20.f);
	m_mRotationMatrix = MatrixMult(TransformMatrix, m_mRotationMatrix);
	TransformMatrix = RotateYMatrix(m_dwCurrentTransform / 10.f);
	m_mRotationMatrix = MatrixMult(TransformMatrix, m_mRotationMatrix);
	TransformMatrix = RotateZMatrix(m_dwCurrentTransform / 5.f);
	m_mRotationMatrix = MatrixMult(TransformMatrix, m_mRotationMatrix);
	TransformMatrix = TranslateMatrix(	40.f * (float)sin(m_dwCurrentTransform / 25.f * pi),
										40.f * (float)sin(m_dwCurrentTransform / 50.f * pi),
										-40.f * (float)cos(m_dwCurrentTransform / 25.f * pi));
	m_mModelMatrix = MatrixMult(TransformMatrix, m_mRotationMatrix);
	SetTransform(D3DTRANSFORMSTATE_WORLD, &m_mModelMatrix);

	if (m_bProjectedTexture)
		m_mProjectionMatrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	else
		m_mProjectionMatrix = OrthoMatrix(100.f, 100.f, fZn, fZf);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&m_mProjectionMatrix))
		return false;

	return true;
}

bool LightBall::SetTextureTransform()
{
	// Texture transform
	D3DMATRIX	scale;
	D3DMATRIX	mwv;
	D3DMATRIX	mwvp;
	D3DMATRIX	mwvps;
	D3DMATRIX	mwvt;
	D3DVALUE	v[4] = {0.5f, 0.f, 0.f, 0.5f};
	D3DVALUE	u[4] = {0.f, 0.5f, 0.f, 0.5f};
	D3DVALUE	r[4] = {0.f, 0.f, 0.f, 1.f};
	D3DVALUE	ut[4];
	D3DVALUE	vt[4];
	D3DVALUE	rt[4];

	
	// Projection matrix
	mwv = MatrixMult(m_mViewMatrix, m_mModelMatrix);
	mwvp = MatrixMult(m_mProjectionMatrix, mwv);

	if (m_bBumpMapping)
	{
		mwvt = MatrixTranspose(mwvp);
	}
	else
	{
		scale = ScaleMatrix(0.6f, 0.6f, 0.6f);
		mwvps = MatrixMult(mwvp, scale);
		mwvt = MatrixTranspose(mwvps);
	}

	ut[0] = mwvt._11*u[0] + mwvt._21*u[1] + mwvt._31*u[2] + mwvt._41*u[3];
	ut[1] = mwvt._12*u[0] + mwvt._22*u[1] + mwvt._32*u[2] + mwvt._42*u[3];
	ut[2] = mwvt._13*u[0] + mwvt._23*u[1] + mwvt._33*u[2] + mwvt._43*u[3];
	ut[3] = mwvt._14*u[0] + mwvt._24*u[1] + mwvt._34*u[2] + mwvt._44*u[3];

	vt[0] = mwvt._11*v[0] + mwvt._21*v[1] + mwvt._31*v[2] + mwvt._41*v[3];
	vt[1] = mwvt._12*v[0] + mwvt._22*v[1] + mwvt._32*v[2] + mwvt._42*v[3];
	vt[2] = mwvt._13*v[0] + mwvt._23*v[1] + mwvt._33*v[2] + mwvt._43*v[3];
	vt[3] = mwvt._14*v[0] + mwvt._24*v[1] + mwvt._34*v[2] + mwvt._44*v[3];

	rt[0] = mwvt._11*r[0] + mwvt._21*r[1] + mwvt._31*r[2] + mwvt._41*r[3];
	rt[1] = mwvt._12*r[0] + mwvt._22*r[1] + mwvt._32*r[2] + mwvt._42*r[3];
	rt[2] = mwvt._13*r[0] + mwvt._23*r[1] + mwvt._33*r[2] + mwvt._43*r[3];
	rt[3] = mwvt._14*r[0] + mwvt._24*r[1] + mwvt._34*r[2] + mwvt._44*r[3];

	InitMatrix(&m_mProjectionTransform,
		ut[0], vt[0], rt[0], 0.f,
		ut[1], vt[1], rt[1], 0.f,
		ut[2], vt[2], rt[2], 0.f,
		ut[3], vt[3], rt[3], 0.f
	);

	// Sphere matrix
	mwv = MatrixMult(m_mViewMatrix, m_mRotationMatrix);
	mwvp = MatrixMult(m_mProjectionMatrix, mwv);
	scale = ScaleMatrix(3.f, 3.f, 3.f);
	mwvps = MatrixMult(mwvp, scale);
	mwvt = MatrixTranspose(mwvps);

	ut[0] = mwvt._11*u[0] + mwvt._21*u[1] + mwvt._31*u[2] + mwvt._41*u[3];
	ut[1] = mwvt._12*u[0] + mwvt._22*u[1] + mwvt._32*u[2] + mwvt._42*u[3];
	ut[2] = mwvt._13*u[0] + mwvt._23*u[1] + mwvt._33*u[2] + mwvt._43*u[3];
	ut[3] = mwvt._14*u[0] + mwvt._24*u[1] + mwvt._34*u[2] + mwvt._44*u[3];

	vt[0] = mwvt._11*v[0] + mwvt._21*v[1] + mwvt._31*v[2] + mwvt._41*v[3];
	vt[1] = mwvt._12*v[0] + mwvt._22*v[1] + mwvt._32*v[2] + mwvt._42*v[3];
	vt[2] = mwvt._13*v[0] + mwvt._23*v[1] + mwvt._33*v[2] + mwvt._43*v[3];
	vt[3] = mwvt._14*v[0] + mwvt._24*v[1] + mwvt._34*v[2] + mwvt._44*v[3];

	rt[0] = mwvt._11*r[0] + mwvt._21*r[1] + mwvt._31*r[2] + mwvt._41*r[3];
	rt[1] = mwvt._12*r[0] + mwvt._22*r[1] + mwvt._32*r[2] + mwvt._42*r[3];
	rt[2] = mwvt._13*r[0] + mwvt._23*r[1] + mwvt._33*r[2] + mwvt._43*r[3];
	rt[3] = mwvt._14*r[0] + mwvt._24*r[1] + mwvt._34*r[2] + mwvt._44*r[3];

	InitMatrix(&m_mSphereTransform,
		ut[0], vt[0], rt[0], 0.f,
		ut[1], vt[1], rt[1], 0.f,
		ut[2], vt[2], rt[2], 0.f,
		ut[3], vt[3], rt[3], 0.f
	);

	return true;
}

/*
bool LightBall::FindRGBTextureFormat()
{
    for (int i = 0; i < m_uCommonTextureFormats; i++)
	{
		if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwRGBBitCount == 16 &&
			m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_RGB)
		{
			m_pMode->nTextureFormat = i;
			return true;
		}
	}

	return false;
}


bool LightBall::FindBumpmapTextureFormat()
{
    if (!(m_pAdapter->Devices[m_pMode->nSrcDevice].Desc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP) ||
		!(m_pAdapter->Devices[m_pMode->nSrcDevice].Desc.dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE))
    {
        return false;
    }

    if (m_pAdapter->Devices[m_pMode->nSrcDevice].Desc.wMaxTextureBlendStages < 2)
    {
        return false;
    }

    for (int i = 0; i < m_uCommonTextureFormats; i++)
	{
		if ((16 == m_pCommonTextureFormats[i].ddpfPixelFormat.dwBumpBitCount) &&
			(DDPF_BUMPDUDV & m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags) &&
			(0 == (DDPF_BUMPLUMINANCE & m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags)))
		{
			m_pMode->nTextureFormat = i;
			return true;
		}
	}

	return false;
}


bool LightBall::FindLuminanceTextureFormat()
{
    for (int i = 0; i < m_uCommonTextureFormats; i++)
	{
		if ((8 == m_pCommonTextureFormats[i].ddpfPixelFormat.dwBumpBitCount) &&
			(DDPF_LUMINANCE & m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags))
		{
			m_pMode->nTextureFormat = i;
			return true;
		}
	}

	return false;
}
*/

bool LightBall::CheckProjectedTextures(void)
{
//    if (!(m_pAdapter->Devices[m_pMode->nSrcDevice].Desc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PROJECTED))
    if (!(m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED))
        return false;
//    if (!(m_pAdapter->Devices[m_pMode->nRefDevice].Desc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PROJECTED))
//        return false;
	return true;
}

