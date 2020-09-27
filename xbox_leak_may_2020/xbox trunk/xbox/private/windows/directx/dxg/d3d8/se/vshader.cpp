/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       vshader.cpp
 *  Content:    Contains all of the vertex shader and stream logic.
 *
 ****************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//------------------------------------------------------------------------------
// g_LoadedVertexShaderSlot
//
// This debug-only array is used for tracking what slots are loaded with
// vertex shader program via LoadVertexShader.  0 in the byte means no
// no program is actively loaded there, 1 means there's some program loaded
// at that slot:

#if DBG

BYTE g_LoadedVertexShaderSlot[VSHADER_PROGRAM_SLOTS];

#endif

//------------------------------------------------------------------------------
// Shader constants

// Vertex Shader 'Magic cookies'

#define VSHADER_TYPE_ORDINARY 0x2078
#define VSHADER_TYPE_WRITE 0x7778
#define VSHADER_TYPE_STATE 0x7378

// The user gets to use constant registers -96 through 95 (or 0 through 95
// if only using 96 registers).  We add 96 before giving those register
// values to the hardware:

#define VSHADER_CONSTANT_MIN -96
#define VSHADER_CONSTANT_MAX 96     // (exclusive)
#define VSHADER_CONSTANT_NUM 192
#define VSHADER_CONSTANT_BIAS 96

// We always download constants and programs in batches of this many
// slots (where a slot is 4 DWORDS, or equivalently 16 BYTEs), because
// that's what the hardware limits us to:

#define VSHADER_BATCH_LIMIT 8

//------------------------------------------------------------------------------
// g_FvfVertexShader is storage used whenever an FVF is specified as the 
// current vertex shader program.  CDevice::m_pVertexShader will often 
// point to this...

VertexShader g_FvfVertexShader;

//------------------------------------------------------------------------------
// g_DirectVertexShader is storage used whenever an structure is used 
// to specify the vertex attribute format. CDevice::m_pVertexShader will often 
// point to this if the Direct vertex shader APIs are used...

VertexShader g_DirectVertexShader;

//------------------------------------------------------------------------------
// g_Stream
//
// Keeps track of all the currently selected streams for the device.

Stream g_Stream[16];

//------------------------------------------------------------------------------
// g_TextureCount is for mapping the FVF count of textures:

D3DCONST DWORD g_TextureCount[] = 
{ 
    2, 3, 4, 1 
};

//------------------------------------------------------------------------------
// g_PassthruProgramSpecularFog 
//
// Xbox Shader Assembler 1.00.3624.1
//
//    ; szPassthruProgramSpecFog
//    ; from nvKelvinState.cpp
//    xvs.1.1
//    
//    #pragma screenspace
//    
//        mov r0,v0
//        rcp r0.w,r0.w
//        mul r0,r0,c[-96]
//        add oPos,r0,c[-95]
//        mov oD0,v3
//        mov oD1,v4
//        mov oFog,v4.w
//        mov oPts,v1.x
//        mov oB0,v7
//        mov oB1,v8
//        mov oT0,v9
//        mov oT1,v10
//        mov oT2,v11
//        mov oT3,v12

D3DCONST DWORD g_PassthruProgramSpecularFog[] =
{
        0x00000000, 0x0020001b, 0x0836106c, 0x2f100ff8,
        0x00000000, 0x0420061b, 0x083613fc, 0x5011f818,
        0x00000000, 0x002008ff, 0x0836106c, 0x2070f828,
        0x00000000, 0x0240081b, 0x1436186c, 0x2f20f824,
        0x00000000, 0x0060201b, 0x2436106c, 0x3070f800,
        0x00000000, 0x00200200, 0x0836106c, 0x2070f830,
        0x00000000, 0x00200e1b, 0x0836106c, 0x2070f838,
        0x00000000, 0x0020101b, 0x0836106c, 0x2070f840,
        0x00000000, 0x0020121b, 0x0836106c, 0x2070f848,
        0x00000000, 0x0020141b, 0x0836106c, 0x2070f850,
        0x00000000, 0x0020161b, 0x0836106c, 0x2070f858,
        0x00000000, 0x0020181b, 0x0836106c, 0x2070f861
};

//------------------------------------------------------------------------------
// g_PassthruProgramZFog
//
// Xbox Shader Assembler 1.00.3624.1
//
//    ; szPassthruProgramZFog
//    ; from nvKelvinState.cpp
//    xvs.1.1
//    
//    #pragma screenspace
//    
//        mov r0,v0
//        rcp r0.w,r0.w
//        mul r0,r0,c[-96]
//        add oPos,r0,c[-95]
//        mov oD0,v3
//        mov oD1,v4
//        mov oFog,v0.z
//        mov oPts,v1.x
//        mov oB0,v7
//        mov oB1,v8
//        mov oT0,v9
//        mov oT1,v10
//        mov oT2,v11
//        mov oT3,v12

D3DCONST DWORD g_PassthruProgramZFog[] =
{
        0x00000000, 0x022000aa, 0x0836086c, 0x201ff828,
        0x00000000, 0x0020081b, 0x0836106c, 0x2070f820,
        0x00000000, 0x0420061b, 0x083613fc, 0x5011f818,
        0x00000000, 0x00200e1b, 0x0836106c, 0x2070f838,
        0x00000000, 0x0240021b, 0x14361800, 0x2f20f834,
        0x00000000, 0x0060201b, 0x2436106c, 0x3070f800,
        0x00000000, 0x0020101b, 0x0836106c, 0x2070f840,
        0x00000000, 0x0020121b, 0x0836106c, 0x2070f848,
        0x00000000, 0x0020141b, 0x0836106c, 0x2070f850,
        0x00000000, 0x0020161b, 0x0836106c, 0x2070f858,
        0x00000000, 0x0020181b, 0x0836106c, 0x2070f861
};

//------------------------------------------------------------------------------
// g_PassthruProgramWFog
//
// Xbox Shader Assembler 1.00.3624.1
//
//    ; szPassthruProgramWFog
//    ; from nvKelvinState.cpp
//    xvs.1.1
//    
//    #pragma screenspace
//    
//        mov r0,v0
//        rcp r0.w,r0.w
//        mul r0,r0,c[-96]
//        add oPos,r0,c[-95]
//        mov oD0,v3
//        mov oD1,v4
//        rcp oFog,v0.w
//        mov oPts,v1.x
//        mov oB0,v7
//        mov oB1,v8
//        mov oT0,v9
//        mov oT1,v10
//        mov oT2,v11
//        mov oT3,v12

D3DCONST DWORD g_PassthruProgramWFog[] =
{
        0x00000000, 0x0020001b, 0x0836106c, 0x2f100ff8,
        0x00000000, 0x0420061b, 0x083613fc, 0x5011f818,
        0x00000000, 0x0400001b, 0x083613fc, 0x2070f82c,
        0x00000000, 0x0240081b, 0x1436186c, 0x2f20f824,
        0x00000000, 0x0060201b, 0x2436106c, 0x3070f800,
        0x00000000, 0x00200200, 0x0836106c, 0x2070f830,
        0x00000000, 0x00200e1b, 0x0836106c, 0x2070f838,
        0x00000000, 0x0020101b, 0x0836106c, 0x2070f840,
        0x00000000, 0x0020121b, 0x0836106c, 0x2070f848,
        0x00000000, 0x0020141b, 0x0836106c, 0x2070f850,
        0x00000000, 0x0020161b, 0x0836106c, 0x2070f858,
        0x00000000, 0x0020181b, 0x0836106c, 0x2070f861
};

//------------------------------------------------------------------------------
// InitializeVertexShaderFromFvf
//
// Loosely derived from nvGenerateFVFData and nvComputeTextureCoordinateData.

VOID InitializeVertexShaderFromFvf(
    VertexShader* pVertexShader,
    DWORD Fvf)
{
    DWORD offset;
    DWORD position;
    DWORD i;
    DWORD weights;

    pVertexShader->Flags = 0;
    pVertexShader->Dimensionality = 0;

    // Kelvin doesn't let us use a type of '0' with a size of '0', so
    // we have to run through and stamp the values:

    for (i = 0; i < 16; i++)
    {
        pVertexShader->Slot[i].SizeAndType = SIZEANDTYPE_DISABLED;
    }

    position = Fvf & D3DFVF_POSITION_MASK;

    offset = 0;

    if (position == D3DFVF_XYZRHW)
    {
        pVertexShader->Flags |= VERTEXSHADER_PASSTHROUGH;

        pVertexShader->Slot[SLOT_POSITION].SizeAndType 
             = DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _4)
             | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        offset = 4 * sizeof(float);
    }
    else if (position != 0)
    {
        pVertexShader->Slot[SLOT_POSITION].SizeAndType 
             = DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _3)
             | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        offset = 3 * sizeof(float);

        if (position >= D3DFVF_XYZB1)
        {
            weights = ((position - D3DFVF_XYZB1) >> 1) + 1;

            if (position == D3DFVF_XYZB1)
            {
                ASSERT(weights == 1);
            }
            else if (position == D3DFVF_XYZB4)
            {
                ASSERT(weights == 4);
            }

            pVertexShader->Slot[SLOT_WEIGHT].Offset = offset;
            pVertexShader->Slot[SLOT_WEIGHT].SizeAndType 
                 = DRF_NUMFAST(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, weights)
                 | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

            offset += weights * sizeof(float);
        }
    }

    if (Fvf & D3DFVF_NORMAL)
    {
        pVertexShader->Slot[SLOT_NORMAL].Offset = offset;
        pVertexShader->Slot[SLOT_NORMAL].SizeAndType
             = DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _3)
             | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        offset += 3 * sizeof(float);
    }

    if (Fvf & D3DFVF_DIFFUSE)
    {
        pVertexShader->Slot[SLOT_DIFFUSE].Offset = offset;
        pVertexShader->Slot[SLOT_DIFFUSE].SizeAndType
             = DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _4)
             | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _UB_D3D);

        pVertexShader->Flags |= VERTEXSHADER_HASDIFFUSE;

        offset += sizeof(DWORD);
    }

    if (Fvf & D3DFVF_SPECULAR)
    {
        pVertexShader->Slot[SLOT_SPECULAR].Offset = offset;
        pVertexShader->Slot[SLOT_SPECULAR].SizeAndType
             = DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _4)
             | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _UB_D3D);

        pVertexShader->Flags |= VERTEXSHADER_HASSPECULAR;

        offset += sizeof(DWORD);
    }

    DWORD numTextures = FVF_TEXCOORD_NUMBER(Fvf);
    DWORD textureFormats = Fvf >> 16;

    ASSERT(numTextures <= D3DDP_MAXTEXCOORD);

    // Handle the texture encoding:

    for (i = 0; i < numTextures; i++, textureFormats >>= 2)
    {
        DWORD count = g_TextureCount[textureFormats & 3];
        DWORD size = count << 2;

        pVertexShader->Slot[i + SLOT_TEXTURE0].Offset = offset;
        pVertexShader->Slot[i + SLOT_TEXTURE0].SizeAndType
            = DRF_NUMFAST(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, count)
            | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);

        pVertexShader->Dimensionality |= (count << (i * 8));

        offset += size;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetStreamSource
//
// DOC: Unlike in DX8, the stride for a FVF does not have to match the vertex
//      size.

extern "C"
void WINAPI D3DDevice_SetStreamSource(
    UINT StreamNumber,
    D3DVertexBuffer* pVertexBuffer,
    UINT Stride) 
{                              
    COUNT_API(API_D3DDEVICE_SETSTREAMSOURCE);

    if (DBG_CHECK(TRUE))
    {
        if (StreamNumber > 15)
        {
            DPF_ERR("Invalid stream number");
        }
        if (Stride > 255)
        {
            DPF_ERR("NV2A allows a maximum stride of 255");
        }
        if (D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_INPUT)
        {
            WARNING("SetStreamSource called while SetVertexShaderInput in effect,\n"
                    "will be deferred");
        }
    }

    if ((pVertexBuffer == g_Stream[StreamNumber].pVertexBuffer) &&
        (Stride == g_Stream[StreamNumber].Stride))
    {
        COUNT_PERF(PERF_REDUNDANT_SETSTREAMSOURCE);
    }

    CDevice* pDevice = g_pDevice;
    
    // Reference the new, release the old, remembering that either may be
    // NULL:

    if (pVertexBuffer)
    {
        InternalAddRef(pVertexBuffer);
    }

    if (g_Stream[StreamNumber].pVertexBuffer)
    {
        pDevice->RecordResourceReadPush(g_Stream[StreamNumber].pVertexBuffer);

        InternalRelease(g_Stream[StreamNumber].pVertexBuffer);
    }

    // If the stride has not changed, we can get away with only reprogramming
    // the vertex offset registers.  If the stride has changed, we also
    // have to reprogram the SizeAndType registers.

    if (Stride == g_Stream[StreamNumber].Stride)
    {
        D3D__DirtyFlags |= D3DDIRTYFLAG_VERTEXFORMAT_OFFSETS;
    }
    else
    {
        D3D__DirtyFlags |= D3DDIRTYFLAG_VERTEXFORMAT;
    }

    // Remember the new data:

    g_Stream[StreamNumber].pVertexBuffer = pVertexBuffer;
    g_Stream[StreamNumber].Stride = Stride;
}

//------------------------------------------------------------------------------
// D3DDevice_GetStreamSource

extern "C"
void WINAPI D3DDevice_GetStreamSource(
    UINT StreamNumber,
    D3DVertexBuffer** ppVertexBuffer,
    UINT* pStride) 
{ 
    COUNT_API(API_D3DDEVICE_GETSTREAMSOURCE);

    if (DBG_CHECK(TRUE))
    {
        if (StreamNumber > 15)
        {
            DPF_ERR("Invalid stream number");
        }
    }

    CDevice* pDevice = g_pDevice;

    D3DVertexBuffer* pVertexBuffer = g_Stream[StreamNumber].pVertexBuffer;

    if (pVertexBuffer)
    {
        pVertexBuffer->AddRef();

        *ppVertexBuffer = pVertexBuffer;
        *pStride = g_Stream[StreamNumber].Stride;
    }
    else
    {
        *ppVertexBuffer = NULL;
        *pStride = 0;
    }
}

//------------------------------------------------------------------------------
// ValidateFVF

void ValidateFVF(
    DWORD type)
{
    DWORD dwTexCoord = FVF_TEXCOORD_NUMBER(type);
    DWORD vertexType = type & D3DFVF_POSITION_MASK;

    // Xbox allows NULL FVF values:

    if (type == 0)
        return;

    // Texture format bits above texture count should be zero
    // Reserved field 0 and 2 should be 0
    // Reserved 1 should be set only for LVERTEX
    // Only two vertex position types allowed

    if (type & g_TextureFormatMask[dwTexCoord])
    {
        D3D_ERR("FVF has incorrect texture format");
    }
    if (type & D3DFVF_RESERVED0)
    {
        D3D_ERR("FVF has reserved bit(s) set");
    }
    if (!(vertexType == D3DFVF_XYZRHW ||
          vertexType == D3DFVF_XYZ ||
          vertexType == D3DFVF_XYZB1 ||
          vertexType == D3DFVF_XYZB2 ||
          vertexType == D3DFVF_XYZB3 ||
          vertexType == D3DFVF_XYZB4))
    {
        D3D_ERR("FVF has incorrect position type");
    }

    if (vertexType == D3DFVF_XYZRHW && type & D3DFVF_NORMAL)
    {
        D3D_ERR("Normal should not be used with XYZRHW position type");
    }
}

//------------------------------------------------------------------------------
// ShadowVertexShaderState
//
// Parse the function stream and update our shadowed copies of the constant
// registers and program slots

VOID ShadowVertexShaderState(
    VertexShader* pVertexShader,
    DWORD Address)
{
    CDevice* pDevice = g_pDevice;

    DWORD* pProgramAndConstants = pVertexShader->ProgramAndConstants;

    DWORD* pSlots = &pDevice->m_VertexShaderProgramSlots[Address][0];

    FLOAT* pConstants;
    DWORD constantSlot;

    // First parse the program loads:

    do {
        DWORD methodEncode = *pProgramAndConstants++;
        DWORD count = methodEncode >> 18;

        ASSERT(count != 0);
        if (count == 1)
        {
            constantSlot = *pProgramAndConstants;

            ASSERT(constantSlot < VSHADER_CONSTANT_NUM);
            ASSERT((methodEncode & 0xffff) 
              == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT_LOAD, 0));

            pConstants = &pDevice->m_VertexShaderConstants[constantSlot][0];
        }
        else if ((methodEncode & 0xffff)
            == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_PROGRAM(0), 0))
        {
            ASSERT(((count & 3) == 0) && 
                   (count != 0) && 
                   (count < VSHADER_PROGRAM_SLOTS*4));
            ASSERT(pSlots + count 
               <= &pDevice->m_VertexShaderProgramSlots[VSHADER_PROGRAM_SLOTS][0]);
            ASSERT((methodEncode & 0xffff) 
               == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_PROGRAM(0), 0));
    
            memcpy(pSlots, pProgramAndConstants, count * sizeof(DWORD));
    
            pSlots += count;
        }
        else
        {
            ASSERT(((count & 3) == 0) && (count < VSHADER_PROGRAM_SLOTS*4));
            ASSERT(pConstants + count <= 
                   &pDevice->m_VertexShaderConstants[VSHADER_CONSTANT_NUM][0]);
            ASSERT((methodEncode & 0xffff) 
                == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT(0), 0));

            memcpy(pConstants, pProgramAndConstants, count * sizeof(DWORD));

            pConstants += count;
        }

        pProgramAndConstants += count;

    } while (*pProgramAndConstants != 0);

    // Make sure we parsed the whole thing, and ended where we expected:

    ASSERT(pProgramAndConstants == &pVertexShader->ProgramAndConstants[
                                pVertexShader->ProgramAndConstantsDwords]);
}

//------------------------------------------------------------------------------
// ParseProgram
//
// Copies an Nvidia opcode vertex shader program and at the same time
// interperses the required NV2A load commands every 32 dwords.
//
// Returns the a pointer to the end of the resulting push buffer snippet.

PPUSH ParseProgram(
    PPUSH pDst,
    CONST DWORD* pProgram,
    DWORD Dwords) // 'pProgram' size, in dwords
{
    DWORD batchSize;
    PPUSH pPush = pDst;

    do {
        batchSize = min(Dwords, 4 * VSHADER_BATCH_LIMIT); // In dwords

        *pPush = PUSHER_METHOD(SUBCH_3D, 
                                 NV097_SET_TRANSFORM_PROGRAM(0), 
                                 batchSize);

        memcpy((void*) (pPush + 1), pProgram, batchSize * sizeof(DWORD));
        
        pProgram += batchSize;
        pPush += batchSize + 1;

    } while ((Dwords -= batchSize) != 0);

    return pPush;
}

//------------------------------------------------------------------------------
// D3DDevice_RunVertexStateShader

extern "C"
void WINAPI D3DDevice_RunVertexStateShader(
    DWORD Address,
    CONST FLOAT *pData)
{
    COUNT_API(API_D3DDEVICE_RUNVERTEXSTATESHADER);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        // At least verify that the currently selected vertex shader is
        // a state shader:

        if (!(pDevice->m_pVertexShader->Flags & VERTEXSHADER_STATE))
        {
            DPF_ERR("Must have a vertex state shader selected");
        }
        if (Address >= VSHADER_PROGRAM_SLOTS)
        {
            DPF_ERR("Start address can't be larger than 135");
        }
    #if DBG
        if (!g_LoadedVertexShaderSlot[Address])
        {
            DPF_ERR("Can't run a vertex state shader if it hasn't been loaded");
        }
    #endif
    }

    PPUSH pPush = pDevice->StartPush();

    if (pData != NULL)
    {
        Push4f(pPush, NV097_SET_TRANSFORM_DATA(0),
               pData[0], pData[1], pData[2], pData[3]);

        pPush += 5;
    }

    Push1(pPush, NV097_LAUNCH_TRANSFORM_PROGRAM, Address);

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// D3DDevice_LoadVertexShader

extern "C"
void WINAPI D3DDevice_LoadVertexShader(
    DWORD Handle,
    DWORD Address)
{
    COUNT_API(API_D3DDEVICE_LOADVERTEXSHADER);

    CDevice* pDevice = g_pDevice;

    VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Must be a vertex shader program, not an FVF");
        }
    #if DBG
        if (pVertexShader->Signature != 'Vshd')
        {
            DPF_ERR("Invalid vertex shader object (already deleted?)");
        }
    #endif
        if (Address >= VSHADER_PROGRAM_SLOTS)
        {
            DPF_ERR("Start address can't be larger than 135");
        }
        if (Address + pVertexShader->ProgramSize > VSHADER_PROGRAM_SLOTS)
        {
            DPF_ERR("End address larger than 136");
        }

        // Remember which slots have been loaded:

    #if DBG
        memset(&g_LoadedVertexShaderSlot[Address], 1, pVertexShader->ProgramSize);
    #endif
    }

    if (!(pDevice->m_StateFlags & STATE_PUREDEVICE))
    {
        ShadowVertexShaderState(pVertexShader, Address);
    }

    DWORD dwordCount = pVertexShader->ProgramAndConstantsDwords;

    PPUSH pPush = pDevice->StartPush(dwordCount + 2);

    Push1(pPush, NV097_SET_TRANSFORM_PROGRAM_LOAD, Address); 

    memcpy((void*) (pPush + 2), pVertexShader->ProgramAndConstants, 4 * dwordCount);

    PushedRaw(pPush + dwordCount + 2);

    pDevice->EndPush(pPush + dwordCount + 2);
}

//------------------------------------------------------------------------------
// D3DDevice_LoadVertexShaderProgram
//
// NOTE: Like LoadVeretxShader, the read/write/state-shader aspect is ignored
//       on the load.

extern "C"
void WINAPI D3DDevice_LoadVertexShaderProgram(
    CONST DWORD* pFunction,
    DWORD Address)
{
    COUNT_API(API_D3DDEVICE_LOADVERTEXSHADERPROGRAM);

    CDevice* pDevice = g_pDevice;

    // 'shaderLength' ranges from 1 to 136:

    DWORD shaderLength = *((WORD*) pFunction + 1);

    if (DBG_CHECK(TRUE))
    {
        if ((shaderLength < 1) || (shaderLength > VSHADER_PROGRAM_SLOTS))
        {
            DPF_ERR("Length specified in second WORD must be between 1 and 136");
        }
        if (Address >= VSHADER_PROGRAM_SLOTS)
        {
            DPF_ERR("Start address can't be larger than 135");
        }
        if (Address + shaderLength > VSHADER_PROGRAM_SLOTS)
        {
            DPF_ERR("End address larger than 136");
        }

        // Remember which slots have been loaded:

    #if DBG
        memset(&g_LoadedVertexShaderSlot[Address], 1, shaderLength);
    #endif
    }

    DWORD dwordCount = shaderLength * 4;

    if (!(pDevice->m_StateFlags & STATE_PUREDEVICE))
    {
        memcpy(&pDevice->m_VertexShaderProgramSlots[Address][0],
               pFunction + 1,
               dwordCount * sizeof(DWORD));
    }

    PPUSH pPush = pDevice->StartPush(dwordCount + 2 + 
                                (VSHADER_PROGRAM_SLOTS / VSHADER_BATCH_LIMIT));

    Push1(pPush, NV097_SET_TRANSFORM_PROGRAM_LOAD, Address); 

    pPush = ParseProgram(pPush + 2, pFunction + 1, dwordCount);

    PushedRaw(pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// D3DDevice_SelectVertexShaderDirect

extern "C"
void WINAPI D3DDevice_SelectVertexShaderDirect(
    D3DVERTEXATTRIBUTEFORMAT *pVAF,       // DOC: Can be NULL
    DWORD Address)
{
    // This API call is counted as an API_D3DDEVICE_SELECTVERTEXSHADER

    if (DBG_CHECK(TRUE))
    {
#if DBG
        if(pVAF != NULL)
        {
            CheckVertexAttributeFormatStruct(pVAF);
        }
#endif    
    }

    if(pVAF != NULL)
    {
        memcpy(&(g_DirectVertexShader.Slot[0]), pVAF, sizeof(VertexShaderSlot)*16);
        D3DDevice_SelectVertexShader((DWORD)(&g_DirectVertexShader) | D3DFVF_RESERVED0, Address);
    }
    else
    {
        D3DDevice_SelectVertexShader(NULL, Address);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SelectVertexShader

extern "C"
void WINAPI D3DDevice_SelectVertexShader(
    DWORD Handle,       // DOC: Can be NULL
    DWORD Address)
{
    COUNT_API(API_D3DDEVICE_SELECTVERTEXSHADER);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (Address >= VSHADER_PROGRAM_SLOTS)
        {
            DPF_ERR("Start address can't be larger than 135");
        }
    #if DBG
        if (!g_LoadedVertexShaderSlot[Address])
        {
            DPF_ERR("Can't select a vertex state shader if it hasn't been loaded");
        }
    #endif
        if ((Handle == NULL) && (pDevice->m_pVertexShader == NULL))
        {
            DPF_ERR("Shader handle required in SelectVertexShader when switching "
                    "from fixed-function to vertex shader");
        }
    }

    if (Handle != NULL)
    {
        VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

        if (DBG_CHECK(TRUE))
        {
            if (!IsVertexProgram(Handle))
            {
                DPF_ERR("Must be a vertex shader program, not an FVF");
            }
        #if DBG
            if (pVertexShader->Signature != 'Vshd')
            {
                DPF_ERR("Invalid vertex shader object (already deleted?)");
            }
        #endif
        }

        pDevice->m_pVertexShader = pVertexShader;

        pDevice->m_VertexShaderHandle = Handle;

        // The 'D3DDIRTYFLAG_LIGHTS' is really needed only if switching from the
        // fixed function  pipeline to a programmable shader.  But it's not
        // a lot of overhead for the non-fixed-function-pipeline case, so I'm
        // not going to special-case it here:

        D3D__DirtyFlags |= (D3DDIRTYFLAG_VERTEXFORMAT |
                            D3DDIRTYFLAG_LIGHTS);

        // Tell the hardware our mode:

        PPUSH pPush = pDevice->StartPush();

        Push2(pPush, 
              
              NV097_SET_TRANSFORM_EXECUTION_MODE,

              // NV097_SET_TRANSFORM_EXECUTION_MODE:

              (DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
               DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)),

              // NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN:

              pVertexShader->Flags & VERTEXSHADER_WRITE);

        // Reset the viewport if the fixed-function pipeline was previously
        // active:

        pPush = CommonSetViewport(pDevice, pPush + 3);

        pDevice->EndPush(pPush);
    }

    // Tell the hardware the start location of the current program:

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_TRANSFORM_PROGRAM_START, Address); 

    pDevice->EndPush(pPush + 2);

    pDevice->m_VertexShaderStart = Address;   // set shadow value

}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderSize

extern "C"
void WINAPI D3DDevice_GetVertexShaderSize(
    DWORD Handle, 
    UINT* pSize)
{
    COUNT_API(API_D3DDEVICE_GETVERTEXSHADERSIZE);

    VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Must be a vertex shader program, not an FVF");
        }
    #if DBG
        if (pVertexShader->Signature != 'Vshd')
        {
            DPF_ERR("Invalid vertex shader object (already deleted?)");
        }
    #endif
    }

    if (pSize != NULL) 
    {
        *pSize = pVertexShader->ProgramSize;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderType

extern "C"
void WINAPI D3DDevice_GetVertexShaderType(
    DWORD Handle, 
    DWORD* pType)
{
    COUNT_API(API_D3DDEVICE_GETVERTEXSHADERTYPE);

    VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Must be a vertex shader program, not an FVF");
        }
    #if DBG
        if (pVertexShader->Signature != 'Vshd')
        {
            DPF_ERR("Invalid vertex shader object (already deleted?)");
        }
    #endif
    }

    DWORD type;

    if (pVertexShader->Flags & VERTEXSHADER_STATE)
    {
        type = D3DSMT_VERTEXSTATESHADER;
    }
    else if (pVertexShader->Flags & VERTEXSHADER_WRITE)
    {
        type = D3DSMT_READWRITE_VERTEXSHADER;
    }
    else 
    {
        type = D3DSMT_VERTEXSHADER;
    }

    if (pType != NULL) 
    {
        *pType = type;
    }
}

//------------------------------------------------------------------------------
// g_FixedFunctionPipelineConstants
//
// These constant register values were copied out of the NV miniport:

D3DCONST DWORD g_FixedFunctionPipelineConstants[] =
{

    0x00000000,   // 0.0
    0x3f000000,   // 0.5
    0x3f800000,   // 1.0
    0x40000000,   // 2.0
    
    0xbf800000,   // -1.0
    0x00000000,   //  0.0
    0x3f800000,   //  1.0
    0x40000000,   //  2.0
    
    0x00000000,   //  0.0
    0x00000000,   //  0.0
    0xbf800000,   // -1.0
    0x00000000,   //  0.0
};

//------------------------------------------------------------------------------
// D3DDevice_SetShaderConstantMode

extern "C"
void WINAPI D3DDevice_SetShaderConstantMode(
    D3DSHADERCONSTANTMODE Mode)
{
    COUNT_API(API_D3DDEVICE_SETSHADERCONSTANTMODE);

    CDevice* pDevice = g_pDevice;

    if (Mode & D3DSCM_NORESERVEDCONSTANTS)
    {
        pDevice->m_StateFlags |= STATE_NORESERVEDCONSTANTS;
    }
    else
    {
        pDevice->m_StateFlags &= ~STATE_NORESERVEDCONSTANTS;
    }

    Mode &= ~D3DSCM_NORESERVEDCONSTANTS;

    if (DBG_CHECK(TRUE))
    {
        if (Mode > D3DSCM_192CONSTANTSANDFIXEDPIPELINE)
        {
            DPF_ERR("Invalid mode");
        }
        if ((pDevice->m_StateFlags & STATE_RECORDBLOCK) &&
            (Mode != D3DSCM_96CONSTANTS))
        {
            DPF_ERR("Can't set constant mode while recording a state block");
        }
    }

    pDevice->m_ConstantMode = Mode;

    if (Mode == D3DSCM_96CONSTANTS)
    {
        // Dirty all of the fixed function T&L constants:

        D3D__DirtyFlags |= (D3DDIRTYFLAG_LIGHTS |
                            D3DDIRTYFLAG_TEXTURE_TRANSFORM |
                            D3DDIRTYFLAG_TRANSFORM);

        // Now reset any remaining fixed function T&L constants that aren't
        // covered by the above:

        PPUSH pPush = pDevice->StartPush();

        // Slot 60 is 'cSMAP', followed by 'cSKIN' and 'cREFL':

        Push1(pPush, NV097_SET_TRANSFORM_CONSTANT_LOAD, NV_IGRAPH_XF_XFCTX_CONS0);

        PushCount(pPush + 2, 
                  NV097_SET_TRANSFORM_CONSTANT(0), 
                  sizeof(g_FixedFunctionPipelineConstants) / 4);

        memcpy((void*) (pPush + 3),
               g_FixedFunctionPipelineConstants, 
               sizeof(g_FixedFunctionPipelineConstants));

        pPush += 3 + sizeof(g_FixedFunctionPipelineConstants) / 4;

        // Our texgen modes always assume an identity texgen plane (unlike
        // in the Nvidia driver, which has to muck with the plane to emulate
        // user clip planes).  

        PushMatrixTransposed(pPush, NV097_SET_TEXGEN_PLANE_S(0, 0), 
                             &g_IdentityMatrix);
        PushMatrixTransposed(pPush + 17, NV097_SET_TEXGEN_PLANE_S(1, 0), 
                             &g_IdentityMatrix);
        PushMatrixTransposed(pPush + 34, NV097_SET_TEXGEN_PLANE_S(2, 0), 
                             &g_IdentityMatrix);
        PushMatrixTransposed(pPush + 51, NV097_SET_TEXGEN_PLANE_S(3, 0), 
                             &g_IdentityMatrix);

        // When we enable FOG_GEN_MODE_PLANAR, we assume we've already 
        // constructed the plane:

        Push4f(pPush + 68, NV097_SET_FOG_PLANE(0), 0.0f, 0.0f, 1.0f, 0.0f);

        pDevice->EndPush(pPush + 73);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetShaderConstantMode

extern "C"
void WINAPI D3DDevice_GetShaderConstantMode(
    D3DSHADERCONSTANTMODE *pMode)
{
    COUNT_API(API_D3DDEVICE_GETSHADERCONSTANTMODE);

    if (DBG_CHECK(pMode == NULL))
    {
        DPF_ERR("Null pointer");
    }

    *pMode = g_pDevice->m_ConstantMode;
}

//------------------------------------------------------------------------------
// ParseDeclarationStream
//
// Parses the Vertex Shader declaration token stream that describes the
// vertex format, stream binding, and vertex shader constant registers,
// and correspondingly initializes the VertexShader structure.
//
// Analagous to CVertexShader::create.

VOID ParseDeclarationStream(
    VertexShader* pVertexShader,
    CONST DWORD* pDeclaration,
    BOOL IsFixedFunction)
{
    DWORD offset = 0;
    DWORD streamIndex = 0;
    DWORD count;
    DWORD sizeAndType;
    DWORD dimensionality;
    DWORD reg;
    DWORD slot;
    DWORD address;

    while (TRUE)
    {
        DWORD token = *pDeclaration++;
    
        switch (token & D3DVSD_TOKENTYPEMASK)
        {
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM):
            ASSERTMSG((token & 0x0ffffff0) == 0,
                      "Bad declaration STREAM token: 0x%lx", token);

            streamIndex = token & 0xf;
            offset = 0;
            break;
    
        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA):
            if (token & 0x10000000)
            {
                // Skip case (both D3DVSD_SKIP and D3DVSD_SKIPBYTES):

                ASSERTMSG((token & 0x07f0ffff) == 0,
                          "Bad declaration SKIP token: 0x%lx", token);

                count = (token & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;

                if (!(token & 0x08000000))
                {
                    count <<= 2;
                }

                offset += count;
            }
            else
            {
                ASSERTMSG((token & 0x0f00ffe0) == 0,
                          "Bad declaration REG token: 0x%lx", token);

                sizeAndType = (token & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT;
                reg = (token & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;

                if (DBG_CHECK(TRUE))
                {
                    if (reg > 15) 
                    {
                        DXGRIP("Can't specify a register more than 15");
                        reg = 15;
                    }
                    if (((sizeAndType & 0xf0) > 0x70) ||
                        ((sizeAndType & 0xf) > 6))
                    {
                        DXGRIP("Invalid type encoding");
                    }
                }

                pVertexShader->Slot[reg].Offset = offset;
                pVertexShader->Slot[reg].StreamIndex = streamIndex;
                pVertexShader->Slot[reg].SizeAndType = sizeAndType;
                pVertexShader->Slot[reg].Flags = AUTONONE;

                if (IsFixedFunction)
                {
                    if ((reg >= D3DVSDE_TEXCOORD0) && (reg <= D3DVSDE_TEXCOORD3))
                    {
                        // Compute dimensionality, watching for the 3W and 
                        // CMP special cases:

                        dimensionality = (sizeAndType & 0xf0) >> 4;
                        if (dimensionality == 7)
                            dimensionality = 4;
                        else if (sizeAndType == D3DVSDT_NORMPACKED3)
                            dimensionality = 3;

                        DWORD shift = 8 * (reg - D3DVSDE_TEXCOORD0);

                        ASSERT((pVertexShader->Dimensionality 
                                    & (0xff << shift)) == 0);

                        pVertexShader->Dimensionality 
                            |= (dimensionality << shift);
                    }
                    if (reg == D3DVSDE_DIFFUSE)
                    {
                        pVertexShader->Flags |= VERTEXSHADER_HASDIFFUSE;
                    }
                    if (reg == D3DVSDE_SPECULAR)
                    {
                        pVertexShader->Flags |= VERTEXSHADER_HASSPECULAR;
                    }
                    if (reg == D3DVSDE_BACKDIFFUSE)
                    {
                        pVertexShader->Flags |= VERTEXSHADER_HASBACKDIFFUSE;
                    }
                    if (reg == D3DVSDE_BACKSPECULAR)
                    {
                        pVertexShader->Flags |= VERTEXSHADER_HASBACKSPECULAR;
                    }
                }

                // Finally, advance the current offset based on the size of 
                // the attribute:

                offset += g_UnitsOfElement[(sizeAndType & 0xf0) >> 4]
                        * g_BytesPerUnit[sizeAndType & 0xf];
            }
            break;

        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR):
            reg = (token & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;

            pVertexShader->Slot[reg].Offset = 0;
            pVertexShader->Slot[reg].StreamIndex = 0;
            pVertexShader->Slot[reg].SizeAndType = SIZEANDTYPE_DISABLED;
            pVertexShader->Slot[reg].Source = (BYTE)
                ((token & D3DVSD_VERTEXREGINMASK) >> D3DVSD_VERTEXREGINSHIFT);

            // Is this a vertex normal definition or a UV definition?

            if (token & 0x10000000)
            {
                pVertexShader->Slot[reg].Flags = AUTOTEXCOORD;
            }
            else
            {
                pVertexShader->Slot[reg].Flags = AUTONORMAL;
            }
            break;

        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM):
            ASSERTMSG((token & 0x01ffff00) == 0,
                      "Bad delcaration CONST token: 0x%lx", token);

            count = (token & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT;

            pDeclaration += (4 * count);
            break;

        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_NOP):
            break;

        case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_END):
            return;

        default:
            NODEFAULT("Unidentified token\n");
            DXGRIP("Token type: 0x%lx", token);
        }
    } 
}

//------------------------------------------------------------------------------
// ParseDeclarationConstants
//
// Returns the size in DWORDS.

DWORD ParseDeclarationConstants(
    CONST DWORD* pDeclaration,
    PPUSH pPushStart)
{
    CDevice* pDevice = g_pDevice;
    PPUSH pPush = pPushStart;
    DWORD oldAddress = 0xdeadbeef;
    DWORD size = 0;

    while (TRUE)
    {
        DWORD token = *pDeclaration++;

        if (token == D3DVSD_END())
        {
            ASSERT((pPush == NULL) || (pPush == pPushStart + size));

            return size;
        }

        if (DBG_CHECK(TRUE))
        {
            // Try to catch some invalid tokens:

            if (((token & D3DVSD_TOKENTYPEMASK)
                    != D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_EXT)) &&
                ((token & 0x0000ff00) != 0))
            {
                DPF_ERR("Invalid declaration token (missing D3DVSD_END?)");
            }
        }
        
        if ((token & D3DVSD_TOKENTYPEMASK) 
                == D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM))
        {
            DWORD count = (token & D3DVSD_CONSTCOUNTMASK) >> 
                D3DVSD_CONSTCOUNTSHIFT;

            DWORD address = (token & D3DVSD_CONSTADDRESSMASK) >> 
                D3DVSD_CONSTADDRESSSHIFT;

            // Note that the macros already pre-bias the constant address
            // by 96:

            if (DBG_CHECK(TRUE))
            {
                if ((address >= VSHADER_CONSTANT_NUM) ||
                    (address + count > VSHADER_CONSTANT_NUM))
                {
                    DPF_ERR("Invalid constant address range (not between -96 "
                            "and 95) (missing D3DVSD_END?)");
                }
                if (!(pDevice->m_StateFlags & STATE_NORESERVEDCONSTANTS) &&
                    (address <= D3DVS_XBOX_RESERVEDCONSTANT2) &&
                    (address + count > D3DVS_XBOX_RESERVEDCONSTANT1))
                {
                    DPF_ERR("Address range can't include reserved addresses -38 "
                            "or -37 (D3DVS_XBOX_RESERVEDCONSTANT1 \nand "
                            "D3DVS_XBOX_RESERVEDCONSTANT2)  (missing D3DVSD_END?)");
                }
            }

            // Set the start address:

            if (address != oldAddress + 1)
            {
                if (pPush)
                {
                    pPush[0] = PUSHER_METHOD(SUBCH_3D, 
                                  NV097_SET_TRANSFORM_CONSTANT_LOAD, 1);
                    pPush[1] = address;
                    pPush += 2;
                }

                size += 2;
            }

            oldAddress = address;

            // The transform constant register is replicated only 32 times,
            // which means that we can only load 8 'slots' in a single batch:

            DWORD batchSize;

            do {
                batchSize = min(count, VSHADER_BATCH_LIMIT);

                if (pPush)
                {
                    *pPush++ = PUSHER_METHOD(SUBCH_3D, 
                               NV097_SET_TRANSFORM_CONSTANT(0), 4 * batchSize);

                    memcpy((void*) pPush, pDeclaration, 4 * batchSize * sizeof(DWORD));

                    pPush += 4 * batchSize;
                }

                pDeclaration += 4 * batchSize;

                size += 4 * batchSize + 1;

                address += batchSize;

            } while ((count -= batchSize) != 0);
        }
    }
}

//------------------------------------------------------------------------------
// RestoreVertexShaders
//
// Intended for use by state blocks and Present to restore any loaded
// vertex shader programs

VOID RestoreVertexShaders(
    CONST DWORD* pProgram,
    DWORD Dwords)
{
    CDevice* pDevice = g_pDevice;

    // Remember which slots have been loaded:

    #if DBG
        memset(&g_LoadedVertexShaderSlot[0], 1, Dwords / 4);
    #endif

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);

    pPush = ParseProgram(pPush + 2, pProgram, Dwords);

    PushedRaw(pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateVertexShader

extern "C"
HRESULT WINAPI D3DDevice_CreateVertexShader(
    CONST DWORD* pDeclaration,
    CONST DWORD* pFunction,
    DWORD* pHandle,
    DWORD Usage) 
{ 
    DWORD shaderType;
    DWORD shaderLength;

    COUNT_API(API_D3DDEVICE_CREATEVERTEXSHADER);

    if (pFunction != NULL)
    {
        // Format of vertex shader microcode is (little endian):
        // Byte offset
        //   0..1   WORD - magic number describing type of shader:
        //                 0x2078 - ordinary vertex shader
        //                 0x7778 - read/write vertex shader
        //                 0x7378 - vertex state shader
        //   2..3   WORD - length of shader, in instructions, ranges from 
        //                 1 to 136
        //   4..end the shader microcode.

        shaderType = *((WORD*) pFunction);
        shaderLength = *((WORD*) pFunction + 1);
    }

    if (DBG_CHECK(TRUE))
    {
        if (Usage & ~(D3DUSAGE_PERSISTENTDIFFUSE |
                      D3DUSAGE_PERSISTENTSPECULAR |
                      D3DUSAGE_PERSISTENTBACKDIFFUSE |
                      D3DUSAGE_PERSISTENTBACKSPECULAR))
        {
            DPF_ERR("Invalid usage flag");
        }
        if (pFunction != NULL)
        {
            if (((shaderType != VSHADER_TYPE_ORDINARY) &&
                 (shaderType != VSHADER_TYPE_WRITE) &&
                 (shaderType != VSHADER_TYPE_STATE)) ||
                (shaderLength == 0) ||
                (shaderLength > VSHADER_PROGRAM_SLOTS))
            {
                DPF_ERR("Invalid function header.  Only valid "
                        "Xbox-assembled function declarations are accepted.");
                DPF_ERR("(Perhaps you're calling with a DX8-defined token "
                        "stream?  Use Xgraphics assembler to compile into Xbox "
                        "form.)");
            }

            if ((shaderType == VSHADER_TYPE_STATE) && (pDeclaration != NULL))
            {
                DPF_ERR("State shaders must pass a NULL declaration pointer");
            }
        }
        if ((pDeclaration == NULL) &&
            ((pFunction == NULL) || (shaderType != VSHADER_TYPE_STATE)))
        {
            DPF_ERR("Declaration pointer can be NULL only for state shaders");
        }
    }

    DWORD programSize = 0;

    if (pFunction != NULL)
    {
        // Compute how many bytes we'll need for the program:

        programSize = 16 * shaderLength;

        // Every 32 dwords requires another DWORD to account for a new
        // NV097_SET_TRANSFORM_CONSTANT instruction:

        programSize += 4 * ((programSize + 127) / 128);
    }

    // See how much room we'll need for the constants:

    DWORD constantsSize = 0;

    if (pDeclaration != NULL)
    {
        constantsSize = 4 * ParseDeclarationConstants(pDeclaration, NULL);
    }

    // Compute the total size:

    DWORD programAndConstantsSize = programSize + constantsSize;

    // Sleazy programming note: We have an extra dword at the end because
    // the structure declaration declares 'DWORD ProgramAndConstants[1]':

    VertexShader* pVertexShader = (VertexShader*) MemAllocNoZero(
                        sizeof(*pVertexShader) + programAndConstantsSize);
    if (pVertexShader == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Zero-init the header:

    ZeroMemory(pVertexShader, sizeof(*pVertexShader));

#if DBG

    pVertexShader->Signature = 'Vshd';

#endif

    pVertexShader->RefCount = 1;

    ASSERT((D3DUSAGE_PERSISTENTDIFFUSE == VERTEXSHADER_HASDIFFUSE) &&
           (D3DUSAGE_PERSISTENTSPECULAR == VERTEXSHADER_HASSPECULAR) &&
           (D3DUSAGE_PERSISTENTBACKDIFFUSE == VERTEXSHADER_HASBACKDIFFUSE) &&
           (D3DUSAGE_PERSISTENTBACKSPECULAR == VERTEXSHADER_HASBACKSPECULAR));

    ASSERT((Usage & ~(D3DUSAGE_PERSISTENTDIFFUSE |
                      D3DUSAGE_PERSISTENTSPECULAR |
                      D3DUSAGE_PERSISTENTBACKDIFFUSE |
                      D3DUSAGE_PERSISTENTBACKSPECULAR)) == 0);

    pVertexShader->Flags = Usage;

    // First disable every slot.  Kelvin doesn't let us use a type of '0' with 
    // a size of '0', so we have to run through and stamp the values:

    for (DWORD i = 0; i < VERTEXSHADER_SLOTS; i++)
    {
        pVertexShader->Slot[i].SizeAndType = SIZEANDTYPE_DISABLED;
    }

    // Parse the stream that describes the vertex format, stream binding, and
    // vertex shader constant registers:

    if (pDeclaration != NULL)
    {
        ParseDeclarationStream(pVertexShader, 
                               pDeclaration, 
                               pFunction == NULL);
    }

    // Assume there's no function:

    DWORD functionDwords = 0;

    // Copy the function declaration, which includes the actual program
    // load and constant setting:

    if (pFunction != NULL)
    {
        pVertexShader->Flags |= VERTEXSHADER_PROGRAM;

        if (shaderType != VSHADER_TYPE_ORDINARY)
        {
            pVertexShader->Flags |= VERTEXSHADER_WRITE;
        }
        if (shaderType == VSHADER_TYPE_STATE)
        {
            pVertexShader->Flags |= VERTEXSHADER_STATE;
        }

        pVertexShader->ProgramSize = shaderLength;

        DWORD* pProgramAndConstants = pVertexShader->ProgramAndConstants;

        functionDwords = ParseProgram(pProgramAndConstants,
                                      pFunction + 1, 
                                      4 * shaderLength) - pProgramAndConstants;

        ASSERT(programSize == 4 * functionDwords);
    }

    // Now parse the constants from the declaration, and add that to
    // our 'ProgramAndConstants' push-buffer snippet:

    if (pDeclaration != NULL)
    {
        ParseDeclarationConstants(
            pDeclaration, 
            pVertexShader->ProgramAndConstants + functionDwords);
    }

    DWORD programAndConstantsDwords = programAndConstantsSize >> 2;

    pVertexShader->ProgramAndConstantsDwords = programAndConstantsDwords;

    // Zero the last byte for easy parsing:

    pVertexShader->ProgramAndConstants[programAndConstantsDwords] = 0;

    // The reserved bit lets us differentiate between an FVF and a vertex
    // shader handle:

    *pHandle = (DWORD) pVertexShader | D3DFVF_RESERVED0;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_DeleteVertexShader

extern "C"
void WINAPI D3DDevice_DeleteVertexShader(
    DWORD Handle) 
{ 
    COUNT_API(API_D3DDEVICE_DELETEVERTEXSHADER);

    CDevice* pDevice = g_pDevice;

    VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Invalid vertex shader handle value");
        }

        if ((pDevice->m_pVertexShader == pVertexShader) &&
            (pVertexShader->RefCount == 1))
        {
            DPF_ERR("Can't delete a vertex shader that's currently active");
        }

        #if DBG

            if (pVertexShader->Signature != 'Vshd')
            {
                DPF_ERR("Invalid vertex shader object (already deleted?)");
            }

        #endif
    }

    // We have to employ a reference count because of state blocks:

    if (--pVertexShader->RefCount == 0)
    {
    #if DBG

        pVertexShader->Signature = 'xxxx';

    #endif

        MemFree(pVertexShader);
    }
}

//------------------------------------------------------------------------------
// CommonSetPassthroughProgram
//
// Loads the correct passthrough program for handling XYZRHW, as appropriate 
// for the current fog state.
//
// Dependencies:
//
//      STATE_FOGSOURCEZ
//      STATE_MULTISAMPLING
//      D3DRS_FOGTABLEMODE
//      D3DRS_ZENABLE
//      SetRenderTarget

VOID FASTCALL CommonSetPassthroughProgram(
    CDevice* pDevice)
{
    DWORD programDwords; 
    CONST DWORD* pProgram; 

    if (pDevice->m_pVertexShader->Flags & VERTEXSHADER_PASSTHROUGH)
    {
        if (D3D__RenderState[D3DRS_FOGTABLEMODE] == D3DFOG_NONE)
        {
            programDwords = sizeof(g_PassthruProgramSpecularFog) / sizeof(DWORD);
            pProgram = g_PassthruProgramSpecularFog;
        }
        else if (pDevice->m_StateFlags & STATE_FOGSOURCEZ)
        {
            programDwords = sizeof(g_PassthruProgramZFog) / sizeof(DWORD);
            pProgram = g_PassthruProgramZFog;
        }
        else
        {
            programDwords = sizeof(g_PassthruProgramWFog) / sizeof(DWORD);
            pProgram = g_PassthruProgramWFog;
        }

        // Make sure that our header #define value is correct:

        ASSERT(programDwords <= D3DVS_XBOX_RESERVEDXYZRHWSLOTS * 4);
    
        // 136 program slots / 8 slots per batch = 17 batches at 1 dword each.
        // Plus the 13 dwords for the constant load plus other stuff:
    
        PPUSH pPush = pDevice->StartPush(30 + programDwords);

        // Load the scales and biases for passthrough mode, for use by the 
        // passthrough program:

        Push1(pPush, NV097_SET_TRANSFORM_CONSTANT_LOAD, 0);
        PushCount(pPush + 2, NV097_SET_TRANSFORM_CONSTANT(0), 8);

        FLOAT* pConstant = (FLOAT*) (pPush + 3);

        FLOAT wScale = (D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
                     ? pDevice->m_InverseWFar * pDevice->m_ZScale
                     : 1.0f;

        pConstant[0] = pDevice->m_SuperSampleScaleX;    // Scale
        pConstant[1] = pDevice->m_SuperSampleScaleY;
        pConstant[2] = pDevice->m_ZScale;
        pConstant[3] = wScale;

        // Adjust for the implicit 0.5 offset that the hardware multisampling 
        // applies:

        FLOAT multiSampleOffset = 0;

        if ((pDevice->m_StateFlags & STATE_MULTISAMPLING) &&
            (D3D__RenderState[D3DRS_MULTISAMPLEANTIALIAS]))
        {
            multiSampleOffset = 0.5f;
        }

        pConstant[4] = pDevice->m_ScreenSpaceOffsetX - multiSampleOffset;
        pConstant[5] = pDevice->m_ScreenSpaceOffsetY - multiSampleOffset;

        pConstant[6] = 0.0f;
        pConstant[7] = 0.0f;
    
        Push1(pPush + 11, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);

        pPush = ParseProgram(pPush + 13, pProgram, programDwords);
    
        PushedRaw(pPush);
    
        pDevice->EndPush(pPush);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexShader
//
// DOC: Calling SetVertexShader will invalidate any programs loaded via
//      LoadVertexShader.

extern "C"
void WINAPI D3DDevice_SetVertexShader(
    DWORD Handle) 
{ 
    VertexShader* pVertexShader;

    COUNT_API(API_D3DDEVICE_SETVERTEXSHADER);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_INPUT))
    {
        WARNING("SetVertexShader called while SetVertexShaderInput in effect,\n"
                "will be deferred");
    }

    if (Handle == pDevice->m_VertexShaderHandle)
    {
        COUNT_PERF(PERF_REDUNDANT_SETVERTEXSHADER);
    }

    // Get a copy of the current flags for later use, because 
    // InitializeVertexShaderFromFvf can modify the flags in-place:

    DWORD previousFlags = pDevice->m_pVertexShader->Flags;
    DWORD previousDimensionality = pDevice->m_pVertexShader->Dimensionality;

    // We can be passed either an FVF, or a handle to a vertex shader
    // program.  In the latter case, it's actually a pointer with the
    // low bit set:

    if (IsVertexProgram(Handle))
    {
        // Vertex shader program...

        pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

        if (DBG_CHECK(TRUE))
        {
        #if DBG
        
            if (pVertexShader->Signature != 'Vshd')
            {
                DPF_ERR("Invalid vertex shader object (already deleted?)");
            }

        #endif
        
            if (!(pVertexShader->Flags & VERTEXSHADER_PROGRAM) &&
                (pDevice->m_ConstantMode == D3DSCM_192CONSTANTS))
            {
                DPF_ERR("Can't use fixed function pipeline if "
                        "D3DSCM_192CONSTANTS is enabled");
            }
        }
    }
    else
    {
        // FVF...

        if (DBG_CHECK(TRUE))
        {
            ValidateFVF(Handle);

            if ((pDevice->m_ConstantMode == D3DSCM_192CONSTANTS) &&
                (Handle != 0) &&
                ((Handle & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW))
            {
                DPF_ERR("Can't use fixed function pipeline if "
                        "D3DSCM_192CONSTANTS is enabled");
            }
        }

        pVertexShader = &g_FvfVertexShader;

        InitializeVertexShaderFromFvf(pVertexShader, Handle);
    }

    D3D__DirtyFlags |= D3DDIRTYFLAG_VERTEXFORMAT;

    if (previousDimensionality != pVertexShader->Dimensionality)
    {
        // Ugh, the texture transform depends on the number of texture 
        // coordinates and the dimensionality:
    
        D3D__DirtyFlags |= D3DDIRTYFLAG_TEXTURE_TRANSFORM;
    }

    if (previousFlags != pVertexShader->Flags)
    {
        // The presence or absence of specular and diffuse components in the
        // vertex can affect the DX8 material behavior.  Plus vertex programs
        // and pass-through programs have special lighting implications:

        D3D__DirtyFlags |= D3DDIRTYFLAG_LIGHTS;

        // The passthrough program stomps on constants 0 and 1 (-96 and -95
        // in user-speak), so we have to regen the composite matrix (which
        // maps to constants 0 through 3) for the fixed function pipeline
        // when switching away from passthrough.  We always re-dirty when
        // the type changes because a vertex shader program won't cause 
        // LazySetTransform to re-initialize the composite matrix:

        D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;
    }

    // Remember our new state:

    pDevice->m_pVertexShader = pVertexShader;

    pDevice->m_VertexShaderHandle = Handle;

    if (pVertexShader->Flags & VERTEXSHADER_PROGRAM)
    {
        // SetVertexShader always defaults to slot zero:

        D3DDevice_LoadVertexShader(Handle, 0);
        D3DDevice_SelectVertexShader(Handle, 0);
    }
    else 
    {
        pDevice->m_VertexShaderStart = 0;   // set shadow value

        // If there's no diffuse or specular color in the vertex, 
        // substitute the D3D defaults.
        //
        // Note that because of persistent vertex attributes, we can't
        // simply whack both registers without checking first (because
        // one attribute may be persistent while the other may simply
        // not be present, requiring the default):
        PPUSH pPush = pDevice->StartPush();

        if (!(pVertexShader->Flags & VERTEXSHADER_HASDIFFUSE))
        {
            Push1(pPush, NV097_SET_VERTEX_DATA4UB(SLOT_DIFFUSE), -1);
            pPush += 2;
        }
        if (!(pVertexShader->Flags & VERTEXSHADER_HASSPECULAR))
        {
            Push1(pPush, NV097_SET_VERTEX_DATA4UB(SLOT_SPECULAR), 0);
            pPush += 2;
        }
        if (!(pVertexShader->Flags & VERTEXSHADER_HASBACKDIFFUSE))
        {
            Push1(pPush, NV097_SET_VERTEX_DATA4UB(SLOT_BACK_DIFFUSE), -1);
            pPush += 2;
        }
        if (!(pVertexShader->Flags & VERTEXSHADER_HASBACKSPECULAR))
        {
            Push1(pPush, NV097_SET_VERTEX_DATA4UB(SLOT_BACK_SPECULAR), 0);
            pPush += 2;
        }
        
        if (pVertexShader->Flags & VERTEXSHADER_PASSTHROUGH)
        {
            Push1(pPush, NV097_SET_TRANSFORM_PROGRAM_START, 0); 
    
            Push2(pPush + 2, 
                  NV097_SET_TRANSFORM_EXECUTION_MODE,
    
                  // NV097_SET_TRANSFORM_EXECUTION_MODE:
    
                  (DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
                   DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)),
    
                  // NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN:
    
                  DRF_DEF (097, _SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, _V, _READ_ONLY));
    
            // Reset the viewport if the fixed-function pipeline was previously
            // active:
    
            pPush = CommonSetViewport(pDevice, pPush + 5);

            pDevice->EndPush(pPush);
    
            // We always load the passthrough program and constants here, 
            // clobbering whatever may have already been in there:
    
            CommonSetPassthroughProgram(pDevice);
    
            // Now note the slots that we've clobbered:
    
            #if DBG
                memset(g_LoadedVertexShaderSlot, 0, D3DVS_XBOX_RESERVEDXYZRHWSLOTS);
            #endif
        }
        else
        {
            // Enable the fixed function pipeline:
    
            Push1(pPush, NV097_SET_TRANSFORM_EXECUTION_MODE, 
                  DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _FIXED) |
                  DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV));
    
            // If a vertex shader program (or passthrough) was previously active,
            // we have to re-initialize the viewport offset:
    
            pPush = CommonSetViewport(pDevice, pPush + 2);

            pDevice->EndPush(pPush);
        }
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShader

extern "C"
void WINAPI D3DDevice_GetVertexShader(
    DWORD* pHandle) 
{ 
    if (DBG_CHECK(pHandle == NULL))
    {
        DPF_ERR("NULL pointer");
    }

    COUNT_API(API_D3DDEVICE_GETVERTEXSHADER);

    CDevice* pDevice = g_pDevice;

    // Note that we do NOT increment the reference count (on account of the
    // fact that there's no Release() API for vertex shader objects!).

    *pHandle = pDevice->m_VertexShaderHandle;
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexShaderConstant
//
// DOC: Addresses D3DVS_XBOX_RESERVEDCONSTANT1 and D3DVS_XBOX_RESERVEDCONSTANT2
//      are reserved

extern "C"
void WINAPI D3DDevice_SetVertexShaderConstant(
    INT Register,
    CONST void* pConstantData,
    DWORD ConstantCount) 
{ 
    COUNT_API(API_D3DDEVICE_SETVERTEXSHADERCONSTANT);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        INT start = Register;
        INT end = Register + ConstantCount;

        if (pConstantData == NULL)
        {
            DPF_ERR("NULL pointer");
        }
        if ((ConstantCount == 0) || (ConstantCount > VSHADER_CONSTANT_NUM))
        {
            DPF_ERR("Invalid count");
        }
        if ((start < VSHADER_CONSTANT_MIN) || (end > VSHADER_CONSTANT_MAX))
        {
            DPF_ERR("Invalid address range (not between -96 and 95)");
        }
        if (pDevice->m_ConstantMode == D3DSCM_96CONSTANTS)
        {
            if (start < 0)
            {
                DPF_ERR("Negative addresses allowed only if not "
                        "D3DSCM_96CONSTANTS");
            }
        }
        else 
        {
            if (!(pDevice->m_StateFlags & STATE_NORESERVEDCONSTANTS) &&
                (start <= D3DVS_XBOX_RESERVEDCONSTANT2) &&
                (end > D3DVS_XBOX_RESERVEDCONSTANT1))
            {
                DPF_ERR("Address range can't include reserved addresses -38 or "
                        "-37 (D3DVS_XBOX_RESERVEDCONSTANT1 \nand "
                        "D3DVS_XBOX_RESERVEDCONSTANT2)");
            }
        }
    }

    // Registers 0 through 95 get mapped to the hardware's 96 through 191
    // (because the fixed function pipeline uses hardware register 0 through
    // 95).  And -96 through 95 gets mapped to 0 through 95 when not using
    // the fixed function pipeline:

    Register += VSHADER_CONSTANT_BIAS;

    // Convert to number of DWORDs:

    DWORD totalCount = 4 * ConstantCount;

    // Shadow the register constants only if we need to:

    if (!(pDevice->m_StateFlags & STATE_PUREDEVICE))
    {
        memcpy(&pDevice->m_VertexShaderConstants[Register][0],
               pConstantData,
               totalCount * sizeof(FLOAT));
    }

    // We can load a maximum of 192 slots here.  Since each batch
    // can download only 8 slots, we may need to do 24 batches, each
    // of which requires a DWORD of overhead.  We also need 2 DWORDs
    // for setting the initial address.  Hence the magic 26 value:

    PPUSH pPush = pDevice->StartPush(totalCount + 26);

    Push1(pPush, NV097_SET_TRANSFORM_CONSTANT_LOAD, Register);

    // I wrote the following in Asm for two compelling reasons:
    //
    // 1. This routine will be called a lot for vertex shader programs;
    // 2. After calling 'rep movsd', 'esi' and 'edi' are properly advanced,
    //    which C doesn't realize when invoking 'memcpy'.  Consequently,
    //    C does a bunch of extra work to remember the pointers and then 
    //    advance them after the 'rep movsd'.

    _asm {

        mov edi, pPush;
        mov esi, pConstantData
        mov eax, totalCount;
        add edi, 12;        
        cmp eax, 32;
        jle last_chunk;

    main_loop:

        mov ecx, 32;
        mov dword ptr [edi - 4], PUSHER_METHOD(SUBCH_3D, 
                                          NV097_SET_TRANSFORM_CONSTANT(0), 32);
        rep movsd;
        add edi, 4;
        sub eax, 32;
        cmp eax, 32;
        jg main_loop;

    last_chunk:

        mov ecx, eax;
        shl eax, 18;
        or eax, PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT(0), 0);
        mov dword ptr [edi - 4], eax;
        rep movsd;
        mov pPush, edi;
    }

    PushedRaw(pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderConstant

extern "C"
void WINAPI D3DDevice_GetVertexShaderConstant(
    INT Register,
    void* pConstantData,
    DWORD ConstantCount) 
{ 
    COUNT_API(API_D3DDEVICE_GETVERTEXSHADERCONSTANT);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        INT start = Register;
        INT end = Register + ConstantCount;

        if (pConstantData == NULL)
        {
            DPF_ERR("NULL pointer");
        }
        if ((ConstantCount == 0) || (ConstantCount > VSHADER_CONSTANT_NUM))
        {
            DPF_ERR("Invalid count");
        }
        if ((start < VSHADER_CONSTANT_MIN) || (end > VSHADER_CONSTANT_MAX))
        {
            DPF_ERR("Invalid range");
        }
        if (pDevice->m_ConstantMode == D3DSCM_96CONSTANTS)
        {
            if (start < 0)
            {
                DPF_ERR("Negative registers allowed only if not "
                        "D3DSCM_96CONSTANTS");
            }
        }
        else
        {
            if (!(pDevice->m_StateFlags & STATE_NORESERVEDCONSTANTS) &&
                (start <= D3DVS_XBOX_RESERVEDCONSTANT2) &&
                (end > D3DVS_XBOX_RESERVEDCONSTANT1))
            {
                DPF_ERR("Register range can't include reserved -38 or -37");
            }
        }
        if (pDevice->m_StateFlags & STATE_PUREDEVICE)
        {
            DPF_ERR("Can't query the constant register on a pure device");
        }
    }

    // Registers 0 through 95 get mapped to the hardware's 96 through 191
    // (because the fixed function pipeline uses hardware register 0 through
    // 95).  And -96 through 95 gets mapped to 0 through 95 when not using
    // the fixed function pipeline:

    Register += VSHADER_CONSTANT_BIAS;

    memcpy(pConstantData,
           &pDevice->m_VertexShaderConstants[Register][0],
           4 * ConstantCount * sizeof(FLOAT));
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderDeclaration
//
// The performance of this function is atrocious.  Ask me if I care.

extern "C"
HRESULT WINAPI D3DDevice_GetVertexShaderDeclaration(
    DWORD Handle,
    void* pData,
    DWORD* pSizeOfData) 
{ 
    COUNT_API(API_D3DDEVICE_GETVERTEXSHADERDECLARATION);

    CDevice* pDevice = g_pDevice;

    VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Invalid vertex shader handle value");
        }

        #if DBG

            if (pVertexShader->Signature != 'Vshd')
            {
                DPF_ERR("Invalid vertex shader object (already deleted?)");
            }

        #endif
        
        if (pSizeOfData == NULL)
        {
            DPF_ERR("Can't have a NULL pSizeOfData");
        }
    }

    // We can only return a declaration if there is actually one:

    if (pVertexShader->Flags & VERTEXSHADER_STATE)
    {
        *pSizeOfData = 0;
        return S_OK;
    }

    DWORD* pDst = NULL;

    // We do two passes, the first to count up the size of the structure
    // we'll need, the second to actually copy it:

    while (TRUE)
    {
        // Process stuff stream by stream:
    
        DWORD currentStream = 0xffffffff;
        DWORD index = 0;
        DWORD slot;
    
        for (DWORD stream = 0; stream < 16; stream++)
        {
            // We're on to a new stream, so zero our current end offset (which
            // is the offset one byte past the last attribute that we've 
            // processed so far for this stream):
    
            DWORD currentEndOffset = 0;
    
            // Loop and find all attributes for this stream:

            while (TRUE)
            {
                // Loop through and find the attribute in this stream with the
                // smallest offset equal to or greater than our current end
                // offset:
        
                DWORD offset = 0xffffffff;

                // Always go backwards so that we find the 'virtual' texture
                // slot first:

                DWORD i = VERTEXSHADER_SLOTS - 1;
                do {
                    if ((pVertexShader->Slot[i].StreamIndex == stream) &&
                        (pVertexShader->Slot[i].Offset < offset) &&
                        (pVertexShader->Slot[i].Offset >= currentEndOffset) &&
                        (pVertexShader->Slot[i].Flags == AUTONONE) &&
                        (pVertexShader->Slot[i].SizeAndType != 
                                                        SIZEANDTYPE_DISABLED))
                    {
                        offset = pVertexShader->Slot[i].Offset;
                        slot = i;
                    }

                } while (i-- != 0);

                // If we found no more attributes for this stream, move on to
                // the next stream:
        
                if (offset == 0xffffffff)
                    break;                      // ===========>
    
                // Insert the stream reference if it's new:
    
                if (stream != currentStream)
                {
                    currentStream = stream;
                    if (pDst != NULL)
                    {
                        pDst[index] = D3DVSD_STREAM(stream);
                    }
                    index++;
                }
    
                // Insert any padding that's necessary:
    
                DWORD padding = offset - currentEndOffset;
                while (padding > 0)
                {
                    // The maximum value of a skip is '15':
    
                    DWORD batchSize = min(padding, 15);
    
                    if (pDst != NULL)
                    {
                        pDst[index] = D3DVSD_SKIPBYTES(batchSize);
                    }
    
                    index++;
                    padding -= batchSize;
                }
    
                // Find the D3D type:
    
                DWORD sizeAndType = pVertexShader->Slot[slot].SizeAndType;
    
                // Insert the token:
    
                if (pDst != NULL)
                {
                    pDst[index] = D3DVSD_REG(slot, sizeAndType);
                }
                index++;
    
                // Advance the current offset to account for the size
                // of the attribute:
    
                currentEndOffset = offset 
                                 + g_UnitsOfElement[(sizeAndType & 0xf0) >> 4]
                                 * g_BytesPerUnit[sizeAndType & 0xf];

                ASSERT(currentEndOffset != offset);
            }
        }
    
        // Handle the tessellator tokens, which aren't related to a stream:
    
        for (slot = 0; slot < VERTEXSHADER_SLOTS; slot++)
        {
            if (pVertexShader->Slot[slot].Flags == AUTOTEXCOORD)
            {
                if (pDst != NULL)
                {
                    pDst[index] = D3DVSD_TESSUV(slot);
                }
                index++;
            }
            else if (pVertexShader->Slot[slot].Flags == AUTONORMAL)
            {
                if (pDst != NULL)
                {
                    pDst[index] = D3DVSD_TESSNORMAL(
                        pVertexShader->Slot[slot].Source, slot);
                }
                index++;
            }
        }
    
        // Now handle the constant registers:
    
        DWORD* pProgramAndConstants = pVertexShader->ProgramAndConstants;
        FLOAT* pConstants;
        DWORD constantSlot;
        DWORD methodEncode;

        while (methodEncode = *pProgramAndConstants++)
        {
            DWORD count = methodEncode >> 18;   // Number of dwords
            ASSERT(count != 0);
    
            if (count == 1)
            {
                constantSlot = *pProgramAndConstants;
    
                ASSERT(constantSlot < VSHADER_CONSTANT_NUM);
                ASSERT((methodEncode & 0xffff) 
                  == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT_LOAD, 
                                     0));
            }
            else if ((methodEncode & 0xffff)
                == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT(0), 0))
            {
                ASSERT(((count & 3) == 0) && (count < VSHADER_PROGRAM_SLOTS*4));
                ASSERT((methodEncode & 0xffff) 
                    == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT(0), 
                                       0));
    
                if (pDst != NULL)
                {
                    DWORD slotCount = count >> 2;
                    ASSERT(slotCount <= 15);
    
                    pDst[index] = D3DVSD_CONST(
                        constantSlot - VSHADER_CONSTANT_BIAS, slotCount);

                    memcpy(&pDst[index + 1], 
                           pProgramAndConstants, 
                           count * sizeof(DWORD));
                }
    
                index += 1 + count;
            }
    
            pProgramAndConstants += count;
        }

        // Finally, add the end token:

        if (pDst != NULL)
        {
            pDst[index] = D3DVSD_END();
        }
        index++;

        // See if we're done the second pass:

        if (pDst != NULL)
            return S_OK;

        // Okay, we've only completed the first pass so far, but now we
        // know the size of the buffer we'll need.

        DWORD sizeOfData = index * sizeof(DWORD);
        if (sizeOfData > *pSizeOfData)
        {
            *pSizeOfData = sizeOfData;
            return (pData == NULL) ? S_OK : D3DERR_MOREDATA;
        }

        *pSizeOfData = sizeOfData;
        if (pData == NULL)
            return S_OK;

        // Set up for the second pass:

        pDst = (DWORD*) pData;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderFunction

extern "C"
HRESULT WINAPI D3DDevice_GetVertexShaderFunction(
    DWORD Handle,
    void* pData,
    DWORD* pSizeOfData) 
{ 
    COUNT_API(API_D3DDEVICE_GETVERTEXSHADERFUNCTION);

    CDevice* pDevice = g_pDevice;

    VertexShader* pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Invalid vertex shader handle value");
        }

        #if DBG

            if (pVertexShader->Signature != 'Vshd')
            {
                DPF_ERR("Invalid vertex shader object (already deleted?)");
            }

        #endif
        
        if (pSizeOfData == NULL)
        {
            DPF_ERR("Can't have a NULL pSizeOfData");
        }
    }

    // We can only return a function if there's actually a program:

    if (!(pVertexShader->Flags & VERTEXSHADER_PROGRAM))
    {
        *pSizeOfData = 0;
        return S_OK;
    }

    // There's one DWORD of type/size, followed by 4 DWORDs per slot:

    DWORD size = 16 * pVertexShader->ProgramSize;

    DWORD sizeOfData = 4 + size;

    if (sizeOfData > *pSizeOfData)
    {
        *pSizeOfData = sizeOfData;
        return (pData == NULL) ? S_OK : D3DERR_MOREDATA;
    }

    *pSizeOfData = sizeOfData;
    if (pData == NULL)
        return S_OK;

    // Skip the first DWORD of the program, which is always
    // NV097_SET_TRANSFORM_PROGRAM:

    BYTE* pSrc = (BYTE*) (pVertexShader->ProgramAndConstants + 1);
    BYTE* pDst = (BYTE*) (pData);

    DWORD type = VSHADER_TYPE_ORDINARY;

    if (pVertexShader->Flags & VERTEXSHADER_STATE)
    {
        type = VSHADER_TYPE_STATE;
    }
    else if (pVertexShader->Flags & VERTEXSHADER_WRITE)
    {
        type = VSHADER_TYPE_WRITE;
    }

    *((DWORD*) pDst) = type | (pVertexShader->ProgramSize << 16);
    pDst += 4;

    // Skip the DWORDs between batches:

    DWORD batchSize;
    do {
        batchSize = min(size, 16 * VSHADER_BATCH_LIMIT);

        memcpy(pDst, pSrc, batchSize);
        pDst += batchSize;
        pSrc += batchSize + 4;

    } while ((size -= batchSize) != 0);

    ASSERT((DWORD) pDst - (DWORD) pData == sizeOfData);

    return S_OK;
}


//------------------------------------------------------------------------------
// D3DDevice_SetVertexShaderInputDirect

extern "C"
void WINAPI D3DDevice_SetVertexShaderInputDirect(
    D3DVERTEXATTRIBUTEFORMAT *pVAF,
    UINT StreamCount,
    CONST D3DSTREAM_INPUT *pStreamInputs)
{
    // This API call is counted as an API_D3DDEVICE_SETVERTEXSHADERINPUT
    
    if (DBG_CHECK(TRUE))
    {
#if DBG
        if(pVAF != NULL)
        {
            CheckVertexAttributeFormatStruct(pVAF);
        }
#endif    
    }

    if(pVAF != NULL)
    {
        memcpy(&(g_DirectVertexShader.Slot[0]), pVAF, sizeof(VertexShaderSlot)*16);
        D3DDevice_SetVertexShaderInput((DWORD)(&g_DirectVertexShader) | D3DFVF_RESERVED0, StreamCount, pStreamInputs);
    }
    else
    {
        D3DDevice_SetVertexShaderInput(0, StreamCount, pStreamInputs);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexShaderInput

extern "C"
void WINAPI D3DDevice_SetVertexShaderInput(
    DWORD Handle,
    UINT StreamCount,
    CONST D3DSTREAM_INPUT *pStreamInputs)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXSHADERINPUT);

    DWORD i;

    CDevice* pDevice = g_pDevice;

    // Let them go back to the old way of doing things:

    if (Handle == 0)
    {
        if (DBG_CHECK(TRUE))
        {
            if ((StreamCount != 0) || (pStreamInputs != NULL))
            {
                DPF_ERR("StreamCount and pStreamInputs must be zero when NULL "
                        "handle");
            }
        }

        D3D__DirtyFlags &= ~D3DDIRTYFLAG_DIRECT_INPUT;
        D3D__DirtyFlags |= D3DDIRTYFLAG_VERTEXFORMAT;

        // Release all VBs:

        for (i = 0; i < pDevice->m_VertexShaderInputCount; i++)
        {
            D3DVertexBuffer* pVertexBuffer 
                = pDevice->m_VertexShaderInputStream[i].VertexBuffer;

            pDevice->RecordResourceReadPush(pVertexBuffer);

            InternalRelease(pVertexBuffer);
        }

        pDevice->m_VertexShaderInputCount = 0;

        return;
    }

    VertexShader *pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Must be a vertex shader program, not an FVF");
        }
    #if DBG
        if (pVertexShader->Signature != 'Vshd')
        {
            DPF_ERR("Invalid vertex shader object (already deleted?)");
        }
    #endif
        if ((StreamCount == 0) || (StreamCount > 15))
        {
            DPF_ERR("Stream count can't be 0 or more than 15");
        }
        for (i = 0; i < StreamCount; i++)
        {
            if (pStreamInputs[i].VertexBuffer == NULL)
            {
                DPF_ERR("Can't specify NULL VertexBuffer");
            }
            if (pStreamInputs[i].Stride > 255)
            {
                DPF_ERR("Can't have a stride more than 255");
            }
        }
        if (((D3D__TextureState[0][D3DTSS_TEXCOORDINDEX] & 0xffff) != 0) ||
            ((D3D__TextureState[1][D3DTSS_TEXCOORDINDEX] & 0xffff) != 1) ||
            ((D3D__TextureState[2][D3DTSS_TEXCOORDINDEX] & 0xffff) != 2) ||
            ((D3D__TextureState[3][D3DTSS_TEXCOORDINDEX] & 0xffff) != 3))
        {
            DPF_ERR("D3DTSS_TEXCOORDINDEX mapping for every stage must be "
                    "identity");
        }
    }

    // Handle the reference counts.  Note that we do NOT reference count
    // the vertex shader (just like SetVertexShader).

    for (i = 0; i < StreamCount; i++)
    {
        InternalAddRef(pStreamInputs[i].VertexBuffer);
    }

    for (i = 0; i < pDevice->m_VertexShaderInputCount; i++)
    {
        D3DVertexBuffer* pVertexBuffer 
            = pDevice->m_VertexShaderInputStream[i].VertexBuffer;

        pDevice->RecordResourceReadPush(pVertexBuffer);

        InternalRelease(pVertexBuffer);
    }

    // Save our state:

    D3D__DirtyFlags &= ~D3DDIRTYFLAG_VERTEXFORMAT;
    D3D__DirtyFlags |= D3DDIRTYFLAG_DIRECT_INPUT;

    pDevice->m_VertexShaderInputHandle = Handle;
    pDevice->m_VertexShaderInputCount = StreamCount;
    memcpy(pDevice->m_VertexShaderInputStream, 
           pStreamInputs, 
           StreamCount * sizeof(pStreamInputs[0]));

    PPUSH pPush = pDevice->StartPush();
    DWORD format[16];

    // Hit the registers:

    PushCount(pPush, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0), 16);

    for (i = 0; i < 16; i++)
    {
        VertexShaderSlot *pSlot = &pVertexShader->Slot[i];

        if (DBG_CHECK(pSlot->StreamIndex >= StreamCount))
        {
            DPF_ERR("Vertex Shader references more streams than given to "
                    "SetVertexShaderInput");
        }

        CONST D3DSTREAM_INPUT* pStreamInput = &pStreamInputs[pSlot->StreamIndex];

        DWORD offset = pStreamInput->VertexBuffer->Data
                     + pStreamInput->Offset
                     + pSlot->Offset;

        pPush[1 + i] = offset;

        format[i] = (pStreamInput->Stride << 8) + pSlot->SizeAndType;
    }

    PushCount(pPush + 17, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);
    memcpy((VOID*) (pPush + 18), format, 16*sizeof(DWORD));

    pDevice->EndPush(pPush + 34);
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderInput

extern "C"
HRESULT WINAPI D3DDevice_GetVertexShaderInput(
    DWORD *pHandle,
    UINT *pStreamCount,
    D3DSTREAM_INPUT *pStreamInputs) // May be NULL
{
    COUNT_API(API_D3DDEVICE_GETVERTEXSHADERINPUT);

    CDevice* pDevice = g_pDevice;

    if (pStreamCount)
    {
        *pStreamCount = pDevice->m_VertexShaderInputCount;
    }
    if (pHandle)
    {
        *pHandle = pDevice->m_VertexShaderInputHandle;
    }
    if (pStreamInputs)
    {
        memcpy(pStreamInputs, 
               pDevice->m_VertexShaderInputStream, 
               pDevice->m_VertexShaderInputCount  * sizeof(pStreamInputs[0]));
    }

    return (D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_INPUT) ? S_OK : S_FALSE;
}

//------------------------------------------------------------------------------
// D3DDevice_GetVertexShaderInput
//
// Checks to make sure that valid formats have been provided in the 
// VertexAttributeFormat Struct and also that no formats overlap in a stream.
// pVAF must not be NULL.

#if DBG
void CheckVertexAttributeFormatStruct(D3DVERTEXATTRIBUTEFORMAT *pVAF)
{
    char buf[256];
    UINT i,j, begin, end, size;
    BYTE type;
    
    // check for valid types and sizes
    for(i=0; i<16; i++)
    {
        type = (BYTE)(pVAF->Input[i].Format & 0xf);
        size = pVAF->Input[i].Format >> 4;     // check upper bits, too
        if(((type == 0) && (size != 4)) ||
           ((type == 1) && ((size < 1) || (size > 4))) ||
           ((type == 2) && ((size == 5) || (size == 6) || (size > 7))) ||
           ((type == 3)) ||
           ((type == 4) && ((size < 1) || (size > 4))) ||
           ((type == 5) && ((size < 1) || (size > 4))) ||
           ((type == 6) && (size != 1)) ||
           (type > 6))
        {
            sprintf(buf, "Invalid Format in Input %d of D3DVERTEXATTRIBUTEFORMAT Structure", i);
            DPF_ERR(buf);
        }
        // Check for overlapping offsets
        if(pVAF->Input[i].Format != SIZEANDTYPE_DISABLED)
        {
            begin = pVAF->Input[i].Offset;
            end = begin - 1 +
                  g_UnitsOfElement[size]* 
                  g_BytesPerUnit[type]; // this is the extent of this attribute
            // check against other attributes
            for(j=i+1; j<16; j++)
            {
                if(pVAF->Input[j].Format != SIZEANDTYPE_DISABLED)
                {
                    if(pVAF->Input[i].StreamIndex != 
                       pVAF->Input[j].StreamIndex) continue; // different stream
                    
                    UINT begin2 = pVAF->Input[j].Offset;
                    if(begin2 > end) continue;  // can't overlap
                    
                    UINT end2 = begin2 - 1 + 
                                g_UnitsOfElement[pVAF->Input[j].Format >> 4] * 
                                g_BytesPerUnit[pVAF->Input[j].Format & 0xf];
                    if(end2 >= begin)
                    {
                        sprintf(buf, "Invalid Format in D3DVERTEXATTRIBUTEFORMAT Structure: Input %d and Input %d overlap", i, j);
                        DPF_ERR(buf);
                    }
                }
            }
        }
    }
}
#endif // DBG

} // end of namespace

