 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVSYS.C                                                           *
*   This is the Windows half of the Resource Manager.  System clients call  *
*   the Resource Manager through these APIS.  The VxD calls into this       *
*   to post a message to the Windows Resource Manager EXE.                  *
*                                                                           *
\***************************************************************************/

//
// Windows main include file
//
#include <windows.h>
//
// USER.275 function to redraw screen.
//
VOID FAR PASCAL RepaintScreen(VOID);
//
// Windows Resource Manager System includes.
//
#include "nv_ref.h"
#include "nvrm.h"
#include "nvrmwin.h"
#include "nvsys.h"
#include "nvcm.h"
#include "nvos.h"

#include <class.h>
#include <vblank.h>
#include <gr.h>
#include <vga.h>

#define DLL_PROCESS_ATTACH  1
#define DLL_PROCESS_DETACH  0

//
// Global cursor control defines
//
#define COLOR_OFFSET    32*sizeof(DWORD)    // offset of XOR mask from start of buffer (= size of and mask)
// white & black filtering
#define WHITE (0xffff)
#define BLACK (0x8000)
#define DRK_BLEND 10    // off-black
#define MED_DRK 14      // a little lighter, for black followed by white
#define LT_BLEND 3      // off-white
#define MED_LT 8        // a little darker, for white followed by black
// formatted filtered white & black
#define MED_WHITE (0x8000 | ((31-MED_LT) << 10) | ((31-MED_LT) << 5) | (31-MED_LT))
#define MED_BLACK (0x8000 | ((31-MED_DRK) << 10) | ((31-MED_DRK) << 5) | (31-MED_DRK))
#define LT_GRAY (0x8000 | ((31-LT_BLEND) << 10) | ((31-LT_BLEND) << 5) | (31-LT_BLEND))
#define DK_GRAY (0x8000 | (DRK_BLEND << 10) | (DRK_BLEND << 5) | DRK_BLEND)

#define IsNvCfgAgp(x) (x>=NV_CFG_AGP_FULLSPEED_FW&&x<=NV_CFG_AGP_LOAD_DUMMY_PAGES)
//---------------------------------------------------------------------------
//
//  Typedefs.
//
//---------------------------------------------------------------------------

typedef struct _def_hw_info      FAR * LPHWINFO;
typedef struct _def_client_info  FAR * LPCLIENTINFO;

//---------------------------------------------------------------------------
//
//  Prototypes.
//
//---------------------------------------------------------------------------

INT   FAR PASCAL LibMain(HINSTANCE, WORD, WORD, LPSTR);
INT   FAR PASCAL _WEP(BOOL);
BOOL  FAR PASCAL THK_ThunkConnect16(LPSTR, LPSTR, WORD, DWORD);
BOOL  FAR PASCAL DllEntryPoint(DWORD, WORD, WORD, WORD, DWORD, WORD);
VOID  FAR PASCAL NvSysIpcCallback(DWORD, DWORD, DWORD);
VOID  FAR PASCAL NvSysNotifyCallback(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef void (_far __cdecl *CAPTUREPROC)(DWORD);
VOID  FAR PASCAL __loadds NvCaptureFrameCallback(DWORD,DWORD);
DWORD FAR PASCAL NvSysDispatch(LPRMPARAMS, DWORD);
VOID  PASCAL CallRM(LPRMPARAMS);
void  CallRM_DeviceSetup(LPRMPARAMS);
BOOL  PASCAL ValidateScreenRect(RMPARAMS FAR *);
BOOL  PASCAL NvThunkNotifyEvent(DWORD, DWORD);
BOOL  PASCAL NvThunkNotifyMsg(DWORD, HWND, WORD);
BOOL  PASCAL NvThunkErrorMsg(DWORD, HWND, WORD);
BOOL  PASCAL NvThunkAllocPages(DWORD, LPDWORD, LPDWORD);
BOOL  PASCAL NvThunkFreePages(DWORD);

DWORD PASCAL REG_RD32(DWORD);
DWORD PASCAL REG_WR32(DWORD, DWORD);
BYTE  PASCAL REG_RD08(DWORD);
DWORD PASCAL REG_WR08(DWORD, DWORD);
VOID  PASCAL NvSysHideEmulatedCursor(WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, LPDWORD);

DWORD PASCAL NvThunkNvArch(DWORD,DWORD,DWORD);

//---------------------------------------------------------------------------
//
//  Data.
//
//---------------------------------------------------------------------------
// Documentation says name should be blank-padded.

char    NVstring[9] = "NVCORE  ";

// Had trouble matching NVRM until I used 0-padded name. Windows bug if name is 4 chars?

//
// Enable flag.
//
BOOL Enabled = FALSE;
//
// Pointer to Resource Manager API routine.
//
DWORD ResManAPI;
//
// Pointers to shared Resource Manager structures.
//
LPCLIENTINFO  lpClientInfo;
U032 FAR * lp16bitProcessDS;
U032     dll16_ClientInstance=0;


LPHWINFO  dll16_NvInfoPtrTable[MAX_INSTANCE];
U032      FAR *lpNvInfoSelectors;

#undef NUM_FIFOS
#define NUM_FIFOS               dll16_NvInfoPtrTable[DeviceHandle-1]->Fifo.HalInfo.Count

//
// Because we are in 16 bit code, we can't just have a selector that describes the entire
// NV register space easily. So we have an array of selectors that point to each major portion
// of the register space. Also, to handle multimon, we need one such array of selectors for each
// NV device.
//
struct {
    WORD  devSelector[NV_DEV_MAX+1]; // Device Selector table.
} devSelectorTable[MAX_INSTANCE];

U008  VblankDummy = NULL;   // dummy vblank status 

//
// Instance of the Windows Resource Manager task.
//
HINSTANCE hInstNvSysEXE;
HANDLE    hTaskNvSysEXE;
//
// Instance of this DLL.
//
HINSTANCE hInstNvSysDLL;
//
// Bit reversal table for cursor glyph.
//
BYTE BitReversal[] =
{
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
    0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
    0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
    0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
    0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
    0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
    0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
    0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
    0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
    0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

//
// Translate  4 bpp into 555
//
U016 color4bpp[] =
{
    0,
    0x4000,
    0x200,
    0x4200,
    0x10,
    0x4010,
    0x210,
    0x6318,
    0x4210,
    0x7c00,
    0x3e0,
    0x7fe0,
    0x1f,
    0x7c1f,
    0x3ff,
    0x7fff
//    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};


//---------------------------------------------------------------------------
//
//  In-line assembly routines.
//
//---------------------------------------------------------------------------

//
//  Get the call gate for calling into the resource manager
//  Now that the RM is a dynamic vxd, it has the undefined device ID and must be located by name
//
VOID GetCallGate(VOID)
{
    __asm {
    mov  bx, 0
    mov  ax, 01684h
    mov  di, ds
    mov  es, di
    lea  di, NVstring
    int  02Fh
    mov  WORD PTR ResManAPI, di
    mov  WORD PTR ResManAPI + 2, es
    }
}

VOID Breakpoint(VOID)
{
    __asm int    3
}

U016 GetSelector(U032 addrLin, U032 limit)
{
    U016    regAX,sel;

    __asm   {
	mov	ax,ds
	mov	regAX,ax
    }
    sel = AllocSelector(regAX);
    SetSelectorBase(sel,addrLin);
    SetSelectorLimit(sel,limit);
    return sel;
}

U032 FAR PASCAL NvSysDriverConnect(U032 osDevHandle, U032 osDeviceName)
{
  RMPARAMS rmParams;
  U032     rmDeviceHandle;

  rmParams.RegEAX = NVRM_API_DRIVER_CONNECT;
  rmParams.RegEBX = osDevHandle;
  rmParams.RegECX = osDeviceName;
  rmParams.RegES  = 0;
  rmParams.RegESI = DEFAULT_DEVICEHANDLE;
  CallRM_DeviceSetup(&rmParams);
  rmDeviceHandle = (U032) rmParams.RegEAX;
  if(rmDeviceHandle != 0)
  {
	  rmParams.RegEAX = NVRM_API_MULTIDEVICE_MAP_RM_INFO;
	  rmParams.RegES  = 0;
	  rmParams.RegESI = DEFAULT_DEVICEHANDLE;
	  CallRM_DeviceSetup(&rmParams);
	  lpNvInfoSelectors = (U032 FAR *) (rmParams.RegEAX << 16);
	  {
		int temp_count;
		for(temp_count=0;temp_count<MAX_INSTANCE;temp_count++)
		  dll16_NvInfoPtrTable[temp_count] = (LPHWINFO) (lpNvInfoSelectors[temp_count] << 16);
	  } 
  }
  
  return rmDeviceHandle;
}

VOID FAR PASCAL NvSysDriverDisConnect(U032 DeviceHandle)
{

  RMPARAMS rmParams;

  rmParams.RegEAX = NVRM_API_DRIVER_DISCONNECT;
  rmParams.RegEBX = DeviceHandle;               // is this parameter even used?
  rmParams.RegES  = 0;
  rmParams.RegESI = DeviceHandle-1;
  CallRM_DeviceSetup(&rmParams);
}

//---------------------------------------------------------------------------
//
//  System routines.
//
//---------------------------------------------------------------------------
//
// Win95 Dll entrypoint.
//
BOOL FAR PASCAL DllEntryPoint
(
    DWORD dwReason,
    WORD  hInstance,
    WORD  wDS,
    WORD  wHeapSize,
    DWORD dwReserved1,
    WORD  dwReserved2
)
{
    static INT AttachCount = 0;

    if (!(THK_ThunkConnect16("NV3SYS.DLL","NV3API.DLL",hInstance,dwReason)))
        return (FALSE);
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            ++AttachCount;
            break;
        case DLL_PROCESS_DETACH:
            --AttachCount;
            break;
    }
    return (TRUE);
}
//
// DLL entrypoint.
//
INT FAR PASCAL LibMain
(
    HINSTANCE hInstance,
    WORD      wDataSeg,
    WORD      cbHeapSize,
    LPSTR     lpszCmdLine
)
{
    INT      i, j;
    RMPARAMS rmParams;
    
    if (Enabled)
        return (1);
    hInstNvSysDLL = hInstance;
    hInstNvSysEXE = 0;
    //
    // Get Resource Manager call gate.
    //
    GetCallGate();
    if (ResManAPI == 0)
    {
        return (FALSE);
    }
    //
    // Check version number.
    //
    
    rmParams.RegEAX = NVRM_API_GET_VERSION;
    rmParams.RegES  = 0;
    rmParams.RegESI = DEFAULT_DEVICEHANDLE;
    CallRM_DeviceSetup(&rmParams);
    if (rmParams.RegEAX != NVRM_VERSION)
    {
        return (FALSE);
    }
    //
    // Install Resource Manager callbacks.
    //
    rmParams.RegEAX = NVRM_API_NOTIFY_CALLBACK;
    rmParams.RegECX = ((DWORD)&NvSysNotifyCallback) & 0xFFFF;
    rmParams.RegEDX = ((DWORD)&NvSysNotifyCallback) >> 16;
    rmParams.RegES  = 0;
    rmParams.RegESI = DEFAULT_DEVICEHANDLE;
    CallRM_DeviceSetup(&rmParams);
    rmParams.RegEAX = NVRM_API_IPC_CALLBACK;
    rmParams.RegECX = ((DWORD)&NvSysIpcCallback) & 0xFFFF;
    rmParams.RegEDX = ((DWORD)&NvSysIpcCallback) >> 16;
    rmParams.RegESI = DEFAULT_DEVICEHANDLE;
    CallRM_DeviceSetup(&rmParams);

    //
    // Install mediaport callback mechanism (Canopus)
    //
	rmParams.RegEAX = NVRM_API_VPE_MISC; // main function
	rmParams.RegEBX = IMAGE_INSTALL_16CALLBACK; // subfunction
	rmParams.RegECX = ((DWORD)&NvCaptureFrameCallback) & 0xFFFF;
	rmParams.RegEDX = ((DWORD)&NvCaptureFrameCallback) >> 16;
	rmParams.RegES  = 0;
	rmParams.RegESI = DEFAULT_DEVICEHANDLE;
	CallRM_DeviceSetup(&rmParams);

    rmParams.RegEAX = NVRM_API_MULTIDEVICE_MAP_RM_INFO;
    rmParams.RegEBX = 0x69;
    rmParams.RegES  = 0;
    rmParams.RegESI = DEFAULT_DEVICEHANDLE;
    CallRM_DeviceSetup(&rmParams);
    lpNvInfoSelectors = (U032 FAR *) (rmParams.RegEAX   << 16);
    {
      int temp_count;
      for(temp_count=0;temp_count<MAX_INSTANCE;temp_count++)
        dll16_NvInfoPtrTable[temp_count] = (LPHWINFO) (lpNvInfoSelectors[temp_count] << 16);
    }    
    
    lpClientInfo  = (LPCLIENTINFO)(rmParams.RegECX << 16);
    lp16bitProcessDS = (U032 FAR *) (rmParams.RegEDI << 16);

    // clear the selector array.
    for(i = 0; i < MAX_INSTANCE; i++)
        for (j = 0; j < NV_DEV_MAX+1; j++)
            devSelectorTable[i].devSelector[j] = 0;

    Enabled = TRUE;
    return (1);
}
//
// DLL exitpoint.
//
INT FAR PASCAL _WEP
(
    BOOL bSysExit
)
{
    RMPARAMS rmParams;

    if (Enabled)
    {
        //
        // Tell Resource Manager not to call back to this DLL anymore.
        //
        rmParams.RegEAX = NVRM_API_UNLOAD_DLL;
        rmParams.RegES  = 0;
        rmParams.RegESI = DEFAULT_DEVICEHANDLE;
        CallRM_DeviceSetup(&rmParams);
    }
    return (1);
}

//
// Capturing Frame callback from mediaport
//
VOID FAR PASCAL __loadds NvCaptureFrameCallback(DWORD DeviceHandle, DWORD dwBufferOffset)
{
// Take care in MultiMonitor environments, the DeviceHandle passed here may NOT necessarily be
// used as an index to the dll_16NvInfoPtrTable !! Check this in the RM VXD
	if (dll16_NvInfoPtrTable[DeviceHandle-1]->MediaPort.ImageCallback)
		((CAPTUREPROC /*__far*/)(dll16_NvInfoPtrTable[DeviceHandle-1]->MediaPort.ImageCallback))( dwBufferOffset);
}


//
// VxD Resource Manager callback to Windows DLL.
//
VOID FAR PASCAL NvSysIpcCallback
(
    DWORD Msg,
    DWORD wParam,
    DWORD lParam
)
{
    ;
    /*
    if (hInstNvSysEXE)
        PostAppMessage(hTaskNvSysEXE,
                       (WORD)Msg,
                       (WORD)wParam,
                       lParam);
    */                       
}

VOID FAR PASCAL NvSysNotifyCallback
(
    DWORD ChID,
    DWORD Object,
    DWORD Offset,
    DWORD Data,
    DWORD Status,
    DWORD Action
)
{
}
//
// Nv thunk dispatcher for 32 bit applications.
//
DWORD FAR PASCAL NvSysDispatch
(
    LPRMPARAMS lpParams,
    DWORD      Function
)
{

//  lpParams->RegEDI holds the DeviceHandle from the 32bit DLL Caller.

    switch (Function)
    {
        //
        // Call Resource Manager.
        //
		case 0:
			lpParams->RegESI = lpParams->RegEDI-1;
            CallRM_DeviceSetup(lpParams);
            break;
        case NV_FUNCTION_NVALLOCPAGES:
            NvThunkAllocPages(GetSelectorBase((UINT)lpParams->RegDS),
                                             &(lpParams->RegEAX),
                                             &(lpParams->RegEDX));
            break;
        case NV_FUNCTION_NVFREEPAGES:
            NvThunkFreePages(lpParams->RegEAX);
            break;
        //
        // Map some routines that aren't available under Win32s.
        //
        case NV_FUNCTION_GETSELECTORBASE:
            lpParams->RegEAX = GetSelectorBase((UINT)lpParams->RegEAX);
            break;
        case NV_FUNCTION_GETCURRENTTASK:
            lpParams->RegEAX = GetCurrentTask();
            break;
        //
        // Configuration Manager APIs for 32 bit apps.
        //
        case NV_FUNCTION_NVCONFIGVERSION:
            lpParams->RegEAX = NvConfigVersion(lpParams->RegEDI);
            break;
        case NV_FUNCTION_NVCONFIGGET:
            lpParams->RegEAX = NvConfigGet(lpParams->RegEAX,lpParams->RegEDI);
            break;
        case NV_FUNCTION_NVCONFIGSET:
            lpParams->RegEAX = NvConfigSet(lpParams->RegEAX, lpParams->RegEBX,lpParams->RegEDI);
            break;
        case NV_FUNCTION_NVCONFIGUPDATE:
            lpParams->RegEAX = NvConfigUpdate(lpParams->RegEAX,lpParams->RegEDI);
            break;
        //
        // Pointer to certain hardware.
        //
        case NV_FUNCTION_NVSYSGETHWPTRS:
            NvSysGetHardwarePointers(&(lpParams->RegEAX),
                                     &(lpParams->RegEBX),
                                     &(lpParams->RegECX),
                                     &(lpParams->RegEDX),
                                     lpParams->RegEDI);
            break;
        case NV_FUNCTION_NVDEVICEBASEGET:
            lpParams->RegEAX = NvDeviceBaseGet(lpParams->RegEAX,lpParams->RegEDI);
            break;
        case NV_FUNCTION_NVDEVICELIMITGET:
            lpParams->RegEAX = NvDeviceLimitGet(lpParams->RegEAX,lpParams->RegEDI);
            break;
        case NVRM_API_ARCHITECTURE_32:
            // EAX-Function EBX-parameters DS-Client Data Selector
            lpParams->RegEAX = NvThunkNvArch(GetSelectorBase((UINT)lpParams->RegDS),lpParams->RegEAX,lpParams->RegEBX);
            break;
    }

    return (0);
}

//---------------------------------------------------------------------------
//
//  Nv display driver entrypoints.
//
//---------------------------------------------------------------------------

VOID FAR PASCAL NvSysMoveCursor
(
    INT    xPos,
    INT    yPos,
    U032   DeviceHandle
)
{

    xPos &= 0xfff;  // bits used on NV3
    yPos &= 0xfff;
    
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorPosX   = xPos;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorPosY   = yPos;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_POS;

}

// This is the new interface to update the cursor image. 
// All we have to do is program the offset, width/height and colorformat.
// We don't have to worry about the converting glyphs etc.
// This is the way a cursor image is defined in NV{04 | 10}_VIDEO_LUT_CURSOR_DAC class.
// Because the cursor manipulation needs to happen independent of what other rendering
// that is going on, the class interface can not be used to update the cursor image and position.
//
// Flags.<0> : if 1, indicates that the rest of the cursor image parameters are valid and they should be
//           :       programmed into the hardware.
//           : if 0, the cursor image values are not valid.
// Flags.<1> : If 1, Flags.<2> indicates how the cursor's enable disable state should be changed.
//           : If 0, don't change the enable/disable state of the cursor.
// Flags.<2> : If 1, enable the cursor.
//           : If 0, disable the cursor.
VOID FAR PASCAL NvSysSetCursorImage
(
    U032  Flags,
    U032  Offset,
    U032  Width,
    U032  Height,
    U032  ColorFormat,
    U032  DeviceHandle
)
{
    // this is the new way of setting cursor image. The cursorType should not be used in this scheme.
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorType = DAC_CURSOR_TYPE_NEW;

    if (Flags & 0x00000001)
    {
        // Offset is assumed to be an offset from the start of the Frame buffer.
        // Make sure The Address Space Indicator for the cursor says it is in the Frame buffer
        // (instead of Instance Memory).

        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorWidth  = Width;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorHeight = Height;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorOffset = Offset;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorColorFormat = ColorFormat;

        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW;
    } // program the cursor image values.

    if (Flags & 0x00000002)
    {
        if (Flags & 0x00000004)
        {
            // Enable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_ENABLE;

            // Make sure there is no pending request to disable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_DISABLE;
        }
        else
        {
            // disable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_DISABLE;

            // Make sure there is no pending request to enable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_ENABLE;
        }
    } // enable or disable the cursor.
}


VOID FAR PASCAL NvSysMoveCursorMulti
(
    INT    xPos,
    INT    yPos,
    U032   DeviceHandle,
    U032   Head
)
{

    xPos &= 0xfff;  // bits used on NV3
    yPos &= 0xfff;
    
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorPosX   = xPos;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorPosY   = yPos;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_POS;

}

// This is the new interface to update the cursor image. 
// All we have to do is program the offset, width/height and colorformat.
// We don't have to worry about the converting glyphs etc.
// This is the way a cursor image is defined in NV{04 | 10}_VIDEO_LUT_CURSOR_DAC class.
// Because the cursor manipulation needs to happen independent of what other rendering
// that is going on, the class interface can not be used to update the cursor image and position.
//
// Flags.<0> : if 1, indicates that the rest of the cursor image parameters are valid and they should be
//           :       programmed into the hardware.
//           : if 0, the cursor image values are not valid.
// Flags.<1> : If 1, Flags.<2> indicates how the cursor's enable disable state should be changed.
//           : If 0, don't change the enable/disable state of the cursor.
// Flags.<2> : If 1, enable the cursor.
//           : If 0, disable the cursor.
VOID FAR PASCAL NvSysSetCursorImageMulti
(
    U032  Flags,
    U032  Offset,
    U032  Width,
    U032  Height,
    U032  ColorFormat,
    U032  DeviceHandle,
    U032  Head
)
{
    // this is the new way of setting cursor image. The cursorType should not be used in this scheme.
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorType = DAC_CURSOR_TYPE_NEW;

    if (Flags & 0x00000001)
    {
        // Offset is assumed to be an offset from the start of the Frame buffer.
        // Make sure The Address Space Indicator for the cursor says it is in the Frame buffer
        // (instead of Instance Memory).

        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorWidth  = Width;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorHeight = Height;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorOffset = Offset;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].CursorColorFormat = ColorFormat;

        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW;
    } // program the cursor image values.

    if (Flags & 0x00000002)
    {
        if (Flags & 0x00000004)
        {
            // Enable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_ENABLE;

            // Make sure there is no pending request to disable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_DISABLE;
        }
        else
        {
            // disable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_DISABLE;

            // Make sure there is no pending request to enable the cursor.
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags &= ~UPDATE_HWINFO_DAC_CURSOR_ENABLE;
        }
    } // enable or disable the cursor.
}


VOID FAR PASCAL NvSysUpdateImageOffsetFormat
(
    U032   offset,
    U032   format,
    U032   chID,
    U032   objectHandle,
    U032   DeviceHandle,
    U032   Head
)
{
    //
    // Note: we don't need the object handle, since the current mode setting
    // pVidLutCurDac object is stashed away in the Dac.CrtcInfo struct already.
    //
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].ImageOffset   = offset;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].ImageFormat   = format;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].ChID          = chID;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].ObjectHandle  = objectHandle;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[Head].UpdateFlags |= UPDATE_HWINFO_DAC_IMAGE_OFFSET_FORMAT;
}


BOOL FAR PASCAL NvSysSetCursor
(
    LPBYTE lpGlyph,
    INT    Width,
    INT    Height,
    U032   DeviceHandle
)
{
    INT i;
    
    //
    // Save width and height.
    //
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorWidth  = Width;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorHeight = Height;
    if (dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorWidth > 32)
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorWidth = 32;
    if (dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorHeight > 32)
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorHeight = 32;
    //
    // Make glyph transparent.
    //
    for (i = 0; i < 32 * sizeof(DWORD); i++)
    {
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorImagePlane[1][i] = 0xFF;
        dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorImagePlane[0][i] = 0x00;
    }
    if (lpGlyph && (Height > 0))
    {
        for (i = 0; i < (INT)(Height * sizeof(DWORD)); i++)
        {
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorImagePlane[1][i] = BitReversal[lpGlyph[i]];
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorImagePlane[0][i] = BitReversal[lpGlyph[i + Height * sizeof(DWORD)]];
        }
    }
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorType = DAC_CURSOR_TWO_COLOR_XOR;
    dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_IMAGE | UPDATE_HWINFO_DAC_CURSOR_POS;
    //
    // False means this isn't emulated.
    //
    
    return (FALSE);
}

DWORD FAR PASCAL NvSysGetScreenInfo
(
    U032    DeviceHandle
)
{
    DWORD returnvalue;

    returnvalue =
        (dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.VertDisplayWidth << 16) |
        dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.HorizDisplayWidth;

    return (returnvalue);
}
DWORD FAR PASCAL NvSysGetRamInfo
(
    U032    DeviceHandle
)
{
    DWORD returnvalue;

    returnvalue = (dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamSizeMb << 16) | dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamType;

    return (returnvalue);
}
DWORD FAR PASCAL NvSysGetBufferInfo
(
    U032    DeviceHandle
)
{
    DWORD returnvalue;

    returnvalue = (dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Count << 16) | dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.Depth;

    returnvalue = 0xdeadbeef;

    return (returnvalue);
}
VOID FAR PASCAL NvSysClearScreen
(
    U032    DeviceHandle
)
{
    RMPARAMS rmParams;

    //
    // Clear visible screen.
    //
    rmParams.RegEAX = NVRM_API_CLEAR_SCREEN;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);

}
VOID FAR PASCAL NvSysDisplayModeHook
(
    DWORD   HookProc,
    U032    DeviceHandle
)
{
    RMPARAMS rmParams;

    //
    // Hook display mode changes.
    //
    rmParams.RegEAX = NVRM_API_MODE_HOOK;
    rmParams.RegEBX = HookProc;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);

}
VOID FAR PASCAL NvSysGetHardwarePointers
(
    DWORD FAR *pNv1,
    DWORD FAR *pAlternate,
    DWORD FAR *pFramebuffer,
    DWORD FAR *pTimer,
    U032      DeviceHandle
)
{
    RMPARAMS rmParams;

    //
    // Get all the hardware pointers.
    //
    rmParams.RegEAX = NVRM_API_GET_NV_ADDRESS;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    if (pNv1)         *pNv1         = rmParams.RegEAX;
    if (pAlternate)   *pAlternate   = rmParams.RegEAX + 0x00618000;
    if (pTimer)       *pTimer       = rmParams.RegEAX + 0x00101400;
    rmParams.RegEAX = NVRM_API_GET_FB_ADDRESS;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    if (pFramebuffer) *pFramebuffer = rmParams.RegEAX;

}
VOID FAR PASCAL NvSysGetCursorEmulatePosPointers
(
    DWORD FAR *pX,
    DWORD FAR *pY,
    U032      DeviceHandle
)
{
#ifdef SW_CURSOR
    //
    // Return pointers to current emulated cursor position.
    //    
    if (pX) *pX = (DWORD)&dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CursorSaveUnderPosX;
    if (pY) *pY = (DWORD)&dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CursorSaveUnderPosY;
#else
    if (pX) *pX = (DWORD)&dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorPosX;
    if (pY) *pY = (DWORD)&dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].CursorPosY;
#endif // SW_CURSOR
    
}
DWORD FAR PASCAL NvSysGetCurrentBufferFlatPointer
(
    U032      DeviceHandle
)
{
    DWORD returnvalue;

    //
    // Return pointer to current buffer value.  This is the physical front buffer, only use
    // for direct frambeffer access.
    //
    returnvalue = (DWORD)GetSelectorBase((UINT)(((DWORD)dll16_NvInfoPtrTable[DeviceHandle-1] >> 16) + (DWORD)&(dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Current) - (DWORD)dll16_NvInfoPtrTable[DeviceHandle-1]));

    return(returnvalue);
}
                               
//---------------------------------------------------------------------------
//
//  Nv system client entrypoints.
//
//---------------------------------------------------------------------------

BOOL FAR PASCAL NvSysAllocPages
(
    LPVOID FAR *Addr,
    LPDWORD     Handle,
    DWORD       Size,
    U032        DeviceHandle
)
{
    RMPARAMS   rmParams;

    rmParams.RegEAX = NVRM_API_ALLOC_PAGES;
    rmParams.RegECX = Size;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    *Addr   = (LPVOID)(rmParams.RegEDX);
    *Handle = rmParams.RegEAX;

    return (rmParams.RegEAX ? FALSE : TRUE);
}
BOOL FAR PASCAL NvSysFreePages
(
    DWORD       Handle,
    U032        DeviceHandle
)
{
    RMPARAMS   rmParams;

    rmParams.RegEAX = NVRM_API_FREE_PAGES;
    rmParams.RegECX = Handle;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);

    return (rmParams.RegEAX ? FALSE : TRUE);
}
BOOL FAR PASCAL NvSysLockPages
(
    WORD        Selector,
    DWORD       Offset,
    DWORD       Size,
    U032        DeviceHandle
)
{
    RMPARAMS   rmParams;

    rmParams.RegEAX = NVRM_API_LOCK_PAGES;
    rmParams.RegEBX = Selector;
    rmParams.RegECX = Offset;
    rmParams.RegEDX = Size;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);

    return (rmParams.RegEAX);
}
BOOL FAR PASCAL NvSysUnlockPages
(
    WORD        Selector,
    DWORD       Offset,
    DWORD       Size,
    U032        DeviceHandle
)
{
    RMPARAMS   rmParams;

    rmParams.RegEAX = NVRM_API_UNLOCK_PAGES;
    rmParams.RegEBX = Selector;
    rmParams.RegECX = Offset;
    rmParams.RegEDX = Size;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);

    return (rmParams.RegEAX);
}
//---------------------------------------------------------------------------
//
//  Thunked entrypoints from Win32 API DLL.
//
//---------------------------------------------------------------------------
BOOL PASCAL NvThunkAllocPages
(
    DWORD   FlatBase,
    LPDWORD AddrSize,
    LPDWORD Handle
)
{
    RMPARAMS   rmParams;

    rmParams.RegEAX = NVRM_API_ALLOC_PAGES;
    rmParams.RegECX = *AddrSize;
    rmParams.RegES  = 0;
    rmParams.RegESI = DEFAULT_DEVICEHANDLE;
    CallRM_DeviceSetup(&rmParams);
    *AddrSize = rmParams.RegEDX - FlatBase;
    *Handle   = rmParams.RegEAX;
    return (rmParams.RegEAX ? FALSE : TRUE);
}
BOOL PASCAL NvThunkFreePages
(
    DWORD Handle
)
{
    RMPARAMS   rmParams;

    rmParams.RegEAX = NVRM_API_FREE_PAGES;
    rmParams.RegECX = Handle;
    rmParams.RegES  = 0;
    rmParams.RegESI = DEFAULT_DEVICEHANDLE;
    CallRM_DeviceSetup(&rmParams);
    return (rmParams.RegEAX ? FALSE : TRUE);
}

DWORD PASCAL NvThunkNvArch
(
    DWORD pParameters_base,
    DWORD pParameters,
    DWORD Function
)
{
  DWORD    pParameters_flat_offset;
  RMPARAMS rmParams;

  pParameters_flat_offset = (DWORD) ((DWORD) pParameters_base + (DWORD) pParameters);

  rmParams.RegEAX = NVRM_API_ARCHITECTURE_32;
  rmParams.RegES  = 0;
  rmParams.RegEBX = Function;
  rmParams.RegECX = pParameters_flat_offset;
  rmParams.RegESI = DEFAULT_DEVICEHANDLE;
  CallRM_DeviceSetup(&rmParams);
  return (rmParams.RegEAX);
}

//---------------------------------------------------------------------------
//
//  Internal routines.
//
//---------------------------------------------------------------------------

// return raster position on nv4/nv10
DWORD PASCAL NvGetRasterPosition
(
    U032        DeviceHandle
)
{
    DWORD        pPCRTC_RASTER;

    pPCRTC_RASTER = ((DWORD)NvDeviceSelectorGet(NV_DEV_PCRTC, DeviceHandle) << 16)
                        | ((DWORD)NV_PCRTC_RASTER - (DWORD)NV_PCRTC_INTR_0);
    return (REG_RD32(pPCRTC_RASTER) & 0x7FF);
}

//---------------------------------------------------------------------------
//
// Configuration API.
//
//---------------------------------------------------------------------------

DWORD FAR PASCAL NvConfigVersion
(
    U032        DeviceHandle
)
{
    RMPARAMS   rmParams;
    CHAR       Title[256];
    CHAR       Message[512];

    rmParams.RegEAX = NVRM_API_GET_VERSION;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    if (rmParams.RegEAX != NVRM_VERSION)
    {
        LoadString(hInstNvSysDLL, NVSYS_STR_RM_ERROR, Title, 256);
        LoadString(hInstNvSysDLL, NVSYS_STR_VERSION_ERROR, Message, 512);
        MessageBox(0, Message, Title, MB_OK);
    }

    return (rmParams.RegEAX);
}
DWORD FAR PASCAL NvConfigGet
(
    DWORD       Index,
    U032        DeviceHandle
)
{
    RMINFO FAR *pRmInfo;
    U016	RmInfoSel = 0;
    U032	pv;

    DWORD        Value;
    U008         lock;
    DWORD        pPRMCIO_INDEX;
    DWORD        pPRMCIO_DATA;
    RMPARAMS     rmParams;

    Value = 0;

    // 
    // Special case the CRTC access since we're using the lower byte of
    // the index to denote the register we want to view
    //
    if ((Index & 0xF00) == NV_CFG_CRTC)
    {
        //
        // Build all the selectors we need: 3C4, 3C5, 3D4, 3D5
        //
        pPRMCIO_INDEX = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                         | ((DWORD)NV_PRMCIO_CRX__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
        pPRMCIO_DATA  = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                         | ((DWORD)NV_PRMCIO_CR__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));

        //
        // Make sure the extended CRTC registers are enabled
        //
        REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
        lock = REG_RD08(pPRMCIO_DATA);                      // 3D5
        REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
        REG_WR08(pPRMCIO_DATA, NV_CIO_SR_UNLOCK_RW_VALUE);  // 3D5

        //
        // Get the register value.
        //
        REG_WR08(pPRMCIO_INDEX, Index & 0xFF);              // 3D4
        Value = REG_RD08(pPRMCIO_DATA);                     // 3D5
        
        //
        // Restore the lock
        //
        if (lock == 0)
        {        
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            REG_WR08(pPRMCIO_DATA, NV_CIO_SR_LOCK_VALUE);       // 3D5
        }

        return (Value);
    }

    if(IsNvCfgAgp(Index))
    {
	pv = (U032)(dll16_NvInfoPtrTable[DeviceHandle-1]->pRmInfo);
	RmInfoSel = GetSelector(pv,sizeof(RMINFO));
	pRmInfo = (RMINFO FAR*)(((U032)RmInfoSel) << 16); 
    }
    
    switch (Index)
    {
        //
        // These values actually look at hardware, not NvInfo.
        //
        case NV_CFG_VBLANK_STATUS:

        // do the scanline method

            //
            // Build all the selectors we need: 3C4, 3C5, 3D4, 3D5
            //
            pPRMCIO_INDEX = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                      | ((DWORD)NV_PRMCIO_CRX__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
            pPRMCIO_DATA  = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                      | ((DWORD)NV_PRMCIO_CR__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
            //
            // Make sure the extended CRTC registers are enabled
            //
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            lock = REG_RD08(pPRMCIO_DATA);                      // 3D5
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            REG_WR08(pPRMCIO_DATA, NV_CIO_SR_UNLOCK_RW_VALUE);  // 3D5

            // Check whether in power saving mode. Return dummy status if so, or else direct draw will hang
            // waiting for vlank to change.
            REG_WR08(pPRMCIO_INDEX, NV_CIO_CRE_RPC1_INDEX);   // 3D4 = scanline hi byte
            Value = 0xC0 & REG_RD08(pPRMCIO_DATA);            // 3D5 bits 7&6 are hsync & vsync inactive
            if (Value)                                        // in power saving mode if either is set
                Value = (VblankDummy ^= 1);                   // just toggle, so we are sure to get both states
            else
            {
                Value = NvGetRasterPosition(DeviceHandle);
                if (Value == 0) // make sure we don't have false count. NV3 seems to have glitch during display period.
                {
                    Value = NvGetRasterPosition(DeviceHandle);
                }
                if (Value == 0)
                    Value = 1;
                else
                    Value = 0;            
            } // if in power saving mode            
            //
            // Restore the lock
            //
            if (lock == 0)
            {        
                REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
                REG_WR08(pPRMCIO_DATA, NV_CIO_SR_LOCK_VALUE);       // 3D5
            }
            break;
        case NV_CFG_GE_STATUS:
            {
                DWORD pGEStatus;
                pGEStatus = ((DWORD)NvDeviceSelectorGet(NV_DEV_PGRAPH,DeviceHandle) << 16)
                          | ((DWORD)NV_PGRAPH_STATUS - (DWORD)DEVICE_BASE(NV_PGRAPH));
                Value = REG_RD32(pGEStatus);
            }
            break;
        case NV_CFG_CURRENT_SCANLINE:
            //
            // Build all the selectors we need: 3C4, 3C5, 3D4, 3D5
            //
            pPRMCIO_INDEX = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                      | ((DWORD)NV_PRMCIO_CRX__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
            pPRMCIO_DATA  = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                      | ((DWORD)NV_PRMCIO_CR__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
            //
            // Make sure the extended CRTC registers are enabled
            //
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            lock = REG_RD08(pPRMCIO_DATA);                      // 3D5
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            REG_WR08(pPRMCIO_DATA, NV_CIO_SR_UNLOCK_RW_VALUE);  // 3D5

            //
            // Get the scanline value.
            //
            Value = NvGetRasterPosition(DeviceHandle);
            if (Value == 0) // make sure we don't have false 0 count
            {
                Value = NvGetRasterPosition(DeviceHandle);
            }
            
            //
            // Restore the lock
            //
            if (lock == 0)
            {        
                REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
                REG_WR08(pPRMCIO_DATA, NV_CIO_SR_LOCK_VALUE);       // 3D5
            }
            break;
        //
        // Fields in NvInfo.
        //
        case NV_CFG_MANUFACTURER:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.Manufacturer;
            break;
        case NV_CFG_ARCHITECTURE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.Architecture;
            break;
        case NV_CFG_REVISION:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.MaskRevision;
            break;
        case NV_CFG_BIOS_REVISION:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.BiosRevision;
            break;
        case NV_CFG_BUS_TYPE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.Bus;
            break;
        case NV_CFG_CRYSTAL_FREQ:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.CrystalFreq;
            break;
        case NV_CFG_ADDRESS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.PhysAddr;
            break;
        case NV_CFG_IRQ:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.IntLine;
            break;
        case NV_CFG_IO_NEEDED:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.doIOToFlushCache;
            break;
        case NV_CFG_PCI_ID:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.PCIDeviceID;
            break;
        case NV_CFG_PCI_SUB_ID:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.PCISubDeviceID;
            break;
        case NV_CFG_GRAPHICS_CAPS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Graphics.Capabilities;
            break;
        case NV_CFG_INSTANCE_TOTAL:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Pram.HalInfo.TotalInstSize;
            break;
        case NV_CFG_INSTANCE_SIZE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Pram.FreeInstSize;
            break;
        case NV_CFG_FIFO_COUNT:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Fifo.HalInfo.Count;
            break;
        case NV_CFG_FIFO_USE_COUNT:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Fifo.HalInfo.AllocateCount;
            break;
        case NV_CFG_RAM_SIZE_MB:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamSizeMb;
            break;
        case NV_CFG_RAM_SIZE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamSize;
            break;
        case NV_CFG_RAM_TYPE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamType;
            break;
        case NV_CFG_AGP_PHYS_BASE:
			Value = pRmInfo->AGP.AGPPhysStart;
            break;
        case NV_CFG_AGP_LINEAR_BASE:
			Value = pRmInfo->AGP.AGPLinearStart;
            break;
        case NV_CFG_AGP_LIMIT:
			Value = pRmInfo->AGP.AGPLimit;
            break;
        case NV_CFG_AGP_HEAP_FREE:
            Value = pRmInfo->AGP.AGPHeapFree;
            break;
        case NV_CFG_AGP_FW_ENABLE:
            Value = pRmInfo->AGP.AGPFWEnable;
            break;
        case NV_CFG_AGP_FULLSPEED_FW:
            Value = pRmInfo->AGP.AGPFullSpeedFW;
            break;
        case NV_CFG_SCREEN_WIDTH:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.HorizDisplayWidth;
            break;
        case NV_CFG_SCREEN_HEIGHT:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.VertDisplayWidth;
            break;
        case NV_CFG_PIXEL_DEPTH:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.Depth;
            break;
        case NV_CFG_PRIMARY_SURFACE_PITCH:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.DisplayPitch;
            break;
        case NV_CFG_SCREEN_MAX_WIDTH:
            {
                // This value depends on the display type.
                // We really should read the EDID of the display device to get the max.
                // For now hard code the values.
                switch (GETMONITORTYPE(dll16_NvInfoPtrTable[DeviceHandle-1], 0))
                {
                    case MONITOR_TYPE_VGA:
                        Value = 2048;
                        break;
                    case MONITOR_TYPE_NTSC:
                        Value = 640;
                        break;
                    case MONITOR_TYPE_PAL:
                        Value = 800;
                        break;
                    case MONITOR_TYPE_FLAT_PANEL:
                        Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.fpHMax;
                        break;
                    default:
                        Value = 0;
                        break;
                } // switch on MonitorType
            }
            break;
        case NV_CFG_SCREEN_MAX_HEIGHT:
            {
                // This value depends on the display type.
                // We really should read the EDID of the display device to get the max.
                // For now hard code the values.
                switch (GETMONITORTYPE(dll16_NvInfoPtrTable[DeviceHandle-1], 0))
                {
                    case MONITOR_TYPE_VGA:
                        Value = 1536;
                        break;
                    case MONITOR_TYPE_NTSC:
                        Value = 480;
                        break;
                    case MONITOR_TYPE_PAL:
                        Value = 600;
                        break;
                    case MONITOR_TYPE_FLAT_PANEL:
                        Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.fpVMax;
                        break;
                    default:
                        Value = 0;
                        break;
                } // switch on MonitorType
            }
            break;
        case NV_CFG_VIDEO_ENCODER_TYPE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.EncoderType;
            break;
        case NV_CFG_VIDEO_ENCODER_ID:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.EncoderID;
            break;
		case NV_CFG_BIOS_DEFAULT_TV_TYPE:
			// Build selectors to access CRTC
	        pPRMCIO_INDEX = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                         | ((DWORD)NV_PRMCIO_CRX__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
    	    pPRMCIO_DATA  = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                         | ((DWORD)NV_PRMCIO_CR__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
			// Unlock ext CRTC
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4 <-1f
            lock = REG_RD08(pPRMCIO_DATA);                      // 3D5
            REG_WR08(pPRMCIO_DATA, NV_CIO_SR_UNLOCK_RW_VALUE);  // 3D5 <-57

            REG_WR08(pPRMCIO_INDEX, NV_CIO_CRE_SCRATCH0__INDEX);// 3D4 <-2B
            Value = REG_RD08(pPRMCIO_DATA) & 0x7;               // 3D5 get BIOS values (defined in nvcm.h)
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4 <-1f
            REG_WR08(pPRMCIO_DATA, lock);  						// 3D5 <-former value
			break;
        case NV_CFG_VIDEO_DISPLAY_TYPE:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_GET_DISPLAY_TYPE;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            Value = rmParams.RegEAX;
            break;
        case NV_CFG_VIDEO_MONITOR_TYPE:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_GET_MONITOR_TYPE;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            Value = rmParams.RegEAX;
//            Value = GETMONITORTYPE(dll16_NvInfoPtrTable[DeviceHandle-1], 0);
            break;
        case NV_CFG_VIDEO_OVERLAY_ALLOWED:            
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Video.HalInfo.OverlayAllowed;
            break;
        case NV_CFG_VIDEO_DOWNSCALE_RATIO_768:            
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Video.HalInfo.OverlayMaxDownscale_768;
            break;
        case NV_CFG_VIDEO_DOWNSCALE_RATIO_1280:            
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Video.HalInfo.OverlayMaxDownscale_1280;
            break;
        case NV_CFG_VIDEO_DOWNSCALE_RATIO_1920:            
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Video.HalInfo.OverlayMaxDownscale_1920;
            break;
        case NV_CFG_VIDEO_REFRESH_RATE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RefreshRate;
            break;
        case NV_CFG_FRAMEBUFFER_UNDERSCAN_X:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_x;
            break;
        case NV_CFG_FRAMEBUFFER_UNDERSCAN_Y:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_y;
            break;
        case NV_CFG_VBLANK_TOGGLE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankToggle;
            break;
        case NV_CFG_VBLANK_COUNTER:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankCounter;
            break;
        case NV_CFG_PRAM_CURRENT_SIZE:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Pram.HalInfo.CurrentSize;
            break;
        case NV_CFG_DAC_INPUT_WIDTH:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.InputWidth;
            break;
        case NV_CFG_DAC_PIXEL_CLOCK:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VClk;
            break;
        case NV_CFG_DAC_MEMORY_CLOCK:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MClk;
            break;
        case NV_CFG_DAC_GRAPHICS_CLOCK:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.NVClk;
            break;
        case NV_CFG_DAC_VPLL_M:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllM;
            break;
        case NV_CFG_DAC_VPLL_N:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllN;
            break;
        case NV_CFG_DAC_VPLL_O:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllO;
            break;
        case NV_CFG_DAC_VPLL_P:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllP;
            break;
        case NV_CFG_DAC_MPLL_M:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllM;
            break;
        case NV_CFG_DAC_MPLL_N:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllN;
            break;
        case NV_CFG_DAC_MPLL_O:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllO;
            break;
        case NV_CFG_DAC_MPLL_P:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllP;
            break;
        case NV_CFG_DAC_PCLK_LIMIT_8BPP:
            Value = GETCRTCHALINFO(dll16_NvInfoPtrTable[DeviceHandle-1], 0, PCLKLimit8bpp);
            break;            
        case NV_CFG_DAC_PCLK_LIMIT_16BPP:
            Value = GETCRTCHALINFO(dll16_NvInfoPtrTable[DeviceHandle-1], 0, PCLKLimit16bpp);
            break;            
        case NV_CFG_DAC_PCLK_LIMIT_32BPP:
            Value = GETCRTCHALINFO(dll16_NvInfoPtrTable[DeviceHandle-1], 0, PCLKLimit32bpp);
            break;            
        case NV_CFG_MAPPING_BUSDEVICEFUNC:
            rmParams.RegEAX = NVRM_API_GET_DEVICE_TYPE;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            Value = (dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.nvBusDeviceFunc << 8) | rmParams.RegEAX;
            break;
        case NV_CFG_FLAT_PANEL_CONNECT_16:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_GET_FP_CONNECT_STATUS;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            Value = rmParams.RegEAX;
            break;
        case NV_CFG_FLAT_PANEL_CONFIG_16:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_GET_FP_CONFIG;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            Value = rmParams.RegEAX;
            break;
        case NV_CFG_NUMBER_OF_HEADS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.NumCrtcs;
            break;
        case NV_CFG_INITIAL_BOOT_HEAD:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.InitialBootHead;
            break;
        case NV_CFG_DEVICE_HANDLE:
            {
                PWIN9XHWINFO    pOsHwInfo = (PWIN9XHWINFO) dll16_NvInfoPtrTable[DeviceHandle-1]->pOsHwInfo;
                Value = pOsHwInfo->osDeviceHandle;
            }
            break;
#ifdef RM_STATS
        case NV_STAT_INTERVAL_NSEC_LO:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.UpdateLo;
            break;
        case NV_STAT_INTERVAL_NSEC_HI:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.UpdateHi;
            break;
        case NV_STAT_FIFO_EXCEPTIONS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.FifoExceptionCount;
            break;
        case NV_STAT_FRAMEBUFFER_EXCEPTIONS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.FramebufferExceptionCount;
            break;
        case NV_STAT_GRAPHICS_EXCEPTIONS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.GraphicsExceptionCount;
            break;
        case NV_STAT_TIMER_EXCEPTIONS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.TimerExceptionCount;
            break;
        case NV_STAT_CHIP_EXCEPTIONS:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.TotalExceptionCount;
            break;
        case NV_STAT_MEM_ALLOCATED:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.MemAlloced;
            break;
        case NV_STAT_MEM_LOCKED:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.MemLocked;
            break;
        case NV_STAT_EXEC_NSEC_LO:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.ExecTimeLo;
            break;
        case NV_STAT_EXEC_NSEC_HI:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.ExecTimeHi;
            break;
        case NV_STAT_INT_NSEC_LO:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.IntTimeLo;
            break;
        case NV_STAT_INT_NSEC_HI:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.IntTimeHi;
            break;
#endif
        case NV_CFG_GET_ALL_DEVICES:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.DevicesBitMap;
            break;
        case NV_CFG_GET_BOOT_DEVICES:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.BootDevicesBitMap;
            break;
    }

    if(RmInfoSel)
	FreeSelector(RmInfoSel);

    return (Value);
}
DWORD FAR PASCAL NvConfigSet
(
    DWORD       Index,
    DWORD       NewValue,
    U032        DeviceHandle
)
{
    RMINFO FAR *pRmInfo;
    U016 RmInfoSel = 0;
    U032 pv;

    DWORD OldValue, Value;
    U008  lock;
    DWORD pPRMCIO_INDEX;
    DWORD pPRMCIO_DATA;
    RMPARAMS     rmParams;
    NV_CFG_STEREO_PARAMS FAR *pStereoParams;

    OldValue = 0;

    // 
    // Special case the CRTC access since we're using the lower byte of
    // the index to denote the register we want to update
    //
    if ((Index & 0xF00) == NV_CFG_CRTC)
    {
        //
        // Build all the selectors we need: 3C4, 3C5, 3D4, 3D5
        //
        pPRMCIO_INDEX = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                  | ((DWORD)NV_PRMCIO_CRX__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
        pPRMCIO_DATA  = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                  | ((DWORD)NV_PRMCIO_CR__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
    
        //
        // Make sure the extended CRTC registers are enabled
        //
        REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
        lock = REG_RD08(pPRMCIO_DATA);                      // 3D5
        REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
        REG_WR08(pPRMCIO_DATA, NV_CIO_SR_UNLOCK_RW_VALUE);  // 3D5

        //
        // Set the register value.
        //
        REG_WR08(pPRMCIO_INDEX, Index & 0xFF);
        OldValue = REG_RD08(pPRMCIO_DATA);
        REG_WR08(pPRMCIO_DATA, NewValue & 0xFF);
        
        //
        // Restore the lock
        //
        if (lock == 0)
        {        
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            REG_WR08(pPRMCIO_DATA, NV_CIO_SR_LOCK_VALUE);       // 3D5
        }

        return (OldValue);
    }

    if(IsNvCfgAgp(Index))
    {
	pv = (U032)(dll16_NvInfoPtrTable[DeviceHandle-1]->pRmInfo);
	RmInfoSel = GetSelector(pv,sizeof(RMINFO));
	pRmInfo = (RMINFO FAR*)(((U032)RmInfoSel) << 16); 
    }
    
    switch (Index)
    {
        //
        // Theses values actually look at hardware, not NvInfo.
        //
        case NV_CFG_VBLANK_STATUS:
            //
            //  Set OldValue = current value. No idea if this is used.
            //
            //
            // Build all the selectors we need: 3C4, 3C5, 3D4, 3D5
            //
            pPRMCIO_INDEX = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                      | ((DWORD)NV_PRMCIO_CRX__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));
            pPRMCIO_DATA  = ((DWORD)NvDeviceSelectorGet(NV_DEV_PRMCIO,DeviceHandle) << 16)
                      | ((DWORD)NV_PRMCIO_CR__COLOR - (DWORD)DEVICE_BASE(NV_PRMCIO));

            //
            // Make sure the extended CRTC registers are enabled
            //
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            lock = REG_RD08(pPRMCIO_DATA);                      // 3D5
            REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
            REG_WR08(pPRMCIO_DATA, NV_CIO_SR_UNLOCK_RW_VALUE);  // 3D5

            //
            // Get the scanline value.
            //
            Value = NvGetRasterPosition(DeviceHandle);
            // return boolean for (scanline == 0)
            if (Value == 0)
                OldValue = 1;
            else
                OldValue = 0;            
            //
            // Restore the lock
            //
            if (lock == 0)
            {        
                REG_WR08(pPRMCIO_INDEX, NV_CIO_SR_LOCK_INDEX);      // 3D4
                REG_WR08(pPRMCIO_DATA, NV_CIO_SR_LOCK_VALUE);       // 3D5
            }
            
            break;
        case NV_CFG_GE_STATUS:
            {
                DWORD pGEStatus;
                pGEStatus = ((DWORD)NvDeviceSelectorGet(NV_DEV_PGRAPH,DeviceHandle) << 16)
                          | ((DWORD)NV_PGRAPH_STATUS - (DWORD)DEVICE_BASE(NV_PGRAPH));
                OldValue = REG_RD32(pGEStatus);
            }
            break;
        //
        // Fields in NvInfo.
        //
        case NV_CFG_MANUFACTURER:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.Manufacturer;
            break;
        case NV_CFG_ARCHITECTURE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.Architecture;
            break;
        case NV_CFG_REVISION:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.Revision;
            break;
        case NV_CFG_BIOS_REVISION:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.BiosRevision;
            break;
        case NV_CFG_IMPLEMENTATION:
            Value = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.HalInfo.Implementation;
            break;
        case NV_CFG_BUS_TYPE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Chip.Bus;
            break;
        case NV_CFG_ADDRESS:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.PhysAddr;
            break;
        case NV_CFG_IRQ:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.IntLine;
            break;
        case NV_CFG_INSTANCE_SIZE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Pram.FreeInstSize;
            break;
        case NV_CFG_FIFO_COUNT:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Fifo.HalInfo.Count;
            break;
        case NV_CFG_FIFO_USE_COUNT:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Fifo.HalInfo.AllocateCount;
            break;
        case NV_CFG_RAM_SIZE_MB:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamSizeMb;
            break;
        case NV_CFG_RAM_SIZE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamSize;
            break;
        case NV_CFG_RAM_TYPE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RamType;
            break;
        case NV_CFG_AGP_PHYS_BASE:
            OldValue = pRmInfo->AGP.AGPPhysStart;
            pRmInfo->AGP.AGPPhysStart = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;
            break;
        case NV_CFG_AGP_LINEAR_BASE:
            OldValue = pRmInfo->AGP.AGPLinearStart;
            pRmInfo->AGP.AGPLinearStart = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;
            break;
        case NV_CFG_AGP_LIMIT:
            OldValue = pRmInfo->AGP.AGPLimit;
            pRmInfo->AGP.AGPLimit = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Mapping.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;
            break;

        case NV_CFG_AGP_LOAD_DUMMY_PAGES:
            rmParams.RegEAX = NVRM_AGP_LOAD_DUMMY_PAGES;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;

        case NV_CFG_SCREEN_WIDTH:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.HorizDisplayWidth;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.HorizDisplayWidth = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;
            break;
        case NV_CFG_SCREEN_HEIGHT:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.VertDisplayWidth;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.VertDisplayWidth = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;
            break;
        case NV_CFG_PIXEL_DEPTH:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.Depth;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.Depth = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;
            break;
        case NV_CFG_VIDEO_DISPLAY_TYPE:
            OldValue = GETDISPLAYTYPE(dll16_NvInfoPtrTable[DeviceHandle-1], 0);
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_SET_DISPLAY_TYPE;
            rmParams.RegECX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NV_CFG_VIDEO_MONITOR_TYPE:
            OldValue = GETMONITORTYPE(dll16_NvInfoPtrTable[DeviceHandle-1], 0);
            SETMONITORTYPE(dll16_NvInfoPtrTable[DeviceHandle-1], 0, NewValue);
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags  |= UPDATE_HWINFO_DISPLAY_PARAMS;
            break;
        case NV_CFG_VIDEO_REFRESH_RATE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RefreshRate;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.RefreshRate     = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HorizFrontPorch = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HorizSyncWidth  = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HorizBackPorch  = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.VertFrontPorch  = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.VertSyncWidth   = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.VertBackPorch   = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VClk            = 0;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_DISPLAY_PARAMS;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_BUFFER_PARAMS;       // kick a mode switch
            //
            // fb will reset the pixel clock, so we need to reload the VPLL
            //
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.UpdateFlags  |= UPDATE_HWINFO_DAC_VPLL;
            //
            // Allow Canopus to set the refresh rate and pull the
            // resolution from the registry (but not the refresh rate)
            // in only one mode set.  Yuck.  But until I can export all
            // modeset code...
            //
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_REFRESH_PARAMS;
            break;
        case NV_CFG_FRAMEBUFFER_UNDERSCAN_X:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_x;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_x = NewValue;
            // Recalculate the scale factor
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Scale_x = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.HorizDisplayWidth << 20;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Scale_x /= dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_x;
            break;
        case NV_CFG_FRAMEBUFFER_UNDERSCAN_Y:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_y;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_y = NewValue;
            // Recalculate the scale factor
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Scale_y = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.VertDisplayWidth << 20;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Scale_y /= dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.Underscan_y;
            break;
        case NV_CFG_VBLANK_TOGGLE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankToggle;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankToggle = NewValue;
            if (dll16_NvInfoPtrTable[DeviceHandle-1]->Vga.Enabled)
                dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankToggle = 0;
            break;
        case NV_CFG_VBLANK_COUNTER:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankCounter;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.CrtcInfo[0].VBlankCounter = NewValue;
            break;
        case NV_CFG_PRAM_CURRENT_SIZE:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Pram.HalInfo.CurrentSize;
            break;
        case NV_CFG_DAC_INPUT_WIDTH:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.InputWidth;
            break;
        case NV_CFG_DAC_PIXEL_CLOCK:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VClk;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VClk = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.UpdateFlags  |= UPDATE_HWINFO_DAC_VPLL;
            //
            // set DISPLAY flag as this parameter may be stored in EEPROM
            //
            dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags |= UPDATE_HWINFO_DISPLAY_PARAMS;
            break;
        case NV_CFG_DAC_MEMORY_CLOCK:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MClk;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MClk = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.UpdateFlags  |= UPDATE_HWINFO_DAC_MPLL;
            break;
        case NV_CFG_DAC_GRAPHICS_CLOCK:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.NVClk;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.NVClk = NewValue;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.UpdateFlags  |= UPDATE_HWINFO_DAC_NVPLL;
            break;
        case NV_CFG_DAC_VPLL_M:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllM;
            break;
        case NV_CFG_DAC_VPLL_N:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllN;
            break;
        case NV_CFG_DAC_VPLL_O:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllO;
            break;
        case NV_CFG_DAC_VPLL_P:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.VPllP;
            break;
        case NV_CFG_DAC_MPLL_M:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllM;
            break;
        case NV_CFG_DAC_MPLL_N:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllN;
            break;
        case NV_CFG_DAC_MPLL_O:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllO;
            break;
        case NV_CFG_DAC_MPLL_P:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Dac.HalInfo.MPllP;
            break;
        case NV_CFG_REGISTRY_STRING:
            rmParams.RegEAX = NVRM_REGISTRY_SET_STRING;
            rmParams.RegEBX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NV_CFG_FRAMECAPTURE_CALLBACK:			// Callback for the VFW capture driver
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->MediaPort.ImageCallback;
            dll16_NvInfoPtrTable[DeviceHandle-1]->MediaPort.ImageCallback = NewValue;
            break;
        case NV_CFG_FLAT_PANEL_CONFIG_16:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_GET_FP_CONFIG;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            OldValue = rmParams.RegEAX;
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_SET_FP_CONFIG;
            rmParams.RegECX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NV_CFG_DISPLAY_CHANGE_CONFIG:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_CHANGE_CONFIG;
            rmParams.RegECX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NV_CFG_DISPLAY_CHANGE_START:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_CHANGE_START;
            rmParams.RegECX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NV_CFG_DISPLAY_CHANGE_END:
            rmParams.RegEAX = NVRM_API_DISPLAY;
            rmParams.RegEBX = NVRM_API_DISPLAY_CHANGE_END;
            rmParams.RegECX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP:
            rmParams.RegEAX = NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP;
//            rmParams.RegEBX = NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP;
            rmParams.RegECX = NewValue;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            break;
        case NV_CFG_STEREO_CONFIG:
            pStereoParams = (NV_CFG_STEREO_PARAMS FAR *)NewValue;
            if (pStereoParams->Flags == STEREOCFG_EXIT_ADJACENT)
            {
                dll16_NvInfoPtrTable[DeviceHandle-1]->StereoFlags &= ~STEREO_ADJACENT_EYES;
            } else
            {
                if (pStereoParams->Flags == STEREOCFG_ENTER_ADJACENT)
                {
                    dll16_NvInfoPtrTable[DeviceHandle-1]->StereoFlags |= STEREO_ADJACENT_EYES;
                } 
            }        
            OldValue = (DWORD)dll16_NvInfoPtrTable[DeviceHandle-1]->pStereoParams;
            break;

#ifdef RM_STATS
        case NV_STAT_INTERVAL_NSEC_LO:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.UpdateLo;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.UpdateLo = NewValue;
            break;
        case NV_STAT_INTERVAL_NSEC_HI:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.UpdateHi;
            dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.UpdateHi = NewValue;
            break;
        case NV_STAT_FIFO_EXCEPTIONS:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.FifoExceptionCount;
            break;
        case NV_STAT_FRAMEBUFFER_EXCEPTIONS:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.FramebufferExceptionCount;
            break;
        case NV_STAT_GRAPHICS_EXCEPTIONS:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.GraphicsExceptionCount;
            break;
        case NV_STAT_TIMER_EXCEPTIONS:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.TimerExceptionCount;
            break;
        case NV_STAT_CHIP_EXCEPTIONS:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.TotalExceptionCount;
            break;
        case NV_STAT_MEM_ALLOCATED:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.MemAlloced;
            break;
        case NV_STAT_MEM_LOCKED:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.MemLocked;
            break;
        case NV_STAT_EXEC_NSEC_LO:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.ExecTimeLo;
            break;
        case NV_STAT_EXEC_NSEC_HI:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.ExecTimeHi;
            break;
        case NV_STAT_INT_NSEC_LO:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.IntTimeLo;
            break;
        case NV_STAT_INT_NSEC_HI:
            OldValue = dll16_NvInfoPtrTable[DeviceHandle-1]->Statistics.IntTimeHi;
            break;
#endif
    }

    if(RmInfoSel)
	FreeSelector(RmInfoSel);

    return (OldValue);
}
DWORD FAR PASCAL NvConfigUpdate
(
    DWORD       UpdateFlag,
    U032        DeviceHandle
)
{
    RMPARAMS rmParams;
    WORD     RepaintFlag;

    if (dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.UpdateFlags & UPDATE_HWINFO_BUFFER_PARAMS)
        RepaintFlag = TRUE;
    else
        RepaintFlag = FALSE;
    rmParams.RegEAX = NVRM_API_UPDATE_CONFIGURATION;
    rmParams.RegEBX = UpdateFlag;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    if (RepaintFlag)
    {
        //
        // Update selectors to framebuffer.
        //
        if (devSelectorTable[DeviceHandle-1].devSelector[NV_DEV_BUFFER_0])
        {
            rmParams.RegEAX = NVRM_API_MAP_SELECTOR;
            rmParams.RegEBX = NvDeviceBaseGet(NV_DEV_BUFFER_0,DeviceHandle);
            rmParams.RegECX = NvDeviceLimitGet(NV_DEV_BUFFER_0,DeviceHandle);
            rmParams.RegEDX = devSelectorTable[DeviceHandle-1].devSelector[NV_DEV_BUFFER_0];
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
        }
        RepaintScreen();
    }
    
    return (rmParams.RegEAX);
}
DWORD FAR PASCAL NvConfigVga
(
    DWORD       Mode,
    U032        DeviceHandle
)
{
    RMPARAMS rmParams;

    dll16_NvInfoPtrTable[DeviceHandle-1]->Vga.GlobalEnable = Mode;
    rmParams.RegEAX = NVRM_API_CONFIG_VGA;
    rmParams.RegEBX = Mode;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    if (!Mode)
    {
        //
        // Update selectors to framebuffer.
        //
        if (devSelectorTable[DeviceHandle-1].devSelector[NV_DEV_BUFFER_0])
        {
            rmParams.RegEAX = NVRM_API_MAP_SELECTOR;
            rmParams.RegEBX = NvDeviceBaseGet(NV_DEV_BUFFER_0,DeviceHandle);
            rmParams.RegECX = NvDeviceLimitGet(NV_DEV_BUFFER_0,DeviceHandle);
            rmParams.RegEDX = devSelectorTable[DeviceHandle-1].devSelector[NV_DEV_BUFFER_0];
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
        }
    }

    return (rmParams.RegEAX);
}

//---------------------------------------------------------------------------
//
// Device API.
//
//---------------------------------------------------------------------------

DWORD FAR PASCAL NvDeviceBaseGet
(
    DWORD       Index,
    DWORD       DeviceHandle
)
{
    DWORD    DeviceOffset;
    RMPARAMS rmParams;

    switch (Index)
    {
        case NV_DEV_BASE:
            DeviceOffset = DEVICE_BASE(NV_SPACE);
            break;
        case NV_DEV_BUFFER_0:
            DeviceOffset = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.Start[0];
            rmParams.RegEAX = NVRM_API_GET_FB_ADDRESS;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);
            
            return (rmParams.RegEAX + DeviceOffset);
            break;
        case NV_DEV_AGP:
            rmParams.RegEAX = NVRM_API_GET_AGP_ADDRESS;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);

            return (rmParams.RegEAX);
            break;
        case NV_DEV_GAMMA:
            rmParams.RegEAX = NVRM_API_GET_GAMMA_ADDRESS;
            rmParams.RegES  = 0;
            rmParams.RegESI = DeviceHandle-1;
            CallRM_DeviceSetup(&rmParams);

            return (rmParams.RegEAX);
            break;
        case NV_DEV_TIMER:
            DeviceOffset = NV_PTIMER_TIME_0;
            break;
        case NV_DEV_PFB:
            DeviceOffset = DEVICE_BASE(NV_PFB);
            break;
        case NV_DEV_PGRAPH:
            DeviceOffset = DEVICE_BASE(NV_PGRAPH);
            break;
        case NV_DEV_PRMCIO:
            DeviceOffset = DEVICE_BASE(NV_PRMCIO);
            break;
        case NV_DEV_PRMVIO:
            DeviceOffset = DEVICE_BASE(NV_PRMVIO);
            break;
        case NV_DEV_PRAMDAC:
            DeviceOffset = DEVICE_BASE(NV_PRAMDAC);
            break;
        case NV_DEV_PCRTC:
            // there's no limit:base define for NV_PCRTC, so use ...
            DeviceOffset = NV_PCRTC_INTR_0;
            break;
        default:

        return (0);
    }
    //
    // Get base pointer to the hardware.
    //
    rmParams.RegEAX = NVRM_API_GET_NV_ADDRESS;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    //
    // Return pointer to desired device.
    //
    
    return (rmParams.RegEAX + DeviceOffset);
}
DWORD FAR PASCAL NvDeviceLimitGet
(
    DWORD       Index,
    DWORD       DeviceHandle
)
{
    DWORD      DeviceLimit;

    switch (Index)
    {
        case NV_DEV_BASE:
            DeviceLimit = (1?NV_SPACE) - DEVICE_BASE(NV_SPACE);
            break;
        case NV_DEV_BUFFER_0:
            DeviceLimit = dll16_NvInfoPtrTable[DeviceHandle-1]->Framebuffer.HalInfo.Limit[0];
            break;
        case NV_DEV_TIMER:
            DeviceLimit = 7;
            break;
        case NV_DEV_GAMMA:
            DeviceLimit = 0x400;
            break;
        case NV_DEV_PFB:
            DeviceLimit = 0x0FFF;
            break;
        case NV_DEV_PGRAPH:
            DeviceLimit = 0x0FFF;
            break;
        case NV_DEV_PRMCIO:
            DeviceLimit = 0x0FFF;
            break;
        case NV_DEV_PRMVIO:
            DeviceLimit = 0x7FFF;
            break;
        case NV_DEV_PRAMDAC:
            DeviceLimit = DEVICE_EXTENT(NV_PRAMDAC) - DEVICE_BASE(NV_PRAMDAC);
            break;
        case NV_DEV_PCRTC:
            // there's no limit:base define for NV_PCRTC, so use ...
            DeviceLimit = ((NV_PCRTC_RASTER+4) - NV_PCRTC_INTR_0);
            break;
        default:
     
            return (0);
    }
    //
    // Return limit of desired device.
    //

    return (DeviceLimit);
}
WORD FAR PASCAL NvDeviceSelectorGet
(
    DWORD       Index,
    U032        DeviceHandle
)
{
    DWORD      DeviceBase;
    DWORD      DeviceLimit;
    RMPARAMS   rmParams;

    if (Index > NV_DEV_MAX)
    {    
        return (0);
    }        
    if (devSelectorTable[DeviceHandle-1].devSelector[Index])
    {    
        return (devSelectorTable[DeviceHandle-1].devSelector[Index]);
    }        
    DeviceBase  = NvDeviceBaseGet(Index,DeviceHandle);
    DeviceLimit = NvDeviceLimitGet(Index,DeviceHandle);
    //
    // Get selector for requested device.
    //
    rmParams.RegEAX = NVRM_API_MAP_SELECTOR;
    rmParams.RegEBX = DeviceBase;
    rmParams.RegECX = DeviceLimit;
    rmParams.RegEDX = 0;
    rmParams.RegES  = 0;
    rmParams.RegESI = DeviceHandle-1;
    CallRM_DeviceSetup(&rmParams);
    devSelectorTable[DeviceHandle-1].devSelector[Index] = (WORD) rmParams.RegEAX;
    
    return (devSelectorTable[DeviceHandle-1].devSelector[Index]);
}

void FAR PASCAL __loadds NvSysRegisterDisplayDriver(RMPARAMS FAR *prmParams)
{
    prmParams->RegEAX = NVRM_API_REGISTER_DISPLAY_DRIVER;
    prmParams->RegESI = 0;
    prmParams->RegES = 0;
    CallRM_DeviceSetup(prmParams);
}

void CallRM_DeviceSetup(LPRMPARAMS lprmParams)
{
    RMUSRAPI_STRUCT rmusrcall;
    WORD seg_,off_;
    DWORD seg_base,rmusrcall_flat_offset;
    
    rmusrcall.function = lprmParams->RegEAX;
    rmusrcall.device = lprmParams->RegESI;

    _asm push di
    _asm lea  di,rmusrcall
    _asm mov  off_,di
    _asm pop  di

	_asm mov  seg_,ss

    seg_base = GetSelectorBase((UINT) seg_);
    rmusrcall_flat_offset = seg_base+((DWORD) off_);
    lprmParams->RegEAX = rmusrcall_flat_offset;

    CallRM(lprmParams);
}

