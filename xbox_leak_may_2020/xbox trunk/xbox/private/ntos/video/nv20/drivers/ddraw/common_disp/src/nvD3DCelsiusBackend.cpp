//**************************************************************************
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
// **************************************************************************
//
//  Module: nvGLBackend.c
//        gl immediate mode entry points for the patch shared library
//
//  History:
//        Daniel Rohrer    (drohrer)      23Jun00      created
//
// **************************************************************************

#include "nvprecomp.h"
#ifdef HOSURF_ENABLE

static void nvCelsiusBeginPrimImm(void *info, NV_PATCH_PRIMITIVE_TYPE primType)
{
    getDC()->nvPusher.makeSpace(2);
    switch (primType) {
    case NV_PATCH_PRIMITIVE_TSTRIP:
        getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_CELSIUS << 13) | NV056_SET_BEGIN_END4);
        getDC()->nvPusher.push (1, NV056_SET_BEGIN_END_OP_TRIANGLE_STRIP);
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Begin TriStrip");
        break;
    case NV_PATCH_PRIMITIVE_TFAN:
        getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_CELSIUS << 13) | NV056_SET_BEGIN_END4);
        getDC()->nvPusher.push (1, NV056_SET_BEGIN_END_OP_TRIANGLE_FAN);
        DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Begin TriFan");
        break;
    }
    getDC()->nvPusher.adjust(2);
}

static void nvCelsiusEndPrimImm(void *info)
{
    getDC()->nvPusher.push (0, 0x00040000 | (NV_DD_CELSIUS << 13) | NV056_SET_BEGIN_END4);
    getDC()->nvPusher.push (1, NV056_SET_BEGIN_END_OP_END);
    getDC()->nvPusher.adjust(2);
    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: End Prim");
}

#define VIEW_AS_DWORD(f) (*(DWORD *)(&(f)))

// map our enables bits to celsius rendering
static int inline_renderorder[] = {
    D3DVSDE_BLENDWEIGHT,
    D3DVSDE_NORMAL,
    D3DVSDE_TEXCOORD1,
    D3DVSDE_TEXCOORD0,
    D3DVSDE_SPECULAR,
    D3DVSDE_DIFFUSE,
    D3DVSDE_POSITION
};

// This won't work quite right until we have state aliasing for vertex programs.
extern DWORD method_dispatch [9][16][2];
static void nvCelsiusSendPrimImm(void *context, NV_PATCH_EVAL_OUTPUT *pData, int index)
{
    float *pAttribs = &pData->vertexAttribs[index][0][0];
    float *pFinalAttribs;
    NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    int evalEnables = info->evalEnables;
    int vertexSize = info->vertexSize;
    int i,j;
    DWORD mapping;
    DWORD packedByte, dwT0, dwT1, dwType;
    PNVD3DCONTEXT pContext;

    BOOL bUseTSSState = FALSE;

    pContext = (PNVD3DCONTEXT)info->context;
    // if we don't have a program we need to double check the TSS State and see what we
    // need to actually output.
    if(pContext && !pContext->pCurrentVShader->hasProgram()){
        // if textures are on enable the texture 'arrays'
        // even though they may not have data... dwT0/dwT1 will fetch from the right place
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0)) {
            DWORD dwTCIndex = (pContext->hwState.dwTexCoordIndices >>  0) & 0xffff;
            dwT0 = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            evalEnables |= (1 << defaultInputRegMap[D3DVSDE_TEXCOORD0]);
        }
        else{
            dwT0 = defaultInputRegMap[D3DVSDE_TEXCOORD0];
            evalEnables &= ~(1 << defaultInputRegMap[D3DVSDE_TEXCOORD0]);
        }
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1)) {
            DWORD dwTCIndex = (pContext->hwState.dwTexCoordIndices >> 16) & 0xffff;
            dwT1 = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            evalEnables |= (1 << defaultInputRegMap[D3DVSDE_TEXCOORD1]);
        }
        else{
            dwT1 = defaultInputRegMap[D3DVSDE_TEXCOORD1];
            evalEnables &= ~(1 << defaultInputRegMap[D3DVSDE_TEXCOORD1]);
        }
        bUseTSSState = TRUE;
        pFinalAttribs = pAttribs;
    }

    getDC()->nvPusher.push (0, (vertexSize << 18) | (NV_DD_CELSIUS << 13) | NVPUSHER_NOINC(NV056_INLINE_ARRAY(0)));
    i=1;

    // do inline vertex data in reverse order
    for (j=0; j<7; j++) {

        mapping = defaultInputRegMap[inline_renderorder[j]];

        if (bUseTSSState) {
            if      (mapping == defaultInputRegMap[D3DVSDE_TEXCOORD0]) mapping = dwT0;
            else if (mapping == defaultInputRegMap[D3DVSDE_TEXCOORD1]) mapping = dwT1;
        }

        nvAssert (mapping >= 0);

        if (evalEnables & (1<<mapping)) {

            nvAssert(pFinalAttribs);
            dwType = info->maps[mapping].Originaltype;

            //hack for weights when someone asks for a weight size greater than we can handle
            if (mapping == defaultInputRegMap[D3DVSDE_BLENDWEIGHT] && dwType > NV_PATCH_VERTEX_FORMAT_FLOAT_1) dwType = NV_PATCH_VERTEX_FORMAT_FLOAT_1;

            switch (dwType) {
                case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
                    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f",mapping,pFinalAttribs[4*mapping+0]);
                    getDC()->nvPusher.push (i, VIEW_AS_DWORD(pFinalAttribs[4*mapping+0]));
                    i++; break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
                    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f\t%f",mapping,pFinalAttribs  [4*mapping+0],pFinalAttribs[4*mapping+1]);
                    getDC()->nvPusher.push (i,   VIEW_AS_DWORD(pFinalAttribs[4*mapping+0]));
                    getDC()->nvPusher.push (i+1, VIEW_AS_DWORD(pFinalAttribs[4*mapping+1]));
                    i+=2; break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
                    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f\t%f\t%f",mapping,pFinalAttribs[4*mapping+0],pFinalAttribs[4*mapping+1],pFinalAttribs[4*mapping+2]);
                    if(info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR && mapping == defaultInputRegMap[D3DVSDE_NORMAL]){
                        getDC()->nvPusher.push (i,   (0x80000000^VIEW_AS_DWORD(pFinalAttribs[4*mapping+0])));
                        getDC()->nvPusher.push (i+1, (0x80000000^VIEW_AS_DWORD(pFinalAttribs[4*mapping+1])));
                        getDC()->nvPusher.push (i+2, (0x80000000^VIEW_AS_DWORD(pFinalAttribs[4*mapping+2])));
                    }else{
                        getDC()->nvPusher.push (i,   VIEW_AS_DWORD(pFinalAttribs[4*mapping+0]));
                        getDC()->nvPusher.push (i+1, VIEW_AS_DWORD(pFinalAttribs[4*mapping+1]));
                        getDC()->nvPusher.push (i+2, VIEW_AS_DWORD(pFinalAttribs[4*mapping+2]));
                    }
                    i+=3; break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_4:
                    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%f\t%f\t%f\t%f",mapping,pFinalAttribs[4*mapping+0],pFinalAttribs[4*mapping+1],
                                                                             pFinalAttribs[4*mapping+2],pFinalAttribs[4*mapping+3]);
                    getDC()->nvPusher.push (i,   VIEW_AS_DWORD(pFinalAttribs[4*mapping+0]));
                    getDC()->nvPusher.push (i+1, VIEW_AS_DWORD(pFinalAttribs[4*mapping+1]));
                    getDC()->nvPusher.push (i+2, VIEW_AS_DWORD(pFinalAttribs[4*mapping+2]));
                    getDC()->nvPusher.push (i+3, VIEW_AS_DWORD(pFinalAttribs[4*mapping+3]));
                    i+=4; break;
                case NV_PATCH_VERTEX_FORMAT_UBYTE:
                    //packed byte will be expanded by tess to 4 vec float
                    //pack it back down so the backend FVF definition isn't different
                    packedByte  =  max(0,min((int)(pFinalAttribs[4*mapping + 0]*256.0),256));
                    packedByte |= (max(0,min((int)(pFinalAttribs[4*mapping + 1]*256.0),256))<<8);
                    packedByte |= (max(0,min((int)(pFinalAttribs[4*mapping + 2]*256.0),256))<<8);
                    packedByte |= (max(0,min((int)(pFinalAttribs[4*mapping + 3]*256.0),256))<<8);
                    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\t%d\t%ld",mapping,packedByte);
                    getDC()->nvPusher.push (i, packedByte);
                    i++; break;
                default:
                    DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS:\tDefault: vertex shouldn't be here!!");
            }
        }
    }
    getDC()->nvPusher.adjust (i);
}

static void nvCelsiusStreamInit(void  *context)
{
    //getDC()->nvPusher.flush(TRUE,CPushBuffer::FLUSH_HEAVY_POLLING);
    //NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    return;
}

static void nvCelsiusStreamDestroy(void *context)
{
    //getDC()->nvPusher.flush(TRUE,CPushBuffer::FLUSH_HEAVY_POLLING);
    //NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    return;
}

static void nvCelsiusSendFrontFace(void *context, int reversed){
    PNVD3DCONTEXT pContext;
    NV_PATCH_INFO *info = (NV_PATCH_INFO*)context;
    pContext = (PNVD3DCONTEXT)info->context;


    //DCR UGLY HACK -- OGL and D3D have different ordering on vertices so I 'remap' mine on the front end
    //but it forces a reversal in windings for the triangular tesellation case... FIND a better solution
    if(info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) reversed=1-reversed;
    switch(pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]){
    case D3DCULL_NONE:
            break;
    case D3DCULL_CW:
            reversed=1-reversed;
    case D3DCULL_CCW:
        getDC()->nvPusher.push(0, (0x1 << 18) | (NV_DD_CELSIUS << 13) | NV056_SET_FRONT_FACE);
        if(reversed){
            DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Cull CW");
            getDC()->nvPusher.push(1, NV056_SET_FRONT_FACE_V_CW);
        } else {
            DPF_LEVEL(NVDBG_LEVEL_HOS_VERTEX_INFO ,"HOS: Cull CCW");
            getDC()->nvPusher.push(1, NV056_SET_FRONT_FACE_V_CCW);
        }
        getDC()->nvPusher.adjust(2);
        break;
    default:
        nvAssert(0);    //should never be here
    }
}

static void nvCelsiusBeginPatch(void *info){
    return;
}
static void nvCelsiusEndPatch(void *info){
    return;
}

NV_PATCH_BACKEND nvCelsiusImm_Backend = {
    nvCelsiusStreamInit,
    nvCelsiusStreamDestroy,
    nvCelsiusBeginPrimImm,
    nvCelsiusEndPrimImm,
    nvCelsiusSendPrimImm,
    nvCelsiusSendFrontFace,
    nvCelsiusBeginPatch,
    nvCelsiusEndPatch
};

#endif
