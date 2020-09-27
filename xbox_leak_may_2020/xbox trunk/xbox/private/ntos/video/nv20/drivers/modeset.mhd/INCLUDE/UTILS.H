/*
    FILE:   utils.h

    DATE:   4/8/99

    This file holds some utility routines.  OS independent
*/

extern int CFUNC FindMode (LPMODESTRUC lpMS, ULONG dwXRes,
                            ULONG dwYRes, ULONG dwRRate);
extern int CFUNC FindModeWithoutRR (LPMODESTRUC lpMS, ULONG dwXRes,
                            ULONG dwYRes);
extern int CFUNC SetupParamsForVidLutClass (LPMODESTRUC lpMS,
                            LPMODEOUT lpModeOut,
                            LPMODEINFO lpModeIn);
extern int CFUNC SetupParamsForModeInfo (LPMODEOUT lpModeOut,
                            LPMODEINFO lpModeOrig, LPMODEINFO lpModeIn);
extern int CFUNC ParseDecimalNumber (LPCHAR szRTString, LPULONG lpIndex,
                            LPULONG lpNum);
extern ULONG CFUNC GetDecimalValue (char cDigit);
extern int CFUNC ParseHexWord (LPCHAR szRTString, LPULONG lpIndex,
                            LPUSHORT lpNum);
extern int CFUNC ParseHexDWord (LPCHAR szRTString, LPULONG lpIndex,
                            LPULONG lpNum);
extern int CFUNC ParseDecimalSeparator (LPCHAR lpStr, LPULONG lpIndex,
                            LPULONG lpNum, char cSeparator1,
                            char cSeparator2);
extern int CFUNC ParseDecimalNumberAndFractionAndSeparator (LPCHAR  lpStr,
                            LPULONG lpIndex, LPULONG lpNum,
                            char    cSeparator1, char    cSeparator2);
extern int CFUNC ParseHexSeparator (LPCHAR  lpStr, LPULONG lpIndex,
                            LPULONG lpNum, ULONG   dwLen,
                            char cSeparator1, char cSeparator2);
extern ULONG CFUNC GetHexValue (char cDigit);

extern ULONG CFUNC GetMaximumRefreshRate (ULONG dwPixelClkXRes,
                            ULONG dwPixelClkYRes, ULONG dwPixelClkRR,
                            LPMONITORLIMITS lpML, ULONG dwTargetXRes,
                            ULONG dwTargetYRes);

