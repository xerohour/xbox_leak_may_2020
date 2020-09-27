/*----------------------------------------------------------------------------*\
|   MMHELP.H - routines to help a app be multimonitor aware
\*----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
//
// I use this contant everywhere for the fWork.
//
const BOOL c_fWork = TRUE ;

///////////////////////////////////////////////////////////////////////////////
//
// Get the rectangle of the monitor containing a point.
//
void multiMonitorRectFromPoint(/*in*/  POINT ptScreenCoords, 
							   /*out*/ LPRECT prc, 
							   /*in*/  BOOL fWork);
///////////////////////////////////////////////////////////////////////////////
//
// Get the rectangle of the monitor containing the rectangle.
//
void multiMonitorRectFromRect(/*in*/  RECT rcScreenCoords, 
							  /*out*/ LPRECT prc, 
							  /*in*/  BOOL fWork) ;

///////////////////////////////////////////////////////////////////////////////
//
//
int multiGetCXMetrics(HWND hWnd) ;
int multiGetCYMetrics(HWND hWnd) ;



void GetMonitorRect(HWND hwnd, LPRECT prc, BOOL fWork);
void ClipRectToMonitor(HWND hwnd, RECT *prc, BOOL fWork);
void CenterRectToMonitor(HWND hwnd, RECT *prc, BOOL fWork);
void CenterWindowToMonitor(HWND hwndP, HWND hwnd, BOOL fWork);
void ClipWindowToMonitor(HWND hwndP, HWND hwnd, BOOL fWork);
BOOL IsWindowOnScreen(HWND hwnd);
void MakeSureWindowIsVisible(HWND hwnd);
