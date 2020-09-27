
#if !defined(IS_OPENGL)
  #include "nvprecomp.h"
#endif

/*****************************************************************************/
#if !defined(IS_OPENGL)
static NV_INLINE void CopyPoint(float *dst, const float *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}
#endif

/*****************************************************************************/
/*****************************************************************************/

// routines for integer tessellations
void SendGuardCurve(unsigned long curvetype, NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, unsigned long flags)
{
    unsigned long dwCount = 0x0;
    int i;
    NV_PATCH_CURVE_INFO *pTemp, *pTempN;
    float *pBegin, *pBeginN;
    float *pEnd, *pEndN;
    unsigned long dwMethod;

    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    nvAssert(curvetype == NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE ||
             curvetype == NV097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE);


    //don't send guard curves if there aren't any attribs on that can be guarded
    if (!(info->evalEnables & (0x1 << NV_PATCH_ATTRIB_POSITION)) && !(flags & NV_PATCH_HAS_NORMAL)) { return; }

    //GUARD CURVE DATA SENT IN THIS ORDER!!!!!
    //(1) GUARDPOINT NORMAL (2) GUARDPOINT POSITION (3) GUARDCURVE POSITION (4) GUARDCURVE NORMAL

    if (NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE == curvetype) {
        pTemp  = (*quadInfo->pSwatchVBegin)[ATTR_V];
        pBegin = info->pSwatchCorner[ATTR_V][0][0];
        pEnd = info->pSwatchCorner[ATTR_V][1][0];
        if (flags & NV_PATCH_HAS_NORMAL) {
            pTempN = (*quadInfo->pSwatchVBegin)[info->dstNormal];
            pBeginN = info->pSwatchCorner[info->dstNormal][0][0];
            pEndN = info->pSwatchCorner[info->dstNormal][1][0];
        }

    } else {
        pTemp  = (*quadInfo->pSwatchVEnd)[ATTR_V];
        pBegin = info->pSwatchCorner[ATTR_V][0][1];
        pEnd = info->pSwatchCorner[ATTR_V][1][1];
        if (flags & NV_PATCH_HAS_NORMAL)
            pTempN = (*quadInfo->pSwatchVEnd)[info->dstNormal];
            pBeginN = info->pSwatchCorner[info->dstNormal][0][1];
            pEndN = info->pSwatchCorner[info->dstNormal][1][1];
    }

    //  ^-----^-----^
    // /|\ s /|\ s /|\
    //  |  w  |  w  |
    //  |  a  |  a  |
    //  |--t--o--t--|
    //  |  h  |  h  |
    //  o-----+-----o

    //if I'm an inner guard curve (i.e. a RIGHT guard curve, or left guard not on first row/last row) then step forward
    if (quadInfo->stitchVBegin
            && ( ((curvetype == NV097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE) && !(flags & NV_PATCH_SWATCH_LAST_COL))
                 || ((curvetype == NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE)  && !(flags & NV_PATCH_SWATCH_FIRST_COL))))
    {
        if (info->evalEnables & (0x1 << NV_PATCH_ATTRIB_POSITION)) {
            // position is on
            OffsetCurve(info, pTemp,1);
            // and tweak to swatch corner
            CopyPoint(pTemp->coeffs[0], pBegin);
        }
        if (flags & NV_PATCH_HAS_NORMAL) {
            // normal is on
            OffsetCurve(info, pTempN,1);
            // and tweak to swatch corner
            CopyPoint(pTempN->coeffs[0], pBeginN);
        }
    }

    HOS_NOTE("Begin Curve (Guard Curve)");
    dwMethod = ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE));
    HOS_PUSH(dwCount, dwMethod);
    HOS_PUSH(dwCount+1, curvetype);
    dwCount += 0x2;

    // send guard point normal
    if (flags & NV_PATCH_HAS_NORMAL) {
        //send guard point normal if active
        HOS_NOTE("Set Curve Coefficients");
        HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
        HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pEndN[0]));
        HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pEndN[1]));
        HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pEndN[2]));
        HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pEndN[3]));
        dwCount+=5;
    }
    HOS_PUSH_ADJUST(dwCount);

    // send guard point position
    dwCount = 0;
    if (info->evalEnables & (0x1 << NV_PATCH_ATTRIB_POSITION)) {
        HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
        HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pEnd[0]));
        HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pEnd[1]));
        HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pEnd[2]));
        HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pEnd[3]));
        dwCount+=5;

        //send guard curve position
        for (i = 0; i < pTemp->order; i++) {
            HOS_NOTE("Set Curve Coefficients");
            HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
            HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pTemp->coeffs[i][0]));
            HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pTemp->coeffs[i][1]));
            HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pTemp->coeffs[i][2]));
            HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pTemp->coeffs[i][3]));
            dwCount+=5;
        }
        HOS_PUSH_ADJUST(dwCount);
    }

    //send guard curve for normal if active
    dwCount = 0;
    if (flags & NV_PATCH_HAS_NORMAL) {
        //send guard curve...
        for (i = 0; i < pTempN->order; i++) {
            HOS_NOTE("Set Curve Coefficients");
            HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
            HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pTempN->coeffs[i][0]));
            HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pTempN->coeffs[i][1]));
            HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pTempN->coeffs[i][2]));
            HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pTempN->coeffs[i][3]));
            dwCount+=5;
        }
    }
    HOS_PUSH(dwCount, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(dwCount+1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    dwCount+=2;
    HOS_PUSH_ADJUST(dwCount);

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
            BPControl[1] |= ((order-1) << ((hwAttr-8)<<2));
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
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_PATCH0)));
    HOS_PUSH(1, BPControl[0]);
    HOS_PUSH(2, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_PATCH1)));
    HOS_PUSH(3, BPControl[1]);
    HOS_PUSH(4, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_PATCH2)));
    HOS_PUSH(5, BPControl[2]);
    HOS_PUSH(6, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_PATCH3)));
    HOS_PUSH(7, BPControl[3]);
    HOS_PUSH_ADJUST(8);

    return;
}

