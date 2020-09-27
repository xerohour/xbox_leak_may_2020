#ifndef  __BITMAP_DECLARE__
#define  __BITMAP_DECLARE__

#include "image.h"
#include <vfw.h>
#pragma comment (lib, "vfw32.lib")

#define PALVERSION 0x300

class CBitmapDataImage : public CImage 
{
public:
    CBitmapDataImage();
   ~CBitmapDataImage();

public:
    BOOL InitData(int nWidth, int nHeight, int nPlandes, int nBitPerPel, void*, LONG);

public:
    BOOL DrawImage(CDC* pcDC, int x, int y, BOOL bForeBackGround);

private:
    int   m_nPlanes;
    int   m_nBitPerPel;
    void* m_pData;
    LONG  m_lDataSize;
};

class CBitmapResourceImage : public CImage 
{
public:
    CBitmapResourceImage();
   ~CBitmapResourceImage();

public:
    BOOL InitData(int nResourceID);

public:
    BOOL DrawImage(CDC* pcDC, int x, int y, BOOL bForeBackGround);

private:
    CBitmap m_cBitmapData;
};

class CBitmapFileImage : public CImage
{
public:
    CBitmapFileImage();
   ~CBitmapFileImage();

public:
    BOOL InitData(LPCSTR lpctszFile);

public:
    BOOL DrawImage(CDC* pcDC, int x, int y, BOOL bForeBackGround = TRUE);

protected:
    CPalette* CreatePalette(void);
    BYTE*     GetBits(void);
    void      Close(void);
    DWORD     GetColorNum(void);
    
private:
    LPBYTE   m_pDIB;
    HDRAWDIB m_hDrawDib;
};

#endif //__BITMAP_DECLARE__
