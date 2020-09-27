#include "stdafx.h"
#include "bitmap.h"
//#include "globfunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
// CBitmapDataImage

CBitmapDataImage::CBitmapDataImage() :
    CImage(),
    m_nPlanes(0),
    m_nBitPerPel(0),
    m_pData(NULL),
    m_lDataSize(0)
{
}

CBitmapDataImage::~CBitmapDataImage()
{
    delete m_pData;
}

BOOL CBitmapDataImage::InitData(
    int   nWidth,
    int   nHeight,
    int   nPlanes, 
    int   nBitPerPel, 
    void* pData,
    LONG  lDataSize)
{
    m_sizeImage.cx = nWidth;
    m_sizeImage.cy = nHeight;
    m_nPlanes      = nPlanes;
    m_nBitPerPel   = nBitPerPel;
    m_pData        = pData;
    m_lDataSize    = lDataSize;

    return TRUE;
}

BOOL CBitmapDataImage::DrawImage(
    CDC* pcDC,
    int  x,
    int  y,
    BOOL bForeBackGround)
{
    CBitmap cBitmap, *pcOldBitmap;
    CRect   rectImage, rectIntersect;
    CDC     cMemDC;

    rectImage.SetRect(0, 0,  m_sizeImage.cx, m_sizeImage.cy);
    rectImage.OffsetRect(x, y);
    if (m_fHaveBounding) {
        if (!rectIntersect.IntersectRect(m_rectBounding, rectImage)) {
            return FALSE;
        }
    } else {
        rectIntersect = rectImage;
    }

    cMemDC.CreateCompatibleDC(pcDC);

    if (!cBitmap.CreateBitmap(m_sizeImage.cx, m_sizeImage.cy, m_nPlanes, 
        m_nBitPerPel, m_pData)) {
        return FALSE;
    }

    pcOldBitmap = (CBitmap*)cMemDC.SelectObject(&cBitmap);

    pcDC->BitBlt(rectIntersect.left, rectIntersect.top, 
        rectIntersect.Width(), rectIntersect.Height(), 
        &cMemDC, 
        rectIntersect.left - rectImage.left, 
        rectIntersect.top  - rectImage.top, SRCCOPY);

    cMemDC.SelectObject(pcOldBitmap);

    return TRUE;
}

/////////////////////////////////////////////////////
// CBitmapResourceImage

CBitmapResourceImage::CBitmapResourceImage() :
    CImage()
{
}

CBitmapResourceImage::~CBitmapResourceImage()
{
}

BOOL CBitmapResourceImage::InitData(
    INT iResourceID)
{
    BITMAP  sBitmapInfo;

    m_cBitmapData.DeleteObject();
    if (!m_cBitmapData.LoadBitmap(iResourceID)) {
        return FALSE;
    }
    if (!m_cBitmapData.GetObject(sizeof(BITMAP), &sBitmapInfo)) {
        return FALSE;
    }
    m_sizeImage.cx = sBitmapInfo.bmWidth;
    m_sizeImage.cy = sBitmapInfo.bmHeight;
    return TRUE;
}

BOOL CBitmapResourceImage::DrawImage(
    CDC* pcDC,
    int  x,
    int  y,
    BOOL bForeBackGround)
{
    CBitmap *pcOldBitmap;
    CRect   rectImage, rectIntersect;
    CDC     cMemDC;

    rectImage.SetRect(0, 0,  m_sizeImage.cx, m_sizeImage.cy);
    rectImage.OffsetRect(x, y);
    if (m_fHaveBounding) {
        if (!rectIntersect.IntersectRect(m_rectBounding, rectImage)) {
            return FALSE;
        }
    } else {
        rectIntersect = rectImage;
    }

    cMemDC.CreateCompatibleDC(pcDC);

    pcOldBitmap = (CBitmap*)cMemDC.SelectObject(&m_cBitmapData);

    pcDC->BitBlt(rectIntersect.left, rectIntersect.top, 
        rectIntersect.Width(), rectIntersect.Height(), 
        &cMemDC, 
        rectIntersect.left - rectImage.left, 
        rectIntersect.top  - rectImage.top, SRCCOPY);

    cMemDC.SelectObject(pcOldBitmap);

    return TRUE;
}

/////////////////////////////////////////////////////
// CBitmapFileImage

CBitmapFileImage::CBitmapFileImage() : 
    CImage(),
    m_pDIB(NULL),
    m_hDrawDib(NULL)
{
}

CBitmapFileImage::~CBitmapFileImage()
{
    Close();
}

void CBitmapFileImage::Close(void)
{
    if (m_hDrawDib) {
        DrawDibClose(m_hDrawDib);
        m_hDrawDib = NULL;
    }
    if (m_pDIB) {
        delete m_pDIB;
        m_pDIB = NULL;
    }
}

BOOL CBitmapFileImage::InitData(
    LPCSTR lpcszFile)
{
    BITMAPFILEHEADER  bmpfh;
    BITMAPINFOHEADER* pDib;
    CFile             cFile;
    INT               nHeaderSize;

    Close();

    //DrawDibOpen initializes the DrawDib library and
    //returns a handle for all DrawDib operations
    if (!(m_hDrawDib = DrawDibOpen())) {
        goto fail;
    }

    //Open and read the Dib file header
    nHeaderSize = sizeof(BITMAPFILEHEADER);

    if (!cFile.Open(lpcszFile, CFile::modeRead | CFile::typeBinary)) {
        goto fail;
    }

    if (!(cFile.Read((void*)&bmpfh, nHeaderSize) == (UINT)nHeaderSize) ||
        !(bmpfh.bfType == *((WORD*)"BM"))) {
        goto failFile;
    }

    m_pDIB = new BYTE[bmpfh.bfSize - nHeaderSize];
    if (!m_pDIB) {
        goto fail;
    }

    cFile.ReadHuge(m_pDIB, bmpfh.bfSize - nHeaderSize);

    pDib = (BITMAPINFOHEADER*)m_pDIB;
    if (pDib->biSizeImage == 0) {
        pDib->biSizeImage = 
            ((((pDib->biWidth * pDib->biBitCount)+31)&~31)>>3)*pDib->biHeight;
    }

    m_sizeImage.cx = pDib->biWidth;
    m_sizeImage.cy = pDib->biHeight;

    cFile.Close();

    return TRUE;
failFile:
    cFile.Close();
fail:
    Close();

    return FALSE;
}

BOOL CBitmapFileImage::DrawImage(
    CDC* pcDC,
    int  x,
    int  y,
    BOOL bForeBackGround)
{
    BITMAPINFOHEADER* pDib;
    CPalette          cPalette, *pOldPalette;

    if (!m_pDIB)
    {
        return FALSE;
    }

    ASSERT(m_hDrawDib != NULL);

    pDib   = (BITMAPINFOHEADER*)m_pDIB;    
    
    CPalette* pPalette = CreatePalette();
    if (pPalette) {
        pOldPalette = pcDC->SelectPalette(&cPalette, bForeBackGround);
    }

    DrawDibRealize(m_hDrawDib, pcDC->GetSafeHdc(), bForeBackGround);

    DrawDibDraw(m_hDrawDib, pcDC->GetSafeHdc(),
        x,
        y, 
        m_sizeImage.cx,
        m_sizeImage.cy,
        (BITMAPINFOHEADER*)m_pDIB,
        (LPVOID)GetBits(),
        0, 
        0, 
        m_sizeImage.cx,
        m_sizeImage.cy,
        DDF_BACKGROUNDPAL);

    if (pPalette) {
        pcDC->SelectPalette(pOldPalette, bForeBackGround);
        delete pPalette;
    }

    return TRUE;
}

LPBYTE CBitmapFileImage::GetBits(void)
{
    BITMAPINFOHEADER* pDib;
    DWORD             dwNumColors, dwColorTableSize;
    WORD              cwBits;

    pDib   = (BITMAPINFOHEADER*)m_pDIB;
    cwBits = pDib->biBitCount;

    dwNumColors = pDib->biSize >= 36 ? dwNumColors = pDib->biClrUsed : 0;
       
    if (!dwNumColors) {
        dwNumColors = cwBits != 24 ? 1L << cwBits : 0;
    }

    dwColorTableSize = dwNumColors * sizeof(RGBQUAD);

    return m_pDIB + pDib->biSize + dwColorTableSize;
}

DWORD CBitmapFileImage::GetColorNum(void)
{
    BITMAPINFOHEADER* pDib;
    DWORD             dwNumColors;
    WORD              cwBits;

    pDib   = (BITMAPINFOHEADER*)m_pDIB;
    cwBits = pDib->biBitCount;

    if (cwBits == 24) {
        return NULL;
    }

    dwNumColors = pDib->biSize >= 36 ? dwNumColors = pDib->biClrUsed : 0;
       
    if (!dwNumColors) {
        dwNumColors = cwBits != 24 ? 1L << cwBits : 0;
    }

    return dwNumColors;
}

CPalette* CBitmapFileImage::CreatePalette(void)
{
    LOGPALETTE* pLogPalette;
    BITMAPINFO* psInfo;
    DWORD       i, dwColorNum;

    dwColorNum = GetColorNum();
    if (!dwColorNum) {
        return NULL;
    }

    psInfo = (BITMAPINFO*)m_pDIB;
    CPalette* pPalette = new CPalette;
    if (!pPalette) {
        return NULL;
    }

    pLogPalette = (LOGPALETTE*)new BYTE[sizeof(LOGPALETTE) + dwColorNum * sizeof(PALETTEENTRY)];
    if (!pLogPalette) {
        delete pPalette;
        return NULL;
    }

    for (i = 0; i < dwColorNum; i++) {
        pLogPalette->palPalEntry[i].peRed   = psInfo->bmiColors[i].rgbRed;
        pLogPalette->palPalEntry[i].peGreen = psInfo->bmiColors[i].rgbGreen;
        pLogPalette->palPalEntry[i].peBlue  = psInfo->bmiColors[i].rgbBlue;
        pLogPalette->palPalEntry[i].peFlags = 0;
    }

    pLogPalette->palVersion    = PALVERSION;
    pLogPalette->palNumEntries = (WORD)dwColorNum;
    pPalette->CreatePalette(pLogPalette);

    delete pLogPalette;

    return pPalette;
}