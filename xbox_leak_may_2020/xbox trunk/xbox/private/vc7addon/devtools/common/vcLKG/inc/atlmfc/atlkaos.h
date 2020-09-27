//NONSHIP

// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLKAOS_H__
#define __ATLKAOS_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error atlkaos.h requires atlwin.h to be included first
#endif

#include <math.h>
#include <atltypes.h>


namespace ATL
{

#define KE_OK			0
#define KE_FAIL			1
#define KE_OUTOFMEMORY		2
#define KE_POINTER		3
#define KE_RESOURCE_NOTFOUND	4

struct KAOS_XFORM
{
	float eM11;
	float eM12;
	float eM21;
	float eM22;
	float eDx;
	float eDy;
	float ePreDx;
	float ePreDy;

	void Transform(int x, int y, int* px, int* py)
	{
		x += (int)ePreDx;
		y += (int)ePreDy;
		*px = (int)(eM11 * x + eM12 * y + eDx);
		*py = (int)(eM21 * x + eM22 * y + eDy);
	}

	void ReverseTransform(int px, int py, int* x, int* y)
	{
		float y1 = (eM11 * ((py - eDy) / eM21) - px + eDx) / (eM22 * eM11 / eM21 - eM12);
		*x = (int)((px - eDx - eM12 * y1) / eM11);
		*y = (int)y1;
	}
};

class KaosDIB
{
public:
	DWORD* m_pdwBits;
	BITMAPINFO m_bmpInfo;
	HBITMAP m_hBmp;
	int m_nWidth;
	int m_nHeight;

	KaosDIB()
	{
		memset(&m_bmpInfo, 0x00, sizeof(m_bmpInfo));
		m_pdwBits = NULL;
		m_hBmp = NULL;
	}

	~KaosDIB()
	{
		if (m_hBmp != NULL)
			::DeleteObject(m_hBmp);
	}

	int GetWidth()
	{
		return m_nWidth;
	}

	int GetHeight()
	{
		return m_nHeight;
	}

	int LoadImage(HINSTANCE hInst, LPCTSTR szFilename)
	{
		if (m_hBmp)
		{
			DeleteObject(m_hBmp);
			m_hBmp = NULL;
		}

		HBITMAP hBmp = (HBITMAP) ::LoadImage(hInst, szFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!hBmp)
			return KE_RESOURCE_NOTFOUND;

		// get the dimensions of the bitmap
		BITMAP bmp;
		GetObject(hBmp, sizeof(bmp), &bmp);

		m_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
		m_bmpInfo.bmiHeader.biHeight = -bmp.bmHeight;	// request a top down DIB
		m_bmpInfo.bmiHeader.biPlanes = 1;
		m_bmpInfo.bmiHeader.biBitCount = 32;
		m_bmpInfo.bmiHeader.biCompression = BI_RGB;		// uncompressed

		HDC hDCDisplay = GetDC(NULL);

		// Create the DIB
		m_hBmp = CreateDIBSection(hDCDisplay, &m_bmpInfo, DIB_RGB_COLORS, (void **) &m_pdwBits, NULL, 0);

		m_nWidth = bmp.bmWidth;
		m_nHeight = bmp.bmHeight;

		GetDIBits(hDCDisplay, hBmp, 0, m_nHeight, m_pdwBits, &m_bmpInfo, DIB_RGB_COLORS);
		ReleaseDC(NULL, hDCDisplay);

		DeleteObject(hBmp);
		return 0;
	}

	int LoadFromResource(HINSTANCE hInst, UINT uId)
	{
		return LoadFromResource(hInst, MAKEINTRESOURCE(uId));
	}

	int LoadFromResource(HINSTANCE hInst, LPCTSTR szRes)
	{
		if (m_hBmp != NULL)
		{
			::DeleteObject(m_hBmp);
			m_hBmp = NULL;
		}

		HBITMAP hBmp = ::LoadBitmap(hInst, szRes);
		if (!hBmp)
			return KE_RESOURCE_NOTFOUND;

		// get the dimensions of the bitmap
		BITMAP bmp;
		::GetObject(hBmp, sizeof(bmp), &bmp);

		m_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
		m_bmpInfo.bmiHeader.biHeight = -bmp.bmHeight;	// request a top down DIB
		m_bmpInfo.bmiHeader.biPlanes = 1;
		m_bmpInfo.bmiHeader.biBitCount = 32;
		m_bmpInfo.bmiHeader.biCompression = BI_RGB;		// uncompressed

		HDC hDCDisplay = ::GetDC(NULL);

		// Create the DIB
		m_hBmp = ::CreateDIBSection(hDCDisplay, &m_bmpInfo, DIB_RGB_COLORS, (void**)&m_pdwBits, NULL, 0);

		m_nWidth = bmp.bmWidth;
		m_nHeight = bmp.bmHeight;

		::GetDIBits(hDCDisplay, hBmp, 0, m_nHeight, m_pdwBits, &m_bmpInfo, DIB_RGB_COLORS);
		::ReleaseDC(NULL, hDCDisplay);

		::DeleteObject(hBmp);
		return 0;
	}

	void Blt(HDC hDC, int x, int y, int width, int height)
	{
		if (!m_pdwBits)
			return;
		::StretchDIBits(hDC, x, y, width, height, 0, 0, m_bmpInfo.bmiHeader.biWidth, -m_bmpInfo.bmiHeader.biHeight, m_pdwBits, &m_bmpInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	void AlphaBlt(HDC hDC, int x, int y, int width, int height, UINT uAlpha, COLORREF clrTransparent, LPCRECT prctUpdate)
	{
		if (!m_pdwBits)
			return;

		if (uAlpha == 255)
		{
			Blt(hDC, x, y, width, height);
			return;
		}

		// get the background dib from the hdc
		HBITMAP hBmpDest = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);
		DIBSECTION dibSrc;
		::GetObject(hBmpDest, sizeof(DIBSECTION), &dibSrc);

		POINT pt;
		::GetViewportOrgEx(hDC, &pt);

		int xSrc = 0;
		int ySrc = 0;

		if (pt.x+x < 0)
		{
			xSrc = -pt.x-x;
			pt.x = 0;
			x = 0;
		}

		if (pt.y+y < 0)
		{
			ySrc = -pt.y-y;
			pt.y = 0;
			y = 0;
		}
#if 0
		if (xSrc < 0)
		{
			x = -xSrc;
			xSrc = 0;
		}
		if (ySrc < 0)
		{
			y = -ySrc;
			ySrc = 0;
		}
#endif

		DWORD* pdwDest = ((DWORD*)dibSrc.dsBm.bmBits);
		pdwDest += ((y+pt.y)*dibSrc.dsBm.bmWidth)+x+pt.x;
		DWORD* pdwSrc = (DWORD*)m_pdwBits+(ySrc*m_nWidth)+xSrc;

		DWORD* pdwDestMax = ((DWORD*)dibSrc.dsBm.bmBits) + dibSrc.dsBm.bmWidth*dibSrc.dsBm.bmHeight;

		int nWidth = m_nWidth-xSrc;
		int nHeight = m_nHeight-ySrc;

		if (nWidth > dibSrc.dsBm.bmWidth - pt.x - x)
			nWidth = dibSrc.dsBm.bmWidth - pt.x - x;

		if (nHeight > dibSrc.dsBm.bmHeight - pt.y - y)
			nHeight = dibSrc.dsBm.bmHeight - pt.y - y;

		if (nWidth > dibSrc.dsBm.bmWidth)
			nWidth = dibSrc.dsBm.bmWidth;

		if (nHeight > dibSrc.dsBm.bmHeight)
			nHeight = dibSrc.dsBm.bmHeight;

//		int nDestWrapWidth = dibSrc.dsBm.bmWidth - nWidth;
		int nDestWrapWidth = prctUpdate->right - prctUpdate->left - nWidth;
		int nSrcWrapWidth = m_nWidth - nWidth;

		for (int py=0; py<nHeight; py++)
		{
			for (int px=0; px<nWidth; px++)
			{
				if (pdwDest >= pdwDestMax)
					break;

				if (pdwDest >= (DWORD*)dibSrc.dsBm.bmBits && pdwSrc >= m_pdwBits)
				{
					DWORD dwSrc = *pdwSrc;
					if (dwSrc != clrTransparent)
					{
						DWORD dwDest = *pdwDest;
						DWORD dwRed = GetRValue(dwSrc);
						dwRed = (255-uAlpha)*((DWORD)GetRValue(dwDest))/255 + uAlpha*dwRed/255;
						dwRed &= 0xff;
						DWORD dwGreen = GetGValue(dwSrc);
						dwGreen = (255-uAlpha)*((DWORD)GetGValue(dwDest))/255 + uAlpha*dwGreen/255;
						dwGreen &= 0xff;
						DWORD dwBlue = GetBValue(dwSrc);
						dwBlue = (255-uAlpha)*((DWORD)GetBValue(dwDest))/255 + uAlpha*dwBlue/255;
						dwGreen &= 0xff;
						*pdwDest = (((DWORD)RGB(dwRed, dwGreen, dwBlue)));
//						*pdwDest = dwSrc;
					}
				}
				pdwSrc++;
				pdwDest++;
			}
			pdwDest += nDestWrapWidth;
			pdwSrc += nSrcWrapWidth;
		}

	}

	KaosDIB* Copy()
	{
		if (!m_pdwBits)
			return NULL;

		KaosDIB* p = NULL;
		ATLTRY(p = new KaosDIB);
		ATLASSERT(p != NULL);

		memcpy(&(p->m_bmpInfo), &(m_bmpInfo), sizeof(m_bmpInfo));

		HDC hDCDisplay = ::GetDC(NULL);

		// Create the DIB
		p->m_hBmp = ::CreateDIBSection(hDCDisplay, &(p->m_bmpInfo), DIB_RGB_COLORS, (void**)&(p->m_pdwBits), NULL, 0);

		p->m_nWidth = m_nWidth;
		p->m_nHeight = m_nHeight;

		memcpy(p->m_pdwBits, m_pdwBits, m_nWidth*m_nHeight*sizeof(DWORD));
		::ReleaseDC(NULL, hDCDisplay);
		return p;
	}

	int Copy(KaosDIB* pDest)
	{
		if (pDest->m_hBmp)
			::DeleteObject(pDest->m_hBmp);

		memcpy(&(pDest->m_bmpInfo), &m_bmpInfo, sizeof(m_bmpInfo));

		HDC hDCDisplay = ::GetDC(NULL);
		pDest->m_hBmp = ::CreateDIBSection(hDCDisplay, &(pDest->m_bmpInfo), DIB_RGB_COLORS, (void**)&(pDest->m_pdwBits), NULL, 0);
		::ReleaseDC(NULL, hDCDisplay);

		pDest->m_nWidth = m_nWidth;
		pDest->m_nHeight = m_nHeight;
		memcpy(pDest->m_pdwBits, m_pdwBits, m_nWidth*m_nHeight*sizeof(DWORD));

		return 0;
	}

	void AdjustBrightness(int uStrength, COLORREF clrTransparent)
	{
		if (!m_pdwBits)
			return;

		AdjustBrightness(m_pdwBits, m_nWidth, m_nHeight, uStrength, clrTransparent);
	}

	void Pixelate(int uStrength, COLORREF clrTransparent)
	{
		if (!m_pdwBits)
			return;

		Pixelate(m_pdwBits, m_nWidth, m_nHeight, uStrength, clrTransparent);
	}

	void ApplyLightSource(int uStrength, int xLight, int yLight, COLORREF clrTransparent)
	{
		if (!m_pdwBits)
			return;

		ApplyLightSource(m_pdwBits, m_nWidth, m_nHeight, uStrength, xLight, yLight, clrTransparent);
	}

	void Blur(int uStrength, COLORREF clrTransparent)
	{
		if (!m_pdwBits)
			return;

		Blur(m_pdwBits, m_nWidth, m_nHeight, uStrength, clrTransparent);
	}

	void AdjustColorLevels(int nRedAdj, int nGreenAdj, int nBlueAdj, COLORREF clrTransparent)
	{
		if (!m_pdwBits)
			return;

		AdjustColorLevels(m_pdwBits, m_nWidth, m_nHeight, nRedAdj, nGreenAdj, nBlueAdj, clrTransparent);
	}

	void ApplyReverseTransform(KAOS_XFORM* pxform, COLORREF clrTransparent)
	{
		if (!m_pdwBits)
			return;
		ApplyReverseTransform(m_pdwBits, m_nWidth, m_nHeight, pxform, clrTransparent);
	}

	void Rotate(double dDegrees, int xCenter, int yCenter, COLORREF clrTransparent)
	{
		KAOS_XFORM xform;

		double dRads = 3.141592*dDegrees/180;

		xform.eM11 = (float)cos(-dRads);
		xform.eM12 = (float)sin(-dRads);
		xform.eM21 = (float)(-xform.eM12);
		xform.eM22 = (float)xform.eM11;
		xform.eDx =  (float)xCenter;
		xform.eDy = (float)yCenter;
		xform.ePreDx = (float)(-xCenter);
		xform.ePreDy = (float)(-yCenter);
		ApplyReverseTransform(&xform, clrTransparent);
	}

	void Scale(float xScale, float yScale, COLORREF clrTransparent)
	{
		KAOS_XFORM xform;

		xform.eM11 = 1.F / xScale;
		xform.eM12 = 0.F;
		xform.eM21 = 0.F;
		xform.eM22 = 1.F / yScale;
		xform.eDx =  0.F;
		xform.eDy = 0.F;
		xform.ePreDx = 0.F;
		xform.ePreDy = 0.F;
		ApplyReverseTransform(&xform, clrTransparent);
	}

	void Shear(float xShear, float yShear, COLORREF clrTransparent)
	{
		KAOS_XFORM xform;

		xform.eM11 = 1.F;
		xform.eM12 = (float)(-xShear);
		xform.eM21 = (float)(-yShear);
		xform.eM22 = 1.F;
		xform.eDx =  0.F;
		xform.eDy = 0.F;
		xform.ePreDx = 0.F;
		xform.ePreDy = 0.F;
		ApplyReverseTransform(&xform, clrTransparent);
	}

// static helpers
	static void AdjustBrightness(DWORD* pdwBits, int nWidth, int nHeight, int uStrength, COLORREF clrTransparent)
	{
		int nWords = nWidth*nHeight;
		
		while (nWords >= 0)
		{
			DWORD dwPixel = *pdwBits;
			if (dwPixel != clrTransparent)
			{
				int nRed = GetRValue(dwPixel);
				int nGreen = GetGValue(dwPixel);
				int nBlue = GetBValue(dwPixel);

				nRed += uStrength;
				nGreen += uStrength;
				nBlue += uStrength;

				if (nRed < 0)
					nRed = 0;
				else if (nRed > 0xff)
					nRed = 0xff;
				if (nGreen < 0)
					nGreen = 0;
				else if (nGreen > 0xff)
					nGreen = 0xff;
				if (nBlue < 0)
					nBlue = 0;
				else if (nBlue > 0xff)
					nBlue = 0xff;

				*pdwBits = RGB(nRed, nGreen, nBlue);
			}
			pdwBits++;
			nWords--;
		}
	}

	static void Pixelate(DWORD* pdwBits, int nWidth, int nHeight, int uStrength, COLORREF clrTransparent)
	{
		if (uStrength <= 0)
			return;
		for (int y=0; y < nHeight; y+=uStrength)
		{
			for (int x=0; x<nWidth; x+=uStrength)
			{

				DWORD dwRed=0;
				DWORD dwGreen = 0;
				DWORD dwBlue = 0;

				int py;
				int px;

				for (py=0; py<uStrength && y+py<nHeight; py++)
				{
					for (px=0; px<uStrength && x+px<nWidth; px++)
					{
						DWORD dwPixel = *(pdwBits+(y+py)*nWidth+x+px);
						if (dwPixel != clrTransparent)
						{
							dwRed += GetRValue(dwPixel);
							dwGreen += GetGValue(dwPixel);
							dwBlue += GetBValue(dwPixel);
						}
					}
				}
				dwRed /= (uStrength*uStrength);
				dwGreen /= (uStrength*uStrength);
				dwBlue /= (uStrength*uStrength);

				for (py=0; py<uStrength && y+py<nHeight; py++)
				{
					for (px=0; px<uStrength && x+px<nWidth; px++)
					{
						DWORD dwPixel = *(pdwBits+(y+py)*nWidth+x+px);
						if (dwPixel != clrTransparent)
						{
							*(pdwBits+(y+py)*nWidth+x+px) = RGB(dwRed, dwGreen, dwBlue);
						}
					}
				}
			}
		}
	}

	static void ApplyLightSource(DWORD* pdwBits, int nWidth, int nHeight, int uStrength, int xLight, int yLight, COLORREF clrTransparent)
	{
		for (int y=0; y<nHeight; y++)
		{
			for (int x=0; x<nWidth; x++)
			{
				DWORD dwPixel = *(pdwBits+y*nWidth+x);
				if (dwPixel != clrTransparent)
				{
					double dDist = sqrt((x-xLight)*(x-xLight)+(y-yLight)*(y-yLight));


					double dIntensity;
					dIntensity = (uStrength-dDist);
					if (dIntensity < 0)
						dIntensity *= -dIntensity/uStrength;
					else
						dIntensity *= dIntensity/uStrength;

					int nRed = GetRValue(dwPixel);
					int nGreen = GetGValue(dwPixel);
					int nBlue = GetBValue(dwPixel);

					nRed += (int)dIntensity;
					nGreen += (int)dIntensity;
					nBlue += (int)dIntensity;

					if (nRed < 0)
						nRed = 0;
					else if (nRed > 0xff)
						nRed = 0xff;
					if (nGreen < 0)
						nGreen = 0;
					else if (nGreen > 0xff)
						nGreen = 0xff;
					if (nBlue < 0)
						nBlue = 0;
					else if (nBlue > 0xff)
						nBlue = 0xff;

					*(pdwBits+y*nWidth+x) = RGB(nRed, nGreen, nBlue);
				}
			}
		}
	}

	static void Blur(DWORD* pdwBits, int nWidth, int nHeight, int uStrength, COLORREF clrTransparent)
	{
		if (uStrength <= 0)
			return;

		DWORD* pdwTemp = NULL;
		ATLTRY(pdwTemp = (DWORD*)malloc(nWidth*nHeight*sizeof(DWORD)));
		ATLASSERT(pdwTemp != NULL);

		memcpy(pdwTemp, pdwBits, nWidth*nHeight*sizeof(DWORD));
		for (int y=0; y<nHeight; y++)
		{
			for (int x=0; x<nWidth; x++)
			{
				if (*(pdwTemp+y*nWidth+x)==clrTransparent)
					continue;

				DWORD dwRed = 0;
				DWORD dwGreen = 0;
				DWORD dwBlue = 0;
				DWORD dwPixel = 0;
				int nCount = 0;
				for (int py=y-uStrength; py < y+uStrength; py++)
				{
					for (int px=x-uStrength; px < x+uStrength; px++)
					{
						if (py >= 0 && py < nHeight && px >= 0 && px < nWidth)
						{
							dwPixel = *(pdwBits+py*nWidth+px);
							if (dwPixel != clrTransparent)
							{
								dwRed += GetRValue(dwPixel);
								dwGreen += GetGValue(dwPixel);
								dwBlue += GetBValue(dwPixel);
								nCount++;
							}
						}
					}
				}
				if (nCount)
				{
					dwRed /= nCount;
					dwGreen /= nCount;
					dwBlue /= nCount;

					if (dwRed > 0xff)
						dwRed = 0xff;
					if (dwGreen > 0xff)
						dwGreen = 0xff;
					if (dwBlue > 0xff)
						dwBlue = 0xff;

					*(pdwTemp+y*nWidth+x) = RGB(dwRed, dwGreen, dwBlue);
				}
				else
					*(pdwTemp+y*nWidth+x) = clrTransparent;
			}
		}
		memcpy(pdwBits, pdwTemp, nWidth*nHeight*sizeof(DWORD));
		free(pdwTemp);
	}

	static void AdjustColorLevels(DWORD* pdwBits, int nWidth, int nHeight, int nRedAdj, int nGreenAdj, int nBlueAdj, COLORREF clrTransparent)
	{
		int nWords = nWidth*nHeight;
		
		while (nWords >= 0)
		{
			DWORD dwPixel = *pdwBits;
			if (dwPixel != clrTransparent)
			{
				int nRed = GetRValue(dwPixel);
				int nGreen = GetGValue(dwPixel);
				int nBlue = GetBValue(dwPixel);

				nRed += nRedAdj;
				nGreen += nGreenAdj;
				nBlue += nBlueAdj;

				if (nRed < 0)
					nRed = 0;
				else if (nRed > 0xff)
					nRed = 0xff;
				if (nGreen < 0)
					nGreen = 0;
				else if (nGreen > 0xff)
					nGreen = 0xff;
				if (nBlue < 0)
					nBlue = 0;
				else if (nBlue > 0xff)
					nBlue = 0xff;

				*pdwBits = RGB(nRed, nGreen, nBlue);
			}
			pdwBits++;
			nWords--;
		}
	}

	static void ApplyReverseTransform(DWORD* pdwBits, int nWidth, int nHeight, KAOS_XFORM* pxform, COLORREF clrTransparent)
	{
		DWORD* pSrc = NULL;
		ATLTRY(pSrc = (DWORD*)malloc(nWidth*nHeight*sizeof(DWORD)));
		if (pSrc == NULL)
			return;
		memcpy(pSrc, pdwBits, nWidth*nHeight*sizeof(DWORD));

		for (int i=0; i<nWidth*nHeight; i++)
			pdwBits[i] = clrTransparent;

		for (int y=0; y<nHeight; y++)
		{
			for (int x=0; x<nWidth; x++)
			{
				int x1;
				int y1;

				pxform->Transform(x, y, &x1, &y1);

				if (x1 < nWidth && x1 >= 0 && y1 >= 0 && y1 < nHeight)
				{
					DWORD dwPixel = *(pSrc+y1*nWidth+x1);
					if (dwPixel != clrTransparent)
						*(pdwBits+y*nWidth+x) = dwPixel;
				}

			}
		}
		free(pSrc);
	}
};


__interface IKaosChildSystem
{
	void InvalidateParents();
	void Repaint();
	IKaosChildSystem* SetMouseCapture(IKaosChildSystem* pChild);
	void ReleaseMouseCapture();
	IKaosChildSystem *GetMouseCapture();
	void SetParent(IKaosChildSystem* pParent);

	LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Paint(CWinDC dc, const CRect& rctRect, const CRect& rctUpdate);

	const CRect& GetRect();
	void GetOldRect(RECT* pRect);
	bool IsDirty();
	void SetDirty(bool bDirty);
	BOOL PtInWindow(POINT pt);
	void ParentResize(const CRect& rctRect);
	void MouseMessage(ULONG uMsg, USHORT nKeyState, CPoint pt);
};

__interface IKaosChild : IKaosChildSystem
{
	void OnPaint(CWinDC dc, const CRect& rctRect, const CRect& rctUpdate);
	void OnMouseMessage(ULONG uMsg, USHORT nKeyState, CPoint pt);
	void OnParentResize(const CRect& rctRect);
};


class CKaosElement : public IKaosChildSystem
{
private:
	CRect m_rectOld;
	CRect m_rect;
	bool m_bDirty;
	CPoint m_ptCaptureOrigin;
	HBITMAP m_hBmpMask;
	UINT m_uAlpha;	// 0=invisible, 255=opaque
protected:
	IKaosChildSystem* m_pParent;
	CSimpleArray<IKaosChildSystem*> m_childList;
	IKaosChildSystem *m_pCapture;
	IKaosChildSystem *m_pFocus;

public:
	CKaosElement() : m_rect(0,0,0,0), m_rectOld(0,0,0,0)
	{
		m_pCapture = NULL;
		m_pFocus = NULL;
		m_hBmpMask = NULL;
		m_pParent = NULL;
		m_uAlpha = 255;
		m_bDirty = true; // We haven't been drawn yet so force repaint
	}
/**/	virtual ~CKaosElement()
	{ }

	UINT GetAlpha()
	{
		return m_uAlpha;
	}
	void SetAlpha(UINT nAlpha)
	{
		m_uAlpha = nAlpha;
		SetDirty();
	}
	void SetParent(IKaosChildSystem* pParent)
	{
		m_pParent = pParent;
	}
	int SetMask(HBITMAP hBmpMask)
	{
		if (m_hBmpMask)
		{
			::DeleteObject(m_hBmpMask);
			m_hBmpMask = NULL;
		}
		m_hBmpMask = CopyBitmap(hBmpMask);
		return 0;
	}

	int CreateMask(HBITMAP hBmp, COLORREF clrTransparent)
	{
		// delete the old mask if we have one
		if (m_hBmpMask)
		{
			::DeleteObject(m_hBmpMask);
			m_hBmpMask = NULL;
		}
		
		BITMAP bmp;
		::GetObject(hBmp, sizeof(bmp), &bmp);

		HDC hDCDisplay = ::GetDC(NULL);
		HDC hDCDest = ::CreateCompatibleDC(hDCDisplay);
		HDC hDCSrc = ::CreateCompatibleDC(hDCDisplay);
		::ReleaseDC(NULL, hDCDisplay);

		m_hBmpMask = ::CreateBitmap(bmp.bmWidth, bmp.bmHeight, 1, 1, NULL);
		
		HBITMAP hOldSrc = (HBITMAP)::SelectObject(hDCSrc, hBmp);
		HBITMAP hOldDest = (HBITMAP)::SelectObject(hDCDest, m_hBmpMask);

		::SetBkColor(hDCSrc, clrTransparent);

		::BitBlt(hDCDest, 0, 0, bmp.bmWidth, bmp.bmHeight, hDCSrc, 0, 0, SRCCOPY);

		::SelectObject(hDCSrc, hOldSrc);
		::SelectObject(hDCDest, hOldDest);
		::DeleteDC(hDCSrc);
		::DeleteDC(hDCDest);
		return 0;
	}

	int CreateMask(KaosDIB* pdib, COLORREF clrTransparent)
	{
		// delete the old mask if we have one
		if (m_hBmpMask)
		{
			::DeleteObject(m_hBmpMask);
			m_hBmpMask = NULL;
		}
		
		HDC hDCDisplay = ::GetDC(NULL);
		HDC hDCSrc = ::CreateCompatibleDC(hDCDisplay);
		HDC hDCDest = ::CreateCompatibleDC(hDCDisplay);

		HBITMAP hBmpTemp = ::CreateCompatibleBitmap(hDCDisplay, pdib->GetWidth(), pdib->GetHeight());

		::ReleaseDC(NULL, hDCDisplay);

		m_hBmpMask = ::CreateBitmap(pdib->GetWidth(), pdib->GetHeight(), 1, 1, NULL);
		
		HBITMAP hOldDest = (HBITMAP)::SelectObject(hDCDest, m_hBmpMask);
		HBITMAP hOldSrc = (HBITMAP)::SelectObject(hDCSrc, hBmpTemp);

		::StretchDIBits(hDCSrc, 0, 0, pdib->GetWidth(), pdib->GetHeight(), 0, 0, pdib->GetWidth(), pdib->GetHeight(), pdib->m_pdwBits,
				&(pdib->m_bmpInfo), DIB_RGB_COLORS, SRCCOPY);

		::SetBkColor(hDCSrc, clrTransparent);

		::BitBlt(hDCDest, 0, 0, pdib->GetWidth(), pdib->GetHeight(), hDCSrc, 0, 0, SRCCOPY);

		::SelectObject(hDCDest, hOldDest);
		::SelectObject(hDCDest, hOldSrc);
		::DeleteDC(hDCSrc);
		::DeleteDC(hDCDest);
		return 0;
	}

	BOOL PtInWindow(POINT pt)
	{
		BOOL fIn = FALSE;
		if (m_hBmpMask)
		{
			CPoint ptTest(pt.x - m_rect.left, pt.y-m_rect.top);
			HDC hDCDisplay = ::GetDC(NULL);
			HDC hDCMem = ::CreateCompatibleDC(hDCDisplay);
			HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, m_hBmpMask);
			::ReleaseDC(NULL, hDCDisplay);

			fIn = GetPixel(hDCMem, ptTest.x, ptTest.y) == 0;

			::SelectObject(hDCMem, hBmpOld);
			::DeleteDC(hDCMem);
		}
		else
		{
			if (m_rect.PtInRect(pt))
				fIn = TRUE;
		}

		return fIn;
	}

	void InvalidateParents()
	{
		SetDirty();
		if (m_pParent)
			m_pParent->InvalidateParents();
	}

	void Repaint()
	{
		if (m_pParent)
			m_pParent->Repaint();
	}

	const CRect& GetRect()
	{
		return m_rect;
	}
	void GetOldRect(LPRECT pRect)
	{
		CRect rctUnion;
		rctUnion.UnionRect(m_rectOld, m_rect);
		*pRect = rctUnion;
	}
	void SetSize(int nWidth, int nHeight)
	{
		m_rectOld = m_rect;
		m_rect.right = m_rect.left + nWidth;
		m_rect.bottom = m_rect.top + nHeight;
	}
	void MoveTo(int nX, int nY)
	{
		m_rectOld = m_rect;
		int nWidth = m_rect.Width();
		int nHeight = m_rect.Height();
		m_rect.left = nX;
		m_rect.top = nY;
		m_rect.right = nX + nWidth;
		m_rect.bottom = nY + nHeight;
	}
	bool IsDirty()
	{
		return m_bDirty || m_rect != m_rectOld;
	}
	void SetDirty(bool bDirty = true)
	{
		m_bDirty = bDirty;
		if (bDirty == false)
			m_rectOld = m_rect;
	}

	IKaosChildSystem* SetMouseCapture(IKaosChildSystem* pChild)
	{
		if (m_pCapture)
			ReleaseMouseCapture();
		if (m_pParent)
			m_pParent->SetMouseCapture(this);
		IKaosChildSystem* pCapture = m_pCapture;
		m_pCapture = pChild;
		return pCapture;
	}
	void ReleaseMouseCapture()
	{
		m_pCapture = NULL;
		if (m_pParent)
			m_pParent->ReleaseMouseCapture();
	}
	IKaosChildSystem *GetMouseCapture()
	{
		return m_pCapture;
	}

	void Add(IKaosChildSystem* pChild)
	{
		pChild->SetParent(this);
		m_childList.Add(pChild);
		// Tell child initial surface size
		pChild->ParentResize(m_rect);
	}

// These are the user overridable notifications
public:
	virtual LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}
	virtual void Paint(CWinDC dc, const CRect& rctRect, const CRect& rctUpdate)
	{
		CPoint ptOrg;
		GetViewportOrgEx(dc, &ptOrg);
		OnPaint(dc, rctRect, rctUpdate);
		int nChildCount = m_childList.GetSize();
		for (int i=0; i<nChildCount; i++)
		{
			IKaosChildSystem* pChild = m_childList[i];
			CRect rctIntersect;
			if (::IntersectRect(&rctIntersect, rctRect, pChild->GetRect()))
			{
				// paint it
				// set the view port to virtualize the coordinates
				SetViewportOrgEx(dc, pChild->GetRect().left + ptOrg.x,
					pChild->GetRect().top + ptOrg.y, NULL);
				CRect rcChild(pChild->GetRect());
				rcChild.OffsetRect(-rcChild.left, -rcChild.top);
				pChild->Paint(dc, rcChild, rctUpdate);
				pChild->SetDirty(false);
			}
		}
	}
	virtual void MouseMessage(ULONG uMsg, USHORT nKeyState, CPoint pt)
	{
		IKaosChildSystem* pChild = NULL;
		if (m_pCapture)
			pChild = m_pCapture;
		else if (m_pParent->GetMouseCapture() != this)
			pChild = WindowFromPoint(pt);
		if (pChild)
		{
			CPoint ptChild(pt.x - m_rect.left, pt.y - m_rect.top);
			pChild->MouseMessage(uMsg, 0, ptChild);
			return;
		}
		OnMouseMessage(uMsg, 0, pt);
	}
	virtual void ParentResize(const CRect& rctRect)
	{
		OnParentResize(rctRect);
	}
	IKaosChildSystem *WindowFromPoint(POINT pt)
	{
		for (int i=m_childList.GetSize()-1; i>=0; i--)
		{
			IKaosChildSystem* pChild = m_childList[i];
			CPoint ptChild(pt.x - m_rect.left, pt.y - m_rect.top);
			// do a high level hit test
			if (PtInRect(&pChild->GetRect(), ptChild))
			{
				if (pChild->PtInWindow(ptChild))
					return pChild;
			}
		}
		return NULL;
	}
	void NormalizePoint(IKaosChildSystem *pChild, POINT *ppt)
	{
		ppt->x -= pChild->GetRect().left;
		ppt->y -= pChild->GetRect().top;
	}

	void DeNormalizePoint(CKaosElement *pChild, POINT *ppt)
	{
		ppt->x += pChild->GetRect().left;
		ppt->y += pChild->GetRect().top;
	}

	void DeNormalizeRect(CKaosElement *pChild, RECT *prect)
	{
		prect->left += pChild->GetRect().left;
		prect->right += pChild->GetRect().left;
		prect->top += pChild->GetRect().top;
		prect->bottom += pChild->GetRect().top;
	}

// These are the user overridable notifications
public:
	virtual void OnPaint(CWinDC dc, const CRect& rctRect, const CRect& rctUpdate)
	{
	}
	virtual void OnMouseMessage(ULONG uMsg, USHORT nKeyState, CPoint pt)
	{
		if (m_pParent)
		{
			if (m_pParent->GetMouseCapture() == this)
			{
				switch (uMsg)
				{
				case WM_MOUSEMOVE:
					MoveTo(pt.x - m_ptCaptureOrigin.x, pt.y - m_ptCaptureOrigin.y);
					InvalidateParents();
					Repaint();
					break;
				case WM_LBUTTONUP:
					m_pParent->ReleaseMouseCapture();
					break;
				}
			}
			else
			{
				switch (uMsg)
				{
				case WM_LBUTTONDOWN:
					m_ptCaptureOrigin = pt - CPoint(m_rect.left, m_rect.top);
					m_pParent->SetMouseCapture(this);
				}
			}
		}
	}
	virtual void OnParentResize(const CRect& rctRect)
	{
	}

// Helpers
	static HBITMAP CopyBitmap(HBITMAP hBmp)
	{
		HBITMAP hBmpRet = NULL;

		BITMAP bmp;
		::GetObject(hBmp, sizeof(BITMAP), &bmp);

		hBmpRet = ::CreateBitmapIndirect(&bmp);
		if (hBmpRet)
		{
			// copy the actual pixels now
			HDC hDCSrc;
			HDC hDCDest;
			HDC hDCDisplay = ::GetDC(NULL);

			hDCSrc = ::CreateCompatibleDC(hDCDisplay);
			hDCDest = ::CreateCompatibleDC(hDCDisplay);

			::ReleaseDC(NULL, hDCDisplay);

			HBITMAP hOldSrc = (HBITMAP)::SelectObject(hDCSrc, hBmp);
			HBITMAP hOldDest = (HBITMAP)::SelectObject(hDCDest, hBmpRet);

			::BitBlt(hDCDest, 0, 0, bmp.bmWidth, bmp.bmHeight, hDCSrc, 0, 0, SRCCOPY);

			::SelectObject(hDCSrc, hOldSrc);
			::SelectObject(hDCDest, hOldDest);

			::DeleteDC(hDCSrc);
			::DeleteDC(hDCDest);
		}
		return hBmpRet;
	}
};

template <class T>
class CKaosImpl : public CKaosElement
{
public:
	BEGIN_MSG_MAP(CKaosImpl< T >)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_RANGE_HANDLER(WM_KEYFIRST, WM_KEYLAST, OnKeyMessage)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
	END_MSG_MAP()
	
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
//		RECT rctClient;
//		GetClientRect(&rctClient);
//		OnPaintBackground(dc, rctClient);
		return TRUE;
	}

	void OptimizedPaint()
	{
		T* pT = static_cast<T*>(this);
		RECT rctClient;
		pT->GetClientRect(&rctClient);
		HDC hDCDisplay = pT->GetDC();
		CWinDC hDCMem = ::CreateCompatibleDC(hDCDisplay);

		int nChildCount = m_childList.GetSize();
		for (int i=nChildCount-1; i>=0; i--)
		{
			IKaosChildSystem* pChild = m_childList[i];
			if (pChild->IsDirty())
			{
				pT->ValidateRect(&rctClient);
				CRect rcOld;
				pChild->GetOldRect(&rcOld);
				pT->InvalidateRect(rcOld, FALSE);
				CRect rctUpdate;
				pT->GetUpdateRect(&rctUpdate, FALSE);
				if (rctUpdate.Width() == 0 || rctUpdate.Height() == 0)
					continue;
				PAINTSTRUCT ps;
				HDC hDC = pT->BeginPaint(&ps);
				// create a dib for the offscreen buffering
				void *pvBits;
				BITMAPINFO bmpInfo;
				bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmpInfo.bmiHeader.biWidth = rctUpdate.right-rctUpdate.left;
				bmpInfo.bmiHeader.biHeight = -(rctUpdate.bottom-rctUpdate.top);
				bmpInfo.bmiHeader.biPlanes = 1;
				bmpInfo.bmiHeader.biBitCount = 32;
				bmpInfo.bmiHeader.biCompression = BI_RGB;
				bmpInfo.bmiHeader.biClrUsed = 0;

				HBITMAP hBmp = ::CreateDIBSection(hDCDisplay, &bmpInfo, DIB_RGB_COLORS, &pvBits, NULL, 0);
				HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDCMem, hBmp);

				SetViewportOrgEx(hDCMem, -rctUpdate.left, -rctUpdate.top, NULL);
				for (int j=0; j<i; j++)
				{
					IKaosChildSystem* pOther = m_childList[j];
					CRect rcIntersect;
					if (::IntersectRect(&rcIntersect, &rcOld, &pOther->GetRect()))
					{
						// paint it
						// set the view port to virtualize the coordinates
						::SetViewportOrgEx(hDCMem, pOther->GetRect().left - rctUpdate.left,
							pOther->GetRect().top - rctUpdate.top, NULL);
						CRect rcChild(pOther->GetRect());
						rcChild.OffsetRect(-rcChild.left, -rcChild.top);
						pOther->Paint(hDCMem, rcChild, rctUpdate);
					}
				}
				// paint it
				// set the view port to virtualize the coordinates
				::SetViewportOrgEx(hDCMem, pChild->GetRect().left - rctUpdate.left,
					pChild->GetRect().top - rctUpdate.top, NULL);
				CRect rcChild(pChild->GetRect());
				rcChild.OffsetRect(-rcChild.left, -rcChild.top);
				pChild->Paint(hDCMem, rcChild, rctUpdate);
				for (j=i+1; j<nChildCount; j++)
				{
					IKaosChildSystem* pOther = m_childList[j];
					CRect rcIntersect;
					if (::IntersectRect(&rcIntersect, &rcOld, &pOther->GetRect()))
					{
						// paint it
						// set the view port to virtualize the coordinates
						::SetViewportOrgEx(hDCMem, pOther->GetRect().left - rctUpdate.left,
							pOther->GetRect().top - rctUpdate.top, NULL);
						CRect rcChild(pOther->GetRect());
						rcChild.OffsetRect(-rcChild.left, -rcChild.top);
						::IntersectClipRect(hDCMem, rcChild.left, rcChild.top, rcChild.right, rcChild.bottom);
						pOther->Paint(hDCMem, rcChild, rctUpdate);
					}
				}

				::SetViewportOrgEx(hDCMem, 0, 0, NULL);
				::BitBlt(hDC, rctUpdate.left, rctUpdate.top, rctUpdate.right-rctUpdate.left, rctUpdate.bottom-rctUpdate.top, hDCMem, 0, 0, SRCCOPY);

				::SelectObject(hDCMem, hOldBmp);
				::DeleteObject(hBmp);

				pT->EndPaint(&ps);
			}
		}
		for (i=0; i<nChildCount; i++)
		{
			IKaosChildSystem* pChild = m_childList[i];
			pChild->SetDirty(false);
		}
		pT->ReleaseDC(hDCDisplay);
		::DeleteDC(hDCMem);
		pT->ValidateRect(&rctClient);
	}
	void SimplePaint()
	{
		T* pT = static_cast<T*>(this);
		RECT rctClient;
		pT->GetClientRect(&rctClient);
		HDC hDCDisplay = pT->GetDC();
		CWinDC hDCMem = ::CreateCompatibleDC(hDCDisplay);

		CRect rctUpdate;
		pT->GetUpdateRect(&rctUpdate, FALSE);
		if (rctUpdate.Width() != 0 && rctUpdate.Height() != 0)
		{
			PAINTSTRUCT ps;
			HDC hDC = pT->BeginPaint(&ps);
			// create a dib for the offscreen buffering
			void *pvBits;
			BITMAPINFO bmpInfo;
			bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpInfo.bmiHeader.biWidth = rctUpdate.right-rctUpdate.left;
			bmpInfo.bmiHeader.biHeight = -(rctUpdate.bottom-rctUpdate.top);
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = 32;
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biClrUsed = 0;

			HBITMAP hBmp = ::CreateDIBSection(hDCDisplay, &bmpInfo, DIB_RGB_COLORS, &pvBits, NULL, 0);
			HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDCMem, hBmp);

			int nChildCount = m_childList.GetSize();
			for (int i=0; i<nChildCount; i++)
			{
				IKaosChildSystem* pChild = m_childList[i];
				CRect rctIntersect;
				if (::IntersectRect(&rctIntersect, rctUpdate, pChild->GetRect()))
				{
					// paint it
					// set the view port to virtualize the coordinates
					::SetViewportOrgEx(hDCMem, pChild->GetRect().left - rctUpdate.left,
						pChild->GetRect().top - rctUpdate.top, NULL);
					CRect rcChild(pChild->GetRect());
					rcChild.OffsetRect(-rcChild.left, -rcChild.top);
					pChild->Paint(hDCMem, rcChild, rctUpdate);
					pChild->SetDirty(false);
				}
			}
			::SetViewportOrgEx(hDCMem, 0, 0, NULL);
			::BitBlt(hDC, rctUpdate.left, rctUpdate.top, rctUpdate.right-rctUpdate.left, rctUpdate.bottom-rctUpdate.top, hDCMem, 0, 0, SRCCOPY);

			::SelectObject(hDCMem, hOldBmp);
			::DeleteObject(hBmp);

			pT->EndPaint(&ps);
		}
		pT->ReleaseDC(hDCDisplay);
		::DeleteDC(hDCMem);
		pT->ValidateRect(&rctClient);
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (IsDirty())
			SimplePaint();
		else
			OptimizedPaint();
		SetDirty(true);
		return 0;
	}

	void InvalidateParents()
	{
	}

	void Repaint()
	{
		SetDirty(false);
		T* pT = static_cast<T*>(this);
		pT->Invalidate();
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T* pT = static_cast<T*>(this);
		CRect rctClient;
		pT->GetClientRect(&rctClient);
		for (int i=0; i<m_childList.GetSize(); i++)
		{
			IKaosChildSystem* pChild = m_childList[i];
			pChild->ParentResize(rctClient);
		}
		return 0;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CPoint pt(LOWORD(lParam), HIWORD(lParam));
		IKaosChildSystem* pChild;
		if (m_pCapture)
			pChild = m_pCapture;
		else
			pChild = WindowFromPoint(pt);
		if (pChild)
		{
			CPoint ptChild(pt.x - GetRect().left, pt.y - GetRect().top);
			pChild->MouseMessage(uMsg, 0, ptChild);
			return 0;
		}
		T* pT = static_cast<T*>(this);
		return pT->DefWindowProc();
	}

	LRESULT OnKeyMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		IKaosChildSystem *pFocus = GetFocus();
		if (pFocus)
			pFocus->ProcessMessage(uMsg, wParam, lParam);
		T* pT = static_cast<T*>(this);
		return pT->DefWindowProc();
	}

	LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
#if 0
		// review what is capture
		if (m_pCapture)
			m_pCapture->OnCaptureChanged(false);
		m_pWndCapture = NULL;
#endif
		return 0;
	}

	IKaosChildSystem* GetFocus()
	{
		return m_pFocus;
	}

	IKaosChildSystem* SetMouseCapture(IKaosChildSystem* pChild)
	{
		T* pT = static_cast<T*>(this);
		if (m_pCapture)
			ReleaseCapture();
		if (pChild)
			pT->SetCapture();
		IKaosChildSystem* pCapture = m_pCapture;
		m_pCapture = pChild;
		return pCapture;
	}
	void ReleaseMouseCapture()
	{
		if (m_pCapture)
			ReleaseCapture();
		m_pCapture = NULL;
	}
	IKaosChildSystem *GetMouseCapture()
	{
		return m_pCapture;
	}
};

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CKaosWindowImpl : public CWindowImpl<CKaosWindowImpl<T, TBase, TWinTraits> >, public CKaosImpl<CKaosWindowImpl<T, TBase, TWinTraits> >
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, -1)

	typedef CKaosWindowImpl<T, TBase, TWinTraits>			thisClass;
	typedef CKaosImpl<CKaosWindowImpl<T, TBase, TWinTraits> >	baseClass;
	BEGIN_MSG_MAP(thisClass)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()
};

class CKaosWindow : public CKaosWindowImpl<CKaosWindow>
{
public:
// nothing here
};

}; //namespace ATL

#endif // __ATLKAOS_H__
