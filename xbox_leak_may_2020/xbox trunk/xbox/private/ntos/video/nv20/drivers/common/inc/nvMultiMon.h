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

// Uncomment this line to enable the Win2K hot key driver code  for laptops
#define ENABLE_WIN2K_HOTKEY_CODE

// maximum number of heads this driver supports
#define NV_MAX_HEADS   2

// Some special bitmask definitions for the new head API of resman.
#define BITMASK_CRT0    0x1
#define BITMASK_CRT1    0x2
#define BITMASK_DFP0    0x10000
#define BITMASK_TV0     0x100
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
            (modeWidth == 1600 * 2 && modeHeight == 1000 * 1) ||\
            (modeWidth == 1600 * 2 && modeHeight == 900 * 1) ||\
            (modeWidth == 1600 * 2 && modeHeight == 880 * 1) ||\
            (modeWidth == 1400 * 2 && modeHeight == 1050 * 1) ||\
            (modeWidth == 1360 * 2 && modeHeight == 768 * 1) ||\
            (modeWidth == 1280 * 2 && modeHeight == 1024 * 1) ||\
            (modeWidth == 1280 * 2 && modeHeight == 960 * 1) ||\
            (modeWidth == 1280 * 2 && modeHeight == 800 * 1) ||\
            (modeWidth == 1280 * 2 && modeHeight == 768 * 1) ||\
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
            (modeWidth == 1600 * 1 && modeHeight == 1000 * 2) ||\
            (modeWidth == 1600 * 1 && modeHeight == 900 * 2) ||\
            (modeWidth == 1600 * 1 && modeHeight == 880 * 2) ||\
            (modeWidth == 1400 * 1 && modeHeight == 1050 * 2) ||\
            (modeWidth == 1360 * 1 && modeHeight == 768 * 2) ||\
            (modeWidth == 1280 * 1 && modeHeight == 1024 * 2) ||\
            (modeWidth == 1280 * 1 && modeHeight == 960 * 2) ||\
            (modeWidth == 1280 * 1 && modeHeight == 800 * 2) ||\
            (modeWidth == 1280 * 1 && modeHeight == 768 * 2) ||\
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
//
// Registry key "PanScanSelection"
// absent or 0: pan-scan in only secondary monitor of clone mode.
// 1: Enable pan-scan in all modes.
// 2: Disable pan-scan in all modes. 
//
#define PAN_SCAN_SELECTION_CLONE_SECONDARY_ONLY 0
#define PAN_SCAN_SELECTION_ENABLE_ALL           1
#define PAN_SCAN_SELECTION_DISABLE_ALL          2

//
// A private named event between the control panel and miniport on Win2K.
//
#define NVCPL_EVENT_HOTKEY "NVCplHotKeyEvent"
#define NVCPL_EVENT_HOTKEY_DRIVER "\\BaseNamedObjects\\"NVCPL_EVENT_HOTKEY


//
// Actions to be performed on named events for Win2K/NT4.0
//
#define EVENT_ACTION_CREATE 1
#define EVENT_ACTION_SIGNAL 2
#define EVENT_ACTION_CLEAR  3


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


// escape for control panel to get the desired deviceMask after the NVSVC is signalled for a device change.
#define NV_ESC_GET_DEVICE_MASK                   0x6994
typedef struct _GET_DEVICE_MASK_DATA_TYPE {
    unsigned long ulDeviceMask;
} GET_DEVICE_MASK_DATA_TYPE;

//
// Support for the hardware icons (such as the battery status for Gateway/Quantas). Used in Win2K/NT4.0 and Win9X/WinME.
// NV_ESC_DISPLAY_ICON: Displays the requested icon as a hardware cursor after caching current cursor.
// NV_ESC_HIDE_ICON: Removes the displayed Icon and restores the desktop cursor to the cached cursor.
//
typedef struct _DISPLAY_ICON_DATA_TYPE {
    unsigned long ulStartX;
    unsigned long ulStartY; // The position of upper left corner of the icon
    unsigned long ulWidth;
    unsigned long ulHeight; // dimensions of the icon bitmap
    void * pvBitmap;        // Bitmap pointer for the 32BPP, ARGB icon
    unsigned long ulFlag;   // For future usage
} DISPLAY_ICON_DATA_TYPE;
#define NV_ESC_DISPLAY_ICON                 0x6996
#define NV_ESC_HIDE_ICON                    0x6997

// DrvEscape function call to disable HW cursor with *(ULONG *)pvIn != FALSE
// if *(ULONG *)pvIn == FALSE, the HW cursor will be re-enabled.
#define NV_ESC_DISABLE_HW_CURSOR            0x6998

//
// DrvEscape() call for the NVSVC to report its current status to the display driver.
//
#define NV_ESC_SET_NVSVC_STATUS            0x6999
//
// The possible NVSVC_STATUS values. At bootup time, display driver will assume a STOPPED status till
// NVSVC makes this call.
//
#define NVSVC_STATUS_STOPPED    0
#define NVSVC_STATUS_STARTED    1
#define NVSVC_STATUS_PAUSED     2

// escape call for the control panel to query the saved settings.
// The input is the QUERY_SAVE_SETTINGS structure and the output is the NVTWINVIEWDATA structure for the
// requested device combo. The caller must have preallocated the memory for the output structure which is
// sizeof(NVTWINVIEWDATA). The display driver saves the settings for standard/clone and span modes. 
// WinXP DualView is not saved since it is all handled directly by the OS and the OS panel.
#define NV_ESC_QUERY_SAVE_SETTINGS            0x7010
// Number of entries in the save settings table. The display driver saves the TwinViewInfo for each
// device and for each device combo in clone and span modes. Currently the devices are CRT0, LCD, TV and CRT1. 
#define NUM_ENTRIES_SAVE_SETTINGS 28
// Input to the query to get the saved settings. Caller should fill up all fields.
typedef struct _QUERY_SAVE_SETTINGS
{
    unsigned long ulState;
    unsigned long ulOrientation;
    unsigned long ulDeviceMask[NV_NO_DACS];
    unsigned long ulDeviceDisplay[NV_NO_DACS]; // Per requirments for example, LCD+CRT is treated 
                                               // as the same combo as CRT+LCD.
} QUERY_SAVE_SETTINGS;
// Input is a fully constructed NVTWINVIEWDATA structure. This structure will be saved in the correct slot
// in the registry "SaveSettings" table for the State+device combo of the NVTWINVIEWDATA structure. 
// No sanity check or validation will be done on
// this input structure. There is no output structure for this escape call.
#define NV_ESC_FORCE_SAVE_SETTINGS  0x7011



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

// returns the last used TV format in the dwTVFormat member of NVTWINVIEW_DEVICE_TYPE_DATA
#define NVTWINVIEW_ACTION_GET_LAST_TV_FORMAT     23

// These escapes are to support DVD ZOOM under Win9X. (and maybe NT?)
#define NVTWINVIEW_ACTION_LOCK_HWCRTC                       30
#define NVTWINVIEW_ACTION_UNLOCK_HWCRTC                     31
#define NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE_IMMEDIATE    32

// The following is for Win9X. Win2K/NT4.0 can ignore this.
#define NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD   40

#define NVTWINVIEW_ACTION_SETGET_CLONE_GAMMA    41
#define NVTWINVIEW_ACTION_ROTATE_CLONE_DEVICES  42
#define NVTWINVIEW_ACTION_BEGIN_DISPLAY_CHANGE  43
#define NVTWINVIEW_ACTION_END_DISPLAY_CHANGE    44
#define NVTWINVIEW_ACTION_GET_LARGEST_EDID_MODE 45

#ifndef WINNT
// This escape allows changing the display device configration
// all in one escape. The caller can specify STANDARD or CLONE
// state and all the display devices necessary for the state specified.
// Note that this sub-escape only uses the dwFlag and dwAction fields
// from NVTWINVIEWDATA. All other data pertinent to this call are
// in the structure _NVTWINVIEW_DISPLAY_CONFIG below.
//
// On a NVTWINVIEW_FLAG_SET, the structure fields should be as follows:
// dwActiveDevices should contain a 1 bit for each display they want to
//   turn on.
//   Bit 0 -- the primary display
//   Bit 1 -- first clone display
//   etc....
//   (Hopefully this is forward looking to the day when we have N heads).
//   Note that with two heads, as we have at the time of this writing,
//   dwState will either equal 1 for STANDARD mode or 3 for CLONE mode.
//
// dwDeviceMask should contain a 1 bit for each device the caller wants
// to be used.
//   Bits 7:0 indicate CRTs:  Bit 0  = CRT0,  Bit 1  = CRT1, etc...
//   Bits 15:8 indiate TVs:   Bit 8  = TV0,   Bit 9  = TV1,  etc...
//   Bits 23:16 indiate DFPs: Bit 16 = DFP0,  Bit 17 = DFP1, etc...
//   Bits 31:24 are not used.
//
// dwPrimaryDeviceMask should contain a 1 bit in the bit position to
//   indicate which device should be the primary.  The bit defintions
//   are the same as for dwDeviceMask.  Note that only 1 bit should be
//   set in this mask and that the same bit in dwDeviceMask must also
//   be a 1.
//
// dwPhysicalModeFlags should contain a 1 in the same bit positions
//   as the dwDeviceMask for every device that the caller is specifying
//   a new physical mode for. Note that you should never have a 1 in
//   any bit position where dwDeviceMask has a 0.
//   Bits 31:24 are not used.
//
// PhysModes[]  If bit position N of dwPhysicalModeFlags has a 1 in it,
//   then PhysModes[N] is the physical mode to use for that device. Note
//   that the pixel depth is not settable here -- it is determined by the
//   desktop (the virtual mode.) There are two special "physical modes."
//
//   If dwXRes = 0, then the xres and yres for the physical mode will be
//   set to the largest resolution the display device is capable of that
//   does not exceed the desktop resolution.
//
//   If dwXRes = -1, then the xres and yres for the physical mode will be
//   set to the same resolution that the display device had the last time
//   that it was used.
//
// dwSetModesNow   If this is non-0, then the display driver will remap
//   all the display devices and set all the modes within the context of
//   this escape call. If this variable is 0, the display driver just saves
//   the desired state change and expects the caller to issue a
//   ChangeDisplaySettingsEx() in order to get the new state banged into
//   the hardware.
//
// -----------------------------------------------------------------------
// On a NVTWINVIEW_FLAG_GET, the caller fills in the
//   dwPhysicalModeFlags field as described a little ways below. The
//   display driver fills in the following fields on return.
//
// dwActiveDevices will contain a 1 bit for each display that is on
//   Bit 0 -- the primary display
//   Bit 1 -- first clone display
//   etc....
//
// dwDeviceMask will contain a 1 bit for each device in use
//   Bits 7:0 indicate CRTs:  Bit 0  = CRT0,  Bit 1  = CRT1, etc...
//   Bits 15:8 indiate TVs:   Bit 8  = TV0,   Bit 9  = TV1,  etc...
//   Bits 23:16 indiate DFPs: Bit 16 = DFP0,  Bit 17 = DFP1, etc...
//   Bits 31:24 are not used.
//
// dwPrimaryDeviceMask will contain a 1 bit in the bit position to
//   indicate which device is the primary.  The bit defintions
//   are the same as for dwDeviceMask.
//
// PhysModes[]  If bit position N of dwDeviceMask has a 1 in it,
//   then PhysModes[N] is the physical mode either currently set
//   on that device or the maximum physical mode settable on that
//   device depending upon how the caller set the dwPhysicalModeFlags.
//
//  dwPhysicalModeFlags is set by the CALLER. If bit N is a 1,
//   then the driver fills in PhysModes[N] with the CURRENT physical
//   mode on the display device. If bit N is a 0, then the driver
//   fills in PhysModes[N] with the maximal physical mode on the
//   display device.

typedef struct  _PHYSICAL_MODE
{
    unsigned long   dwXRes;
    unsigned long   dwYRes;
    unsigned long   dwRefreshRate;
}   PHYSICAL_MODE;

#define NVTWINVIEW_ACTION_SETGET_DISPLAY_CONFIG 50
typedef struct  _NVTWINVIEW_DISPLAY_CONFIG
{
    unsigned long   dwActiveDevices;
    unsigned long   dwDeviceMask;
    unsigned long   dwPrimaryDeviceMask;
    unsigned long   dwPhysicalModeFlags;
    PHYSICAL_MODE   PhysModes[24];
    unsigned long   dwSetModesNow;
}   NVTWINVIEW_DISPLAY_CONFIG;
#endif

// use these values for the "dwState" member of NVTWINVIEWDATA
#define NVTWINVIEW_STATE_NORMAL                 0
#define NVTWINVIEW_STATE_MULTIMON               1 // to be deleted
#define NVTWINVIEW_STATE_DUALVIEW               1
#define NVTWINVIEW_STATE_CLONE                  2
#define NVTWINVIEW_STATE_SPAN                   3

// When the NVTWINVIEW_ACTION_SETGET_STATE escape is issued,
// a return of the following values in dwSuccess of NVTINWVIEWDATA
// indicates what is needed to make the state change happen.
#define NVTWINVIEW_STATE_CHANGE_FAILED          0
#define NVTWINVIEW_STATE_CHANGE_DONE            1
#define NVTWINVIEW_STATE_CHANGE_NEEDS_REBOOT    2

// use these values for the "dwAutoPan" member of NVTWINVIEW_AUTOPAN_DATA
#define NVTWINVIEW_VIRTUALDESKTOP_OFF           0
#define NVTWINVIEW_VIRTUALDESKTOP_ON            1

// use these values for the "dwState" member of NVTWINVIEWDATA
#define NVTWINVIEW_STATE_AUTO                   0
#define NVTWINVIEW_STATE_DMT                    1
#define NVTWINVIEW_STATE_GTF                    2

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

// Toshiba wants to keep the same desktop on hotkey switches.
typedef MODE_ENTRY TOSHIBA_HOTKEY_MODE;

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
        NVTWINVIEW_DISPLAY_CONFIG  nvtwDisplayConfig;
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
