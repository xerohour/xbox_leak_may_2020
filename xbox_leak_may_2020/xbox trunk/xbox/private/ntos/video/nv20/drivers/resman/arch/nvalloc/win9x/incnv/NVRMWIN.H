#ifndef _NVRMWIN_H_
#define _NVRMWIN_H_
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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
* Module: NVRMWIN.H                                                         *
*       Operating system wrapper functions used to abstract the OS.         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  OS specific defines.
//
//---------------------------------------------------------------------------

//
// Max values for tables.
//
#define MAX_REGISTER                    16
//#define NUM_CLIENTS                     16
//
// Messages to Resource Manager Windows client.
//
#ifndef WM_USER
#define WM_USER                         0x0400
#endif
#define NVRM_WM_ERROR                   WM_USER
#define NVRM_WM_OPEN_HWND               (WM_USER+0x10)
#define NVRM_WM_CLOSE_HWND              (WM_USER+0x11)
#define NVRM_WM_REDRAW_HWND             (WM_USER+0x12)
#define NVRM_WM_REDRAW_SCREEN           (WM_USER+0x13)
//
// IOCTL Interface functions. 
//
#define NVRM_IOCTL_BASE 0x10
#define NVRM_IOCTL_CRTC_GET   (NVRM_IOCTL_BASE+0)
#define NVRM_IOCTL_CRTC_SET   (NVRM_IOCTL_BASE+1)
#define NVRM_IOCTL_DUMPNVDATA (NVRM_IOCTL_BASE+2)
#define NVRM_IOCTL_NV_READ    (NVRM_IOCTL_BASE+3)
#define NVRM_IOCTL_NV_WRITE   (NVRM_IOCTL_BASE+4)
#define NVRM_IOCTL_NV_ARCH    (NVRM_IOCTL_BASE+5)
#define NVRM_IOCTL_CRTC_TEST  (NVRM_IOCTL_BASE+6)
//
// API services to Resource Manager VxD.
//
#define NVRM_API_BASE                    1000
#define NVRM_API_GET_VERSION             (NVRM_API_BASE+0)
#define NVRM_API_SET_CANVAS              (NVRM_API_BASE+10)
#define NVRM_API_SET_CANVAS_RECT         (NVRM_API_BASE+11)
#define NVRM_API_SET_CANVAS_CLIP         (NVRM_API_BASE+12)
#define NVRM_API_SET_CANVAS_REGION       (NVRM_API_BASE+13)
#define NVRM_API_ALLOC_PAGES             (NVRM_API_BASE+20)
#define NVRM_API_FREE_PAGES              (NVRM_API_BASE+21)
#define NVRM_API_LOCK_PAGES              (NVRM_API_BASE+22)
#define NVRM_API_UNLOCK_PAGES            (NVRM_API_BASE+23)
#define NVRM_API_CLEAR_SCREEN            (NVRM_API_BASE+24)
#define NVRM_API_MAP_SELECTOR            (NVRM_API_BASE+25)
#define NVRM_API_HIDE_SW_CURSOR          (NVRM_API_BASE+26)
#define NVRM_API_IPC_CALLBACK            (NVRM_API_BASE+30)
#define NVRM_API_NOTIFY_CALLBACK         (NVRM_API_BASE+31)
#define NVRM_API_UNLOAD_DLL              (NVRM_API_BASE+32)
#define NVRM_API_GET_NV_ADDRESS          (NVRM_API_BASE+50)
#define NVRM_API_GET_FB_ADDRESS          (NVRM_API_BASE+51)
#define NVRM_API_GET_AGP_ADDRESS         (NVRM_API_BASE+52)
#define NVRM_API_GET_GAMMA_ADDRESS       (NVRM_API_BASE+53)
#define NVRM_API_GET_VDD_PRIV_ADDRESS    (NVRM_API_BASE+54)
#define NVRM_API_UPDATE_CONFIGURATION    (NVRM_API_BASE+60)
#define NVRM_API_UNLOAD_CONFIGURATION    (NVRM_API_BASE+61)
#define NVRM_API_LOAD_CONFIGURATION      (NVRM_API_BASE+62)
#define NVRM_API_CONFIG_VGA              (NVRM_API_BASE+63)
#define NVRM_API_MODE_HOOK               (NVRM_API_BASE+64)
#define NVRM_API_DMAPUSH_OPERATION       (NVRM_API_BASE+65)
#define NVRM_API_DMAPUSH_LOCK            (NVRM_API_BASE+69)
#define NVRM_API_SEMAPHORE               (NVRM_API_BASE+70)
#define NVRM_API_SIGNAL_MODE_SAVE        (NVRM_API_BASE+71)
#define NVRM_API_ARCHITECTURE_16         (NVRM_API_BASE+72)
#define NVRM_API_ARCHITECTURE_32         (NVRM_API_BASE+73)
#define NVRM_API_MULTIDEVICE_MAP_RM_INFO (NVRM_API_BASE+74)
#define NVRM_API_DISPLAY                 (NVRM_API_BASE+75)
#define NVRM_API_RM_TOKEN                (NVRM_API_BASE+76)
#define NVRM_API_GET_DEVICE_TYPE         (NVRM_API_BASE+77)
#define NVRM_API_TEST                    (NVRM_API_BASE+78)
#define NVRM_API_REGISTER_DISPLAY_DRIVER (NVRM_API_BASE+79)
#define NVRM_AGP_LOAD_DUMMY_PAGES        (NVRM_API_BASE+80)
#define NVRM_REGISTRY_SET_STRING         (NVRM_API_BASE+81)
#define NVRM_API_ACPI_ENABLED            (NVRM_API_BASE+82)

#define NVRM_API_INIT                    (NVRM_API_BASE+100)
#define NVRM_API_ENABLEVGA               (NVRM_API_BASE+101)
#define NVRM_API_ENABLEHIRES             (NVRM_API_BASE+102)
#define NVRM_API_DEV_HANDLE              (NVRM_API_BASE+103)
#define NVRM_API_POWER_UP                (NVRM_API_BASE+105)
#define NVRM_API_POWER_DOWN              (NVRM_API_BASE+106)
#define NVRM_API_EXIT                    (NVRM_API_BASE+107)
#define NVRM_API_POWER_D1                (NVRM_API_BASE+108)
#define NVRM_API_POWER_D2                (NVRM_API_BASE+109)
#define NVRM_API_DRIVER_CONNECT          (NVRM_API_BASE+110)
#define NVRM_API_DRIVER_DISCONNECT       (NVRM_API_BASE+111)
#define NVRM_API_READ_EDID               (NVRM_API_BASE+112)
#define NVRM_API_NUM_HEADS               (NVRM_API_BASE+113)
#define NVRM_API_GET_DDC_CAPS            (NVRM_API_BASE+114)
#define NVRM_API_HIBERNATE               (NVRM_API_BASE+115)
#define NVRM_API_SET_MONITOR_POWER       (NVRM_API_BASE+116)
#define NVRM_API_BLANK_SECONDARY         (NVRM_API_BASE+117)
#define NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP (NVRM_API_BASE+118)

#define NVRM_API_NV1ALLOCCHANNELPIO      (NVRM_API_BASE+700)
#define NVRM_API_VPE_MISC                (NVRM_API_BASE+600)
#define NVRM_API_OVERLAY_MISC            (NVRM_API_BASE+601)

#define NVRM_ARCH_NV01FREE              0
#define NVRM_ARCH_NV01ALLOCROOT         1
#define NVRM_ARCH_NV01ALLOCMEMORY       2
#define NVRM_ARCH_NV01ALLOCCONTEXTDMA   3
#define NVRM_ARCH_NV01ALLOCCHANNELPIO   4
#define NVRM_ARCH_NV01ALLOCOBJECT       5
#define NVRM_ARCH_NV01ALLOCDEVICE       6
#define NVRM_ARCH_NV03ALLOCCHANNELDMA   7
#define NVRM_ARCH_NV03DMAFLOWCONTROL    8
#define NVRM_ARCH_NV01INTERRUPT         9
#define NVRM_ARCH_NV01ALLOCEVENT        10
#define NVRM_ARCH_NV03HEAP              11
#define NVRM_ARCH_CONFIG_VERSION        12
#define NVRM_ARCH_CONFIG_GET            13
#define NVRM_ARCH_CONFIG_SET            14
#define NVRM_ARCH_CONFIG_UPDATE         15
#define NVRM_ARCH_RING0CALLBACK         16
#define NVRM_ARCH_CONFIG_GET_EX         17
#define NVRM_ARCH_CONFIG_SET_EX         18
#define NVRM_ARCH_I2C_ACCESS            19
#define NVRM_ARCH_NV01DEBUGCONTROL      20
#define NVRM_ARCH_NV04ALLOC             21
// audio stuff
#define NVRM_ARCH_UNIFIEDFREE           30

#define NVRM_ARCH_MAX_IOCTL             30

// ResourceManagerGlobalToken stuff.
#define AQUIRE_TOKEN 0
#define RETURN_TOKEN 1
#define QUERY_TOKEN  2

// *** Beginning of VPE_EXTRAS
// Videoport api subfunction list
#define SKIP_NEXT_FIELD                 1
#define SKIP_FIELD_PATTERN              2
#define MULTIPLE_SURFACE                3
#define STATUS_CURRENT                  4
#define STATUS_NEXT                     5
#define STATUS_ALL                      6
#define DMA_ATTACH_TO_SURFACE           7
#define DMA_DETTACH_FROM_SURFACE        8
#define DMA_STATUS                      9
#define DMA_QUERY_CAPS                  10

// 16 bit access only
#define VBI_INSTALL_16CALLBACK          11
#define VBI_UNINSTALL_16CALLBACK        12
#define IMAGE_INSTALL_16CALLBACK        13
#define IMAGE_UNINSTALL_16CALLBACK      14

// 32 bit access only
#define VBI_INSTALL_32CALLBACK          15
#define VBI_UNINSTALL_32CALLBACK        16
#define IMAGE_INSTALL_32CALLBACK        17
#define IMAGE_UNINSTALL_32CALLBACK      18

// special extras
#define CAPTURE_FIELD_IMAGE_LIMIT       19
#define CAPTURE_FIELD_VBI_LIMIT         20
#define EVENT_SET_INFO                  21
#define EVENT_START                     22
#define EVENT_STOP                      23
#define EVENT_DESTROY                   24

#define SET_VIDEOPORT_SIZE              25

#define VPE_API_MAX                     25

// vpe api error/warnings
#define VPE_MAX_API_LIST_EXCEEDED       1
#define VPE_API_FUNCTION_FAILED         2

#define SEMAPHORE_SIGNAL                0
#define SEMAPHORE_WAIT                  1

// Function equates for dll interfaces.
#define NV_FUNCTION_NVOPEN              1  
#define NV_FUNCTION_NVCLOSE             2  
#define NV_FUNCTION_NVSYSCLIENTEXIT     3  
#define NV_FUNCTION_NVNOTIFYEVENT       5  
#define NV_FUNCTION_NVNOTIFYMSG         6  
#define NV_FUNCTION_NVERRORMSG          7  
#define NV_FUNCTION_NVALLOCPAGES        8  
#define NV_FUNCTION_NVFREEPAGES         9  
#define NV_FUNCTION_GETSELECTORBASE     100  
#define NV_FUNCTION_GETCURRENTTASK      101  
#define NV_FUNCTION_NVCONFIGVERSION     200  
#define NV_FUNCTION_NVCONFIGGET         201  
#define NV_FUNCTION_NVCONFIGSET         202  
#define NV_FUNCTION_NVCONFIGUPDATE      203  
#define NV_FUNCTION_NVSYSGETHWPTRS      300  
#define NV_FUNCTION_NVDEVICEBASEGET     301  
#define NV_FUNCTION_NVDEVICELIMITGET    302 
#define NV_FUNCTION_NVDEVICESELECTORGET 303
#define NV_FUNCTION_NVSYSOPEN           500  
#define NV_FUNCTION_NVSYSCLOSE          501  
#define NV_FUNCTION_NVSYSSHOWHIDECRSR   502  
#define NV_FUNCTION_NVOPENDMA           503  
#define NV_FUNCTION_NVSYSOPENDMA        504

// structures ....
//
// Paramater structure used for communication between Resource Manager modules.
//
// Resource Manager RMUSRAPI parameters.
typedef struct def_rmusrapi_struct {
  U032  function;       
  U032  device; 
} RMUSRAPI_STRUCT,*PRMUSRAPI_STRUCT;

typedef struct def_diagstruct_struct {
  U032  nv_pfifo_intr_0;      
  U032  nv_pfifo_intr_en_0;   
  U032  nv_pfifo_caches;      
  U032  nv_pfifo_mode;        
  U032  nv_pfifo_cache1_push0;
  U032  nv_pfifo_cache1_push1;
  U032  nv_pfifo_cache1_pull0;
  U032  nv_pfifo_dma_put;     
  U032  nv_pfifo_dma_get;     
  U032  nv_pmc_intr_0;
  U032  nv_pmc_intr_en_0;
  U032  nv_pmc_intr_read_0;
  U032  nv_pmc_enable;
}NVWATCHDIAGSTRUCT,*PNVWATCHDIAGSTRUCT;


//
// IMPORTANT: The display driver uses this exact data structure alignment on the
// other side of the DLL thunks.  DO NOT CHANGE THIS STRUCTURE without also changing
// the display driver.  Otherwise everything will just stop working.
//
typedef struct _def_rmParams
{
    U032 RegEAX;
    U032 RegEBX;
    U032 RegECX;
    U032 RegEDX;
    U032 RegEDI;
    U032 RegESI;
    U032 RegES;
    U032 RegDS;
    U032 RegCS;
} RMPARAMS, *PRMPARAMS;

typedef struct _def_rmInterfaceParameters
{
    U032 Param0;
    U032 Param1;
    U032 Param2;
    U032 Param3;
    U032 Param4;
    U032 Param5;
    U032 Param6;
    U032 Param7;
} RMINTPARAMS, *PRMINTPARAMS;

// this structure is redfined in sdk\nvidia\inc\nvarmapi.h
// DO NOT CHANGE it!
typedef struct _def_NvIoctlArch
{
    V032 nvarchFunction;
    V032 nvarchParameters;
} NV_IOCTL_ARCH_STRUCT;

typedef struct
{
U032 dwOffset;
U032 dwLength;
U032 dwFormat;
U032 dwPitch;
} SURFACES, * PSURFACES;

typedef struct
{
U032 dwObjectIdent;
U032 dwNoOfSurfaces;
PSURFACES pSurfaceList;
U032 dwMasterFormat;
U032 dwRequiredXsize;
U032 dwRequiredYsize;
U032 dwSrcXsize;
U032 dwSrcYsize;
} MULTSURF, * PMULTSURF;

typedef struct 
{
U032 dwSkipPattern;
U032 dwSkipBits;
} SKIPPATTERN, * PSKIPPATTERN;

#define SYNC_CONTROL_NONE       0
#define SYNC_CONTROL_ODD        1
#define SYNC_CONTROL_EVEN       2
#define SYNC_CONTROL_BUFF_0     3
#define SYNC_CONTROL_BUFF_1     4

typedef struct
{
U032 dwFieldCount;
U032 dwSyncControl;
} CAPTURE_LIMIT, * PCAPTURE_LIMIT;

// mediaport temporary callback facility
typedef struct 
{
U032 Selector;
U032 Offset;
} CallBackDef;


// begining of overlay extras
// definitions...
#define OVERLAY_MULTIPLE_SURFACE        1
#define OVERLAY_API_MAX                 1

#define OVERLAY_MAX_API_LIST_EXCEEDED   1
#define OVERLAY_API_FUNCTION_FAILED     2

// structures....
typedef struct
{
U032 dwObjectIdent;
U032 dwNoOfSurfaces;
PSURFACES pSurfaceList;
U032 dwMasterFormat; // BOB or INTERLEAVED
} OVERLAY_MULTSURF, * POVERLAY_MULTSURF;

typedef struct
{
U032 hVideoEvent;
U032 hVBIEvent;
U032 dwVBISize;     // don't pay any attention to this
U032 dwFlags;       // which field caused the event
U032 pVBIBuffer;
// following are debug only
U032 fieldCounter;  // incremented every field interrupt
U032 vbiCounter;    // incremented every vbi interrupt
} EVENT_INFO, * PEVENT_INFO;

typedef struct
{
U032 dwMaxSizeX;
U032 dwMaxSizeY;
} VIDEOPORT_SIZE_INFO, * PVIDEOPORT_SIZE_INFO;

typedef struct
{
U032 chPtr;         // NvChannel *
U032 ctxDma;
U032 putAddress;
U032 getAddress;
U032 jumpAddress;
U032 pNotificationDmaToMemDmaPushStatus;
U032 fDmaPusherActive;
} DMAPUSH_ADDRESS_INFO, * PDMAPUSH_ADDRESS_INFO;

//
// DmaPush flag options.
//
#define SET_JUMP_ADDRESS                0x0001
#define WRITE_PUT_ADDRESS               0x0002
#define READ_GET_ADDRESS                0x0004
#define DMAPUSH_LOCK                    0x0008
#define DMAPUSH_FREE                    0x0010

//
// Display Device API functions
//
#define NVRM_API_DISPLAY_GET_MONITOR_TYPE           0x0000
#define NVRM_API_DISPLAY_GET_DISPLAY_TYPE           0x0001
#define NVRM_API_DISPLAY_SET_DISPLAY_TYPE           0x0002
#define NVRM_API_DISPLAY_GET_FP_CONNECT_STATUS      0x0003
#define NVRM_API_DISPLAY_GET_FP_CONFIG              0x0004
#define NVRM_API_DISPLAY_SET_FP_CONFIG              0x0005
#define NVRM_API_DISPLAY_CHANGE_START               0x0006
#define NVRM_API_DISPLAY_CHANGE_END                 0x0007
#define NVRM_API_DISPLAY_CHANGE_CONFIG              0x0008

//
// Mapping options.
//
#define MAP_LINEAR                      1
#define MAP_SELECTOR                    2
//
// Configuration options.
//
#define CONFIG_PERMANENT                1
#define CONFIG_TEMPORARY                2
//
// Resource String Definitions.
//
// NVSYS.DLL
//
#define NVSYS_STR_CLIENT_ERROR          1
#define NVSYS_STR_RM_ERROR              2
#define NVSYS_STR_EXE_NOT_FOUND         3
#define NVSYS_STR_TOO_MANY_CLIENTS      4
#define NVSYS_STR_VERSION_ERROR         5
//
// NVRM.EXE
//
#define NVEXE_STR_CLIENT_ERROR          1

//---------------------------------------------------------------------------
//
//  OS specific structures.
//
//---------------------------------------------------------------------------

#endif // _MVRMWIN_H_

