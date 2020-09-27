/*
    FILE:   restime.c
    DATE:   4/8/99

    This file holds code to process restriction and timing lists
    and also a few utility routines to get mode lists from the
    registry, etc.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "utils.h"
#include "restime.h"
#include "debug.h"

// Just in cae we can't GlobalAlloc a list, we have a single entry one
// we can use here.
RESTIME     sRTList;

/*
    ParseRTString

    This routine parses an R&T string given by szRTString into the
    RESTIME structure pointed to by lpRT. The function returns
    TRUE if the szRTString was a valid R&T string and parsed with
    no errors. Otherwise the routine returns FALSE.
*/
int CFUNC
ParseRTString (LPRESTIME lpRT, LPCHAR szRTString)
{
    int     nRet;
    ULONG   dwIndex;

    DBG_PRINT1(DL_5, "\r\nParsing R&T string %s", szRTString);

    // dwIndex is the offset into the szRTString we are currently looking at
    dwIndex = 0;

    // The first thing in an R&T string is the X-resolution. Make sure
    // it is there and get its value.
    if  (!ParseDecimalNumberComma (szRTString, &dwIndex, &(lpRT->dwXRes)))
        return  (FALSE);
    DBG_PRINT1(DL_5, "\r\nXRes found to be %ld", lpRT->dwXRes);

    // The next thing in an R&T string is the Y-resolution. Make sure
    // it is there and get its value.
    if  (!ParseDecimalNumberComma (szRTString, &dwIndex, &(lpRT->dwYRes)))
        return  (FALSE);
    DBG_PRINT1(DL_5, "\r\nYRes found to be %ld", lpRT->dwYRes);

    // The next thing in an R&T string is the pixel depth. This
    // is also allowed to be a wildcard indicating everything.
    if  (!ParseDecimalNumberOrWildcard (szRTString, &dwIndex, &(lpRT->dwPixelDepth)))
        return  (FALSE);
    DBG_PRINT1(DL_5, "\r\nPixelDepth found to be %ld", lpRT->dwPixelDepth);

    // The next thing in an R&T string is the refresh rate. This
    // is also allowed to be a wildcard indicating everything.
    if  (!ParseDecimalNumberOrWildcard (szRTString, &dwIndex, &(lpRT->dwRefreshRate)))
        return  (FALSE);
    DBG_PRINT1(DL_5, "\r\nRefreshRate found to be %ld", lpRT->dwRefreshRate);

    // The next thing in an R&T string is a dot separated list
    // of Device IDs.
    if  (!ParseDeviceIDs (szRTString, &dwIndex, lpRT->wDeviceID))
        return  (FALSE);

    // The next thing in an R&T string is a dot separated list
    // of Monitor IDs.
    if  (!ParseMonitorIDs (szRTString, &dwIndex, lpRT->dwMonitorID))
        return  (FALSE);

    // The next thing in an R&T string is a dot separated list
    // of Timing Standards.
    nRet = ParseTimingStandards (szRTString, &dwIndex, lpRT->dwTimingStandard);
    if  (!nRet)
        return  (FALSE);

    // If one of the timing standards was OEM, then there must be a set
    // of timing parameters in a MODEOUT format as the last thing in the
    // R&T string. We know if there was an OEM timing standard because
    // ParseTimingStandard returns a number greater than 1 if there was
    // an OEM timing standard listed.
    if  (nRet > 1)
    {
        // The timing parameter format is hexdigits;hexdigits; etc
        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wHBlankStart)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nHBlankStart found to be %lx", (ULONG) lpRT->sModeOut.wHBlankStart);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wHBlankWidth)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nHblankWidth found to be %lx", (ULONG) lpRT->sModeOut.wHBlankWidth);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wHSyncStart)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nHSyncStart found to be %lx", (ULONG) lpRT->sModeOut.wHSyncStart);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wHSyncWidth)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nHSyncWidth found to be %lx", (ULONG) lpRT->sModeOut.wHSyncWidth);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wVBlankStart)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nVBlankStart found to be %lx", (ULONG) lpRT->sModeOut.wVBlankStart);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wVBlankWidth)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nHblankWidth found to be %lx", (ULONG) lpRT->sModeOut.wVBlankWidth);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wVSyncStart)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nVSyncStart found to be %lx", (ULONG) lpRT->sModeOut.wVSyncStart);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wVSyncWidth)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nVSyncWidth found to be %lx", (ULONG) lpRT->sModeOut.wVSyncWidth);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wHTotal)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nXTotal found to be %lx", (ULONG) lpRT->sModeOut.wHTotal);

        if  (!ParseHexWord (szRTString, &dwIndex, &(lpRT->sModeOut.wVTotal)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nYTotal found to be %lx", (ULONG) lpRT->sModeOut.wVTotal);

        if  (!ParseHexDWord (szRTString, &dwIndex, &(lpRT->sModeOut.dwPixelClock)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nPixelClock found to be %lx", lpRT->sModeOut.dwPixelClock);

        if  (!ParseHexDWord (szRTString, &dwIndex, &(lpRT->sModeOut.dwFormat)))
            return  (FALSE);
        DBG_PRINT1(DL_5, "\r\nFormat found to be %lx", lpRT->sModeOut.dwFormat);

        lpRT->sModeOut.wXRes = (USHORT) lpRT->dwXRes;
        lpRT->sModeOut.wYRes = (USHORT) lpRT->dwYRes;
        lpRT->sModeOut.wRefreshRate = (USHORT) lpRT->dwRefreshRate;
    }

    return  (TRUE);
}


/*
    ParseDecimalNumberComma

    This routine parses a field that can either be a decimal number
    or the wildcard character. Basically, if it isn't the wildcard
    character, then it is assumed to be a decimal number
    This routine parses a decimal number from the string passed in.
    It assigns it into lpNum. Upkn returning lpIndex is adjusted so
    that it points to the first char after the comma following the
    number parsed. If the number was bad, could not be found before
    the end of the string or before the next comma, FALSE is returned.
    If a number was found and everything is fine, TRUE is returned.
    in RESTIME structures of the array pointed to by lpRTList.
*/
int CFUNC
ParseDecimalNumberComma (LPCHAR szRTString, LPULONG lpIndex, LPULONG lpNum)
{
    return  (ParseDecimalSeparator (szRTString, lpIndex, lpNum, ',', ','));
}



/*
    ParseDecimalNumberOrWildcard

    This routine parses a field that can either be a decimal number
    or the wildcard character. Basically, if it isn't the wildcard
    character, then it is assumed to be a decimal number
    This routine parses a decimal number from the string passed in.
    It assigns it into lpNum. Upkn returning lpIndex is adjusted so
    that it points to the first char after the comma following the
    number parsed. If the number was bad, could not be found before
    the end of the string or before the next comma, FALSE is returned.
    If a number was found and everything is fine, TRUE is returned.
    in RESTIME structures of the array pointed to by lpRTList.
*/
int CFUNC
ParseDecimalNumberOrWildcard (LPCHAR szRTString, LPULONG lpIndex, LPULONG lpNum)
{
    if  (szRTString[*lpIndex] == ALL_WILDCARD)
    {
        *lpNum = ALL_WILDCARD;

        // If the next char is not immediately a comma, then error
        if  (szRTString[*lpIndex + 1] != ',')
        {
            DBG_PRINT1(DL_5, "\r\nComma not found after wildcard number in %s.", szRTString);
            return  (FALSE);
        }

        *lpIndex += 2;
    }
    else
    {
        // Not a wildcard, so parse a number
        if  (!ParseDecimalNumberComma (szRTString, lpIndex, lpNum))
            return  (FALSE);
    }

    return  (TRUE);
}



/*
    ParseDeviceIDs

    This routine parses a '.' separated list of DeviceIDs. There can
    be up to MAX_DOT_FIELDS deviceIDs. Any more than this constitutes
    a parse error. Each deviceID is assumed to be exactly four hex
    digits long. The alphabetic characters can be upper or lower case.
*/
int CFUNC
ParseDeviceIDs (LPCHAR szRTString, LPULONG lpIndex, LPUSHORT lpNum)
{
    int     nRet;
    ULONG   dwID, dwNum;

    // Set all the device IDs to 0
    for (dwID = 0; dwID < MAX_DOT_FIELDS; dwID++)
        lpNum[dwID] = 0;

    for (dwID = 0; TRUE; dwID++)
    {
        // The deviceID can be a WILDCARD char
        if  (szRTString[*lpIndex] == ALL_WILDCARD)
        {
            lpNum[dwID] = ALL_WILDCARD;
            *lpIndex += 1;
        }
        else
        {
            nRet = ParseHexSeparator (szRTString, lpIndex,
                                     &dwNum, 4, ',', '.');

            if  (!nRet)
            {
                DBG_PRINT1(DL_5, "\r\nBad DeviceID in %s", szRTString);
                return  (FALSE);
            }

            // We found a number!
            lpNum[dwID] = (USHORT) dwNum;

            // Backup pointer to put to separator.
            *lpIndex = *lpIndex - 1;
        }

        // If the next char is not either a '.' indicating another
        // deviceID follows, or a ',' indicating we are done with the
        // deviceID parsing, then there is a parse error.
        if  (szRTString[*lpIndex] == ',')
        {
            *lpIndex += 1;
            DBG_PRINT1(DL_5, "\r\nFound %ld DeviceIDs.", dwID + 1);
            return  (TRUE);
        }

        if  (szRTString[*lpIndex] != '.')
        {
            DBG_PRINT1(DL_5, "\r\nBad DeviceID separator in %s", szRTString);
            return  (FALSE);
        }

        // Skip over the '.' and keep going for the next deviceID
        *lpIndex += 1;

    }

    DBG_PRINT1(DL_5, "\r\nFound %ld deviceIDs.", dwID + 1);
    return  (TRUE);
}


/*
    ParseMonitorIDs

    This routine parses a '.' separated list of MonitorIDs. There can
    be up to MAX_DOT_FIELDS deviceIDs. Any more than this constitutes
    a parse error. Each MonitorID is assumed to be exactly four byte
    characters long. An ALL_WILDCARD character is allowed.
*/
ULONG CFUNC
ParseIsHexChar (UCHAR c)
{
    if  ((c >= '0') && (c <= '9'))
        return  (c - '0');

    if  ((c >= 'A') && (c <= 'F'))
        return  ((c - 'A') + 10);
    
    if  ((c >= 'a') && (c <= 'f'))
        return  ((c - 'a') + 10);

    return  (0xFFFFFFFF);
}


/*
    ParseMonitorIDs

    This routine parses a '.' separated list of MonitorIDs. There can
    be up to MAX_DOT_FIELDS deviceIDs. Any more than this constitutes
    a parse error. Each MonitorID is assumed to be exactly four byte
    characters long. An ALL_WILDCARD character is allowed.
*/
int CFUNC
ParseMonitorIDs (LPCHAR szRTString, LPULONG lpIndex, LPULONG lpNum)
{
    ULONG   c1, c2;
    int     i;
    ULONG   dwID;

    // Set all the monitor IDs to 0
    for (dwID = 0; dwID < MAX_DOT_FIELDS; dwID++)
        lpNum[dwID] = 0;

    for (dwID = 0; TRUE; dwID++)
    {
        // The monitorID can be a WILDCARD char
        if  (szRTString[*lpIndex] == ALL_WILDCARD)
        {
            lpNum[dwID] = ALL_WILDCARD;
            *lpIndex += 1;
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                c1 = ParseIsHexChar(szRTString[*lpIndex + i * 2 + 0]);
                c2 = ParseIsHexChar(szRTString[*lpIndex + i * 2 + 1]);

                if  ((c1 == 0xFFFFFFFF) || (c2 == 0xFFFFFFFF))
    
                {
                    DBG_PRINT1(DL_5, "\r\nBad MonitorID in %s", szRTString);
                    return  (FALSE);
                }

                lpNum[dwID] |= ((c1 * 0x10) + c2) << (24 - i * 8);
            }

            *lpIndex += 8;
        }

        // If the next char is not either a '.' indicating another
        // monitorID follows, or a ',' indicating we are done with the
        // monitorID parsing, then there is a parse error.
        if  (szRTString[*lpIndex] == ',')
        {
            *lpIndex += 1;
            DBG_PRINT1(DL_5, "\r\nFound %ld monitorIDs.", dwID + 1);
            return  (TRUE);
        }

        if  (szRTString[*lpIndex] != '.')
        {
            DBG_PRINT1(DL_5, "\r\nBad MonitorID separator in %s", szRTString);
            return  (FALSE);
        }

        // Skip over the '.' and keep going for the next monitorID
        *lpIndex += 1;
    }

    return  (TRUE);
}


/*
    ParseTimingStandards

    This routine parses a '.' separated list of Timing Standards. There
    can be up to MAX_DOT_FIELDS Timing Standards. Any more than this
    constitutes a parse error. Each Timing Standard is up to four
    characters long. The permissable ones are in restime.h

    Returns: 0 is returned if there was any parse error
             1 is returned if there were no parse errors but OEMN was
                 not one of the timing standards
             2 is returned if there were no parse errors and OEMN was
                 one of the timing standards.
*/
int CFUNC
ParseTimingStandards (LPCHAR szRTString, LPULONG lpIndex, LPULONG lpNum)
{
    ULONG   dwID, dwTS;
    int     nRet;

    nRet = 1;

    // Set all the Timing Standards to 0
    for (dwID = 0; dwID < MAX_DOT_FIELDS; dwID++)
        lpNum[dwID] = 0;

    for (dwID = 0; TRUE; dwID++)
    {
        // Make sure the next thing is a Timing Stndard
        dwTS = * ((LPULONG) (szRTString + *lpIndex));

        if  ((dwTS != NONE_ID) &&
             (dwTS != EDID_ID) &&
             (dwTS != DMTV_ID) &&
             (dwTS != DMTR_ID) &&
             (dwTS != GTFV_ID) &&
             (dwTS != GTFR_ID) &&
             (dwTS != OEMX_ID) &&
             (dwTS != OEMR_ID) &&
             (dwTS != FGTF_ID) &&
             (dwTS != FGTR_ID))
        {
            DBG_PRINT1(DL_5, "\r\nBad Timing Standard in %s", szRTString);
            return  (0);
        }

        if  ((dwTS == OEMX_ID) || (dwTS == OEMR_ID))
            nRet = 2;

        lpNum[dwID] = dwTS;
        *lpIndex += 4;

        // If the next char is not either a '.' indicating another
        // TimingStandard follows, or a ',' indicating we are done
        // with the TimingStandard parsing, then there is a parse error.
        if  ((szRTString[*lpIndex] == ',') || (szRTString[*lpIndex] == 0))
        {
            DBG_PRINT1(DL_5, "\r\nFound %ld Timing Standards.", dwID + 1);

            // If a comma follows, skip over it
            if  (szRTString[*lpIndex] == ',')
                *lpIndex += 1;

            return  (nRet);
        }

        if  (szRTString[*lpIndex] != '.')
        {
            DBG_PRINT1(DL_5, "\r\nBad Timing Standard separator in %s", szRTString);
            return (0);
        }

        // Skip over the '.' and keep going for the next timing standard
        *lpIndex += 1;
    }

    return  (TRUE);
}




/*
    RTFindNextMatchingString

    This procedure finds a string in the RTList that matches all the
    parameters in the R&T string. This is board data, mode, and
    monitor id. It returns an index into the list. If no match is
    found, then RTERR_NOT_FOUND is returned.
*/

int CFUNC
RTFindNextMatchingString (
    LPRESTIME   lpRTList,
    int         nIndex,
    LPDISPDATA  lpDispData,
    LPRTMODE    lpRTMode,
    ULONG       dwMonitorID)
{
    int i, b;
    int bMatchDevice, bMatchMode, bMatchMonitor;

    if  ((ULONG) nIndex > lpRTList->dwXRes)
    {
        // The requested index of the string at which to begin
        // searching is larger than the total number of R&T
        // strings in the array. So return error.
        return  (RTERR_NOT_FOUND);
    }

    for  (i = nIndex; i <= (int) lpRTList->dwXRes; i++)
    {
        bMatchDevice  = FALSE;
        bMatchMode    = FALSE;
        bMatchMonitor = FALSE;
        for (b = 0; b < MAX_DOT_FIELDS; b++)
        {
            // Return index if there is a wildcard match
            if  (lpRTList[i].wDeviceID[b] == ALL_WILDCARD)
                bMatchDevice = TRUE;

            // Return index if there is an exact match
            if  (lpRTList[i].wDeviceID[b] == (USHORT) lpDispData->dwDeviceID)
                bMatchDevice = TRUE;
        }

        // Match on the mode
        if  ((lpRTList[i].dwXRes == lpRTMode->dwXRes) &&
             (lpRTList[i].dwYRes == lpRTMode->dwYRes) &&
             ((lpRTList[i].dwPixelDepth == ALL_WILDCARD) ||
              (lpRTList[i].dwPixelDepth == lpRTMode->dwBpp)) &&
             ((lpRTList[i].dwRefreshRate == ALL_WILDCARD) ||
              (lpRTList[i].dwRefreshRate == lpRTMode->dwRefreshRate)))
            bMatchMode = TRUE;

        for (b = 0; b < MAX_DOT_FIELDS; b++)
        {
            // Return success on a wildcard match
            if  (lpRTList[i].dwMonitorID[b] == ALL_WILDCARD)
                bMatchMonitor = TRUE;;

            // Return success on an exact match
            if  (lpRTList[i].dwMonitorID[b] == dwMonitorID)
                bMatchMonitor = TRUE;;
        }
        
        if  (bMatchDevice && bMatchMode & bMatchMonitor)
            return  (i);
    }

    return  (RTERR_NOT_FOUND);
}


/*
    RTRestrictByTimingStandard

    This procedure looks at the R&T string given by nIndex in the
    lpRTList and sees if there is a match on the TimingStandard given
    by dwTimingStandard. If there is, TRUE is returned, else FALSE is
    returned.
*/
int CFUNC
RTRestrictByTimingStandard (
    LPRESTIME   lpRTList,
    int         nIndex,
    ULONG       dwTimingStandard)
{
    int b;

    for (b = 0; b < MAX_DOT_FIELDS; b++)
        if  (lpRTList[1].dwTimingStandard[b] == dwTimingStandard)
            return  (TRUE);

    return  (FALSE);
}

