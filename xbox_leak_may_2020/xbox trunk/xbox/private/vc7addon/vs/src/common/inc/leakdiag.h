/* LeakDiag.h

	Copyright (c) 2001 Microsoft Corporation.	All Rights Reserved.

	Abstract:
        LeakDiag detects leaks of GDI and USER object handles.

***************************************************
Author: MikhailA
First revision: 05-14-2001
***************************************************

The following objects are handled

GDI
	HDC
    HPEN
    HBRUSH
    HBITMAP
    HFONT
    HRGN
    HPALETTE
    HCOLORSPACE

USER
    HMENU
    HACCEL
    HICON
    HCURSOR

COMCTL
    HIMAGELIST


How does it work?

LeakDiag redefines GDI and USER functions (such as CreateBitmap or DeleteObject) so the call 
first goes to the leakdiag.dll along with the location (file name and the line #) where the allocation
took place. The DLL maintains a list of allocated handles along with file names and line numbers.
When the correspondent destructor (DeleeObject, ReleaseDC, etc) get called, the call again goes
to the leakdiag.dll and the handle gets removed from the list.

LeakDiag will assert if the destructor fails (for example, when you use DeleteObject instead 
of DestroyCursor on a cursor handle).


How to use

1. Include leakdiag.h after system and CRT headers, but before your local header files

2. Add a reference to the leakdiag.lib in your sources file	BEFORE system libraries. 
In VS7 packages the best place is next to the vsassert.lib

!IF ("$(BUILD_RETAIL)" == "no")
        $(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vsassert.lib \
        $(VSBUILT)\$(BUILT_FLAVOR)\lib\*\leakdiag.lib \
!ENDIF

3. Run suites or just play with the application, executing code that allocates and destroys
GDI and USER object.

4. Close the application. If there are leaks, you'll see them in the debugger output window,
like

LeakDiag: handle leak at c:\foo\bar.cpp (123)

OR

LeakDiag: no leaks detected

5. If you have code that creates an object and then hands it over to another module
that is not under the leakdiag control, you will see a leak since the leakdiag.dll
nevers sees a call that deletes the object. To fix this you may call
LeakDiag_RemoveHandle() before you hand the object over to another module. This
will remove the handle from the leakdiag list of handles so it won't report 
a bogus leak.


Current (5/14/01) version does not assert on leaks since it is not fully tested so there
may be some noise in the reports and I don't want suites to break without a reason.
For the same reason leakdiag is not yet permanently active. 

IMPORTANT NOTICE:

One may see that in the header some GDI/USER functions are redeclared using macros
while some redeclared as functions. All functions started as macros. but some VS7 modules
required them to be declared as functions. For instance, if module declares its own
LoadBitmap as a member of a class, the macro won't work since the code won't compile.
This is done on a case by case basis, as needed.

*/

#pragma once

#define LEAKDIAG_ACTIVE                         0x01
#define LEAKDIAG_ASSERT_LEAKS                   0x02
#define LEAKDIAG_OUTPUT_DEBUG_STRING            0x04
#define LEAKDIAG_ASSERT_REDUNDANT_DELETEOBJECT  0x08

#ifdef DEBUG

#define _LEAKDIAG_ACTIVE_

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _LEAKDIAG_IMPL
#define LD_DLLEXPORT __declspec(dllexport) __stdcall
#else
#define LD_DLLEXPORT __declspec(dllimport) __stdcall
#endif

HANDLE   LD_DLLEXPORT   LeakDiag_AddHandle(HANDLE hObjToAdd, LPCSTR pszFile, int nLine);
HANDLE   LD_DLLEXPORT   LeakDiag_RemoveHandle(HANDLE hObj);
void     LD_DLLEXPORT   LeakDiag_DumpLeaks(void);
DWORD    LD_DLLEXPORT   LeakDiag_SetMode(DWORD dwMode);
BOOL     LD_DLLEXPORT   LeakDiag_DeleteObject(HGDIOBJ h);
BOOL     LD_DLLEXPORT   LeakDiag_EndPaint(HWND hwnd, CONST PAINTSTRUCT* pps);
HMENU    LD_DLLEXPORT   LeakDiag_CreateMenu(LPCSTR pszFile, int nLine);
HMENU    LD_DLLEXPORT   LeakDiag_CreatePopupMenu(LPCSTR pszFile, int nLine);
HICON    LD_DLLEXPORT   LeakDiag_LoadIconA(HINSTANCE hInst, LPCSTR lpszIcon, LPCSTR pszFile, int nLine);
HICON    LD_DLLEXPORT   LeakDiag_LoadIconW(HINSTANCE hInst, LPCWSTR lpszIcon, LPCSTR pszFile, int nLine);
HANDLE   LD_DLLEXPORT   LeakDiag_LoadImageA(HINSTANCE hInst, LPCSTR lpszName, UINT uType, int cxDesired, int cyDesired, UINT fuLoad, LPCSTR pszFile, int nLine);
HANDLE   LD_DLLEXPORT   LeakDiag_LoadImageW(HINSTANCE hInst, LPCWSTR lpszName, UINT uType, int cxDesired, int cyDesired, UINT fuLoad, LPCSTR pszFile, int nLine);
UINT     LD_DLLEXPORT   LeakDiag_ExtractIconExA(LPCSTR pszIconFile, int index, HICON* phLarge, HICON* hSmall, UINT uiIcon, LPCSTR pszFile, int nLine);
UINT     LD_DLLEXPORT   LeakDiag_ExtractIconExW(LPCWSTR pszIconFile, int index, HICON* phLarge, HICON* hSmall, UINT uiIcon, LPCSTR pszFile, int nLine);

#ifdef __cplusplus
}
#endif

#ifndef LEAKDIAG_NO_REDEFINITION

#ifndef _LEAKDIAG_IMPL

#ifdef __cplusplus

template <class T>
T LeakDiag_Convert(HANDLE h)
{ return static_cast<T>(h); }

// General
#define DeleteObject(h)         LeakDiag_DeleteObject(h)

#ifdef LEAKDIAG_VBDC
// DC
#define VBDeleteDC(hdc)           VBDeleteDC(LeakDiag_Convert<HDC>(LeakDiag_RemoveHandle(hdc)))
#define VBReleaseDC(hwnd, hdc)    VBReleaseDC(hwnd, LeakDiag_Convert<HDC>(LeakDiag_RemoveHandle(hdc)))
#define VBEndPaint(hwnd, ps)      LeakDiag_EndPaint(hwnd, ps)

#define VBBeginPaint(hwnd, ps)        LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBBeginPaint(hwnd, ps), __FILE__, __LINE__))
#define VBGetDC(hwnd)                 LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBGetDC(hwnd), __FILE__, __LINE__))
#define VBGetDCEx(hwnd, hrgn, dw)     LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBGetDCEx(hwnd, hrgn, dw), __FILE__, __LINE__))
#define VBGetWindowDC(hwnd)           LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBGetWindowDC(hwnd), __FILE__, __LINE__))
#define VBCreateDCA(s1, s2, s3, dm)   LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBCreateDCA(s1, s2, s3, dm), __FILE__, __LINE__))
#define VBCreateDCW(s1, s2, s3, dm)   LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBCreateDCW(s1, s2, s3, dm), __FILE__, __LINE__))
#define VBCreateCompatibleDC(a)       LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::VBCreateCompatibleDC(a), __FILE__, __LINE__))

#else
// DC
#define DeleteDC(hdc)           DeleteDC(LeakDiag_Convert<HDC>(LeakDiag_RemoveHandle(hdc)))
#define ReleaseDC(hwnd, hdc)    ReleaseDC(hwnd, LeakDiag_Convert<HDC>(LeakDiag_RemoveHandle(hdc)))
#define EndPaint(hwnd, ps)      LeakDiag_EndPaint(hwnd, ps)

#define BeginPaint(hwnd, ps)        LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::BeginPaint(hwnd, ps), __FILE__, __LINE__))
#define GetDC(hwnd)                 LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::GetDC(hwnd), __FILE__, __LINE__))
#define GetDCEx(hwnd, hrgn, dw)     LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::GetDCEx(hwnd, hrgn, dw), __FILE__, __LINE__))
#define GetWindowDC(hwnd)           LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::GetWindowDC(hwnd), __FILE__, __LINE__))
#define CreateDCA(s1, s2, s3, dm)   LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::CreateDCA(s1, s2, s3, dm), __FILE__, __LINE__))
#define CreateDCW(s1, s2, s3, dm)   LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::CreateDCW(s1, s2, s3, dm), __FILE__, __LINE__))
#define CreateCompatibleDC(a)       LeakDiag_Convert<HDC>(LeakDiag_AddHandle(::CreateCompatibleDC(a), __FILE__, __LINE__))

#endif // LEAKDIAG_VBDC

// Pens
#define CreatePen(a, b, c)                      LeakDiag_Convert<HPEN>(LeakDiag_AddHandle(::CreatePen(a, b, c), __FILE__, __LINE__))
#define CreatePenIndirect(a)                    LeakDiag_Convert<HPEN>(LeakDiag_AddHandle(::CreatePenIndirect(a), __FILE__, __LINE__))
#define ExtCreatePen(dw1, dw2, lb, dw3, pdw)    LeakDiag_Convert<HPEN>(LeakDiag_AddHandle(::ExtCreatePen(dw1, dw2, lb, dw3, pdw), __FILE__, __LINE__))

// Brushes
#define CreateBrush(a, b, c)            LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreateBrush(a, b, c), __FILE__, __LINE__))
#define CreateBrushIndirect(a)          LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreateBrushIndirect(a), __FILE__, __LINE__))
#define CreateDIBPatternBrushPt(a, b)   LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreateDIBPatternBrushPt(a, b), __FILE__, __LINE__))
#define CreateDIBPatternBrush(a, b)     LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreateDIBPatternBrush(a, b), __FILE__, __LINE__))
#define CreateHatchBrush(a, b)          LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreateHatchBrush(a, b), __FILE__, __LINE__))
#define CreatePatternBrush(a)           LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreatePatternBrush(a), __FILE__, __LINE__))
#define CreateSolidBrush(a)             LeakDiag_Convert<HBRUSH>(LeakDiag_AddHandle(::CreateSolidBrush(a), __FILE__, __LINE__))

// Bitmaps
#define CreateBitmap(a1, a2, a3, a4, a5)            LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::CreateBitmap(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define CreateBitmapIndirect(a)                     LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::CreateBitmapIndirect(a), __FILE__, __LINE__))
#define CreateCompatibleBitmap(a1, a2, a3)          LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::CreateCompatibleBitmap(a1, a2, a3), __FILE__, __LINE__))
#define CreateDIBitmap(a1, a2, a3, a4, a5, a6)      LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::CreateDIBitmap(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define CreateDIBSection(a1, a2, a3, a4, a5, a6)    LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::CreateDIBSection(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define LoadBitmapA(a1, a2)                         LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::LoadBitmapA(a1, a2), __FILE__, __LINE__))
#define LoadBitmapW(a1, a2)                         LeakDiag_Convert<HBITMAP>(LeakDiag_AddHandle(::LoadBitmapW(a1, a2), __FILE__, __LINE__))

// Icons
#define LoadIconA(a1, a2)                                       LeakDiag_LoadIconA(a1, a2, __FILE__, __LINE__)
#define LoadIconW(a1, a2)                                       LeakDiag_LoadIconW(a1, a2, __FILE__, __LINE__)
#define DuplicateIcon(a1, a2)                                   LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::DuplicateIcon(a1, a2), __FILE__, __LINE__))
#define CopyIcon(a1)                                            LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::CopyIcon(a1), __FILE__, __LINE__))
#define ExtractIconA(a1, a2, a3)                                LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::ExtractIconA(a1, a2, a3), __FILE__, __LINE__))
#define ExtractIconW(a1, a2, a3)                                LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::ExtractIconW(a1, a2, a3), __FILE__, __LINE__))
#define ExtractIconExA(a1, a2, a3, a4, a5)                      LeakDiag_ExtractIconExA(a1, a2, a3, a4, a5, __FILE__, __LINE__)
#define ExtractIconExW(a1, a2, a3, a4, a5)                      LeakDiag_ExtractIconExW(a1, a2, a3, a4, a5, __FILE__, __LINE__)
#define ExtractAssociatedIconA(a1, a2, a3)                      LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::ExtractAssociatedIconA(a1, a2, a3), __FILE__, __LINE__))
#define ExtractAssociatedIconW(a1, a2, a3)                      LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::ExtractAssociatedIconW(a1, a2, a3), __FILE__, __LINE__))
#define CreateIcon(a1, a2, a3, a4, a5, a6, a7)                  LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::CreateIcon(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define CreateIconFromResource(a1, a2, a3, a4)                  LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::CreateIconFromResource(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateIconFromResourceEx(a1, a2, a3, a4, a5, a6, a7)    LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::CreateIconFromResourceEx(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define CreateIconIndirect(a1)                                  LeakDiag_Convert<HICON>(LeakDiag_AddHandle(::CreateIconIndirect(a1), __FILE__, __LINE__))
#define DestroyIcon(h)                                          DestroyIcon(LeakDiag_Convert<HICON>(LeakDiag_RemoveHandle(h)))

// Fonts
#define CreateFontA(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)  LeakDiag_Convert<HFONT>(LeakDiag_AddHandle(::CreateFontA(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14), __FILE__, __LINE__))
#define CreateFontW(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)  LeakDiag_Convert<HFONT>(LeakDiag_AddHandle(::CreateFontW(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14), __FILE__, __LINE__))
#define CreateFontIndirectA(a)                                                    LeakDiag_Convert<HFONT>(LeakDiag_AddHandle(::CreateFontIndirectA(a), __FILE__, __LINE__))
#define CreateFontIndirectW(a)                                                    LeakDiag_Convert<HFONT>(LeakDiag_AddHandle(::CreateFontIndirectW(a), __FILE__, __LINE__))
#define CreateFontIndirectExA(a)                                                  LeakDiag_Convert<HFONT>(LeakDiag_AddHandle(::CreateFontIndirectExA(a), __FILE__, __LINE__))
#define CreateFontIndirectExW(a)                                                  LeakDiag_Convert<HFONT>(LeakDiag_AddHandle(::CreateFontIndirectExW(a), __FILE__, __LINE__))

// Cursors

//#define CopyCursor(a1)                              LeakDiag_Convert<HCURSOR>(LeakDiag_AddHandle(::CopyCursor(a1), __FILE__, __LINE__))
#define CreateCursor(a1, a2, a3, a4, a5, a6, a7)    LeakDiag_Convert<HCURSOR>(LeakDiag_AddHandle(::CreateCursor(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define LoadCursorFromFileA(a1)                     LeakDiag_Convert<HCURSOR>(LeakDiag_AddHandle(::LoadCursorFromFileA(a1), __FILE__, __LINE__))
#define LoadCursorFromFileW(a1)                     LeakDiag_Convert<HCURSOR>(LeakDiag_AddHandle(::LoadCursorFromFileW(a1), __FILE__, __LINE__))
#define DestroyCursor(h)                            DestroyCursor(LeakDiag_Convert<HCURSOR>(LeakDiag_RemoveHandle(h)))

// Images
#ifdef VSD_VSCOMMODULE_H_INCLUDED_ 
// HACK(MikhailA): hack for VS7 deployment package. Remove when deploy starts using common Unicode wrapper
#define LoadImageW(a1, a2, a3, a4, a5)   LeakDiag_LoadImageW(a1, a2, a3, a4, a5, __FILE__, __LINE__)
#else
#define LoadImageW(a1, a2, a3, a4, a5, a6)   LeakDiag_LoadImageW(a1, a2, a3, a4, a5, a6, __FILE__, __LINE__)
#endif

#define LoadImageA(a1, a2, a3, a4, a5, a6)   LeakDiag_LoadImageA(a1, a2, a3, a4, a5, a6, __FILE__, __LINE__)
#define CopyImage(a1, a2, a3, a4, a5)        LeakDiag_AddHandle(::CopyImage(a1, a2, a3, a4, a5), __FILE__, __LINE__)

// Palette
#define CreatePalette(a1)           LeakDiag_Convert<HPALETTE>(LeakDiag_AddHandle(::CreatePalette(a1), __FILE__, __LINE__))
#define CreateHalftonePalette(a1)   LeakDiag_Convert<HPALETTE>(LeakDiag_AddHandle(::CreateHalftonePalette(a1), __FILE__, __LINE__))

// Accelerators
#define LoadAcceleratorsA(a1, a2)   LeakDiag_Convert<HACCEL>(LeakDiag_AddHandle(::LoadAcceleratorsA(a1, a2), __FILE__, __LINE__))
#define LoadAcceleratorsW(a1, a2)   LeakDiag_Convert<HACCEL>(LeakDiag_AddHandle(::LoadAcceleratorsW(a1, a2), __FILE__, __LINE__))
#define DestroyAcceleratorTable(h)  DestroyAcceleratorTable(LeakDiag_Convert<HACCEL>(LeakDiag_RemoveHandle(h)))

// Menu
#define LoadMenuA(a1, a2)     LeakDiag_Convert<HMENU>(LeakDiag_AddHandle(::LoadMenuA(a1, a2), __FILE__, __LINE__))
#define LoadMenuW(a1, a2)     LeakDiag_Convert<HMENU>(LeakDiag_AddHandle(::LoadMenuW(a1, a2), __FILE__, __LINE__))
#define LoadMenuIndirectA(a1) LeakDiag_Convert<HMENU>(LeakDiag_AddHandle(::LoadMenuIndirectA(a1), __FILE__, __LINE__))
#define LoadMenuIndirectW(a1) LeakDiag_Convert<HMENU>(LeakDiag_AddHandle(::LoadMenuIndirectW(a1), __FILE__, __LINE__))
//#define CreateMenu           LeakDiag_CreateMenu
//#define CreatePopupMenu      LeakDiag_CreatePopupMenu
#define DestroyMenu(h)        DestroyMenu((HMENU)LeakDiag_RemoveHandle(h))

// Colorspace
#define CreateColorSpaceA(a1)    LeakDiag_Convert<HCOLORSPACE>(LeakDiag_AddHandle(::CreateColorSpaceA(a1), __FILE__, __LINE__))
#define CreateColorSpaceW(a1)    LeakDiag_Convert<HCOLORSPACE>(LeakDiag_AddHandle(::CreateColorSpaceW(a1), __FILE__, __LINE__))
#define DeleteColorSpace(h)     DeleteColorSpace(LeakDiag_Convert<HCOLORSPACE>(LeakDiag_RemoveHandle(h)))

// Regions
#define CreateEllipticRgn(a1, a2, a3, a4)           LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreateEllipticRgn(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateEllipticRgnIndirect(a1)               LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreateEllipticRgnIndirect(a1), __FILE__, __LINE__))
#define CreatePolygonRgn(a1, a2, a3)                LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreatePolygonRgn(a1, a2, a3), __FILE__, __LINE__))
#define CreatePolyPolygonRgn(a1, a2, a3, a4)        LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreatePolyPolygonRgn(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateRectRgn(a1, a2, a3, a4)               LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreateRectRgn(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateRectRgnIndirect(a1)                   LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreateRectRgnIndirect(a1), __FILE__, __LINE__))
#define CreateRoundRectRgn(a1, a2, a3, a4, a5, a6)  LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::CreateRoundRectRgn(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define ExtCreateRegion(a1, a2, a3)                 LeakDiag_Convert<HRGN>(LeakDiag_AddHandle(::ExtCreateRegion(a1, a2, a3), __FILE__, __LINE__))
#define SetWindowRgn(hWnd, hRgn, bRedraw)           SetWindowRgn(hWnd, LeakDiag_Convert<HRGN>(LeakDiag_RemoveHandle(hRgn)), bRedraw)

// ImageList
#define ImageList_LoadImageA(a1, a2, a3, a4, a5, a6, a7)    LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_LoadImageA(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define ImageList_LoadImageW(a1, a2, a3, a4, a5, a6, a7)    LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_LoadImageW(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define ImageList_LoadBitmapA(a1, a2, a3, a4, a5)           LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_LoadBitmapA(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define ImageList_LoadBitmapW(a1, a2, a3, a4, a5)           LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_LoadBitmapW(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define ImageList_Merge(a1, a2, a3, a4, a5, a6)             LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_Merge(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define ImageList_Read(a1)                                  LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_Read(a1), __FILE__, __LINE__))
#define ImageList_Duplicate(a1)                             LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_Duplicate(a1), __FILE__, __LINE__))
#define ImageList_Create(a1, a2, a3, a4, a5)                LeakDiag_Convert<HIMAGELIST>(LeakDiag_AddHandle(::ImageList_Create(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define ImageList_Destroy(h)                                ImageList_Destroy(LeakDiag_Convert<HIMAGELIST>(LeakDiag_RemoveHandle(h)))

#else

// plain C

// General
#define DeleteObject(h)         LeakDiag_DeleteObject(h)

#ifdef LEAKDIAG_VBDC

// DC
#define VBDeleteDC(hdc)           VBDeleteDC((HDC)LeakDiag_RemoveHandle(hdc))
#define VBReleaseDC(hwnd, hdc)    VBReleaseDC(hwnd, (HDC)LeakDiag_RemoveHandle(hdc))
#define VBEndPaint(hwnd, ps)      LeakDiag_EndPaint(hwnd, ps)

#define VBBeginPaint(hwnd, ps)        (HDC)LeakDiag_AddHandle(VBBeginPaint(hwnd, ps), __FILE__, __LINE__))
#define VBGetDC(hwnd)                 ((HDC)LeakDiag_AddHandle(VBGetDC(hwnd), __FILE__, __LINE__))
#define VBGetDCEx(hwnd, hrgn, dw)     ((HDC)LeakDiag_AddHandle(VBGetDCEx(hwnd, hrgn, dw), __FILE__, __LINE__))
#define VBGetWindowDC(hwnd)           ((HDC)LeakDiag_AddHandle(VBGetWindowDC(hwnd), __FILE__, __LINE__))
#define VBCreateDCA(s1, s2, s3, dm)   ((HDC)LeakDiag_AddHandle(VBCreateDCA(s1, s2, s3, dm), __FILE__, __LINE__))
#define VBCreateDCW(s1, s2, s3, dm)   ((HDC)LeakDiag_AddHandle(VBCreateDCW(s1, s2, s3, dm), __FILE__, __LINE__))
#define VBCreateCompatibleDC(a)       ((HDC)LeakDiag_AddHandle(VBCreateCompatibleDC(a), __FILE__, __LINE__))

#else
// DC
#define DeleteDC(hdc)           DeleteDC((HDC)LeakDiag_RemoveHandle(hdc))
#define ReleaseDC(hwnd, hdc)    ReleaseDC(hwnd, (HDC)LeakDiag_RemoveHandle(hdc))
#define EndPaint(hwnd, ps)      LeakDiag_EndPaint(hwnd, ps)

#define BeginPaint(hwnd, ps)        (HDC)LeakDiag_AddHandle(BeginPaint(hwnd, ps), __FILE__, __LINE__))
#define GetDC(hwnd)                 ((HDC)LeakDiag_AddHandle(GetDC(hwnd), __FILE__, __LINE__))
#define GetDCEx(hwnd, hrgn, dw)     ((HDC)LeakDiag_AddHandle(GetDCEx(hwnd, hrgn, dw), __FILE__, __LINE__))
#define GetWindowDC(hwnd)           ((HDC)LeakDiag_AddHandle(GetWindowDC(hwnd), __FILE__, __LINE__))
#define CreateDCA(s1, s2, s3, dm)   ((HDC)LeakDiag_AddHandle(CreateDCA(s1, s2, s3, dm), __FILE__, __LINE__))
#define CreateDCW(s1, s2, s3, dm)   ((HDC)LeakDiag_AddHandle(CreateDCW(s1, s2, s3, dm), __FILE__, __LINE__))
#define CreateCompatibleDC(a)       ((HDC)LeakDiag_AddHandle(CreateCompatibleDC(a), __FILE__, __LINE__))

#endif // LEAKDIAG_VBDC

// Pens
#define CreatePen(a, b, c)                      ((HPEN)LeakDiag_AddHandle(CreatePen(a, b, c), __FILE__, __LINE__))
#define CreatePenIndirect(a)                    ((HPEN)LeakDiag_AddHandle(CreatePenIndirect(a), __FILE__, __LINE__))
#define ExtCreatePen(dw1, dw2, lb, dw3, pdw)    ((HPEN)LeakDiag_AddHandle(ExtCreatePen(dw1, dw2, lb, dw3, pdw), __FILE__, __LINE__))

// Brushes
#define CreateBrush(a, b, c)            ((HBRUSH)LeakDiag_AddHandle(::CreateBrush(a, b, c), __FILE__, __LINE__))
#define CreateBrushIndirect(a)          ((HBRUSH)LeakDiag_AddHandle(CreateBrushIndirect(a), __FILE__, __LINE__))
#define CreateDIBPatternBrushPt(a, b)   ((HBRUSH)LeakDiag_AddHandle(CreateDIBPatternBrushPt(a, b), __FILE__, __LINE__))
#define CreateDIBPatternBrush(a, b)     ((HBRUSH)LeakDiag_AddHandle(CreateDIBPatternBrush(a, b), __FILE__, __LINE__))
#define CreateHatchBrush(a, b)          ((HBRUSH)LeakDiag_AddHandle(CreateHatchBrush(a, b), __FILE__, __LINE__))
#define CreatePatternBrush(a)           ((HBRUSH)LeakDiag_AddHandle(CreatePatternBrush(a), __FILE__, __LINE__))
#define CreateSolidBrush(a)             ((HBRUSH)LeakDiag_AddHandle(CreateSolidBrush(a), __FILE__, __LINE__))

// Bitmaps
#define CreateBitmap(a1, a2, a3, a4, a5)            ((HBITMAP)LeakDiag_AddHandle(CreateBitmap(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define CreateBitmapIndirect(a)                     ((HBITMAP)LeakDiag_AddHandle(CreateBitmapIndirect(a), __FILE__, __LINE__))
#define CreateCompatibleBitmap(a1, a2, a3)          ((HBITMAP)LeakDiag_AddHandle(CreateCompatibleBitmap(a1, a2, a3), __FILE__, __LINE__))
#define CreateDIBitmap(a1, a2, a3, a4, a5, a6)      ((HBITMAP)LeakDiag_AddHandle(CreateDIBitmap(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define CreateDIBSection(a1, a2, a3, a4, a5, a6)    ((HBITMAP)LeakDiag_AddHandle(CreateDIBSection(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define LoadBitmapA(a1, a2)                         ((HBITMAP)LeakDiag_AddHandle(LoadBitmapA(a1, a2), __FILE__, __LINE__))
#define LoadBitmapW(a1, a2)                         ((HBITMAP)LeakDiag_AddHandle(LoadBitmapW(a1, a2), __FILE__, __LINE__))

// Icons
#define LoadIconA(a1, a2)                                       LeakDiag_LoadIconA(a1, a2, __FILE__, __LINE__)
#define LoadIconW(a1, a2)                                       LeakDiag_LoadIconW(a1, a2, __FILE__, __LINE__)
#define DuplicateIcon(a1, a2)                                   ((HICON)LeakDiag_AddHandle(DuplicateIcon(a1, a2), __FILE__, __LINE__))
#define CopyIcon(a1)                                            ((HICON)LeakDiag_AddHandle(CopyIcon(a1), __FILE__, __LINE__))
#define ExtractIconA(a1, a2, a3)                                ((HICON)LeakDiag_AddHandle(ExtractIconA(a1, a2, a3), __FILE__, __LINE__))
#define ExtractIconW(a1, a2, a3)                                ((HICON)LeakDiag_AddHandle(ExtractIconW(a1, a2, a3), __FILE__, __LINE__))
#define ExtractIconExA(a1, a2, a3, a4, a5)                      LeakDiag_ExtractIconExA(a1, a2, a3, a4, a5, __FILE__, __LINE__)
#define ExtractIconExW(a1, a2, a3, a4, a5)                      LeakDiag_ExtractIconExW(a1, a2, a3, a4, a5, __FILE__, __LINE__)
#define ExtractAssociatedIconA(a1, a2, a3)                      ((HICON)LeakDiag_AddHandle(ExtractAssociatedIconA(a1, a2, a3), __FILE__, __LINE__))
#define ExtractAssociatedIconW(a1, a2, a3)                      ((HICON)LeakDiag_AddHandle(ExtractAssociatedIconW(a1, a2, a3), __FILE__, __LINE__))
#define CreateIcon(a1, a2, a3, a4, a5, a6, a7)                  ((HICON)LeakDiag_AddHandle(CreateIcon(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define CreateIconFromResource(a1, a2, a3, a4)                  ((HICON)LeakDiag_AddHandle(CreateIconFromResource(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateIconFromResourceEx(a1, a2, a3, a4, a5, a6, a7)    ((HICON)LeakDiag_AddHandle(CreateIconFromResourceEx(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define CreateIconIndirect(a1)                                  ((HICON)LeakDiag_AddHandle(CreateIconIndirect(a1), __FILE__, __LINE__))
#define DestroyIcon(h)                                          DestroyIcon((HICON)LeakDiag_RemoveHandle(h))

// Fonts
#define CreateFontA(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)  ((HFONT)LeakDiag_AddHandle(CreateFontA(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14), __FILE__, __LINE__))
#define CreateFontW(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)  ((HFONT)LeakDiag_AddHandle(CreateFontW(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14), __FILE__, __LINE__))
#define CreateFontIndirectA(a)                                                    ((HFONT)LeakDiag_AddHandle(CreateFontIndirectA(a), __FILE__, __LINE__))
#define CreateFontIndirectW(a)                                                    ((HFONT)LeakDiag_AddHandle(CreateFontIndirectW(a), __FILE__, __LINE__))
#define CreateFontIndirectExA(a)                                                  ((HFONT)LeakDiag_AddHandle(CreateFontIndirectExA(a), __FILE__, __LINE__))
#define CreateFontIndirectExW(a)                                                  ((HFONT)LeakDiag_AddHandle(CreateFontIndirectExW(a), __FILE__, __LINE__))

// Cursors

//#define CopyCursor(a1)                              ((HCURSOR)LeakDiag_AddHandle(CopyCursor(a1), __FILE__, __LINE__))
#define CreateCursor(a1, a2, a3, a4, a5, a6, a7)    ((HCURSOR)LeakDiag_AddHandle(CreateCursor(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define LoadCursorFromFileA(a1)                     ((HCURSOR)LeakDiag_AddHandle(LoadCursorFromFileA(a1), __FILE__, __LINE__))
#define LoadCursorFromFileW(a1)                     ((HCURSOR)LeakDiag_AddHandle(LoadCursorFromFileW(a1), __FILE__, __LINE__))
#define DestroyCursor(h)                            DestroyCursor((HCURSOR)LeakDiag_RemoveHandle(h))

// Images
#define LoadImageA(a1, a2, a3, a4, a5, a6)   LeakDiag_LoadImageA(a1, a2, a3, a4, a5, a6, __FILE__, __LINE__)
#define LoadImageW(a1, a2, a3, a4, a5, a6)   LeakDiag_LoadImageW(a1, a2, a3, a4, a5, a6, __FILE__, __LINE__)
#define CopyImage(a1, a2, a3, a4, a5)        LeakDiag_AddHandle(::CopyImage(a1, a2, a3, a4, a5), __FILE__, __LINE__)

// Palette
#define CreatePalette(a1)           ((HPALETTE)LeakDiag_AddHandle(CreatePalette(a1), __FILE__, __LINE__))
#define CreateHalftonePalette(a1)   ((HPALETTE)LeakDiag_AddHandle(CreateHalftonePalette(a1), __FILE__, __LINE__))

// Accelerators
#define LoadAcceleratorsA(a1, a2)   ((HACCEL)LeakDiag_AddHandle(LoadAcceleratorsA(a1, a2), __FILE__, __LINE__))
#define LoadAcceleratorsW(a1, a2)   ((HACCEL)LeakDiag_AddHandle(LoadAcceleratorsW(a1, a2), __FILE__, __LINE__))
#define DestroyAcceleratorTable(h)  DestroyAcceleratorTable((HACCEL)LeakDiag_RemoveHandle(h))

// Menu
#define LoadMenuA(a1, a2)     ((HMENU)LeakDiag_AddHandle(LoadMenuA(a1, a2), __FILE__, __LINE__))
#define LoadMenuW(a1, a2)     ((HMENU)LeakDiag_AddHandle(LoadMenuW(a1, a2), __FILE__, __LINE__))
#define LoadMenuIndirectA(a1) ((HMENU)LeakDiag_AddHandle(LoadMenuIndirectA(a1), __FILE__, __LINE__))
#define LoadMenuIndirectW(a1) ((HMENU)LeakDiag_AddHandle(LoadMenuIndirectW(a1), __FILE__, __LINE__))
//#define CreateMenu           LeakDiag_CreateMenu
//#define CreatePopupMenu      LeakDiag_CreatePopupMenu
#define DestroyMenu(h)        DestroyMenu((HMENU)LeakDiag_RemoveHandle(h))

// Colorspace
#define CreateColorSpaceA(a1)   ((HCOLORSPACE)LeakDiag_AddHandle(CreateColorSpaceA(a1), __FILE__, __LINE__))
#define CreateColorSpaceW(a1)   ((HCOLORSPACE)LeakDiag_AddHandle(CreateColorSpaceW(a1), __FILE__, __LINE__))
#define DeleteColorSpace(h)     DeleteColorSpace((HCOLORSPACE)LeakDiag_RemoveHandle(h))

// Regions
#define CreateEllipticRgn(a1, a2, a3, a4)           ((HRGN)LeakDiag_AddHandle(CreateEllipticRgn(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateEllipticRgnIndirect(a1)               ((HRGN)LeakDiag_AddHandle(CreateEllipticRgnIndirect(a1), __FILE__, __LINE__))
#define CreatePolygonRgn(a1, a2, a3)                ((HRGN)LeakDiag_AddHandle(CreatePolygonRgn(a1, a2, a3), __FILE__, __LINE__))
#define CreatePolyPolygonRgn(a1, a2, a3, a4)        ((HRGN)LeakDiag_AddHandle(CreatePolyPolygonRgn(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateRectRgn(a1, a2, a3, a4)               ((HRGN)LeakDiag_AddHandle(CreateRectRgn(a1, a2, a3, a4), __FILE__, __LINE__))
#define CreateRectRgnIndirect(a1)                   ((HRGN)LeakDiag_AddHandle(CreateRectRgnIndirect(a1), __FILE__, __LINE__))
#define CreateRoundRectRgn(a1, a2, a3, a4, a5, a6)  ((HRGN)LeakDiag_AddHandle(CreateRoundRectRgn(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define ExtCreateRegion(a1, a2, a3)                 ((HRGN)LeakDiag_AddHandle(ExtCreateRegion(a1, a2, a3), __FILE__, __LINE__))
#define SetWindowRgn(hWnd, hRgn, bRedraw)           SetWindowRgn(hWnd, (HRGN)LeakDiag_RemoveHandle(hRgn), bRedraw)

// ImageList
#define ImageList_LoadImageA(a1, a2, a3, a4, a5, a6, a7)    ((HIMAGELIST)LeakDiag_AddHandle(ImageList_LoadImageA(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define ImageList_LoadImageW(a1, a2, a3, a4, a5, a6, a7)    ((HIMAGELIST)LeakDiag_AddHandle(ImageList_LoadImageW(a1, a2, a3, a4, a5, a6, a7), __FILE__, __LINE__))
#define ImageList_LoadBitmapA(a1, a2, a3, a4, a5)           ((HIMAGELIST)LeakDiag_AddHandle(ImageList_LoadBitmapA(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define ImageList_LoadBitmapW(a1, a2, a3, a4, a5)           ((HIMAGELIST)LeakDiag_AddHandle(ImageList_LoadBitmapW(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define ImageList_Merge(a1, a2, a3, a4, a5, a6)             ((HIMAGELIST)LeakDiag_AddHandle(ImageList_Merge(a1, a2, a3, a4, a5, a6), __FILE__, __LINE__))
#define ImageList_Read(a1)                                  ((HIMAGELIST)LeakDiag_AddHandle(ImageList_Read(a1), __FILE__, __LINE__))
#define ImageList_Duplicate(a1)                             ((HIMAGELIST)LeakDiag_AddHandle(ImageList_Duplicate(a1), __FILE__, __LINE__))
#define ImageList_Create(a1, a2, a3, a4, a5)                ((HIMAGELIST)LeakDiag_AddHandle(ImageList_Create(a1, a2, a3, a4, a5), __FILE__, __LINE__))
#define ImageList_Destroy(h)                                ImageList_Destroy((HIMAGELIST)LeakDiag_RemoveHandle(h))

#endif // __cplusplus

#endif // _LEAKDIAG_IMPL

#endif // LEAKDIAG_NO_REDEFINITION
#else

// release

inline HANDLE LeakDiag_RemoveHandle(HANDLE hObj) { return hObj; }
inline void LeakDiag_SetMode(DWORD) {}

#endif // DEBUG

