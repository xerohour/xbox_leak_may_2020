/*
    FILE:   dmt.h
    DATE:   4/8/99

    This file is the dmt find or set mode code.
*/

extern int CFUNC DmtFindMode (LPMODEINFO lpModeIn, LPMODEOUT lpModeOut);

extern int CFUNC DmtGetDmtTablePointer (LPMODESTRUC FAR *lplpDmtTable);

extern int CFUNC DmtGetMaximumRefreshRate (ULONG dwXRes, ULONG dwYRes,
                                           ULONG dwPixelClk,
                                           ULONG *dwRefreshRate);

