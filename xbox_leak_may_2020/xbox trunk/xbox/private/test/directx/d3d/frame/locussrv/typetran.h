/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    server.h

Author:

    Matt Bronder

Description:

    Server code for remote reference verification.

*******************************************************************************/

#ifndef __TYPETRAN_H__
#define __TYPETRAN_H__

//***********************************************************************************
// Preprocessor definitions
//***********************************************************************************

#define NUM_STATIC_TRANSLATIONS      33

#define XBOX_D3DCLEAR_TARGET         0x000000f0l  /* Clear target surface */
#define XBOX_D3DCLEAR_ZBUFFER        0x00000001l  /* Clear target z buffer */
#define XBOX_D3DCLEAR_STENCIL        0x00000002l  /* Clear stencil planes */

#define XBOX_D3DWRAPCOORD_0          0x00000010L    // same as D3DWRAP_U
#define XBOX_D3DWRAPCOORD_1          0x00001000L    // same as D3DWRAP_V
#define XBOX_D3DWRAPCOORD_2          0x00100000L    // same as D3DWRAP_W
#define XBOX_D3DWRAPCOORD_3          0x01000000L

#define XBOX_D3DLOCK_TILED           0x00000040L // Xbox extension
#define XBOX_D3DLOCK_NOFLUSH         0x00000010L // Xbox extension
#define XBOX_D3DLOCK_FLAGS           (XBOX_D3DLOCK_TILED | XBOX_D3DLOCK_NOFLUSH)

//***********************************************************************************
// Data types
//***********************************************************************************

typedef enum _D3DTYPEINDEX {
    D3DTI_D3DBACKBUFFER_TYPE       =  0,
    D3DTI_D3DBLEND                 =  1,
    D3DTI_D3DBLENDOP               =  2,
    D3DTI_D3DCMPFUNC               =  3,
    D3DTI_D3DCUBEMAP_FACES         =  4,
    D3DTI_D3DCULL                  =  5,
    D3DTI_D3DDEBUGMONITORTOKENS    =  6,
    D3DTI_D3DDEVTYPE               =  7,
    D3DTI_D3DFILLMODE              =  8,
    D3DTI_D3DFOGMODE               =  9,
    D3DTI_D3DFORMAT                = 10,
    D3DTI_D3DBASISTYPE             = 11,
    D3DTI_D3DLIGHTTYPE             = 12,
    D3DTI_D3DMATERIALCOLORSOURCE   = 13,
    D3DTI_D3DMULTISAMPLE_TYPE      = 14,
    D3DTI_D3DORDERTYPE             = 15,
    D3DTI_D3DPATCHEDGESTYLE        = 16,
    D3DTI_D3DPOOL                  = 17,
    D3DTI_D3DPRIMITIVETYPE         = 18,
    D3DTI_D3DRENDERSTATETYPE       = 19,
    D3DTI_D3DRESOURCETYPE          = 20,
    D3DTI_D3DSHADEMODE             = 21,
    D3DTI_D3DSTATEBLOCKTYPE        = 22,
    D3DTI_D3DSTENCILOP             = 23,
    D3DTI_D3DSWAPEFFECT            = 24,
    D3DTI_D3DTEXTUREADDRESS        = 25,
    D3DTI_D3DTEXTUREFILTERTYPE     = 26,
    D3DTI_D3DTEXTUREOP             = 27,
    D3DTI_D3DTEXTURESTAGESTATETYPE = 28,
    D3DTI_D3DTEXTURETRANSFORMFLAGS = 29,
    D3DTI_D3DTRANSFORMSTATETYPE    = 30,
    D3DTI_D3DVERTEXBLENDFLAGS      = 31,
    D3DTI_D3DZBUFFERTYPE           = 32,
    D3DTI_FORCE_DWORD              = 0x7FFFFFFF
} D3DTYPEINDEX;

typedef enum _XBOX_D3DBACKBUFFER_TYPE
{
    XBOX_D3DBACKBUFFER_TYPE_MONO         = 0,
    XBOX_D3DBACKBUFFER_TYPE_LEFT         = 1,
    XBOX_D3DBACKBUFFER_TYPE_RIGHT        = 2,

    XBOX_D3DBACKBUFFER_TYPE_FORCE_DWORD  = 0x7fffffff
} XBOX_D3DBACKBUFFER_TYPE;

typedef enum _XBOX_D3DBLEND {
    XBOX_D3DBLEND_ZERO               = 0,
    XBOX_D3DBLEND_ONE                = 1,
    XBOX_D3DBLEND_SRCCOLOR           = 0x300,
    XBOX_D3DBLEND_INVSRCCOLOR        = 0x301,
    XBOX_D3DBLEND_SRCALPHA           = 0x302,
    XBOX_D3DBLEND_INVSRCALPHA        = 0x303,
    XBOX_D3DBLEND_DESTALPHA          = 0x304,
    XBOX_D3DBLEND_INVDESTALPHA       = 0x305,
    XBOX_D3DBLEND_DESTCOLOR          = 0x306,
    XBOX_D3DBLEND_INVDESTCOLOR       = 0x307,
    XBOX_D3DBLEND_SRCALPHASAT        = 0x308,
    XBOX_D3DBLEND_BOTHSRCALPHA       = 0x302,
    XBOX_D3DBLEND_BOTHINVSRCALPHA    = 0x303,
    XBOX_D3DBLEND_CONSTANTCOLOR      = 0x8001,
    XBOX_D3DBLEND_INVCONSTANTCOLOR   = 0x8002,
    XBOX_D3DBLEND_CONSTANTALPHA      = 0x8003,
    XBOX_D3DBLEND_INVCONSTANTALPHA   = 0x8004,
    XBOX_D3DBLEND_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DBLEND;

typedef enum _XBOX_D3DBLENDOP {
    XBOX_D3DBLENDOP_ADD              = 0x8006,
    XBOX_D3DBLENDOP_SUBTRACT         = 0x800a,
    XBOX_D3DBLENDOP_REVSUBTRACT      = 0x800b,
    XBOX_D3DBLENDOP_MIN              = 0x8007,
    XBOX_D3DBLENDOP_MAX              = 0x8008,
    XBOX_D3DBLENDOP_FORCE_DWORD      = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DBLENDOP;

typedef enum _XBOX_D3DCMPFUNC {
    XBOX_D3DCMP_NEVER                = 0x200,
    XBOX_D3DCMP_LESS                 = 0x201,
    XBOX_D3DCMP_EQUAL                = 0x202,
    XBOX_D3DCMP_LESSEQUAL            = 0x203,
    XBOX_D3DCMP_GREATER              = 0x204,
    XBOX_D3DCMP_NOTEQUAL             = 0x205,
    XBOX_D3DCMP_GREATEREQUAL         = 0x206,
    XBOX_D3DCMP_ALWAYS               = 0x207,
    XBOX_D3DCMP_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DCMPFUNC;

typedef enum _XBOX_D3DCUBEMAP_FACES
{
    XBOX_D3DCUBEMAP_FACE_POSITIVE_X     = 0,
    XBOX_D3DCUBEMAP_FACE_NEGATIVE_X     = 1,
    XBOX_D3DCUBEMAP_FACE_POSITIVE_Y     = 2,
    XBOX_D3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
    XBOX_D3DCUBEMAP_FACE_POSITIVE_Z     = 4,
    XBOX_D3DCUBEMAP_FACE_NEGATIVE_Z     = 5,

    XBOX_D3DCUBEMAP_FACE_FORCE_DWORD    = 0x7fffffff
} XBOX_D3DCUBEMAP_FACES;

typedef enum _XBOX_D3DCULL {
    XBOX_D3DCULL_NONE                = 0,
    XBOX_D3DCULL_CW                  = 0x900,
    XBOX_D3DCULL_CCW                 = 0x901,
    XBOX_D3DCULL_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DCULL;

typedef enum _XBOX_D3DDEBUGMONITORTOKENS {
    XBOX_D3DDMT_ENABLE            = 0,    // enable debug monitor
    XBOX_D3DDMT_DISABLE           = 1,    // disable debug monitor
    XBOX_D3DDMT_FORCE_DWORD     = 0x7fffffff,
} XBOX_D3DDEBUGMONITORTOKENS;

typedef enum _XBOX_D3DDEVTYPE
{
    XBOX_D3DDEVTYPE_HAL         = 1,
    XBOX_D3DDEVTYPE_REF         = 2,
    XBOX_D3DDEVTYPE_SW          = 3,

    XBOX_D3DDEVTYPE_FORCE_DWORD  = 0x7fffffff
} XBOX_D3DDEVTYPE;

typedef enum _XBOX_D3DFILLMODE {
    XBOX_D3DFILL_POINT               = 0x1b00,
    XBOX_D3DFILL_WIREFRAME           = 0x1b01,
    XBOX_D3DFILL_SOLID               = 0x1b02,
    XBOX_D3DFILL_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DFILLMODE;

typedef enum _XBOX_D3DFOGMODE {
    XBOX_D3DFOG_NONE                 = 0,
    XBOX_D3DFOG_EXP                  = 1,
    XBOX_D3DFOG_EXP2                 = 2,
    XBOX_D3DFOG_LINEAR               = 3,
    XBOX_D3DFOG_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DFOGMODE;

typedef enum _XBOX_D3DFORMAT
{
    XBOX_D3DFMT_UNKNOWN              = 0xFFFFFFFF,

    /* Swizzled formats */

    XBOX_D3DFMT_A8R8G8B8             = 0x00000006,
    XBOX_D3DFMT_X8R8G8B8             = 0x00000007,
    XBOX_D3DFMT_R5G6B5               = 0x00000005,
    XBOX_D3DFMT_X1R5G5B5             = 0x00000003,
    XBOX_D3DFMT_A1R5G5B5             = 0x00000002,
    XBOX_D3DFMT_A4R4G4B4             = 0x00000004,
    XBOX_D3DFMT_A8                   = 0x00000019,
    XBOX_D3DFMT_A8B8G8R8             = 0x0000003A,   
    XBOX_D3DFMT_B8G8R8A8             = 0x0000003B,   
    XBOX_D3DFMT_R4G4B4A4             = 0x00000039,   
    XBOX_D3DFMT_R5G5B5A1             = 0x00000038,   
    XBOX_D3DFMT_R8G8B8A8             = 0x0000003C,   
    XBOX_D3DFMT_R8B8                 = 0x00000029,   
    XBOX_D3DFMT_G8B8                 = 0x00000028,   

    XBOX_D3DFMT_P8                   = 0x0000000B,

    XBOX_D3DFMT_L8                   = 0x00000000,
    XBOX_D3DFMT_A8L8                 = 0x0000001A,
    XBOX_D3DFMT_AL8                  = 0x00000001,   
    XBOX_D3DFMT_L16                  = 0x00000032,   

    XBOX_D3DFMT_V8U8                 = 0x00000028,
    XBOX_D3DFMT_L6V5U5               = 0x00000027,
    XBOX_D3DFMT_X8L8V8U8             = 0x00000007,
    XBOX_D3DFMT_Q8W8V8U8             = 0x0000003A,
    XBOX_D3DFMT_V16U16               = 0x00000033,

    XBOX_D3DFMT_D16_LOCKABLE         = 0x0000002C,
    XBOX_D3DFMT_D16                  = 0x0000002C,
    XBOX_D3DFMT_D24S8                = 0x0000002A,
    XBOX_D3DFMT_F16                  = 0x0000002D,   
    XBOX_D3DFMT_F24S8                = 0x0000002B,

    /* YUV formats */

    XBOX_D3DFMT_UYVY                 = 0x00000024,
    XBOX_D3DFMT_YUY2                 = 0x00000025,

    /* Compressed formats */

    XBOX_D3DFMT_DXT1                 = 0x0000000C,
    XBOX_D3DFMT_DXT2                 = 0x0000000E,
    XBOX_D3DFMT_DXT3                 = 0x0000000E,
    XBOX_D3DFMT_DXT4                 = 0x0000000F,
    XBOX_D3DFMT_DXT5                 = 0x0000000F,

    /* Linear formats */

    XBOX_D3DFMT_LIN_A1R5G5B5         = 0x00000010,   
    XBOX_D3DFMT_LIN_A4R4G4B4         = 0x0000001D,   
    XBOX_D3DFMT_LIN_A8               = 0x0000001F,   
    XBOX_D3DFMT_LIN_A8B8G8R8         = 0x0000003F,   
    XBOX_D3DFMT_LIN_A8R8G8B8         = 0x00000012,   
    XBOX_D3DFMT_LIN_B8G8R8A8         = 0x00000040,   
    XBOX_D3DFMT_LIN_G8B8             = 0x00000017,   
    XBOX_D3DFMT_LIN_R4G4B4A4         = 0x0000003E,   
    XBOX_D3DFMT_LIN_R5G5B5A1         = 0x0000003D,   
    XBOX_D3DFMT_LIN_R5G6B5           = 0x00000011,   
    XBOX_D3DFMT_LIN_R6G5B5           = 0x00000037,   
    XBOX_D3DFMT_LIN_R8B8             = 0x00000016,   
    XBOX_D3DFMT_LIN_R8G8B8A8         = 0x00000041,   
    XBOX_D3DFMT_LIN_X1R5G5B5         = 0x0000001C,   
    XBOX_D3DFMT_LIN_X8R8G8B8         = 0x0000001E,   

    XBOX_D3DFMT_LIN_A8L8             = 0x00000020,   
    XBOX_D3DFMT_LIN_AL8              = 0x0000001B,   
    XBOX_D3DFMT_LIN_L16              = 0x00000035,   
    XBOX_D3DFMT_LIN_L8               = 0x00000013,   

    XBOX_D3DFMT_LIN_V16U16           = 0x00000036,
    XBOX_D3DFMT_LIN_V8U8             = 0x00000017,
    XBOX_D3DFMT_LIN_L6V5U5           = 0x00000037,
    XBOX_D3DFMT_LIN_X8L8V8U8         = 0x0000001E,
    XBOX_D3DFMT_LIN_Q8W8V8U8         = 0x00000012,

    XBOX_D3DFMT_LIN_D24S8            = 0x0000002E,   
    XBOX_D3DFMT_LIN_F24S8            = 0x0000002F,   
    XBOX_D3DFMT_LIN_D16              = 0x00000030,   
    XBOX_D3DFMT_LIN_F16              = 0x00000031,   

    XBOX_D3DFMT_VERTEXDATA           = 100,
    XBOX_D3DFMT_INDEX16              = 101,
    XBOX_D3DFMT_INDEX32              = 102,

    XBOX_D3DFMT_FORCE_DWORD          =0x7fffffff

} XBOX_D3DFORMAT;

typedef enum _XBOX_D3DBASISTYPE
{
   XBOX_D3DBASIS_BEZIER      = 0,
   XBOX_D3DBASIS_BSPLINE     = 1,
   XBOX_D3DBASIS_INTERPOLATE = 2,
   XBOX_D3DBASIS_FORCE_DWORD = 0x7fffffff,
} XBOX_D3DBASISTYPE;

typedef enum _XBOX_D3DLIGHTTYPE {
    XBOX_D3DLIGHT_POINT          = 1,
    XBOX_D3DLIGHT_SPOT           = 2,
    XBOX_D3DLIGHT_DIRECTIONAL    = 3,
    XBOX_D3DLIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DLIGHTTYPE;

typedef enum _XBOX_D3DMATERIALCOLORSOURCE
{
    XBOX_D3DMCS_MATERIAL         = 0,            // Color from material is used
    XBOX_D3DMCS_COLOR1           = 1,            // Diffuse vertex color is used
    XBOX_D3DMCS_COLOR2           = 2,            // Specular vertex color is used
    XBOX_D3DMCS_FORCE_DWORD      = 0x7fffffff,   // force 32-bit size enum
} XBOX_D3DMATERIALCOLORSOURCE;

typedef enum _XBOX_D3DMULTISAMPLE_TYPE
{
    XBOX_D3DMULTISAMPLE_NONE            =  0x0011,
    XBOX_D3DMULTISAMPLE_2_SAMPLES       =  0x1121,
    XBOX_D3DMULTISAMPLE_3_SAMPLES       =  3,
    XBOX_D3DMULTISAMPLE_4_SAMPLES       =  0x1222,
    XBOX_D3DMULTISAMPLE_5_SAMPLES       =  5,
    XBOX_D3DMULTISAMPLE_6_SAMPLES       =  6,
    XBOX_D3DMULTISAMPLE_7_SAMPLES       =  7,
    XBOX_D3DMULTISAMPLE_8_SAMPLES       =  8,
    XBOX_D3DMULTISAMPLE_9_SAMPLES       =  0x2233,
    XBOX_D3DMULTISAMPLE_10_SAMPLES      = 10,
    XBOX_D3DMULTISAMPLE_11_SAMPLES      = 11,
    XBOX_D3DMULTISAMPLE_12_SAMPLES      = 12,
    XBOX_D3DMULTISAMPLE_13_SAMPLES      = 13,
    XBOX_D3DMULTISAMPLE_14_SAMPLES      = 14,
    XBOX_D3DMULTISAMPLE_15_SAMPLES      = 15,
    XBOX_D3DMULTISAMPLE_16_SAMPLES      = 16,

    XBOX_D3DMULTISAMPLE_FORCE_DWORD     = 0x7fffffff
} XBOX_D3DMULTISAMPLE_TYPE;

typedef enum _XBOX_D3DORDERTYPE
{
   XBOX_D3DORDER_LINEAR      = 1,
   XBOX_D3DORDER_CUBIC       = 3,
   XBOX_D3DORDER_QUINTIC     = 5,
   XBOX_D3DORDER_FORCE_DWORD = 0x7fffffff,
} XBOX_D3DORDERTYPE;

typedef enum _XBOX_D3DPATCHEDGESTYLE
{
   XBOX_D3DPATCHEDGE_DISCRETE    = 0,
   XBOX_D3DPATCHEDGE_CONTINUOUS  = 1,
   XBOX_D3DPATCHEDGE_FORCE_DWORD = 0x7fffffff,
} XBOX_D3DPATCHEDGESTYLE;

typedef enum _XBOX_D3DPRIMITIVETYPE {
    XBOX_D3DPT_POINTLIST             = 1,
    XBOX_D3DPT_LINELIST              = 2,
    XBOX_D3DPT_LINELOOP              = 3,  // Xbox extension
    XBOX_D3DPT_LINESTRIP             = 4,
    XBOX_D3DPT_TRIANGLELIST          = 5,
    XBOX_D3DPT_TRIANGLESTRIP         = 6,
    XBOX_D3DPT_TRIANGLEFAN           = 7,
    XBOX_D3DPT_QUADLIST              = 8,  // Xbox extension
    XBOX_D3DPT_QUADSTRIP             = 9,  // Xbox extension
    XBOX_D3DPT_POLYGON               = 10, // Xbox extension

    XBOX_D3DPT_MAX                   = 11,
    XBOX_D3DPT_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DPRIMITIVETYPE;

typedef enum _XBOX_D3DRENDERSTATETYPE {

    // Simple render states that are processed by D3D immediately:

    XBOX_D3DRS_PSALPHAINPUTS0            = 0,    // Pixel shader, Stage 0 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS1            = 1,    // Pixel shader, Stage 1 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS2            = 2,    // Pixel shader, Stage 2 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS3            = 3,    // Pixel shader, Stage 3 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS4            = 4,    // Pixel shader, Stage 4 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS5            = 5,    // Pixel shader, Stage 5 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS6            = 6,    // Pixel shader, Stage 6 alpha inputs                         
    XBOX_D3DRS_PSALPHAINPUTS7            = 7,    // Pixel shader, Stage 7 alpha inputs                         
    XBOX_D3DRS_PSFINALCOMBINERINPUTSABCD = 8,    // Pixel shader, Final combiner inputs ABCD
    XBOX_D3DRS_PSFINALCOMBINERINPUTSEFG  = 9,    // Pixel shader, Final combiner inputs EFG
    XBOX_D3DRS_PSCONSTANT0_0             = 10,   // Pixel shader, C0 in stage 0
    XBOX_D3DRS_PSCONSTANT0_1             = 11,   // Pixel shader, C0 in stage 1
    XBOX_D3DRS_PSCONSTANT0_2             = 12,   // Pixel shader, C0 in stage 2
    XBOX_D3DRS_PSCONSTANT0_3             = 13,   // Pixel shader, C0 in stage 3
    XBOX_D3DRS_PSCONSTANT0_4             = 14,   // Pixel shader, C0 in stage 4
    XBOX_D3DRS_PSCONSTANT0_5             = 15,   // Pixel shader, C0 in stage 5
    XBOX_D3DRS_PSCONSTANT0_6             = 16,   // Pixel shader, C0 in stage 6
    XBOX_D3DRS_PSCONSTANT0_7             = 17,   // Pixel shader, C0 in stage 7
    XBOX_D3DRS_PSCONSTANT1_0             = 18,   // Pixel shader, C1 in stage 0
    XBOX_D3DRS_PSCONSTANT1_1             = 19,   // Pixel shader, C1 in stage 1
    XBOX_D3DRS_PSCONSTANT1_2             = 20,   // Pixel shader, C1 in stage 2
    XBOX_D3DRS_PSCONSTANT1_3             = 21,   // Pixel shader, C1 in stage 3
    XBOX_D3DRS_PSCONSTANT1_4             = 22,   // Pixel shader, C1 in stage 4
    XBOX_D3DRS_PSCONSTANT1_5             = 23,   // Pixel shader, C1 in stage 5
    XBOX_D3DRS_PSCONSTANT1_6             = 24,   // Pixel shader, C1 in stage 6
    XBOX_D3DRS_PSCONSTANT1_7             = 25,   // Pixel shader, C1 in stage 7
    XBOX_D3DRS_PSALPHAOUTPUTS0           = 26,   // Pixel shader, Stage 0 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS1           = 27,   // Pixel shader, Stage 1 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS2           = 28,   // Pixel shader, Stage 2 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS3           = 29,   // Pixel shader, Stage 3 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS4           = 30,   // Pixel shader, Stage 4 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS5           = 31,   // Pixel shader, Stage 5 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS6           = 32,   // Pixel shader, Stage 6 alpha outputs                        
    XBOX_D3DRS_PSALPHAOUTPUTS7           = 33,   // Pixel shader, Stage 7 alpha outputs                        
    XBOX_D3DRS_PSRGBINPUTS0              = 34,   // Pixel shader, Stage 0 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS1              = 35,   // Pixel shader, Stage 1 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS2              = 36,   // Pixel shader, Stage 2 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS3              = 37,   // Pixel shader, Stage 3 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS4              = 38,   // Pixel shader, Stage 4 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS5              = 39,   // Pixel shader, Stage 5 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS6              = 40,   // Pixel shader, Stage 6 RGB inputs                           
    XBOX_D3DRS_PSRGBINPUTS7              = 41,   // Pixel shader, Stage 7 RGB inputs                           
    XBOX_D3DRS_PSCOMPAREMODE             = 42,   // Pixel shader, Compare modes for clipplane texture mode     
    XBOX_D3DRS_PSFINALCOMBINERCONSTANT0  = 43,   // Pixel shader, C0 in final combiner
    XBOX_D3DRS_PSFINALCOMBINERCONSTANT1  = 44,   // Pixel shader, C1 in final combiner
    XBOX_D3DRS_PSRGBOUTPUTS0             = 45,   // Pixel shader, Stage 0 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS1             = 46,   // Pixel shader, Stage 1 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS2             = 47,   // Pixel shader, Stage 2 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS3             = 48,   // Pixel shader, Stage 3 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS4             = 49,   // Pixel shader, Stage 4 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS5             = 50,   // Pixel shader, Stage 5 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS6             = 51,   // Pixel shader, Stage 6 RGB outputs                          
    XBOX_D3DRS_PSRGBOUTPUTS7             = 52,   // Pixel shader, Stage 7 RGB outputs                          
    XBOX_D3DRS_PSCOMBINERCOUNT           = 53,   // Pixel shader, Active combiner count (Stages 0-7)           
                                            // Pixel shader, Reserved
    XBOX_D3DRS_PSDOTMAPPING              = 55,   // Pixel shader, Input mapping for dot product modes          
    XBOX_D3DRS_PSINPUTTEXTURE            = 56,   // Pixel shader, Texture source for some texture modes        

    XBOX_D3DRS_ZFUNC                     = 57,   // D3DCMPFUNC 
    XBOX_D3DRS_ALPHAFUNC                 = 58,   // D3DCMPFUNC 
    XBOX_D3DRS_ALPHABLENDENABLE          = 59,   // TRUE to enable alpha blending 
    XBOX_D3DRS_ALPHATESTENABLE           = 60,   // TRUE to enable alpha tests 
    XBOX_D3DRS_ALPHAREF                  = 61,   // BYTE
    XBOX_D3DRS_SRCBLEND                  = 62,   // D3DBLEND 
    XBOX_D3DRS_DESTBLEND                 = 63,   // D3DBLEND 
    XBOX_D3DRS_ZWRITEENABLE              = 64,   // TRUE to enable z writes 
    XBOX_D3DRS_DITHERENABLE              = 65,   // TRUE to enable dithering 
    XBOX_D3DRS_SHADEMODE                 = 66,   // D3DSHADEMODE 
    XBOX_D3DRS_COLORWRITEENABLE          = 67,   // D3DCOLORWRITEENABLE_ALPHA, etc. per-channel write enable
    XBOX_D3DRS_STENCILZFAIL              = 68,   // D3DSTENCILOP to do if stencil test passes and Z test fails 
    XBOX_D3DRS_STENCILPASS               = 69,   // D3DSTENCILOP to do if both stencil and Z tests pass 
    XBOX_D3DRS_STENCILFUNC               = 70,   // D3DCMPFUNC
    XBOX_D3DRS_STENCILREF                = 71,   // BYTE reference value used in stencil test 
    XBOX_D3DRS_STENCILMASK               = 72,   // BYTE mask value used in stencil test 
    XBOX_D3DRS_STENCILWRITEMASK          = 73,   // BYTE write mask applied to values written to stencil buffer 
    XBOX_D3DRS_BLENDOP                   = 74,   // D3DBLENDOP setting
    XBOX_D3DRS_BLENDCOLOR                = 75,   // D3DCOLOR for D3DBLEND_CONSTANT, etc. (Xbox extension)
    XBOX_D3DRS_SWATHWIDTH                = 76,   // D3DSWATHWIDTH (Xbox extension)
    XBOX_D3DRS_POLYGONOFFSETZSLOPESCALE  = 77,   // float Z factor for shadow maps (Xbox extension)
    XBOX_D3DRS_POLYGONOFFSETZOFFSET      = 78,   // float bias for polygon offset (Xbox extension)
    XBOX_D3DRS_POINTOFFSETENABLE         = 79,   // TRUE to enable polygon offset for points (Xbox extension)
    XBOX_D3DRS_WIREFRAMEOFFSETENABLE     = 80,   // TRUE to enable polygon offset for lines (Xbox extension)
    XBOX_D3DRS_SOLIDOFFSETENABLE         = 81,   // TRUE to enable polygon offset for fills (Xbox extension)

    XBOX_D3DRS_SIMPLE_MAX                = 82,

    // State whose handling is deferred until the next Draw[Indexed]Vertices
    // call because of interdependencies on other states:

    XBOX_D3DRS_FOGENABLE                 = 82,   // TRUE to enable fog blending 
    XBOX_D3DRS_FOGTABLEMODE              = 83,   // D3DFOGMODE 
    XBOX_D3DRS_FOGSTART                  = 84,   // float fog start (for both vertex and pixel fog) 
    XBOX_D3DRS_FOGEND                    = 85,   // float fog end      
    XBOX_D3DRS_FOGDENSITY                = 86,   // float fog density  
    XBOX_D3DRS_RANGEFOGENABLE            = 87,   // TRUE to enable range-based fog 
    XBOX_D3DRS_WRAP0                     = 88,   // DWORD wrap for 1st texture coord. set 
    XBOX_D3DRS_WRAP1                     = 89,   // DWORD wrap for 2nd texture coord. set 
    XBOX_D3DRS_WRAP2                     = 90,   // DWORD wrap for 3rd texture coord. set 
    XBOX_D3DRS_WRAP3                     = 91,   // DWORD wrap for 4th texture coord. set 
    XBOX_D3DRS_LIGHTING                  = 92,   // TRUE to enable lighting
    XBOX_D3DRS_SPECULARENABLE            = 93,   // TRUE to enable specular 
    XBOX_D3DRS_LOCALVIEWER               = 94,   // TRUE to enable camera-relative specular highlights
    XBOX_D3DRS_COLORVERTEX               = 95,   // TRUE to enable per-vertex color
    XBOX_D3DRS_BACKSPECULARMATERIALSOURCE= 96,  // D3DMATERIALCOLORSOURCE (Xbox extension)
    XBOX_D3DRS_BACKDIFFUSEMATERIALSOURCE = 97,  // D3DMATERIALCOLORSOURCE (Xbox extension)
    XBOX_D3DRS_BACKAMBIENTMATERIALSOURCE = 98,  // D3DMATERIALCOLORSOURCE (Xbox extension)
    XBOX_D3DRS_BACKEMISSIVEMATERIALSOURCE= 99,  // D3DMATERIALCOLORSOURCE (Xbox extension)
    XBOX_D3DRS_SPECULARMATERIALSOURCE    = 100,  // D3DMATERIALCOLORSOURCE 
    XBOX_D3DRS_DIFFUSEMATERIALSOURCE     = 101,  // D3DMATERIALCOLORSOURCE 
    XBOX_D3DRS_AMBIENTMATERIALSOURCE     = 102,  // D3DMATERIALCOLORSOURCE 
    XBOX_D3DRS_EMISSIVEMATERIALSOURCE    = 103,  // D3DMATERIALCOLORSOURCE 
    XBOX_D3DRS_BACKAMBIENT               = 104,  // D3DCOLOR (Xbox extension)
    XBOX_D3DRS_AMBIENT                   = 105,  // D3DCOLOR 
    XBOX_D3DRS_POINTSIZE                 = 106,  // float point size 
    XBOX_D3DRS_POINTSIZE_MIN             = 107,  // float point size min threshold 
    XBOX_D3DRS_POINTSPRITEENABLE         = 108,  // TRUE to enable point sprites
    XBOX_D3DRS_POINTSCALEENABLE          = 109,  // TRUE to enable point size scaling
    XBOX_D3DRS_POINTSCALE_A              = 110,  // float point attenuation A value 
    XBOX_D3DRS_POINTSCALE_B              = 111,  // float point attenuation B value 
    XBOX_D3DRS_POINTSCALE_C              = 112,  // float point attenuation C value 
    XBOX_D3DRS_POINTSIZE_MAX             = 113,  // float point size max threshold 
    XBOX_D3DRS_PATCHEDGESTYLE            = 114,  // D3DPATCHEDGESTYLE
    XBOX_D3DRS_PATCHSEGMENTS             = 115,  // DWORD number of segments per edge when drawing patches

    XBOX_D3DRS_DEFERRED_MAX              = 116,

    // Complex state that has immediate processing:

    XBOX_D3DRS_PSTEXTUREMODES            = 116,  // Pixel shader, Texture addressing modes                     
    XBOX_D3DRS_VERTEXBLEND               = 117,   // D3DVERTEXBLENDFLAGS
    XBOX_D3DRS_FOGCOLOR                  = 118,  // D3DCOLOR 
    XBOX_D3DRS_FILLMODE                  = 119,  // D3DFILLMODE        
    XBOX_D3DRS_BACKFILLMODE              = 120,  // D3DFILLMODE (Xbox extension)
    XBOX_D3DRS_TWOSIDEDLIGHTING          = 121,  // TRUE to enable two-sided lighting (Xbox extension)
    XBOX_D3DRS_NORMALIZENORMALS          = 122,  // TRUE to enable automatic normalization
    XBOX_D3DRS_ZENABLE                   = 123,  // D3DZBUFFERTYPE (or TRUE/FALSE for legacy) 
    XBOX_D3DRS_STENCILENABLE             = 124,  // TRUE to enable stenciling
    XBOX_D3DRS_STENCILFAIL               = 125,  // D3DSTENCILOP to do if stencil test fails 
    XBOX_D3DRS_FRONTFACE                 = 126,  // D3DFRONT (Xbox extension)
    XBOX_D3DRS_CULLMODE                  = 127,  // D3DCULL 
    XBOX_D3DRS_TEXTUREFACTOR             = 128,  // D3DCOLOR used for multi-texture blend 
    XBOX_D3DRS_ZBIAS                     = 129,  // LONG Z bias 
    XBOX_D3DRS_LOGICOP                   = 130,  // D3DLOGICOP
    XBOX_D3DRS_EDGEANTIALIAS             = 131,  // TRUE to enable edge antialiasing 
    XBOX_D3DRS_MULTISAMPLEANTIALIAS      = 132,  // TRUE to enable multisample antialiasing
    XBOX_D3DRS_MULTISAMPLEMASK           = 133,  // DWORD per-pixel and sample enable/disable
    XBOX_D3DRS_MULTISAMPLETYPE           = 134,  // D3DMULTISAMPLE_TYPE (Xbox extension)
    XBOX_D3DRS_SHADOWFUNC                = 135,  // D3DCMPFUNC (Xbox extension)
    XBOX_D3DRS_LINEWIDTH                 = 136,  // float (Xbox extension)
    XBOX_D3DRS_DXT1NOISEENABLE           = 137,  // TRUE to enable DXT1 decompression noise (Xbox extension)

    XBOX_D3DRS_MAX                       = 138,  // Total number of renderstates 

    // Render states that are not supported on Xbox:
    //
    // D3DRS_LINEPATTERN
    // D3DRS_LASTPIXEL
    // D3DRS_CLIPPING
    // D3DRS_FOGVERTEXMODE
    // D3DRS_CLIPPLANEENABLE
    // D3DRS_SOFTWAREVERTEXPROCESSING
    // D3DRS_DEBUGMONITORTOKEN
    // D3DRS_INDEXEDVERTEXBLENDENABLE
    // D3DRS_TWEENFACTOR

    XBOX_D3DRS_FORCE_DWORD               = 0x7fffffff, /* force 32-bit size enum */

} XBOX_D3DRENDERSTATETYPE;

typedef enum _XBOX_D3DRESOURCETYPE {
    XBOX_D3DRTYPE_NONE                   =  0,
    XBOX_D3DRTYPE_SURFACE                =  1,
    XBOX_D3DRTYPE_VOLUME                 =  2,
    XBOX_D3DRTYPE_TEXTURE                =  3,
    XBOX_D3DRTYPE_VOLUMETEXTURE          =  4,
    XBOX_D3DRTYPE_CUBETEXTURE            =  5,
    XBOX_D3DRTYPE_VERTEXBUFFER           =  6,
    XBOX_D3DRTYPE_INDEXBUFFER            =  7,
    XBOX_D3DRTYPE_PUSHBUFFER             =  8,
    XBOX_D3DRTYPE_PALETTE                =  9,

    XBOX_D3DRTYPE_FORCE_DWORD            = 0x7fffffff
} XBOX_D3DRESOURCETYPE;

typedef enum _XBOX_D3DSHADEMODE {
    XBOX_D3DSHADE_FLAT               = 0x1d00,
    XBOX_D3DSHADE_GOURAUD            = 0x1d01,
    XBOX_D3DSHADE_PHONG              = 0x1d01,
    XBOX_D3DSHADE_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DSHADEMODE;

typedef enum _XBOX_D3DSTATEBLOCKTYPE
{
    XBOX_D3DSBT_ALL           = 1, // capture all state
    XBOX_D3DSBT_PIXELSTATE    = 2, // capture pixel state
    XBOX_D3DSBT_VERTEXSTATE   = 3, // capture vertex state
    XBOX_D3DSBT_FORCE_DWORD   = 0x7fffffff,
} XBOX_D3DSTATEBLOCKTYPE;

typedef enum _XBOX_D3DSTENCILOP {
    XBOX_D3DSTENCILOP_KEEP           = 0x1e00,
    XBOX_D3DSTENCILOP_ZERO           = 0,
    XBOX_D3DSTENCILOP_REPLACE        = 0x1e01,
    XBOX_D3DSTENCILOP_INCRSAT        = 0x1e02,
    XBOX_D3DSTENCILOP_DECRSAT        = 0x1e03,
    XBOX_D3DSTENCILOP_INVERT         = 0x150a,
    XBOX_D3DSTENCILOP_INCR           = 0x8507,
    XBOX_D3DSTENCILOP_DECR           = 0x8508,
    XBOX_D3DSTENCILOP_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DSTENCILOP;

typedef enum _XBOX_D3DSWAPEFFECT
{
    XBOX_D3DSWAPEFFECT_DISCARD           = 1,
    XBOX_D3DSWAPEFFECT_FLIP              = 2,
    XBOX_D3DSWAPEFFECT_COPY              = 3,
    XBOX_D3DSWAPEFFECT_COPY_VSYNC        = 4,

    XBOX_D3DSWAPEFFECT_FORCE_DWORD       = 0x7fffffff
} XBOX_D3DSWAPEFFECT;

typedef enum _XBOX_D3DTEXTUREADDRESS {
    XBOX_D3DTADDRESS_WRAP            = 1,
    XBOX_D3DTADDRESS_MIRROR          = 2,
    XBOX_D3DTADDRESS_CLAMP           = 3,
    XBOX_D3DTADDRESS_BORDER          = 4,
    XBOX_D3DTADDRESS_CLAMPTOEDGE     = 5, // Xbox extension

    // D3DTADDRESS_MIRRORONCE not supported on Xbox

    XBOX_D3DTADDRESS_MAX             = 6,
    XBOX_D3DTADDRESS_FORCE_DWORD     = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DTEXTUREADDRESS;

typedef enum _XBOX_D3DTEXTUREFILTERTYPE
{
    XBOX_D3DTEXF_NONE            = 0,    // filtering disabled (valid for mip filter only)
    XBOX_D3DTEXF_POINT           = 1,    // nearest
    XBOX_D3DTEXF_LINEAR          = 2,    // linear interpolation
    XBOX_D3DTEXF_ANISOTROPIC     = 3,    // anisotropic
    XBOX_D3DTEXF_FLATCUBIC       = 4,    // cubic
    XBOX_D3DTEXF_GAUSSIANCUBIC   = 5,    // different cubic kernel

    XBOX_D3DTEXF_MAX             = 6,
    XBOX_D3DTEXF_FORCE_DWORD     = 0x7fffffff,   // force 32-bit size enum
} XBOX_D3DTEXTUREFILTERTYPE;

typedef enum _XBOX_D3DTEXTUREOP
{
    // Control
    XBOX_D3DTOP_DISABLE              = 1,      // disables stage
    XBOX_D3DTOP_SELECTARG1           = 2,      // the default
    XBOX_D3DTOP_SELECTARG2           = 3,

    // Modulate
    XBOX_D3DTOP_MODULATE             = 4,      // multiply args together
    XBOX_D3DTOP_MODULATE2X           = 5,      // multiply and  1 bit
    XBOX_D3DTOP_MODULATE4X           = 6,      // multiply and  2 bits

    // Add
    XBOX_D3DTOP_ADD                  =  7,   // add arguments together
    XBOX_D3DTOP_ADDSIGNED            =  8,   // add with -0.5 bias
    XBOX_D3DTOP_ADDSIGNED2X          =  9,   // as above but left  1 bit
    XBOX_D3DTOP_SUBTRACT             = 10,   // Arg1 - Arg2, with no saturation
    XBOX_D3DTOP_ADDSMOOTH            = 11,   // add 2 args, subtract product
                                        // Arg1 + Arg2 - Arg1*Arg2
                                        // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    XBOX_D3DTOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
    XBOX_D3DTOP_BLENDTEXTUREALPHA    = 13, // texture alpha
    XBOX_D3DTOP_BLENDFACTORALPHA     = 14, // alpha from D3DRENDERSTATE_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    XBOX_D3DTOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
    XBOX_D3DTOP_BLENDCURRENTALPHA    = 16, // by alpha of current color

    // Specular mapping
    XBOX_D3DTOP_PREMODULATE            = 17,     // modulate with next texture before use
    XBOX_D3DTOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
                                            // COLOROP only
    XBOX_D3DTOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
                                            // COLOROP only
    XBOX_D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
                                            // COLOROP only
    XBOX_D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
                                            // COLOROP only

    // This can do either diffuse or specular bump mapping with correct input.
    // Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
    // where each component has been scaled and offset to make it signed.
    // The result is replicated into all four (including alpha) channels.
    // This is a valid COLOROP only.
    XBOX_D3DTOP_DOTPRODUCT3          = 22,

    // Triadic ops
    XBOX_D3DTOP_MULTIPLYADD          = 23, // Arg0 + Arg1*Arg2
    XBOX_D3DTOP_LERP                 = 24, // (Arg0)*Arg1 + (1-Arg0)*Arg2

    // Bump mapping
    XBOX_D3DTOP_BUMPENVMAP           = 25, // per pixel env map perturbation
    XBOX_D3DTOP_BUMPENVMAPLUMINANCE  = 26, // with luminance channel

    XBOX_D3DTOP_MAX                  = 27,
    XBOX_D3DTOP_FORCE_DWORD = 0x7fffffff,
} XBOX_D3DTEXTUREOP;

typedef enum _XBOX_D3DTEXTURESTAGESTATETYPE
{
    // State whose handling is deferred until the next Draw[Indexed]Vertices
    // call because of interdependencies on other states:

    XBOX_D3DTSS_COLOROP               =  0,  // D3DTEXTUREOP - per-stage blending controls for color channels 
    XBOX_D3DTSS_COLORARG0             =  1,  // D3DTA_* (D3DTA_TEXTURE etc.) third arg for triadic ops 
    XBOX_D3DTSS_COLORARG1             =  2,  // D3DTA_* (D3DTA_TEXTURE etc.) texture arg
    XBOX_D3DTSS_COLORARG2             =  3,  // D3DTA_* (D3DTA_TEXTURE etc.) texture arg 
    XBOX_D3DTSS_ALPHAOP               =  4,  // D3DTEXTUREOP - per-stage blending controls for alpha channel 
    XBOX_D3DTSS_ALPHAARG0             =  5,  // D3DTA_* (D3DTA_TEXTURE etc.) third arg for triadic ops 
    XBOX_D3DTSS_ALPHAARG1             =  6,  // D3DTA_* (D3DTA_TEXTURE etc.) texture arg
    XBOX_D3DTSS_ALPHAARG2             =  7,  // D3DTA_* (D3DTA_TEXTURE etc.) texture arg) 
    XBOX_D3DTSS_RESULTARG             =  8,  // D3DTA_* (D3DTA_TEXTURE etc.) arg for result (CURRENT or TEMP) 
    XBOX_D3DTSS_TEXTURETRANSFORMFLAGS =  9,  // D3DTEXTURETRANSFORMFLAGS controls texture transform 
    XBOX_D3DTSS_ADDRESSU              = 10,  // D3DTEXTUREADDRESS for U coordinate 
    XBOX_D3DTSS_ADDRESSV              = 11,  // D3DTEXTUREADDRESS for V coordinate 
    XBOX_D3DTSS_ADDRESSW              = 12,  // D3DTEXTUREADDRESS for W coordinate 
    XBOX_D3DTSS_MAGFILTER             = 13,  // D3DTEXF_* (D3DTEXF_LINEAR etc.) filter to use for magnification 
    XBOX_D3DTSS_MINFILTER             = 14,  // D3DTEXF_* (D3DTEXF_LINEAR etc.) filter to use for minification 
    XBOX_D3DTSS_MIPFILTER             = 15,  // D3DTEXF_* (D3DTEXF_LINEAR etc.) filter to use between mipmaps during minification 
    XBOX_D3DTSS_MIPMAPLODBIAS         = 16,  // float mipmap LOD bias 
    XBOX_D3DTSS_MAXMIPLEVEL           = 17,  // DWORD 0..(n-1) LOD index of largest map to use (0 == largest) 
    XBOX_D3DTSS_MAXANISOTROPY         = 18,  // DWORD maximum anisotropy 
    XBOX_D3DTSS_COLORKEYOP            = 19,  // D3DTEXTURECOLORKEYOP (Xbox extension)
    XBOX_D3DTSS_COLORSIGN             = 20,  // D3DTSIGN_* (D3DTSIGN_ASIGNED etc.) for color channels (xbox extension)
    XBOX_D3DTSS_ALPHAKILL             = 21,  // D3DTEXTUREALPHAKILL (Xbox extension)

    XBOX_D3DTSS_DEFERRED_MAX          = 22,

    // State that has immediate processing:

    XBOX_D3DTSS_BUMPENVMAT00          = 22,  // float (bump mapping matrix) 
    XBOX_D3DTSS_BUMPENVMAT01          = 23,  // float (bump mapping matrix) 
    XBOX_D3DTSS_BUMPENVMAT10          = 24,  // float (bump mapping matrix) 
    XBOX_D3DTSS_BUMPENVMAT11          = 25,  // float (bump mapping matrix) 
    XBOX_D3DTSS_BUMPENVLSCALE         = 26,  // float scale for bump map luminance 
    XBOX_D3DTSS_BUMPENVLOFFSET        = 27,  // float offset for bump map luminance 
    XBOX_D3DTSS_TEXCOORDINDEX         = 28,  // DWORD identifies which set of texture coordinates index this texture 
    XBOX_D3DTSS_BORDERCOLOR           = 29,  // D3DCOLOR 
    XBOX_D3DTSS_COLORKEYCOLOR         = 30,  // D3DCOLOR value for color key (Xbox extension)
                                 
    XBOX_D3DTSS_MAX                   = 32,  // Total number of texture stage states (bumped to a power of 2)

    XBOX_D3DTSS_FORCE_DWORD           = 0x7fffffff, // force 32-bit size enum 
} XBOX_D3DTEXTURESTAGESTATETYPE;

typedef enum _XBOX_D3DTEXTURETRANSFORMFLAGS {
    XBOX_D3DTTFF_DISABLE         = 0,    // texture coordinates are passed directly
    XBOX_D3DTTFF_COUNT1          = 1,    // rasterizer should expect 1-D texture coords
    XBOX_D3DTTFF_COUNT2          = 2,    // rasterizer should expect 2-D texture coords
    XBOX_D3DTTFF_COUNT3          = 3,    // rasterizer should expect 3-D texture coords
    XBOX_D3DTTFF_COUNT4          = 4,    // rasterizer should expect 4-D texture coords
    XBOX_D3DTTFF_PROJECTED       = 256,  // texcoords to be divided by COUNTth element
    XBOX_D3DTTFF_FORCE_DWORD     = 0x7fffffff,
} XBOX_D3DTEXTURETRANSFORMFLAGS;

typedef enum _XBOX_D3DTRANSFORMSTATETYPE {
    XBOX_D3DTS_VIEW          = 0,
    XBOX_D3DTS_PROJECTION    = 1,
    XBOX_D3DTS_TEXTURE0      = 2,
    XBOX_D3DTS_TEXTURE1      = 3,
    XBOX_D3DTS_TEXTURE2      = 4,
    XBOX_D3DTS_TEXTURE3      = 5,
    XBOX_D3DTS_WORLD         = 6,
    XBOX_D3DTS_WORLD1        = 7,
    XBOX_D3DTS_WORLD2        = 8,
    XBOX_D3DTS_WORLD3        = 9,

    XBOX_D3DTS_MAX           = 10,
    XBOX_D3DTS_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DTRANSFORMSTATETYPE;

typedef enum _XBOX_D3DVERTEXBLENDFLAGS
{
    XBOX_D3DVBF_DISABLE           = 0,     // Disable vertex blending
    XBOX_D3DVBF_1WEIGHTS          = 1,     // 2 matrix blending
    XBOX_D3DVBF_2WEIGHTS          = 3,     // 3 matrix blending
    XBOX_D3DVBF_3WEIGHTS          = 5,     // 4 matrix blending
    XBOX_D3DVBF_2WEIGHTS2MATRICES = 2,     // Xbox extension
    XBOX_D3DVBF_3WEIGHTS3MATRICES = 4,     // Xbox extension
    XBOX_D3DVBF_4WEIGHTS4MATRICES = 6,     // Xbox extension

    XBOX_D3DVBF_MAX               = 7,
    XBOX_D3DVBF_FORCE_DWORD       = 0x7fffffff, // force 32-bit size enum
} XBOX_D3DVERTEXBLENDFLAGS;

typedef enum _XBOX_D3DZBUFFERTYPE {
    XBOX_D3DZB_FALSE                 = 0,
    XBOX_D3DZB_TRUE                  = 1, // Z buffering
    XBOX_D3DZB_USEW                  = 2, // W buffering
    XBOX_D3DZB_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
} XBOX_D3DZBUFFERTYPE;

// Other possible translations:
//  D3DLOCK flags
//  D3DUSAGE flags
//  D3DCAPS8 flags
//  Gamma ramp flags
//  Private data flags

//***********************************************************************************
// Function prototypes
//***********************************************************************************

void    InitTypeTranslationMaps(CMap32* pTypeMapTable);

#endif // __TYPETRAN_H__
