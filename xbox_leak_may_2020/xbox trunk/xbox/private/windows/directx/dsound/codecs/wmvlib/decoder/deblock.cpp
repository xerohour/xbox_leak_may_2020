#include "bldsetup.h"

#include "xplatform.h"

#ifdef _SUPPORT_POST_FILTERS_
#include "limits.h"
#include "stdio.h"
#include "stdlib.h"
#include "typedef.hpp"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "postfilter_wmv.hpp"
#include <math.h>
#include "tables_wmv.h"

Void_WMV DeblockIFrame (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY,
    U8_WMV* ppxliU,
    U8_WMV* ppxliV,
    U32_WMV iMBStartY, U32_WMV iMBEndY
    )
{ 
    assert (pWMVDec->m_bDeblockOn);
    U8_WMV* ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U32_WMV imbY;
    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV* ppxliCodedPostY = ppxliPostY;
        U8_WMV* ppxliCodedPostU = ppxliPostU;
        U8_WMV* ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            
                g_pDeblockMB(
                    pWMVDec,
                    ppxliCodedPostY, 
                    ppxliCodedPostU, 
                    ppxliCodedPostV, 
                    imbX,                       // left
                    FALSE,                      // right half-edge
                    imbY,                       // top
                    FALSE,                      // bottom half-edge
                    TRUE_WMV,                       // middle
                    pWMVDec->m_iStepSize,
                    pWMVDec->m_iWidthPrevY,
                    pWMVDec->m_iWidthPrevUV);

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

    if (pWMVDec->m_bDeringOn == FALSE)
        return;

    ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV* ppxliCodedPostY = ppxliPostY;
        U8_WMV* ppxliCodedPostU = ppxliPostU;
        U8_WMV* ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            
                if (!((imbX == 0)            ||
                     (imbX == pWMVDec->m_uintNumMBX-1)||
                     (imbY == 0)             ||
                     (imbY == pWMVDec->m_uintNumMBY-1))) 
                     g_pDeringMB(ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,pWMVDec->m_iStepSize,pWMVDec->m_iWidthPrevY,pWMVDec->m_iWidthPrevUV);
      
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }
}


Void_WMV DeblockPFrame (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY,
    U8_WMV* ppxliU,
    U8_WMV* ppxliV,
    U32_WMV iMBStartY, U32_WMV iMBEndY
    )
{   
    assert (pWMVDec->m_bDeblockOn);
    U8_WMV* ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    const CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd + iMBStartY * pWMVDec->m_uintNumMBX;
    U32_WMV imbY;
	for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV* ppxliCodedPostY = ppxliPostY;
        U8_WMV* ppxliCodedPostU = ppxliPostU;
        U8_WMV* ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, pmbmd++) {
#ifndef _XBOX
            if (pmbmd->m_bSkip == FALSE) {
#else
            if (1) { // 
#endif
                Bool_WMV bdoBottomHalfEdge = FALSE, bdoRightHalfEdge = FALSE_WMV;
                if (imbX != (pWMVDec->m_uintNumMBX - 1)) {
                    bdoRightHalfEdge = (pmbmd + 1)->m_bSkip;
                }
                if (imbY != (pWMVDec->m_uintNumMBY - 1)) {
                    bdoBottomHalfEdge = (pmbmd + pWMVDec->m_uintNumMBX)->m_bSkip;
                }
                g_pDeblockMB(
                    pWMVDec,
                    ppxliCodedPostY, 
                    ppxliCodedPostU, 
                    ppxliCodedPostV, 
                    imbX,                           // left
                    bdoRightHalfEdge,               // right half-edge
                    imbY,                           // top
                    bdoBottomHalfEdge,              // bottom half-edge
                    TRUE_WMV,                           // middle
                    pWMVDec->m_iStepSize,
                    pWMVDec->m_iWidthPrevY,
                    pWMVDec->m_iWidthPrevUV);
            }
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY; 
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

    if (pWMVDec->m_bDeringOn == FALSE)
        return;

    ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pmbmd = pWMVDec->m_rgmbmd + iMBStartY * pWMVDec->m_uintNumMBX;
    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV* ppxliCodedPostY = ppxliPostY;
        U8_WMV* ppxliCodedPostU = ppxliPostU;
        U8_WMV* ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, pmbmd++) {
#ifndef _XBOX
            if (pmbmd->m_bSkip == FALSE) {
#else
            if (1) {
#endif
           
                if (!((imbX == 0)            ||
                     (imbX == pWMVDec->m_uintNumMBX-1)||
                     (imbY == 0)             ||
                     (imbY == pWMVDec->m_uintNumMBY-1))) 
                     g_pDeringMB(ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,pWMVDec->m_iStepSize,pWMVDec->m_iWidthPrevY,pWMVDec->m_iWidthPrevUV);
  
            }
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY; 
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }
}


Void_WMV AssignDeblokRoutines (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_iPostProcessMode < 0 || pWMVDec->m_iPostProcessMode >= 5) 
        return;

    if (pWMVDec->m_iPostProcessMode == 4) {
        pWMVDec->m_pDecodeI = WMVideoDecDecodeIDeblock;
        pWMVDec->m_pDecodeP = WMVideoDecDecodePDeblock;
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = TRUE_WMV;
        g_InitPostFilter (FALSE);
    }
    else if (pWMVDec->m_iPostProcessMode == 3) {
        pWMVDec->m_pDecodeI = WMVideoDecDecodeIDeblock;
        pWMVDec->m_pDecodeP = WMVideoDecDecodePDeblock;
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = TRUE_WMV;
        g_InitPostFilter (TRUE_WMV);
    }
    else if (pWMVDec->m_iPostProcessMode == 2) {
        pWMVDec->m_pDecodeI = WMVideoDecDecodeIDeblock;
        pWMVDec->m_pDecodeP = WMVideoDecDecodePDeblock;
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = FALSE_WMV;
        g_InitPostFilter (FALSE);
    }
    else if (pWMVDec->m_iPostProcessMode == 1) {
        pWMVDec->m_pDecodeI = WMVideoDecDecodeIDeblock;
        pWMVDec->m_pDecodeP = WMVideoDecDecodePDeblock;
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = FALSE_WMV;
        g_InitPostFilter (TRUE_WMV);
    }
    else {
        pWMVDec->m_pDecodeI = WMVideoDecDecodeI;
        pWMVDec->m_pDecodeP = WMVideoDecDecodeP;
        pWMVDec->m_bDeblockOn = FALSE_WMV;
        pWMVDec->m_bDeringOn = FALSE_WMV;
    }
}

#endif // _SUPPORT_POST_FILTERS_
