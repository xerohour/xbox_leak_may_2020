/*
    FILE:   restime.h
    DATE:   4/8/99

    This file has routines to support getting and parsing the restriction
    and timing strings. OS independent stuff.
*/

// This is the maximum number of R&T strings allowed.
#define MAX_RESTIMES    128

// For any of the fields in the R&T string that can be separated by a
// '.' (see specification for details), there can be at most the
// following number of DOT separated values per field. For example,
// the MonitorID field can have up to MAX_DOT_FIELDS monitorIDs.
// Example DT71.SN17.VS85 is three monitor IDS.
#define MAX_DOT_FIELDS  16

// For those R&T strings which allow a wildcard indicating ALL, this
// is the wildcard character.
#define ALL_WILDCARD    ('*')

// For each restriction and timing string that gets parsed, the string
// is parsed into the following structure.
typedef struct  tagRESTIME
{
    ULONG   dwXRes;
    ULONG   dwYRes;
    ULONG   dwPixelDepth;
    ULONG   dwRefreshRate;
    USHORT  wDeviceID[MAX_DOT_FIELDS];
    ULONG   dwMonitorID[MAX_DOT_FIELDS];
    ULONG   dwTimingStandard[MAX_DOT_FIELDS];
    MODEOUT sModeOut;
} RESTIME;

typedef RESTIME FAR *LPRESTIME;

// A couple of special MonitorIDs. UKWN_ID is a monitor ID which matches
// all monitors that do not have an EDID.
// A couple of special MonitorIDs. UKWN_ID is a monitor ID which matches
// all monitors that do not have an EDID.
#define UKWN_ID ((ULONG) 'U') + (((ULONG) 'K') << 8) + (((ULONG) 'W') << 16) + (((ULONG) 'N') << 24)
#define NTSC_ID ((ULONG) 'N') + (((ULONG) 'T') << 8) + (((ULONG) 'S') << 16) + (((ULONG) 'C') << 24)
#define PALX_ID ((ULONG) 'P') + (((ULONG) 'A') << 8) + (((ULONG) 'L') << 16) + (((ULONG) 'X') << 24)
#define PANL_ID ((ULONG) 'P') + (((ULONG) 'A') << 8) + (((ULONG) 'N') << 16) + (((ULONG) 'L') << 24)

// Several TimingStandardIDs.
#define NONE_ID ((ULONG) 'N') + (((ULONG) 'O') << 8) + (((ULONG) 'N') << 16) + (((ULONG) 'E') << 24)
#define EDID_ID ((ULONG) 'E') + (((ULONG) 'D') << 8) + (((ULONG) 'I') << 16) + (((ULONG) 'D') << 24)
#define DMTV_ID ((ULONG) 'D') + (((ULONG) 'M') << 8) + (((ULONG) 'T') << 16) + (((ULONG) 'V') << 24)
#define DMTR_ID ((ULONG) 'D') + (((ULONG) 'M') << 8) + (((ULONG) 'T') << 16) + (((ULONG) 'R') << 24)
#define GTFV_ID ((ULONG) 'G') + (((ULONG) 'T') << 8) + (((ULONG) 'F') << 16) + (((ULONG) 'V') << 24)
#define GTFR_ID ((ULONG) 'G') + (((ULONG) 'T') << 8) + (((ULONG) 'F') << 16) + (((ULONG) 'R') << 24)
#define OEMX_ID ((ULONG) 'O') + (((ULONG) 'E') << 8) + (((ULONG) 'M') << 16) + (((ULONG) 'X') << 24)
#define OEMR_ID ((ULONG) 'O') + (((ULONG) 'E') << 8) + (((ULONG) 'M') << 16) + (((ULONG) 'R') << 24)
#define FGTF_ID ((ULONG) 'F') + (((ULONG) 'G') << 8) + (((ULONG) 'T') << 16) + (((ULONG) 'F') << 24)
#define FGTR_ID ((ULONG) 'F') + (((ULONG) 'G') << 8) + (((ULONG) 'T') << 16) + (((ULONG) 'R') << 24)

// A few of the R&T parsing functions require that a mode be passed in.
// This is the structure that the R&T functions require the mode to be in.
typedef struct  tagRTMODE
{
    ULONG   dwXRes;
    ULONG   dwYRes;
    ULONG   dwBpp;
    ULONG   dwRefreshRate;
} RTMODE;

typedef RTMODE FAR *LPRTMODE;

extern int CFUNC RTFindNextMatchingString (LPRESTIME lpRTList,
                        int nIndex, LPDISPDATA lpDispData,
                        LPRTMODE lpRTMode, ULONG dwMonitorID);;

// Return values for a successful RTFindNextMatchingString are any
// positive integer which represents the index of the RTstring in the
// RTList that was a match. The value below is returned if there was no
// match.
#define RTERR_NOT_FOUND -1

extern int CFUNC RTRestrictByTimingStandard (LPRESTIME lpRTList, int nIndex,
                                    ULONG dwTimingStandard);

int CFUNC ParseRTString (LPRESTIME lpRT, LPCHAR szRTString);
int CFUNC ParseDecimalNumberOrWildcard (LPCHAR szRTString, LPULONG lpIndex,
                                LPULONG lpNum);
int CFUNC ParseDeviceIDs (LPCHAR szRTString, LPULONG lpIndex,
                                LPUSHORT lpNum);
int CFUNC ParseMonitorIDs (LPCHAR szRTString, LPULONG lpIndex,
                                LPULONG lpNum);
int CFUNC ParseTimingStandards (LPCHAR szRTString,
                                LPULONG lpIndex, LPULONG lpNum);
int CFUNC ParseDecimalNumberComma (LPCHAR szRTString, LPULONG lpIndex,
                                LPULONG lpNum);

