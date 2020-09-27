/*
    FILE:   modelist.c
    DATE:   4/8/99

    This file holds code to build a modelist from the registry
    and to walk the list looking for different things.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "mywin.h"
#include "regfuncs.h"
#include "debug.h"
#include "utils.h"
#include "..\..\common\inc\nvreg.h"
#include "restime.h"
#include "osapi.h"

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

extern int CFUNC GetSafeModeList (LPMODEENTRY lpModeList);


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

    DBG_PRINT0(DL_0, "\r\nEntering GetMasterModeList.");

    // Next, grab the mode list from the registry
    if  (!(lpML = (LPMODEENTRY) MemoryAlloc ((MAX_MODEENTRYS + 1) * sizeof(MODEENTRY))))
    {
        DBG_PRINT0(DL_0, "\r\nAllocation of Registry Mode List Failed.");
        DBG_PRINT0(DL_0, "\r\nSetting up a default safe mode list.");
        lpML = SafeModeList;
    }

    // Zero out the memory
    lpC = (LPCHAR) lpML;
    for (i = 0; i < ((MAX_MODEENTRYS + 1) * sizeof(MODEENTRY)); i++)
        lpC[i] = 0;

    // Use the safe mode list settings initially
    GetSafeModeList(lpML);

    // Try to get the path in the registry to where the mode list is.
    if  (GetLocalPath (lpDispData, szRegPath) < 0)
    {
        GetSafeModeList(lpML);
        DBG_PRINT0(DL_0, "\r\nGetLocalRegistryPath failed. Returning safe mode list.");
        return  (lpML);
    }

    // Tack on "\\MODES" to the returned regpath
    dwMajorKey = * ((ULONG *) szRegPath);
    lstrcat (szRegPath + 4, "\\Modes");

    // Start off with the first mode
    nMode = 0;

    // Create a loop so we can extract the modes for each pixel depth
    for (pd = 0; pd < 4; pd++)
    {
        // Copy the path to ...DISPLAY\000X\MODES and then tack on
        // the correct pixel depth key
        lstrcpy (szPDPath, szRegPath + 4);

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

    // Add all the non-desktop, low resolution modes.
    for (i = 0; sLowModes[i].dwXRes != 0; i++)
    {
        lpML[nMode] = sLowModes[i];
        if  (nMode < MAX_MODEENTRYS)
            nMode++;
    }

    return  (lpML);
}


/*
    FreeMasterModeList

    This routine is called when the caller is done with the mode
    list it got earlier.
*/
int CFUNC
FreeMasterModeList (LPMODEENTRY lpModeList)
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


