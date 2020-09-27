/*
    FILE:   panel.c
    DATE:   5/3/00

    This file holds code to support the control panel by writing
    the registry for the given board.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "mywin.h"
#include "regfuncs.h"
#include "debug.h"
#include "..\..\common\inc\nvreg.h"
#include "..\..\common\win9x\inc\escape.h"
#include "restime.h"
#include "osapi.h"
#include "utils.h"
#include "..\code\edid.h"
#include "..\code\gtfmath.h"
#include "..\code\dmt.h"

typedef struct  _DESKTOP_STATE_DATA
{
    char    szName[8];
    int     nID;
}   DESKTOP_STATE_DATA;

DESKTOP_STATE_DATA  DesktopStateData[] =
{
    { NV4_REG_DRV_DESKTOP_STATE_NORMAL,     NVTWINVIEW_STATE_NORMAL },
    { NV4_REG_DRV_DESKTOP_STATE_MULTI_MON,  NVTWINVIEW_STATE_MULTIMON },
    { NV4_REG_DRV_DESKTOP_STATE_CLONE,      NVTWINVIEW_STATE_CLONE },
    { "\0",                                 NVTWINVIEW_STATE_NORMAL }
};


/*
    Function:   SetVirtualDesktop

    Purpose:    This routine writes the registry key VirtualDesktop
                with a "1" if dwState is non-zero or deletes the key
                if dwState = 0.

    Arguments:  lpRegData   - identifies the specific board
                dwLogDevice - 0 based logical device number
                dwState     - If 1 -> enable the key
                              if 0 -> delete it

    Returns:    Always returns TRUE
*/
int WINAPI
SetVirtualDesktop (LPREGDATA lpRegData, ULONG dwLogDevice, ULONG dwState)
{
    DBG_PRINT0 (DL_0, "\r\nEntering SetVirtualDesktop.");

    if  (dwState)
        SetLocalNvidiaDisplayLogRegString (lpRegData, dwLogDevice,
                    NV4_REG_DRV_VIRTUALDESKTOP, NV4_REG_DRV_TRUE, 1);
    else
        DeleteLocalNvidiaDisplayLogRegEntry (lpRegData, dwLogDevice,
                    NV4_REG_DRV_VIRTUALDESKTOP);

    DBG_PRINT0 (DL_1, "\r\nExiting SetVirtualDesktop.");
    return  (TRUE);
}


/*
    Function:   GetVirtualDesktop

    Purpose:    This routine reads the registry key VirtualDesktop

    Arguments:  lpRegData   - identifies the specific board
                dwLogDevice - 0 based logical device number
                lpState     - pointer to variable for return value

    Returns:    Always returns TRUE
*/
int WINAPI
GetVirtualDesktop (LPREGDATA lpRegData, ULONG dwLogDevice, ULONG *lpState)
{
    DBG_PRINT0 (DL_0, "\r\nEntering GetVirtualDesktop.");

    lpState[0] = 0;
    GetLocalNvidiaDisplayLogRegString (lpRegData, dwLogDevice,
                        NV4_REG_DRV_VIRTUALDESKTOP, (LPCHAR) lpState);
    if  (lpState[0] == '1')
        *lpState = 1;
    else
        *lpState = 0;

    DBG_PRINT0 (DL_1, "\r\nExiting GetVirtualDesktop.");
    return  (TRUE);
}


/*
    Function:   SetDesktopState

    Purpose:    This routine writes the registry key DesktopState with
                the desired desktop state.

    Arguments:  lpRegData   - identifies the specific board
                dwState     -  0 -> NVTWINVIEW_STATE_NORMAL
                               1 -> NVTWINVIEW_STATE_MULTIMON
                               2 -> NVTWINVIEW_STATE_CLONE

    Returns:    Always returns TRUE
*/
int WINAPI
SetDesktopState (LPREGDATA lpRegData, ULONG dwState)
{
    DBG_PRINT0 (DL_0, "\r\nEntering SetDesktopState.");

    if  (dwState > NVTWINVIEW_STATE_CLONE)
        dwState = NVTWINVIEW_STATE_NORMAL;

    SetLocalNvidiaRegValue (lpRegData, NV4_REG_DRV_DESKTOPSTATE, (int) dwState);

    DBG_PRINT0 (DL_1, "\r\nExiting SetDesktopState.");
    return  (TRUE);
}


/*
    Function:   GetDesktopState

    Purpose:    This routine reads the registry key DesktopState

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpState     - pointer to variable for return value

    Returns:    Always returns TRUE
*/
int WINAPI
GetDesktopState (LPREGDATA lpRegData, ULONG *lpState)
{

    DBG_PRINT0 (DL_0, "\r\nEntering GetDesktopState.");

    *lpState = GetLocalNvidiaRegValue (lpRegData, NV4_REG_DRV_DESKTOPSTATE, 0);

    DBG_PRINT0 (DL_1, "\r\nExiting GetDesktopState.");
    return  (TRUE);
}


/*
    Function:   SetAutoPanMode

    Purpose:    This routine writes the registry key AutoPanMode
                with the coordinates of the Top Left corner of the display
                if dwState = 1 (disable auto-panning: freeze origin)
                or deletes the key if dwState = 0 (enable auto-panning:
                unfreeze origin).

    Arguments:  lpRegData   - identifies the specific board
                dwState     - If 1 -> write the key
                              if 0 -> delete the key
                lpDevData   - LPDEVDATA
                dwLeft      - if dwState is TRUE, then the auto pan
                dwTop       - is locked at this rectangle.
                dwRight
                dwBottom

    Returns:    Always returns TRUE
*/
int WINAPI
SetAutoPanMode (LPREGDATA lpRegData, LPDEVDATA lpDevData, ULONG dwLeft,
                ULONG dwTop, ULONG dwRight, ULONG dwBottom, ULONG dwState)
{
    char    szTemp[256];

    DBG_PRINT0 (DL_0, "\r\nEntering SetAutoPanMode.");

    if  (dwState)
    {
        wsprintf (szTemp, "%ld,%ld,%ld,%ld", dwLeft, dwTop, dwRight, dwBottom);
        SetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                                    NV4_REG_DRV_AUTOPANMODE, szTemp, 1);
    }
    else
        DeleteLocalNvidiaDisplayDevRegEntry (lpRegData, lpDevData,
                                            NV4_REG_DRV_AUTOPANMODE);

    DBG_PRINT0 (DL_1, "\r\nExiting SetAutoPanMode.");
    return  (TRUE);
}


/*
    Function:   GetAutoPanMode

    Purpose:    This routine reads the registry key AutoPanMode

    Arguments:  lpRegData   - identifies the specific board that we
                lpDevData   - LPDEVDATA
                lpState     - pointer to variable for return value
                lpLeft      - if the autopan key exists dwState is TRUE, then the auto pan
                lpTop       - is locked at this rectangle.
                lpRight
                lpBottom

    Returns:    Always returns TRUE
*/
int WINAPI
GetAutoPanMode (LPREGDATA lpRegData, LPDEVDATA lpDevData, ULONG *lpLeft,
                ULONG *lpTop, ULONG *lpRight, ULONG *lpBottom)
{
    char    szTemp[256];
    ULONG   dwIndex;

    DBG_PRINT0 (DL_0, "\r\nEntering GetAutoPanMode.");

    szTemp[0] = 0;
    GetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                                        NV4_REG_DRV_AUTOPANMODE, szTemp);

    if (szTemp[0] != 0)
    {
        dwIndex = 0;
        ParseDecimalSeparator (szTemp, &dwIndex, lpLeft, ',', ',');
        ParseDecimalSeparator (szTemp, &dwIndex, lpTop, ',', ',');
        ParseDecimalSeparator (szTemp, &dwIndex, lpRight, ',', ',');
        ParseDecimalSeparator (szTemp, &dwIndex, lpBottom, 0, 0);
    }
    else
        return (FALSE);

    DBG_PRINT0 (DL_1, "\r\nExiting GetAutoPanMode.");
    return  (TRUE);
}


/*
    Function:   SetTimingOverride

    Purpose:    This routine writes the registry key MonitorTiming
                or deletes the key if dwState = 0 (AUTO).

    Arguments:  lpRegData   - identifies the specific board
                lpDevData   - LPDEVDATA
                dwState     - If 0 -> Delete the key (AUTO)
                dwState     - If 1 -> DMT
                              if 2 -> GTF

    Returns:    Always returns TRUE
*/
int WINAPI
SetTimingOverride (LPREGDATA lpRegData, LPDEVDATA lpDevData, ULONG dwState)
{
    char    szTemp[256];

    DBG_PRINT0 (DL_0, "\r\nEntering SetTimingOverride.");

    if (dwState == 1)
    {
        wsprintf (szTemp, "DMT");
        SetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                                NV4_REG_DRV_MONITOR_TIMING, szTemp, 1);
    }
    else if (dwState == 2)
    {
        wsprintf (szTemp, "GTF");
        SetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                                NV4_REG_DRV_MONITOR_TIMING, szTemp, 1);
    }
    else
        DeleteLocalNvidiaDisplayDevRegEntry (lpRegData, lpDevData,
                                NV4_REG_DRV_MONITOR_TIMING);

    DBG_PRINT0 (DL_1, "\r\nExiting SetTimingOverride.");
    return  (TRUE);
}


/*
    Function:   GetTimingOverride

    Purpose:    This routine reads the registry key MonitorTiming

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpDevData   - LPDEVDATA
                lpState     - pointer to variable for return value

    Returns:    Always returns TRUE
*/
int WINAPI
GetTimingOverride (LPREGDATA lpRegData, LPDEVDATA lpDevData, ULONG *lpState)
{
    char    szTemp[256];

    DBG_PRINT0 (DL_0, "\r\nEntering GetTimingOverride.");

    szTemp[0] = 0;
    GetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                            NV4_REG_DRV_MONITOR_TIMING, szTemp);

    if (szTemp[0] != 0)
    {
        if  (!lstrcmpi(szTemp, NV4_REG_DRV_MONITOR_TIMING_DMT))
            *lpState = 1;
        else if  (!lstrcmpi(szTemp, NV4_REG_DRV_MONITOR_TIMING_GTF))
            *lpState = 2;
        else
            *lpState = 0;
    }
    else
        return (FALSE);

    DBG_PRINT0 (DL_1, "\r\nExiting GetTimingOverride.");
    return  (TRUE);
}

