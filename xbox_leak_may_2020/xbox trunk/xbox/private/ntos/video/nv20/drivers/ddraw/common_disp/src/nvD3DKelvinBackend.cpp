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
//     those rights set forth herein./
//
// ********************************* Direct 3D ******************************
//
//  Module: nvD3DKelvinBackend.cpp
//      basically just a pointer to a shared file
//
// **************************************************************************
//
//  History:
//      Daniel Rohrer          14Aug2000         DX8 development
//
// **************************************************************************
#include "nvprecomp.h"

#include "..\..\..\common\src\nvD3DKelvinBackend.c"
//The include file goes to the shared code (native kelvin)

// this is the software fallback case which draws emulated triangles... VERY slow and not efficient.
static void nvKelvinBeginPrimImm(void *info, NV_PATCH_PRIMITIVE_TYPE primType)
{
    getDC()->nvPusher.makeSpace(2);
    switch (primType) {
    case NV_PATCH_PRIMITIVE_TSTRIP:
        getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_BEGIN_END);
        getDC()->nvPusher.push (1, NV056_SET_BEGIN_END_OP_TRIANGLE_STRIP);
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Begin TriStrip");
        break;
    case NV_PATCH_PRIMITIVE_TFAN:
        getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_BEGIN_END);
        getDC()->nvPusher.push (1, NV056_SET_BEGIN_END_OP_TRIANGLE_FAN);
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Begin TriFan");
        break;
    }
    getDC()->nvPusher.adjust(2);
}

static void nvKelvinEndPrimImm(void *info)
{
    getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_BEGIN_END);
    getDC()->nvPusher.push (1, NV097_SET_BEGIN_END_OP_END);
    getDC()->nvPusher.adjust(2);
    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: End Prim");
}

#define VIEW_AS_DWORD(f) (*(DWORD *)(&(f)))

//map our enables bits to kelvin rendering
static int inline_renderorder[] = {
    D3DVSDE_POSITION,
    D3DVSDE_BLENDWEIGHT,
    D3DVSDE_NORMAL,
    D3DVSDE_DIFFUSE,
    D3DVSDE_SPECULAR,
    D3DVSDE_PSIZE,           
    D3DVSDE_TEXCOORD0,
    D3DVSDE_TEXCOORD1,
    D3DVSDE_TEXCOORD2,
    D3DVSDE_TEXCOORD3
};

NV_INLINE int dumpAttribute(float *pFinalAttribs, CVertexShader *pShader, DWORD dwReg, int i){
    DWORD packedByte;
    DWORD index = dwReg*4;
    switch(pShader->getVAType(dwReg)){
    case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f",dwReg,pFinalAttribs[4*dwReg+0]);
        getDC()->nvPusher.push (i, VIEW_AS_DWORD(pFinalAttribs[index+0]));
        i++; break;
    case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f\t%f",dwReg,pFinalAttribs  [4*dwReg+0],pFinalAttribs[4*dwReg+1]);
        getDC()->nvPusher.push (i,   VIEW_AS_DWORD(pFinalAttribs[index+0]));
        getDC()->nvPusher.push (i+1, VIEW_AS_DWORD(pFinalAttribs[index+1]));
        i+=2; break;
    case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f\t%f\t%f",dwReg,pFinalAttribs[4*dwReg+0],pFinalAttribs[4*dwReg+1],pFinalAttribs[4*dwReg+2]);
        getDC()->nvPusher.push (i,   VIEW_AS_DWORD(pFinalAttribs[index+0]));
        getDC()->nvPusher.push (i+1, VIEW_AS_DWORD(pFinalAttribs[index+1]));
        getDC()->nvPusher.push (i+2, VIEW_AS_DWORD(pFinalAttribs[index+2]));
        i+=3; break;
    case NV_PATCH_VERTEX_FORMAT_FLOAT_4:
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f\t%f\t%f\t%f",dwReg,pFinalAttribs[4*dwReg+0],pFinalAttribs[4*dwReg+1],
                                                                 pFinalAttribs[4*dwReg+2],pFinalAttribs[4*dwReg+3]);
        getDC()->nvPusher.push (i,   VIEW_AS_DWORD(pFinalAttribs[index+0]));
        getDC()->nvPusher.push (i+1, VIEW_AS_DWORD(pFinalAttribs[index+1]));
        getDC()->nvPusher.push (i+2, VIEW_AS_DWORD(pFinalAttribs[index+2]));
        getDC()->nvPusher.push (i+3, VIEW_AS_DWORD(pFinalAttribs[index+3]));
        i+=4; break;
    case NV_PATCH_VERTEX_FORMAT_UBYTE:
    case NV_PATCH_VERTEX_FORMAT_D3DCOLOR: 
        //packed byte will be expanded by tess to 4 vec float
        //pack it back down so the backend FVF definition isn't different                                    
        packedByte  = (max(0,min((int)(pFinalAttribs[index + 0]*255.0),255))<<16);
        packedByte |= (max(0,min((int)(pFinalAttribs[index + 1]*255.0),255))<<8);
        packedByte |= (max(0,min((int)(pFinalAttribs[index + 2]*255.0),255))<<0);
        packedByte |= (max(0,min((int)(pFinalAttribs[index + 3]*255.0),255))<<24);
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%x",dwReg,packedByte);
        getDC()->nvPusher.push (i, packedByte);
        i++; break; 
    case D3DVSDT_SHORT2:   // 6
    case D3DVSDT_SHORT4:   // 7
    default:
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\tDefault: vertex shouldn't be here!!");
        nvAssert(0);
    }
    return i;
}

static void nvKelvinSendPrimImm(void *context, NV_PATCH_EVAL_OUTPUT *pData, int index)
{
    float *pFinalAttribs = &pData->vertexAttribs[index][0][0];
    NV_PATCH_INFO     *info = (NV_PATCH_INFO*)context;
    PNVD3DCONTEXT  pContext = (PNVD3DCONTEXT)info->context;    
    CVertexShader  *pShader = pContext->hwState.pVertexShader;                  
    int i,j,bUsesTexture;    
    DWORD dwReg;

    if (pShader->hasProgram()) {        
        getDC()->nvPusher.push (0, (info->vertexSize << 18) | (NV_DD_KELVIN << 13) | NVPUSHER_NOINC(NV097_INLINE_ARRAY));
        i = 1;
        for(j=0; j < 16; j++){                        
            if(info->evalEnables & 1<<j){
                i = dumpAttribute(pFinalAttribs, pShader, j, i);
            }
        }
        getDC()->nvPusher.adjust(info->vertexSize + 1);
    } else{
        getDC()->nvPusher.push (0, (info->vertexSize << 18) | (NV_DD_KELVIN << 13) | NVPUSHER_NOINC(NV097_INLINE_ARRAY));
        i=1;
        bUsesTexture = 1;
        for(j=0; j < 10; j++){            
            //THIS SHOULD ONLY EVER BE CALLED VIA KELVIN            
            //this will force the inline vertex stride to match kelvin setup in sequencer mode
            dwReg = inline_renderorder[j];                        
            if(!pShader->hasProgram() && dwReg >= D3DVSDE_TEXCOORD0 && dwReg <= D3DVSDE_TEXCOORD3){
                dwReg = defaultInputRegMap[dwReg];            
                bUsesTexture = 0;
                DWORD dwTCIndex, dwVAIndex, dwHWStage;
                for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
                    if ((pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage))) {
                        dwTCIndex = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;
                        dwVAIndex = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
                        if(dwVAIndex == dwReg){ bUsesTexture = 1; break;}
                    }
                }
            }
            else{
                dwReg = defaultInputRegMap[dwReg];            
            }
            if((info->evalEnables & 1<<dwReg) && bUsesTexture){
                i = dumpAttribute(pFinalAttribs, pShader, dwReg,i);
            }
        }
        getDC()->nvPusher.adjust(info->vertexSize + 1);
    }
}

static void nvKelvinStreamInit(void  *context)
{
    //getDC()->nvPusher.flush(TRUE,CPushBuffer::FLUSH_HEAVY_POLLING);
    //NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    return;
}

static void nvKelvinStreamDestroy(void *context)
{
    //getDC()->nvPusher.flush(TRUE,CPushBuffer::FLUSH_HEAVY_POLLING);
    //NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    return;
}

static void nvKelvinSendFrontFace(void *context, int reversed){
    PNVD3DCONTEXT pContext;
    NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    pContext = (PNVD3DCONTEXT)info->context;

    switch(pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]){
    case D3DCULL_NONE:
            break;
    case D3DCULL_CW:
            reversed=1-reversed;
    case D3DCULL_CCW:
        getDC()->nvPusher.push(0, (0x1 << 18) | (NV_DD_KELVIN << 13) | NV097_SET_FRONT_FACE);
        if(reversed){
            DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Cull CW");
            getDC()->nvPusher.push(1, NV097_SET_FRONT_FACE_V_CW);
        } else {
            DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Cull CCW");
            getDC()->nvPusher.push(1, NV097_SET_FRONT_FACE_V_CCW);
        }
        getDC()->nvPusher.adjust(2);
        break;
    default:
        nvAssert(0);    //should never be here
    }
}

static void nvKelvinBeginPatch(void *context){      
#ifdef DCR_SEMAPHORE
    NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;   
    //COUNTER IS STORED AT OFFSET 0x0 in the page of semaphores
    //SEMAPHORE IS STORED AT 0x4
#if defined(IS_OPENGL)
#if defined(linux)
    //NEED AN INTERLOCK EXCHANGE HERE!!!!
    //COULD WE JUST USE THE CLIP_SPIN LOCK INSTEAD??? TO 
    //MUTEX ANY CHANGES TO THE COUNTER VALUE
#endif
#if defined(OSX)
    //JAYANT SAYS MAC DOESN'T PREEMPTIVE SWITCH YET
    //SO UNTIL OSX IS READY NOTHING NEEDED HERE
#endif
#else
    info->counter = InterlockedIncrement(&(g_FDGlobalData.FDSynchCounter[0]));
#endif       
    //set the semaphore context DMA to the FD semaphore set
    getDC()->nvPusher.push (0, (0x00040000 | (NV_DD_KELVIN << 13) | NV206E_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (1, FD_SEMAPHORE_206E_CONTEXT_DMA);    // must be read-only
    getDC()->nvPusher.push (2, (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (3, FD_SEMAPHORE_097_CONTEXT_DMA);   
   
    getDC()->nvPusher.push (4, (0x00040000 | NV206E_SEMAPHORE_OFFSET));
    getDC()->nvPusher.push (5, 0x4);       
    getDC()->nvPusher.push (6, (0x00040000 | NV206E_SEMAPHORE_ACQUIRE));
    getDC()->nvPusher.push (7, info->counter-1);

    //reset the semaphore context DMA to the standard kelvin flipper
    getDC()->nvPusher.push (8, (0x00040000 | (NV_DD_KELVIN << 13) | NV206E_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (9, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_FROM_MEMORY);    //must be read-only
    getDC()->nvPusher.push (10, (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (11,  CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_IN_MEMORY);    //must be read-write

    getDC()->nvPusher.adjust (12);
    getDC()->nvPusher.start(TRUE);
#endif
    return;
}

static void nvKelvinEndPatch(void *context){
#ifdef DCR_SEMAPHORE
    NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;   

    //set the semaphore context DMA to the FD semaphore set
    getDC()->nvPusher.push (0, (0x00040000 | (NV_DD_KELVIN << 13) | NV206E_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (1, FD_SEMAPHORE_206E_CONTEXT_DMA);    
    getDC()->nvPusher.push (2, (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (3, FD_SEMAPHORE_097_CONTEXT_DMA);   

    getDC()->nvPusher.push (4, (0x00040000 | NV206E_SEMAPHORE_OFFSET));        
    getDC()->nvPusher.push (5, 0x4);
    getDC()->nvPusher.push (6, (0x00040000 | NV206E_SEMAPHORE_RELEASE));
    getDC()->nvPusher.push (7, info->counter);

    //reset the semaphore context DMA to the standard kelvin flipper
    getDC()->nvPusher.push (8, (0x00040000 | (NV_DD_KELVIN << 13) | NV206E_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (9, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_FROM_MEMORY);   //must be read-only
    getDC()->nvPusher.push (10, (0x00040000 | (NV_DD_KELVIN << 13) | NV097_SET_CONTEXT_DMA_SEMAPHORE));
    getDC()->nvPusher.push (11, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_IN_MEMORY);    //must be read-write
    
    getDC()->nvPusher.adjust (12);
    getDC()->nvPusher.start(TRUE);
#endif
    return;
}

NV_PATCH_BACKEND nvKelvinImm_Backend = {
    nvKelvinStreamInit,
    nvKelvinStreamDestroy,
    nvKelvinBeginPrimImm,
    nvKelvinEndPrimImm,
    nvKelvinSendPrimImm,
    nvKelvinSendFrontFace,
    nvKelvinBeginPatch,
    nvKelvinEndPatch
};
