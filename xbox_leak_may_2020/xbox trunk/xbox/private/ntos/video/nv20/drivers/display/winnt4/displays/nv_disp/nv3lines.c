//******************************************************************************
//
// Module Name: NV3LINES.C
// 
// Contains most of the required GDI line support.  Supports drawing
// lines in short 'strips' when clipping is complex or coordinates
// are too large to be drawn by the line hardware.
// 
// Copyright (c) 1990-1996 Microsoft Corporation
// 
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"

#include "oldnv332.h"

#include "nvsubch.h"
#include "nvalpha.h"

//******************************************************************************
// NV:   Currently, we'll be using the NV1 line functions for NV3 since
//       they're compatible.  We can add the NV3 specific versions later...
//******************************************************************************

