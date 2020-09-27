/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    conio.cpp

Author:

    Matt Bronder

Description:

    Console and text output functions.

*******************************************************************************/

#define DXCONIO_NO_OVERRIDES

#include "d3dbase.h"
#include "fontmap.h"
#include "conio.h"
#ifdef UNDER_XBOX
#include <xboxverp.h>
#endif

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define TAB_LENGTH          8

#define BORDER_LEFT         10.0f
#define BORDER_RIGHT        10.0f
#define BORDER_TOP          10.0f
#define BORDER_BOTTOM       10.0f

#define SHADOW_OFFSET_X     4.0f
#define SHADOW_OFFSET_Y     4.0f
#define SHADOW_INTENSITY    191.0f

#define BACKTEX_WIDTH       256
#define BACKTEX_HEIGHT      256

#define MAX_TEXTOUT_LENGTH  256

#define POS_BLANK           -3.402823466e+36F

//******************************************************************************
// Globals
//******************************************************************************

LPDIRECT3DDEVICE8           g_pDevice = NULL;

// Console text output buffer
LPSTR                       g_pchOut = NULL;
UINT                        g_uOutSize = 4096;
UINT                        g_uOutHead = 0;
UINT                        g_uOutTail = 0;
UINT                        g_uOutWidth = 80;

// Console text vertex layout
UINT                        g_uTextSize = 0;
UINT                        g_uTextHead = 0;
UINT                        g_uTextTail = 0;
UINT                        g_uTextLine = 0;
UINT                        g_uTextHeight = 1;
UINT                        g_uTextCol = 0;
UINT                        g_uTextCursor = 0;
float                       g_fTextX;

// Visible console text vertices
UINT                        g_uVStart1;
UINT                        g_uVStart2;
UINT                        g_uVCount1;
UINT                        g_uVCount2;

// xTextOut vertices
PTLVERTEX                   g_prTextOut = NULL;
LPWORD                      g_pwTextOut = NULL;

// Console vertices
LPDIRECT3DVERTEXBUFFER8     g_pd3drPosition = NULL;
UINT                        g_uVBPositionSize;
LPDIRECT3DVERTEXBUFFER8     g_pd3drTexCoord = NULL;
UINT                        g_uVBTexCoordSize;
LPDIRECT3DINDEXBUFFER8      g_pd3diText = NULL;
UINT                        g_uIBTextSize;
// ##DELETE
LPDIRECT3DVERTEXBUFFER8     g_pd3drDeleteMe = NULL;
UINT                        g_uVBDeleteMeSize;
// ##END DELETE

// Console background
LPDIRECT3DVERTEXBUFFER8     g_pd3drBack = NULL;
LPDIRECT3DTEXTURE8          g_pd3dtBackBase = NULL;
LPDIRECT3DTEXTURE8          g_pd3dtBackBump = NULL;
LPDIRECT3DTEXTURE8          g_pd3dtBackEnv = NULL;
CPerlinNoise2D*             g_pNoise = NULL;

// Console state
float                       g_fCharWidth = 8.0f;
float                       g_fCharHeight = 15.0f; //(CHAR_ASPECT * g_fCharWidth);
DWORD                       g_dwTextColor = 0xFFFFFFFF;
//DWORD                       g_dwBackgroundColor = 0xBFFFFF00;
DWORD                       g_dwBackgroundColor = 0xBF7F7F00;
//DWORD                       g_dwBackgroundColor = 0xBF44FF44;
TCHAR                       g_szBackImage[MAX_PATH] = {0};
BOOL                        g_bUseBackground = TRUE;
BOOL                        g_bInScene = FALSE;
BOOL                        g_bDoConsoleUpdate = TRUE;

// Device information
float                       g_fTargetWidth;
float                       g_fTargetHeight;
ADAPTERDESC                 g_adpd;
DEVICEDESC                  g_devd;
DISPLAYMODE                 g_dm;

// Console scrolling
D3DVECTOR                   g_vTranslate;

float                       g_fMaxRowScroll = 0.0f;
float                       g_fMaxColScroll = 0.0f;
float                       g_fCurrentRow = 0.0f;
float                       g_fCurrentCol = 0.0f;

// Serial access to dxconio functions
CRITICAL_SECTION            g_csConsoleLock;

BOOL                        g_bInitialized = FALSE;

//******************************************************************************
// Function prototypes
//******************************************************************************

void                        PutStringA(LPCSTR pszString);
void                        PutStringW(LPCWSTR pszString);

BOOL                        InitConsole();
void                        EffaceConsole();

static void                 UpdateConsole(UINT uStart);
static void                 RefreshConsole();
static void                 DrawConsole();

static BOOL                 UpdateVisibility();

static BOOL                 CreateGeometry(UINT uNumChars);
static void                 ReleaseGeometry();

static BOOL                 SetBackgroundImage(LPCTSTR pszImage);

static void                 UpdateConsoleScrollY();

static BOOL                 BeginConsoleScene();
static void                 EndConsoleScene();
static void                 DrawConsoleBackground();
static void                 DrawConsoleText();

static void                 DrawTextQuads(LPCSTR szText, UINT uCount, float fX, 
                                float fY, float fHeight, float fWidth, 
                                D3DCOLOR c, float fLeftMargin);

BOOL                        ValidateDisplay(LPDIRECT3D8 pd3d, 
                                    D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm);

inline void                 LockConsole() { EnterCriticalSection(&g_csConsoleLock); }
inline void                 UnlockConsole() { LeaveCriticalSection(&g_csConsoleLock); }

//******************************************************************************
// Console management functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     LoadConsole
//
// Description:
//
//     Create a heap for the console and allocate a buffer to contain the
//     output stream.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL LoadConsole() {

#ifndef UNDER_XBOX
    OSVERSIONINFO osvi;
#else
//    extern PULONG NtBuildVersion
#endif

    // Initialize the critical section
    InitializeCriticalSection(&g_csConsoleLock);

    g_uOutHead = 0;
    g_uOutTail = 0;

    // Allocate a buffer for the output stream
    g_pchOut = (LPSTR)MemAlloc(g_uOutSize);
    if (!g_pchOut) {
        xReleaseConsole();
        return FALSE;
    }

#ifndef UNDER_XBOX
    memset(&osvi, 0, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

    xprintf("Microsoft Windows 2000 [Version %d.%02d.%04d]\n", 
            osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
    xputs("(C) Copyright 1985-1999 Microsoft Corp.\n");
#else
//    xprintf("Microsoft Xbox OS [Version %08x]\n", *NtBuildVersion);
    xprintf("Microsoft Xbox OS [Version %02d.%02d.%04d.%02d]\n", VER_PRODUCTVERSION);
    xputs("(C) Copyright 1999-2000 Microsoft Corp.\n");
#endif // UNDER_XBOX

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     UnloadConsole
//
// Description:
//
//     Free the console output buffer and release the heap.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void UnloadConsole() {

    if (g_pchOut) {
        MemFree(g_pchOut);
        g_pchOut = NULL;
    }

    // Release the critical section
    DeleteCriticalSection(&g_csConsoleLock);
}

//******************************************************************************
//
// Function:
//
//     xCreateConsole
//
// Description:
//
//     Create all Direct3D and related objects necessary for rendering the
//     console output stream to the screen.
//
// Arguments:
//
//     LPVOID pvDevice                - Pointer to the device object
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xCreateConsole(LPVOID pvDevice) {

    USHORT i, j, k;

    if (!pvDevice) {
        return FALSE;
    }

    LockConsole();

    xReleaseConsole();

    g_pDevice = (LPDIRECT3DDEVICE8)pvDevice;

    // Create a perlin noise object
    g_pNoise = new CPerlinNoise2D(0.5f, 5, GetTickCount(), INTERPOLATE_COSINE, FALSE, FALSE);
    if (!g_pNoise) {
        UnlockConsole();
        return FALSE;
    }

    g_uTextHead = 0;
    g_uTextTail = 0;
    g_uTextLine = 0;
    g_uTextHeight = 1;
    g_uTextCol = 0;
    g_uTextCursor = 0;
    g_fTextX = 0.0f;

    // Initialize the device objects for the console
    if (!InitConsole()) {
        xReleaseConsole();
        UnlockConsole();
        return FALSE;
    }

    // Allocate memory for the xTextOut vertices
    g_prTextOut = (PTLVERTEX)MemAlloc32(MAX_TEXTOUT_LENGTH * 2 * 4 * sizeof(TLVERTEX));
    if (!g_prTextOut) {
        xReleaseConsole();
        UnlockConsole();
        return FALSE;
    }
    memset(g_prTextOut, 0, MAX_TEXTOUT_LENGTH * 2 * 4 * sizeof(TLVERTEX));
    for (i = 0; i < MAX_TEXTOUT_LENGTH * 2 * 4; i++) {
        g_prTextOut[i].vPosition.z = 0.00001f;
        g_prTextOut[i].fRHW = 100000.0f;
    }

    // Allocate memory for the xTextOut indices
    g_pwTextOut = (LPWORD)MemAlloc32(MAX_TEXTOUT_LENGTH * 6 * sizeof(WORD));
    if (!g_pwTextOut) {
        xReleaseConsole();
        UnlockConsole();
        return FALSE;
    }

    // Initialize the xTextOut indices
    for (i = 0, j = 0, k = 0; i < MAX_TEXTOUT_LENGTH; i++, j += 4, k += 6) {
        g_pwTextOut[k]   = j;
        g_pwTextOut[k+1] = j + 2;
        g_pwTextOut[k+2] = j + 1;
        g_pwTextOut[k+3] = j + 1;
        g_pwTextOut[k+4] = j + 2;
        g_pwTextOut[k+5] = j + 3;
    }

    g_bInitialized = TRUE;

    RefreshConsole();

    UnlockConsole();

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     xReleaseConsole
//
// Description:
//
//     Release all Direct3D and related objects created by the console.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xReleaseConsole() {

    if (g_bInitialized) {

        LockConsole();

        EffaceConsole();

        if (g_pNoise) {
            delete g_pNoise;
            g_pNoise = NULL;
        }

        g_bInitialized = FALSE;

        UnlockConsole();
    }
}

//******************************************************************************
//
// Function:
//
//     xClearScreen
//
// Description:
//
//     Clear the console output stream buffer of its contents.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xClearScreen(void) {

    LockConsole();

    g_uOutHead = 0;
    g_uOutTail = 0;

    if (g_bInitialized) {

        g_fCurrentRow = 0.0f;
        g_fCurrentCol = 0.0f;

        RefreshConsole();
    }

    UnlockConsole();
}

//******************************************************************************
//
// Function:
//
//     xSetCharSize
//
// Description:
//
//     Set the width and height of the characters used in outputting the console
//     text.
//
// Arguments:
//
//     float fWidth             - Width of a console character in pixels
//
//     float fHeight            - Height of a console character in pixels
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xSetCharSize(float fWidth, float fHeight) {

    LockConsole();

    g_fCharWidth = fWidth;
    g_fCharHeight = fHeight;
    if (g_bInitialized) {
        xResetConsole();
    }

    UnlockConsole();
}

//******************************************************************************
//
// Function:
//
//     xSetTextColor
//
// Description:
//
//     Set the color of the console text.
//
// Arguments:
//
//     BYTE red                 - Red intensity of the text color
//
//     BYTE green               - Green intensity of the text color
//
//     BYTE blue                - Blue intensity of the text color
//
//     BYTE alpha               - Alpha intensity of the text color (where
//                                0 indicates transparent and 255 indicates
//                                opaque)
//
// Return Value:
//
//     The old color of the console text.
//
//******************************************************************************
XCOLOR DXCIOAPI xSetTextColor(BYTE red, BYTE green, BYTE blue, BYTE alpha) {

    DWORD dwColor;

    LockConsole();

    dwColor = g_dwTextColor;
    g_dwTextColor = RGBA_MAKE(red, green, blue, alpha);

    UnlockConsole();

    return dwColor;
}

//******************************************************************************
//
// Function:
//
//     xSetBackgroundColor
//
// Description:
//
//     Set the background color of the console.  If a background texture has
//     been specified, the background color will be modulated with it.
//
// Arguments:
//
//     BYTE red                 - Red intensity of the background color
//
//     BYTE green               - Green intensity of the background color
//
//     BYTE blue                - Blue intensity of the background color
//
//     BYTE alpha               - Alpha intensity of the background color (where
//                                0 indicates transparent and 255 indicates
//                                opaque).  This value will be ignored if
//                                the console is managing Direct3D
//
// Return Value:
//
//     The old background color of the console.
//
//******************************************************************************
XCOLOR DXCIOAPI xSetBackgroundColor(BYTE red, BYTE green, BYTE blue, BYTE alpha) {

    PTLBVERTEX prBack;
    DWORD      dwColor;
    UINT       i;
    HRESULT    hr;

    LockConsole();

    dwColor = g_dwBackgroundColor;
    g_dwBackgroundColor = RGBA_MAKE(red, green, blue, alpha);

    if (g_bInitialized) {

        if (g_pd3drBack) {

            hr = g_pd3drBack->Lock(0, 4 * sizeof(TLBVERTEX), (LPBYTE*)&prBack, 0);
            if (SUCCEEDED(hr)) {
                for (i = 0; i < 4; i++) {
                    prBack[i].cDiffuse = g_dwBackgroundColor;
                }
                g_pd3drBack->Unlock();
            }
        }
    }

    UnlockConsole();

    return dwColor;
}

//******************************************************************************
//
// Function:
//
//     xSetBackgroundImageA
//
// Description:
//
//     Set the background texture for the console.
//
// Arguments:
//
//     LPCSTR pszImage          - A string containing the file or resource
//                                name of the image to use as the background
//                                texture.  If set to NULL, the current
//                                background texture will be released and
//                                no background texture will be used
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xSetBackgroundImageA(LPCSTR pszImage) {

    BOOL bRet;

    LockConsole();

    if (!pszImage) {
        bRet = SetBackgroundImage(NULL);
    }
    else {
#ifndef UNICODE
        bRet = SetBackgroundImage(pszImage);
#else
        WCHAR wszImage[MAX_PATH];
        mbstowcs(wszImage, pszImage, MAX_PATH);
        bRet = SetBackgroundImage(wszImage);
#endif // !UNICODE
    }

    UnlockConsole();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     xSetBackgroundImageW
//
// Description:
//
//     Set the background texture for the console.
//
// Arguments:
//
//     LPCWSTR pszImage         - A string containing the file or resource
//                                name of the image to use as the background
//                                texture.  If set to NULL, the current
//                                background texture will be released and
//                                no background texture will be used
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xSetBackgroundImageW(LPCWSTR pszImage) {

    BOOL bRet;

    LockConsole();

    if (!pszImage) {
        bRet = SetBackgroundImage(NULL);
    }
    else {
#ifndef UNICODE
        char szImage[MAX_PATH];
        wcstombs(szImage, pszImage, MAX_PATH);
        bRet = SetBackgroundImage(szImage);
#else
        bRet = SetBackgroundImage(pszImage);
#endif // !UNICODE
    }

    UnlockConsole();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     SetBackgroundImage
//
// Description:
//
//     Set the background texture for the console.
//
// Arguments:
//
//     LPCTSTR pszImage         - A string containing the file or resource
//                                name of the image to use as the background
//                                texture.  If set to NULL, the current
//                                background texture will be released and
//                                no background texture will be used
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetBackgroundImage(LPCTSTR pszImage) {

    LPDIRECT3DTEXTURE8 pd3dt = NULL;

    if (!pszImage) {
        g_bUseBackground = FALSE;
    }
    else {
        _tcscpy(g_szBackImage, pszImage);
    }

    if (!g_bInitialized) {
        return TRUE;
    }

    if (pszImage) {

        pd3dt = (LPDIRECT3DTEXTURE8)CreateTexture(g_pDevice, pszImage, D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
        if (!pd3dt) {
            return FALSE;
        }
        RemoveTexture(pd3dt);
        g_bUseBackground = TRUE;
    }

    if (!pszImage || pd3dt) {
        if (g_pd3dtBackEnv) {
            ReleaseTexture(g_pd3dtBackEnv);
            g_pd3dtBackEnv = NULL;
        }
        if (g_pd3dtBackBump) {
            ReleaseTexture(g_pd3dtBackBump);
            g_pd3dtBackBump = NULL;
        }
        if (g_pd3dtBackBase) {
            ReleaseTexture(g_pd3dtBackBase);
            g_pd3dtBackBase = NULL;
        }
        g_pd3dtBackBase = pd3dt;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     xScrollConsole
//
// Description:
//
//     Scroll the console according to the scroll flag(s) specified.
//
// Arguments:
//
//     DWORD dwScroll           - Scrolling flags that indicate the direction
//                                and amount to scroll the console
//
//     float* pfHorz            - Pointer to a floating point value indicating 
//                                the amount to scroll the console horizontally
//                                in pixels if the XSCROLL_HORZ flag has been 
//                                specified.  On return, this value will contain 
//                                the horizontal amount the console has been 
//                                scrolled.
//
//     float* pfVert            - Pointer to a floating point value indicating 
//                                the amount to scroll the console vertically
//                                in pixels if the XSCROLL_VERT flag has been 
//                                specified.  On return, this value will contain 
//                                the vertical amount the console has been 
//                                scrolled.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xScrollConsole(DWORD dwScroll, float* pfHorz, float* pfVert) {

    float fX = 0.0f, 
          fY = 0.0f;

    if (!g_bInitialized) {
        return;
    }

    LockConsole();

    if (dwScroll & XSCROLL_LINEUP) {
        fY -= 1.0f;
    }
    if (dwScroll & XSCROLL_LINEDOWN) {
        fY += 1.0f;
    }
    if (dwScroll & XSCROLL_LINELEFT) {
        fX -= 1.0f;
    }
    if (dwScroll & XSCROLL_LINERIGHT) {
        fX += 1.0f;
    }
    if (dwScroll & XSCROLL_PAGEUP) {
        fY -= (g_fTargetHeight - BORDER_TOP - BORDER_BOTTOM) / g_fCharHeight - 1.0f;
    }
    if (dwScroll & XSCROLL_PAGEDOWN) {
        fY += (g_fTargetHeight - BORDER_TOP - BORDER_BOTTOM) / g_fCharHeight - 1.0f;
    }
    if (dwScroll & XSCROLL_PAGELEFT) {
        fX -= (g_fTargetWidth - BORDER_LEFT - BORDER_RIGHT) / g_fCharWidth;
    }
    if (dwScroll & XSCROLL_PAGERIGHT) {
        fX += (g_fTargetWidth - BORDER_LEFT - BORDER_RIGHT) / g_fCharWidth;
    }
    if (dwScroll & XSCROLL_HORZ && pfHorz) {
        fX += *pfHorz;
    }
    if (dwScroll & XSCROLL_VERT && pfVert) {
        fY += *pfVert;
    }

    fX = (float)(int)(fX * 100.0f) / 100.0f;
    fY = (float)(int)(fY * 100.0f) / 100.0f;

    if (g_fMaxColScroll > 0.0f) {

        if (dwScroll & XSCROLL_LEFT) {
            g_fCurrentCol = 0.0f;
        }
        else if (dwScroll & XSCROLL_RIGHT) {
            g_fCurrentCol = g_fMaxColScroll;
        }
        else {
            g_fCurrentCol += fX;

            if (g_fCurrentCol < 0.0f) {
                g_fCurrentCol = 0.0f;
            }
            else if (g_fCurrentCol > g_fMaxColScroll) {
                g_fCurrentCol = g_fMaxColScroll;
            }
        }

        g_vTranslate.x = BORDER_LEFT - g_fCurrentCol * g_fCharWidth;
    }

    if (g_fMaxRowScroll > 0.0f) {

        if (dwScroll & XSCROLL_TOP) {
            g_fCurrentRow = g_fMaxRowScroll;
        }
        else if (dwScroll & XSCROLL_BOTTOM) {
            g_fCurrentRow = 0.0f;
        }
        else {
            g_fCurrentRow -= fY;
        }

        UpdateConsoleScrollY();

        UpdateVisibility();
    }

    if (pfHorz) {
        *pfHorz = g_fCurrentCol;
    }
    if (pfVert) {
        *pfVert = g_fMaxRowScroll - g_fCurrentRow;
    }

    UnlockConsole();
}

//******************************************************************************
//
// Function:
//
//     UpdateConsoleScrollY
//
// Description:
//
//     Update the vertical translation value for the console.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void UpdateConsoleScrollY() {

    if (g_fCurrentRow < 0.0f) {
        g_fCurrentRow = 0.0f;
    }
    else if (g_fMaxRowScroll > 0.0f && g_fCurrentRow > g_fMaxRowScroll) {
        g_fCurrentRow = g_fMaxRowScroll;
    }

    if (g_fMaxRowScroll <= 0.0f) {
        g_vTranslate.y = g_fTargetHeight - BORDER_BOTTOM + g_fMaxRowScroll * g_fCharHeight;
    }
    else {
        g_vTranslate.y = g_fTargetHeight - BORDER_BOTTOM + g_fCurrentRow * g_fCharHeight;
    }
}

//******************************************************************************
//
// Function:
//
//     xSetOutputBufferSize
//
// Description:
//
//     Set the size of the console output stream buffer.
//
// Arguments:
//
//     UINT uBytes              - Size of the output stream buffer in bytes
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xSetOutputBufferSize(UINT uBytes) {

    LPDIRECT3DVERTEXBUFFER8 pd3drPos;
    LPDIRECT3DVERTEXBUFFER8 pd3drTC;
    LPDIRECT3DINDEXBUFFER8  pd3diText;
    LPSTR                   pchNewOut;

    LockConsole();

    pchNewOut = (LPSTR)MemAlloc(uBytes);
    if (!pchNewOut) {
        UnlockConsole();
        return FALSE;
    }

    if (g_bInitialized) {

        pd3drPos = g_pd3drPosition;
        pd3drTC = g_pd3drTexCoord;
        pd3diText = g_pd3diText;

        pd3drPos->AddRef();
        pd3drTC->AddRef();
        pd3diText->AddRef();
        ReleaseGeometry();

        if (!CreateGeometry(uBytes)) {
            MemFree(pchNewOut);
            g_pd3drPosition = pd3drPos;
            g_pd3drTexCoord = pd3drTC;
            g_pd3diText = pd3diText;
            UnlockConsole();
            return FALSE;
        }
    }

    xClearScreen();

    if (g_pchOut) {
        MemFree(g_pchOut);
    }

    if (g_bInitialized) {    
        pd3drPos->Release();
        pd3drTC->Release();
        pd3diText->Release();
    }

    g_pchOut = pchNewOut;
    g_uOutSize = uBytes;

    UnlockConsole();

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     xSetDisplayWidth
//
// Description:
//
//     Set the width of the console display, in characters.  This width will
//     determine how long a line of console text can be before it is wrapped
//     to the next line.
//
// Arguments:
//
//     UINT uWidth              - Width of the console in characters
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xSetDisplayWidth(UINT uWidth) {

    if (uWidth == 0) {
        return FALSE;
    }

    LockConsole();

    xClearScreen();

    if (g_bInitialized) {
        g_fMaxColScroll = (float)uWidth - ((g_fTargetWidth - BORDER_LEFT - BORDER_RIGHT) / g_fCharWidth);
    }

    g_uOutWidth = uWidth;

    UnlockConsole();

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     xDisplayConsole
//
// Description:
//
//     Draw the console on the screen.  If the console manages Direct3D and
//     text has been drawn to the frame buffer using xTextOut, the console
//     will be overlayed in the scene and the frame buffer will be presented.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xDisplayConsole(void) {

    LockConsole();
    DrawConsole();
    UnlockConsole();
}

//******************************************************************************
//
// Function:
//
//     xResetConsole
//
// Description:
//
//     Reset the console in response to a device loss or change.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xResetConsole(void) {

    LockConsole();

    EffaceConsole();

    if (!InitConsole()) {
        UnlockConsole();
        return FALSE;
    }

    RefreshConsole();

    UnlockConsole();

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     xTextOutA
//
// Description:
//
//     Draw the given text string on the frame buffer.
//
// Arguments:
//
//     float fXStart            - x screen coordinate of the upper left corner 
//                                of the start of the string
//
//     float fYStart            - y screen coordinate of the upper left corner
//                                of the start of the string
//
//     float fCWidth            - Width in pixels of the characters to be used
//                                to display the string
//
//     float fCHeight           - Height in pixels of the characters to be used
//                                to display the string
//
//     XCOLOR xcColor           - Text color used to display the string
//
//     LPCSTR pszString         - String to output on the screen
//
//     UINT uCount              - Number of characters in the string
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xTextOutA(float fXStart, float fYStart, float fCWidth, 
                        float fCHeight, XCOLOR xcColor, LPCSTR pszString, 
                        UINT uCount)
{
/*
    CBaseTexture8*  pd3dt;
    CVertexBuffer8* pd3dr;
    CIndexBuffer8*  pd3di;
    UINT            uStride;
    UINT            uBaseIndex;
    DWORD           dwSrcBlend, dwDstBlend, dwAlphaBlend, dwColorOp0, 
                    dwColorOp1, dwAlphaOp0, dwColorArg1, dwColorArg2, 
                    dwAlphaArg1, dwAlphaArg2, dwAlphaTest, dwShader;
*/
    DWORD dwBlock;
    HRESULT hr;

    if (!g_bInitialized) {
        return FALSE;
    }

    LockConsole();

    // Save the device state
    hr = g_pDevice->CreateStateBlock(D3DSBT_ALL, &dwBlock);
    if (FAILED(hr)) {
        UnlockConsole();
        return FALSE;
    }
/*
    g_pDevice->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
    g_pDevice->GetRenderState(D3DRS_DESTBLEND, &dwDstBlend);
    g_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
    g_pDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    g_pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp0);
    g_pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    g_pDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    g_pDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp0);
    g_pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    g_pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
    g_pDevice->GetTextureStageState(1, D3DTSS_COLOROP, &dwColorOp1);
    g_pDevice->GetStreamSource(0, &pd3dr, &uStride);
    g_pDevice->GetIndices(&pd3di, &uBaseIndex);
    g_pDevice->GetTexture(0, &pd3dt);
    g_pDevice->GetVertexShader(&dwShader);
*/
    // Prepare to render the text
    g_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    g_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)FALSE);

    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
    SetColorStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pDevice->SetTexture(0, g_pd3dtFontMap);
    g_pDevice->SetVertexShader(FVF_TLVERTEX);

    // Draw the text
    DrawTextQuads(pszString, uCount, fXStart, fYStart, fCWidth, 
                  fCHeight, (D3DCOLOR)xcColor, fXStart);

    // Restore the device state
/*
    g_pDevice->SetVertexShader(dwShader);
    if (pd3di) {
        g_pDevice->SetIndices(pd3di, uBaseIndex);
    }
    if (pd3dr) {
        g_pDevice->SetStreamSource(0, pd3dr, uStride);
    }
    g_pDevice->SetTexture(0, pd3dt);
    SetColorStage(g_pDevice, 0, dwColorArg1, dwColorArg2, (D3DTEXTUREOP)dwColorOp0);
    SetAlphaStage(g_pDevice, 0, dwAlphaArg1, dwAlphaArg2, (D3DTEXTUREOP)dwAlphaOp0);
    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, dwColorOp1);
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    g_pDevice->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
    g_pDevice->SetRenderState(D3DRS_DESTBLEND, dwDstBlend);
*/
    g_pDevice->ApplyStateBlock(dwBlock);
    g_pDevice->DeleteStateBlock(dwBlock);

    UnlockConsole();

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     xTextOutW
//
// Description:
//
//     Draw the given text string on the frame buffer.
//
// Arguments:
//
//     float fXStart            - x screen coordinate of the upper left corner 
//                                of the start of the string
//
//     float fYStart            - y screen coordinate of the upper left corner
//                                of the start of the string
//
//     float fCWidth            - Width in pixels of the characters to be used
//                                to display the string
//
//     float fCHeight           - Height in pixels of the characters to be used
//                                to display the string
//
//     XCOLOR xcColor           - Text color used to display the string
//
//     LPCWSTR pszString        - String to output on the screen
//
//     UINT uCount              - Number of characters in the string
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL DXCIOAPI xTextOutW(float fXStart, float fYStart, float fCWidth,
                        float fCHeight, XCOLOR xcColor, LPCWSTR pszString, 
                        UINT uCount) 
{
    char szBuffer[MAX_TCHAR_OUTPUT];
    wcstombs(szBuffer, pszString, MAX_TCHAR_OUTPUT);
    return xTextOutA(fXStart, fYStart, fCWidth, fCHeight, xcColor, szBuffer, uCount);
}

//******************************************************************************
//
// Function:
//
//     xDebugStringA
//
// Description:
//
//     Take the formatted output and send it to both the console output stream
//     and to the debugger.
//
// Arguments:
//
//     LPCSTR pszFormat         - Formatted string describing the output
//
//     Varaiable argument list  - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xDebugStringA(const char* pszFormat, ...) {

    char szBuffer[MAX_TCHAR_OUTPUT];
    va_list vl;
    va_start(vl, pszFormat);
    xvprintf(pszFormat, vl);
    vsprintf(szBuffer, pszFormat, vl);
    va_end(vl);
    OutputDebugStringA(szBuffer);
}

//******************************************************************************
//
// Function:
//
//     xDebugStringW
//
// Description:
//
//     Take the formatted output and send it to both the console output stream
//     and to the debugger.
//
// Arguments:
//
//     LPCWSTR pszFormat        - Formatted string describing the output
//
//     Varaiable argument list  - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DXCIOAPI xDebugStringW(const wchar_t* pszFormat, ...) {

    wchar_t wszBuffer[MAX_TCHAR_OUTPUT];
    va_list vl;
    va_start(vl, pszFormat);
    xvwprintf(pszFormat, vl);
    vswprintf(wszBuffer, pszFormat, vl);
    va_end(vl);
    OutputDebugStringW(wszBuffer);
}

//******************************************************************************
// stdio functions
//******************************************************************************

//******************************************************************************
int DXCIOAPI xprintf(const char* format, ...) {

    va_list vl;
    int     nPrinted;

    va_start(vl, format);
    nPrinted = xvprintf(format, vl);
    va_end(vl);

    return nPrinted;
}

//******************************************************************************
int DXCIOAPI xwprintf(const wchar_t* format, ...) {

    va_list vl;
    int     nPrinted;

    va_start(vl, format);
    nPrinted = xvwprintf(format, vl);
    va_end(vl);

    return nPrinted;
}

//******************************************************************************
int DXCIOAPI xvprintf(const char* format, va_list argptr) {

    char szBuffer[MAX_TCHAR_OUTPUT];
    int  nWritten;
    
    nWritten = vsprintf(szBuffer, format, argptr);
    LockConsole();
    PutStringA(szBuffer);
    UnlockConsole();

    return nWritten;
}

//******************************************************************************
int DXCIOAPI xvwprintf(const wchar_t* format, va_list argptr) {

    wchar_t wszBuffer[MAX_TCHAR_OUTPUT];
    int     nWritten;

    nWritten = vswprintf(wszBuffer, format, argptr);
    LockConsole();
    PutStringW(wszBuffer);
    UnlockConsole();

    return nWritten;
}

//******************************************************************************
int DXCIOAPI xputchar(int c) {

    char sz[2];

    sz[0] = (char)c;
    sz[1] = '\0';
    LockConsole();
    PutStringA(sz);
    UnlockConsole();

    return (int)sz[0];
}

//******************************************************************************
wint_t DXCIOAPI xputwchar(wint_t c) {

    wchar_t wsz[2];

    wsz[0] = c;
    wsz[1] = L'\0';
    LockConsole();
    PutStringW(wsz);
    UnlockConsole();

    return wsz[0];
}

//******************************************************************************
int DXCIOAPI xputs(const char* string) {

    LockConsole();
    PutStringA(string);
    PutStringA("\n");
    UnlockConsole();
    return 0;
}

//******************************************************************************
int DXCIOAPI xputws(const wchar_t* string) {

    LockConsole();
    PutStringW(string);
    PutStringW(L"\n");
    UnlockConsole();
    return 0;
}

//******************************************************************************
// Output buffer functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     PutStringA
//
// Description:
//
//     Append the given string to the end of the output stream buffer.
//
// Arguments:
//
//     LPCSTR                   - Null-terminated string to add to the stream
//
// Return Value:
//
//     None.
//
//******************************************************************************
void PutStringA(LPCSTR pszString) {

    LPCSTR psz;
    UINT   uStart;
    UINT   i;

    if (!pszString) {
        return;
    }

    uStart = g_uOutTail;

    for (psz = pszString; *psz; psz++) {

        g_pchOut[g_uOutTail++] = *psz;
        if (g_uOutTail == g_uOutSize) {
            g_uOutTail = 0;
        }
        if (g_uOutTail == g_uOutHead) {
            for (i = 0; g_pchOut[g_uOutHead] != '\n'; ) {
                switch (g_pchOut[g_uOutHead]) {
                    case '\t':
                        i += (i / TAB_LENGTH + 1) * TAB_LENGTH - i;
                        break;
                    case '\r':
                        i = 0;
                        break;
                    case '\b':
                        if (i > 0) i--;
                        break;
                    default:
                        i++;
                        break;
                }
                if (i >= g_uOutWidth) {
                    break;
                }
                if (++g_uOutHead == g_uOutSize) {
                    g_uOutHead = 0;
                }
                if (g_uOutHead == g_uOutTail) {
                    // Complete wraparound (the buffer is too small
                    // relative to the width of the margin and the length
                    // of the line.
                    uStart = g_uOutHead + 1;
                    if (uStart == g_uOutSize) {
                        uStart = 0;
                    }
                    if (g_bInitialized) {
                        g_uTextHead = 0;
                        g_uTextTail = 0;
                        g_uTextLine = 0;
                        g_uTextCol = 0;
                        g_uTextCursor = 0;
                    }
                    break;
                }
            }
            if (++g_uOutHead == g_uOutSize) {
                g_uOutHead = 0;
            }
        }
    }

    if (g_bInitialized) {
        UpdateConsole(uStart);
    }
}

//******************************************************************************
//
// Function:
//
//     PutStringW
//
// Description:
//
//     Append the given string to the end of the output stream buffer.
//
// Arguments:
//
//     LPCWSTR                  - Null-terminated string to add to the stream
//
// Return Value:
//
//     None.
//
//******************************************************************************
void PutStringW(LPCWSTR pszString) {

    char szBuffer[MAX_TCHAR_OUTPUT];
    wcstombs(szBuffer, pszString, MAX_TCHAR_OUTPUT);
    PutStringA(szBuffer);
}

//******************************************************************************
// Display functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     InitConsole
//
// Description:
//
//     Initialize the console's textures, vertex buffers, and index buffers.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL InitConsole() {

    LPDIRECT3DSURFACE8  pd3ds;
    D3DSURFACE_DESC     d3dsd;
// ##TODO: Switch to bump mapping a noise background when DX8 supports it on the GeForce2
    D3DLOCKED_RECT      d3dlr;
    LPDWORD             pdwPixel;
    DWORD               dwPitch;
    UINT                i, j;
    float               fX, fY, fDX, fDY, fIntensity;
    HRESULT             hr;

#ifndef UNDER_XBOX
    hr = g_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"));
        xReleaseConsole();
        return FALSE;
    }
    hr = pd3ds->GetDesc(&d3dsd);
    pd3ds->Release();
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
        xReleaseConsole();
        return FALSE;
    }
#else
    d3dsd.Width = 640;
    d3dsd.Height = 480;
#endif // UNDER_XBOX

    g_fTargetWidth = (float)d3dsd.Width;
    g_fTargetHeight = (float)d3dsd.Height;

    g_fMaxColScroll = (float)g_uOutWidth - ((g_fTargetWidth - BORDER_LEFT - BORDER_RIGHT) / g_fCharWidth);
    g_fMaxRowScroll = (float)g_uTextHeight - ((g_fTargetHeight - BORDER_TOP - BORDER_BOTTOM) / g_fCharHeight);
    g_fCurrentRow = 0.0f;
    g_fCurrentCol = 0.0f;

    g_vTranslate.x = BORDER_LEFT;
    g_vTranslate.y = g_fTargetHeight - BORDER_BOTTOM - g_fMaxRowScroll * g_fCharHeight;

    // Create a font map to use in displaying the output text on the screen
    if (!CreateFontMap(g_pDevice)) {
        return FALSE;
    }

    if (!CreateGeometry(g_uOutSize)) {
        EffaceConsole();
        return FALSE;
    }

    if (g_bUseBackground) {

        if (*g_szBackImage) {
            g_pd3dtBackBase = (LPDIRECT3DTEXTURE8)CreateTexture(g_pDevice, g_szBackImage, D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
            if (!g_pd3dtBackBase) {
                g_bUseBackground = FALSE;
            }
            else {
                RemoveTexture(g_pd3dtBackBase);
            }
        }

        else {

#ifdef UNDER_XBOX
            Swizzler swz(BACKTEX_WIDTH, BACKTEX_HEIGHT, 1);
#endif
            g_pd3dtBackBase = (LPDIRECT3DTEXTURE8)CreateTexture(g_pDevice, TEXT("T_XBump"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
            if (!g_pd3dtBackBase) {
//##DELETE
                g_pd3dtBackBase = (LPDIRECT3DTEXTURE8)CreateTexture(g_pDevice, BACKTEX_WIDTH, BACKTEX_HEIGHT, D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
                if (!g_pd3dtBackBase) {
                    EffaceConsole();
                    return FALSE;
                }
                RemoveTexture(g_pd3dtBackBase);

                hr = g_pd3dtBackBase->LockRect(0, &d3dlr, NULL, 0);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
                    EffaceConsole();
                    return FALSE;
                }

                pdwPixel = (LPDWORD)d3dlr.pBits;

                dwPitch = d3dlr.Pitch >> 2;

                g_pNoise->SetRange(0.0f, 255.0f);

                fDX = 1.0f / (float)(BACKTEX_WIDTH - 1) * g_fTargetWidth / ((float)BACKTEX_WIDTH / 4.0f);
                fDY = 1.0f / (float)(BACKTEX_HEIGHT - 1) * g_fTargetHeight / ((float)BACKTEX_HEIGHT / 4.0f);

#ifdef UNDER_XBOX
                swz.SetU(0);
                swz.SetV(0);
#endif

                for (i = 0, fY = 0.0f; i < BACKTEX_HEIGHT; i++, fY += fDY) {

                    for (j = 0, fX = 0.0f; j < BACKTEX_WIDTH; j++, fX += fDX) {

                        fIntensity = g_pNoise->GetIntensity(fX, fY);
#ifndef UNDER_XBOX
                        pdwPixel[j] = RGBA_MAKE((BYTE)fIntensity, (BYTE)fIntensity, (BYTE)fIntensity, 255);
#else
                        pdwPixel[swz.Get2D()] = RGBA_MAKE((BYTE)fIntensity, (BYTE)fIntensity, (BYTE)fIntensity, 255);
                        swz.IncU();
#endif
                    }

#ifndef UNDER_XBOX
                    pdwPixel += dwPitch;
#else
                    swz.IncV();
#endif
                }

                hr = g_pd3dtBackBase->UnlockRect(0);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
                    EffaceConsole();
                    return FALSE;
                }
//##END DELETE
//                g_bUseBackground = FALSE;
            }
            else {
                RemoveTexture(g_pd3dtBackBase);
            }
/*
            // ##TODO: Switch to bump mapping a noise background when DX8 supports it on the GeForce2

            g_pd3dtBackBase = (LPDIRECT3DTEXTURE8)CreateTexture(g_pDevice, BACKTEX_WIDTH, BACKTEX_HEIGHT, D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
            if (!g_pd3dtBackBase) {
                EffaceConsole();
                return FALSE;
            }
            RemoveTexture(g_pd3dtBackBase);

            hr = g_pd3dtBackBase->LockRect(0, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
                EffaceConsole();
                return FALSE;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

            dwPitch = d3dlr.Pitch >> 2;

            g_pNoise->SetRange(0.0f, 255.0f);

            fDX = 1.0f / (float)(BACKTEX_WIDTH - 1) * g_fTargetWidth / ((float)BACKTEX_WIDTH / 4.0f);
            fDY = 1.0f / (float)(BACKTEX_HEIGHT - 1) * g_fTargetHeight / ((float)BACKTEX_HEIGHT / 4.0f);

#ifdef UNDER_XBOX
            swz.SetU(0);
            swz.SetV(0);
#endif

            for (i = 0, fY = 0.0f; i < BACKTEX_HEIGHT; i++, fY += fDY) {

                for (j = 0, fX = 0.0f; j < BACKTEX_WIDTH; j++, fX += fDX) {

                    fIntensity = g_pNoise->GetIntensity(fX, fY);
#ifndef UNDER_XBOX
                    pdwPixel[j] = RGBA_MAKE((BYTE)fIntensity, (BYTE)fIntensity, (BYTE)fIntensity, 255);
#else
                    pdwPixel[swz.Get2D()] = RGBA_MAKE((BYTE)fIntensity, (BYTE)fIntensity, (BYTE)fIntensity, 255);
                    swz.IncU();
#endif
                }

#ifndef UNDER_XBOX
                pdwPixel += dwPitch;
#else
                swz.IncV();
#endif
            }

            hr = g_pd3dtBackBase->UnlockRect(0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
                EffaceConsole();
                return FALSE;
            }

            g_pd3dtBackBump = (LPDIRECT3DTEXTURE8)CreateTexture(g_pDevice, TEXT("T_XBump"), D3DFMT_V8U8, TTYPE_BUMPMAP);
            if (!g_pd3dtBackBump) {
                EffaceConsole();
                return FALSE;
            }
            RemoveTexture(g_pd3dtBackBump);

            g_pd3dtBackEnv = (LPDIRECT3DTEXTURE8)CreateDiffuseMap(g_pDevice, BACKTEX_WIDTH, BACKTEX_HEIGHT, RGBA_MAKE(255, 255, 255, 255), D3DFMT_A8R8G8B8, FALSE);
            if (!g_pd3dtBackEnv) {
                EffaceConsole();
                return FALSE;
            }
            RemoveTexture(g_pd3dtBackEnv);
*/
        }
    }

    g_bInScene = FALSE;

    g_bDoConsoleUpdate = TRUE;

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     EffaceConsole
//
// Description:
//
//     Release the textures, vertex buffers, and index buffers used by the 
//     console.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void EffaceConsole() {

    g_bDoConsoleUpdate = FALSE;

    if (g_pd3dtBackEnv) {
        ReleaseTexture(g_pd3dtBackEnv);
        g_pd3dtBackEnv = NULL;
    }
    if (g_pd3dtBackBump) {
        ReleaseTexture(g_pd3dtBackBump);
        g_pd3dtBackBump = NULL;
    }
    if (g_pd3dtBackBase) {
        ReleaseTexture(g_pd3dtBackBase);
        g_pd3dtBackBase = NULL;
    }

    ReleaseGeometry();

    ReleaseFontMap();
}

//******************************************************************************
//
// Function:
//
//     CreateGeometry
//
// Description:
//
//     Create and initialize the vertex and index buffers used by the console
//     in drawing the background and outputting text.
//
// Arguments:
//
//     UINT uNumChars           - Number of quads to be used in displaying
//                                output stream text on the screen
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateGeometry(UINT uNumChars) {

    LPBYTE     pData;
    D3DVECTOR* pvPos;
    PTLBVERTEX prBack;
    D3DXMATRIX mIdentity;
    LPWORD     pwText;
    USHORT     i, j, k;
    HRESULT    hr;

// ##DELETE: Until support for vertex shaders is introduced, use a temporary vertex buffer to hold transformed vertices
// Create a vertex buffer to hold the positions of text quads
g_uVBDeleteMeSize = uNumChars * 4 * sizeof(TLVERTEX);
hr = g_pDevice->CreateVertexBuffer(g_uVBDeleteMeSize,
                                        0, 0, POOL_DEFAULT, 
                                        &g_pd3drDeleteMe);
if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
    return FALSE;
}
// ##END DELETE

    // Create a vertex buffer to hold the positions of text quads
    g_uVBPositionSize = uNumChars * 4 * sizeof(D3DVECTOR);
    hr = g_pDevice->CreateVertexBuffer(g_uVBPositionSize,
                                            0, 0, POOL_DEFAULT, 
                                            &g_pd3drPosition);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
        return FALSE;
    }

    // Initialize position vertices to zero so nothing will be initially drawn
    hr = g_pd3drPosition->Lock(0, g_uVBPositionSize, &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        ReleaseGeometry();
        return FALSE;
    }
    pvPos = (D3DVECTOR*)pData;

    for (i = 0; i < uNumChars * 4; i++) {
        pvPos[i].x = POS_BLANK;
        pvPos[i].y = POS_BLANK;
        pvPos[i].z = 0.0f;
    }

    hr = g_pd3drPosition->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        ReleaseGeometry();
        return FALSE;
    }

    // Create a vertex buffer to hold the texture coordinates of text quads
    g_uVBTexCoordSize = uNumChars * 4 * sizeof(TCOORD2);
    hr = g_pDevice->CreateVertexBuffer(g_uVBTexCoordSize,
                                            0, 0, POOL_DEFAULT, 
                                            &g_pd3drTexCoord);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
        ReleaseGeometry();
        return FALSE;
    }

    // Create an index buffer for the text geometry
    g_uIBTextSize = uNumChars * 6 * sizeof(WORD);
    hr = g_pDevice->CreateIndexBuffer(g_uIBTextSize,
                                            0, D3DFMT_INDEX16, POOL_DEFAULT,
                                            &g_pd3diText);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateIndexBuffer"))) {
        ReleaseGeometry();
        return FALSE;
    }

    hr = g_pd3diText->Lock(0, g_uIBTextSize, &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::Lock"));
        ReleaseGeometry();
        return FALSE;
    }

    // Initialize the indices
    for (pwText = (LPWORD)pData, i = 0, j = 0, k = 0; i < uNumChars; i++, j += 4, k += 6) {
        pwText[k]   = j;
        pwText[k+1] = j + 2;
        pwText[k+2] = j + 1;
        pwText[k+3] = j + 1;
        pwText[k+4] = j + 2;
        pwText[k+5] = j + 3;
    }

    hr = g_pd3diText->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::Unlock"));
        ReleaseGeometry();
        return FALSE;
    }

    // Create a vertex buffer the hold the background vertices
    hr = g_pDevice->CreateVertexBuffer(4 * sizeof(TLBVERTEX),
                                            0, FVF_TLBVERTEX, POOL_DEFAULT, 
                                            &g_pd3drBack);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
        ReleaseGeometry();
        return FALSE;
    }

    // Initialize the background vertices
    hr = g_pd3drBack->Lock(0, 4 * sizeof(TLBVERTEX), &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        ReleaseGeometry();
        return FALSE;
    }

    prBack = (PTLBVERTEX)pData;
    *prBack++ = TLBVERTEX(D3DXVECTOR3(0.0f, g_fTargetHeight, 0.9999f), 0.0001f, g_dwBackgroundColor, 0, 0.0f, 1.0f, 0.0f, 0.0f);
    *prBack++ = TLBVERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.9999f), 0.0001f, g_dwBackgroundColor, 0, 0.0f, 0.0f, 0.0f, 0.0f);
    *prBack++ = TLBVERTEX(D3DXVECTOR3(g_fTargetWidth, 0.0f, 0.9999f), 0.0001f, g_dwBackgroundColor, 0, 1.0f, 0.0f, 0.0f, 0.0f);
    *prBack   = TLBVERTEX(D3DXVECTOR3(g_fTargetWidth, g_fTargetHeight, 0.9999f), 0.0001f, g_dwBackgroundColor, 0, 1.0f, 1.0f, 0.0f, 0.0f);

    prBack = (PTLBVERTEX)pData;
    D3DXMatrixIdentity(&mIdentity);
//    SetDiffuseMapCoords(&D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0, 
//                       (PTCOORD2)&prBack[0].u1, sizeof(TLBVERTEX), 
//                       4, &mIdentity, &D3DXVECTOR3(0.0f, 1.0f, 0.5f), M_3PIDIV2);

    hr = g_pd3drBack->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        ReleaseGeometry();
        return FALSE;
    }

    g_uTextSize = uNumChars;

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ReleaseGeometry
//
// Description:
//
//     Release the vertex and index buffers used by the console.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseGeometry() {

// ##DELETE: Until support for vertex shaders is introduced, use a temporary vertex buffer to hold transformed vertices
if (g_pd3drDeleteMe){
g_pd3drDeleteMe->Release();
g_pd3drDeleteMe = NULL;
}
// ##END DELETE

    if (g_pd3drBack) {
        g_pd3drBack->Release();
        g_pd3drBack = NULL;
    }

    if (g_pd3drPosition) {
        g_pd3drPosition->Release();
        g_pd3drPosition = NULL;
    }

    if (g_pd3drTexCoord) {
        g_pd3drTexCoord->Release();
        g_pd3drTexCoord = NULL;
    }

    if (g_pd3diText) {
        g_pd3diText->Release();
        g_pd3diText = NULL;
    }
}

//******************************************************************************
//
// Function:
//
//     RefreshConsole
//
// Description:
//
//     Recalculate all vertex data pertaining to the display of text for the 
//     console from the output stream buffer.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RefreshConsole() {

    LPBYTE      pData;
    D3DVECTOR*  pvPos;
    HRESULT     hr;
    UINT        i;

    g_uTextHead = 0;
    g_uTextTail = 0;
    g_uTextLine = 0;
    g_uTextHeight = 1;
    g_uTextCol = 0;
    g_uTextCursor = 0;
    g_fTextX = 0.0f;

    // Clear the position buffer (such that it won't get displayed)
    hr = g_pd3drPosition->Lock(0, g_uVBPositionSize, &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return;
    }
    pvPos = (D3DVECTOR*)pData;

    for (i = 0; i < g_uTextSize * 4; i++) {
        pvPos[i].x = POS_BLANK;
        pvPos[i].y = POS_BLANK;
        pvPos[i].z = 0.0f;
    }

    hr = g_pd3drPosition->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return;
    }

    UpdateConsole(g_uOutHead);
}

//******************************************************************************
//
// Function:
//
//     UpdateConsole
//
// Description:
//
//     Update the vertex buffers with new data from the console output stream.
//
// Arguments:
//
//     UINT uStart              - Starting index into the console output buffer
//                                from which to update the console with new text
//
// Return Value:
//
//     None.
//
//******************************************************************************
void UpdateConsole(UINT uStart) {

    UINT  src;
    LPBYTE pData;
    D3DVECTOR* pvPos;
    PTCOORD2   ptcUV, ptcChar;
    UINT i, j;
    UINT uSpaces;
    HRESULT hr;

    if (!g_bDoConsoleUpdate) {
        return;
    }

    g_bDoConsoleUpdate = FALSE;

    // Lock the vertex buffers
    hr = g_pd3drPosition->Lock(0, g_uVBPositionSize, &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        g_bDoConsoleUpdate = TRUE;
        return;
    }
    pvPos = (D3DVECTOR*)pData;

    hr = g_pd3drTexCoord->Lock(0, g_uVBTexCoordSize, &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        g_bDoConsoleUpdate = TRUE;
        return;
    }
    ptcUV = (PTCOORD2)pData;

    for (src = uStart; src != g_uOutTail; ) {

        if (g_pchOut[src] == '\n' || g_uTextCol >= g_uOutWidth) {
            g_fTextX = 0.0f;
            g_uTextLine = g_uTextTail;
            g_uTextCursor = g_uTextTail;
            g_uTextCol = 0;
            for (i = g_uTextHead; i != g_uTextTail; ) {
                pvPos[i * 4].y     -= g_fCharHeight;
                pvPos[i * 4 + 1].y -= g_fCharHeight;
                pvPos[i * 4 + 2].y -= g_fCharHeight;
                pvPos[i * 4 + 3].y -= g_fCharHeight;

                if (++i == g_uTextSize) {
                    i = 0;
                }
            }
            g_uTextHeight++;
            g_fMaxRowScroll = (float)g_uTextHeight - ((g_fTargetHeight - BORDER_TOP - BORDER_BOTTOM) / g_fCharHeight);
            if (g_fCurrentRow != 0.0f) {
                g_fCurrentRow += 1.0f;
            }
            UpdateConsoleScrollY();
        }

        uSpaces = 0;

        switch (g_pchOut[src]) {

            case '\r':
                g_uTextCursor = g_uTextLine;
                g_uTextCol = 0;
                g_fTextX = 0.0f;
                break;

            case '\b':
                if (g_uTextCursor != g_uTextLine) {
                    if (g_uTextCursor > 0) {
                        g_uTextCursor--;
                    }
                    else {
                        g_uTextCursor = g_uTextSize - 1;
                    }
                    g_uTextCol--;
                    g_fTextX -= g_fCharWidth;
                }
                break;

            case '\n':
                // Already handled
                break;

            case '\t':
                uSpaces = (g_uTextCol / TAB_LENGTH + 1) * TAB_LENGTH - g_uTextCol;
                if (g_uTextCol + uSpaces >= g_uOutWidth) {
                    uSpaces = g_uOutWidth - g_uTextCol;
                }
                // Fall through

            default:
                if (uSpaces) {
                    ptcChar = CHAR_TO_TCOORD(' ');
                    i = 1;
                }
                else {
                    ptcChar = CHAR_TO_TCOORD(g_pchOut[src]);
                    i = 0;
                }

                for (; i <= uSpaces; i++) {

                    pvPos[g_uTextCursor * 4].x        = g_fTextX;
                    pvPos[g_uTextCursor * 4].y        = -g_fCharHeight;
                    ptcUV[g_uTextCursor * 4].u        = ptcChar->u;
                    ptcUV[g_uTextCursor * 4].v        = ptcChar->v;

                    pvPos[g_uTextCursor * 4 + 1].x    = g_fTextX;
                    pvPos[g_uTextCursor * 4 + 1].y    = 0.0f;
                    ptcUV[g_uTextCursor * 4 + 1].u    = ptcChar->u;
                    ptcUV[g_uTextCursor * 4 + 1].v    = ptcChar->v + FONT_TCOORD_HEIGHT;

                    pvPos[g_uTextCursor * 4 + 2].x    = g_fTextX + g_fCharWidth;
                    pvPos[g_uTextCursor * 4 + 2].y    = -g_fCharHeight;
                    ptcUV[g_uTextCursor * 4 + 2].u    = ptcChar->u + FONT_TCOORD_WIDTH;
                    ptcUV[g_uTextCursor * 4 + 2].v    = ptcChar->v;

                    pvPos[g_uTextCursor * 4 + 3].x    = g_fTextX + g_fCharWidth;
                    pvPos[g_uTextCursor * 4 + 3].y    = 0.0f;
                    ptcUV[g_uTextCursor * 4 + 3].u    = ptcChar->u + FONT_TCOORD_WIDTH;
                    ptcUV[g_uTextCursor * 4 + 3].v    = ptcChar->v + FONT_TCOORD_HEIGHT;

                    if (g_uTextCursor == g_uTextTail) {
                        if (++g_uTextTail == g_uTextSize) {
                            g_uTextTail = 0;
                        }
                        if (g_uTextTail == g_uTextHead) {

                            float       y;
                            D3DVECTOR*  pv;

                            // Advance the head of the list to the start of the 
                            // next line, clearing the text in between
                            for (y = pvPos[g_uTextHead * 4].y; y == pvPos[g_uTextHead * 4].y; ) {
                                for (j = 0, pv = pvPos + g_uTextHead * 4; j < 4; j++, pv++) {
                                    pv->x = POS_BLANK;
                                    pv->y = POS_BLANK;
                                    pv->z = 0.0f;
                                }

                                if (++g_uTextHead == g_uTextSize) {
                                    g_uTextHead = 0;
                                }
                            }

                            g_uTextHeight -= (UINT)((pvPos[g_uTextHead * 4].y - y) / g_fCharHeight + 0.5f);
                            g_fMaxRowScroll = (float)g_uTextHeight - ((g_fTargetHeight - BORDER_TOP - BORDER_BOTTOM) / g_fCharHeight);
                            UpdateConsoleScrollY();
                        }
                    }
                    if (++g_uTextCursor == g_uTextSize) {
                        g_uTextCursor = 0;
                    }

                    g_fTextX += g_fCharWidth;
                    g_uTextCol++;
                }
                break;
        }

        if (++src == g_uOutSize) {
            src = 0;
        }
    }

    // Unlock the vertex buffers
    hr = g_pd3drPosition->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        g_bDoConsoleUpdate = TRUE;
        return;
    }

    hr = g_pd3drTexCoord->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        g_bDoConsoleUpdate = TRUE;
        return;
    }

    // Calculate the visible portions of the vertex buffer
    UpdateVisibility();

    g_bDoConsoleUpdate = TRUE;
}

//******************************************************************************
//
// Function:
//
//     BeginConsoleScene
//
// Description:
//
//     Pump messages, check for a lost device, clear the background, and begin a 
//     new scene for the console.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL BeginConsoleScene() {

    HRESULT hr;
#ifndef UNDER_XBOX
    MSG     msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

        // If a message is ready, process it and proceed to
        // check for another message
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hr = g_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return FALSE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!xResetConsole()) {
                return FALSE;
            }
        }
    }

#endif // !UNDER_XBOX

    // Clear the rendering target
    if (!g_bUseBackground) {
        g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_dwBackgroundColor, 1.0f, 0);
    }

    // Begin the scene
    g_pDevice->BeginScene();

    g_bInScene = TRUE;

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     EndConsoleScene
//
// Description:
//
//     End a console scene and present the frame buffer.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
static void EndConsoleScene() {

    // End the scene
    g_pDevice->EndScene();

    // Update the screen
    g_pDevice->Present(NULL, NULL, NULL, NULL);

    g_bInScene = FALSE;
}

//******************************************************************************
//
// Function:
//
//     DrawConsoleBackground
//
// Description:
//
//     Draw the background for the console.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
static void DrawConsoleBackground() {

    g_pDevice->SetVertexShader(FVF_TLBVERTEX);

    // Set textures
/*
    // ##TODO: Switch to bump mapping a noise background when DX8 supports it on the GeForce2

    g_pDevice->SetTexture(0, g_pd3dtBackBase);
    g_pDevice->SetTexture(1, g_pd3dtBackBump);
    g_pDevice->SetTexture(2, g_pd3dtBackEnv);

    g_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    g_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    g_pDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 1);

    SetColorStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    if (g_pd3dtBackBump) {
        SetColorStage(g_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_BUMPENVMAP);
        SetColorStage(g_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE4X);
    }
    else {
        g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    }

    g_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, F2DW(1.0f));
    g_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
    g_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
    g_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, F2DW(1.0f));

    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
//    SetColorStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
//    SetAlphaStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
//    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//    g_pDevice->SetTexture(0, g_pd3dtBackBase);
*/

    g_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
    g_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
    SetColorStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pDevice->SetTexture(0, g_pd3dtBackBase);

    g_pDevice->SetStreamSource(0, g_pd3drBack, sizeof(TLBVERTEX));

    g_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    g_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)D3DTEXF_POINT);
    g_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_POINT);
}

//******************************************************************************
//
// Function:
//
//     DrawConsoleText
//
// Description:
//
//     Draw the console text.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
static void DrawConsoleText() {

    D3DCOLOR    cShadow;
// ##DELETE: Until support for vertex shaders is introduced, transform the vertices
LPBYTE pData, pPos, pTex;
D3DVECTOR* pvPos;
PTCOORD2 ptc;
PTLVERTEX pr;
UINT    i;
// ##END DELETE

    // ##REVIEW: When supported, switch to using vertex shaders to translate the vertices.
    // The output from the shader must be in clip-space
    // (0.0f to 1.0f) and will later be mapped to the viewport.  Any
    // clipping will be performed by d3d after shading.

    g_pDevice->SetVertexShader(FVF_TLVERTEX);

    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
    SetColorStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pDevice->SetTexture(0, g_pd3dtFontMap);
    g_pDevice->SetTexture(1, NULL);
    g_pDevice->SetTexture(2, NULL);

    g_pDevice->SetIndices(g_pd3diText, 0);

    cShadow = RGBA_MAKE(0, 0, 0, (BYTE)((float)RGBA_GETALPHA(g_dwTextColor) / 255.0f * SHADOW_INTENSITY));

// ##DELETE: Until support for vertex shaders is introduced, transform the vertices
// Transform the vertices
g_pd3drDeleteMe->Lock(0, g_uVBDeleteMeSize, &pData, 0);
g_pd3drPosition->Lock(0, g_uVBPositionSize, &pPos, 0);
g_pd3drTexCoord->Lock(0, g_uVBTexCoordSize, &pTex, 0);
//pr = (PTLVERTEX)pData;
//pvPos = (D3DVECTOR*)pPos;
//ptc = (PTCOORD2)pTex;

for (i = 0, pvPos = ((D3DVECTOR*)pPos) + g_uVStart1, pr = ((PTLVERTEX)pData) + g_uVStart1, ptc = ((PTCOORD2)pTex) + g_uVStart1; i < g_uVCount1; i++, pr++, pvPos++, ptc++) {

    pr->fRHW = 110000.0f;
    pr->vPosition.x = pvPos->x + g_vTranslate.x + SHADOW_OFFSET_X;
    pr->vPosition.y = pvPos->y + g_vTranslate.y + SHADOW_OFFSET_Y;
    pr->vPosition.z = 0.000009f;
    pr->cDiffuse = cShadow;
    pr->cSpecular = 0;
    pr->u0 = ptc->u;
    pr->v0 = ptc->v;
}
for (i = 0, pvPos = ((D3DVECTOR*)pPos) + g_uVStart2, pr = ((PTLVERTEX)pData) + g_uVStart2, ptc = ((PTCOORD2)pTex) + g_uVStart2; i < g_uVCount2; i++, pr++, pvPos++, ptc++) {

    pr->fRHW = 110000.0f;
    pr->vPosition.x = pvPos->x + g_vTranslate.x + SHADOW_OFFSET_X;
    pr->vPosition.y = pvPos->y + g_vTranslate.y + SHADOW_OFFSET_Y;
    pr->vPosition.z = 0.000009f;
    pr->cDiffuse = cShadow;
    pr->cSpecular = 0;
    pr->u0 = ptc->u;
    pr->v0 = ptc->v;
}

g_pd3drTexCoord->Unlock();
g_pd3drPosition->Unlock();
g_pd3drDeleteMe->Unlock();
// ##END DELETE

    g_pDevice->SetStreamSource(0, g_pd3drDeleteMe, sizeof(TLVERTEX));

    if (g_uVCount1) {
        g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, g_uVStart1, g_uVCount1, g_uVStart1 / 4 * 6, g_uVCount1 / 2);
    }
    if (g_uVCount2) {
        g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, g_uVStart2, g_uVCount2, g_uVStart2 / 4 * 6, g_uVCount2 / 2);
    }

// ##DELETE: Until support for vertex shaders is introduced, transform the vertices
// Transform the vertices
g_pd3drDeleteMe->Lock(0, g_uVBDeleteMeSize, &pData, 0);
g_pd3drPosition->Lock(0, g_uVBPositionSize, &pPos, 0);
g_pd3drTexCoord->Lock(0, g_uVBTexCoordSize, &pTex, 0);
pr = (PTLVERTEX)pData;
pvPos = (D3DVECTOR*)pPos;
ptc = (PTCOORD2)pTex;

for (i = 0, pvPos = ((D3DVECTOR*)pPos) + g_uVStart1, pr = ((PTLVERTEX)pData) + g_uVStart1, ptc = ((PTCOORD2)pTex) + g_uVStart1; i < g_uVCount1; i++, pr++, pvPos++, ptc++) {

    pr->fRHW = 120000.0f;
    pr->vPosition.x = pvPos->x + g_vTranslate.x;
    pr->vPosition.y = pvPos->y + g_vTranslate.y;
    pr->vPosition.z = 0.000008f;
    pr->cDiffuse = g_dwTextColor;
    pr->cSpecular = 0;
    pr->u0 = ptc->u;
    pr->v0 = ptc->v;
}
for (i = 0, pvPos = ((D3DVECTOR*)pPos) + g_uVStart2, pr = ((PTLVERTEX)pData) + g_uVStart2, ptc = ((PTCOORD2)pTex) + g_uVStart2; i < g_uVCount2; i++, pr++, pvPos++, ptc++) {

    pr->fRHW = 120000.0f;
    pr->vPosition.x = pvPos->x + g_vTranslate.x;
    pr->vPosition.y = pvPos->y + g_vTranslate.y;
    pr->vPosition.z = 0.000008f;
    pr->cDiffuse = g_dwTextColor;
    pr->cSpecular = 0;
    pr->u0 = ptc->u;
    pr->v0 = ptc->v;
}
g_pd3drTexCoord->Unlock();
g_pd3drPosition->Unlock();
g_pd3drDeleteMe->Unlock();
// ##END DELETE

    g_pDevice->SetStreamSource(0, g_pd3drDeleteMe, sizeof(TLVERTEX));

    if (g_uVCount1) {
        g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, g_uVStart1, g_uVCount1, g_uVStart1 / 4 * 6, g_uVCount1 / 2);
    }
    if (g_uVCount2) {
        g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, g_uVStart2, g_uVCount2, g_uVStart2 / 4 * 6, g_uVCount2 / 2);
    }
}

//******************************************************************************
//
// Function:
//
//     DrawConsole
//
// Description:
//
//     Draw the console to the screen.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DrawConsole() {

/*
    CBaseTexture8*  pd3dt;
    CVertexBuffer8* pd3dr;
    CIndexBuffer8*  pd3di;
    UINT            uStride;
    UINT            uBaseIndex;
    DWORD           dwSrcBlend, dwDstBlend, dwAlphaBlend, dwColorOp0, 
                    dwColorOp1, dwAlphaOp0, dwColorArg1, dwColorArg2, 
                    dwAlphaArg1, dwAlphaArg2, dwAlphaTest, dwShader;
    DWORD           dwZCmp, dwTexFMin, dwTexFMag;
*/
    DWORD           dwBlock;
    HRESULT         hr;

    if (!g_bInitialized) {
        return;
    }

    if (g_bInScene) {
        return;
    }

    // Save the device state
    hr = g_pDevice->CreateStateBlock(D3DSBT_ALL, &dwBlock);
    if (FAILED(hr)) {
        return;
    }
/*
    g_pDevice->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
    g_pDevice->GetRenderState(D3DRS_DESTBLEND, &dwDstBlend);
    g_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
    g_pDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    g_pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp0);
    g_pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    g_pDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    g_pDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp0);
    g_pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    g_pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
    g_pDevice->GetTextureStageState(1, D3DTSS_COLOROP, &dwColorOp1);
    g_pDevice->GetStreamSource(0, &pd3dr, &uStride);
    g_pDevice->GetIndices(&pd3di, &uBaseIndex);
    g_pDevice->GetTexture(0, &pd3dt);
    g_pDevice->GetVertexShader(&dwShader);

    // Prepare to render the text
    g_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    g_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)FALSE);

    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
    SetColorStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(g_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pDevice->SetTexture(0, g_pd3dtFontMap);
    g_pDevice->SetVertexShader(FVF_TLVERTEX);
*/

    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)FALSE);
//    g_pDevice->GetRenderState(D3DRS_ZFUNC, &dwZCmp);
    g_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
//    g_pDevice->GetTextureStageState(0, D3DTSS_MINFILTER, &dwTexFMin);
//    g_pDevice->GetTextureStageState(0, D3DTSS_MAGFILTER, &dwTexFMag);

    DrawConsoleBackground();
    DrawConsoleText();

/*
    g_pDevice->SetTexture(1, NULL);
    g_pDevice->SetTexture(2, NULL);
    g_pDevice->SetRenderState(D3DRS_ZFUNC, dwZCmp);
    g_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, dwTexFMin);
    g_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, dwTexFMag);

    // Restore the device state
    g_pDevice->SetVertexShader(dwShader);
    if (pd3di) {
        g_pDevice->SetIndices(pd3di, uBaseIndex);
    }
    if (pd3dr) {
        g_pDevice->SetStreamSource(0, pd3dr, uStride);
    }
    g_pDevice->SetTexture(0, pd3dt);
    SetColorStage(g_pDevice, 0, dwColorArg1, dwColorArg2, (D3DTEXTUREOP)dwColorOp0);
    SetAlphaStage(g_pDevice, 0, dwAlphaArg1, dwAlphaArg2, (D3DTEXTUREOP)dwAlphaOp0);
    g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, dwColorOp1);
    g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
    g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    g_pDevice->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
    g_pDevice->SetRenderState(D3DRS_DESTBLEND, dwDstBlend);
*/
    g_pDevice->ApplyStateBlock(dwBlock);
    g_pDevice->DeleteStateBlock(dwBlock);
}

//******************************************************************************
//
// Function:
//
//     DrawTextQuads
//
// Description:
//
//     Draw the given text string on the frame buffer.
//
// Arguments:
//
//     LPCSTR szText            - String to output on the screen
//
//     UINT uCount              - Number of characters in the string
//
//     float fXStart            - x screen coordinate of the upper left corner 
//                                of the start of the string
//
//     float fYStart            - y screen coordinate of the upper left corner
//                                of the start of the string
//
//     float fWidth             - Width in pixels of the characters to be used
//                                to display the string
//
//     float fHeight            - Height in pixels of the characters to be used
//                                to display the string
//
//     D3DCOLOR c               - Text color used to display the string
//
//     float fLeftMargin        - Location of the left margin for the text in
//                                screen coordinates.  If a newline character
//                                is encountered the continuation of the
//                                string will begin at this point.
//
// Return Value:
//
//     None.
//
//******************************************************************************
static void DrawTextQuads(LPCSTR szText, UINT uCount, float fX, float fY, 
                          float fWidth, float fHeight, D3DCOLOR c,
                          float fLeftMargin)
{
    PTCOORD2 ptcChar;
    UINT uLen = uCount;
//    D3DCOLOR cShadow;
    UINT i, j;

    if (uCount > MAX_TEXTOUT_LENGTH) {
        uLen = MAX_TEXTOUT_LENGTH;
    }

    for (i = 0, j = 0; i < uLen; i++) {

        if (szText[i] == '\n') {
            fX = fLeftMargin;
            fY += fHeight;
            continue;
        }

        ptcChar = CHAR_TO_TCOORD(szText[i]);

        g_prTextOut[j].vPosition.x     = fX;
        g_prTextOut[j].vPosition.y     = fY;
        g_prTextOut[j].cDiffuse        = c;
        g_prTextOut[j].u0              = ptcChar->u;
        g_prTextOut[j].v0              = ptcChar->v;

        g_prTextOut[j+1].vPosition.x   = fX;
        g_prTextOut[j+1].vPosition.y   = fY + fHeight;
        g_prTextOut[j+1].cDiffuse      = c;
        g_prTextOut[j+1].u0            = ptcChar->u;
        g_prTextOut[j+1].v0            = ptcChar->v + FONT_TCOORD_HEIGHT;

        g_prTextOut[j+2].vPosition.x   = fX + fWidth;
        g_prTextOut[j+2].vPosition.y   = fY;
        g_prTextOut[j+2].cDiffuse      = c;
        g_prTextOut[j+2].u0            = ptcChar->u + FONT_TCOORD_WIDTH;
        g_prTextOut[j+2].v0            = ptcChar->v;

        g_prTextOut[j+3].vPosition.x   = fX + fWidth;
        g_prTextOut[j+3].vPosition.y   = fY + fHeight;
        g_prTextOut[j+3].cDiffuse      = c;
        g_prTextOut[j+3].u0            = ptcChar->u + FONT_TCOORD_WIDTH;
        g_prTextOut[j+3].v0            = ptcChar->v + FONT_TCOORD_HEIGHT;

        fX += fWidth;

        j += 4;
    }

/*
    if (!g_bAttached) {

        cShadow = RGBA_MAKE(0, 0, 0, (BYTE)((float)RGBA_GETALPHA(c) / 255.0f * SHADOW_INTENSITY));

        for (i = 0; i < j; i++) {
            g_prTextOut[MAX_TEXTOUT_LENGTH+i].vPosition.x = g_prTextOut[i].vPosition.x + SHADOW_OFFSET_X;
            g_prTextOut[MAX_TEXTOUT_LENGTH+i].vPosition.y = g_prTextOut[i].vPosition.y - SHADOW_OFFSET_Y;
            g_prTextOut[MAX_TEXTOUT_LENGTH+i].cDiffuse = cShadow;
            g_prTextOut[MAX_TEXTOUT_LENGTH+i].u0 = g_prTextOut[i].u0;
            g_prTextOut[MAX_TEXTOUT_LENGTH+i].v0 = g_prTextOut[i].v0;
        }

        if (j) {
            g_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
                            j, j / 2, g_pwTextOut, D3DFMT_INDEX16, 
                            g_prTextOut + MAX_TEXTOUT_LENGTH, sizeof(TLVERTEX));
        }
    }
*/

    if (j) {
        g_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
                        j, j / 2, g_pwTextOut, 
                        D3DFMT_INDEX16, g_prTextOut, sizeof(TLVERTEX));
    }

    if (uCount > MAX_TEXTOUT_LENGTH) {
        DrawTextQuads(szText + MAX_TEXTOUT_LENGTH, uCount - MAX_TEXTOUT_LENGTH, fX, fY, fWidth, 
                        fHeight, c, fLeftMargin);
    }
}

//******************************************************************************
//
// Function:
//
//     UpdateVisibility
//
// Description:
//
//     Update the visibility information for the console so only the visible
//     text will be drawn.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL UpdateVisibility() {

    LPBYTE      pData;
    D3DVECTOR*  pvPos;
    float       fTop, fBottom;
    UINT        uTextVertices;
    UINT        i;
    HRESULT     hr;

    // Lock the position buffer
    hr = g_pd3drPosition->Lock(0, g_uVBPositionSize, &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }
    pvPos = (D3DVECTOR*)pData;

    fTop = -g_fTargetHeight - g_fCurrentRow * g_fCharHeight - g_fCharHeight;
    fBottom = -g_fCurrentRow * g_fCharHeight + g_fCharHeight;

    // Identify the visible text vertices
    uTextVertices = g_uTextSize * 4;
    for (i = 0; i < uTextVertices; i += 4) {
        if (pvPos[i].y > fTop && pvPos[i].y < fBottom) {
            g_uVStart1 = i;
            break;
        }
    }
    for (g_uVCount1 = 0; i < uTextVertices && (pvPos[i].y > fTop && pvPos[i].y < fBottom); i += 4, g_uVCount1 += 4);
    for (i += 4; i < uTextVertices; i += 4) {
        if (pvPos[i].y > fTop && pvPos[i].y < fBottom) {
            g_uVStart2 = i;
            break;
        }
    }
    for (g_uVCount2 = 0; i < uTextVertices && (pvPos[i].y > fTop && pvPos[i].y < fBottom); i += 4, g_uVCount2 += 4);

    hr = g_pd3drPosition->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }

    return TRUE;
}

