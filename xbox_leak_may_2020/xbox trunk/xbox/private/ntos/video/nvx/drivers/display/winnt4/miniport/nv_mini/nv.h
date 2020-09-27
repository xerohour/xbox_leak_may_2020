//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     nv.h
//
// Abstract:
//
//     This module contains the definitions for the code that implements the
//     NVidia NV device driver.
//
// Environment:
//
//     Kernel mode
//
// Revision History:
//
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

//******************************************************************************
// Include IOCTL codes to be shared among all components of driver
//******************************************************************************

#include <dspioctl.h>
#include "ntnvreg.h"

#ifdef NVPE
#include "nvpecntx.h"
#endif // NVPE

#include "nvMultiMon.h"

// Uncomment this line to enable the common modeset code
//#define ENABLE_COMMON_MODESET_CODE

// Uncomment this line to enable exporting the EDID data in nvGetChildDescriptor().
// Terry needs to automatically disble EDID processing when the user wants to switch to spanning modes before
// This code can be enabled.
// #define ENABLE_EXPORTING_EDID_TO_WIN2K

// Uncomment this line to enable the new ResMan power management API code
#define ENABLE_RESMAN_ACPI_CODE

//
// Win2K currently supports 8 different power states as defined in ntddvdeo.h
// We use 16 slots just as a cushion for future additional power states in succeeding
// versions of Win2K.
//
#define MAX_WIN2K_POWER_STATES 16
//
// The only power states of relevance in Win2K currently are on, standby, suspend, off, hibernate
//
#define NV_VideoPowerUnspecified 0
#define NV_VideoPowerOn 1
#define NV_VideoPowerStandBy 2
#define NV_VideoPowerSuspend 3
#define NV_VideoPowerOff 4
#define NV_VideoPowerHibernate 5
#define NV_VideoPowerShutdown 6


//******************************************************************************
// Common typedefs
//******************************************************************************

typedef unsigned char   U008;
typedef unsigned short  U016;
typedef unsigned long   U032;

typedef unsigned char   V008;
typedef unsigned short  V016;
typedef unsigned long   V032;

typedef unsigned long   BOOL;
typedef long            S032;

#define PHYSICAL_ADDRESS_VALUE(p) ((p).LowPart)

//******************************************************************************
// Polymorphic type stubs for NT4
//******************************************************************************

#if (_WIN32_WINNT < 0x0500)
#define SIZE_T ULONG
#define ULONG_PTR ULONG
#endif // _WIN32_WINNT < 0x0500

//******************************************************************************
// Monitor types  -> Values MUST agree with those from RM !!!
//******************************************************************************

#define NV_MONITOR_VGA                  0
#define NV_MONITOR_NTSC                 2
#define NV_MONITOR_PAL                  3

// Use the following for monitor types. The ones above are obsolete names.
#define MONITOR_TYPE_VGA                0
#define MONITOR_TYPE_NTSC               2
#define MONITOR_TYPE_PAL                3
#define MONITOR_TYPE_FLAT_PANEL         4

// Use the following for display types. The ones above are obsolete names.
#define DISPLAY_TYPE_MONITOR            0
#define DISPLAY_TYPE_TV                 1
#define DISPLAY_TYPE_DUALSURFACE        2
#define DISPLAY_TYPE_FLAT_PANEL         3

//******************************************************************************
// Our hardcoded defines
//******************************************************************************

#define MAX_CLIP_REGIONS      16
#define MAX_GRPATCH_FANOUT    56
#define MAX_GRPATCH_INPUT     56
#define NUM_PATCHES           10
#define NUM_SUBCHANNELS       8
#define NUM_GRAPHICS_DEVICES  0x1e
#define INVALID               0xffffffff;
#define BIOS_STR_MAX 50

//******************************************************************************
// Size of saved BIOS image (used for manual posting)
//******************************************************************************

#define SAVED_BIOS_IMAGE_SIZE   0x10000

//******************************************************************************
// Bios memory sizing adjust value (for RM Post Device memory sizing)
//******************************************************************************

#define BIOS_MEM_SIZE_ADJUST_FACTOR 0x00010000;

//******************************************************************************
// EDID buffer size (in bytes)
//******************************************************************************

#define EDID_V2_SIZE   256

//******************************************************************************
// Max Size in bytes of saved instance memory (used when powering down)
//******************************************************************************

#define MAX_INSTANCE_MEM_SIZE   0x30000

//******************************************************************************
// Enough memory to store registry data
//
// Set MAX of 30 numbers per mode entry (each line) in INF
// Set MAX of 90  mode entries ( MAX 30 width_height * 3 (for depths))
// Data table will consist of type U016
//
//          Registry Data Format (word values)
//              MAX word values per line = 30 word values
//              MAX of 90 lines (90 mode entries)
//
//           BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
//           BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
//           BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
//           BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
//           etc...
//******************************************************************************

#define MAX_STRING_REGISTRY_VALUES  30
#define MAX_STRING_REGISTRY_LINES   90
#define MAX_STRING_REGISTRY_DATA_WORDS (MAX_STRING_REGISTRY_VALUES * MAX_STRING_REGISTRY_LINES)

//******************************************************************************
// Enough memory to store valid mode table data
// Set MAX of 1800 mode entries ( MAX 30 width/height * 3 (for depths) * MAX 20 refresh rates)
// Each struct entry  has 5 entries (valid flag, width,height, bpp, refresh)
// Data table will consist of type U016
//******************************************************************************

#define MAX_VALID_MODE_REFRESH_RATES 20
#define MAX_VALID_MODE_TABLE_DATA_WORDS (MAX_STRING_REGISTRY_LINES * MAX_VALID_MODE_REFRESH_RATES * 5)


#define MAX_BIOS_SCAN 1024

//******************************************************************************
// VSync/HSync defines
//******************************************************************************

#define BUFFER_HSYNC_NEGATIVE  0
#define BUFFER_HSYNC_POSITIVE  1

//******************************************************************************
// Memory types
//******************************************************************************

#define NV_BUFFER_SDRAM            0
#define NV_BUFFER_SGRAM            1

//******************************************************************************
// Resolution defines
//******************************************************************************
#define RESOLUTION_640X480      0
#define RESOLUTION_320X240      3
#define RESOLUTION_320X400      6
#define RESOLUTION_400X300      9
#define RESOLUTION_480X360      12
#define RESOLUTION_512X384      15
#define RESOLUTION_640X400      18
#define RESOLUTION_320X200      21
#define RESOLUTION_800X600      24
#define RESOLUTION_960X720      27
#define RESOLUTION_1024X768     30
#define RESOLUTION_1152X864     33
#define RESOLUTION_1280X1024    36
#define RESOLUTION_1600X1200    39
#define RESOLUTION_1800X1440    42
#define RESOLUTION_1920X1080    45
#define RESOLUTION_1920X1200    48

//******************************************************************************
// Various NV Chip ID's
//******************************************************************************

#define NV1_REV_B_02             2
#define NV1_REV_B_03             3
#define NV1_REV_C_01             4
#define NV1_REV_C_02             5
#define NV3_REV_A_00             0
#define NV3_REV_B_00             1
#define NV3_REV_C_00             2


//******************************************************************************
// NV1 Color Format values to be used to generate CONTEXT values
// for STATIC image objects. (Assumes only Destination Buffer 0 enabled)
//******************************************************************************

#define COLOR_FORMAT_X17R5G5B5        0x00000000
#define COLOR_FORMAT_X16A1R5G5B5      0x00002000
#define COLOR_FORMAT_X8R8G8B8         0x00000200
#define COLOR_FORMAT_A8R8G8B8         0x00002200
#define COLOR_FORMAT_X2R10G10B10      0x00000400
#define COLOR_FORMAT_A2R10G10B10      0x00002400
#define COLOR_FORMAT_X24Y8            0x00000600
#define COLOR_FORMAT_X16A8Y8          0x00002600

//******************************************************************************
// NV3 Color Format values to be used to generate CONTEXT values
// for STATIC image objects. (Assumes only Destination Buffer 0 enabled)
// See PGRAPH_CTX_SWITCH register definition for more information.
//******************************************************************************

#define NV3_COLOR_FORMAT_X17R5G5B5        (0x00000000)
#define NV3_COLOR_FORMAT_X16A1R5G5B5      (0x00000008 | 0x00000000)
#define NV3_COLOR_FORMAT_X8R8G8B8         (0x00000001)
#define NV3_COLOR_FORMAT_A8R8G8B8         (0x00000008 | 0x00000001)
#define NV3_COLOR_FORMAT_X2R10G10B10      (0x00000002)
#define NV3_COLOR_FORMAT_A2R10G10B10      (0x00000008 | 0x00000002)
#define NV3_COLOR_FORMAT_X24Y8            (0x00000003)
#define NV3_COLOR_FORMAT_X16A8Y8          (0x00000008 | 0x00000003)
#define NV3_COLOR_FORMAT_Y16                           (0x00000004)


//******************************************************************************
// Monochrome Format values to be used to generate CONTEXT values
// See the CTX_SWITCH register definition for more information (Bit 14)
//******************************************************************************

#define MONO_FORMAT_LE        0x00000000
#define MONO_FORMAT_CGA6      0x00004000

#define NV3_MONO_FORMAT_LE    0x00000000
#define NV3_MONO_FORMAT_CGA6  0x00000100

//******************************************************************************
// NV1 Cursor defines
//******************************************************************************

#define NV1_DAC_CURSOR_TWO_COLOR_XOR    2
#define NV1_DAC_CURSOR_TWO_COLOR        3

//******************************************************************************
// ID values for STATIC image objects
// (Index into the CommonObject array - Currently allow up to 10)
//******************************************************************************

#define IMAGE_SOLID_ID                 0
#define IMAGE_BLACK_RECTANGLE_ID       1
#define IMAGE_PATTERN_ID               2
#define IMAGE_ROP_ID                   3

//******************************************************************************
// ID values for the patches, each is associated with a Rendering object
// (Index into the Render Object array - Currently allow up to 15)
//******************************************************************************

#define RECTANGLE_PATCH                0
#define TRIANGLE_PATCH                 1
#define BLIT_PATCH                     2
#define IMAGE_FROM_CPU_PATCH           3
#define IMAGE_MONO_FROM_CPU_PATCH      4
#define D3DTRIANGLE_PATCH              5
#define CLEARZ_PATCH                   6
#define SRCIMAGE_IN_MEMORY_PATCH       7
#define DSTIMAGE_IN_MEMORY_PATCH       8
#define MEMORY_TO_MEMORY_PATCH         9
#define D3DTRIANGLE_SYSMEM_PATCH       10
#define RENDER_SOLID_LIN_PATCH         11

//******************************************************************************
// Special cased ROP3 operations
//******************************************************************************

#define BLACKNESS 0x0000
#define DSTINVERT 0x0055
#define PATINVERT 0x005A
#define SRCAND    0x0088
#define DSTCOPY   0x00AA
#define SRCCOPY   0x00CC
#define SRCPAINT  0x00EE
#define PATCOPY   0x00F0
#define WHITENESS 0x00FF
//******************************************************************************
// Number of dacs
//******************************************************************************
#define NV_NO_DACS                  2

//******************************************************************************
// maximum Number of output device connectors on a board.
//******************************************************************************
#define NV_NO_CONNECTORS                  24

//******************************************************************************
// Type of video ram
//******************************************************************************

#define BUFFER_DRAM 0
#define BUFFER_VRAM 1

//******************************************************************************
// Page size definitions.
//******************************************************************************
#if defined(_M_IA64)

#define PAGE_SIZE 0x2000

#elif defined(_M_IX86)

#define PAGE_SIZE 0x1000

#endif

//******************************************************************************
// This cursor structure MUST also match the one defined in driver.h
// (in the display driver directory)
//******************************************************************************

typedef struct _TV_CURSOR_ADJUST_INFO
    {
    ULONG   MonitorType;
    ULONG   Underscan_x;
    ULONG   Underscan_y;
    ULONG   Scale_x;
    ULONG   Scale_y;
    ULONG   FilterEnable;
    ULONG   TVCursorMin;
    ULONG   TVCursorMax;
    } TV_CURSOR_ADJUST_INFO;


//******************************************************************************
// Current PRAMIN map: (Currently only 64k is allocated for total PRAMIN size)
//                      in NV3_IsPresent()
//
// Offset   Type of Instance Data
// ======   =====================
//
//          ------------------------------   <--- Start of PRAMIN area
// 0        Hash Table                       ^
//          Run Out                          |
//          Fifo Context                     |
//          Free Instances                   |
//                                           |
//                                           |
//                                           |
//          ------------------------------   |
// 16k                                       |
//                                           |
//                                           |
//                                           |
//                                           |
//                                           |
//                                           |
//          ------------------------------   |----> 64k allocated for PRAMIN
// 32k      VRAM Texture Page Table Entries  |
//          (Used by D3DTriangle Object for  |
//           textures in VRAM)               |
//                                           |
// 42k      SYSMEM Texture Page Table Entreis|
//          (Used by the D3dTriangle Object  |
//           for textures in System Memory)  |
//                                           |
//          ------------------------------   |
// 48k      Cursor data (8 cached images)    |
//          Each bitmap takes up 2k          |
//                                           |
//                                           |
//                                           |
//                                           v
// 64k      ------------------------------
//
//******************************************************************************

//******************************************************************************
// Types of Engine known to FIFO (NV3)
//******************************************************************************

#define ENGINE_SW       0
#define ENGINE_GRAPHICS 1

//******************************************************************************
// Dedicated privileged device ram size in bytes. (NV1)
//******************************************************************************

#define NV_PRAM_DEVICE_SIZE_12_KBYTES   0x3000  // 12KB
#define NV_PRAM_DEVICE_SIZE_20_KBYTES  0x5000   // 20KB
#define NV_PRAM_DEVICE_SIZE_36_KBYTES  0x9000   // 36KB
#define NV_PRAM_DEVICE_SIZE_68_KBYTES  0x11000  // 68KB

//******************************************************************************
// NV3's privileged size, minus audio (NV3)
//******************************************************************************

#define NV_PRAM_DEVICE_SIZE_5_5_KBYTES   0x1600 // 5.5KB
#define NV_PRAM_DEVICE_SIZE_9_5_KBYTES   0x2600 // 9.5KB
#define NV_PRAM_DEVICE_SIZE_17_5_KBYTES  0x4600 // 17.5KB
#define NV_PRAM_DEVICE_SIZE_33_5_KBYTES  0x8600 // 33.5KB

//******************************************************************************
// Offscreen Instance Memory size.
//******************************************************************************

#define NV_PRAM_MIN_SIZE_INSTANCE_MEM  0x1000   // 4KB

#define BUFFER_HSYNC_NEGATIVE           0
#define BUFFER_HSYNC_POSITIVE           1
#define BUFFER_VSYNC_NEGATIVE           0
#define BUFFER_VSYNC_POSITIVE           1
#define BUFFER_CSYNC_DISABLED           0
#define BUFFER_CSYNC_ENABLED            1

//******************************************************************************
// Forward declaration of all structures
//******************************************************************************

typedef struct _def_object                  OBJECT,*POBJECT;
typedef struct _def_common_object           COMMONOBJECT, *PCOMMONOBJECT;
typedef struct _def_render_common           RENDERCOMMON, *PRENDERCOMMON;
typedef struct _def_render_common_object    RENDERCOMMONOBJECT, *PRENDERCOMMONOBJECT;
typedef struct _def_image_blt_object        IMAGEBLITOBJECT, *PIMAGEBLITOBJECT;
typedef struct _def_image_mem_object        IMAGEMEMOBJECT, *PIMAGEMEMOBJECT;
typedef struct _def_render_object           RENDEROBJECT, *PRENDEROBJECT;
typedef struct _def_render_beta_object      RENDERBETAOBJECT, *PRENDERBETAOBJECT;
typedef struct _def_video_object            VIDEOOBJECT, *PVIDEOOBJECT;
typedef struct _def_video_patchcord         VIDEOPATCHCORD, *PVIDEOPATCHCORD;
typedef struct _def_video_switch_object     VIDEOSWITCHOBJECT, *PVIDEOSWITCHOBJECT;
typedef struct _def_ordinal_object          ORDINALOBJECT, *PORDINALOBJECT;
typedef struct _def_video_sink              VIDEOSINKOBJECT, *PVIDEOSINKOBJECT;
typedef struct _def_video_colormap_object   VIDEOCOLORMAPOBJECT, *PVIDEOCOLORMAPOBJECT;
typedef struct _def_image_object            IMAGEOBJECT, *PIMAGEOBJECT;
typedef struct _def_image_patchcord         IMAGEPATCHCORD, *PIMAGEPATCHCORD;
typedef struct _def_image_video_object      IMAGEVIDEOOBJECT, *PIMAGEVIDEOOBJECT;
typedef struct _def_rop_patchcord           ROPPATCHCORD, *PROPPATCHCORD;
typedef struct _def_solid_object            SOLIDOBJECT, *PSOLIDOBJECT;
typedef struct _def_black_rect_object       CLIPOBJECT, *PCLIPOBJECT;
typedef struct _def_pattern_object          PATTERNOBJECT, *PPATTERNOBJECT;
typedef struct _def_rop_object              ROPOBJECT, *PROPOBJECT;
typedef struct _def_beta_object             BETAOBJECT, *PBETAOBJECT;
typedef struct _def_beta_max_object         BETAMAXOBJECT, *PBETAMAXOBJECT;
typedef struct _def_bool_object             BOOLOBJECT, *PBOOLOBJECT;
typedef struct _def_blend_object            BLENDOBJECT, *PBLENDOBJECT;
typedef struct _def_beta_patchcord          BETAPATCHCORD, *PBETAPATCHCORD;
typedef struct _def_stencil_object          STENCILOBJECT, *PSTENCILOBJECT;
typedef struct _def_colorkey_object         COLORKEYOBJECT, *PCOLORKEYOBJECT;
typedef struct _def_plane_mask_object       PLANEMASKOBJECT, *PPLANEMASKOBJECT;
typedef struct _def_fifo                    FIFO, *PFIFO;
typedef struct _def_patch_context           PATCHCONTEXT, *PPATCHCONTEXT;
typedef struct _def_graphics_patch          GRAPHICSPATCH, *PGRAPHICSPATCH;
typedef struct _def_graphics_channel        GRAPHICSCHANNEL, *PGRAPHICSCHANNEL;
typedef struct _def_canvas                  CANVAS, *PCANVAS;
typedef struct _def_node                    NODE, *PNODE;

//******************************************************************************
// Regular Object
//******************************************************************************


struct _def_object
{
    U032       Name;
    U032       ChID;
    U032       ClassType;
};

//******************************************************************************
// Common Object
//******************************************************************************

struct _def_common_object
{
    OBJECT      Base;
    U032        Valid;
    U032        NotifyPending;
    U032        Context;
    U032        Device;
    U032        Instance;           // Useful for NV3 (Uses Instance Addresses)
    PCOMMONOBJECT Next;
    U032        ColorFormat;
    U032        MonoFormat;
};

//******************************************************************************
// Macros to get to the common fields
//******************************************************************************

#define CBase            Common.Base
#define CValid           Common.Valid
#define CNotifyPending   Common.NotifyPending
#define CContext         Common.Context
#define CDevice          Common.Device
#define CInstance        Common.Instance
#define CNext            Common.Next
#define CColorFormat     Common.ColorFormat
#define CMonoFormat      Common.MonoFormat


//******************************************************************************
// BTREE structure.
//******************************************************************************

struct _def_node
{
    U032              Value;
    VOID             *Data;
    U032              LeftCount;
    U032              RightCount;
    struct _def_node *LeftBranch;
    struct _def_node *RightBranch;
} ;

//******************************************************************************
// Canvas
//******************************************************************************

struct _def_canvas
{
    NODE Node;
    U032 CanvasID;
    U032 UsageCount;
    U032 BufferToggle;
    U032 xyMin;
    U032 xyMax;
    U032 ClipCount;
    U032 ClipMisc;
    U032 xyClipMin[MAX_CLIP_REGIONS];
    U032 xyClipMax[MAX_CLIP_REGIONS];
    U032 InOutClip[MAX_CLIP_REGIONS];
    U032 CplxClipCount;
    U032 xyCplxClipMin[2];
    U032 xyCplxClipMax[2];
} ;


//******************************************************************************
// Video Object
//******************************************************************************

struct _def_video_object
{
    OBJECT          Base;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoInput[MAX_GRPATCH_INPUT];
} ;

//******************************************************************************
// Video Patchcord
//******************************************************************************

struct _def_video_patchcord
{
    OBJECT       Base;
    PVIDEOOBJECT Source;
    PVIDEOOBJECT Destination;
} ;

//******************************************************************************
// Video Switch Object
//******************************************************************************

struct _def_video_switch_object
{
    OBJECT          Base;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoInput[2];
//    VBLANKNOTIFY    VBlankNotify[2];
    PORDINALOBJECT  Ordinal;
    PCANVAS         Canvas;
} ;

//******************************************************************************
// Ordinal Object
//******************************************************************************

struct _def_ordinal_object
{
    OBJECT             Base;
    U032               Value;
    PVIDEOSWITCHOBJECT Switch[MAX_GRPATCH_FANOUT];
    U032               FanOut;
} ;

//******************************************************************************
// Video Sink Object
//******************************************************************************

struct _def_video_sink
{
    OBJECT          Base;
//    PDMAOBJECT      NameXlate;
    PVIDEOPATCHCORD VideoInput[MAX_GRPATCH_INPUT];
} ;

//******************************************************************************
// Video ColorMap Object
//******************************************************************************

struct _def_video_colormap_object
{
    OBJECT          Base;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoInput;
    U032            ColorFormat;
//    PDMAOBJECT      Xlate;
    U032            Start;
    U032            Length;
    U032            DirtyStart;
    U032            DirtyLength;
    U032            ColorMap[256];
} ;

//******************************************************************************
// Image Object
//******************************************************************************

struct _def_image_object
{
    OBJECT          Base;
    PIMAGEPATCHCORD ImageOutput;
    PIMAGEPATCHCORD ImageInput[MAX_GRPATCH_INPUT];
} ;

//******************************************************************************
// Image Patchcord
//******************************************************************************

struct _def_image_patchcord
{
    OBJECT       Base;
    U032         FanOut;
    PIMAGEOBJECT Source;
    PIMAGEOBJECT Destination[MAX_GRPATCH_FANOUT];
} ;

//******************************************************************************
// Image Video Object
//******************************************************************************

struct _def_image_video_object
{
    OBJECT          Base;
    PIMAGEPATCHCORD ImageOutput;
    PIMAGEPATCHCORD ImageInput[MAX_GRPATCH_INPUT];
    PVIDEOPATCHCORD VideoOutput;
//    PDMAOBJECT      NameXlate;
    PCANVAS         Canvas;
    U032            Buffer;
} ;

//******************************************************************************
// ROP Patchcord
//******************************************************************************

struct _def_rop_patchcord
{
    OBJECT      Base;
    U032        FanOut;
    PROPOBJECT  Source;
    PBOOLOBJECT Destination[MAX_GRPATCH_FANOUT];
} ;

//******************************************************************************
// Solid Object
//******************************************************************************

struct _def_solid_object
{
        COMMONOBJECT    Common;
    PIMAGEPATCHCORD ImageOutput;
    U032            Color;
} ;

//******************************************************************************
// Clip Object
//******************************************************************************

struct _def_black_rect_object
{
    COMMONOBJECT    Common;
    PIMAGEPATCHCORD ImageOutput;
    U032            xClipMin;
    U032            xClipMax;
    U032            yClipMin;
    U032            yClipMax;
} ;

//******************************************************************************
// Pattern Object
//******************************************************************************

struct _def_pattern_object
{
    COMMONOBJECT    Common;
    PIMAGEPATCHCORD ImageOutput;
    U032            PattColor0;
    U032            PattColor0Alpha;
    U032            PattColor1;
    U032            PattColor1Alpha;
    U032            Pattern0;
    U032            Pattern1;
    U032            PatternShape;
} ;

//******************************************************************************
// Rop Object
//******************************************************************************

struct _def_rop_object
{
    COMMONOBJECT  Common;
    PROPPATCHCORD RopOutput;
    U032          Rop3;
} ;


//******************************************************************************
// Beta Object
//******************************************************************************

struct _def_beta_object
{
    COMMONOBJECT   Common;
    PBETAPATCHCORD BetaOutput;
    U032           Beta;
} ;

//******************************************************************************
// Beta Max Object
//******************************************************************************

struct _def_beta_max_object
{
    OBJECT         Base;
    PBETAPATCHCORD BetaOutput;
    PBETAPATCHCORD BetaInput[2];
} ;

//******************************************************************************
// Bool object
//******************************************************************************

struct _def_bool_object
{
    OBJECT            Base;
    PIMAGEPATCHCORD   ImageOutput;
    PIMAGEPATCHCORD   ImageInput[3];
    PROPPATCHCORD     RopInput;
    PROPOBJECT        RopObject;
    PPATTERNOBJECT    PatObject;
    PIMAGEVIDEOOBJECT DstObject;
    U032              Config;
} ;

//******************************************************************************
// Blend Object
//******************************************************************************

struct _def_blend_object
{
    OBJECT            Base;
    PIMAGEPATCHCORD   ImageOutput;
    PIMAGEPATCHCORD   ImageInput[2];
    PBETAPATCHCORD    BetaInput;
    PPATTERNOBJECT    PatObject;
    PCOMMONOBJECT     SrcObject;
    PBETAOBJECT       BetaObject;
    PIMAGEVIDEOOBJECT DstObject;
    U032              Config;
} ;


//******************************************************************************
// Beta Patchcord Object
//******************************************************************************

struct _def_beta_patchcord
{
    OBJECT  Base;
    U032    FanOut;
    POBJECT Source;
    POBJECT Destination[MAX_GRPATCH_FANOUT];
} ;

//******************************************************************************
// Stencil Object
//******************************************************************************

struct _def_stencil_object
{
    OBJECT          Base;
    PIMAGEPATCHCORD ImageOutput;
    PIMAGEPATCHCORD ImageInput[2];
    PCLIPOBJECT     ClipObject;
} ;

//******************************************************************************
// Color Key Object
//******************************************************************************

struct _def_colorkey_object
{
    OBJECT            Base;
    PIMAGEPATCHCORD   ImageOutput;
    PIMAGEPATCHCORD   ImageInput[2];
    PSOLIDOBJECT      ColorObject;
    PIMAGEVIDEOOBJECT DstObject;
} ;

//******************************************************************************
// Plane Mask Object
//******************************************************************************

struct _def_plane_mask_object
{
    OBJECT            Base;
    PIMAGEPATCHCORD   ImageOutput;
    PIMAGEPATCHCORD   ImageInput[3];
    PSOLIDOBJECT      ColorObject;
    PIMAGEVIDEOOBJECT DstObject;
} ;

//******************************************************************************
// Graphics patch context representation,
//
// NOTE: In the RESOURCE MANAGER, this structure mainly consists of POINTERS.
//       But here, we store the actual structures, so we don't have to
//       allocate the memory dynamically.  That is, instead of PROPOBJECT,
//       we use ROPOBJECT instead.  This will take up memory in the
//       HwDeviceExtension structure (which holds all the 'global' values.
//
//******************************************************************************

struct _def_patch_context
{
    U032                 ImageConfig;
    U032                 VideoConfig;
//    PIMAGEVIDEOOBJECT    ImageVideo;
//    PCANVAS              Canvas;
//    PROPOBJECT           Rop;
//    PPATTERNOBJECT       RopPat;
//    PIMAGEVIDEOOBJECT    RopDst;
//    PBETAOBJECT          Beta;
//    PCOMMONOBJECT        BetaSrc;
//    PPATTERNOBJECT       BetaPat;
//    PIMAGEVIDEOOBJECT    BetaDst;
//    PSOLIDOBJECT         ColorKey;
//    PSOLIDOBJECT         PlaneMask;
//    PIMAGEVIDEOOBJECT    PlaneMaskDst;
//    PCLIPOBJECT          Clip;
//    PPATTERNOBJECT       Pattern;
//    PVIDEOCOLORMAPOBJECT ColorMap0;
//    PVIDEOCOLORMAPOBJECT ColorMap1;
//    PVIDEOSWITCHOBJECT   VideoSwitch;

      IMAGEVIDEOOBJECT    ImageVideo;
      CANVAS              Canvas;
      ROPOBJECT           Rop;
      PATTERNOBJECT       RopPat;
      IMAGEVIDEOOBJECT    RopDst;
      BETAOBJECT          Beta;
      COMMONOBJECT        BetaSrc;
      PATTERNOBJECT       BetaPat;
      IMAGEVIDEOOBJECT    BetaDst;
      SOLIDOBJECT         ColorKey;
      SOLIDOBJECT         PlaneMask;
      IMAGEVIDEOOBJECT    PlaneMaskDst;
      CLIPOBJECT          Clip;
      PATTERNOBJECT       Pattern;
      VIDEOCOLORMAPOBJECT ColorMap0;
      VIDEOCOLORMAPOBJECT ColorMap1;
      VIDEOSWITCHOBJECT   VideoSwitch;


} ;

//******************************************************************************
// Render Common Object
//******************************************************************************

struct _def_render_common
{
    PGRAPHICSPATCH  Patch;
    PATCHCONTEXT    PatchContext;
//    PDMAOBJECT      Xlate;
} ;

//******************************************************************************
// Macros to get to the common fields
//******************************************************************************

#define RCPatch            RenderCommon.Patch
#define RCPatchContext     RenderCommon.PatchContext
#define RCXlate            RenderCommon.Xlate


//******************************************************************************
// RenderCommon Object
//******************************************************************************

struct _def_render_common_object
{
    COMMONOBJECT    Common;
    RENDERCOMMON    RenderCommon;
} ;

//******************************************************************************
// Image Blit Object
//******************************************************************************

struct _def_image_blt_object
{
    COMMONOBJECT    Common;
    RENDERCOMMON    RenderCommon;
    PIMAGEPATCHCORD ImageOutput;
    PIMAGEPATCHCORD ImageInput;
} ;

//******************************************************************************
// ImageMem Object
//******************************************************************************

struct _def_image_mem_object
{
    COMMONOBJECT    Common;
    RENDERCOMMON    RenderCommon;
    PIMAGEPATCHCORD ImageInputOutput;
} ;

//******************************************************************************
// Render Object
//******************************************************************************

struct _def_render_object
{
    COMMONOBJECT    Common;
    RENDERCOMMON    RenderCommon;
    PIMAGEPATCHCORD ImageOutput;
} ;

//******************************************************************************
// Render Beta Object
//******************************************************************************

struct _def_render_beta_object
{
    COMMONOBJECT    Common;
    RENDERCOMMON    RenderCommon;
    PIMAGEPATCHCORD ImageOutput;
    PBETAPATCHCORD  BetaOutput;
} ;

//******************************************************************************
// FIFO structure
//******************************************************************************

struct _def_fifo
{
    BOOL    InUse;
    U032    ChID;
//    POBJECT ObjectStack[NUM_SUBCHANNELS];
    ULONG   ObjectStack[NUM_SUBCHANNELS];
    ULONG   SavedChDevInstance[NUM_SUBCHANNELS];
    ULONG   SavedContext[NUM_SUBCHANNELS];
    U032    ObjectCount;
} ;

//******************************************************************************
// Graphics Channel Patch Table
//******************************************************************************

struct _def_graphics_patch
{
    U032                 UsageCount;        // # of objects using this patch c
    U032                 RopUsage;          // # of objects using ROP.
    U032                 BetaUsage;         // # of objects using Beta.
    U032                 ColorKeyUsage;     // # of objects using Color Key.
    U032                 PlaneMaskUsage;    // # of objects using Plane Mask.
    U032                 ClipUsage;         // # of objects using Clip.
    U032                 PatternUsage;      // # of objects using Pattern.
    PGRAPHICSPATCH       Next;
    PRENDERCOMMONOBJECT  XferObject;        // DMA information for graphics.
    PCANVAS              Canvas;            // Associated canvas to this patch
    U032                 xyMinPrev;         // Previous xyMin of canvas.
    U032                 xyMaxPrev;         // Previous xyMax of canvas.
    PVIDEOCOLORMAPOBJECT ColorMap0;         // Associated colormap to buffer 0
    PVIDEOCOLORMAPOBJECT ColorMap1;         // Associated colormap to buffer 1
    PVIDEOSWITCHOBJECT   VideoSwitch;       // Associated video switch.
    PCOMMONOBJECT        UserObjects[NUM_GRAPHICS_DEVICES];
    U032                 AbsX[18];          // Internal state that isn't
    U032                 AbsY[18];          // object specific.  It is
    U032                 XYLogicMisc0;      // restored for whatever
    U032                 XYLogicMisc1;      // object that was last
    U032                 XMisc;             // rendering.
    U032                 YMisc;             // ...
    U032                 CanvasMisc;        // ...
    U032                 MonoColor0;        // ...
    U032                 MonoColor1;        // ...
    U032                 SourceColor;       // ...
    U032                 SubDivide;         // ...
    U032                 EdgeFill;          // ...
    U032                 BetaRam[14];       // ...
    U032                 XAbsIClipMax;      // ...
    U032                 YAbsIClipMax;      // ...
    U032                 Bit33;             // ...
} ;


//******************************************************************************
// Graphics Channel Patch Table
//******************************************************************************

struct _def_graphics_channel
{
    PCOMMONOBJECT  NotifyObject;
    U032           Notify;
    PROPOBJECT     CurrentRop;
    PBETAOBJECT    CurrentBeta;
    PSOLIDOBJECT   CurrentColorKey;
    PSOLIDOBJECT   CurrentPlaneMask;
    PCLIPOBJECT    CurrentClip;
    PPATTERNOBJECT CurrentPattern;
    PGRAPHICSPATCH CurrentPatch;
    PGRAPHICSPATCH PatchList;
    U032           PatchCount;
    U032           Exceptions;
} ;

//******************************************************************************
// Register base macros
//******************************************************************************

#define PMC_Base            HwDeviceExtension->NV1_Lin_PMC_Registers
#define PFB_Base            HwDeviceExtension->NV1_Lin_PFB_Registers
#define PRM_Base            HwDeviceExtension->NV1_Lin_PRM_Registers
#define PDAC_Base           HwDeviceExtension->NV1_Lin_PDAC_Registers
#define PRAM_Base           HwDeviceExtension->NV1_Lin_PRAM_Registers
#define PRAMFC_Base         HwDeviceExtension->NV1_Lin_PRAMFC_Registers
#define PRAMHT_Base         HwDeviceExtension->NV1_Lin_PRAMHT_Registers
#define USER_Base           HwDeviceExtension->NV1_Lin_USER_Registers
#define PFIFO_Base          HwDeviceExtension->NV1_Lin_PFIFO_Registers
#define PBUS_Base           HwDeviceExtension->NV1_Lin_PBUS_Registers
#define PGRAPH_Base         HwDeviceExtension->NV1_Lin_PGRAPH_Registers
#define CONFIG_Base         HwDeviceExtension->NV1_Lin_CONFIG_Registers
#define MEM_Base            HwDeviceExtension->NV1_Lin_MEM_Registers
#define IO_Base             HwDeviceExtension->NV1_Lin_IO_Registers
#define PDMA_Base           HwDeviceExtension->NV1_Lin_PDMA_Registers
#define PTIMER_Base         HwDeviceExtension->NV1_Lin_PTIMER_Registers
#define PAUDIO_Base         HwDeviceExtension->NV1_Lin_PAUDIO_Registers
#define PAUTH_Base          HwDeviceExtension->NV1_Lin_PAUTH_Registers
#define PEXTDEV_Base        HwDeviceExtension->NV1_Lin_PEXTDEV_Registers
#define PEEPROM_Base        HwDeviceExtension->NV1_Lin_PEEPROM_Registers
#define PROM_Base           HwDeviceExtension->NV1_Lin_PROM_Registers
#define PALT_Base           HwDeviceExtension->NV1_Lin_PALT_Registers
#define PRMIO_Base          HwDeviceExtension->NV1_Lin_PRMIO_Registers
#define URECT_Base          HwDeviceExtension->NV1_Lin_URECT_Registers
#define UTRI_Base           HwDeviceExtension->NV1_Lin_UTRI_Registers
#define UBLIT_Base          HwDeviceExtension->NV1_Lin_UBLIT_Registers
#define UIMAGE_Base         HwDeviceExtension->NV1_Lin_UIMAGE_Registers
#define UBITMAP_Base        HwDeviceExtension->NV1_Lin_UBITMAP_Registers
#define PRAMIN_Base         HwDeviceExtension->NV3_Lin_PRAMIN_Registers
#define PRMVIO_Base         HwDeviceExtension->NV3_Lin_PRMVIO_Registers
#define PRMCIO_Base         HwDeviceExtension->NV3_Lin_PRMCIO_Registers
#define PRAMDAC_Base        HwDeviceExtension->NV3_Lin_PRAMDAC_Registers
#define USERDAC_Base        HwDeviceExtension->NV3_Lin_USERDAC_Registers
#define PVIDEO_Base         HwDeviceExtension->NV3_Lin_PVIDEO_Registers

//******************************************************************************
// Device macros
//******************************************************************************


#define GR_DEVICE_INDEX(d)      ((((d)>>16)&0x7F)-0x41)


//******************************************************************************
//
// Save/Restore state macros for graphics engine
//
//******************************************************************************

#define GR_SAVE_STATE(misc)                                                             \
{                                                                                   \
        misc = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_MISC);    \
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC, DRF_DEF(_PGRAPH, _MISC, _FIFO_WRITE, _ENABLED)     \
                        | DRF_DEF(_PGRAPH, _MISC, _FIFO,       _DISABLED));  \
}

#define GR_RESTORE_STATE(misc)                                                          \
{                                                                                   \
        PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_MISC, DRF_DEF(_PGRAPH, _MISC, _FIFO_WRITE, _ENABLED)     \
                              | (DRF_DEF(_PGRAPH, _MISC, _FIFO, _ENABLED) & misc)); \
}

//*************************************************************************
// Graphics Engine DONE and IDLE macros
//*************************************************************************

#define GR_IDLE     while ((volatile)PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_STATUS))  \
                        {                                                            \
                        V032 pmc;                                                    \
                        pmc = PMC_REG_RD32(PMC_Base,NV_PMC_INTR_0);                  \
                        if (pmc & DRF_DEF(_PMC,_INTR_0,_PGRAPH,_PENDING))            \
                            VideoDebugPrint((0, "Interrupt pending in GR_IDLE"));    \
                        }

#define GR_DONE     while ((volatile)PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_STATUS));

#define GR_IDLE_NV3 while ((volatile)PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_STATUS))  \
                        {                                                            \
                        V032 pmc;                                                    \
                        pmc = PMC_REG_RD32(PMC_Base,NV_PMC_INTR_0);                  \
                        if (pmc & DRF_DEF(_PMC,_INTR_0,_PGRAPH0,_PENDING))            \
                            VideoDebugPrint((0, "Interrupt pending in GR_IDLE_NV3"));  \
                        }



//******************************************************************************
//
// Hash table function.
//
//******************************************************************************

#define FIFO_HASH(h,c)  ((((h)^((h)>>8)^((h)>>16)^((h)>>24))&0xFF)^((c)&0x7F))
#define HASH_DEPTH      4
#define HASH_ENTRY(h,d) ((h)*HASH_DEPTH+(d))


//******************************************************************************
//
// NV Reference Manual register access definitions.
//
//******************************************************************************


typedef union _def_HwReg
{
    volatile V008 Reg008[1];
    volatile V016 Reg016[1];
    volatile V032 Reg032[1];
} HWREG, * PHWREG;

// Start using flat register range (instead of using multiple ranges)

#define REG_WR32(a,d)   (HwDeviceExtension->NvRegisterBase)->Reg032[(a)/4]=(U032)(d)
#define REG_RD32(a)     (HwDeviceExtension->NvRegisterBase)->Reg032[(a)/4]
#define REG_WR08(a,d)   (HwDeviceExtension->NvRegisterBase)->Reg008[(a)]  =(U008)(d)
#define REG_RD08(a)     (HwDeviceExtension->NvRegisterBase)->Reg008[(a)]


//#ifdef PC98
#define MEM_WR32(a,d)   fbAddr->Reg032[(a)/4]=(U032)(d)
#define MEM_RD32(a)     fbAddr->Reg032[(a)/4]
//#endif // PC98


#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d) - DEVICE_BASE(d) + 1
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#define REG_WR_DRF_NUM(d,r,f,n) REG_WR32(NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(d,r,f,c) REG_WR32(NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(d,r,f,n) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(d,r,f,c) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define REG_RD_DRF(d,r,f)       (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRAMIN registers (NV3)
//******************************************************************************

#define PRAMIN_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMIN))/4]=(U032)(d)
#define PRAMIN_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMIN))/4]
#define PRAMIN_REG_WR_DRF_NUM(b,d,r,f,n) PRAMIN_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRAMIN_REG_WR_DRF_DEF(b,d,r,f,c) PRAMIN_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRAMIN_FLD_WR_DRF_NUM(b,d,r,f,n) PRAMIN_REG_WR32(b,NV##d##r,(PRAMIN_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRAMIN_FLD_WR_DRF_DEF(b,d,r,f,c) PRAMIN_REG_WR32(b,NV##d##r,(PRAMIN_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRAMIN_REG_RD_DRF(b,d,r,f)       (((PRAMIN_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
//
// NV Reference Manual INSTANCE MEMORY structure access definitions.
//
//******************************************************************************

#define INST_WR32(b,i,o,d)      PRAMIN_REG_WR32(b,DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o),(d))
#define INST_RD32(b,i,o)        PRAMIN_REG_RD32(b,DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o))
#define SF_OFFSET(sf)           (((0?sf)/32)<<2)
#define SF_SHIFT(sf)            ((0?sf)&31)
#define SF_MASK(sf)             (0xFFFFFFFF>>(31-(1?sf)+(0?sf)))
#define SF_DEF(s,f,c)           ((NV ## s ## f ## c)<<SF_SHIFT(NV ## s ## f))
#define SF_NUM(s,f,n)           (((n)&SF_MASK(NV ## s ## f))<<SF_SHIFT(NV ## s ## f))
#define SF_VAL(s,f,v)           (((v)>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))
#define RAM_WR_ISF_NUM(b,i,s,f,n) INST_WR32(b,i,SF_OFFSET(NV ## s ## f),SF_NUM(s,f,n))
#define RAM_WR_ISF_DEF(b,i,s,f,c) INST_WR32(b,i,SF_OFFSET(NV ## s ## f),SF_DEF(s,f,c))
#define FLD_WR_ISF_NUM(b,i,s,f,n) INST_WR32(b,i,SF_OFFSET(NV##s##f),(INST_RD32(b,i,SF_OFFSET(NV##s##f))&~(SF_MASK(NV##s##f)<<SF_SHIFT(NV##s##f)))|SF_NUM(s,f,n))
#define FLD_WR_ISF_DEF(b,i,s,f,c) INST_WR32(b,i,SF_OFFSET(NV##s##f),(INST_RD32(b,i,SF_OFFSET(NV##s##f))&~(SF_MASK(NV##s##f)<<SF_SHIFT(NV##s##f)))|SF_DEF(s,f,c))
#define RAM_RD_ISF(b,i,s,f)       (((INST_RD32(b,i,SF_OFFSET(NV ## s ## f)))>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))

//******************************************************************************
// Unlike Win95, we don't allocate one pointer to reference the entire
// address space mapped by the NV memory map (32Mb). NT doesn't like us
// taking all this memory.  So what we'll do instead is allocate separate
// chunks of memory for the various classes of registers.  The downside
// is that we can't use the REG_RD and REG_WR macros as is.  We'll need
// to use specific macros for each type of register class.
// The goal is that we want to use all those DEFINES that have already
// been done for us (Instead of having to Re-define the REGISTER BASES)
//
// Format is as follows:
//    b = Base address mapped for this type of register class
//    o = Register Offset address from the base of the NV device (address=0)
//        The correct address will be fixed up as follows:
//        OFFSET_FROM_REGISTER_BASE = OFFSET_FROM_0 - REGISTER_BASE_OFFSET
//
//        For example, OFFSET of NV_PFIFO_INTR_0 = 2100.
//        NEW OFFSET (from register base ) = 2100 - NV_PFIFO = 100
//
// ???? What about performance ?????
// ???? Redefine macros to improve performance ??????
// ???? Does compiler optimize well ?????
//******************************************************************************

//******************************************************************************
// Temporary register range
//******************************************************************************

#define TEMP_REG_WR32(b,d)   (b)->Reg032[0]=(U032)(d)
#define TEMP_REG_RD32(b)     (b)->Reg032[0]

//******************************************************************************
// Macros for NV_CONFIG registers
//******************************************************************************

#define CONFIG_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_CONFIG))/4]=(U032)(d)
#define CONFIG_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_CONFIG))/4]
#define CONFIG_REG_WR_DRF_NUM(b,d,r,f,n) CONFIG_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define CONFIG_REG_WR_DRF_DEF(b,d,r,f,c) CONFIG_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define CONFIG_FLD_WR_DRF_NUM(b,d,r,f,n) CONFIG_REG_WR32(b,NV##d##r,(CONFIG_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define CONFIG_FLD_WR_DRF_DEF(b,d,r,f,c) CONFIG_REG_WR32(b,NV##d##r,(CONFIG_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define CONFIG_REG_RD_DRF(b,d,r,f)       (((CONFIG_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_MEMORY registers
//******************************************************************************

#define MEMORY_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_MEMORY))/4]=(U032)(d)
#define MEMORY_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_MEMORY))/4]
#define MEMORY_REG_WR_DRF_NUM(b,d,r,f,n) MEMORY_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define MEMORY_REG_WR_DRF_DEF(b,d,r,f,c) MEMORY_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define MEMORY_FLD_WR_DRF_NUM(b,d,r,f,n) MEMORY_REG_WR32(b,NV##d##r,(MEMORY_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define MEMORY_FLD_WR_DRF_DEF(b,d,r,f,c) MEMORY_REG_WR32(b,NV##d##r,(MEMORY_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define MEMORY_REG_RD_DRF(b,d,r,f)       (((MEMORY_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_IO registers
//******************************************************************************

#define IO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_IO))/4]=(U032)(d)
#define IO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_IO))/4]
#define IO_REG_WR_DRF_NUM(b,d,r,f,n) IO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define IO_REG_WR_DRF_DEF(b,d,r,f,c) IO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define IO_FLD_WR_DRF_NUM(b,d,r,f,n) IO_REG_WR32(b,NV##d##r,(IO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define IO_FLD_WR_DRF_DEF(b,d,r,f,c) IO_REG_WR32(b,NV##d##r,(IO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define IO_REG_RD_DRF(b,d,r,f)       (((IO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PMC registers
//******************************************************************************

#define PMC_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PMC))/4]=(U032)(d)
#define PMC_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PMC))/4]
#define PMC_REG_WR_DRF_NUM(b,d,r,f,n) PMC_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PMC_REG_WR_DRF_DEF(b,d,r,f,c) PMC_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PMC_FLD_WR_DRF_NUM(b,d,r,f,n) PMC_REG_WR32(b,NV##d##r,(PMC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PMC_FLD_WR_DRF_DEF(b,d,r,f,c) PMC_REG_WR32(b,NV##d##r,(PMC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PMC_REG_RD_DRF(b,d,r,f)       (((PMC_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PBUS registers
//******************************************************************************

#define PBUS_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PBUS))/4]=(U032)(d)
#define PBUS_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PBUS))/4]
#define PBUS_REG_WR_DRF_NUM(b,d,r,f,n) PBUS_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PBUS_REG_WR_DRF_DEF(b,d,r,f,c) PBUS_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PBUS_FLD_WR_DRF_NUM(b,d,r,f,n) PBUS_REG_WR32(b,NV##d##r,(PBUS_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PBUS_FLD_WR_DRF_DEF(b,d,r,f,c) PBUS_REG_WR32(b,NV##d##r,(PBUS_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PBUS_REG_RD_DRF(b,d,r,f)       (((PBUS_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PFIFO registers
//******************************************************************************

#define PFIFO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PFIFO))/4]=(U032)(d)
#define PFIFO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PFIFO))/4]
#define PFIFO_REG_WR_DRF_NUM(b,d,r,f,n) PFIFO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PFIFO_REG_WR_DRF_DEF(b,d,r,f,c) PFIFO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PFIFO_FLD_WR_DRF_NUM(b,d,r,f,n) PFIFO_REG_WR32(b,NV##d##r,(PFIFO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PFIFO_FLD_WR_DRF_DEF(b,d,r,f,c) PFIFO_REG_WR32(b,NV##d##r,(PFIFO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PFIFO_REG_RD_DRF(b,d,r,f)       (((PFIFO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PDMA registers
//******************************************************************************

#define PDMA_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PDMA))/4]=(U032)(d)
#define PDMA_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PDMA))/4]
#define PDMA_REG_WR_DRF_NUM(b,d,r,f,n) PDMA_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PDMA_REG_WR_DRF_DEF(b,d,r,f,c) PDMA_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PDMA_FLD_WR_DRF_NUM(b,d,r,f,n) PDMA_REG_WR32(b,NV##d##r,(PDMA_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PDMA_FLD_WR_DRF_DEF(b,d,r,f,c) PDMA_REG_WR32(b,NV##d##r,(PDMA_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PDMA_REG_RD_DRF(b,d,r,f)       (((PDMA_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PTIMER registers
//******************************************************************************

#define PTIMER_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PTIMER))/4]=(U032)(d)
#define PTIMER_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PTIMER))/4]
#define PTIMER_REG_WR_DRF_NUM(b,d,r,f,n) PTIMER_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PTIMER_REG_WR_DRF_DEF(b,d,r,f,c) PTIMER_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PTIMER_FLD_WR_DRF_NUM(b,d,r,f,n) PTIMER_REG_WR32(b,NV##d##r,(PTIMER_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PTIMER_FLD_WR_DRF_DEF(b,d,r,f,c) PTIMER_REG_WR32(b,NV##d##r,(PTIMER_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PTIMER_REG_RD_DRF(b,d,r,f)       (((PTIMER_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PAUDIO registers
//******************************************************************************

#define PAUDIO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PAUDIO))/4]=(U032)(d)
#define PAUDIO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PAUDIO))/4]
#define PAUDIO_REG_WR_DRF_NUM(b,d,r,f,n) PAUDIO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PAUDIO_REG_WR_DRF_DEF(b,d,r,f,c) PAUDIO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PAUDIO_FLD_WR_DRF_NUM(b,d,r,f,n) PAUDIO_REG_WR32(b,NV##d##r,(PAUDIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PAUDIO_FLD_WR_DRF_DEF(b,d,r,f,c) PAUDIO_REG_WR32(b,NV##d##r,(PAUDIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PAUDIO_REG_RD_DRF(b,d,r,f)       (((PAUDIO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PGRAPH registers
//******************************************************************************

#define PGRAPH_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PGRAPH))/4]=(U032)(d)
#define PGRAPH_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PGRAPH))/4]
#define PGRAPH_REG_WR_DRF_NUM(b,d,r,f,n) PGRAPH_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PGRAPH_REG_WR_DRF_DEF(b,d,r,f,c) PGRAPH_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PGRAPH_FLD_WR_DRF_NUM(b,d,r,f,n) PGRAPH_REG_WR32(b,NV##d##r,(PGRAPH_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PGRAPH_FLD_WR_DRF_DEF(b,d,r,f,c) PGRAPH_REG_WR32(b,NV##d##r,(PGRAPH_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PGRAPH_REG_RD_DRF(b,d,r,f)       (((PGRAPH_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRMVIO registers (NV3)
//******************************************************************************

#define PRMVIO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRMVIO))/4]=(U032)(d)
#define PRMVIO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRMVIO))/4]
#define PRMVIO_REG_WR_DRF_NUM(b,d,r,f,n) PRMVIO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRMVIO_REG_WR_DRF_DEF(b,d,r,f,c) PRMVIO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRMVIO_FLD_WR_DRF_NUM(b,d,r,f,n) PRMVIO_REG_WR32(b,NV##d##r,(PRMVIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRMVIO_FLD_WR_DRF_DEF(b,d,r,f,c) PRMVIO_REG_WR32(b,NV##d##r,(PRMVIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRMVIO_REG_RD_DRF(b,d,r,f)       (((PRMVIO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

#define PRMVIO_REG_WR08(b,a,d)   (b)->Reg008[(a)-DEVICE_BASE(NV_PRMVIO)]=(U008)(d)
#define PRMVIO_REG_RD08(b,a)     (b)->Reg008[(a)-DEVICE_BASE(NV_PRMVIO)]

#define NV_SR_UNLOCK_VALUE                               0x00000057
#define NV_SR_LOCK_VALUE                                 0x00000099

//******************************************************************************
// Macros for NV_PRMCIO registers (NV3)
//******************************************************************************

#define PRMCIO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRMCIO))/4]=(U032)(d)
#define PRMCIO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRMCIO))/4]
#define PRMCIO_REG_WR_DRF_NUM(b,d,r,f,n) PRMCIO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRMCIO_REG_WR_DRF_DEF(b,d,r,f,c) PRMCIO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRMCIO_FLD_WR_DRF_NUM(b,d,r,f,n) PRMCIO_REG_WR32(b,NV##d##r,(PRMCIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRMCIO_FLD_WR_DRF_DEF(b,d,r,f,c) PRMCIO_REG_WR32(b,NV##d##r,(PRMCIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRMCIO_REG_RD_DRF(b,d,r,f)       (((PRMCIO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

#define PRMCIO_REG_WR08(b,o,d)   (b)->Reg008[(o)-DEVICE_BASE(NV_PRMCIO)]=(U008)(d)
#define PRMCIO_REG_RD08(b,o)     (b)->Reg008[(o)-DEVICE_BASE(NV_PRMCIO)]

//******************************************************************************
// Macros for NV_PRAMDAC registers (NV3)
//******************************************************************************

#define PRAMDAC_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMDAC))/4]=(U032)(d)
#define PRAMDAC_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMDAC))/4]
#define PRAMDAC_REG_WR_DRF_NUM(b,d,r,f,n) PRAMDAC_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRAMDAC_REG_WR_DRF_DEF(b,d,r,f,c) PRAMDAC_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRAMDAC_FLD_WR_DRF_NUM(b,d,r,f,n) PRAMDAC_REG_WR32(b,NV##d##r,(PRAMDAC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRAMDAC_FLD_WR_DRF_DEF(b,d,r,f,c) PRAMDAC_REG_WR32(b,NV##d##r,(PRAMDAC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRAMDAC_REG_RD_DRF(b,d,r,f)       (((PRAMDAC_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_USERDAC registers (NV3)
//******************************************************************************

#define USERDAC_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_USER_DAC))/4]=(U032)(d)
#define USERDAC_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_USER_DAC))/4]
#define USERDAC_REG_WR_DRF_NUM(b,d,r,f,n) USERDAC_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define USERDAC_REG_WR_DRF_DEF(b,d,r,f,c) USERDAC_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define USERDAC_FLD_WR_DRF_NUM(b,d,r,f,n) USERDAC_REG_WR32(b,NV##d##r,(USERDAC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define USERDAC_FLD_WR_DRF_DEF(b,d,r,f,c) USERDAC_REG_WR32(b,NV##d##r,(USERDAC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define USERDAC_REG_RD_DRF(b,d,r,f)       (((USERDAC_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//#ifdef PC98
#define USERDAC_REG_WR08(b,o,d)   (b)->Reg008[(o)-DEVICE_BASE(NV_USER_DAC)]=(U008)(d)
#define USERDAC_REG_RD08(b,o)     (b)->Reg008[(o)-DEVICE_BASE(NV_USER_DAC)]
//#endif // PC98

//******************************************************************************
// Macros for NV_PVIDEO registers (NV3)
//******************************************************************************

#define PVIDEO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PVIDEO))/4]=(U032)(d)
#define PVIDEO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PVIDEO))/4]
#define PVIDEO_REG_WR_DRF_NUM(b,d,r,f,n) PVIDEO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PVIDEO_REG_WR_DRF_DEF(b,d,r,f,c) PVIDEO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PVIDEO_FLD_WR_DRF_NUM(b,d,r,f,n) PVIDEO_REG_WR32(b,NV##d##r,(PVIDEO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PVIDEO_FLD_WR_DRF_DEF(b,d,r,f,c) PVIDEO_REG_WR32(b,NV##d##r,(PVIDEO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PVIDEO_REG_RD_DRF(b,d,r,f)       (((PVIDEO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))




//******************************************************************************
// CRTC Access Macros
//
// For now use the priviliged space, but we may have to switch to standard VGA i/o
//******************************************************************************

#define CRTC_WR(i,d)    {PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_CRX__COLOR, (i) & 0x3F);\
                        PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_CR__COLOR, (d));}
#define CRTC_RD(i,d)    {PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_CRX__COLOR, (i) & 0x3F);\
                        (d) = PRMCIO_REG_RD08(PRMCIO_Base,NV_PRMCIO_CR__COLOR);}

//******************************************************************************
// Macros for NV_UBETA registers
//******************************************************************************

#define UBETA_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UBETA))/4]=(U032)(d)
#define UBETA_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UBETA))/4]

//******************************************************************************
// Macros for NV_UROP registers
//******************************************************************************

#define UROP_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UROP))/4]=(U032)(d)
#define UROP_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UROP))/4]

//******************************************************************************
// Macros for NV_UCHROMA registers
//******************************************************************************

#define UCHROMA_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UCHROMA))/4]=(U032)(d)
#define UCHROMA_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UCHROMA))/4]

//******************************************************************************
// Macros for NV_UPLANE registers
//******************************************************************************

#define UPLANE_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UPLANE))/4]=(U032)(d)
#define UPLANE_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UPLANE))/4]

//******************************************************************************
// Macros for NV_UCLIP registers
//******************************************************************************

#define UCLIP_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UCLIP))/4]=(U032)(d)
#define UCLIP_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UCLIP))/4]

//******************************************************************************
// Macros for NV_UPATT registers
//******************************************************************************

#define UPATT_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UPATT))/4]=(U032)(d)
#define UPATT_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UPATT))/4]

//******************************************************************************
// Macros for NV_UPOINT registers
//******************************************************************************

#define UPOINT_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UPOINT))/4]=(U032)(d)
#define UPOINT_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UPOINT))/4]

//******************************************************************************
// Macros for NV_ULINE registers
//******************************************************************************

#define ULINE_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_ULINE))/4]=(U032)(d)
#define ULINE_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_ULINE))/4]

//******************************************************************************
// Macros for NV_ULIN registers
//******************************************************************************

#define ULIN_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_ULIN))/4]=(U032)(d)
#define ULIN_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_ULIN))/4]

//******************************************************************************
// Macros for NV_UTRI registers
//******************************************************************************

#define UTRI_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UTRI))/4]=(U032)(d)
#define UTRI_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UTRI))/4]

//******************************************************************************
// Macros for NV_URECT registers
//******************************************************************************

#define URECT_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_URECT))/4]=(U032)(d)
#define URECT_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_URECT))/4]

//******************************************************************************
// Macros for NV_UBTM registers
//******************************************************************************

#define UBTM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UBTM))/4]=(U032)(d)
#define UBTM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UBTM))/4]

//******************************************************************************
// Macros for NV_UQTM registers
//******************************************************************************

#define UQTM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UQTM))/4]=(U032)(d)
#define UQTM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UQTM))/4]

//******************************************************************************
// Macros for NV_UBLIT registers
//******************************************************************************

#define UBLIT_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UBLIT))/4]=(U032)(d)
#define UBLIT_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UBLIT))/4]

//******************************************************************************
// Macros for NV_UIMAGE registers
//******************************************************************************

#define UIMAGE_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UIMAGE))/4]=(U032)(d)
#define UIMAGE_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UIMAGE))/4]

//******************************************************************************
// Macros for NV_UBITMAP registers
//******************************************************************************

#define UBITMAP_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UBITMAP))/4]=(U032)(d)
#define UBITMAP_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UBITMAP))/4]

//******************************************************************************
// Macros for NV_UFROMEM registers
//******************************************************************************

#define UFROMEM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UFROMEM))/4]=(U032)(d)
#define UFROMEM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UFROMEM))/4]

//******************************************************************************
// Macros for NV_UTOMEM registers
//******************************************************************************

#define UTOMEM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UTOMEM))/4]=(U032)(d)
#define UTOMEM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UTOMEM))/4]

//******************************************************************************
// Macros for NV_UBTMB registers
//******************************************************************************

#define UBTMB_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UBTMB))/4]=(U032)(d)
#define UBTMB_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UBTMB))/4]

//******************************************************************************
// Macros for NV_UQTMB registers
//******************************************************************************

#define UQTMB_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_UQTMB))/4]=(U032)(d)
#define UQTMB_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_UQTMB))/4]

//******************************************************************************
// Macros for NV_PFB registers
//******************************************************************************

#define PFB_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PFB))/4]=(U032)(d)
#define PFB_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PFB))/4]
#define PFB_REG_WR_DRF_NUM(b,d,r,f,n) PFB_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PFB_REG_WR_DRF_DEF(b,d,r,f,c) PFB_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PFB_FLD_WR_DRF_NUM(b,d,r,f,n) PFB_REG_WR32(b,NV##d##r,(PFB_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PFB_FLD_WR_DRF_DEF(b,d,r,f,c) PFB_REG_WR32(b,NV##d##r,(PFB_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PFB_REG_RD_DRF(b,d,r,f)       (((PFB_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRAM registers
//******************************************************************************

#define PRAM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRAM))/4]=(U032)(d)
#define PRAM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRAM))/4]
#define PRAM_REG_WR_DRF_NUM(b,d,r,f,n) PRAM_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRAM_REG_WR_DRF_DEF(b,d,r,f,c) PRAM_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRAM_FLD_WR_DRF_NUM(b,d,r,f,n) PRAM_REG_WR32(b,NV##d##r,(PRAM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRAM_FLD_WR_DRF_DEF(b,d,r,f,c) PRAM_REG_WR32(b,NV##d##r,(PRAM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRAM_REG_RD_DRF(b,d,r,f)       (((PRAM_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRAMFC registers
//******************************************************************************

#define PRAMFC_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMFC))/4]=(U032)(d)
#define PRAMFC_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMFC))/4]
#define PRAMFC_REG_WR_DRF_NUM(b,d,r,f,n) PRAMFC_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRAMFC_REG_WR_DRF_DEF(b,d,r,f,c) PRAMFC_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRAMFC_FLD_WR_DRF_NUM(b,d,r,f,n) PRAMFC_REG_WR32(b,NV##d##r,(PRAMFC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRAMFC_FLD_WR_DRF_DEF(b,d,r,f,c) PRAMFC_REG_WR32(b,NV##d##r,(PRAMFC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRAMFC_REG_RD_DRF(b,d,r,f)       (((PRAMFC_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRAMHT registers
//******************************************************************************

#define PRAMHT_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMHT))/4]=(U032)(d)
#define PRAMHT_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRAMHT))/4]
#define PRAMHT_REG_WR_DRF_NUM(b,d,r,f,n) PRAMHT_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRAMHT_REG_WR_DRF_DEF(b,d,r,f,c) PRAMHT_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRAMHT_FLD_WR_DRF_NUM(b,d,r,f,n) PRAMHT_REG_WR32(b,NV##d##r,(PRAMHT_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRAMHT_FLD_WR_DRF_DEF(b,d,r,f,c) PRAMHT_REG_WR32(b,NV##d##r,(PRAMHT_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRAMHT_REG_RD_DRF(b,d,r,f)       (((PRAMHT_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PAUTH registers
//******************************************************************************

#define PAUTH_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PAUTH))/4]=(U032)(d)
#define PAUTH_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PAUTH))/4]
#define PAUTH_REG_WR_DRF_NUM(b,d,r,f,n) PAUTH_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PAUTH_REG_WR_DRF_DEF(b,d,r,f,c) PAUTH_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PAUTH_FLD_WR_DRF_NUM(b,d,r,f,n) PAUTH_REG_WR32(b,NV##d##r,(PAUTH_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PAUTH_FLD_WR_DRF_DEF(b,d,r,f,c) PAUTH_REG_WR32(b,NV##d##r,(PAUTH_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PAUTH_REG_RD_DRF(b,d,r,f)       (((PAUTH_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_EXTDEV registers
//******************************************************************************

#define PEXTDEV_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PEXTDEV))/4]=(U032)(d)
#define PEXTDEV_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PEXTDEV))/4]
#define PEXTDEV_REG_WR_DRF_NUM(b,d,r,f,n) PEXTDEV_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PEXTDEV_REG_WR_DRF_DEF(b,d,r,f,c) PEXTDEV_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PEXTDEV_FLD_WR_DRF_NUM(b,d,r,f,n) PEXTDEV_REG_WR32(b,NV##d##r,(PEXTDEV_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PEXTDEV_FLD_WR_DRF_DEF(b,d,r,f,c) PEXTDEV_REG_WR32(b,NV##d##r,(PEXTDEV_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PEXTDEV_REG_RD_DRF(b,d,r,f)       (((PEXTDEV_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PDAC registers
//******************************************************************************

#define PDAC_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PDAC))/4]=(U032)(d)
#define PDAC_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PDAC))/4]
#define PDAC_REG_WR_DRF_NUM(b,d,r,f,n) PDAC_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PDAC_REG_WR_DRF_DEF(b,d,r,f,c) PDAC_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PDAC_FLD_WR_DRF_NUM(b,d,r,f,n) PDAC_REG_WR32(b,NV##d##r,(PDAC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PDAC_FLD_WR_DRF_DEF(b,d,r,f,c) PDAC_REG_WR32(b,NV##d##r,(PDAC_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PDAC_REG_RD_DRF(b,d,r,f)       (((PDAC_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PEEPROM registers
//******************************************************************************

#define PEEPROM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PEEPROM))/4]=(U032)(d)
#define PEEPROM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PEEPROM))/4]
#define PEEPROM_REG_WR_DRF_NUM(b,d,r,f,n) PEEPROM_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PEEPROM_REG_WR_DRF_DEF(b,d,r,f,c) PEEPROM_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PEEPROM_FLD_WR_DRF_NUM(b,d,r,f,n) PEEPROM_REG_WR32(b,NV##d##r,(PEEPROM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PEEPROM_FLD_WR_DRF_DEF(b,d,r,f,c) PEEPROM_REG_WR32(b,NV##d##r,(PEEPROM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PEEPROM_REG_RD_DRF(b,d,r,f)       (((PEEPROM_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PROM registers
//******************************************************************************

#define PROM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PROM))/4]=(U032)(d)
#define PROM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PROM))/4]
#define PROM_REG_WR_DRF_NUM(b,d,r,f,n) PROM_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PROM_REG_WR_DRF_DEF(b,d,r,f,c) PROM_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PROM_FLD_WR_DRF_NUM(b,d,r,f,n) PROM_REG_WR32(b,NV##d##r,(PROM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PROM_FLD_WR_DRF_DEF(b,d,r,f,c) PROM_REG_WR32(b,NV##d##r,(PROM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PROM_REG_RD_DRF(b,d,r,f)       (((PROM_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PALT registers
//******************************************************************************

#define PALT_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PALT))/4]=(U032)(d)
#define PALT_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PALT))/4]
#define PALT_REG_WR_DRF_NUM(b,d,r,f,n) PALT_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PALT_REG_WR_DRF_DEF(b,d,r,f,c) PALT_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PALT_FLD_WR_DRF_NUM(b,d,r,f,n) PALT_REG_WR32(b,NV##d##r,(PALT_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PALT_FLD_WR_DRF_DEF(b,d,r,f,c) PALT_REG_WR32(b,NV##d##r,(PALT_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PALT_REG_RD_DRF(b,d,r,f)       (((PALT_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRM registers
//******************************************************************************

#define PRM_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRM))/4]=(U032)(d)
#define PRM_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRM))/4]
#define PRM_REG_WR_DRF_NUM(b,d,r,f,n) PRM_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRM_REG_WR_DRF_DEF(b,d,r,f,c) PRM_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRM_FLD_WR_DRF_NUM(b,d,r,f,n) PRM_REG_WR32(b,NV##d##r,(PRM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRM_FLD_WR_DRF_DEF(b,d,r,f,c) PRM_REG_WR32(b,NV##d##r,(PRM_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRM_REG_RD_DRF(b,d,r,f)       (((PRM_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_PRMIO registers
//******************************************************************************

#define PRMIO_REG_WR32(b,o,d)   (b)->Reg032[((o)-DEVICE_BASE(NV_PRMIO))/4]=(U032)(d)
#define PRMIO_REG_RD32(b,o)     (b)->Reg032[((o)-DEVICE_BASE(NV_PRMIO))/4]
#define PRMIO_REG_WR_DRF_NUM(b,d,r,f,n) PRMIO_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define PRMIO_REG_WR_DRF_DEF(b,d,r,f,c) PRMIO_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define PRMIO_FLD_WR_DRF_NUM(b,d,r,f,n) PRMIO_REG_WR32(b,NV##d##r,(PRMIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define PRMIO_FLD_WR_DRF_DEF(b,d,r,f,c) PRMIO_REG_WR32(b,NV##d##r,(PRMIO_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define PRMIO_REG_RD_DRF(b,d,r,f)       (((PRMIO_REG_RD32(b,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//******************************************************************************
// Macros for NV_USER registers
//******************************************************************************

#define USER_REG_WR32(b,o,d)           (b)->Reg032[(o)/4]=(U032)(d)
#define USER_REG_WR_DRF_NUM(b,d,r,f,n) USER_REG_WR32(b,NV ## d ## r, DRF_NUM(d,r,f,n))
#define USER_REG_WR_DRF_DEF(b,d,r,f,c) USER_REG_WR32(b,NV ## d ## r, DRF_DEF(d,r,f,c))
#define USER_FLD_WR_DRF_NUM(b,d,r,f,n) USER_REG_WR32(b,NV##d##r,(USER_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define USER_FLD_WR_DRF_DEF(b,d,r,f,c) USER_REG_WR32(b,NV##d##r,(USER_REG_RD32(b,NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))

//******************************************************************************
// Mode Entry Structure (Taken from Resource Manager code and modified)
//******************************************************************************

#define NUM_RESOLUTIONS 18
#define NUM_REFRESHES 7
#define NUM_DEPTHS 5
#define NUM_ELEMENTS 5

extern U016 fbTimingTable[NUM_RESOLUTIONS*NUM_DEPTHS][NUM_REFRESHES][NUM_ELEMENTS];


//******************************************************************************
// Master NVInfo Structure
//
// CAREFUL:  There are now 2 HWINFO structures, the original one (this one)
//           and the one that also exists in the RM (in nvrm.h).  Modeset.c
//           includes nvrm.h but does NOT include NV.H, so for now, this is ok.
//           (This confuses Soft-Ice though when loading symbols).
// TODO:     These structures should be renamed or consolidated to minimize confusion!
//******************************************************************************

typedef struct _def_hw_info
{
    struct _def_chip_info
    {
        U032 Size;
        char ID[32];
        U032 SavePmc;
        U032 SaveIntrEn0;
        U032 IntrEn0;
        U032 Implementation;
        U032 Revision;
        U032 Architecture;
        U032 Manufacturer;
        U032 Bus;
        U032 ChipToken[2];
        U032 ServiceCount;
        U032 ServiceTimeLo;
        U032 ServiceTimeHi;
        U032 ExceptionTimeLo;
        U032 ExceptionTimeHi;
    } Chip;
    struct _def_mapping_info
    {
        U032 Size;
        char ID[32];
        U032 PhysAddr;
        U032 IntLine;
        U032 IntPin;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Mapping;
    struct _def_pram_info
    {
        U032 Size;
        char ID[32];
        U032 ConfigReg;
        U032 HashDepth;
        U032 RunOutMask;
        U032 FreeInstSize;
        U032 FreeInstBase;
        U032 FreeInstMax;
        U032 CurrentSize;
        U032 AvailableSize[4];
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 HashTableAddr;
        U032 FifoRunoutAddr;
        U032 FifoContextAddr;
        U032 AudioScratchAddr;
        U032 AuthScratchAddr;
        U032 DMATextureInstances;
    } Pram;
    struct _def_fifo_info
    {
        U032 Size;
        char ID[32];
        U032 ServiceCount;
        U032 AllocateCount;
        U032 CacheDepth;
        U032 LieDepth;
        U032 RetryCount;
        U032 Count;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 DmaFetchAmount;       // Burst amount the DMA pusher should use
                                   // when reading into the FIFO
        U032 DmaWaterMark;         // Fifo level which will kick of the next
                                   // DMA burst
    } Fifo;
    struct _def_master_info
    {
        U032 Size;
        char ID[32];
        U032 UpdateFlags;
        U032 FinishFlags;
    } Master;
    struct _def_framebuffer_info
    {
        U032 Size;
        char ID[32];
        U032 ConfigReg;
        U032 ConfigPageHeight;
        U032 RamSizeMb;
        U032 RamSize;
        U032 RamType;
        U032 RamTech;
        U032 DpmLevel;
        U032 Depth;
        U032 Resolution;
        U032 RefreshRate;
        U032 HorizFrontPorch;
        U032 HorizSyncWidth;
        U032 HorizBackPorch;
        U032 HorizDisplayWidth;
        U032 VertFrontPorch;
        U032 VertSyncWidth;
        U032 VertBackPorch;
        U032 VertDisplayWidth;
        U032 HSyncPolarity;
        U032 VSyncPolarity;
        U032 CSync;
        U032 Count;
        U032 ServiceCount;
        U032 ActiveCount;
        U032 ActiveMask;
        U032 Start[3];
        U032 Limit[3];
        U032 Current;
        U032 FlipUsageCount;
        U032 FlipTo;
        U032 FlipFrom;
        U032 VBlankToggle;
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 MonitorType;
        U032 FilterEnable;
        U032 SaveToEEPROMEnable;
        U032 Underscan_x;
        U032 Underscan_y;
        U032 Scale_x;
        U032 Scale_y;
        U032 Instance;
        U032 DmaInstance;
    } Framebuffer;
    struct _def_graphics_info
    {
        U032 Size;
        char ID[32];
        U032 Debug0;
        U032 Debug1;
        U032 Debug2;
        U032 Debug3;
        U032 CurrentChID;
        U032 Count;
        U032 CanvasCount;
        U032 Enabled;
        U032 ServiceCount;
        U032 ComplexClipCount;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Graphics;
    struct _def_video_info
    {
        U032 CurrentChID;
        U032 ColorKeyEnabled;
        U032 ScaleFactor;   // 12.20
        U032 Enabled;
        U032 VideoStart;
        U032 VideoSize;
        U032 VideoScale;
        U032 VideoColorKey;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Video;
    struct _def_mediaport_info
    {
        U032 CurrentChID;
        U032 Enabled;
    } MediaPort;
    struct _def_audio_info
    {
        U032 Size;
        char ID[32];
        U032 OutLogVolumeLeft;
        U032 OutLogVolumeRight;
        U032 NoteLogVolumeLeft;
        U032 NoteLogVolumeRight;
        U032 OutLinVolumeLeft;
        U032 OutLinVolumeRight;
        U032 NoteLinVolumeLeft;
        U032 NoteLinVolumeRight;
        U032 VolumeThreshold;
        U032 MaxNotes;
        U032 MaxIOs;
        U032 NoteCount;
        U032 InCount;
        U032 OutCount;
        U032 NearMark;
        U032 BlockLength;
        U032 RateConversion;
        U032 ServiceCount;
        U032 Enabled;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Audio;
    struct _def_dac_info
    {
        U032 Size;
        char ID[32];
        U032 InputWidth;
        U032 PClkVClkRatio;
        U032 VClk;
        U032 MClk;
        U032 VPllM;
        U032 VPllN;
        U032 VPllO;
        U032 VPllP;
        U032 MPllM;
        U032 MPllN;
        U032 MPllO;
        U032 MPllP;
        U032 DpmLevel;
        S032 CursorPosX;
        S032 CursorPosY;
        U032 CursorWidth;
        U032 CursorHeight;
        U008 CursorImagePlane[2][32 * sizeof(U032)];
        U032 CursorType;
        U032 CursorColor1;
        U032 CursorColor2;
        U032 CursorColor3;
        U008 CursorColorImage[32][32 * sizeof(U032)];
        S032 CursorSaveUnderPosX;
        S032 CursorSaveUnderPosY;
        U032 CursorSaveUnderWidth;
        U032 CursorSaveUnderHeight;
        U032 CursorSaveUnder[32][32];
        U032 CursorExclude;
        U032 CursorEmulation;
        U032 Palette[272];
        U032 UpdateFlags;
        U032 FinishFlags;
        U032 CursorImageInstance;
        U032 TVCursorMin;
        U032 TVCursorMax;
    } Dac;
    struct _def_codec_info
    {
        U032 Size;
        char ID[32];
        U032 Chip;
        U032 DpmLevel;
        U032 Frequency;
        U032 OutputAttenuation;
        U032 InputSource;
        U032 InputLineLogGainLeft;
        U032 InputLineLogGainRight;
        U032 InputAuxLogGainLeft;
        U032 InputAuxLogGainRight;
        U032 InputMicLogGainLeft;
        U032 InputMicLogGainRight;
        U032 InputMixLogGainLeft;
        U032 InputMixLogGainRight;
        U032 PassThruAux1LogLeft;
        U032 PassThruAux1LogRight;
        U032 PassThruAux2LogLeft;
        U032 PassThruAux2LogRight;
        U032 InputLineLinGainLeft;
        U032 InputLineLinGainRight;
        U032 InputAuxLinGainLeft;
        U032 InputAuxLinGainRight;
        U032 InputMicLinGainLeft;
        U032 InputMicLinGainRight;
        U032 InputMixLinGainLeft;
        U032 InputMixLinGainRight;
        U032 PassThruAux1LinLeft;
        U032 PassThruAux1LinRight;
        U032 PassThruAux2LinLeft;
        U032 PassThruAux2LinRight;
        U032 Monitor;
        U032 ControlPins;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Codec;
    struct _def_dgp_chip_info
    {
        U032 Size;
        char ID[32];
        U032 Chip;
        U032 Enable3D;
        U032 Scale;
        U032 HorizDelay;
        U032 HorizOffset;
        U032 HorizScale;
        U032 VertOffset;
        U032 VertScale;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Dgp;
    struct _def_gameport_info
    {
        U032 Size;
        char ID[32];
        U032 DevicePort;
        V032 InputTypes;
        V032 DigitalInputs;
        S032 AnalogInputs[8];
        U032 XferRateDelay;
        U032 HorizRange;
        S032 HorizCenter;
        U032 VertRange;
        S032 VertCenter;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Gameport[13];
    struct _def_timer_info
    {
        U032 Size;
        char ID[32];
        U032 Denominator;
        U032 Numerator;
        U032 ServiceCount;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Timer;
    struct _def_vga_info
    {
        U032 Size;
        char ID[32];
        U032 Enabled;
        U032 GlobalEnable;
        U032 RomVersion;
        U032 RomFixupCode;
        U032 UpdateFlags;
        U032 FinishFlags;
    } Vga;
    struct _def_classes_info
    {
        U032 Size;
        char ID[32];
        U032 UpdateFlags;
        U032 FinishFlags;
    } Classes;
    struct _def_stat_info
    {
        U032 Size;
        char ID[32];
        U032 AudioExceptionCount;
        U032 FifoExceptionCount;
        U032 FramebufferExceptionCount;
        U032 GraphicsExceptionCount;
        U032 TimerExceptionCount;
        U032 TotalExceptionCount;
        U032 IntTimeLo;
        U032 IntTimeHi;
        U032 ExecTimeLo;
        U032 ExecTimeHi;
        U032 MemAlloced;
        U032 MemLocked;
        U032 UpdateLo;
        U032 UpdateHi;
    } Statistics;
    struct _def_null_info
    {
        U032 Size;
        char ID[32];
    } Null;
} HWINFO, * PHWINFO;


typedef struct _VIDEO_COPROCESSOR_INFORMATION {
    ULONG ulChipID;         // ET3000, ET4000, W32, W32I, W32P, or ET6000
    ULONG ulRevLevel;       // REV_A, REV_B, REV_C, REV_D, REV_UNDEF
    ULONG ulVideoMemory;    // in bytes
} VIDEO_COPROCESSOR_INFORMATION, *PVIDEO_COPROCESSOR_INFORMATION;

typedef enum _CHIP_TYPE {
    NV3 = 1,
    NV4,
} CHIP_TYPE;

typedef enum _REV_TYPE {
    REV_UNDEF = 1,
    REV_A,
    REV_B,
    REV_C,
    REV_D,
} REV_TYPE;

//
//  NV1 PCI defines - These values should match those in NV1_REF.H
//
#define NV1_VENDOR_ID       0x10DE
#define ST1_VENDOR_ID       0x104A
#define NV1_DEVICE_VGA_ID   0x0008
#define NV1_DEVICE_NV1_ID   0x0009


//
//  NV3 PCI defines - These values should match those in NV3_REF.H
//
#define NV3_VENDOR_ID       0x12D2
#define ST3_VENDOR_ID       0x12D2
#define NV3_DEVICE_VGA_ID   0x0008
#define NV3_DEVICE_NV3_ID   0x0018
#define NV3_DEVICE_NV3_ID_2 0x0019


//
//  NV4 PCI defines - These values should match those in NV4_REF.H
//
#define NV4_VENDOR_ID                   0x10DE
#define ST4_VENDOR_ID                   0x10DE
#define NV4_DEVICE_VGA_ID               0x0008
#define NV4_DEVICE_NV4_ID               0x0020
#define NV5_DEVICE_NV5_ID               0x0028
#define NV5ULTRA_DEVICE_NV5ULTRA_ID     0x0029
#define NV5VANTA_DEVICE_NV5VANTA_ID     0x002C
#define NV5MODEL64_DEVICE_NV5MODEL64_ID 0x002D
#define NV0A_DEVICE_NV0A_ID             0x00A0
#define NV10_DEVICE_NV10_ID             0x0100
#define NV10DDR_DEVICE_NV10DDR_ID       0x0101
#define NV10GL_DEVICE_NV10GL_ID         0x0103
#define NV11_DEVICE_NV11_ID             0x0110
#define NV11DDR_DEVICE_NV11DDR_ID       0x0111
#define NV11M_DEVICE_NV11M_ID           0x0112
#define NV11GL_DEVICE_NV11GL_ID         0x0113
#define NV15_DEVICE_NV15_ID             0x0150
#define NV15DDR_DEVICE_NV15DDR_ID       0x0151
#define NV15BR_DEVICE_NV15BR_ID         0x0152
#define NV15GL_DEVICE_NV15GL_ID         0x0153
#define NV20_DEVICE_NV20_ID             0x0200
#define NV20_DEVICE_NV20_1_ID           0x0201
#define NV20_DEVICE_NV20_2_ID           0x0202
#define NV20_DEVICE_NV20_3_ID           0x0203


//////////////////////////////////////////////////////////////////////////////

//
// Do full save and restore.
//

#define EXTENDED_REGISTER_SAVE_RESTORE 1

//
// BIOS Variables
//

#define BIOS_INFO_1 0x488
#define PRODESIGNER_BIOS_INFO 0x4E8

//
// Define type of ET4000 boards
//

typedef enum _BOARD_TYPE {
    SPEEDSTARPLUS = 1,
    SPEEDSTAR24,
    SPEEDSTAR,
    PRODESIGNERIISEISA,
    PRODESIGNERIIS,
    PRODESIGNER2,
    TSENG3000,
    TSENG4000,
    TSENG4000W32,
    STEALTH32,
    TSENG6000,
    OTHER
} BOARD_TYPE;



//
// Base address of VGA memory range.  Also used as base address of VGA
// memory when loading a font, which is done with the VGA mapped at A0000.
//

#define MEM_VGA      0xA0000
#define MEM_VGA_SIZE 0x20000

//******************************************************************************
// Indices corresponding to the NV Memory ranges
//******************************************************************************

#define NV_PHYSICAL_ADDRESS_QTY         3
#define NV_PHYSICAL_BASE_ADDRESS        0
#define NV_PHYSICAL_DFB_ADDRESS         1
#define NV_PHYSICAL_REMAPPER_ADDRESS    2

#define NV_PMC_INDEX          0
#define NV_PFB_INDEX          1
#define NV_PRM_INDEX          2
#define NV_PDAC_INDEX         3
#define NV_PRAM_INDEX         4
#define NV_PRAMFC_INDEX       5
#define NV_PRAMHT_INDEX       6
#define NV_PFIFO_INDEX        7
#define NV_PBUS_INDEX         8
#define NV_PGRAPH_INDEX       9
#define NV_URECT_INDEX        10
#define NV_UTRI_INDEX         11
#define NV_UBLIT_INDEX        12
#define NV_UIMAGE_INDEX       13
#define NV_UBITMAP_INDEX      14
#define NV_CONFIG_INDEX       15
#define NV_PDMA_INDEX         16
#define NV_PTIMER_INDEX       17
#define NV_PAUDIO_INDEX       18
#define NV_PAUTH_INDEX        19
#define NV_PEXTDEV_INDEX      20
#define NV_PEEPROM_INDEX      21
#define NV_PROM_INDEX         22
#define NV_PALT_INDEX         23

#define NV_USER_INDEX          24
#define NV_PGRAPH_STATUS_INDEX 25
#define NV_PDFB_INDEX          26

#define NV_PRAMIN_INDEX        27
#define NV_PRMVIO_INDEX        28
#define NV_PRMCIO_INDEX        29

#define NV_PFB_CONFIG_0_INDEX  30
#define NV_PFB_START_ADDRESS_INDEX  31

#define NV_PRAMDAC_INDEX       32
#define NV_USERDAC_INDEX       33

#define NV_SGS_DAC_INDEX       34
#define NV_PVIDEO_INDEX        35

#define NV_NUM_RANGES          36

#define BANKED_FRAME_BUFFER 3
#define LINEAR_FRAME_BUFFER 4

//
// W32 MMU stuff
//

#define PORT_IO_ADDR                0
#define PORT_IO_LEN                 0x10000

// When we are banked

#define BANKED_MMU_BUFFER_MEMORY_ADDR          0xB8000
#define BANKED_MMU_BUFFER_MEMORY_LEN           (0xBE000 - 0xB8000)
#define BANKED_MMU_MEMORY_MAPPED_REGS_ADDR     0xBFF00
#define BANKED_MMU_MEMORY_MAPPED_REGS_LEN      (0xC0000 - 0xBFF00)
#define BANKED_MMU_EXTERNAL_MAPPED_REGS_ADDR   0xBE000
#define BANKED_MMU_EXTERNAL_MAPPED_REGS_LEN    (0xBF000 - 0xBE000)

#define BANKED_APERTURE_0_OFFSET   0x0000
#define BANKED_APERTURE_1_OFFSET   0x2000
#define BANKED_APERTURE_2_OFFSET   0x4000

// When we are linear

#define MMU_BUFFER_MEMORY_ADDR          0x200000
#define MMU_BUFFER_MEMORY_LEN           0x180000
#define MMU_MEMORY_MAPPED_REGS_ADDR     0x3FFF00
#define MMU_MEMORY_MAPPED_REGS_LEN      0x000100
#define MMU_EXTERNAL_MAPPED_REGS_ADDR   0x3FE000
#define MMU_EXTERNAL_MAPPED_REGS_LEN    0x001000

typedef struct {
    ULONG  ulOffset;
    ULONG  ulLength;
} RANGE_OFFSETS;

#define APERTURE_0_OFFSET   0x000000
#define APERTURE_1_OFFSET   0x080000
#define APERTURE_2_OFFSET   0x100000

#define MMU_APERTURE_2_ACL_BIT  0x04

typedef struct {
    ULONG   ulPhysicalAddress;
    ULONG   ulLength;
    ULONG   ulInIoSpace;
    PVOID   pvVirtualAddress;
} W32_ADDRESS_MAPPING_INFORMATION, *PW32_ADDRESS_MAPPING_INFORMATION;


//
// Port definitions for filling the ACCESS_RANGES structure in the miniport
// information, defines the range of I/O ports the VGA spans.
// There is a break in the IO ports - a few ports are used for the parallel
// port. Those cannot be defined in the ACCESS_RANGE, but are still mapped
// so all VGA ports are in one address range.
//

#define VGA_BASE_IO_PORT      0x000003B0
#define VGA_START_BREAK_PORT  0x000003BB
#define VGA_END_BREAK_PORT    0x000003C0
#define VGA_MAX_IO_PORT       0x000003DF

//
// W32 CRTCB port addresses (used for ID)
//

#define CRTCB_IO_PORT_BASE    0x0000217A
#define CRTCB_IO_PORT_LEN     0x00000002

#define CRTCB_IO_PORT_INDEX   CRTCB_IO_PORT_BASE
#define CRTCB_IO_PORT_DATA    (CRTCB_IO_PORT_INDEX+1)
#define IND_CRTCB_CHIP_ID     0xEC




//
// VGA register definitions
//
#define NUM_VGA_ACCESS_RANGES  3
                                            // ports in monochrome mode
#define CRTC_ADDRESS_PORT_MONO      0x03B4  // CRT Controller Address and
#define CRTC_DATA_PORT_MONO         0x03B5  // Data registers in mono mode
#define MODE_CONTROL_PORT_MONO      0x03B8  // Tseng Mode Control port, used
                                            //  here only for unlocking the
                                            //  key so we can get at extended
                                            //  registers
#define FEAT_CTRL_WRITE_PORT_MONO   0x03BA  // Feature Control write port
                                            // in mono mode
#define INPUT_STATUS_1_MONO         0x03BA  // Input Status 1 register read
                                            // port in mono mode
#define ATT_INITIALIZE_PORT_MONO    INPUT_STATUS_1_MONO
                                            // Register to read to reset
                                            // Attribute Controller index/data
                                            // toggle in mono mode
#define HERCULES_COMPATIBILITY_PORT 0x03BF  // used to unlock Tseng key to
                                            //  get at extended ports

#define ATT_ADDRESS_PORT            0x03C0  // Attribute Controller Address and
#define ATT_DATA_WRITE_PORT         0x03C0  // Data registers share one port
                                            // for writes, but only Address is
                                            // readable at 0x010
#define ATT_DATA_READ_PORT          0x03C1  // Attribute Controller Data reg is
                                            // readable here
#define MISC_OUTPUT_REG_WRITE_PORT  0x03C2  // Miscellaneous Output reg write
                                            // port
#define INPUT_STATUS_0_PORT         0x03C2  // Input Status 0 register read
                                            // port
#define VIDEO_SUBSYSTEM_ENABLE_PORT 0x03C3  // Bit 0 enables/disables the
                                            // entire VGA subsystem
#define SEQ_ADDRESS_PORT            0x03C4  // Sequence Controller Address and
#define SEQ_DATA_PORT               0x03C5  // Data registers
#define DAC_PIXEL_MASK_PORT         0x03C6  // DAC pixel mask reg
#define DAC_ADDRESS_READ_PORT       0x03C7  // DAC register read index reg,
                                            // write-only
#define DAC_STATE_PORT              0x03C7  // DAC state (read/write),
                                            // read-only
#define DAC_ADDRESS_WRITE_PORT      0x03C8  // DAC register write index reg
#define DAC_DATA_REG_PORT           0x03C9  // DAC data transfer reg
#define FEAT_CTRL_READ_PORT         0x03CA  // Feature Control read port
#define MISC_OUTPUT_REG_READ_PORT   0x03CC  // Miscellaneous Output reg read
                                            // port
#define SEGMENT_SELECT_PORT         0x03CD  // Tseng banking control register
#define SEGMENT_SELECT_HIGH         0x03CB  // Tseng W32 SegSel extension
#define GRAPH_ADDRESS_PORT          0x03CE  // Graphics Controller Address
#define GRAPH_DATA_PORT             0x03CF  // and Data registers

                                            // ports in color mode
#define CRTC_ADDRESS_PORT_COLOR     0x03D4  // CRT Controller Address and
#define CRTC_DATA_PORT_COLOR        0x03D5  // Data registers in color mode
#define MODE_CONTROL_PORT_COLOR     0x03D8  // Tseng Mode Control port, used
                                            //  here only for unlocking the
                                            //  key so we can get at extended
                                            //  registers
#define FEAT_CTRL_WRITE_PORT_COLOR  0x03DA  // Feature Control write port
#define INPUT_STATUS_1_COLOR        0x03DA  // Input Status 1 register read
                                            // port in color mode
#define ATT_INITIALIZE_PORT_COLOR   INPUT_STATUS_1_COLOR
                                            // Register to read to reset
                                            // Attribute Controller index/data
                                            // toggle in color mode
//
// Offsets in HardwareStateHeader->PortValue[] of save areas for non-indexed
// VGA registers.
//

#define CRTC_ADDRESS_MONO_OFFSET      0x04
#define FEAT_CTRL_WRITE_MONO_OFFSET   0x0A
#define ATT_ADDRESS_OFFSET            0x10
#define MISC_OUTPUT_REG_WRITE_OFFSET  0x12
#define VIDEO_SUBSYSTEM_ENABLE_OFFSET 0x13
#define SEQ_ADDRESS_OFFSET            0x14
#define DAC_PIXEL_MASK_OFFSET         0x16
#define DAC_STATE_OFFSET              0x17
#define DAC_ADDRESS_WRITE_OFFSET      0x18
#define GRAPH_ADDRESS_OFFSET          0x1E
#define CRTC_ADDRESS_COLOR_OFFSET     0x24
#define FEAT_CTRL_WRITE_COLOR_OFFSET  0x2A

//
// VGA indexed register indexes.
//

#define IND_CURSOR_START        0x0A    // index in CRTC of the Cursor Start
#define IND_CURSOR_END          0x0B    //  and End registers
#define IND_CURSOR_HIGH_LOC     0x0E    // index in CRTC of the Cursor Location
#define IND_CURSOR_LOW_LOC      0x0F    //  High and Low Registers
#define IND_VSYNC_END           0x11    // index in CRTC of the Vertical Sync
                                        //  End register, which has the bit
                                        //  that protects/unprotects CRTC
                                        //  index registers 0-7
#define IND_SET_RESET_ENABLE    0x01    // index of Set/Reset Enable reg in GC
#define IND_DATA_ROTATE         0x03    // index of Data Rotate reg in GC
#define IND_READ_MAP            0x04    // index of Read Map reg in Graph Ctlr
#define IND_GRAPH_MODE          0x05    // index of Mode reg in Graph Ctlr
#define IND_GRAPH_MISC          0x06    // index of Misc reg in Graph Ctlr
#define IND_BIT_MASK            0x08    // index of Bit Mask reg in Graph Ctlr
#define IND_SYNC_RESET          0x00    // index of Sync Reset reg in Seq
#define IND_MAP_MASK            0x02    // index of Map Mask in Sequencer
#define IND_MEMORY_MODE         0x04    // index of Memory Mode reg in Seq
#define IND_STATE_CONTROL       0x06    // index of TS State Control reg in Seq
#define IND_TS_AUX_MODE         0x07    // index of TS Aux Mode reg in Seq
#define IND_CRTC_PROTECT        0x11    // index of reg containing regs 0-7 in
                                        //  CRTC
#define IND_RAS_CAS_CONFIG      0x32    // index of RAS/CAS Config reg in CRTC
#define IND_EXT_START_ADDR      0x33    // index of Extended Start Address reg
                                        //  in CRTC
#define IND_CRTC_COMPAT         0x34    // index of CRTC Compatibility reg
                                        //  in CRTC
#define IND_OFLOW_HIGH          0x35    // index of Overflow High reg in CRTC
#define IND_VID_SYS_CONFIG_1    0x36    // index of Video System Configuration
#define IND_VID_SYS_CONFIG_2    0x37    //  1 & 2 registers in CRTC
#define IND_ATC_MISC            0x16    // index of Miscellaneous reg in ATC

#define START_SYNC_RESET_VALUE  0x01    // value for Sync Reset reg to start
                                        //  synchronous reset
#define END_SYNC_RESET_VALUE    0x03    // value for Sync Reset reg to end
                                        //  synchronous reset

#define UNLOCK_KEY_1            0x03    // value to output to Herc Compat
                                        //  register as first step in unlocking
                                        //  key so Tseng registers can be set
#define UNLOCK_KEY_2            0xA0    // value to output to Mode Control Port
                                        //  register as 2nd step in unlocking
                                        //  key so Tseng registers can be set
#define LOCK_KEY_1              0x00    // value to output to Herc Compat
                                        //  register as first step in locking
                                        //  key so Tseng registers can't be set
#define LOCK_KEY_2              0x00    // value to output to Mode Control Port
                                        //  register as 2nd step in locking
                                        //  key so Tseng registers can't be set
#define HERCULES_COMPATIBILITY_DEFAULT 0x00
                                        // value to output to Herc Compat
                                        //  register to put back to MDA
                                        //  compatibility

#define MODE_CONTROL_PORT_COLOR_DEFAULT 0x00
#define MODE_CONTROL_PORT_MONO_DEFAULT 0x00
                                        // values to output to CGA and MDA mode
                                        //  registers to put to default state
                                        //  (video disabled).

//
// Values for Attribute Controller Index register to turn video off
// and on, by setting bit 5 to 0 (off) or 1 (on).
//

#define VIDEO_DISABLE 0
#define VIDEO_ENABLE  0x20

// Masks to keep only the significant bits of the Graphics Controller and
// Sequencer Address registers. Masking is necessary because some VGAs, such
// as S3-based ones, don't return unused bits set to 0, and some SVGAs use
// these bits if extensions are enabled.
//

#define GRAPH_ADDR_MASK 0x0F
#define SEQ_ADDR_MASK   0x07

//
// Mask used to toggle Chain4 bit in the Sequencer's Memory Mode register.
//

#define CHAIN4_MASK 0x08

//
// Value written to the Read Map register when identifying the existence of
// a VGA in VgaInitialize. This value must be different from the final test
// value written to the Bit Mask in that routine.
//

#define READ_MAP_TEST_SETTING 0x03

//
// Default text mode setting for various registers, used to restore their
// states if VGA detection fails after they've been modified.
//

#define MEMORY_MODE_TEXT_DEFAULT 0x02
#define BIT_MASK_DEFAULT 0xFF
#define READ_MAP_DEFAULT 0x00

//
// Palette-related info.
//

//
// Highest valid DAC color register index.
//

#define VIDEO_MAX_COLOR_REGISTER  0xFF

//
// Highest valid palette register index
//

#define VIDEO_MAX_PALETTE_REGISTER 0x0F



//
// Indices for type of memory mapping; used in ModesVGA[], must match
// MemoryMap[].
//

typedef enum _VIDEO_MEMORY_MAP {
    MemMap_Mono,
    MemMap_CGA,
    MemMap_VGA
} VIDEO_MEMORY_MAP, *PVIDEO_MEMORY_MAP;

//
// Memory map table definition
//

typedef struct {
    ULONG   MaxSize;        // Maximum addressable size of memory
    ULONG   Start;          // Start address of display memory
} MEMORYMAPS;

//
// For a mode, the type of banking supported. Controls the information
// returned in VIDEO_BANK_SELECT. PlanarHCBanking includes NormalBanking.
//

typedef enum _BANK_TYPE {
    NoBanking = 0,
    MemMgrBanking,
    NormalBanking,
    PlanarHCBanking
} BANK_TYPE, *PBANK_TYPE;


//
// Structure used to describe each video mode in ModesVGA[].
//

typedef struct {
    USHORT  fbType;             // color or monochrome, text or graphics, via
                                //  VIDEO_MODE_COLOR and VIDEO_MODE_GRAPHICS
    USHORT  numPlanes;          // # of video memory planes
    USHORT  bitsPerPlane;       // # of bits of color in each plane
    SHORT   col;                // # of text columns across screen with default font

    SHORT   row;                // # of text rows down screen with default font
    USHORT  hres;               // # of pixels across screen
    USHORT  vres;               // # of scan lines down screen
    USHORT  wbytes;             // # of bytes from start of one scan line to start of next
    ULONG   sbytes;             // total size of addressable display memory in bytes
    ULONG   Frequency;          // Vertical Frequency
    ULONG   Interlaced;         // Determines if the mode is interlaced or not
    BANK_TYPE banktype;         // NoBanking, NormalBanking, PlanarHCBanking
    VIDEO_MEMORY_MAP MemMap;    // index from VIDEO_MEMORY_MAP of memory
                                //  mapping used by this mode
    BOOLEAN ValidMode;          // Determines which modes are valid.
    ULONG   Int10ModeNumber;    // Mode number via Int 10
    PUSHORT CmdStrings;         // pointer to array of register-setting commands
} VIDEOMODE, *PVIDEOMODE;


//
// Mode into which to put the VGA before starting a VDM, so it's a plain
// vanilla VGA.  (This is the mode's index in ModesVGA[], currently standard
// 80x25 text mode.)
//

#define DEFAULT_MODE 0



// 
// Added structs for control panel support
//

// For setting the current device type 
typedef struct  tagSETGETDEVTYPE
{
    unsigned long   dwSuccessFlag;
    unsigned long   dwDeviceType;
    unsigned long   dwTvFormat;
} SETGETDEVTYPE, *PSETGETDEVTYPE;

// For validating a particular mode on a particular device type 
typedef struct  tagVALMODEEXTR
{
    unsigned long   dwHRes;
    unsigned long   dwVRes;
    unsigned long   dwBpp;
    unsigned long   dwRefresh;
    unsigned long   dwDeviceType;
} VALMODEXTR;

// Size of the color lookup table cache in the device extension
#define MAX_CLUT_SIZE (sizeof(VIDEO_CLUT) + (sizeof(ULONG) * 256))


//
// Info used by the Validator functions and save/restore code.
// Structure used to trap register accesses that must be done atomically.
//


//
// Number of each type of indexed register in a standard VGA, used by
// validator and state save/restore functions.
//
// Note: VDMs currently only support basic VGAs only.
//

#define VGA_NUM_SEQUENCER_PORTS     5
#define VGA_NUM_CRTC_PORTS         25
#define VGA_NUM_GRAPH_CONT_PORTS    9
#define VGA_NUM_ATTRIB_CONT_PORTS  21
#define VGA_NUM_DAC_ENTRIES       256

#ifdef EXTENDED_REGISTER_SAVE_RESTORE

//
// Indices to start save/restore in extension registers:
// For both chip types

#define ET4000_SEQUENCER_EXT_START     0x06
#define ET4000_SEQUENCER_EXT_END       0x07

#define ET4000_CRTC_EXT_START          0x31
#define ET4000_CRTC_EXT_END            0x37
#define ET4000_CRTC_1_EXT_START        0x3F
#define ET4000_CRTC_1_EXT_END          0x3F

#define ET4000_ATTRIB_EXT_START        0x16
#define ET4000_ATTRIB_EXT_END          0x16

//
// Number of extended regs for both chip types.
//

#define ET4000_NUM_SEQUENCER_EXT_PORTS (ET4000_SEQUENCER_EXT_END - ET4000_SEQUENCER_EXT_START + 1)
#define ET4000_NUM_CRTC_EXT_PORTS      (ET4000_CRTC_EXT_END - ET4000_CRTC_EXT_START + 1) + \
                                       (ET4000_CRTC_1_EXT_END - ET4000_CRTC_1_EXT_START + 1)
#define ET4000_NUM_ATTRIB_EXT_PORTS    (ET4000_ATTRIB_EXT_END - ET4000_ATTRIB_EXT_START + 1)

//
// set values for save/restore area based on largest value for a chipset.
//

#define EXT_NUM_GRAPH_CONT_PORTS    0
#define EXT_NUM_SEQUENCER_PORTS     ET4000_NUM_SEQUENCER_EXT_PORTS
#define EXT_NUM_CRTC_PORTS          ET4000_NUM_CRTC_EXT_PORTS
#define EXT_NUM_ATTRIB_CONT_PORTS   ET4000_NUM_ATTRIB_EXT_PORTS
#define EXT_NUM_DAC_ENTRIES         0

#else

#define EXT_NUM_GRAPH_CONT_PORTS    0
#define EXT_NUM_SEQUENCER_PORTS     0
#define EXT_NUM_CRTC_PORTS          0
#define EXT_NUM_ATTRIB_CONT_PORTS   0
#define EXT_NUM_DAC_ENTRIES         0

#endif


#define VGA_MAX_VALIDATOR_DATA       100

#define VGA_VALIDATOR_UCHAR_ACCESS   1
#define VGA_VALIDATOR_USHORT_ACCESS  2
#define VGA_VALIDATOR_ULONG_ACCESS   3

typedef struct _VGA_VALIDATOR_DATA {
   ULONG Port;
   UCHAR AccessType;
   ULONG Data;
} VGA_VALIDATOR_DATA, *PVGA_VALIDATOR_DATA;

//
// Number of bytes to save in each plane.
//

#define VGA_PLANE_SIZE 0x10000

//
// These constants determine the offsets within the
// VIDEO_HARDWARE_STATE_HEADER structure that are used to save and
// restore the VGA's state.
//

#define VGA_HARDWARE_STATE_SIZE sizeof(VIDEO_HARDWARE_STATE_HEADER)

#define VGA_BASIC_SEQUENCER_OFFSET (VGA_HARDWARE_STATE_SIZE + 0)
#define VGA_BASIC_CRTC_OFFSET (VGA_BASIC_SEQUENCER_OFFSET + \
         VGA_NUM_SEQUENCER_PORTS)
#define VGA_BASIC_GRAPH_CONT_OFFSET (VGA_BASIC_CRTC_OFFSET + \
         VGA_NUM_CRTC_PORTS)
#define VGA_BASIC_ATTRIB_CONT_OFFSET (VGA_BASIC_GRAPH_CONT_OFFSET + \
         VGA_NUM_GRAPH_CONT_PORTS)
#define VGA_BASIC_DAC_OFFSET (VGA_BASIC_ATTRIB_CONT_OFFSET + \
         VGA_NUM_ATTRIB_CONT_PORTS)
#define VGA_BASIC_LATCHES_OFFSET (VGA_BASIC_DAC_OFFSET + \
         (3 * VGA_NUM_DAC_ENTRIES))

#define VGA_EXT_SEQUENCER_OFFSET (VGA_BASIC_LATCHES_OFFSET + 4)
#define VGA_EXT_CRTC_OFFSET (VGA_EXT_SEQUENCER_OFFSET + \
         EXT_NUM_SEQUENCER_PORTS)
#define VGA_EXT_GRAPH_CONT_OFFSET (VGA_EXT_CRTC_OFFSET + \
         EXT_NUM_CRTC_PORTS)
#define VGA_EXT_ATTRIB_CONT_OFFSET (VGA_EXT_GRAPH_CONT_OFFSET +\
         EXT_NUM_GRAPH_CONT_PORTS)
#define VGA_EXT_DAC_OFFSET (VGA_EXT_ATTRIB_CONT_OFFSET + \
         EXT_NUM_ATTRIB_CONT_PORTS)

#define VGA_VALIDATOR_OFFSET (VGA_EXT_DAC_OFFSET + 4 * EXT_NUM_DAC_ENTRIES)

#define VGA_VALIDATOR_AREA_SIZE  sizeof (ULONG) + (VGA_MAX_VALIDATOR_DATA * \
                                 sizeof (VGA_VALIDATOR_DATA)) +             \
                                 sizeof (ULONG) +                           \
                                 sizeof (ULONG) +                           \
                                 sizeof (PVIDEO_ACCESS_RANGE)

#define VGA_MISC_DATA_AREA_OFFSET VGA_VALIDATOR_OFFSET + VGA_VALIDATOR_AREA_SIZE

#define VGA_MISC_DATA_AREA_SIZE  0

#define VGA_PLANE_0_OFFSET VGA_MISC_DATA_AREA_OFFSET + VGA_MISC_DATA_AREA_SIZE

#define VGA_PLANE_1_OFFSET VGA_PLANE_0_OFFSET + VGA_PLANE_SIZE
#define VGA_PLANE_2_OFFSET VGA_PLANE_1_OFFSET + VGA_PLANE_SIZE
#define VGA_PLANE_3_OFFSET VGA_PLANE_2_OFFSET + VGA_PLANE_SIZE

//
// Space needed to store all state data.
//

#define VGA_TOTAL_STATE_SIZE VGA_PLANE_3_OFFSET + VGA_PLANE_SIZE

#if 0
//////////////////////
// H.AZAR: (02/15/99)
//

#define NVP_MAX_VPORTS              1
#define NVP_MAX_SURFACES            5
#define NVP_MAX_TRANSFER_BUFFERS    10

//
// Surface definition (Video or VBI) and related data
//
typedef struct _NVP_SURFACE_INFO
{
    //HANDLE              hSurface;           // DirectDraw surface handle
    ULONG               dwCaps;             // capabilites of this DD surface
    ULONG               pVidMem;            // Flat pointer to the surface
    ULONG               dwOffset;           // Offset from the base of the frame buffer
    ULONG               dwPitch;
    ULONG               dwLength;
    ULONG               dwHeight;
    ULONG               dwWidth;            // Ignored for VBI
    ULONG               dwStartLine;        // Programs Y-crop for image, and startline for vbi
    ULONG               dwPreScaleSizeX;    // Ignored for VBI.. prescale size width
    ULONG               dwPreScaleSizeY;    // Ignored for VBI.. prescale size height
    ULONG               dwFieldType;        // what field was this surface setup to capture?
    //ULONG               bAutoFlip;          // do we turn around and auto program this surface?
    ULONG               bIsFirstFieldCaptured;
    LONG                nNextSurfaceIdx;     // the next surface idx in the chain
} NVP_SURFACE_INFO, *PNVP_SURFACE_INFO;

#define NVP_MAX_EXTRA422 10
// Overlay info
typedef struct _NVP_OVERLAY {
    ULONG   dwOverlayFormat;
    ULONG   dwOverlaySrcX;
    ULONG   dwOverlaySrcY;
    ULONG   dwOverlaySrcPitch;
    ULONG   dwOverlaySrcSize;
    ULONG   dwOverlaySrcWidth;
    ULONG   dwOverlaySrcHeight;
    ULONG   dwOverlayDstWidth;
    ULONG   dwOverlayDstHeight;
    ULONG   dwOverlayDstX;
    ULONG   dwOverlayDstY;
    ULONG   dwOverlayDeltaX;
    ULONG   dwOverlayDeltaY;
    ULONG   dwOverlayColorKey;
    ULONG   dwOverlayMode;
    ULONG   aExtra422OverlayOffset[NVP_MAX_EXTRA422];   // extra memory space used for prescaling
    ULONG   dwExtra422Index;            // index of current extra surface
    ULONG   dwExtra422NumSurfaces;      // number of extra allocated surfaces
    ULONG   dwExtra422Pitch;            // extra surface pitch
    ULONG   pNvPioFlipOverlayNotifierFlat;
    ULONG   dwOverlayMaxDownScale;
    ULONG   dwOverlayBufferIndex;
    ULONG   dwReserved1;
    ULONG   dwReserved2;
} NVP_OVERLAY, *PNVP_OVERLAY;

// Transfer
typedef struct _NVP_TRANSFER_BUFFER
{
    ULONG   dwTransferId;
    ULONG   dwTransferStatus;
    ULONG   pSurfaceData;
    //LONG  nSurfaceIdx;    // Idx of surface we're transfering from
    LONG    lStartLine;
    LONG    lEndLine;
    ULONG   dwTransferFlags;
    ULONG   pDestMDL;
    LONG    nNextTransferIdx;
    LONG    nPrevTransferIdx;
} NVP_TRANSFER_BUFFER, *PNVP_TRANSFER_BUFFER;

// transfer status
#define NVP_TRANSFER_QUEUED         0x01
#define NVP_TRANSFER_IN_PROGRESS    0x02
#define NVP_TRANSFER_DONE           0x03
#define NVP_TRANSFER_FAILURE        0x04
#define NVP_TRANSFER_FREE           0xFFFFFFFF

//
// NVidia Video Port context
//
typedef struct _NVP_CONTEXT
{
    NVP_SURFACE_INFO    aVideoSurfaceInfo[NVP_MAX_SURFACES];
    NVP_SURFACE_INFO    aVBISurfaceInfo[NVP_MAX_SURFACES];
    int                 nNumVidSurf;            // number of allocated video surfaces
    int                 nNumVBISurf;            // number of allocated vbi surfaces
    int                 nVidSurfIdx;            // current video surface index
    int                 nVBISurfIdx;            // current vbi surface index
    int                 nNextVideoSurfId;
    int                 nNextVBISurfId;

    int                 nLastFieldId;           // Id of last field captured
    ULONG               bLastFieldType;         // type of last field captured (TRUE if VBI, FALSE if VIDEO)

    ULONG               dwVPState;              // NVP_STATE_STOPPED, NVP_STATE_STARTED
    ULONG               dwVPFlags;
    ULONG               dwVPConFlags;           // connection flags

    ULONG               dwChipId;               // chip version Id
    ULONG               hClient;
    ULONG               dwDeviceId;             // NV_WIN_DEVICE
    ULONG               dwOverlayObjectID;      // overlay object Id (shared object!)
    ULONG               dwScalerObjectID;       // scaler object Id
    ULONG               dwDVDSubPicObjectID;    // used for prescaling on nv4 and nv5
    ULONG               dwDVDObjectCtxDma;      // context dma used for synchronization.
    int                 nFreeCount;     // fifo free count

    // BUGBUG: hacking declerations to avoid including hardware dependent header files
    //         in "nv.h"
    ULONG               pNVPChannel;            // NvChannel *pNVPChannel; // VPE pio channel
    UCHAR               avpNotifiers[16*9];     // (Media port notifiers) NvNotification avpNotifiers[NVP_CLASS04D_MAX_NOTIFIERS];
    UCHAR               aovNotifiers[16*3];     // (Overlay notifiers) NvNotification aovNotifiers[NVP_CLASS07A_MAX_NOTIFIERS];

    // overlay
    NVP_OVERLAY         nvpOverlay;             // information retrieved from UpdateOverlay32() in Ring3
    ULONG               pOverlayChannel;        // NvChannel *pOverlayChannel; // Overlay channel
    ULONG               dwOvrSubChannel;


    // vidmem to sysmem transfer (vbi and video capture !)
    NVP_TRANSFER_BUFFER aTransferBuffers[NVP_MAX_TRANSFER_BUFFERS];
    UCHAR               axfNotifiers[16*6]; //
    int                 nCurTransferIdx;    // current transfer buffer index
    int                 nLastTransferIdx;
    int                 nRecentTransferIdx; // index of most recently transfered buffer
    int                 nNumTransfer;       // current number of transfer buffer in queue
    ULONG               bxfContextCreated;  // TRUE if context already created
    ULONG               bxfContextFlushed;  // TRUE if context flushed
    ULONG               dwTransferCtxStart; // old transfer context start
    ULONG               dwTransferCtxSize;  // transfer size

    ULONG               hVPSyncEvent;       // media port Vsync event handle // PKEVENT hVSyncEvent;

    // DMA push buffer
    PVOID               pDmaChannel;
    ULONG               *nvDmaFifo;
    ULONG               nvDmaCount;
    ULONG               DmaPushBufTotalSize;    // 32 DWORDs
    UCHAR               DmaPushBufErrNotifier[16*1];

} NVP_CONTEXT, *PNVP_CONTEXT;

//////////////////////
#endif

//
// Device extension for the driver object.  This data is only used
// locally, so this structure can be added to as needed.
//

typedef struct _HW_DEVICE_EXTENSION {

    PUCHAR  IOAddress;            // base I/O address of VGA ports
    PUCHAR  VgaPortRange[NUM_VGA_ACCESS_RANGES];
                                  // Non-palette related VGA regs
    PVOID   VideoMemoryAddress;   // base virtual memory address of VGA memory
    ULONG   AdapterMemorySize;    // size, in bytes, of the memory on the
                                  // board.
    ULONG   ModeIndex;            // index of current mode in ModesVGA[]
    ULONG   NumRegistryModes;     // number of modes specified from registry/vesadata
    ULONG   NumAvailableModes;    // number of valid modes on this device for first head
    ULONG   NumAvailableModes1;    // number of valid modes on this device for second head.
    ULONG   NonPrimaryDevice;     // Denotes if system was booted on this device

//    PVIDEOMODE  CurrentMode;    // pointer to VIDEOMODE structure for
//                                // current mode.



    PMODE_ENTRY  CurrentMode;     // pointer to MODE_ENTRY structure for
                                  // current mode.

    ULONG   ulChipID;
    REV_TYPE    ulRevLevel;
    WCHAR LegacyPnPId[8];         // legacy PnP ID

    USHORT  FontPelColumns;       // Width of the font in pels
    USHORT  FontPelRows;          // height of the font in pels

    NV_CURSOR_INFO NvCursorInfo;  // Cursor position and other info

    UCHAR CursorEnable;           // whether cursor is enabled or not
    UCHAR CursorTopScanLine;      // Cursor Start register setting (top scan)
    UCHAR CursorBottomScanLine;   // Cursor End register setting (bottom scan)

    USHORT BoardID;                // Used to Identify Diamond boards

    //**************************************************************************
    // OLD Style modeset code
    // ModeTiming table  (represents modes that are valid for this adapter)
    // This needs to be put here in the hw device extension for multi adapter
    // configurations. (still used by NV3)
    //**************************************************************************

    MODE_ENTRY fbTimingTable[NUM_RESOLUTIONS*NUM_DEPTHS*NUM_REFRESHES];

    //**************************************************************************
    // NEW Style modeset code
    // Allocate a table which will contain all valid modes
    //**************************************************************************
    USHORT ValidModeTable[MAX_VALID_MODE_TABLE_DATA_WORDS]; // For the first head
    USHORT ValidModeTable1[MAX_VALID_MODE_TABLE_DATA_WORDS];// For the second head

    //***************************************************************************
    // The OEM-specific and monitor-specific restriction mode lists.
    //***************************************************************************
    USHORT RestrictionModeTable0[MAX_VALID_MODE_TABLE_DATA_WORDS]; // For the first head
    USHORT RestrictionModeTable1[MAX_VALID_MODE_TABLE_DATA_WORDS]; // For the second head
    ULONG  NumRestrictionModes0; // Number of monitor restriction modes for head0
    ULONG  NumRestrictionModes1; // Number of monitor restriction modes for head1

    

    //**************************************************************************
    // Used to keep track of where to story parsed registry data
    //**************************************************************************

    ULONG RegistryDataOffset;

    //**************************************************************************
    // Structure containing registry switch data
    //**************************************************************************

    NT_NV_REGISTRY NV_Registry_Table;

    //**************************************************************************
    // Saved BIOS image (64k)
    //**************************************************************************

    UCHAR SavedBiosImageArray[SAVED_BIOS_IMAGE_SIZE];

    

    //**************************************************************************
    // Saved instance memory (128k)
    //**************************************************************************

    UCHAR SavedInstanceMemory[MAX_INSTANCE_MEM_SIZE];
    ULONG TotalInstanceMemory;

    //**************************************************************************
    // NV1 Specific registers
    //**************************************************************************

    PHYSICAL_ADDRESS NV1Registers;     // NV1 initialization registers
    ULONG  NV1RegistersLength;         // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PMC_Registers;    // NV1 Master Control Registers
    PHWREG NV1_Lin_PMC_Registers;      // Linear address to Master Control Registers
    ULONG  NV1_PMC_Length;             // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PFB_Registers;    // NV1 Frame Buffer Control Registers
    PHWREG  NV1_Lin_PFB_Registers;     // Linear address to Frame Buffer Control Registers
    ULONG  NV1_PFB_Length;             // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PRM_Registers;    // NV1 Real Mode Control Registers
    PHWREG  NV1_Lin_PRM_Registers;     // Linear address to Real Mode Control Registers
    ULONG  NV1_PRM_Length;             // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PDAC_Registers;   // NV1 DAC Registers
    PHWREG  NV1_Lin_PDAC_Registers;    // Linear address to DAC Registers
    ULONG  NV1_PDAC_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PRAM_Registers;   // NV1 RAM Registers
    PHWREG  NV1_Lin_PRAM_Registers;    // Linear address to RAM Registers
    ULONG  NV1_PRAM_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PRAMFC_Registers; // NV1 RAMFC Registers
    PHWREG  NV1_Lin_PRAMFC_Registers;  // Linear address to RAMFC Registers
    ULONG  NV1_PRAMFC_Length;          // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PRAMHT_Registers;   // NV1 RAMHT Registers
    PHWREG  NV1_Lin_PRAMHT_Registers;    // Linear address to RAMHT Registers
    ULONG  NV1_PRAMHT_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_USER_Registers;   // NV1 USER Registers
    PHWREG  NV1_Lin_USER_Registers;    // Linear address to USER Registers
    ULONG  NV1_USER_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PFIFO_Registers;  // NV1 FIFO Registers
    PHWREG  NV1_Lin_PFIFO_Registers;   // Linear address to FIFO Registers
    ULONG  NV1_PFIFO_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PBUS_Registers;   // NV1 PBUS Registers
    PHWREG  NV1_Lin_PBUS_Registers;    // Linear address to PBUS Registers
    ULONG  NV1_PBUS_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PGRAPH_Registers; // NV1 PGRAPH Registers
    PHWREG  NV1_Lin_PGRAPH_Registers;  // Linear address to PGRAPH Registers
    ULONG  NV1_PGRAPH_Length;          // NV1 register window



    PHYSICAL_ADDRESS NV1_Phys_CONFIG_Registers; // NV1 Config Registers
    PHWREG NV1_Lin_CONFIG_Registers;   // Linear address to Config Registers
    ULONG  NV1_CONFIG_Length;          // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_MEM_Registers;    // NV1 Mermory Registers
    PHWREG NV1_Lin_MEM_Registers;      // Linear address to Memory Registers
    ULONG  NV1_MEM_Length;             // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_IO_Registers;     // NV1 IO Registers
    PHWREG NV1_Lin_IO_Registers;       // Linear address to IO Registers
    ULONG  NV1_IO_Length;              // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PDMA_Registers;   // NV1 DMA Registers
    PHWREG NV1_Lin_PDMA_Registers;     // Linear address to DMA Registers
    ULONG  NV1_PDMA_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PTIMER_Registers; // NV1 Timer Registers
    PHWREG NV1_Lin_PTIMER_Registers;   // Linear address to Timer Registers
    ULONG  NV1_PTIMER_Length;          // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PAUDIO_Registers; // NV1 Audio Registers
    PHWREG NV1_Lin_PAUDIO_Registers;   // Linear address to Audio Registers
    ULONG  NV1_PAUDIO_Length;          // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PAUTH_Registers;  // NV1 Authority Registers
    PHWREG NV1_Lin_PAUTH_Registers;    // Linear address to Authority Registers
    ULONG  NV1_PAUTH_Length;           // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PEXTDEV_Registers; // NV1 Extdev Registers
    PHWREG NV1_Lin_PEXTDEV_Registers;   // Linear address to Extdev Registers
    ULONG  NV1_PEXTDEV_Length;          // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PEEPROM_Registers;// NV1 Eeprom Registers
    PHWREG NV1_Lin_PEEPROM_Registers;  // Linear address to Eeprom Registers
    ULONG  NV1_PEEPROM_Length;         // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PROM_Registers;   // NV1 ROM Registers
    PHWREG NV1_Lin_PROM_Registers;     // Linear address to ROM Registers
    ULONG  NV1_PROM_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PALT_Registers;   // NV1 Alt Registers
    PHWREG NV1_Lin_PALT_Registers;     // Linear address to Alt Registers
    ULONG  NV1_PALT_Length;            // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_PRMIO_Registers;  // NV1 Real Mode IO Registers
    PHWREG NV1_Lin_PRMIO_Registers;    // Linear address to Real Mode IO Registers
    ULONG  NV1_PRMIO_Length;           // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_URECT_Registers;  // NV1 User Rectangle Registers
    PHWREG NV1_Lin_URECT_Registers;    // Linear address to User Rectangle Registers
    ULONG  NV1_URECT_Length;           // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_UTRI_Registers;  // NV1 User Triangle Registers
    PHWREG NV1_Lin_UTRI_Registers;    // Linear address to User Triangle Registers
    ULONG  NV1_UTRI_Length;           // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_UBLIT_Registers;  // NV1 User BLIT Registers
    PHWREG NV1_Lin_UBLIT_Registers;    // Linear address to User BLIT Registers
    ULONG  NV1_UBLIT_Length;           // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_UIMAGE_Registers;  // NV1 User IMAGE Registers
    PHWREG NV1_Lin_UIMAGE_Registers;    // Linear address to User IMAGE Registers
    ULONG  NV1_UIMAGE_Length;           // NV1 register window

    PHYSICAL_ADDRESS NV1_Phys_UBITMAP_Registers;  // NV1 User IMAGE MONO Registers
    PHWREG NV1_Lin_UBITMAP_Registers;   // Linear address to User IMAGE MONO Registers
    ULONG  NV1_UBITMAP_Length;           // NV1 register window

    //**************************************************************************
    // NV3 Specific registers
    //**************************************************************************

    PHYSICAL_ADDRESS NV3_Phys_PRAMIN_Registers;   // NV3 RAMIN Registers
    PHWREG  NV3_Lin_PRAMIN_Registers;    // Linear address to RAMIN Registers
    ULONG  NV3_PRAMIN_Length;            // NV3 register window

    PHYSICAL_ADDRESS NV3_Phys_PRMVIO_Registers;   // NV3 PRMVIO Registers
    PHWREG  NV3_Lin_PRMVIO_Registers;    // Linear address to PRMVIO Registers
    ULONG  NV3_PRMVIO_Length;            // NV3 register window

    PHYSICAL_ADDRESS NV3_Phys_PRMCIO_Registers;   // NV3 PRMCIO Registers
    PHWREG  NV3_Lin_PRMCIO_Registers;    // Linear address to PRMCIO Registers
    ULONG  NV3_PRMCIO_Length;            // NV3 register window

    PHYSICAL_ADDRESS NV3_Phys_PRAMDAC_Registers;   // NV3 PRAMDAC Registers
    PHWREG  NV3_Lin_PRAMDAC_Registers;    // Linear address to PRAMDAC Registers
    ULONG  NV3_PRAMDAC_Length;            // NV3 register window

    PHYSICAL_ADDRESS NV3_Phys_USERDAC_Registers;   // NV3 USERDAC Registers
    PHWREG  NV3_Lin_USERDAC_Registers;    // Linear address to USERDAC Registers
    ULONG  NV3_USERDAC_Length;            // NV3 register window

    PHYSICAL_ADDRESS NV3_Phys_PVIDEO_Registers;   // NV3 USERDAC Registers
    PHWREG  NV3_Lin_PVIDEO_Registers;     // Linear address to USERDAC Registers
    ULONG  NV3_PVIDEO_Length;            // NV3 register window

    PHYSICAL_ADDRESS PhysicalVideoMemoryBase; // physical memory address and
    ULONG PhysicalVideoMemoryLength;          // length of display memory
    PHYSICAL_ADDRESS PhysicalFrameBase;  // physical memory address and
    ULONG PhysicalFrameLength;           // length of display memory for the

    ULONG TextureDataLength;            //  Length of allocated texture data in Sys Mem

    ULONG DMABufferLength;            //  Length of allocated texture data in Sys Mem

    ULONG DMAPushLength;            // Size of DMA Push Buffer
    PVOID FrameBufferPtr;           // Get access to frame buffer

    //**************************************************************************
    // NV Software structures
    //**************************************************************************

    ULONG HashTable[HASH_DEPTH*256*2];   // Our local hash table
    ULONG ColorFormatTable[5];         // Color format conversion table
    GRAPHICSCHANNEL GrTable;           // Currently using 1 channel only
    FIFO FifoTable;                    // Currently using 1 channel only
    RENDEROBJECT RenderObject[15];     // Allocate up to 15 objects for now
    COMMONOBJECT CommonObject[10];     // Allocate up to 10 NON Render objects
    HWINFO NvInfo;                     // NVInfo structure information
    UCHAR CRTC[64];                    // Holds CRTC values
    UCHAR FreeInstanceUsage[NV_PRAM_MIN_SIZE_INSTANCE_MEM / 16];
                                       // Array denoting usage of FreeInstance mem
                                       // Each byte represents 1 paragraph of instance mem

    ULONG LastInstanceIndex;      //
    ULONG DisableVBlankFlag;      // Check for VBlank in OpenGL?
    PUSHORT BiosArea;             // address of the BIOS area
    USHORT OriginalBiosData;      // Orignal value in the Bios data area.

    BOOLEAN bLinearModeSupported; // Do we support linear modes?
    BOOLEAN bInLinearMode;        // Are we currently in a linear mode?
    ULONG ulSlot;                 // the slot that the card is in

    //**************************************************************************
    // Denotes if Flat panel is active or not
    //**************************************************************************

    BOOLEAN FlatPanelIsPresent;
    BOOLEAN TvIsPresent;
    BOOLEAN FlatPanelIsOff[NV_NO_DACS];

    //**************************************************************************
    // Max width and height of display (detected by RM)
    //**************************************************************************

    ULONG   MaxFlatPanelWidth;
    ULONG   MaxFlatPanelHeight;

    //**************************************************************************
    // Flag to denote if driver is being installed during unattended install
    //**************************************************************************

    BOOL    DeviceMappingFailed;
    
    //**************************************************************************
    // Save last DPMS state
    //**************************************************************************

    ULONG   CurrentDPMSState;
    
    //**************************************************************************
    // Used to save/restore power managment related registers for Win2k
    //**************************************************************************

    ULONG   Power_refresh;
    ULONG   Power_PfbBoot0;
    ULONG   Power_PfbConfig0;
    ULONG   Power_PfbConfig1;
    ULONG   Power_PfbMrs;
    ULONG   Power_PfbEmrs;
    ULONG   Power_PfbRef;
    ULONG   Power_PbusDebug1;
    ULONG   Power_PextdevBoot0;
    ULONG   Power_PfifoRamHT;
    ULONG   Power_PfifoRamRO;
    ULONG   Power_PfifoRamFC;    
    ULONG   Power_coeff_select;
    ULONG   Power_nvpll;
    ULONG   Power_vpll;
    ULONG   Power_mpll;

    ULONG   Master_Enable;
    ULONG   Fifo_Mode    ;
    ULONG   Numerator    ;
    ULONG   Denominator  ;
    ULONG   PCINV19      ;

    //
    // These 4 fields must be at the end of the device extension and must be
    // kept in this order since this data will be copied to and from the save
    // state buffer that is passed to and from the VDM.
    //

    ULONG TrappedValidatorCount;   // number of entries in the Trapped
                                   // validator data Array.
    VGA_VALIDATOR_DATA TrappedValidatorData[VGA_MAX_VALIDATOR_DATA];
                                   // Data trapped by the validator routines
                                   // but not yet played back into the VGA
                                   // register.

    ULONG SequencerAddressValue;   // Determines if the Sequencer Address Port
                                   // is currently selecting the SyncReset data
                                   // register.

    ULONG CurrentNumVdmAccessRanges;           // Number of access ranges in
                                               // the access range array pointed
                                               // to by the next field
    PVIDEO_ACCESS_RANGE CurrentVdmAccessRange; // Access range currently
                                               // associated to the VDM
    FLONG   flMapMemory;            // Flags passed to VideoPortMapMemory for mapping fb
    BOOLEAN bHwVidOvl;              // Is hw video overlay supported in the current mode?
    ULONG   bEnableIF09;             // Should we support IF09 format video data?
    ULONG   bUseDMT;                // Did a registry flag indicate we should use DMT
                                    // monitor timings?

    // MP/RM integration...
    PHWREG      NvRegisterBase;
    PHWREG      FrameBufferBase;
    ULONG       DeviceReference;

    ULONG       curPowerState;      // Last known power state of the device
    ULONG       ulRmTurnedOffMonitor;     // Last known value power state of monitor

#if _WIN32_WINNT < 0x0500
    ULONG       bExtRefreshDisable;     // DWORD is defined in INF file
#endif // #if _WIN32_WINNT < 0x0500
    ULONG       bMax16x12;
    ULONG       bDisable19x12x32at60;
    ULONG       bDisable19x10x32ge70;
    ULONG       bDisable16x12x32ge70;
    ULONG       bDellSwitch1;
    ULONG       OglStencilSwitch;
    ULONG       ulForceSwCursor;

    ULONG       Via694ChipsetFound;
    ULONG       AMD751ChipsetFound;
    USHORT      AMD751_AGP_Enable_And_Status_Register_Saved_Value_USHORT;
    ULONG       AMD751_AGP_GART_Base_Address_Register_Saved_Value_ULONG;
    PHWREG      AMD751_BAR1_Mapped_Address;
    ULONG       AliChipset_Problem_Found;
    PVOID       AliFixupIOBase;
    ULONG       LastWidth;
    ULONG       LastHeight;
    ULONG       LastDepth;
    ULONG       LastRefreshRate;

    ULONG       SavedPMCState;


    ULONG       BiosRevision;
    PUCHAR      pBios;
#if _WIN32_WINNT >= 0x0500
    BOOLEAN     bAgp;
    BOOLEAN     bDisableAgp;
    VIDEO_PORT_AGP_SERVICES AgpServices;
    PHYSICAL_ADDRESS AgpAddressRange;
    ULONG       AgpPagesTotal;
    PVOID       *AgpPhysContext;
    PVOID       dummyVirtualContext;
    PVOID       AgpHeapBase;
    PVOID       AgpVirtualBaseAddress;
    ULONGLONG   SystemMemorySize;
#endif // #if _WIN32_WINNT >= 0x0500

    // per-device PCI addresses
    VIDEO_ACCESS_RANGE NVAccessRanges[NV_PHYSICAL_ADDRESS_QTY];


// Added to support gamma clut support
   VIDEO_CLUT       GammaCache;
   VIDEO_CLUTDATA   GammaCacheData[NV_NO_DACS*256];    // Make it 256 just to be safe...

// Added to support GTF or DMT method selection
    ULONG   bUseGTF; // Did a registry flag indicate we should use GTF?
    // On the fly valid device mode work area
    USHORT ValidDeviceModeTable[MAX_VALID_MODE_TABLE_DATA_WORDS];

    // VPE integration
    ULONG       BusInterruptLevel;
    ULONG       BusInterruptVector;
    PVOID       IRQCallback;
    PVOID       pIRQData;
    ULONG       dwIRQSources;

#ifdef NVPE
    /////////// 
    // H.AZAR: (02/15/99)
    NVP_CONTEXT avpContexts[NVP_MAX_VPORTS];
    ULONG       dwVPEEnabled;       // TRUE Enabled/default, FALSE disabled (registry key)
    ///////////
#endif // NVPE

    // TwinView related fields with info about the dacs.
    //**************************************************************************
    // Saved EDID Buffer
    //**************************************************************************
    UCHAR EDIDBuffer[NV_NO_CONNECTORS][EDID_V2_SIZE];
    ULONG EDIDBufferValid[NV_NO_CONNECTORS]; // Whether this EDID buffer for this head is valid.
    ULONG EDIDBufferSize[NV_NO_CONNECTORS];
    //
    // The max pixelclock frequencies of each head as returned by ResMan.
    // Note: if CRT is connected for second head, only then should we
    // use the pixel clock for the second head. Otherwise, we use the pixel clock of
    // DAC 0 for both heads.
    //
    ULONG maxPixelClockFrequency_8bpp[NV_NO_DACS];
    ULONG maxPixelClockFrequency_16bpp[NV_NO_DACS];
    ULONG maxPixelClockFrequency_32bpp[NV_NO_DACS];

    //
    // We create a mapping array from Win2K's power state to NVidia's supported power states
    //
    ULONG NVMonitorPowerState[NV_NO_DACS][MAX_WIN2K_POWER_STATES];
    ULONG NVAdapterPowerState[MAX_WIN2K_POWER_STATES];
    
    NV_DESKTOP_INFO DeskTopInfo;         // Information about the virtual desktop
    // End TwinView related fields

#if (_WIN32_WINNT >= 0x0500)
    // macrovision
    ULONG ulCurrentMacrovisionKey;
    ULONG ulNextMacrovisionKey;
    ULONG ulMacrovisionMode;
#endif

} HW_DEVICE_EXTENSION, *PHW_DEVICE_EXTENSION;



//
// Function prototypes.
//

//
// Entry points for the VGA validator. Used in VgaEmulatorAccessEntries[].
//

VP_STATUS
VgaValidatorUcharEntry (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PUCHAR Data
    );

VP_STATUS
VgaValidatorUshortEntry (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PUSHORT Data
    );

VP_STATUS
VgaValidatorUlongEntry (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PULONG Data
    );

BOOLEAN
VgaPlaybackValidatorData (
    PVOID Context
    );

//
// Bank switch code start and end labels, define in HARDWARE.ASM
//

extern UCHAR BankSwitchStart;
extern UCHAR PlanarHCBankSwitchStart;
extern UCHAR EnablePlanarHCStart;
extern UCHAR DisablePlanarHCStart;
extern UCHAR BankSwitchEnd;

//
// Vga init scripts for font loading
//

extern USHORT EnableA000Data[];
extern USHORT DisableA000Color[];

extern MEMORYMAPS MemoryMaps[];

extern VIDEOMODE ModesVGA[];
extern ULONG NumVideoModes;

extern RANGE_OFFSETS RangeOffsets[NV_NUM_RANGES];

extern VIDEO_ACCESS_RANGE VgaAccessRange[];
extern VIDEO_ACCESS_RANGE VgaPaletteAccessRange;

#define VGA_NUM_EMULATOR_ACCESS_ENTRIES     6
extern EMULATOR_ACCESS_ENTRY VgaEmulatorAccessEntries[];

#define NUM_MINIMAL_VGA_VALIDATOR_ACCESS_RANGE 4
extern VIDEO_ACCESS_RANGE MinimalVgaValidatorAccessRange[];

#define NUM_FULL_VGA_VALIDATOR_ACCESS_RANGE 2
extern VIDEO_ACCESS_RANGE FullVgaValidatorAccessRange[];



//
// These are macros used to call the VideoPort AGP functions. Their
// purpose is to make the code more readable. VideoPortGetAGPServices
// MUST be called prior to invoking these macros.
//
#define VideoPortAgpReservePhysical(_HwDeviceExt, _Pages, _Caching, _PhysContext) \
(_HwDeviceExt)->AgpServices.AgpReservePhysical((_HwDeviceExt), (_Pages), \
                (_Caching), (_PhysContext))

#define VideoPortAgpReleasePhysical(_HwDeviceExt, _PhysContext) \
(_HwDeviceExt)->AgpServices.AgpReleasePhysical((_HwDeviceExt), (_PhysContext))

#define VideoPortAgpCommitPhysical(_HwDeviceExt, _PhysContext, _Pages, _Offset) \
(_HwDeviceExt)->AgpServices.AgpCommitPhysical((_HwDeviceExt), (_PhysContext), \
                _Pages, _Offset)

#define VideoPortAgpFreePhysical(_HwDeviceExt, _PhysContext, _Pages, _Offset) \
(_HwDeviceExt)->AgpServices.AgpFreePhysical((_HwDeviceExt), \
                (_PhysContext), (_Pages), (_Offset))

#define VideoPortAgpReserveVirtual(_HwDeviceExt, _Pid, _PhysContext, _VirtContext) \
(_HwDeviceExt)->AgpServices.AgpReserveVirtual((_HwDeviceExt), (_Pid), \
                (_PhysContext), (_VirtContext))

#define VideoPortAgpReleaseVirtual(_HwDeviceExt, _VirtContext) \
(_HwDeviceExt)->AgpServices.AgpReleaseVirtual((_HwDeviceExt), (_VirtContext))

#define VideoPortAgpCommitVirtual(_HwDeviceExt, _VirtContext, _Pages, _Offset) \
(_HwDeviceExt)->AgpServices.AgpCommitVirtual((_HwDeviceExt), (_VirtContext), \
                (_Pages), (_Offset))

#define VideoPortAgpFreeVirtual(_HwDeviceExt, _VirtContext, _Pages, _Offset) \
(_HwDeviceExt)->AgpServices.AgpFreeVirtual((_HwDeviceExt), (_VirtContext), \
                (_Pages), (_Offset))

//******************************************************************************
// Capabilities flags
//
// These are private flags passed to the NV display driver.  They
// come from the 'DriverSpecificAttributeFlags' field of the
// 'VIDEO_MODE_INFORMATION' structure (found in 'ntddvdeo.h') passed
// to us via an 'VIDEO_QUERY_AVAIL_MODES' or 'VIDEO_QUERY_CURRENT_MODE'
// IOCTL.
//
// NOTE: These definitions must match those in the display drivers 'driver.h'!
//******************************************************************************

typedef enum {
    CAPS_NV1_IS_PRESENT     = 0x00000010,   // NV1 Chip is present
    CAPS_NV3_IS_PRESENT     = 0x00000020,   // NV3 Chip is present
    CAPS_NV4_IS_PRESENT     = 0x00000040,   // NV4 Chip is present
    CAPS_NV5_IS_PRESENT     = 0x00000080,   // NV5 Chip is present
    CAPS_NV5VANTA_IS_PRESENT= 0x00000100,   // NV5 VANTA Chip is present
    CAPS_NV5ULTRA_IS_PRESENT= 0x00000200,   // NV5 ULTRA Chip is present
    CAPS_NV0A_IS_PRESENT    = 0x00000400,   // NV0A Chip is present
    CAPS_NV10_IS_PRESENT    = 0x00000800,   // NV10 Chip is present
  CAPS_NV5MODEL64_IS_PRESENT= 0x00001000,   // NV5 MODEL64 Chip is present
  CAPS_NV10DDR_IS_PRESENT   = 0x00002000,   // NV10 DDR Chip is present
  CAPS_NV10GL_IS_PRESENT    = 0x00004000,   // NV10 GL Chip is present
  CAPS_NV11_IS_PRESENT      = 0x00008000,   // NV11 Chip is present
  CAPS_NV11DDR_IS_PRESENT   = 0x00010000,   // NV11 DDR Chip is present
// defined at end of list:    CAPS_NV11M_IS_PRESENT   = 0x80000000,   // NV11M Chip is present
  CAPS_NV11GL_IS_PRESENT    = 0x00020000,   // NV11 GL Chip is present
  CAPS_NV15_IS_PRESENT      = 0x00040000,   // NV15 Chip is present
  CAPS_NV15DDR_IS_PRESENT   = 0x00080000,   // NV15 DDR Chip is present
  CAPS_NV15GL_IS_PRESENT    = 0x00100000,   // NV15 GL Chip is present
  CAPS_NV15BR_IS_PRESENT    = 0x00200000,   // NV15 BR Chip is present
  CAPS_NV20_IS_PRESENT      = 0x00400000,   // NV20 Chip is present
// not used    CAPS_TI025_POINTER      = 0x00800000,   // Use TI TVP3020/3025 pointer
// not used    CAPS_SCALE_POINTER      = 0x01000000,   // Set if the S3 hardware pointer
// not used                                            //   x position has to be scaled by
// not used                                            //   two
// not used    CAPS_SPARSE_SPACE       = 0x02000000,   // Frame buffer is mapped in sparse
// not used                                            //   space on the Alpha
// not used    CAPS_NEW_BANK_CONTROL   = 0x04000000,   // Set if 801/805/928 style banking
// not used    CAPS_NEWER_BANK_CONTROL = 0x08000000,   // Set if 864/964 style banking
// not used    CAPS_RE_REALIZE_PATTERN = 0x10000000,   // Set if we have to work around the
// not used                                            //   864/964 hardware pattern bug
// not used    CAPS_SLOW_MONO_EXPANDS  = 0x20000000,   // Set if we have to slow down
// not used                                            //   monochrome expansions
    CAPS_SW_POINTER         = 0x40000000,   // No hardware pointer; use software simulation
    CAPS_NV11M_IS_PRESENT   = 0x80000000,   // NV11M Chip is present


} CAPS;

//**********************************************************************************
// Defines for VESA power management function.
//**********************************************************************************
#define VESA_POWER_FUNCTION 0x4f10
#define VESA_POWER_ON       0x0000
#define VESA_POWER_STANDBY  0x0100
#define VESA_POWER_SUSPEND  0x0200
#define VESA_POWER_OFF      0x0400
#define VESA_GET_POWER_FUNC 0x0000
#define VESA_SET_POWER_FUNC 0x0001
#define VESA_STATUS_SUCCESS 0x004f

#if DBG
VOID DbgBreakPoint(VOID);
#define NvDbgBreak() DbgBreakPoint();
#else
#define NvDbgBreak()
#endif

//******************************************************************************
// External declarations.
//******************************************************************************
BOOLEAN NVGetNVInfo(PHW_DEVICE_EXTENSION);
//BOOLEAN NVIsPresent(PHW_DEVICE_EXTENSION,PVIDEO_ACCESS_RANGE,PULONG);
BOOLEAN NVIsPresent(PHW_DEVICE_EXTENSION,PULONG);
ULONG NVMapMemoryRanges(PVOID);
ULONG NVMapFrameBuffer(PVOID HwDeviceExtension);
ULONG NVMapVgaPaletteRegisters(PHW_DEVICE_EXTENSION);
VOID NVGetMonitorType(PHW_DEVICE_EXTENSION HwDeviceExtension);
VOID NV_SetColorLookup(PHW_DEVICE_EXTENSION,PVIDEO_CLUT,ULONG);
VOID NVSetBiosInfo(PHW_DEVICE_EXTENSION);
VOID wtoa(USHORT, PUCHAR, UCHAR, UCHAR);

#if _WIN32_WINNT < 0x0500
ULONG wcslen(PWCHAR pString);
#endif // #if _WIN32_WINNT < 0x0500

#if _WIN32_WINNT >= 0x0500
VOID NV_SetPixMixBits(PHW_DEVICE_EXTENSION);
#endif
