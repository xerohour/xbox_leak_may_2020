/*
    FILE:   osapi.h
    DATE:   4/8/99

    This file holds the externs for the os-specific routines that
    support the mode set code.
*/

extern LPMODEENTRY CFUNC GetMasterModeList (LPDISPDATA lpDispData);
extern int CFUNC FreeMasterModeList (LPMODEENTRY lpModeList);

extern LPRESTIME CFUNC GetMasterRTList (LPDISPDATA lpDispData);
extern int CFUNC FreeMasterRTList (LPRESTIME lpRTList);

extern ULONG CFUNC GetTimingStandardOverride (LPDISPDATA lpDispData);
extern ULONG CFUNC GetPitchAndSize (LPDISPDATA lpDispData, ULONG dwXRes,
            ULONG dwYRes, ULONG dwBpp, ULONG *dwPitch, ULONG *dwModeSize);

extern int CFUNC GetMonitorLimitMode (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo);

int CFUNC GetMonitorInfRangeLimits (LPDISPDATA lpDispData,
                     LPMONITORLIMITS lpML, ULONG *lpdwXRes, ULONG *lpdwYRes);

ULONG CFUNC GetAllowAllModesFlag(LPDISPDATA lpDispData);


