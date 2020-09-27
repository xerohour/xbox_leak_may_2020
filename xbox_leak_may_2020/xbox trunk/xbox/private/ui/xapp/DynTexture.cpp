#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Texture.h"
#include "surfx.h"

class CPalette : public CNode
{
	DECLARE_NODE(CPalette, CNode)
public:
	CPalette();
	~CPalette();

	int m_type;
	float m_changePeriod;
	float m_changePeriodRandomness;
	float m_timeToBlend;

	void Advance(float nSeconds);

	const DWORD* GetPalette() { return m_palette; }

#ifdef _XBOX
	D3DPalette* m_pPalette;
#endif

protected:
	void Update();
	void RenderDynamicTexture(CSurfx* pSurfx);

	DWORD m_palette [256];
	int m_nBlendPalette;
	float m_nBlendPaletteAmount;
	XTIME m_nextPaletteChangeTime;
	XTIME m_startBlendTime;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Palette", CPalette, CNode)

START_NODE_PROPS(CPalette, CNode)
	NODE_PROP(pt_integer, CPalette, type)
	NODE_PROP(pt_number, CPalette, changePeriod)
	NODE_PROP(pt_number, CPalette, changePeriodRandomness)
END_NODE_PROPS()


CPalette::CPalette() :
	m_changePeriod(0.0f),
	m_changePeriodRandomness(0.0f),
	m_timeToBlend(2.0f),
	m_type(0)
{
	m_nextPaletteChangeTime = 0.0f;
	m_nBlendPalette = -1;

#ifdef _XBOX
	m_pPalette = NULL;
#endif
}

CPalette::~CPalette()
{
#ifdef _XBOX
	if (m_pPalette != NULL)
		m_pPalette->Release();
#endif
}

void CPalette::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	XTIME now = XAppGetNow();

	if (m_changePeriod > 0.0f && now > m_nextPaletteChangeTime)
	{
		m_nextPaletteChangeTime = now + m_changePeriod + rnd(m_changePeriodRandomness);
		m_nBlendPalette = m_type;
		m_type += 1;
		m_nBlendPaletteAmount = 0.0f;
		m_startBlendTime = now;

//		TRACE(_T("Palette: change to %d\n"), m_type);
	}

	Update();

	if (m_nBlendPalette != -1)
	{
		m_nBlendPaletteAmount = (float) (now - m_startBlendTime) / m_timeToBlend;
		if (m_nBlendPaletteAmount >= 1.0f)
			m_nBlendPalette = -1;
	}
}

extern void HSV2RGB(float H, float S, float V, DWORD& outRGB);
extern void GetHSV(int nPalette, float i, float t, float& H, float& S, float& V);
extern int g_nPalette;

void CPalette::Update()
{
	XTIME now = XAppGetNow();
	float H, S, V, inc = 1.0f / 255.0f;
	float mIntensity = 0.0f;

	for (int i = 0; i < 256; i++, mIntensity += inc)
	{
		GetHSV(m_type, mIntensity, (float) now, H, S, V);
		m_type = g_nPalette;

		if (m_nBlendPalette != -1)
		{
			float H2, S2, V2;

			// This is the previous palette...
			GetHSV(m_nBlendPalette, mIntensity, (float) now, H2, S2, V2);

			H = m_nBlendPaletteAmount * H + (1.0f - m_nBlendPaletteAmount) * H2;
			S = m_nBlendPaletteAmount * S + (1.0f - m_nBlendPaletteAmount) * S2;
			V = m_nBlendPaletteAmount * V + (1.0f - m_nBlendPaletteAmount) * V2;
		}

		HSV2RGB(H, S, V, m_palette[i]);
	}

#ifdef _XBOX
	if (m_pPalette == NULL)
		VERIFYHR(XAppGetD3DDev()->CreatePalette(D3DPALETTE_256, &m_pPalette));

	D3DCOLOR* rgColor;
	VERIFYHR(m_pPalette->Lock(&rgColor, D3DLOCK_NOOVERWRITE));
	for (i = 0; i < 256; i += 1)
	{
		BYTE r = (BYTE)(m_palette[i] >> 16);
		BYTE g = (BYTE)(m_palette[i] >> 8);
		BYTE b = (BYTE)m_palette[i];
		rgColor[i] = D3DCOLOR_RGBA(r, g, b, 255);
	}

	VERIFYHR(m_pPalette->Unlock());
#endif
}

void CPalette::RenderDynamicTexture(CSurfx* pSurfx)
{
	for (int y = 0; y < pSurfx->m_nHeight; y += 1)
		FillMemory(pSurfx->m_pels + y * pSurfx->m_nWidth, pSurfx->m_nWidth, (BYTE)(y * 256 / pSurfx->m_nHeight));
}

////////////////////////////////////////////////////////////////////////////

class CDynamicTexture : public CTexture
{
	DECLARE_NODE(CDynamicTexture, CTexture)
public:
	CDynamicTexture();
	~CDynamicTexture();

	CNodeArray m_children;
	int m_size;
	bool m_erase;
	float m_fps;
	CNode* m_palette;

	virtual bool Create(int nWidth, int nHeight);

	void Advance(float nSeconds);
	LPDIRECT3DTEXTURE8 GetTextureSurface();

protected:
	void Update();

	CSurfx* m_pSurfx;
	XTIME m_lastUpdateTime;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("DynamicTexture", CDynamicTexture, CTexture)

START_NODE_PROPS(CDynamicTexture, CTexture)
	NODE_PROP(pt_children, CDynamicTexture, children)
	NODE_PROP(pt_integer, CDynamicTexture, size)
	NODE_PROP(pt_boolean, CDynamicTexture, erase)
	NODE_PROP(pt_number, CDynamicTexture, fps)
	NODE_PROP(pt_node, CDynamicTexture, palette)
END_NODE_PROPS()


CDynamicTexture::CDynamicTexture() :
	m_size(256),
	m_erase(true),
	m_fps(15.0f),
	m_palette(NULL)
{
	m_lastUpdateTime = 0.0f;
	m_pSurfx = NULL;

#ifdef _XBOX
	m_format = D3DFMT_P8;
#endif
}

CDynamicTexture::~CDynamicTexture()
{
	if (m_palette != NULL)
		m_palette->Release();

	delete m_pSurfx;
}

bool CDynamicTexture::Create(int nWidth, int nHeight)
{
    D3DLOCKED_RECT d3dlr;

	m_nImageWidth = 512;
	m_nImageHeight = 512;

	if (FAILED(D3DXCreateTexture(XAppGetD3DDev(), m_nImageWidth, m_nImageHeight, 1, 0, m_format, D3DPOOL_MANAGED, &m_pSurface)))
	{
		TRACE(_T("\001CreateTexture(%d,%d) failed!\n"), m_nImageWidth, m_nImageHeight);
		return false;
	}

    m_pSurface->LockRect(0, &d3dlr, NULL, 0);
    memset(d3dlr.pBits, 0, m_nImageWidth * m_nImageHeight);
    m_pSurface->UnlockRect(0);

	XAppGetTextureSize(m_pSurface, m_nImageWidth, m_nImageHeight);

	return true;
}

void CDynamicTexture::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_palette != NULL)
		m_palette->Advance(nSeconds);

	int nChildCount = m_children.GetLength();
	for (int i = 0; i < nChildCount; i += 1)
	{
		CNode* pChildNode = m_children.GetNode(i);
		pChildNode->Advance(nSeconds);
	}
}

LPDIRECT3DTEXTURE8 CDynamicTexture::GetTextureSurface()
{
	if (XAppGetNow() > m_lastUpdateTime + (1.0f / m_fps))
	{
		m_lastUpdateTime = XAppGetNow();
		Update();
	}

#ifdef _XBOX
	if (m_palette != NULL && m_palette->IsKindOf(NODE_CLASS(CPalette)))
	{
		CPalette* pPalette = (CPalette*)m_palette;
		VERIFYHR(XAppGetD3DDev()->SetPalette(0, pPalette->m_pPalette));
	}
#endif

	return m_pSurface;
}

void CDynamicTexture::Update()
{
	if (m_pSurface == NULL && m_size > 0)
	{
		if (!Create(m_size, m_size))
			return;
	}

	if (m_pSurfx == NULL && m_size > 0)
	{
		m_pSurfx = new CSurfx(m_size, m_size);
	}

	if (m_pSurface == NULL || m_pSurfx == NULL || XAppGetNow() < m_lastUpdateTime)
		return;

	if (m_erase)
		ZeroMemory(m_pSurfx->m_pels, m_pSurfx->m_nWidth * m_pSurfx->m_nHeight);

	int nChildCount = m_children.GetLength();
	for (int i = 0; i < nChildCount; i += 1)
	{
		CNode* pChildNode = m_children.GetNode(i);
		pChildNode->RenderDynamicTexture(m_pSurfx);
	}

	// BLOCK: Copy the internal palettized pixmap to our surface...
	{
		const DWORD* rgpe = NULL;

		if (m_palette != NULL)
			rgpe = m_palette->GetPalette();

		DWORD rgpeDefault [256];
		if (rgpe == NULL)
		{
			rgpe = rgpeDefault;
			MakePalette(rgpeDefault);
		}

#ifdef _XBOX // Swizzle the image into the texture

#if 0

        RECT rect = {0, 0, m_size, m_size};
        POINT point = {0, 0};
		D3DLOCKED_RECT lr;
		VERIFYHR(m_pSurface->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));
		XGSwizzleRect(m_pSurfx->m_pels, m_size, &rect, lr.pBits, m_nImageWidth, m_nImageHeight, &point, 1);

		VERIFYHR(m_pSurface->UnlockRect(0));

#else

        Swizzler swz(m_nImageWidth, m_nImageHeight, 1);
        BYTE* pSrc = (BYTE*)m_pSurfx->m_pels;
        BYTE* pDst;
		D3DLOCKED_RECT lr;
		VERIFYHR(m_pSurface->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));
        pDst = (BYTE*)lr.pBits;
        for (int i = 0; i < m_size; i++, swz.IncV()) {
            for (int j = 0; j < m_size; j++, swz.IncU()) {
                pDst[swz.Get2D()] = pSrc[j];
            }
            pSrc += m_size;
            swz.AddU(swz.SwizzleU(m_nImageWidth - m_size));
        }
		VERIFYHR(m_pSurface->UnlockRect(0));

#endif

#else

		D3DLOCKED_RECT lr;
		VERIFYHR(m_pSurface->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));
		DWORD* pDest = (DWORD*)lr.pBits;

		BYTE* pbSrc = (BYTE*)m_pSurfx->m_pels;
		int nPels = m_size * m_size;
		for (int i = 0; i < nPels; i += 1)
		{
			BYTE b = *pbSrc++;
			*pDest++ = 0xff000000 | rgpe[b];
		}
/*
		for (int y = 0; y < m_size; y += 1)
		{
			BYTE* pbDest = (BYTE*)ssd.lpSurface + y * ssd.lPitch;
			BYTE* pbSrc = (BYTE*)m_pSurfx->m_pels + y * m_pSurfx->m_nWidth;

			for (int x = 0; x < m_size; x += 1)
			{
				PALETTEENTRY& pe = rgpe[*pbSrc];
				*pbDest++ = pe.peBlue;
				*pbDest++ = pe.peGreen;
				*pbDest++ = pe.peRed;
				*pbDest++ = 255;
				pbSrc += 1;
			}
		}
*/

		VERIFYHR(m_pSurface->UnlockRect(0));
#endif
	}
}

////////////////////////////////////////////////////////////////////////////


class CImageFader : public CNode
{
	DECLARE_NODE(CImageFader, CNode)
public:
	CImageFader();
	~CImageFader();

	int m_type;
	float m_changePeriod;
	float m_changePeriodRandomness;

	void RenderDynamicTexture(CSurfx* pSurfx);
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

protected:
	DeltaField* m_pDeltaField;
	DeltaField* m_pDeltaField2;
	XTIME m_nextChangeTime;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("ImageFader", CImageFader, CNode)

START_NODE_PROPS(CImageFader, CNode)
	NODE_PROP(pt_integer, CImageFader, type)
	NODE_PROP(pt_number, CImageFader, changePeriod)
	NODE_PROP(pt_number, CImageFader, changePeriodRandomness)
END_NODE_PROPS()

CImageFader::CImageFader() :
	m_type(0),
	m_changePeriod(0.0f),
	m_changePeriodRandomness(0.0f)
{
	m_nextChangeTime = 0.0f;
	m_pDeltaField = NULL;
	m_pDeltaField2 = NULL;
}

CImageFader::~CImageFader()
{
	delete m_pDeltaField;
	delete m_pDeltaField2;
}

void CImageFader::RenderDynamicTexture(CSurfx* pSurfx)
{
	if (m_pDeltaField == NULL)
	{
		m_pDeltaField = new DeltaField;
		m_pDeltaField->m_nStyle = m_type;
		m_pDeltaField->Assign();
		m_pDeltaField->SetSize(pSurfx->m_nWidth, pSurfx->m_nHeight);

		m_nextChangeTime = XAppGetNow() + m_changePeriod + rnd(m_changePeriodRandomness);
	}
	else if (m_changePeriod > 0.0f && XAppGetNow() >= m_nextChangeTime)
	{
		m_nextChangeTime = XAppGetNow() + m_changePeriod;

		m_type += 1;

		delete m_pDeltaField;
		m_pDeltaField = m_pDeltaField2;
		m_pDeltaField2 = NULL;
	}

	if (m_pDeltaField2 == NULL)
	{
		m_pDeltaField2 = new DeltaField;
		m_pDeltaField2->m_nStyle = m_type + 1;
		m_pDeltaField2->Assign();
		m_pDeltaField2->SetSize(pSurfx->m_nWidth, pSurfx->m_nHeight);
	}

	if (m_pDeltaField->IsCalculating())
	{
		for (int i = 0; i < 10; i += 1)
			m_pDeltaField->CalcSome();
		m_type = m_pDeltaField->m_nStyle; // in case this got fixed up...
		return;
	}

	if (m_pDeltaField2->IsCalculating())
	{
		for (int i = 0; i < 5; i += 1)
		{
			if (m_pDeltaField2 != NULL)
				m_pDeltaField2->CalcSome();
		}
	}

	pSurfx->Fade(m_pDeltaField->GetField());
}

bool CImageFader::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_type))
	{
		delete m_pDeltaField;
		m_pDeltaField = m_pDeltaField2;
		m_pDeltaField2 = NULL;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////
