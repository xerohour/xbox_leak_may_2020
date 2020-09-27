
// **************************************************************************
///
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
//  Module: nvCelsiusNvTSS.cpp
//      Celsius 2 stage combiner setup routines.
//
// **************************************************************************
//
//  History:
//      Lorie Sixia Deng       12Dec99         NV10 development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

BOOL nvCheckBumpMapStates(PNVD3DCONTEXT pContext, int nStage)
{

    PNVD3DTEXSTAGESTATE ptssState0, ptssState1, ptssState2, ptssState3,
                        ptssState4, ptssState5, ptssState6, ptssState7;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];
    ptssState3 = &pContext->tssState[3];
    ptssState4 = &pContext->tssState[4];
    ptssState5 = &pContext->tssState[5];
    ptssState6 = &pContext->tssState[6];
    ptssState7 = &pContext->tssState[7];

    /*
    * Make specific check for our version of bump mapping.
    *
    * THIS IS A VERY SPECIFIC CHECK FOR OUR BUMP MAPPING ALGORITHM.  ALL CONDITIONS MUST BE
    * TRUE OR WE WILL FAIL VALIDATION AND NOT RENDER ANYTHING PREDICTABLE.
    *
    * Even though D3D specifies that Arg2 cannot be a texture, since we used to do this for
    * our bump mapping algorithm, I'm going to continue to allow it so that we don't break
    * anything.
    */

    if ((ptssState0->dwValue[D3DTSS_TEXTUREMAP] == NULL) ||
        (ptssState1->dwValue[D3DTSS_TEXTUREMAP] == NULL))
    {
        return FALSE;
    }

    if (nStage==4)
    {

        if ((ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
            || (ptssState0->dwValue[D3DTSS_ALPHAARG1] != (D3DTA_TEXTURE | D3DTA_COMPLEMENT))
            || (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_DISABLE)
            || (ptssState0->dwValue[D3DTSS_TEXCOORDINDEX] != 0)
            || (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED)
            || (!((ptssState1->dwValue[D3DTSS_ALPHAARG1] == D3DTA_TEXTURE)
            && (ptssState1->dwValue[D3DTSS_ALPHAARG2] == D3DTA_CURRENT))
            && !((ptssState1->dwValue[D3DTSS_ALPHAARG1] == D3DTA_CURRENT)
            && (ptssState1->dwValue[D3DTSS_ALPHAARG2] == D3DTA_TEXTURE)))
            || (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_DISABLE)
            || (ptssState1->dwValue[D3DTSS_TEXCOORDINDEX] != 1)
            || (ptssState2->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
            || (ptssState2->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED2X)
            || (ptssState2->dwValue[D3DTSS_COLORARG1] != (D3DTA_CURRENT | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE))
            || (ptssState2->dwValue[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
            || (ptssState3->dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
            || (ptssState3->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
            || (ptssState3->dwValue[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
            || (ptssState3->dwValue[D3DTSS_TEXCOORDINDEX] != 0))
            return FALSE;

        else
            return TRUE;
    }

    else if (nStage==8)
    {
        /*
        * Check the alphaops and colorops.
        *
        * Stages 0, 2, 4, and 6 should all be set as D3DTOP_MODULATE.
        */
        if ((ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
            || (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
            || (ptssState2->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
            || (ptssState2->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
            || (ptssState4->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
            || (ptssState4->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE)
            || (ptssState6->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
            || (ptssState6->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE))
            return FALSE;

        /*
        * Stages 3 and and 7 should all be set as D3DTOP_SELECTARG1
        */
        if ((ptssState3->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
            || (ptssState3->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
            || (ptssState7->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
            || (ptssState7->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1))
            return FALSE;

        /*
        * Stages 1 and 5 should be one of the following:
        * D3DTOP_ADD, D3DTOP_ADDSIGNED, D3DTOP_ADDSIGNED2X or D3DTOP_SUBTRACT.
        * We also allow D3DTOP_MODULATE2X and D3DTOP_MODULATE4X to be specified
        * but these operations are translated to ADD2 and ADD4 respectively.
        */
        if ((ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADD)
            && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED)
            && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED2X)
            && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SUBTRACT)
            && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE2X)
            && (ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE4X))
            return FALSE;

        if ((ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_ADD)
            && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED)
            && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED2X)
            && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_SUBTRACT)
            && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE2X)
            && (ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE4X))
            return FALSE;

        if ((ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADD)
            && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED)
            && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_ADDSIGNED2X)
            && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SUBTRACT)
            && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE2X)
            && (ptssState5->dwValue[D3DTSS_ALPHAOP] != D3DTOP_MODULATE4X))
            return FALSE;

        if ((ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_ADD)
            && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED)
            && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_ADDSIGNED2X)
            && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_SUBTRACT)
            && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE2X)
            && (ptssState5->dwValue[D3DTSS_COLOROP] != D3DTOP_MODULATE4X))
            return FALSE;

        return TRUE;

    }
    else
    {
        return FALSE;
    }
}

//set up the celsius combiners for either the current texture stage state
// or a legacy texture blend
HRESULT nvSetCelsius4StageBumpMapCombiners  (PNVD3DCONTEXT pContext)
{
    PNVD3DTEXSTAGESTATE ptssState0, ptssState1, ptssState2, ptssState3;

    //dbgTracePush ("nvSetCelsuisBumpMap1Combiners");

    /*
    * Set up some pointers to the individual texture stages.
    */
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];
    ptssState3 = &pContext->tssState[3];

    /*
    * Set up Bump Mapping State.
    *
    * Texture0 and Texture1 comes from stage 0, 1 or 3 (always same texture).
    * Texture0 is used for stages with TEXCOORDINDEX = 0
    * Texture1 is used for stages with TEXCOORDINDEX = 1
    */
    pContext->hwState.dwTexUnitToTexStageMapping[0]=0;
    pContext->hwState.dwTexUnitToTexStageMapping[1]=1;
    pContext->hwState.dwNumActiveCombinerStages=2;

    pContext->hwState.dwAlphaICW[0] = DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_SOURCE, 8)   // texture0
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_MAP,1)       // replicate alpha
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_SOURCE,0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_SOURCE,9)    // texture1
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_SOURCE,0);

    pContext->hwState.dwAlphaOCW[0] = DRF_NUM(056, _SET_COMBINER_ALPHA_OCW, _OPERATION, 1)  // add bais
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_MUX_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_SUM_DST, 0xc)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_AB_DST, 0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_CD_DST, 0);

    pContext->hwState.dwColorICW[0] = DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_ALPHA,1)     // replicate alpha
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_SOURCE, 8)   // texture0
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_ALPHA,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_SOURCE,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_ALPHA,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_SOURCE,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_ALPHA,1)     // replicate alpha
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_SOURCE,9);   // texture1

    pContext->hwState.dwColorOCW[0] = DRF_NUM(056, _SET_COMBINER0_COLOR_OCW, _OPERATION, 3) // add bais shiftleft by1
                                    | DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_MUX_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_AB_DOT_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_CD_DOT_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_SUM_DST, 0xc)  // out put from combiner0
                                    | DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_AB_DST, 0)
                                    | DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_CD_DST, 0);

    pContext->hwState.dwAlphaICW[1] = DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_SOURCE, 0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_MAP,1)       // unsigned_invert
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_SOURCE,0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_SOURCE,0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_ALPHA,1)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_SOURCE,0);

    pContext->hwState.dwAlphaOCW[1] = DRF_NUM(056, _SET_COMBINER_ALPHA_OCW, _OPERATION, 0)  // add no shift
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_MUX_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_SUM_DST, 0xc)   // output from combiner0
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_AB_DST, 0)
                                    | DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_CD_DST, 0);

    pContext->hwState.dwColorICW[1] = DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_MAP,1)       // unsinged_invert
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_ALPHA,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_SOURCE, 0xc) // output from combiner0
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_ALPHA,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_SOURCE,8)    // texture0
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_ALPHA,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_SOURCE,8)    // texture0
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_MAP,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_ALPHA,0)
                                    | DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_SOURCE,4);   // diffuse

    pContext->hwState.dwColorOCW[1] = DRF_NUM(056, _SET_COMBINER1_COLOR_OCW, _ITERATION_COUNT, 2) // use both cominbers
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_MUX_SELECT, 0)
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW, _OPERATION, 0)
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_MUX_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_AB_DOT_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_CD_DOT_ENABLE, 0)
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_SUM_DST, 0xc)  // output from cominber0
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_AB_DST, 0)
                                    | DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_CD_DST, 0);

#if COMBINER_STATUS
    DPF ("Result in 4 stage bump mapping set up");
    DPF ("   color icw[0] = 0x%08x", pContext->hwState.dwColorICW[0]);
    DPF ("   color ocw[0] = 0x%08x", pContext->hwState.dwColorOCW[0]);
    DPF ("   alpha icw[0] = 0x%08x", pContext->hwState.dwAlphaICW[0]);
    DPF ("   alpha ocw[0] = 0x%08x", pContext->hwState.dwAlphaOCW[0]);
    DPF ("   color icw[1] = 0x%08x", pContext->hwState.dwColorICW[1]);
    DPF ("   color ocw[1] = 0x%08x", pContext->hwState.dwColorOCW[1]);
    DPF ("   alpha icw[1] = 0x%08x", pContext->hwState.dwAlphaICW[1]);
    DPF ("   alpha ocw[1] = 0x%08x", pContext->hwState.dwAlphaOCW[1]);
    __asm int 3;
#endif

    return (D3D_OK);
}

HRESULT nvSetCelsius8StageBumpMapCombiners(PNVD3DCONTEXT pContext)
{
    DWORD                   operation, finDst;
    DWORD                   argASource, argAMap, argAAlpha;
    DWORD                   argBSource, argBMap, argBAlpha;
    DWORD                   argCSource, argCMap, argCAlpha;
    DWORD                   argDSource, argDMap, argDAlpha;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2, ptssState3, ptssState4, ptssState5, ptssState6, ptssState7;
    PNVD3DTEXSTAGESTATE     ptssTexture0, ptssTexture1;

    finDst=0xc;

    //dbgTracePush ("nvSetCelsuisBumpMap2Combiners");

    /*
     * Set up some pointers to the individual texture stages.
     */
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];
    ptssState3 = &pContext->tssState[3];
    ptssState4 = &pContext->tssState[4];
    ptssState5 = &pContext->tssState[5];
    ptssState6 = &pContext->tssState[6];
    ptssState7 = &pContext->tssState[7];
    // pmtsState  = &pContext->mtsState;


    /*
     * Next track down all the textures being used.
     * The method for selecting the textures is simple:
     *   Map Stage 0 Texture/TextureCoordIndex to TEXTURE0.
     *   Map Stage 1 Texture/TextureCoordIndex to TEXTURE1.
     */

    ptssTexture0 = &pContext->tssState[0];
    ptssTexture1 = &pContext->tssState[1];

    pContext->hwState.dwTexUnitToTexStageMapping[0]=0;
    pContext->hwState.dwTexUnitToTexStageMapping[1]=1;
    pContext->hwState.dwNumActiveCombinerStages=2;


    /*
     * Setup hardware texture combiner 0 alpha components based on texture stages 0-3.
     * Operation comes from AlphaOp of stage 1.
     * Arguments come from AlphaArg1 of stages 0-3.
     */
    switch (ptssState1->dwValue[D3DTSS_ALPHAOP])
    {
    case D3DTOP_ADD:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT;
        break;
    case D3DTOP_ADDSIGNED:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT_BIAS;
        break;
    case D3DTOP_ADDSIGNED2X:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY1_BIAS;
        break;
    case D3DTOP_SUBTRACT:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT;
        pContext->hwState.dwStateFlags |= (CELSIUS_FLAG_ADDCOMPLEMENTALPHA(0));
        break;
    case D3DTOP_MODULATE2X:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY1;
        break;
    case D3DTOP_MODULATE4X:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY2;
        break;
    }

    /*
     * Select Alpha Argument A Combiner 0.
     */
    switch (ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9;
        else
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }
    argAMap=!((ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument B Combiner 0.
     */
    switch (ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }
    argBMap=!((ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
    * Select Alpha Argument C Combiner 0.
    */
    switch (ptssState2->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState2->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState2->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9;
        else
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState2->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }
    argCMap=!((ptssState2->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Select Alpha Argument D Combiner 0.
     */
    switch (ptssState3->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState3->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState3->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9;
        else
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState3->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }
    argDMap=!((ptssState3->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
     * Program Texture Combine 0 Alpha Stage.
     */
    pContext->hwState.dwAlphaICW[0]= DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_MAP,argAMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_SOURCE, argASource)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_MAP,argBMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_SOURCE,argBSource)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_MAP,argCMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_SOURCE,argCSource)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_MAP,argDMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_SOURCE,argDSource);

    pContext->hwState.dwAlphaOCW[0]=DRF_NUM(056, _SET_COMBINER_ALPHA_OCW, _OPERATION, operation)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_MUX_ENABLE, 0)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_SUM_DST, finDst)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_AB_DST, 0)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_CD_DST, 0);

    /*
    * Setup hardware texture combiner 0 color components based on texture stages 0-3.
    * Operation comes from ColorOp of stage 1.
    * Arguments come from ColorArg1 of stages 0-3.
    */
    switch (ptssState1->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_ADD:
        operation = NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT;
        break;
    case D3DTOP_ADDSIGNED:
        operation = NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT_BIAS;
        break;
    case D3DTOP_ADDSIGNED2X:
        operation = NV056_SET_COMBINER0_COLOR_OCW_OPERATION_SHIFTLEFTBY1_BIAS;
        break;
    case D3DTOP_SUBTRACT:
        operation = NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT;
        pContext->hwState.dwStateFlags |= (CELSIUS_FLAG_ADDCOMPLEMENTRGB(0));
        break;
    case D3DTOP_MODULATE2X:
        operation = NV056_SET_COMBINER0_COLOR_OCW_OPERATION_SHIFTLEFTBY1;
        break;
    case D3DTOP_MODULATE4X:
        operation = NV056_SET_COMBINER0_COLOR_OCW_OPERATION_SHIFTLEFTBY2;
        break;
    }

    /*
    * Select Color Argument A Combiner 0.
    */
    switch (ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_9;
        else
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1;
        else
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0;
        break;
    }
    argAMap=!((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argAAlpha = !((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
    * Select Color Argument B Combiner 0.
    */
    switch (ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_9;
        else
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_1;
        else
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0;
        break;
    }
    argBMap=!((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argBAlpha = !((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
    * Select Color Argument C Combiner 0.
    */
    switch (ptssState2->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState2->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState2->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_9;
        else
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState2->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1;
        else
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0;
        break;
    }
    argCMap=!((ptssState2->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argCAlpha = !((ptssState2->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
    * Select Color Argument D Combiner 0.
    */
    switch (ptssState3->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState3->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState3->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;
        else
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState3->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_1;
        else
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0;
        break;
    }
    argDMap=!((ptssState3->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argDAlpha = !((ptssState3->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
    * Program Texture Combine 0 Color Stage.
    */
    pContext->hwState.dwColorICW[0]= DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_MAP,argAMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_ALPHA,argAAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_SOURCE, argASource)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_MAP,argBMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_ALPHA,argBAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_SOURCE,argBSource)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_MAP,argCMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_ALPHA,argCAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_SOURCE,argCSource)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_MAP,argDMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_ALPHA,argDAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_SOURCE,argDSource);

    pContext->hwState.dwColorOCW[0]= DRF_NUM(056, _SET_COMBINER0_COLOR_OCW, _OPERATION, operation)
                                        |DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_MUX_ENABLE, 0)
                                        |DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_AB_DOT_ENABLE, 0)
                                        |DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_CD_DOT_ENABLE, 0)
                                        |DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_SUM_DST, finDst)
                                        |DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_AB_DST, 0)
                                        |DRF_NUM(056, _SET_COMBINER0_COLOR_OCW,_CD_DST, 0);

    /*
    * Setup hardware texture combiner 1 alpha components based on texture stages 0-3.
    * Operation comes from AlphaOp of stage 5.
    * Arguments come from AlphaArg1 of stages 4-7.
    */

    argAMap=argBMap=argCMap=argDMap=0;

    switch (ptssState5->dwValue[D3DTSS_ALPHAOP])
    {
    case D3DTOP_ADD:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT;
        break;
    case D3DTOP_ADDSIGNED:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT_BIAS;
        break;
    case D3DTOP_ADDSIGNED2X:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY1_BIAS;
        break;
    case D3DTOP_SUBTRACT:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT;
        pContext->hwState.dwStateFlags |= (CELSIUS_FLAG_ADDCOMPLEMENTALPHA(1));
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_SPECFOG_COMBINER;
        break;
    case D3DTOP_MODULATE2X:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY1;
        break;
    case D3DTOP_MODULATE4X:
        operation = NV056_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY2;
        break;
    }

    /*
    * Select Alpha Argument A Combiner 1.
    */

    switch (ptssState4->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTALPHA(0))
            argAMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState4->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState4->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9;
        else
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState4->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argASource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }

    argAMap ^=!((ptssState4->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
    * Select Alpha Argument B Combiner 1.
    */
    switch (ptssState5->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTALPHA(0))
            argBMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState5->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState5->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argBSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }
    argBMap  =argBMap ^(!((ptssState5->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0));

    /*
    * Select Alpha Argument C Combiner 1.
    */
    switch (ptssState6->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTALPHA(0))
            argCMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState6->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState6->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9;
        else
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState6->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argCSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }
    argCMap ^=!((ptssState6->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
    * Select Alpha Argument D Combiner 1.
    */
    switch (ptssState6->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTALPHA(0))
            argDMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState7->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState7->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9;
        else
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState7->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1;
        else
            argDSource = NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0;
        break;
    }

    argDMap ^=!((ptssState7->dwValue[D3DTSS_ALPHAARG1] & D3DTA_COMPLEMENT) == 0);

    /*
    * Program Texture Combine 1 Alpha Stage.
    */
    pContext->hwState.dwAlphaICW[1]= DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_MAP,argAMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _A_SOURCE, argASource)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_MAP,argBMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _B_SOURCE,argBSource)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_MAP,argCMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _C_SOURCE,argCSource)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_MAP,argDMap)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_ALPHA,1)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_ICW, _D_SOURCE,argDSource);

    pContext->hwState.dwAlphaOCW[1]=DRF_NUM(056, _SET_COMBINER_ALPHA_OCW, _OPERATION, operation)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_MUX_ENABLE, 0)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_SUM_DST, finDst)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_AB_DST, 0)
                                |DRF_NUM(056, _SET_COMBINER_ALPHA_OCW,_CD_DST, 0);



    /*
     * Setup hardware texture combiner 1 color components based on texture stages 0-3.
     * Operation comes from ColorOp of stage 5.
     * Arguments come from ColorArg1 of stages 4-7.
     */
    argAMap=argBMap=argCMap=argDMap=0;

    switch (ptssState5->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_ADD:
        operation = NV056_SET_COMBINER1_COLOR_OCW_OPERATION_NOSHIFT;
        break;
    case D3DTOP_ADDSIGNED:
        operation = NV056_SET_COMBINER1_COLOR_OCW_OPERATION_NOSHIFT_BIAS;
        break;
    case D3DTOP_ADDSIGNED2X:
        operation = NV056_SET_COMBINER1_COLOR_OCW_OPERATION_SHIFTLEFTBY1_BIAS;
        break;
    case D3DTOP_SUBTRACT:
        operation = NV056_SET_COMBINER1_COLOR_OCW_OPERATION_NOSHIFT;
        pContext->hwState.dwStateFlags |= (CELSIUS_FLAG_ADDCOMPLEMENTRGB(1));
        pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_SPECFOG_COMBINER);
        break;
    case D3DTOP_MODULATE2X:
        operation = NV056_SET_COMBINER1_COLOR_OCW_OPERATION_SHIFTLEFTBY1;
        break;
    case D3DTOP_MODULATE4X:
        operation = NV056_SET_COMBINER1_COLOR_OCW_OPERATION_SHIFTLEFTBY2;
        break;
    }

    /*
     * Select Color Argument A Combiner 1.
     */
    switch (ptssState4->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTRGB(0))
            argAMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState4->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_9;
        else
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState4->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1;
        else
            argASource = NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0;
        break;
    }
    argAMap ^= !(ptssState4->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0;
    argAAlpha = !((ptssState4->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument B Combiner 1.
     */
    switch (ptssState5->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTRGB(0))
            argBMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8;
        else if (ptssTexture0 && ((ptssState5->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_9;
        else
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState5->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_1;
        else
            argBSource = NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0;
        break;
    }
    argBMap ^=!((ptssState5->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argBAlpha = !((ptssState5->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument C Combiner 1.
     */
    switch (ptssState6->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTRGB(0))
            argCMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState6->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState6->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_9;
        else
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState6->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1;
        else
            argCSource = NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0;
        break;
    }
    argCMap ^= !((ptssState6->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argCAlpha = !((ptssState6->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Select Color Argument D Combiner 1.
     */
    switch (ptssState7->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)
    {
    case D3DTA_DIFFUSE:
        argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4;
        break;
    case D3DTA_CURRENT:
        argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_C;
        if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_ADDCOMPLEMENTRGB(0))
            argDMap=1;
        break;
    case D3DTA_TEXTURE:
        if (ptssTexture0 && ((ptssState7->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_8;
        else if (ptssTexture1 && ((ptssState7->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE))
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;
        else
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4;
        break;
    case D3DTA_TFACTOR:
        if ((ptssState7->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) != D3DTA_TFACTOR)
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_1;
        else
            argDSource = NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0;
        break;
    }
    argDMap ^= !((ptssState7->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) == 0);
    argDAlpha = !((ptssState7->dwValue[D3DTSS_COLORARG1] & D3DTA_ALPHAREPLICATE) == 0);

    /*
     * Program Texture Combine 1 Color Stage.
     */
    pContext->hwState.dwColorICW[1]= DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_MAP,argAMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_ALPHA,argAAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _A_SOURCE, argASource)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_MAP,argBMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_ALPHA,argBAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _B_SOURCE,argBSource)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_MAP,argCMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_ALPHA,argCAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _C_SOURCE,argCSource)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_MAP,argDMap)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_ALPHA,argDAlpha)
                                        |DRF_NUM(056, _SET_COMBINER_COLOR_ICW, _D_SOURCE,argDSource);

    pContext->hwState.dwColorOCW[1]= DRF_NUM(056, _SET_COMBINER1_COLOR_OCW, _ITERATION_COUNT, 2)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_MUX_SELECT, 0)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW, _OPERATION, operation)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_MUX_ENABLE, 0)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_AB_DOT_ENABLE, 0)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_CD_DOT_ENABLE, 0)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_SUM_DST, finDst)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_AB_DST, 0)
                                        |DRF_NUM(056, _SET_COMBINER1_COLOR_OCW,_CD_DST, 0);

#if COMBINER_STATUS
    DPF ("Result in 8 stage mapping set up");
    DPF ("   color icw[0] = 0x%08x", pContext->hwState.dwColorICW[0]);
    DPF ("   color ocw[0] = 0x%08x", pContext->hwState.dwColorOCW[0]);
    DPF ("   alpha icw[0] = 0x%08x", pContext->hwState.dwAlphaICW[0]);
    DPF ("   alpha ocw[0] = 0x%08x", pContext->hwState.dwAlphaOCW[0]);
    DPF ("   color icw[1] = 0x%08x", pContext->hwState.dwColorICW[1]);
    DPF ("   color ocw[1] = 0x%08x", pContext->hwState.dwColorOCW[1]);
    DPF ("   alpha icw[1] = 0x%08x", pContext->hwState.dwAlphaICW[1]);
    DPF ("   alpha ocw[1] = 0x%08x", pContext->hwState.dwAlphaOCW[1]);
    __asm int 3;
#endif

    return (D3D_OK);
}


static BOOL Check3StageColor(PNVD3DCONTEXT pContext)
{
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];

    //In stage 0: Color Op = D3DTOP_SELECTARG1 or D3DTOP_SELECTARG2
    if ((ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
        &&(ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG2))
        goto Fail;

   //in stage 0: texture has to be selected
    if(!((((ptssState0->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_TEXTURE)
        &&(ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1))
        ||(((ptssState0->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG2))))
        goto Fail;

    //In Stage 0: validate the texture
    if (!(ptssState0->dwValue[D3DTSS_TEXTUREMAP]))
        goto Fail;

    //In stage 1: Color Arg1=current xor Arg2=current
    //And not alpha replication could be used for current in stage 1
    if (!(((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_CURRENT)
        ^((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK) == D3DTA_CURRENT)))
        goto Fail;
    else
    {
        if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_CURRENT)
        {
            if(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)
                goto Fail;
        }
        else
        {
             if(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)
                goto Fail;
        }
    }

    //When the stage1 color op = BlendCurrentAlpha, the stage0 alpha has to be select texture
    //restrictly, cannot use complement of the texture
    if((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_BLENDCURRENTALPHA)
        &&(!(((ptssState0->dwValue[D3DTSS_ALPHAOP] == D3DTOP_SELECTARG1)
        &&(ptssState0->dwValue[D3DTSS_ALPHAARG1] == D3DTA_TEXTURE))
        ||((ptssState0->dwValue[D3DTSS_ALPHAOP] == D3DTOP_SELECTARG2)
        &&(ptssState0->dwValue[D3DTSS_ALPHAARG2] == D3DTA_TEXTURE)))))
        goto Fail;

    //In Stage 1: Vaidate texture if it is used
    //We know that op1!= selectArg1 or selectArg2 (if it is, it is going to fail the final setting)
    if ((((ptssState1->dwValue[D3DTSS_COLORARG1] &D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        ||((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_TEXTURE))
        &&(!(ptssState1->dwValue[D3DTSS_TEXTUREMAP])))
        goto Fail;

    //In stage 2: Color Arg1=current or Arg2==current
    if(((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) != D3DTA_CURRENT)
        &&((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)!= D3DTA_CURRENT))
        goto Fail;
     else
    {
        if((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_CURRENT)
        {
            if(ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)
                goto Fail;
        }
        else
        {
             if(ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)
                goto Fail;
        }
    }


    //In Stage 2: Non of the Arg could be Texture
    if (((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) == D3DTA_TEXTURE)
        ||((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK) == D3DTA_TEXTURE))
        goto Fail;

    return TRUE;

Fail:
    DPF_LEVEL (NVDBG_LEVEL_INFO,"Failed the Color Arg check or the Color Op0 for 2 texture 3 stage");

    return FALSE;
}


static BOOL Check3StageAlpha(PNVD3DCONTEXT pContext)
{
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;
    DWORD alphaStageCount = 0;
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    while ((alphaStageCount < 8) &&
        (pContext->tssState[alphaStageCount].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE))
    {
        alphaStageCount++;
    }

    if(alphaStageCount==0)
        return TRUE;
    else //alphaStageCount>0
    {
        //In stage 0: Texture is validated in Color Check alrady

        if(alphaStageCount==1)
            return TRUE;
        else //alphaStageCount>1
        {
            //In stage 1: Validate texture if is used.
            if (((((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK) == D3DTA_TEXTURE)
                &&(ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2))
                ||((ptssState1->dwValue[D3DTSS_ALPHAARG2] == D3DTA_TEXTURE)
                &&((ptssState1->dwValue[D3DTSS_ALPHAOP]&D3DTA_SELECTMASK)!= D3DTOP_SELECTARG1)))
                &&(!(ptssState1->dwValue[D3DTSS_TEXTUREMAP])))
                return FALSE;
            else
            {
                if(alphaStageCount==2)
                    return TRUE;
                else
                {
                    DPF_LEVEL (NVDBG_LEVEL_INFO,"Alpha Stage# > 2, Too many alpha stages");

                    return FALSE;
                }
            }
        }
    }
}

//Check for the conditions that meet for 2 texture stage one combiner
static BOOL colorCheck2Stage1Combiner (PNVD3DCONTEXT pContext)
{
    PNVD3DTEXSTAGESTATE    ptssState0, ptssState1;
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //Add, Modulate
    if ((ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
        &&
        ((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)   ||
         (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X) ||
         (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X)))
    {
        //one and only one arg ins tage1 be current
        //no apha replicate or complement allowed for the "current" arg
        if (((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
            ^((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT))
        {
            if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
            {
                if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
                    ||(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE))
                    goto Fail;
            }
            else
            {
                 if((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
                    ||(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE))
                    goto Fail;
            }
            return TRUE;
        }
        else
            goto Fail;
    }

    //Modulate, Add
    if((ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
        &&((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED2X)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT)))
    {
        //one and only one arg ins tage1 be current
        //no apha replicate or complement allowed for the "current" arg
           if (((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
            ^((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT))
        {
            if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
            {
                if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
                    ||(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE))
                    goto Fail;
            }
            else
            {
                 if((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
                    ||(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE))
                    goto Fail;
            }
            return TRUE;
        }
        else
            goto Fail;
    }

Fail:
/*#ifdef DEBUG
        DPF("Cannot put 2 stages Color setting into one combiner");
        _asm int 3;
#endif
*/
        return FALSE;
}
static BOOL alphaCheck2Stage1Combiner (PNVD3DCONTEXT pContext)
{
    PNVD3DTEXSTAGESTATE    ptssState0, ptssState1;
    DWORD                  alphaStageCount=0;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    while ((alphaStageCount < 2)
        &&(pContext->tssState[alphaStageCount].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE))
    {
        alphaStageCount++;
    }

    if (alphaStageCount < 2)
        return TRUE;

    else //alpha stage > 1
    {
        //Add, Modulate
        if((ptssState0->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADD)
           &&
           ((ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE)   ||
            (ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE2X) ||
            (ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE4X)))
        {
            //one and only one arg ins tage1 be current
            //no complement allowed for the "current" arg
            if (((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
                ^((ptssState1->dwValue[D3DTSS_ALPHAARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT))
            {
                if((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
                {
                    if(ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_COMPLEMENT)
                        goto Fail;
                }
                else
                {
                    if(ptssState1->dwValue[D3DTSS_ALPHAARG2]&D3DTA_COMPLEMENT)
                        goto Fail;
                }
                return TRUE;
            }
            else
                goto Fail;
        }

        //Modulate, Add
        if((ptssState0->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE)
            &&((ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADD)
            ||(ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADDSIGNED)
            ||(ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADDSIGNED2X)
            ||(ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_SUBTRACT)))
        {
            //one and only one arg ins tage1 be current
            //no complement allowed for the "current" arg
            if (((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
                ^((ptssState1->dwValue[D3DTSS_ALPHAARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT))
            {
                if((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
                {
                    if(ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_COMPLEMENT)
                        goto Fail;
                }
                else
                {
                    if(ptssState1->dwValue[D3DTSS_ALPHAARG2]&D3DTA_COMPLEMENT)
                        goto Fail;
                }
                return TRUE;
            }
            else
                goto Fail;
        }
    }

Fail:
/*#ifdef DEBUG
DPF("Cannot put 2 stages alpha setting into one combiner");
_asm int 3;
#endif
    */
    return FALSE;
}


/***************************************************************************************
* This is a special case to save one combiner stage
* Handle the case of (X0+X1)*X2 for color operation
* Set Color Combiner0 A=X0; B=X2; C=X1; D=X2
* Where  X0= one Arg of Stage 0; could be T0
*        X1= one Arg of Stage 1; could be T1;(the other Arg of Stage 1 is CURRENT)
* Disable Combiner1
*        X3= one Arg of Stage 2; cannot be TEXTURE;(the other Arg of Stage 1 is CURRENT)
* Alpha setting may not follow this
****************************************************************************************/

static HRESULT AddMod1(PNVD3DCONTEXT pContext)
{
    int      shift;
    BOOL     compA,compC, repA, repC, expA, expC;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2;

    //initialize the flag for combiner
    shift  = 0;
    compA=compC=repA=repC=expA=expC=FALSE;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];

    //initialize color combiner0
    pContext->hwState.dwColorICW[0] = 0;
    pContext->hwState.dwColorOCW[0] = 0;

    //Disable color combine1
    pContext->hwState.dwColorICW[1]=0;
    pContext->hwState.dwColorOCW[1]=0x10000000;
    pContext->hwState.dwNumActiveCombinerStages = 1;

    //deal with the situation for current complement

    if((((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        &((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)!=0))
        ||(((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        &((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)!=0)))
        compA ^=TRUE;

    /***************************************************************/
    /*Color Combiner 0 Setup
    /**************************************************************/
    //Stage0 Op = SelectArg1 or SelectArg2
    //Check Stage1 Op

    if (ptssState1->dwValue[D3DTSS_COLOROP]==D3DTOP_SUBTRACT)
    {
        if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        {   //(T0*B)+(-T1)*D
            compC ^=TRUE;
            expC=TRUE;
        }
        else
        {   //(-T0)*B+(T1)*D
            compA ^=TRUE;
            expA=TRUE;
        }
    }
    //Check Stage2 Op
    switch (ptssState2->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_MODULATE2X:
        shift++;
        break;
    case D3DTOP_MODULATE4X:
        shift +=2;
        break;
    }

    //Color combiner ICW[0]
    pContext->hwState.dwNextAvailableTextureUnit=0;
    //input value A from stage0 Arg
    if (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1)
        SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG1,compA, expA, repA);
    else//D3DOP_SELECTARG2
        SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG2,compA, expA, repA);

    pContext->hwState.dwNextAvailableTextureUnit=1;

    //input value C from stage1 args
    if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
        SelectColorInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG2,compC, expC, repC);
    else
        SelectColorInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG1,compC, expC, repC);

    //input value  B & D from stage2 args (B=D)
    if((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
    {
        SelectColorInput(pContext, celsiusCombinerInputB, 0, 2, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
        SelectColorInput(pContext, celsiusCombinerInputD, 0, 2, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
    }
    else
    {
        SelectColorInput(pContext, celsiusCombinerInputB, 0, 2, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);
        SelectColorInput(pContext, celsiusCombinerInputD, 0, 2, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);
    }

    //Color Combiner OCW[0]
    ConstructOutputColorCombiners(pContext, TRUE, 0, 0, shift, 0, 0, 0 );
    //End of Color Combiner 0 Setup
    return (D3D_OK);

} //end AddMod1


/******************************************************************************************
/*
/*Normal case of Color Combiner0 set up, DX7 Stage 0 and Stage 1
/*
/******************************************************************************************/

/*****************************************************************************************
/*Set Color Combiner0 for op1=add/subtract: A=X0; B=1/-1; C=X1; D=1/-1
/*****************************************************************************************/
static HRESULT Add1(PNVD3DCONTEXT pContext)
{
    int                     bias, shift;
    BOOL                    compA, repA, negB, negD, smoothA, smoothC;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;

    //initialize the flags for combiner
    bias=shift = 0;
    compA=repA=FALSE;
    negB=negD=smoothA=smoothC=FALSE;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //initialize color combiner0
    pContext->hwState.dwColorICW[0] = 0;
    pContext->hwState.dwColorOCW[0] = 0;

    //deal with the situation for current complement and alpha replicat
    if((((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        &((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)!=0))
        ||(((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        &((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)!=0)))
        compA ^=TRUE;

    //Color Combiner 0
    switch (ptssState1->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_ADDSIGNED:
        bias=1;
        break;
    case D3DTOP_ADDSIGNED2X:
        bias=1;
        shift++;
        break;
    case D3DTOP_SUBTRACT:
        if((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
            negB=TRUE; // T0*(-1)+T1*1
        else
            negD=TRUE; // T0*1+T1*(-1)
        break;
    case D3DTOP_ADDSMOOTH:
        if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
            smoothA=TRUE;
        else
            smoothC=TRUE;
        break;
    }

    pContext->hwState.dwNextAvailableTextureUnit=0;
    //input value A from stage0 Arg
    if (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1)
    {
        SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG1,compA,FALSE,repA);
        if(smoothA)  //input D as ~(A-alpha)
        {
            compA ^=TRUE;
            SelectColorInput(pContext, celsiusCombinerInputD, 0, 0, D3DTSS_COLORARG1,compA,FALSE,repA);
        }
    }
    else//D3DOP_SELECTARG2
    {
        SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG2,compA,FALSE,repA);
        if(smoothA) //input D as ~(A-alpha)
        {
            compA ^=TRUE;
            SelectColorInput(pContext, celsiusCombinerInputD, 0, 0, D3DTSS_COLORARG1,compA,FALSE,repA);
        }
    }

    pContext->hwState.dwNextAvailableTextureUnit=1;

    //input value C from stage1 args
    if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
    {
        SelectColorInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG2,FALSE,FALSE,FALSE);
        if(smoothC) //input B as ~(C-alpha)
        {
            SelectColorInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG2,TRUE,FALSE,FALSE);
            SetColorInputOne(pContext, celsiusCombinerInputD, 0, FALSE);
        }
    }
    else
    {
        SelectColorInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG1,FALSE,FALSE,FALSE);
        if(smoothC) //input B as ~(C-alpha)
        {
            SelectColorInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG1,TRUE,FALSE,FALSE);
            SetColorInputOne(pContext, celsiusCombinerInputD, 0, FALSE);
        }
    }

    //If no addSmooth is used, use the normal setting
    if((!smoothC)&&(!smoothA))
    {
        //Set B=1 or (-1 if op 1 is subtract)
        SetColorInputOne (pContext, celsiusCombinerInputB, 0, negB);
        //Set D=1 or (-1 if op 1 is subtract)
        SetColorInputOne (pContext, celsiusCombinerInputD, 0, negD);
    }

    //Color Combiner OCW[0]
    ConstructOutputColorCombiners(pContext, TRUE, 0, bias, shift, 0, 0, 0);
    //End of Color Combiner 0 Setup

    return (DD_OK);
}//end Add1

/***************************************************************************************
/*Set Color Combiner0 for op1=add/subtract: A=X0; B=1; C=0; D=0
/****************************************************************************************/
static HRESULT Mod1(PNVD3DCONTEXT pContext)
{
    int                     shift;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;
    BOOL                    compA0, repA0;

     //initialize the flags for combiner
    shift = 0;
    compA0=repA0=FALSE;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //initialize
    pContext->hwState.dwColorICW[0] = 0;
    pContext->hwState.dwColorOCW[0] = 0;


    if((((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        &((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)!=0))
        ||(((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        &((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)!=0)))
        compA0 ^=TRUE;

    switch (ptssState1->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_MODULATE2X:
        shift++;
        break;
    case D3DTOP_MODULATE4X:
        shift +=2;
        break;
    }
    //Color combiner ICW[0]
    //input value A from stage 0
    pContext->hwState.dwNextAvailableTextureUnit=0;
    if (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1)
        SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG1,compA0, FALSE, repA0);
    else//D3DOP_SELECTARG2
        SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG2,compA0, FALSE, repA0);

    //input value B from Stage 1
    pContext->hwState.dwNextAvailableTextureUnit=1;
    if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
        SelectColorInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
    else
        SelectColorInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);

    //input value  C & D default to be 0

    //Color Combiner OCW[0]
    ConstructOutputColorCombiners(pContext, TRUE, 0, 0, shift, 0, 0, 0);
    //End of Color Combiner 0 Setup

    return (D3D_OK);
}//end Mod1


/********************************************************************
/* Set Color Combiner0 for AlphaBlend
/* alpha * arg1 + (1-alpha) * arg2
/********************************************************************/
static HRESULT Blend1(PNVD3DCONTEXT  pContext)
{
    int                    pm, type, temp;
    BOOL                   compA0, repA0;
    PNVD3DTEXSTAGESTATE    ptssState0, ptssState1;

    //initialize the flags for combiner
    pm=0;
    compA0=repA0=FALSE;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //initialize combiner0 & Combiner1
    pContext->hwState.dwColorICW[0] = 0;
    pContext->hwState.dwColorOCW[0] = 0;

    //deal with the situation for current complement
    if ((((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK) == D3DTA_CURRENT) &&
         ((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_COMPLEMENT) != 0))
        ||
        (((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_CURRENT) &&
         ((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_COMPLEMENT) != 0))) {
        compA0 ^= TRUE;
    }

    // check Stage1 op to determine source of alpha
    switch (ptssState1->dwValue[D3DTSS_COLOROP]) {
        case D3DTOP_BLENDDIFFUSEALPHA:
            type=D3DTA_DIFFUSE;
            break;
        case D3DTOP_BLENDTEXTUREALPHA:
            type=D3DTA_TEXTURE;
            // alpha comes from stage 1's texture
            pContext->hwState.dwNextAvailableTextureUnit = 1;
            break;
        case D3DTOP_BLENDFACTORALPHA:
            type=D3DTA_TFACTOR;
            break;
        case D3DTOP_BLENDTEXTUREALPHAPM:
            type=D3DTA_TEXTURE;
            // alpha comes from stage 1's texture
            pContext->hwState.dwNextAvailableTextureUnit = 1;
            pm=1;
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            type=D3DTA_TEXTURE;
            // current alpha comes in from stage 0's texture
            pContext->hwState.dwNextAvailableTextureUnit = 0;
            break;
    }

    // A gets alpha (or 1)
    if (pm == 1) {
        // Make A 1 since texture is already pre-multiplied
        SetColorInputOne (pContext, celsiusCombinerInputA, 0, FALSE);
    }
    else {
        // Make A alpha, appropriate type
        temp = ptssState1->dwValue[D3DTSS_COLORARG2];
        ptssState1->dwValue[D3DTSS_COLORARG2] = type;
        SelectColorInput (pContext, celsiusCombinerInputA, 0, 1, D3DTSS_COLORARG2, FALSE, FALSE, TRUE);
        ptssState1->dwValue[D3DTSS_COLORARG2] = temp;
    }

    // C gets (1-alpha)
    temp = ptssState1->dwValue[D3DTSS_COLORARG1];
    ptssState1->dwValue[D3DTSS_COLORARG1] = type;
    SelectColorInput (pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG1, TRUE, FALSE, TRUE);
    ptssState1->dwValue[D3DTSS_COLORARG1] = temp;

    // B gets arg1
    if ((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK) == D3DTA_CURRENT) {
        // take output of d3d stage 0 (stage 0 op guaranteed to be a simple selectarg)
        pContext->hwState.dwNextAvailableTextureUnit = 0;
        SelectColorInput (pContext, celsiusCombinerInputB, 0, 0,
                          (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1) ? D3DTSS_COLORARG1 : D3DTSS_COLORARG2,
                          compA0, FALSE, repA0);
    }
    else {
        // take arg1 from this stage in the normal fashion
        pContext->hwState.dwNextAvailableTextureUnit = 1;
        SelectColorInput (pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    }

    // D gets arg2
    if ((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK) == D3DTA_CURRENT) {
        // take output of d3d stage 0 (stage 0 op guaranteed to be a simple selectarg)
        pContext->hwState.dwNextAvailableTextureUnit = 0;
        SelectColorInput (pContext, celsiusCombinerInputD, 0, 0,
                          (ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1) ? D3DTSS_COLORARG1 : D3DTSS_COLORARG2,
                          compA0, FALSE, repA0);
    }
    else if (((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK) == D3DTA_CURRENT)
        && ((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK) == D3DTA_TEXTURE)) {
        // take output of d3d stage 0 - fix for mfc tex bug ((tex0 OP tex1) MOD diffuse)
        pContext->hwState.dwNextAvailableTextureUnit = 0;
        SelectColorInput (pContext, celsiusCombinerInputD, 0, 0, D3DTSS_COLORARG1, compA0, FALSE, repA0);
    }
    else {
        // take arg2 from this stage in the normal fashion
        pContext->hwState.dwNextAvailableTextureUnit = 1;
        SelectColorInput (pContext, celsiusCombinerInputD, 0, 1, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    }

    // color Combiner OCW[0]
    ConstructOutputColorCombiners(pContext, TRUE, 0, 0, 0, 0, 0, 0);

    return(D3D_OK);

} // Blend1


/*************************************************************************************
/* Color combiner1 set up for DX7 Stage 2
/**************************************************************************************/

/************************************************************************************
/ Color Combiner1 for op=add/mod/select in stage2
/************************************************************************************/
static HRESULT AddModSelect2(PNVD3DCONTEXT  pContext)
{
    int                     dotproduct, bias,shift, modselect;
    BOOL                    negD, smooth, forcearg1to1, forcearg2to1;
    PNVD3DTEXSTAGESTATE     ptssState2;

    //initialize
    negD=smooth=forcearg1to1=forcearg2to1=FALSE;
    dotproduct=bias =shift=modselect=0;

    ptssState2 = &pContext->tssState[2];

    pContext->hwState.dwColorICW[1] = 0; //enable 2 combiners
    pContext->hwState.dwColorOCW[1] = 0x20000000;

     //Check the Stage2 Op
     switch (ptssState2->dwValue[D3DTSS_COLOROP])
     {
     case D3DTOP_ADDSIGNED:
         bias=1;
         break;
     case D3DTOP_ADDSIGNED2X:
         bias=1;
         shift++;
         break;
     case D3DTOP_SUBTRACT:
         negD=TRUE;
         break;
     case D3DTOP_ADDSMOOTH:
         smooth=TRUE;
         break;
     case D3DTOP_MODULATE:
         modselect=1;
         break;
     case D3DTOP_MODULATE2X:
         modselect=1;
         shift++;
         break;
     case D3DTOP_MODULATE4X:
         modselect=1;
         shift +=2;
         break;
     case D3DTOP_SELECTARG1:
         modselect=1;
         forcearg2to1=TRUE;
         break;
     case D3DTOP_SELECTARG2:
         modselect=1;
         forcearg1to1=TRUE;
         break;
     case D3DTOP_DOTPRODUCT3: //in DX stage2
         dotproduct=1;
         modselect=1;
         pContext->hwState.dwStateFlags |= CELSIUS_FLAG_DOTPRODUCT3(1);
         break;
     }

     //Color combiner ICW[1]
     //input value A & B from Stage 2
     if(modselect) //modulate setting
     {
         if (forcearg1to1) SetColorInputOne (pContext, celsiusCombinerInputA, 1, FALSE);
         else              SelectColorInput(pContext, celsiusCombinerInputA, 1, 2, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);

         if (forcearg2to1) SetColorInputOne (pContext, celsiusCombinerInputB, 1, FALSE);
         else              SelectColorInput(pContext, celsiusCombinerInputB, 1, 2, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
         //input value  C & D defalt to 0
     }
     else //add involved
     {
         SelectColorInput(pContext, celsiusCombinerInputA, 1, 2, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);
         SelectColorInput(pContext, celsiusCombinerInputC, 1, 2, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
         SetColorInputOne (pContext, celsiusCombinerInputB, 1, FALSE);
         if(smooth)
             SelectColorInput(pContext, celsiusCombinerInputD, 1, 2, D3DTSS_COLORARG1,TRUE, FALSE, FALSE);
         else
             SetColorInputOne (pContext, celsiusCombinerInputD, 1, negD);
     }

     //Color Combiner OWC[1]
     ConstructOutputColorCombiners(pContext, TRUE, 1, bias, shift, dotproduct, 0, 0);

     //update the number of combinerstage that are used
     pContext->hwState.dwNumActiveCombinerStages = 2;

     return (D3D_OK);
}//end AddModSelect2

/************************************************************
Color: stage0: arg0+arg1; stage1: arg0*arg1
*************************************************************/
static HRESULT colorAddMod(PNVD3DCONTEXT  pContext, int textureCount)
{
    PNVD3DTEXSTAGESTATE ptssState0, ptssState1;
    int                 shift;

    //initialize the flag for combiner
    shift = 0;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //initialize color combiner0
    pContext->hwState.dwColorICW[0] = 0;
    pContext->hwState.dwColorOCW[0] = 0;

    //Disable color combine1
    pContext->hwState.dwColorICW[1]=0;
    pContext->hwState.dwColorOCW[1]=0x10000000;
    pContext->hwState.dwNumActiveCombinerStages = 1;

    //Check Stage0 Op

    // can't do subtraction. this would require a mapping like [0 1] -> [0 -1] which we don't have
    nvAssert (ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_SUBTRACT);

    //Chech Stage1 Op
    switch (ptssState1->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_MODULATE2X:
        shift++;
        break;
    case D3DTOP_MODULATE4X:
        shift +=2;
        break;
    }

    //Color combiner ICW[0]
    pContext->hwState.dwNextAvailableTextureUnit=0;

    //input A, C from stage0 args
    SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    SelectColorInput(pContext, celsiusCombinerInputC, 0, 0, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    pContext->hwState.dwNextAvailableTextureUnit=textureCount-1;

    //input value  B & D from stage2 args (B=D)
    if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
    {
        SelectColorInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
        SelectColorInput(pContext, celsiusCombinerInputD, 0, 1, D3DTSS_COLORARG2,FALSE, FALSE, FALSE);
    }
    else
    {
        SelectColorInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);
        SelectColorInput(pContext, celsiusCombinerInputD, 0, 1, D3DTSS_COLORARG1,FALSE, FALSE, FALSE);
    }

    //Color Combiner OCW[0]
    ConstructOutputColorCombiners(pContext, TRUE, 0, 0, shift,0, 0, 0);
    return (D3D_OK);
}//colorAddMod


/************************************************************
Color: stage0: arg0*arg1; stage1: arg0+arg1
*************************************************************/
static HRESULT colorModAdd(PNVD3DCONTEXT  pContext, int textureCount)
{
    int                     shift, bias;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;

    //initialize the flags for combiner
    shift = bias = 0;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //initialize
    pContext->hwState.dwColorICW[0] = 0;
    pContext->hwState.dwColorOCW[0] = 0;
    pContext->hwState.dwColorICW[1] = 0;
    pContext->hwState.dwColorOCW[1] = 0x10000000;


    SetColorInputOne(pContext, celsiusCombinerInputD, 0, 0); // Set D = 1

    //Check Stage1 Op
    switch (ptssState1->dwValue[D3DTSS_COLOROP])
    {
    case D3DTOP_ADDSIGNED:
        bias=1;
        break;
    case D3DTOP_ADDSIGNED2X:
        bias=1;
        shift++;
        break;
    }

    //input A & B from stage 0
    pContext->hwState.dwNextAvailableTextureUnit=0;
    SelectColorInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    SelectColorInput(pContext, celsiusCombinerInputB, 0, 0, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    //input C from stage1
    pContext->hwState.dwNextAvailableTextureUnit=textureCount-1;
    if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
        SelectColorInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    else
        SelectColorInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);


    if (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT) {
        if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        {
            SetColorInputOne(pContext, celsiusCombinerInputD, 0, 1); //Set D = -1
        }
        else //Make the setting to be (A*(-B))+C*1
        {
            SetColorMapping(pContext, celsiusCombinerInputB, 0, NV056_SET_COMBINER_COLOR_ICW_D_MAP_SIGNED_NEGATE);
        }
    }

    //Color Combiner OCW[0]
    ConstructOutputColorCombiners(pContext, TRUE, 0, bias, shift,  0, 0, 0);
    //End of Color Combiner 0 Setup
    return (D3D_OK);
}//colorModAdd

/************************************************************
Alpha: stage0: arg0+arg1; stage1: arg0*arg1
*************************************************************/
static HRESULT alphaAddMod(PNVD3DCONTEXT  pContext, int textureCount)
{
    PNVD3DTEXSTAGESTATE ptssState0, ptssState1;
    int                 shift;

    //initialize the flag for combiner
    shift = 0;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //Disable alpha combines
    pContext->hwState.dwAlphaICW[0]=0;
    pContext->hwState.dwAlphaOCW[0]=0;
    pContext->hwState.dwAlphaICW[1]=0;
    pContext->hwState.dwAlphaOCW[1]=0;
    pContext->hwState.dwNumActiveCombinerStages = 1;

    //Check Stage0 Op

    // can't subtract. this would require a mapping like [0 1] -> [0 -1] which we don't have
    nvAssert (ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SUBTRACT);

    //Chech Stage1 Op
    switch (ptssState1->dwValue[D3DTSS_ALPHAOP])
    {
    case D3DTOP_MODULATE2X:
        shift++;
        break;
    case D3DTOP_MODULATE4X:
        shift += 2;
        break;
    }

    //ALPHA combiner ICW[0]
    pContext->hwState.dwNextAvailableTextureUnit=0;
    //input A, C from stage0 args
    SelectAlphaInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_ALPHAARG1, FALSE, FALSE);
    SelectAlphaInput(pContext, celsiusCombinerInputC, 0, 0, D3DTSS_ALPHAARG2, FALSE, FALSE);

    pContext->hwState.dwNextAvailableTextureUnit = textureCount-1;

    //input value  B & D from stage1 args (B=D)
    if((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
    {
        SelectAlphaInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_ALPHAARG2, FALSE, FALSE);
        SelectAlphaInput(pContext, celsiusCombinerInputD, 0, 1, D3DTSS_ALPHAARG2, FALSE, FALSE);
    }
    else
    {
        SelectAlphaInput(pContext, celsiusCombinerInputB, 0, 1, D3DTSS_ALPHAARG1, FALSE, FALSE);
        SelectAlphaInput(pContext, celsiusCombinerInputD, 0, 1, D3DTSS_ALPHAARG1, FALSE, FALSE);
    }


    //Color Combiner OCW[0]
    ConstructOutputAlphaCombiners(pContext, TRUE, 0, 0, shift, 0);
    //End of Color Combiner 0 Setup

    return (D3D_OK);

}//alphaAddMod

/************************************************************
Alpha: stage0: arg0*arg1; stage1: arg0+arg1
*************************************************************/
static HRESULT alphaModAdd(PNVD3DCONTEXT  pContext, int textureCount)
{   int                     shift, bias;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;
    BOOL     compA,repA,expA;

    //initialize the flags for combiner
    shift =bias = 0;
    compA=repA=expA=FALSE;

    //Set up some pointers to the individual texture stages.
    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    //initialize
    pContext->hwState.dwAlphaICW[0] = 0;
    pContext->hwState.dwAlphaOCW[0] = 0;
    pContext->hwState.dwAlphaICW[1] = 0;
    pContext->hwState.dwAlphaOCW[1] = 0; //0x10000000;

    //Set D = 1
    SetAlphaInputOne(pContext, celsiusCombinerInputD, 0, 0);

    //Check Stage1 Op
    switch (ptssState1->dwValue[D3DTSS_ALPHAOP])
    {
    case D3DTOP_ADDSIGNED:
        bias=1;
        break;
    case D3DTOP_ADDSIGNED2X:
        bias=1;
        shift++;
        break;
    case D3DTOP_SUBTRACT:
        if((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
        {
            //Set A*B+C*(-1)
            SetAlphaInputOne(pContext, celsiusCombinerInputD,0, 1);
        }
        else//Make the setting to be ((-A)*B))+C*1
        {
            compA ^=TRUE;
            expA=TRUE;
        }
        break;
    }

    //input A & B from stage 0
    pContext->hwState.dwNextAvailableTextureUnit=0;
    SelectAlphaInput(pContext, celsiusCombinerInputA, 0, 0, D3DTSS_ALPHAARG1,compA, expA);
    SelectAlphaInput(pContext, celsiusCombinerInputB, 0, 0, D3DTSS_ALPHAARG2,FALSE, FALSE);

    //input C from stage1
    pContext->hwState.dwNextAvailableTextureUnit=textureCount-1;
    if((ptssState1->dwValue[D3DTSS_ALPHAARG1]&D3DTA_SELECTMASK) == D3DTA_CURRENT)
        SelectAlphaInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_ALPHAARG2,FALSE, FALSE);
    else
        SelectAlphaInput(pContext, celsiusCombinerInputC, 0, 1, D3DTSS_ALPHAARG1,FALSE, FALSE);


    //Alpha Combiner OCW[0]
    ConstructOutputAlphaCombiners(pContext, TRUE, 0, bias, shift, 0);

    return (D3D_OK);
}//alphaModAdd
 /***********************************************************************************
 /*Alpha combiner setup
/***********************************************************************************/
static HRESULT SetAlphaCombiner(PNVD3DCONTEXT pContext, DWORD alphaCount, DWORD textureCount)
{
    DWORD combCount;

    //Initialize alpha combiners
    pContext->hwState.dwAlphaICW[0] = 0;
    pContext->hwState.dwAlphaOCW[0] = 0;
    pContext->hwState.dwAlphaICW[1] = 0;
    pContext->hwState.dwAlphaOCW[1] = 0;

    combCount=pContext->hwState.dwNumActiveCombinerStages;

    pContext->hwState.dwNextAvailableTextureUnit=0;


    if(alphaCount<=1)
        ConstructAlphaCombiners(pContext, 0, 0, TRUE );

    else if(alphaCount==2)
    {
        if(combCount==1)
        {
            if(alphaCheck2Stage1Combiner(pContext))
            {
                PNVD3DTEXSTAGESTATE     ptssState0, ptssState1;
                //Set up some pointers to the individual texture stages.
                ptssState0 = &pContext->tssState[0];
                ptssState1 = &pContext->tssState[1];

                if((ptssState0->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADD)
                   &&
                   ((ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE)   ||
                    (ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE2X) ||
                    (ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE4X)))
                {
                    if (alphaAddMod(pContext, textureCount)==D3D_OK)
                        return D3D_OK;
                }

                //Modulate, Add
                if((ptssState0->dwValue[D3DTSS_ALPHAOP] == D3DTOP_MODULATE)
                    &&((ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADD)
                    ||(ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADDSIGNED)
                    ||(ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_ADDSIGNED2X)
                    ||(ptssState1->dwValue[D3DTSS_ALPHAOP] == D3DTOP_SUBTRACT)))
                {
                    if (alphaModAdd(pContext, textureCount)==D3D_OK)
                        return D3D_OK;
                }
            }

            else
                pContext->hwState.dwColorOCW[1] = 0x20000000;//enable color combiner1
        }

        pContext->hwState.dwNumActiveCombinerStages = 2;

        pContext->hwState.dwNextAvailableTextureUnit=0;
        ConstructAlphaCombiners(pContext, 0,0, TRUE );
        pContext->hwState.dwNextAvailableTextureUnit=textureCount-1;
        ConstructAlphaCombiners(pContext, 1,1, TRUE );
    }
    else
        pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;

    return (D3D_OK);
}//end SetAlphaCombiner

/*************************************************************************
Set up color combiners for (X0 op X1) op X2 with 3 DX7 TSS:
*X0 is one of the colorargs from stage0
*X1 is one of the colorargs from stage1, cannot be D3DTA_CURRENT
*X2 is color arg from stage2, cannot be D3DTA_TEXTURE
In order the above is true:
*colorOp is stage0 has to be D3DTOP_SELECTARG1/2
*one and only one of the color args in stage1 to be D3DTA_CURRENT
*at least one of the color args in stage2 to be D3DTA_CURRENT

 op=all Modulateion,addition,subtraction and linear alpha blending
 Including D3DTOP_DUCTPRODUCT3 and D3DTOP_ADDSMOOTH

Only allow 2 alpha active stages, if has more, only look at the first 2
**************************************************************************/

#ifdef THREE_STAGE_ALPHA_SUPPORT

#define D3DTA_FLAGSMASK 0x000000f0
#define D3DTA_STAGEMASK 0xf0000000
#define D3DTA_STAGE0    0x00000000
#define D3DTA_STAGE1    0x10000000
#define D3DTA_STAGE2    0x20000000
#define D3DTA_STAGE3    0x30000000

HRESULT SelectAlphaInput2(PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, DWORD dwArg, BOOL bComplement, BOOL bInv) {
    DWORD dwICW = 0x10101010;

    switch (dwArg & D3DTA_SELECTMASK) {
    case D3DTA_DIFFUSE:   dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_4);   break;
    case D3DTA_CURRENT:   dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_C);   break;
    case D3DTA_TEXTURE:
        if ((dwArg & D3DTA_STAGEMASK) == D3DTA_STAGE0) {
            dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_8);
        } else {
            dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_9);
        }
        break;
    case D3DTA_TFACTOR:   dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_1);   break;
    case D3DTA_SPECULAR:  dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_5);   break;
    case D3DTA_TEMP:      dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_D);   break;
    }

    if (bComplement && bInv) {
        // not quite right, but close enough
        dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NORMAL);
    } else if (bComplement && !bInv) {
        dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_INVERT);
    } else if (!bComplement && bInv) {
        dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _SIGNED_NEGATE);
    }

    pContext->hwState.dwAlphaICW[dwCombinerStage] |= (dwICW << celsiusCombinerInputShift[eInput]);

    return TRUE;
}


BOOL nvCelsiusSet3StageAlphaCombiners(PNVD3DCONTEXT pContext, BOOL program) {
    NVD3DTEXSTAGESTATE tssState0 = pContext->tssState[0];
    NVD3DTEXSTAGESTATE tssState1 = pContext->tssState[1];
    NVD3DTEXSTAGESTATE tssState2 = pContext->tssState[2];

    if (tssState0.dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP) return FALSE;

    int current;
    if (tssState0.dwValue[D3DTSS_ALPHAOP] == D3DTOP_SELECTARG1) {
        current = tssState0.dwValue[D3DTSS_ALPHAARG1];
    } else if (tssState0.dwValue[D3DTSS_ALPHAOP] == D3DTOP_SELECTARG2) {
        current = tssState0.dwValue[D3DTSS_ALPHAARG2];
    } else {
        return FALSE;
    }

    tssState1.dwValue[D3DTSS_ALPHAARG1] |= D3DTA_STAGE1;
    tssState1.dwValue[D3DTSS_ALPHAARG2] |= D3DTA_STAGE1;

    if ((tssState1.dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK) == D3DTA_CURRENT) {
        tssState1.dwValue[D3DTSS_ALPHAARG1] = ((tssState1.dwValue[D3DTSS_ALPHAARG1] & D3DTA_FLAGSMASK) ^ (current & D3DTA_FLAGSMASK)) | current;
    }
    if ((tssState1.dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_CURRENT) {
        tssState1.dwValue[D3DTSS_ALPHAARG2] = ((tssState1.dwValue[D3DTSS_ALPHAARG2] & D3DTA_FLAGSMASK) ^ (current & D3DTA_FLAGSMASK)) | current;
    }


    // stage 0 current is really diffuse
    if ((tssState1.dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK) == D3DTA_CURRENT) {
        tssState1.dwValue[D3DTSS_ALPHAARG1] = (tssState1.dwValue[D3DTSS_ALPHAARG1] & D3DTA_FLAGSMASK) | D3DTA_DIFFUSE;
    }
    if ((tssState1.dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_CURRENT) {
        tssState1.dwValue[D3DTSS_ALPHAARG2] = (tssState1.dwValue[D3DTSS_ALPHAARG2] & D3DTA_FLAGSMASK) | D3DTA_DIFFUSE;
    }

    int shift=0, bias=0;
    int inputA=0xf, inputB=0xf, inputC=0xf, inputD=0xf, inputTemp;
    int invA=0, invB=0, invC=0, invD=0;
    int compA=0, compB=0, compC=0, compD=0;

    switch (tssState1.dwValue[D3DTSS_ALPHAOP]) {
    case D3DTOP_SELECTARG1:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG1];
        compB = 1;
        break;
    case D3DTOP_SELECTARG2:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG2];
        compB = 1;
        break;

    case D3DTOP_MODULATE4X:
        shift = 1;
    case D3DTOP_MODULATE2X:
        shift++;
    case D3DTOP_MODULATE:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG1];
        inputB = tssState1.dwValue[D3DTSS_ALPHAARG2];
        break;

    case D3DTOP_PREMODULATE:
    case D3DTOP_MODULATEALPHA_ADDCOLOR:
    case D3DTOP_MODULATECOLOR_ADDALPHA:
    case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
    case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
        break;

    case D3DTOP_ADDSIGNED2X:
        shift = 1;
    case D3DTOP_ADDSIGNED:
        bias = 1;
    case D3DTOP_ADD:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG1];
        compB = 1;
        inputC = tssState1.dwValue[D3DTSS_ALPHAARG2];
        compD = 1;
        break;

    case D3DTOP_SUBTRACT:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG1];
        compB = 1;
        inputC = tssState1.dwValue[D3DTSS_ALPHAARG2];
        compD = 1;
        invD = 1;
        break;

    case D3DTOP_ADDSMOOTH:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG1];
        inputB = tssState1.dwValue[D3DTSS_ALPHAARG2];
        compB = 1;
        inputC = tssState1.dwValue[D3DTSS_ALPHAARG2];
        compD = 1;
        break;

    case D3DTOP_BLENDDIFFUSEALPHA:
    case D3DTOP_BLENDTEXTUREALPHA:
    case D3DTOP_BLENDFACTORALPHA:
    case D3DTOP_BLENDTEXTUREALPHAPM:
    case D3DTOP_BLENDCURRENTALPHA:
        inputA = tssState1.dwValue[D3DTSS_ALPHAARG1];
        inputC = tssState1.dwValue[D3DTSS_ALPHAARG2];
        compD = 1;

        switch (tssState1.dwValue[D3DTSS_ALPHAOP]) {
        case D3DTOP_BLENDDIFFUSEALPHA:     inputTemp = D3DTA_DIFFUSE;  break;
        case D3DTOP_BLENDTEXTUREALPHA:     inputTemp = D3DTA_TEXTURE;  break;
        case D3DTOP_BLENDFACTORALPHA:      inputTemp = D3DTA_TFACTOR;  break;
        case D3DTOP_BLENDCURRENTALPHA:     inputTemp = current;        break;
        case D3DTOP_BLENDTEXTUREALPHAPM:   return FALSE;   // not supported yet
        }

        inputB = inputTemp;
        inputD = inputTemp;

        break;


    case D3DTOP_BUMPENVMAP:
    case D3DTOP_BUMPENVMAPLUMINANCE:
    case D3DTOP_DOTPRODUCT3:
    case D3DTOP_MULTIPLYADD:
    case D3DTOP_LERP:
        // not supported yet
        return FALSE;
    }

    if (program) {
        pContext->hwState.dwAlphaICW[0] = 0;
        pContext->hwState.dwAlphaOCW[0] = 0;
        SelectAlphaInput2(pContext, celsiusCombinerInputA, 0, inputA, compA, invA);
        SelectAlphaInput2(pContext, celsiusCombinerInputB, 0, inputB, compB, invB);
        SelectAlphaInput2(pContext, celsiusCombinerInputC, 0, inputC, compC, invC);
        SelectAlphaInput2(pContext, celsiusCombinerInputD, 0, inputD, compD, invD);
        ConstructOutputAlphaCombiners(pContext, TRUE, 0, bias, shift, tssState1.dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP);
    }

//////////////////////////////////////

    shift = 0;
    bias = 0;
    inputA = inputB = inputC = inputD = 0xf;
    invA = invB = invC = invD = 0;
    compA = compB = compC = compD = 0;

    if (((tssState2.dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK) == D3DTA_TEXTURE) ||
        ((tssState2.dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEXTURE) ||
        ((tssState2.dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK) == D3DTA_TEMP) ||
        ((tssState2.dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK) == D3DTA_TEMP))
        return FALSE;

    switch (tssState2.dwValue[D3DTSS_ALPHAOP]) {
    case D3DTOP_SELECTARG1:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG1];
        compB = 1;
        break;
    case D3DTOP_SELECTARG2:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG2];
        compB = 1;
        break;

    case D3DTOP_MODULATE4X:
        shift = 1;
    case D3DTOP_MODULATE2X:
        shift++;
    case D3DTOP_MODULATE:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG1];
        inputB = tssState2.dwValue[D3DTSS_ALPHAARG2];
        break;

    case D3DTOP_PREMODULATE:
    case D3DTOP_MODULATEALPHA_ADDCOLOR:
    case D3DTOP_MODULATECOLOR_ADDALPHA:
    case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
    case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
        break;

    case D3DTOP_ADDSIGNED2X:
        shift = 2;
    case D3DTOP_ADDSIGNED:
        bias = 2;
    case D3DTOP_ADD:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG1];
        compB = 2;
        inputC = tssState2.dwValue[D3DTSS_ALPHAARG2];
        compD = 2;
        break;

    case D3DTOP_SUBTRACT:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG1];
        compB = 2;
        inputC = tssState2.dwValue[D3DTSS_ALPHAARG2];
        compD = 2;
        invD = 2;
        break;

    case D3DTOP_ADDSMOOTH:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG1];
        inputB = tssState2.dwValue[D3DTSS_ALPHAARG2];
        compB = 2;
        inputC = tssState2.dwValue[D3DTSS_ALPHAARG2];
        compD = 2;
        break;

    case D3DTOP_BLENDDIFFUSEALPHA:
    case D3DTOP_BLENDTEXTUREALPHA:
    case D3DTOP_BLENDFACTORALPHA:
    case D3DTOP_BLENDTEXTUREALPHAPM:
    case D3DTOP_BLENDCURRENTALPHA:
        inputA = tssState2.dwValue[D3DTSS_ALPHAARG1];
        inputC = tssState2.dwValue[D3DTSS_ALPHAARG2];
        compD = 1;

        switch (tssState2.dwValue[D3DTSS_ALPHAOP]) {
        case D3DTOP_BLENDDIFFUSEALPHA:     inputTemp = D3DTA_DIFFUSE;  break;
        case D3DTOP_BLENDTEXTUREALPHA:     inputTemp = D3DTA_TEXTURE;  break;
        case D3DTOP_BLENDFACTORALPHA:      inputTemp = D3DTA_TFACTOR;  break;
        case D3DTOP_BLENDCURRENTALPHA:     inputTemp = D3DTA_CURRENT;  break;
        case D3DTOP_BLENDTEXTUREALPHAPM:   return FALSE;   // not supported yet
        }

        break;

    case D3DTOP_BUMPENVMAP:
    case D3DTOP_BUMPENVMAPLUMINANCE:
    case D3DTOP_DOTPRODUCT3:
    case D3DTOP_MULTIPLYADD:
    case D3DTOP_LERP:
        // not supported yet
        return FALSE;
    }


    if (program) {
        pContext->hwState.dwAlphaICW[1] = 0;
        pContext->hwState.dwAlphaOCW[1] = 0;
        SelectAlphaInput2(pContext, celsiusCombinerInputA, 1, inputA, compA, invA);
        SelectAlphaInput2(pContext, celsiusCombinerInputB, 1, inputB, compB, invB);
        SelectAlphaInput2(pContext, celsiusCombinerInputC, 1, inputC, compC, invC);
        SelectAlphaInput2(pContext, celsiusCombinerInputD, 1, inputD, compD, invD);
        ConstructOutputAlphaCombiners(pContext, TRUE, 1, bias, shift, tssState2.dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP);
    }

    return TRUE;
}
#endif

BOOL nvCelsiusCheck3Stage2Textures(PNVD3DCONTEXT pContext)
{
    if((Check3StageAlpha(pContext)
#ifdef THREE_STAGE_ALPHA_SUPPORT
        || nvCelsiusSet3StageAlphaCombiners(pContext, FALSE)
#endif
        )
        &&Check3StageColor(pContext))
        return TRUE;
    else
        return FALSE;
}

HRESULT nvCelsiusSetTextures3StageCombiners(PNVD3DCONTEXT pContext)
{

    PNVD3DTEXSTAGESTATE    ptssState0, ptssState1, ptssState2;
    DWORD                  alphaStageCount=0;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];

    dbgTracePush("nvCelsiusSetTextures3StageCombiners");

    while ((alphaStageCount < 8)
        &&(pContext->tssState[alphaStageCount].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE))
    {
        alphaStageCount++;
    }

    //Set up Color Combiners
   //Check for Add in Stage1
    if((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED2X)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSMOOTH))

    {
        //Cehck for AddMod which could be done in one combiner.
        if(((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
            ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT))
            &&((ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X))
            &&(((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_CURRENT)
            ^((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT)))
        {
            if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_CURRENT)
            {
                if(!((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
                    ||(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)))
                {
                    if (AddMod1(pContext)==D3D_OK)
                        goto FinalStep;
                }
            }
            else
            {
                //hack for single combiner (arg1 OP arg2) MOD diffuse where OP=subtract when
                //arg1=tex0, arg2=tex0,diffuse,etc. the math fits in one combiner stage but the result
                //does not match refrast. TODO: make two tex,3 stage subtract work in one combiner
                if((ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1)
                    &&((ptssState0->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK )== D3DTA_TEXTURE)
                    &&(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT))
                {
                       goto Normal2ColorCombiner;
                }

                if(!((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
                   ||(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)))
                {
                   if (AddMod1(pContext)==D3D_OK)
                       goto FinalStep;
                 }
            }
        }

Normal2ColorCombiner:

        //Fall to the normal 2 color combiner stage
        if((ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSMOOTH)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X)
            //Last stage, stage2: Color operation could be dotprocut3
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_DOTPRODUCT3))

        {
            Add1(pContext);
            AddModSelect2(pContext);
            goto FinalStep;
        }

        else //Something Wrong
            goto Invalid;
    }

    //Check for Modulate in stage 1
    else if ((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X))

    {
        if((ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSMOOTH)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X)
            //Last stage, stage2: Color operation could be dotprocut3
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_DOTPRODUCT3))
        {
            Mod1(pContext);
            AddModSelect2(pContext);
            goto FinalStep;
        }

        else //Something Wrong
            goto Invalid;
    }

    //Check for alphaBlend in Stage1
    else if((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_BLENDDIFFUSEALPHA)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_BLENDTEXTUREALPHA)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_BLENDFACTORALPHA)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_BLENDTEXTUREALPHAPM)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_BLENDCURRENTALPHA))
    {
        if((ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSMOOTH)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG1)
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_SELECTARG2)
            //Last stage, stage2: Color operation could be dotprocut3
            ||(ptssState2->dwValue[D3DTSS_COLOROP] == D3DTOP_DOTPRODUCT3))
        {
            Blend1(pContext);
            AddModSelect2(pContext);
            goto FinalStep;
        }
        else
            goto Invalid;
    }
    else
        goto Invalid;

Invalid:

    DPF ("Invalid COLOR OPERATION for 2 texture 3 stages");
    nvAssert(0);

    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
    dbgTracePop();
    return (D3D_OK);

FinalStep:
#ifdef THREE_STAGE_ALPHA_SUPPORT
    if (alphaStageCount > 2) {
        nvCelsiusSet3StageAlphaCombiners(pContext, TRUE);
    } else
#endif
    {
        SetAlphaCombiner (pContext,alphaStageCount, 2);
    }
    pContext->hwState.dwTexUnitToTexStageMapping[0]=0;
    pContext->hwState.dwTexUnitToTexStageMapping[1]=1;

#if COMBINER_STATUS
    DPF ("Result in 3 stage 2 set up");
    DPF ("   color icw[0] = 0x%08x", pContext->hwState.dwColorICW[0]);
    DPF ("   color ocw[0] = 0x%08x", pContext->hwState.dwColorOCW[0]);
    DPF ("   alpha icw[0] = 0x%08x", pContext->hwState.dwAlphaICW[0]);
    DPF ("   alpha ocw[0] = 0x%08x", pContext->hwState.dwAlphaOCW[0]);
    DPF ("   color icw[1] = 0x%08x", pContext->hwState.dwColorICW[1]);
    DPF ("   color ocw[1] = 0x%08x", pContext->hwState.dwColorOCW[1]);
    DPF ("   alpha icw[1] = 0x%08x", pContext->hwState.dwAlphaICW[1]);
    DPF ("   alpha ocw[1] = 0x%08x", pContext->hwState.dwAlphaOCW[1]);
    __asm int 3;
#endif
    dbgTracePop();
    return (D3D_OK);

} // nvCelsiusSetTextures3StagesCombiners

/**************************************************************
For special 2 DX texutre stages but only one combiner stage
case 1:
stage0: arg0+arg1
stage1: arg0*arg1  (arg0=current or arg1=current)

case 2:
stage0: arg0*arg1
stage1: arg0+arg1  (arg0=current or arg1=current)

+ = ADD, ADDSIGNED, ADDSIGNED2X, SUBTRACR
* = MODULATE, MODULATE2X, MODULATE4X, cannot be DOT3
***************************************************************/


BOOL nvCelsiusCheck2Stage1Combiner (PNVD3DCONTEXT pContext)
{
    if(colorCheck2Stage1Combiner(pContext)
        &&alphaCheck2Stage1Combiner(pContext))
        return TRUE;
    else
        return FALSE;
}//nvCelsiusCheck2Stage1Combiner


HRESULT nvCelsiusSet2Stage1Combiner (PNVD3DCONTEXT pContext)
{
    PNVD3DTEXSTAGESTATE    ptssState0, ptssState1;
    WORD alphaStageCount, textureCount;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];

    dbgTracePush("nvCelsiusSet2Stage1Combiner");

    alphaStageCount=textureCount=0;

    //Get the # of active alpha stages
    while ((alphaStageCount < 8)
        &&(pContext->tssState[alphaStageCount].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE))
    {
        alphaStageCount++;
    }

    //Get the  # of active textures
    if((((ptssState0->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG2))
        ||(((ptssState0->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState0->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1))
        ||(((ptssState0->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2))
        ||(((ptssState0->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState0->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)))
        textureCount++;

    if((((ptssState1->dwValue[D3DTSS_COLORARG1] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG2))
        ||(((ptssState1->dwValue[D3DTSS_COLORARG2] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState1->dwValue[D3DTSS_COLOROP] != D3DTOP_SELECTARG1))
        ||(((ptssState1->dwValue[D3DTSS_ALPHAARG1] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2))
        ||(((ptssState1->dwValue[D3DTSS_ALPHAARG2] & D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&(ptssState1->dwValue[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)))
        textureCount++;

    //Add, Modulate
    if ((ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
        &&
        ((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)   ||
         (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE2X) ||
         (ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE4X)))
    {
        if (colorAddMod(pContext,textureCount) == D3D_OK)
            goto Final;
        else
            goto Fail;
    }

    //Modulate, Add
    if((ptssState0->dwValue[D3DTSS_COLOROP] == D3DTOP_MODULATE)
        &&((ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADD)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_ADDSIGNED2X)
        ||(ptssState1->dwValue[D3DTSS_COLOROP] == D3DTOP_SUBTRACT)))
    {
        if(colorModAdd(pContext,textureCount)==D3D_OK)
            goto Final;
        else
            goto Fail;
    }


Final:
    SetAlphaCombiner(pContext, alphaStageCount, textureCount);

#if COMBINER_STATUS
    DPF ("Result in 3 stage 2 set up");
    DPF ("   color icw[0] = 0x%08x", pContext->hwState.dwColorICW[0]);
    DPF ("   color ocw[0] = 0x%08x", pContext->hwState.dwColorOCW[0]);
    DPF ("   alpha icw[0] = 0x%08x", pContext->hwState.dwAlphaICW[0]);
    DPF ("   alpha ocw[0] = 0x%08x", pContext->hwState.dwAlphaOCW[0]);
    DPF ("   color icw[1] = 0x%08x", pContext->hwState.dwColorICW[1]);
    DPF ("   color ocw[1] = 0x%08x", pContext->hwState.dwColorOCW[1]);
    DPF ("   alpha icw[1] = 0x%08x", pContext->hwState.dwAlphaICW[1]);
    DPF ("   alpha ocw[1] = 0x%08x", pContext->hwState.dwAlphaOCW[1]);
    __asm int 3;
#endif
    dbgTracePop();
    return (D3D_OK);

Fail:
    //reset the texture unit
    pContext->hwState.dwNextAvailableTextureUnit =0;
    dbgTracePop();
    return (DD_FALSE);
}//nvCelsiusSet2Stage1Combiner


/**************************************************************
*For special 3 DX texutre stages but only one combiner stage
***************************************************************/
BOOL nvCelsiusCheck3StageSpecial (PNVD3DCONTEXT pContext)
{

    PNVD3DTEXSTAGESTATE    ptssState0, ptssState1, ptssState2;
    DWORD                  alphaStageCount=0;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];


    //check for specular and fog off
    if((pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE])
        ||(pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]))
        return FALSE;

    //check for the # of active alpha stages
    while ((alphaStageCount < 8)
        &&(pContext->tssState[alphaStageCount].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE))
    {
        alphaStageCount++;
    }

    if(alphaStageCount>1)
        return FALSE;

    //check the color operations in all 3 stages
    if((ptssState0->dwValue[D3DTSS_COLOROP]!= D3DTOP_DOTPRODUCT3)
        ||(ptssState1->dwValue[D3DTSS_COLOROP]!= D3DTOP_MODULATE)
        ||(ptssState2->dwValue[D3DTSS_COLOROP]!= D3DTOP_MODULATE))
        return FALSE;

    //check for the stage0 args: one and only one arg to be texture
    if(!(((ptssState0->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        ^((ptssState0->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)))
        return FALSE;

    //check for the stage1 args:
    //arg1=texture arg2=current  or arg1=current, arg2=texture
    if(!((((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT))
        ||(((ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        &&((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_CURRENT))))
        return FALSE;

    //check for the stage2 args: at least one of the arg to be current
    if(((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)!= D3DTA_CURRENT)
        &&((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)!= D3DTA_CURRENT))
        return FALSE;

    //check for stage2 args: not texture should be used
    if(((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_TEXTURE)
        ||((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_TEXTURE))
        return FALSE;

    //alpha replication cannot be used in stage2 current
    if(((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)
        &&((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)== D3DTA_CURRENT))
        ||((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)
        &&((ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)== D3DTA_CURRENT)))
        return FALSE;

    //validate textures
    if(!((ptssState0->dwValue[D3DTSS_TEXTUREMAP])
        &&(ptssState1->dwValue[D3DTSS_TEXTUREMAP])))
        return FALSE;

    return TRUE;
}

HRESULT nvCelsiusSet3StageSpecial (PNVD3DCONTEXT pContext)
{
    // HW control words for SpecularFog combiner
    DWORD dwControl0, dwControl1;
    PNVD3DTEXSTAGESTATE     ptssState0, ptssState1, ptssState2;

    ptssState0 = &pContext->tssState[0];
    ptssState1 = &pContext->tssState[1];
    ptssState2 = &pContext->tssState[2];

    //Disable color combinr1
    pContext->hwState.dwNumActiveCombinerStages = 1;
    pContext->hwState.dwColorICW[1]=0;
    pContext->hwState.dwColorOCW[1]=0x10000000;

    //initialize alpha combiner
    pContext->hwState.dwAlphaICW[0]=0;
    pContext->hwState.dwAlphaOCW[0]=0;
    pContext->hwState.dwAlphaICW[1]=0;
    pContext->hwState.dwAlphaOCW[1]=0;

    //initialize SpecularFog combiner
    dwControl0=dwControl1=0;

    //map stage0 to color combiner0
    pContext->hwState.dwNextAvailableTextureUnit=0;
    ConstructColorCombiners (pContext, 0, 0);
    //alpha value is going to be pull from the result of color stage0
    //don't bother to canculate.

    pContext->hwState.dwNextAvailableTextureUnit=1;

    //Use SpecularFog combiner as another two layer of color combiner

    //A= current (E*F)
    dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _A_SOURCE, _REG_EF_PROD);

    //B= ARG in stage2

    //in stage2 arg1=current
    if((ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
    {
        //complement of A (current/arg1)
        if(ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _A_INVERSE, _TRUE);

        //complement and alpha replication of B (arg2)
        if(ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_INVERSE, _TRUE);

        if(ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_ALPHA, _TRUE);

        switch (ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_SELECTMASK)
        {
        case D3DTA_CURRENT:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_EF_PROD);
            break;

        case D3DTA_DIFFUSE:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_4);
            break;

        case D3DTA_TFACTOR:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_1);
            break;

        case D3DTA_SPECULAR:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_5);
            break;

        default:
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
            break;
        }
    }

    else //in stage2 arg2 is current
    {
        //complement of A (current/arg2)
        if(ptssState2->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _A_INVERSE, _TRUE);

        //complement and alpha replication of B (arg2)
        if(ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_INVERSE, _TRUE);

        if(ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_ALPHA, _TRUE);

        switch (ptssState2->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)
        {
        case D3DTA_CURRENT:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_EF_PROD);
            break;

        case D3DTA_DIFFUSE:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_4);
            break;

        case D3DTA_TFACTOR:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_1);
            break;

        case D3DTA_SPECULAR:
            dwControl0 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW0, _B_SOURCE, _REG_5);
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_COMBINERSNEEDSPECULAR;
            break;

        default:
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
            break;
        }
    }

    //C=0, D=0

    //E=T1, F=current

    dwControl1 |= (DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _E_SOURCE, _REG_9)|
                   DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _F_SOURCE, _REG_C));

    //in stage1 arg1=current
    if((ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_SELECTMASK)==D3DTA_CURRENT)
    {
        //complement and alphareplication of F (current/arg1)
        if(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _F_INVERSE, _TRUE);
        if(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _F_ALPHA, _TRUE);

        //complement and alphareplication of E (T1/arg2)
        if(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _E_INVERSE, _TRUE);
        if(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _E_ALPHA, _TRUE);
    }
    else  //in stage1 arg2=current
    {
        //complement and alphareplication of F (current/arg2)
        if(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_COMPLEMENT)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _F_INVERSE, _TRUE);
        if(ptssState1->dwValue[D3DTSS_COLORARG2]&D3DTA_ALPHAREPLICATE)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _F_ALPHA, _TRUE);

        //complement and alphareplication of E (T1/arg1)
        if(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_COMPLEMENT)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _E_INVERSE, _TRUE);
        if(ptssState1->dwValue[D3DTSS_COLORARG1]&D3DTA_ALPHAREPLICATE)
            dwControl1 |= DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _E_ALPHA, _TRUE);
    }



    //G= out put of color from stage0 (regC' color chanel)
    dwControl1 |= (DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _G_SOURCE, _REG_C)|
                   DRF_DEF(056, _SET_COMBINER_SPECULAR_FOG_CW1, _G_ALPHA, _FALSE));

    //Set the value
    pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW0, dwControl0);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_SPECULAR_FOG_CW1, dwControl1);

    pContext->hwState.dwDirtyFlags &= ~CELSIUS_DIRTY_SPECFOG_COMBINER;

    return D3D_OK;

}//nvCelsiusSet3Stage1Combiner


#endif // (NVARCH >= 0x10)

