/*
    FILE:   getit.c
    DATE:   4/8/99

    This file holds code to get the mode, and output device from
    the registry for the given board.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
//#include "..\..\common\inc\nvreg.h"
#include "restime.h"
#include "osapi.h"
#include "utils.h"
#include "::code:edid.h"
#include "::code:gtfmath.h"

typedef struct  tagDEVNAMEANDTYPE
{
    LPCHAR  lpszName;
    LPCHAR  lpszForceModeName;
    LPCHAR  lpszLastDevMode;
    LPCHAR  lpszDefRefName;
    LPCHAR  lpszOptRefName;
    ULONG   dwDevType;
    ULONG   dwTvType;
} DEVNAMEANDTYPE;

ULONG CFUNC GetPitch(LPDISPDATA lpDispData, ULONG dwXRes, ULONG dwBpp);
ULONG CFUNC GetTimingStandardOverride(LPDISPDATA lpDispData);
int WINAPI GetDisplayDevice(LPDISPDATA lpDispData, LPULONG lpDevType, LPULONG lpTvFormat);
int WINAPI SetDisplayDevice(LPDISPDATA lpDispData, ULONG dwDevType, ULONG dwTvFormat);
int WINAPI SetLastDisplayDevice(LPDISPDATA lpDispData, ULONG dwDevType, ULONG dwTvFormat);
int WINAPI SetDisplayDeviceMode(LPDISPDATA lpDispData, LPMODEINFO lpModeInfo);
int WINAPI GetEdidRegistryKey(LPDISPDATA lpDispData);
int WINAPI GetDisplayOverrideKey(LPDISPDATA lpDispData);
int CFUNC GrabModeInfoString(LPDISPDATA lpDispData, LPCHAR lpModeString, LPMODEINFO lpMI);
int CFUNC GetMonitorInfRangeLimits(LPDISPDATA lpDispData, LPMONITORLIMITS lpML, ULONG *lpdwXRes, ULONG *lpdwYRes);
ULONG CFUNC GetDefaultRefreshRate(LPDISPDATA lpDispData, ULONG dwDevType);
ULONG CFUNC GetOptimalRefreshRate(LPDISPDATA lpDispData, LPMODEINFO lpModeInfo, LPCHAR lpEdidBuffer, ULONG dwEdidSize);
int CFUNC GrabRefreshRateFromKey(ULONG hKey, LPDISPDATA lpDispData, LPCHAR lpszPath, LPMODEINFO lpModeInfo, LPCHAR lpEdidBuffer, ULONG dwEdidSize);
int WINAPI GetForcedMode(LPDISPDATA lpDispData, LPMODEINFO lpSuggMode, LPCHAR lpEdidBuffer, ULONG dwEdidSize);
int WINAPI GetFirstBootMode(LPDISPDATA lpDispData, LPMODEINFO lpModeInfo, LPCHAR lpEdidBuffer, ULONG dwEdidSize);
int WINAPI GetDisplayDeviceMode(LPDISPDATA lpDispData, LPMODEINFO lpModeInfo, LPCHAR lpEdidBuffer, ULONG dwEdidSize, ULONG dwFirstBootLogic);
int CFUNC GetMonitorLimitMode(LPDISPDATA lpDispData, LPMODEINFO lpModeInfo);



/*
    Function:   GetPitch

    Purpose:    This routine returns the pitch for a given mode.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                dwXRes      - The X res in pixels for the mode
                              should be returned.
                dwBpp       - the bits per pixel for the mode

    Returns:    The pitch in bytes
*/
typedef ULONG   (*LPFNGETMODESIZE) (ULONG, ULONG, ULONG, ULONG, ULONG, ULONG *, ULONG *);
ULONG CFUNC GetPitchAndSize
(
    LPDISPDATA  lpDispData,
    ULONG       dwXRes,
    ULONG       dwYRes,
    ULONG       dwBpp,
    ULONG      *pdwPitch,
    ULONG      *pdwSize
)
{
    LPFNGETMODESIZE lpfnGetModeSize;

    lpfnGetModeSize = (LPFNGETMODESIZE) lpDispData->lpfnGetModeSize;
    return  ((*lpfnGetModeSize) (lpDispData->dwContext1, lpDispData->dwContext2, dwXRes, dwYRes, dwBpp, pdwPitch, pdwSize));
}


/*
    GetTimingStandardOverride

    This routine gets any timing standard override for the board
    specified. It should return one of the constants defined in
    restime.h like DMTV_ID. A timing standard override is a
    timing standard that applies to all modes. Just something
    that the user might want to enforce.
*/
ULONG CFUNC GetTimingStandardOverride
(
    LPDISPDATA lpDispData
)
{

/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (NONE_ID);
}


/*
    Function:   GetDisplayDevice

    Purpose:    This routine gets the display device for the board
                specified. The particular display device is stored
                in the registry.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpDevType   - ptr to where the basic display type
                              should be returned.
                lpTvFormart - ptr to where the specific flavor of
                              the tv type should be returned.

    Returns:    lpDevType should be filled in with one of the
                DEVTYPE_XXXX constants defined in modeext.h
                If lpDevType is set to DEVTYPE_NTSC or DEVTYPE_PAL,
                then lpTvFormat should be set to one of the
                TVTYPE_XXXX constants defined in modeext.h
*/
int WINAPI GetDisplayDevice
(
    LPDISPDATA lpDispData,
    LPULONG lpDevType,
    LPULONG lpTvFormat
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */


    *lpDevType  = DEVTYPE_CRT;
    *lpTvFormat = 0; // Always 0 for CRTC
    return  (TRUE);
}


/*
    Function:   SetDisplayDevice

    Purpose:    This routine sets the display device for the board
                specified. The particular display device is stored
                in the registry.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                dwDevType   - the basic display type
                              should be one of the DEVTYPE_XXXX
                              sonstants definde in modeext.h
                dwTvFormart - the specific flavor of the tv type
                              should be one of the TVTYPE_XXXX
                              sonstants definde in modeext.h

    Returns:    Always returns TRUE
*/
int WINAPI SetDisplayDevice
(
    LPDISPDATA lpDispData,
    ULONG dwDevType,
    ULONG dwTvFormat
)
{

/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (TRUE);
}


/*
    Function:   SetLastDisplayDevice

    Purpose:    This routine sets the display device for the board
                specified. The particular display device is stored
                in the registry. This routine is subtley different
                than SetDisplayDevice.  It is a different key in
                the registry and is used by the caller to track
                mode changes, etc.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                dwDevType   - the basic display type
                              should be one of the DEVTYPE_XXXX
                              sonstants definde in modeext.h
                dwTvFormart - the specific flavor of the tv type
                              should be one of the TVTYPE_XXXX
                              sonstants definde in modeext.h

    Returns:    Always returns TRUE
*/
int WINAPI SetLastDisplayDevice
(
    LPDISPDATA lpDispData,
    ULONG dwDevType,
    ULONG dwTvFormat
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (TRUE);
}


/*
    Function:   SetDisplayDeviceMode

    Purpose:    This routine writes a string in the registry
                inidcating the xres,yres,bpp,refrate for a
                given display device (CRT,PAL,NTSC,DFP) on
                a specific board (given by the location of
                the string in the registry.)

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - tells the mode and display device
                              to write.

    Returns:    Always returns TRUE
*/
int WINAPI SetDisplayDeviceMode
(
    LPDISPDATA lpDispData,
    LPMODEINFO lpModeInfo
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (TRUE);
}


/*
    Function:   GetEdidRegistryKey

    Purpose:    This routine gets the registry key that tells
                whether or not we are supposed to read the EDID.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.

    Returns:    0       registry key does not exist or says not to
                        read the EDID
                non-0   registry key exists and says to read the EDID
*/
int WINAPI GetEdidRegistryKey
(
    LPDISPDATA lpDispData
)
{
    int     nFlag;

/*
 * CHASCHAS - How will we do this on Mac?
 */
    nFlag = 0;
    return  (nFlag);
}


/*
    Function:   GetDisplayOverrideKey

    Purpose:    This routine reads the key that determines if we
                are supposed to force to a particular display
                device (like DFP over CRT, etc.)

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.

    Returns:
                DEVTYPE_CRT registry key exists and says to use CRT
                DEVTYPE_DFP registry key exists and says to use DFP
                else        registry key does not exist or does not
                            specify one of the possible display override
                            options.
*/
int WINAPI GetDisplayOverrideKey
(
    LPDISPDATA lpDispData
)
{
    int     nFlag;

/*
 * CHASCHAS - How will we do this on Mac?
 */

    nFlag = DEVTYPE_NONE;
    return  (nFlag);
}


/*
    Function:   GrabModeInfoString

    Purpose:    This routine reads and parses a string of the
                form "xres,yres,bpp,refrate" from the registry.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeString - Name of entry in registry (relative
                              to XXXX\NVidia\Display) that tells the
                              specific mode string to read.
                lpMI        - MODEINFO ptr where parsed mode goes

    Returns:    TRUE  that a valid mode was parsed and stored in lpMI
                FALSE there was a problem with the parsing and lpMI
                does not contain valid data.

                NOTE: Even if this routine returns TRUE, it does not
                mean the mode is reasonable. 1,1,55,9876 would be parsed
                fine, but it is not a valid mode string. The caller
                must check for validity if it needs to ensure this.
*/
int CFUNC GrabModeInfoString
(
    LPDISPDATA lpDispData,
    LPCHAR lpModeString,
    LPMODEINFO lpMI
)
{
    int     bModeValid = FALSE;

/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (bModeValid);
}


/*
    Function:   GetMonitorInfRangeLimits

    Purpose:    This routine reads the "MaxResolution" key in the
                active monitor node in the registry to get the
                maximum resolution the monitor can do. It uses
                the maximum resolution itself (like "1600,1200")
                as another subkey under which is typically a
                monitor range limits string which looks like
                "30-95,60-120,-,+" where the first range is
                the horizontal limits inKHz and the second is
                the vertical range in hertz. The last two characters
                are horizontal and vertical sync polarities.
                This range limits string doesn't always exist.

    Arguments:
                lpDispData - identifies the specific board that we
                              want the active display port for.
                lpML        - ptr to a monitor limits structure
                              that will be filled in with range
                              limits obtained or computed from
                              values stuffed in the registry
                              by the moitor .inf file.
                lpdwXRes    - ptr to a variable that will be set
                              to the maximum X-resolution found
                              for this monitor.
                lpdwYRes    - ptr to a variable that will be set
                              to the maximum Y-resolution found
                              for this monitor.

    Returns:    TRUE    then lpML, lpdwXRes, and lpdwYRes have been
                        filled in correctly.
                FALSE   then there was an error getting some value.
*/
int CFUNC GetMonitorInfRangeLimits
(
    LPDISPDATA lpDispData,
    LPMONITORLIMITS lpML,
    ULONG *lpdwXRes,
    ULONG *lpdwYRes
)
{
    if (lpDispData->edidSize == 0)
	    /*
	     * When there's no EDID, just continue processing as normal.
	     */
	    return (FALSE);        
    /*
     * When there's an EDID and this routine is called, that means
     * the EDID didn't have any range limits, but there has been
     * a maximum pixel clock calculated.  Set up a dummy monitor
     * limits table and return true so that modes that use pixel
     * clocks higher than the max pixel clock calculated will get
     * rejected.  All other modes will be accepted.
     */
    /*
     * This just shows that we don't know what the maximum
     * horizontal and vertical values are.
     */
    lpML->dwMaxHorz = 0;
    lpML->dwMaxVert = 0;
    return (TRUE);
}


/*
    Function:   GetDefaultRefreshRate

    Purpose:    This routine reads a key in the registry to get
                a specific value for the default refresh rate.
                There is an optional OEM specific key that can
                specify what default should mean. If the key does
                not exist, then 60 is returned.

    Arguments:
                lpDispData - identifies the specific board that we
                              want the active display port for.
                dwDevType   - tells which display device port is being
                              used since there might be different
                              default refresh rates for DFPs and CRTs.

    Returns:    always returns a value that is the default refresh rate
*/
ULONG CFUNC GetDefaultRefreshRate
(
    LPDISPDATA lpDispData,
    ULONG dwDevType
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (60);
}


/*
    Function:   GetOptimalRefreshRate

    Purpose:    This routine reads a key in the registry to get
                a specific value for the default refresh rate.
                There is an optional OEM specific key that can
                specify what default should mean. If the key does
                not exist, then 60 is returned.

    Arguments:
                lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - Current mode and device identifier.
                lpEdidBuffer - points to a buffer with EDID data in
                              it if dwEdidSize is not 0. If dwEdidSize
                              is 0, this argument is ignored.
                dwEdidSize    tells the size of the lpEdidBuffer
                              in bytes. If this variables is 0,
                              then lpEdidBuffer ptr may be invalid.

    Returns:    always returns a value that is the optimal refresh rate
*/
ULONG CFUNC GetOptimalRefreshRate
(
    LPDISPDATA lpDispData,
    LPMODEINFO lpModeInfo,
    LPCHAR lpEdidBuffer,
    ULONG dwEdidSize
)
{

/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (60);
}



/*
    Function:   GrabRefreshRateFromKey

    Purpose:    This routine reads the "RefreshRate" string value
                from the key and path provided and does a little
                preprocessing on what it finds. i.e. it handles
                optimal (-1) and default (0) cases.

    Arguments:  hKey        base Key in registry from which to read
                lpDispData - identifies the specific board that we
                              want the active display port for.
                lpszPath    path in registry to access which may
                            contain a "RefreshRate" string.
                lpModeInfo  - Current mode and device identifier.
                lpEdidBuffer - points to a buffer with EDID data in
                              it if dwEdidSize is not 0. If dwEdidSize
                              is 0, this argument is ignored.
                dwEdidSize    tells the size of the lpEdidBuffer
                              in bytes. If this variables is 0,
                              then lpEdidBuffer ptr may be invalid.

    Returns:    TRUE  then lpModeInfo->dwRefreshRate has a real
                      value stuffed in it. Also the field
                      lpModeInfo->dwOriginalRefreshRate has the
                      "Windows" value in it which might be the same
                      as ->dwRefreshRate if Windows was requesting
                      a specific refresh rate, or it can be 0 for
                      default or -1 for optimal.
                FALSE if nothing was found or there was an error.
*/
int CFUNC GrabRefreshRateFromKey
(
    ULONG hKey,
    LPDISPDATA lpDispData,
    LPCHAR lpszPath,
    LPMODEINFO lpModeInfo,
    LPCHAR lpEdidBuffer,
    ULONG dwEdidSize
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (FALSE);
}


/*
    Function:   GetForcedMode

    Purpose:    This routine gte s a forced mode if one exists
                in the registry. It then erases the forced mode
                string and processes it appropriately.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - contains the display device on which
                              the mode will be set and the mode
                              suggested by the caller.
                lpEdidBuffer - points to a buffer with EDID data in
                              it if dwEdidSize is not 0. If dwEdidSize
                              is 0, this argument is ignored.
                dwEdidSize    tells the size of the lpEdidBuffer
                              in bytes. If this variables is 0,
                              then lpEdidBuffer ptr may be invalid.

    Returns:    This routine may override the mode provided in the
                lpModeInfo structure. Whether it does or not, it
                MUST do the following:
                If Windows wants a specific refresh rate value,
                then both the lpModeInfo->dwRefreshRate and the
                lpModeInfo->dwOriginalRefreshRate fields must be set
                to this specific value (liek 60, 75, 100, etc).
                If Windows wants the optimal rate, then the
                lpModeInfo->dwRefreshRate should be set to the
                specific value for the optimal refresh rate and
                lpModeInfo->dwOriginalRefreshRate should be set to -1.
                If Windows wants the default refresh rate, then the
                lpModeInfo->dwRefreshRate should be set to the
                specific value for the default refresh rate and
                lpModeInfo->dwOriginalRefreshRate should be set to 0.
*/
int WINAPI GetForcedMode
(
    LPDISPDATA lpDispData,
    LPMODEINFO lpSuggMode,
    LPCHAR lpEdidBuffer,
    ULONG dwEdidSize
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */
    return  (FALSE);
}



/*
    Function:   GetFirstBootMode

    Purpose:    This routine does some special logic for first
                boot after a restart.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - contains the display device on which
                              the mode will be set and the mode
                              suggested by the caller.
                lpEdidBuffer - points to a buffer with EDID data in
                              it if dwEdidSize is not 0. If dwEdidSize
                              is 0, this argument is ignored.
                dwEdidSize    tells the size of the lpEdidBuffer
                              in bytes. If this variables is 0,
                              then lpEdidBuffer ptr may be invalid.

    Returns:    This routine may override the mode provided in the
                lpModeInfo structure. Whether it does or not, it
                MUST do the following:
                If Windows wants a specific refresh rate value,
                then both the lpModeInfo->dwRefreshRate and the
                lpModeInfo->dwOriginalRefreshRate fields must be set
                to this specific value (liek 60, 75, 100, etc).
                If Windows wants the optimal rate, then the
                lpModeInfo->dwRefreshRate should be set to the
                specific value for the optimal refresh rate and
                lpModeInfo->dwOriginalRefreshRate should be set to -1.
                If Windows wants the default refresh rate, then the
                lpModeInfo->dwRefreshRate should be set to the
                specific value for the default refresh rate and
                lpModeInfo->dwOriginalRefreshRate should be set to 0.
*/
int WINAPI GetFirstBootMode
(
    LPDISPDATA lpDispData,
    LPMODEINFO lpModeInfo,
    LPCHAR lpEdidBuffer,
    ULONG dwEdidSize
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */
    return  (FALSE);
}


/*
    Function:   GetDisplayDeviceMode

    Purpose:    This routine gets the correct display mode for
                the device specified by lpDispData and the
                display device given by lpModeInfo->dwDevType.
                lpModeInfo->dwXRes, lpModeInfo->dwYRes, and
                lpModeInfo->dwBpp are valid, but the field
                lpModeInfo->dwRefreshRate is not.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - contains the display device on which
                              the mode will be set and the mode
                              suggested by the caller.
                lpEdidBuffer - points to a buffer with EDID data in
                              it if dwEdidSize is not 0. If dwEdidSize
                              is 0, this argument is ignored.
                dwEdidSize    tells the size of the lpEdidBuffer
                              in bytes. If this variables is 0,
                              then lpEdidBuffer ptr may be invalid.
                dwFirstBootLogic  If this variable is non-0, then
                              this is the first boot after a power
                              up.  This is just a flag so that this
                              routine can use different logic if it
                              wants to.

    Returns:    This routine may override the mode provided in the
                lpModeInfo structure. Whether it does or not, it
                MUST do the following:
                If Windows wants a specific refresh rate value,
                then both the lpModeInfo->dwRefreshRate and the
                lpModeInfo->dwOriginalRefreshRate fields must be set
                to this specific value (liek 60, 75, 100, etc).
                If Windows wants the optimal rate, then the
                lpModeInfo->dwRefreshRate should be set to the
                specific value for the optimal refresh rate and
                lpModeInfo->dwOriginalRefreshRate should be set to -1.
                If Windows wants the default refresh rate, then the
                lpModeInfo->dwRefreshRate should be set to the
                specific value for the default refresh rate and
                lpModeInfo->dwOriginalRefreshRate should be set to 0.

                It Always returns TRUE.
*/
int WINAPI GetDisplayDeviceMode
(
    LPDISPDATA lpDispData,
    LPMODEINFO lpModeInfo,
    LPCHAR lpEdidBuffer,
    ULONG dwEdidSize,
    ULONG dwFirstBootLogic
)
{

/*
 * CHASCHAS - How will we do this on Mac?
 */


    // Well, we just have to set up a default refresh rate then
    lpModeInfo->dwRefreshRate = 60;
    lpModeInfo->dwOriginalRefreshRate = lpModeInfo->dwRefreshRate;

    return  (TRUE);
}


/*
    Function:   GetMonitorLimitMode

    Purpose:    This routine gets the mode which should be an
                upper bound for a monitor which has no edid and
                no .inf.

    Arguments:  lpDispData - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - ptr to a place to store the limit mode

    Returns:    If this routine returns TRUE, then lpModeInfo was
                filled in with the limit mode. If this routine
                returns FALSE, then there is no limit mode.
*/
int CFUNC GetMonitorLimitMode
(
    LPDISPDATA lpDispData,
    LPMODEINFO lpModeInfo
)
{
    return  (GrabModeInfoString (lpDispData, NULL, lpModeInfo));
}

/*
    Function:   GetAllowAllModesFlag

    Purpose:    WIN9X specific function for testing only.

    Arguments:
                lpDispData - identifies the specific board that we
                              want the active display port for.
    Returns:    FALSE.
*/
ULONG WINAPI GetAllowAllModesFlag
(
    LPREGDATA lpRegData
)
{
    return (FALSE);
}


