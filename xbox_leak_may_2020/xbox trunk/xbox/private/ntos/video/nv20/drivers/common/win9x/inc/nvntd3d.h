/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1997 SGS-THOMSON Microelectronics  All Rights Reserved.
 *
 *  File:       nvd3ddrv.c
 *  Content:    master D3D include file - OS specific includes
 *
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// Type redefs excluded from dx95type.h.
//
#pragma warning(disable:4005)

#if (DIRECT3D_VERSION >= 0x0800)

#define D3DERR_TEXTURE_CREATE_FAILED MAKE_DDHRESULT(721)
#define D3DRS_EXTENTS 138

/*
 * This DDPF flag is used to indicate a DX8+ format capability entry in
 * the texture format list. It is not visible to applications.
 */
#define DDPF_D3DFORMAT                                          0x00200000l

/*
 * List of operations supported on formats in DX8+ texture list.
 * See the DX8 DDK for a complete description of these flags.
 */
#define D3DFORMAT_OP_TEXTURE                    0x00000001L
#define D3DFORMAT_OP_VOLUMETEXTURE              0x00000002L
#define D3DFORMAT_OP_CUBETEXTURE                0x00000004L
#define D3DFORMAT_OP_OFFSCREEN_RENDERTARGET     0x00000008L
#define D3DFORMAT_OP_SAME_FORMAT_RENDERTARGET   0x00000010L
#define D3DFORMAT_OP_BACKBUFFER                 0x00000020L
#define D3DFORMAT_OP_ZSTENCIL                   0x00000040L
#define D3DFORMAT_OP_ZSTENCIL_WITH_ARBITRARY_COLOR_DEPTH 0x00000080L

// D3DFORMAT_OP_D3DFMT_D16_LOCKABLE
// --------------------------------
//
// This flag is only valid on a format op list entry for D3DFMT_D16.
// This flag implies that the format is true 16 bit unsigned integer.
// This flag implies that the contents of a Z buffer of this format
// are persistent:
//  - The contents of the Z buffer will be the same from
//    the last EndScene (SceneCapture) to the next BeginScene.
//  - The contents of the Z buffer are not lost if the Z buffer
//    is attached to a different render target (until rendering takes
//    place, of course).
// This flag implies that data can be copied from one such surface
// to another (vidmem to vidmem, no stretch) via the Blt DDI.
#define D3DFORMAT_OP_D3DFMT_D16_LOCKABLE        0x00000200L

/*
 * Indicates to the driver that the "execute" buffer that is to be created is actually
 * an index buffer.
 */
#define DDSCAPS2_INDEXBUFFER                    DDSCAPS2_RESERVED3

#else  // !(DIRECT3D_VERSION >= 0x0800)

typedef D3DNTHAL_DP2POINTS *LPD3DNTHAL_DP2POINTS;
typedef D3DNTHAL_DP2RENDERSTATE *LPD3DNTHAL_DP2RENDERSTATE;
typedef D3DNTHAL_DP2LINELIST *LPD3DNTHAL_DP2LINELIST;
typedef D3DNTHAL_DP2INDEXEDLINELIST *LPD3DNTHAL_DP2INDEXEDLINELIST;
typedef D3DNTHAL_DP2LINESTRIP *LPD3DNTHAL_DP2LINESTRIP;
typedef D3DNTHAL_DP2TRIANGLELIST *LPD3DNTHAL_DP2TRIANGLELIST;
typedef D3DNTHAL_DP2INDEXEDTRIANGLELIST *LPD3DNTHAL_DP2INDEXEDTRIANGLELIST; // nV addition
typedef D3DNTHAL_DP2TRIANGLESTRIP *LPD3DNTHAL_DP2TRIANGLESTRIP;
typedef D3DNTHAL_DP2TRIANGLEFAN *LPD3DNTHAL_DP2TRIANGLEFAN;
typedef D3DNTHAL_DP2TEXTURESTAGESTATE *LPD3DNTHAL_DP2TEXTURESTAGESTATE;
typedef D3DNTHAL_D3DEXTENDEDCAPS D3DHAL_D3DEXTENDEDCAPS;
typedef LPD3DNTHAL_DP2POINTS LPD3DHAL_DP2POINTS;
typedef LPD3DNTHAL_DP2RENDERSTATE LPD3DHAL_DP2RENDERSTATE;
typedef LPD3DNTHAL_DP2STATESET LPD3DHAL_DP2STATESET;
typedef LPD3DNTHAL_DP2ZRANGE LPD3DHAL_DP2ZRANGE;
typedef LPD3DNTHAL_DP2TEXBLT LPD3DHAL_DP2TEXBLT;
typedef LPD3DNTHAL_DP2SETPRIORITY LPD3DHAL_DP2SETPRIORITY;
typedef LPD3DNTHAL_DP2LINELIST LPD3DHAL_DP2LINELIST;
typedef LPD3DNTHAL_DP2INDEXEDLINELIST LPD3DHAL_DP2INDEXEDLINELIST;
typedef LPD3DNTHAL_DP2LINESTRIP LPD3DHAL_DP2LINESTRIP;
typedef LPD3DNTHAL_DP2STARTVERTEX LPD3DHAL_DP2STARTVERTEX;
typedef LPD3DNTHAL_DP2TRIANGLELIST LPD3DHAL_DP2TRIANGLELIST;
typedef LPD3DNTHAL_DP2INDEXEDTRIANGLELIST LPD3DHAL_DP2INDEXEDTRIANGLELIST;   // nV addition
typedef LPD3DNTHAL_DP2INDEXEDTRIANGLELIST2 LPD3DHAL_DP2INDEXEDTRIANGLELIST2; // nV addition
typedef LPD3DNTHAL_DP2TRIANGLESTRIP LPD3DHAL_DP2TRIANGLESTRIP;
typedef LPD3DNTHAL_DP2TRIANGLEFAN LPD3DHAL_DP2TRIANGLEFAN;
typedef LPD3DNTHAL_DP2TEXTURESTAGESTATE LPD3DHAL_DP2TEXTURESTAGESTATE;
typedef LPD3DNTHAL_DP2VIEWPORTINFO LPD3DHAL_DP2VIEWPORTINFO;
typedef LPD3DNTHAL_DP2WINFO LPD3DHAL_DP2WINFO;
typedef LPD3DNTHAL_DP2TRIANGLEFAN_IMM LPD3DHAL_DP2TRIANGLEFAN_IMM;
typedef LPD3DNTHAL_DP2SETRENDERTARGET LPD3DHAL_DP2SETRENDERTARGET;
typedef LPD3DNTHAL_DP2CLEAR LPD3DHAL_DP2CLEAR;
typedef LPD3DNTHAL_DP2SETCLIPPLANE LPD3DHAL_DP2SETCLIPPLANE;
typedef LPD3DNTHAL_DP2SETLIGHT LPD3DHAL_DP2SETLIGHT;
typedef LPD3DNTHAL_DP2SETMATERIAL LPD3DHAL_DP2SETMATERIAL;
typedef LPD3DNTHAL_DP2SETTRANSFORM LPD3DHAL_DP2SETTRANSFORM;
typedef LPD3DNTHAL_DP2CREATELIGHT LPD3DHAL_DP2CREATELIGHT;

#endif  // !(DIRECT3D_VERSION >= 0x0800)

typedef DD_D3DBUFCALLBACKS DDHAL_DDEXEBUFCALLBACKS;
typedef PDD_D3DBUFCALLBACKS LPDDHAL_DDEXEBUFCALLBACKS;
typedef PDD_SURFACE_INT LPDDRAWI_DDRAWSURFACE_INT;
typedef DD_MISCELLANEOUSCALLBACKS DDHAL_DDMISCELLANEOUSCALLBACKS;
typedef DD_MORESURFACECAPS DDMORESURFACECAPS;
typedef DD_NONLOCALVIDMEMCAPS DDNONLOCALVIDMEMCAPS;
typedef PDD_NONLOCALVIDMEMCAPS LPDDNONLOCALVIDMEMCAPS;

/*
 * DirectX7 types not redefined in DX95TYPE.H
 */
typedef PDD_DIRECTDRAW_LOCAL  LPDDRAWI_DIRECTDRAW_LCL;
#define NvGetFlatDataSelector() (0xffffffff)
// Values for dwDataType in D3DHAL_DP2SETLIGHT
#define D3DHAL_SETLIGHT_ENABLE   0
#define D3DHAL_SETLIGHT_DISABLE  1
// If this is set, light data will be passed in after the
// D3DLIGHT7 structure
#define D3DHAL_SETLIGHT_DATA     2

#define D3DHAL2_CB32_CLEAR              0x00000002L
#define D3DHAL2_CB32_DRAWONEPRIMITIVE   0x00000004L
#define D3DHAL2_CB32_DRAWONEINDEXEDPRIMITIVE 0x00000008L
#define D3DHAL2_CB32_DRAWPRIMITIVES     0x00000010L

/*
 * NT doesn't define the DX5 Clear routine's data structure, but the DX5 driver
 * routine can still be called from the DX6 clear routine to do fast buffer
 * clears, so need to define this ourselves (taken from d3d.h)
 */
#if (DIRECT3D_VERSION < 0x0800)

#ifndef IA64
typedef struct _D3DHAL_CLEARDATA
{
    DWORD               dwhContext;     // in:  Context handle

    // dwFlags can contain D3DCLEAR_TARGET or D3DCLEAR_ZBUFFER
    DWORD               dwFlags;        // in:  surfaces to clear

    DWORD               dwFillColor;    // in:  Color value for rtarget
    DWORD               dwFillDepth;    // in:  Depth value for Z buffer

    LPD3DRECT           lpRects;        // in:  Rectangles to clear
    DWORD               dwNumRects;     // in:  Number of rectangles

    HRESULT             ddrval;         // out: Return value
} D3DHAL_CLEARDATA;
typedef D3DHAL_CLEARDATA FAR *LPD3DHAL_CLEARDATA;
#endif

/*
 * DX7 DP2 structs not yet defined in d3dnthal.h.
 */
typedef D3DNTHAL_DP2SETTEXLOD   D3DHAL_DP2SETTEXLOD;
typedef LPD3DNTHAL_DP2SETTEXLOD LPD3DHAL_DP2SETTEXLOD;

#endif  // DIRECT3D_VERSION < 0x0800

/*
 * Indicates to the driver that the "execute" buffer that is to be created is actually
 * a vertex buffer. Used by CreateVertexBuffer in D3D
 */
#define DDSCAPS2_VERTEXBUFFER                   DDSCAPS2_RESERVED1

/*
 * Indicates to the driver that the "execute" buffer that is to be created is actually
 * a command buffer. Used by internally in D3D
 */
#define DDSCAPS2_COMMANDBUFFER                  DDSCAPS2_RESERVED2

/*
 * Macros for GDI functions missing from Win2K.
 */
#define IntersectRect(pIntersect, pSrc, pDst) \
    bIntersect((PRECTL) (pSrc), (PRECTL) (pDst), (PRECTL) (pIntersect))

/*
 * Macros for DirectX heap manager functions. Under Win2k, these macros
 * call wrapper functions which massage the parameters, call the DX
 * heap manager,  and adjust the offset of the surface returned by the
 * the heap manager. This is done to minimize changes the the common code.
 */
typedef struct _GLOBALDATA GLOBALDATA;
FLATPTR NvWin2kDxAllocMem(GLOBALDATA *pDriverData, ULONG ulHeapId, ULONG ulSize);
#define DDHAL32_VidMemAlloc(pdrv, heap, x, y) \
    NvWin2kDxAllocMem(pDriverData, (heap), ((x) * (y)))

void NvWin2kDxFreeMem(GLOBALDATA *pDriverData, ULONG ulHeapId, FLATPTR fpOffset);
#define DDHAL32_VidMemFree(pdrv, heap, surface) \
    NvWin2kDxFreeMem(pDriverData, (heap), (surface))

/*
 * Actual protos for the NT display driver functions which perform the
 * Ioctl call to These represent the "OS Independent" RM entry points. For NT they are functions
 * in the display driver which package the parms into a IOCTL packet and make
 * an IOCTL call to the miniport where the RM lives.
 * For WinNT, these are defined in nvapi.c
 */
HANDLE  __cdecl NvOpen            (HANDLE);
VOID    __cdecl NvClose           (HANDLE);

// control codes
ULONG __cdecl NvAllocRoot       (HANDLE, ULONG, ULONG*);
ULONG __cdecl NvAllocDevice     (HANDLE, ULONG, ULONG, ULONG, PUCHAR);
ULONG __cdecl NvAllocContextDma (HANDLE, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG __cdecl NvAllocChannelPio (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*, ULONG);
ULONG __cdecl NvAllocChannelDma (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*);
ULONG __cdecl NvAllocMemory     (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*, ULONG*);
ULONG __cdecl NvAllocObject     (HANDLE, ULONG, ULONG, ULONG, ULONG);
ULONG __cdecl NvAlloc           (HANDLE, ULONG, ULONG, ULONG, ULONG, PVOID);
ULONG __cdecl NvFree            (HANDLE, ULONG, ULONG, ULONG);
#if !defined(WINNT) || defined(IA64)
ULONG __cdecl NvAllocEvent      (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID);
#else
ULONG __cdecl NvAllocEvent      (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);
#endif
ULONG __cdecl NvFree            (HANDLE, ULONG, ULONG, ULONG);
ULONG __cdecl NvDmaFlowControl  (HANDLE, ULONG, ULONG, ULONG, ULONG);
ULONG __cdecl NvArchHeap        (HANDLE, PVOID);
ULONG __cdecl NvConfigVersion   (HANDLE, ULONG, ULONG, ULONG*);
ULONG __cdecl NvConfigGet       (HANDLE, ULONG, ULONG, ULONG, ULONG*);
ULONG __cdecl NvConfigGetEx     (HANDLE, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG __cdecl NvConfigSet       (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG*);
ULONG __cdecl NvConfigSetEx     (HANDLE, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG __cdecl NvConfigUpdate    (HANDLE, ULONG, ULONG, ULONG);
ULONG __cdecl NvGetDmaPushInfo  (HANDLE, ULONG, ULONG, ULONG, ULONG,ULONG);

/*
 * These represent the "OS Independent" RM entry points. For NT they are functions
 * in the display driver which package the parms into a IOCTL packet and make
 * an IOCTL call to the miniport where the RM lives.
 */

#define NvRmOpen() (!NULL)

#define NvRmClose() NULL

#define NvRmAllocRoot(pClient) \
        NvAllocRoot(ppdev->hDriver, NV01_ROOT, (pClient))

#define NvRmAllocDevice(hClient, hDevice, hClass, pName) \
    NvAllocDevice(ppdev->hDriver, hClient, hDevice, NV03_DEVICE_XX, (pName))
#define NvRmAllocContextDma(hClient, hDma, hClass, flags, base, limit) \
        NvAllocContextDma(ppdev->hDriver, hClient, hDma, hClass, flags, base, limit)

#define NvRmAlloc(hCli, hChan, hObj, hCls, parms) \
        NvAlloc(ppdev->hDriver, hCli, hChan, hObj, hCls, parms)

#define NvRmFree(hClient, hParent, hObj) \
        NvFree(ppdev->hDriver, hClient, hParent, hObj)

#define NvRmAllocChannelPio(hClient, hDev, hChan, hClass, hErrorCtx, ppChan, flags) \
        NvAllocChannelPio(ppdev->hDriver, (hClient), hDev, (hChan), (hClass), (hErrorCtx), (ppChan), (flags))

#define NvRmAllocChannelDma(hClient, hDev, hChan, hClass, hErrorCtx, hDataCtx, offset, ppChan) \
        NvAllocChannelDma(ppdev->hDriver, (hClient), hDev, (hChan), (hClass), (hErrorCtx), (hDataCtx), (offset), (ppChan))

#define NvRmAllocMemory(hClient, hPar, hMem, hClass, fl, ppAddr, pLim) \
        NvAllocMemory(ppdev->hDriver, hClient, hPar, hMem, hClass, fl, ppAddr, pLim)

#define NvRmAllocObject(hCli, hChan, hObj, hCls) \
        NvAllocObject(ppdev->hDriver, hCli, hChan, hObj, hCls)

#define NvRmGetConfigEx(hCli, hDevice, index, pValue, dummy) \
        NvGetConfigEx(ppdev->hDriver, hCli, hDevice, index, (pValue))

#define NvRmConfigGet(hCli, hDevice, index, pValue) \
        NvConfigGet(ppdev->hDriver, hCli, hDevice, index, (pValue))
#define NvRmConfigGetEx(hCli, hDevice, index, pParams, paramSize) \
        NvConfigGetEx(ppdev->hDriver, hCli, hDevice, index, pParams, paramSize)

#define NvRmConfigSet(hCli, hDev, index, newValue, pOldValue) \
        NvConfigSet(ppdev->hDriver, hCli, hDev, index, newValue, (pOldValue))

#define NvRmAllocObject(hCli, hChan, hObj, hCls) \
        NvAllocObject(ppdev->hDriver, hCli, hChan, hObj, hCls)

#if !defined(WINNT) || defined(IA64)
#define NvRmAllocEvent(hCli, hPar, hObjNew, hClass, index, hEvent) \
        NvAllocEvent(ppdev->hDriver, hCli, hPar, hObjNew, hClass, index, hEvent)

#else
#define NvRmAllocEvent(hCli, hPar, hObjNew, hClass, index, hEvent) \
        NvAllocEvent(ppdev->hDriver, hCli, hPar, hObjNew, hClass, index, (ULONG)hEvent)

#endif
#define NV_SLEEP     Sleep(0)
#define NV_SLEEP1    Sleep(1)
#define NV_SLEEP2    Sleep(2)
#define NV_SLEEP3    Sleep(3)
#define NV_SLEEP4    Sleep(4)
#define NV_SLEEP5    Sleep(5)
#define NV_SLEEPFLIP Sleep(1)

#define SetPriorityClass(a, b)
#define SetThreadPriority(a, b)
#define GetCurrentThread()
#define GetSystemInfo(a)
#define GetCurrentProcessId() ((DWORD)EngGetCurrentProcessId())
#define IsBadReadPtr(nop, nop1) ((nop) == NULL)
#define RegCloseKey(nop) NULL;

/*
 Obtain the amount of available memory in megabytes.
 */
#define GlobalMemoryStatus(pms) \
    (pms)->dwTotalPhys = (DWORD)ppdev->SysMemInfo.SystemMemorySize

#define EXTERN_DDAPI    WINAPI

/*
 * Global memory allocation macros.
 */
#define HeapCreate(nop, nop1, nop2)       (!NULL)
#define HeapDestroy(heap)                 (TRUE)
#define HeapAlloc(nop, nop1, size)            EngAllocMem(FL_ZERO_MEMORY, (size), 'x_VN')
#define HeapFree(nop, nop1, ptr)              { \
    if (ptr != NULL)            \
    {                           \
    EngFreeMem((ptr));          \
    }                           \
}
#define HeapRealloc(nop, ptr, size)     {   \
                                            HeapFree(nop, nop, (ptr)); \
                                            *((PULONG)(ptr)) = HeapAlloc(nop, nop, size); \
                                        }

// HeapSize should never be used on NT. it doesn't work.
#define HeapSize(nop,nop1,nop2)         { _asm int 3 }

/*
 * User mode memory allocation functions. There are no Globalxxx fcts in NT
 * kernel mode.
 */
#define GlobalAlloc(nop, size)  \
    EngAllocUserMem((size), 'x_VN')
#define GlobalLock(ptr) (ptr)
#define GlobalUnlock(nop)
#define GlobalFree(ptr)     \
    EngFreeUserMem(ptr)
#define Sleep(time)         \
    nvDelay()
#define GetCurrentProcess() \
    EngGetProcessHandle()

#define GetThreadPriority(a) \
    (0)
#define GetPriorityClass(a) \
    (0)
#define ConvertRing3EventToRing0(a) \
    HANDLE (0)
#define CreateEventA(a, b, c, d) \
    (0)
#define CreateThread(a, b, c, d, e, f) \
    (0)
#define OpenEventA(a, b, c) \
    (0)
#define CloseHandle(a)
#define GetTickCount()  0

//
// WINNT BUG - disable misc C lib, OS functions til we come up with a better
// solution.
//

#ifdef CreateFile
#undef CreateFile
#endif
#define CreateFile(a,b,c,d,e,f,g) (INVALID_HANDLE_VALUE)

#ifdef WriteFile
#undef WriteFile
#endif
#define WriteFile(a,b,c,d,e)

#ifdef SetFilePointer
#undef SetFilePointer
#endif
#define SetFilePointer(a,b,c,d)

#ifdef FlushFileBuffers
#undef FlushFileBuffers
#endif
#define FlushFileBuffers(a)

#ifdef QueryPerformanceCounter
#undef QueryPerformanceCounter
#endif
#define QueryPerformanceCounter(a) EngQueryPerformanceCounter((__int64 *)a)

#ifdef QueryPerformanceFrequency
#undef QueryPerformanceFrequency
#endif
#define QueryPerformanceFrequency(a) EngQueryPerformanceFrequency((__int64 *)a)

#ifdef RegQueryValueEx
#undef RegQueryValueEx
#endif
#define RegQueryValueEx(a,b,c,d,e,f) TRUE

#ifdef RegOpenKeyEx
#undef RegOpenKeyEx
#endif
#define RegOpenKeyEx(a,b,c,d,e) TRUE

#ifdef IsBadReadPtr
#undef IsBadReadPtr
#endif
#define IsBadReadPtr(a,b) FALSE

#ifdef IsBadWritePtr
#undef IsBadWritePtr
#endif
#define IsBadWritePtr(a,b) FALSE

/*
 * Debug print function, defined in NT display driver file DEBUG.C.
 */
#if defined(DEVELOP) || defined(DEBUG)
VOID DebugPrint(
    LONG  DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );
#endif // (DEVELOP || DEBUG)

#ifdef __cplusplus
}
#endif // __cplusplus

#define DX7_DEVICE_OBJECT_HANDLE 0x0c7c6c5

