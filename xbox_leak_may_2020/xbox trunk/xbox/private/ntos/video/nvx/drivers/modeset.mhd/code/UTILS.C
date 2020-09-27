/*
    FILE:   utils.c
    DATE:   4/8/99

    This file provides common utilities other modules.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "utils.h"
#include "debug.h"
#include "gtfmath.h"

/*
    Function:   FindMode

    Purpose:    This routine searches the table passed in lpMS for the
                XRes, YRes, and RRate specified.

    Arguments:  lpMS        ptr to current MODESTRUC table
                lpModeOut   ptr to MODEOUT structure where timing
                            parameters are placed if the mode given
                            by lpMS is found.
                dwXRes      X-Resolution
                dwYRes      Y-Resolution
                dwRRate     Refresh Rate

    Returns:    Index into MODESTRUC table if a matching mode is found.
                -1 if no matching mode is found.
*/

int CFUNC
FindMode (
    LPMODESTRUC lpMS,
    ULONG       dwXRes,
    ULONG       dwYRes,
    ULONG       dwRRate)
{
    int     i;

    for (i = 0; lpMS[i].wHorizVisible != 0x00; i++)
    {
        if (((ULONG) lpMS[i].wHorizVisible == dwXRes) &&
            ((ULONG) lpMS[i].wVertVisible == dwYRes)  &&
            ((ULONG) lpMS[i].wRefresh == dwRRate))
        return  (i);
    }

    return  (-1);
}



/*
    Function:   FindModeWithoutRR

    Purpose:    This routine searches the table passed in lpMS for the
                XRes, YRes specified with the highest RRate.

    Arguments:  lpMS        ptr to current MODESTRUC table
                dwXRes      X-Resolution
                dwYRes      Y-Resolution
                dwRRate     Refresh Rate

    Returns:    Index into MODESTRUC table if a matching mode is found.
                -1 if no matching mode is found.
*/

int CFUNC
FindModeWithoutRR (
    LPMODESTRUC lpMS,
    ULONG       dwXRes,
    ULONG       dwYRes)
{
    int     i;

    for (i = 0; lpMS[i].wHorizVisible != 0x00; i++)
    {
        if (((ULONG) lpMS[i].wHorizVisible == dwXRes) &&
            ((ULONG) lpMS[i].wVertVisible == dwYRes))
        return  (i);
    }

    return  (-1);
}



/*
    Function:   SetupParamsForVidLutClass

    Purpose:    This routine translates the mode found into the
                parameter format of the VIDEOLUTCURSORDAC class.

    Arguments:  lpMS        ptr to MODESTRUC structure
                lpModeOut   ptr to MODEOUT structure where parameters go.
                lpModeIn    MODEINFO ptr describing mode

    Returns:    Ptr to table with parameters in proper format for the
                VIDEOLUTCURSORDAC class.
*/

int CFUNC
SetupParamsForVidLutClass (
    LPMODESTRUC         lpMS,
    LPMODEOUT           lpModeOut,
    LPMODEINFO          lpModeIn)
{
    ULONG   dwFormat;

    lpModeOut->wXRes        = lpMS->wHorizVisible;
    lpModeOut->wYRes        = lpMS->wVertVisible;
    lpModeOut->wRefreshRate = lpMS->wRefresh;
    lpModeOut->wHBlankStart = lpMS->wHorizBlankStart;
    lpModeOut->wHBlankWidth = lpMS->wHorizBlankEnd - lpMS->wHorizBlankStart;
    lpModeOut->wHSyncStart  = lpMS->wHorizSyncStart;
    lpModeOut->wHSyncWidth  = lpMS->wHorizSyncEnd - lpMS->wHorizSyncStart;
    lpModeOut->wVBlankStart = lpMS->wVertBlankStart;
    lpModeOut->wVBlankWidth = lpMS->wVertBlankEnd - lpMS->wVertBlankStart;
    lpModeOut->wVSyncStart  = lpMS->wVertSyncStart;
    lpModeOut->wVSyncWidth  = lpMS->wVertSyncEnd - lpMS->wVertSyncStart;
    lpModeOut->wHTotal      = lpMS->wHorizTotal;
    lpModeOut->wVTotal      = lpMS->wVertTotal;

    lpModeOut->dwPixelClock = (ULONG) lpMS->wDotClock;

    dwFormat = 0;
    if  (lpModeOut->wYRes < 400)    // if YRes < 400 then this is a double
        dwFormat |= 1;              // scan mode, set dwFormat[0] = 1
    if  (!lpMS->wHSyncPolarity)     // 1 = positive polarity in tbl
        dwFormat |= 4;              // 1 = negative polarity in format field
    if  (!lpMS->wVSyncPolarity)     // 1 = positive polarity in tbl
        dwFormat |= 8;              // 1 = negative polarity in format field

    if  (lpModeIn->dwBpp == 15)
        dwFormat |= 0x20000;        // 15 BPP
    if  (lpModeIn->dwBpp == 16)
        dwFormat |= 0x10000;        // 16 BPP
    if  (lpModeIn->dwBpp == 32)
        dwFormat |= 0x30000;        // 32 BPP

    // Set the display type. Note: DEVTYPE_CRT = 0 so no bits need to be
    // set in dwFormat.
    if  (lpModeIn->MIDevData.cType == DEVTYPE_DFP)
        dwFormat |= 0x300000;
    if  (lpModeIn->MIDevData.cType == DEVTYPE_TV)
    {
        dwFormat |= 0x100000;
        // Set the TV Format
        dwFormat |= ((ULONG) (lpModeIn->MIDevData.cFormat)) << 22;
    }

    lpModeOut->dwFormat = dwFormat; // Store Format

    return  (TRUE);
}



/*
    Function:   SetupParamsForModeINfo

    Purpose:    This routine translates the mode found into the
                parameter format of the MODEINFO structure.

    Arguments:  lpModeOut   MODEOUT ptr to mode found
                lpModeOrig  This is the originally requested mode
                lpModeIn    This will be a MODEINFO structure that
                            describes the same thing as the lpModeOut
                            describes

    Returns:    dwXRes, dwYRes, dwBpp, dwRefreshRate from MODEOUT structure
                in MODEINFO structure.
*/

int CFUNC
SetupParamsForModeInfo (
    LPMODEOUT           lpModeOut,
    LPMODEINFO          lpModeOrig,
    LPMODEINFO          lpModeIn)
{
    ULONG   dwBpp;

    *lpModeIn = *lpModeOrig;

    dwBpp = 0;
    if ((lpModeOut->dwFormat & 0x30000) == 0x00000)
        dwBpp = 8;
    if ((lpModeOut->dwFormat & 0x30000) == 0x20000)
        dwBpp = 15;
    if ((lpModeOut->dwFormat & 0x30000) == 0x10000)
        dwBpp = 16;
    if ((lpModeOut->dwFormat & 0x30000) == 0x30000)
        dwBpp = 32;

    lpModeIn->dwXRes        = (ULONG) lpModeOut->wXRes;
    lpModeIn->dwYRes        = (ULONG) lpModeOut->wYRes;
    lpModeIn->dwBpp         = dwBpp;
    lpModeIn->dwRefreshRate = (ULONG) lpModeOut->wRefreshRate;

    return  (TRUE);
}


/*
    ParseDecimalSeparator

    This routine parses a decimal number from the string passed in. It
    assigns it into lpNum. Upon returning lpIndex is adjusted so that it
    points to the first char after the separator following the number
    parsed. If a NULL followed the number parse (i.e. end of the string),
    lpIndex is left pointing to the NULL, but the routine still
    succeeds. If the number was bad or wasn't found before the comma
    separatorthe separator wasn't a could not be found before the end of
    the string or before the next comma, FALSE is returned. If a number
    was found and everything is fine, TRUE is returned.
*/
int CFUNC
ParseDecimalSeparator (
    LPCHAR  lpStr,
    LPULONG lpIndex,
    LPULONG lpNum,
    char    cSeparator1,
    char    cSeparator2)
{
    ULONG   dwIsNum;
    USHORT  wDec;

    *lpNum  = 0;
    dwIsNum = FALSE;
    while   (TRUE)
    {
        if  ((lpStr[*lpIndex] == cSeparator1) ||
             (lpStr[*lpIndex] == cSeparator2))
        {
            // If we did not find at least one decimal digit, then
            // the Num field was not present -- this is an error.
            if  (!dwIsNum)
            {
                DBG_PRINT1(DL_5, "\r\nNo Num found before comma in %s", lpStr);
                return  (FALSE);
            }

            DBG_PRINT1(DL_5, "\r\nParseDecimal found %ld.", *lpNum);

            // Only bump to after the separator if the separator was
            // not the end of the string
            if  (lpStr[*lpIndex] != 0)
                *lpIndex += 1;

            return  (TRUE);
        }

        wDec = (USHORT) GetDecimalValue (lpStr[*lpIndex]);
        if  (wDec == 0x0FFFF)
        {
            DBG_PRINT1(DL_5, "\r\nNon-decimal digit found in Num in %s", lpStr);
            return  (FALSE);
        }

        // We have found at least one decimal digit
        dwIsNum = TRUE;

        // Keep a running computation going here.
        *lpNum = *lpNum * 10 + (ULONG) wDec;

        // go to next char
        *lpIndex += 1;
    }

    return  (TRUE);
}


/*
    ParseDecimalNumberAndFractionAndSeparator

    This routine parses a decimal number from the string passed in. It
    assigns it into lpNum. Upon returning lpIndex is adjusted so that it
    points to the first char after the separator following the number
    parsed. If the number has a decimal point and some fraction following
    it, like 50.03, the fractional portion is parsed but discarded and
    *lpIndex is updated to the first character after the separator
    following 50.03
    If a NULL followed the number parse (i.e. end of the string),
    lpIndex is left pointing to the NULL, but the routine still
    succeeds. If the number was bad or wasn't found before the
    separator or the separator couldd not be found before the end of
    the string , FALSE is returned. If a number
    was found and everything is fine, TRUE is returned.
*/
int CFUNC
ParseDecimalNumberAndFractionAndSeparator (
    LPCHAR  lpStr,
    LPULONG lpIndex,
    LPULONG lpNum,
    char    cSeparator1,
    char    cSeparator2)
{

    ParseDecimalSeparator (lpStr, lpIndex, lpNum, cSeparator1, cSeparator2);

    if  (lpStr[*lpIndex] == '.')
    {
        do
        {
            (*lpIndex)++;
        }
        while   ((lpStr[*lpIndex] != '-') && (lpStr[*lpIndex] != ','));

        (*lpIndex)++;
    }

    return  (TRUE);
}


/*
    GetDecimalValue

    This routine accepts a single char as an argument and returns
    the decimal value of that character.  If the char is not
    0-9 then 0x0FFFF is returned.
*/
ULONG CFUNC
GetDecimalValue (char cDigit)
{
    if  ((cDigit >= '0') && (cDigit <= '9'))
    {
        return  ((ULONG) (cDigit - '0'));
    }

    return  (0x0FFFFFFFF);
}


/*
    GetHexValue

    This routine accepts a single char as an argument and returns
    the hexidecimal value of that character.  If the char is not
    0-9,A-F,or a-f, then 0x0FFFF is returned.
*/
ULONG CFUNC
GetHexValue (char cDigit)
{
    if  ((cDigit >= '0') && (cDigit <= '9'))
    {
        return  ((ULONG) (cDigit - '0'));
    }
    else
    {
        if  ((cDigit >= 'A') && (cDigit <= 'F'))
        {
            return  ((ULONG) (cDigit - 'A' + 10));
        }
        else
        {
            if  ((cDigit >= 'a') && (cDigit <= 'f'))
            {
                return  ((ULONG) (cDigit - 'a' + 10));
            }
            else
            {
                return  (0x0FFFFFFFF);
            }
        }
    }
}


/*
    ParseHexSeparator

    This routine parses up to 8 hex digits from the string passed in.
    It assigns it into lpNum. Upon returning lpIndex is adjusted so
    that it points to the first char after the separator following the
    number parsed. If the number was bad, or if the separator was not
    one of two possible types or if the end of string was not found
    (i.e. NULL) immediately after it, then a FALSE is returned.
    Otherwise TRUE is returned.
*/
int CFUNC
ParseHexSeparator (
        LPCHAR  lpStr,
        LPULONG lpIndex,
        LPULONG lpNum,
        ULONG   dwLen,
        char    cSeparator1,
        char    cSeparator2)
{
    ULONG   dwI;
    ULONG   bFoundDigit;
    ULONG   dwHex;

    // We have to find at least one digit or there is an error.
    bFoundDigit = FALSE;

    // Start off lpNum at 0
    *lpNum = 0;

    // Get the value of the next eight hex digits.
    for (dwI = 0; dwI < dwLen; dwI++)
    {
        if  ((lpStr[*lpIndex] == cSeparator1) ||
             (lpStr[*lpIndex] == cSeparator2))
        {
            if  (!bFoundDigit)
            {
                DBG_PRINT1(DL_5, "\r\nBad Hex value in %s", lpStr);
                return  (FALSE);
            }

            DBG_PRINT1(DL_5, "\r\nParseHex found %lx.", *lpNum);

            // Only bump to after the separator if the separator was
            // not the end of the string
            if  (lpStr[*lpIndex] != 0)
                *lpIndex += 1;

            return  (TRUE);
        }

        // See if it is a hex digit
        dwHex = GetHexValue(lpStr[*lpIndex]);
        if  (dwHex == 0x0FFFFFFFF)
        {
            DBG_PRINT1(DL_5, "\r\nBad Hex value in %s", lpStr);
            return  (FALSE);
        }

        // We have found at least one decimal digit
        bFoundDigit = TRUE;

        *lpNum = (*lpNum << 4) + dwHex;

        // go to next char
        *lpIndex += 1;
    }

    // We fell out of the loop. That means that the maximum number of
    // hex digits has been seen. That is OK, but if the next char is
    // not one of the separators, then there is a parse error.
    if  ((lpStr[*lpIndex] != cSeparator1) &&
         (lpStr[*lpIndex] != cSeparator2))
    {
        DBG_PRINT1(DL_5, "\r\nHex value too big in %s", lpStr);
        return (FALSE);
    }

    // Only bump to after the separator if it was there
    if  ((lpStr[*lpIndex] == cSeparator1) ||
         (lpStr[*lpIndex] == cSeparator2))
        *lpIndex += 1;

    return  (TRUE);
}


/*
    ParseHexWord

    This routine parses up to 4 hex digits from the string passed in.
    If there are more than 4, then it is a parse error.
    It assigns it into lpNum. Upon returning lpIndex is adjusted so
    that it points to the first char after the semicolon following the
    number parsed. If the number was bad, or there was no semicolon
    or end of string (i.e. NULL) immediately after it, then a FALSE
    is returned. Otherwise TRUE is returned.
*/
int CFUNC
ParseHexWord (LPCHAR lpStr, LPULONG lpIndex, LPUSHORT lpNum)
{
    int     nRet;
    ULONG   dwNum;

    nRet = ParseHexSeparator (lpStr, lpIndex, &dwNum, 4, ';', ';');
    *lpNum = (USHORT) dwNum;

    return  (nRet);
}


/*
    ParseHexDWord

    This routine parses up to 8 hex digits from the string passed in.
    If there are more than 8, then it is a parse error.
    It assigns it into lpNum. Upon returning lpIndex is adjusted so
    that it points to the first char after the semicolon following the
    number parsed. If the number was bad, or there was no semicolon
    or end of string (i.e. NULL) immediately after it, then a FALSE
    is returned. Otherwise TRUE is returned.
*/
int CFUNC
ParseHexDWord (LPCHAR lpStr, LPULONG lpIndex, LPULONG lpNum)
{
    return  (ParseHexSeparator (lpStr, lpIndex, lpNum, 8, ';', 0));
}


/*
    GetMaximumRefreshRate

    This routine calculates the maximum refresh rate for the target mode
    specified given the maimum XRes and YRes, and the maximum horizontal
    and vertical frequencies.

*/

ULONG CFUNC
GetMaximumRefreshRate (ULONG dwPixelClkXRes,
                       ULONG dwPixelClkYRes,
                       ULONG dwPixelClkRR,
                       LPMONITORLIMITS lpML,
                       ULONG dwTargetXRes,
                       ULONG dwTargetYRes)
{
    ULONG   i;
    ULONG   dwRefreshRate;
    GTFIN   GTFIn;
    GTFOUT  GTFOut;
    ULONG   dwTotalPixels;
    ULONG   dwPixelClockInHz;
    ULONG   dwPixelClockInMHz;
    ULONG   dwHorizRR;
    ULONG   dwHTotalPixels;
    char    FoundRR;

    // If no pixel clock was passed in, calculate one.
    if (!lpML->dwMaxPixelClock)
    {
        // Find a pixel clock from the dwPixelClkXRes, dwPixelClkXRes, and
        // dwPixelClkRR.
        GTFIn.dwHPixels      = dwPixelClkXRes;
        GTFIn.dwVLines       = dwPixelClkYRes;
        GTFIn.dwMarginsRqd   = FALSE;
        GTFIn.dwIntRqd       = FALSE;
        GTFIn.dwIPParmType   = IPTYPE_VERTICAL_REFRESH_RATE;
        GTFIn.dwIPFreqRqd    = dwPixelClkRR;

        // Get the timing parameters.
        ComputeGTFParameters(&GTFIn, &GTFOut);
        dwPixelClockInHz = Multiply32By32(GTFOut.dwPixelClockIn10KHertz,
                                          10000);
        lpML->dwMaxPixelClock = dwPixelClockInHz;
    }
    else
        dwPixelClockInHz = lpML->dwMaxPixelClock;

    for (i = 0, FoundRR = FALSE; (FoundRR != TRUE) && i < 50; i++)
    {
        dwPixelClockInMHz = Divide32By32(dwPixelClockInHz + 1000000/2, 1000000);
    
        GTFIn.dwHPixels      = dwTargetXRes;
        GTFIn.dwVLines       = dwTargetYRes;
        GTFIn.dwMarginsRqd   = FALSE;
        GTFIn.dwIntRqd       = FALSE;
        GTFIn.dwIPParmType   = IPTYPE_PIXEL_CLOCK_RATE;
        GTFIn.dwIPFreqRqd    = dwPixelClockInMHz;
    
        // Get the timing parameters.
        ComputeGTFParameters(&GTFIn, &GTFOut);
    
        dwTotalPixels = Multiply32By32((GTFOut.dwHTotalChars << 3), GTFOut.dwVTotalScans);
    
        dwPixelClockInHz = Multiply32By32(GTFOut.dwPixelClockIn10KHertz, 10000);
        dwRefreshRate = Divide32By32(dwPixelClockInHz + dwTotalPixels/2, dwTotalPixels);
    
        if (dwRefreshRate < 56)
          dwRefreshRate = 60;

        dwHTotalPixels = GTFOut.dwHTotalChars << 3;
        dwHorizRR = Divide32By32(dwPixelClockInHz + dwHTotalPixels/2, dwHTotalPixels);
        if (dwHorizRR > lpML->dwMaxHorz)
        {
          dwPixelClockInHz = Multiply32By32(lpML->dwMaxHorz, dwHTotalPixels);
        }
        else if (dwRefreshRate > lpML->dwMaxVert)
        {
          dwPixelClockInHz = Multiply32By32(lpML->dwMaxVert, dwHTotalPixels);
          dwPixelClockInHz = Multiply32By32(dwPixelClockInHz, GTFOut.dwVTotalScans);
        }
        else
          FoundRR = TRUE;
    }
            
    if  (dwRefreshRate != 0)
        return  (dwRefreshRate);

    return  (60);
}



