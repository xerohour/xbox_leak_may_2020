//******************************Module*Header*******************************
// Module Name: nvdoublebuf.c
//
// Double buffer, windowed full screen flipping support.
//
// Copyright (c) 1998 NVidia Corporation
//*************************************************************************

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#pragma hdrstop
#include "nv32.h"
#include "Nvcm.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "nvReg.h"
#include "cliplist.h"
#include "oglsync.h"

//
// Export:
//
#include "nvdoublebuf.h"


/*
#define DOUBLE_PASS(PROC) {                             \
    ULONG FirstBufferOffset = ppdev->CurrentDestOffset; \
    ppdev->DoubleBuffer.SavedProcs.PROC;                \
    if (ppdev->CurrentDestOffset <                      \
        (ppdev->ulPrimarySurfaceOffset +                \
         (ppdev->lDelta * ppdev->cyScreen))) {          \
            ppdev->pfnSetDestBase(ppdev, ppdev->DoubleBuffer.SecondBufferOffset + ppdev->CurrentDestOffset, ppdev->CurrentDestPitch); \
            ppdev->DoubleBuffer.SavedProcs.PROC;                                                           \
            ppdev->pfnSetDestBase(ppdev, FirstBufferOffset, ppdev->CurrentDestPitch);                      \
    }                                                                                                      \
}
*/
#define DBL_SRC  0x0001
#define DBL_DEST 0x0002
#define DBL_PSO  0x0004

#define NOPFN 0 // NOP function used as call prior to double pumped PROC

//*************************************************************************
// DOUBLE_PASS
// 
// macro to douple pump low level GDI functions to all buffers in 
// ppdev->DoubleBuffer.MultiBufferOffsets.
// 
// PSO          optional SURFOBJ*
//              NOTE: call macro with '0' if not used!
// PROC         function including parameter list that has to be called
// PREPROC      optional code called prior to the double pumped PROC
//              NOTE: call macro with 'NOPFN' if not used!
// RET_STRING   optional code that is placed in front of the PROC call
//              used to set return values.
//              NOTE: Call macro with ';' if not used!
// Examples: code below
//*************************************************************************
#define DOUBLE_PASS(PSO,PROC,PREPROC,RET_STRING) {           \
    int i;                                                   \
    unsigned int doubleFlags;                                \
    ULONG LastDestOffset = ppdev->CurrentDestOffset;         \
    ULONG LastSourceOffset = ppdev->CurrentSourceOffset;     \
    VOID  *LastPSOPtr = NULL;                                \
    if ((PSO) && ((SURFOBJ*)(PSO))->dhsurf) {                \
        LastPSOPtr = ((DSURF*)((SURFOBJ*)(PSO))->dhsurf)->LinearPtr; \
    }                                                        \
    doubleFlags = GetDoubleFlags(ppdev,(SURFOBJ*)(PSO));     \
    RET_STRING ppdev->DoubleBuffer.SavedProcs.PROC;          \
    for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) {       \
        if (doubleFlags & DBL_SRC) {                         \
            ppdev->pfnSetSourceBase(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i] +                  \
                LastSourceOffset - ppdev->ulPrimarySurfaceOffset, ppdev->CurrentSourcePitch);           \
        }                                                                                               \
        if (doubleFlags & DBL_DEST) {                                                                   \
            ppdev->pfnSetDestBase(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i] +                    \
                LastDestOffset - ppdev->ulPrimarySurfaceOffset, ppdev->CurrentDestPitch);               \
        }                                                    \
        if (doubleFlags & DBL_PSO) {                         \
            (char *)((DSURF*)((SURFOBJ*)(PSO))->dhsurf)->LinearPtr = (char *) LastPSOPtr +              \
                ppdev->DoubleBuffer.MultiBufferOffsets[i] -  \
                ppdev->ulPrimarySurfaceOffset;               \
        }                                                    \
        if (doubleFlags & (DBL_DEST|DBL_PSO)) {              \
            PREPROC;                                         \
            RET_STRING ppdev->DoubleBuffer.SavedProcs.PROC;  \
        }                                                    \
    }                                                        \
    if (doubleFlags & DBL_SRC) {                             \
        ppdev->pfnSetSourceBase(ppdev, LastSourceOffset, ppdev->CurrentSourcePitch); \
    }                                                                                \
    if (doubleFlags & DBL_DEST) {                                                    \
        ppdev->pfnSetDestBase(ppdev, LastDestOffset, ppdev->CurrentDestPitch);       \
    }                                                                                \
    if (doubleFlags & DBL_PSO) {                                                     \
        ASSERTDD(NULL != LastPSOPtr, "Bad LastPSOPtr");                              \
        ((DSURF*)((SURFOBJ*)(PSO))->dhsurf)->LinearPtr = LastPSOPtr;                 \
    }                                                                                \
}

unsigned int GetDoubleFlags(PDEV* ppdev, SURFOBJ *pso)
{
    unsigned int doubleFlags = 0;
    NvU32        maxOffset;
    NvU32        newOffset;
    NvS32        delta;

    maxOffset = (ppdev->ulPrimarySurfaceOffset +                
                 (ppdev->lDelta * ppdev->cyScreen));

    if ((ppdev->CurrentSourceOffset >= ppdev->ulPrimarySurfaceOffset) &&
        (ppdev->CurrentSourceOffset < maxOffset)) {
        doubleFlags |= DBL_SRC;
    }
    if ((ppdev->CurrentDestOffset >= ppdev->ulPrimarySurfaceOffset) &&
        (ppdev->CurrentDestOffset < maxOffset)) {
        doubleFlags |= DBL_DEST;
    }
    // If only the source offset is inside of the primary
    // surface while the destination offset is offscreen, then
    // we have to make sure that we blit from memory that 
    // is actually onscreen since OpenGL may
    // have flipped the visible surface.  PaulP 10/11/2000
    if (doubleFlags == DBL_SRC) {
        if (OGL_FLIPPED()) {
            // The visible surface is flipped to the backbuffer
            // Need to set the source to the backbuffer.
            delta = ppdev->ulFlipBase - ppdev->ulPrimarySurfaceOffset;
            newOffset = ppdev->CurrentSourceOffset + delta;
            ppdev->pfnSetSourceBase(ppdev, newOffset, ppdev->CurrentSourcePitch);
        }
    }
    if (pso && pso->dhsurf) {
        VOID *LinearPtr = ((DSURF*)pso->dhsurf)->LinearPtr;
        if (((PUCHAR)LinearPtr >= ppdev->pjScreen) &&
            ((PUCHAR)LinearPtr < ppdev->pjFrameBufbase + maxOffset)) {
            doubleFlags |= DBL_PSO;
        }
    }

    return doubleFlags;
}

VOID NVDmaPushFillSolid_DBLBUF(       // Type FNFILL
    PDEV*           ppdev,
    LONG            c,              // Can't be zero
    RECTL*          prcl,           // List of rectangles to be filled, in relative
                                    //   coordinates
    ULONG           rop4,           // rop4
    RBRUSH_COLOR    rbc,            // Drawing colour is rbc.iSolidColor
    POINTL*         pptlBrush)      // Not used

{
    DOUBLE_PASS(0,pfnFillSolid(ppdev, c, prcl, rop4, rbc, pptlBrush),NOPFN,;);
}


VOID NVDmaPushFillPatFast_DBLBUF(            // Type FNFILL
    PDEV*           ppdev,
    LONG            c,              // Can't be zero
    RECTL*          prcl,           // List of rectangles to be filled, in relative
                                    //   coordinates
    ULONG           rop4,           // rop4
    RBRUSH_COLOR    rbc,            // rbc.prb points to brush realization structure
    POINTL*         pptlBrush)      // Pattern alignment

{
    DOUBLE_PASS(0,pfnFillPat(ppdev, c, prcl, rop4, rbc, pptlBrush),NOPFN,;);
}


VOID NVDmaPushXfer1bpp_DBLBUF(       // Type FNXFER
    PDEV*       ppdev,
    LONG        c,          // Count of rectangles, can't be zero
    RECTL*      prcl,       // List of destination rectangles, in relative
                            //   coordinates
    ROP4        rop4,       // rop4
    SURFOBJ*    psoSrc,     // Source surface
    POINTL*     pptlSrc,    // Original unclipped source point
    RECTL*      prclDst,    // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,     // Solid Color of pattern
    BLENDOBJ*   pBlendObj)
{
    DOUBLE_PASS(0,pfnXfer1bpp(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}


VOID NVDmaPushXfer4bpp_DBLBUF(     // Type FNXFER
    PDEV*       ppdev,
    LONG        c,          // Count of rectangles, can't be zero
    RECTL*      prcl,       // Array of relative coordinates destination rectangles
    ROP4        rop4,       // rop4
    SURFOBJ*    psoSrc,     // Source surface
    POINTL*     pptlSrc,    // Original unclipped source point
    RECTL*      prclDst,    // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,
    BLENDOBJ*   pBlendObj)  // Not used
{
    DOUBLE_PASS(0,pfnXfer4bpp(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}


VOID NVDmaPushXferNative_DBLBUF(     // Type FNXFER
    PDEV*       ppdev,
    LONG        c,          // Count of rectangles, can't be zero
    RECTL*      prcl,       // Array of relative coordinates destination rectangles
    ROP4        rop4,       // rop4
    SURFOBJ*    psoSrc,     // Source surface
    POINTL*     pptlSrc,    // Original unclipped source point
    RECTL*      prclDst,    // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,
    BLENDOBJ*   pBlendObj)  // Not used
{
    DOUBLE_PASS(0,pfnXferNative(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
/*    {                       
        unsigned int i, doubleFlags;                             
        ULONG LastDestOffset = ppdev->CurrentDestOffset;         
        ULONG LastSourceOffset = ppdev->CurrentSourceOffset;     
        doubleFlags = GetDoubleFlags(ppdev);                     
        ppdev->DoubleBuffer.SavedProcs.pfnXferNative(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj);                     
        for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) {       
            if (doubleFlags & DBL_SRC) {                         
                ppdev->pfnSetSourceBase(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i] +                  
                    LastSourceOffset - ppdev->ulPrimarySurfaceOffset, ppdev->CurrentSourcePitch); 
            }                                                                                               
            if (doubleFlags & DBL_DEST) {                                                                   
                ppdev->pfnSetDestBase(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i] +                    
                    LastDestOffset - ppdev->ulPrimarySurfaceOffset, ppdev->CurrentDestPitch);     
            }                                                    
            if (doubleFlags & DBL_DEST) {                                   
                ppdev->DoubleBuffer.SavedProcs.pfnXferNative(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj);  
            }                                                    
        }                                                        
        if (doubleFlags & DBL_SRC) {                             
            ppdev->pfnSetSourceBase(ppdev, LastSourceOffset, ppdev->CurrentSourcePitch); 
        }                                                                                
        if (doubleFlags & DBL_DEST) {                                                    
            ppdev->pfnSetDestBase(ppdev, LastDestOffset, ppdev->CurrentDestPitch);       
        }                                                                                
    }
    */
}


VOID NVDmaPushCopyBlt_DBLBUF(    // Type FNCOPY
    PDEV*   ppdev,
    LONG    c,          // Can't be zero
    RECTL*  prcl,       // Array of relative coordinates destination rectangles
    ULONG   rop4,       // rop4
    POINTL* pptlSrc,    // Original unclipped source point
    RECTL*  prclDst,    // Original unclipped destination rectangle
    BLENDOBJ *pBlendObj)
{
    DOUBLE_PASS(0,pfnCopyBlt(ppdev, c, prcl, rop4, pptlSrc, prclDst, pBlendObj),NOPFN,;);
/*    {                       
        unsigned int i, doubleFlags;                             
        ULONG LastDestOffset = ppdev->CurrentDestOffset;         
        ULONG LastSourceOffset = ppdev->CurrentSourceOffset;     
        doubleFlags = GetDoubleFlags(ppdev);                     
        ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, c, prcl, rop4, pptlSrc, prclDst, pBlendObj);                     
        for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) {       
            if (doubleFlags & DBL_SRC) {                         
                ppdev->pfnSetSourceBase(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i] +                  
                    LastSourceOffset - ppdev->ulPrimarySurfaceOffset, ppdev->CurrentSourcePitch); 
            }                                                                                               
            if (doubleFlags & DBL_DEST) {                                                                   
                ppdev->pfnSetDestBase(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i] +                    
                    LastDestOffset - ppdev->ulPrimarySurfaceOffset, ppdev->CurrentDestPitch);     
            }                                                    
            if (doubleFlags & DBL_DEST) {                                   
                ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, c, prcl, rop4, pptlSrc, prclDst, pBlendObj);  
            }                                                    
        }                                                        
        if (doubleFlags & DBL_SRC) {                             
            ppdev->pfnSetSourceBase(ppdev, LastSourceOffset, ppdev->CurrentSourcePitch); 
        }                                                                                
        if (doubleFlags & DBL_DEST) {                                                    
            ppdev->pfnSetDestBase(ppdev, LastDestOffset, ppdev->CurrentDestPitch);       
        }                                                                                
    }
*/
}


VOID NVDmaPushXfer4to16bpp_DBLBUF(     // Type FNXFER
    PDEV*       ppdev,
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    ROP4        rop4,               // rop4
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    RECTL*      prclDst,            // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,       // Solid Color of pattern - NOT USED
    BLENDOBJ*   pBlendObj)
{
    DOUBLE_PASS(0,pfnXfer4to16bpp(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}



VOID NVDmaPushXfer8to16bpp_DBLBUF(     // Type FNXFER
    PDEV*       ppdev,
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    ROP4        rop4,               // rop4
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    RECTL*      prclDst,            // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,       // Solid Color of pattern - NOT USED
    BLENDOBJ*   pBlendObj)
{
    DOUBLE_PASS(0,pfnXfer8to16bpp(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}


VOID NVDmaPushXfer4to32bpp_DBLBUF(     // Type FNXFER
    PDEV*       ppdev,
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    ROP4        rop4,               // rop4
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    RECTL*      prclDst,            // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,       // Solid Color of pattern - NOT USED
    BLENDOBJ*   pBlendObj)
{
    DOUBLE_PASS(0,pfnXfer4to32bpp(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}



VOID NVDmaPushXfer8to32bpp_DBLBUF(     // Type FNXFER
    PDEV*       ppdev,
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    ROP4        rop4,               // rop4
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    RECTL*      prclDst,            // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,       // Solid Color of pattern - NOT USED
    BLENDOBJ*   pBlendObj)
{
    DOUBLE_PASS(0,pfnXfer8to32bpp(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}


VOID NVDmaPushFastXfer8to32_DBLBUF(   // Type FastXFER
    PDEV*       ppdev,
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    XLATEOBJ*   pxlo)
{
    DOUBLE_PASS(0,pfnFastXfer8to32(ppdev, prcl, psoSrc, pptlSrc,pxlo),NOPFN,;);
}


VOID NVDmaPushFastXfer8to16_DBLBUF(   // Type FastXFER
    PDEV*       ppdev,
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    XLATEOBJ*   pxlo)
{
    DOUBLE_PASS(0,pfnFastXfer8to16(ppdev, prcl, psoSrc, pptlSrc,pxlo),NOPFN,;);
}

VOID NVDmaPushIndexedImage_DBLBUF(    // Type FNXFER

    PDEV*       ppdev,
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    ROP4        rop4,               // rop4
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    RECTL*      prclDst,            // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,       // Solid Color of pattern
    BLENDOBJ*   pBlendObj)
{
    DOUBLE_PASS(0,pfnIndexedImage(ppdev, c, prcl, rop4, psoSrc, pptlSrc, prclDst, pxlo, PatternColor, pBlendObj),NOPFN,;);
}


VOID NVDmaPushMemToScreenBlt_DBLBUF(
    PDEV*   ppdev,
    SURFOBJ* psoSrc,
    POINTL* pptlSrc,
    RECTL*  prclDst)
{
    DOUBLE_PASS(0,pfnMemToScreenBlt(ppdev, psoSrc, pptlSrc, prclDst),NOPFN,;);
}

VOID NVDmaPushMemToScreenWithPatternBlt_DBLBUF(
    PDEV*   ppdev,
    SURFOBJ* psoSrc,
    POINTL* pptlSrc,
    RECTL*  prclDst,
    ULONG rop3,
    RBRUSH_COLOR   *rbc,
    RECTL*  prclClip)
{
    DOUBLE_PASS(0,pfnMemToScreenPatternBlt(ppdev, psoSrc, pptlSrc, prclDst, rop3, rbc, prclClip),NOPFN,;);
}


BOOL NVDmaPushTextOut_DBLBUF(
    SURFOBJ*  pso,
    STROBJ*   pstro,
    FONTOBJ*  pfo,
    CLIPOBJ*  pco,
    RECTL*    prclOpaque,
    BRUSHOBJ* pboFore,
    BRUSHOBJ* pboOpaque)
{
    PDEV*  ppdev;
    BOOL retval;
    
    ppdev = (PDEV*) pso->dhpdev;

    // need to reset STROBJ in the case it was enumerated but not started properly
    DOUBLE_PASS(0,pfnTextOut(pso, pstro, pfo, pco, prclOpaque, pboFore, pboOpaque),STROBJ_vEnumStart(pstro),retval =);
    return retval;
}


//
//  Function bFlippingBufferAdjustPass
//
//  Function sets psoSrc-bufferoffset to ICD's frontbuffer if psoSrc
//  is our Primary and calls unflipped function
//
//  return value: return value of given  unflipped function
//
//  MSchwarzer 12/10/2000: new
//
BOOL bFlippingBufferAdjustPass( 
    FNSCRNTOMEM*    pfnScreenToMem,
    PDEV*           ppdev,
    LONG            c,
    RECTL*          prcl,
    SURFOBJ*        psoSrc,
    SURFOBJ*        psoDst,
    POINTL*         pptlSrc,
    RECTL*          prclDst,
    XLATEOBJ*       pxlo)
{
    BOOL bRet;
    PVOID pvLastSourceOffset;

    ASSERT(ppdev);
    ASSERT(psoSrc);
    ASSERT(psoSrc->dhsurf);     // Src is our Screen => we have an dhsurf !
    ASSERT(pfnScreenToMem);

    pvLastSourceOffset = ((DSURF*)psoSrc->dhsurf)->LinearPtr;

    // do we blt from Primary ?
    // if( STYPE_DEVICE == psoSrc->iType ) //: another possibility to test
    if(pvLastSourceOffset == ppdev->pjScreen)
    {
        if(OGL_FLIPPED()) 
        {
            // The visible surface is flipped to the backbuffer => set the source to the backbuffer.
            ((DSURF*)psoSrc->dhsurf)->LinearPtr = ppdev->pjScreen + ppdev->ulFlipBase;
        }
    }

    bRet = pfnScreenToMem(ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    // reset base of PSO
    ((DSURF*)psoSrc->dhsurf)->LinearPtr = pvLastSourceOffset;

    return bRet;
}


//
//  Function NVScreenTo1bppMemBlt_DBLBUF
//
//  Function is the wrapper of NV4ScreenTo1bppMemBlt for flipped doublepumped  
//  mode to set the psoSrc bufferoffset to ICD's frontbuffer
//
//  return value: return value of given  unflipped function (NV4ScreenTo1bppMemBlt)
//
//  MSchwarzer 12/10/2000: new
//
BOOL NVScreenTo1bppMemBlt_DBLBUF(
    PDEV     *ppdev,
    LONG      c,                  // Count of rectangles, can't be zero
    RECTL    *prcl,               // List of destination rectangles, in relative
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoDst,
    POINTL   *pptlSrc,
    RECTL    *prclDst,
    XLATEOBJ *pxlo)               // Provides colour-compressions information
{
    BOOL retval;

    ASSERT(ppdev);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt!=NVScreenTo1bppMemBlt_DBLBUF);
    
    retval = bFlippingBufferAdjustPass( ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt, ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    return retval;
}

//
//  Function NVScreenToMemBlt_DBLBUF
//
//  Function is the wrapper of NV4ScreenToMemBlt for flipped doublepumped  
//  mode to set the psoSrc bufferoffset to ICD's frontbuffer
//
//  return value: return value of given  unflipped function (NV4ScreenToMemBlt)
//
//  MSchwarzer 12/10/2000: new
//
BOOL NVScreenToMemBlt_DBLBUF(
    PDEV     *ppdev,
    LONG      c,                  // Count of rectangles, can't be zero
    RECTL    *prcl,               // List of destination rectangles, in relative
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoDst,
    POINTL   *pptlSrc,
    RECTL    *prclDst,
    XLATEOBJ *pxlo)               // Provides colour-compressions information
{
    BOOL retval;

    ASSERT(ppdev);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt!=NVScreenToMemBlt_DBLBUF);
    
    retval = bFlippingBufferAdjustPass( ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt, ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    return retval;
}

//
//  Function NVScreenToMem16to4bppBlt_DBLBUF
//
//  Function is the wrapper of NV4ScreenToMem16to4bppBlt for flipped doublepumped  
//  mode to set the psoSrc bufferoffset to ICD's frontbuffer
//
//  return value: return value of given  unflipped function (NV4ScreenToMem16to4bppBlt)
//
//  MSchwarzer 12/10/2000: new
//
BOOL NVScreenToMem16to4bppBlt_DBLBUF(
    PDEV     *ppdev,
    LONG      c,                  // Count of rectangles, can't be zero
    RECTL    *prcl,               // List of destination rectangles, in relative
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoDst,
    POINTL   *pptlSrc,
    RECTL    *prclDst,
    XLATEOBJ *pxlo)               // Provides colour-compressions information
{
    BOOL retval;

    ASSERT(ppdev);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to4bppBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt!=NVScreenToMem16to4bppBlt_DBLBUF);
    
    retval = bFlippingBufferAdjustPass( ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to4bppBlt, ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    return retval;
}

//
//  Function NVScreenToMem32to4bppBlt_DBLBUF
//
//  Function is the wrapper of NV4ScreenToMem32to4bppBlt for flipped doublepumped  
//  mode to set the psoSrc bufferoffset to ICD's frontbuffer
//
//  return value: return value of given  unflipped function (NV4ScreenToMem32to4bppBlt)
//
//  MSchwarzer 12/10/2000: new
//
BOOL NVScreenToMem32to4bppBlt_DBLBUF(
    PDEV     *ppdev,
    LONG      c,                  // Count of rectangles, can't be zero
    RECTL    *prcl,               // List of destination rectangles, in relative
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoDst,
    POINTL   *pptlSrc,
    RECTL    *prclDst,
    XLATEOBJ *pxlo)               // Provides colour-compressions information
{
    BOOL retval;

    ASSERT(ppdev);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to4bppBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt!=NVScreenToMem32to4bppBlt_DBLBUF);
    
    retval = bFlippingBufferAdjustPass( ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to4bppBlt, ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    return retval;
}

//
//  Function NVScreenToMem16to8bppBlt_DBLBUF
//
//  Function is the wrapper of NV4ScreenToMem16to8bppBlt for flipped doublepumped  
//  mode to set the psoSrc bufferoffset to ICD's frontbuffer
//
//  return value: return value of given  unflipped function (NV4ScreenToMem16to8bppBlt)
//
//  MSchwarzer 12/10/2000: new
//
BOOL NVScreenToMem16to8bppBlt_DBLBUF(
    PDEV     *ppdev,
    LONG      c,                  // Count of rectangles, can't be zero
    RECTL    *prcl,               // List of destination rectangles, in relative
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoDst,
    POINTL   *pptlSrc,
    RECTL    *prclDst,
    XLATEOBJ *pxlo)               // Provides colour-compressions information
{
    BOOL retval;

    ASSERT(ppdev);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to8bppBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt!=NVScreenToMem16to8bppBlt_DBLBUF);
    
    retval = bFlippingBufferAdjustPass( ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to8bppBlt, ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    return retval;
}

//
//  Function NVScreenToMem32to8bppBlt_DBLBUF
//
//  Function is the wrapper of NV4ScreenToMem32to8bppBlt for flipped doublepumped  
//  mode to set the psoSrc bufferoffset to ICD's frontbuffer
//
//  return value: return value of given  unflipped function (NV4ScreenToMem32to8bppBlt)
//
//  MSchwarzer 12/10/2000: new
//
BOOL NVScreenToMem32to8bppBlt_DBLBUF(
    PDEV     *ppdev,
    LONG      c,                  // Count of rectangles, can't be zero
    RECTL    *prcl,               // List of destination rectangles, in relative
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoDst,
    POINTL   *pptlSrc,
    RECTL    *prclDst,
    XLATEOBJ *pxlo)               // Provides colour-compressions information
{
    BOOL retval;

    ASSERT(ppdev);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to8bppBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt!=NVScreenToMem32to8bppBlt_DBLBUF);
    
    retval = bFlippingBufferAdjustPass( ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to8bppBlt, ppdev, c, prcl, psoSrc, psoDst, pptlSrc, prclDst, pxlo);

    return retval;
}



VOID NVScreenToScreenWithPatBlt_DBLBUF(
    
    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    BRUSHOBJ*   pbo,                // Brush object
    POINTL*     pptlBrush,          // Brush start coordinate in the pattern bits.
    ROP4        rop4)               // Op that describes the blit (must be 1 byte max)
{
    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->DoubleBuffer.SavedProcs.pfnScreenToScreenWithPatBlt);
    ASSERT(ppdev->DoubleBuffer.SavedProcs.pfnScreenToScreenWithPatBlt != NVScreenToScreenWithPatBlt_DBLBUF);

    DOUBLE_PASS(0,pfnScreenToScreenWithPatBlt(ppdev, c, parcl, pptlSrc, prclDst, pbo, pptlBrush, rop4),NOPFN,;);
}



BOOL NVDmaPushStrokePath_DBLBUF(
    SURFOBJ*   pso,
    PATHOBJ*   ppo,
    CLIPOBJ*   pco,
    XFORMOBJ*  pxo,
    BRUSHOBJ*  pbo,
    POINTL*    pptlBrush,
    LINEATTRS* pla,
    MIX        mix)
{
    PDEV*  ppdev;
    BOOL retval;
    
    ppdev = (PDEV*) pso->dhpdev;

    // need to reset PATHOBJ in the case it was enumerated but not started properly
    DOUBLE_PASS(pso,pfnStrokePath(pso, ppo, pco, pxo, pbo, pptlBrush, pla, mix),PATHOBJ_vEnumStart(ppo),retval =);
    return retval;
}


BOOL NVDmaPushLineTo_DBLBUF(
    SURFOBJ*    pso,
    CLIPOBJ*    pco,
    BRUSHOBJ*   pbo,
    LONG        x1,
    LONG        y1,
    LONG        x2,
    LONG        y2,
    RECTL*      prclBounds,
    MIX         mix)
{
    PDEV*  ppdev;
    BOOL retval;

    ppdev = (PDEV*) pso->dhpdev;
    
    DOUBLE_PASS(0,pfnLineTo(pso, pco, pbo, x1, y1, x2, y2, prclBounds, mix),NOPFN,retval =);
    return TRUE;
}

static unsigned int 
GetEngDoubleFlags(PDEV *ppdev, 
                  SURFOBJ *psoSrc, SURFOBJ *psoDst,
                  PVOID *oldSrcBits, PVOID *oldSrcScan,
                  PVOID *oldDstBits, PVOID *oldDstScan)
{
    unsigned int  doubleFlags = 0;
    NvU32         maxOffset;
    NvS32         delta;

    if (ppdev == NULL) {
        return doubleFlags;
    }

    maxOffset = (ppdev->ulPrimarySurfaceOffset +                
                 (ppdev->cyScreen * ppdev->lDelta ));
    
    // Check if destination is within the front screen
    // Now using pvScan0 instead of pvBits because Eng is using it, too. Had a bluescreen because
    // our code didn't set up pso->pvBits correctly.
    if (psoDst && ((PUCHAR)psoDst->pvScan0 >= ppdev->pjScreen) &&   /***********/
        ((PUCHAR)psoDst->pvScan0 <  ppdev->pjFrameBufbase + maxOffset)) {
        doubleFlags |= DBL_DEST;
        *oldDstBits = psoDst->pvBits;
        *oldDstScan = psoDst->pvScan0;
    }

    // Check if source is within the front screen
    if (psoSrc && ((PUCHAR)psoSrc->pvScan0 >= ppdev->pjScreen) &&
        ((PUCHAR)psoSrc->pvScan0 <  ppdev->pjFrameBufbase + maxOffset)) {
        doubleFlags |= DBL_SRC;
        *oldSrcBits = psoSrc->pvBits;
        *oldSrcScan = psoSrc->pvScan0;
    }
    // If only the source offset is inside of the primary
    // surface while the destination offset is offscreen, then
    // we have to make sure that we blit from memory that 
    // is actually onscreen since OpenGL may
    // have flipped the visible surface.  PaulP 10/11/2000
    if (doubleFlags == DBL_SRC) {
        if (OGL_FLIPPED()) {
            // The visible surface is flipped to the backbuffer
            // Need to set the source to the backbuffer.
            delta = ppdev->ulFlipBase - ppdev->ulPrimarySurfaceOffset;
            (char *) psoSrc->pvBits  += delta;
            (char *) psoSrc->pvScan0 += delta;
        }
    }
    return doubleFlags;
}


#define ENG_DOUBLE_PASS(PROC,PREPROC,RET_STRING) {           \
    int i;                                                   \
    unsigned int doubleFlags;                                \
    PVOID oldSrcBits, oldSrcScan;                            \
    PVOID oldDstBits, oldDstScan;                            \
    doubleFlags = GetEngDoubleFlags(ppdev,psoSrc,psoDst,&oldSrcBits,&oldSrcScan,&oldDstBits,&oldDstScan);     \
    RET_STRING PROC;                                         \
    if (ppdev != NULL) {                                     \
        for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) {   \
            if (doubleFlags & DBL_SRC) {                                                                      \
                (char *) psoSrc->pvBits = (char *) oldSrcBits + ppdev->DoubleBuffer.MultiBufferOffsets[i] -   \
                    ppdev->ulPrimarySurfaceOffset;                                                            \
                (char *) psoSrc->pvScan0 = (char *) oldSrcScan + ppdev->DoubleBuffer.MultiBufferOffsets[i] -  \
                    ppdev->ulPrimarySurfaceOffset;                                                            \
            }                                                                                                 \
            if (doubleFlags & DBL_DEST) {                                                                     \
                (char *) psoDst->pvBits = (char *) oldDstBits + ppdev->DoubleBuffer.MultiBufferOffsets[i] -   \
                    ppdev->ulPrimarySurfaceOffset;                                                            \
                (char *) psoDst->pvScan0 = (char *) oldDstScan + ppdev->DoubleBuffer.MultiBufferOffsets[i] -  \
                    ppdev->ulPrimarySurfaceOffset;                                                            \
            }                                                \
            if (doubleFlags & DBL_DEST) {                    \
                PREPROC;                                     \
                RET_STRING PROC;                             \
            }                                                \
        }                                                    \
        if (doubleFlags & DBL_SRC) {                         \
            psoSrc->pvBits  = oldSrcBits;                    \
            psoSrc->pvScan0 = oldSrcScan;                    \
        }                                                    \
        if (doubleFlags & DBL_DEST) {                        \
            psoDst->pvBits  = oldDstBits;                    \
            psoDst->pvScan0 = oldDstScan;                    \
        }                                                    \
    }                                                        \
}


BOOL APIENTRY NV_EngBitBlt_DBLBUF(
    SURFOBJ  *psoDst,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4      rop4)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    
    ENG_DOUBLE_PASS(EngBitBlt(psoDst,psoSrc,psoMask,pco,pxlo,prclTrg,pptlSrc,pptlMask,pbo,pptlBrush,rop4),NOPFN,retval &=);

    return retval;
}

BOOL APIENTRY NV_EngCopyBits_DBLBUF(
    SURFOBJ  *psoDst,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclDest,
    POINTL   *pptlSrc)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    
    ENG_DOUBLE_PASS(EngCopyBits(psoDst,psoSrc,pco,pxlo,prclDest,pptlSrc),NOPFN,retval &=);

    return retval;
}

BOOL APIENTRY NV_EngTextOut_DBLBUF(
    SURFOBJ  *psoDst,
    STROBJ   *pstro,
    FONTOBJ  *pfo,
    CLIPOBJ  *pco,
    RECTL    *prclExtra,
    RECTL    *prclOpaque,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque,
    POINTL   *pptlOrg,
    MIX       mix)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    SURFOBJ   *psoSrc = NULL;

    // need to reset STROBJ in the case it was enumerated but not started properly
    ENG_DOUBLE_PASS(EngTextOut(psoDst,pstro,pfo,pco,prclExtra,prclOpaque,pboFore,pboOpaque,pptlOrg,mix),STROBJ_vEnumStart(pstro),retval &=);

    return retval;
}

BOOL APIENTRY NV_EngStrokePath_DBLBUF(
    SURFOBJ   *psoDst,
    PATHOBJ   *ppo,
    CLIPOBJ   *pco,
    XFORMOBJ  *pxo,
    BRUSHOBJ  *pbo,
    POINTL    *pptlBrushOrg,
    LINEATTRS *plineattrs,
    MIX        mix)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    SURFOBJ   *psoSrc = NULL;
    
    // need to reset PATHOBJ in the case it was enumerated but not started properly
    ENG_DOUBLE_PASS(EngStrokePath(psoDst,ppo,pco,pxo,pbo,pptlBrushOrg,plineattrs,mix),PATHOBJ_vEnumStart(ppo),retval &=);

    return retval;
}

BOOL APIENTRY NV_EngLineTo_DBLBUF(
    SURFOBJ   *psoDst,
    CLIPOBJ   *pco,
    BRUSHOBJ  *pbo,
    LONG       x1,
    LONG       y1,
    LONG       x2,
    LONG       y2,
    RECTL     *prclBounds,
    MIX        mix)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    SURFOBJ   *psoSrc = NULL;
    
    ENG_DOUBLE_PASS(EngLineTo(psoDst,pco,pbo,x1,y1,x2,y2,prclBounds,mix),NOPFN,retval &=);

    return retval;
}

#if _WIN32_WINNT >= 0x0500
BOOL APIENTRY NV_EngAlphaBlend_DBLBUF(
    SURFOBJ       *psoDst,
    SURFOBJ       *psoSrc,
    CLIPOBJ       *pco,
    XLATEOBJ      *pxlo,
    RECTL         *prclDest,
    RECTL         *prclSrc,
    BLENDOBJ      *pBlendObj)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    
    ENG_DOUBLE_PASS(EngAlphaBlend(psoDst,psoSrc,pco,pxlo,prclDest,prclSrc,pBlendObj),NOPFN,retval &=);

    return retval;
    
}

BOOL APIENTRY NV_EngTransparentBlt_DBLBUF(
    SURFOBJ       *psoDst,
    SURFOBJ       *psoSrc,
    CLIPOBJ       *pco,
    XLATEOBJ      *pxlo,
    RECTL         *prclDest,
    RECTL         *prclSrc,
    ULONG         iTransColor,
    ULONG         Reserved)
{
    BOOL      retval = TRUE;
    PDEV*     ppdev = (PDEV*) psoDst->dhpdev;
    
    ENG_DOUBLE_PASS(EngTransparentBlt(psoDst,psoSrc,pco,pxlo,prclDest,prclSrc,iTransColor,Reserved),NOPFN,retval &=);

    return retval;
    
}
#endif


//*************************************************************************
// NV_InitDoubleBufferSavedProcs
// 
// Initialize the SaveProcs structure with all non double pumping 
// hardware functions.
//*************************************************************************
VOID NV_InitDoubleBufferSavedProcs(PPDEV ppdev)
{
    ASSERT(NULL!=ppdev);
    ASSERT(NULL!=ppdev->pfnFillSolid);

    ppdev->DoubleBuffer.SavedProcs.pfnFillSolid              = ppdev->pfnFillSolid;
    ppdev->DoubleBuffer.SavedProcs.pfnFillPat                = ppdev->pfnFillPat;
    ppdev->DoubleBuffer.SavedProcs.pfnXfer1bpp               = ppdev->pfnXfer1bpp;
    ppdev->DoubleBuffer.SavedProcs.pfnXfer4bpp               = ppdev->pfnXfer4bpp;
    ppdev->DoubleBuffer.SavedProcs.pfnXferNative             = ppdev->pfnXferNative;
    ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt                = ppdev->pfnCopyBlt;
                                               
    ppdev->DoubleBuffer.SavedProcs.pfnXfer4to16bpp           = ppdev->pfnXfer4to16bpp;
    ppdev->DoubleBuffer.SavedProcs.pfnXfer8to16bpp           = ppdev->pfnXfer8to16bpp;
    ppdev->DoubleBuffer.SavedProcs.pfnXfer4to32bpp           = ppdev->pfnXfer4to32bpp;
    ppdev->DoubleBuffer.SavedProcs.pfnXfer8to32bpp           = ppdev->pfnXfer8to32bpp;
    ppdev->DoubleBuffer.SavedProcs.pfnFastXfer8to32          = ppdev->pfnFastXfer8to32;
    ppdev->DoubleBuffer.SavedProcs.pfnFastXfer8to16          = ppdev->pfnFastXfer8to16;
    ppdev->DoubleBuffer.SavedProcs.pfnIndexedImage           = ppdev->pfnIndexedImage;
                                               
    ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenBlt         = ppdev->pfnMemToScreenBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenPatternBlt  = ppdev->pfnMemToScreenPatternBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnTextOut                = ppdev->pfnTextOut;
    ppdev->DoubleBuffer.SavedProcs.pfnSetPalette             = ppdev->pfnSetPalette;           
                                               
    ppdev->DoubleBuffer.SavedProcs.pfnStrokePath             = ppdev->pfnStrokePath;
    ppdev->DoubleBuffer.SavedProcs.pfnLineTo                 = ppdev->pfnLineTo;

    ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt     = ppdev->pfnScreenTo1BppMemBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt         = ppdev->pfnScreenToMemBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to4bppBlt = ppdev->pfnScreenToMem16to4bppBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to4bppBlt = ppdev->pfnScreenToMem32to4bppBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to8bppBlt = ppdev->pfnScreenToMem16to8bppBlt;
    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to8bppBlt = ppdev->pfnScreenToMem32to8bppBlt;

    ppdev->DoubleBuffer.SavedProcs.pfnScreenToScreenWithPatBlt = ppdev->pfnScreenToScreenWithPatBlt;
}


//*************************************************************************
// NV_CopyPrimaryToDoubleBuffer
// 
// Copy entire screen from primary to fullscreen surface with given offset.
//*************************************************************************
VOID NV_CopyPrimaryToDoubleBuffer(
    PPDEV ppdev, 
    ULONG ulSurfaceOffset)
{
    POINTL  ptlSrc;
    RECTL   rcl;

    ASSERT(NULL!=ppdev);
    ASSERT(ppdev->ulPrimarySurfaceOffset!=ulSurfaceOffset);

    // First copy the front buffer to the multi buffers
    rcl.left   = 0;
    rcl.top    = 0;
    rcl.right  = ppdev->cxScreen;
    rcl.bottom = ppdev->cyScreen;
    ptlSrc.x   = 0;
    ptlSrc.y   = 0;

    OglSyncEngineUsingPDEV(ppdev);

    ppdev->pfnSetSourceBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);

    // Hmm, can we assume that our dest surface has the same stride as the primary?
    ppdev->pfnSetDestBase(ppdev, ulSurfaceOffset, ppdev->lDelta);

    //
    // We only copy the GDI to the other buffers, not the OpenGL client which 
    // already might have a different content!
    //

    if (   (ppdev->OglNegativeClipList.c > 0)
        && (ulSurfaceOffset == ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT]) )
    {
        DISPDBG((3, "NV_CopyPrimaryToDoubleBuffer: copy %d exclusive primary to 0x%x", ppdev->OglNegativeClipList.c, ulSurfaceOffset));

        ASSERT(ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC);
        ASSERT(bClipListCheck(&ppdev->OglNegativeClipList));
        ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, ppdev->OglNegativeClipList.c, ppdev->OglNegativeClipList.prcl, 0xcccc, &ptlSrc, &rcl, NULL);
    }
    else
    {
        DISPDBG((3, "NV_CopyPrimaryToDoubleBuffer: copy primary to 0x%x", ulSurfaceOffset));
        // This will copy the entire screen to the back or other buffers
        // destroying their content. Visible backs are overwritten!
        ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, 1, &rcl, 0xcccc, &ptlSrc, &rcl, NULL);
    }

    // finish blit
    ppdev->pfnWaitEngineBusy(ppdev);
}


//*************************************************************************
// NV_ClearDoubleBuffer
// 
// Clear entire fullscreen surface with given offset.
//*************************************************************************
VOID NV_ClearDoubleBuffer(
    PPDEV ppdev, 
    ULONG ulSurfaceOffset)
{
    RECTL   rcl;
    RBRUSH_COLOR rbc = {0};        // Realized brush or solid colour

    ASSERT(NULL!=ppdev);
    ASSERT(ppdev->ulPrimarySurfaceOffset!=ulSurfaceOffset); // never want to clear primary

    // First copy the front buffer to the multi buffers
    rcl.left   = 0;
    rcl.top    = 0;
    rcl.right  = ppdev->cxScreen;
    rcl.bottom = ppdev->cyScreen;

    rbc.iSolidColor = 0;

    // Hmm, can we assume that our dest surface has the same stride as the primary?
    ppdev->pfnSetDestBase(ppdev, ulSurfaceOffset, ppdev->lDelta);
    if (ppdev->OglNegativeClipList.c > 0)
    {
        ASSERT(bClipListCheck(&ppdev->OglNegativeClipList));
        ppdev->DoubleBuffer.SavedProcs.pfnFillSolid(ppdev, ppdev->OglNegativeClipList.c, ppdev->OglNegativeClipList.prcl, 0xF0F0, rbc, NULL);
    }
    else
    {
        ppdev->DoubleBuffer.SavedProcs.pfnFillSolid(ppdev, 1, &rcl, 0xF0F0, rbc, NULL);
    }

    // finish blit
    ppdev->pfnWaitEngineBusy(ppdev);
}



//*************************************************************************
// NV_InitDoubleBufferMode
// 
// Initializes double buffer mode for the given buffers and resets the 
// function pointer list to DBLBUF functions.
//*************************************************************************
void NV_InitDoubleBufferMode(PDEV *ppdev, LONG NumBuffers, ULONG *MultiBufferOffsets)
{
    LONG    i;
    POINTL  ptlSrc;
    RECTL   prclDst;
    RECTL   rcl;
    DWORD   dwStatus;

    ASSERT(NULL != ppdev);
    ASSERT(0 != NumBuffers);
    ASSERT(NULL != MultiBufferOffsets);
    ASSERT(!ppdev->DoubleBuffer.bEnabled);

    if (ppdev->DoubleBuffer.bEnabled) {
        return;
    }

    ASSERT(0 == ppdev->DoubleBuffer.NumBuffers);

    ppdev->DoubleBuffer.bEnabled = TRUE;

    if (NumBuffers > NV_MAX_BUFFERS) {
        ASSERTDD(NumBuffers <= NV_MAX_BUFFERS,"Too many buffers");
        NumBuffers = NV_MAX_BUFFERS;
    }

    ppdev->DoubleBuffer.NumBuffers = NumBuffers;

    // need this buffer to exchange front to back when disabling flippingmode
    NVHEAP_ALLOC(dwStatus, ppdev->ulTempOffscreenBufferOffset, ppdev->lDelta, TYPE_IMAGE);
    if(dwStatus==0)
        ppdev->ulTempOffscreenBufferSize = ppdev->lDelta;
    else
    {
        ppdev->ulTempOffscreenBufferSize = 0;
        DISPDBG((0,"could not allocate temporary offscreen buffer in NV_InitDoubleBufferMode"));
    }

    for (i=0; i<NumBuffers; i++) {
        ASSERT(MultiBufferOffsets[i] != ppdev->ulPrimarySurfaceOffset);
        ppdev->DoubleBuffer.MultiBufferOffsets[i] = MultiBufferOffsets[i];
        NV_CopyPrimaryToDoubleBuffer(ppdev, MultiBufferOffsets[i]);
    }

    // Initialize save SavedProcs
    NV_InitDoubleBufferSavedProcs(ppdev);

    ppdev->pfnFillSolid              = NVDmaPushFillSolid_DBLBUF;
    ppdev->pfnFillPat                = NVDmaPushFillPatFast_DBLBUF;
    ppdev->pfnXfer1bpp               = NVDmaPushXfer1bpp_DBLBUF;   
    ppdev->pfnXfer4bpp               = NVDmaPushXfer4bpp_DBLBUF;   
    ppdev->pfnXferNative             = NVDmaPushXferNative_DBLBUF; 
    ppdev->pfnCopyBlt                = NVDmaPushCopyBlt_DBLBUF;    
                                                              
    ppdev->pfnXfer4to16bpp           = NVDmaPushXfer4to16bpp_DBLBUF; 
    ppdev->pfnXfer8to16bpp           = NVDmaPushXfer8to16bpp_DBLBUF; 
    ppdev->pfnXfer4to32bpp           = NVDmaPushXfer4to32bpp_DBLBUF; 
    ppdev->pfnXfer8to32bpp           = NVDmaPushXfer8to32bpp_DBLBUF; 
    ppdev->pfnFastXfer8to32          = NVDmaPushFastXfer8to32_DBLBUF; 
    ppdev->pfnFastXfer8to16          = NVDmaPushFastXfer8to16_DBLBUF; 
    ppdev->pfnIndexedImage           = NVDmaPushIndexedImage_DBLBUF;
                                                              
    ppdev->pfnMemToScreenBlt         = NVDmaPushMemToScreenBlt_DBLBUF;           
    ppdev->pfnMemToScreenPatternBlt  = NVDmaPushMemToScreenWithPatternBlt_DBLBUF;
    ppdev->pfnTextOut                = NVDmaPushTextOut_DBLBUF;
                                                              
    ppdev->pfnStrokePath             = NVDmaPushStrokePath_DBLBUF;
    ppdev->pfnLineTo                 = NVDmaPushLineTo_DBLBUF;

    ppdev->pfnScreenTo1BppMemBlt     = NVScreenTo1bppMemBlt_DBLBUF;
    ppdev->pfnScreenToMemBlt         = NVScreenToMemBlt_DBLBUF;
    ppdev->pfnScreenToMem16to4bppBlt = NVScreenToMem16to4bppBlt_DBLBUF;     
    ppdev->pfnScreenToMem32to4bppBlt = NVScreenToMem32to4bppBlt_DBLBUF;     
    ppdev->pfnScreenToMem16to8bppBlt = NVScreenToMem16to8bppBlt_DBLBUF;     
    ppdev->pfnScreenToMem32to8bppBlt = NVScreenToMem32to8bppBlt_DBLBUF;     

    ppdev->pfnScreenToScreenWithPatBlt = NVScreenToScreenWithPatBlt_DBLBUF;

    ppdev->pfnEngBitBlt     = NV_EngBitBlt_DBLBUF;
    ppdev->pfnEngCopyBits   = NV_EngCopyBits_DBLBUF;
    ppdev->pfnEngTextOut    = NV_EngTextOut_DBLBUF;
    ppdev->pfnEngLineTo     = NV_EngLineTo_DBLBUF;
    ppdev->pfnEngStrokePath = NV_EngStrokePath_DBLBUF;
#if _WIN32_WINNT >= 0x0500
    ppdev->pfnEngAlphaBlend = NV_EngAlphaBlend_DBLBUF;
    ppdev->pfnEngTransparentBlt = NV_EngTransparentBlt_DBLBUF;
#endif
}


//*************************************************************************
// NV_DisableDoubleBufferMode
// 
// disables double pumping to all buffers, restores function pointers
//*************************************************************************
void NV_DisableDoubleBufferMode(PDEV *ppdev)
{
    ASSERT(NULL != ppdev);

    if (!ppdev->DoubleBuffer.bEnabled) {
        return;
    }
    ppdev->DoubleBuffer.bEnabled = FALSE;

    ASSERT(0!=ppdev->DoubleBuffer.NumBuffers);

#if DBG
    {
        LONG i;
        for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) {
            NV_ClearDoubleBuffer(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i]);
        }
    }
#endif

    ppdev->DoubleBuffer.NumBuffers = 0;

    // free temporary doublebuffer-tempbuffer
    if(0!=ppdev->ulTempOffscreenBufferSize)
        NVHEAP_FREE(ppdev->ulTempOffscreenBufferOffset);
    ppdev->ulTempOffscreenBufferSize   = 0;
    ppdev->ulTempOffscreenBufferOffset = 0;

    ppdev->pfnFillSolid              = ppdev->DoubleBuffer.SavedProcs.pfnFillSolid              ;
    ppdev->pfnFillPat                = ppdev->DoubleBuffer.SavedProcs.pfnFillPat                ;
    ppdev->pfnXfer1bpp               = ppdev->DoubleBuffer.SavedProcs.pfnXfer1bpp               ;
    ppdev->pfnXfer4bpp               = ppdev->DoubleBuffer.SavedProcs.pfnXfer4bpp               ;
    ppdev->pfnXferNative             = ppdev->DoubleBuffer.SavedProcs.pfnXferNative             ;
    ppdev->pfnCopyBlt                = ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt                ;
                                                                                  
    ppdev->pfnXfer4to16bpp           = ppdev->DoubleBuffer.SavedProcs.pfnXfer4to16bpp           ;
    ppdev->pfnXfer8to16bpp           = ppdev->DoubleBuffer.SavedProcs.pfnXfer8to16bpp           ;
    ppdev->pfnXfer4to32bpp           = ppdev->DoubleBuffer.SavedProcs.pfnXfer4to32bpp           ;
    ppdev->pfnXfer8to32bpp           = ppdev->DoubleBuffer.SavedProcs.pfnXfer8to32bpp           ;
    ppdev->pfnFastXfer8to32          = ppdev->DoubleBuffer.SavedProcs.pfnFastXfer8to32          ;
    ppdev->pfnFastXfer8to16          = ppdev->DoubleBuffer.SavedProcs.pfnFastXfer8to16          ;
    ppdev->pfnIndexedImage           = ppdev->DoubleBuffer.SavedProcs.pfnIndexedImage           ;
                                                                                  
    ppdev->pfnMemToScreenBlt         = ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenBlt         ;
    ppdev->pfnMemToScreenPatternBlt  = ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenPatternBlt  ;
    ppdev->pfnTextOut                = ppdev->DoubleBuffer.SavedProcs.pfnTextOut                ;
                                                                                  
    ppdev->pfnStrokePath             = ppdev->DoubleBuffer.SavedProcs.pfnStrokePath             ;
    ppdev->pfnLineTo                 = ppdev->DoubleBuffer.SavedProcs.pfnLineTo                 ;

    ppdev->pfnScreenTo1BppMemBlt     = ppdev->DoubleBuffer.SavedProcs.pfnScreenTo1BppMemBlt     ;
    ppdev->pfnScreenToMemBlt         = ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt         ;
    ppdev->pfnScreenToMem16to4bppBlt = ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to4bppBlt ;     
    ppdev->pfnScreenToMem32to4bppBlt = ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to4bppBlt ;     
    ppdev->pfnScreenToMem16to8bppBlt = ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem16to8bppBlt ;     
    ppdev->pfnScreenToMem32to8bppBlt = ppdev->DoubleBuffer.SavedProcs.pfnScreenToMem32to8bppBlt ;     

    ppdev->pfnScreenToScreenWithPatBlt = ppdev->DoubleBuffer.SavedProcs.pfnScreenToScreenWithPatBlt ;     

    ppdev->pfnEngBitBlt     = EngBitBlt;
    ppdev->pfnEngCopyBits   = EngCopyBits;
    ppdev->pfnEngTextOut    = EngTextOut;
    ppdev->pfnEngLineTo     = EngLineTo;
    ppdev->pfnEngStrokePath = EngStrokePath;
#if _WIN32_WINNT >= 0x0500
    ppdev->pfnEngAlphaBlend = EngAlphaBlend;
#endif
}

//
// bDoublePumped
// 
// returns true if the surface offset belongs to a surface that is 2D double pumped
// 
BOOL bDoublePumped(PPDEV ppdev, ULONG ulSurfaceOffset)
{
    BOOL bRet=FALSE; // default exit: not double pumped
    LONG i;

    ASSERT(NULL!=ppdev);

    if (   ppdev->bOglSingleBackDepthSupported
        && (   ppdev->bOglSingleBackDepthCreated
            || ppdev->bOglOverlaySurfacesCreated
            || ppdev->ulOglActiveViews != 0) )
    {
        for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) 
        {
            ASSERT(ppdev->DoubleBuffer.bEnabled);
            if (ulSurfaceOffset == ppdev->DoubleBuffer.MultiBufferOffsets[i])
            {
                // this surface is double pumped
                bRet=TRUE;
                break;
            }
        }
    }

    return bRet;
}


//*************************************************************************
// NV_AddDoubleBufferSurface
// 
// Adds a surface to the double pumping code and activates double 
// buffering if not enabled.
// Returns TRUE if succeeded.
//*************************************************************************
BOOL NV_AddDoubleBufferSurface(
    PPDEV  ppdev, 
    ULONG  ulSurfaceOffset)
{
    BOOL bRet = FALSE;

    if (   ppdev->bOglSingleBackDepthSupported
        && ppdev->bOglSingleBackDepthCreated)
    {
        ASSERT(!bDoublePumped(ppdev, ulSurfaceOffset));
        ASSERT(ulSurfaceOffset != ppdev->ulPrimarySurfaceOffset);
        ASSERT(ppdev->DoubleBuffer.NumBuffers >= 0);

        if (ppdev->DoubleBuffer.bEnabled)
        {
            // Already active, just add a buffer and initialize it
            NV_CopyPrimaryToDoubleBuffer(ppdev, ulSurfaceOffset);

            ppdev->DoubleBuffer.MultiBufferOffsets[ppdev->DoubleBuffer.NumBuffers] = ulSurfaceOffset;
            ASSERT(ppdev->DoubleBuffer.NumBuffers < NV_MAX_BUFFERS);
            ppdev->DoubleBuffer.NumBuffers++;
        }
        else
        {
            NV_InitDoubleBufferMode(ppdev, 1, &ulSurfaceOffset);
        }

        bRet = TRUE;
    }

    return bRet;
}


//*************************************************************************
// NV_RemoveDoubleBufferSurface
// 
// remove the surface with the given offset from the double pumping list 
// and deactivate double buffering if it was the last buffer. 
// Returns TRUE if succeeded.
//*************************************************************************
BOOL NV_RemoveDoubleBufferSurface(
    PPDEV  ppdev, 
    ULONG  ulSurfaceOffset)
{
    LONG i;
    BOOL bRet = FALSE;

    ASSERT(NULL!=ppdev);

    if (   ppdev->bOglSingleBackDepthSupported
        && ppdev->bOglSingleBackDepthCreated)
    {
        ASSERT(bDoublePumped(ppdev, ulSurfaceOffset));
        ASSERT(ulSurfaceOffset != ppdev->ulPrimarySurfaceOffset);
        ASSERT(ppdev->DoubleBuffer.NumBuffers > 0);

        if (1 == ppdev->DoubleBuffer.NumBuffers)
        {
            NV_DisableDoubleBufferMode(ppdev);
        }
        else if (0 != ppdev->DoubleBuffer.NumBuffers)
        {
            // find offset in list (cannot fail!)
            for (i=0; i<ppdev->DoubleBuffer.NumBuffers; i++) 
            {
                if (ulSurfaceOffset == ppdev->DoubleBuffer.MultiBufferOffsets[i])
                {
                    break;
                }
            }

            #if DBG
            // clear surface and remove it from list
            NV_ClearDoubleBuffer(ppdev, ppdev->DoubleBuffer.MultiBufferOffsets[i]);
            #endif

            // move all ohter entries down to fill the hole
            for (/* i */; i<ppdev->DoubleBuffer.NumBuffers-1; i++) 
            {
                ppdev->DoubleBuffer.MultiBufferOffsets[i] = ppdev->DoubleBuffer.MultiBufferOffsets[i+1];
            }

            ppdev->DoubleBuffer.NumBuffers--;
        }

        ASSERT(ppdev->DoubleBuffer.NumBuffers >= 0);
        bRet = TRUE;
    }

    return bRet;
}


