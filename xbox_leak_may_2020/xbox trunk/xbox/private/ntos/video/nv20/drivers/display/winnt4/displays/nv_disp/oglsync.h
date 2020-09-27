#ifndef _OGLSYNC_H
#define _OGLSYNC_H
//******************************Module*Header***********************************
// Module Name: oglsync.h
//
// interface file for syncing routines GDI against ogl, and ogl against ogl
//
// Copyright (c) 2001, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

void OglSyncEngineUsingPDEV(PDEV *ppdev);
#ifdef DISABLE_OGLSYNC
    #pragma message("  FNicklisch: WARNING: OglSync against GDI is disabled, shouldn't be default!")
    #define bOglSyncOglIfGdiRenderedToWindow(ppdev)                                     TRUE
    #define bOglSyncIfGdiInOglWindowRect(ppdev,pso,prclTrg,pco)                         TRUE
    #define bOglSyncIfGdiInOglWindowPath(ppdev,pso,ppo,pco)                             TRUE
    #define bOglSyncIfGdiInOglWindowBlt(ppdev,psoDst,psoSrc,pco,prclDst,pptlSrc)        TRUE
    #define bOglSyncIfGdiInOglWindowStretch(ppdev,psoDst,psoSrc,pco,prclDst,prclSrc)    TRUE
#else

BOOL bOglSyncOglIfGdiRenderedToWindow(
    IN PPDEV ppdev);

BOOL bOglSyncIfGdiInOglWindowRect(
    IN PPDEV    ppdev,
    IN SURFOBJ *pso,        // surface belonging to prclBounds
    IN RECTL   *prclTrg,    // destination rectangle of drawing surface
    IN CLIPOBJ *pco);       // clip obj for destination

BOOL bOglSyncIfGdiInOglWindowPath(
    IN PPDEV    ppdev,
    IN SURFOBJ *pso,        // surface belonging to object
    IN PATHOBJ *ppo,        // pathobject to be rendered on pso
    IN CLIPOBJ *pco);       // clip obj for destination

BOOL bOglSyncIfGdiInOglWindowBlt(
    IN PPDEV    ppdev,
    IN SURFOBJ *psoDst,     // surface belonging to dst object     
    IN SURFOBJ *psoSrc,     // surface belonging to src object     
    IN CLIPOBJ *pco,        // clip obj for destination        
    IN RECTL   *prclDst,    // destination rectangle
    IN POINTL  *pptlSrc);   // source point

BOOL bOglSyncIfGdiInOglWindowStretch(
    IN PPDEV    ppdev,
    IN SURFOBJ *psoDst,     // surface belonging to dst object     
    IN SURFOBJ *psoSrc,     // surface belonging to src object     
    IN CLIPOBJ *pco,        // clip obj for destination        
    IN RECTL   *prclDst,    // destination rectangle
    IN RECTL   *prclSrc);   // source point
#endif //DISABLE_OGLSYNC

#endif // _OGLSYNC_H
