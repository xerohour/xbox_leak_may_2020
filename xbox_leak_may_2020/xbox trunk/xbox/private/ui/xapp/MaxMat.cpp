#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Shape.h"

extern class CMeshNode* g_pRenderMeshNode;
extern void SetFalloffShaderValues(const D3DXCOLOR& sideColor, const D3DXCOLOR& frontColor);
extern DWORD GetEffectShader(int nEffect, DWORD fvf);
extern LPDIRECT3DTEXTURE8 CreateTexture(int& nWidth, int& nHeight, D3DFORMAT format);
extern LPDIRECT3DTEXTURE8 GetTexture(const TCHAR* szURL, XTIME* pTimeLoaded = NULL, UINT width=0, UINT height=0, bool binXIP=false);
extern void SetReflectShaderFrameValues();

BOOL g_bEdgeAntialiasOverride; //REVIEW: Hackorama!
const TCHAR* g_szCurTitleImage; // REVIEW: Hackorama!
const TCHAR* g_szSelTitleImage; // REVIEW: Hackorama!
bool g_bActiveKey = false; // REVIEW: Hackorama!
extern float g_nEffectAlpha;
extern D3DRECT g_scissorRect;
extern D3DRECT g_scissorRectx2;
XTIME g_pulseStartTime;
static class CMaxMaterial* g_pLastPulseMaxMat = NULL;

class CMaxMaterial : public CMaterial
{
	DECLARE_NODE(CMaxMaterial, CMaterial)
public:
	CMaxMaterial();
	~CMaxMaterial();

	TCHAR* m_name;
	XTIME m_param;

	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	void Render();

	class CMatInfo* m_pMatInfo;

	DECLARE_NODE_PROPS()
};

////////////////////////////////////////////////////////////////////////////

void GenerateRadialAlphaMask(BYTE* pbPels, int nPitch, int nWidth, int nHeight, float nFactor, float nMax, float nScale = 1.0f)
{
    LPDWORD pdwPels = (LPDWORD)pbPels;
    int nRadialWidth = (int)((float)nWidth * nScale);
    int nRadialHeight = (int)((float)nHeight * nScale);
    int x, y;

	for (y = 0; y < nRadialHeight; y += 1)
	{
		for (x = 0; x < nRadialWidth; x += 1)
		{
			float cx = (float)(x - nRadialWidth / 2) / (nRadialWidth / 2);
			float cy = (float)(y - nRadialHeight / 2) / (nRadialHeight / 2);
			float d = sqrtf(cx * cx + cy * cy);

			if (d < 0.0f)
				d = 0.0f;
			else if (d > 1.0f)
				d = 1.0f;

			float a = 1.0f - d;

			a = a * nFactor;

			if (a < 0.0f)
				a = 0.0f;
			else if (a > nMax)
				a = nMax;

            pdwPels[x] = ((BYTE)(a * 255.0f)) << 24;
		}

        for (; x < nWidth; x += 1)
        {
            pdwPels[x] = 0;
        }

        pdwPels += (nPitch >> 2);
	}

	for (; y < nHeight; y += 1)
	{
        memset(pdwPels, 0, nWidth << 2);
        pdwPels += (nPitch >> 2);
    }
}

LPDIRECT3DTEXTURE8 GetRadialAlphaMask()
{
	static LPDIRECT3DTEXTURE8 pTexture = NULL;

	if (pTexture != NULL)
		return pTexture;

	int nWidth = 256;
	int nHeight = 256;
	pTexture = CreateTexture(nWidth, nHeight, D3DFMT_A8R8G8B8);
	ASSERT(pTexture != NULL);

	D3DLOCKED_RECT lr;
	VERIFYHR(pTexture->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));

#ifdef _XBOX
	BYTE* pbBuf = new BYTE [4 * nWidth * nHeight];
	GenerateRadialAlphaMask(pbBuf, nWidth * 4, nWidth, nHeight, 1.0f, 1.0f);
	XGSwizzleRect(pbBuf, 0, NULL, lr.pBits, nWidth, nHeight, NULL, 4);
	delete [] pbBuf;
#else
	GenerateRadialAlphaMask((BYTE*)lr.pBits, lr.Pitch, nWidth, nHeight, 1.0f, 1.0f);
#endif

	VERIFYHR(pTexture->UnlockRect(0));

	return pTexture;
}

LPDIRECT3DTEXTURE8 GetRadialAVAlphaMask()
{
	static LPDIRECT3DTEXTURE8 pTexture = NULL;

	if (pTexture != NULL)
		return pTexture;

	int nWidth = 256;
	int nHeight = 256;
	pTexture = CreateTexture(nWidth, nHeight, D3DFMT_A8R8G8B8);
	ASSERT(pTexture != NULL);

	D3DLOCKED_RECT lr;
	VERIFYHR(pTexture->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));

#ifdef _XBOX
	BYTE* pbBuf = new BYTE [4 * nWidth * nHeight];
	GenerateRadialAlphaMask(pbBuf, nWidth * 4, nWidth, nHeight, 1.0f, 1.0f, 0.6875f);
	XGSwizzleRect(pbBuf, 0, NULL, lr.pBits, nWidth, nHeight, NULL, 4);
	delete [] pbBuf;
#else
	GenerateRadialAlphaMask((BYTE*)lr.pBits, lr.Pitch, nWidth, nHeight, 1.0f, 1.0f);
#endif

	VERIFYHR(pTexture->UnlockRect(0));

	return pTexture;
}

LPDIRECT3DTEXTURE8 GetRadialEdgeAlphaMask()
{
	static LPDIRECT3DTEXTURE8 pTexture = NULL;

	if (pTexture != NULL)
		return pTexture;

	int nWidth = 256;
	int nHeight = 256;
	pTexture = CreateTexture(nWidth, nHeight, D3DFMT_A8R8G8B8);
	ASSERT(pTexture != NULL);

	D3DLOCKED_RECT lr;
	VERIFYHR(pTexture->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));

#ifdef _XBOX
	BYTE* pbBuf = new BYTE [4 * nWidth * nHeight];
	GenerateRadialAlphaMask(pbBuf, nWidth * 4, nWidth, nHeight, 10.0f, 0.8f);
	XGSwizzleRect(pbBuf, 0, NULL, lr.pBits, nWidth, nHeight, NULL, 4);
	delete [] pbBuf;
#else
	GenerateRadialAlphaMask((BYTE*)lr.pBits, lr.Pitch, nWidth, nHeight, 10.0f, 0.8f);
#endif

	VERIFYHR(pTexture->UnlockRect(0));

	return pTexture;
}

LPDIRECT3DTEXTURE8 GetRadialEdgeAlphaMainMask()
{
	static LPDIRECT3DTEXTURE8 pTexture = NULL;

	if (pTexture != NULL)
		return pTexture;

	int nWidth = 256;
	int nHeight = 256;
	pTexture = CreateTexture(nWidth, nHeight, D3DFMT_A8R8G8B8);
	ASSERT(pTexture != NULL);

	D3DLOCKED_RECT lr;
	VERIFYHR(pTexture->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));

#ifdef _XBOX
	BYTE* pbBuf = new BYTE [4 * nWidth * nHeight];
	GenerateRadialAlphaMask(pbBuf, nWidth * 4, nWidth, nHeight, 3.0f, 1.0f);
	XGSwizzleRect(pbBuf, 0, NULL, lr.pBits, nWidth, nHeight, NULL, 4);
	delete [] pbBuf;
#else
	GenerateRadialAlphaMask((BYTE*)lr.pBits, lr.Pitch, nWidth, nHeight, 3.0f, 1.0f);
#endif

	VERIFYHR(pTexture->UnlockRect(0));

	return pTexture;
}

////////////////////////////////////////////////////////////////////////////

#define MATINFO_STANDARD_MATERIAL		0x00000001
#define MATINFO_CULL_NONE				0x00000002
#define MATINFO_RADIAL_ALPHA			0x00000004
#define MATINFO_RADIAL_EDGE_ALPHA		0x00000008
#define MATINFO_RADIAL_EDGE_MAIN_ALPHA	0x00000010
#define MATINFO_RADIAL_AV_ALPHA			0x00000020

class CMatInfo
{
public:
	CMatInfo(const TCHAR* szName, DWORD dwFlags = 0);

	virtual bool Setup(CMaxMaterial* pMaxMat);

	const TCHAR* m_szName;
	DWORD m_dwFlags;
};

class CSolidMatInfo : public CMatInfo
{
public:
	CSolidMatInfo(const TCHAR* szName, BYTE r, BYTE g, BYTE b, BYTE a, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);

	BYTE m_r, m_g, m_b, m_a;
};

class CFalloffMatInfo : public CMatInfo
{
public:
	CFalloffMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);

	D3DCOLOR m_colorSide;
	D3DCOLOR m_colorFront;
	int m_nShaderEffect;
	TCHAR m_matName[MAX_PATH];
};

class CFalloffTexInfo : public CMatInfo
{
public:
	CFalloffTexInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);

	D3DCOLOR m_colorSide;
	D3DCOLOR m_colorFront;
	int m_nShaderEffect;
};

class CAnisoMatInfo : public CMatInfo
{
public:
	CAnisoMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);

	D3DCOLOR m_colorSide;
	D3DCOLOR m_colorFront;
	int m_nShaderEffect;
};

class CIconMatInfo : public CMatInfo
{
public:
	CIconMatInfo(const TCHAR* szName, UINT width, UINT height, bool bFadeIn = false, bool binXIP = false, bool bSelImg = false);

	bool Setup(CMaxMaterial* pMaxMat);

	bool m_bFadeIn;
	bool m_binXIP;
	bool m_bSelImg;

private:
    UINT m_width, m_height;
};

class CKeyMatInfo : public CFalloffMatInfo
{
public:
	CKeyMatInfo(const TCHAR* szName, bool bBright, bool bText);

	bool Setup(CMaxMaterial* pMaxMat);

	bool m_bBright;
	bool m_bText;
};

class CEggGlowPulseMatInfo : public CFalloffMatInfo
{
public:
	CEggGlowPulseMatInfo(const TCHAR* szName);

	bool Setup(CMaxMaterial* pMaxMat);
};

class CEggGlowFadeMatInfo : public CFalloffMatInfo
{
public:
	CEggGlowFadeMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);
};

class CBackingMatInfo : public CMatInfo
{
public:
	CBackingMatInfo(const TCHAR* szName, BYTE r, BYTE g, BYTE b, BYTE a, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);

	BYTE m_r, m_g, m_b, m_a;
};

class CModulateTextureMatInfo : public CMatInfo
{
public:
	CModulateTextureMatInfo(const TCHAR* szName, BYTE r, BYTE g, BYTE b, BYTE a, DWORD dwFlags/*=0*/);

	bool Setup(CMaxMaterial* pMaxMat);

	BYTE m_r, m_g, m_b, m_a;
};

class CReflectMatInfo : public CMatInfo
{
public:
	CReflectMatInfo(const TCHAR* szName);

	bool Setup(CMaxMaterial* pMaxMat);
	
	int m_nShaderEffect;
};

class CMaskTextureMatInfo : public CMatInfo
{
public:
	CMaskTextureMatInfo(const TCHAR* szName, DWORD dwFlags);

	bool Setup(CMaxMaterial* pMaxMat);
};

class CFreeSpaceMatInfo : public CMatInfo
{
public:
	CFreeSpaceMatInfo(const TCHAR* szName);

	bool Setup(CMaxMaterial* pMaxMat);
};

class CInnerWallMatInfo : public CMatInfo
{
public:
	CInnerWallMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags = 0);

	bool Setup(CMaxMaterial* pMaxMat);

	D3DCOLOR m_colorSide;
	D3DCOLOR m_colorFront;
	int m_nShaderEffect;
};


CMatInfo* g_rgMatInfo [120];
int g_nMatInfoCount = 0;


////////////////////////////////////////////////////////////////////////////


CInnerWallMatInfo::CInnerWallMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_nShaderEffect = 3;
	m_colorSide = colorSide;
	m_colorFront = colorFront;
}
bool CInnerWallMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_pRenderMeshNode == NULL)
		return false;

	XAppSetVertexShader(GetEffectShader(m_nShaderEffect, g_pRenderMeshNode->GetFVF()));
	SetFalloffShaderValues(m_colorSide, m_colorFront);

	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	XAppSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

	XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	XAppSetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return CMatInfo::Setup(pMaxMat);
}


CFreeSpaceMatInfo::CFreeSpaceMatInfo(const TCHAR* szName) :
	CMatInfo(szName)
{
}

bool CFreeSpaceMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	// This is the free/used space guage texture for memory units...
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
//	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
//	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255, 255, 255, (BYTE)(128 * g_nEffectAlpha)));
	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255, 255, 255, (BYTE)(70 * g_nEffectAlpha)));
	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	mat._32 = (float) ((1.0f - pMaxMat->m_param) * 2.0f - 1.0f) * 0.3f + 0.1f;
	XAppSetTransform(D3DTS_TEXTURE0, &mat);
	XAppSetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	return true;
}

CMaskTextureMatInfo::CMaskTextureMatInfo(const TCHAR* szName, DWORD dwFlags) :
	CMatInfo(szName, dwFlags)
{
}

bool CMaskTextureMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if ((m_dwFlags & MATINFO_RADIAL_ALPHA) != 0)
		XAppSetTexture(1, GetRadialAlphaMask());
	else if ((m_dwFlags & MATINFO_RADIAL_EDGE_ALPHA) != 0)
		XAppSetTexture(1, GetRadialEdgeAlphaMask());
	else if ((m_dwFlags & MATINFO_RADIAL_EDGE_MAIN_ALPHA) != 0)
		XAppSetTexture(1, GetRadialEdgeAlphaMainMask());
    else if ((m_dwFlags & MATINFO_RADIAL_AV_ALPHA) != 0)
        XAppSetTexture(1, GetRadialAVAlphaMask());

	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	XAppSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	XAppSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	XAppSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	XAppSetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT);
	XAppSetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	XAppSetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, (BYTE)(255.0f * g_nEffectAlpha)));

	return CMatInfo::Setup(pMaxMat);
}

CModulateTextureMatInfo::CModulateTextureMatInfo(const TCHAR* szName, BYTE r, BYTE g, BYTE b, BYTE a, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

bool CModulateTextureMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
//	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	if (g_pLastPulseMaxMat != pMaxMat && g_nEffectAlpha == 1.0f)
	{
//		TRACE(_T("Resetting pulse (hilite) 0x%08x\n"), pMaxMat);
		g_pLastPulseMaxMat = pMaxMat;
		g_pulseStartTime = XAppGetNow();
	}

	float t = (float) (XAppGetNow() - g_pulseStartTime) / 2.0f;
	float a = 1.0f - fabsf(sinf(t * D3DX_PI));

	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(m_r, m_g, m_b, (BYTE)(m_a * (a * 0.25f + 0.75f) * g_nEffectAlpha)));

	// Edge Aliasing mode enable
	XAppSetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
	XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
//	XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN);
//	XAppGetD3DDev()->SetScissors(1, FALSE, &g_scissorRectx2);

	return CMatInfo::Setup(pMaxMat);
}

CReflectMatInfo::CReflectMatInfo(const TCHAR* szName) :
	CMatInfo(szName, 0)
{
	m_nShaderEffect = 4;
}

bool CReflectMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_pRenderMeshNode == NULL)
		return false;

	XAppSetVertexShader(GetEffectShader(m_nShaderEffect, g_pRenderMeshNode->GetFVF()));
	SetFalloffShaderValues(D3DCOLOR_RGBA(229, 229, 229, 0), D3DCOLOR_RGBA(229, 229, 229, 255));

	SetReflectShaderFrameValues();

	XAppSetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, (BYTE)(255.0f * g_nEffectAlpha)));

	return CMatInfo::Setup(pMaxMat);
}

CBackingMatInfo::CBackingMatInfo(const TCHAR* szName, BYTE r, BYTE g, BYTE b, BYTE a, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

bool CBackingMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if ((m_dwFlags & MATINFO_RADIAL_ALPHA) != 0)
		XAppSetTexture(0, GetRadialAlphaMask());
	else if ((m_dwFlags & MATINFO_RADIAL_EDGE_ALPHA) != 0)
		XAppSetTexture(0, GetRadialEdgeAlphaMask());
	else if ((m_dwFlags & MATINFO_RADIAL_EDGE_MAIN_ALPHA) != 0)
		XAppSetTexture(0, GetRadialEdgeAlphaMainMask());
    else if ((m_dwFlags & MATINFO_RADIAL_AV_ALPHA) != 0)
        XAppSetTexture(0, GetRadialAVAlphaMask());

	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(m_r, m_g, m_b, (BYTE)(m_a * g_nEffectAlpha)));
	//XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(m_r, m_g, m_b, (BYTE)()));

	return CMatInfo::Setup(pMaxMat);
}


CEggGlowPulseMatInfo::CEggGlowPulseMatInfo(const TCHAR* szName) :
	CFalloffMatInfo(szName, 0, D3DCOLOR_RGBA(0, 0, 0, 0), D3DCOLOR_RGBA(0, 0, 0, 0))
{
}

bool CEggGlowPulseMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_pLastPulseMaxMat != pMaxMat)
	{
		//TRACE(_T("Resetting pulse (egg)\n"));
		g_pLastPulseMaxMat = pMaxMat;
		//g_pulseStartTime = XAppGetNow();
		g_pulseStartTime = 2.0f;
	}

	float t = (float) (XAppGetNow() - g_pulseStartTime) / 2.0f;
	float a = 1.0f - fabsf(sinf(t * D3DX_PI));

	//m_colorSide = D3DCOLOR_RGBA(252, 255, 0, 0);
	//m_colorFront = D3DCOLOR_RGBA(255, 255, 128, (int)(64.0f + 64.0f * a));

#ifdef COOL_XDASH
	m_colorSide = D3DCOLOR_RGBA(0, 146, 208, 0);
	m_colorFront = D3DCOLOR_RGBA(128, 236, 255, (int)(64.0f + 128.0f * a));
#else
	m_colorSide = D3DCOLOR_RGBA(178, 208, 0, 0);
	m_colorFront = D3DCOLOR_RGBA(255, 255, 128, (int)(64.0f + 128.0f * a));
#endif
	
	//m_colorSide = D3DCOLOR_RGBA(178, 208, 0, (int)(64.0f + 191.0f * a));
	//m_colorFront = D3DCOLOR_RGBA(178, 208, 0, (int)(64.0f + 191.0f * a));

	return CFalloffMatInfo::Setup(pMaxMat);
}

CEggGlowFadeMatInfo::CEggGlowFadeMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags) :
	CFalloffMatInfo(szName, dwFlags, colorSide, colorFront)
{
}

bool CEggGlowFadeMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	float a = 0.0f;

	if (pMaxMat->m_param == 1.0f)
	{
		pMaxMat->m_param = XAppGetNow();
		ASSERT(pMaxMat->m_param > 1.0f);
	}

	if (pMaxMat->m_param > 0.0f)
	{
		float t = (float) (XAppGetNow() - pMaxMat->m_param) / 1.25f;
		if (t >= 1.0f)
		{
			t = 1.0f;
			pMaxMat->m_param = 0.0f;
		}

		a = 1.0f - t;
	}

	m_colorFront = D3DCOLOR_RGBA(254, 255, 188, (int)(255.0f * a));

	return CFalloffMatInfo::Setup(pMaxMat);
}

CKeyMatInfo::CKeyMatInfo(const TCHAR* szName, bool bBright, bool bText) :
	CFalloffMatInfo(szName, D3DCOLOR_RGBA(0, 0, 0, 0), D3DCOLOR_RGBA(0, 0, 0, 0), 0)
{
	m_bBright = bBright;
	m_bText = bText;
}

bool CKeyMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_bActiveKey)
	{
		if (g_pLastPulseMaxMat != pMaxMat && g_nEffectAlpha > 0.0f)
		{
			//TRACE(_T("Resetting pulse (key)\n"));
			g_pLastPulseMaxMat = pMaxMat;
			g_pulseStartTime = XAppGetNow();
		}

		float t = (float) (XAppGetNow() - g_pulseStartTime) / 2.0f;
		float a = fabsf(sinf(t * D3DX_PI));

		if (m_bBright)
		{
#ifdef COOL_XDASH
			m_colorSide = D3DCOLOR_RGBA(0, 146, 208, 255);//D3DCOLOR_RGBA(0, 188, 0, 0);
			m_colorFront = D3DCOLOR_RGBA(0, 146, 208, 190);//D3DCOLOR_RGBA(0, 188, 0, (int)(192.0f + 64.0f * a));
#else
			m_colorSide = D3DCOLOR_RGBA(178, 208, 0, 255);//D3DCOLOR_RGBA(0, 188, 0, 0);
			m_colorFront = D3DCOLOR_RGBA(178, 208, 0, 190);//D3DCOLOR_RGBA(0, 188, 0, (int)(192.0f + 64.0f * a));
#endif
		}
		else
		{
#ifdef COOL_XDASH
			m_colorSide = D3DCOLOR_RGBA(0, 146, 208, 255);//D3DCOLOR_RGBA(0, 188, 0, 0);
			m_colorFront = D3DCOLOR_RGBA(0, 146, 208, 190);//D3DCOLOR_RGBA(0, 188, 0, (int)(128.0f + 64.0f * a));
#else
			m_colorSide = D3DCOLOR_RGBA(178, 208, 0, 255);//D3DCOLOR_RGBA(0, 188, 0, 0);
			m_colorFront = D3DCOLOR_RGBA(178, 208, 0, 190);//D3DCOLOR_RGBA(0, 188, 0, (int)(128.0f + 64.0f * a));
#endif
		}

		if(m_bText)
		{
			XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
			XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, (BYTE)(192 * g_nEffectAlpha)));
		}
	}
	else
	{
		if (m_bBright)
		{
#ifdef COOL_XDASH
			m_colorSide = D3DCOLOR_RGBA(107, 220, 255, 255);
			m_colorFront = D3DCOLOR_RGBA(13, 0, 255, 34);
#else
			m_colorSide = D3DCOLOR_RGBA(243, 255, 107, 255);
			m_colorFront = D3DCOLOR_RGBA(25, 225, 0, 34);
#endif
		}
		else
		{
#ifdef COOL_XDASH
			m_colorSide = D3DCOLOR_RGBA(107, 220, 255, 222);//D3DCOLOR_RGBA(243, 255, 107, 255);
			m_colorFront = D3DCOLOR_RGBA(8, 0, 192, 15);//D3DCOLOR_RGBA(30, 255, 0, 32);
#else
			m_colorSide = D3DCOLOR_RGBA(243, 255, 107, 222);//D3DCOLOR_RGBA(243, 255, 107, 255);
			m_colorFront = D3DCOLOR_RGBA(20, 192, 0, 15);//D3DCOLOR_RGBA(30, 255, 0, 32);
#endif
		}

		if(m_bText)
		{
			XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
			XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
#ifdef COOL_XDASH
			XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(94, 167, 250, (BYTE)(178 * g_nEffectAlpha)));
#else
			XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(190, 250, 94, (BYTE)(178 * g_nEffectAlpha)));
#endif
		}
	}

	// Edge Aliasing mode enable
	XAppSetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
	XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
//	XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN);
//	XAppGetD3DDev()->SetScissors(1, FALSE, &g_scissorRectx2);

	return CFalloffMatInfo::Setup(pMaxMat);
}

CIconMatInfo::CIconMatInfo(const TCHAR* szName, UINT width, UINT height, bool bFadeIn/*=false*/, bool binXIP /*=false*/, bool bSelImg) :
	CMatInfo(szName)
{
	m_bFadeIn = bFadeIn;
	m_binXIP = binXIP;
	m_bSelImg = bSelImg;
    m_width = width;
    m_height = height;
}

bool CIconMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	float alpha = 1.0f;
    bool bInXip = m_binXIP;

	if ((g_szCurTitleImage != NULL) && (!m_bSelImg))
	{
#ifdef COOL_XDASH
		if((_tcsicmp(g_szCurTitleImage, _T("xboxlogo64.xbx.cool")) == 0) ||
		  (_tcsicmp(g_szCurTitleImage, _T("xboxlogo128.xbx.cool")) == 0))
#else
        if((_tcsicmp(g_szCurTitleImage, _T("xboxlogo64.xbx")) == 0) ||
          (_tcsicmp(g_szCurTitleImage, _T("xboxlogo128.xbx")) == 0))
#endif
		{
			bInXip = true;
		}

		XTIME timeLoaded = 0.0f;
		pTexture = GetTexture(g_szCurTitleImage, &timeLoaded, m_width, m_height, bInXip);
		if (pTexture != NULL && m_bFadeIn)
		{
			alpha = (float) (XAppGetNow() - timeLoaded) / 0.25f;
			if (alpha > 1.0f)
				alpha = 1.0f;
		}
	}
	else if ((g_szSelTitleImage != NULL) && (m_bSelImg))  //for the selected image orb
	{
#ifdef COOL_XDASH
		if((_tcsicmp(g_szSelTitleImage, _T("xboxlogo64.xbx.cool")) == 0) ||
		  (_tcsicmp(g_szSelTitleImage, _T("xboxlogo128.xbx.cool")) == 0))
#else
		if((_tcsicmp(g_szSelTitleImage, _T("xboxlogo64.xbx")) == 0) ||
		  (_tcsicmp(g_szSelTitleImage, _T("xboxlogo128.xbx")) == 0))
#endif
		{
			bInXip = true;
		}

		XTIME timeLoaded = 0.0f;
		pTexture = GetTexture(g_szSelTitleImage, &timeLoaded, m_width, m_height, bInXip);
		if (pTexture != NULL && m_bFadeIn)
		{
			alpha = (float) (XAppGetNow() - timeLoaded) / 0.25f;
			if (alpha > 1.0f)
				alpha = 1.0f;
		}
	}

	if (pTexture != NULL)
	{
		XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		XAppSetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		XAppSetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		XAppSetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		XAppSetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		XAppSetTexture(0, pTexture);
		XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, (alpha * g_nEffectAlpha)));
	}
	else
	{
		XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
		XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.0f));
	}

	XAppSetTexture(1, GetRadialEdgeAlphaMask());
	XAppSetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
	XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	XAppSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	XAppSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	XAppSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	return true;
}




CMatInfo::CMatInfo(const TCHAR* szName, DWORD dwFlags/*=0*/)
{
	ASSERT(g_nMatInfoCount < countof (g_rgMatInfo));
	g_rgMatInfo[g_nMatInfoCount] = this;
	g_nMatInfoCount += 1;

	m_szName = szName;
	m_dwFlags = dwFlags;
}

CSolidMatInfo::CSolidMatInfo(const TCHAR* szName, BYTE r, BYTE g, BYTE b, BYTE a, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

bool CMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if ((m_dwFlags & MATINFO_STANDARD_MATERIAL) != 0)
		return false;

	if ((m_dwFlags & MATINFO_CULL_NONE) != 0)
		XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

bool CSolidMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(m_r, m_g, m_b, (BYTE)(m_a * g_nEffectAlpha)));

	// Edge Aliasing mode enable
	XAppSetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
	XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
//	XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN);
//	XAppGetD3DDev()->SetScissors(1, FALSE, &g_scissorRectx2);

	return CMatInfo::Setup(pMaxMat);
}

CFalloffMatInfo::CFalloffMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_nShaderEffect = 1;
	m_colorSide = colorSide;
	m_colorFront = colorFront;
	_tcscpy(m_matName, szName);
}

bool CFalloffMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_pRenderMeshNode == NULL)
		return false;

	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetVertexShader(GetEffectShader(m_nShaderEffect, g_pRenderMeshNode->GetFVF()));
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// Edge Aliasing mode enable
	XAppSetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
	XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
//	XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN);
//	XAppGetD3DDev()->SetScissors(1, FALSE, &g_scissorRectx2);

	SetFalloffShaderValues(m_colorSide, m_colorFront);

	return CMatInfo::Setup(pMaxMat);
}

CFalloffTexInfo::CFalloffTexInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_nShaderEffect = 3;
	m_colorSide = colorSide;
	m_colorFront = colorFront;
}


bool CFalloffTexInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_pRenderMeshNode == NULL)
		return false;

	XAppSetVertexShader(GetEffectShader(m_nShaderEffect, g_pRenderMeshNode->GetFVF()));
	SetFalloffShaderValues(m_colorSide, m_colorFront);

	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	XAppSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

	XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	return CMatInfo::Setup(pMaxMat);
}

CAnisoMatInfo::CAnisoMatInfo(const TCHAR* szName, D3DCOLOR colorSide, D3DCOLOR colorFront, DWORD dwFlags/*=0*/) :
	CMatInfo(szName, dwFlags)
{
	m_nShaderEffect = 2;
	m_colorSide = colorSide;
	m_colorFront = colorFront;
}

bool CAnisoMatInfo::Setup(CMaxMaterial* pMaxMat)
{
	if (g_pRenderMeshNode == NULL)
		return false;

	XAppSetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	XAppSetVertexShader(GetEffectShader(m_nShaderEffect, g_pRenderMeshNode->GetFVF()));
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	SetFalloffShaderValues(m_colorSide, m_colorFront);

	return CMatInfo::Setup(pMaxMat);
}

static int __cdecl SortMatInfoCompare(const void *elem1, const void *elem2)
{
	const CMatInfo* pMatInfo1 = *(const CMatInfo**)elem1;
	const CMatInfo* pMatInfo2 = *(const CMatInfo**)elem2;
	return _tcscmp(pMatInfo1->m_szName, pMatInfo2->m_szName);
}

static int __cdecl SearchMatInfoCompare(const void *elem1, const void *elem2)
{
	const TCHAR* szName = (const TCHAR*)elem1;
	const CMatInfo* pMatInfo = *(const CMatInfo**)elem2;
	return _tcscmp(szName, pMatInfo->m_szName);
}

CMatInfo* LookupMatInfo(const TCHAR* szName)
{
	CMatInfo** ppMatInfo = (CMatInfo**)bsearch(szName, g_rgMatInfo, g_nMatInfoCount, sizeof (CMatInfo*), SearchMatInfoCompare);
	if (ppMatInfo == NULL)
		return NULL;
	return *ppMatInfo;
}

void Material_Init()
{

#ifdef COOL_XDASH
	new CInnerWallMatInfo(_T("InnerWall_01"), D3DCOLOR_RGBA(107, 180, 255, 255), D3DCOLOR_RGBA(9, 0, 192, 20));
	new CFalloffMatInfo(_T("InnerWall_02"), D3DCOLOR_RGBA(107, 180, 255, 255), D3DCOLOR_RGBA(9, 0, 192, 20), MATINFO_CULL_NONE);
#else
	new CInnerWallMatInfo(_T("InnerWall_01"), D3DCOLOR_RGBA(243, 255, 107, 255), D3DCOLOR_RGBA(40, 212, 20, 20));
	new CFalloffMatInfo(_T("InnerWall_02"), D3DCOLOR_RGBA(243, 255, 107, 255), D3DCOLOR_RGBA(20, 192, 0, 20), MATINFO_CULL_NONE);
#endif

	new CMatInfo(_T("MetaInfo_Text"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("NamePanel_Text"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("GameNameText_01"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("GameNameText_02"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("GameNameText_03"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("GameIcon_01"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("GameIcon_03"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("Material #132"), MATINFO_STANDARD_MATERIAL);
	new CMatInfo(_T("u2 info"), MATINFO_STANDARD_MATERIAL);

	new CSolidMatInfo(_T("Material #1334"), 11, 32, 0, 192);
	new CSolidMatInfo(_T("XBOXgreendark"), 6, 33, 0, 255);
#ifdef COOL_XDASH
	new CSolidMatInfo(_T("XBOXgreen"), 25, 114, 201, 255);
	new CSolidMatInfo(_T("XBoxGreen2"), 25, 114, 201, 255);
#else
	new CSolidMatInfo(_T("XBOXgreen"), 140, 201, 25, 255);
	new CSolidMatInfo(_T("XBoxGreen2"), 139, 200, 24, 255);
#endif
	new CSolidMatInfo(_T("GameHilite33"), 221, 208, 120, 178);
	new CSolidMatInfo(_T("Nothing"), 128, 128, 128, 255);

#ifdef COOL_XDASH
	new CSolidMatInfo(_T("NavType"), 94, 190, 250, 178);
#else
	new CSolidMatInfo(_T("NavType"), 190, 250, 94, 178);
#endif
	new CSolidMatInfo(_T("RedType"), 200, 30, 30, 255);

#ifdef COOL_XDASH
	new CSolidMatInfo(_T("XBoxGreen"), 25, 114, 201, 255);
#else
	new CSolidMatInfo(_T("XBoxGreen"), 139, 200, 24, 255);
#endif
	new CSolidMatInfo(_T("Type"), 100, 200, 25, 255);
	new CSolidMatInfo(_T("Typesdsafsda"), 255, 255, 255, 179);
	new CSolidMatInfo(_T("Material #133"), 0, 0, 0, 192);

#ifdef COOL_XDASH
	new CSolidMatInfo(_T("Material #1335"), 0, 52, 162, 200);
#else
	new CSolidMatInfo(_T("Material #1335"), 76, 162, 0, 200);
#endif

	new CSolidMatInfo(_T("Material #133511"), 41, 87, 0, 255);
	new CSolidMatInfo(_T("Material #1336"), 0, 0, 0, 128);

#ifdef COOL_XDASH
	new CSolidMatInfo(_T("HilightedType"), 4, 0, 44, 255);
#else
	new CSolidMatInfo(_T("HilightedType"), 3, 44, 0, 255);
#endif

	new CSolidMatInfo(_T("XBoxGreenq"), 139, 200, 24, 255);
	new CSolidMatInfo(_T("Black80"), 0, 0, 0, 204);

#ifdef COOL_XDASH
	new CSolidMatInfo(_T("CellEgg/Partsw"), 9, 0, 192, 20, MATINFO_CULL_NONE);
	new CSolidMatInfo(_T("CellEgg/Partsz"), 107, 180, 255, 255, MATINFO_CULL_NONE);
#else
	new CSolidMatInfo(_T("CellEgg/Partsw"), 77, 224, 57, 166, MATINFO_CULL_NONE);
	new CSolidMatInfo(_T("CellEgg/Partsz"), 170, 170, 170, 255, MATINFO_CULL_NONE);
#endif

	new CSolidMatInfo(_T("Material #108"), 160, 252, 0, 255, MATINFO_CULL_NONE);
	new CSolidMatInfo(_T("ItemsType"), 182, 245, 96, 255, MATINFO_CULL_NONE);
	new CSolidMatInfo(_T("GameHiliteMemory"), 178, 208, 0, 255);

//	new CBackingMatInfo(_T("highlight"), 254, 255, 188, 80, MATINFO_CULL_NONE | MATINFO_RADIAL_ALPHA);

	new CSolidMatInfo(_T("red"), 255, 0, 0, 255);

	// m_nFalloff == 1 colors
	{
#ifdef COOL_XDASH
		const D3DCOLOR side = D3DCOLOR_RGBA(107, 180, 255, 192);
		const D3DCOLOR front = D3DCOLOR_RGBA(9, 0, 192, 0);
#else
		const D3DCOLOR side = D3DCOLOR_RGBA(243, 255, 107, 192);
		const D3DCOLOR front = D3DCOLOR_RGBA(20, 192, 0, 0);
#endif

		new CFalloffMatInfo(_T("FlatSurfaces"), side, front);

		new CFalloffMatInfo(_T("FlatSurfacesSelected"), side, D3DCOLOR_RGBA(30, 255, 0, 128));
#ifdef COOL_XDASH
		new CFalloffMatInfo(_T("FlatSurfacesMemory"), side, D3DCOLOR_RGBA(0, 6, 160, 128));
        new CFalloffMatInfo(_T("DarkSurfaces"), D3DCOLOR_RGBA(85, 133, 205, 90), front);
		new CFalloffMatInfo(_T("DarkSurfaces2"), D3DCOLOR_RGBA(85, 133, 205, 90), D3DCOLOR_RGBA(7, 0, 100, 0));
#else
		new CFalloffMatInfo(_T("FlatSurfacesMemory"), side, D3DCOLOR_RGBA(30, 160, 0, 128));
        new CFalloffMatInfo(_T("DarkSurfaces"), D3DCOLOR_RGBA(203, 205, 85, 90), front);
		new CFalloffMatInfo(_T("DarkSurfaces2"), D3DCOLOR_RGBA(203, 205, 85, 90), D3DCOLOR_RGBA(10, 100, 0, 0));
#endif
		new CFalloffMatInfo(_T("FlatSurfaces2sided"), side, front, MATINFO_CULL_NONE);
		new CFalloffMatInfo(_T("DetailLegSkin_Inner"), side, front);
		new CFalloffMatInfo(_T("Screen"), side, front, MATINFO_CULL_NONE);
		new CFalloffMatInfo(_T("Spout"), side, front);
		new CFalloffMatInfo(_T("NavType34"), side, front);
		new CFalloffMatInfo(_T("MetaFlatSurfaces"), side, front);
		new CFalloffMatInfo(_T("SC_SavedGame_Row01"), side, front);
		new CFalloffMatInfo(_T("HK_SavedGame_Row01"), side, front);
		new CFalloffMatInfo(_T("SavedEgg_Selected"), side, front);
		new CFalloffMatInfo(_T("FlatUnselected"), side, front);
		new CFalloffMatInfo(_T("Mem_InnerWall_Outer"), side, front);
		new CFalloffMatInfo(_T("SavedGameEgg"), side, front);
		new CFalloffMatInfo(_T("GameMenuEgg"), side, front);
		new CFalloffMatInfo(_T("Shell"), side, front);
		new CFalloffMatInfo(_T("Material #133sdsfdsf"), side, front);
		new CFalloffMatInfo(_T("IconParts"), side, front, MATINFO_CULL_NONE);
		new CFalloffMatInfo(_T("IconParts1"), side, front);
		new CFalloffMatInfo(_T("MU1Pod_HL"), side, front);
		new CFalloffMatInfo(_T("MU1Pod"), side, front);
		new CFalloffMatInfo(_T("GamePodb"), side, front, MATINFO_CULL_NONE);
		new CFalloffMatInfo(_T("GamePod"), side, front);
		new CFalloffMatInfo(_T("JwlSrfc01/InfoPnls"), side, front);

		new CFalloffMatInfo(_T("MenuCell"), side, front);

//		new CFalloffMatInfo(_T("InnerWall_02"), D3DCOLOR_RGBA(243, 255, 107, 255/*100*/), front, MATINFO_CULL_NONE);
//		new CFalloffMatInfo(_T("InnerWall_02"), D3DCOLOR_RGBA(243, 255, 107, 100), D3DCOLOR_RGBA(20, 192, 0, 0), MATINFO_CULL_NONE);
//		new CFalloffMatInfo(_T("InnerWall_02"), side, front, MATINFO_CULL_NONE);
		

		new CFalloffTexInfo(_T("Wireframe"), D3DCOLOR_RGBA(125, 198, 34, 100), D3DCOLOR_RGBA(125, 198, 34, 0));
	}

	// m_nFalloff == 2 colors
	{
#ifdef COOL_XDASH
		const D3DCOLOR side = D3DCOLOR_RGBA(107, 180, 255, 192);
		const D3DCOLOR front = D3DCOLOR_RGBA(9, 0, 192, 0);
#else
        const D3DCOLOR side = D3DCOLOR_RGBA(242, 250, 153, 215);
        const D3DCOLOR front = D3DCOLOR_RGBA(7, 104, 0, 37);
#endif

		new CFalloffMatInfo(_T("Tubes"), side, front, MATINFO_CULL_NONE);
		new CFalloffMatInfo(_T("JewelSurface02/PodMesh"), side, front);
		new CFalloffMatInfo(_T("TubesFade"), side, front, MATINFO_CULL_NONE);
		new CFalloffMatInfo(_T("TubesQ"), side, front);
		new CFalloffMatInfo(_T("EmptyMU"), side, front);
		new CFalloffMatInfo(_T("Tube"), side, front);
		new CFalloffMatInfo(_T("MemoryHeader"), D3DCOLOR_RGBA(60, 198, 67, 122), front, MATINFO_CULL_NONE);
	}

	// m_nFalloff == 3 colors
	{
		const D3DCOLOR side = D3DCOLOR_RGBA(252, 255, 0, 0);
		const D3DCOLOR front = D3DCOLOR_RGBA(254, 255, 188, 228);
//		const D3DCOLOR side = D3DCOLOR_RGBA(252, 0, 255, 0);
//		const D3DCOLOR front = D3DCOLOR_RGBA(254, 188, 255, 228);

		new CEggGlowFadeMatInfo(_T("EggGlow"), side, front);
		new CFalloffMatInfo(_T("ButtonGlow"), side, front);
	}

	// m_nFalloff == 4 colors
	{
#ifdef COOL_XDASH
		const D3DCOLOR side = D3DCOLOR_RGBA(107, 180, 255, 192);
		const D3DCOLOR front = D3DCOLOR_RGBA(9, 0, 192, 0);
#else
        const D3DCOLOR side = D3DCOLOR_RGBA(191, 255, 107, 51);
        const D3DCOLOR front = D3DCOLOR_RGBA(0, 255, 18, 0);
#endif

		new CFalloffMatInfo(_T("gradient"), side, front);
		new CFalloffMatInfo(_T("CellWallStructure"), side, front);
		new CFalloffMatInfo(_T("FlatSrfc/PodParts"), side, front);
		new CFalloffMatInfo(_T("Cell_Light"), side, front);
		new CFalloffMatInfo(_T("Cell_Light/LegSkin"), side, front);
	}

#ifdef COOL_XDASH
	new CFalloffMatInfo(_T("CellEgg/Parts"), D3DCOLOR_RGBA(107, 180, 255, 192), D3DCOLOR_RGBA(9, 0, 192, 0));
	new CFalloffMatInfo(_T("GameEgg"), D3DCOLOR_RGBA(107, 180, 255, 192), D3DCOLOR_RGBA(9, 0, 192, 0));
	new CFalloffMatInfo(_T("FlatSurfaces2sided3"), D3DCOLOR_RGBA(107, 180, 255, 192), D3DCOLOR_RGBA(9, 0, 192, 0), MATINFO_CULL_NONE);
	new CFalloffMatInfo(_T("console_hilite"), D3DCOLOR_RGBA(255, 173, 107, 255), D3DCOLOR_RGBA(246, 255, 0, 0));
#else
	new CFalloffMatInfo(_T("CellEgg/Parts"), D3DCOLOR_RGBA(243, 255, 107, 178), D3DCOLOR_RGBA(30, 255, 0, 0));
	new CFalloffMatInfo(_T("GameEgg"), D3DCOLOR_RGBA(243, 255, 107, 178), D3DCOLOR_RGBA(30, 255, 0, 0));
	new CFalloffMatInfo(_T("FlatSurfaces2sided3"), D3DCOLOR_RGBA(253, 30, 0, 255), D3DCOLOR_RGBA(242, 28, 0, 0), MATINFO_CULL_NONE);
	new CFalloffMatInfo(_T("console_hilite"), D3DCOLOR_RGBA(255, 173, 107, 255), D3DCOLOR_RGBA(246, 255, 0, 0));
#endif


	new CIconMatInfo(_T("TitleIcon"), 128, 128);
	new CIconMatInfo(_T("TitleSoundtrackIcon"), 128, 128, false, true);
	new CIconMatInfo(_T("SavedGameIcon"), 64, 64, true);
	new CIconMatInfo(_T("SoundtrackIcon"), 64, 64, true, true);
	new CIconMatInfo(_T("SelectedIcon"), 128, 128, false, false, true);
	new CKeyMatInfo(_T("Key"), false, false);
	new CKeyMatInfo(_T("BrightKey"), true, false);
	new CKeyMatInfo(_T("KeyText"), false, true);
	new CEggGlowPulseMatInfo(_T("EggGlowPulse"));
	new CAnisoMatInfo(_T("Metal_Chrome"), D3DCOLOR_RGBA(229, 229, 229, 0), D3DCOLOR_RGBA(229, 229, 229, 255));
	new CAnisoMatInfo(_T("Tvbox"), D3DCOLOR_RGBA(229, 229, 229, 0), D3DCOLOR_RGBA(229, 229, 229, 255));
	new CAnisoMatInfo(_T("AudioCD"), D3DCOLOR_RGBA(255, 255, 255, 40), D3DCOLOR_RGBA(255, 255, 255, 180));

#ifdef COOL_XDASH
	new CBackingMatInfo(_T("PanelBacking_01"), 0, 4, 20, 255);
	new CBackingMatInfo(_T("PanelBacking_02"), 0, 0, 0, 255);
	new CBackingMatInfo(_T("PanelBacking_03"), 0, 4, 20, 240);
	new CBackingMatInfo(_T("PanelBacking_04"), 7, 14, 46, 255);
	new CBackingMatInfo(_T("NameBacking"), 0, 1, 8, 255);
	new CBackingMatInfo(_T("ModeBacking"), 0, 1, 8, 255);
	new CBackingMatInfo(_T("SavedGameBacking"), 11, 10, 18, 255);
	new CBackingMatInfo(_T("MemManMetaBacking"), 0, 4, 25, 255);
	new CBackingMatInfo(_T("DarkenBacking"), 0, 4, 50, 255, MATINFO_CULL_NONE | MATINFO_RADIAL_EDGE_ALPHA);
#else
	new CBackingMatInfo(_T("PanelBacking_01"), 4, 20, 0, 255);
	new CBackingMatInfo(_T("PanelBacking_02"), 0, 0, 0, 255);
	new CBackingMatInfo(_T("PanelBacking_03"), 4, 20, 0, 240);
	new CBackingMatInfo(_T("PanelBacking_04"), 14, 46, 7, 255);
	new CBackingMatInfo(_T("NameBacking"), 1, 8, 0, 255);
	new CBackingMatInfo(_T("ModeBacking"), 1, 8, 0, 255);
	new CBackingMatInfo(_T("SavedGameBacking"), 11, 18, 10, 255);
	new CBackingMatInfo(_T("MemManMetaBacking"), 4, 25, 0, 255);
	new CBackingMatInfo(_T("DarkenBacking"), 4, 50, 0, 255, MATINFO_CULL_NONE | MATINFO_RADIAL_EDGE_ALPHA);
#endif

	new CBackingMatInfo(_T("TextBacking"), 0, 0, 0, 255, MATINFO_CULL_NONE | MATINFO_RADIAL_ALPHA);
	new CBackingMatInfo(_T("MetaBacking"), 4, 50, 0, 255, MATINFO_CULL_NONE | MATINFO_RADIAL_ALPHA);
	new CBackingMatInfo(_T("DarkenBackingDark"), 1, 8, 0, 255, MATINFO_CULL_NONE | MATINFO_RADIAL_ALPHA);

	new CModulateTextureMatInfo(_T("GameHilite"), 255, 255, 255, 255/*192*/, MATINFO_CULL_NONE);
	new CMaskTextureMatInfo(_T("equalizer"), MATINFO_RADIAL_AV_ALPHA);
	new CMaskTextureMatInfo(_T("MainMenuOrb"), MATINFO_RADIAL_EDGE_MAIN_ALPHA);  //new texture added for main menu

	new CReflectMatInfo(_T("ReflectSurface"));
	
	new CModulateTextureMatInfo(_T("PanelBacking"), 4, 20, 0, 255, MATINFO_CULL_NONE);

	new CFreeSpaceMatInfo(_T("Material #10822"));

	qsort(g_rgMatInfo, g_nMatInfoCount, sizeof (CMatInfo*), SortMatInfoCompare);

#ifdef _DEBUG
	// Check for duplicates!
	for (int i = 1; i < g_nMatInfoCount; i += 1)
		ASSERT(_tcscmp(g_rgMatInfo[i - 1]->m_szName, g_rgMatInfo[i]->m_szName) != 0);
#endif
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("MaxMaterial", CMaxMaterial, CMaterial)

START_NODE_PROPS(CMaxMaterial, CMaterial)
	NODE_PROP(pt_string, CMaxMaterial, name)
	NODE_PROP(pt_number, CMaxMaterial, param)
END_NODE_PROPS()

CMaxMaterial::CMaxMaterial() :
	m_name(NULL),
	m_param(0.0f)
{
	m_pMatInfo = NULL;
}

CMaxMaterial::~CMaxMaterial()
{
	delete [] m_name;
}

bool CMaxMaterial::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_name))
		m_pMatInfo = NULL;
    else if ((int)pprd->pbOffset == offsetof(m_param))
    {
        m_param = (XTIME)(*(float*)pvValue);
        return false;
    }

	return true;
}

void CMaxMaterial::Render()
{
	if (m_pMatInfo == NULL && m_name != NULL)
		m_pMatInfo = LookupMatInfo(m_name);

	if (m_pMatInfo == NULL)
	{
		TRACE(_T("\001Referencing undefined MaxMaterial '%s'\n"), m_name);
//		ASSERT(FALSE);
		return;
	}

	//Disable Edge Antialiasing
	XAppSetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
	XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
//	XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN);
//	XAppGetD3DDev()->SetScissors(1, FALSE, &g_scissorRect);

	if (_tcscmp(m_name, TEXT("Tubes")) == 0 ||
		_tcscmp(m_name, TEXT("TubesFade")) == 0 ||
		_tcscmp(m_name, TEXT("TubesQ")) == 0 ||
		_tcscmp(m_name, TEXT("Tube")) == 0)
	{
		g_bEdgeAntialiasOverride = TRUE;
	}
	else {
		g_bEdgeAntialiasOverride = FALSE;
	}

	m_pMatInfo->Setup(this);
}
