
/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d8perf.h
 *  Content:    Xbox Direct3D debug library API
 *
 ****************************************************************************/


#ifndef _D3D8PERF_H_
#define _D3D8PERF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file exposes the extra functionality implemented in the
 * debug version of the Direct3D library (d3d8d.lib).
 */

// Enumeration of all of the non-api events that the driver counts.
typedef enum _D3DPERF_PerformanceCounters
{
    PERF_VERTICES,

    PERF_SETTRANSFORM_WORLD,
    PERF_SETTRANSFORM_VIEW,
    PERF_SETTRANSFORM_PROJECTION,
    PERF_SETTRANSFORM_TEXTURE,

    PERF_PUSHBUFFER_SEGMENTS,
    PERF_PUSHBUFFER_WAITS,
    PERF_OBJECTLOCK_WAITS,
    PERF_PRESENT_WAITS,
    PERF_D3DDEVICE_BLOCKUNTILIDLE_WAITS,
    PERF_D3DDEVICE_BLOCKUNTILVERTICALBLANK_WAITS,
    PERF_D3DDEVICE_BLOCKONFENCE_WAITS,
    PERF_CPUSPINDURINGWAIT_TIME,
    PERF_UNSWIZZLING_TEXTURE_LOCKS,
    PERF_PUSHBUFFER_JUMPTOBEGINNING,
    PERF_RUNPUSHBUFFER_BYTES,

    PERF_REDUNDANT_SETRENDERSTATE,
    PERF_REDUNDANT_SETTEXTURESTATE,
    PERF_REDUNDANT_SETVERTEXSHADER,
    PERF_REDUNDANT_SETTRANSFORM,
    PERF_REDUNDANT_SETTEXTURE,
    PERF_REDUNDANT_SETPALETTE,
    PERF_REDUNDANT_SETSTREAMSOURCE,

    PERF_SETSTATE_POINTPARAMS,
    PERF_SETSTATE_COMBINERS,
    PERF_SETSTATE_TEXTURESTATE,
    PERF_SETSTATE_SPECFOGCOMBINER,
    PERF_SETSTATE_TEXTRANSFORM,
    PERF_SETSTATE_LIGHTS,
    PERF_SETSTATE_TRANSFORM,

    D3DPERF_MAX,
    D3DPERF_FORCE_LONG  =   0xFFFFFFFF

} D3DPERF_PerformanceCounters;

// Counts of the number of times each API is called.
typedef enum _D3DPERF_APICounters
{

    /* Direct3D APIs */

    API_DIRECT3D_CHECKDEPTHSTENCILMATCH,
    API_DIRECT3D_CHECKDEVICEFORMAT,
    API_DIRECT3D_CHECKDEVICEMULTISAMPLETYPE,
    API_DIRECT3D_CHECKDEVICETYPE,
    API_DIRECT3D_CREATEDEVICE,
    API_DIRECT3D_ENUMADAPTERMODES,
    API_DIRECT3D_GETADAPTERDISPLAYMODE,
    API_DIRECT3D_GETADAPTERIDENTIFIER,
    API_DIRECT3D_GETADAPTERMODECOUNT,
    API_DIRECT3D_GETDEVICECAPS,
    API_DIRECT3D_SETPUSHBUFFERSIZE,

    /* D3DDevice APIs */

    API_D3DDEVICE_APPLYSTATEBLOCK,
    API_D3DDEVICE_BEGIN,
    API_D3DDEVICE_BEGINPUSHBUFFER,
    API_D3DDEVICE_BEGINSTATEBLOCK,
    API_D3DDEVICE_BEGINVISIBILITYTEST,
    API_D3DDEVICE_BLOCKONFENCE,
    API_D3DDEVICE_BLOCKUNTILIDLE,
    API_D3DDEVICE_BLOCKUNTILVERTICALBLANK,
    API_D3DDEVICE_CAPTURESTATEBLOCK,
    API_D3DDEVICE_CLEAR,
    API_D3DDEVICE_COPYRECTS,
    API_D3DDEVICE_CREATECUBETEXTURE,
    API_D3DDEVICE_CREATEDEPTHSTENCILSURFACE,
    API_D3DDEVICE_CREATEFIXUP,
    API_D3DDEVICE_CREATEIMAGESURFACE,
    API_D3DDEVICE_CREATEINDEXBUFFER,
    API_D3DDEVICE_CREATEPALETTE,
    API_D3DDEVICE_CREATEPIXELSHADER,
    API_D3DDEVICE_CREATEPUSHBUFFER,
    API_D3DDEVICE_CREATERENDERTARGET,
    API_D3DDEVICE_CREATESTATEBLOCK,
    API_D3DDEVICE_CREATETEXTURE,
    API_D3DDEVICE_CREATEVERTEXBUFFER,
    API_D3DDEVICE_CREATEVERTEXSHADER,
    API_D3DDEVICE_CREATEVOLUMETEXTURE,
    API_D3DDEVICE_DELETEPATCH,
    API_D3DDEVICE_DELETEPIXELSHADER,
    API_D3DDEVICE_DELETESTATEBLOCK,
    API_D3DDEVICE_DELETEVERTEXSHADER,
    API_D3DDEVICE_DRAWINDEXEDVERTICES,
    API_D3DDEVICE_DRAWINDEXEDVERTICESUP,
    API_D3DDEVICE_DRAWRECTPATCH,
    API_D3DDEVICE_DRAWTRIPATCH,
    API_D3DDEVICE_DRAWVERTICES,
    API_D3DDEVICE_DRAWVERTICESUP,
    API_D3DDEVICE_ENABLECC,
    API_D3DDEVICE_ENABLEOVERLAY,
    API_D3DDEVICE_END,
    API_D3DDEVICE_ENDPUSHBUFFER,
    API_D3DDEVICE_ENDSTATEBLOCK,
    API_D3DDEVICE_ENDVISIBILITYTEST,
    API_D3DDEVICE_FLUSHVERTEXCACHE,
    API_D3DDEVICE_GETBACKBUFFER,
    API_D3DDEVICE_GETBACKMATERIAL,
    API_D3DDEVICE_GETCCSTATUS,
    API_D3DDEVICE_GETCREATIONPARAMETERS,
    API_D3DDEVICE_GETDEPTHSTENCILSURFACE,
    API_D3DDEVICE_GETDEVICECAPS,
    API_D3DDEVICE_GETDIRECT3D,
    API_D3DDEVICE_GETDISPLAYFIELDSTATUS,
    API_D3DDEVICE_GETDISPLAYMODE,
    API_D3DDEVICE_GETGAMMARAMP,
    API_D3DDEVICE_GETINDICES,
    API_D3DDEVICE_GETLIGHT,
    API_D3DDEVICE_GETLIGHTENABLE,
    API_D3DDEVICE_GETMATERIAL,
    API_D3DDEVICE_GETMODELVIEW,
    API_D3DDEVICE_GETOVERLAYUPDATESTATUS,
    API_D3DDEVICE_GETPALETTE,
    API_D3DDEVICE_GETPIXELSHADER,
    API_D3DDEVICE_GETPIXELSHADERCONSTANT,
    API_D3DDEVICE_GETPIXELSHADERFUNCTION,
    API_D3DDEVICE_GETPROJECTIONVIEWPORTMATRIX,
    API_D3DDEVICE_GETPUSHBUFFEROFFSET,
    API_D3DDEVICE_GETRASTERSTATUS,
    API_D3DDEVICE_GETRENDERSTATE,
    API_D3DDEVICE_GETRENDERTARGET,
    API_D3DDEVICE_GETSCISSORS,
    API_D3DDEVICE_GETSHADERCONSTANTMODE,
    API_D3DDEVICE_GETSTREAMSOURCE,
    API_D3DDEVICE_GETTEXTURE,
    API_D3DDEVICE_GETTEXTURESTAGESTATE,
    API_D3DDEVICE_GETTILE,
    API_D3DDEVICE_GETTILECOMPRESSIONTAGS,
    API_D3DDEVICE_GETTRANSFORM,
    API_D3DDEVICE_GETVERTEXBLENDMODELVIEW,
    API_D3DDEVICE_GETVERTEXSHADER,
    API_D3DDEVICE_GETVERTEXSHADERCONSTANT,
    API_D3DDEVICE_GETVERTEXSHADERDECLARATION,
    API_D3DDEVICE_GETVERTEXSHADERFUNCTION,
    API_D3DDEVICE_GETVERTEXSHADERINPUT,
    API_D3DDEVICE_GETVERTEXSHADERSIZE,
    API_D3DDEVICE_GETVERTEXSHADERTYPE,
    API_D3DDEVICE_GETVIEWPORT,
    API_D3DDEVICE_GETVISIBILITYTESTRESULT,
    API_D3DDEVICE_INSERTCALLBACK,
    API_D3DDEVICE_INSERTFENCE,
    API_D3DDEVICE_ISBUSY,
    API_D3DDEVICE_ISFENCEPENDING,
    API_D3DDEVICE_KICKPUSHBUFFER,
    API_D3DDEVICE_LIGHTENABLE,
    API_D3DDEVICE_LOADVERTEXSHADER,
    API_D3DDEVICE_LOADVERTEXSHADERPROGRAM,
    API_D3DDEVICE_MULTIPLYTRANSFORM,
    API_D3DDEVICE_NOP,
    API_D3DDEVICE_PERSISTDISPLAY,
    API_D3DDEVICE_PRESENT,
    API_D3DDEVICE_PRIMEVERTEXCACHE,
    API_D3DDEVICE_READVERTEXSHADERCONSTANT,
    API_D3DDEVICE_RESET,
    API_D3DDEVICE_RESUME,
    API_D3DDEVICE_RUNPUSHBUFFER,
    API_D3DDEVICE_RUNVERTEXSTATESHADER,
    API_D3DDEVICE_SELECTVERTEXSHADER,
    API_D3DDEVICE_SENDCC,
    API_D3DDEVICE_SETBACKMATERIAL,
    API_D3DDEVICE_SETGAMMARAMP,
    API_D3DDEVICE_SETINDICES,
    API_D3DDEVICE_SETLIGHT,
    API_D3DDEVICE_SETMATERIAL,
    API_D3DDEVICE_SETMODELVIEW,
    API_D3DDEVICE_SETPALETTE,
    API_D3DDEVICE_SETPIXELSHADER,
    API_D3DDEVICE_SETPIXELSHADERCONSTANT,
    API_D3DDEVICE_SETPIXELSHADERPROGRAM,
    API_D3DDEVICE_SETRENDERSTATE_BACKFILLMODE,
    API_D3DDEVICE_SETRENDERSTATE_CULLMODE,
    API_D3DDEVICE_SETRENDERSTATE_DEFERRED,
    API_D3DDEVICE_SETRENDERSTATE_DONOTCULLUNCOMPRESSED,
    API_D3DDEVICE_SETRENDERSTATE_DXT1NOISEENABLE,
    API_D3DDEVICE_SETRENDERSTATE_EDGEANTIALIAS,
    API_D3DDEVICE_SETRENDERSTATE_FILLMODE,
    API_D3DDEVICE_SETRENDERSTATE_FOGCOLOR,
    API_D3DDEVICE_SETRENDERSTATE_FRONTFACE,
    API_D3DDEVICE_SETRENDERSTATE_LINEWIDTH,
    API_D3DDEVICE_SETRENDERSTATE_LOGICOP,
    API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLEANTIALIAS,
    API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLEMASK,
    API_D3DDEVICE_SETRENDERSTATE_MULTISAMPLETYPE,
    API_D3DDEVICE_SETRENDERSTATE_NORMALIZENORMALS,
    API_D3DDEVICE_SETRENDERSTATE_OCCLUSIONCULLENABLE,
    API_D3DDEVICE_SETRENDERSTATE_PARAMETERCHECK,
    API_D3DDEVICE_SETRENDERSTATE_PSTEXTUREMODES,
    API_D3DDEVICE_SETRENDERSTATE_ROPZCMPALWAYSREAD,
    API_D3DDEVICE_SETRENDERSTATE_ROPZREAD,
    API_D3DDEVICE_SETRENDERSTATE_SHADOWFUNC,
    API_D3DDEVICE_SETRENDERSTATE_SIMPLE,
    API_D3DDEVICE_SETRENDERSTATE_STENCILCULLENABLE,
    API_D3DDEVICE_SETRENDERSTATE_STENCILENABLE,
    API_D3DDEVICE_SETRENDERSTATE_STENCILFAIL,
    API_D3DDEVICE_SETRENDERSTATE_TEXTUREFACTOR,
    API_D3DDEVICE_SETRENDERSTATE_TWOSIDEDLIGHTING,
    API_D3DDEVICE_SETRENDERSTATE_VERTEXBLEND,
    API_D3DDEVICE_SETRENDERSTATE_YUVENABLE,
    API_D3DDEVICE_SETRENDERSTATE_ZBIAS,
    API_D3DDEVICE_SETRENDERSTATE_ZENABLE,
    API_D3DDEVICE_SETRENDERSTATENOTINLINE,
    API_D3DDEVICE_SETRENDERTARGET,
    API_D3DDEVICE_SETSCISSORS,
    API_D3DDEVICE_SETSHADERCONSTANTMODE,
    API_D3DDEVICE_SETSTREAMSOURCE,
    API_D3DDEVICE_SETTEXTURE,
    API_D3DDEVICE_SETTEXTURESTAGESTATENOTINLINE,
    API_D3DDEVICE_SETTEXTURESTATE_BORDERCOLOR,
    API_D3DDEVICE_SETTEXTURESTATE_BUMPENV,
    API_D3DDEVICE_SETTEXTURESTATE_COLORKEYCOLOR,
    API_D3DDEVICE_SETTEXTURESTATE_DEFERRED,
    API_D3DDEVICE_SETTEXTURESTATE_PARAMETERCHECK,
    API_D3DDEVICE_SETTEXTURESTATE_TEXCOORDINDEX,
    API_D3DDEVICE_SETTILE,
    API_D3DDEVICE_SETTRANSFORM,
    API_D3DDEVICE_SETVERTEXBLENDMODELVIEW,
    API_D3DDEVICE_SETVERTEXDATA2F,
    API_D3DDEVICE_SETVERTEXDATA2S,
    API_D3DDEVICE_SETVERTEXDATA4F,
    API_D3DDEVICE_SETVERTEXDATA4S,
    API_D3DDEVICE_SETVERTEXDATA4UB,
    API_D3DDEVICE_SETVERTEXDATACOLOR,
    API_D3DDEVICE_SETVERTEXSHADER,
    API_D3DDEVICE_SETVERTEXSHADERCONSTANT,
    API_D3DDEVICE_SETVERTEXSHADERINPUT,
    API_D3DDEVICE_SETVERTICALBLANKCALLBACK,
    API_D3DDEVICE_SETVIEWPORT,
    API_D3DDEVICE_SUSPEND,
    API_D3DDEVICE_SWITCHTEXTURE,
    API_D3DDEVICE_UPDATEOVERLAY,
    API_D3DDEVICE_UPDATETEXTURE,

    /* D3DResource APIs */

    API_D3DRESOURCE_ADDREF,
    API_D3DRESOURCE_BLOCKUNTILNOTBUSY,
    API_D3DRESOURCE_FREEPRIVATEDATA,
    API_D3DRESOURCE_GETDEVICE,
    API_D3DRESOURCE_GETPRIVATEDATA,
    API_D3DRESOURCE_GETTYPE,
    API_D3DRESOURCE_ISBUSY,
    API_D3DRESOURCE_REGISTER,
    API_D3DRESOURCE_RELEASE,
    API_D3DRESOURCE_SETPRIVATEDATA,

    /* D3DBaseTexture APIs */

    API_D3DBASETEXTURE_GETLEVELCOUNT,

    /* D3DTexture APIs */

    API_D3DTEXTURE_GETLEVELDESC,
    API_D3DTEXTURE_GETSURFACELEVEL,
    API_D3DTEXTURE_LOCKRECT,

    /* D3DCubeTexture APIs */

    API_D3DCUBETEXTURE_GETCUBEMAPSURFACE,
    API_D3DCUBETEXTURE_GETLEVELDESC,
    API_D3DCUBETEXTURE_LOCKRECT,

    /* D3DVolumeTexture APIs */

    API_D3DVOLUMETEXURE_GETLEVELDESC,
    API_D3DVOLUMETEXURE_GETVOLUMELEVEL,
    API_D3DVOLUMETEXURE_LOCKBOX,

    /* D3DVertexBuffer APIs */

    API_D3DVERTEXBUFFER_GETDESC,
    API_D3DVERTEXBUFFER_LOCK,

    /* D3DIndexBuffer APIs */

    API_D3DINDEXBUFFER_GETDESC,

    /* D3DVolume APIs */

    API_D3DVOLUME_GETCONTAINER,
    API_D3DVOLUME_GETDESC,
    API_D3DVOLUME_LOCKBOX,

    /* D3DSurface APIs */

    API_D3DSURFACE_GETCONTAINER,
    API_D3DSURFACE_GETDESC,
    API_D3DSURFACE_LOCKRECT,

    /* D3DPalette APIs */

    API_D3DPALETTE_GETSIZE,
    API_D3DPALETTE_LOCK,

    /* D3DPushBuffer APIs */

    API_D3DPUSHBUFFER_BEGINFIXUP,
    API_D3DPUSHBUFFER_ENDFIXUP,
    API_D3DPUSHBUFFER_ENDVISIBILITYTEST,
    API_D3DPUSHBUFFER_JUMP,
    API_D3DPUSHBUFFER_RUNPUSHBUFFER,
    API_D3DPUSHBUFFER_SETMODELVIEW,
    API_D3DPUSHBUFFER_SETPALETTE,
    API_D3DPUSHBUFFER_SETRENDERTARGET,
    API_D3DPUSHBUFFER_SETTEXTURE,
    API_D3DPUSHBUFFER_SETVERTEXBLENDMODELVIEW,
    API_D3DPUSHBUFFER_SETVERTEXSHADERCONSTANT,
    API_D3DPUSHBUFFER_SETVERTEXSHADERINPUT,
    API_D3DPUSHBUFFER_VERIFY,

    /* D3DFixup APIs */

    API_D3DFIXUP_GETSIZE,
    API_D3DFIXUP_GETSPACE,
    API_D3DFIXUP_RESET,

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
#define D3DPERF_DUMP_FPS_INTINFO        0x00000001  // interrupts & context switches
#define D3DPERF_DUMP_FPS_MEMORY         0x00000002  // memory info
#define D3DPERF_DUMP_FPS_PUSHBUFFER     0x00000004  // pushbuffer info
#define D3DPERF_DUMP_FPS_WAITINFO       0x00000008  // *_WAITS timings
#define D3DPERF_DUMP_FPS_PERFPROFILE    0x00000010  // 100ns timer which pings GPU to
                                                    // determine %busy/idle time.
#define D3DPERF_DUMP_FPS_VERTEXINFO     0x00000020  // vertex processing info

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

#ifdef __cplusplus
}
#endif

#endif _D3D8PERF_H_
