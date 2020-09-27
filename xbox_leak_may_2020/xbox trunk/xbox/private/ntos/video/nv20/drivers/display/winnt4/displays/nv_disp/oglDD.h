#ifndef __OGLDD_H
#define __OGLDD_H

// DirectDraw ------------------------------------------------------
#if _WIN32_WINNT >= 0x0500
#include "nvPriv.h"

// ----- Escapes: these are duplicated in escape.h to block out these numbers
// lpIn = pointer to input structure to nvMoComp
// lpOut = pointer to a DWORD return code
#define NV_ESC_DDPRIV_DMABLIT               0x6E88

// lpIn = pointer to input structure to nvOverlayControl
// lpOut = pointer to a DWORD return code
#define NV_ESC_DDPRIV_OVERLAYCONTROL        0x6E89

// ----- Prototypes
extern NVS2VERROR __stdcall nvMoComp(LPNVS2VDATATYPE lpS2VData);
extern NVOCERROR __stdcall nvOverlayControl(LPNVOCDATATYPE lpOCData);
#endif  // WINNT >= 0x500

// OGL -------------------------------------------------------------

//*************************************************************************
// OGLSERVER
// 
// This structure is hooked to the display driver ppdev, created at startup
// time of the driver (DrvEnableSurface and deleted in DrvDisableSurface). 
// On creation it is initialized with 0 (zero), you can rely on this!
// It should be used to store all OpenGL server related data. 
// All OpenGL stuff inside ppdev will be moved to this structure (later).
//
// Please add a good comment to all variables!
//*************************************************************************
typedef struct _OGLSERVER
{
    // rclAllClientsBounds is the bounding rectangle around all visible
    // OpenGL clients.
    RECTL rclAllClientsBounds; 

    // bGdiInOglWindowNeedsSync to determine if a GDI command was rendering 
    // to the OGL window and needs to be synced before OGL is allowed to 
    // continue.
    // TRUE:  GDI is rendering to OGL window and needs to be synced before 
    //        OGL is rendering!
    // FALSE: nothing needs to be done
    BOOL  bGdiInOglWindowNeedsSync;   

    // bWindowInMovement is used to notify the oglsync code that a 
    // copy on windowmove of the ogl is in progress.
    // TRUE  : OGL window is moving.
    // TRUE  : OGL window isn't moving.
    BOOL  bWindowInMovement;          

    // ulPageFlipRetryCounter will control how and if bOglPageFlipModeUpdate
    // is allowed to enable pageflipping again, after it was disabled.
    ULONG ulPageFlipRetryCounter;     
} OGLSERVER;


//
// Defines used to work with OGLSERVER->ulPageFlipRetryCounter
// PAGEFLIP_IMMEDIATE_UPDATE
//      An immediate pageflip update is enabled. This is the default 
//      behaviour of the driver. Pageflip updates are not defer and 
//      the retry counter is disabled.
//      
// PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP
//      retry to enable pageflipping in next swap buffers
//
// PAGEFLIP_RETRY_COUNTER_START >= i > PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP
//      countdown area, do not enable pageflipping within this area
//
// PAGEFLIP_RETRY_COUNTER_START
//      Use this to initialize the counter. As long it is within the 
//      countdown area, pageflipping isn't enabled!
#define PAGEFLIP_IMMEDIATE_UPDATE           0
#define PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP  1
#define PAGEFLIP_RETRY_COUNTER_START        (ppdev->ulGDIRefreshRate)


//******************************************************************
// This is the maximum number of ddraw surfaces that we allow to
// exist before we disable window flipping for OpenGL.  This is tested
// against in __glNVDoDACParams() and in NV_OglDDrawSurfaceCreated().
//
// The surface count lives in ppdev->oglDDrawSurfaceCount and is
// incremented by NV_OglDDrawSurfaceCreated() and decremented by
// NV_OglDDrawSurfaceDestroyed().  On NT5, the real ddraw driver calls
// these routines from CreateSurface32/DestroySurface32 in the ddraw
// subtree.  On NT4 they are called from within the display driver in
// DdCreateSurface and Nv4DestroySurface since ddraw is built into the
// display driver.
//
// For NT5 this maximum is 2 since running any opengl app will create 
// 2 ddraw surfaces. On NT4, no ddraw surfaces are created for an opengl
// app so anything above 0 signifies that an actual ddraw app was
// started
//******************************************************************
#if _WIN32_WINNT >= 0x0500
#define OGL_MAX_DDRAW_SURFACES_FOR_FLIPPING 2
#else
#define OGL_MAX_DDRAW_SURFACES_FOR_FLIPPING 0
#endif

#if DBG
#define OglDebugPrint OglDebugPrintFunc
#else
#define OglDebugPrint
#endif

#ifdef _WIN64
#define __GLNV_FENCE()                                                  \
{                                                                       \
    extern void nvFence(void);                                          \
                                                                        \
    nvFence();                                                          \
}
#else
#define __GLNV_FENCE()                                                  \
{                                                                       \
    GLuint dummy = 0xDEADBEEF;                                          \
    GLuint *fencePtr = &dummy;                                          \
    __asm mov  eax, (fencePtr)                                          \
    __asm xchg [eax], ecx                                               \
}
#endif

#define OGL_CLIENT_DLL_NAME_NV4                 L"nvoglnt.dll"
#define OGL_CLIENT_DLL_NAME_NV3                 L"nv3oglnt.dll"



#define NV_OGL_CLIENT_DRAWABLE_INFO 0x0001 // if bit ON then drawable info exists...
#define NV_OGL_CLIENT_CONTEXT_INFO  0x0002 // if bit ON then context info exits...
#define NV_OGL_CLIENT_SHARED_INFO   0x0004 // if bit ON then context info exits...
#define NV_OGL_CLIENT_WOC_DELETE    0x0008 // if bit ON then WOC_DELETE occured on window...
#define NV_OGL_CLIENT_RM_CLIENT     0x0010 // if bit ON then tracking RM client in node

//
// Use client side rectangle list or display driver side...
//
#undef USE_CLIENT_RECT_LIST
#define USE_CLIENT_RECT_LIST 1


/*******************************************************************************
 *
 *    Useful information for OpenGL support.  Not guaranteed to work.
 *
 ******************************************************************************/


#define     OPENGL_CMD              4352
#define     OPENGL_GETINFO          4353
#define     NV_OPENGL_SET_ENABLE    0x6988
#define     OPENGL_GETINFO_DRVNAME  0

#define     OPENGL_ICD_VER          2
#define     OPENGL_DRIVER_VER       0x10000
#define     OPENGL_KEY_NAME_NV3     L"RIVA128"
#define     OPENGL_KEY_NAME_NV4     L"RIVATNT"

typedef struct {
    unsigned long   ulVersion;
    unsigned long   ulDriverVersion;
    unsigned char   awch[ MAX_PATH + 1 ];
} OPENGL_INFO, FAR *LP_OPENGL_INFO;

#define     ESC_ALLOC_CONTEXT_DMA                   0x6989
#define     ESC_FREE_CONTEXT_DMA                    0x698A
#define     ESC_ALLOC_CHANNEL_DMA                   0x698B
#define     ESC_FREE_CHANNEL_DMA                    0x698C
#define     ESC_DMA_FLOW_CONTROL                    0x698D
#define     ESC_SET_GAMMA_RAMP                      0x698E
#define     ESC_GET_GAMMA_RAMP                      0x698F
#define     ESC_GET_PIXEL_FORMAT                    0x7000
#define     ESC_NV_OPENGL_DMA_PUSH_GO               0x7001

// Reserving Escape for Intel Diagnostics - Escape value is ASCII "nv"+1
#define     ESC_INTEL_DIAGNOSTICS                   0x6e77

/*******************************************************************************
 * Macros for PAGE ALIGNMENT and PAGE SIZE.
 *
 * It would be preferable to grab this out of the DDK header files,
 * but the Windows SDK header files conflict with the DDK header files.
 ******************************************************************************/

#ifdef _M_IA64

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x2000
#endif

#ifndef PAGE_SHIFT
#define PAGE_SHIFT 13L
#endif

#else // #ifdef _M_IA64

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12L
#endif

#endif // #ifdef _M_IA64

#ifndef BYTES_TO_PAGES
#define BYTES_TO_PAGES(Size)  (((ULONG)(Size) >> PAGE_SHIFT) + \
                            (((ULONG)(Size) & (PAGE_SIZE - 1)) != 0))
#endif

#ifndef ROUND_TO_PAGES
#define ROUND_TO_PAGES(Size)  (((ULONG)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#endif


#define PAGE_ALIGN_UP(Va) ((PVOID)(((ULONG_PTR)(Va) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)))

/************************************WARNING*******************************************/
/********* These are duplicated from OpenGL gl\glcore.h********************************/
/**************************************************************************************/

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

#ifdef _WIN32
typedef struct __GLregionRectRec {
    /* lower left (inside the rectangle)  = (x0, y0)*/
    /* upper right (outside the rectangle)   = (x1, y1)*/
    /* This structure is modified and arranged so that it matches with win32
     * structure RECT and the structure RECTL used by DDK.
     * This avoids us doing extra copies of the rectangle lists
     * when we share the code in Display Driver.
     * RECT structure is
     * typedef struct _RECT 
     * {    
     *  // rc     
     *      LONG left;     
     *      LONG top;     
     *      LONG right; 
     *      LONG bottom; 
     *  } RECT;  
     * the order of the members (x0, y1, x1, y0).
     */ 
    LONG x0;  /* left */
    LONG y0;  /* top   $%$%$ XXX NEED TO REPAIR nv10pushbufclip to have correct meaning of y1 = TOP, y0 = BOTTOM*/
    LONG x1;  /* right */
    LONG y1;  /* bottom */
} __GLregionRect;
#else // _WIN32
typedef struct __GLregionRectRec {
    /* lower left (inside the rectangle)  = (x0, y0)*/
    /* upper right (outside the rectangle)   = (x1, y1)*/
    /* This structure is modified and arranged so that it matches with win32
     * structure RECT and the structure RECTL used by DDK.
     * This avoids us doing extra copies of the rectangle lists
     * when we share the code in Display Driver.
     * RECT structure is
     * typedef struct _RECT 
     * {    
     *  // rc     
     *      LONG left;     
     *      LONG top;     
     *      LONG right; 
     *      LONG bottom; 
     *  } RECT;  
     * the order of the members (x0, y1, x1, y0).
     */ 
    int x0;  /* left */
    int y0;  /* top   $%$%$ XXX NEED TO REPAIR nv10pushbufclip to have correct meaning of y1 = TOP, y0 = BOTTOM*/
    int x1;  /* right */
    int y1;  /* bottom */
} __GLregionRect;
#endif // _WIN32

#include "nvEscDef.h"
#include "nvcom.h"

//////////////////////////////////////////////////////////////////////
// OpenGL exports.
VOID OglRemoveClient(PDEV *ppdev, NV_OPENGL_CLIENT_INFO_LIST *clientList);
NV_OPENGL_DRAWABLE_INFO *CreateDrawableClientInfo(SURFOBJ *pso, HWND hWnd, HDC hDC, struct tagPIXELFORMATDESCRIPTOR *ppfd);
VOID DestroyDrawableClientInfo(PDEV *ppdev, HWND hWnd, HDC hDC, ULONG deleteFlag);
ULONG CreateContextClientInfo(PDEV *ppdev, HWND hWnd, ULONG hClient);
VOID DestroyContextClientInfo(PDEV *ppdev, ULONG hClient, ULONG deleteFlag);
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoFromHWndHDC(PDEV *ppdev, HWND hWnd, HDC hDc);
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoFromHWnd(PDEV *ppdev, HWND hWnd);
VOID CALLBACK DrvClipChanged(WNDOBJ *pwo, FLONG fl);
VOID CALLBACK DrvClipChanged1(WNDOBJ *pwo, FLONG fl);
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoHClient(PDEV *ppdev, ULONG hClient);
NV_OPENGL_CLIENT_INFO *OglFindDrawableInClientInfoList(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *inDrawablePtr);
VOID OglDebugPrintFunc(PCHAR DebugMessage, ...);
NV_OPENGL_CLIENT_INFO_LIST *OglAppendToClientInfoList(
    PDEV    *ppdev,
    DWORD   flags,                               // bits indicating allocated/free resources
    HDRVOBJ hDrvObj,                             // driver object handle for this client
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo, // drawable information for this client
    NV_OPENGL_CONTEXT_INFO  *clientContextInfo); // context information for this client
void OglRemoveFromClientInfoList(PDEV *ppdev, NV_OPENGL_CLIENT_INFO_LIST *toFree);
void OglRemovePdevListFromClientList(NV_OPENGL_CLIENT_INFO_LIST *clientList);

NV_OPENGL_CLIENT_INFO_LIST *OglFindClientListFromClientInfo(PDEV *ppdev, NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL OglValidateDrawableInfo(NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
void CreateSharedClientInfo(PDEV *ppdev, ULONG hClient, ULONG processHandle, PVOID *globalData, PVOID *pfifoAddress, PVOID *pbusAddress);
void DestroySharedClientInfo(PDEV *ppdev, ULONG hClient, ULONG processHandle, PVOID globalData, ULONG deleteFlag);
BOOL OglEngDeleteDriverObj(PDEV *ppdev, NV_OPENGL_CLIENT_INFO *clientInfo, ULONG flags);
void OglIncClientCount(PDEV *ppdev);
void OglDecClientCount(PDEV *ppdev);
BOOL OglMapGlobalPageAddress(PDEV *ppdev, PVOID *globalData);
BOOL OglUnmapGlobalPageAddress(PDEV *ppdev, PVOID globalData);
BOOL OglMapPFifoAddress(PDEV *ppdev, PVOID *pfifoAddress);
BOOL OglUnmapPFifoAddress(PDEV *ppdev, PVOID pfifoAddress);
BOOL OglMapPBusAddress(PDEV *ppdev, PVOID *pbusAddress);
BOOL OglUnmapPBusAddress(PDEV *ppdev, PVOID pbusAddress);
BOOL OglAcquireClipMutex(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
VOID OglReleaseClipMutex(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
#if DBG
BOOL OglVerifyClientList(PDEV *ppdev, char *s);
#endif // DBG

void OglChannelWait(PDEV *ppdev, ULONG ourChannelId, ULONG maskOtherChannels);
ULONG NV4_OglSwapBuffers(PDEV *ppdev, HWND hWnd, HDC hDC, ULONG hClient, __GLNVswapInfo *swapInfo);
ULONG NV4_OglFlushClipped(PDEV *ppdev, HWND hWnd, HDC hDC, ULONG hClient, __GLNVflushInfo *flushInfo);
void NV_OglInitState(PDEV *ppdev, void *cmdInfo);
void NV_OglGetAppSupportSettings(PDEV *ppdev);

#if (NVARCH >= 0x4)
ULONG NV_OglSwapBuffers(PDEV *ppdev, HWND hWnd, HDC hDC, ULONG hClient, __GLNVswapInfo *swapInfo);
ULONG NV_OglFlushClipped(PDEV *ppdev, HWND hWnd, HDC hDC, ULONG hClient, __GLNVflushInfo *flushInfo);
ULONG NV_OglOverlayMergeBlit(PDEV *ppdev, HWND hWnd, HDC hDC, __GLNVoverlayMergeBlitInfo *mergeInfo);
ULONG NV_OglOperation(PDEV *ppdev, HWND hWnd, HDC hDC, __GLNVoperation *op);
BOOL NV_OglUpdateClipLists(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);

// This lives in the shared lib and is called into only by the display driver
void STDCALL __glNVKickOffPushBufferNT(__GLNVcmdInfo *cmdInfo);
void STDCALL __glNV4KickOffPushBufferNT(__GLNVcmdInfo *cmdInfo);
#endif  // (NVARCH >= 0x4)

void CombineRectLists(RECTL *, ULONG , RECTL *, ULONG, LONG, LONG, RECTL *, ULONG *);

int OglAllocUnifiedSurfaces(PDEV *ppdev, __GLNVsbdFlags *sbdFlags);
int OglFreeUnifiedSurfaces(PDEV *ppdev);
NvU32 OglGetUnifiedSurfaceInfo(PDEV *ppdev, __GLNVsbdFlags *sbdFlags);
void OglTranslateCachedClipRectList(PDEV *ppdev,
                                    RECTL *srcRects, ULONG srcNumRects, 
                                    RECTL *destRects, ULONG *destNumRects,
                                    RECTL *boundingBox);
void OglTranslateClientRelativeRectListToDevice(PDEV *ppdev,
                                    RECTL *srcRects, ULONG srcNumRects, 
                                    RECTL *destRects, ULONG *destNumRects,
                                    RECTL *boundingBox);
void OglRegisterRmClient(PDEV *ppdev, ULONG rmClient);
void OglUnRegisterRmClient(PDEV *ppdev, ULONG rmClient);
ULONG OglCpuBlit(PDEV *ppdev, __GLNvCpuBltData *oglNVCpuBltData);
void OglReadRegistry(PDEV *ppdev);


void OglUseDisplayDriverBlt(
    PDEV *ppdev, 
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
    LONG srcOffset,
    LONG srcPitch,
    LONG srcLeft, 
    LONG srcTop, 
    LONG dstOffset,
    LONG dstPitch,
    LONG dstLeft, 
    LONG dstTop, 
    LONG width, 
    LONG height);

BOOL bGetOglClientInfoHWnd(struct _NV_OPENGL_CLIENT_INFO *clientInfo, HWND* phWnd);
BOOL bExchangeClientsFrontAndBack(PPDEV,NV_OPENGL_DRAWABLE_INFO*);//resides in oglsurf.c
VOID vOglFlipChangedUpdate(PPDEV ppdev, NV_OPENGL_CLIENT_INFO*);
VOID vFlipToPrimary(PPDEV);//resides in oglsurf.c
void OglAllocateAndCopyWindowClipList(PDEV *, NV_OPENGL_DRAWABLE_INFO *, RECTL *, ULONG);

#endif // __OGD_DD_H

