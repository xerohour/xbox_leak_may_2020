/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d8perf.h
 *  Content:    Xbox Direct3D debug library API
 *
 ****************************************************************************/

;begin_remove

!!!!!! This file is a very funky format to allow us to generate both a nice
!!!!!! external header file as well as a functional table header file that
!!!!!! eliminates the need to enter all of the information twice.  Be
!!!!!! very careful when modifying this file.

;end_remove
;begin_external
#define paster(x) x
paster(#ifndef _D3D8PERF_H_)
paster(#define _D3D8PERF_H_)

paster(#ifdef __cplusplus)
paster(extern "C" {)
paster(#endif)

/*
 * This file exposes the extra functionality implemented in the
 * performance-instrumented and debug version of the Direct3D library 
 * (d3d8i.lib and d3d8d.lib).
 */

// Enumeration of all of the non-api events that the driver counts.
typedef enum _D3DPERF_PerformanceCounters
{
;end_external
;begin_remove
#define DEFINE_PERF(x)  PERF_##x,
;end_remove
;begin_internal
#ifdef DEFINE_PERF

;end_internal
    DEFINE_PERF(VERTICES)

    DEFINE_PERF(SETTRANSFORM_WORLD)
    DEFINE_PERF(SETTRANSFORM_VIEW)
    DEFINE_PERF(SETTRANSFORM_PROJECTION)
    DEFINE_PERF(SETTRANSFORM_TEXTURE)

    DEFINE_PERF(PUSHBUFFER_SEGMENTS)
    DEFINE_PERF(PUSHBUFFER_WAITS)
    DEFINE_PERF(OBJECTLOCK_WAITS)
    DEFINE_PERF(PRESENT_WAITS)
    DEFINE_PERF(D3DDEVICE_BLOCKUNTILIDLE_WAITS)
    DEFINE_PERF(D3DDEVICE_BLOCKUNTILVERTICALBLANK_WAITS)
    DEFINE_PERF(D3DDEVICE_BLOCKONFENCE_WAITS)
    DEFINE_PERF(CPUSPINDURINGWAIT_TIME)
    DEFINE_PERF(UNSWIZZLING_TEXTURE_LOCKS)
    DEFINE_PERF(PUSHBUFFER_JUMPTOBEGINNING)
    DEFINE_PERF(RUNPUSHBUFFER_BYTES)

    DEFINE_PERF(REDUNDANT_SETRENDERSTATE)
    DEFINE_PERF(REDUNDANT_SETTEXTURESTATE)
    DEFINE_PERF(REDUNDANT_SETVERTEXSHADER)
    DEFINE_PERF(REDUNDANT_SETTRANSFORM)
    DEFINE_PERF(REDUNDANT_SETTEXTURE)
    DEFINE_PERF(REDUNDANT_SETPALETTE)
    DEFINE_PERF(REDUNDANT_SETSTREAMSOURCE)

    DEFINE_PERF(SETSTATE_POINTPARAMS)
    DEFINE_PERF(SETSTATE_COMBINERS)
    DEFINE_PERF(SETSTATE_TEXTURESTATE)
    DEFINE_PERF(SETSTATE_SPECFOGCOMBINER)
    DEFINE_PERF(SETSTATE_TEXTRANSFORM)
    DEFINE_PERF(SETSTATE_LIGHTS)
    DEFINE_PERF(SETSTATE_TRANSFORM)

;begin_internal
#endif

;end_internal
;begin_external
    D3DPERF_MAX,
    D3DPERF_FORCE_LONG  =   0xFFFFFFFF

} D3DPERF_PerformanceCounters;

// Counts of the number of times each API is called.
typedef enum _D3DPERF_APICounters
{
;end_external
;begin_remove
#define DEFINE_API(x)   API_##x,
;end_remove
;begin_internal
#ifdef DEFINE_API
;end_internal

    /* Direct3D APIs */

    DEFINE_API(DIRECT3D_CHECKDEPTHSTENCILMATCH)
    DEFINE_API(DIRECT3D_CHECKDEVICEFORMAT)
    DEFINE_API(DIRECT3D_CHECKDEVICEMULTISAMPLETYPE)
    DEFINE_API(DIRECT3D_CHECKDEVICETYPE)
    DEFINE_API(DIRECT3D_CREATEDEVICE)
    DEFINE_API(DIRECT3D_ENUMADAPTERMODES)
    DEFINE_API(DIRECT3D_GETADAPTERDISPLAYMODE)
    DEFINE_API(DIRECT3D_GETADAPTERIDENTIFIER)
    DEFINE_API(DIRECT3D_GETADAPTERMODECOUNT)
    DEFINE_API(DIRECT3D_GETDEVICECAPS)
    DEFINE_API(DIRECT3D_SETPUSHBUFFERSIZE)

    /* D3DDevice APIs */

    DEFINE_API(D3DDEVICE_APPLYSTATEBLOCK)
    DEFINE_API(D3DDEVICE_BEGIN)
    DEFINE_API(D3DDEVICE_BEGINPUSH)
    DEFINE_API(D3DDEVICE_BEGINPUSHBUFFER)
    DEFINE_API(D3DDEVICE_BEGINSTATEBLOCK)
    DEFINE_API(D3DDEVICE_BEGINVISIBILITYTEST)
    DEFINE_API(D3DDEVICE_BLOCKONFENCE)
    DEFINE_API(D3DDEVICE_BLOCKUNTILIDLE)
    DEFINE_API(D3DDEVICE_BLOCKUNTILVERTICALBLANK)
    DEFINE_API(D3DDEVICE_CAPTURESTATEBLOCK)
    DEFINE_API(D3DDEVICE_CLEAR)
    DEFINE_API(D3DDEVICE_COPYRECTS)
    DEFINE_API(D3DDEVICE_CREATECUBETEXTURE)
    DEFINE_API(D3DDEVICE_CREATEDEPTHSTENCILSURFACE)
    DEFINE_API(D3DDEVICE_CREATEFIXUP)
    DEFINE_API(D3DDEVICE_CREATEIMAGESURFACE)
    DEFINE_API(D3DDEVICE_CREATEINDEXBUFFER)
    DEFINE_API(D3DDEVICE_CREATEPALETTE)
    DEFINE_API(D3DDEVICE_CREATEPIXELSHADER)
    DEFINE_API(D3DDEVICE_CREATEPUSHBUFFER)
    DEFINE_API(D3DDEVICE_CREATERENDERTARGET)
    DEFINE_API(D3DDEVICE_CREATESTATEBLOCK)
    DEFINE_API(D3DDEVICE_CREATETEXTURE)
    DEFINE_API(D3DDEVICE_CREATEVERTEXBUFFER)
    DEFINE_API(D3DDEVICE_CREATEVERTEXSHADER)
    DEFINE_API(D3DDEVICE_CREATEVOLUMETEXTURE)
    DEFINE_API(D3DDEVICE_DELETEPATCH)
    DEFINE_API(D3DDEVICE_DELETEPIXELSHADER)
    DEFINE_API(D3DDEVICE_DELETESTATEBLOCK)
    DEFINE_API(D3DDEVICE_DELETEVERTEXSHADER)
    DEFINE_API(D3DDEVICE_DRAWINDEXEDVERTICES)
    DEFINE_API(D3DDEVICE_DRAWINDEXEDVERTICESUP)
    DEFINE_API(D3DDEVICE_DRAWRECTPATCH)
    DEFINE_API(D3DDEVICE_DRAWTRIPATCH)
    DEFINE_API(D3DDEVICE_DRAWVERTICES)
    DEFINE_API(D3DDEVICE_DRAWVERTICESUP)
    DEFINE_API(D3DDEVICE_ENABLECC)
    DEFINE_API(D3DDEVICE_ENABLEOVERLAY)
    DEFINE_API(D3DDEVICE_END)
    DEFINE_API(D3DDEVICE_ENDPUSH)
    DEFINE_API(D3DDEVICE_ENDPUSHBUFFER)
    DEFINE_API(D3DDEVICE_ENDSTATEBLOCK)
    DEFINE_API(D3DDEVICE_ENDVISIBILITYTEST)
    DEFINE_API(D3DDEVICE_FLUSHVERTEXCACHE)
    DEFINE_API(D3DDEVICE_GETBACKBUFFER)
    DEFINE_API(D3DDEVICE_GETBACKBUFFERSCALE)
    DEFINE_API(D3DDEVICE_GETBACKMATERIAL)
    DEFINE_API(D3DDEVICE_GETCCSTATUS)
    DEFINE_API(D3DDEVICE_GETCREATIONPARAMETERS)
    DEFINE_API(D3DDEVICE_GETDEPTHSTENCILSURFACE)
    DEFINE_API(D3DDEVICE_GETDEVICECAPS)
    DEFINE_API(D3DDEVICE_GETDIRECT3D)
    DEFINE_API(D3DDEVICE_GETDISPLAYFIELDSTATUS)
    DEFINE_API(D3DDEVICE_GETDISPLAYMODE)
    DEFINE_API(D3DDEVICE_GETGAMMARAMP)
    DEFINE_API(D3DDEVICE_GETINDICES)
    DEFINE_API(D3DDEVICE_GETLIGHT)
    DEFINE_API(D3DDEVICE_GETLIGHTENABLE)
    DEFINE_API(D3DDEVICE_GETMATERIAL)
    DEFINE_API(D3DDEVICE_GETMODELVIEW)
    DEFINE_API(D3DDEVICE_GETOVERLAYUPDATESTATUS)
    DEFINE_API(D3DDEVICE_GETPALETTE)
    DEFINE_API(D3DDEVICE_GETPIXELSHADER)
    DEFINE_API(D3DDEVICE_GETPIXELSHADERCONSTANT)
    DEFINE_API(D3DDEVICE_GETPIXELSHADERFUNCTION)
    DEFINE_API(D3DDEVICE_GETPROJECTIONVIEWPORTMATRIX)
    DEFINE_API(D3DDEVICE_GETPUSHBUFFEROFFSET)
    DEFINE_API(D3DDEVICE_GETRASTERSTATUS)
    DEFINE_API(D3DDEVICE_GETRENDERSTATE)
    DEFINE_API(D3DDEVICE_GETRENDERTARGET)
    DEFINE_API(D3DDEVICE_GETSCISSORS)
    DEFINE_API(D3DDEVICE_GETSCREENSPACEOFFSET)
    DEFINE_API(D3DDEVICE_GETSHADERCONSTANTMODE)
    DEFINE_API(D3DDEVICE_GETSTREAMSOURCE)
    DEFINE_API(D3DDEVICE_GETTEXTURE)
    DEFINE_API(D3DDEVICE_GETTEXTURESTAGESTATE)
    DEFINE_API(D3DDEVICE_GETTILE)
    DEFINE_API(D3DDEVICE_GETTILECOMPRESSIONTAGS)
    DEFINE_API(D3DDEVICE_GETTRANSFORM)
    DEFINE_API(D3DDEVICE_GETVERTEXBLENDMODELVIEW)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADER)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADERCONSTANT)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADERDECLARATION)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADERFUNCTION)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADERINPUT)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADERSIZE)
    DEFINE_API(D3DDEVICE_GETVERTEXSHADERTYPE)
    DEFINE_API(D3DDEVICE_GETVIEWPORT)
    DEFINE_API(D3DDEVICE_GETVISIBILITYTESTRESULT)
    DEFINE_API(D3DDEVICE_INSERTCALLBACK)
    DEFINE_API(D3DDEVICE_INSERTFENCE)
    DEFINE_API(D3DDEVICE_ISBUSY)
    DEFINE_API(D3DDEVICE_ISFENCEPENDING)
    DEFINE_API(D3DDEVICE_KICKPUSHBUFFER)
    DEFINE_API(D3DDEVICE_LIGHTENABLE)
    DEFINE_API(D3DDEVICE_LOADVERTEXSHADER)
    DEFINE_API(D3DDEVICE_LOADVERTEXSHADERPROGRAM)
    DEFINE_API(D3DDEVICE_MULTIPLYTRANSFORM)
    DEFINE_API(D3DDEVICE_NOP)
    DEFINE_API(D3DDEVICE_PERSISTDISPLAY)
    DEFINE_API(D3DDEVICE_PRESENT)
    DEFINE_API(D3DDEVICE_PRIMEVERTEXCACHE)
    DEFINE_API(D3DDEVICE_READVERTEXSHADERCONSTANT)
    DEFINE_API(D3DDEVICE_RESET)
    DEFINE_API(D3DDEVICE_RESUME)
    DEFINE_API(D3DDEVICE_RUNPUSHBUFFER)
    DEFINE_API(D3DDEVICE_RUNVERTEXSTATESHADER)
    DEFINE_API(D3DDEVICE_SELECTVERTEXSHADER)
    DEFINE_API(D3DDEVICE_SENDCC)
    DEFINE_API(D3DDEVICE_SETBACKBUFFERSCALE)
    DEFINE_API(D3DDEVICE_SETBACKMATERIAL)
    DEFINE_API(D3DDEVICE_SETGAMMARAMP)
    DEFINE_API(D3DDEVICE_SETINDICES)
    DEFINE_API(D3DDEVICE_SETLIGHT)
    DEFINE_API(D3DDEVICE_SETMATERIAL)
    DEFINE_API(D3DDEVICE_SETMODELVIEW)
    DEFINE_API(D3DDEVICE_SETPALETTE)
    DEFINE_API(D3DDEVICE_SETPIXELSHADER)
    DEFINE_API(D3DDEVICE_SETPIXELSHADERCONSTANT)
    DEFINE_API(D3DDEVICE_SETPIXELSHADERPROGRAM)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_BACKFILLMODE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_CULLMODE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_DONOTCULLUNCOMPRESSED)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_DXT1NOISEENABLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_EDGEANTIALIAS)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_FILLMODE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_FOGCOLOR)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_FRONTFACE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_LINEWIDTH)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_LOGICOP)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_MULTISAMPLEANTIALIAS)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_MULTISAMPLEMASK)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_MULTISAMPLEMODE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_MULTISAMPLERENDERTARGETMODE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_NORMALIZENORMALS)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_OCCLUSIONCULLENABLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_PARAMETERCHECK)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_PSTEXTUREMODES)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_ROPZCMPALWAYSREAD)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_ROPZREAD)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_SHADOWFUNC)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_SIMPLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_STENCILCULLENABLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_STENCILENABLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_STENCILFAIL)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_TEXTUREFACTOR)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_TWOSIDEDLIGHTING)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_VERTEXBLEND)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_YUVENABLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_ZBIAS)
    DEFINE_API(D3DDEVICE_SETRENDERSTATE_ZENABLE)
    DEFINE_API(D3DDEVICE_SETRENDERSTATENOTINLINE)
    DEFINE_API(D3DDEVICE_SETRENDERTARGET)
    DEFINE_API(D3DDEVICE_SETSCISSORS)
    DEFINE_API(D3DDEVICE_SETSCREENSPACEOFFSET)
    DEFINE_API(D3DDEVICE_SETSHADERCONSTANTMODE)
    DEFINE_API(D3DDEVICE_SETSTREAMSOURCE)
    DEFINE_API(D3DDEVICE_SETSWAPCALLBACK)
    DEFINE_API(D3DDEVICE_SETTEXTURE)
    DEFINE_API(D3DDEVICE_SETTEXTURESTAGESTATENOTINLINE)
    DEFINE_API(D3DDEVICE_SETTEXTURESTATE_BORDERCOLOR)
    DEFINE_API(D3DDEVICE_SETTEXTURESTATE_BUMPENV)
    DEFINE_API(D3DDEVICE_SETTEXTURESTATE_COLORKEYCOLOR)
    DEFINE_API(D3DDEVICE_SETTEXTURESTATE_PARAMETERCHECK)
    DEFINE_API(D3DDEVICE_SETTEXTURESTATE_TEXCOORDINDEX)
    DEFINE_API(D3DDEVICE_SETTILE)
    DEFINE_API(D3DDEVICE_SETTRANSFORM)
    DEFINE_API(D3DDEVICE_SETVERTEXBLENDMODELVIEW)
    DEFINE_API(D3DDEVICE_SETVERTEXDATA2F)
    DEFINE_API(D3DDEVICE_SETVERTEXDATA2S)
    DEFINE_API(D3DDEVICE_SETVERTEXDATA4F)
    DEFINE_API(D3DDEVICE_SETVERTEXDATA4S)
    DEFINE_API(D3DDEVICE_SETVERTEXDATA4UB)
    DEFINE_API(D3DDEVICE_SETVERTEXDATACOLOR)
    DEFINE_API(D3DDEVICE_SETVERTEXSHADER)
    DEFINE_API(D3DDEVICE_SETVERTEXSHADERCONSTANT)
    DEFINE_API(D3DDEVICE_SETVERTEXSHADERINPUT)
    DEFINE_API(D3DDEVICE_SETVERTICALBLANKCALLBACK)
    DEFINE_API(D3DDEVICE_SETVIEWPORT)
    DEFINE_API(D3DDEVICE_SUSPEND)
    DEFINE_API(D3DDEVICE_SWITCHTEXTURE)
    DEFINE_API(D3DDEVICE_UPDATEOVERLAY)

    /* D3DResource APIs */

    DEFINE_API(D3DRESOURCE_ADDREF)
    DEFINE_API(D3DRESOURCE_BLOCKUNTILNOTBUSY)
    DEFINE_API(D3DRESOURCE_FREEPRIVATEDATA)
    DEFINE_API(D3DRESOURCE_GETDEVICE)
    DEFINE_API(D3DRESOURCE_GETPRIVATEDATA)
    DEFINE_API(D3DRESOURCE_GETTYPE)
    DEFINE_API(D3DRESOURCE_ISBUSY)
    DEFINE_API(D3DRESOURCE_REGISTER)
    DEFINE_API(D3DRESOURCE_RELEASE)
    DEFINE_API(D3DRESOURCE_SETPRIVATEDATA)

    /* D3DBaseTexture APIs */

    DEFINE_API(D3DBASETEXTURE_GETLEVELCOUNT)

    /* D3DTexture APIs */

    DEFINE_API(D3DTEXTURE_GETLEVELDESC)
    DEFINE_API(D3DTEXTURE_GETSURFACELEVEL)
    DEFINE_API(D3DTEXTURE_LOCKRECT)

    /* D3DCubeTexture APIs */

    DEFINE_API(D3DCUBETEXTURE_GETCUBEMAPSURFACE)
    DEFINE_API(D3DCUBETEXTURE_GETLEVELDESC)
    DEFINE_API(D3DCUBETEXTURE_LOCKRECT)

    /* D3DVolumeTexture APIs */

    DEFINE_API(D3DVOLUMETEXURE_GETLEVELDESC)
    DEFINE_API(D3DVOLUMETEXURE_GETVOLUMELEVEL)
    DEFINE_API(D3DVOLUMETEXURE_LOCKBOX)

    /* D3DVertexBuffer APIs */

    DEFINE_API(D3DVERTEXBUFFER_GETDESC)
    DEFINE_API(D3DVERTEXBUFFER_LOCK)

    /* D3DIndexBuffer APIs */

    DEFINE_API(D3DINDEXBUFFER_GETDESC)

    /* D3DVolume APIs */

    DEFINE_API(D3DVOLUME_GETCONTAINER)
    DEFINE_API(D3DVOLUME_GETDESC)
    DEFINE_API(D3DVOLUME_LOCKBOX)

    /* D3DSurface APIs */

    DEFINE_API(D3DSURFACE_GETCONTAINER)
    DEFINE_API(D3DSURFACE_GETDESC)
    DEFINE_API(D3DSURFACE_LOCKRECT)

    /* D3DPalette APIs */

    DEFINE_API(D3DPALETTE_GETSIZE)
    DEFINE_API(D3DPALETTE_LOCK)

    /* D3DPushBuffer APIs */

    DEFINE_API(D3DPUSHBUFFER_BEGINFIXUP)
    DEFINE_API(D3DPUSHBUFFER_ENDFIXUP)
    DEFINE_API(D3DPUSHBUFFER_ENDVISIBILITYTEST)
    DEFINE_API(D3DPUSHBUFFER_JUMP)
    DEFINE_API(D3DPUSHBUFFER_RUNPUSHBUFFER)
    DEFINE_API(D3DPUSHBUFFER_SETMODELVIEW)
    DEFINE_API(D3DPUSHBUFFER_SETPALETTE)
    DEFINE_API(D3DPUSHBUFFER_SETRENDERTARGET)
    DEFINE_API(D3DPUSHBUFFER_SETTEXTURE)
    DEFINE_API(D3DPUSHBUFFER_SETVERTEXBLENDMODELVIEW)
    DEFINE_API(D3DPUSHBUFFER_SETVERTEXSHADERCONSTANT)
    DEFINE_API(D3DPUSHBUFFER_SETVERTEXSHADERINPUT)
    DEFINE_API(D3DPUSHBUFFER_VERIFY)

    /* D3DFixup APIs */

    DEFINE_API(D3DFIXUP_GETSIZE)
    DEFINE_API(D3DFIXUP_GETSPACE)
    DEFINE_API(D3DFIXUP_RESET)

;begin_internal
#endif

;end_internal
;begin_external
    D3DAPI_MAX,
    D3DAPI_FORCE_LONG  =   0xFFFFFFFF

} D3DPERF_APICounters;

// A structure that contains performance information gathered by
// the driver.
//
typedef struct _D3DWAITINFO
{
    DWORD Count;
    ULONGLONG Cycles;

} D3DWAITINFO;

// function callback declaration for cycle time threshold callback
//
typedef void (WINAPI *PFNCycleThresholdHandler)(D3DPERF_PerformanceCounters perfctr,
    ULONGLONG Cycles);

// function callback declaration for spewing framerate information
//
typedef void (WINAPI *PFNDumpFrameRateInfoHandler)(void);

// m_dwDumpFPSInfoMask flags which specify what data to display during default
// m_pfnDumpFrameRateInfoHandler callback. By default everything is enabled
// except D3DPERF_DUMP_FPS_PERFPROFILE which has a much higher overhead.
//
// To use set m_FrameRateIntervalTime to 10000 (10ms) or similar.
//
paster(#define D3DPERF_DUMP_FPS_INTINFO)        0x00000001  // interrupts & context switches
paster(#define D3DPERF_DUMP_FPS_MEMORY)         0x00000002  // memory info
paster(#define D3DPERF_DUMP_FPS_PUSHBUFFER)     0x00000004  // pushbuffer info
paster(#define D3DPERF_DUMP_FPS_WAITINFO)       0x00000008  // *_WAITS timings
paster(#define D3DPERF_DUMP_FPS_PERFPROFILE)    0x00000010  // 100ns timer which pings GPU to
                                                    // determine %busy/idle time.
paster(#define D3DPERF_DUMP_FPS_VERTEXINFO)     0x00000020  // vertex processing info
paster(#define D3DPERF_DUMP_FPS_PERFEVENTS)     0x00000040  // performance events info

// Structure which allows SetRenderState calls to be overriden. For example,
// set m_RenderStateOverrides[D3DRS_FILLMODE] = { TRUE, D3DFILL_WIREFRAME }
// to turn on wireframe mode.
//
typedef struct _D3DOVERRIDERENDERSTATE
{
    BOOL Override;
    DWORD Value;
} D3DOVERRIDERENDERSTATE;

// D3D profile busy/idle samples
//
typedef enum _D3DPERFPROF
{
    D3DPERF_PROF_GPU,
    D3DPERF_PROF_FE,
    D3DPERF_PROF_BE,
    D3DPERF_PROF_MAX
} D3DPERFPROF;

// D3D performance event types.
//
typedef enum _D3DPERFEventsType
{
    D3DPERFEvent_Header                 = 0,
    D3DPERFEvent_DrawVerticesUP         = 1,
    D3DPERFEvent_DrawIndexedVerticesUP  = 2,
    D3DPERFEvent_DrawVertices           = 3,
    D3DPERFEvent_DrawIndexedVertices    = 4,
    D3DPERFEvent_BeginEnd               = 5,
    D3DPERFEvent_RunPushBuffer          = 6,

    D3DPERFEvent_VBlank                 = 7,
    D3DPERFEvent_Kickoff                = 8,
    D3DPERFEvent_Present                = 9,

    D3DPERFEvent_BlockUntilIdle         = 10,
    D3DPERFEvent_BlockOnFence           = 11,
    D3DPERFEvent_PushBufferWait         = 12,
    D3DPERFEvent_ObjectLockWait         = 13,

    D3DPERFEvent_User0                  = 21,
    D3DPERFEvent_User1                  = 22,
    D3DPERFEvent_User2                  = 23,
    D3DPERFEvent_User3                  = 24,
    D3DPERFEvent_User4                  = 25,
    D3DPERFEvent_User5                  = 26,
    D3DPERFEvent_User6                  = 27,
    D3DPERFEvent_User7                  = 28,
    D3DPERFEvent_User8                  = 29,
    D3DPERFEvent_User9                  = 30,
    D3DPERFEvent_User10                 = 31,

    D3DPERFEvent_Max                    = 32,
} D3DPERFEventsType;

// D3D performance event structure.
// 
typedef struct _D3DPERFEvent
{
    WORD Type;                          // Event type. D3DPERFEventsType or
                                        // user defined event.
    union
    {
        char HeaderText[30];            // D3DPERFEvent_Header

        struct
        {
            WORD GpuPct;                // Pct fe/be gpu was busy since last sample.
                                        // 0xffff means there weren't enough samples.
            DWORD CpuPutPtr;            // cpu put ptr location at CycleCpuStart

            ULONGLONG CycleCpuStart;    // rdtsc for start of timing
            DWORD CycleCpuLength;       // cpu cycle length of timing block

            DWORD CycleGpuStartDelta;   // gpu start cycle delta from CycleCpuStart
            DWORD CycleGpuLength;       // total gpu processing time

            DWORD Data;                 // Event Data. Space event took in pushbuffer
                                        // or for RunPushBuffer size of pushbuffer.
        };
    };
} D3DPERFEvent;

// Structure that contains all of the performance information gathered by
// the driver.
//
typedef struct _D3DPERF
{
    // Time Stamp Counter value of last D3DPERF_Reset() call.
    ULONGLONG m_TSCLastResetVal;

    // Time Stamp Counter value of last Present() call
    ULONGLONG m_TSCLastPresentVal;

    // Max and Min Frame number (.Count) with cycle counts (.Cycles)
    D3DWAITINFO m_FrameTimeMin;
    D3DWAITINFO m_FrameTimeMax;

    // Issues debug break when perfctr and break number line up.
    DWORD m_BreakCount;
    D3DPERF_PerformanceCounters m_Breakperfctr;

    // Pushbuffer location at D3DPERF_Reset call
    DWORD *m_pPutLocationAtReset;

    // Count of dwords skipped jumping back to pushbuffer start
    ULONGLONG m_PushBufferEndDwordsSkipped;

    // Interval time to dump framerate information in milliseconds
    DWORD m_FrameRateIntervalTime;
    // Mask which controls what information D3DPERF_DumpFrameRateInfo
    // (default m_pfnDumpFrameRateInfoHandler handler) spits out.
    DWORD m_dwDumpFPSInfoMask;
    PFNDumpFrameRateInfoHandler m_pfnDumpFrameRateInfoHandler;

    // Holds threshold cycle count for spewing wait times
    DWORD m_DumpWaitCycleTimesThreshold;
    // Handler function to call when threshold cycle count hit. This
    // function defaults to D3DPERF_DumpCounterCycleInfo which simply
    // dumps PERF_OBJECTLOCK_WAITS count and wait times.
    PFNCycleThresholdHandler m_pfnCycleThresholdHandler;

    // If true, the next D3DPERF_Reset call will go through
    // m_RenderStateOverrides and set any renderstates which have
    // the Override member set to true.
    BOOL m_SetRenderStateOverridesOnReset;

    // Profile data gathered when profile is started via D3DPERF_StartPerfProfile
    //  (also via m_dwDumpFPSInfoMask/m_DumpWaitCycleTimesThreshold)

    // Total number of profile samples read
    DWORD m_ProfileSamples;

    // Time profiler was started. Initialized in D3DPERF_StartPerfProfile.
    ULONGLONG m_TSCProfileStartTime;

    // Perf state bitmask history over last 10ms where it then wraps.
    // 0x2 means gpu (1 << D3DPERF_PROF_GPU) and (1 << D3DPERF_PROF_FE) were busy.
    // Cleared in D3DPERF_StartPerfProfile and D3DPERF_Reset calls.
    WORD m_ProfileData[105000];

    // Total busy counts for each unit read while profiler was enabled
    DWORD m_ProfileBusyCounts[D3DPERF_PROF_MAX];


    // Performance monitoring members.

    // Mask of performance events to record.
    // D3DPERF_PerfEventStart(WORD Type, ...) does the following:
    //   if((1 << Type) && !(g_PerfCounters.RecordD3DPerfEvents & (1 << Type)))
    //       return NULL;
    DWORD RecordD3DPerfEvents;

    // if SpewFrameCount != 0, D3DPERF_DumpPerfEvents will spew
    // Count frame(s) starting at present #D3DPerfEventSpewFrameStart
    // in pD3DPerfEvents.
    DWORD D3DPerfEventSpewFrameStart;
    DWORD D3DPerfEventSpewFrameCount;

    // Holds threshold cycle count for spewing perf times
    DWORD m_DumpPerfCycleTimesThresholdCpu;
    DWORD m_DumpPerfCycleTimesThresholdGpu;

    // The current open event in pD3DPerfEvents.
    DWORD CurrentD3DPerfEvent;
    // Count of elements in pD3DPerfEvents
    DWORD CountD3DPerfEvents;
    // Pointer to array of pD3DPerfEvents
    D3DPERFEvent *pD3DPerfEvents;


    // Values used to override any calls to SetRenderState.
    //
    D3DOVERRIDERENDERSTATE m_RenderStateOverrides[D3DRS_MAX];

    // The count of the number of times certain performance-related events
    // happen.  Use D3DPERF_PerformanceCounters to index into this array.
    //
    D3DWAITINFO m_PerformanceCounters[D3DPERF_MAX];

    // The count of the number of times each API is called.  Use
    // D3DPERF_APICounters to index into this array.
    //
    DWORD m_APICounters[D3DAPI_MAX];

    // The count of the number of times each render state is set.  Use
    // D3DRENDERSTATE to index into this array.
    //
    DWORD m_RenderStateCounters[D3DRS_MAX];

    // The count of the number of times each texture state is set.
    DWORD m_TextureStateCounters[D3DTSS_MAX];

} D3DPERF;

// Holds the name of an individual renderstate or texturestate.  You need
// to search through the array of these things to find the name for the
// state.
//
typedef struct _D3DSTATENAME
{
    DWORD m_State;
    char *m_Name;

} D3DSTATENAME;

// Holds the names for the different performance things gathered
// by the driver.
//
typedef struct _D3DPERFNAMES
{
    // Array of string of human-readable names for the above counters.
    char *m_PerformanceCounterNames[D3DPERF_MAX];

    // An array of human-readable names for the above counters.
    char *m_APICounterNames[D3DAPI_MAX];

    // Human-readable names for the renderstate counters.
    D3DSTATENAME m_RenderStateNames[D3DRS_MAX];

    // Human-readable names for the texture states.
    D3DSTATENAME m_TextureStateNames[D3DTSS_MAX];

} D3DPERFNAMES;

// Holds information on the pushbuffer and guesstimate on count of bytes
// written since last D3DPERF_Reset.
//
typedef struct _D3DPUSHBUFFERINFO
{
    DWORD PushBufferSize;
    DWORD PushSegmentSize;
    DWORD PushSegmentCount;

    DWORD *pPushBase;
    DWORD *pPushLimit;

    ULONGLONG PushBufferBytesWritten;

} D3DPUSHBUFFERINFO;


// Returns the performance structure which is statically allocated and should
// never be freed.
//
D3DPERF * WINAPI D3DPERF_GetStatistics();

// Returns an array of names which is statically allocated.
//
D3DPERFNAMES * WINAPI D3DPERF_GetNames();

// Returns information on the pushbuffer
//
void WINAPI D3DPERF_GetPushBufferInfo(D3DPUSHBUFFERINFO *pPushBufferInfo);

// Dumps current frame rate information.
//
void WINAPI D3DPERF_DumpFrameRateInfo();

// Reset all of the perf counters to zero.
//
void WINAPI D3DPERF_Reset();

// Trivial dumper of all statistics to the debug output.
//
void WINAPI D3DPERF_Dump();


// D3D Perf states
//
typedef enum _D3DPERFSTATETYPE
{
    D3DPERFSTATE_VTX_CACHE,     // 4KB pre-t&l cache
    D3DPERFSTATE_VTX_FILE,      // 24 entry post t&l cache
    D3DPERFSTATE_VTX_ALL,       // enable/disable all vtx states
} D3DPERFSTATETYPE;

// Set state routine which allows you to enable/disable
// the vertex cache and/or file. This can be useful to determine
// how well you're currently, or not, using vertex caching.
//
HRESULT WINAPI D3DPERF_SetState(D3DPERFSTATETYPE State, DWORD Value);

// GPU Profile control routines. Enabling the PerfProfiler starts
// profiler interrupt run every 100ns which checks the current busy/idle
// status of the gpu.
//
BOOL WINAPI D3DPERF_StartPerfProfile();
void WINAPI D3DPERF_StopPerfProfile();
void WINAPI D3DPERF_DumpPerfProfCounts();

// Routine to spew current list of performance monitoring events.
// Setting D3DPERF_DUMP_FPS_PERFEVENTS will cause D3DPERF_DumpFrameRateInfo to
// dump this information also. Looks something like:
//
//   Event                  CpuStart   CpuTime  GpuStart  GpuTime  CpuPut  Data Gpu%
//   Kickoff                       0      1683         0        0  113320      0
//      *** FrameMove ***
//   user0                    12705    106084   1900673   109887  113376   6176
//    DrawIndexedVertices     108691      6869   1947551    54773  115132   3924  90
//    DrawIndexedVertices     123267    278445   2216913  1471443  120420  86052
//
// FrameMove: is a user inserted header via D3DPERF_InsertPerfEventHeader
// user0: user event inserted via
//      D3DPERFEvent *pD3DPerfEvent = D3DPERF_PerfEventStart(D3DPERFEvent_User0, TRUE);
//      ...
//      D3DPERF_PerfEventEnd(pD3DPerfEvent, TRUE);
// CpuStart: cycle time of when event was start
// CpuTime: count of cycles cpu took for event
// GpuStart: cycle time of when gpu started working on event
// GpuTime: count of cycles gpu took for event
// CpuPut: where the CpuPut pushbuffer pointer was at start of event
// Gpu%: Gpu busy percentage since last event with more than 15 samples
//
void WINAPI D3DPERF_DumpPerfEvents();

/*
 * Helper routines to set up various d3dperf members to dump
 *  framerate information.
 */

// Sets a breakpoint on a specified performance counter number
//
__inline void WINAPI D3DPERF_SetBreakPerfCount(D3DPERF_PerformanceCounters perfctr,
    DWORD BreakCount)
{
    D3DPERF *pPerf = D3DPERF_GetStatistics();
    pPerf->m_Breakperfctr = perfctr;
    pPerf->m_BreakCount = BreakCount;
}

// Sets frame rate interval (in ms) to call D3DPERF_DumpFrameRateInfo()
//
__inline void WINAPI D3DPERF_SetShowFrameRateInterval(DWORD FrameRateIntervalTime)
{
    D3DPERF *pPerf = D3DPERF_GetStatistics();
    pPerf->m_FrameRateIntervalTime = FrameRateIntervalTime;
}

// Default handler for DumpWaitCycleTimesThreshold which just spews
// PERF_OBJECTLOCK_WAITS count and time
void WINAPI D3DPERF_DumpCounterCycleInfo(D3DPERF_PerformanceCounters perfctr,
    ULONGLONG Cycles);

// Sets threshold Cycle count and handler function for hitting wait time thresholds
__inline void WINAPI D3DPERF_SetWaitCycleTimeThreshold(
    PFNCycleThresholdHandler pfnCycleThresholdHandler,
    DWORD DumpWaitCycleTimesThreshold)
{
    D3DPERF *pPerf = D3DPERF_GetStatistics();
    pPerf->m_DumpWaitCycleTimesThreshold = DumpWaitCycleTimesThreshold;
    pPerf->m_pfnCycleThresholdHandler = pfnCycleThresholdHandler;
}

// D3DPERF_StartCountingPerfEvent
//
// This should only be done when the chip is idle as
// any outstanding requests to the gpu will try to write
// back to pPerf->pD3DPerfEvents.
//
__inline BOOL WINAPI D3DPERF_StartCountingPerfEvent(
    DWORD EventCount
    )
{
    D3DPERF *pPerf = D3DPERF_GetStatistics();

    // if we've got a count of events and it's not the same as last time
    if(EventCount && pPerf->CountD3DPerfEvents != EventCount)
    {
        // free old block, alloc new block
        free(pPerf->pD3DPerfEvents);

        pPerf->pD3DPerfEvents = (D3DPERFEvent *)malloc(sizeof(D3DPERFEvent) * EventCount);
        if(pPerf->pD3DPerfEvents)
            memset(pPerf->pD3DPerfEvents, 0, sizeof(D3DPERFEvent) * EventCount);
        else
            EventCount = 0;
    }

    pPerf->CurrentD3DPerfEvent = 0;                 // current entry in pD3DPerfEvents
    pPerf->CountD3DPerfEvents = EventCount;         // count of items in pD3DPerfEvents

    if(!pPerf->D3DPerfEventSpewFrameCount)
    {
        pPerf->D3DPerfEventSpewFrameStart = 2;      // by default just spew one frame
        pPerf->D3DPerfEventSpewFrameCount = 1;      // starting at the second present
    }

    // record mask (-1 is all)
    if(!pPerf->RecordD3DPerfEvents)
    {
        pPerf->RecordD3DPerfEvents =
            (1 << D3DPERFEvent_Header) |
            (1 << D3DPERFEvent_Present) |
            (1 << D3DPERFEvent_BlockUntilIdle) |
            (1 << D3DPERFEvent_BlockOnFence) |
            (1 << D3DPERFEvent_PushBufferWait) |
            (1 << D3DPERFEvent_ObjectLockWait) |
            (1 << D3DPERFEvent_User0) | (1 << D3DPERFEvent_User1) |
            (1 << D3DPERFEvent_User2) | (1 << D3DPERFEvent_User3) |
            (1 << D3DPERFEvent_User4) | (1 << D3DPERFEvent_User5) |
            (1 << D3DPERFEvent_User6) | (1 << D3DPERFEvent_User7) |
            (1 << D3DPERFEvent_User8) | (1 << D3DPERFEvent_User9) |
            (1 << D3DPERFEvent_User10);
    }

    return pPerf->RecordD3DPerfEvents;
}

// Insert a performance event header
//
__inline void WINAPI D3DPERF_InsertPerfEventHeader(
    char *szHeader
    )
{
    D3DPERF *pPerf = D3DPERF_GetStatistics();

    if(pPerf->CountD3DPerfEvents)
    {
        DWORD CurrentEvent =
            (InterlockedIncrement((PLONG)&pPerf->CurrentD3DPerfEvent) - 1) %
            pPerf->CountD3DPerfEvents;
        D3DPERFEvent *pD3DPerfEvent = &pPerf->pD3DPerfEvents[CurrentEvent];

        pD3DPerfEvent->Type = D3DPERFEvent_Header;

        strncpy(pD3DPerfEvent->HeaderText, szHeader, sizeof(pD3DPerfEvent->HeaderText));
        pD3DPerfEvent->HeaderText[sizeof(pD3DPerfEvent->HeaderText) - 1] = 0;
    }
}

// D3DPERF_PerfEventStart/End.
//
//  Insert a performance Event. Type should be from D3DPERFEvent_User0 to User10.
//  RecordGpuTime states whether you want to insert fences for counting gpu times.
//
//      D3DPERFEvent *pD3DPerfEvent = D3DPERF_PerfEventStart(D3DPERFEvent_User0, TRUE);
//      ...
//      D3DPERF_PerfEventEnd(pD3DPerfEvent, TRUE);
//
D3DPERFEvent * WINAPI D3DPERF_PerfEventStart(
    WORD Type, BOOL RecordGpuTime);

void WINAPI D3DPERF_PerfEventEnd(
    D3DPERFEvent *pD3DPerfEvent, BOOL RecordGpuTime);

// D3DPERF_BlockTimer / PERFEVENT_BLOCKTIMER
// 
// Convenient struct+macro for inserting user-defined perf events.
// Will time the period spanning the life of the object.
//
// Example usage:
//
//     void RenderScene()
//     {
//         PERFEVENT_BLOCKTIMER("RenderScene", TRUE); // do record GPU time
//
//         // BLOCK OF CODE TO BE TIMED
//     }
// -or-
//     void UpdateLogic()
//     {
//         PERFEVENT_BLOCKTIMER("UpdateLogic", FALSE); // don't record GPU time
//
//         // BLOCK OF CODE TO BE TIMED
//     }
//
paster(#ifdef __cplusplus)
struct D3DPERF_BlockTimer
{
    D3DPERFEvent *m_pD3DPerfEvent;
    BOOL          m_bRecordGpuTime;

    __forceinline D3DPERF_BlockTimer(char *pszHeaderString,
                                     BOOL  bRecordGpuTime,
                                     WORD  EventType = D3DPERFEvent_User0)
    {
        if(pszHeaderString) { D3DPERF_InsertPerfEventHeader(pszHeaderString); }
        m_bRecordGpuTime = bRecordGpuTime;
        m_pD3DPerfEvent = D3DPERF_PerfEventStart(EventType, m_bRecordGpuTime);
    }

    __forceinline ~D3DPERF_BlockTimer()
    {
        D3DPERF_PerfEventEnd(m_pD3DPerfEvent, m_bRecordGpuTime);
    }
};

paster(#define PERFEVENT_BLOCKTIMER(x,y))    D3DPERF_BlockTimer _PerfEventTimer(x,y)
paster(#endif __cplusplus)

paster(#ifdef __cplusplus)
paster(})
paster(#endif)

paster(#endif _D3D8PERF_H_)
;end_external
