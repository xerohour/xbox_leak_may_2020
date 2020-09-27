#ifndef _NVHAL_H_
#define _NVHAL_H_

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

/************************ NV HAL Defines and Structures ********************\
*                                                                           *
* Module: nvhal.h                                                           *
*       HAL interface defines and structures.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//
// HAL Interface Id:
//
//  31      27                              11                     0
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  | 4 bits| 16 bits                       | 12 bits               |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   Engine  Function number         Function revision
//
// Engine tag: unique engine tag
// Function number: unique interface number
// Function revision: specific revision of interface
//
typedef U032    NV_HAL_ID;

#define MKHALID(eng,func,rev)           (eng << 28 | func << 12 | rev)

//
// Engine tags to be used by both RM/HAL to reference specific engines.
//
// These values are used in the engine descriptor table (see nvromdat.h>
// as well as in the class descriptor table.
//
// WARNING: There are exactly 4 bits for the engine tags.
//
#define MC_ENGINE_TAG                   0x1
#define FB_ENGINE_TAG                   0x2
#define DAC_ENGINE_TAG                  0x3
#define TMR_ENGINE_TAG                  0x4
#define DMA_ENGINE_TAG                  0x5
#define GR_ENGINE_TAG                   0x6
#define FIFO_ENGINE_TAG                 0x7
#define MP_ENGINE_TAG                   0x8
#define VIDEO_ENGINE_TAG                0x9

//
// These are "special" tags for classes that don't need true
// engine handling.
//

// This tag is for the NV_CLASS "pseudo" class.
#define CLASS_PSEUDO_ENGINE_TAG         0xe
// This tag is for the memory classes etc.
#define NO_ENGINE                       0xf

//
// Forwards.
//
typedef struct _def_hal_hw_info HALHWINFO, *PHALHWINFO;
typedef struct _def_hal_rm_funcs_000 HALRMFUNCS_000, *PHALRMFUNCS_000;
typedef VOID *PHALRMFUNCS;

//--------------------------------------------------------------------
// Processor info
//--------------------------------------------------------------------
typedef struct _def_processor_hal_info
{
    U032 FrontSideBusFreq;
    U032 SystemMemoryFreq;
} PROCESSORHALINFO, *PPROCESSORHALINFO;

//--------------------------------------------------------------------
// Mc
//--------------------------------------------------------------------

// Current HAL mc interface revision ids.
#define MC_CONTROL_000                  MKHALID(MC_ENGINE_TAG,0x0001,0)
#define MC_POWER_000                    MKHALID(MC_ENGINE_TAG,0x0002,0)

#define MAX_PCI_BARS 8

typedef struct _def_mc_hal_info
{   
    U032 Implementation;
    U032 Revision;
    U032 Subrevision;
    U032 Architecture;
    U032 Manufacturer;
    U032 MaskRevision;
    U032 CrystalFreq;
    U032 PCIDeviceID;
    U032 PCISubDeviceID;
    U032 EndianControl;
	U032 ChipBugs;
    U032 TotalPciBars;
    U032 PciBars[MAX_PCI_BARS];
} MCHALINFO, *PMCHALINFO;

#define MCHALINFO(nvinfo,a)             (nvinfo->Chip.HalInfo.a)

// Endian control records current endian-ness of the NV chip as
// well as how we were compiled (big or little endian).  We need
// to know this information early in the init process.

#define MC_ENDIAN_CONTROL_CHIP_BIG_ENDIAN         BIT(0)
#define MC_ENDIAN_CONTROL_CODE_BIG_ENDIAN         BIT(1)
#define MC_ENDIAN_CONTROL_ALL_BIG_ENDIAN          BIT(2)  // bit0 & bit1
#define MC_ENDIAN_CONTROL_SWAP_REQUIRED           BIT(3)  // bit0 ^ bit1

// Chip bugs that are tracked in MCHALINFO.ChipBugs
// If bit is set, that bug is an issue for this device

#define ChipBugs       MCHALINFO(pDev, ChipBugs)

#define MC_BUG_NV11_BIG_ENDIAN_PCI_CONFIG_READS   BIT(0)

//
// MC_CONTROL parameters.
//
typedef struct _def_mc_control_arg_000
{
    NV_HAL_ID id;                       // MC_CONTROL_000
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} MCCONTROLARG_000, *PMCCONTROLARG_000;

// MC_CONTROL commands.
#define MC_CONTROL_UPDATE               1
#define MC_CONTROL_LOAD                 2
#define MC_CONTROL_UNLOAD               3
#define MC_CONTROL_DESTROY              4
#define MC_CONTROL_INIT                 5
#define MC_CONTROL_SETREVISION          6

//
// MC_POWER parameters.
//
typedef struct _def_mc_power_arg_000
{
    NV_HAL_ID id;
    U032 newLevel;
    U032 oldLevel;
    PHALHWINFO pHalHwInfo;
} MCPOWERARG_000, *PMCPOWERARG_000;

// Power management states.
#define MC_POWER_LEVEL_0                0
#define MC_POWER_LEVEL_1                1
#define MC_POWER_LEVEL_2                2
#define MC_POWER_LEVEL_3                3
#define MC_POWER_LEVEL_4                4
#define MC_POWER_LEVEL_5                5
#define MC_POWER_LEVEL_6                6
#define MC_POWER_LEVEL_7                7

//
// HAL wrappers.
//
RM_STATUS nvHalMcControl(PHWINFO, U032);
RM_STATUS nvHalMcPower(PHWINFO, U032, U032);

//--------------------------------------------------------------------
// Pram
//--------------------------------------------------------------------
typedef struct _def_pram_hal_info
{
    U032 InstPadSize;
    U032 TotalInstSize;
    U032 CurrentSize;
    U032 HashDepth;
    U032 HashSearch;
    U032 PraminOffset;
} PRAMHALINFO, *PPRAMHALINFO;

//--------------------------------------------------------------------
// Fifo
//--------------------------------------------------------------------

// Current HAL fifo interface revision ids.
#define FIFO_CONTROL_000                MKHALID(FIFO_ENGINE_TAG,0x0001,0x0)
#define FIFO_ALLOC_PIO_000              MKHALID(FIFO_ENGINE_TAG,0x0002,0x0)
#define FIFO_ALLOC_DMA_000              MKHALID(FIFO_ENGINE_TAG,0x0003,0x0)
#define FIFO_FREE_000                   MKHALID(FIFO_ENGINE_TAG,0x0004,0x0)
#define FIFO_GET_EXCEPTION_DATA_000     MKHALID(FIFO_ENGINE_TAG,0x0005,0x0)
#define FIFO_SERVICE_000                MKHALID(FIFO_ENGINE_TAG,0x0006,0x0)
#define FIFO_ACCESS_000                 MKHALID(FIFO_ENGINE_TAG,0x0007,0x0)
#define FIFO_HASH_ADD_000               MKHALID(FIFO_ENGINE_TAG,0x0008,0x0)
#define FIFO_HASH_DELETE_000            MKHALID(FIFO_ENGINE_TAG,0x0009,0x0)
#define FIFO_HASH_FUNC_000              MKHALID(FIFO_ENGINE_TAG,0x000a,0x0)
#define FIFO_HASH_SEARCH_000            MKHALID(FIFO_ENGINE_TAG,0x000b,0x0)

// Fifo HAL state info structure.
typedef struct _def_fifo_hal_info
{
    U032 IntrEn0;                       // interrupt enable mask
    U032 AllocateCount;
    U032 Count;                         // total available fifos
    U032 ContextSize;                   // per-fifo context size
    U032 RetryCount;
    U032 UserBase;                      // offset of user channel area
} FIFOHALINFO, *PFIFOHALINFO;

#define FIFOHALINFO(nvinfo,a)            (nvinfo->Fifo.StateInfo.a)

//
// FIFO_CONTROL_000 parameters.
//
typedef struct _def_fifo_control_arg_000
{
    NV_HAL_ID id;                       // FIFO_CONTROL_000
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} FIFOCONTROLARG_000, *PFIFOCONTROLARG_000;

// FIFO_CONTROL commands.
#define FIFO_CONTROL_UPDATE             1
#define FIFO_CONTROL_LOAD               2
#define FIFO_CONTROL_UNLOAD             3
#define FIFO_CONTROL_DESTROY            4
#define FIFO_CONTROL_INIT               5

//
// FIFO_ALLOC_PIO_000 parameters.
//
typedef struct _def_fifo_alloc_pio_arg_000 {
    NV_HAL_ID id;
    U032 chid;
    U032 instance;
    PHALHWINFO pHalHwInfo;
} FIFOALLOCPIOARG_000, *PFIFOALLOCPIOARG_000;

//
// FIFO_ALLOC_DMA_000 parameters.
//
typedef struct _def_fifo_alloc_dma_arg_000
{
    NV_HAL_ID id;
    U032 chid;
    U032 instance;
    U032 dmaInstance;
    U032 fetchTrigger;
    U032 fetchSize;
    U032 fetchRequests;
    PHALHWINFO pHalHwInfo;
} FIFOALLOCDMAARG_000, *PFIFOALLOCDMAARG_000;

//
// FIFO_FREE_000 parameters.
//
typedef struct _def_fifo_free_arg_000
{
    NV_HAL_ID id;
    U032 chid;
    U032 instance;
    PHALHWINFO pHalHwInfo;
} FIFOFREEARG_000, *PFIFOFREEARG_000;

//
// FIFO_ACCESS_000 parameters.
//

typedef struct _def_fifo_access_info
{
    U032 FifoReassign;
    U032 FifoPush;
    U032 FifoPull;
} FIFOACCESSINFO, *PFIFOACCESSINFO;

typedef struct _def_fifo_access_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PFIFOACCESSINFO pAccessInfo;
    PHALHWINFO pHalHwInfo;
} FIFOACCESSARG_000, *PFIFOACCESSARG_000;

// FIFO_ACCESS commands.
#define FIFO_ACCESS_DISABLE             0x0
#define FIFO_ACCESS_ENABLE              0x1

//
// FIFO_GET_EXCEPTION_DATA_000 parameters.
//
typedef struct _def_fifo_exception_data
{
    U032 Reason;
    U032 ChID;
    U032 GetPtr;
    U032 SubChannel;
    U032 Method;
    U032 Data;
} FIFOEXCEPTIONDATA, *PFIFOEXCEPTIONDATA;

typedef struct _def_fifo_get_exception_data_arg_000
{
    NV_HAL_ID id;
    PFIFOEXCEPTIONDATA pExceptionData;
    PHALHWINFO pHalHwInfo;
} FIFOGETEXCEPTIONDATAARG_000, *PFIFOGETEXCEPTIONDATAARG_000;

//
// FIFO_SERVICE_000 parameters.
//
typedef struct _def_fifo_service_arg_000
{
    NV_HAL_ID id;
    U032 intr;
    PFIFOEXCEPTIONDATA pExceptionData;
    PHALHWINFO pHalHwInfo;
} FIFOSERVICEARG_000, *PFIFOSERVICEARG_000;

//
// FIFO_HASH_ADD_000 parameters.
//
typedef struct _def_fifo_hash_add_arg_000
{
    NV_HAL_ID id;
    U032 entry;
    U032 handle;
    U032 chid;
    U032 instance;
    U032 engine;
    PHALHWINFO pHalHwInfo;
} FIFOHASHADDARG_000, *PFIFOHASHADDARG_000;

//
// FIFO_HASH_DELETE_000 parameters.
//
typedef struct _def_fifo_hash_delete_arg_000
{
    NV_HAL_ID id;
    U032 entry;
    PHALHWINFO pHalHwInfo;
} FIFOHASHDELETEARG_000, *PFIFOHASHDELETEARG_000;

//
// FIFO_HASH_FUNC_000 parameters.
//
typedef struct _def_fifo_hash_func_arg_000
{
    NV_HAL_ID id;
    U032 handle;
    U032 chid;
    U032 result;
    PHALHWINFO pHalHwInfo;
} FIFOHASHFUNCARG_000, *PFIFOHASHFUNCARG_000;

//
// FIFO_HASH_FUNC_000 parameters.
//
typedef struct _def_fifo_hash_search_arg_000
{
    NV_HAL_ID id;
    U032 handle;
    U032 chid;
    U032 result;
    PHALHWINFO pHalHwInfo;
} FIFOHASHSEARCHARG_000, *PFIFOHASHSEARCHARG_000;

// FIFO HAL wrappers.
RM_STATUS nvHalFifoControl(PHWINFO, U032);
RM_STATUS nvHalFifoAllocPio(PHWINFO, U032, U032);
RM_STATUS nvHalFifoAllocDma(PHWINFO, U032, U032, U032, U032, U032, U032);
RM_STATUS nvHalFifoFree(PHWINFO, U032, U032);
RM_STATUS nvHalFifoGetExceptionData(PHWINFO, PFIFOEXCEPTIONDATA);
RM_STATUS nvHalFifoService(PHWINFO, U032, PFIFOEXCEPTIONDATA);
RM_STATUS nvHalFifoAccess(PHWINFO, U032, PFIFOACCESSINFO);
RM_STATUS nvHalFifoHashAdd(PHWINFO, U032, U032, U032, U032, U032);
RM_STATUS nvHalFifoHashDelete(PHWINFO, U032);
RM_STATUS nvHalFifoHashFunc(PHWINFO, U032, U032, U032 *);
RM_STATUS nvHalFifoHashSearch(PHWINFO, U032, U032, U032 *);

//--------------------------------------------------------------------
// Framebuffer
//--------------------------------------------------------------------

// Current HAL gr interface revision ids.
#define FB_CONTROL_000                  MKHALID(FB_ENGINE_TAG,0x0001,0x0)
#define FB_SET_ALLOC_PARAMS_000         MKHALID(FB_ENGINE_TAG,0x0002,0x0)
#define FB_ALLOC_000                    MKHALID(FB_ENGINE_TAG,0x0003,0x0)
#define FB_FREE_000                     MKHALID(FB_ENGINE_TAG,0x0004,0x0)
#define FB_GET_SURFACE_PITCH_000        MKHALID(FB_ENGINE_TAG,0x0005,0x0)
#define FB_LOAD_OVERRIDE_000            MKHALID(FB_ENGINE_TAG,0x0006,0x0)
#define FB_GET_SURFACE_DIMENSIONS_000   MKHALID(FB_ENGINE_TAG,0x0007,0x0)

//
// Moving forward, nv chips will support fb sizes that are no longer
// powers of 2 (i.e. the external banks have different sizes).
//
#define MAX_EXTBANKS                    2

typedef struct _def_fb_extbank_hal_info
{
    U032 size;                          // size of bank
    U032 intBanks;                      // number of bank addressing bits
} FBEXTBANKHALINFO, *PFBEXTBANKHALINFO;

// FB allocation resources structure.
typedef struct _def_fb_alloc_info
{
    U032 type;
    U032 hwResId;
    U032 height;
    U032 pitch;    
    U032 size;
    U032 align;
    U032 alignPerf;
    U032 offset;
    //U032 limit;
} FBALLOCINFO, *PFBALLOCINFO;

typedef struct _def_fb_hal_info
{   
    U032 RamSizeMb;                     // combined size of all extbanks (Mb)
    U032 RamSize;                       // combined size of all extbanks
    U032 RamType;                       // sdram vs ddr
    U032 ExtBankCount;                  // number of external banks
    U032 BankCount;                     // number of internal banks
    U032 Depth;
    U032 HorizDisplayWidth;
    U032 VertDisplayWidth;
    U032 RefreshRate;
    U032 Start[2];
    U032 Limit[2];
    FBEXTBANKHALINFO ExtBankInfo[MAX_EXTBANKS];
    FBALLOCINFO PrimaryFbAllocInfo[2];  // This should be MAX_CRTCS
    VOID_PTR pBiosDrvSlwParms;
    U032 AdjustDriveSlew;
} FBHALINFO, *PFBHALINFO;

//
// Use the following for display types.
//
#define DISPLAY_TYPE_MONITOR            0
#define DISPLAY_TYPE_TV                 1
#define DISPLAY_TYPE_DUALSURFACE        2
#define DISPLAY_TYPE_FLAT_PANEL         3
#define DISPLAY_TYPE_NONE             (~0)

//
// Use the following for monitor types.
//
#define MONITOR_TYPE_VGA                0
#define MONITOR_TYPE_NTSC               2
#define MONITOR_TYPE_PAL                3
#define MONITOR_TYPE_FLAT_PANEL         4

#define FBHALINFO(nvinfo,a)              (nvinfo->Framebuffer.HalInfo.a)

//
// FB_CONTROL_000 parameters.
//
typedef struct _def_fb_control_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} FBCONTROLARG_000, *PFBCONTROLARG_000;

// FB_CONTROL commands.
#define FB_CONTROL_UPDATE               1
#define FB_CONTROL_LOAD                 2
#define FB_CONTROL_UNLOAD               3
#define FB_CONTROL_DESTROY              4
#define FB_CONTROL_INIT                 5
#define FB_CONTROL_INSTWR_DISABLE       6
#define FB_CONTROL_INSTWR_ENABLE        7
#define FB_CONTROL_TILING_DISABLE       8
#define FB_CONTROL_TILING_ENABLE        9
#define FB_CONTROL_TILING_CONFIGURE     10

//
// FB_SET_ALLOC_PARAMS_000 parameters.
//
typedef struct _def_fb_set_alloc_params_arg_000
{
    NV_HAL_ID id;
    PFBALLOCINFO pAllocInfo;
    PHALHWINFO pHalHwInfo;
} FBSETALLOCPARAMSARG_000, *PFBSETALLOCPARAMSARG_000;

//
// FB_ALLOC_000 parameters.
//
typedef struct _def_fb_alloc_arg_000
{
    NV_HAL_ID id;
    U032 flags;
    PFBALLOCINFO pAllocInfo;
    PHALHWINFO pHalHwInfo;
} FBALLOCARG_000, *PFBALLOCARG_000;

//
// Flags.
//
#define FB_ALLOC_NO_PITCH_ADJUST                0x01

//
// FB_FREE_000 parameters.
//
typedef struct _def_fb_free_arg_000
{
    NV_HAL_ID id;
    U032 hwResId;
    PHALHWINFO pHalHwInfo;
} FBFREEARG_000, *PFBFREEARG_000;

//
// FB_GET_SURFACE_PITCH_000 parameters.
//
typedef struct _def_fb_get_surface_pitch_arg_000
{
    NV_HAL_ID id;
    U032 flags;
    U032 width;
    U032 bpp;
    U032 pitch;
    PHALHWINFO pHalHwInfo;
} FBGETSURFACEPITCHARG_000, *PFBGETSURFACEPITCHARG_000;

//
// FB_GET_SURFACE_DIMENSIONS_000 parameters.
//
typedef struct _def_fb_get_surface_dimensions_arg_000
{
    NV_HAL_ID id;
    U032 flags;
    U032 width;
    U032 height;
    U032 bpp;
    U032 pitch;
    U032 size;
    PHALHWINFO pHalHwInfo;
} FBGETSURFACEDIMENSIONSARG_000, *PFBGETSURFACEDIMENSIONSARG_000;

//
// Flags.
//
#define FB_GET_SURFACE_PITCH_NO_ADJUST          0x01

//
// FB_LOAD_OVERRIDE_000 parameters.
//
typedef struct _def_fb_load_override_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    U032 value;
    PHALHWINFO pHalHwInfo;
} FBLOADOVERRIDEARG_000, *PFBLOADOVERRIDEARG_000;

// FB_LOAD_OVERRIDE commands.
#define FB_LOAD_OVERRIDE_MEMORY         1
#define FB_LOAD_OVERRIDE_RTL            2
#define FB_LOAD_OVERRIDE_REMAP          3

// FB HAL wrappers.
RM_STATUS nvHalFbControl(PHWINFO, U032);
RM_STATUS nvHalFbAlloc(PHWINFO, PFBALLOCINFO);
RM_STATUS nvHalFbFree(PHWINFO, U032);
RM_STATUS nvHalFbSetAllocParameters(PHWINFO, PFBALLOCINFO);
RM_STATUS nvHalFbGetSurfacePitch(PHWINFO, U032, U032, U032 *);
RM_STATUS nvHalFbGetSurfaceDimensions(PHWINFO, U032, U032, U032, U032 *, U032 *);
RM_STATUS nvHalFbLoadOverride(PHWINFO, U032, U032);

//--------------------------------------------------------------------
// Graphics
//--------------------------------------------------------------------

// Graphics engine state info structure.
typedef struct _def_gr_hal_info
{
    U032 CurrentChID;
    U032 Capabilities;
#if MAX_FIFOS <= 32
    U032 has3dObject;       // bitmask of chIDs containing 3d objs
#endif
} GRHALINFO, *PGRHALINFO;

#define GRHALINFO(nvinfo,a)             (nvinfo.Graphics.HalInfo.a)

// Current HAL gr interface revision ids.
#define GR_CONTROL_000                  MKHALID(GR_ENGINE_TAG,0x0001,0x0)
#define GR_ALLOC_000                    MKHALID(GR_ENGINE_TAG,0x0002,0x0)
#define GR_FREE_000                     MKHALID(GR_ENGINE_TAG,0x0003,0x0)
#define GR_GET_EXCEPTION_DATA_000       MKHALID(GR_ENGINE_TAG,0x0004,0x0)
#define GR_SERVICE_000                  MKHALID(GR_ENGINE_TAG,0x0005,0x0)
#define GR_GET_NOTIFY_DATA_000          MKHALID(GR_ENGINE_TAG,0x0006,0x0)
#define GR_SET_OBJECT_CONTEXT_000       MKHALID(GR_ENGINE_TAG,0x0007,0x0)
#define GR_LOAD_OVERRIDE_000            MKHALID(GR_ENGINE_TAG,0x0008,0x0)

//
// Graphics capabilities.
//
// _Must_ line up with equivalent #define's from Nvcm.h.
//
#define NVHAL_GRAPHICS_CAPS_UNKNOWN            0x00000000
#define NVHAL_GRAPHICS_CAPS_MAXCLIPS_MASK      0x000000FF  // bits 7:0
#define NVHAL_GRAPHICS_CAPS_MAXCLIPS_SHIFT     0
#define NVHAL_GRAPHICS_CAPS_AA_LINES           0x00000100  // bit  8
#define NVHAL_GRAPHICS_CAPS_AA_POLYS           0x00000200  // bit  9
#define NVHAL_GRAPHICS_CAPS_AKILL_USERCLIP     0x00000400  // bit 10
#define NVHAL_GRAPHICS_CAPS_LOGIC_OPS          0x00000800  // bit 11
#define NVHAL_GRAPHICS_CAPS_NV15_ALINES        0x00001000  // bit 12
#define NVHAL_GRAPHICS_CAPS_2SIDED_LIGHTING    0x00002000  // bit 13
#define NVHAL_GRAPHICS_CAPS_QUADRO_GENERIC     0x00004000  // bit 14
#define NVHAL_GRAPHICS_CAPS_UBB                0x00008000  // bit 15
#define NVHAL_GRAPHICS_CAPS_NV1X_COLOR_INVARIANCE 0x00010000 // bit 16
#define NVHAL_GRAPHICS_CAPS_3D_TEXTURES        0x00020000  // bit 17
#define NVHAL_GRAPHICS_CAPS_ANISOTROPIC        0x00040000  // bit 18

//
// GR_CONTROL_000 parameters.
//
typedef struct _def_gr_control_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} GRCONTROLARG_000, *PGRCONTROLARG_000;

// GR_CONTROL commands.
#define GR_CONTROL_UPDATE               1
#define GR_CONTROL_LOAD                 2
#define GR_CONTROL_UNLOAD               3
#define GR_CONTROL_DESTROY              4
#define GR_CONTROL_INIT                 5

//
// GR_ALLOC_000 parameters.
//
typedef struct _def_gr_alloc_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    U032 flags;
    PHALHWINFO pHalHwInfo;
} GRALLOCARG_000, *PGRALLOCARG_000;

// GR_ALLOC flags
#define GR_ALLOC_ENDIAN_OVERRIDE		1:0
#define GR_ALLOC_ENDIAN_OVERRIDE_NONE 	          0       // use native endianness
#define GR_ALLOC_ENDIAN_OVERRIDE_LITTLE	          1
#define GR_ALLOC_ENDIAN_OVERRIDE_BIG              2

//
// GR_FREE_000 parameters.
//
typedef struct _def_gr_free_arg_000
{
    NV_HAL_ID id;
    U032 instance;
    U032 chid;
    PHALHWINFO pHalHwInfo;
} GRFREEARG_000, *PGRFREEARG_000;

//
// Graphics engine exception data.
//
typedef struct _def_gr_exception_data
{
    U032 classNum;
    U032 NotifyInstance;
    U032 Nsource;
    U032 Instance;
    U032 Offset;
    U032 Data;
    U032 ChID;
    U032 MethodStatus;
} GREXCEPTIONDATA, *PGREXCEPTIONDATA;

//
// GR_GET_EXCEPTION_DATA_000 parameters.
//
typedef struct _def_gr_get_exception_data_arg_000
{
    NV_HAL_ID id;
    PGREXCEPTIONDATA pExceptionData;
    PHALHWINFO pHalHwInfo;
} GRGETEXCEPTIONDATAARG_000, *PGRGETEXCEPTIONDATAARG_000;

//
// The following is used to setup object method handling in the
// HAL.
//
typedef RM_STATUS (*HALGRMETHODPROC)(PHALHWINFO, PGREXCEPTIONDATA);

typedef struct _def_hal_gr_method
{
    HALGRMETHODPROC Proc;
    U032 Low;
    U032 High;    
} HALGRMETHOD, *PHALGRMETHOD;

typedef struct _def_hal_gr_methods
{
    U032 classNum;
    PHALGRMETHOD pMethods;
    U032 methodMax;
} HALGRMETHODS, *PHALGRMETHODS;

//
// GR_SERVICE_000 parameters.
//
typedef struct _def_gr_service_arg_000
{
    NV_HAL_ID id;
    U032 intr;
    PGREXCEPTIONDATA pExceptionData;
    PHALHWINFO pHalHwInfo;
} GRSERVICEARG_000, *PGRSERVICEARG_000;

//
// GR_GET_NOTIFY_DATA_000 parameters.
//
typedef struct _def_gr_notify_data
{
    U032 trigger;
    U032 action;
} GRNOTIFYDATA, *PGRNOTIFYDATA;

typedef struct _def_gr_get_notify_data_arg_000
{
    NV_HAL_ID id;
    PGRNOTIFYDATA pGrNotifyData;
    PHALHWINFO pHalHwInfo;
} GRGETNOTIFYDATAARG_000, *PGRGETNOTIFYDATAARG_000;

//
// GR_SET_OBJECT_CONTEXT_000 parameters.
//
typedef struct _def_gr_set_object_context_arg_000
{
    NV_HAL_ID id;
    U032 andMask;
    U032 orMask;
    U032 instance;
    U032 chid;
    PHALHWINFO pHalHwInfo;
} GRSETOBJECTCONTEXTARG_000, *PGRSETOBJECTCONTEXTARG_000;

//
// GR_LOAD_OVERRIDE_000 parameters.
//
typedef struct _def_gr_load_override_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    U032 value;
    PHALHWINFO pHalHwInfo;
} GRLOADOVERRIDEARG_000, *PGRLOADOVERRIDEARG_000;

// GR_LOAD_OVERRIDE commands.
#define GR_LOAD_OVERRIDE_DEBUG0         1
#define GR_LOAD_OVERRIDE_DEBUG1         2
#define GR_LOAD_OVERRIDE_DEBUG2         3
#define GR_LOAD_OVERRIDE_DEBUG3         4
#define GR_LOAD_OVERRIDE_SWATHCTL       5

#define SWATH_CONTROL_VALID_BIT         (1 << 31)

// HAL wrappers.
RM_STATUS nvHalGrControl(PHWINFO, U032);
RM_STATUS nvHalGrAlloc(PHWINFO, U032, U032, U032, U032);
RM_STATUS nvHalGrFree(PHWINFO, U032, U032);
RM_STATUS nvHalGrGetExceptionData(PHWINFO, PGREXCEPTIONDATA);
RM_STATUS nvHalGrService(PHWINFO, U032, PGREXCEPTIONDATA);
RM_STATUS nvHalGrGetNotifyData(PHWINFO, PGRNOTIFYDATA);
RM_STATUS nvHalGrSetObjectContext(PHWINFO, U032, U032, U032, U032);
RM_STATUS nvHalGrLoadOverride(PHWINFO, U032, U032);

//--------------------------------------------------------------------
// Dma
//--------------------------------------------------------------------

// Current HAL dma interface revision ids.
#define DMA_CONTROL_000                 MKHALID(DMA_ENGINE_TAG,0x0001,0x0)
#define DMA_ALLOC_000                   MKHALID(DMA_ENGINE_TAG,0x0002,0x0)
#define DMA_FREE_000                    MKHALID(DMA_ENGINE_TAG,0x0003,0x0)
#define DMA_GET_INSTSIZE_000            MKHALID(DMA_ENGINE_TAG,0x0004,0x0)

typedef struct _def_dma_hal_obj_info DMAHALOBJINFO, *PDMAHALOBJINFO;

//
// DMA engine state info.
//
// We need to use a U032 here to ensure pointer size argreement
// across 16 and 32 bit land.
// 
typedef struct _def_dma_hal_info
{
    VOID_PTR pDmaHalObjInfos;     // pointer to list of dma context objects
} DMAHALINFO, *PDMAHALINFO;

//
// DMA_CONTROL_000 parameters.
//
typedef struct _def_dma_control_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} DMACONTROLARG_000, *PDMACONTROLARG_000;

// DMA_CONTROL commands.
#define DMA_CONTROL_UPDATE              1
#define DMA_CONTROL_LOAD                2
#define DMA_CONTROL_UNLOAD              3
#define DMA_CONTROL_DESTROY             4
#define DMA_CONTROL_INIT                5

// Context dma address space identifiers.
typedef U032       NV_ADDRESS_SPACE;
#define ADDR_UNKNOWN 0              // Address space is unknown
#define ADDR_SYSMEM  1              // System memory (PCI)
#define ADDR_FBMEM   2              // Frame buffer memory space
#define ADDR_AGPMEM  3              // AGP memory space

struct _def_dma_hal_obj_info
{
    U032 classNum;                  // Class number
    U032 Flags;                     // API flags (coherency, access, etc)
    VOID_PTR BufferBase;            // Base address of client buffer
    U032 BufferSize;                // Size of client buffer
    U032 Instance;                  // Pointer to instance memory context
    U032 NumUnits;                  // Size of instance memory
    U032 PteCount;                  // Number of pages in dma context
    U032 PteAdjust;                 // Offset within first page
    U032 PteLimit;                  // Length of dma context
    U032 *PteArray;                 // Page data
    U032 MemoryClass;               // Class number of underlying memory
    NV_ADDRESS_SPACE AddressSpace;  // Address space of context
    PDMAHALOBJINFO Next;            // Next in list
};

//
// DMA_ALLOC_000 parameters.
//
typedef struct _def_dma_alloc_arg_000
{
    NV_HAL_ID id;
    PDMAHALOBJINFO pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} DMAALLOCARG_000, *PDMAALLOCARG_000;

//
// DMA_FREE_000 parameters.
//
typedef struct _def_dma_free_arg_000
{
    NV_HAL_ID id;
    PDMAHALOBJINFO pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} DMAFREEARG_000, *PDMAFREEARG_000;

//
// DMA_GETINSTSIZE_000 parameters.
//
typedef struct _def_dma_getinstsize_arg_000
{
    NV_HAL_ID id;
    PDMAHALOBJINFO pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} DMAGETINSTSIZEARG_000, *PDMAGETINSTSIZEARG_000;

RM_STATUS nvHalDmaControl(PHWINFO, U032);
RM_STATUS nvHalDmaAlloc(PHWINFO, PDMAHALOBJINFO);
RM_STATUS nvHalDmaFree(PHWINFO, PDMAHALOBJINFO);
RM_STATUS nvHalDmaGetInstSize(PHWINFO, PDMAHALOBJINFO);

//--------------------------------------------------------------------
// Dac
//--------------------------------------------------------------------

// Current HAL dac interface revision ids.
#define DAC_CONTROL_000                 MKHALID(DAC_ENGINE_TAG,0x0001,0x0)
#define DAC_ALLOC_000                   MKHALID(DAC_ENGINE_TAG,0x0002,0x0)
#define DAC_FREE_000                    MKHALID(DAC_ENGINE_TAG,0x0003,0x0)
#define DAC_SET_START_ADDR_000          MKHALID(DAC_ENGINE_TAG,0x0004,0x0)
#define DAC_PROGRAMMCLK_000             MKHALID(DAC_ENGINE_TAG,0x0005,0x0)
#define DAC_PROGRAMNVCLK_000            MKHALID(DAC_ENGINE_TAG,0x0006,0x0)
#define DAC_PROGRAMPCLK_000             MKHALID(DAC_ENGINE_TAG,0x0007,0x0)
#define DAC_PROGRAMCURSOR_000           MKHALID(DAC_ENGINE_TAG,0x0008,0x0)
#define DAC_RASTERPOS_000               MKHALID(DAC_ENGINE_TAG,0x0009,0x0)
#define DAC_VALIDATEARBSETTINGS_000     MKHALID(DAC_ENGINE_TAG,0x000a,0x0)
#define DAC_UPDATEARBSETTINGS_000       MKHALID(DAC_ENGINE_TAG,0x000b,0x0)
#define DAC_VALIDATEBANDWIDTH_000       MKHALID(DAC_ENGINE_TAG,0x000c,0x0)
#define DAC_HOTPLUGINTERRUPTS_000       MKHALID(DAC_ENGINE_TAG,0x000d,0x0)

//
// HAL VIDEO_LUT_CURSOR_DAC object.
//
typedef struct _def_video_lut_cursor_dac_hal_object
{
    U032 Head;

    struct {
        U032       VisibleImageWidth;    
        U032       VisibleImageHeight;   
        U032       HorizontalBlankStart;
        U032       HorizontalBlankWidth;
        U032       HorizontalSyncStart;
        U032       HorizontalSyncWidth;
        U032       VerticalBlankStart; 
        U032       VerticalBlankHeight; 
        U032       VerticalSyncStart; 
        U032       VerticalSyncHeight; 
        U032       TotalWidth; 
        U032       TotalHeight;     
        U032       PixelClock;
        U032       ColorFormat;                  
        U032       PixelDepth;
        struct {
            unsigned int DoubleScanMode         : 1;
            unsigned int InterlacedMode         : 1;
            unsigned int HorizontalSyncPolarity : 1;
            unsigned int VerticalSyncPolarity   : 1;
            unsigned int unusedBits1            : 12;
            unsigned int unusedBits2            : 16;
        } Format;
    }  Dac[2];

} VIDEO_LUT_CURSOR_DAC_HAL_OBJECT, *PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT;

// This constant defines the maximum number of crtcs on any
// chip supported by this HAL revision.  An increase in this
// value means a new revision of the DAC section of the HAL.
//
#define MAX_CRTCS                       2

//
// There are MAX_CRTCS of these structures.
//
typedef struct _def_dac_crtc_hal_info
{
    #define DISPLAYTYPEBIT(d)   (1 << d)

    U032 DisplayType;
    U032 MonitorType;
    U032 CurrentDisplayType;            // Tracks current active display for mobile display transitions
    U032 DisplayTypesAllowed;
    U032 PCLKLimit8bpp;
    U032 PCLKLimit16bpp;
    U032 PCLKLimit32bpp;
    U032 RegOffset;
    U032 fpMode;

    PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pVidLutCurDac;

} DACCRTCHALINFO, *PDACCRTCHALINFO;

#define NV_HAL_DAC_UPDATE_MPLL          0x00000001

typedef struct _def_dac_hal_info
{
    U032 UpdateFlags;                   // HAL version of update flags
    U032 InputWidth;
    U032 Depth;
    U032 MClk;
    U032 VClk;
    U032 NVClk;
    U032 MPllM;
    U032 MPllN;
    U032 MPllO;
    U032 MPllP;
    U032 VPllM;
    U032 VPllN;
    U032 VPllO;
    U032 VPllP;
    U032 NVPllM;
    U032 NVPllN;
    U032 NVPllO;
    U032 NVPllP;
    U032 NumCrtcs;
    U032 MClkDivide;
    U032 Head2RegOwner;                 // cr44 shadow; toggled for CXXXXX IOs
    U032 InitialBootHead;
    U032 Enable256Burst;
    U032 PixelClock;
    U032 fpHMax;                        // flat panel max h & v
    U032 fpVMax;
    DACCRTCHALINFO CrtcInfo[MAX_CRTCS];
} DACHALINFO, *PDACHALINFO;

// Until an EnableHead is called, the Head2RegOwner is unknown
#define HEAD2_OWNER_UNKNOWN    0xFFFFFFFF

//
// Use these for convenient access to HAL data from within RM (i.e.
// these require a pDev).
//
#define DACHALINFO(nvinfo,a)            (nvinfo->Dac.HalInfo.a)
#define GETCRTCHALINFO(nvinfo,c,a)      (nvinfo->Dac.HalInfo.CrtcInfo[c].a)

#define GETDISPLAYTYPE(nvinfo, c)		(nvinfo->Dac.HalInfo.CrtcInfo[c].DisplayType)
#define SETDISPLAYTYPE(nvinfo, c, v)	(nvinfo->Dac.HalInfo.CrtcInfo[c].DisplayType = v)

#define GETMONITORTYPE(nvinfo, c)		(nvinfo->Dac.HalInfo.CrtcInfo[c].MonitorType)
#define SETMONITORTYPE(nvinfo, c, v)	(nvinfo->Dac.HalInfo.CrtcInfo[c].MonitorType = v)

// HAL versions
#define HAL_GETDISPLAYTYPE(c)		    (pDacHalInfo->CrtcInfo[c].DisplayType)

//
// DAC_CONTROL_000 parameters.
//
typedef struct _def_dac_control_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} DACCONTROLARG_000, *PDACCONTROLARG_000;

// DAC_CONTROL commands.
#define DAC_CONTROL_UPDATE              1
#define DAC_CONTROL_LOAD                2
#define DAC_CONTROL_UNLOAD              3
#define DAC_CONTROL_DESTROY             4
#define DAC_CONTROL_INIT                5
#define DAC_CONTROL_DITHER_DISABLE      6
#define DAC_CONTROL_DITHER_ENABLE       7

//
// Get head assignment for VIDEO_LUT_CURSOR_DAC_OBJECT.
//
#define DACGETHEAD(p)                   (p->HalObject.Head)

//
// DAC_ALLOC_000 parameters.
//
typedef struct _def_dac_alloc_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} DACALLOCARG_000, *PDACALLOCARG_000;

//
// DAC_FREE_000 parameters.
//
typedef struct _def_dac_free_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} DACFREEARG_000, *PDACFREEARG_000;

//
// DAC_SET_START_ADDR_000 parameters.
//
typedef struct _def_dac_set_start_addr_arg_000
{
    NV_HAL_ID id;
    U032 startAddr;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} DACSETSTARTADDRARG_000, *PDACSETSTARTADDRARG_000;

//
// DAC_PROGRAMMCLK_000
//
typedef struct _def_dac_program_mclk_000
{
    NV_HAL_ID id;
    PHALHWINFO pHalHwInfo;
    U032 DisableDLLs;
} DACPROGRAMMCLKARG_000, *PDACPROGRAMMCLKARG_000;

//
// DAC_PROGRAMNVCLK_000
//
typedef struct _def_dac_program_nvclk_000
{
    NV_HAL_ID id;
    PHALHWINFO pHalHwInfo;
} DACPROGRAMNVCLKARG_000, *PDACPROGRAMNVCLKARG_000;

//
// DAC_PROGRAMPCLK_000
//
typedef struct _def_dac_program_pclk_000
{
    NV_HAL_ID id;
    PHALHWINFO pHalHwInfo;
    VOID    *pHalObjInfo;
    U032 PixelClock;
} DACPROGRAMPCLKARG_000, *PDACPROGRAMPCLKARG_000;

//
// DAC_PROGRAMCURSOR_000
//
typedef struct _def_dac_program_curosor_arg_000
{
    NV_HAL_ID id;
    PHALHWINFO pHalHwInfo;
    VOID    *pHalObjInfo;
    U032    startAddr; 
    U032    asi;          // Address space indicator: 0=Instance Mem, 1=Frame buffer
    U032    width; 
    U032    height; 
    U032    colorFormat;
} DACPROGRAMCURSORARG_000, *PDACPROGRAMCURSORARG_000;

#define DAC_ASI_INSTMEM 0
#define DAC_ASI_FRAMEBUFFER 1

//
// DAC_RASTERPOS_000
//
typedef struct _def_dac_raster_pos_arg_000
{
    NV_HAL_ID   id;
    PHALHWINFO  pHalHwInfo;
    VOID        *pHalObjInfo;
    U016        result;
} DACRASTERPOSARG_000, *PDACRASTERPOSARG_000;


//
// Struct used in nvHalDacValidateBandwidth
//
typedef struct _def_bandwidth_params
{
    U032 Enable;
    U032 TotalWidth;
    U032 VisibleWidth;
    U032 TotalHeight;
    U032 VisibleHeight;
    U032 PixelDepth;
    U032 PixelClock;
//  U032 RefreshRate;
} BANDWIDTH_CALC_PARAMS;
//
// DAC_VALIDATEBANDWIDTH_000
//
typedef struct _def_dac_validate_bandwidth_arg_000
{
    NV_HAL_ID   id;
    PHALHWINFO  pHalHwInfo;
    U032        VidScalerHead;
    BANDWIDTH_CALC_PARAMS* head0;
    BANDWIDTH_CALC_PARAMS* head1;
} DACVALIDATEBANDWIDTHARG_000, *PDACVALIDATEBANDWIDTHARG_000;


//
// DAC_VALIDATEARBSETTINGS_000
//
typedef struct _def_dac_validate_arb_settings_arg_000
{
    NV_HAL_ID   id;
    PHALHWINFO  pHalHwInfo;
    U032        result;
    U032        bpp;
    U032        video_enabled;
    U032        mport_enabled;
    U032        vclk;
    U032        head;
} DACVALIDATEARBSETTINGSARG_000, *PDACVALIDATEARBSETTINGSARG_000;

//
// DAC_UPDATEARBSETTINGS_000
//
typedef struct _def_dac_update_arb_settings_000
{
    NV_HAL_ID   id;
    PHALHWINFO  pHalHwInfo;
    VOID        *pHalObjInfo;
} DACUPDATEARBSETTINGSARG_000, *PDACUPDATEARBSETTINGSARG_000;

typedef struct _def_dac_hotplug_interrupts_000
{
    NV_HAL_ID   id;
    PHALHWINFO  pHalHwInfo;
    VOID*       pHalObjInfo;
    U032        HotPlugStatus;
    U032        HotUnplugStatus;
    BOOL        ReadWrite;
    BOOL        Reset;
} DACHOTPLUGINTERRUPTS_000, *PDACHOTPLUGINTERRUPTS_000;


RM_STATUS nvHalDacControl(PHWINFO, U032);
RM_STATUS nvHalDacAlloc(PHWINFO, U032, U032, U032, VOID *);
RM_STATUS nvHalDacFree(PHWINFO, U032, U032, U032, VOID *);
RM_STATUS nvHalDacSetStartAddr(PHWINFO, U032, VOID *);
RM_STATUS nvHalDacProgramMClk(PHWINFO, U032);
RM_STATUS nvHalDacProgramNVClk(PHWINFO);
RM_STATUS nvHalDacProgramPClk(PHWINFO, U032, VOID *);
RM_STATUS nvHalDacProgramCursorImage(PHWINFO, U032, U032, U032, U032, U032, VOID *);
RM_STATUS nvHalDacGetRasterPosition(PHWINFO, U016 *, VOID *);
RM_STATUS nvHalDacValidateBandwidth(PHWINFO, U032, BANDWIDTH_CALC_PARAMS*, BANDWIDTH_CALC_PARAMS*);
RM_STATUS nvHalDacValidateArbSettings(PHWINFO, U032, U032, U032, U032, U032, U032*);
RM_STATUS nvHalDacUpdateArbSettings(PHWINFO, VOID *);
RM_STATUS nvHalDacHotPlugInterrupts(PHWINFO, VOID *, U032 *, U032 *, BOOL, BOOL);


//--------------------------------------------------------------------
// Mediaport
//--------------------------------------------------------------------

// Current HAL mediaport interface revision ids.
#define MP_CONTROL_000                  MKHALID(MP_ENGINE_TAG,0x0001,0x0)
#define MP_ALLOC_000                    MKHALID(MP_ENGINE_TAG,0x0002,0x0)
#define MP_FREE_000                     MKHALID(MP_ENGINE_TAG,0x0003,0x0)
#define MP_METHOD_000                   MKHALID(MP_ENGINE_TAG,0x0004,0x0)
#define MP_GET_EVENT_STATUS_000         MKHALID(MP_ENGINE_TAG,0x0005,0x0)
#define MP_SERVICE_EVENT_000            MKHALID(MP_ENGINE_TAG,0x0006,0x0)

//
// The data in this structure is shared by both the RM and HAL.
//
typedef struct _def_mp_hal_info
{
    U032 Enabled;
    U032 DetectEnable;
    U032 ImageBufferNumber;
    U032 MPCPresent;
    U032 MPCDeviceAddr;
} MPHALINFO, *PMPHALINFO;

//
// MP_CONTROL_000 parameters.
//
typedef struct _def_mp_control_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} MPCONTROLARG_000, *PMPCONTROLARG_000;

// MP_CONTROL commands.
#define MP_CONTROL_UPDATE               1
#define MP_CONTROL_LOAD                 2
#define MP_CONTROL_UNLOAD               3
#define MP_CONTROL_DESTROY              4
#define MP_CONTROL_INIT                 5

//
// HAL NV_EXTERNAL_VIDEO_DECODER object.
//
typedef struct _def_video_decoder_hal_object
{
    U032    ImageDataWidth;
    U032    Task;
    U032    NullData;
    U032    NullValue;
    U032    ImageStartLine;
    struct {
        U032    FirstLine;         
        U032    Height;           
        U032    Offset;
        U032    Pitch;
        U032    Field;
        U032    Notify;
        U032    GetOffsetData;
        PDMAHALOBJINFO   CtxDma; 
    } SetVbi[2];
    struct {
        U032    WidthIn;         
        U032    HeightIn;
        U032    WidthOut;
        U032    HeightOut;
        U032    Offset;           
        U032    Pitch;
        U032    Field;
        U032    Notify;
        U032    GetOffsetData;
        U032    Info32;
        PDMAHALOBJINFO   CtxDma;
    } SetImage[2];
    U032    lastWidthIn;
    U032    lastWidthOut;
    U032    lastHeightIn;
    U032    lastHeightOut;
    U032    lastImageStartLine;  
    U032    lastImageConfig;
} VIDEODECODERHALOBJECT, *PVIDEODECODERHALOBJECT;

#define EVENT_PROCESSED_VBI 0
#define EVENT_PROCESSED_IMAGE 1

//
// HAL NV_EXTERNAL_VIDEO_DECOMPRESSOR object.
//
typedef struct _def_video_decompressor_hal_object
{
    struct {
        U032    Offset;         
        U032    Size;           
        U032    Notify;
        PDMAHALOBJINFO   CtxDma;
    } ReadData[2];
    struct {
        U032    Offset;         
        U032    Size;           
        U032    FormatIn;
        U032    FormatOut;
        U032    Notify;
        PDMAHALOBJINFO   CtxDma;
    } WriteImage[2];
} VIDEODECOMPRESSORHALOBJECT, *PVIDEODECOMPRESSORHALOBJECT;

//
// HAL NV_EXTERNAL_PARALLEL_BUS object.
//
typedef struct _def_parallel_bus_hal_object
{
    U032    BusType;
    struct {
        U016    DataSize;
        U016    AddressSize;
        U032    Address;
        U032    Data;
    } Write;
    struct {
        U016    DataSize;
        U016    AddressSize;
        U032    Address;
        U032    Notify;
        U032    ReadData;
    } Read;
} PARALLELBUSHALOBJECT, *PPARALLELBUSHALOBJECT;

//
// MP_ALLOC_000 parameters.
//
typedef struct _def_mp_alloc_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} MPALLOCARG_000, *PMPALLOCARG_000;

//
// MP_FREE_000 parameters.
//
typedef struct _def_mp_free_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} MPFREEARG_000, *PMPFREEARG_000;

//
// MP_METHOD_000 parameters.
//
typedef struct _def_mp_method_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    VOID *pHalObjInfo;
    U032 offset;
    U032 data;
    U032 mthdStatus;
    PHALHWINFO pHalHwInfo;
} MPMETHODARG_000, *PMPMETHODARG_000;

//
// MP_GET_EVENT_STATUS_000 parameters.
//
typedef struct _def_mp_get_event_status_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    VOID *pHalObjInfo;
    U032 events;
    V032 intrStatus;
    PHALHWINFO pHalHwInfo;
} MPGETEVENTSTATUSARG_000, *PMPGETEVENTSTATUSARG_000;

//
// The events field holds a class-dependent bit mask
// specifying which "event" is to be processed.  For the decoder
// class, an event is a vbi or image buffer completion.  For the
// parallel bus class, it's an interrupt notify completion.

//
// The class04d decoder class uses 4 bits (1:0 for vbi, 3:2 for image).
//
#define CLASS04D_VBI_EVENTS_BASE            0
#define CLASS04D_IMAGE_EVENTS_BASE          2
#define CLASS04D_VBI_EVENT(n)               (1 << (CLASS04D_VBI_EVENTS_BASE+n))
#define CLASS04D_IMAGE_EVENT(n)             (1 << (CLASS04D_IMAGE_EVENTS_BASE+n))

//
// The class04e decompressor class uses 6 bits.
//
#define CLASS04E_VIDEO_EVENTS_BASE          0
#define CLASS04E_AUDIO_EVENTS_BASE          2
#define CLASS04E_IMAGE_EVENTS_BASE          4
#define CLASS04E_VIDEO_EVENT(n)             (1 << (CLASS04E_VIDEO_EVENTS_BASE+n))
#define CLASS04E_AUDIO_EVENT(n)             (1 << (CLASS04E_AUDIO_EVENTS_BASE+n))
#define CLASS04E_IMAGE_EVENT(n)             (1 << (CLASS04E_IMAGE_EVENTS_BASE+n))

//
// The parallel bus class uses a single event constant.
//
#define CLASS04F_PBUS_EVENT                 1

//
// MP_SERVICE_EVENT_000 parameters.
//
typedef struct _def_mp_service_event_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    VOID *pHalObjInfo;
    U032 event;
    V032 intrStatus;
    PHALHWINFO pHalHwInfo;
} MPSERVICEEVENTARG_000, *PMPSERVICEEVENTARG_000;

RM_STATUS nvHalMpControl(PHWINFO, U032);
RM_STATUS nvHalMpAlloc(PHWINFO, U032, U032, U032, VOID *);
RM_STATUS nvHalMpFree(PHWINFO, U032, U032, U032, VOID *);
RM_STATUS nvHalMpMethod(PHWINFO, U032, VOID *, U032, U032, U032 *);
RM_STATUS nvHalMpGetEventStatus(PHWINFO, U032, VOID *, U032 *, V032 *);
RM_STATUS nvHalMpServiceEvent(PHWINFO, U032, VOID *, U032, V032 *);

//--------------------------------------------------------------------
// Video
//--------------------------------------------------------------------

// Current HAL video interface revision ids.
#define VIDEO_CONTROL_000               MKHALID(VIDEO_ENGINE_TAG,0x0001,0x0)
#define VIDEO_ALLOC_000                 MKHALID(VIDEO_ENGINE_TAG,0x0002,0x0)
#define VIDEO_FREE_000                  MKHALID(VIDEO_ENGINE_TAG,0x0003,0x0)
#define VIDEO_METHOD_000                MKHALID(VIDEO_ENGINE_TAG,0x0004,0x0)
#define VIDEO_GET_EVENT_STATUS_000      MKHALID(VIDEO_ENGINE_TAG,0x0005,0x0)
#define VIDEO_SERVICE_EVENT_000         MKHALID(VIDEO_ENGINE_TAG,0x0006,0x0)

//
// The data in this structure is shared by both the RM and HAL.
//
typedef struct _def_video_hal_info
{
    U032 OverlayAllowed;
    U032 Head;
    U032 UpdateFlags;
    U032 FinishFlags;
    U032 Enabled;
    U032 ColorKeyEnabled;
    U032 ScaleFactor;       // 12.20
    U032 VideoStart;
    U032 VideoSize;
    U032 VideoScaleX;       // nv10's scale factors are 20/21 bits long
    U032 VideoScaleY;       // and need to be kept in separate dwords
    U032 VideoColorFormat;
    U032 VideoColor;
    U032 OverlayMaxDownscale_768;
    U032 OverlayMaxDownscale_1280;
    U032 OverlayMaxDownscale_1920;
    U032 CustomizationCode;
    VOID_PTR ActiveVideoOverlayObject;
    U032 PanOffsetDeltaX[MAX_CRTCS];
    U032 PanOffsetDeltaY[MAX_CRTCS];
} VIDEOHALINFO, *PVIDEOHALINFO;

//
// VIDEO_CONTROL_000 parameters.
//
typedef struct _def_video_control_arg_000
{
    NV_HAL_ID id;
    U032 cmd;
    PHALHWINFO pHalHwInfo;
} VIDEOCONTROLARG_000, *PVIDEOCONTROLARG_000;

// VIDEO_CONTROL commands.
#define VIDEO_CONTROL_UPDATE            1
#define VIDEO_CONTROL_LOAD              2
#define VIDEO_CONTROL_UNLOAD            3
#define VIDEO_CONTROL_DESTROY           4
#define VIDEO_CONTROL_INIT              5
#define VIDEO_CONTROL_ENABLE            6
#define VIDEO_CONTROL_DISABLE           7

//
// Video buffer kick off interface.
//
typedef RM_STATUS (*PVIDEOKICKOFFPROC)(PHALHWINFO, VOID *, U032);

//
// Video engine defines.
//
#define MAX_OVERLAY_BUFFERS             2

#define OVERLAY_BUFFER_IDLE             0
#define OVERLAY_BUFFER_BUSY             1
#define OVERLAY_BUFFER_NOTIFY_PENDING   2
#define OVERLAY_BUFFER_STOP_PENDING     3

//
// HAL NV_VIDEO_FROM_MEMORY object.
//
typedef struct _def_video_from_mem_hal_object
{
    PVIDEOKICKOFFPROC KickOffProc;
    U032 InitState;
    U032 Format;
    struct {
        U032 State;
        U032 Start;
        U032 Pitch;
        U032 Length;
        U032 Width;
        U032 VideoStart;
        U032 VideoSize;
        U032 VideoScaleX;
        U032 VideoScaleY;
        U032 VideoColorFormat;
        U032 VideoColor;
        PDMAHALOBJINFO bufferCtxDma;
    } Buffer[MAX_OVERLAY_BUFFERS];
} VIDEOFROMMEMHALOBJECT, *PVIDEOFROMMEMHALOBJECT;

//
// HAL NV_VIDEO_SCALER object.
//
typedef struct _def_video_scaler_hal_object
{
    V032 DeltaDuDx;
    V032 DeltaDvDy;
    V032 xStart;   
    V032 yStart;   
} VIDEOSCALERHALOBJECT, *PVIDEOSCALERHALOBJECT;

//
// HAL NV_VIDEO_COLOR_KEY object.
//
typedef struct _def_video_colorkey_hal_object
{
    U032 ColorFormat;
    U032 Color;
    U032 xClipMin;
    U032 xClipMax;
    U032 yClipMin;
    U032 yClipMax; 
} VIDEOCOLORKEYHALOBJECT, *PVIDEOCOLORKEYHALOBJECT;

//
// HAL video overlay object for use by classes:
//   NV04_VIDEO_OVERLAY
//   NV10_VIDEO_OVERLAY
//

typedef struct _def_video_overlay_hal_object
{
    PVIDEOKICKOFFPROC KickOffProc;
	U032 PvideoBufferCopy;
	U032 PvideoStopActive;
    struct {
        PDMAHALOBJINFO OverlayCtxDma;
		U032 ColorKey;
		U032 Luminance;
		U032 Chrominance;
        U032 Offset;                       
        U016 SizeIn_width;
        U016 SizeIn_height;
        U016 SizeOut_width;
        U016 SizeOut_height;
        U016 PointIn_s;
        U016 PointIn_t;
        U016 PointOut_x;
        U016 PointOut_y;
        U032 OverlayStart;
        U032 DsDx;
        U032 DtDy;
        U032 ScaleFactor;
        U032 WindowStart; 
        U032 WindowSize;
        U032 Pitch;                        
        U032 OverlayFormat;
        U032 DisplayWhen;
        U032 State;
		U016 FormatPitch;
		U016 FormatColor;
		U016 FormatDisplay;
		U016 FormatMatrix;
    } Overlay[MAX_OVERLAY_BUFFERS];
} VIDEO_OVERLAY_HAL_OBJECT, *PVIDEO_OVERLAY_HAL_OBJECT;

#define OVERLAY_BUFFER_IS_IDLE(obj,buffer) (obj->Overlay[buffer].State == OVERLAY_BUFFER_IDLE)

//
// VIDEO_ALLOC_000 parameters.
//
typedef struct _def_video_alloc_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} VIDEOALLOCARG_000, *PVIDEOALLOCARG_000;

//
// VIDEO_FREE_000 parameters.
//
typedef struct _def_video_free_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    U032 instance;
    U032 chid;
    VOID *pHalObjInfo;
    PHALHWINFO pHalHwInfo;
} VIDEOFREEARG_000, *PVIDEOFREEARG_000;

//
// VIDEO_METHOD_000 parameters.
//
typedef struct _def_video_method_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    VOID *pHalObjInfo;
    U032 offset;
    U032 data;
    U032 mthdStatus;
    PHALHWINFO pHalHwInfo;
} VIDEOMETHODARG_000, *PVIDEOMETHODARG_000;

//
// VIDEO_GET_EVENT_STATUS_000 parameters.
//
typedef struct _def_video_get_event_status_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    VOID *pHalObjInfo;
    U032 events;
    V032 intrStatus;
    PHALHWINFO pHalHwInfo;
} VIDEOGETEVENTSTATUSARG_000, *PVIDEOGETEVENTSTATUSARG_000;

//
// VIDEO_SERVICE_EVENT_000 parameters.
//
typedef struct _def_video_service_event_arg_000
{
    NV_HAL_ID id;
    U032 classNum;
    VOID *pHalObjInfo;
    U032 events;
    V032 intrStatus;
    PHALHWINFO pHalHwInfo;
} VIDEOSERVICEEVENTARG_000, *PVIDEOSERVICEEVENTARG_000;

RM_STATUS nvHalVideoControl(PHWINFO, U032);
RM_STATUS nvHalVideoAlloc(PHWINFO, U032, U032, U032, VOID *);
RM_STATUS nvHalVideoFree(PHWINFO, U032, U032, U032, VOID *);
RM_STATUS nvHalVideoMethod(PHWINFO, U032, VOID *, U032, U032, U032 *);
RM_STATUS nvHalVideoGetEventStatus(PHWINFO, U032, VOID *, U032 *, V032 *);
RM_STATUS nvHalVideoServiceEvent(PHWINFO, U032, VOID *, U032, V032 *);

//--------------------------------------------------------------------
// HAL device data.
//--------------------------------------------------------------------

//
// Each engine in the chip has the following state (setup by the RM):
//
//  - HalInfo       = Pointer to state held in pDev (shared between RM and HAL)
//  - HalPvtInfo    = Pointer to malloc'd system memory for engine state that
//                    is "private" to the HAL
//  - MallocBuffer  = Pointer to temporary malloced buffer for engine state
//                    transitions (e.g. save areas for STATE_LOAD calls)
//  - InstMem       = Offset into instance memory for engine state needed
//                    by hardware
//
// The amount of space (if any) to setup for these fields is determined
// by the contents of the chip-specific engine descriptor table (see
// kernel/inc/nvromdat.h).
//
struct _def_hal_hw_info
{
    PHWREG nvBaseAddr;                  // pointer to mapped registers
    PHWREG fbBaseAddr;                  // pointer to mapped framebuffer

    PHALRMFUNCS pHalRmFuncs;            // pointer to rm services table
    VOID_PTR pDeviceId;                 // pDev reference handle

    PPROCESSORHALINFO pProcessorHalInfo;
    PPRAMHALINFO pPramHalInfo;

    PMCHALINFO pMcHalInfo;
    VOID_PTR pMcHalPvtInfo;
    VOID_PTR pMcMallocBuffer;
    U032 mcInstMem;

    PFIFOHALINFO pFifoHalInfo;
    VOID_PTR pFifoHalPvtInfo;
    VOID_PTR pFifoMallocBuffer;
    U032 fifoInstMem;

    PFBHALINFO pFbHalInfo;
    VOID_PTR pFbHalPvtInfo;
    VOID_PTR pFbMallocBuffer;
    U032 fbInstMem;

    PGRHALINFO pGrHalInfo;
    VOID_PTR pGrHalPvtInfo;
    VOID_PTR pGrMallocBuffer;
    U032 grInstMem;

    PDMAHALINFO pDmaHalInfo;
    VOID_PTR pDmaHalPvtInfo;
    VOID_PTR pDmaMallocBuffer;
    U032 dmaInstMem;

    PDACHALINFO pDacHalInfo; 
    VOID_PTR pDacHalPvtInfo;
    VOID_PTR pDacMallocBuffer;
    U032 dacInstMem;

    PMPHALINFO pMpHalInfo;
    VOID_PTR pMpHalPvtInfo;
    VOID_PTR pMpMallocBuffer;
    U032 mpInstMem;

    PVIDEOHALINFO pVideoHalInfo;
    VOID_PTR pVideoHalPvtInfo;
    VOID_PTR pVideoMallocBuffer;
    U032 videoInstMem;
};

//--------------------------------------------------------------------
// HAL function table.
// The format of this table is ROM revision dependent.
//--------------------------------------------------------------------
typedef struct _def_halfuncs_000
{
    // mc interfaces
    RM_STATUS (*_nvHalMcControl)(VOID *);
    RM_STATUS (*_nvHalMcPower)(VOID *);

    // fifo interfaces
    RM_STATUS (*_nvHalFifoControl)(VOID *);
    RM_STATUS (*_nvHalFifoAllocPio)(VOID *);
    RM_STATUS (*_nvHalFifoAllocDma)(VOID *);
    RM_STATUS (*_nvHalFifoFree)(VOID *);
    RM_STATUS (*_nvHalFifoGetExceptionData)(VOID *);
    RM_STATUS (*_nvHalFifoService)(VOID *);
    RM_STATUS (*_nvHalFifoAccess)(VOID *);
    RM_STATUS (*_nvHalFifoHashAdd)(VOID *);
    RM_STATUS (*_nvHalFifoHashDelete)(VOID *);
    RM_STATUS (*_nvHalFifoHashFunc)(VOID *);
    RM_STATUS (*_nvHalFifoHashSearch)(VOID *);

    // framebuffer interfaces
    RM_STATUS (*_nvHalFbControl)(VOID *);
    RM_STATUS (*_nvHalFbAlloc)(VOID *);
    RM_STATUS (*_nvHalFbFree)(VOID *);
    RM_STATUS (*_nvHalFbSetAllocParameters)(VOID *);
    RM_STATUS (*_nvHalFbGetSurfacePitch)(VOID *);
    RM_STATUS (*_nvHalFbGetSurfaceDimensions)(VOID *);
    RM_STATUS (*_nvHalFbLoadOverride)(VOID *);

    // graphics interfaces
    RM_STATUS (*_nvHalGrControl)(VOID *);
    RM_STATUS (*_nvHalGrAlloc)(VOID *);
    RM_STATUS (*_nvHalGrFree)(VOID *);
    RM_STATUS (*_nvHalGrGetExceptionData)(VOID *);
    RM_STATUS (*_nvHalGrService)(VOID *);
    RM_STATUS (*_nvHalGrGetNotifyData)(VOID *);
    RM_STATUS (*_nvHalGrSetObjectContext)(VOID *);
    RM_STATUS (*_nvHalGrLoadOverride)(VOID *);

    // dma interfaces
    RM_STATUS (*_nvHalDmaControl)(VOID *);
    RM_STATUS (*_nvHalDmaAlloc)(VOID *);
    RM_STATUS (*_nvHalDmaFree)(VOID *);
    RM_STATUS (*_nvHalDmaGetInstSize)(VOID *);

    // dac interfaces
    RM_STATUS (*_nvHalDacControl)(VOID *);
    RM_STATUS (*_nvHalDacAlloc)(VOID *);
    RM_STATUS (*_nvHalDacFree)(VOID *);
    RM_STATUS (*_nvHalDacSetStartAddr)(VOID *);
    RM_STATUS (*_nvHalDacProgramMClk)(VOID *);
    RM_STATUS (*_nvHalDacProgramNVClk)(VOID *);
    RM_STATUS (*_nvHalDacProgramPClk)(VOID *);
    RM_STATUS (*_nvHalDacProgramCursorImage)(VOID *);
    RM_STATUS (*_nvHalDacGetRasterPosition)(VOID *);
    RM_STATUS (*_nvHalDacValidateBandwidth)(VOID *);
    RM_STATUS (*_nvHalDacValidateArbSettings)(VOID *);
    RM_STATUS (*_nvHalDacUpdateArbSettings)(VOID *);
    RM_STATUS (*_nvHalDacHotPlugInterrupts)(VOID *);

    // media port interfaces
    RM_STATUS (*_nvHalMpControl)(VOID *);
    RM_STATUS (*_nvHalMpAlloc)(VOID *);
    RM_STATUS (*_nvHalMpFree)(VOID *);
    RM_STATUS (*_nvHalMpMethod)(VOID *);
    RM_STATUS (*_nvHalMpGetEventStatus)(VOID *);
    RM_STATUS (*_nvHalMpServiceEvent)(VOID *);

    // video interfaces
    RM_STATUS (*_nvHalVideoControl)(VOID *);
    RM_STATUS (*_nvHalVideoAlloc)(VOID *);
    RM_STATUS (*_nvHalVideoFree)(VOID *);
    RM_STATUS (*_nvHalVideoMethod)(VOID *);
    RM_STATUS (*_nvHalVideoGetEventStatus)(VOID *);
    RM_STATUS (*_nvHalVideoServiceEvent)(VOID *);
} HALFUNCS_000, *PHALFUNCS_000;

typedef VOID *PHALFUNCS;

#define HALFUNC_000(i)                  (*((PHALFUNCS_000)pDev->pHalFuncs)->i)

extern HALFUNCS_000 NV04_HalFuncs_000;
extern HALFUNCS_000 NV10_HalFuncs_000;
extern HALFUNCS_000 NV20_HalFuncs_000;

//--------------------------------------------------------------------
// RM entry points for use by HAL.
//--------------------------------------------------------------------

struct _def_hal_rm_funcs_000
{
    // interrupt service request interface
    //   engine, phalhwinfo
    RM_STATUS (*_nvHalRmServiceIntr)(VOID *, U032);

    // debug print routines
    VOID (*_nvHalRmPrintString) (VOID *, U032, char*);
    VOID (*_nvHalRmPrintValue) (VOID *, U032, U032);
    VOID (*_nvHalRmPrintStringValue) (VOID *, U032, char*, U032);
    VOID (*_nvHalRmPrintStringPtr) (VOID *, U032, char*, VOID*);
    VOID (*_nvHalRmPrintStringVal64) (VOID *, U032, char*, U064);

    // register i/o interfaces
    VOID (*_nvHalRmRegWr08) (VOID *, U032, U008);
    U008 (*_nvHalRmRegRd08) (VOID *, U032);

    //   *address = value
    VOID (*_nvHalRmRegWr32) (VOID *, U032, U032);
    //   value = *address
    U032 (*_nvHalRmRegRd32) (VOID *, U032);

    VOID (*_nvHalRmBreakPoint) (VOID *);
    RM_STATUS (*_nvHalRmDelay) (VOID *, U032);

    // i2c read/write interfaces
    RM_STATUS (*_nvHalRmI2cRead) (VOID *, U032, U008, U016, U008 *, U016, U008 *);
    RM_STATUS (*_nvHalRmI2cWrite) (VOID *, U032, U008, U016, U008 *, U016, U008 *);
};

// from kernel/hal/halrm.c
extern HALRMFUNCS_000 HalRmFuncs_000;

// 
// RM service macros.
// 

#define HALRMSERVICEINTR(p,e)   (*((PHALRMFUNCS_000)((PHALHWINFO)p)->pHalRmFuncs)->_nvHalRmServiceIntr)((PHALHWINFO)p->pDeviceId,e)

#define HAL_GR_IDLE(p) \
    while ((volatile U032)REG_RD32(NV_PGRAPH_STATUS))  {         \
        V032 pmc;                                                \
        pmc = REG_RD32(NV_PMC_INTR_0);                           \
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING)) {   \
            (void)HALRMSERVICEINTR((PHALHWINFO)p,GR_ENGINE_TAG); \
        }                                                        \
        if (pmc & (DRF_DEF(_PMC, _INTR_0, _PCRTC,  _PENDING) |   \
                   DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING))) { \
            (void)HALRMSERVICEINTR((PHALHWINFO)p,DAC_ENGINE_TAG);\
        }                                                        \
    }

//
// RM ic2 read/write macros.
//
#define HALRMI2CREAD(p,id,chipadr,adrlen,addr,datalen,data) \
    (*((PHALRMFUNCS_000)((PHALHWINFO)p)->pHalRmFuncs)->_nvHalRmI2cRead)((PHALHWINFO)p->pDeviceId,id,chipadr,adrlen,addr,datalen,data)
#define HALRMI2CWRITE(p,id,chipadr,adrlen,addr,datalen,data) \
    (*((PHALRMFUNCS_000)((PHALHWINFO)p)->pHalRmFuncs)->_nvHalRmI2cWrite)((PHALHWINFO)p->pDeviceId,id,chipadr,adrlen,addr,datalen,data)

//--------------------------------------------------------------------
// RM routines.
//--------------------------------------------------------------------

// kernel/hal/halinit.c
RM_STATUS initHal(PHWINFO);
RM_STATUS initHalMappings(PHWINFO);
RM_STATUS destroyHal(PHWINFO);
RM_STATUS allocHalEngineInstMem(PHWINFO);
RM_STATUS freeHalEngineInstMem(PHWINFO);

#endif // _NVHAL_H_
