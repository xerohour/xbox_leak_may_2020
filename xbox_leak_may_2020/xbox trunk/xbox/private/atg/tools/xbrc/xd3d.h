//-----------------------------------------------------------------------------
//  
//  File: xd3d.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Xbox-specific defines, taken from d3d8types-xbox.h and d3d8-xbox.h
//  
//-----------------------------------------------------------------------------
#pragma once
 
#define XD3DVERTEXBUFFER_ALIGNMENT     4
#define XD3DINDEXBUFFER_ALIGNMENT        4      
#define XD3DPUSHBUFFER_ALIGNMENT 4

#define XD3DPS_CONSTREG_MAX 8

#define XD3DVS_CONSTREG_COUNT		192
#define XD3DVS_STREAMS_MAX         	16
#define XD3DVS_INPUTREG_MAX         16

#define XD3DPUSHBUFFER_RUN_USING_CPU_COPY  0x80000000

#define XD3DCOMMON_TYPE_SHIFT         16
#define XD3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define XD3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define XD3DCOMMON_TYPE_PUSHBUFFER    0x00020000
#define XD3DCOMMON_TYPE_PALETTE       0x00030000
#define XD3DCOMMON_TYPE_TEXTURE       0x00040000
#define XD3DCOMMON_TYPE_SURFACE       0x00050000

#define XD3DERR_BUFFERTOOSMALL		MAKE_D3DHRESULT(2089) // Xbox extension

typedef enum _XD3DPRIMITIVETYPE {
    XD3DPT_POINTLIST             = 1,
    XD3DPT_LINELIST              = 2,
    XD3DPT_LINELOOP              = 3,  // Xbox extension
    XD3DPT_LINESTRIP             = 4,
    XD3DPT_TRIANGLELIST          = 5,
    XD3DPT_TRIANGLESTRIP         = 6,
    XD3DPT_TRIANGLEFAN           = 7,
    XD3DPT_QUADLIST              = 8,  // Xbox extension
    XD3DPT_QUADSTRIP             = 9,  // Xbox extension
    XD3DPT_POLYGON               = 10, // Xbox extension

    XD3DPT_MAX                   = 11,
    XD3DPT_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
} XD3DPRIMITIVETYPE;

typedef enum _XD3DVSD_TOKENTYPE
{
    XD3DVSD_TOKEN_NOP        = 0,    // NOP or extension
    XD3DVSD_TOKEN_STREAM,            // stream selector
    XD3DVSD_TOKEN_STREAMDATA,        // stream data definition (map to vertex input memory)
    XD3DVSD_TOKEN_TESSELLATOR,       // vertex input memory from tessellator
    XD3DVSD_TOKEN_CONSTMEM,          // constant memory from shader
    XD3DVSD_TOKEN_EXT,               // extension
    XD3DVSD_TOKEN_END = 7,           // end-of-array (requires all DWORD bits to be 1)
    XD3DVSD_FORCE_DWORD = 0x7fffffff,// force 32-bit size enum
} XD3DVSD_TOKENTYPE;

#define XD3DVSD_TOKENTYPESHIFT   29
#define XD3DVSD_TOKENTYPEMASK    (7 << XD3DVSD_TOKENTYPESHIFT)

#define XD3DVSD_STREAMNUMBERSHIFT 0
#define XD3DVSD_STREAMNUMBERMASK (0xF << XD3DVSD_STREAMNUMBERSHIFT)

#define XD3DVSD_DATALOADTYPESHIFT 28
#define XD3DVSD_DATALOADTYPEMASK (0x1 << XD3DVSD_DATALOADTYPESHIFT)

#define XD3DVSD_DATATYPESHIFT 16
#define XD3DVSD_DATATYPEMASK (0xFF << XD3DVSD_DATATYPESHIFT)

#define XD3DVSD_SKIPCOUNTSHIFT 16
#define XD3DVSD_SKIPCOUNTMASK (0xF << XD3DVSD_SKIPCOUNTSHIFT)

#define XD3DVSD_VERTEXREGSHIFT 0
#define XD3DVSD_VERTEXREGMASK (0x1F << XD3DVSD_VERTEXREGSHIFT)

#define XD3DVSD_VERTEXREGINSHIFT 20
#define XD3DVSD_VERTEXREGINMASK (0xF << XD3DVSD_VERTEXREGINSHIFT)

#define XD3DVSD_CONSTCOUNTSHIFT 25
#define XD3DVSD_CONSTCOUNTMASK (0xF << XD3DVSD_CONSTCOUNTSHIFT)

#define XD3DVSD_CONSTADDRESSSHIFT 0
#define XD3DVSD_CONSTADDRESSMASK (0xFF << XD3DVSD_CONSTADDRESSSHIFT)

#define XD3DVSD_CONSTRSSHIFT 16
#define XD3DVSD_CONSTRSMASK (0x1FFF << XD3DVSD_CONSTRSSHIFT)

#define XD3DVSD_EXTCOUNTSHIFT 24
#define XD3DVSD_EXTCOUNTMASK (0x1F << XD3DVSD_EXTCOUNTSHIFT)

#define XD3DVSD_EXTINFOSHIFT 0
#define XD3DVSD_EXTINFOMASK (0xFFFFFF << XD3DVSD_EXTINFOSHIFT)

#define XD3DVSD_MAKETOKENTYPE(tokenType) ((tokenType << XD3DVSD_TOKENTYPESHIFT) & XD3DVSD_TOKENTYPEMASK)

// macros for generation of CreateVertexShader Declaration token array

// Set current stream
// _StreamNumber [0..(MaxStreams-1)] stream to get data from
//
#define XD3DVSD_STREAM( _StreamNumber ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAM) | (_StreamNumber))

// Set tessellator stream
//
#define XD3DVSD_STREAMTESSSHIFT 28
#define XD3DVSD_STREAMTESSMASK (1 << XD3DVSD_STREAMTESSSHIFT)
#define XD3DVSD_STREAM_TESS( ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAM) | (XD3DVSD_STREAMTESSMASK))

// bind single vertex register to vertex element from vertex stream
//
// _VertexRegister [0..26] address of the vertex register
// _Type [XD3DVSDT_*] dimensionality and arithmetic data type

#define XD3DVSD_REG( _VertexRegister, _Type ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAMDATA) |            \
     ((_Type) << XD3DVSD_DATATYPESHIFT) | (_VertexRegister))

// Skip _DWORDCount DWORDs in vertex
//
#define XD3DVSD_SKIP( _DWORDCount ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAMDATA) | 0x10000000 | \
     ((_DWORDCount) << XD3DVSD_SKIPCOUNTSHIFT))

// Skip _BYTECount BYTEs in vertex (Xbox extension)
//
#define XD3DVSD_SKIPBYTES( _BYTECount ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_STREAMDATA) | 0x18000000 | \
     ((_BYTECount) << XD3DVSD_SKIPCOUNTSHIFT))

// load data into vertex shader constant memory
//
// _ConstantAddress [-96..95] - address of constant array to begin filling data
// _Count [0..15] - number of constant vectors to load (4 DWORDs each)
// followed by 4*_Count DWORDS of data
//
#define XD3DVSD_CONST( _ConstantAddress, _Count ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_CONSTMEM) | \
     ((_Count) << XD3DVSD_CONSTCOUNTSHIFT) | ((_ConstantAddress) + 96))

// enable tessellator generated normals
//
// _VertexRegisterIn  [0..15] address of vertex register whose input stream
//                            will be used in normal computation
// _VertexRegisterOut [0..15] address of vertex register to output the normal to
//
#define XD3DVSD_TESSNORMAL( _VertexRegisterIn, _VertexRegisterOut ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_TESSELLATOR) | \
     ((_VertexRegisterIn) << XD3DVSD_VERTEXREGINSHIFT) | \
     (_VertexRegisterOut))

// enable tessellator generated surface parameters
//
// _VertexRegister [0..15] address of vertex register to output parameters
//
#define XD3DVSD_TESSUV( _VertexRegister ) \
    (XD3DVSD_MAKETOKENTYPE(XD3DVSD_TOKEN_TESSELLATOR) | 0x10000000 | \
     (_VertexRegister))

// Generates END token
//
#define XD3DVSD_END() 0xFFFFFFFF

// Generates NOP token
#define XD3DVSD_NOP() 0x00000000

// Vertex attribute formats
#define XD3DVSDT_FLOAT1      0x12    // 1D float expanded to (value, 0., 0., 1.)
#define XD3DVSDT_FLOAT2      0x22    // 2D float expanded to (value, value, 0., 1.)
#define XD3DVSDT_FLOAT3      0x32    // 3D float expanded to (value, value, value, 1.)
#define XD3DVSDT_FLOAT4      0x42    // 4D float
#define XD3DVSDT_D3DCOLOR    0x40    // 4D packed unsigned bytes mapped to 0. to 1. range
#define XD3DVSDT_SHORT2      0x25    // 2D signed short expanded to (value, value, 0., 1.)
#define XD3DVSDT_SHORT4      0x45    // 4D signed short
#define XD3DVSDT_NORMSHORT1  0x11    // 1D signed, normalized short expanded to (value, 0, 0., 1.)
#define XD3DVSDT_NORMSHORT2  0x21    // 2D signed, normalized short expanded to (value, value, 0., 1.)
#define XD3DVSDT_NORMSHORT3  0x31    // 3D signed, normalized short expanded to (value, value, value, 1.)  
#define XD3DVSDT_NORMSHORT4  0x41    // 4D signed, normalized short expanded to (value, value, value, value)  
#define XD3DVSDT_NORMPACKED3 0x16    // 3 signed, normalized components packed in 32-bits.  (11,11,10).  
#define XD3DVSDT_SHORT1      0x15    // 1D signed short expanded to (value, 0., 0., 1.)  
#define XD3DVSDT_SHORT3      0x35    // 3D signed short expanded to (value, value, value, 1.)
#define XD3DVSDT_PBYTE1      0x14    // 1D packed byte expanded to (value, 0., 0., 1.)  
#define XD3DVSDT_PBYTE2      0x24    // 2D packed byte expanded to (value, value, 0., 1.)
#define XD3DVSDT_PBYTE3      0x34    // 3D packed byte expanded to (value, value, value, 1.)
#define XD3DVSDT_PBYTE4      0x44    // 4D packed byte expanded to (value, value, value, value) 
#define XD3DVSDT_FLOAT2H     0x72    // 2D homogeneous float expanded to (value, value,0., value.)

// Xbox-specific defines, taken from d3d8types-xbox.h and d3d8-xbox.h
#define X_D3DFMT_A8R8G8B8              0x00000006
#define X_D3DFMT_X8R8G8B8              0x00000007
#define X_D3DFMT_R5G6B5                0x00000005
#define X_D3DFMT_R6G5B5                0x00000027
#define X_D3DFMT_X1R5G5B5              0x00000003
#define X_D3DFMT_A1R5G5B5              0x00000002
#define X_D3DFMT_A4R4G4B4              0x00000004
#define X_D3DFMT_A8                    0x00000019
#define X_D3DFMT_A8B8G8R8              0x0000003A   
#define X_D3DFMT_B8G8R8A8              0x0000003B   
#define X_D3DFMT_R4G4B4A4              0x00000039   
#define X_D3DFMT_R5G5B5A1              0x00000038   
#define X_D3DFMT_R8G8B8A8              0x0000003C   
#define X_D3DFMT_R8B8                  0x00000029   
#define X_D3DFMT_G8B8                  0x00000028   
#define X_D3DFMT_P8                    0x0000000B
#define X_D3DFMT_L8                    0x00000000
#define X_D3DFMT_A8L8                  0x0000001A
#define X_D3DFMT_AL8                   0x00000001   
#define X_D3DFMT_L16                   0x00000032   
#define X_D3DFMT_V8U8                  0x00000028
#define X_D3DFMT_L6V5U5                0x00000027
#define X_D3DFMT_X8L8V8U8              0x00000007
#define X_D3DFMT_Q8W8V8U8              0x0000003A
#define X_D3DFMT_V16U16                0x00000033
#define X_D3DFMT_DXT1                  0x0000000C
#define X_D3DFMT_DXT2                  0x0000000E
#define X_D3DFMT_DXT3                0x0000000E // Not valid on Xbox
#define X_D3DFMT_DXT4                  0x0000000F
#define X_D3DFMT_DXT5                0x0000000F // Not valid on Xbox
#define X_D3DFMT_LIN_A1R5G5B5          0x00000010   
#define X_D3DFMT_LIN_A4R4G4B4          0x0000001D   
#define X_D3DFMT_LIN_A8                0x0000001F   
#define X_D3DFMT_LIN_A8B8G8R8          0x0000003F   
#define X_D3DFMT_LIN_A8R8G8B8          0x00000012   
#define X_D3DFMT_LIN_B8G8R8A8          0x00000040   
#define X_D3DFMT_LIN_G8B8              0x00000017   
#define X_D3DFMT_LIN_R4G4B4A4          0x0000003E   
#define X_D3DFMT_LIN_R5G5B5A1          0x0000003D   
#define X_D3DFMT_LIN_R5G6B5            0x00000011   
#define X_D3DFMT_LIN_R6G5B5            0x00000037   
#define X_D3DFMT_LIN_R8B8              0x00000016   
#define X_D3DFMT_LIN_R8G8B8A8          0x00000041   
#define X_D3DFMT_LIN_X1R5G5B5          0x0000001C   
#define X_D3DFMT_LIN_X8R8G8B8          0x0000001E   
#define X_D3DFMT_LIN_A8L8              0x00000020   
#define X_D3DFMT_LIN_AL8               0x0000001B   
#define X_D3DFMT_LIN_L16               0x00000035   
#define X_D3DFMT_LIN_L8                0x00000013   
#define X_D3DFMT_LIN_V16U16            0x00000036
#define X_D3DFMT_LIN_V8U8              0x00000017
#define X_D3DFMT_LIN_L6V5U5            0x00000037
#define X_D3DFMT_LIN_X8L8V8U8          0x0000001E
#define X_D3DFMT_LIN_Q8W8V8U8          0x00000012

#define D3DTEXTURE_ALIGNMENT            128

#define D3DFORMAT_DMACHANNEL_MASK       0x00000003
#define D3DFORMAT_DMACHANNEL_A          0x00000001
#define D3DFORMAT_DMACHANNEL_B          0x00000002

#define D3DFORMAT_CUBEMAP               0x00000004
#define D3DFORMAT_BORDERSOURCE_COLOR    0x00000008
#define D3DFORMAT_DIMENSION_MASK        0x000000F0
#define D3DFORMAT_DIMENSION_SHIFT       4
#define D3DFORMAT_FORMAT_MASK           0x0000FF00
#define D3DFORMAT_FORMAT_SHIFT          8
#define D3DFORMAT_MIPMAP_MASK           0x000F0000
#define D3DFORMAT_MIPMAP_SHIFT          16
#define D3DFORMAT_USIZE_MASK            0x00F00000
#define D3DFORMAT_USIZE_SHIFT           20
#define D3DFORMAT_VSIZE_MASK            0x0F000000
#define D3DFORMAT_VSIZE_SHIFT           24
#define D3DFORMAT_PSIZE_MASK            0xF0000000
#define D3DFORMAT_PSIZE_SHIFT           28

#define D3DCOMMON_TYPE_SHIFT         16
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define D3DCOMMON_TYPE_PALETTE       0x00030000
#define D3DCOMMON_TYPE_TEXTURE       0x00040000
#define D3DCOMMON_TYPE_SURFACE       0x00050000
// #define D3DCOMMON_VIDEOMEMORY        0x00800000

#define D3DTEXTURE_PITCH_ALIGNMENT   64


// Texture Resource Format
struct XD3DTexture
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
    DWORD Format;
    DWORD Size;
};

struct XD3DVertexBuffer
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
};

struct XD3DIndexBuffer {
    DWORD Common;                    
    DWORD Data;
    DWORD Lock;
};

struct XD3DPushBuffer
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
    DWORD Size; 			// Size, in bytes, of the push-buffer program.
    DWORD AllocationSize;   // Size, in bytes, of the allocation of the buffer pointed to by 'Data'.
};
