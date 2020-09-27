//******************************Module*Header***********************************
//
//  Module Name: fillpath.c
//
//  Contains the DrvFillPath routine.
//
//  Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "oglsync.h"
/******************************Public*Routine******************************\
* DrvFillPath
*
* Fill the specified path with the specified brush and ROP.  This routine
* detects single convex polygons, and will call to separate faster convex
* polygon code for those cases.  This routine also detects polygons that
* are really rectangles, and handles those separately as well.
*
* Note: Multiple polygons in a path cannot be treated as being disjoint;
*       the fill must consider all the points in the path.  That is, if the
*       path contains multiple polygons, you cannot simply draw one polygon
*       after the other (unless they don't overlap).
*
* Note: This function is optional, but is recommended for good performance.
*       To get GDI to call this function, not only do you have to
*       HOOK_FILLPATH, you have to set GCAPS_ALTERNATEFILL and/or
*       GCAPS_WINDINGFILL.
*
\**************************************************************************/

BOOL DrvFillPath(
SURFOBJ*    pso,
PATHOBJ*    ppo,
CLIPOBJ*    pco,
BRUSHOBJ*   pbo,
POINTL*     pptlBrush,
MIX         mix,
FLONG       flOptions)
{
    //**************************************************************************
    // NV: Not implemented.
    //**************************************************************************
    return FALSE;
}

