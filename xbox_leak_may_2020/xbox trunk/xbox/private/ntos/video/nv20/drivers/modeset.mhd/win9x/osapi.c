/*
    FILE:   osapi.c
    DATE:   4/8/99

    This file holds code for functions that are called back to by
    the generic modeset code.
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

MODEENTRY   SafeModeList[] =
{
    { 640,  480, 8,  60 },
    { 800,  600, 8,  60 },
    { 1024, 768, 8,  60 },
    { 640,  480, 16, 60 },
    { 800,  600, 16, 60 },
    { 1024, 768, 16, 60 },
    { 640,  480, 32, 60 },
    { 800,  600, 32, 60 },
    { 1024, 768, 32, 60 },
    { 0,    0,   0,  0  },
    { 1,    0,   0,  0  }
};

MODEENTRY   sLowModes[] =
{
    { 320,  200, 8,  60 },
    { 320,  200, 8,  70 },
    { 320,  200, 8,  72 },
    { 320,  200, 8,  75 },
    { 320,  200, 8,  85 },
    { 320,  200, 8,  100 },
    { 320,  200, 8,  120 },
    { 320,  240, 8,  60 },
    { 320,  240, 8,  70 },
    { 320,  240, 8,  72 },
    { 320,  240, 8,  75 },
    { 320,  240, 8,  85 },
    { 320,  240, 8,  100 },
    { 320,  240, 8,  120 },
    { 400,  300, 8,  60 },
    { 400,  300, 8,  70 },
    { 400,  300, 8,  72 },
    { 400,  300, 8,  75 },
    { 400,  300, 8,  85 },
    { 400,  300, 8,  100 },
    { 400,  300, 8,  120 },
    { 480,  360, 8,  60 },
    { 480,  360, 8,  70 },
    { 480,  360, 8,  72 },
    { 480,  360, 8,  75 },
    { 480,  360, 8,  85 },
    { 480,  360, 8,  100 },
    { 480,  360, 8,  120 },
    { 512,  384, 8,  60 },
    { 512,  384, 8,  70 },
    { 512,  384, 8,  72 },
    { 512,  384, 8,  75 },
    { 512,  384, 8,  85 },
    { 512,  384, 8,  100 },
    { 640,  400, 8,  60 },
    { 640,  400, 8,  70 },
    { 640,  400, 8,  72 },
    { 640,  400, 8,  75 },
    { 640,  400, 8,  85 },
    { 640,  400, 8,  100 },
    { 640,  400, 8,  120 },
    { 320,  200, 16, 60 },
    { 320,  200, 16, 70 },
    { 320,  200, 16, 72 },
    { 320,  200, 16, 75 },
    { 320,  200, 16, 85 },
    { 320,  200, 16, 100 },
    { 320,  200, 16, 120 },
    { 320,  240, 16, 60 },
    { 320,  240, 16, 70 },
    { 320,  240, 16, 72 },
    { 320,  240, 16, 75 },
    { 320,  240, 16, 85 },
    { 320,  240, 16, 100 },
    { 320,  240, 16, 120 },
    { 400,  300, 16, 60 },
    { 400,  300, 16, 70 },
    { 400,  300, 16, 72 },
    { 400,  300, 16, 75 },
    { 400,  300, 16, 85 },
    { 400,  300, 16, 100 },
    { 400,  300, 16, 120 },
    { 480,  360, 16, 60 },
    { 480,  360, 16, 70 },
    { 480,  360, 16, 72 },
    { 480,  360, 16, 75 },
    { 480,  360, 16, 85 },
    { 480,  360, 16, 100 },
    { 480,  360, 16, 120 },
    { 512,  384, 16, 60 },
    { 512,  384, 16, 70 },
    { 512,  384, 16, 72 },
    { 512,  384, 16, 75 },
    { 512,  384, 16, 85 },
    { 512,  384, 16, 100 },
    { 640,  400, 16, 60 },
    { 640,  400, 16, 70 },
    { 640,  400, 16, 72 },
    { 640,  400, 16, 75 },
    { 640,  400, 16, 85 },
    { 640,  400, 16, 100 },
    { 640,  400, 16, 120 },
    { 320,  200, 32, 60 },
    { 320,  200, 32, 70 },
    { 320,  200, 32, 72 },
    { 320,  200, 32, 75 },
    { 320,  200, 32, 85 },
    { 320,  200, 32, 100 },
    { 320,  200, 32, 120 },
    { 320,  240, 32, 60 },
    { 320,  240, 32, 70 },
    { 320,  240, 32, 72 },
    { 320,  240, 32, 75 },
    { 320,  240, 32, 85 },
    { 320,  240, 32, 100 },
    { 320,  240, 32, 120 },
    { 400,  300, 32, 60 },
    { 400,  300, 32, 70 },
    { 400,  300, 32, 72 },
    { 400,  300, 32, 75 },
    { 400,  300, 32, 85 },
    { 400,  300, 32, 100 },
    { 400,  300, 32, 120 },
    { 480,  360, 32, 60 },
    { 480,  360, 32, 70 },
    { 480,  360, 32, 72 },
    { 480,  360, 32, 75 },
    { 480,  360, 32, 85 },
    { 480,  360, 32, 100 },
    { 480,  360, 32, 120 },
    { 512,  384, 32, 60 },
    { 512,  384, 32, 70 },
    { 512,  384, 32, 72 },
    { 512,  384, 32, 75 },
    { 512,  384, 32, 85 },
    { 512,  384, 32, 100 },
    { 640,  400, 32, 60 },
    { 640,  400, 32, 70 },
    { 640,  400, 32, 72 },
    { 640,  400, 32, 75 },
    { 640,  400, 32, 85 },
    { 640,  400, 32, 100 },
    { 640,  400, 32, 120 },
    { 720,  480, 16, 60 },
    { 720,  480, 16, 75 },
    { 720,  480, 16, 85 },
    { 720,  480, 32, 60 },
    { 720,  480, 32, 75 },
    { 720,  480, 32, 85 },
    { 720,  576, 16, 60 },
    { 720,  576, 16, 75 },
    { 720,  576, 16, 85 },
    { 720,  576, 32, 60 },
    { 720,  576, 32, 75 },
    { 720,  576, 32, 85 },
    { 960,  720, 8,  60 },
    { 960,  720, 8,  70 },
    { 960,  720, 8,  72 },
    { 960,  720, 8,  75 },
    { 960,  720, 8,  85 },
    { 960,  720, 8,  100 },
    { 960,  720, 8,  120 },
    { 960,  720, 8,  140 },
    { 960,  720, 8,  144 },
    { 960,  720, 8,  150 },
    { 960,  720, 8,  170 },
    { 960,  720, 8,  200 },
    { 960,  720, 8,  240 },
    { 960,  720, 16, 60 },
    { 960,  720, 16, 70 },
    { 960,  720, 16, 72 },
    { 960,  720, 16, 75 },
    { 960,  720, 16, 85 },
    { 960,  720, 16, 100 },
    { 960,  720, 16, 120 },
    { 960,  720, 16, 140 },
    { 960,  720, 16, 144 },
    { 960,  720, 16, 150 },
    { 960,  720, 16, 170 },
    { 960,  720, 16, 200 },
    { 960,  720, 16, 240 },
    { 960,  720, 32, 60 },
    { 960,  720, 32, 70 },
    { 960,  720, 32, 72 },
    { 960,  720, 32, 75 },
    { 960,  720, 32, 85 },
    { 960,  720, 32, 100 },
    { 960,  720, 32, 120 },
    { 960,  720, 32, 140 },
    { 960,  720, 32, 144 },
    { 960,  720, 32, 150 },
    { 960,  720, 32, 170 },
    { 960,  720, 32, 200 },
    { 960,  720, 32, 240 },
    { 0,    0,   0,  0  },
};

#define USE_60HZ                        0x00
#define USE_REFRESH_RATES_FROM_LOWER    0x01
#define USE_REFRESH_RATES_FROM_STRING   0x02

// Just in cae we can't GlobalAlloc a list, we have a single entry one
// we can use here.
RESTIME     sRTList;

extern int CFUNC GetSafeModeList (LPMODEENTRY lpModeList);


/*
    GetModeListFromRegistry

    This routine extracts the mode list for the board requested
    from the registry. This routine will always return some mode
    list.  If it can't find the board requested a safe mode list
    is returned.
*/
LPMODEENTRY CFUNC
GetModeListFromRegistry (LPDISPDATA lpDispData)
{
    ULONG       i, j, pd, dwIndex, dwXRes, dwYRes;
    ULONG       hModeBppKey, hModeResKey, dwSize, dwMatchIndex;
    ULONG       dwSubKey, dwValue;
    ULONG       dwMajorKey;
    ULONG       dwValueNameSize;
    LPCHAR      lpC;
    int         nMode, nFlagRR;
    char        szValueName[MAX_KEY_LEN];
    char        szRegPath[MAX_KEY_LEN];
    char        szPDPath[MAX_KEY_LEN];
    char        szRes[MAX_KEY_LEN];
    char        szResPath[MAX_KEY_LEN];
    char        szRefreshRate[MAX_KEY_LEN];
    long        lRet;
    LPMODEENTRY lpML;

    DBG_PRINT0(DL_0, "\r\nEntering GetModeListFromRegistry.");

    // Next, grab the mode list from the registry
    if  (!(lpML = (LPMODEENTRY) MemoryAlloc ((MAX_MODEENTRYS + 1) * sizeof(MODEENTRY))))
    {
        DBG_PRINT0(DL_0, "\r\nAllocation of Registry Mode List Failed.");
        DBG_PRINT0(DL_0, "\r\nSetting up a default safe mode list.");
        lpML = SafeModeList;
        return  (lpML);
    }

    // Zero out the memory
    lpC = (LPCHAR) lpML;
    for (i = 0; i < ((MAX_MODEENTRYS + 1) * sizeof(MODEENTRY)); i++)
        lpC[i] = 0;

    // Use the safe mode list settings initially
    GetSafeModeList(lpML);

    // Try to get the path in the registry to where the mode list is.
    if  (GetLocalPath (lpDispData->lpBoardRegistryData, szRegPath) < 0)
    {
        GetSafeModeList(lpML);
        DBG_PRINT0(DL_0, "\r\nGetLocalRegistryPath failed. Returning safe mode list.");
        return  (lpML);
    }

    // Tack on "\\MODES" to the returned regpath
    dwMajorKey = lpDispData->lpBoardRegistryData->dwMainKey;
    lstrcat (szRegPath, "\\Modes");

    // Start off with the first mode
    nMode = 0;

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
                break;
            case 1:
                lstrcat (szPDPath, "\\16");
                break;
            case 2:
                lstrcat (szPDPath, "\\24");
                break;
            case 3:
                lstrcat (szPDPath, "\\32");
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
        // look at the same resolution in the next lower pixel depth
        // and use that its refresh rates. Finally, if there is no
        // lower color depth, then assume just a 60Hz refresh rate.
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

            // Build a full path to the resolution key so we can open it
            lstrcpy (szResPath, szPDPath);
            lstrcat (szResPath, "\\");
            lstrcat (szResPath, szRes);

            // OK, now open the spatial resolution key so that we can
            // get the list of refresh rates for this resolution.
            lRet = RegOpenKey (dwMajorKey, szResPath, &hModeResKey);

            if  (lRet == ERROR_SUCCESS)
            {
                // The refresh rate string which looks like "60,72,75"
                // or some such thing, does not have a tag name associated
                // with it, so I cannot just query a value -- there is no
                // name to query the value of!  Must enumerate the values.
                for (dwValue = 0; TRUE; dwValue++)
                {
                    dwValueNameSize = MAX_KEY_LEN;
                    dwSize = MAX_KEY_LEN;
                    lRet = RegEnumValue (hModeResKey, dwValue, szValueName,
                                        &dwValueNameSize, NULL, NULL,
                                        szRefreshRate, &dwSize);


                    if  (lRet == ERROR_SUCCESS)
                    {
                        // We got back a key. Technically we should
                        // check to see if this is a valid refresh rate
                        // string, but we won't. We do a quick check
                        // here to see if the value name is NULL --
                        // that is the way MSoft stores it for some
                        // reason. The actual refresh rate string
                        // values will get pseudo-validated later in
                        // this routine.
                        if  ((dwValueNameSize == 0) &&
                             (szRefreshRate[0] != 0))
                        {
                            // Close the resolution key
                            RegCloseKey (hModeResKey);

                            nFlagRR = USE_REFRESH_RATES_FROM_STRING;
                            break;
                        }
                    }
                    else
                    {
                        // If we got an error, it is porbbaly because
                        // there are no more values to enumerate in the
                        // key. Unfortunately we didn't find a refresh
                        // rate string, so just close the key. The logic
                        // in the next section handles the error.
                        RegCloseKey (hModeResKey);
                        break;
                    }
                }
            }

            if  (lRet != ERROR_SUCCESS)
            {
                // Here's the thing. If either opening the resolution key
                // to query the refresh rate string, OR if the query of
                // the refresh rate string failed, we still have to come
                // up with a list of refresh rates somehow,
                // so check if we are looking for 8bpp modes.
                if  (pd == 0)
                {
                    // In 8bpp, there is no lower pixel depth so we
                    // have to assume a single value of 60Hz
                    nFlagRR = USE_60HZ;
                }
                else
                {
                    // We can make a good guess as to the refresh
                    // rates by using the ones for the same
                    // resolution mode in a lower pixel depth,
                    // if a lower pixel depth has the same
                    // resolution mode. IF not, use 60Hz.
                    nFlagRR = USE_REFRESH_RATES_FROM_LOWER;
                }
            }

            if  (nFlagRR == USE_REFRESH_RATES_FROM_STRING)
            {
                dwIndex = 0;

                // Generate a new mode for each refresh rate in the string
                while   (ParseDecimalSeparator (szRefreshRate,
                            &dwIndex, &(lpML[nMode].dwRefreshRate),
                            ',', 0))
                {
                    lpML[nMode].dwXRes = dwXRes;
                    lpML[nMode].dwYRes = dwYRes;
                    lpML[nMode].dwBpp  = (pd + 1) * 8;
                    if  (nMode < MAX_MODEENTRYS)
                        nMode++;
                }
            }

            if  (nFlagRR == USE_REFRESH_RATES_FROM_LOWER)
            {
                dwMatchIndex = FALSE;

                // Start with the pixel depth immediately below, but
                // if no matching res is found at that one, keep going.
                for (j = pd - 1; (((long) j) >= 0) && (!dwMatchIndex); j--)
                {
                    // Search through all modes already added to the table
                    for (i = 0; i < (ULONG) nMode; i++)
                    {
                        // Matching xres, yres, and prior pixel depth
                        // is a hit!
                        if  ((dwXRes == lpML[i].dwXRes) &&
                             (dwYRes == lpML[i].dwYRes) &&
                             (((j + 1) * 8) == lpML[i].dwBpp))
                        {
                            // Grab the prior mode, and modify the bpp
                            lpML[nMode] = lpML[i];
                            lpML[nMode].dwBpp = (pd + 1) * 8;
                            if  (nMode < MAX_MODEENTRYS)
                                nMode++;
                            dwMatchIndex = TRUE;
                        }
                    }
                }

                if  (!dwMatchIndex)
                {
                    // If there wasn't a lower pixel depth with a
                    // matching resolution, then do a single 60Hz mode.
                    nFlagRR = USE_60HZ;
                }
            }

            if  (nFlagRR == USE_60HZ)
            {
                lpML[nMode].dwXRes = dwXRes;
                lpML[nMode].dwYRes = dwYRes;
                lpML[nMode].dwBpp  = (pd + 1) * 8;
                lpML[nMode].dwRefreshRate = 60;
                if  (nMode < MAX_MODEENTRYS)
                    nMode++;
            }
        }

        // Close this bpp key
        RegCloseKey (hModeBppKey);
    }

    // Only one thing. If we didn't find any modes, then use a safe list
    if  (nMode == 0)
    {
        GetSafeModeList(lpML);
        DBG_PRINT0(DL_0, "\r\nNo modes found. Returning safe mode list.");
        return  (lpML);
    }

    return  (lpML);
}


/*
    GetMasterModeList

    This routine extracts the mode list for the board requested
    from the registry. This routine will always return some mode
    list.  If it can't find the board requested a safe mode list
    is returned.
*/
LPMODEENTRY CFUNC
GetMasterModeList (LPDISPDATA lpDispData)
{
    int         i, nLen;
    LPMODEENTRY lpML;

    DBG_PRINT0(DL_0, "\r\nEntering GetMasterModeList.");

    if(lpDispData->lpModeList != NULL)
        return  lpDispData->lpModeList;

    lpDispData->lpModeList = lpML = GetModeListFromRegistry (lpDispData);

    // Count the number of items in the list
    for (nLen = 0; nLen < MAX_MODEENTRYS; nLen++)
        if  (lpML[nLen].dwXRes == 0)
            break;

    // Add all the non-desktop, low resolution modes.
    for (i = 0; sLowModes[i].dwXRes != 0; i++)
    {
        lpML[nLen] = sLowModes[i];
        if  (nLen < MAX_MODEENTRYS)
            nLen++;
    }

    DBG_PRINT0(DL_0, "\r\nExiting GetMasterModeList.");
    return  (lpML);
}


/*
    FreeMasterModeList

    This routine is called when the caller is done with the mode
    list it got earlier.
*/
int CFUNC
FreeMasterModeListW9X (LPMODEENTRY lpModeList)
{
    if  (lpModeList != SafeModeList)
    {
        DBG_PRINT0(DL_0, "\r\nCalling MemoryFree on lpModeList.");
        MemoryFree ((LPULONG) lpModeList);
    }

    return  (TRUE);
}


/*
    GetSafeModeList

    This routine just returns a few safe modes in the mode list.
    It always returns TRUE.
*/
int CFUNC GetSafeModeList (LPMODEENTRY lpModeList)
{
    int     i;

    for (i = 0; SafeModeList[i].dwXRes != 1; i++)
        lpModeList[i] = SafeModeList[i];

    return  (TRUE);
}


/*
    GetMasterRTList

    This routine extracts the Restriction and Timing list for the
    device requested from the registry. nRTListLen is the length
    in RESTIME structures of the array pointed to by lpRTList.
*/
LPRESTIME CFUNC
GetMasterRTList (LPDISPDATA lpDispData)
{
    int         i;
    long        lRet;
    ULONG       dwSubKey;
    ULONG       dwSize, dwValueNameSize;
    ULONG       hRTKey;
    char        szItem[MAX_KEY_LEN];
    char        szRTString[MAX_KEY_LEN];
    LPRESTIME   lpRTList;

    DBG_PRINT0(DL_0, "\r\nEntering GetMasterRTList.");

    if(lpDispData->lpRTList != NULL)
        return  lpDispData->lpRTList;

    if  (!(lpRTList = (LPRESTIME) MemoryAlloc (MAX_RESTIMES * sizeof(RESTIME))))
    {
        DBG_PRINT0 (DL_0, "\r\nAllocation of Restriction And Timing List Failed.");
        DBG_PRINT0 (DL_0, "\r\nSetting up a default list with no restrictions.");
        lpRTList = &sRTList;

        // Use the dwXRes field in the first array element as a count
        // of the number of restriction list entries.
        lpRTList[0].dwXRes = 0;
        return  (lpRTList);
    }
    else
        lpDispData->lpRTList = lpRTList;

    // Use the dwXRes field in the first array element as a count
    // of the number of restriction list entries.
    lpRTList[0].dwXRes = 0;

    // First, find the path to the restriction and timing strings.
    if  (!(hRTKey = GetLocalNvidiaDisplaySubKey (lpDispData->lpBoardRegistryData)))
    {
        DBG_PRINT0(DL_0, "\r\nGetLocalDisplaySubKey failed. Returning empty RTList.");
        return  (lpRTList);
    }

    // Start enumerating and parsing the R&T strings.
    for (dwSubKey = 0; TRUE; dwSubKey++)
    {
        // Zero out the strings
        for (i = 0; i < MAX_KEY_LEN; i++)
        {
            szItem[i] = 0;
            szRTString[i] = 0;
        }

        dwValueNameSize = MAX_KEY_LEN;
        dwSize = MAX_KEY_LEN;
        lRet = RegEnumValue (hRTKey, dwSubKey, szItem,
                            &dwValueNameSize, NULL, NULL,
                            szRTString, &dwSize);

        // If there was an error, it probably means that there are
        // no more values to enumerate. That's fine. We are done.
        if  (lRet != ERROR_SUCCESS)
            break;

        // Is this subkey an R&T string? It is if it begins "R&T"
        if  ((szItem[0] == 'R') && (szItem[1] == '&') && (szItem[2] == 'T'))
        {
            // If the R&T string parses legitimately, include it in the list.
            if  (lpRTList[0].dwXRes < (MAX_RESTIMES - 1))
            {
                if  (ParseRTString (lpRTList + lpRTList[0].dwXRes + 1, szRTString))
                {
                    DBG_PRINT1(DL_0, "\r\nR&T string %s parsed fine.", (ULONG) szRTString);
                    lpRTList[0].dwXRes++;
                }
            }
        }
    }

    // Close the key
    RegCloseKey (hRTKey);

    return  (lpRTList);
}


/*
    FreeMasterRTList

    This routine is called when the caller is done with the RTList.
*/
int CFUNC
FreeMasterRTListW9X (LPRESTIME lpRTList)
{
    // The dwYRes field in the first array element is used as a flag
    // to indicate that the array was GlobalAlloced.
    if  (lpRTList != &sRTList)
    {
        DBG_PRINT0(DL_0, "\r\nCalling MemoryFree on RTList.");
        MemoryFree ((LPULONG) lpRTList);
    }

    return  (TRUE);
}

/*
    FreeMasterLists

    This routine is called from Display driver when the CRTC context is destroying.
*/
int WINAPI
FreeMasterLists ( LPDISPDATA lpDispData )
{
    if(lpDispData->lpRTList)
    {
        FreeMasterRTListW9X (lpDispData->lpRTList);
        lpDispData->lpRTList = NULL;
    }

    if(lpDispData->lpModeList)
    {
        FreeMasterModeListW9X (lpDispData->lpModeList);
        lpDispData->lpModeList = NULL;
    }
    return  (TRUE);
}


/*
    FreeMasterModeList

    This routine is called when the caller is done with the ModeList.
*/
int CFUNC
FreeMasterModeList (LPMODEENTRY lpModeList)
{
    return  (TRUE);
}


/*
    FreeMasterRTList

    This routine is called when the caller is done with the RTList.
*/
int CFUNC
FreeMasterRTList (LPRESTIME lpRTList)
{
    return  (TRUE);
}

/*
    GetTimingStandardOverride

    This routine gets any timing standard override for the board,
    logical device, and output device specified.
    It should return one of the constants defined in
    restime.h like DMTV_ID. A timing standard override is a
    timing standard that applies to all modes. Just something
    that the user might want to enforce.
*/
ULONG CFUNC
GetTimingStandardOverride (LPDISPDATA lpDispData)
{
    char    szVal[32];

    if (GetLocalNvidiaDisplayDevRegString (lpDispData->lpBoardRegistryData,
        (LPDEVDATA) lpDispData->lpDevData, NV4_REG_DRV_MONITOR_TIMING, szVal))
    {
        if  (!lstrcmpi(szVal, NV4_REG_DRV_MONITOR_TIMING_DMT))
            return  (DMTV_ID);

        if  (!lstrcmpi(szVal, NV4_REG_DRV_MONITOR_TIMING_GTF))
            return  (GTFV_ID);
    }

    return  (NONE_ID);
}


/*
    Function:   GetPitchAndSize

    Purpose:    This routine returns the pitch for a given mode.

    Arguments:  lpDispData  - identifies the specific board that we
                              want the active display port for.
                dwXRes      - The X res in pixels for the mode
                              should be returned.
                dwBpp       - the bits per pixel for the mode

    Returns:    The pitch in bytes
*/
typedef ULONG   (__far __pascal *LPFNGETMODESIZE) (PVOID, PVOID, ULONG, ULONG, ULONG, ULONG *, ULONG *);

ULONG CFUNC
GetPitchAndSize (LPDISPDATA lpDispData, ULONG dwXRes, ULONG dwYRes, ULONG dwBpp, ULONG *pdwPitch, ULONG *pdwSize)
{
    LPFNGETMODESIZE lpfnGetModeSize;

    lpfnGetModeSize = (LPFNGETMODESIZE) lpDispData->lpfnGetModeSize;
    return  ((*lpfnGetModeSize) (lpDispData->dwContext1, lpDispData->dwContext2, dwXRes, dwYRes, dwBpp, pdwPitch, pdwSize));
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
                lpDispData  - identifies the specific board that we
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
int CFUNC
GetMonitorInfRangeLimits (
    LPDISPDATA      lpDispData,
    LPMONITORLIMITS lpML,
    ULONG           *lpdwXRes,
    ULONG           *lpdwYRes)
{
    ULONG   i;
    char    szMaxResolution[10];
    ULONG   hMonitor, lRet;
    ULONG   dwSize;
    ULONG   dwHorizMin;
    ULONG   dwHorizMax;
    ULONG   dwVertMin;
    ULONG   dwVertMax;
    char    szBuffer[MAX_KEY_LEN];
    GTFIN   GTFIn;
    GTFOUT  GTFOut;

    // First zero out monitor limits structure.
    lpML->dwMinHorz = 0;
    lpML->dwMaxHorz = 0;
    lpML->dwMinVert = 0;
    lpML->dwMaxVert = 0;
    lpML->dwMaxPixelClock = 0;

    szMaxResolution[0] = 0;
    if  (!GetMonitorLocalRegString (lpDispData->lpMonitorRegistryData,
                                "MaxResolution", szMaxResolution))
        return  (FALSE);

    // If no string was returned from RegQueryEx, just return 60Hz.
    if (!szMaxResolution[0])
        return (FALSE);

    // Now convert the strings to numbers.
    i = 0;
    *lpdwXRes = 0;
    *lpdwYRes = 0;
    ParseDecimalSeparator (szMaxResolution, &i, lpdwXRes, ',', ',');
    ParseDecimalSeparator (szMaxResolution, &i, lpdwYRes, 0, 0);

    // Create path for MODES subkey.
    lstrcpy (szBuffer, lpDispData->lpMonitorRegistryData->szRegPath);
    lstrcat (szBuffer, "\\MODES");
    lstrcat (szBuffer, "\\");
    lstrcat (szBuffer, szMaxResolution);
    lRet = RegOpenKey (lpDispData->lpMonitorRegistryData->dwMainKey, szBuffer, &hMonitor);
    if  (lRet != ERROR_SUCCESS)
        return  (FALSE);

    szBuffer[0] = 0;
    dwSize = MAX_KEY_LEN;
    RegQueryValueEx (hMonitor, "Mode1", NULL, NULL,
                         szBuffer, (ULONG FAR *) &dwSize);

    RegCloseKey (hMonitor);

    // If no string was returned from RegQueryEx, just return 60Hz.
    if (!szBuffer[0])
        return (FALSE);

    // Now convert the strings to numbers. Be careful here because some
    // INF range limits are in the form xx.x-yy.y (a range) while others
    // are of the form xx.x (fixed frequency). For fixed frequency, set
    // max horizontal and max vertical to the same value as the minimums.
    i = 0;
    ParseDecimalNumberAndFractionAndSeparator (szBuffer, &i, &dwHorizMin, '-', ',');
    if (i > 0)
    {
        if (szBuffer[i - 1] == ',')
            dwHorizMax = dwHorizMin;
        else
            ParseDecimalNumberAndFractionAndSeparator (szBuffer, &i, &dwHorizMax, ',', ',');
    }
    ParseDecimalNumberAndFractionAndSeparator (szBuffer, &i, &dwVertMin, '-', ',');
    if (i > 0)
    {
        if (szBuffer[i - 1] == ',')
            dwVertMax = dwVertMin;
        else
            ParseDecimalNumberAndFractionAndSeparator (szBuffer, &i, &dwVertMax, ',', ',');
    }

    // Change horizontals from KHz to Hz.
    dwHorizMin = Multiply32By32(dwHorizMin, 1000);
    dwHorizMax = Multiply32By32(dwHorizMax, 1000);

    lpML->dwMinHorz = dwHorizMin;
    lpML->dwMaxHorz = dwHorizMax;
    lpML->dwMinVert = dwVertMin;
    lpML->dwMaxVert = dwVertMax;

    // Find a pixel clock limit.
    GTFIn.dwHPixels      = *lpdwXRes;
    GTFIn.dwVLines       = *lpdwYRes;
    GTFIn.dwMarginsRqd   = FALSE;
    GTFIn.dwIntRqd       = FALSE;
    GTFIn.dwIPParmType   = IPTYPE_VERTICAL_REFRESH_RATE;
    GTFIn.dwIPFreqRqd    = lpML->dwMaxVert;

    // Get the timing parameters.
    ComputeGTFParameters(&GTFIn, &GTFOut);
    lpML->dwMaxPixelClock = Multiply32By32(GTFOut.dwPixelClockIn10KHertz, 10000);

    // Make sure there's at least some kind of pixel clock. The fallback
    // pixel clock being used is the pixel clock for DMT 640x480@60Hz.
    if (lpML->dwMaxPixelClock == 0)
        lpML->dwMaxPixelClock = 25180000;

    return  (TRUE);
}


/*
    Function:   GetMonitorLimitMode

    Purpose:    This routine gets the mode which should be an
                upper bound for a monitor which has no edid and
                no .inf.

    Arguments:  lpDispData  - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - ptr to a place to store the limit mode

    Returns:    If this routine returns TRUE, then lpModeInfo was
                filled in with the limit mode. If this routine
                returns FALSE, then there is no limit mode.
*/
int CFUNC
GetMonitorLimitMode (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo)
{
    return  (GrabModeInfoString (lpDispData->lpBoardRegistryData,
                0x0FFFFFFFF, NULL,
                NV4_REG_DRV_MONITOR_LIMIT_MODE, lpModeInfo, 0));
}

