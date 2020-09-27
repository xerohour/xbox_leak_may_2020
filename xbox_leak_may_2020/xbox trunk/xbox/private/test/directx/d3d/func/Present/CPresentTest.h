//
//  CPresentTest.h
//
//  Class specification for CPresentTest.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//

#ifndef __CPresentTest_H__
#define __CPresentTest_H__


#define TESTDXVER 0x0800

//#define STRICT

//USETESTFRAME

//
// fun macros
//


#define COUNTOF(a) (sizeof(a)/sizeof((a)[0]))
#define DecodeHResult(hr) TEXT("") //(GetHResultString(hr).c_str())

//#define GROUP_CREATE(GroupName)  \
//GroupName Class##GroupName



typedef struct __RECT_SCALE
{
    FLOAT left;
    FLOAT top;
    FLOAT right;
    FLOAT bottom;
}
RECT_SCALE;

typedef struct __D3DTEST_RECT
{
    LPTSTR szName;
    BOOL bScale;
    const RECT_SCALE *pRectScale;
    const RECT *pRect;
}
D3DTEST_RECT;

typedef struct __D3DTEST_PRESENT
{
    LPTSTR szName;
    BOOL bScaleSrc;
    const RECT_SCALE *pSrcRectScale;
    const RECT *pSrcRect;
    BOOL bScaleDest;
    const RECT_SCALE *pDestRectScale;
    const RECT *pDestRect;
    BOOL bDestWndOverride;
    HWND hDestWindowOverride;
//    RGNDATA *pDirtyRegion;
}
D3DTEST_PRESENT;


typedef union __MYCOLOR
{
    struct
    {
        BYTE blue;
        BYTE green;
        BYTE red;
        BYTE alpha;
    };
    DWORD color;
}
MYCOLOR;


//
//  The CDevice8Test specification
//


class CPresentTest : public CD3DTest
{
private:
//    LPDIRECT3DTEXTURE8 m_pTex;
//    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    CTexture8* m_pTex;
    CVertexBuffer8* m_pVB;

//    HWND m_hWnd;
//    HWND m_hWndOverride;
//    HWND m_hWndFramework;
//    COLORREF m_FBColor;
    D3DCOLOR m_FBColor;

    UINT m_ScreenWidth;
    UINT m_ScreenHeight;

    UINT m_SrcWidth;
    UINT m_SrcHeight;
    UINT m_DestWidth;
    UINT m_DestHeight;

    const RECT *m_pSrcRect;
    const RECT *m_pDestRect;
//    HWND m_hDestWnd;
//    RGNDATA *m_pDirtyRegion;

public:

    CPresentTest();

    UINT TestInitialize();
    bool TestTerminate();
    bool ExecuteTest(UINT nTest);
    bool DisplayFrame(void);
    bool ProcessFrame(void);
    BOOL ValidateRects( const RECT *pSrc, const RECT *pDest );
    bool CompareRangePixel( MYCOLOR SrcMin, MYCOLOR SrcMax, MYCOLOR Dest, D3DFORMAT Format, MYCOLOR *Diff );
    bool CompareSurfaces( D3DLOCKED_RECT *pSrcLocked, D3DLOCKED_RECT *pDestLocked, D3DFORMAT Format );
};



#endif

