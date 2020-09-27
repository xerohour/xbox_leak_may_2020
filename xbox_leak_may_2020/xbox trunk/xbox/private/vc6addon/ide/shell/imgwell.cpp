#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define DSx		0x00660046L
#define DSna	0x00220326L

CImageWell::CImageWell() 
	: m_bitmap(), m_dc(), m_mask(), m_maskDC(), m_imageSize(0, 0)
{
	m_nBitmapID = 0;
	m_pbitmap = NULL;
}


CImageWell::CImageWell(UINT nBitmapID, CSize imageSize) 
	: m_bitmap(), m_dc(), m_mask(), m_maskDC(), m_imageSize(imageSize)
{
	m_nBitmapID = nBitmapID;
	m_pbitmap = NULL;
}


CImageWell::~CImageWell()
{
	// Destructor needed to ensure the DC is deleted before the bitmap
	m_dc.DeleteDC();
	m_bitmap.DeleteObject();
	m_maskDC.DeleteDC();
	m_mask.DeleteObject();
}

void CImageWell::SetBitmap(CBitmap *pBitmap, CSize imageSize) 
{
	m_imageSize =imageSize;
	m_pbitmap = pBitmap;
}


BOOL CImageWell::Load(UINT nBitmapID, CSize imageSize)
{
	if(m_bitmap.m_hObject)
	{
		Unload();
	}

	ASSERT(m_bitmap.m_hObject == NULL);

	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapID), RT_BITMAP); 
	HINSTANCE hOldInst = AfxGetResourceHandle();

	AfxSetResourceHandle(hInst);
	if (!m_bitmap.LoadBitmap(nBitmapID))
	{
		TRACE1("Could not load image well %d\n", nBitmapID);
		return FALSE;
	}
	AfxSetResourceHandle(hOldInst);
	
	m_nBitmapID = nBitmapID;
	m_imageSize = imageSize;

	m_pbitmap = &m_bitmap;	
	return TRUE;
}

/* LoadSysColor
 *	Loads the bitmap containing the image well using AfxLoadSysColorBitmap,
 *	which will convert black, grays and white to the current system colors for buttons.
 */
BOOL CImageWell::LoadSysColor(UINT nBitmapID, CSize imageSize)
{
	ASSERT(m_bitmap.m_hObject == NULL);

	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapID), RT_BITMAP); 
	HINSTANCE hOldInst = AfxGetResourceHandle();

	AfxSetResourceHandle(hInst);

	HRSRC hRes = ::FindResource(hInst, MAKEINTRESOURCE(nBitmapID), RT_BITMAP);
	HBITMAP hBitmap = AfxLoadSysColorBitmap(hInst, hRes);
	if( hBitmap == NULL )
	{
		TRACE1("Could not load image well %d\n", nBitmapID);
		return FALSE;
	}

	AfxSetResourceHandle(hOldInst);

	m_nBitmapID = 0;
	m_imageSize = imageSize;
	m_bitmap.Attach(hBitmap);
	m_pbitmap = &m_bitmap;
	return TRUE;
}

BOOL CImageWell::Attach(HBITMAP hBitmap, CSize imageSize)
{
	ASSERT(m_bitmap.m_hObject == NULL);
	ASSERT(hBitmap != NULL);

	if (!m_bitmap.Attach(hBitmap))
	{
		TRACE0("Could not attach image well\n");
		return FALSE;
	}

	m_nBitmapID = 0;
	m_imageSize = imageSize;

	m_pbitmap = &m_bitmap;	
	return TRUE;
}

HBITMAP CImageWell::Detach()
{
	m_pbitmap = NULL;
	m_nBitmapID = 0;

	return (HBITMAP)m_bitmap.Detach();
}

void CImageWell::Unload()
{
	ASSERT(m_dc.m_hDC == NULL); // can't unload if it's open!
	ASSERT(m_maskDC.m_hDC == NULL);
	ASSERT(m_bitmap.m_hObject != NULL);
	
	m_bitmap.DeleteObject();
	m_mask.DeleteObject();
}


BOOL CImageWell::CalculateMask()
{
	ASSERT(m_maskDC.m_hDC == NULL);
	ASSERT(m_dc.m_hDC != NULL);
	ASSERT(m_pbitmap->m_hObject != NULL);
	
	if (!m_maskDC.CreateCompatibleDC(NULL))
		return FALSE;
	
	if (m_mask.m_hObject != NULL)
	{
		VERIFY(m_maskDC.SelectObject(&m_mask) != NULL);
		return TRUE;
	}
		
	BITMAP bmp;
	m_pbitmap->GetObject(sizeof (BITMAP), &bmp);
	
	if (!m_mask.CreateBitmap(bmp.bmWidth, bmp.bmHeight, 1, 1, NULL))
	{
		m_maskDC.DeleteDC();
		return FALSE;
	}

	VERIFY( m_maskDC.SelectObject(&m_mask) != NULL);
	
	COLORREF oldBkColor = m_dc.SetBkColor(m_dc.GetPixel(0, 0));
	m_maskDC.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, 
		&m_dc, 0, 0, NOTSRCCOPY);
	m_dc.SetBkColor(oldBkColor);
	return TRUE;
}


BOOL CImageWell::Open()
{
	ASSERT(m_dc.m_hDC == NULL); // make sure this is not already open
	ASSERT(m_nBitmapID != 0 || m_pbitmap != NULL);
		
	if (m_pbitmap == NULL && !Load(m_nBitmapID, m_imageSize))
		return FALSE;
	
	if (!m_dc.CreateCompatibleDC(NULL))
		return FALSE;
	
	VERIFY(m_dc.SelectObject(m_pbitmap));
	
	return TRUE;
}


void CImageWell::Close()
{
	ASSERT(m_dc.m_hDC != NULL);
	
	m_dc.DeleteDC();
	m_maskDC.DeleteDC();
}

BOOL CImageWell::DrawImage(CDC* pDestDC, CPoint destPoint, 
	UINT nImage, DWORD rop)
{
	BOOL bClose = FALSE;
	if (m_dc.m_hDC == NULL)
	{
		ASSERT(rop != NULL); // must open first and calc mask for rop==0!
		
		if (!Open())
		{
			TRACE("Could not open image well!\n");
			return FALSE;
		}
		
		bClose = TRUE;
	}
	
	if (rop == 0)
	{
		if (m_maskDC.m_hDC == NULL && !CalculateMask())
			return FALSE;
		
		COLORREF oldBkColor = pDestDC->SetBkColor(RGB(255, 255, 255));
		COLORREF oldTextColor = pDestDC->SetTextColor(RGB(0, 0, 0));

		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			m_imageSize.cx, m_imageSize.cy,
			&m_dc, m_imageSize.cx * nImage, 0, DSx);
		
		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			m_imageSize.cx, m_imageSize.cy,
			&m_maskDC, m_imageSize.cx * nImage, 0, DSna);
		
		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			m_imageSize.cx, m_imageSize.cy,
			&m_dc, m_imageSize.cx * nImage, 0, DSx);
		pDestDC->SetBkColor(oldBkColor);
		pDestDC->SetTextColor(oldTextColor);
	}
	else
	{
		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			m_imageSize.cx, m_imageSize.cy,
			&m_dc, m_imageSize.cx * nImage, 0, rop);
	}
	
	if (bClose)
		Close();
	
	return TRUE;
}

BOOL CImageWell::DrawImage(CDC* pDestDC, CPoint destPoint, 
	UINT nImage, CSize sizeOverride, DWORD rop)
{
	BOOL bClose = FALSE;
	if (m_dc.m_hDC == NULL)
	{
		ASSERT(rop != NULL); // must open first and calc mask for rop==0!
		
		if (!Open())
		{
			TRACE("Could not open image well!\n");
			return FALSE;
		}
		
		bClose = TRUE;
	}
	
	if (rop == 0)
	{
		if (m_maskDC.m_hDC == NULL && !CalculateMask())
			return FALSE;
		
		COLORREF oldBkColor = pDestDC->SetBkColor(RGB(255, 255, 255));
		COLORREF oldTextColor = pDestDC->SetTextColor(RGB(0, 0, 0));

		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			sizeOverride.cx, sizeOverride.cy,
			&m_dc, m_imageSize.cx * nImage, 0, DSx);
		
		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			sizeOverride.cx, sizeOverride.cy,
			&m_maskDC, m_imageSize.cx * nImage, 0, DSna);
		
		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			sizeOverride.cx, sizeOverride.cy,
			&m_dc, m_imageSize.cx * nImage, 0, DSx);
		pDestDC->SetBkColor(oldBkColor);
		pDestDC->SetTextColor(oldTextColor);
	}
	else
	{
		pDestDC->BitBlt(destPoint.x, destPoint.y, 
			sizeOverride.cx, sizeOverride.cy,
			&m_dc, m_imageSize.cx * nImage, 0, rop);
	}
	
	if (bClose)
		Close();
	
	return TRUE;
}

BOOL CImageWell::DrawMask(CDC* pDestDC, CPoint destPoint, 
	UINT nImage, DWORD rop)
{
	ASSERT(m_maskDC.m_hDC != NULL); // must open first and calc mask
	
	pDestDC->BitBlt(destPoint.x, destPoint.y, 
		m_imageSize.cx, m_imageSize.cy,
		&m_maskDC, m_imageSize.cx * nImage, 0, rop);
	
	return TRUE;
}
