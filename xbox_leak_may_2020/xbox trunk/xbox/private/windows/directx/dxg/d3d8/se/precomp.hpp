/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       precomp.hpp
 *  Content:    Common include file
 *
 ***************************************************************************/

// Wherever we call SetRenderState or SetTextureStageState internally is
// not speed-critical, so save some space by always calling the out-of-
// line version:

#define D3DCOMPILE_NOTINLINE 1

#ifdef STARTUPANIMATION
#define _NTSYSTEM_
#endif

#pragma code_seg("D3D")
#pragma data_seg("D3D_RW")
#pragma bss_seg("D3D_URW")
#pragma const_seg("D3D_RD")

#include <stdio.h>
#include <stddef.h>

extern "C" 
{
    #include <ntos.h>
    #include <nturtl.h>
    #include <av.h>
    #include <avmode.h>
};

#include <xtl.h>
#include <xdbg.h>
#include "xmetal.h"

#include "d3dver.h"

#include "nv32.h"
#include "nv_ref_2a.h"

// The following all encase themselves in the D3D namespace:

#include "hw.h"
#include "debug.hpp"
#include "mp.hpp"
#include "memalloc.h"
#include "misc.hpp"
#include "floatmath.hpp"
#include "math.hpp"
#include "memory.hpp"
#include "pixeljar.hpp"
#include "pusher.hpp"
#include "resource.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "enum.hpp"
#include "buffer.hpp"
#include "d3di.hpp"
#include "device.hpp"
#include "drawprim.hpp"
#include "state.hpp"
#include "stats.hpp"
#include "dumper.hpp"
#include "PatchUtil.hpp"
#include "patch.hpp"
#include "caps.hpp"
