#ifndef _VDD_H_
#define _VDD_H_
/************ Operating System Interface for NVidia Hardware ***************\
*                                                                           *
* Module: VDD.H                                                             *
*       Header file for Virtual Display device.                             *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
* History:                                                                  *
*        David Schmenk (dschmenk)      7/22/95 - wrote it                   *
*                                                                           *
\*********************** Copyright 1995 NVidia, Inc. ***********************/
//***************************************************************************
//
//   (C) Copyright MICROSOFT Corp., 1988-1993
//
//   Title: minivdd.inc - VDD definitions for other VxD's and multiple VDD's
//
//   Version:   4.00
//
//   Date:
//
//   Author: FredE for the Mini-VDD interface.  Adapted from VDD.INC in
//      the general include area.  Added services are ONLY for the
//      use of the Mini-VDD and should therefore not be documented.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------
//
// VDD services.
//
#define VDD_Get_Version                     0x0000  // Get version number and ID string ptr
#define VDD_PIF_State                       0x0001  // Pass video PIF bits to VDD
#define VDD_Get_GrabRtn                     0x0002  // Get routine addr for video grab
#define VDD_Hide_Cursor                     0x0003  // Hide cursor when display is windowed
#define VDD_Set_VMType                      0x0004  // Set VM type(windowed, bckgrnd, excl)
#define VDD_Get_ModTime                     0x0005  // Return system time of last mod
#define VDD_Set_HCurTrk                     0x0006  // Track horiz cursor movement in window
#define VDD_Msg_ClrScrn                     0x0007  // Clear screen for sysmodal message
#define VDD_Msg_ForColor                    0x0008  // Set Msg_TextOut forground color
#define VDD_Msg_BakColor                    0x0009  // Set Msg_TextOut background color
#define VDD_Msg_TextOut                     0x000A  // Output a string
#define VDD_Msg_SetCursPos                  0x000B  // Set cursor position
#define VDD_Query_Access                    0x000C  // Is it OK to access video now?
#define VDD_Check_Update_Soon               0x000D  // User action may update screen
#define VDD_Get_Mini_Dispatch_Table         0x000E  // Get addr of dispatch table to mini-VDD
#define VDD_Register_Virtual_Port           0x000F  // Mini-VDD asks us to setup I/O trap
#define VDD_Get_VM_Info                     0x0010  // Returns CRTC owner and MemC owners
                                                    // and other special VM handles
#define VDD_Get_Special_VM_IDs              0x0011  // returns planar & msg mode IDs
#define VDD_Register_Extra_Screen_Selector  0x0012  // allows display driver to use second
                                                    // screen selector for block moves
#define VDD_Takeover_VGA_Port               0x0013  //allows MiniVDD to takeover a port
                                                    // in range 3C0H through 3DFH
#define VDD_Get_DISPLAYINFO                 0x0014  //get DISPLAYINFO data structure
#define VDD_Do_Physical_IO                  0x0015  //perform physical I/O for trapped port
#define VDD_Set_Sleep_Flag_Addr             0x0016  //when display driver can't be interrupted
//
// Following are function codes that can be called via the VDD's
// API entry point. These are mainly for display driver --> VDD communication.
// Since Windows 3.0 and 3.1 VDD's may have used the sequential numbers
// (after the Grabber functions) for other VDD API services, we start our
// numbering at 80H so as to avoid ugly conflicts with old 3.1 stuff:
//
#define MINIVDD_SVC_BASE_OFFSET             0x80
#define VDD_DRIVER_REGISTER                 (0 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_DRIVER_UNREGISTER               (1 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_SAVE_DRIVER_STATE               (2 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_REGISTER_DISPLAY_DRIVER_INFO    (3 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_REGISTER_SSB_FLAGS              (4 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_GET_DISPLAY_CONFIG              (5 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_PRE_MODE_CHANGE                 (6 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_POST_MODE_CHANGE                (7 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_SET_USER_FLAGS                  (8 | MINIVDD_SVC_BASE_OFFSET)
#define VDD_SET_BUSY_FLAG_ADDR              (9 | MINIVDD_SVC_BASE_OFFSET)
typedef struct
{
    WORD  HdrSize;
    WORD  InfoFlags;
    DWORD DevNodeHandle;
    BYTE  DriverName[16];
    WORD  XRes;
    WORD  YRes;
    WORD  DPI;
    BYTE  Planes;
    BYTE  Bpp;
    WORD  RefreshRateMax;
    WORD  RefreshRateMin;
    WORD  LowHorz;
    WORD  HighHorz;
    WORD  LowVert;
    WORD  HighVert;
    DWORD MonitorDevNodeHandle;
    BYTE  HorzSyncPolarity;
    BYTE  VertSyncPolarity;
} DISPLAYINFO, * PDISPLAYINFO;
//
// Following are values and explanations for the diInfoFlags word in DISPLAYINFO:
//
// RETURNED_DATA_IS_STALE, if set, means that this call to VDD_GET_DISPLAY_CONFIG
// or VDD_GetDisplayInfo (which are the Ring 3 and Ring 0 methods by which a
// program would get the DISPLAYINFO structure returned to him) caused the VDD
// to return data that was read in a previous call to VDD_GET_DISPLAY_CONFIG
// insted of actually going out and reading "fresh" data from the Registry.
//
// This flag brings to light the fact that there are some circumstances when the
// VDD cannot go out and read the registry in response to the call to
// VDD_GET_DISPLAY_CONFIG or VDD_GetDisplayInfo (due to system multi-tasking
// considerations).  In this case, this flag will be set to a 1 to indicate that
// the information being returned isn't "fresh" -- that is -- it may be
// incorrect and obsolete.  The caller should respond accordingly if this flag
// is set.
//
#define RETURNED_DATA_IS_STALE          0x0001
//
// MINIVDD_FAILED_TO_LOAD if set, indicates that for some reason (typically
// that the MiniVDD didn't match the chipset installed in the machine), the
// MiniVDD didn't load.  Callers can examine this flag and act accordingly.
//
#define MINIVDD_FAILED_TO_LOAD          0x0002
//
// MINIVDD_CHIP_ID_DIDNT_MATCH means that although the MiniVDD did load
// successfully, when the ChipID that the MiniVDD calculated was compared
// against the value saved in the registry, they didn't match.  An example of
// when this would happen is when the user is happily using an S3-911 card
// and then decides to upgrade his display card to an S3-864.  Since both
// cards use S3.VXD, the MiniVDD will load, however, since the card model
// is different, the VDD will return a defect to configuration manager and
// set this flag.  Callers of the GET_DISPLAY_CONFIG functions can use this
// flag to take appropriate actions to make sure that the user gets his
// configuration correct.
//
#define MINIVDD_CHIP_ID_DIDNT_MATCH     0x0004
//
// REGISTRY_BPP_NOT_VALID if set, means that we failed to obtain the BPP value
// from the registry when the VDD tried to read it.
//
#define REGISTRY_BPP_NOT_VALID          0x0008
//
// REGISTRY_RESOLUTION_NOT_VALID if set, means that we failed to obtain the
// resolution value from the registry when the VDD tried to read it.
//
#define REGISTRY_RESOLUTION_NOT_VALID   0x0010
//
// REGISTRY_DPI_NOT_VALID if set, means that we failed to obtain the
// DPI value from the registry when the VDD tried to read it.
//
#define REGISTRY_DPI_NOT_VALID          0x0020
//
// MONITOR_DEVNODE_NOT_ACTIVE is set if someone tries to make a call to the
// GET_DISPLAY_CONFIG function before the monitor DevNode has been created.
// This is certainly not fatal by any means.  It simply means that the
// monitor refresh rate info in the DISPLAYINFO data structure is totally
// invalid!
//
#define MONITOR_DEVNODE_NOT_ACTIVE      0x0040
//
// MONITOR_INFO_NOT_VALID indicates that something within the code which
// retrieves and calculates the refresh rate data has failed.  This indicates
// that the values in diRefreshRateMax through diVertSyncPolarity are not
// valid and could contain random data.
//
#define MONITOR_INFO_NOT_VALID          0x0080
//
// MONITOR_INFO_DISABLED_BY_USER indicates that the either the RefreshRate=
// string in SYSTEM.INI had a negative number in it or that the string in
// the display's software key (HKEY_CURRENT_CONFIG\Display\Settings\RefreshRate)
// was 0 or a negative number.
//
#define MONITOR_INFO_DISABLED_BY_USER   0x0100
//
// REFRESH_RATE_MAX_ONLY indicates that there was no diLowHorz, diHighHorz,
// diLowVert, diHighVert, or sync polarity data in the registry.    The
// value returned in diRefreshRateMax is the only refresh rate data that
// we have available.  This was derived either from RefreshRate= in SYSTEM.INI
// or the HKEY_CURRENT_CONFIG\Display\Settings\RefreshRate = string in the
// registry.
//
#define REFRESH_RATE_MAX_ONLY           0x0200
//
// CARD_VDD_LOADED_OK indicates that a second MiniVDD (which is useful for
// display card manufacturers wishing to extend the capabilities of the chip level
// MiniVDD's ) has successfully been loaded and initialized.
//
#define CARD_VDD_LOADED_OK              0x0400
//
// Make functions to these services.
//
WORD  vddGetVersion(VOID);
DWORD vddGetDisplayInfo(PDISPLAYINFO, DWORD);

#pragma aux vddGetVersion =                                 \
            CALL_VXD(VDD_Device_ID, VDD_Get_Version)        \
            value [AX];

#pragma aux vddGetDisplayInfo =                             \
            CALL_VXD(VDD_Device_ID, VDD_Get_DISPLAYINFO)    \
            parm [EAX] [ECX]                                \
            value [EAX]                                     \
            modify [EBX ECX EDX ESI EDI];
#endif // _VDD_H_
