// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvVShad.cpp
//
// **************************************************************************
//
//  History:
//      Hadden Hoppert?           May00           DX8 development
//
// **************************************************************************

#include "nvprecomp.h"

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CVertexShader  *********************************************************
 *****************************************************************************
 *****************************************************************************
 ****************************************************************************/

CVertexShader::CVertexShader()
{
    this->reset();
}

CVertexShader::reset( void )
{
    m_dwHandle    = 0;
    m_dwFVF       = 0;
    m_dwCodeSize  = 0;
    m_pCodeData   = NULL;
    m_dwStride    = 0;
    m_bHasConsts  = FALSE;
    m_one         = 1.0f;
    m_zero        = 0.0f;
    m_ProgramOutput.residentProgram = NULL;
    m_ProgramOutput.residentNumInstructions = 0;
    m_ProgramOutput.residentSize = 0;

    for (int i = 0; i < NV_CAPS_MAX_STREAMS; i++) {
        // sources of the data loaded into the 16 vertex attribute registers
        m_vaSourceData[i].dwStream = VA_STREAM_NONE;
        m_vaSourceData[i].dwOffset = 0;
        m_vaSourceData[i].dwType = 0;
        m_vaSourceData[i].dwSrc  = 0;
        m_vaSourceData[i].flag = AUTONONE;
    }

#if (NVARCH >= 0x020)
    m_pKelvinProgram = NULL;
#endif

    return TRUE;
}

/*****************************************************************************
 * CVertexShader::create
 *
 * create a vertex shader from code
 */
static char *typeDecode[8] = { "D3DVSDT_FLOAT1  ", "D3DVSDT_FLOAT2  ", "D3DVSDT_FLOAT3  ",
                               "D3DVSDT_FLOAT4  ", "D3DVSDT_D3DCOLOR", "D3DVSDT_UBYTE4  ",
                               "D3DVSDT_SHORT2  ", "D3DVSDT_SHORT4  "};

#define SwapSources(a,b)    {   VASOURCEINFO temp;                                      \
                            DWORD dwTemp;                                           \
                    memcpy(&temp, &m_vaSourceData[a], sizeof(VASOURCEINFO));                    \
                            memcpy(&m_vaSourceData[a], &m_vaSourceData[b],sizeof(VASOURCEINFO));                    \
                            memcpy(&m_vaSourceData[b], &temp, sizeof(VASOURCEINFO));                            \
                            for(i=0; i < 16; i++){  if((unsigned int)m_CallerKelvin.inputRegMap[i] == a) break; }           \
                            for(j=0; j < 16; j++){  if((unsigned int)m_CallerKelvin.inputRegMap[j] == b)        break; }    \
                            dwTemp = m_CallerKelvin.inputRegMap[i];                             \
                            m_CallerKelvin.inputRegMap[i] = m_CallerKelvin.inputRegMap[j];                  \
                            m_CallerKelvin.inputRegMap[j] = dwTemp;                             \
                }

CVertexShader::create
(
    PNVD3DCONTEXT pContext,
    DWORD         dwHandle,
    DWORD         dwDeclSize,
    DWORD         dwCodeSize,
    DWORD        *lpDecl,
    DWORD        *lpCode
)
{
    DWORD dwCurrentStream, dwCurrentOffset;
    DWORD dwToken, dwLoadType, dwType, dwReg, dwSwappedReg, dwCount, i, j, dwSrc, dwSwappedSrc;

    // cache the handle
    m_dwHandle = dwHandle;
    nvAssert(m_dwFVF == 0);
    nvAssert( m_dwHandle & 0x1 );

    DPF_LEVEL(NVDBG_LEVEL_VSHADER_INS, "Create vertex shader 0x%02x", dwHandle);

    m_dwCodeSize = dwCodeSize;
    if (dwCodeSize) {
        m_pCodeData = (DWORD *)AllocIPM(dwCodeSize);
        memcpy (m_pCodeData, lpCode, dwCodeSize);  //lpDecl ?
    }

    // parse the declaration
    m_dwStride = 0;

    for (i = 0; i < dwDeclSize; i++, lpDecl++) {

        dwToken = *lpDecl;

        switch ((dwToken & D3DVSD_TOKENTYPEMASK) >> D3DVSD_TOKENTYPESHIFT) {

        case D3DVSD_TOKEN_NOP:
            // NOP or extension
            //nvAssert (dwToken == 0);
            break;

        case D3DVSD_TOKEN_STREAM:
            // stream selector
            dwCurrentStream = (dwToken & D3DVSD_STREAMNUMBERMASK) >> D3DVSD_STREAMNUMBERSHIFT;
            dwCurrentOffset = 0;
            break;

        case D3DVSD_TOKEN_STREAMDATA:
            // stream data definition (map to vertex input memory)
            dwLoadType = (dwToken & D3DVSD_DATALOADTYPEMASK) >> D3DVSD_DATALOADTYPESHIFT;
            if (dwLoadType == 0) {
                // vertex input register load
                dwType = (dwToken & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT;
                dwReg  = (dwToken & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;
                dwSwappedReg  = defaultInputRegMap[dwReg];        //remap inputs to conform to internal formats
                m_vaSourceData[dwSwappedReg].dwStream = dwCurrentStream;
                m_vaSourceData[dwSwappedReg].dwOffset = dwCurrentOffset;
                m_vaSourceData[dwSwappedReg].dwType   = dwType;
                m_vaSourceData[dwSwappedReg].flag     = CVertexShader::AUTONONE;
                DPF_LEVEL (NVDBG_LEVEL_VSHADER_INS, "reg: %02d->%02d , stream: %02d, type: %s, offset %02x, src: %x, flag: %d",
                           dwReg, dwSwappedReg, m_vaSourceData[dwSwappedReg].dwStream, typeDecode[m_vaSourceData[dwSwappedReg].dwType], m_vaSourceData[dwSwappedReg].dwOffset,
                           m_vaSourceData[dwSwappedReg].dwSrc, m_vaSourceData[dwSwappedReg].flag);
                DWORD dwVASize = getVASize(dwSwappedReg);
                dwCurrentOffset += dwVASize;
                m_dwStride      += dwVASize;
            } else {
                // data skip (no register load)
                dwCount = (dwToken & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;
                dwCurrentOffset += dwCount*sizeof(DWORD);
            }
            break;

        case D3DVSD_TOKEN_TESSELLATOR:
            dwReg = (dwToken & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;
            dwSwappedReg = defaultInputRegMap[dwReg];        //remap inputs to conform to internal formats
            m_vaSourceData[dwSwappedReg].dwStream = dwCurrentStream;
            m_vaSourceData[dwSwappedReg].dwOffset = NULL;
            dwSrc = (dwToken & D3DVSD_VERTEXREGINMASK) >> D3DVSD_VERTEXREGINSHIFT;
            dwSwappedSrc = defaultInputRegMap[dwSrc];
            m_vaSourceData[dwSwappedReg].dwSrc = dwSwappedSrc;
            m_vaSourceData[dwSwappedReg].dwType   = (dwToken & D3DVSD_DATATYPEMASK)    >> D3DVSD_DATATYPESHIFT;
            if (dwToken & 0x10000000) { // is this a vertex normal def or a UV def
                m_vaSourceData[dwSwappedReg].flag = CVertexShader::AUTOTEXCOORD;
                m_dwStride += 2 * sizeof(DWORD);
            }
            else {
                m_vaSourceData[dwSwappedReg].flag = CVertexShader::AUTONORMAL;
                m_dwStride += 3 * sizeof(DWORD);
            }
            DPF_LEVEL (NVDBG_LEVEL_VSHADER_INS, "reg: %02d->%02d, stream: %02d, type: %s, offset %02x, src: %x->%x, flag: %s",
                       dwReg, dwSwappedReg, m_vaSourceData[dwSwappedReg].dwStream, typeDecode[m_vaSourceData[dwSwappedReg].dwType],
                       m_vaSourceData[dwSwappedReg].dwOffset, dwSrc, dwSwappedSrc,
                       (m_vaSourceData[dwSwappedReg].flag == CVertexShader::AUTOTEXCOORD ? "AUTOTEX" : "AUTONORM" ));
            break;

        case D3DVSD_TOKEN_CONSTMEM:
            // constant memory from shader
            if (!m_bHasConsts) {
                memset(m_numConsts, 0, sizeof(m_numConsts));
                m_bHasConsts = TRUE;
            }
            dwCount = (dwToken & D3DVSD_CONSTCOUNTMASK)   >> D3DVSD_CONSTCOUNTSHIFT;
            dwReg   = (dwToken & D3DVSD_CONSTADDRESSMASK) >> D3DVSD_CONSTADDRESSSHIFT;
            nvAssert(dwReg < D3DVS_CONSTREG_MAX_V1_1);
            nvAssert((dwReg + dwCount) <= D3DVS_CONSTREG_MAX_V1_1);
            memcpy((DWORD *)&m_vertexShaderConstants[dwReg], lpDecl + 1, dwCount * sizeof(VSHADERREGISTER));
            m_numConsts[dwReg] = dwCount;
            lpDecl += 4 * dwCount;
            break;

        case D3DVSD_TOKEN_EXT:
            // extension
            dwCount = (dwToken & D3DVSD_EXTCOUNTMASK) >> D3DVSD_EXTCOUNTSHIFT;
            lpDecl += dwCount;
            break;

        case D3DVSD_TOKEN_END:
            // end-of-array (requires all DWORD bits to be 1)
            i = dwDeclSize; // force exit of for loop
            break;
        }
    }

    //create a kelvin program and upload it
#if (NVARCH >= 0x020)
    if (dwCodeSize)
    {
        m_pKelvinProgram = (PKELVIN_PROGRAM)AllocIPM(sizeof(KELVIN_PROGRAM));

        //parse the D3D code into kelvin meta-code
        nvKelvinParseVertexShaderCode (m_pKelvinProgram, &m_ParsedProgram, lpCode, dwCodeSize);

        m_ParsedProgram.firstInstruction = m_pKelvinProgram->code;
        m_ParsedProgram.liveOnEntry      = FALSE;
        m_ParsedProgram.IsStateProgram   = FALSE; // compiler will add viewport transform code
        vp_Optimize(&m_ParsedProgram, m_pKelvinProgram->dwNumInstructions, 0);

        //compile to kelvin microcode
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
        {
            // set up kelvin compiler
            m_CallerKelvin.caller_id = 0xD3D;
            m_CallerKelvin.malloc    = glLikeAllocIPM;
            m_CallerKelvin.free      = glLikeFreeIPM;
            m_CallerKelvin.mallocArg = NULL;

            memcpy (m_CallerKelvin.inputRegMap,  defaultInputRegMap,  sizeof(defaultInputRegMap));
            memcpy (m_CallerKelvin.outputRegMap, defaultOutputRegMap, sizeof(defaultOutputRegMap));

            DWORD dwStream = getAutoNormalStream();
            //if the autonormal stream is NOT equal to stream 2 (where we can guard curve) swap 2 and wherever it was?
            if(dwStream != VA_STREAM_NONE && dwStream != 2){

                //swap normal stream into position 2
                SwapSources(dwStream,2);

                //if we have created an autonormal make sure that its src is from stream 0 and swap as necessary.
                if(m_vaSourceData[2].dwSrc != 0x0){     //if src for an autonormal is NOT position 0 --> move it there.
                    dwStream = m_vaSourceData[2].dwSrc;
                    SwapSources(dwStream,0);
                }
                m_vaSourceData[2].dwSrc = 0x0;  //input will be swapped below to come from channel 0

            }

            if (!vp_CompileKelvin(&m_CallerKelvin, &m_ParsedProgram, m_pKelvinProgram->dwNumInstructions, &m_ProgramOutput)) {
                DPF ("Failed to compile kelvin vertex shader code");
                dbgD3DError();
            }
        }
        else
        {
            // set up x86 assembly compiler
            m_Caller.caller_id         = 0xD3D;
            m_Caller.malloc            = glLikeAllocIPM;
            m_Caller.free              = glLikeFreeIPM;
            m_Caller.mallocArg         = NULL;
            m_Caller.exec_malloc       = glLikeAllocIPM;
            m_Caller.expf              = expf;
            m_Caller.logf              = logf;
            // we use addresses instead of offsets and then just pass in 0 for the base address later
            m_Caller.attrib_offset     = int(m_Attrib);
            m_Caller.temp_offset       = int(pContext->pVShaderConsts->vertexShaderTemp);
            m_Caller.result_offset     = int(m_Result);
            m_Caller.param_offset      = int(pContext->pVShaderConsts->vertexShaderConstants);
            m_Caller.float_one_offset  = int(&m_one);
            m_Caller.float_zero_offset = int(&m_zero);

            if (!vp_CompileX86(&m_Caller, &m_ParsedProgram, m_pKelvinProgram->dwNumInstructions, &m_ProgramOutput)) {
                DPF ("Failed to compile X86 vertex shader code");
                dbgD3DError();
            }
        }
    }
#endif
    return TRUE;
}

/*****************************************************************************
 * CVertexShader::create
 *
 * create a vertex shader from FVF format
 */
CVertexShader::create
(
    PNVD3DCONTEXT pContext,
    DWORD         dwFVF,
    DWORD         dwHandle
)
{
    static BYTE bTextureSize[4]  = {2*4, 3*4, 4*4, 1*4};
    static BYTE bTextureCount[4] = { 2,   3,   4,   1 };
    static BYTE bTextureType[4]  = { D3DVSDT_FLOAT2,
                                     D3DVSDT_FLOAT3,
                                     D3DVSDT_FLOAT4,
                                     D3DVSDT_FLOAT1 };
    static BYTE bWeightType[6]   = { 0,
                                     D3DVSDT_FLOAT1,
                                     D3DVSDT_FLOAT2,
                                     D3DVSDT_FLOAT3,
                                     D3DVSDT_FLOAT4,
                                     D3DVSDT_FLOAT4 };

    DWORD dwTextureFormats,dwNumTexSets;
    DWORD dwOffset, dwBCount, i;

    nvAssert( dwFVF );

    // if the FVF handle hasn't changed, just return
    if (m_dwFVF == dwFVF) {
        return (TRUE);
    }

    m_dwHandle   = dwHandle;
    m_dwFVF      = dwFVF;
    m_dwCodeSize = 0;

    dwOffset = 0;

    // x,y,z
    m_vaSourceData[defaultInputRegMap[D3DVSDE_POSITION]].dwStream = 0;
    m_vaSourceData[defaultInputRegMap[D3DVSDE_POSITION]].dwType   = D3DVSDT_FLOAT3;
    m_vaSourceData[defaultInputRegMap[D3DVSDE_POSITION]].dwOffset = dwOffset;
    dwOffset += 3*sizeof(DWORD);

    // default to having no geometry other than x,y,z
    m_vaSourceData[defaultInputRegMap[D3DVSDE_BLENDWEIGHT]].dwStream = VA_STREAM_NONE;
    dwBCount = 0;

    // check for geometry beyond just x,y,z
    switch (dwFVF & D3DFVF_POSITION_MASK) {
        case D3DFVF_XYZ:
            break;
        case D3DFVF_XYZRHW:
            m_vaSourceData[defaultInputRegMap[D3DVSDE_POSITION]].dwType = D3DVSDT_FLOAT4;
            dwOffset += 1*sizeof(DWORD);
            break;
        case D3DFVF_XYZB5:   // we have x,y,z,b1,b2,b3,b4,b5
            dwBCount++;
        case D3DFVF_XYZB4:   // we have x,y,z,b1,b2,b3,b4
            dwBCount++;
        case D3DFVF_XYZB3:   // we have x,y,z,b1,b2,b3
            dwBCount++;
        case D3DFVF_XYZB2:   // we have x,y,z,b1,b2
            dwBCount++;
        case D3DFVF_XYZB1:   // we have x,y,z,b1
            dwBCount++;
            m_vaSourceData[defaultInputRegMap[D3DVSDE_BLENDWEIGHT]].dwStream = 0;
            m_vaSourceData[defaultInputRegMap[D3DVSDE_BLENDWEIGHT]].dwType   = bWeightType[dwBCount];
            m_vaSourceData[defaultInputRegMap[D3DVSDE_BLENDWEIGHT]].dwOffset = dwOffset;
            dwOffset += dwBCount * sizeof(D3DVALUE);
            break;
        default:
            DPF ("unknown position mask in FVF");
            dbgD3DError();
            break;
    }  // switch

    // normal
    if (dwFVF & D3DFVF_NORMAL) {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_NORMAL]].dwStream = 0;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_NORMAL]].dwType   = D3DVSDT_FLOAT3;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_NORMAL]].dwOffset = dwOffset;
        dwOffset += 3*sizeof(DWORD);
    }
    else {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_NORMAL]].dwStream = VA_STREAM_NONE;
    }

    // point size
    if (dwFVF & D3DFVF_PSIZE) {
        // must check runtime version since annoying DX7 apps (Isle of Morg)
        // will set the RESERVED1 flag which is the same as the PSIZE flag
        if (pContext->dwDXAppVersion >= 0x800) {
            m_vaSourceData[defaultInputRegMap[D3DVSDE_PSIZE]].dwStream = 0;
            m_vaSourceData[defaultInputRegMap[D3DVSDE_PSIZE]].dwType   = D3DVSDT_FLOAT1;
            m_vaSourceData[defaultInputRegMap[D3DVSDE_PSIZE]].dwOffset = dwOffset;
        }
        dwOffset += 1*sizeof(D3DVALUE);
    }
    else {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_PSIZE]].dwStream = VA_STREAM_NONE;
    }

    // diffuse
    if (dwFVF & D3DFVF_DIFFUSE) {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_DIFFUSE]].dwStream = 0;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_DIFFUSE]].dwType   = D3DVSDT_D3DCOLOR;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_DIFFUSE]].dwOffset = dwOffset;
        dwOffset += 1*sizeof(DWORD);
    }
    else {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_DIFFUSE]].dwStream = VA_STREAM_NONE;
    }

    // specular
    if (dwFVF & D3DFVF_SPECULAR) {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_SPECULAR]].dwStream = 0;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_SPECULAR]].dwType   = D3DVSDT_D3DCOLOR;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_SPECULAR]].dwOffset = dwOffset;
        dwOffset += 1*sizeof(DWORD);
    }
    else {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_SPECULAR]].dwStream = VA_STREAM_NONE;
    }

    // texture coordinates
    dwNumTexSets     = FVF_TEXCOORD_NUMBER(dwFVF);
    dwTextureFormats = dwFVF >> 16;
    for (i=0; i<dwNumTexSets; i++) {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_TEXCOORD0+i]].dwStream = 0;
        m_vaSourceData[defaultInputRegMap[D3DVSDE_TEXCOORD0+i]].dwType   = bTextureType[dwTextureFormats & 0x3];
        m_vaSourceData[defaultInputRegMap[D3DVSDE_TEXCOORD0+i]].dwOffset = dwOffset;
        dwOffset += bTextureSize[dwTextureFormats & 0x3];
        dwTextureFormats >>= 2;
    }
    for (i=dwNumTexSets; i<NV_CAPS_MAX_UV_SETS; i++) {
        m_vaSourceData[defaultInputRegMap[D3DVSDE_TEXCOORD0+i]].dwStream = VA_STREAM_NONE;
    }

    m_dwStride = dwOffset;

    // set dirty state as required
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TL_MODE |
                                      CELSIUS_DIRTY_FVF     |
                                      CELSIUS_DIRTY_LIGHTS  |
                                      CELSIUS_DIRTY_TEXTURE_STATE;
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF     |
                                      KELVIN_DIRTY_LIGHTS  |
                                      KELVIN_DIRTY_TEXTURE_STATE;
#endif

    return TRUE;
}

/*****************************************************************************
 * CVertexShader::~CVertexShader
 *
 * set the constants of a shader
 */
CVertexShader::~CVertexShader(void)
{
    if (m_pCodeData) {
        FreeIPM (m_pCodeData);
    }

    if (m_ProgramOutput.residentProgram) {
        FreeIPM(m_ProgramOutput.residentProgram);
        m_ProgramOutput.residentProgram = NULL;
        m_ProgramOutput.residentNumInstructions = 0;
        m_ProgramOutput.residentSize = 0;
    }

#if (NVARCH >= 0x020)
    if (m_pKelvinProgram) {
        FreeIPM (m_pKelvinProgram);
    }
#endif
}
