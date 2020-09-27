#include <multimon.h>

// A few more API to simply getting Montiro info when the HMONITOR is not needed by the caller
extern BOOL WINAPI MonitorInfoFromWindow(HWND,    LPMONITORINFO, UINT);
extern BOOL WINAPI MonitorInfoFromRect  (LPCRECT, LPMONITORINFO, UINT);
extern BOOL WINAPI MonitorInfoFromPoint (POINT,   LPMONITORINFO, UINT);

// The following functions are from MSDN and are helpers for centering 
// rects and windows.
#define MONITOR_CENTER   0x0001        // center rect to monitor
#define MONITOR_CLIP     0x0000        // clip rect to monitor
#define MONITOR_WORKAREA 0x0002        // use monitor work area
#define MONITOR_AREA     0x0000        // use monitor entire area

extern void WINAPI ClipOrCenterRectToMonitor(LPRECT prc, UINT flags);
extern void WINAPI ClipOrCenterWindowToMonitor(HWND hwnd, UINT flags);


