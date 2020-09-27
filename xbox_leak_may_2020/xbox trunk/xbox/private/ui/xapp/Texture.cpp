#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"
#include "Texture.h"

////////////////////////////////////////////////////////////////////////////

LPDIRECT3DTEXTURE8 CreateTexture(int& nWidth, int& nHeight, D3DFORMAT format)
{
	LPDIRECT3DTEXTURE8 pTexture;

	// REVIEW: usage flags and pool?
	if (FAILED(D3DXCreateTexture(XAppGetD3DDev(), nWidth, nHeight, 1, 0, format, D3DPOOL_MANAGED, &pTexture)))
		return NULL;

	XAppGetTextureSize(pTexture, nWidth, nHeight);

	return pTexture;
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("Texture", CTexture, CNode)

START_NODE_PROPS(CTexture, CNode)
	NODE_PROP(pt_boolean, CTexture, repeatS)
	NODE_PROP(pt_boolean, CTexture, repeatT)
END_NODE_PROPS()

CTexture::CTexture() :
	m_repeatS(true),
	m_repeatT(true)
{
	m_pSurface = NULL;
	m_format = D3DFMT_A8R8G8B8;
}

CTexture::~CTexture()
{
	if (m_pSurface != NULL)
		m_pSurface->Release();
}

LPDIRECT3DTEXTURE8 CTexture::GetTextureSurface()
{
	return m_pSurface;
}

bool CTexture::Create(int nWidth, int nHeight)
{
	m_nImageWidth = nWidth;
	m_nImageHeight = nHeight;
	m_pSurface = ::CreateTexture(nWidth, nHeight, m_format);
	if (m_pSurface == NULL)
	{
		TRACE(_T("\001CreateTexture(%d,%d) failed!\n"), nWidth, nHeight);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("ImageTexture", CImageTexture, CTexture)

START_NODE_PROPS(CImageTexture, CTexture)
	NODE_PROP(pt_string, CImageTexture, url)
	NODE_PROP(pt_boolean, CImageTexture, alpha)
END_NODE_PROPS()

CImageTexture::CImageTexture() :
	m_url(NULL),
	m_alpha(false)
{
	m_bDirty = true;
	m_pSurface = NULL;
}

CImageTexture::~CImageTexture()
{
	delete [] m_url;
}


LPDIRECT3DTEXTURE8 CImageTexture::GetTextureSurface()
{
	if (m_bDirty && m_url != NULL)
	{
		m_bDirty = false;

//		TCHAR szURL [1024];
//		MakeAbsoluteURL(szURL, m_url);

		Load(m_url);
	}

	return CTexture::GetTextureSurface();
}

class CBackgroundLoader
{
public:
	CBackgroundLoader();
	virtual ~CBackgroundLoader();

	bool Fetch(const TCHAR* szURL);

	virtual void OnComplete();

	static CBackgroundLoader* c_pFirstLoader;
	CBackgroundLoader* m_pNextLoader;

	HANDLE m_hFile;
    OVERLAPPED m_overlapped;
	BYTE* m_pbContent;
	int m_cbContent;
	TCHAR* m_szURL;
};

CBackgroundLoader::CBackgroundLoader()
{
	ZeroMemory(&m_overlapped, sizeof (m_overlapped));

	m_szURL = NULL;
	m_hFile = INVALID_HANDLE_VALUE;
	m_pbContent = NULL;
	m_cbContent = 0;

	// Stick this one at the end of the list...
	m_pNextLoader = NULL;
	for (CBackgroundLoader** ppLoader = &c_pFirstLoader; *ppLoader != NULL; ppLoader = &(*ppLoader)->m_pNextLoader)
		;
	*ppLoader = this;
}

CBackgroundLoader::~CBackgroundLoader()
{
	for (CBackgroundLoader** ppLoader = &c_pFirstLoader; *ppLoader != NULL; ppLoader = &(*ppLoader)->m_pNextLoader)
	{
		if (*ppLoader == this)
		{
			*ppLoader = m_pNextLoader;
			break;
		}
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);

	delete [] m_szURL;
	delete [] m_pbContent;
}

CBackgroundLoader* CBackgroundLoader::c_pFirstLoader;

bool CBackgroundLoader::Fetch(const TCHAR* szURL)
{
	ASSERT(m_szURL == NULL);

	TCHAR szBuf [MAX_PATH];
	MakeAbsoluteURL(szBuf, szURL);

	m_szURL = new TCHAR [_tcslen(szBuf) + 1];
	_tcscpy(m_szURL, szBuf);

	DWORD dwFlags = 0;
#ifdef _XBOX
	// Overlapped needs FILE_FLAG_NO_BUFFERING for the hard drive, but can't have it on memory units...
	if ((szURL[0] == 'c' || szURL[0] == 'C' || szURL[0] == 'y' || szURL[0] == 'Y') && szURL[1] == ':')
		dwFlags = FILE_FLAG_NO_BUFFERING;
#endif
	m_hFile = XAppCreateFile(szBuf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN | dwFlags, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001CBackgroundLoader::Fetch(%s) failed %d\n"), szURL, GetLastError());
		return false;
	}

	m_cbContent = GetFileSize(m_hFile, NULL);
#ifdef _XBOX
	// Round up to next 512 bytes (sector) or overlapped will fail on xbox...
	m_cbContent = (m_cbContent + 511) & ~511;
#endif
	m_pbContent = new BYTE [m_cbContent];

	ZeroMemory(&m_overlapped, sizeof (m_overlapped));

	if (!ReadFile(m_hFile, m_pbContent, m_cbContent, NULL, &m_overlapped))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_IO_PENDING)
		{
			TRACE(_T("\001CBackgroundLoader::Fetch ReadFile (%s) failed %d\n"), m_szURL, dwError);
			return false;
		}
	}
	else
	{
//		TRACE(_T("Finished loading %s (right away!)\n"), m_szURL);
		OnComplete();
		delete this;
	}

	return true;
}

void CBackgroundLoader::OnComplete()
{
	// override this to find out when the load is done
}

void BackgroundLoader_Frame()
{
	CBackgroundLoader* pNextLoader;
	for (CBackgroundLoader* pLoader = CBackgroundLoader::c_pFirstLoader; pLoader != NULL; pLoader = pNextLoader)
	{
		pNextLoader = pLoader->m_pNextLoader; // in case pLoader is deleted and removed from the list...

		ASSERT(pLoader->m_hFile != INVALID_HANDLE_VALUE);

		if (HasOverlappedIoCompleted(&pLoader->m_overlapped))
		{
//			TRACE(_T("Finished loading %s\n"), pLoader->m_szURL);
			pLoader->OnComplete();
			delete pLoader;
//return; // one at a time for now
		}
	}
}

struct TXTCACHE
{
	TCHAR* m_szURL;
	LPDIRECT3DTEXTURE8 m_pTexture;
	XTIME m_usage;
	bool m_bLoading;
	XTIME m_timeLoaded;
};


class CBackgroundTexture : public CBackgroundLoader
{
public:
	CBackgroundTexture(TXTCACHE* pTxt, UINT width=0, UINT height=0);
	~CBackgroundTexture();

	virtual void OnComplete();
	TXTCACHE* m_pTxt;

private:
    UINT m_width, m_height;
};

CBackgroundTexture::CBackgroundTexture(TXTCACHE* pTxt, UINT width, UINT height)
{
	ASSERT(pTxt != NULL);
	m_pTxt = pTxt;
    m_width = width;
    m_height = height;
}

CBackgroundTexture::~CBackgroundTexture()
{
	if (m_pTxt != NULL)
		m_pTxt->m_bLoading = false;
}

void CBackgroundTexture::OnComplete()
{
	m_pTxt->m_pTexture = ParseTexture(m_szURL, m_pbContent, m_cbContent, m_width, m_height);
	if((m_pTxt->m_pTexture == NULL) && (m_width == 64))
	{
#ifdef COOL_XDASH
		m_pTxt->m_pTexture = LoadTexture(_T("xboxlogo64.xbx.cool"), 64, 64);
#else
		m_pTxt->m_pTexture = LoadTexture(_T("xboxlogo64.xbx"), 64, 64);
#endif
    }
	else if((m_pTxt->m_pTexture == NULL) && (m_width == 128))
	{
#ifdef COOL_XDASH
		m_pTxt->m_pTexture = LoadTexture(_T("xboxlogo128.xbx.cool"), 128, 128);
#else
		m_pTxt->m_pTexture = LoadTexture(_T("xboxlogo128.xbx"), 128, 128);
#endif
	}
	m_pTxt->m_bLoading = false;
	m_pTxt->m_timeLoaded = XAppGetNow();
}




static TXTCACHE TextureCache [100];
static TXTCACHE* pTxtLock;

bool CleanupTextureCache()
{
	TRACE(_T("Looking for a texture to free...\n"));

	TXTCACHE* pOldTxt = NULL;
	for (int i = 0; i < countof(TextureCache); i += 1)
	{
		TXTCACHE* pTxt = &TextureCache[i];
		if (pTxt->m_szURL == NULL || pTxt == pTxtLock || pTxt->m_bLoading)
			continue;

		if (pOldTxt == NULL || pOldTxt->m_usage > pTxt->m_usage)
			pOldTxt = pTxt;
	}

	if (pOldTxt == NULL)
	{
		TRACE(_T("    none left!\n"));
		return false;
	}

	// Free this one up...

	TRACE(_T("    freeing %s\n"), pOldTxt->m_szURL);

	delete [] pOldTxt->m_szURL;

#ifdef _XBOX
	if (pOldTxt->m_pTexture != NULL)
		pOldTxt->m_pTexture->Release();
#endif

	ZeroMemory(pOldTxt, sizeof (TXTCACHE));

	return true;
}

TXTCACHE* FindTexture(const TCHAR* szURL, bool bAsync, UINT width, UINT height, bool binXIP=false)
{
	TXTCACHE* pFreeOne = NULL;
	TXTCACHE* pOldOne = NULL;
	
	for (int i = 0; i < countof(TextureCache); i += 1)
	{
		if (TextureCache[i].m_szURL != NULL && _tcsicmp(TextureCache[i].m_szURL, szURL) == 0)
		{
//			TRACE(_T("Found %s in Texture cache\n"), szURL);
			TextureCache[i].m_usage = XAppGetNow();
			return &TextureCache[i];
		}

		if (pFreeOne != NULL)
			continue;

		if (TextureCache[i].m_bLoading)
			continue;

		if (TextureCache[i].m_szURL == NULL)
			pFreeOne = &TextureCache[i];
		else if (pOldOne == NULL || pOldOne->m_usage > TextureCache[i].m_usage)
			pOldOne = &TextureCache[i];
	}

	if (pFreeOne == NULL && pOldOne != NULL)
	{
//		TRACE(_T("Unloading %s from Texture cache\n"), pOldOne->m_szURL);
		delete [] pOldOne->m_szURL;

#ifdef _XBOX
		if (pOldOne->m_pTexture != NULL)
			pOldOne->m_pTexture->Release();
#endif

		ZeroMemory(pOldOne, sizeof (TXTCACHE));
		pFreeOne = pOldOne;
	}

	ASSERT(pFreeOne != NULL);

//	TRACE(_T("Loading %s into Texture cache...\n"), szURL);

	ASSERT(pTxtLock == NULL);
	pTxtLock = pFreeOne;

	pFreeOne->m_szURL = new TCHAR [_tcslen(szURL) + 1];
	_tcscpy(pFreeOne->m_szURL, szURL);

	pFreeOne->m_usage = XAppGetNow();

	if (bAsync && !binXIP)
	{
		pFreeOne->m_bLoading = true;
		CBackgroundTexture* pLoader = new CBackgroundTexture(pFreeOne, width, height);
		if (!pLoader->Fetch(szURL))
			delete pLoader;
	}
	else
	{
		pFreeOne->m_pTexture = LoadTexture(szURL, width, height);
		pFreeOne->m_timeLoaded = XAppGetNow();
	}
	
	pTxtLock = NULL;

	return pFreeOne;
}

LPDIRECT3DTEXTURE8 GetTexture(const TCHAR* szURL, XTIME* pTimeLoaded, UINT width, UINT height, bool binXIP = false)
{
	TXTCACHE* pTxt = FindTexture(szURL, true, width, height, binXIP);
	ASSERT(pTxt != NULL);
	if (pTxt->m_pTexture == NULL)
		return NULL;

	if (pTimeLoaded != NULL)
		*pTimeLoaded = pTxt->m_timeLoaded;

	return pTxt->m_pTexture;
}


////////////////////////////////////////////////////////////////////////////

//extern bool CreateTextureFromFile(const TCHAR* szFileName, CTexture* pTexture);

void CImageTexture::Load(const TCHAR* szURL)
{
//	TRACE(_T("0x%08x: Loading texture image: \"%s\"\n"), this, szURL);

//#ifdef _DEBUG
//	int nStartTime = GetTickCount();
//#endif

	if (m_pSurface != NULL)
	{
		m_pSurface->Release();
		m_pSurface = NULL;
	}

	TXTCACHE* pTxt = FindTexture(szURL, false, 0, 0);
	ASSERT(pTxt != NULL);
	if (pTxt->m_pTexture == NULL)
		return;

	m_pSurface = pTxt->m_pTexture;
	m_pSurface->AddRef();

//	if (!CreateTextureFromFile(szURL, this))
//		return;

	XAppGetTextureSize(m_pSurface, m_nImageWidth, m_nImageHeight);

/*
	if (m_alpha)
	{
		D3DLOCKED_RECT lr;
		VERIFYHR(m_pSurface->LockRect(0, &lr, NULL, 0));
		BYTE* pDibPels = (BYTE*)lr.pBits;
		INT nDibPitch = lr.Pitch;
		for (int y = 0; y < m_nImageHeight; y += 1)
		{
			BYTE* pbScan = pDibPels + y * nDibPitch;
			for (int x = 0; x < m_nImageWidth; x += 1)
			{
				BYTE b = pbScan[0];
				BYTE g = pbScan[1];
				BYTE r = pbScan[2];
				BYTE a = pbScan[3];

				a = (r + g + b) / 3;
				a = a * 7 / 8;

				pbScan[0] = b;
				pbScan[1] = g;
				pbScan[2] = r;
				pbScan[3] = a;

				pbScan += 4;
			}
		}
		m_pSurface->UnlockRect(0);
	}
*/

//#ifdef _DEBUG
//	TRACE(_T("%s took %d mS to load\n"), szURL, GetTickCount() - nStartTime);
//#endif
}

bool CImageTexture::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_url))
		m_bDirty = true;

	return true;
}
