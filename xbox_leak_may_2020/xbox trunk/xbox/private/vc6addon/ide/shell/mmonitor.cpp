/*----------------------------------------------------------------------------*\
|   MMONITOR.CPP - routines to help a app be multimonitor aware
\*----------------------------------------------------------------------------*/

#include "stdafx.h"

#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#include "mmonitor.h"

#include <math.h> //for abs

///////////////////////////////////////////////////////////////////////////////
//
// Direct replacement for GetSysmteMetrics(SM_CYSCREEN) ;
//
//
int multiGetCYMetrics(HWND hwnd)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;
	ASSERT(::IsWindow(hwnd)) ;

    // Core
	RECT rect ;
	GetMonitorRect(hwnd, &rect, c_fWork) ;
	return abs(rect.bottom - rect.top) ;
}

///////////////////////////////////////////////////////////////////////////////
//
// Direct replacement for GetSystemMetrics(SM_CXSCREEN) ;
//
//
int multiGetCXMetrics(HWND hwnd)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;
	ASSERT(::IsWindow(hwnd)) ;

    // Core
	RECT rect ;
	GetMonitorRect(hwnd, &rect, c_fWork) ;
	return abs(rect.right - rect.left) ;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the rectangle of the monitor containing a point.
//
void multiMonitorRectFromPoint(/*in*/  POINT ptScreenCoords, 
							   /*out*/ LPRECT prc, 
							   /*in*/  BOOL fWork)
{
	// precondition
	ASSERT(prc != NULL) ;
	ASSERT(AfxIsValidAddress(prc, sizeof(RECT))) ;

	// Get the monitor which contains the point.
	HMONITOR hMonitor = MonitorFromPoint(ptScreenCoords, MONITOR_DEFAULTTOPRIMARY) ;
	ASSERT(hMonitor != NULL) ;
	
	// Prepare to get the information for this monitor.
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);

	// Get the rect of this monitor.
    VERIFY(GetMonitorInfo(hMonitor, &mi));
	
	// Return the rectangle.
    if (fWork)
        *prc = mi.rcWork;
    else
        *prc = mi.rcMonitor;

}
///////////////////////////////////////////////////////////////////////////////
//
// Get the rectangle of the monitor containing the rectangle.
//
void multiMonitorRectFromRect(/*in*/  RECT rcScreenCoords, 
							  /*out*/ LPRECT prc, 
							  /*in*/  BOOL fWork)
{
	//Preconditions
	ASSERT(prc != NULL) ;
	ASSERT(AfxIsValidAddress(prc, sizeof(RECT))) ;

	// Get monitor which contains this rectangle.
	HMONITOR hMonitor = ::MonitorFromRect(&rcScreenCoords, MONITOR_DEFAULTTOPRIMARY) ;
	ASSERT(hMonitor != NULL) ;

	// Prepare to get the information for this monitor.
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);

	// Get the rect of this monitor.
    VERIFY(GetMonitorInfo(hMonitor, &mi));
	
	// Return the rectangle.
    if (fWork)
        *prc = mi.rcWork;
    else
        *prc = mi.rcMonitor;
}

///////////////////////////////////////////////////////////////////////////////
//
//  GetMonitorRect
//
//  gets the "screen" or work area of the monitor that the passed
//  window is on.  this is used for apps that want to clip or
//  center windows.
//
//  the most common problem apps have with multimonitor systems is
//  when they use GetSystemMetrics(SM_C?SCREEN) to center or clip a
//  window to keep it on screen.  If you do this on a multimonitor
//  system the window we be restricted to the primary monitor.
//
//  this is a example of how you used the new Win32 multimonitor APIs
//  to do the same thing.
//
void GetMonitorRect(HWND hwnd, LPRECT prc, BOOL fWork)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;
	ASSERT(::IsWindow(hwnd)) ;

    // Core
	MONITORINFO mi;

    mi.cbSize = sizeof(mi);
    GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);

    if (fWork)
        *prc = mi.rcWork;
    else
        *prc = mi.rcMonitor;
}

//
// ClipRectToMonitor
//
// uses GetMonitorRect to clip a rect to the monitor that
// the passed window is on.
//
void ClipRectToMonitor(HWND hwnd, RECT *prc, BOOL fWork)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;
	ASSERT(::IsWindow(hwnd)) ;

    // Core
    RECT rc;
    int  w = prc->right  - prc->left;
    int  h = prc->bottom - prc->top;

    if (hwnd != NULL)
    {
        GetMonitorRect(hwnd, &rc, fWork);
    }
    else
    {
        MONITORINFO mi;

        mi.cbSize = sizeof(mi);
        GetMonitorInfo(MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST), &mi);

        if (fWork)
            rc = mi.rcWork;
        else
            rc = mi.rcMonitor;
    }

    prc->left   = max(rc.left, min(rc.right-w,  prc->left));
    prc->top    = max(rc.top,  min(rc.bottom-h, prc->top));
    prc->right  = prc->left + w;
    prc->bottom = prc->top  + h;
}

//
// CenterRectToMonitor
//
// uses GetMonitorRect to center a rect to the monitor that
// the passed window is on.
//
void CenterRectToMonitor(HWND hwnd, RECT *prc, BOOL fWork)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;

    // Core
    RECT rc;
    int  w = prc->right  - prc->left;
    int  h = prc->bottom - prc->top;

    GetMonitorRect(hwnd, &rc, fWork);

    prc->left	= rc.left + (rc.right  - rc.left - w) / 2;
    prc->top	= rc.top  + (rc.bottom - rc.top  - h) / 2;
    prc->right	= prc->left + w;
    prc->bottom = prc->top  + h;
}

//
// CenterWindowToMonitor
//
void CenterWindowToMonitor(HWND hwndP, HWND hwnd, BOOL fWork)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;
	ASSERT(hwndP != NULL) ;

    // Core
    RECT rc;
    GetWindowRect(hwnd, &rc);
    CenterRectToMonitor(hwndP, &rc, fWork);
    SetWindowPos(hwnd, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//
// ClipWindowToMonitor
//
void ClipWindowToMonitor(HWND hwndP, HWND hwnd, BOOL fWork)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;
	ASSERT(hwndP != NULL) ;

    // Core
    RECT rc;
    GetWindowRect(hwnd, &rc);
    ClipRectToMonitor(hwndP, &rc, fWork);
    SetWindowPos(hwnd, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//
// IsWindowOnScreen
//
BOOL IsWindowOnScreen(HWND hwnd)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;

    // Core
    HDC hdc;
    RECT rc;
    BOOL f;

    GetWindowRect(hwnd, &rc);
    hdc = GetDC(NULL);
    f = RectVisible(hdc, &rc);
    ReleaseDC(NULL, hdc);
    return f;
}

//
// MakeSureWindowIsVisible
//
void MakeSureWindowIsVisible(HWND hwnd)
{
	// Preconditions
	ASSERT(hwnd != NULL) ;

    // Core
    if (!IsWindowOnScreen(hwnd))
    {
		ClipWindowToMonitor(hwnd, hwnd, TRUE);
    }
}
