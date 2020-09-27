//*****************************Module*Header******************************
//
// Module Name: elsaesc.h
//
// This file conatins all declarations for the ELSA Escape interface
// used by all tools and applications to access graphic driver data.
//
// FNicklisch 09/14/2000: 
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1998-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//
#ifndef __ELSAESC
#define __ELSAESC

/*
** $Header$
**
** MODULE:   ELSAESC.H
**
** AUTHOR:   Fred Nicklisch
**           Copyright (c) 1991-99 ELSA AG, Aachen (Germany)
**
** PROJECT:  ELSA Graphics Software
**
** PURPOSE:  define Escape interface between Windows 95/NT display driver
**           and tools
**
** DESCR:    All ELSA drivers will provide a escape interface to allow
**           user-applications (tools) to configure the display driver and
**           retrieve information. The application has to use ExtEscape.
**           The implementation depends on one single esc entry and
**           multiple subescapes.
**
**           WARNING: We only allow the basic types
**                    LONG, DWORD and char in all in and out structures!
**                    Don't use Tabs (blanks instead)
**                    Don't use german "Umlaute"
**
** NOTES:    01.12.97 FNicklis: First Release, basic functionality
**           02.12.97 MSuhre  : Added support for Monitor-Key,
**                              changed CHAR to char
**           03.12.97 SKuklik : Added support for DUO Boards
**           03.12.97 SKuklik : Added support for DUO Boards
**           04.12.97 FNicklis: struct packing and basic types
**           04.12.97 MSuhre  : #pragma pack (push/pop,..)
**                              push and pop are not valid for MS-C 1.52
**           06.03.98 tu      : Query and set value for string and dword added
**           06.03.98 FNicklis: Fixed "query and set value" to make it compilable
**           08.03.98 FNicklis: defined ET_SET_VALUE_RET
**           12.03.98 tu      : Added query and set value ID's for WinNT:
**                                HardwareInformation 0x20000500-0x200005ff
**                                Desktop coordinates 0x20000600-0x200006ff
**                                Some Basic/system values 0x20000002-0x20000004
**                              Added dwFlags to ET_VALUE_STRING and ET_VALUE_DWORD;
**                              Added return flags convention
**           12.03.98 THorn   : New grouping of the items in this file,
**                              no compiler relevant change.
**           13.03.98 tu      : Added value ID ET_VALUE_DW_ICDINTERFACE_SIZEOFBUFFER
**           13.04.98 db      : replaced dwAlign member of ET_QUERY with dwBoard
**                              Somehow we have to know for what board we want to read or write
**           13.04.98 db      : number of dma buffers is not number of subbuffers (used the same ID)
**           12.05.98 THorn   : SUBESC_ET_WINMAN_DDC_EDID_GET added.
**           09.06.98 tu      : Transfer ESCAPE calls from EDDESC.H (SUBESC_ET_EDD_xxx)
**           08.07.98 tu      : Added value ID ET_VALUE_DW_OPENGL_DISABLETEXTUREPATCHING (0x20000320)
**           14.08.98 FNicklis: Added value IDs ET_VALUE_DW_xx for NT driver
**           16.09.98 THorn   : SUBESC_ET_WINMAN2_... added.
**           25.09.98 tu      : added ET_VALUE_ERROR (0x80000000)
**                              added #pragma warning( disable:4201 ) at struct tagET_VERSION
**           30.09.98 FNicklis: Added version stamp and capabilities to ET_VERSION retrieved with
**                              SUBESC_ET_QUERY_VERSION_INFO
**           21.10.98 FNicklis: Extension to ET_VALUE_xx-interface (ET_VALUE_BLOCKED)
**           23.10.98 FNicklis: NT doesn't know FAR and somtimes doesn't know DWORD
**           25.10.98 FNicklis: Modified ET_VALUE_xx-interface to get reserved bits for OS internals
**           12.11.98 js:       ET_VALUE_DW_OPENGL_SUPPORTPRODESIGNER for OpenGL.SupportProDesigner added
**           21.11.98 FNicklis: ET_VALUE_DW_OPENGL_DMAMAXCOUNT, ET_VALUE_DW_OPENGL_DMAMINPUSHCOUNT added
**           25.11.98 MPietsch: ET_VALUE_DW_OPENGL_PIXELTUBE added
**                    FNicklis: Added some informational stuff regarding version stamp and version checking
**           02.12.98 FNicklis: New ET_VALUE_TYPE_DWORD_DEFAULT, ...
**           29.01.99 FNicklis: Registry Key-IDs 0x2000032A-0x2000032E
**           29.01.99 SKuklik:  Registry Key-IDs 0x20000069-0x2000006c, 0x20000097, 0x20000129, 0x20000208, 0x20000405 - 406, 0x20000601 - 602
**           03.02.99 CSkopins: Escape-Codes fuer Video-In/-Out hinzugefuegt.
**           11.02.99 SKuklik : Registry Key-ID  0x2000012B (ICDInterface.PFD_SWAP_EXCHANGE)
**           12.02.99 FNicklis: SUBESC_ET_QUERY_ICDCLIENTINFO
**           13.04.99 CSchalle: ET_VALUE_SZ_HARDWAREINFORMATION_BIOSSTRING
**           14.04.99 CSchalle: added ET_VALUE_DW_OPENGL_ALIGNED_SURFACES
**           22.04.99 CSchalle: added ET_VALUE_DW_OPENGL_FRAMEBUFFER_DMA
**           11.05.99 JS:       added ET_SUBESC_ET_xxx_VALUE_BINARY, ET_VALUE_BIN_GDI_GAMMARAMP
**                              added SUBESC_ET_EDD_xETGAMMARAMP
**           19.05.99 FNicklis: ET_VALUE_DW_OPENGL_AUTODESKEXTENSIONS fixed
**           28.06.99 HHornig : added RES_ESC_ELSA_TOOL call for generic drivers
**           30.06.99 FNicklis: added SUBESC_ET_MULTIBOARDSETUP for internal Display driver vs multiboard driver calls
**           25.08.99 CSkopins: added Capability flags for STEREO and OVERCLOCKING
**           22.09.99 HHornig : added ET_EDD_GETCOLORCAPSLIMITS
**           28.09.99 TU:       added SUBESC_ET_QUERY_TEMPERATURE for temperature and fan status
**           29.09.99 FNicklis: added SUBESC_ET_DPMS and sample code
**           18.10.99 CSkopins: added SUBESC_RMAPI_CONFIGSETGET for ERAZOR X² TV-Out
**           20.10.99 CSchalle: added ET_VALUE_BIN_OEM_MODELIST
**           10.02.00 FNicklis: added ET_VALUE_DW_DMA_7DUMMYREADS, ET_VALUE_DW_DMA_DUMMYREADUC, ET_VALUE_DW_DMA_FORCEIOFLUSH
**           10.03.00 HHornig : added SUBESC_GET_OUTPUT_DEVICE_INFO / SUBESC_SET_OUTPUT_DEVICE
**           23.05.00 BSchwall: added ET_VALUE_DW_OPENGL_UNIGRAPHICS_TEXTURE_FIX
*/

// ********************************************************
// ********************************************************
// Basic defines
// -------------

// We need a common alignment for all structures:
// save current packing and switch to 8 byte (64bit)
#ifdef WIN32
#pragma pack (push, fixed_forELSAESC)
#endif
#pragma pack(8)

#define ET_MAX_PATH   260 // MAX_PATH is not available in all environments!
#define ET_MAX_STRING 260


#define ESC_ELSA_TOOLS 0xE15A // Escape for ELSA Tools
// HHORNIG : 28.6.99
// if we build generic drivers some tools are allowed to pop up
// to detect this the tool should use this call combined with
// a special code that is only used for this tool
#define RES_ESC_ELSA_TOOLS 0xE15B // restricted Escape for ELSA Tools

// FNicklis 23.10.98: Need it sometimes ;-)
#ifndef DWORD
typedef unsigned long DWORD;
#endif

// FNicklis 23.10.98: Under NT we don't have FAR and NEAR
#ifndef FAR
#define FAR
#endif

#ifdef NTMINIPORT
// FNicklis 29.01.1999: don't know HWND in NT miniport
#ifndef HWND
#define HWND PVOID
#endif
#endif// NTMINIPORT

// Subescapes: 0x00000000 - 0x0fffffffbasic sub escapes
//             0x10000000 - 0x1fffffff to be defined by Win95
//             0x20000000 - 0x2fffffff to be defined by NT (NTeam)
//             0x30000000 - 0xffffffff unused
// Naming: SUBESC_ET_xxxx, read: Sub escape for ELSA tools
//
// Note: Queries should use the same in structure ET_QUERY but different
//       query subescapes SUBESC_ET_QUERY_yyyy

// ********************************************************
// Sub escapes, common for Windows 95 and Windows NT:
#define SUBESC_ET_QUERY_VERSION_INFO        0x00000000

#define SUBESC_ET_QUERY_VALUE_STRING        0x00000010
#define SUBESC_ET_QUERY_VALUE_DWORD         0x00000011
#define SUBESC_ET_QUERY_VALUE_BINARY        0x00000012

#define SUBESC_ET_SET_VALUE_STRING          0x00000018
#define SUBESC_ET_SET_VALUE_DWORD           0x00000019
#define SUBESC_ET_SET_VALUE_BINARY          0x00000020

// 0x30-0x4F reserved for WINman
#define SUBESC_ET_WINMAN_DATA_GET           0x00000030
#define SUBESC_ET_WINMAN_DATA_SET           0x00000031
#define SUBESC_ET_WINMAN_DDC_EDID_GET       0x00000032
#define SUBESC_ET_WINMAN2_DATA_GET          0x00000035
#define SUBESC_ET_WINMAN2_DATA_SET          0x00000036
#define SUBESC_ET_WINMAN2_DDC_EDID_GET      0x00000037

// 0x50-0x6F reserved for elsa directdraw caps
#define SUBESC_ET_EDD_GETCAPS               0x00000050
#define SUBESC_ET_EDD_GETFEATURES           0x00000051
#define SUBESC_ET_EDD_SETFEATURES           0x00000052
#define SUBESC_ET_EDD_GETVIDMEMINFO         0x00000053
#define SUBESC_ET_EDD_GETCOLORADJUSTMENT    0x00000054
#define SUBESC_ET_EDD_SETCOLORADJUSTMENT    0x00000055
#define SUBESC_ET_EDD_SETGAMMARAMP          0x00000056
#define SUBESC_ET_EDD_GETGAMMARAMP          0x00000057
#define SUBESC_ET_EDD_GETCOLORCAPSLIMITS    0x00000058

// 0x100-0x11F reserved for Video-In / -Out Escapes
//
#define SUBESC_ET_VIDEO_DMACOPYSM           0x00000100
#define SUBESC_ET_VIDEO_OUTCONTROL          0x00000110
#define SUBESC_ET_RMAPI_CONFIGSETGET        0x0000011F

// 0x200-0x21F reserved for all tools to decide if they are
//             allowed to pop up, only to be used in combination
//             with a resctricted call RES_ESC_ELSA_TOOL
//
#define SUBESC_STEREO_TOOL                  0x00000200

// 0x300-0x3FF reserved for hardware status and control
//
#define SUBESC_ET_QUERY_TEMPERATURE         0x00000300

#define SUBESC_ET_GET_OUTPUT_DEVICE_INFO    0x00000310
#define SUBESC_ET_SET_OUTPUT_DEVICE         0x00000311


// ********************************************************
// Windows 95 sub escapes:

#define SUBESC_ET_QUERY_MONITORKEY_REGPATH  0x10000000
#define SUBESC_ET_QUERY_D3DSETTINGS_REGPATH 0x10000001

// ********************************************************
// Windows NT sub escapes:

#define SUBESC_ET_QUERY_OPENGLICD_REGPATH   0x20000000
#define SUBESC_ET_DUO_ESC_GETPANELINFO      0x20000002  //Returns Information on our Panel/PanelCaps
#define SUBESC_ET_DUO_ESC_SETPANELINFO      0x20000004  //Sets Panel Information
#define SUBESC_ET_POWERLIB_START            0x20000008  //Tells driver that one powerlib has started
#define SUBESC_ET_POWERLIB_EXIT             0x20000009  //Tells driver that one powerlib will end

// 0x10-0x2F reserved for driver internal escapes
#define SUBESC_ET_GETREGISTER               0x20000010  // direct hw access
#define SUBESC_ET_SETREGISTER               0x20000011
#define SUBESC_ET_MODIFYREGISTER            0x20000012
#define SUBESC_ET_SCROLL                    0x20000013
#define SUBESC_ET_GFXOPTIONQUERY            0x20000014

#define SUBESC_ET_NOTIFYDRIVER              0x20000015  // special support for video capture driver
#define SUBESC_ET_SAA                       0x20000016
#define SUBESC_ET_DDSURFOVERRIDE            0x20000017

#define SUBESC_ET_MULTIBOARDSETUP           0x20000018 // FNicklis: setup singleboard client with information from multiboard wrapper.
                                                       // IO structures ET_MULTIBOARDSETUP_IN and ET_MULTIBOARDSETUP_OUT are defined in driver code!
#define SUBESC_ET_DPMS                      0x20000019 // FNicklis: set DPMS power save mode; uses ET_DPMS_IN and ET_DPMS_OUT

// 0x30- ... Common driver Escapes
#define SUBESC_ET_SETWINDOW_STEREOMODE      0x20000030 // set the stereo mode of an OpenGL window
#define SUBESC_ET_QUERY_ICDCLIENTINFO       0x20000031 // receive information about OpenGL-Clients


// ********************************************************
// ********************************************************
// In structure (Tool -> driver):
// Input Query structure, common part of all subescape input structures
// --------------------------------------------------------------------
typedef struct tagET_QUERY
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_QUERY)
  DWORD dwSubEsc;  // has to be (SUBESC_ET_QUERY_VERSION_INFO, SUBESC_ET_QUERY_OPENGLICD_REGPATH, ...)
  DWORD dwOutSize; // size of output data structure (if there is such data)
  DWORD dwBoard;   // for registry issues we need to know for what board we have to read/write @@db 130498
                   // Calls that affect all boards use ET_ALL_BOARDS to initialize dwBoard
                   // 0 -> board 0, 1 -> board 1, ET_ALL_BOARDS -> all boards
} ET_QUERY;

#define ET_ALL_BOARDS 0xFFFFFFFF   // FNicklis, 11.03.99 reserved key for all boards

// driver get's PVOIDs but needs a fast access to members
#define ET_QUERY_GET_SUBESC(pv)    (((ET_QUERY*)pv)->dwSubEsc)
#define ET_QUERY_GET_BOARD(pv)     (((ET_QUERY*)pv)->dwBoard)

// ********************************************************
// ********************************************************
// Version info and some other data from driver
// --------------------------------------------

#define MAX_ET_VERSION 64

#define ET_VER_RELEASE_DRIVER    0x0
#define ET_VER_WHQL_DRIVER       0x1 // Release driver for WHQL
#define ET_VER_BETA_DRIVER       0x2 // Beta driver neither release nor WHQL

// Out structure (Driver -> Tool):
// Version information used in ET_VERSION_INFO, retrieved by SUBESC_ET_QUERY_VERSION_INFO

#ifdef WIN32
#pragma warning( disable:4201 ) // nonstandard extension used : nameless struct/union
#endif

// Usage of ET_VERSION_STAMP:
//
// Allowes to check which bits of the capabilities fields are defined.
// You may only test the bits defined in the version matching the version
// stamp.
//
// NOTE: The version stamp is only valid for the ET_VERSION structure!
//   Don't validate other structures with this!
//
// NOTE: A tool may only test the capabilities defined for the version
//   matching the version stamp.
//
// NOTE: Newer versions must support the capabilites of older versions!
//
// NOTE: A tool must check for an equal version stamp as older drivers
//   didn't touch the reserved flags, and leaved them initialized!
//   It isn't allowed to do a greater equal test on the verstion stamp!
//
// example:
/*
// FNicklis 25.11.98 15:30:15: example program
BOOL bIsCVBoard(
  IN const ET_VERSION_INFO *pVersionInfo)  // pointer to version info (read outside)
{
  const ET_VERSION *pVersion;
  BOOL              bRet     = FALSE; // default is no CV board

  assert(NULL!=pVersionInfo);

  pVersion = &(pVersionInfo->Version);

  // as it is a union it must be the same!
  assert(pVersion->dwNTVersionStamp ==pVersion->dwW95VersionStamp);
  assert(pVersion->dwW95Capabilities==pVersion->dwNTCapabilities);

  //Not allowed! if ( pVersion->dwNTVersionStamp>ET_VERSION_STAMP10 )

  // ET_VERSION_STAMP10 was the first version that supported
  // this extenstion

  if ( ET_VERSION_STAMP10==pVersion->dwNTVersionStamp )
  {
    // it's a version 1.0, supported by our app

    // we may test 1.0 features: ET_VERSION_CAPS_CV_PRODUCT and ET_VERSION_CAPS_2D_BOARD_ONLY
    if ( pVersion->dwNTCapabilities & ET_VERSION_CAPS_CV_PRODUCT )
    {
      bRet = TRUE;
    }
  }
  else
  {
    // error, interface not valid
    // older board -> fallback
    // unknown board -> exit

    // .... add code here
  }

  return (bRet);
}
*/
// History:
// ???????? uninitialized and any other value than defined VERSION_STAMPS are
//          invalid and don't support the interface!
// 00000010: First version 1.0 valid and testableET_VERSION_CAPS_xx:
//     xx_CV_PRODUKT
//     xx_2D_BOARD
#define ET_VERSION_STAMP10 0x00000010         // FNicklis 30.09.1998 first version

// please don't use ET_VERSION_STAMP any longer!
#define ET_VERSION_STAMP  ET_VERSION_STAMP10  // Current version stamp is highest available, but better is
                                              // to use the stamp you know you support!
// capabilities
// defined in 1.0:
//                      CH == !CV product
#define ET_VERSION_CAPS_CV_PRODUCT     0x01 // The board is a CV product (GLoria, Synergy, ...)
                                            // GLoria Settings, POWERlib, ... must run
//                      3D == !2D
#define ET_VERSION_CAPS_2D_BOARD_ONLY  0x02 // The board has only 2D and no hardware 3D capabilities (Trio, S3 986,..)

//
#define ET_VERSION_CAPS_DIRECT3DSTEREO 0x04 // Driver supports REVELATOR type Stereo for Direct3D
#define ET_VERSION_CAPS_OPENGLSTEREO   0x08 // Driver supports REVELTAOR type Stereo for OpenGL
#define ET_VERSION_CAPS_OVERCLOCKING   0x10 // Driver supports overclocking
#define ET_VERSION_CAPS_TEMPERATURE    0x20 // Driver supports temperature control

// ... to be continued


// used with SUBESC_ET_QUERY_VERSION_INFO
typedef struct tagET_VERSION
{
  DWORD dwFlags;                     // Flags to define WHQL, BETA, ... driver
  DWORD dwAlign1;                    // needed to align on 8 byte boundaries

  // NT and Win95 version data is different, so use a union to match all
  union
  {
    struct // Windows 95 version information
    {
      DWORD dwW95Major;        //
      DWORD dwW95Minor;        //
      DWORD dwW95BuildMajor;   //
      DWORD dwW95BuildMinor;   //
      DWORD dwW95BuildPrivate; //
      DWORD dwW95Reserved1;    // unused
      DWORD dwW95Capabilities; // bitfield containing capabilities (ET_VERSION_CAPS_xx)
      DWORD dwW95VersionStamp; // == ET_VERSION_STAMP (read comment above!) Has to be verified before reading dwW95Capabilities!
    };
    struct // Windows NT version information
    {           // e.g 5.12.00.345
      DWORD dwNTMajor;      //   5
      DWORD dwNTMinor;      //  12
      DWORD dwNTBuildMajor; //  00
      DWORD dwNTBuildMinor; // 345
      DWORD dwNTReserved0;
      DWORD dwNTReserved1;
      DWORD dwNTCapabilities; // bitfield containing capabilities (ET_VERSION_CAPS_xx)
      DWORD dwNTVersionStamp; // == ET_VERSION_STAMP (read comment above!). Has to be verified before reading dwNTCapabilities!
    };
  };

  char  szVersion[MAX_ET_VERSION]; // Version string (ANSI/ASCCI)

} ET_VERSION;

#ifdef WIN32
#pragma warning( default:4201 ) // nonstandard extension used : nameless struct/union
#endif

// Out structure (Driver -> Tool):
// Structure to retrieve driver version and registry data
typedef struct tagET_VERSION_INFO
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_VERSION_INFO)

  // Driver version information (system dependent!)
  ET_VERSION Version;

  // Complete path to drivers registry (be sure it is defined long enough!)
  char  szRegDriverKey[ET_MAX_PATH]; // e.g. NT:    "\Registry\Machine\System\CurrentControlSet\Services\EGLXLM"
                                 // e.g. Win95: "\Registry\Machine\System\CurrentControlSet\Services\Class\Display\0000"

} ET_VERSION_INFO;


// Out structure (Driver -> Tool):
// Structure to retrieve complete path to key where the monitor is defined
typedef struct tagET_MONITORKEY_REGPATH
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_MONITORKEY_REGPATH)

  // Complete path to Key where the ICD is defined
  char  szRegMonitorKey[ET_MAX_PATH];  // e.g. Win95: "\Registry\Machine\System\CurrentControlSet\Services\Class\Monitor\0000"

} ET_MONITORKEY_REGPATH;


// Out structure (Driver -> Tool):
// Structure to retrieve complete path to key where D3DSettings store its settings
typedef struct tagET_D3DSETTINGS_REGPATH
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_D3DSETTINGS_REGPATH)

  // Complete path to Key where the ICD is defined
  char  szRegD3DSettingsKey[ET_MAX_PATH];  // e.g. Win95: "\Registry\Machine\Software\ELSA\Erazor"

} ET_D3DSETTINGS_REGPATH;


// Out structure (Driver -> Tool):
// Structure to retrieve complete path to Key where the ICD is defined
typedef struct tagET_OPENGLICD_REGPATH
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_OPENGLICD_REGPATH)

  // Complete path to Key where the ICD is defined
  char  szRegICDKey[ET_MAX_PATH];  // e.g. NT: "\Registry\Machine\Software\Microsoft\WindowsNT\OpenGLDrivers\ELSA EGLXLM driver"
  char  szRegICDName[ET_MAX_PATH]; // e.g. NT: "EOGLXL"

} ET_OPENGLICD_REGPATH;




// ET_EDD_GETVIDMEMINFO
// get surface counts
typedef struct tagET_EDD_GETVIDMEMINFODATA
{
  DWORD   dwSize;
  DWORD   dwPrimaryCount;
  DWORD   dwOffscreenCount;
  DWORD   dwOverlayCount;
} ET_EDD_GETVIDMEMINFODATA, FAR* LPET_EDD_GETVIDMEMINFODATA;


#define ET_COLOR_CAPS_CONTRAST    0x01    // driver supports contrast settings and limitations are valid
#define ET_COLOR_CAPS_BRIGHTNESS  0x02    // driver supports brightness settings and limitations are valid
#define ET_COLOR_CAPS_SATURATION  0x04    // driver supports saturation settings and limitations are valid
#define ET_COLOR_CAPS_HUE         0x08    // driver supports hue settings and limitations are valid

// ET_EDD_GETCOLORCAPSLIMITS
// get capability of contrast, brightness saturation and hue
// and their limits. The limits are only valid if the capability
// is set.
typedef struct tagET_EDD_GETCOLORCAPSLIMITS
{
  DWORD   dwSize;
  DWORD   dwColorCaps;
  long    lContrastMin;
  long    lContrastMax;
  long    lBrightnessMin;
  long    lBrightnessMax;
  long    lSaturationMin;
  long    lSaturationMax;
  long    lHueMin;
  long    lHueMax;
  long    reserved[20];     // some spare left for future
} ET_EDD_GETCOLORCAPSLIMITS, FAR* LPET_EDD_GETCOLORCAPSLIMITS;


// ET_EDD_SETCOLORADJUSTMENT
// set contrast, brightness saturation and hue
// values for each item is 0..9999
typedef struct tagET_EDD_SETCOLORADJUSTMENTDATA
{
  ET_QUERY hdr;				// size and subescape SUBESC_ET_EDD_SETCOLORADJUSTMENT
  long   dwContrast;
  long   dwBrightness;
  long   dwSaturation;
  long   dwHue;
} ET_EDD_SETCOLORADJUSTMENTDATA, FAR* LPET_EDD_SETCOLORADJUSTMENTDATA;


// ET_EDD_GETCOLORADJUSTMENT
// get contrast, brightness saturation and hue
// values for each item is 0..9999
typedef struct tagET_EDD_GETCOLORADJUSTMENTDATA
{
  DWORD   dwSize;
  long    dwContrast;
  long    dwBrightness;
  long    dwSaturation;
  long    dwHue;
  DWORD   dwYUVOverlayVisibleCount;
} ET_EDD_GETCOLORADJUSTMENTDATA, FAR* LPET_EDD_GETCOLORADJUSTMENTDATA;


// ET_EDD_SETFEATURES  set ELSA DirectDraw features
typedef struct tagET_EDD_SETFEATURESDATA
{
  ET_QUERY hdr;				// size and subescape SUBESC_ET_EDD_SETFEATURES
  DWORD   dwEddCaps0;
  DWORD   dwEddCaps1;
} ET_EDD_SETFEATURESDATA, FAR* LPET_EDD_SETFEATURESDATA;


// ET_EDD_GETFEATURES get ELSA DirectDraw features
typedef struct tagET_EDD_GETFEATURESDATA
{
  DWORD   dwSize;
  DWORD   dwEddCaps0;
  DWORD   dwEddCaps1;
} ET_EDD_GETFEATURESDATA, FAR* LPET_EDD_GETFEATURESDATA;


// ET_EDD_GETCAPS
//   retrieves capabilitie of ELSA DirectDraw features
//   capabilities depends on hardware, resolution and pixel depth
typedef struct tagET_EDD_GETCAPSDATA
{
  DWORD   dwSize;
  DWORD   dwEddCaps0;
  DWORD   dwEddCaps1;
} ET_EDD_GETCAPSDATA, FAR* LPET_EDD_GETCAPSDATA;

// ET_GAMMA_SET
//   _TEMPORARY_ set a gamma table.
//   This escape immediately sets the gamma table to the device. The table
//   is not stored permanently by the device. Means, after reboot, the
//   original 1:1 (or what ever else) table is reloaded by hardware.
//   To store the table, use the ELSA Subescape:
//      SUBESC_ET_SET_VALUE_BINARY::ET_VALUE_BIN_GDI_GAMMARAMP
//
//   Information about the success of the SET call is provided
//   via a ET_SET_VALUE_RET structure
//   If the hardware generally supports gamma ramps, but not in the current
//   mode (256 colors), the flags field in the output structure contains
//   ET_VALUE_BLOCKED.

typedef struct tagET_EDD_SETGAMMARAMP
{
  ET_QUERY hdr;       // size and subescape SUBESC_ET_EDD_SETGAMMARAMP
  WORD     red[256];
  WORD     green[256];
  WORD     blue[256];
} ET_EDD_SETGAMMARAMP, FAR* LPET_EDD_SETGAMMARAMP, * PET_EDD_SETGAMMARAMP;

typedef struct tagET_EDD_GETGAMMARAMP
{
  DWORD   dwSize;
  DWORD   dwResult;		// ET_VALUE_OK if succeeded and ET_VALUE_BLOCKED if Gamma not supported yet
  WORD    red[256];
  WORD    green[256];
  WORD    blue[256];
} ET_EDD_GETGAMMARAMP, FAR* LPET_EDD_GETGAMMARAMP, * PET_EDD_GETGAMMARAMP;

// ET_VIDEO_DMACOPYSM
//   copies video memory to system memory by DMA
typedef struct tagET_VIDEO_DMACOPYSMDATA
{
  ET_QUERY  hdr;

  DWORD     dwDstSel;       // Selector of destination memory (16Bit) / NULL (32Bit)
  DWORD     dwDstOfs;       // Offset of destination memory (16Bit) / Linear Address (32Bit)
  DWORD     dwDstStride;    //

  DWORD     dwSrcSel;       // Selector of source video memory (16Bit) / NULL (32Bit)
                            // The base of the selector points to the start of the video memory
  DWORD     dwSrcOfs;       // Offset of source video memory (16Bit) / Linear Address (32Bit)
                            // This is the offset relative to the beginning of video memory
  DWORD     dwSrcStride;    //

  DWORD     dwWidthBytes;   // Amount of bytes to transfer in one line (must be aligned to 4)
  DWORD     dwHeight;       // Anount of lines to transfer
} ET_VIDEO_DMACOPYSMDATA, FAR* LPET_VIDEO_DMACOPYSMDATA;

// ET_VIDEO_OUTCONTROL
//   controls video-out
typedef struct tagET_VIDEO_OUTCONTROL
{
  ET_QUERY  hdr;

  DWORD     dwEnable;
  DWORD     dwControl;
  DWORD     dwParam1;
  DWORD     dwParam2;
} ET_VIDEO_OUTCONTROL, FAR* LPET_VIDEO_OUTCONTROL, * PET_VIDEO_OUTCONTROL;

// ET_RMAPI_CONFIGSETGET
//   allows escape access to stateConfigGet [...] routines.
typedef struct tagET_RMAPI_CONFIGSETGET
{
  ET_QUERY  hdr;
  DWORD     dwSetGetCommand;
  DWORD     dwIndex;
  DWORD     dwlpParamStruct;
  DWORD     dwParamSize;
  DWORD     dwValue;
  DWORD     dwParam0;
  DWORD     dwParam1;
  DWORD     dwParam2;
} ET_RMAPI_CONFIGSETGET, FAR* LPET_RMAPI_CONFIGSETGET, * PET_RMAPI_CONFIGSETGET;

// Constants for ELSA DirectDraw features:
//
// dwEddCaps0:
#define EDDCAPS_DITHER              0x00000001
#define EDDCAPS_FILTERX             0x00000002
#define EDDCAPS_FILTERY             0x00000004
#define EDDCAPS_FILTERX_LIN02420    0x00000008
#define EDDCAPS_FILTERX_BILINEAR    0x00000010
#define EDDCAPS_FILTERX_LIN12221    0x00000020
#define EDDCAPS_OFFSCREENINHIBIT    0x00000040
#define EDDCAPS_OFFSCREENLINEAR     0x00000080
#define EDDCAPS_COLORADJUSTYUV_CB   0x00000100
#define EDDCAPS_COLORADJUSTYUV_HS   0x00000200
#define EDDCAPS_FILTERXY_BILINEAR   0x00000400
#define EDDCAPS_DITHER_ALWAYS_ON    0x00000800
#define EDDCAPS_ENABLE_332_TEXTURE  0x00001000

// 3D-Features
#define EDDCAPS_INHIBIT_ZBUFFER     0x00010000
#define EDDCAPS_INHIBIT_PERSCORRECT 0x00020000
#define EDDCAPS_INHIBIT_TEXTURE     0x00040000
#define EDDCAPS_INHIBIT_FOG         0x00080000
#define EDDCAPS_INHIBIT_FILTER      0x00100000
#define EDDCAPS_INHIBIT_ALPHA       0x00200000
// ViRGE/DX has a fast bilinear algorithm
#define EDDCAPS_FASTBILINEAR        0x00400000
// emuliere Colorkey for a Texture using texture alpha blending
#define EDDCAPS_EMULATE_TEX_CKEY    0x00800000
#define EDDCAPS_SIMULATE_MIPMAP     0x01000000

// Commands
#define EDDCMD_ALLOCFILTERYBUF      0x80000001

// Bedeutung der einzelnen Bits in dwEddCaps0 beim Aufruf von
// EDDESC_SETFEATURES:
//
// Achtung: Ein Bit darf nur gesetzt werden, wenn das entsprechende Bit
//          vorher bei einem Aufruf von EDD_GETCAPS als gueltig gemeldet
//          wurde.
//
// EDDCAPS_DITHER  : 1=Dithering ein. 0=Dithering aus.
//
// EDDCAPS_FILTERX : 1=Schalte einen Filter ein. Ggf. kann mit anderen
//                   Konstanten eine spezieller Filter definiert werden.
//                   Wird kein spezieller Filter ausgewaehlt (obwohl
//                   es moeglich waere) so sucht der Treiber einen adequaten
//                   Filter aus.
//                   0=Alle Filter aus.
//
// EDDCAPS_FILTERX_xxxxx : Schalte diesen speziellen Filter ein, wenn auch
//                         EDDCAPS_FILTERX gesetzt ist.
//
// EDDCAPS_OFFSCREENLINEAR: Weist den DirectDraw-Treiber an, linearen
//                          Offscreen falls moeglich (!) zu benutzen, um z.B.
//                          eine bessere Speicherausnutzung zu erreichen, oder
//                          die Erzeugung von Offscreen-Surfaces zu ermoeglichen,
//                          deren Breite (in Bytes) groesser ist als die Breite
//                          des Primary-Surface.
//                          Wenn dieses Bit beim Zuruecklesen gesetzt ist,
//                          ist der Offscreen tatsaechlich linear, ansonsten
//                          nicht.
//
//
// EDDCAPS_COLORADJUSTYUV_CB : 0=Kontrast/Helligkeit-Werte ohne Wirkung
//                             1=Kontrast/Helligkeit wird gemaess der per
//                               EDDESC_SETCOLORADJUSTMENT gewaehlten Werte
//                               eingestellt.
//
// EDDCAPS_COLORADJUSTYUV_HS : 0=Phase/Saettigungs-Werte ohne Wirkung
//                             1=Phase/Saettigung wird gemaess der per
//                               EDDESC_SETCOLORADJUSTMENT gewaehlten Werte
//                               eingestellt.

// Bedeutung der einzelnen Bits in dwEddCaps0 beim Aufruf von EDDESC_GETCAPS:
// -------------------------------------------------------------------------
//
// EDDCAPS_DITHER  : Der Treiber beherrscht Dithering zur Verbesserung der
//                   Bildqualitaet.
//
// EDDCAPS_FILTERX : Der Treiber beherrscht eine allgemeine Methode zum
//                   Filtern in X-Richtung. Ueber andere Bits
//                   EDDCAPS_FILTERX_xxxxx KANN der Treiber angeben, welche
//                   speziellen Filterfaehigkeiten er kennt.
//
// EDDCAPS_FILTERY : Der Treiber beherrscht eine allgemeine Methode zum
//                   Filtern in Y-Richtung.
//
// EDDCAPS_FILTERX_LIN02420
// EDDCAPS_FILTERX_BILINEAR
// EDDCAPS_FILTERX_LIN12221 : Der Treiber beherrscht diesen Filter in X-
//                            Richtung. Wenn diese Bit gesetzt ist, wird
//                            auch EDDCAPS_FILTERX gesetzt.
//
// EDDCAPS_OFFSCREENINHIBIT : Der Treiber bietet die Moeglichkeit, die
//                            Erzeugung von Offscreen-Surfaces zu
//                            unterbinden.
//
// EDDCAPS_OFFSCREENLINEAR : Der Treiber bietet die Moeglichkeit, lineare
//                           Offscreenverwaltung zu erwuenschen.
//                           Diesen Wunsch kann der Treiber erfuellen, er
//                           muss es aber nicht.
//
// EDDCAPS_COLORADJUSTYUV_CB : Der Treiber bietet die Moeglichkeit Kontrast
//                             und Helligkeit eines YUV-Videos einzustellen.
//
// EDDCAPS_COLORADJUSTYUV_HS : Der Treiber bietet die Moeglichkeit "Hue" und
//                             Saettigung eines YUV-Videos einzustellen.





// ********************************************************
// ********************************************************
// New *_VALUE_* Interface to replace registry accesses for driver data by escapes
// -------------------------------------------------------------------------------

// In structure (Tool -> driver):
// Input Structure to set a registry string
// Output Structure ET_SET_VALUE_RET
typedef struct tagET_SET_VALUE_STRING
{
  ET_QUERY hdr;   // size and subescape SUBESC_ET_SET_VALUE_STRING
                  // dwSize parameter of ET_QUERY holds length of this structure
                  //    i.e. hdr.dwSize >= sizeof(ET_SET_VALUE_STRING)
                  //   ET_MAX_STRING is the minimal length of the string!

  DWORD dwID;     // ID of string to set (ET_VALUE_SZ_...)
  char  szValue[ET_MAX_STRING];

} ET_SET_VALUE_STRING, *PET_SET_VALUE_STRING;

// In structure (Tool -> driver):
// Input Structure to set a registry DWORD
// Output Structure ET_SET_VALUE_RET
typedef struct tagET_SET_VALUE_DWORD
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_SET_VALUE_DWORD

  DWORD    dwID;    // ID of dword to set (ET_VALUE_DW_...)
  DWORD    dwValue; // value to set

} ET_SET_VALUE_DWORD, *PET_SET_VALUE_DWORD;

// In structure (Tool -> driver):
// Input Structure to set binary data to registry
// Output Structure ET_SET_VALUE_RET
typedef struct tagET_SET_VALUE_BINARY
{
  ET_QUERY hdr;       // size and subescape SUBESC_ET_SET_VALUE_DWORD

  DWORD    dwID;      // ID of dword to set (ET_VALUE_DW_...)
  char     ajData[1]; // array of binary data
  // The true size of this array can be re-calculated from hdr.dwSize

} ET_SET_VALUE_BINARY, *PET_SET_VALUE_BINARY;
// Size of the structure without the data array
#define ET_SET_VALUE_BINARY_HDR_SIZE (sizeof(ET_SET_VALUE_BINARY)-sizeof(DWORD))

// use in struct ET_QUERY_VALUE.dwType
#define ET_VALUE_TYPE_DWORD           0x00  // subesc SUBESC_ET_QUERY_VALUE_DWORD
#define ET_VALUE_TYPE_STRING          0x01  // subesc SUBESC_ET_VALUE_TYPE_STRING
#define ET_VALUE_TYPE_BINARY          0x02  // subesc SUBESC_ET_VALUE_TYPE_BINARY

// Query defaults: Same as ET_VALUE_TYPE_xx but driver allways returns the values default.
// If an ESC-error is returned, the driver doesn't support the default query!
#define ET_VALUE_DEFAULT_FLAG         0x80
#define ET_VALUE_TYPE_DEFAULT_DWORD   (ET_VALUE_DEFAULT_FLAG|ET_VALUE_TYPE_DWORD)   // subesc SUBESC_ET_QUERY_VALUE_DWORD
#define ET_VALUE_TYPE_DEFAULT_STRING  (ET_VALUE_DEFAULT_FLAG|ET_VALUE_TYPE_STRING)  // subesc SUBESC_ET_VALUE_TYPE_STRING

// In structure (Tool -> driver):
// Input Structure to query a registry DWORD
// Output data is returned in ET_VALUE_STRING or ET_VALUE_DWORD
typedef struct tagET_QUERY_VALUE
{
  ET_QUERY hdr;    // size and subescapes SUBESC_ET_QUERY_VALUE_STRING or SUBESC_ET_QUERY_VALUE_DWORD
                   // if outsize for strings > sizeof(ET_VALUE_STRING) the buffer can take longer than the
                   // default strings! Only check if the out buffer is to small to fit the data. The bigger
                   // the better!

  DWORD dwID;      // ID of dword to query (ET_VALUE_DW_..., ET_VALUE_SZ_...)
  DWORD dwType;    // This identifies what kind of data is requested (type of output structure) (ET_VALUE_TYPE_DWORD,...)
                   // Allways assign this value and be sure to set all necessary bits too!
} ET_QUERY_VALUE, *PET_QUERY_VALUE;


// Out structure (Driver -> Tool):
// Output Structure used to return a string from the driver in subesc SUBESC_ET_QUERY_VALUE_STRING
// used by input structure ET_QUERY_VALUE and dwType ET_VALUE_TYPE_STRING
typedef struct tagET_VALUE_STRING
{
  DWORD dwSize;    // Length of this structure (should be the same as ET_QUERY_VALUE's outbuffer size)
  DWORD dwID;      // identifier of string to query (additional and as alignment) (ET_VALUE_SZ_...)
                   // should be the same as in the calling ET_QUERY_VALUE
  DWORD dwFlags;   // return value, ET_VALUE_OK=value set and all is OK, others as ET_SET_VALUE_REBOOT are
                   // error codes or additional information flags
  char  szValue[ET_MAX_STRING]; // zero terminated ASCII/ANSI string (no UNICODE!)

} ET_VALUE_STRING, *PET_VALUE_STRING;


// Out structure (Driver -> Tool):
// Structure used to return a DWORD from the driver in subesc SUBESC_ET_QUERY_VALUE_DWORD
// used by input structure ET_QUERY_VALUE and dwType ET_VALUE_TYPE_DWORD
typedef struct tagET_VALUE_DWORD
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_VALUE_DWORD)
                   // (should be the same as ET_QUERY_VALUE's outbuffer size)

  DWORD dwID;      // identifier of queried dword to query (additional and as alignment) (ET_VALUE_DW_...)
                   // should be the same as in the calling ET_QUERY_VALUE
  DWORD dwFlags;   // return value, ET_VALUE_OK=value set and all is OK, others as ET_SET_VALUE_REBOOT are
                   // error codes or additional information flags
  DWORD dwValue;   // return value

} ET_VALUE_DWORD, *PET_VALUE_DWORD;

// Out structure (Driver -> Tool):
// Structure used to return binary data from the driver in subesc
// SUBESC_ET_QUERY_VALUE_BINARY used by input structure ET_QUERY_VALUE
// and dwType ET_VALUE_TYPE_BINARY
typedef struct tagET_VALUE_BINARY
{
  DWORD dwSize;    // Length of this structure, at least ET_QUERY_VALUE_BINARY_HDR_SIZE
                   // (should be the same as ET_QUERY_VALUE's outbuffer size)
  DWORD dwID;      // identifier of binary to query (additional and as alignment) (ET_VALUE_DW_...)
                   // should be the same as in the calling ET_QUERY_VALUE
  DWORD dwFlags;   // return value,
                   // ET_VALUE_OK=value set and all is OK,
                   // ET_VALUE_ERROR_SIZE=structure is too short for all the data,
                   //                     see dwSizeData for the required size
                   // others are error codes or additional information flags
  DWORD dwSizeData; // size of binary data, especially the required size, when
                    // dwFlags return ET_VALUE_ERROR_SIZE

  char  ajData[1]; // dwSizeData bytes of data

} ET_VALUE_BINARY, *PET_VALUE_BINARY;
// Size of the structure without the data array
#define ET_QUERY_VALUE_BINARY_HDR_SIZE (sizeof(ET_VALUE_BINARY)-sizeof(DWORD))


// Flags used in ET_SET_VALUE_RET or ET_VALUE_XXX as return values
//   bit 31 (higest bit) == 0: function was successfull and additional information for values != 0
//   bit 31 (higest bit) == 1: error
#define IsError(Value)      ((Value & 0x80000000)!=0)
#define IsOK(Value)         ((Value & 0x80000000)==0)


//  ET_VALUE_xx return codes used in dwFlags are 32 bit values layed out as follows:

//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1              Bit No
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-----------------------------------------------------------+
//  |0 r|                                       Bitfield            | success
//  +---+-----------------------------------------------------------+
//  |1 r|                                       ErrorCode           | error
//  +---+-----------------------------------------------------------+
//  r = reserved
//                            bitfield
#define ET_VALUE_OK         0x00000000 // Value function is successfull
#define ET_VALUE_REBOOT     0x00000001 // a reboot is necessary to acivate after value set
#define ET_VALUE_DEFAULT    0x00000002 // Value function had problems and returned defaults
#define ET_VALUE_BLOCKED    0x00000004 // Value is blocked by running application and not set in driver! (e.g. ask user to close application)

//                          Bit   Code
#define ET_VALUE_ERROR		  0x80000000 // application reserved value for ESCAPE failed (not used in driver!)
#define ET_VALUE_UNKNOWN    0x80000001 // Value to get/set is unknown or not supported (e.g. grey button)
#define ET_VALUE_ERROR_SIZE 0x80000002 // returnbuffer is smaller than the value to get

// Out structure (Driver -> Tool):
// structure passed back to caller
typedef struct tagET_SET_VALUE_RET
{
  DWORD dwSize;    // Length of this structure, sizeof(ET_SET_VALUE_RET)

  DWORD dwID;      // identifier of set value (additional and as alignment) (ET_VALUE_DW_...)

  DWORD dwFlags;   // return value, ET_VALUE_OK=value set and all is OK, others as ET_SET_VALUE_REBOOT are
                   // error codes or additional information flags

  DWORD dwAlign;   // unused
} ET_SET_VALUE_RET, *PET_SET_VALUE_RET;


// ********************************************************
// query/set values identifier:
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1              Bit No
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-------+-------+-----------------------------------------------+
//  |o o o o|r r r r|                                          code |
//  +-------+-------+-----------------------------------------------+
//
//  o = bitfield defining OS dependencies
//    0       = no specific OS
//    0x1     = Win9x
//    0x2     = Win NT
//    0x4,0x8 = unused
//  r = bitfield reserved for driver internal stuff.
//    This field must be 0 in all IDs-codes called via ExtEscape!
//
//    Use this code to check Esc-IDs validity:
//
//    DrvEscape (...)
//    {
//      // check if the caller gives us a valid dwID,
//      // All reserved bits have to be 0!
//      ASSERT( 0==(pEtValue->dwID & ET_VALUE_OS_RESERVED) ); // check the tool which called!
//      ...
//    }
//
//  code = number of identifer, no bitfield.
//    valid range: 0x000000 - 0xffffff (3 Byte)
//    New ID code increments by one.
//
//  Resulting ranges:
//    0x00000000 - 0x00ffffff basic values, common to all operating systems
//    0x10000000 - 0x10ffffff to be defined by Win95
//    0x20000000 - 0x20ffffff to be defined by NT (NTeam)
//    0x40000000 - 0x40ffffff unused
//    0x80000000 - 0x80ffffff unused
// Naming: ET_VALUE_DW_xxxx, read: ELSA tools value dword
//         ET_VALUE_SZ_xxxx, read: ELSA tools value zero terminated string (single byte char)
//

#define ET_VALUE_OS_WIN9x                                   0x10000000 // Bit defining IDs known by Windows 9x driver
#define ET_VALUE_OS_WINNT                                   0x20000000 // Bit defining IDs known by Windows NT driver
#define ET_VALUE_OS_unused0                                 0x40000000 //
#define ET_VALUE_OS_unused1                                 0x80000000 //

#define ET_VALUE_OS_RESERVED                                0x0F000000 // Bits that are reserved for the driver internaly

// Windows 9x
//
#define ET_VALUE_DW_D3DOGL_MCLK                             ( ET_VALUE_OS_WIN9x | 0x00CA0001 )
#define ET_VALUE_DW_D3DOGL_NVCLK                            ( ET_VALUE_OS_WIN9x | 0x00CA0002 )
#define ET_VALUE_DW_D3DOGL_OVERCLOCK_ENABLE                 ( ET_VALUE_OS_WIN9x | 0x00CA0003 )
#define ET_VALUE_DW_D3DOGL_OVERCLOCK_CRASHSTATUS            ( ET_VALUE_OS_WIN9x | 0x00CA0004 )
#define ET_VALUE_DW_D3DOGL_DEFAULT_MCLK                     ( ET_VALUE_OS_WIN9x | 0x00CA0005 )
#define ET_VALUE_DW_D3DOGL_DEFAULT_NVCLK                    ( ET_VALUE_OS_WIN9x | 0x00CA0006 )

// Windows NT
//
// Basic/system 0x20000000-0x200000ff
//#define ET_VALUE_SZ_IMAGEPATH                               0x20000000
//#define ET_VALUE_SZ_INSTALLEDDISPLAYDRIVERS                 0x20000001

#define ET_VALUE_DW_GDI_DESKTOPONMAXIMIZE                   0x20000050 // DesktopOnMaximize
#define ET_VALUE_DW_GDI_ENABLEDEVICEBITMAPS                 0x20000051 // EnableDeviceBitmaps
#define ET_VALUE_DW_GDI_ENABLEPCIDISCONNECT                 0x20000052 // PCIDisconnect
#define ET_VALUE_DW_GDI_EXPORTONLYDOUBLEBUFFERMODES         0x20000053 // ExportOnlyDoubleBufferModes
#define ET_VALUE_DW_GDI_NTEAMPRIVATEKEY                     0x20000054
#define ET_VALUE_DW_GDI_P6ENABLEUSWC                        0x20000055 // DRV_PPRO_ENABLE_WCB
#define ET_VALUE_DW_GDI_REVERSESCREENORDER                  0x20000056 // ReverseScreenOrder
#define ET_VALUE_DW_GDI_USEGETACCESSRANGES                  0x20000057 // UseGetAccessRanges
#define ET_VALUE_DW_GDI_USELCDPANEL                         0x20000058 // UseLCDPanel
#define ET_VALUE_DW_GDI_VERTICALSCREENORDER                 0x20000059 // VerticalScreenOrder
#define ET_VALUE_DW_GDI_WAITFORFIFOS                        0x2000005a // EngineSynchronization
#define ET_VALUE_DW_GDI_DEVICEORIGINX                       0x2000005b
#define ET_VALUE_DW_GDI_DEVICEORIGINY                       0x2000005c
#define ET_VALUE_DW_GDI_DEVICESIZEX                         0x2000005d
#define ET_VALUE_DW_GDI_DEVICESIZEY                         0x2000005e
#define ET_VALUE_DW_GDI_VISUALORIGINX                       0x2000005f
#define ET_VALUE_DW_GDI_VISUALORIGINY                       0x20000060
#define ET_VALUE_DW_GDI_VISUALSIZEX                         0x20000061
#define ET_VALUE_DW_GDI_VISUALSIZEY                         0x20000062
#define ET_VALUE_DW_GDI_DELLXXL                             0x20000063 // DELLXXL
#define ET_VALUE_DW_GDI_PERMEDIACLOCKSPEED                  0x20000064 // PermediaClockSpeed
#define ET_VALUE_DW_GDI_GLINTCLOCKSPEED                     0x20000065 // GlintClockSpeed
#define ET_VALUE_DW_GDI_GLINTGAMMACLOCKSPEED                0x20000066 // GlintGammaClockSpeed
#define ET_VALUE_DW_GDI_GLINTDELTACLOCKSPEED                0x20000067 // GlintDeltaClockSpeed
#define ET_VALUE_DW_GDI_HWLINES                             0x20000068 // TNT: interface to enable nonconformant but faster lines
#define ET_VALUE_DW_GDI_FIFOEXTRA_P2                        0x20000069 // GDI.FifoExtra
#define ET_VALUE_DW_GDI_FIFOEXTRA                           0x2000006a // GDI.FifoExtra
#define ET_VALUE_DW_GDI_FIFOUSAGE                           0x2000006b // GDI.FifoUsage
#define ET_VALUE_DW_GDI_EXPORTMULTIBOARDMODES               0x2000006c // ExportMultiboardModes
#define ET_VALUE_DW_GDI_SURFACESONBANKBOUNDARIES            0x2000006d // boolean
#define ET_VALUE_DW_GDI_DMABUFFERSIZE                       0x2000006e // ulong ( > 8kB)
#define ET_VALUE_DW_GDI_DMAPADDING                          0x2000006f // ulong (ET_VALUE_DW_GDI_DMAPADDING < ET_VALUE_DW_GDI_DMABUFFERSIZE)
#define ET_VALUE_DW_GDI_BOARD_PATTERN                       0x20000070 // binary: defines boards used for the mode (00 01 02,...)
#define ET_VALUE_DW_GDI_AGP_DMA                             0x20000071 // bool: 1=use AGP for display driver DMA, 0=don't use AGP
#define ET_VALUE_DW_GDI_FRAMEBUFFER_DMA                     0x20000072 // bool: 1=use framebuffer DMA for display driver

#define ET_VALUE_DW_MAP_FRAMEBUFFER                         0x20000090 // Map.FrameBuffer
#define ET_VALUE_DW_MAP_GLINTMMIO                           0x20000091 // Map.GLintMmio
#define ET_VALUE_DW_MAP_DELTAMMIO                           0x20000092 // Map.DeltaMmio
#define ET_VALUE_DW_MAP_LOCALBUFFER                         0x20000093 // Map.LocalBuffer

#define ET_VALUE_DW_PPRO_ENABLEWCB                          0x20000095 // PPro.EnableWCB
#define ET_VALUE_DW_ALPHA_MIATA                             0x20000096 // Alpha.Miata
#define ET_VALUE_DW_PRINTANDCHECKRENDERSTATE                0x20000097 //

#define ET_VALUE_BIN_GDI_GAMMARAMP                          0x200000A0 // Gammatable in the same format as in GDI::SetGammaRamp
#define ET_VALUE_BIN_OEM_MODELIST                           0x200000A1 // mode list override string: "bpp#1 width#1 height#1 refresh1#1 refresh2#1... 0 bpp#2 width#2 height#2 refresh1#2 refresh2#2... 0";

// ICDInterface.* 0x20000100-0x200001ff
#define ET_VALUE_DW_ICDINTERFACE_DISABLE                    0x20000100 // ICDInterface.Disable
//#define ET_VALUE_DW_ICDINTERFACE_DOUBLEBUFFER               0x20000101
//#define ET_VALUE_DW_ICDINTERFACE_MAXDOUBLEBUFFER            0x20000102
#define ET_VALUE_DW_ICDINTERFACE_NUMBEROFDISPLAYBUFFERS     0x20000101 // ICDInterface.NumberOfDisplayBuffers
#define ET_VALUE_DW_ICDINTERFACE_MAXNUMBEROFDISPLAYBUFFERS  0x20000102 // ICDInterface.MaxNumberOfDisplayBuffers
#define ET_VALUE_DW_ICDINTERFACE_SUPPORTSINGLEBUFFEREDGDI   0x20000103 // ICDInterface.SupportSingleBufferedGDI
#define ET_VALUE_DW_ICDINTERFACE_SUPPORTDOUBLEBUFFEREDGDI   0x20000104 // ICDInterface.SupportDoubleBufferedGDI
#define ET_VALUE_DW_ICDINTERFACE_EXPORTALPHA                0x20000105 // ICDInterface.ExportAlpha
#define ET_VALUE_DW_ICDINTERFACE_SUPPORTOVERLAYPLANES       0x20000106 // ICDInterface.SupportOverlayPlanes
#define ET_VALUE_DW_ICDINTERFACE_VBLANKWAIT                 0x20000107 // ICDInterface.VBlankWait
#define ET_VALUE_DW_ICDINTERFACE_PAGEFLIPPING               0x20000108 // ICDInterface.PageFlipping
#define ET_VALUE_DW_ICDINTERFACE_LBWIDTH                    0x20000109 // ICDInterface.LBWidth
#define ET_VALUE_DW_ICDINTERFACE_SIZEOFBUFFER               0x2000010A // ICDInterface.SizeOfBuffer

#define ET_VALUE_DW_ICDINTERFACE_DUALTXLBMOVES              0x2000010B // ICDInterface.DualTXLBMoves
#define ET_VALUE_DW_ICDINTERFACE_EXTENDEDPAGEFLIP           0x2000010C // ICDInterface.ExtendedPageFlip
#define ET_VALUE_DW_ICDINTERFACE_FNSENDDATATST              0x2000010D // ICDInterface.FNSendDataTst
#define ET_VALUE_DW_ICDINTERFACE_FULLSCNTOPBORDER           0x2000010E // DoubleBuffer.FullScnTopBorder
#define ET_VALUE_DW_ICDINTERFACE_LBSIZE                     0x2000010F // ICDInterface.LBSize
#define ET_VALUE_DW_ICDINTERFACE_P2ENABLEBYPASSDMA          0x20000110 // ICDInterface.P2EnableBypassDma
#define ET_VALUE_DW_ICDINTERFACE_P2ENABLEBYPASSPATCH        0x20000111 // ICDInterface.P2EnableBypassPatch
#define ET_VALUE_DW_ICDINTERFACE_PAGEFLIPFORCESYNC          0x20000112 // ICDInterface.PageFlipForceSync
#define ET_VALUE_DW_ICDINTERFACE_TEXTURESIZE                0x20000113 // ICDInterface.MaxTextureSize
#define ET_VALUE_DW_ICDINTERFACE_USESOFTWAREWRITEMASK       0x20000114 // ICDInterface.UseSoftwareWriteMask
#define ET_VALUE_DW_ICDINTERFACE_USESINGLETX                0x20000115 // ICDInterface.UseSingleTX

#define ET_VALUE_DW_ICDINTERFACE_EXPORTCOLORINDEX           0x20000116
#define ET_VALUE_DW_ICDINTERFACE_OPENGLICDNAME              0x20000117
#define ET_VALUE_DW_ICDINTERFACE_OPENGLICDELSAOGLNAME       0x20000118
#define ET_VALUE_DW_ICDINTERFACE_PFDREDUCTION               0x20000119 // ICDInterface.PFDReduction
#define ET_VALUE_DW_ICDINTERFACE_STEREOFORCEBUFFERS         0x2000011A // ICDInterface.StereoForceBuffers
#define ET_VALUE_DW_ICDINTERFACE_STEREOLEFTEYETOP           0x2000011B // ICDInterface.StereoLeftEyeTop
#define ET_VALUE_DW_ICDINTERFACE_STEREOSUPPORT              0x2000011C // ICDInterface.StereoSupport
#define ET_VALUE_DW_ICDINTERFACE_TEXTURESMARTCACHE          0x2000011D // ICDInterface.TextureSmartCache
#define ET_VALUE_DW_ICDINTERFACE_TEXTURETRANSIENTONLY       0x2000011E // ICDInterface.TextureTransientOnly

#define ET_VALUE_DW_ICDINTERFACE_MAXTEXTURESIZE             0x2000011F // ICDInterface.MaxTextureSize
#define ET_VALUE_DW_ICDINTERFACE_PFD_TYPE_COLORINDEX        0x20000120 // ICDInterface.PFD_TYPE_COLORINDEX
#define ET_VALUE_DW_ICDINTERFACE_LBPATCHED                  0x20000121 // ICDInterface.LBPatched
#define ET_VALUE_DW_ICDINTERFACE_DUALGLINTWINDOWMOVES       0x20000122 // ICDInterface.DualGlintWindowMoves
#define ET_VALUE_DW_ICDINTERFACE_FORCEGDIBROADCASTINGINOGLVIEWS 0x20000123 // ICDInterface.ForceGdiBroadCastingInOglViews
#define ET_VALUE_DW_ICDINTERFACE_PFD_STEREO                 0x20000124 // ICDInterface.PFD_STEREO
#define ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_LAYER_BUFFERS     0x20000125 // ICDInterface.PFD_SWAP_LAYER_BUFFERS
#define ET_VALUE_DW_ICDINTERFACE_PFD_SUPPORT_GDI            0x20000126 // ICDInterface.PFD_SUPPORT_GDI

#define ET_VALUE_DW_ICDINTERFACE_FORCE_STENCIL              0x20000127 // ICDInterface.ForceStencil
#define ET_VALUE_DW_ICDINTERFACE_FORCE_MORETHAN16BITZ       0x20000128 // 0=default, else force to this value!
#define ET_VALUE_DW_ICDINTERFACE_TEXTURE_REDUCE_FACTOR      0x20000129 // ICDInterface.TextureReduceFactor
#define ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_COPY              0x2000012A // ICDInterface.PFD_SWAP_COPY
#define ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_EXCHANGE          0x2000012B // ICDInterface.PFD_SWAP_EXCHANGE

// GLintDMA.* 0x20000200-0x2000024f
#define ET_VALUE_DW_GLINTDMA_NUMBEROFBUFFERS                0x20000200 // GLintDMA.NumberOfBuffers
#define ET_VALUE_DW_GLINTDMA_NUMBEROFSUBBUFFERS             0x20000201 // GLintDMA.NumberOfSubBuffers
#define ET_VALUE_DW_GLINTDMA_SIZEOFBUFFERS                  0x20000202 // GLintDMA.SizeOfBuffer
#define ET_VALUE_DW_GLINTDMA_CACHEDBUFFERS                  0x20000203 // GLintDMA.CachedBuffers

#define ET_VALUE_DW_GLINTDMA_USEAGP                         0x20000204 // GLintDMA.UseAGP
#define ET_VALUE_DW_GLINTDMA_SUPPORTAGPONBX                 0x20000205 // GLintDMA.SupportAGPonBX

#define ET_VALUE_DW_AGP_ENABLE                              ET_VALUE_DW_GLINTDMA_USEAGP // AGP.Enable:  0, 1
#define ET_VALUE_DW_AGP_RATE                                0x20000206                  // AGP.Rate:    1, 2, 4
#define ET_VALUE_DW_AGP_SIDEBANDADDRESSING                  0x20000207                  // AGP.SBA:     0, 1
#define ET_VALUE_DW_AGP_REQUEST_DEPTH                       0x20000208                  // AGP.RQDepth: 1...255
#define ET_VALUE_DW_AGP_FAST_WRITES                         0x20000209                  // AGP.FW:      0, 1

#define ET_VALUE_DW_DMAWAITFORPOLL                          0x2000020A // DMAWaitForPoll

#define ET_VALUE_DW_AGP_CACHING                             0x2000020B // 0=No, 1=Yes (as used in AgpReservePhysical)
#define ET_VALUE_DW_AGP_MSWORKAROUND                        0x2000020C // 0=No, 1=Yes (to enable workaround for bug in MS W2K videoport.sys AGP mapping)

#define ET_VALUE_DW_DMA_7DUMMYREADS                         0x2000020D // 0=No, 1=Yes (7 dummy reads in dma buffer to flush writecombined caches)
#define ET_VALUE_DW_DMA_DUMMYREADUC                         0x2000020E // 0=No, 1=Yes (Perform a dummy read to the UC system memory but not WC to flush writecombined caches)

#define ET_VALUE_DW_DMA_FORCEIOFLUSH                        0x2000020F // 0=No, 1=Yes (Perform inp and outp to flush io buffers)

// GLintIRQ.* 0x20000250-0x200002ff
#define ET_VALUE_DW_GLINTIRQ_ENABLE                         0x20000250 // GLintIRQ.Enable

// OpenGL.* 0x20000300-0x200003ff
#define ET_VALUE_DW_OPENGL_USEHIQUALITYTEXTURE              0x20000300
#define ET_VALUE_DW_OPENGL_UNIXCONFORM                      0x20000301
#define ET_VALUE_DW_OPENGL_DOUBLEWRITE                      0x20000302
#define ET_VALUE_DW_OPENGL_DISABLEFASTCLEAR                 0x20000303 // OpenGL.DisableFastClear
#define ET_VALUE_DW_OPENGL_DISABLEMIPMAPS                   0x20000304
#define ET_VALUE_DW_OPENGL_TEXTURECOMPRESSION               0x20000305
#define ET_VALUE_DW_OPENGL_ENABLEP2ANTIALIAS                0x20000306
#define ET_VALUE_DW_OPENGL_SUPPORTOPENGLVERSION11           0x20000307
#define ET_VALUE_DW_OPENGL_FORCETRUECOLOR16BIT              0x20000308 // OpenGL.ForceTrueColorTexturesTo16bit
#define ET_VALUE_DW_OPENGL_SUPPORTSOFTIMAGE                 0x20000309 // OpenGL.SupportSoftimage
#define ET_VALUE_DW_OPENGL_SUPPORTSOFTIMAGE_351             0x2000030A // OpenGL.SupportSoftimage351

#define ET_VALUE_DW_OPENGL_DRAWLINEENDS                     0x2000030B
#define ET_VALUE_DW_OPENGL_ENABLEFIFOACCESS                 0x2000030C
#define ET_VALUE_DW_OPENGL_DISABLEDELTA                     0x2000030D
#define ET_VALUE_DW_OPENGL_MXDISABLEMIPMAPS                 0x2000030E
#define ET_VALUE_DW_OPENGL_FORCENNTEXFILTER                 0x2000030F
#define ET_VALUE_DW_OPENGL_DISABLEDMAFLUSHES                0x20000310
#define ET_VALUE_DW_OPENGL_DISABLEGXCULL                    0x20000311
#define ET_VALUE_DW_OPENGL_INVERTGXCFORMAT                  0x20000312
#define ET_VALUE_DW_OPENGL_PERSPECTIVECORRECTION            0x20000313
#define ET_VALUE_DW_OPENGL_REALLOCBLOCKSIZE                 0x20000314
#define ET_VALUE_DW_OPENGL_DISABLEDLISTTEXTURES             0x20000315
#define ET_VALUE_DW_OPENGL_DELTANODRAW                      0x20000316
#define ET_VALUE_DW_OPENGL_ENABLEBYPASSPATCH                0x20000317
#define ET_VALUE_DW_OPENGL_ENABLEBYPASSDMA                  0x20000318
#define ET_VALUE_DW_OPENGL_FORCENESTEDDMA                   0x20000319
#define ET_VALUE_DW_OPENGL_DISABLETRIANGLEPACKET            0x2000031A
#define ET_VALUE_DW_OPENGL_PRESERVETRUECOLORTEXTURES        0x2000031B // OpenGL.PreserveTrueColorTextures
#define ET_VALUE_DW_OPENGL_DUALTXPRO                        0x2000031C
#define ET_VALUE_DW_OPENGL_VERTEXCOOPTMAX                   0x2000031D
#define ET_VALUE_DW_OPENGL_NVVVOPTIMIZATION                 0x2000031E
#define ET_VALUE_DW_OPENGL_MIPMAPCONTROL                    0x2000031F
#define ET_VALUE_DW_OPENGL_DISABLETEXTUREPATCHING           0x20000320 // OpenGL.DisableTexturePatching

#define ET_VALUE_DW_OPENGL_USEGIDCLIPPING                   0x20000321 // OpenGL.UseGidClipping
#define ET_VALUE_DW_OPENGL_PLACEMIPMAPSINALTERNATEBANKS     0x20000322 // OpenGL.PlaceMipMapsInAlternateBanks
#define ET_VALUE_DW_OPENGL_DISABLETEXTUREMANAGERLRU         0x20000323 // OpenGL.DisableTextureManagerLRU
#define ET_VALUE_DW_OPENGL_SUPPORTPRODESIGNER               0x20000324 // OpenGL.SupportProDesigner

#define ET_VALUE_DW_OPENGL_DMAMAXCOUNT                      0x20000325 // TNT: OpenGL.DMAMaxCount (size of DMA buffer)
#define ET_VALUE_DW_OPENGL_DMAMINPUSHCOUNT                  0x20000326 // TNT: OpenGL.DMAMinPushCount (fill grade of buffer to force flush)
#define ET_VALUE_DW_OPENGL_MAXTEXTURESIZE                   0x20000327
#define ET_VALUE_DW_OPENGL_MINVIDMEMTEXWIDTH                0x20000328
#define ET_VALUE_DW_OPENGL_MAXTEXHEAPSIZE                   0x20000329
#define ET_VALUE_DW_OPENGL_LIGHTINGANALYSIS                 0x2000032A  // BITFIELD
#define ET_VALUE_DW_OPENGL_LIGHTPICKINGOVERRIDE             0x2000032B  // BITFIELD
#define ET_VALUE_DW_OPENGL_STATISTICS                       0x2000032C  // BITFIELD
#define ET_VALUE_DW_OPENGL_BENCHING                         0x2000032D  // OpenGL.Benching
#define ET_VALUE_DW_OPENGL_USEKATMAI                        0x2000032E // BOOLEAN
#define ET_VALUE_DW_OPENGL_VERTEXCACHESIZE                  0x2000032F // DWORD
#define ET_VALUE_DW_OPENGL_PIXELTUBE                        0x20000330 // DWORD

// OpenGL AGP settings
#define ET_VALUE_DW_OPENGL_AGP_DMA                          0x20000331  // OpenGL.AGP.DMA
#define ET_VALUE_DW_OPENGL_AGP_TEXTURES                     0x20000332  // OpenGL.AGP.Textures
#define ET_VALUE_DW_OPENGL_FORCE_AGP_TEXTURES               0x20000333  // OpenGL.ForceAGPTextures

#define ET_VALUE_DW_OPENGL_FORCE_CLIPPING                   0x20000334  // OpenGL.ForceClipping
#define ET_VALUE_DW_OPENGL_ERRORLOG                         0x20000335  // OpenGL.ErrorLog
#define ET_VALUE_DW_OPENGL_DUMPLOG                          0x20000336  // OpenGL.DumpLog
#define ET_VALUE_DW_OPENGL_MCAD_MODE                        0x20000337  // OpenGL.MCAD
#define ET_VALUE_DW_OPENGL_SUPPORT_P2_CI4TEXTURES           0x20000338  // OpenGL.SupportP2CI4Textures
#define ET_VALUE_DW_OPENGL_FORCE_SINGLEBUFFERED             0x20000339  // BOOLEAN
#define ET_VALUE_DW_OPENGL_ALIGNED_SURFACES                 0x2000033A  // ULONG
#define ET_VALUE_DW_OPENGL_FRAMEBUFFER_DMA                  0x2000033B  // OpenGL.FramebufferDMA
#define ET_VALUE_DW_OPENGL_AUTODESKEXTENSIONS               0x2000033C  // Bitfield enabling Autocad 2000 opengl extensions
#define ET_VALUE_DW_OPENGL_USESENDDATAESCAPE                0x2000033D  // ULONG (0:ICD accesses HW, 1:automatic (ICD or Escape), 2: ICD uses Escape to fire DMA), FNicklis 07.05.1999
#define ET_VALUE_DW_OPENGL_DISABLE_STENCIL                  0x2000033E  // BOOLEAN (0: default, export stencil, 1: remove stencil formats), FNicklis 11.06.199
#define ET_VALUE_DW_OPENGL_DISABLE_ALPHA                    0x2000033F  // BOOLEAN (0: default, export alpha,   1: remove alpha formats), FNicklis 11.06.199
#define ET_VALUE_DW_OPENGL_PREFER_AGP_TEXTURES              0x20000340  // BOOL ('OpenGL.PreferAGPTextures, 0=default)
#define ET_VALUE_DW_OPENGL_DMAMINPUSHCOUNT_SENDDATA         0x20000341  // TNT: OpenGL.DMAMinPushCountSendData (fill grade of buffer to force flush)
#define ET_VALUE_DW_OPENGL_MULTIBOARD                       0x20000342  // BOOLEAN (1:default, allow OpenGL; 0:OpenGl not in mulitboard) OpenGL.MultiBoard
#define ET_VALUE_DW_OPENGL_UNIGRAPHICS_TEXTURE_FIX          0x20000343  // Enable bug fix for Unigraphics 15, 16 (border color for texture)
#define ET_VALUE_DW_ICDINTERFACE_STEREOWAITCONCEPT          0x20000344  // ULONG  (0:default do not wait 1:allways use DPCs 2:do wait until VBlank occured
#define ET_VALUE_DW_ICDINTERFACE_STEREOWAITLINESBEFORE      0x20000345  // ULONG  ( default: 2 ) amount of rasterlines before VBlank until to loop and wait for VBlank 
#define ET_VALUE_DW_ICDINTERFACE_STEREOWAITLINESAFTER       0x20000346  // ULONG  ( default: 1 ) amount of rasterlines after VBlank when to trigger stereo glasses + offsets 

                                                          //0x20000347  ...

// Heidi/Kinetix.* 0x20000400-0x200004ff
#define ET_VALUE_DW_HEIDI_USEEXCLUSIVE                      0x20000400
#define ET_VALUE_DW_HEIDI_USELINEARTEXTUREFILTER            0x20000401
#define ET_VALUE_DW_KTX_BUFFERREGIONS                       0x20000402 // e.g. KTX.BufferRegions
#define ET_VALUE_DW_KTX_SWAPHINTS                           0x20000403 // e.g. KTX.SwapHints
#define ET_VALUE_DW_KTX_ELSABUFFERREGIONS                   0x20000404 // e.g. KTX.ElsaBufferRegions
#define ET_VALUE_DW_KTX_BUFFER_REGIONS_FULL                 0x20000405 // e.g. KTX.BufferRegionsFull
#define ET_VALUE_DW_KTX_BUFFER_REGIONS_FIX                  0x20000406 // e.g. KTX.BufferRegionsFix

// HardwareInformation.* 0x20000500-0x200005ff
#define ET_VALUE_SZ_HARDWAREINFORMATION_ADAPTERSTRING       0x20000500
#define ET_VALUE_SZ_HARDWAREINFORMATION_CHIPTYPE            0x20000501
#define ET_VALUE_SZ_HARDWAREINFORMATION_DACTYPE             0x20000502
#define ET_VALUE_DW_HARDWAREINFORMATION_MEMORYSIZE          0x20000503
#define ET_VALUE_SZ_BOARDNAME                               0x20000504
#define ET_VALUE_SZ_DESCRIPTION                             0x20000505
#define ET_VALUE_SZ_SERNUM                                  0x20000506
#define ET_VALUE_SZ_HARDWAREINFORMATION_BIOSSTRING          0x20000507

// DDraw ids    0x20000600-0x200006ff
#define ET_VALUE_DW_DDRAWMULTIBOARDPRIMARY                  0x20000600 // DirectDraw.MultiboardPrimary
#define ET_VALUE_DW_DDRAWENABLEVIDEOIO                      0x20000601 // EnableVideoIO
#define ET_VALUE_DW_DDRAWMULTIBOARDHW_ENABLE                0x20000602 // DirectDraw.MultiboardEnabled
#define ET_VALUE_DW_DDRAWSUPPORT_OVERLAYSHRINK              0x20000603 // DirectDraw.SupportOverlayshrink

// LCD Panel    0x20000700-0x200007ff
#define ET_VALUE_DW_LCDPANELBORDERLEFTRIGHT                 0x20000700 // DUO.PanningBorderLeftRight
#define ET_VALUE_DW_LCDPANELBORDERTOPBOTTOM                 0x20000701 // DUO.PanningBorderTopBottom
#define ET_VALUE_DW_LCDPANELSTEPLEFTRIGHT                   0x20000702 // DUO.PanningStepLeftRight
#define ET_VALUE_DW_LCDPANELSTEPTOPBOTTOM                   0x20000703 // DUO.PanningStepTopBottom
#define ET_VALUE_DW_LCDPANELPANSTARTXRES                    0x20000704 // DUO.PanningStartXResolution
#define ET_VALUE_DW_LCDPANELSTRETCHXRES                     0x20000705 // DUO.LcdExpand
#define ET_VALUE_DW_LCDPANELSINGLEMODE                      0x20000706 // DUO.SingleControllerMode
#define ET_VALUE_DW_LCDPANELENABLECRT                       0x20000707 // DUO.CrtEnable
#define ET_VALUE_DW_LCDPANELENABLELCD                       0x20000708 // DUO.LcdEnable
#define ET_VALUE_DW_LCDPANELENABLETV                        0x20000709 // DUO.TvEnable
#define ET_VALUE_DW_LCDPANELTRUECOLORDITHER                 0x2000070A // DUO.LcdTruecolorDither
#define ET_VALUE_DW_LCDPANELTVFLICKERFILTER                 0x2000070B // DUO.TvFlickerFilter

// ********************************************************
// ********************************************************
// NV10 / NV 15 boards
// -----------------------------
// #define SUBESC_ET_GET_OUTPUT_DEVICE_INFO    0x00000310
// #define SUBESC_ET_SET_OUTPUT_DEVICE         0x00000311

#define PANEL_RESOLUTION_STRETCHED	0x0001
#define PANEL_RESOLUTION_CENTERED	0x0002
#define PANEL_RESOLUTION_NATIVE	0x0004

#define DEVICE_TYPE_INVALID   0x0000
#define DEVICE_TYPE_CRT       0x0001
#define DEVICE_TYPE_LCD       0x0002
#define DEVICE_TYPE_TV        0x0004

typedef struct tagET_COMMON_OUTPUT_DEVICE_INFO
{
  DWORD   dwDeviceType;         // holds the type of the device (CRT/LCD/TV)
  DWORD   dwDeviceConnected;    // is this device connected to graphics board
  DWORD   dwDeviceOn;           // is the device switched on
  DWORD   dwHorSize;            // horizontal size of the device in pixel
  DWORD   dwVertSize;           // vertical size of the device in pixel
  DWORD   dwDisplayMode;        // display mode of device ( stretched, native, centered )
  DWORD   dwSupportedDisplayModes;   // supported display modes of device ( stretched, native, centered )
}ET_COMMON_OUTPUT_DEVICE_INFO, *PET_COMMON_OUTPUT_DEVICE_INFO;

typedef struct tagET_OUTPUT_DEVICE_INFO
{
  DWORD   dwSize;                   // size of structure
  DWORD   dwDevicesSupported;       // bitfield containing type of supported devices
  DWORD   dwSimultaniousDevices;      // bitfield containing type of devices supported at the same time
  ET_COMMON_OUTPUT_DEVICE_INFO  crt;  // information on the CRT
  ET_COMMON_OUTPUT_DEVICE_INFO  lcd;  // information on the LCD
  ET_COMMON_OUTPUT_DEVICE_INFO  tv;   // information on the tv set
  DWORD   dwNumAdditionalDevices;   // number of additional ET_COMMON_OUTPUT_DEVICE_INFO structures needed
  ET_COMMON_OUTPUT_DEVICE_INFO  *pAdditionalInformation;
} ET_OUTPUT_DEVICE_INFO, *PET_OUTPUT_DEVICE_INFO;

// Structure to set output device
typedef struct tagET_SET_OUTPUT_DEVICE
{
  ET_QUERY                    hdr;

  DWORD                       dwParam1;             //1 saves to registry, 0 for testing
  ET_OUTPUT_DEVICE_INFO       deviceInfo;
} ET_SET_OUTPUT_DEVICE, *PET_SET_OUTPUT_DEVICE;

//Structure to retrieve complete information on the output devices
typedef struct tagET_GET_OUTPUT_DEVICE_INFO
{
  DWORD                       dwSize;
  ET_OUTPUT_DEVICE_INFO       deviceInfo;
} ET_GET_OUTPUT_DEVICE_INFO,*PET_GET_OUTPUT_DEVICE_INFO;


// ********************************************************
// ********************************************************
// LCD panel with WINNER 1000DUO
// -----------------------------

// Common information on actual PANEL settings (used in Get and Set info)
typedef struct tagET_COMMON_PANEL_INFO
{
  LONG        lPanStepLR;           //Panning stepsize right/left panning
  LONG        lPanStepTB;           //Panning stepsize up/down panning
  LONG        lPanBorderLR;         //Area where panning begins before cursor hits panel end left/right
  LONG        lPanBorderTB;         //Area where panning begins before cursor hits panel end top/bottom
  LONG        lPanStartResX;        //We start panning if our Resolution exceeds this value
  LONG        lStretchEndXRes;      //Bitfield to describe the resolutions at which we want to stretch/expand
  DWORD       dwEnablePanel;        //We enable the LCD
  DWORD       dwEnableCrt;          //We enable the CRT
  DWORD       dwEnableTV;           //We enable the TV
  DWORD       dwPanelSingleMode;    //One Controller for both displays
  LONG        lTVFlickerFilter;     //1(disabled),2 or 3 line Flickerfilter
  DWORD       dwLCDTrueColorDither; //True Color dithering or reducing to 18bit
} ET_COMMON_PANEL_INFO, *PET_COMMON_PANEL_INFO;


// In structure (Tool -> driver):
// Structure to set the new Panel settings
typedef struct tagET_SET_PANEL_INFO
{
  ET_QUERY              hdr;

  DWORD                 dwParam1;             //1 saves to registry, 0 for testing
  ET_COMMON_PANEL_INFO  CommonPanelInfo;
} ET_SET_PANEL_INFO, *PET_SET_PANEL_INFO;


// Out structure (Driver -> Tool):
//Structure to retrieve complete information on the actual Panel
typedef struct tagET_GET_PANEL_INFO
{
  DWORD                 dwSize;

  DWORD                 dwPanelSupport;       //Does the Board support a LCD Panel
  LONG                  lXResPanel;           //X-Res of our Panel
  LONG                  lYResPanel;           //Y-Res of our Panel

  ET_COMMON_PANEL_INFO  CommonPanelInfo;

} ET_GET_PANEL_INFO, *PET_GET_PANEL_INFO;

// ********************************************************
// ********************************************************
//NEW: SK 13011999
//Interface for Maxtreme to enable/disable stereo for a given window

//In structure (Tool -> driver )
//Structure to set the Stereomode of a oglwindow. (for Maxtreme)
typedef struct tagET_SET_WINDOW_STEREOMODE
{
  ET_QUERY  hdr;

  PVOID     hwnd;   //Windowhandle // ELSA-FNicklis 27.04.00: not all of our code knows a HWND
  DWORD     dwMode; //STEREOMODE_ENABLE or STEREOMODE_DISABLE
} ET_SET_WINDOW_STEREOMODE, *PET_SET_WINDOW_STEREOMODE;

#define STEREOMODE_DISABLE  0x00000000
#define STEREOMODE_ENABLE   0x00000001

// ********************************************************


//
// NT direct hw access support - development only
//

// SUBESC_ET_GETREGISTER
typedef struct tagET_GETREG
{
  DWORD dwSize;
  DWORD dwRegister;
  DWORD dwValue;
  DWORD dwFlags;
}ET_GETREG, *PET_GETREG, FAR *LPET_GETREG;

// SUBESC_ET_SETREGISTER
typedef struct tagET_SETREG
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_SETREGISTER
  DWORD dwRegister;
  DWORD dwValue;
  DWORD dwFlags;
}ET_SETREG, *PET_SETREG, FAR *LPET_SETREG;

// SUBESC_ET_MODIFYREGISTER
typedef struct tagET_MODIFYREG
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_MODIFYREGISTER
  DWORD dwRegister;
  DWORD dwValue;
  DWORD dwFlags;
  DWORD dwMask; // (all bits to modify set to 1)
}ET_MODIFYREG, *PET_MODIFYREG, FAR *LPET_MODIFYREG;

//
// dwFlags values
//
#define MMIO_ACCESS             0x00 << 16  // default: mmio, sync + vblankwait
#define PIO_ACCESS              0x01 << 16
#define NO_SYNC                 0x02 << 16
#define NO_VBLANKWAIT           0x04 << 16
#define NO_FIFOWAIT             0x08 << 16

#define ACCESS_DWORD 0x01 << 0
#define ACCESS_WORD  0x02 << 0
#define ACCESS_BYTE  0x04 << 0



// SUBESC_ET_NOTIFYDRIVER
typedef struct tagET_NOTIFYDRIVER
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_NOTIFYDRIVER
  DWORD dwNotification;
}ET_NOTIFYDRIVER, *PET_NOTIFYDRIVER, FAR *LPET_NOTIFYDRIVER;

// notifications: video capture driver to display driver
#define EDDESC_VIO2DRVNOTIFY_TVOUTON   0x01
#define EDDESC_VIO2DRVNOTIFY_TVOUTOFF  0x02



// SUBESC_ET_SCROLL
// scrolling into offscreen (debug purposes)
typedef struct tagET_SCROLL_DATA
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_SCROLL
  LONG  cyLines;            // # of lines vertical panning
}ET_SCROLL_DATA, FAR *LPET_SCROLL_DATA;



// SUBESC_ET_GFXOPTIONQUERY
// GfxOptionQuery support
typedef struct tagET_GFXOPTIONQUERY
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_GFXOPTIONQUERY
  DWORD dwGfxOption;        // in/out parameter
}ET_GFXOPTIONQUERY_DATA, FAR *LPET_GFXOPTIONQUERY_DATA;

// SUBESC_ET_SAA
// video in/out related stuff, used by Windows NT video capture driver for hw access
typedef struct tagET_SAA_DATA
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_SAA
  DWORD dwSAACommand;       // = defines below
  DWORD dwData;
  DWORD dwRegister;
  DWORD dwReturn;
}ET_SAA_DATA, FAR *LPET_SAA_DATA;

#define I2COPEN                   1000
#define I2CCLOSE                  1001

#define CHECKSAA7111              1
#define WRITESAA7111              2
#define READSAA7111               3

#define CHECKSAA7125              101
#define WRITESAA7125              102
#define READSAA7125               103

// ********************************************************
// code for SUBESC_ET_QUERY_ICDCLIENTINFO:
// 12.02.1999 FNicklis

// input structure app -> driver
typedef struct tagET_QUERY_ICDCLIENTINFO_IN
{
  ET_QUERY hdr;  // size and subescape ET_QUERY_ICDCLIENTINFO
} ET_QUERY_ICDCLIENTINFO_IN;

typedef struct tagET_ICDCLIENTINFO
{
  DWORD  dwClientID;     // 1 per window
  DWORD  dwContextCount; // n per window > 0; 0==invalid
  DWORD  dwProcessID;    // 0==invalid/unused/unkown
  HANDLE hWnd;           // window handle
  LONG   lPixelFormat;   // used pixelformat number
} ET_ICDCLIENTINFO;

// output structure driver -> app
typedef struct tagET_QUERY_ICDCLIENTINFO_OUT
{
  DWORD  dwSize;          // Length of this out buffer
  DWORD  dwTotalClients;  // Total count of running OpenGL clients
  DWORD  dwReturnClients; // Count of information units returned to app

  ET_ICDCLIENTINFO aClientInfo[1]; // array of 1, ...
} ET_QUERY_ICDCLIENTINFO_OUT;


// ********************************************************
// code for SUBESC_ET_QUERY_TEMPERATURE:
// 28.09.1999 Thorsten Schumann

// bits used as caps and as status
#define ET_TEMPERATURE_FANSLOW     0x00000001   // fan slow or stop
#define ET_TEMPERATURE_FANSHORTVCC 0x00000002   // fan short VCC (only on boot phase available)
#define ET_TEMPERATURE_OVERTEMP    0x00000004   // over temperature
#define ET_TEMPERATURE_VALUE       0x00000008   // for future release, not yet used
                                                //    entry dwTemperatureValue is used

// output structure driver -> app
typedef struct tagET_QUERY_TEMPERATURE
{
  DWORD  dwSize;              // Length of this out buffer
  DWORD  dwTemperatureCaps;    // caps of valid record entries / hardware caps
  DWORD  dwTemperatureStatus; // status of flags
  DWORD  dwTemperatureValue;  // for future release, not yet used
  DWORD  dwReserved1;
  DWORD  dwReserved2;
} ET_QUERY_TEMPERATURE, FAR *LPET_QUERY_TEMPERATURE;


// ********************************************************
// begin of code for SUBESC_ET_DPMS:
//
// Set or query DPMS power save mode. Send ET_DPMS_IN structure
// with wanted DPMS mode to driver and always receive selected
// mode. If Escape fails, the DPMS mode was not set!
//
// FNicklis 29.09.1999: added SUBESC_ET_DPMS and sample code
//
// Sample code to set powersave mode:
//   taken from PowerSaver.scr Screen saver
//
  /*
//
// bSetDPMSMode
//
// enables ELSAESC-ET_DPMS power save mode via Escape call to driver.
//
// return: TRUE  - mode set successfully
//         FALSE - failed to set powersave mode
//
// FNicklis 29.09.1999: New
//
static BOOL bSetDPMSMode(
  IN ET_DPMS_MODE lMode)      // Power save mode to set
{
  BOOL        bRet=FALSE;     // pessimistic
  LONG        lResult;        // return value of extescape
  ET_DPMS_IN  etPowersaveIn;  // App -> Driver
  ET_DPMS_OUT etPowersaveOut; // Driver -> App
  HDC         hDC;            // Desktops HDC (to reach the display driver)

  ZeroMemory(&etPowersaveIn, sizeof(etPowersaveIn));

  etPowersaveIn.hdr.dwSize    = sizeof(etPowersaveIn);
  etPowersaveIn.hdr.dwSubEsc  = SUBESC_ET_DPMS;
  etPowersaveIn.hdr.dwOutSize = sizeof(etPowersaveOut);
  etPowersaveIn.hdr.dwBoard   = 0;
  etPowersaveIn.lMode         = lMode;

  hDC = GetDC(0);

  lResult = ExtEscape(hDC,
    ESC_ELSA_TOOLS,
    sizeof(etPowersaveIn),
    (PVOID)&etPowersaveIn,
    sizeof(etPowersaveOut),
    (PVOID)&etPowersaveOut);

  if (lResult>0)
  {
    assert(sizeof(etPowersaveOut)==etPowersaveOut.dwSize);
    assert((lMode==ET_DPMS_QUERY) || (etPowersaveIn.lMode==etPowersaveOut.lMode));
    bRet=TRUE;
  }
  else
  {
    // error: failed for call SUBESC_ET_DPMS");
  }

  return(bRet);
}
  */

// defines for DPMS-Power save modes
typedef enum tagET_DPMS_MODE
{
  ET_DPMS_ON,      // in/out: default mode, turn on monitor
                // Powersave modes:
  ET_DPMS_STANDBY, // in/out:
  ET_DPMS_SUSPEND, // in/out:
  ET_DPMS_OFF,     // in/out:

  ET_DPMS_QUERY    // in: return current mode in ET_DPMS_OUT. Only used in ET_DPMS_IN.ulMode!
} ET_DPMS_MODE;

// input structure app -> driver
typedef struct tagET_DPMS_IN
{
  ET_QUERY     hdr;    // size and subescape ET_DPMS_IN
  ET_DPMS_MODE lMode;  // type of mode to set or query
} ET_DPMS_IN;

// output structure driver -> app
typedef struct tagET_DPMS_OUT
{
  DWORD        dwSize; // Length of this out buffer
  ET_DPMS_MODE lMode;  // return current mode which is POWERSAVE_DISABLE or POWERSAVE_ENABLE
} ET_DPMS_OUT;

// end of code for SUBESC_ET_DPMS:
// ********************************************************

#ifdef WIN32
#pragma pack (pop, fixed_forELSAESC)
#else
#pragma pack ()
#endif

#endif //__ELSAESC


