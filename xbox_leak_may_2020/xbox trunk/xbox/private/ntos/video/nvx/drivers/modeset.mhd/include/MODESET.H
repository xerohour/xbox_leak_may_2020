/*
    FILE:   modeset.h
    DATE:   4/8/99

    This file holds the format for some tables and structures used
    internally by the DLL. OS independent.
*/

// This is the structure format in which we the mode timing tables
// in this DLL are stored. This should never need to be changed.
typedef struct  _tagMODESTRUC
{
    USHORT  wHorizVisible;
    USHORT  wVertVisible;
    USHORT  wRefresh;
    USHORT  wHorizTotal;
    USHORT  wHorizBlankStart;
    USHORT  wHorizSyncStart;
    USHORT  wHorizSyncEnd;
    USHORT  wHorizBlankEnd;
    USHORT  wVertTotal;
    USHORT  wVertBlankStart;
    USHORT  wVertSyncStart;
    USHORT  wVertSyncEnd;
    USHORT  wVertBlankEnd;
    USHORT  wDotClock;
    USHORT  wHSyncPolarity;
    USHORT  wVSyncPolarity;
} MODESTRUC;

typedef MODESTRUC FAR *LPMODESTRUC;

// These constants are used in internal tables
#define BUFFER_HSYNC_NEGATIVE           0
#define BUFFER_HSYNC_POSITIVE           1
#define BUFFER_VSYNC_NEGATIVE           0
#define BUFFER_VSYNC_POSITIVE           1


// This information can often be gotten from an EDID. It can be
// used to restrict mode settings.
typedef struct  _tagMONITORLIMITS
{
    ULONG   dwMinHorz;
    ULONG   dwMaxHorz;
    ULONG   dwMinVert;
    ULONG   dwMaxVert;
    ULONG   dwMaxPixelClock;
} MONITORLIMITS;

typedef MONITORLIMITS FAR *LPMONITORLIMITS;


// Here are the external functions
extern int CFUNC  FindNextSmallerResolution (LPMODEENTRY lpModeList,
                                            LPMODEINFO lpModeDesc);

extern int CFUNC  FindNextSmallerRefreshRate (LPMODEENTRY lpModeList,
                                            LPMODEINFO  lpModeDesc);
extern int CFUNC  FindNextLargerRefreshRate (LPMODEENTRY lpModeList,
                                            LPMODEINFO  lpModeDesc);
extern int WINAPI GetSafeTiming (LPMODEINFO lpModeDesc, LPMODEOUT lpModeOut);
extern int CFUNC  GetModeFlags (LPMODEINFO lpModeOrig, LPMODEINFO lpModeFound);
extern int WINAPI FormatModeList (LPDISPDATA lpDispData, LPCHAR lpModes, ULONG dwType, LPMODEENTRY lpME);

