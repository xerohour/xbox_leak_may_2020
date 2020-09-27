/*
    FILE:   modeext.h
    DATE:   4/8/99

    This file is the external API to the world for the
    FindModeExtry and GetModeList functions in the modeset DLL.
    There can be other EXTERNAL function that are OS specific,
    but they should be defined in other header files.
*/

#if defined(LINUX) | defined(MAC)

#define FAR

#if defined(MAC)
typedef unsigned short  USHORT;
#endif

#else
// Need these for C To ASM conversion reason.
#ifndef ULONG
#ifndef FAR
#ifdef _WIN32
#define FAR
#else
#define FAR             __far
#endif
#endif

typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef unsigned char   UCHAR;
typedef UCHAR FAR   *   LPCHAR;
typedef void        *   PVOID;
#endif
#endif  // !linux


// Here is the calling format for both 16bit and 32bit land.
// ULONG __cdecl
// FindModeEntry (
//              LPDISPDATA  lpDispData,
//              LPCHAR      lpEdidBuffer,
//              ULONG       dwEdidSize,
//              LPMODEINFO  lpModeDesc,
//              LPMODEOUT   lpModeOut)
//
// All the structures and types you need are defined in this file.
//
// The FindModeEntry function will always return some legal set of
// values to program in the lpModeOut structure. It may not be the
// requested mode, but the function will never fail to return some
// legitimate values. The actual number returned by the function can
// be a logical OR of the following to indicate whether the parameters
// being returned in lpModeOut are for the requested mode or are for
// a different mode (because the requested mode was not allowed.)
// Note that the return value tells you how different the mode
// returned is from the requested mode -- i.e. whether the mode is
// the same, differes only in the resolution, differes in the pixel
// depth, differs in the refresh rate or some combination thereof.

#define MODE_EXACT_MATCH            0x00
#define MODE_RESOLUTION_ADJUSTED    0x01
#define MODE_PIXEL_DEPTH_ADJUSTED   0x02
#define MODE_REFRESH_RATE_ADJUSTED  0x04

// Although this is kind of Win9X/WinNT specific, it won't hurt to
// put in here. This structure is only used for some calls in the
// Win9X and WinNT specific portion of the modeset dll.
typedef struct  tagREGDATA
{
    ULONG   dwMainKey;
    char    szRegPath[128];
}   REGDATA;
typedef REGDATA FAR *LPREGDATA;

typedef struct  tagDISPDATA
{
    // Every OS probably uses these
    ULONG       dwDeviceID;
    ULONG       dwCRTCIndex;
    ULONG       dwMaxTVOutResolution;
    ULONG       dwVideoMemoryInBytes;
    ULONG       dwMaxDacSpeedInHertz8bpp;
    ULONG       dwMaxDacSpeedInHertz16bpp;
    ULONG       dwMaxDacSpeedInHertz32bpp;
    ULONG       dwMaxDfpScaledXResInPixels;
    ULONG       dwTVEncoderType;
    PVOID       lpfnGetModeSize;
    PVOID       dwContext1;
    PVOID       dwContext2;

    /////////////////////////////////////////////////////////////
    // Win9X uses these
    // Registry base path to ...DISPLAY\000X and MONITOR\000X key.
    REGDATA     *lpBoardRegistryData;
    REGDATA     *lpMonitorRegistryData;
    ULONG       dwMonitorInfType;
    ULONG       lpDevData;
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // MAC uses these
    ULONG       edidSize;
    // MAC Done
    /////////////////////////////////////////////////////////////
}   DISPDATA;

typedef DISPDATA FAR *LPDISPDATA;

// dwInfType can have one of the following three values
#define SPECIFIC_DEVICE         0
#define UNKNOWN_DEVICE          1
#define PLUG_AND_PLAY_DEVICE    2

typedef struct  tagDEVDATA
{
    char    cType;
    char    cNumber;
    char    cFormat;
    char    cReserved;
}   DEVDATA;

typedef DEVDATA   FAR *LPDEVDATA;


// Unfortunately we need to pass the TVEncoderType in here. Optimal
// mode timing paramteres for the different TVEncoders are different.
// I hate having aliased constant, but I dont wat to include the
// RM files right now. I'm thinking about how to do this whole thing
// in a bit better a fashion.
#define TV_ENCODER_NONE                 0x000
#define TV_ENCODER_BROOKTREE            0x100
#define TV_ENCODER_CHRONTEL             0x200
#define TV_ENCODER_PHILIPS              0x400
#define NV_ENCODER_NONE                 TV_ENCODER_NONE
#define NV_ENCODER_BROOKTREE_868        (TV_ENCODER_BROOKTREE + 1)
#define NV_ENCODER_BROOKTREE_869        (TV_ENCODER_BROOKTREE + 2)
#define NV_ENCODER_BROOKTREE_871        (TV_ENCODER_BROOKTREE + 3)
#define NV_ENCODER_CHRONTEL_7003        (TV_ENCODER_CHRONTEL + 3)
#define NV_ENCODER_CHRONTEL_7004        (TV_ENCODER_CHRONTEL + 4)
#define NV_ENCODER_CHRONTEL_7005        (TV_ENCODER_CHRONTEL + 5)
#define NV_ENCODER_CHRONTEL_7006        (TV_ENCODER_CHRONTEL + 6)
#define NV_ENCODER_CHRONTEL_7007        (TV_ENCODER_CHRONTEL + 7)
#define NV_ENCODER_CHRONTEL_7008        (TV_ENCODER_CHRONTEL + 8)
#define NV_ENCODER_PHILIPS_7102	        (TV_ENCODER_PHILIPS + 2)
#define NV_ENCODER_PHILIPS_7103	        (TV_ENCODER_PHILIPS + 3)
// = 7102 encoder + 7114 decoder
#define NV_ENCODER_PHILIPS_7108	        (TV_ENCODER_PHILIPS + 8)
// = 7103 encoder + 7114 decoder
#define NV_ENCODER_PHILIPS_7109	        (TV_ENCODER_PHILIPS + 9)
// early version of 7108 with clock active edge reversed
#define NV_ENCODER_PHILIPS_7108B        (TV_ENCODER_PHILIPS + 8 + 0x10)


// This is a description of the incoming mode to try and validate/set.
// The dwXRes, dwYRes, and dwBpp fields should be filled in with the
// desired values. dwRefreshRate must be an actual refresh rate in hertz.
// dwDevType should be set to one of the special device type constants
// (CRT, NTSC, PAL, FLAT_PANEL, etc.) given later in this file.
// The dwOriginalRefreshRate is not used as an input parameter, but
// some of the functions in the modeset DLL that return a MODEINFO
// structure return information in this field. Also, dwCRTCIndex is
// not used by the modeset DLL, but by other modules that utilize
// this structure.
typedef struct  tagMODEINFO
{
    ULONG   dwXRes;
    ULONG   dwYRes;
    ULONG   dwBpp;
    ULONG   dwRefreshRate;
    ULONG   dwDevType;
    ULONG   dwTVFormat;
    DEVDATA MIDevData;
    ULONG   dwOriginalRefreshRate;
}   MODEINFO;

typedef MODEINFO FAR *LPMODEINFO;

// Display Device Types which can be in MODEINFO.dwDevType
#define DEVTYPE_CRT         0x00
#define DEVTYPE_TV          0x01
#define DEVTYPE_DFP         0x02
#define MAX_DEVTYPES        0x03

// These are for the TvType -- they weren't being used in the generic
// code at the time they were added, but they may be needed in the
// future, or the OS-specific sections may need them. We might as well
// all agree on their values.
#define TVTYPE_NTSCM                0x00
#define TVTYPE_NTSCJ                0x01
#define TVTYPE_PALM                 0x02
#define TVTYPE_PALA                 0x03
#define TVTYPE_PALN                 0x04
#define TVTYPE_PALNC                0x05
#define MAX_TVTYPES                 0x06

// This is the format that the mode timing parameters are handed back
// to the caller. This format was chosen because, as of the time of
// this writing, it matches the format of an NVidia object -- class 46.
typedef struct  _tagMODEOUT
{
    USHORT  wXRes;
    USHORT  wYRes;
    USHORT  wHBlankStart;
    USHORT  wHBlankWidth;
    USHORT  wHSyncStart;
    USHORT  wHSyncWidth;
    USHORT  wVBlankStart;
    USHORT  wVBlankWidth;
    USHORT  wVSyncStart;
    USHORT  wVSyncWidth;
    USHORT  wHTotal;
    USHORT  wVTotal;
    ULONG   dwPixelClock;
    ULONG   dwFormat;
    USHORT  wRefreshRate;
    ULONG   dwPitchInBytes;
} MODEOUT;

typedef MODEOUT FAR *LPMODEOUT;


// There is an API call in this DLL --
// int GetModeList (LPDISPDATA lpDD, LPCHAR lpModes, ULONG dwType)
// lpDD specifies the board to fetch the modelist for.
// If lpModes is NULL, the routine returns the number of modes.
// If lpModes is not NULL, the routine returns the number of modes
// and fills in lpModes with an array of either MODEENTRY, MODEENTRYNORR,
// or MODEENTRYANDPITCH structures depending upon the value of dwType.

// The routine will never return more than this number of modes
#define MAX_MODEENTRYS  2048

// dwType can be one of these values.
#define MODETYPE_MODEENTRYNORR      0
#define MODETYPE_MODEENTRY          1
#define MODETYPE_MODEENTRYANDPITCH  2

// dwType of MODETYPE_MODEENTRYNORR causes these structurs to be returned
typedef struct  tagMODEENTRYNORR
{
    ULONG   dwXRes;
    ULONG   dwYRes;
    ULONG   dwBpp;
} MODEENTRYNORR;

typedef MODEENTRYNORR FAR *LPMODEENTRYNORR;

// dwType of MODETYPE_MODEENTRY causes these structurs to be returned
typedef struct  tagMODEENTRY
{
    ULONG   dwXRes;
    ULONG   dwYRes;
    ULONG   dwBpp;
    ULONG   dwRefreshRate;
} MODEENTRY;

typedef MODEENTRY FAR *LPMODEENTRY;

// dwType of MODETYPE_MODEENTRYANDPITCH causes these structurs to be returned
typedef struct  tagMODEENTRYANDPITCH
{
    ULONG   dwXRes;
    ULONG   dwYRes;
    ULONG   dwBpp;
    ULONG   dwPitch;
} MODEENTRYANDPITCH;

typedef MODEENTRYANDPITCH FAR *LPMODEENTRYANDPITCH;


