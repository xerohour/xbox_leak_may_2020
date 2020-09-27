/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       PatchBackend.cpp
 *  Content:    High order surface implementation.
 *
 ****************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

void setFrontFace(NV_PATCH_INFO *info, int reversed)
{
    PPUSH pPush;
    CDevice* pDevice = g_pDevice;

    switch(D3D__RenderState[D3DRS_CULLMODE])
    {
    case D3DCULL_NONE:
        break;

    case D3DCULL_CW:
        reversed = 1 - reversed;

    case D3DCULL_CCW:

        pPush = pDevice->StartPush();

        if (reversed)
        {
            Push1(pPush, NV097_SET_FRONT_FACE, NV097_SET_FRONT_FACE_V_CW);
        } 
        else 
        {
            Push1(pPush, NV097_SET_FRONT_FACE, NV097_SET_FRONT_FACE_V_CCW);
        }

        pDevice->EndPush(pPush + 2);
        break;
    }
}

// routines for integer tessellations
void SendGuardCurve(unsigned long curvetype, NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, unsigned long flags)
{
    int i;
    NV_PATCH_CURVE_INFO *pTemp, *pTempN;
    unsigned long dwMethod;
    unsigned long preSteppedInnerCurve = 0x0;

    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    nvAssert(curvetype == NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE ||
             curvetype == NV097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE);


    //don't send guard curves if there aren't any attribs on that can be guarded
    if (!(info->evalEnables & (0x1 << NV_PATCH_ATTRIB_POSITION)) && !(flags & NV_PATCH_HAS_NORMAL)) { return; }

    //GUARD CURVE DATA SENT IN THIS ORDER!!!!!
    //(1) GUARDPOINT NORMAL (2) GUARDPOINT POSITION (3) GUARDCURVE POSITION (4) GUARDCURVE NORMAL

    if (NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE == curvetype) {
        pTemp  = (*quadInfo->pSwatchVBegin)[NV_PATCH_ATTRIB_POSITION];
        if (flags & NV_PATCH_HAS_NORMAL) {
            pTempN = (*quadInfo->pSwatchVBegin)[info->dstNormal];
        }

    } else {
        pTemp  = (*quadInfo->pSwatchVEnd)[NV_PATCH_ATTRIB_POSITION];
        if (flags & NV_PATCH_HAS_NORMAL)
            pTempN = (*quadInfo->pSwatchVEnd)[info->dstNormal];
    }

    //  ^-----^-----^
    // /|\ s /|\ s /|\
    //  |  w  |  w  |
    //  |  a  |  a  |
    //  |--t--o--t--|
    //  |  h  |  h  |
    //  o-----+-----o

    //TODO FIX THIS!!!!
    //if I'm an inner guard curve (i.e. a RIGHT guard curve, or left guard not on first row/last row) then step forward
    if (quadInfo->stitchVBegin
            && ( ((curvetype == NV097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE) && !(flags & NV_PATCH_SWATCH_LAST_COL))
                 || ((curvetype == NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE)  && !(flags & NV_PATCH_SWATCH_FIRST_COL))))
    {
        if (info->evalEnables & (0x1 << NV_PATCH_ATTRIB_POSITION)) OffsetCurve(info, pTemp,1);   //is position on?
        if (flags & NV_PATCH_HAS_NORMAL) OffsetCurve(info, pTempN,1);                            //do we have a normal?
        preSteppedInnerCurve = 0x1; //this is a hack because I prestep the curve here then step forward again for the guard pts later.
    }

    HOS_NOTE("Begin Curve (Guard Curve)");
    pPush = pDevice->StartPush();
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, curvetype);
    pDevice->EndPush(pPush + 2);

    //send guard point position
    if (flags & NV_PATCH_HAS_NORMAL) {
        //send guard point normal if active

        CopyCurve(info, &info->tempCurve, pTempN);
        OffsetCurve(info, &info->tempCurve, quadInfo->vMaxSegs - quadInfo->stitchVEnd - preSteppedInnerCurve);

        HOS_NOTE("Set Curve Coefficients");
        pPush = pDevice->StartPush(5);
        PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
        *(pPush + 1) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][0]);
        *(pPush + 2) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][1]);
        *(pPush + 3) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][2]);
        *(pPush + 4) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][3]);
        pDevice->EndPush(pPush + 5);
    }

    if (info->evalEnables & (0x1 << NV_PATCH_ATTRIB_POSITION)) {
        CopyCurve(info, &info->tempCurve, pTemp);
        OffsetCurve(info, &info->tempCurve, quadInfo->vMaxSegs - quadInfo->stitchVEnd - preSteppedInnerCurve);

        pPush = pDevice->StartPush(5);
        PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
        *(pPush + 1) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][0]);
        *(pPush + 2) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][1]);
        *(pPush + 3) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][2]);
        *(pPush + 4) = VIEW_AS_DWORD(info->tempCurve.coeffs[0][3]);
        pDevice->EndPush(pPush + 5);

        //send guard curve position
        for (i = 0; i < pTemp->order; i++) {
            HOS_NOTE("Set Curve Coefficients");
            pPush = pDevice->StartPush(5);
            PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
            *(pPush + 1) = VIEW_AS_DWORD(pTemp->coeffs[i][0]);
            *(pPush + 2) = VIEW_AS_DWORD(pTemp->coeffs[i][1]);
            *(pPush + 3) = VIEW_AS_DWORD(pTemp->coeffs[i][2]);
            *(pPush + 4) = VIEW_AS_DWORD(pTemp->coeffs[i][3]);
            pDevice->EndPush(pPush + 5);
        }
    }

    //send guard curve for normal if active
    if (flags & NV_PATCH_HAS_NORMAL) {
        //send guard curve...
        for (i = 0; i < pTempN->order; i++) {
            HOS_NOTE("Set Curve Coefficients");
            pPush = pDevice->StartPush(5);
            PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
            *(pPush + 1) = VIEW_AS_DWORD(pTempN->coeffs[i][0]);
            *(pPush + 2) = VIEW_AS_DWORD(pTempN->coeffs[i][1]);
            *(pPush + 3) = VIEW_AS_DWORD(pTempN->coeffs[i][2]);
            *(pPush + 4) = VIEW_AS_DWORD(pTempN->coeffs[i][3]);
            pDevice->EndPush(pPush + 5);
        }
    }
    pPush = pDevice->StartPush();
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    pDevice->EndPush(pPush + 2);

    return;
}

void ComputeKelvinHOSControlWords(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, long flags)
{
    int hwAttr, numcoeffs, order;
    unsigned int BPControl[4];
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();
    unsigned int t1, t2;
    int uMaxSegs = quadInfo->uMaxSegs;
    int vMaxSegs = quadInfo->vMaxSegs;
    int partialWidth, partialHeight, nSwatchU, nSwatchV;
    int doFrac = info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL;

    nvAssert(NV_PATCH_NUMBER_OF_ATTRIBS == 16);   //this is assumed in this loop that follows
    BPControl[0] = BPControl[1] = 0x0;
    numcoeffs = 0;

    for (hwAttr = 0; hwAttr <= 7; hwAttr++) {               //BPControl0
        if (info->evalEnables & (1 << hwAttr)) {
            order = info->maps[hwAttr].uorder;
            nvAssert(order-1 > 0);
            BPControl[0] |= ((order-1) << (hwAttr<<2));     //hwAttr*4
            numcoeffs+=order;
        }
    }

    for (hwAttr = 8; hwAttr < 16; hwAttr++) {               //BPControl1
        if (info->evalEnables & (1 << hwAttr)) {
            order = info->maps[hwAttr].uorder;
            nvAssert(order-1 > 0);
            BPControl[1] |= ((order-1) << (hwAttr<<2));
            numcoeffs+=order;
        }
    }       

    //in the fractional case we've intentionally rounded up by 1 to subsume the fractional portion of the transition
    //but we don't want to fool the HW with this notational convenience, drop by 1 here if necessary
    if (doFrac) { 
        //stitchUEnd/stitchVEnd are never set in the fractional case so decrement these in the non degenerate cases
        //because the partialwidth/height below won't account for it properly -- in the degenerate case
        //we don't have a last row or last col transition so DON'T decrement here... UGH.
        if (uMaxSegs>1) uMaxSegs--;     
        if (vMaxSegs>1) vMaxSegs--; 
    }
        
    partialWidth = (uMaxSegs - quadInfo->stitchUEnd) % info->maxSwatch;   //should truncate to integer...   
    partialHeight = (vMaxSegs - quadInfo->stitchVEnd) % info->maxSwatch;
    nSwatchU = (uMaxSegs - quadInfo->stitchUEnd - partialWidth)  / info->maxSwatch;
    nSwatchV = (vMaxSegs - quadInfo->stitchVEnd - partialHeight) / info->maxSwatch;   

    //if we have a degenerate regular grid -- ignore the normal
    //counts of SwatchU,SwatchV and set them to 0.  This is required by HW
    //despite appearances.  The Transition inside/outside params in the transition
    //control words will disambiguate this apparent strangeness.
    if (quadInfo->stitchUBegin || quadInfo->stitchUEnd) {
        if (uMaxSegs  <= 1) { nSwatchU = 0; nSwatchV = 0; }
    }
    if (quadInfo->stitchVBegin || quadInfo->stitchVEnd) {
        if (vMaxSegs  <= 1) { nSwatchU = 0; nSwatchV = 0; }
    }

    BPControl[2] = 0x0; //BPControl2
    nvAssert(info->maxSwatch <=17);

    //assumed as a QUAD..
    BPControl[2] = (partialHeight << 26) | (partialWidth << 21) | (info->maxSwatch << 16) | (nSwatchU << 8) | (nSwatchV << 0);

    //Don't turn this on because we don't actully support triangle patches natively... for future work
    //BPControl[2] |=  ((info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR ? 1 : 0) << 31 /*primitive type */);//
    if (doFrac) {        
        BPControl[3] = (NV097_SET_BEGIN_PATCH3_TESSELATION_FIXED_STITCH << 16); 
        if (quadInfo->uMaxSegs == 1 || quadInfo->vMaxSegs == 1) {
            BPControl[3] |= NV097_SET_BEGIN_PATCH3_ROW_TRNS_FIRST;
            BPControl[3] |= (NV097_SET_BEGIN_PATCH3_COL_TRNS_FIRST << 3);
        } else {
            BPControl[3] |= (NV097_SET_BEGIN_PATCH3_ROW_TRNS_FIRST_AND_LAST | (NV097_SET_BEGIN_PATCH3_COL_TRNS_FIRST_AND_LAST << 3));
        }

    } else {
        // row transitions
        t1 = (quadInfo->stitchVEnd ? 2 : 0) | (quadInfo->stitchVBegin ? 1 : 0);
        if (quadInfo->u0Dir != quadInfo->u1Dir  &&  t1) {
            // reverse transition, and not degenerate
            t1 |= 4;
        }
        nvAssert(t1 != 7);

        // col transitions
        t2 = (quadInfo->stitchUEnd ? 16 : 0) | (quadInfo->stitchUBegin ? 8 : 0);
        if (quadInfo->v0Dir != quadInfo->v1Dir  &&  t2) {
            // reverse col transition, and not degenerate
            t2 |= 0x20;
        }
        nvAssert(t2 != 0x38);
        BPControl[3] = t2 | t1;
    }

    if (quadInfo->uMaxDir ^ quadInfo->vMaxDir ^ info->flipUV) {
        BPControl[3] |= (1 << 14);
    }

    if (info->evalEnables & (1 << NV_PATCH_ATTRIB_POSITION)) {
        order = (info->maps[NV_PATCH_ATTRIB_POSITION].vorder - 1);
        nvAssert(order > 0);
        BPControl[3] |= (order << 6 );
    }
    if (flags & NV_PATCH_HAS_NORMAL || info->evalEnables & (1 << NV_PATCH_ATTRIB_NORMAL)) {
        order = (info->maps[info->dstNormal].vorder - 1);
        nvAssert(order > 0);
        BPControl[3] |= (order << 10);
    }
    BPControl[3] |= (numcoeffs << 24);

    HOS_NOTE("Set Begin Patch");
    pPush = pDevice->StartPush();
    Push1(pPush + 0, NV097_SET_BEGIN_PATCH0, BPControl[0]);
    Push1(pPush + 2, NV097_SET_BEGIN_PATCH1, BPControl[1]);
    Push1(pPush + 4, NV097_SET_BEGIN_PATCH2, BPControl[2]);
    Push1(pPush + 6, NV097_SET_BEGIN_PATCH3, BPControl[3]);
    pDevice->EndPush(pPush + 8);

    return;
}

} // end of namespace

