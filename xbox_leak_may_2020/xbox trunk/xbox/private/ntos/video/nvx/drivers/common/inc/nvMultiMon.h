 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#ifndef _nvMultiMon_h
#define _nvMultiMon_h

//
// This file contains constants shared between the display and miniport drivers for NT40 and Win2K.
// These are used for the MultiMon modes for NV11 dual head cards.
// This file also contains the TwinView definitions (previously in win9x\inc\escape.h) used by Win9x driver and 
// the NT40/Win2K drivers.
//

#ifdef __cplusplus
extern "C"
{
#endif

// maximum number of heads this driver supports
#define NV_MAX_HEADS   2

// Uncomment this line to enable the new API for assigning devices to heads.
// #define ENABLE_HEAD_API
// Some special bitmask definitions for the new head API of resman.
#define BITMASK_CRT0    0x1
#define BITMASK_INVALID_DEVICE 0x0
#define BITMASK_ALL_CRT 0xff
#define BITMASK_ALL_TV 0xff00
#define BITMASK_ALL_DFP 0xff0000

    
//
// The table entries were taken from the GTF table in the miniport in vesadata.c
// Returns true if the resolution specified by (modeWidth, modeHeight) is a horizontally doubled resolution.
//
#define HORIZONTAL_MODE(modeWidth, modeHeight) \
        (\
            (modeWidth == 2048 * 2 && modeHeight == 1536 * 1) ||\
            (modeWidth == 1920 * 2 && modeHeight == 1440 * 1) ||\
            (modeWidth == 1920 * 2 && modeHeight == 1200 * 1) ||\
            (modeWidth == 1920 * 2 && modeHeight == 1080 * 1) ||\
            (modeWidth == 1856 * 2 && modeHeight == 1392 * 1) ||\
            (modeWidth == 1800 * 2 && modeHeight == 1440 * 1) ||\
            (modeWidth == 1792 * 2 && modeHeight == 1344 * 1) ||\
            (modeWidth == 1600 * 2 && modeHeight == 1200 * 1) ||\
            (modeWidth == 1600 * 2 && modeHeight == 1024 * 1) ||\
            (modeWidth == 1600 * 2 && modeHeight == 900 * 1) ||\
            (modeWidth == 1600 * 2 && modeHeight == 880 * 1) ||\
            (modeWidth == 1280 * 2 && modeHeight == 1024 * 1) ||\
            (modeWidth == 1280 * 2 && modeHeight == 960 * 1) ||\
            (modeWidth == 1152 * 2 && modeHeight == 864 * 1) ||\
            (modeWidth == 1024 * 2 && modeHeight == 768 * 1) ||\
            (modeWidth == 960 * 2 && modeHeight == 720 * 1) ||\
            (modeWidth == 864 * 2 && modeHeight == 480 * 1) ||\
            (modeWidth == 856 * 2 && modeHeight == 480 * 1) ||\
            (modeWidth == 852 * 2 && modeHeight == 480 * 1) ||\
            (modeWidth == 848 * 2 && modeHeight == 480 * 1) ||\
            (modeWidth == 800 * 2 && modeHeight == 600 * 1) ||\
            (modeWidth == 720 * 2 && modeHeight == 480 * 1) ||\
            (modeWidth == 640 * 2 && modeHeight == 480 * 1) ||\
            (modeWidth == 640 * 2 && modeHeight == 400 * 1) ||\
            (modeWidth == 512 * 2 && modeHeight == 384 * 1) ||\
            (modeWidth == 480 * 2 && modeHeight == 360 * 1) ||\
            (modeWidth == 400 * 2 && modeHeight == 300 * 1) ||\
            (modeWidth == 320 * 2 && modeHeight == 240 * 1) ||\
            (modeWidth == 320 * 2 && modeHeight == 200 * 1)\
        )

//
// The table entries were taken from the GTF table in the miniport in vesata.c
// Returns true if the resolution specified by (modeWidth, modeHeight) is a vertically doubled resolution.
//
#define VERTICAL_MODE(modeWidth, modeHeight) \
        (\
            (modeWidth == 2048 * 1 && modeHeight == 1536 * 2) ||\
            (modeWidth == 1920 * 1 && modeHeight == 1440 * 2) ||\
            (modeWidth == 1920 * 1 && modeHeight == 1200 * 2) ||\
            (modeWidth == 1920 * 1 && modeHeight == 1080 * 2) ||\
            (modeWidth == 1856 * 1 && modeHeight == 1392 * 2) ||\
            (modeWidth == 1800 * 1 && modeHeight == 1440 * 2) ||\
            (modeWidth == 1792 * 1 && modeHeight == 1344 * 2) ||\
            (modeWidth == 1600 * 1 && modeHeight == 1200 * 2) ||\
            (modeWidth == 1600 * 1 && modeHeight == 1024 * 2) ||\
            (modeWidth == 1600 * 1 && modeHeight == 900 * 2) ||\
            (modeWidth == 1600 * 1 && modeHeight == 880 * 2) ||\
            (modeWidth == 1280 * 1 && modeHeight == 1024 * 2) ||\
            (modeWidth == 1280 * 1 && modeHeight == 960 * 2) ||\
            (modeWidth == 1152 * 1 && modeHeight == 864 * 2) ||\
            (modeWidth == 1024 * 1 && modeHeight == 768 * 2) ||\
            (modeWidth == 960 * 1 && modeHeight == 720 * 2) ||\
            (modeWidth == 864 * 1 && modeHeight == 480 * 2) ||\
            (modeWidth == 856 * 1 && modeHeight == 480 * 2) ||\
            (modeWidth == 852 * 1 && modeHeight == 480 * 2) ||\
            (modeWidth == 848 * 1 && modeHeight == 480 * 2) ||\
            (modeWidth == 800 * 1 && modeHeight == 600 * 2) ||\
            (modeWidth == 720 * 1 && modeHeight == 480 * 2) ||\
            (modeWidth == 640 * 1 && modeHeight == 480 * 2) ||\
            (modeWidth == 640 * 1 && modeHeight == 400 * 2) ||\
            (modeWidth == 512 * 1 && modeHeight == 384 * 2) ||\
            (modeWidth == 480 * 1 && modeHeight == 360 * 2) ||\
            (modeWidth == 400 * 1 && modeHeight == 300 * 2) ||\
            (modeWidth == 320 * 1 && modeHeight == 240 * 2) ||\
            (modeWidth == 320 * 1 && modeHeight == 200 * 2)\
        )

//
// Returns TRUE if the mode is a dual screen multi mon mode. It can beeither horizontal or vertical.
//
#define MULTIMON_MODE(modeWidth, modeHeight)\
        (HORIZONTAL_MODE(modeWidth, modeHeight) || VERTICAL_MODE(modeWidth, modeHeight))

//
// Fills in a nvRECTL structure (which is the same structure as RECTL).
//
#define SET_RECT(nvRECTLPtr, leftVal, topVal, rightVal, bottomVal)\
{\
    (nvRECTLPtr)->left = (leftVal);\
    (nvRECTLPtr)->top =  (topVal);\
    (nvRECTLPtr)->right =  (rightVal);\
    (nvRECTLPtr)->bottom =  (bottomVal);\
}



//******************************************************************************
// Bitmask flag for support desktop
// NOTE: These definitions must match those in the NV miniport's 'NV.h'!
//******************************************************************************
//
// 31            24 23           16                               0
// .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
// |  Secondary        Primary             Master Flag             |
// `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
// Master Flag            : bit 0 - bit 15
// Primary Display Flag   : bit 16 - bit 23
// Secondary Display Flag : bit 24 - bit 31
//
//******************************************************************************
#define NV_VIRTUAL_DESKTOP_1        0x00010000
#define NV_VIRTUAL_DESKTOP_2        0x01000000

// NV_SPANNING_MODE is virtual desktop (cxScreen, cyScreen) enabled.
// ie, ppdev->ulMode to mode table resolution does not match to (cxScreen, cyScreen)
// additional information for set mode is passed from IOCTL_VIDEO_SET_DESKTOP_INFO_MODE 
// to miniport driver.

#define NV_TWO_DACS                 0x01
#define NV_CLONE_DESKTOP            0x02
#define NV_SPANNING_MODE            0x04
#define NV_ENABLE_CLASS_SET_MODE    0x08

#define NV_ENABLE_VIRTUAL_DESKTOP   (NV_VIRTUAL_DESKTOP_1 | NV_VIRTUAL_DESKTOP_2)
//
// Values for the ppdev->ulDACStatus[NV_NO_DACS]
//
#define DAC_STATUS_ON_BOARD      1   // The DAC is onboard.
#define DAC_STATUS_CONNECTED    2   // The DAC is connected to a device (CRT/DFP/TV).
#define DAC_STATUS_ACTIVE       4   // The DAC is active. For example, in spanning mode and clone mode, both dacs are active.
                                    // In normal mode, only one head is active.



//*****************************************************************************
//
// NV11 TwiNView definitions for controlling dual CRTC functionality
// (previously in Win9x\inc\escape.h).
//
typedef struct  _nvRECTL
{
    long    left;
    long    top;
    long    right;
    long    bottom;
}   nvRECTL;
#define NV_NO_DACS  2

//
// The device scan bit flags. Used in Win2K/NT4.0 only.
// These are used for the "ulHeadDeviceOptions" field in the GET_DEVICE_SCAN_DATA escape call.
//
#define HEAD_DEVICE_OPTION_CRT 0x1
#define HEAD_DEVICE_OPTION_DFP 0x2
#define HEAD_DEVICE_OPTION_TV  0x4

//
// This indicates that nothing is connected on this head
//
#define INVALID_DEVICE_TYPE 0x9999

// - Added escapes for the control panel to Get the deviceOptions matrix and to initiate the device scan. And
// - to set the timing mode override.
// - This is for TwinView for Win2K/NT4.0 only.
#define NV_ESC_GET_DEVICE_SCAN_DATA                 0x6992
#define NV_ESC_INDUCE_DEVICE_SCAN                   0x6993

typedef struct _GET_DEVICE_SCAN_DATA_TYPE {
    unsigned long ulNumberDacsOnBoard;
    unsigned long ulNumberDacsConnected;
    unsigned long ulNumberDacsActive;
    unsigned long ulHeadDeviceOptions[NV_NO_DACS];
} GET_DEVICE_SCAN_DATA_TYPE;




// use these values for the "dwFlag" member of NVTWINVIEWDATA
#define NVTWINVIEW_FLAG_GET                      0
#define NVTWINVIEW_FLAG_SET                      1
// use these values for the "dwAction" member of NVTWINVIEWDATA
#define NVTWINVIEW_ACTION_SETGET_STATE           1
#define NVTWINVIEW_ACTION_SETGET_DEVICE_TYPE     2
#define NVTWINVIEW_ACTION_SETGET_VIRTUALDESKTOP  3
#define NVTWINVIEW_ACTION_SETGET_FREEZE_ORIGIN   4
#define NVTWINVIEW_ACTION_SETGET_PRIMARY_CRTC    5
#define NVTWINVIEW_ACTION_SETGET_VIRTUAL_MODE    6
#define NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE   7

// The following actions are applicable only for NT40 and Win2K. Win9X can ignore this.
#define NVTWINVIEW_ACTION_SETGET_PHYSICAL_VIRTUAL_MODELIST   8
#define NVTWINVIEW_ACTION_SETGET_GDI_MODELIST    9
#define NVTWINVIEW_ACTION_SETGET_PHYSICAL_VIRTUAL_MODE     11

#define NVTWINVIEW_ACTION_VALIDATE_VIRTUAL_MODE  20
#define NVTWINVIEW_ACTION_VALIDATE_PHYSICAL_MODE 21
#define NVTWINVIEW_ACTION_SETGET_TIMING_OVERRIDE 22

// These escapes are to support DVD ZOOM under Win9X. (and maybe NT?)
#define NVTWINVIEW_ACTION_LOCK_HWCRTC                       30
#define NVTWINVIEW_ACTION_UNLOCK_HWCRTC                     31
#define NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE_IMMEDIATE    32

// The following is for Win9X. Win2K/NT4.0 can ignore this.
#define NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD   40

#define NVTWINVIEW_ACTION_SETGET_CLONE_GAMMA    41

// use these values for the "dwState" member of NVTWINVIEWDATA
#define NVTWINVIEW_STATE_NORMAL                  0
#define NVTWINVIEW_STATE_MULTIMON                1
#define NVTWINVIEW_STATE_CLONE                   2

// When the NVTWINVIEW_ACTION_SETGET_STATE escape is issued,
// a return of the following values in dwSuccess of NVTINWVIEWDATA
// indicates what is needed to make the state change happen.
#define NVTWINVIEW_STATE_CHANGE_FAILED          0
#define NVTWINVIEW_STATE_CHANGE_DONE            1
#define NVTWINVIEW_STATE_CHANGE_NEEDS_REBOOT    2

// use these values for the "dwAutoPan" member of NVTWINVIEW_AUTOPAN_DATA
#define NVTWINVIEW_VIRTUALDESKTOP_OFF            0
#define NVTWINVIEW_VIRTUALDESKTOP_ON             1

// use these values for the "dwState" member of NVTWINVIEWDATA
#define NVTWINVIEW_STATE_AUTO                    0
#define NVTWINVIEW_STATE_DMT                     1
#define NVTWINVIEW_STATE_GTF                     2

// use these values for the "ulTimingOverRide" field of NVTWINVIEW_DEVICE_TYPE_DATA. Used only in Win2K and NT4.0
#define TIMING_OVERRIDE_AUTO                    0
#define TIMING_OVERRIDE_DMT                     1
#define TIMING_OVERRIDE_GTF                     2

typedef struct {
unsigned long dwDeviceMask;        // A unique bitmask in a 32 bit Dword identifying
                                    // this specific output device. (as defined in NVCM.H)

   unsigned long dwDeviceType;      // Monitor, DFP, TV (as defined in NVCM.H)
   unsigned long dwTVFormat;        // Format for TV    (as defined in NVCM.H)
   unsigned long dwXRes;            // Horizontal resolution of secondary device in clone mode (in pixels). Used only by Win9X.
   unsigned long dwYRes;            // Vertical resolution of secondary device in clone mode (in Pixels). Used only by Win9X.
   unsigned long dwBpp;             // For Win9X: color depth of secondary device in clone mode (in Bits per pixel)
                                    // For Win2K/NT40: color depth of this head. The color depths of both heads will be the same.
   unsigned long dwRefresh;         // For Win9X: refresh rate of secondary device in clone mode (actual rate in Hz or 1 for "optimal" or 0 for "default")
                                    // For Win2K/NT40: refresh rate of this head (actual rate in Hz or 1 for "optimal" or 0 for "default"), the
                                    // refresh rates can be different for each head.

   // The following fields are applicable only for NT40 and Win2K. Win9X can ignore this.
   unsigned long dwNumPhysModes;    // The number of physical modes supported by the device. This list is obtained after
                                    // an intersection of EDID and the registry modes. If the device has no EDID, then this
                                    // list simply equals the registry modelist.
   unsigned long dwNumVirtualModes; // The number of virtual modes supported by the device. This list is obtained after
                                    // an intersection of registry modes and the board frame buffer size. 
   nvRECTL    PhysicalRectl;        // Physical coordinates of the device.
   nvRECTL    VirtualRectl;         // Virtual coordinates of the device.
   unsigned long dwEnableDDC; 
   unsigned long dwEnablePanScan;   // Enables the virtual desktop to let the physical resolution
                                    // to be less than the virtual resolution.
   unsigned long dwFreezePanScan;   // Freeze the pan-scan
   unsigned long dwFreezeOriginX;   // The user specified upper left corner for freezing the pan-scan.
   unsigned long dwFreezeOriginY;   // The user specified upper left corner for freezing the pan-scan.
   unsigned long dwTimingOverRide;  // GTF or DMT preference by the user.


} NVTWINVIEW_DEVICE_TYPE_DATA;

typedef struct {
   unsigned long dwVirtualDesktop;  // Virtual Desktop On/Off (clone mode only)
   unsigned long dwPad;
}  NVTWINVIEW_VIRTUALDESKTOP_DATA;

typedef struct {
   unsigned long dwFreezeOrigin;    // Freeze Origin On/Off (clone mode only)
   unsigned long dwOriginX;         // freeze mode x origin (clone mode only)
   unsigned long dwOriginY;         // freeze mode y origin (clone mode only)
}  NVTWINVIEW_FREEZE_ORIGIN_DATA;

typedef struct {
   unsigned long pHWCrtcCX;
   unsigned long pGNLogdevCX;
   unsigned long dwLogicalCrtcIndex;
   unsigned long dwPad;
}  NVTWINVIEW_LOCK_UNLOCK_HWCRTC_DATA;

typedef struct {
   unsigned long pHWCrtcCX;
   unsigned long pGNLogdevCX;
   unsigned long dwLogicalCrtcIndex;
   unsigned long dwPad;
}  NVTWINVIEW_SETGET_PHYSICAL_MODE_IMMEDIATE;




// The mode_entry structure is copied from miniport\nv_mini\nv.h. Anychanges in one file should be reflected
// in the other.
struct Mode_Entry
   {
   unsigned short  ValidMode;
   unsigned short  Width;
   unsigned short  Height;
   unsigned short  Depth;
   unsigned short  RefreshRate;
#ifndef WINNT
   unsigned short  wPad;
   unsigned long   dwPad;
#endif
   };
typedef struct Mode_Entry *PMODE_ENTRY, MODE_ENTRY;

// This is per head information
typedef struct {
   MODE_ENTRY * PhysModeEntries;         // Pointer to alloced memory for the physical mode list.
   unsigned long dwNumPhysModeEntries;   // The number of mode entries in the alloced memory for the physical mode list.
   MODE_ENTRY * VirtualModeEntries;         // Pointer to alloced memory for the physical mode list.
   unsigned long dwNumVirtualModeEntries;   // The number of mode entries in the alloced memory for the physical mode list.

   unsigned long dwDeviceMask;  // Return the modelist info for this deviceMask

   unsigned long dwDeviceType;  // Return the modelist info for this deviceType on this head
   unsigned long dwTVFormat;    // Return the modelist info for this TV format on this head
}  NVTWINVIEW_MODELIST_DATA;

// Values for the dwOrientation field
#define HORIZONTAL_ORIENTATION  1
#define VERTICAL_ORIENTATION    2


typedef struct {
   unsigned long dwFlag;            // flag indicating GET or SET
   unsigned long dwAction;          // indicates action to be performed
   unsigned long dwState;           // normal, multi-mon, clone
   unsigned long dwCrtc;            // indicates CRT number for which action is performed. Used only by Win9X.
                                    // The actions apply to all the heads in Win2K and NT40.
   unsigned long dwSuccess;         // indicates success of call. Driver should return non-zero in this member
                                    // if the escape is successfully handled, or zero for failure
   nvRECTL    DeskTopRectl;           // The overall desktop rectangle. The (left,top) is always (0, 0).
                                    // Applicable only for Win2K and NT40. Win9X can ignore it.
   unsigned long dwBoard;           // The board number in a multi-adapter system.
                                    // Applicable only for NT40. Win9X and Win2K can ignore it.
   unsigned long dwOrientation;     // Horizontal or vertical. For multi-mon mode only.
                                    // Applicable only for NT4.0 and Win2K. Win9x can ignore this.
   unsigned long dwDeviceDisplay[NV_NO_DACS]; // Dynamically switch device; ulDeviceDisplay[0] will be primary; head 0 or head 1
                                    // Applicable only for NT4.0 and Win2K. Win9x can ignore this.

   unsigned long dwAllDeviceMask;  // Bitmask for all the output device connectors in the board. NT4.0/Win2K only.
   unsigned long dwConnectedDeviceMask;  // Bitmask for all the output device connectors in the board. NT4.0/Win2K only.
                                    


   union {
       NVTWINVIEW_DEVICE_TYPE_DATA nvtwdevdata[NV_NO_DACS]; // use with action NVTWINVIEW_ACTION_SETGET_DEVICE_TYPE and NVTWINVIEW_ACTION_SETGET_STATE (if
                                    // dwState == NVTWINVIEW_STATE_CLONE)

       NVTWINVIEW_VIRTUALDESKTOP_DATA    nvtwvirtualdesktopdata;  // use with action NVTWINVIEW_ACTION_SETGET_VIRTUALDESKTOP

       NVTWINVIEW_FREEZE_ORIGIN_DATA nvtwfreezedata; // use with action NVTWINVIEW_ACTION_SETGET_FREEZE_ORIGIN
                                      // Note: union is ignored for  and NVTWINVIEW_ACTION_SETGET_PRIMARY_CRTC and
                                      // NVTWINVIEW_ACTION_SETGET_STATE (if dwState != NVTWINVIEW_STATE_CLONE)

       NVTWINVIEW_MODELIST_DATA   nvtwModeListData[NV_NO_DACS]; // Applicable only for Win2K and NT40. Win9X can ignore it.
                                                    // Use with actions NVTWINVIEW_ACTION_SETGET_PHYS_MODELIST and
                                                    // NVTWINVIEW_ACTION_SETGET_GDI_MODELIST.
#ifndef WINNT
        unsigned long adwGammaTable[256];
#endif
   } NVTWINVIEWUNION;

   NVTWINVIEW_LOCK_UNLOCK_HWCRTC_DATA nvtwLockUnlockHwcrtc;
   NVTWINVIEW_SETGET_PHYSICAL_MODE_IMMEDIATE nvtwPhysicalModeImmediate;
} NVTWINVIEWDATA;


#ifdef __cplusplus
}
#endif

#endif // _nvPM_h
