/*
    FILE:   edid.h
    DATE:   4/8/99

    This file contains the includes for the EDID code.
    Os Independent
*/

#ifdef WINNT
// This disables any packing optimizations of the compiler.
// We achieve this by telling the compiler to pack on a 1-byte boundary.
// Note: There must be a corresponding pragma call at the end of this file to
// restore the default compiler packing (specified by /Zp).
#pragma pack(1)
#endif

struct  _tagDETAILEDTIMINGDESCRIPTOR
{
    USHORT  wDTPixelClock;              // 0x00
    UCHAR   bDTHorizontalActive;        // 0x02
    UCHAR   bDTHorizontalBlanking;      // 0x03
    UCHAR   bDTHorizActiveBlank;        // 0x04
    UCHAR   bDTVerticalActive;          // 0x05
    UCHAR   bDTVerticalBlanking;        // 0x06
    UCHAR   bDTVertActiveBlank;         // 0x07
    UCHAR   bDTHorizontalSync;          // 0x08
    UCHAR   bDTHorizontalSyncWidth;     // 0x09
    UCHAR   bDTVerticalSync;            // 0x0A
    UCHAR   bDTHorizVertSyncOverFlow;   // 0x0B
    UCHAR   bDTHorizontalImage;         // 0x0C
    UCHAR   bDTVerticalImage;           // 0x0D
    UCHAR   bDTHorizVertImage;          // 0x0E
    UCHAR   bDTHorizontalBorder;        // 0x0F
    UCHAR   bDTVerticalBorder;          // 0x10
    UCHAR   bDTFlags;                   // 0x11
} __STRUCTURE_PACKING__;

typedef struct _tagDETAILEDTIMINGDESCRIPTOR DETAILEDTIMINGDESCRIPTOR;
typedef DETAILEDTIMINGDESCRIPTOR FAR *LPDETAILEDTIMINGDESCRIPTOR;


struct  _tagMONITORDESCRITOR
{
    USHORT  wZeroFlag;                  // 0x00
    UCHAR   bZeroFlag;                  // 0x02
    UCHAR   bTypeDescriptorBlock;       // 0x03
    UCHAR   bZeroFlag2;                 // 0x04
    UCHAR   bMinVertRateInHz;           // 0x05
    UCHAR   bMaxVertRateInHz;           // 0x06
    UCHAR   bMinHorzRateInKHz;          // 0x07
    UCHAR   bMaxHorzRateInKHz;          // 0x08
    UCHAR   bMaxPixelClockInCKHz;       // 0x09
    UCHAR   bUnused[8];                 // 0x0A
} __STRUCTURE_PACKING__;

typedef struct _tagMONITORDESCRITOR MONITORDESCRITOR;
typedef MONITORDESCRITOR FAR *LPMONITORDESCRITOR;

// Note: The bMaxPixelClockInCKHz is in centi-kilohertz which is
// Megahertz divided by 10. This field may not be specified in which
// case it is set to 0x0FF.

// These are the values that can be in the bTypeDescriptorBlock field.
// Note: 0x10-0xF9 are currently undefined. 0x00-0x09 are monitor-
// manufacturer specific.
#define DT_MONITOR_SERIAL_NUMBER        0x0FF
#define DT_ASCII_STRING_DATA            0x0FE
#define DT_MONITOR_RANGE_LIMITS         0x0FD
#define DT_MONITOR_NAME_AS_ASCII        0x0FC
#define DT_EXTRA_COLOR_POINT_DATA       0x0FB
#define DT_EXTRA_STANDARD_TIMINGS       0x0FA


struct  _tagEDIDV1STRUC
{
    UCHAR   bHeaderByte0;               // 0x00
    UCHAR   bHeaderByte1;               // 0x01
    UCHAR   bHeaderByte2;               // 0x02
    UCHAR   bHeaderByte3;               // 0x03
    UCHAR   bHeaderByte4;               // 0x04
    UCHAR   bHeaderByte5;               // 0x05
    UCHAR   bHeaderByte6;               // 0x06
    UCHAR   bHeaderByte7;               // 0x07
    USHORT  wIDManufName;               // 0x08
    USHORT  wIDProductCode;             // 0x0A
    ULONG   wIDSerialNumber;            // 0x0C
    UCHAR   bWeekManuf;                 // 0x10
    UCHAR   bYearManuf;                 // 0x11
    UCHAR   bVersionNumber;             // 0x12
    UCHAR   bRevisionNumber;            // 0x13
    UCHAR   bVideoInputDef;             // 0x14
    UCHAR   bMaxHorizImageSize;         // 0x15
    UCHAR   bMaxVertImageSize;          // 0x16
    UCHAR   bDisplayXferChar;           // 0x17
    UCHAR   bFeatureSupport;            // 0x18
    UCHAR   bRedGreenLowBits;           // 0x19
    UCHAR   bBlueWhiteLowBits;          // 0x1A
    UCHAR   bRedX;                      // 0x1B
    UCHAR   bRedY;                      // 0x1C
    UCHAR   bGreenX;                    // 0x1D
    UCHAR   bGreenY;                    // 0x1E
    UCHAR   bBlueX;                     // 0x1F
    UCHAR   bBlueY;                     // 0x20
    UCHAR   bWhiteX;                    // 0x21
    UCHAR   bWhiteY;                    // 0x22
    UCHAR   bEstablishedTimings1;       // 0x23
    UCHAR   bEstablishedTimings2;       // 0x24
    UCHAR   bManufReservedTimings;      // 0x25
    USHORT  wStandardTimingID[8];       // 0x26
    DETAILEDTIMINGDESCRIPTOR    DetailedTimingDesc[4];  // 0x36
    UCHAR   bExtensionFlag;             // 0x7E
    UCHAR   bChecksum;                  // 0x7F
} __STRUCTURE_PACKING__;

typedef struct _tagEDIDV1STRUC EDIDV1STRUC;
typedef EDIDV1STRUC FAR *LPEDIDV1STRUC;


struct  _tagEDIDV2STRUC
{
    UCHAR   bHeader;                        // 0x00
    USHORT  wIDManufName;                   // 0x01
    USHORT  wIDProductCode;                 // 0x03
    UCHAR   bWeekManuf;                     // 0x05
    USHORT  wYearManuf;                     // 0x06
    UCHAR   bProductIDString[32];           // 0x08
    UCHAR   bSerialNumber[16];              // 0x28
    UCHAR   bReserved1[8];                  // 0x38
    UCHAR   bPhysicalInterfaceType;         // 0x40
    UCHAR   bVideoInterfaceType;            // 0x41
    UCHAR   bInterfaceDataFormat[8];        // 0x42
    UCHAR   bInterfaceColor[5];             // 0x4A
    UCHAR   bDisplayTechType;               // 0x4F
    UCHAR   bMajorDisplayChar;              // 0x50
    UCHAR   bFeaturesSupported[3];          // 0x51
    USHORT  wDisplayResponseTime;           // 0x54
    ULONG   dwDisplayXferChar;              // 0x56
    ULONG   dwMaxLuminance;                 // 0x5A
    UCHAR   bColorimetry[20];               // 0x5E
    ULONG   dwMaxImageSize;                 // 0x72
    ULONG   dwMaxAddressability;            // 0x76
    USHORT  wDotPixelPitch;                 // 0x7A
    UCHAR   bReserved2;                     // 0x7C
    UCHAR   bGTFSupportInfo;                // 0x7D
    USHORT  wTimingInfoMap;                 // 0x7E
    UCHAR   bTableDescriptors[127];         // 0x80
    UCHAR   bChecksum;                      // 0xFF
} __STRUCTURE_PACKING__;

typedef struct _tagEDIDV2STRUC EDIDV2STRUC;
typedef EDIDV2STRUC FAR *LPEDIDV2STRUC;


struct  _tagTIMINGCODES
{
    ULONG   dwNumLuminanceTables;
    ULONG   dwSizeLuminanceTables;
    ULONG   dwNumFreqRanges;
    ULONG   dwNumRangeLimits;
    ULONG   dwNumTimingCodes;
    ULONG   dwNumDetailedTimings;
} __STRUCTURE_PACKING__;

typedef struct _tagTIMINGCODES TIMINGCODES;
typedef TIMINGCODES FAR *LPTIMINGCODES;


struct  _tagDISPLAYTIMINGRANGE
{
    UCHAR   bHiMinVert;
    UCHAR   bHiMaxVert;
    UCHAR   bHiMinHorz;
    UCHAR   bHiMaxHorz;
    UCHAR   bLoHV;
    UCHAR   bLoMinPixClock;
    UCHAR   bLoMaxPixClock;
    UCHAR   bHiMinMaxPixClock;
}  __STRUCTURE_PACKING__;

typedef struct _tagDISPLAYTIMINGRANGE DISPLAYTIMINGRANGE;
typedef DISPLAYTIMINGRANGE FAR *LPDISPLAYTIMINGRANGE;


struct  _tagDETAILEDTIMINGRANGE
{
    USHORT  wMinPixelClock;
    UCHAR   bLowMinHBlank;
    UCHAR   bLowMinVBlank;
    UCHAR   bHiMinHVBlank;
    UCHAR   bMinHSyncOffsetFromBlankStart;
    UCHAR   bLowMinHSyncPulseWidth;
    UCHAR   bLowMinVSyncPulseOffsetAndWidth;
    UCHAR   bHiMinSyncPulseOffsetsAndWidths;
    USHORT  wMaxPixelClock;
    UCHAR   bLowMaxHBlank;
    UCHAR   bLowMaxVBlank;
    UCHAR   bHiMaxHVBlank;
    UCHAR   bMaxHSyncOffset;
    UCHAR   bLowMaxHSyncPulseWidth;
    UCHAR   bLowMaxVSyncPulseOffsetAndWidth;
    UCHAR   bHiMaxSyncPulseOffsetsAndWidths;
    UCHAR   bLowHImageSizeInMM;
    UCHAR   bLowVImageSizeInMM;
    UCHAR   bHiHVImageSizeInMM;
    UCHAR   bLowHActive;
    UCHAR   bLowVActive;
    UCHAR   bHiHVActive;
    UCHAR   bHBorder;
    UCHAR   bVBorder;
    UCHAR   bFlags;
}   __STRUCTURE_PACKING__;

typedef struct _tagDETAILEDTIMINGRANGE DETAILEDTIMINGRANGE;
typedef DETAILEDTIMINGRANGE FAR *LPDETAILEDTIMINGRANGE;


extern int CFUNC EdidFindDetailedMode (LPCHAR lpEdidBuffer, ULONG dwEdidSize,
                            LPMODEINFO lpModeDesc, LPMODEOUT lpModeOut);
extern int CFUNC EdidFindStandardMode (LPCHAR lpEdidBuffer, ULONG dwEdidSize,
                            LPMODEINFO lpModeDesc, LPMODEOUT lpModeOut);
extern int CFUNC EdidFindEstablishedMode (LPCHAR lpEdidBuffer,
                            ULONG dwEdidSize, LPMODEINFO lpModeDesc,
                            LPMODEOUT lpModeOut);
extern int CFUNC EdidIsGtfMonitor (LPCHAR lpEdidBuffer, ULONG dwEdidSize);
// This routine returns TRUE if the Monitor is a GTF monitor and
// FALSE, otherwise. Remember that the DOCS said that technically,
// a monitor is not fully GTF compliant unless in addition to the
// GTF bit, there was also a MONITORRANGELIMITS structure.

extern int CFUNC EdidGetMonitorLimits (LPCHAR lpEdidBuffer,
                            ULONG dwEdidSize, LPMONITORLIMITS lpML);
// This routine gets a MONITORLIMITS structure out of the EDID if
// the EDID is the correct version and a MONITORLIMITS structure
// is present (the structure is optional in v 2.x). If one is
// obtained the function returns TRUE; else it returns FALSE.

extern int CFUNC EdidFindSameModeOrLargerWithHighestRR (LPCHAR lpEdidBuffer,
                    ULONG dwEdidSize, LPMODEINFO lpModeDesc);
// EdidFindModeHighestRR should find a the highest refresh rate in
// the detailed, established, or standard timing sections that has a
// resolution greater than or equal to the dwXRes and dwYRes in
// lpModeDesc. The refresh rate for that mode is returned. If there
// is no matching xres, yres, then 0 is returned.

extern int CFUNC EdidFindHighestRRinTable (LPMODESTRUC lpMS,
                    LPMODEINFO  lpModeDesc);
// EdidFindHighestRRinTable should return the highest refresh rate
// in the table passed in which corresponds to a resolution which is
// greater than or equal to the requested mode.

extern int CFUNC EdidFindLargestEdidMode (LPCHAR lpEdidBuffer,
                    ULONG dwEdidSize, LPMODEINFO lpModeInfo);
// EdidFindLargestEdidMode should return the largest XRes and YRes
// that can be found in the EDID.

extern int CFUNC EdidFindLargestModeinTable (LPMODESTRUC lpMS,
                    LPMODEINFO lpModeInfo);
// EdidFindLargestModeinTable should return the largest XRes and YRes
// that can be found in the table.

extern int CFUNC EdidCmpToLargestEdidMode (LPCHAR lpEdidBuffer,
                    ULONG dwEdidSize, LPMODEINFO lpModeDesc);
// The return value can be a logical OR of one of the first three
// flags with one of the second three flags with the last flag.
#define REQUESTED_MODE_SMALLER_XYRES_THAN_LARGEST_EDID_MODE 0x01
#define REQUESTED_MODE_SAME_XYRES_AS_LARGEST_EDID_MODE      0x02
#define REQUESTED_MODE_LARGER_XYRES_THAN_LARGEST_EDID_MODE  0x04

#define REQUESTED_MODE_SMALLER_PXCLK_THAN_LARGEST_EDID_MODE 0x08
#define REQUESTED_MODE_SAME_PXCLK_AS_LARGEST_EDID_MODE      0x10
#define REQUESTED_MODE_LARGER_PXCLK_THAN_LARGEST_EDID_MODE  0x20

#define REQUESTED_MODE_XYRES_EXACTLY_MATCHES_AN_EDID_MODE   0x40

extern int CFUNC EdidCheckIfModeIsSmaller (LPMODESTRUC lpMS,
                    LPMODEINFO  lpModeDesc);
// EdidCheckIfModeIsSmaller should return TRUE if an lpMS timing
// mode exists which is greater than or equal to the requested mode.

extern int CFUNC EdidFindTimingCodesHighestRR (LPCHAR lpEdidBuffer,
                        ULONG dwEdidSize, LPMODEINFO lpModeDesc,
                        LPMODEOUT lpModeOut);
// EdidTimingCodesHighestRR should find a mode in the timing codes
// section (only in version 2.X of the EDID) that matches the dwXRes
// and dwYRes in lpModeDesc and has the highest refresh rate. That
// mode should be returned in lpModeOut.

extern int CFUNC EdidGetVersion (LPCHAR lpEdidBuffer, ULONG dwEdidSize);
// This routine should return the EDID version multiplied by 0x100.
// So, for example, version 1.10 should be returned as 0x110.

extern ULONG CFUNC EdidGetMonitorId (LPCHAR lpEdidBuffer, ULONG dwEdidSize);
// This routine should return the monitorID from the EDID. This is
// defined as a DWORD from high byte to low byte as follows:
// high byte of product code, low byte of product code, high byte of
// manufacturer code, low byte of manufacturer code. ASsemble these
// four bytes into a ULONG and return. If there is any kind of error
// then return XXXX_ID which is defined in restime.h  It means any
// monitor for which an EDID is not defined. Since you can't parse
// the EDID for some reason, this is a reasonable return value.

extern ULONG CFUNC EdidFindLargestPixelClock (LPCHAR lpEdidBuffer, ULONG dwEdidSize);
// Finds the EDID mode with the largest pixel clock and returns
// the pixel clock.

extern int CFUNC EdidFindExactModeWithHighestRR (LPCHAR lpEdidBuffer,
                        ULONG dwEdidSize, LPMODEINFO lpModeDesc);
// This routine searches for the highest refresh rate in the detailed,
// established, or standard timing sections that has a resolution exactly
// equal to the dwXRes and dwYRes in lpModeDesc. The refresh rate for that
// mode is returned. If there is no matching Xres, Yres, then 0 is returned.

#ifdef WINNT
// Restore the compiler's default specified packing mode (specified by /Zp).
// This directive needs the corresponding directive in the beginning of this file.
#pragma pack()
#endif
