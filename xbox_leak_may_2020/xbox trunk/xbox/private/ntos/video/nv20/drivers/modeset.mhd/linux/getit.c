#include "cmntypes.h"
#include "modeext.h"
#include "restime.h"

/*
    Function:   GetPitch

    Purpose:    This routine returns the pitch for a given mode.

    Arguments:  lpBoardData - identifies the specific board that we
                              want the active display port for.
                dwXRes      - The X res in pixels for the mode
                              should be returned.
                dwBpp       - the bits per pixel for the mode

    Returns:    The pitch in bytes
*/
typedef ULONG   (*LPFNGETPITCH) (ULONG, ULONG);

ULONG CFUNC
GetPitch (LPBOARDDATA lpBoardData, ULONG dwXRes, ULONG dwBpp)
{
    LPFNGETPITCH    lpfnGetPitchNew;

    lpfnGetPitchNew = (LPFNGETPITCH) lpBoardData->lpfnGetDisplayPitch;
    return  ((*lpfnGetPitchNew) (dwXRes, dwBpp));
}
/*
    GetTimingStandardOverride

    This routine gets any timing standard override for the board
    specified. It should return one of the constants defined in
    restime.h like DMTV_ID. A timing standard override is a
    timing standard that applies to all modes. Just something
    that the user might want to enforce.
*/
ULONG CFUNC
GetTimingStandardOverride (LPBOARDDATA lpBoardData)
{
    return  (NONE_ID);
}

/*
    Function:   GetMonitorLimitMode

    Purpose:    This routine gets the mode which should be an
                upper bound for a monitor which has no edid and
                no .inf.

    Arguments:  lpBoardData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - ptr to a place to store the limit mode

    Returns:    If this routine returns TRUE, then lpModeInfo was
                filled in with the limit mode. If this routine
                returns FALSE, then there is no limit mode.
*/
int CFUNC
GetMonitorLimitMode (LPBOARDDATA lpBoardData, LPMODEINFO lpModeInfo)
{
    /*
     * CHASCHAS - not yet implemented!!!
     */
//    return  (GrabModeInfoString (lpBoardData, NV4_REG_DRV_MONITOR_LIMIT_MODE, lpModeInfo));
    return (FALSE);
}

