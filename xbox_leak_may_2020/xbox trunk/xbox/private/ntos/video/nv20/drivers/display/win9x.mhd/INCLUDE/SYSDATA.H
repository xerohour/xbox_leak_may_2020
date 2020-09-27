/*=============================================================================

    Copyright (C) 1997, NVidia Coporation

    File:       sysdata.h

    Purpose:    This file holds structures and equates dealing with the
                pdevice.

=============================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
typedef struct _tagDIBHEADER
{
    ULONG       biSize;
    ULONG       biWidth;
    ULONG       biHeight;
    USHORT      biPlanes;
    USHORT      biBitCount;
    ULONG       biCompression;
    ULONG       biSizeImage;
    ULONG       biXPelsPerMeter;
    ULONG       biYPelsPerMeter;
    ULONG       biClrUsed;
    ULONG       biClrImportant;
}   DIBHEADER;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// This structure is similar to what is defined in dibeng.inc.
// The differences are we use the reserved field for a few things.
// Hence this is essentially the DIBENGINE structure with
// different names for the modified fields.
typedef struct  _tagDIBENGINEHDR
{
    USHORT      deType;
    USHORT      deWidth;
    USHORT      deHeight;
    USHORT      deWidthBytes;
    UCHAR       dePlanes;
    UCHAR       deBitsPixel;
    ULONG       deReserved1;            // cannot be used. (bug #32933)
    ULONG       deDeltaScan;
    ULONG       delpPDevice;
    ULONG       deBitsOffset;
    USHORT      deBitsSelector;
    USHORT      deFlags;
    USHORT      deVersion;
    ULONG       deBitmapInfo;
    ULONG       deBeginAccess;
    ULONG       deEndAccess;

    // We're using this field to store the three fields that follow. In
    // multi-mon mode, we do not get the opportunity to create the bitmap
    // header so we can not increase the size.
//    ULONG       deDriverReserved;

    // This field should be filled in with the same value as is in the
    // deBitsPixel field except when the display driver is running
    // 15bpp. In this case, the deBitsPixel field will have 16 in it
    // and the bRealBpp field will have 15 in it.
    UCHAR       bRealBpp;

    // Reserved fields
    UCHAR       bTemp1;
    USHORT      wTemp2;
}   DIBENGINEHDR;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// This structure is passed into ValidateMode
typedef struct  _tagMYDISPVALMODE
{
    USHORT  dvmSize;
    USHORT  dvmBpp;
    USHORT  dvmXRes;
    USHORT  dvmYRes;
}   MYDISPVALMODE;

#define MYVALMODE_YES           0
#define MYVALMODE_NO_WRONGDRV   1
#define MYVALMODE_NO_NOMEM      2
#define MYVALMODE_NO_NODAC      3
#define MYVALMODE_NO_UNKNOWN    4
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// This structure is returned by a call to VddGetDisplayInfo
typedef struct _tagDISPLAYINFO
{
    USHORT  diHdrSize;
    USHORT  diInfoFlags;

    ULONG   diDevNodeHandle;
    UCHAR   diDriverName[16];
    USHORT  diXRes;
    USHORT  diYRes;
    USHORT  diDPI;
    UCHAR   diPlanes;
    UCHAR   diBpp;
    USHORT  diRefreshRateMax;
    USHORT  diRefreshRateMin;
    USHORT  diLowHorz;
    USHORT  diHighHorz;
    USHORT  diLowVert;
    USHORT  diHighVert;
    ULONG   diMonitorDevNodeHandle;
    UCHAR   diHorzSyncPolarity;
    UCHAR   diVertSyncPolarity;

    // Only for Windows 98
    ULONG   diUnitNumber;             // device unit number
    ULONG   diDisplayFlags;           // mode specific flags
    ULONG   diXDesktopPos;            // position of desktop
    ULONG   diYDesktopPos;            // ...
    ULONG   diXDesktopSize;           // size of desktop (for panning)
    ULONG   diYDesktopSize;           // ...

    USHORT  wJimKellerRoundToDwordSize; // Solely to make the structure
                                        // size a multiple of a dword.
}   MYDISPLAYINFO;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// These are some DirectDraw Init structures that get passed into the
// display driver.
typedef struct  _tagMYDDHALDDRAWFNS
{
    ULONG       dwSize;
    ULONG       lpSetInfo;
    ULONG       lpVidMemAlloc;
    ULONG       lpVidMemFree;
}   MYDDHALDDRAWFNS;

typedef struct _MYDCICMD
{
    ULONG   dwCommand;
    ULONG   dwParam1;
    ULONG   dwParam2;
    ULONG   dwVersion;
    ULONG   dwReserved;
} MYDCICMD;

typedef struct
{
    UCHAR   szName[260];            // 32-bit driver name
    UCHAR   szEntryPoint[64];       // entry point
    ULONG   dwContext;              // context to pass to entry point
} MYDD32BITDRIVERDATA;

typedef struct
{
    ULONG   dwHALVersion;           // Version of DirectDraw for which the
                                    //   HAL was created
    ULONG   dwReserved1;            // Reserved for future use
    ULONG   dwReserved2;            // Reserved for future use
} MYDDVERSIONDATA;

#define DDNVCREATEDRIVEROBJECT  98
#define DDNVDESTROYDRIVEROBJECT 99
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// A few others things
typedef struct  _MYRECTL
{
    ULONG       dwLeft;
    ULONG       dwTop;
    ULONG       dwRight;
    ULONG       dwBottom;
}   MYRECTL;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


///////////////////////////////////////////////////////////////////////
// Cursor information
#define CURSOR_SIZE     0x20        // 32x32 cursors
#define CURSOR_WIDTH    CURSOR_SIZE
#define CURSOR_HEIGHT   CURSOR_SIZE
#define AND_MASK_SIZE   ((CURSOR_WIDTH * CURSOR_HEIGHT) / 8)

// Not a true structure since bCursorXorMask is actually a variable
// length array, but this is good enough.
typedef struct  _tagCURSORDESC
{
    USHORT      wHotX;
    USHORT      wHotY;
    USHORT      wWidth;
    USHORT      wHeight;
    USHORT      wWidthBytes;
    UCHAR       bPlanes;
    UCHAR       bBitsPerPixel;
    UCHAR       bCursorAndMask[AND_MASK_SIZE];
    UCHAR       bCursorXorMask;
}   CURSORDESC;
///////////////////////////////////////////////////////////////////////

// Not a true structure since bCursorXorMask is actually a variable
// length array, but this is good enough.
typedef struct  _tagCURSORHEADER
{
    USHORT      wHotX;
    USHORT      wHotY;
    USHORT      wWidth;
    USHORT      wHeight;
    USHORT      wWidthBytes;
    UCHAR       bPlanes;
    UCHAR       bBitsPerPixel;
}   CURSORHEADER;
///////////////////////////////////////////////////////////////////////

typedef struct  _tagMODULELINK
{
    ULONG       lpfnFunc;
    USHORT      wOrdinal;
}   MODULELINK;
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Since we don't know whether we will be running on a wide OS or not,
// we will need to define both.
typedef struct _OSVERSIONINFOW
{
    ULONG   dwOSVersionInfoSize;
    ULONG   dwMajorVersion;
    ULONG   dwMinorVersion;
    ULONG   dwBuildNumber;
    ULONG   dwPlatformId;
    USHORT  szCSDVersion[128];
}   OSVERSIONINFOW;

typedef struct _OSVERSIONINFOA
{
    ULONG   dwOSVersionInfoSize;
    ULONG   dwMajorVersion;
    ULONG   dwMinorVersion;
    ULONG   dwBuildNumber;
    ULONG   dwPlatformId;
    UCHAR   szCSDVersion[128];
}   OSVERSIONINFOA;
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
#define MAX_ATTACHED_CRTCS      2
///////////////////////////////////////////////////////////////////////

