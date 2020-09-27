/*
    FILE:   tvmode.h
    DATE:   4/8/99

    This file contains the high level logic for the tv modeset code.
    OS Independent.
*/

extern int CFUNC FindTVModeEntry (LPDISPDATA lpDispData,
                                LPMODEINFO lpModeDesc, LPMODEOUT lpModeOut,
                                LPMODEENTRY lpModeList, LPRESTIME lpRTList);

