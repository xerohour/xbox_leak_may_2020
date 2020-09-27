/*
    FILE:   digmode.h
    DATE:   4/8/99

    This file contains the code for mode setting/determination
    ona CRT or FLAT PANEL. OS independent.
*/

extern int CFUNC FindDigModeEntry (LPCHAR lpEdidBuffer, ULONG dwEdidSize,
                        LPDISPDATA lpDispData, LPMODEINFO lpModeDesc,
                        LPMODEOUT lpModeOut, LPMODEENTRY lpModeList,
                        LPRESTIME lpRTList);
extern int CFUNC CheckTimingStandardOverride (LPCHAR lpEdidBuffer,
                        ULONG dwEdidSize, LPDISPDATA lpDispData,
                        LPMODEINFO lpModeDesc, LPMODEOUT lpModeOut);
extern int CFUNC BackOffDigModeEntry (LPMODEENTRY lpModeList,
                        LPMODEINFO lpModeDesc, LPMODEINFO lpModeOrig, int iRRFlag);
extern int CFUNC ProcessRTWithDigModeEntry (LPDISPDATA lpDispData,
                        LPCHAR lpEdidBuffer,
                        ULONG dwEdidSize, LPRESTIME lpRTList,
                        LPMODEINFO lpModeDesc, LPMODEOUT lpModeOut);
extern int CFUNC ModeWithinMonitorTimingLimits (LPDISPDATA lpDispData,
                        LPMODEOUT lpModeOut,
                        LPCHAR lpEdidBuffer, ULONG dwEdidSize);

