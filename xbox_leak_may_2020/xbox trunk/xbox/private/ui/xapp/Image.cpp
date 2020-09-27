#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Texture.h"
#include "ActiveFile.h"
#include "xip.h"
#include "Image.h"


/*
TODO:
	Load images from arbitrary URL
	Perform load/decode in background thread
	Deal with incremental display (as an option)
	Deal with animated GIF's
	Handle the same image formats for both BITMAP and SURFACE cases
	Optimize case where CreateTexture actually returns the right size/format
*/

bool DecodeRAW(const TCHAR* szFileName, CTexture* pTexture);

////////////////////////////////////////////////////////////////////////////

class CImage
{
public:
	CImage();
	~CImage();

	int m_nWidth;
	int m_nHeight;
	BYTE* m_pels;
	int m_nPitch;
};


CImage::CImage()
{
	m_pels = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
}

CImage::~CImage()
{
}

////////////////////////////////////////////////////////////////////////////

LPDIRECT3DTEXTURE8 ParseTexture(const TCHAR* szURL, const BYTE* pbContent, int cbContent, UINT width, UINT height)
{
#ifdef _XBOX
	const TCHAR* pch = _tcsrchr(szURL, '.');
	if (pch != NULL)
	{
		pch += 1;
		if (_tcsicmp(pch, _T("xt")) == 0)
		{
			IDirect3DTexture8* pTexture = (IDirect3DTexture8*)XAppD3D_AllocNoncontiguousMemory(sizeof (D3DBaseTexture));
			CopyMemory(pTexture, pbContent, sizeof (IDirect3DTexture8));

			int cbData = cbContent - sizeof (IDirect3DTexture8);

			BYTE* pbData = (BYTE*)XAppD3D_AllocContiguousMemory(cbData, D3DTEXTURE_ALIGNMENT);
			CopyMemory(pbData, pbContent + sizeof (IDirect3DTexture8), cbData);
			D3D_CopyContiguousMemoryToVideo(pbData);

			pTexture->Data = NULL;
			pTexture->Register(pbData);
			pTexture->Common |= D3DCOMMON_D3DCREATED;

			return pTexture;
		}
		else if (_tcsicmp(pch, _T("xbx")) == 0)
		{
//			TRACE(_T("Attempting to load XBX format image: %s...\n"), szURL);
			const XPR_HEADER* pxprh = (const XPR_HEADER*)pbContent;
			if (pxprh->dwMagic == XPR_MAGIC_VALUE)
			{
				int cbHeaders = pxprh->dwHeaderSize - sizeof (XPR_HEADER);
				int cbData = pxprh->dwTotalSize - pxprh->dwHeaderSize;

				// BLOCK: do some validation before we do any more
				{
					if (cbHeaders < sizeof (IDirect3DTexture8))
					{
						TRACE(_T("Invalid XBX image file (wrong header size; is %d should be %d)!\n"), cbHeaders, sizeof (IDirect3DTexture8));
						return NULL;
					}

					D3DResource* pResource = (D3DResource*)(pbContent + sizeof (XPR_HEADER));
					if ((pResource->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_TEXTURE)
					{
						TRACE(_T("Invalid XBX image file (not a texture)!\n"));
						return NULL;
					}

					if (cbData <= 0 || (UINT)cbContent < pxprh->dwHeaderSize + cbData)
					{
						TRACE(_T("Invalid XBX image file! (wrong data size)\n"));
						return NULL;
					}

                    if (width != 0 && height != 0)
                    {
                        DWORD dwU, dwV;
                        DWORD dwInfo = *((DWORD*)pbContent + 6);
                        const DWORD exptbl[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,0,0,0,0,0,0};

                        dwU = exptbl[(dwInfo & D3DFORMAT_USIZE_MASK) >> D3DFORMAT_USIZE_SHIFT];
                        dwV = exptbl[(dwInfo & D3DFORMAT_VSIZE_MASK) >> D3DFORMAT_VSIZE_SHIFT];

                        if (dwU != width || dwV != height)
                        {
    						TRACE(_T("\001Invalid XBX image size! (not %dx%d)\n"), width, height);
    						return NULL;
                        }
                    }
                }

				IDirect3DTexture8* pTexture = (IDirect3DTexture8*)XAppD3D_AllocNoncontiguousMemory(sizeof (D3DBaseTexture));
				if (pTexture == NULL)
				{
					TRACE(_T("Not enough memory to load XBX image file!\n"));
					return NULL;
				}

				CopyMemory(pTexture, pbContent + sizeof (XPR_HEADER), sizeof (IDirect3DTexture8));

				BYTE* pbData = (BYTE*)XAppD3D_AllocContiguousMemory(cbData, D3DTEXTURE_ALIGNMENT);
				if (pbData == NULL)
				{
					// TODO: Leaking pTexture!
					TRACE(_T("Not enough memory to load XBX image file!\n"));
					return NULL;
				}

				CopyMemory(pbData, pbContent + pxprh->dwHeaderSize, cbData);
				D3D_CopyContiguousMemoryToVideo(pbData);

				pTexture->Data = NULL;
				pTexture->Register(pbData);
				pTexture->Common |= D3DCOMMON_D3DCREATED;

				return pTexture;
			}
		}
	}

	TRACE(_T("\002Texture '%s' needs to be converted to an Xbox friendly format!\n"), szURL);
#endif

	for (;;)
	{
		LPDIRECT3DTEXTURE8 lpTexture = NULL;
		HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(XAppGetD3DDev(), pbContent, cbContent, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &lpTexture);
		if (hr == D3D_OK)
			return lpTexture;

		if (hr != E_OUTOFMEMORY || NewFailed(cbContent) == 0)
			break;
	}

	return NULL;
}

LPDIRECT3DTEXTURE8 LoadTexture(const TCHAR* szURL, UINT width, UINT height)
{
//#ifdef _DEBUG
//	TRACE(_T("Loading texture image: \"%s\"\n"), szURL);
//	int nStartTime = GetTickCount();
//#endif
	// Look for an XBX file
	{
		TCHAR szBuf [MAX_PATH];
		MakeAbsoluteURL(szBuf, szURL);

		TCHAR* pch = _tcsrchr(szBuf, '.');
		if (pch != NULL)
		{
#ifdef COOL_XDASH
			_tcscpy(pch + 1, _T("xbx.cool"));
#else
			_tcscpy(pch + 1, _T("xbx"));
#endif

			LPDIRECT3DTEXTURE8 lpTexture = (LPDIRECT3DTEXTURE8)FindObjectInXIP(szBuf, szURL, XIP_TYPE_TEXTURE);
			if (lpTexture != NULL)
				return lpTexture;

			TRACE(_T("\002Cannot find %s in a XIP!\n"), szBuf);
		}
	}

	/*
	CActiveFile file;
	if (!file.Fetch(szURL, true))
		return NULL;

	LPDIRECT3DTEXTURE8 lpTexture = ParseTexture(szURL, file.GetContent(), file.GetContentLength(), width, height);
	*/

	ALERT(_T("Unable to load XBX file (%s) for scene!"), szURL);

	//#ifdef _DEBUG
	//	TRACE(_T("%s took %d mS to load\n"), szURL, GetTickCount() - nStartTime);
	//#endif

	return NULL;
}

/*
bool CreateTextureFromFile(const TCHAR* szFileName, CTexture* pTexture)
{
	const TCHAR* pch = _tcsrchr(szFileName, '.');

	if (pch == NULL)
		return false;

	if (_tcsicmp(pch, _T(".raw")) == 0)
		return DecodeRAW(szFileName, pTexture);

	CActiveFile file;

	if (!file.Fetch(szFileName, true))
		return false;

	if (D3DXCreateTextureFromFileInMemoryEx(XAppGetD3DDev(), file.GetContent(), file.GetContentLength(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture->m_pSurface) == D3D_OK)
		return true;

	TRACE(_T("\001Could not load texture: \"%s\"\n"), szFileName);

	return false;
}
*/

////////////////////////////////////////////////////////////////////////////

HIMAGE MyLoadImage(const TCHAR* szURL);

struct IMGCACHE
{
	TCHAR* m_szURL;
	HIMAGE m_hImage;
	SIZE m_size;
	XTIME m_usage;
};

static IMGCACHE imageCache [100];
static IMGCACHE* pImgLock;

void CleanupImageCache()
{
	for (int i = 0; i < countof(imageCache); i += 1)
	{
		IMGCACHE* pImg = &imageCache[i];
		if (pImg == pImgLock)
			continue;

		delete [] pImg->m_szURL;

#ifdef _USEGDI
		if (pImg->m_hImage != NULL)
			DeleteObject(pImg->m_hImage);
#endif
#ifdef _XBOX
		if (pImg->m_hImage != NULL)
			pImg->m_hImage->Release();
#endif

		ZeroMemory(pImg, sizeof (IMGCACHE));
	}
}

IMGCACHE* FindImage(const TCHAR* szURL)
{
	IMGCACHE* pFreeOne = NULL;
	IMGCACHE* pOldOne = NULL;
	for (int i = 0; i < countof(imageCache); i += 1)
	{
		if (imageCache[i].m_szURL != NULL && _tcsicmp(imageCache[i].m_szURL, szURL) == 0)
		{
//			TRACE(_T("Found %s in image cache\n"), szURL);
			imageCache[i].m_usage = XAppGetNow();
			return &imageCache[i];
		}

		if (pFreeOne != NULL)
			continue;

		if (imageCache[i].m_szURL == NULL)
			pFreeOne = &imageCache[i];
		else if (pOldOne == NULL || pOldOne->m_usage > imageCache[i].m_usage)
			pOldOne = &imageCache[i];
	}

	if (pFreeOne == NULL && pOldOne != NULL)
	{
		TRACE(_T("Unloading %s from image cache\n"), pOldOne->m_szURL);
		delete [] pOldOne->m_szURL;

#ifdef _USEGDI
		if (pOldOne->m_hImage != NULL)
			DeleteObject(pOldOne->m_hImage);
#endif
#ifdef _XBOX
		if (pOldOne->m_hImage != NULL)
			pOldOne->m_hImage->Release();
#endif

		ZeroMemory(pOldOne, sizeof (IMGCACHE));
		pFreeOne = pOldOne;
	}

	ASSERT(pFreeOne != NULL);

	TRACE(_T("Loading %s into image cache...\n"), szURL);

	ASSERT(pImgLock == NULL);
	pImgLock = pFreeOne;

	pFreeOne->m_szURL = new TCHAR [_tcslen(szURL) + 1];
	_tcscpy(pFreeOne->m_szURL, szURL);

	pFreeOne->m_usage = XAppGetNow();

	pFreeOne->m_hImage = MyLoadImage(szURL);
	
	if (pFreeOne->m_hImage != NULL)
	{
#ifdef _USEGDI
		BITMAP bitmap;
		GetObject(pFreeOne->m_hImage, sizeof (BITMAP), &bitmap);
		pFreeOne->m_size.cx = bitmap.bmWidth;
		pFreeOne->m_size.cy = bitmap.bmHeight;
#endif
#ifdef _XBOX
		D3DSURFACE_DESC sd;
		VERIFYHR(pFreeOne->m_hImage->GetLevelDesc(0, &sd));
		pFreeOne->m_size.cx = (int)sd.Width;
		pFreeOne->m_size.cy = (int)sd.Height;
#endif
	}

	pImgLock = NULL;

	return pFreeOne;
}


extern "C" // this one is used by the RenderHTML C-code...
HIMAGE FetchImage(const TCHAR* szURL, SIZE* pSize)
{
	IMGCACHE* pImgCache = FindImage(szURL);
	if (pImgCache == NULL)
		return NULL;

	if (pSize != NULL)
		*pSize = pImgCache->m_size;

	return pImgCache->m_hImage;
}

HIMAGE MyLoadImage(const TCHAR* szURL)
{
	return LoadTexture(szURL, 0, 0);
}

////////////////////////////////////////////////////////////////////////////

bool DecodeRAW(const TCHAR* szFileName, CTexture* pTexture)
{
	FILE* fp = _tfopen(szFileName, _T("rb"));
	if (fp == NULL)
		return false;

	fseek(fp, 0, SEEK_END);
	DWORD dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int nWidth = 0;
	int nHeight = 0;
	int nDepth = 0;

	if (dwFileSize == 1024 * 1024 * 3)
	{
		nWidth = 1024;
		nHeight = 1024;
		nDepth = 24;
	}

	if (nWidth == 0 || nHeight == 0 || nDepth == 0)
	{
		fclose(fp);
		return false;
	}

	if (!pTexture->Create(nWidth, nHeight))
	{
		TRACE(_T("Create texture failed!\n"));
		fclose(fp);
		return false;
	}

/*
	D3DX_SURFACEFORMAT sf = D3DX_SF_R8G8B8;
	DWORD nMipMaps = 0;
	DWORD dwWidth = nWidth;
	DWORD dwHeight = nHeight;
	DWORD dwFlags = DDSD_CKSRCBLT;
	if (FAILED(D3DXCreateTexture(XAppGetD3DDev(), &dwFlags, &dwWidth, &dwHeight, &sf, NULL, &pSurface, &nMipMaps)))
	{
		TRACE(_T("Create texture failed!\n"));
		fclose(fp);
		return false;
	}

	ASSERT(sf == D3DX_SF_A8R8G8B8 || sf == D3DX_SF_X8R8G8B8);
*/

	D3DLOCKED_RECT lr;
	VERIFYHR(pTexture->m_pSurface->LockRect(0, &lr, NULL, D3DLOCK_DISCARD));
	void* pvPels = lr.pBits;
	int nPitch = lr.Pitch;

	BYTE* rgbsrc = new BYTE [nWidth * (nDepth / 8)];

	for (int y = 0; y < nHeight; y += 1)
	{
		fread(rgbsrc, 1, nWidth * (nDepth / 8), fp);

		BYTE* pbSrc = rgbsrc;
		BYTE* pbDest = (BYTE*)pvPels + y * nPitch;

		for (int x = 0; x < nWidth; x += 1)
		{
			pbDest[0] = pbSrc[2];
			pbDest[1] = pbSrc[1];
			pbDest[2] = pbSrc[0];
			pbDest[3] = 255;

			pbDest += 4;
			pbSrc += 3;
		}
	}

	delete [] rgbsrc;

	pTexture->m_pSurface->UnlockRect(0);

	fclose(fp);

	return true;
}




////////////////////////////////////////////////////////////////////////////

EXTERN_C BOOL GetImageSize(const TCHAR* szImgFile, SIZE* pSize)
{
	return FetchImage(szImgFile, pSize) != NULL;
}

EXTERN_C void DrawImage(HDRAW hDC, const TCHAR* szImgFile, int x, int y, int align, WORD* pcx, WORD* pcy)
{
	HIMAGE hImage;
	SIZE size;

	hImage = FetchImage(szImgFile, &size);

	if (hImage != NULL)
	{
		if ((align & TA_RIGHT) != 0)
			x -= size.cx;

		if ((align & TA_BOTTOM) != 0)
			y -= size.cy;

#ifdef _USEGDI
		HDRAW hImageDC = CreateCompatibleDC(hDC);
		if (hImageDC != NULL)
		{
			HGDIOBJ hOldBitmap = SelectObject(hImageDC, hImage);
			BitBlt(hDC, x, y, size.cx, size.cy, hImageDC, 0, 0, SRCCOPY);
			SelectObject(hImageDC, hOldBitmap);
			DeleteDC(hImageDC);
		}
#endif
#ifdef _XBOX
		X_BitBlt(hDC, x, y, size.cx, size.cy, hImage, 0, 0);
#endif
	}
	else
	{
		size.cx = 20;
		size.cy = 20;
	}

	if (pcx != NULL)
		*pcx = (WORD)size.cx;

	if (pcy != NULL)
		*pcy = (WORD)size.cy;
}

////////////////////////////////////////////////////////////////////////////

#ifndef _USEGDI
static bool Clip(HDRAW hDraw/*LPDIRECT3DSURFACE8 pSurface*/, int& x, int& y, int& cx, int& cy)
{
	ASSERT(hDraw != NULL);
//	ASSERT(pSurface != NULL);

//	D3DSURFACE_DESC desc;
//	VERIFYHR(pSurface->GetDesc(&desc));

	if (x < 0)
	{
		cx += x;
		x = 0;
	}

	if (y < 0)
	{
		cy += y;
		y = 0;
	}

	if (x + cx > (int)hDraw->Desc/*desc*/.Width)
		cx = (int)hDraw->Desc/*desc*/.Width - x;

	if (y + cy > (int)hDraw->Desc/*desc*/.Height)
		cy = (int)hDraw->Desc/*desc*/.Height - y;

	if (cx <= 0 || cy <= 0)
		return false;

	return true;
}

EXTERN_C HRESULT X_FillRect(HDRAW hDraw, int x, int y, int cx, int cy, D3DCOLOR color)
{
	ASSERT(hDraw != NULL);
//	ASSERT(pSurface != NULL);

//	TRACE(_T("X_FillRect: %d,%d %dx%d\n"), x, y, cx, cy);

#ifdef _DEBUG
	{
//		D3DSURFACE_DESC desc;
//		VERIFYHR(pSurface->GetDesc(&desc));
		ASSERT(hDraw->Desc/*desc*/.Format == D3DFMT_A8R8G8B8 || hDraw->Desc/*desc*/.Format == D3DFMT_X8R8G8B8);
	}
#endif

	if (!Clip(hDraw/*pSurface*/, x, y, cx, cy))
		return S_OK;

//	TRACE(_T("clipped to: %d,%d %dx%d\n"), x, y, cx, cy);

//    D3DLOCKED_RECT lock;
//	HRESULT hr = pSurface->LockRect(&lock, NULL, 0);
//	if (FAILED(hr))
//		return hr;

	for (int j = 0; j < cy; j += 1)
	{
		DWORD* ppel = (DWORD*)((BYTE*)hDraw->Lock/*lock*/.pBits + hDraw->Lock/*lock*/.Pitch * (y + j)) + x;
		for (int i = 0; i < cx; i += 1)
			*ppel++ = color;
	}

//	pSurface->UnlockRect();

	return S_OK;
}


EXTERN_C HRESULT X_BitBlt(HDRAW hDraw, int x, int y, int cx, int cy, LPDIRECT3DTEXTURE8 pSrcSurface, int xSrc, int ySrc)
{
	ASSERT(hDraw != NULL);
	ASSERT(pSrcSurface != NULL);

#ifdef _DEBUG
	{
		D3DSURFACE_DESC descSrc;

		VERIFYHR(pSrcSurface->GetLevelDesc(0, &descSrc));

		ASSERT(hDraw->Desc.Format == D3DFMT_A8R8G8B8 || hDraw->Desc.Format == D3DFMT_X8R8G8B8);
		ASSERT(hDraw->Desc.Format == descSrc.Format);
	}
#endif

	// REVIEW: This doesn't clip against the source size...

	if (!Clip(hDraw, x, y, cx, cy))
		return S_OK;

    D3DLOCKED_RECT lockSrc;
	HRESULT hr = pSrcSurface->LockRect(0, &lockSrc, NULL, D3DLOCK_READONLY);
	if (FAILED(hr))
		return hr;

	for (int j = 0; j < cy; j += 1)
	{
		DWORD* ppel = (DWORD*)((BYTE*)hDraw->Lock.pBits + hDraw->Lock.Pitch * (y + j)) + x;
		DWORD* ppelSrc = (DWORD*)((BYTE*)lockSrc.pBits + lockSrc.Pitch * (ySrc + j)) + xSrc;

		for (int i = 0; i < cx; i += 1)
			*ppel++ = *ppelSrc++;
	}

	pSrcSurface->UnlockRect(0);

	return S_OK;
}
#endif


