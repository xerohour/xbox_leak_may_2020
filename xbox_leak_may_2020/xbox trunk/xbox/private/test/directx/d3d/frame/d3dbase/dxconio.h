/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

Author:

    Matt Bronder

File Name:

    dxconio.h

*******************************************************************************/

#ifndef __DXCONIO_H__
#define __DXCONIO_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define DXCIOAPI __cdecl
#define DXCONIOAPI

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define MAX_TCHAR_OUTPUT        4096

#define CHAR_ASPECT             1.875f

#define XSCROLL_LINEUP          0x0001
#define XSCROLL_LINELEFT        0x0002
#define XSCROLL_LINEDOWN        0x0004
#define XSCROLL_LINERIGHT       0x0008
#define XSCROLL_PAGEUP          0x0010
#define XSCROLL_PAGELEFT        0x0020
#define XSCROLL_PAGEDOWN        0x0040
#define XSCROLL_PAGERIGHT       0x0080
#define XSCROLL_TOP             0x0100
#define XSCROLL_LEFT            0x0200
#define XSCROLL_BOTTOM          0x0400
#define XSCROLL_RIGHT           0x0800
#define XSCROLL_HORZ            0x1000
#define XSCROLL_VERT            0x2000

#define RGBA_XCOLOR(r, g, b, a) ((XCOLOR)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

//******************************************************************************
// Data types
//******************************************************************************

typedef DWORD                   XCOLOR;

//******************************************************************************
// stdio functions
//******************************************************************************

// xprintf, xwprintf, and xtprintf

DXCONIOAPI
int
DXCIOAPI
xprintf(
    const char *, 
    ...
);
DXCONIOAPI
int
DXCIOAPI
xwprintf(
    const wchar_t *,
    ...
);
#ifndef UNICODE
#define xtprintf            xprintf
#else
#define xtprintf            xwprintf
#endif // UNICODE

// xvprintf, xvwprintf, and xvtprintf

DXCONIOAPI
int
DXCIOAPI
xvprintf(
    const char *,
    va_list
);
DXCONIOAPI
int
DXCIOAPI
xvwprintf(
    const wchar_t *,
    va_list
);
#ifndef UNICODE
#define xvtprintf           xvprintf
#else
#define xvtprintf           xvwprintf
#endif // UNICODE

// xputchar, xputwchar, and xputtchar

DXCONIOAPI
int
DXCIOAPI
xputchar(
    int
);
DXCONIOAPI
wint_t
DXCIOAPI
xputwchar(
    wint_t
);
#ifndef UNICODE
#define xputtchar           xputchar
#else
#define xputtchar           xputwchar
#endif // UNICODE

// xputs, xputws, and xputts

DXCONIOAPI
int
DXCIOAPI
xputs(
    const char *
);
DXCONIOAPI
int
DXCIOAPI
xputws(
    const wchar_t *
);
#ifndef UNICODE
#define xputts              xputs
#else
#define xputts              xputws
#endif // UNICODE

//******************************************************************************
// Console management functions
//******************************************************************************

// xCreateConsole

DXCONIOAPI
BOOL
DXCIOAPI
xCreateConsole(
    LPVOID pvDevice
);

// xReleaseConsole

DXCONIOAPI
void
DXCIOAPI
xReleaseConsole(
    void
);

// xClearScreen

DXCONIOAPI
void
DXCIOAPI
xClearScreen(
    void
);

// xSetCharSize

DXCONIOAPI
void
DXCIOAPI
xSetCharSize(
    float fWidth,
    float fHeight
);

// xSetTextColor

DXCONIOAPI
XCOLOR
DXCIOAPI
xSetTextColor(
    BYTE red, 
    BYTE green, 
    BYTE blue, 
    BYTE alpha
);

// xSetBackgroundColor

DXCONIOAPI
XCOLOR
DXCIOAPI
xSetBackgroundColor(
    BYTE red,
    BYTE green,
    BYTE blue,
    BYTE alpha
);

// xSetBackgroundImageA, xSetBackgroundImageW, and xSetBackgroundImage

DXCONIOAPI
BOOL
DXCIOAPI
xSetBackgroundImageA(
    LPCSTR pszImage
);
DXCONIOAPI
BOOL
DXCIOAPI
xSetBackgroundImageW(
    LPCWSTR pszImage
);
#ifndef UNICODE
#define xSetBackgroundImage xSetBackgroundImageA
#else
#define xSetBackgroundImage xSetBackgroundImageW
#endif // UNICODE

// xScrollConsoleX

DXCONIOAPI
void
DXCIOAPI
xScrollConsole(
    DWORD  dwScroll,
    float* pfHorz,
    float* pfVert
);

// xSetOutputBufferSize

DXCONIOAPI
BOOL
DXCIOAPI
xSetOutputBufferSize(
    UINT uBytes
);

// xSetDisplayWidth

DXCONIOAPI
BOOL
DXCIOAPI
xSetDisplayWidth(
    UINT uWidth
);

// xDisplayConsole

DXCONIOAPI
void
DXCIOAPI
xDisplayConsole(
    void
);

// xResetConsole

DXCONIOAPI
BOOL
DXCIOAPI
xResetConsole(
    void
);

// xTextOutA, xTextOutW, and xTextOut

DXCONIOAPI
BOOL
DXCIOAPI
xTextOutA(
    float   fXStart,
    float   fYStart,
    float   fCWidth,
    float   fCHeight,
    XCOLOR  xcColor,
    LPCSTR  pszString,
    UINT    uCount
);
DXCONIOAPI
BOOL
DXCIOAPI
xTextOutW(
    float   fXStart,
    float   fYStart,
    float   fCWidth,
    float   fCHeight,
    XCOLOR  xcColor,
    LPCWSTR pszString,
    UINT    uCount
);
#ifndef UNICODE
#define xTextOut            xTextOutA
#else
#define xTextOut            xTextOutW
#endif // UNICODE

// xDebugStringA, xDebugStringW, and xDebugString

DXCONIOAPI
void
DXCIOAPI
xDebugStringA(
    LPCSTR pszFormat,
    ...
);
DXCONIOAPI
void
DXCIOAPI
xDebugStringW(
    LPCWSTR pszFormat, 
    ...
);
#ifndef UNICODE
#define xDebugString        xDebugStringA
#else
#define xDebugString        xDebugStringW
#endif // UNICODE

//******************************************************************************
// stdio overrides
//******************************************************************************

#ifdef  printf
#undef  printf
#endif // printf
#define printf                  xprintf

#ifdef  wprintf
#undef  wprintf
#endif // wprintf
#define wprintf                 xwprintf

#ifdef  _tprintf
#undef  _tprintf
#endif // _tprintf
#define _tprintf                xtprintf

#ifdef  vprintf
#undef  vprintf
#endif // vprintf
#define vprintf                 xvprintf

#ifdef  vwprintf
#undef  vwprintf
#endif // vwprintf
#define vwprintf                xvwprintf

#ifdef  _vtprintf
#undef  _vtprintf
#endif // _vtprintf
#define _vtprintf               xvtprintf

#ifdef  putchar
#undef  putchar
#endif // putchar
#define putchar                 xputchar

#ifdef  putwchar
#undef  putwchar
#endif // putwchar
#define putwchar                xputwchar

#ifdef  _puttchar
#undef  _puttchar
#endif // _puttchar
#define _puttchar               xputtchar

#ifdef  puts
#undef  puts
#endif // puts
#define puts                    xputs

#ifdef  _putws
#undef  _putws
#endif // _putws
#define _putws                  xputws

#ifdef  _putts
#undef  _putts
#endif // _puttchar
#define _putts                  xputts

//******************************************************************************
// Debug output overrides
//******************************************************************************

#ifndef DXCONIO_NO_OVERRIDES

#undef  OutputDebugString
#define OutputDebugString(x)    xDebugString(TEXT("%s"), x)

#endif // !DXCONIO_NO_OVERRIDES

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DXCONIO_H__
