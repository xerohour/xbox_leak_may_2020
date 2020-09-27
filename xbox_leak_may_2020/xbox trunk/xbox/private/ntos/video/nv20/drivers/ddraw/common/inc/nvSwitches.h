// **************************************************************************
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
//  Module: nvSwitches.h
//     compile-time switches used in the D3D driver
//
// **************************************************************************
//
//  History:
//       Craig Duttweiler    (bertrem)   15Mar00     created
//
// **************************************************************************

#ifndef NVDD32
#define NVDD32                              // so shared files know what we are talking about
#endif

//---------------------------------------------------------------------------
// general d3d and external functionality
//---------------------------------------------------------------------------

//#define DO_NOT_SUPPORT_VERTEX_BUFFERS       // define to disable d3d vb exports (we use the internal vb still)
#define HOSURF_ENABLE               // enable HOS support
//#define DX7PROPER                           // emit proper dx7 usage
//#define TEX_MANAGE                          // enable texture management
//#define MULTISTAGE_BUMPMAPPING              // enable code to detect special 4- and 8-stage TSS cases
#define CELSIUS_NVCOMBINER                  // enable special NV multi-stage combiner settings in celsius
#ifndef WINNT
#define STEREO_SUPPORT                      // enable stereo support
#endif

//---------------------------------------------------------------------------
// Release 20 functionality
//---------------------------------------------------------------------------

#ifdef WINNT
#define NV_AGP       // NVIDIA managed AGP (does not request AGP heap from MS, required for winXP dual-view)
#endif

//#define REL20

#ifdef REL20

//#define KPFS
//#define LPC_OPT

#endif // REL20



//---------------------------------------------------------------------------
// internal functionality
//---------------------------------------------------------------------------

#define HWFLIP                              // enable use of NV15 ff. HW flip
//#define KELVIN_SEMAPHORES                   // employ kelvin-style semaphores for HW flipping
//#define KELVIN_SEMAPHORES2                  // employ kelvin-style semaphores in place of ref counts / ref count maps
#define SUPERTRI                            // enable super triangle processing on real fast machines
#define CELSIUS_TEX_MATRIX_HACK             // enable workaround for broken texture matrix in nv10
#define ALPHA_CULL 3
//#define ALTERNATE_STENCIL_MODE
//#define SPOOF_KELVIN                        // pretend HW has kelvin support even tho it doesn't
// Uncomment the following line for a ~10-15% gain on DX7 on Win2K
//#define WIN2K_DX7OPT                        // enable to copy sysmem VB's to AGP for Windows 2000
#define KELVIN_CLEAR                        // clear via the kelvin class rather than through 2d classes
#define KELVIN_ILC                          // build kelvin inner loops
#define KELVIN_INLINE_REUSE                 // enable inline primitive reuse for Kelvin innerloops
//#define PROFILE_INDEX
//#define SW_WC
//#define FORCE_CT 1                          // 0 = force CT off, 1 = force CT on
//#define DISABLE_Z_COMPR                     // Disable Z compression
#define SYSVB2AGP 1                         // 0 = disable copy of system memory VB's to AGP 1 = enable.
//#define TEXFORMAT_CRD                       // improved texture format export mechanism
#define STATE_OPTIMIZE 0
#define CHECK_DUP_MATRIX 0
//#define SPOOF_ALPHA                           //create an A8R8G8B8 surface instead of an X8R8G8B8. workaround for a ms issue.
// SPOOF_ALPHA causes problems in 3DWinBench200 or WHQL
#define THREE_STAGE_ALPHA_SUPPORT              // limited three-stage alpha support

//---------------------------------------------------------------------------
// debugging and instrumentation
//---------------------------------------------------------------------------

//#define FORCE_NV10_AS_NV5                   // make NV10 be treated like an NV5
//#define VTUNE_SUPPORT
//#define INSTRUMENT_INNER_LOOPS              // capture MB/S data for inner loops
//#define NV_DISASM                           // compile with push buffer disassembling code
//#define TEX_MANAGE_TEST                     // stress the texture manager by forcing lots of allocation failures, etc.
//#define NV_NULL_DRIVER_DYNAMIC              // allowes dynamically to null out particular drawing primitives.

// null driver flavors
//#define NV_NULL_DRIVER                      // do no work in driver
//#define NV_NULL_HW_DRIVER                   // do work in driver, copy verts (if needed) but dont render geometry (hw will not T&L or rasterize - will set state)
//#define NV_NULL_FILL_DRIVER                 // zero sized triangles
//#define NV_NULL_LIGHTS_DRIVER               // turn of HW lighting
//#define NV_NULL_TEXTURE_DRIVER              // suppress all texture ops (except alloc/free)
//#define NV_NULL_BLEND                       // disable texture and alpha blending
//#define NV_NULL_TEXTURES                    // disable all texturing operations (must also enable NV_NULL_BLEND)

// locutus
//#define NV_PROFILE_CPULOCK                  // LOCUTUS 1 -> CSimpleSurface::cpuLock profiling
//#define NV_PROFILE_PUSHER                   // LOCUTUS 1 -> push buffer profiling
//#define NV_PROFILE_CALLSTACK                // LOCUTUS 1 -> breakdown of time spend in different layers of call stack
//#define NV_PROFILE_DEFVB                    // LOCUTUS 1 -> defualt VB profile
//#define NV_PROFILE_COPIES                   // LOCUTUS 1 -> check major copy routines

// nvDP2Ops profiling
//#define NV_PROFILE_DP2OPS

// texture profiling
//#define NO_TEX_BLTS                         // Do not allow texture blits/downloads
//#define NO_TEX_SWZ                          // Allow texturing downloads, but do not allow swizzling (force linear)
//#define CNT_TEX                             // Count all texture downloads

// procedure instrumentation
//#define INSTRUMENT_ALL_CALLS                // time each and every dbgTraceXXX call (only works for !DEBUG)
#define IAC_THRESHOLD           10          // time in ms to exceed before logging

// capture file creation
//#define CAPTURE

//#define FORCE_INLINE_VERTICES               // force use of inline vertices
//#define CHECK_DEPTHS                        // check that transformed z's aren't outside [0...1].

//#define DEBUG_SURFACE_PLACEMENT

//---------------------------------------------------------------------------
// switch logic
//---------------------------------------------------------------------------

#if defined(NV_PROFILE_CPULOCK) || defined(NV_PROFILE_PUSHER) || defined(NV_PROFILE_CALLSTACK) || defined(NV_PROFILE_DEFVB) || defined(NV_PROFILE_COPIES)
#define NV_PROFILE
#endif

#if defined(STEREO_SUPPORT) && defined(DEBUG)
//#define PER_PRIMITIVE_SYNC
//#define NEW_FRONT_RENDER
#endif

#ifdef WINNT
#undef CAPTURE
#endif

#ifndef WINNT
#undef WIN2K_DX7OPT
#endif

#ifdef CHECK_DEPTHS
#define FORCE_INLINE_VERTICES
#endif

#ifndef TEXFORMAT_CRD
//#define HILO_SUPPORT_DX7          // Support HILO texture formats NVHS and NVHU via dx7
#define HILO_SUPPORT_DX8          // Support HILO texture formats NVHS and NVHU via dx8
//#define NVxn_SUPPORT_DX7
#define NVxn_SUPPORT_DX8
#define DXT_SUPPORT
#endif

