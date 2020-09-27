/*
    FILE:   getit.c
    DATE:   4/8/99

    This file holds code to get the mode, and output device from
    the registry for the given board.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "mywin.h"
#include "regfuncs.h"
#include "debug.h"
#include "..\..\common\inc\nvreg.h"
#include "restime.h"
#include "osapi.h"
#include "utils.h"
#include "..\code\edid.h"
#include "..\code\gtfmath.h"
#include "..\code\dmt.h"

DEVICEANDTYPE   DeviceTypes[] =
{
    { "CRT", DEVTYPE_CRT },
    { "TV",  DEVTYPE_TV },
    { "DFP", DEVTYPE_DFP },
    { NULL,  0 }
};

typedef struct  tagTVFORMATANDTYPE
{
    LPCHAR  lpszName;
    char    cFormat;
}   TVFORMATANDTYPE;

TVFORMATANDTYPE TVFormat[] =
{
    { "NTSCM", TVTYPE_NTSCM },
    { "NTSCJ", TVTYPE_NTSCJ },
    { "PALM",  TVTYPE_PALM },
    { "PALA",  TVTYPE_PALA },
    { "PALN",  TVTYPE_PALN },
    { "PALNC", TVTYPE_PALNC },
    { NULL,    0 }
};

char sz640_480[] = "640,480";
char szSettings[] = "Display\\Settings";
char szControl[] = "System\\CurrentControlSet\\Control";
char szDefUser[] = "Config\\0001\\Display\\Settings";
char szDeviceType[MAX_KEY_LEN];
char szLastDevice[MAX_KEY_LEN];
char szPath[MAX_KEY_LEN];
char szFullPath[MAX_KEY_LEN];
char szExtraKey[MAX_KEY_LEN];

extern int CFUNC GrabRefreshRateFromKey (ULONG hKey, LPDISPDATA lpDispData,
                                        LPCHAR lpszPath, LPMODEINFO lpModeInfo,
                                        LPCHAR lpEdidBuffer, ULONG dwEdidSize,
                                        char *RefreshRateName);
extern LPMODEENTRY CFUNC GetModeListFromRegistry (LPDISPDATA lpDispData);
extern int WINAPI ParseDeviceTypeAndNumber (LPCHAR lpszString, LPDEVDATA lpDevData);

/*
    GetBootDevice

    Purpose:    This routine gets the Initial Boot display device for
                the board given. lpDevtype should be filled in with one
                of the basic types like DEVTYPE_CRT, DEVTYPE_NTSC, etc
                defined in modeext.h or filled with 0xffffffff if subkey
                is not found or invlalid Device type is found.
                If the basic type is either NTSC or PAL, then the
                lpTvFormat should be filled in with the specific NTSC
                or PAL formats like: TVTYPE_NTSC_M, TYTYPE_NTSC_J etc
                which are given in modeext.h

    Arguments:  lpRegData   - identifies the specific board
                lpDevData   - where to place the device found

    Returns:    TRUE    if lpDevType is filled in correctly.
                FALSE   otherwise
*/
int WINAPI
GetBootDevice (LPREGDATA lpRegData, LPDEVDATA lpDevData)
{
    char    szDev[MAX_KEY_LEN];

    DBG_PRINT0 (DL_0, "\r\nEntering GetBootDevice");

    szDeviceType[0] = 0;
    GetLocalNvidiaDisplayRegString (lpRegData,
                            NV4_REG_DRV_BOOT_DEVICE_TYPE, szDev);

    // Parse the device type
    if  (!ParseDeviceTypeAndNumber (szDev, lpDevData))
        return  (FALSE);

    lpDevData->cFormat = 0;

    // If this was a TV, then get the last format
    if  (lpDevData->cType == DEVTYPE_TV)
    {
        // JKTODO -- for now
        lpDevData->cFormat = TVTYPE_NTSCM;
    }

    DBG_PRINT0 (DL_1, "\r\nExiting GetBootDevice");
    return  (TRUE);
}


/*
    GetBootDeviceSelection
    Purpose:    This routine gets the Initial Boot device selection
                for desktop to use HW selected device. (Mobile Behaviour)


    Arguments:  lpRegData   - identifies the specific board

    Returns:    TRUE    If HW selected device will be used as boot device
                FALSE   otherwise
*/
int WINAPI
GetBootDeviceSelection (LPREGDATA lpRegData)
{
    int nRet;

    DBG_PRINT0 (DL_0, "\r\nEntering GetBootDeviceSelection");

    nRet = GetLocalNvidiaDisplayRegValue (lpRegData,
                NV4_REG_DRV_USE_HW_SELECTED_DEVICE, 0);

    DBG_PRINT0 (DL_1, "\r\nExiting GetBootDeviceSelection");
    return  (nRet);
}

/*
    Function:   GetLastDisplayFormat

    Purpose:    This routine gets called to get the last display
                format used on a particular display device. This
                really only applies in the case of a TV. It will
                set the format field of lpDevData to 0 if the device
                type is not DEVTYPE_TV.

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpDevData   - ptr to the DEVDATA structure to be filled in

    Returns:    lpDevData filled in correctly
*/
int WINAPI
GetLastDisplayFormat (LPREGDATA lpRegData, LPDEVDATA lpDevData)
{
    int     i;
    char    szTvFormat[16];

    DBG_PRINT0 (DL_0, "\r\nEntering GetLastDisplayFormat.");

    // Assume the format is 0
    lpDevData->cFormat = 0;

    // If this was a TV, then get the last format
    if  (lpDevData->cType == DEVTYPE_TV)
    {
        szTvFormat[0] = 0;
        GetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                                NV4_REG_DRV_LAST_TV_FORMAT, szTvFormat);
        if(szTvFormat[0] == 0)
                                return(FALSE);

        // Parse the device type and reduce it a bit
        for (i = 0; TVFormat[i].lpszName != NULL; i++)
            if  (!lstrcmpi (TVFormat[i].lpszName, szTvFormat))
                break;

        lpDevData->cFormat = TVFormat[i].cFormat;
    }

    DBG_PRINT0 (DL_1, "\r\nExiting GetDisplayDevice.");
    return  (TRUE);
}


/*
    Function:   GetDisplayDevice

    Purpose:    This routine gets the display device for the board
                specified. The particular display device is stored
                in the registry.

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                dwLogDevice - 0 based logical device number
                dwPhysDevice- 0 based physical device number
                lpDevData   - ptr to the DEVDATA structure to be filled in

    Returns:    lpDevData filled in correctly
*/
int WINAPI
GetDisplayDevice (LPREGDATA lpRegData, ULONG dwLogDevice, ULONG dwPhysDevice,
                    LPDEVDATA lpDevData)
{
    char    szDev[32];
    char    szTemp[32];

    DBG_PRINT0 (DL_0, "\r\nEntering GetDisplayDevice.");

    wsprintf (szTemp, "%s%c", NV4_REG_DRV_PHYSICAL_DEVICE,
                                (char) (dwPhysDevice + '0'));
    GetLocalNvidiaDisplayLogRegString (lpRegData, dwLogDevice, szTemp, szDev);

    if  (!ParseDeviceTypeAndNumber (szDev, lpDevData))
        return  (FALSE);

    GetLastDisplayFormat (lpRegData, lpDevData);

    DBG_PRINT0 (DL_1, "\r\nExiting GetDisplayDevice.");
    return  (TRUE);
}


/*
    Function:   SetDisplayDevice

    Purpose:    This routine sets the display device for the board
                specified. The particular display device is stored
                in the registry.

    Arguments:  lpRegData   - identifies the specific board
                dwLogDevice - 0 based logical device number
                dwPhysDevice - 0 based physical device number
                lpDevData   - the basic display type
                              should be one of the DEVTYPE_XXXX
                              sonstants definde in modeext.h

    Returns:    Always returns TRUE
*/
int WINAPI
SetDisplayDevice (LPREGDATA lpRegData, ULONG dwLogDevice, ULONG dwPhysDevice,
                    LPDEVDATA lpDevData)
{
    char    szTemp[MAX_KEY_LEN];
    char    szDev[16];

    DBG_PRINT0 (DL_0, "\r\nEntering SetDisplayDevice.");

    if  (lpDevData->cType > MAX_DEVTYPES)
        lpDevData->cType = 0;

    if  (lpDevData->cNumber > 9)
        lpDevData->cNumber = 0;

    wsprintf (szDev, "%s%c", DeviceTypes[lpDevData->cType].lpszName,
                            lpDevData->cNumber + '0');

    wsprintf (szTemp, "%s%d", NV4_REG_DRV_PHYSICAL_DEVICE, dwPhysDevice);
    SetLocalNvidiaDisplayLogRegString (lpRegData, dwLogDevice, szTemp,
                                        szDev, lstrlen(szDev));

    // If this was a TV device, then write out the format
    if  (lpDevData->cType == DEVTYPE_TV)
    {
        SetLocalNvidiaDisplayDevRegString (lpRegData,
                                lpDevData, NV4_REG_DRV_LAST_TV_FORMAT,
                                TVFormat[lpDevData->cFormat].lpszName,
                                lstrlen(TVFormat[lpDevData->cFormat].lpszName));
    }

    DBG_PRINT0 (DL_1, "\r\nExiting SetDisplayDevice.");
    return  (TRUE);
}


/*
    Function:   GetForcedDesktopMode

    Purpose:    This routine gets a forced desktop mode if one exists
                in the registry. Then it deletes the key.

    Arguments:  lpRegData    - identifies the specific board
                lpSuggMode   - This has a suggested mode on the way in
                               If a forced mode is found it is saved
                               here on the way out.

    Returns:    lpSuggMode is updated to the forced mode if one is
                found and the routine returns TRUE. Otherwise,
                lpSuggMode is left alone and the routine returns FALSE.

                Note that at the time of this call, the only fields
                in lpSuggMode that are valid are: dwXRes, dwYRes, dwBpp.

                The values in the other fields should be ignored by
                this routine. The caller will overwrite the other
                fields after this routine returns, so changing the
                other fields in this routine will make no difference.

                FALSE   no forced desktop mode was found
                TRUE    a forced desktop mode was found.
*/
int WINAPI
GetForcedDesktopMode (LPREGDATA lpRegData, LPMODEINFO lpSuggMode)
{
    MODEINFO    sMI;

    DBG_PRINT0 (DL_0, "\r\nEntering GetForcedMode");

    // If the force mode string exists in the registry, then it specifies
    // the mode that should be set period, regardless of anything else.
    if  (!GrabModeInfoString (lpRegData, 0x0FFFFFFFF, &lpSuggMode->MIDevData,
                                NV4_REG_DRV_FORCED_DESKTOP_MODE, &sMI, 1))
    {
        DBG_PRINT0 (DL_0, "\r\nExiting GetForcedMode with Failure.");
        return  (FALSE);
    }

    // Now erase the force mode string
    DeleteLocalNvidiaDisplayDevRegEntry (lpRegData, &lpSuggMode->MIDevData,
                                        NV4_REG_DRV_FORCED_DESKTOP_MODE);
    // Return the mode.
    sMI.MIDevData = lpSuggMode->MIDevData;
    *lpSuggMode = sMI;

    DBG_PRINT0 (DL_1, "\r\nExiting GetForcedMode with Success.");
    return  (TRUE);
}


/*
    Function:   GetLargestEdidMode

    Purpose:    This routine gets the largest edid mode found.

    Arguments:
                lpEdidBuffer - points to a buffer with EDID data in
                               it if dwEdidSize is not 0. If dwEdidSize
                               is 0, this argument is ignored.
                dwEdidSize     tells the size of the lpEdidBuffer
                               in bytes. If this variables is 0,
                               then lpEdidBuffer ptr may be invalid.
                lpModeFound    MODEINFO ptr where the largest EDID mode
                               found should be stored.

    Returns:    TRUE    then lpModeFound was updated with the largest
                        EDID mode
                FALSE   lpModeFound was not updated. This could be because
                        there was no EDID or there was an error parsing
                        the EDID. It doesn't matter.
*/
int WINAPI
GetLargestEdidMode (LPCHAR lpEdidBuffer, ULONG dwEdidSize,
                    LPMODEINFO lpModeFound)
{
    if  (!dwEdidSize)
    {
        // Ignore the force mode if it asks us to look through an
        // EDID which is not there.
        DBG_PRINT0 (DL_0, "\r\nExiting GetLargestEdidMode with Failure.");
        return  (FALSE);
    }

    EdidFindLargestEdidMode (lpEdidBuffer, dwEdidSize, lpModeFound);
    return  (TRUE);
}



/*
    Function:   GetDeviceDesktopMode

    Purpose:    This routine gets the desktop mode for the board
                specified.

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - MODEINFO ptr that holds the desktop mode
                              ion return. Note that only the x,y,bpp
                              are relevent here. Refresh rates are
                              CRTC specific

    Returns:    lpModeInfo -> dwXRes, dwYRes, and dwBpp are filled in
*/
int WINAPI
GetDeviceDesktopMode (LPREGDATA lpRegData, LPMODEINFO lpModeInfo)
{
    int     nRet;

    DBG_PRINT0 (DL_0, "\r\nEntering GetDeviceDesktopMode.");

    nRet = GrabModeInfoString (lpRegData, 0x0FFFFFFFF, &lpModeInfo->MIDevData,
                            NV4_REG_DRV_LAST_DESKTOP_MODE, lpModeInfo, 1);

    DBG_PRINT0 (DL_1, "\r\nExiting GetDeviceDesktopMode.");
    return  (nRet);
}


/*
    Function:   SetDeviceDesktopMode

    Purpose:    This routine writes the desktop mode given into the
                registry for the particular deivce and board specified.

    Arguments:  lpRegData   - identifies the specific board
                lpModeInfo    MODEINFO ptr specifying desktop mode

    Returns:    Always returns TRUE
*/
int WINAPI
SetDeviceDesktopMode (LPREGDATA lpRegData, LPMODEINFO lpModeInfo)
{
    char    szTemp[32];

    DBG_PRINT0 (DL_0, "\r\nEntering SetDeviceDesktopMode.");

    // Set it into the registry
    wsprintf (szTemp, "%ld,%ld,%ld", lpModeInfo->dwXRes,
                lpModeInfo->dwYRes, lpModeInfo->dwBpp);
    SetLocalNvidiaDisplayDevRegString (lpRegData, &lpModeInfo->MIDevData,
                NV4_REG_DRV_LAST_DESKTOP_MODE, szTemp, lstrlen(szTemp));

    DBG_PRINT0 (DL_1, "\r\nExiting SetDeviceDesktopMode.");
    return  (TRUE);
}


/*
    Function:   GetForcedDisplayMode

    Purpose:    This routine gets the forced display mode for the board
                and device specified.

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - MODEINFO ptr that holds the desktop mode
                              ion return.

    Returns:    TRUE    lpModeInfo is filled in with a forced mode
                FALSE   there was no forced mode
*/
int WINAPI
GetForcedDisplayMode (LPREGDATA lpRegData, LPMODEINFO lpModeInfo)
{
    int     nRet;

    DBG_PRINT0 (DL_0, "\r\nEntering GetForcedDisplayMode.");

    nRet = GrabModeInfoString (lpRegData, 0x0FFFFFFFF, &lpModeInfo->MIDevData,
                            NV4_REG_DRV_FORCED_MODE, lpModeInfo, 0);

    // Now erase the force mode string
    DeleteLocalNvidiaDisplayDevRegEntry (lpRegData, &lpModeInfo->MIDevData,
                                        NV4_REG_DRV_FORCED_MODE);

    DBG_PRINT0 (DL_1, "\r\nExiting GetForcedDisplayMode.");
    return  (nRet);
}


/*
    Function:   GetDeviceDisplayMode

    Purpose:    This routine gets the display mode for the board
                and device specified.

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - MODEINFO ptr that holds the desktop mode
                              ion return.

    Returns:    lpModeInfo is filled in
*/
int WINAPI
GetDeviceDisplayMode (LPREGDATA lpRegData, LPMODEINFO lpModeInfo)
{
    int     nRet;

    DBG_PRINT0 (DL_0, "\r\nEntering GetDeviceDisplayMode.");

    nRet = GrabModeInfoString (lpRegData, 0x0FFFFFFFF, &lpModeInfo->MIDevData,
                            NV4_REG_DRV_LAST_MODE, lpModeInfo, 0);

    DBG_PRINT0 (DL_1, "\r\nExiting GetDeviceDisplayMode.");
    return  (nRet);
}


/*
    Function:   SetDeviceDisplayMode

    Purpose:    This routine writes the display mode given into the
                registry for the particular device and board specified.

    Arguments:  lpRegData   - identifies the specific board
                lpModeInfo    MODEINFO ptr specifying desktop mode

    Returns:    Always returns TRUE
*/
int WINAPI
SetDeviceDisplayMode (LPREGDATA lpRegData, LPMODEINFO lpModeInfo)
{
    char    szTemp[32];

    DBG_PRINT0 (DL_0, "\r\nEntering SetDeviceDisplayMode.");

    // Set it into the registry
    wsprintf (szTemp, "%ld,%ld,%ld,%ld", lpModeInfo->dwXRes,
                lpModeInfo->dwYRes, lpModeInfo->dwBpp,
                lpModeInfo->dwRefreshRate);
    SetLocalNvidiaDisplayDevRegString (lpRegData, &lpModeInfo->MIDevData,
                    NV4_REG_DRV_LAST_MODE, szTemp, lstrlen(szTemp));

    DBG_PRINT0 (DL_1, "\r\nExiting SetDeviceDisplayMode.");
    return  (TRUE);
}


/*
    Function:   GetPanningFlag

    Purpose:    This routine gets the panning flag especially for the clone mode
                 for the board and device specified.

    Arguments:  lpRegData   - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - MODEINFO ptr that holds the desktop mode
                              ion return.

    Returns:    0 - the clone device has the maximum possible resolution
                1 - resolution limited (custom panning enabled).
*/
int WINAPI
GetClonePanningFlag (LPREGDATA lpRegData, LPMODEINFO lpModeInfo)
{
    ULONG nRet = 0;

    DBG_PRINT0 (DL_0, "\r\nEntering GetPanningFlag.");

    GetLocalNvidiaDisplayDevRegString (lpRegData, &lpModeInfo->MIDevData,
                                NV4_REG_DRV_PANNING_FLAG, (char*)&nRet);

    DBG_PRINT0 (DL_1, "\r\nExiting GetPanningFlag.");
    return  (int)(nRet & 1);
}


/*
    Function:   SetPanningFlag

    Purpose:    This routine writes the panning flag especially for the clone mode
                 for the board and device specified.

    Arguments:  lpRegData   - identifies the specific board
                lpModeInfo    MODEINFO ptr specifying desktop mode

    Returns:    Always returns TRUE
*/
int WINAPI
SetClonePanningFlag (LPREGDATA lpRegData, LPMODEINFO lpModeInfo, ULONG dwFlag)
{
    char    szTemp[32];

    DBG_PRINT0 (DL_0, "\r\nEntering SetPanningFlag.");

    // Set it into the registry
    wsprintf (szTemp, "%ld", dwFlag);
    SetLocalNvidiaDisplayDevRegString (lpRegData, &lpModeInfo->MIDevData,
                    NV4_REG_DRV_PANNING_FLAG, szTemp, lstrlen(szTemp));

    DBG_PRINT0 (DL_1, "\r\nExiting SetPanningFlag.");
    return  (TRUE);
}

/*
    Function:   GetRefreshRateForDesktop

    Purpose:    This routine gets the correct refresh rate for
                the desktop as Windows has it buried in the registry.
                display device given by lpModeInfo->dwDevData.
                lpModeInfo->dwXRes, lpModeInfo->dwYRes, and
                lpModeInfo->dwBpp are valid, but the field
                lpModeInfo->dwRefreshRate is not.

    Arguments:  lpDispData  - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - contains the display device on which
                              the mode will be set and the mode
                              suggested by the caller.
                lpEdidBuffer- points to a buffer with EDID data in
                              it if dwEdidSize is not 0. If dwEdidSize
                              is 0, this argument is ignored.
                dwEdidSize    tells the size of the lpEdidBuffer
                              in bytes. If this variables is 0,
                              then lpEdidBuffer ptr may be invalid.

    Returns:    This routine should not override the Xres,Yres, and Bpp
                in lpModeInfo. Only the dwRefreshRate field should be
                adjusted. The rest of the mode is provided just in case
                it is needed to determine the correct refresh rate.

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
int WINAPI
GetRefreshRateForDesktop (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo,
                            LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    ULONG       dwSize, hControl;
    long        lRet;
#ifdef  STEREO_SUPPORT
    char        StereoKey[256];
    ULONG       StereoEnabled;
    ULONG       dwType = REG_DWORD;
#endif  //STEREO_SUPPORT

    DBG_PRINT0 (DL_0, "\r\nEntering GetRefreshRateForDesktop.");

#ifdef  STEREO_SUPPORT
    // If stereo is enabled the StereoRefreshRate associated with the mode always overrides
    // what windows wants.
    lstrcpy(StereoKey, NV4_REG_GLOBAL_BASE_PATH);
    lstrcat(StereoKey, "\\");
    lstrcat(StereoKey, NV4_REG_STEREO_SUBKEY);

    if  (RegOpenKey (NV4_REG_GLOBAL_BASE_KEY, StereoKey, &hControl) == ERROR_SUCCESS)
    {
        // Read the current user name
        dwSize = sizeof (long);
        lRet = RegQueryValueEx (hControl, NV_REG_STEREO_ENABLED, NULL, &dwType, 
                        (UCHAR *)&StereoEnabled, (ULONG FAR *) &dwSize);
        RegCloseKey (hControl);
        if  (lRet == ERROR_SUCCESS && StereoEnabled)
        {
            // We know stereo is on. Now let's see if there is a StereoRefreshRate setting
            // in the folder like
            // HKLM\System\CurrentControlSet\Services\Class\Display\000X\MODES\16\1024,768
            GetLocalPath (lpDispData->lpBoardRegistryData, szPath);
            wsprintf(szFullPath, "%s\\MODES\\%ld\\%ld,%ld", szPath,
                                lpModeInfo->dwBpp,
                                lpModeInfo->dwXRes, lpModeInfo->dwYRes);
            // Try to read the user refresh rate from there
            if  (GrabRefreshRateFromKey (HKEY_LOCAL_MACHINE, lpDispData, szFullPath,
                                            lpModeInfo, lpEdidBuffer, dwEdidSize, "StereoRefreshRate"))
                 return  (TRUE);
        }   
    }
#endif  //STEREO_SUPPORT

    // First, is there a current user?
    lRet = RegOpenKey (HKEY_LOCAL_MACHINE, szControl, &hControl);
    if  (lRet == ERROR_SUCCESS)
    {
        // Read the current user name
        szPath[0] = 0;
        dwSize = MAX_KEY_LEN;
        RegQueryValueEx (hControl, "Current User", NULL, NULL,
                         szPath, (ULONG FAR *) &dwSize);

        RegCloseKey (hControl);

        // Try to open HKU\szPath as a key
        lRet = RegOpenKey (HKEY_USERS, szPath, &hControl);
        if  (lRet == ERROR_SUCCESS)
        {
            // Try to read HKU\szPath\Display\Settings
            RegCloseKey (hControl);

            lstrcat (szPath, "\\Display\\Settings");

            // Try to read the user refresh rate
            if  (GrabRefreshRateFromKey (HKEY_USERS, lpDispData, szPath,
                                    lpModeInfo, lpEdidBuffer, dwEdidSize, "RefreshRate"))
                 return  (TRUE);
        }
    }


    // OK, we didn't find it there, so go onto the next stage.
    // This is a path something like
    // HKLM\System\CurrentControlSet\Services\Class\Display\000X\MODES\16\1024,768
    GetLocalPath (lpDispData->lpBoardRegistryData, szPath);
    wsprintf(szFullPath, "%s\\MODES\\%ld\\%ld,%ld", szPath,
                        lpModeInfo->dwBpp,
                        lpModeInfo->dwXRes, lpModeInfo->dwYRes);

    // Try to read the user refresh rate from there
    if  (GrabRefreshRateFromKey (HKEY_LOCAL_MACHINE, lpDispData, szFullPath,
                                    lpModeInfo, lpEdidBuffer, dwEdidSize, "RefreshRate"))
         return  (TRUE);


    // Well, that didn't work either, so last chance is:
    // HKLM\System\CurrentControlSet\Services\Class\Display\000X\Default
    GetLocalPath (lpDispData->lpBoardRegistryData, szPath);
    lstrcat (szPath, "\\Default");
    // Try to read the user refresh rate from there
    if  (GrabRefreshRateFromKey (HKEY_LOCAL_MACHINE, lpDispData, szPath,
                                    lpModeInfo, lpEdidBuffer, dwEdidSize, "RefreshRate"))
         return  (TRUE);

    // Well, we just have to set up a default refresh rate then
    lpModeInfo->dwRefreshRate = 60;
    lpModeInfo->dwOriginalRefreshRate = lpModeInfo->dwRefreshRate;

    return  (TRUE);
}


/*
    Function:   GrabModeInfoString

    Purpose:    This routine reads and parses a string of the
                form "xres,yres,bpp,refrate" from the registry.

    Arguments:  lpRegData    - identifies the specific board
                dwLogDevice  - 0 based logical device number
                               if 0x0FFFFFFFF, then not used.
                lpDevData    - 0 based physical device number
                               if 0x0FFFFFFFF, then not used.
                lpModeString - Name of entry in registry (relative
                               to XXXX\NVidia\Display) that tells the
                               specific mode string to read.
                lpMI         - MODEINFO ptr where parsed mode goes
                nType        - 0 then this is a CRTC info string of the
                               form "xres,yres,bpp,refrate"
                             - 1 then this is a Desktop info string of the
                               form "xres,yres,bpp"

    Returns:    TRUE  that a valid mode was parsed and stored in lpMI
                FALSE there was a problem with the parsing and lpMI
                does not contain valid data.

                NOTE: Even if this routine returns TRUE, it does not
                mean the mode is reasonable. 1,1,55,9876 would be parsed
                fine, but it is not a valid mode string. The caller
                must check for validity if it needs to ensure this.
*/
int CFUNC
GrabModeInfoString (LPREGDATA lpRegData, ULONG dwLogDevice, LPDEVDATA lpDevData,
                    LPCHAR lpModeString, LPMODEINFO lpMI, int nType)
{
    char    szModeInfo[MAX_KEY_LEN];
    ULONG   dwIndex;
    int     bModeValid = FALSE;

    DBG_PRINT0 (DL_0, "\r\nEntering GrabModeInfoString.");

    szModeInfo[0] = 0;

    // If dwLogDevice is 0x0FFFFFFFF, then we get the string from ...
    if  ((dwLogDevice == 0x0FFFFFFFF) && (lpDevData == NULL))
    {
        // ... Nvidia\Display
        GetLocalNvidiaDisplayRegString (lpRegData, lpModeString, szModeInfo);
    }
    else
    {
        // If lpDevData is 0x0FFFFFFFF, then we get the string from ...
        if  (lpDevData == NULL)
        {
            // ... Nvidia\Display\LogDevice%dwLogDevice%.
            GetLocalNvidiaDisplayLogRegString (lpRegData, dwLogDevice,
                                            lpModeString, szModeInfo);
        }
        else
        {
            // ... otherwise we get the string from
            // Nvidia\Display\LogDevice%dwLogDevice%\PhysDevice%dwPhysDevice%
            GetLocalNvidiaDisplayDevRegString (lpRegData, lpDevData,
                                                lpModeString, szModeInfo);
        }
    }

    // if we got something back from reading the key, then use it
    if  (szModeInfo[0] != 0)
    {
        // Try to parse the mode
        dwIndex = 0;
        bModeValid = TRUE;

        if  (!ParseDecimalNumberComma (szModeInfo, &dwIndex, &lpMI->dwXRes))
            bModeValid = FALSE;
        DBG_PRINT1(DL_2, "\r\nXRes found to be %ld", lpMI->dwXRes);

        if  (!ParseDecimalNumberComma (szModeInfo, &dwIndex, &lpMI->dwYRes))
            bModeValid = FALSE;
        DBG_PRINT1(DL_2, "\r\nYRes found to be %ld", lpMI->dwYRes);

        if  (!ParseDecimalNumberComma (szModeInfo, &dwIndex, &lpMI->dwBpp))
            bModeValid = FALSE;
        DBG_PRINT1(DL_2, "\r\nBpp found to be %ld", lpMI->dwBpp);

        // The XRes and YRes can be whatever, but let's validate the
        // Bpp here.
        if  ((lpMI->dwBpp != 8) && (lpMI->dwBpp != 16) && (lpMI->dwBpp != 32))
        {
            DBG_PRINT0(DL_2, "\r\nBpp invalid. Changed to 8.");
            lpMI->dwBpp = 8;
        }

        if  (nType == 0)
        {
            if  (!ParseDecimalSeparator (szModeInfo, &dwIndex, &lpMI->dwRefreshRate, ',', 0))
                bModeValid = FALSE;
            DBG_PRINT1(DL_2, "\r\nRefresh Rate found to be %ld", lpMI->dwRefreshRate);
        }
    }

    DBG_PRINT0 (DL_1, "\r\nExiting GrabModeInfoString.");
    return  (bModeValid);
}


/*
    Function:   GetDefaultRefreshRate

    Purpose:    This routine reads a key in the registry to get
                a specific value for the default refresh rate.
                There is an optional OEM specific key that can
                specify what default should mean. If the key does
                not exist, then 60 is returned.

    Arguments:
                lpDispData  - identifies the specific board
                lpModeInfo  - tells which display device port is being
                              used since there might be different
                              default refresh rates for DFPs and CRTs.

    Returns:    always returns a value that is the default refresh rate
*/
ULONG CFUNC
GetDefaultRefreshRate (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo)
{
    int     nRet;
    ULONG   dwRefreshRate, dwIndex;
    char    szDev[32];

    // Filter out TVs here -- always say 60Hz.
    if  (lpModeInfo->MIDevData.cType == DEVTYPE_TV)
        return  (60);

    if  (lpModeInfo->MIDevData.cType == DEVTYPE_CRT)
        lstrcpy (szDev, NV4_REG_DRV_DEFAULT_CRT_VERT_REFRESH_RATE);
    else
        lstrcpy (szDev, NV4_REG_DRV_DEFAULT_DFP_VERT_REFRESH_RATE);

    nRet = GetLocalNvidiaDisplayRegString (lpDispData->lpBoardRegistryData,
                                            szDev, szPath);
    if  (nRet)
    {
        dwIndex = 0;
        if  (ParseDecimalSeparator (szPath, &dwIndex, &dwRefreshRate, 0, 0))
        {
            return  (dwRefreshRate);
        }
    }

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
                lpDispData   - identifies the specific board
                lpModeInfo   - Current mode and device identifier.
                lpEdidBuffer - points to a buffer with EDID data in
                               it if dwEdidSize is not 0. If dwEdidSize
                               is 0, this argument is ignored.
                dwEdidSize     tells the size of the lpEdidBuffer
                               in bytes. If this variables is 0,
                               then lpEdidBuffer ptr may be invalid.

    Returns:    always returns a value that is the optimal refresh rate
*/
ULONG CFUNC
GetOptimalRefreshRate (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo,
                        LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    ULONG           dwRefreshRate, dwRegRefreshRate, dwIndex;
    ULONG           dwMaxXRes;
    ULONG           dwMaxYRes;
    MONITORLIMITS   ML;
    char            szDev[32];

    if ((dwEdidSize == 0) && (lpDispData->dwMonitorInfType == UNKNOWN_DEVICE))
    {
        // If monitor is unknown and no EDID, then only 60Hz allowed.
        return  (60);
    }

    // Filter out TVs here -- always say 60Hz.
    if  (lpModeInfo->MIDevData.cType == DEVTYPE_TV)
        return  (60);

    // Check if RestrictOptimal key exists and is equal to ASCII "1".
    // If so, check detailed, established, and standard timings for
    // an exact match of the XRes and YRes and use that RefreshRate
    // if it exists.
    if (GetLocalNvidiaDisplayRegString (lpDispData->lpBoardRegistryData,
            NV4_REG_DRV_USE_REFRESH_RATE_OF_XYRES_EDID_MODE_MATCH, szPath))
    {
        if  (!lstrcmpi (szPath, NV4_REG_DRV_TRUE))
        {
            if (dwEdidSize)
            {
                dwRefreshRate = (ULONG) EdidFindExactModeWithHighestRR
                                             (lpEdidBuffer, dwEdidSize,
                                              lpModeInfo);
                if  (dwRefreshRate)
                    return  (dwRefreshRate);
            }
        }
    }

    // Now let's deal with an EDID if it exists
    if  (dwEdidSize != 0)
    {
        // Use the mode in the EDID with the largest refresh rate
        // whose resolution is at least as big as the one passed in.
        dwRefreshRate = (ULONG) EdidFindSameModeOrLargerWithHighestRR (
                                lpEdidBuffer, dwEdidSize, lpModeInfo);
    }
    else
    {
        if  (!GetMonitorInfRangeLimits (lpDispData, &ML, &dwMaxXRes, &dwMaxYRes))
        {
            // Something went wrong getting monitor range limits.
            // Limit ourselves to 60Hz
            return (60);
        }

        if  ((!dwMaxXRes) || (!dwMaxYRes))
        {
            return  (60);
        }

        // Use the maximum xres and yres at the max vertical refresh rate
        // to compute what the monitor limits will be at the desired target
        // resolution.
        dwRefreshRate = GetMaximumRefreshRate (dwMaxXRes, dwMaxYRes,
                        ML.dwMaxVert, &ML, lpModeInfo->dwXRes,
                        lpModeInfo->dwYRes);
    }

    // See if the registry has an upper limit on the optimal
    // refresh rate value.
    dwRegRefreshRate = 0;
    if  (lpModeInfo->MIDevData.cType == DEVTYPE_CRT)
        lstrcpy (szDev, NV4_REG_DRV_OPTIMAL_CRT_VERT_REFRESH_RATE);
    else
        lstrcpy (szDev, NV4_REG_DRV_OPTIMAL_DFP_VERT_REFRESH_RATE);

    if (GetLocalNvidiaDisplayRegString (lpDispData->lpBoardRegistryData,
                                    szDev, szPath))
    {
        dwIndex = 0;
        if  (!ParseDecimalSeparator (szPath, &dwIndex, &dwRegRefreshRate, 0, 0))
        {
            // Parsing the refresh rate found in the registry failed,
            // so set the refresh rate back to 0.
            dwRegRefreshRate = 0;
        }
    }

    // If we found an upper bound in the registry, use it
    if  (dwRegRefreshRate)
    {
        if  (dwRefreshRate > dwRegRefreshRate)
            dwRefreshRate = dwRegRefreshRate;
    }

    // Now return the refresh rate or 60Hz.
    if  (dwRefreshRate != 0)
        return  (dwRefreshRate);

    return  (60);
}



/*
    Function:   GrabRefreshRateFromKey

    Purpose:    This routine reads the RefreshRateName string value
                from the key and path provided and does a little
                preprocessing on what it finds. i.e. it handles
                optimal (-1) and default (0) cases.

    Arguments:  hKey          base Key in registry from which to read
                lpDispData  - identifies the specific board that we
                              want the active display port for.
                lpszPath      path in registry to access which may
                              contain a RefreshRateName string.
                lpModeInfo  - Current mode and device identifier.
                lpEdidBuffer- points to a buffer with EDID data in
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
int CFUNC
GrabRefreshRateFromKey (ULONG hKey, LPDISPDATA lpDispData, LPCHAR lpszPath,
                LPMODEINFO lpModeInfo, LPCHAR lpEdidBuffer, ULONG dwEdidSize, char *RefreshRateName)
{
    ULONG   hControl;
    char    szRefreshRate[8];
    int     bFoundRefRate;
    ULONG   dwIndex, dwSize;
    long    lRet;

    lRet = RegOpenKey (hKey, lpszPath, &hControl);
    if  (lRet != ERROR_SUCCESS)
    {
        return  (FALSE);
    }

    // OK, now hUser is a key to someplace in the registry that
    // may have a RefreshRateName value string in it. Find out.
    szRefreshRate[0] = 0;
    dwSize = 8;
    RegQueryValueEx (hControl, RefreshRateName, NULL, NULL,
                         szRefreshRate, (ULONG FAR *) &dwSize);

    bFoundRefRate = FALSE;
    if  (szRefreshRate[0] != 0)
    {
        // OK -- if the value is a specific refresh rate then
        // that is what we use.
        dwIndex = 0;
        if  (ParseDecimalSeparator (szRefreshRate, &dwIndex, &lpModeInfo->dwOriginalRefreshRate, 0, 0))
        {
            if  (lpModeInfo->dwOriginalRefreshRate == 0)
            {
                // If we actually parsed a '0' that means adapter default
                lpModeInfo->dwRefreshRate =
                GetDefaultRefreshRate (lpDispData, lpModeInfo);
            }
            else
            {
                // dwOriginalRefreshRate has specific refresh rate. Save
                // in dwRefreshRate.
                lpModeInfo->dwRefreshRate = lpModeInfo->dwOriginalRefreshRate;
            }

            bFoundRefRate = TRUE;
        }

        // Is the refresh rate set to "-1"?  'Cause that is optimal.
        if  (!lstrcmpi(szRefreshRate, "-1"))
        {
            lpModeInfo->dwOriginalRefreshRate = 0x0FFFFFFFF;
            lpModeInfo->dwRefreshRate = GetOptimalRefreshRate (lpDispData,
                                                    lpModeInfo, lpEdidBuffer,
                                                    dwEdidSize);
            bFoundRefRate = TRUE;
        }

        // OK, we either found a value and parsed it or the value was
        // set to "-1". If either one of those things happened, then
        // we "found" a refresh rate. If neither happened, we didn't.
    }

    // Done with the key
    RegCloseKey (hControl);

    if  (bFoundRefRate)
    {
        // Just as a safety precausion, force refresh rates < 60 up to 60
        if  (lpModeInfo->dwRefreshRate < 60)
        {
            lpModeInfo->dwRefreshRate = 60;
            lpModeInfo->dwOriginalRefreshRate = lpModeInfo->dwRefreshRate;
        }

        return  (TRUE);
    }

    return  (FALSE);
}


char  *(szBppAppend[]) =
{
    "\\8", "\\16", "\\24", "\\32"
};

// I would put these locally on the stack in the following function,
// but we ran out of stack space!!
char            szResPath[MAX_KEY_LEN];
char            szRefreshRateList[MAX_KEY_LEN];
char            szNewRefreshRateList[MAX_KEY_LEN];
char            szPDPath[MAX_KEY_LEN];
char            szNPDPath[MAX_KEY_LEN];

/*
    Function:   ResetRefreshRates

    Purpose:    This routine walks through the refresh rate strings in
                all the leaf subkeys of ....\DISPLAY\000X\MODES and updates
                the Default string. The Default string has a refresh
                rate list of something like "60,70,72,75,85,100,120".
                This string is replaced with a new refresh rate string
                containing those refresh rates that are supported in
                the current configuration which is given by lpDispData.

                This routine assumes that all of the leaf subkeys of
                ....\DISPLAY\000X\MODES have in them a master refresh
                rate list string somethig like:
                "ModeRefreshRateList"   "60,70,72,75,85,etc.."

    Arguments:
                lpDevData    - LPDEVDATA
                lpDispData   - identifies the specific board and
                               configuration that we want the refresh
                               rate list to agree with.
                lpEdidBuffer - ptr to EDID
                dwEdidSize   - length of edid in bytes. 0 if no edid.

    Returns:    TRUE    if refresh rate list was updated
                FALSE   if it wasn't because of some error dealing with
                        the registry
*/
int WINAPI
ResetRefreshRates (LPDEVDATA lpDevData, LPDISPDATA lpDispData,
                    LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    ULONG           dwSrcIndex;
    int             i, pd, npd, bDone;
    ULONG           dwSubKey, dwMajorKey;
    ULONG           hModeBppKey, hModeResKey, dwSize;
    ULONG           hModeNewBppKey, hModeNewResKey;
    ULONG           dwIndex, dwMaxDacSpeed, dwMaxRefreshRate, dwRefreshRate;
    ULONG           dwMaxXRes, dwMaxYRes;
    ULONG           dwXRes, dwYRes, dwTotalPixels;
    GTFIN           GTFIn;
    GTFOUT          GTFOut;
    LPCHAR          lpMasterRefreshRateList, lpNewRefreshRateList;
    long            lRet;
    char            szRes[32];
    char            szRegPath[MAX_KEY_LEN];
    char            szAddedRefreshRate[16];
    MONITORLIMITS   sML;

    // Start off with a "Last" value that will never match
#define MAX_CACHE_EDID_SIZE 512
    int             nMustReset, nSmallerSize;
    static          DEVDATA sLastDevData = { -1, 0, 0, 0 };
    static          ULONG   dwLastEdidSize = 0x0FFFFFFFF;
    static          unsigned char  szLastEdid[MAX_CACHE_EDID_SIZE];
    static          ULONG   dwLastMonitorInfType = 0x0FFFFFFFF;
    static          ULONG   dwLastMaxDacSpeed8bpp = 0x0FFFFFFFF;
    static          ULONG   dwLastMaxDacSpeed16bpp = 0x0FFFFFFFF;
    static          ULONG   dwLastMaxDacSpeed32bpp = 0x0FFFFFFFF;

    DBG_PRINT0(DL_0, "\r\nEntering ResetRefreshRates.");

    ///////////////////////////////////////////////////////////////
    // A little caching scheme to speed things up.
    // If the device type is DIFFERENT from the last one passed
    // into this routine then we definitely need to update the
    // refresh rate list. If it is the same type of device, then
    // we do some other checks.
    nMustReset = FALSE;
    if  (sLastDevData.cType != lpDevData->cType)
        nMustReset = TRUE;
    else
    {
        // Same device type. Do the current and last devices
        // both have EDIDS?
        if  (dwEdidSize != dwLastEdidSize)
            nMustReset = TRUE;
        else
        {
            // Edid Sizes are the same. If the edid size is not 0,
            // then compare the two EDIDs
            if  (dwEdidSize != 0)
            {
                if  (dwLastEdidSize < dwEdidSize)
                    nSmallerSize = (int) dwLastEdidSize;
                else
                    nSmallerSize = (int) dwEdidSize;

                for (i = 0; i < nSmallerSize; i++)
                {
                    if  (szLastEdid[i] != lpEdidBuffer[i])
                    {
                        nMustReset = TRUE;
                        break;
                    }
                }
            }

            // Make sure that both have the same type of .inf file
            if  (dwLastMonitorInfType != lpDispData->dwMonitorInfType)
                nMustReset = TRUE;

            // If we have different limits on the DAC speed ....
            if  ((dwLastMaxDacSpeed8bpp != lpDispData->dwMaxDacSpeedInHertz8bpp)
              || (dwLastMaxDacSpeed16bpp != lpDispData->dwMaxDacSpeedInHertz16bpp)
              || (dwLastMaxDacSpeed32bpp != lpDispData->dwMaxDacSpeedInHertz32bpp))
                nMustReset = TRUE;
        }
    }

    if  (!nMustReset)
    {
        // Last device and current device are the same.
        // Nothing to do.
        return  (TRUE);
    }
    else
    {
        // Save off all the stuff for cache comparison next time.
        sLastDevData = *lpDevData;
        dwLastEdidSize = dwEdidSize;
        dwLastMonitorInfType = lpDispData->dwMonitorInfType;
        if  (dwLastEdidSize)
        {
            // We simply can't cache EDIDs larger than 512 bytes
            if  (dwLastEdidSize > MAX_CACHE_EDID_SIZE)
                 dwLastEdidSize = 0;

            for (i = 0; i < (int) dwLastEdidSize; i++)
                szLastEdid[i] = lpEdidBuffer[i];
        }

        dwLastMaxDacSpeed8bpp = lpDispData->dwMaxDacSpeedInHertz8bpp;
        dwLastMaxDacSpeed16bpp = lpDispData->dwMaxDacSpeedInHertz16bpp;
        dwLastMaxDacSpeed32bpp = lpDispData->dwMaxDacSpeedInHertz32bpp;

        // Fall through and let's reset the refresh rate list correctly.
    }
    ///////////////////////////////////////////////////////////////


    // Try to get the path in the registry to where the mode list is.
    if  (GetLocalPath (lpDispData->lpBoardRegistryData, szRegPath) < 0)
    {
        DBG_PRINT0(DL_0, "\r\nExiting ResetRefreshRates because RegOpenKey (...MODES) failed.");
        return  (FALSE);
    }

    dwMajorKey = lpDispData->lpBoardRegistryData->dwMainKey;

    // Tack on "\\MODES" to the returned regpath
    lstrcat (szRegPath, "\\Modes");

    // Create a loop so we can extract the modes for each pixel depth
    for (pd = 0; pd < 4; pd++)
    {
        // Copy the path to ...DISPLAY\000X\MODES and then tack on
        // the correct pixel depth key
        lstrcpy (szPDPath, szRegPath);

        switch  (pd)
        {
            case 0:
                lstrcat (szPDPath, "\\8");
                dwMaxDacSpeed = lpDispData->dwMaxDacSpeedInHertz8bpp;
                break;
            case 1:
                lstrcat (szPDPath, "\\16");
                dwMaxDacSpeed = lpDispData->dwMaxDacSpeedInHertz16bpp;
                break;
            case 2:
                lstrcat (szPDPath, "\\24");
                // We don't support 24bpp so don't need to worry here.
                break;
            case 3:
                lstrcat (szPDPath, "\\32");
                dwMaxDacSpeed = lpDispData->dwMaxDacSpeedInHertz32bpp;
                break;
        }

        // We now have the key where the pixel depth specific modes are.
        // Go ahead and open the key.
        lRet = RegOpenKey (dwMajorKey, szPDPath, &hModeBppKey);

        if  (lRet != ERROR_SUCCESS)
        {
            // If we have trouble opening a Bpp key, that is OK, since
            // we might not support a particular pixel depth. Just go
            // on to the next one.
            continue;
        }

        // Enumerate the subkeys -- each key is a spatial resolution.
        // Within each key there is a "(default)" string which has a
        // list of refresh rates which looks like: "60,72,75,85"
        // If the key does not have a list of refresh rates, then
        // we just write 60Hz as the only available refresh rate for
        // that mode.

        for (dwSubKey = 0; TRUE; dwSubKey++)
        {
            lRet = RegEnumKey (hModeBppKey, dwSubKey, szRes, MAX_KEY_LEN);

            if  (lRet != ERROR_SUCCESS)
            {
                // If there was an error, it probably means that there are
                // no more subkeys -- i.e. resolutions for this pixel
                // depth. That's fine. Break out of the loop.
                break;
            }

            // Get the xres and yres from the key string.
            dwIndex = 0;
            ParseDecimalSeparator (szRes, &dwIndex, &dwXRes, ',', ',');
            ParseDecimalSeparator (szRes, &dwIndex, &dwYRes, 0, 0);

            // So, here's the thing. We have set dwMaxDacSpeed to the
            // maximum dac speed for the pixel depth we are currently
            // working with. dwMaxDacSpeed takes into account memory
            // speed limitations, graphics chip CRTC limitations, etc.
            // Given this MaxSpeed, we need to come up with a max
            // vertical refresh rate. We need Microsoft's algorithm
            // here so that we could match what they would show, but
            // we don't have it, so for now we do our best.
            // Use GTF to find a refresh rate.

            dwMaxRefreshRate = 0;
            if  ((dwEdidSize == 0) ||
                 (!EdidGetMonitorLimits (lpEdidBuffer, dwEdidSize, &sML)))
            {
                if  ((lpDispData->dwMonitorInfType != SPECIFIC_DEVICE) ||
                     (!GetMonitorInfRangeLimits (lpDispData, &sML, &dwMaxXRes, &dwMaxYRes)))
                {
                    // There are no monitor limits, from either an EDID
                    // or an .inf, so just say YES.
                    GTFIn.dwHPixels = dwXRes;
                    GTFIn.dwVLines  = dwYRes;
                    GTFIn.dwMarginsRqd = FALSE;
                    GTFIn.dwIntRqd = FALSE;
                    GTFIn.dwIPParmType = IPTYPE_PIXEL_CLOCK_RATE;
                    GTFIn.dwIPFreqRqd = Divide32By32 (dwMaxDacSpeed, 1000000);

                    // Get the timing parameters.
                    ComputeGTFParameters(&GTFIn, &GTFOut);
                    dwMaxRefreshRate = Multiply32By32 (GTFOut.dwPixelClockIn10KHertz, 10000);
                    dwTotalPixels = Multiply32By32 (GTFOut.dwVTotalScans, (GTFOut.dwHTotalChars << 3));
                    dwMaxRefreshRate = Divide32By32 (dwMaxRefreshRate, dwTotalPixels);
                }
            }

            if  (dwMaxRefreshRate == 0)
            {
                if (sML.dwMaxPixelClock > dwMaxDacSpeed)
                    sML.dwMaxPixelClock = dwMaxDacSpeed;
                dwMaxRefreshRate = GetMaximumRefreshRate (0, 0, 0, &sML,
                                        dwXRes, dwYRes);
            }

            DmtGetMaximumRefreshRate (dwXRes, dwYRes, sML.dwMaxPixelClock,
                                      &dwRefreshRate);

            if (dwRefreshRate > dwMaxRefreshRate)
                dwMaxRefreshRate = dwRefreshRate;

            // Build a full path to the resolution key so we can open it
            lstrcpy (szResPath, szPDPath);
            lstrcat (szResPath, "\\");
            lstrcat (szResPath, szRes);

            // OK, now open the spatial resolution key so that we can
            // get the list of refresh rates for this resolution.
            lRet = RegOpenKey (dwMajorKey, szResPath, &hModeResKey);

            if  (lRet == ERROR_SUCCESS)
            {
                // Read the "ModeRefreshRateList" key.
                dwSize = MAX_KEY_LEN;
                lRet = RegQueryValueEx (hModeResKey, "ModeRefreshRateList",
                                NULL, NULL, szRefreshRateList, &dwSize);

                if  (lRet != ERROR_SUCCESS)
                {
                    // If we didn't find a ModeRefreshRateList, there
                    // might be several reasons. Asuume, though, that
                    // we could find one if we looked at the same
                    // resolution key in a lower color depth.
                    for (npd = pd - 1; npd >= 0; npd--)
                    {
                        lstrcpy (szNPDPath, szRegPath);
                        lstrcat (szNPDPath, szBppAppend[npd]);
                        lRet = RegOpenKey (dwMajorKey, szNPDPath, &hModeNewBppKey);
                        if  (lRet == ERROR_SUCCESS)
                        {
                            lstrcat (szNPDPath, "\\");
                            lstrcat (szNPDPath, szRes);
                            lRet = RegOpenKey (dwMajorKey, szNPDPath, &hModeNewResKey);
                            if  (lRet == ERROR_SUCCESS)
                            {
                                // Read the "ModeRefreshRateList" key.
                                dwSize = MAX_KEY_LEN;
                                lRet = RegQueryValueEx (hModeNewResKey,
                                        "ModeRefreshRateList", NULL,
                                        NULL, szRefreshRateList, &dwSize);

                                RegCloseKey (hModeNewResKey);
                            }

                            RegCloseKey (hModeNewBppKey);
                        }

                        if  (lRet == ERROR_SUCCESS)
                            break;
                    }
                }


                if  (lRet == ERROR_SUCCESS)
                {
                    // Now build a refresh rate string consisting of
                    // those refresh rates that are supported according
                    // to the BoardData.

                    // Always specify 60Hz.
                    lpNewRefreshRateList = szNewRefreshRateList;
                    wsprintf (lpNewRefreshRateList, "%d", 60);

                    if  ((lpDevData->cType == DEVTYPE_CRT) ||
                         (lpDevData->cType == DEVTYPE_DFP))
                    {
                        dwSrcIndex = 0;
                        bDone = FALSE;
                        lpMasterRefreshRateList = szRefreshRateList;
                        for (i = 0; !bDone; i++)
                        {
                            // Get the next refresh rate in the naster list
                            bDone = !ParseDecimalSeparator (lpMasterRefreshRateList, &dwSrcIndex, &dwRefreshRate, ',', 0);

                            if  (!bDone)
                            {
                                // We found a decimal number refresh rate
                                // Is this one within bounds?
                                if  (dwRefreshRate <= dwMaxRefreshRate)
                                {
                                    // Add it to the new list if it is not 60Hz
                                    // since we already added that in above
                                    if  (dwRefreshRate != 60)
                                    {
                                        wsprintf (szAddedRefreshRate, ",%d", dwRefreshRate);
                                        lstrcat (lpNewRefreshRateList, szAddedRefreshRate);
                                    }
                                }
                            }
                        }
                    }

                    // Write back the updated refresh rate list as
                    // the defaukt field in this key. That is what
                    // windows expects.
                    RegSetValueEx (hModeResKey, "", NULL, REG_SZ, lpNewRefreshRateList, lstrlen(lpNewRefreshRateList));
                }
            }

            // Close this bpp key
            RegCloseKey (hModeResKey);
        }

        // Close this bpp key
        RegCloseKey (hModeBppKey);
    }

    DBG_PRINT0(DL_0, "\r\nExiting ResetRefreshRates.");

    return  (TRUE);
}

/*
    Function:   GetEdidRegistryKey

    Purpose:    This routine gets the registry key that tells
                whether or not we are supposed to read the EDID.

    Arguments:  lpRegData   - identifies the specific board

    Returns:    0       registry key does not exist or says not to
                        read the EDID
                non-0   registry key exists and says to read the EDID
*/
int WINAPI
GetEdidRegistryKey (LPREGDATA lpRegData)
{
    int     nFlag;

    DBG_PRINT0 (DL_0, "\r\nEntering GetEdidRegistryKey.");

    nFlag = GetLocalNvidiaDisplayRegValue (lpRegData,
                                        NV4_REG_DRV_FORCE_EDID_READ, 0);

    DBG_PRINT1(DL_1, "\r\nExiting GetEdidRegistryKey.", nFlag);
    return  (nFlag);
}


int WINAPI
SetResolutionAndColorDepth (ULONG Bpp, ULONG XRes, ULONG YRes)
{
    ULONG       hDisplay;
    long        lRet;
    char        szResolution[MAX_KEY_LEN];
    char        szBpp[MAX_KEY_LEN];

    wsprintf (szResolution, "%ld,%ld", XRes, YRes);
    wsprintf (szBpp, "%ld", Bpp);

    // Open display settings key
    lRet = RegOpenKey (HKEY_CURRENT_CONFIG, szSettings, &hDisplay);
    if  (lRet == ERROR_SUCCESS)
    {
        RegSetValueEx (hDisplay, "Resolution", NULL, REG_SZ, szResolution, strlen(szResolution));
        RegCloseKey (hDisplay);
        return  (TRUE);
    }
    return  (FALSE);
}


/*
    GetDesktopModeList

    This routine gets the desktop mode list and returns it.

    The modes are returned as either MODEENTRY, MODEENTRYNORR, or
    MODEENTRYANDPITCH structures depending upon the value of dwType.

    If dwType is MODETYPE_MODEENTRY, then MODEENTRY structures are
    returned. If dwType is MODETYPE_MODEENTRYNORR, then MODEENTRYNORR
    structures are returned. If dwType is MODETYPE_MODEENTRYANDPITCH,
    then MODEENTRYANDPITCH structures are returned.
*/
int WINAPI
GetDesktopModeList (LPDISPDATA lpDispData, LPCHAR lpModes, ULONG dwType)
{
    LPMODEENTRY lpME;
    int         j;

    lpME = GetModeListFromRegistry (lpDispData);

    j = FormatModeList (lpDispData, lpModes, dwType, lpME);

    FreeMasterModeList (lpME);
    return  (j);
}


/*
    Function:   GetAllowAllModesFlag

    Purpose:    WIN9X spacific function for testing only.
                This function get "AllowAllModes" registry key
                for specified DispData.

    Arguments:
                lpDispData - identifies the specific board that we
                             want the active display port for.
    Returns:    "AllowAllModes" key value or FALSE.
*/
ULONG WINAPI GetAllowAllModesFlag(LPREGDATA lpRegData)
{

    int     nDefault = 0;
    int     wAllowAllModes = 0;

    wAllowAllModes =
        GetLocalNvidiaDisplayRegValue (lpRegData,
                                    NV4_REG_DRV_ALLOW_ALL_MODES, nDefault);

    return ((ULONG) wAllowAllModes);
}


/*
    Function:   GetAllowMightyModesFlag

    Purpose:    WIN9X spacific function for testing only.
                This function gets "AllowMightyModes" registry key
                for specified DispData.

    Arguments:
                lpDispData - identifies the specific board that we
                             want the active display port for.
    Returns:    "AllowMightyModes" key value or FALSE.
*/
ULONG WINAPI GetAllowMightyModesFlag(LPREGDATA lpRegData)
{

    int     nDefault = 0;
    int     wAllowMightyModes = 0;

    wAllowMightyModes =
        GetLocalNvidiaDisplayRegValue (lpRegData,
                                    NV4_REG_DRV_ALLOW_MIGHTY_MODES, nDefault);

    return ((ULONG) wAllowMightyModes);
}

/*
    Function:   GetControlVisionFlag

    Purpose:    WIN9X spacific function for testing only.
        This function get "ControlVision" registry key
        for specified DispData.

    Arguments:
                lpDispData - identifies the specific board that we
                             want the active display port for.
    Returns:    "ControlVision" key value or FALSE.
*/


int WINAPI
GetControlVisionFlag(LPDISPDATA lpDispData)
{

    int     nDefault = 0;
    int     wControlVisionFlag = 0;

    wControlVisionFlag = GetLocalNvidiaDisplayRegValue (lpDispData->lpBoardRegistryData,
                                   "ControlVision", nDefault);

    return wControlVisionFlag;
}


/*
    Function:   GetCheckSXGAPanelFlag

    Purpose:    This function gets "CheckSXGAPanel" registry key
                for specified DispData.

    Arguments:
                lpDispData - identifies the specific board that we
                             want the active display port for.
    Returns:    "CheckSXGAPanel" key value or FALSE.
*/
ULONG WINAPI GetCheckSXGAPanelFlag(LPREGDATA lpRegData)
{

    int     nDefault = 0;
    int     wCheckSXGAPanel = 0;

    wCheckSXGAPanel =
        GetLocalNvidiaDisplayRegValue (lpRegData,
                                    NV4_REG_DRV_CHECK_SXGA_PANEL, nDefault);

    return ((ULONG) wCheckSXGAPanel);
}


/*
    ParseDeviceTypeAndNumber

    Purpose:    This routine parses a string of the form "CRT4"
                or "TV2".  It gets the device type followed by
                the device number hich can be 0 - 9.

    Arguments:  lpszString  - string of the form described in Purpose:
                lpDevData   - ptr to a DEVTYPE structure.

    Returns:    TRUE    if lpDevData is filled in correctly.
                        Note: Only the DEVTYPE.cType and DEVTYPE.cNumber
                        are filled in. The DEVTYPE.cFormat is untouched.
                FALSE   otherwise
*/
int WINAPI
ParseDeviceTypeAndNumber (LPCHAR lpszString, LPDEVDATA lpDevData)
{
    int     i, nIndex, nEnd, nRet;
    char    cOld;

    nIndex = 0;
    for (i = 0; DeviceTypes[i].lpszName != NULL; i++)
    {
        // lstrncmpi does not exist, so I do it this way ...
        nEnd = (int) lstrlen (DeviceTypes[i].lpszName);
        cOld = lpszString[nEnd];

        lpszString[nEnd] = 0;
        nRet = (int) lstrcmpi (DeviceTypes[i].lpszName, lpszString);
        lpszString[nEnd] = cOld;

        if  (!nRet)
        {
            lpDevData->cType = DeviceTypes[i].cType;
            nIndex = nEnd;
            break;
        }
    }

    if  (nIndex != 0)
    {
        // We must have found a device type
        lpDevData->cNumber = lpszString[nIndex] - '0';
        if  ((lpDevData->cNumber < 0) || (lpDevData->cNumber > 9))
            lpDevData->cNumber = 0;

        return  (TRUE);
    }

    return  (FALSE);
}


/*
    IsDriverAttachedToDesktop

    Purpose:    This routine reads a Widnows OS maintained string
                in part of the registry that says whether or not
                the device specified is part of the virtual desktop
                in a multi-mon sceanrio. This way we can tell whether
                the device is enabled or not.

    Arguments:  lpszPciSpecifier
                This is a string that looks something like:
                PCI\VEN_10DE&DEV_0110&SUBSYS_00000000&REV_A1\000800

                We will need to convert this to
                PCI_VEN_10DE&DEV_0110&SUBSYS_00000000&REV_A1_000800_0001

                NOTE that the '\' got changed to an underscore and
                the _0001 got tacked on the end. Look for this key
                under HKEY_CURRENT_CONFIG\Display\Settings. Then go
                into it and look for something called "AttachToDesktop"
                It is a string value and if is present and has value "1"
                then return TRUE. Else we return FALSE.

    Returns:    TRUE    Driver is attached to desktop (device is enabled)
                FALSE   otherwise
*/
int WINAPI
IsDriverAttachedToDesktop (LPCHAR lpszPciSpecifier,ULONG ulDeviceNum)
{
    char    szRegPath[128], szRet[MAX_KEY_LEN];
    int     i, nLen, nRet, nUseCurrentConfig;
    ULONG   lRet, hKey;

    // First read registry to see if a user is logged in. The
    // AttachToDesktop key is valid in different locations in the
    // registry depending on whether or not a user is logged in.
    lstrcpy (szRegPath, "System\\CurrentControlSet\\Control\\");
    nLen = (int) lstrlen (szRegPath);

    lRet = RegOpenKey (HKEY_LOCAL_MACHINE, szRegPath, &hKey);

    // If we don't know, then play it safe and return TRUE
    if  (lRet != ERROR_SUCCESS)
        return  (TRUE);

    // Now read the AttachToDesktop value name
    nRet = GetRegString (hKey, "Current User", szRet);

    // If Current User exists, can only use HKEY_CURRENT_USER if
    // the user in "Current User" has a registry subdirectory in
    // HKEY_USERS.
    nUseCurrentConfig = TRUE;
    if (nRet)
    {
        lRet = RegOpenKey (HKEY_USERS, szRet, &hKey);
        if (lRet == ERROR_SUCCESS)
        {
            nUseCurrentConfig = FALSE;
            RegCloseKey (hKey);
        }
    }

    lstrcpy (szRegPath, "Display\\Settings\\");
    nLen = (int) lstrlen (szRegPath);

    // Copy lpszPciSpecifier replacing any '\' with an underscore
    for (i = 0; lpszPciSpecifier[i] != 0; i++)
    {
        szRegPath[nLen + i] = lpszPciSpecifier[i];
        if  (szRegPath[nLen + i] == '\\')
            szRegPath[nLen + i] = '_';
    }

    // Null terminate it
    szRegPath[nLen + i] = 0;

    // Tack a _0001 on the end for the second device on this board
	if(ulDeviceNum != 0)
	{
		wsprintf (szRegPath + nLen + i, "_000%1d", ulDeviceNum);
	}

    // Open the key. If "Current User" exists, use HKEY_CURRENT_USER;
    // otherwise, use HKEY_CURRENT_CONFIG.
    if (nUseCurrentConfig)
        lRet = RegOpenKey (HKEY_CURRENT_CONFIG, szRegPath, &hKey);
    else
        lRet = RegOpenKey (HKEY_CURRENT_USER, szRegPath, &hKey);

    // If we don't know, then play it safe and return TRUE
    if  (lRet != ERROR_SUCCESS)
        return  (TRUE);

    szRet[0] = 0;

    // Now read the AttachToDesktop value name
    nRet = GetRegString (hKey, "AttachToDesktop", szRet);

    if (nRet == TRUE)
    {
        // AttachToDesktop can be '0'. Make sure the key not only exists
        // but is equal to '1'.
        if (szRet[0] == '1')
            return  (TRUE);
    }

    return  (FALSE);
}


/*
    lpfnResetDisabledDeviceResolutions

    Purpose:    This routine runs through all the devices that have
                ever been secondaries and reset their resolutions
                to 640x480

    Arguments:  None

    Returns:    TRUE
*/
int WINAPI
lpfnResetDisabledDeviceResolutions ()
{
    char    szRegPath[128], szYes[4];
    ULONG   dwSubKey, lRet, dwSize;
    ULONG   hKey, hFullKey;

    lstrcpy (szRegPath, szSettings);

    // Open the key
    lRet = RegOpenKey (HKEY_CURRENT_CONFIG, szRegPath, &hKey);

    // Just return if it failed.
    if  (lRet != ERROR_SUCCESS)
        return  (TRUE);

    for (dwSubKey = 0; TRUE; dwSubKey++)
    {
        lRet = RegEnumKey (hKey, dwSubKey, szExtraKey, MAX_KEY_LEN);
        if  (lRet != ERROR_SUCCESS)
        {
            // If there was an error, it probably means that there are
            // no more subkeys -- i.e. no more secondaries to look at.
            break;
        }

        // Tack the base key onto this and open
        lstrcpy (szFullPath, szRegPath);
        lstrcat (szFullPath, "\\");
        lstrcat (szFullPath, szExtraKey);

        lRet = RegOpenKey (HKEY_CURRENT_CONFIG, szFullPath, &hFullKey);
        if  (lRet == ERROR_SUCCESS)
        {
            // Is this device enabled?
            // Read the AttachToDesktop key
            szYes[0] = 0;
            dwSize = 4;
            lRet = RegQueryValueEx (hFullKey, "AttachToDesktop", NULL,
                                NULL, (LPCHAR) &szYes, &dwSize);

            if  ((szYes[0] == 0) || (szYes[0] == '0'))
            {
                // If the AttachToDesktop key doesn't exist OR
                // it exists AND it is explicitly set to "0", then
                // the device is disabled.

                RegSetValueEx (hFullKey, "Resolution", NULL,
                                    REG_SZ, sz640_480, lstrlen(sz640_480));
            }

            RegCloseKey (hFullKey);
        }
    }

    RegCloseKey (hKey);
    return  (TRUE);
}

int WINAPI
DisplayDriverControl()
{
    //Just set breakpoint here and you will control any driver Loading-Unloading.
    //ax should be the value for SYMLOC
    return  (TRUE);
}
