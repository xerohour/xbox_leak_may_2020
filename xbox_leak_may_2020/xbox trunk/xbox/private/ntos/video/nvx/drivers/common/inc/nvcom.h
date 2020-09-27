#ifndef __NV_COM_H
#define __NV_COM_H

#include "nvOverlay.h"

// XXX Having to define this here sucks.  This is in mtexport.h but
// mtexport.h needs NVstate stuff :(   -paul
#ifndef WQ_MAX_THREAD
#define WQ_MAX_THREAD 2
#endif

typedef struct  __GLNVtagInfoRec {
    unsigned int  type;
    void          *address;
} __GLNVtagInfo;    

// Basic element of the pushbuffer
typedef union __GLNVdataRec {
    NvF32 f;
    NvS32 i;
    NvU32 u;
} __GLNVdata;

// Flags passed into the display driver when allocating or requesting
// info regarding the unified buffers.

enum  {
    SBD_BACK_1_OFFSET = 0,
    SBD_BACK_1_PITCH,
    SBD_BACK_2_OFFSET,
    SBD_BACK_2_PITCH,
    SBD_DEPTH_OFFSET,
    SBD_DEPTH_PITCH,
    SBD_FRONT_MAIN_OFFSET,
    SBD_FRONT_MAIN_PITCH,
    SBD_BACK_1_MAIN_OFFSET,
    SBD_BACK_1_MAIN_PITCH,
    SBD_FRONT_OVERLAY_OFFSET,
    SBD_FRONT_OVERLAY_PITCH,
    SBD_BACK_1_OVERLAY_OFFSET,
    SBD_BACK_1_OVERLAY_PITCH
};

typedef struct __GLNVsbdFlagsRec {
    int windowFlipping;  // Enable window flipping
    int overlaySupport;  // Allocate overlay buffers
    int surfaceInfoType;
} __GLNVsbdFlags;


typedef struct __GLNVpaletteDataRec {
  void         *windowHandle;
  unsigned int  entries[ICD_PALETTE_ENTRIES];
  unsigned int  crTransparent;
} __GLNVpaletteData;

typedef struct __GLMergeBlitDataRec
{
    NvU32 PrimaryFrontOffset; // offset of primary buffer (where DAC is working)
    NvU32 PrimaryFrontPitch ;
    NvU32 ScratchOffset     ; // offset of scratch buffer for mergeblit
    NvU32 ScratchPitch      ;

    NvU32 MainFrontOffset   ; // offset of main front plane (where overlay app has its main)
    NvU32 MainFrontPitch    ;
    NvU32 MainBackOffset    ; // offset of main back plane (where overlay app has its main)
    NvU32 MainBackPitch     ;

    NvU32 OverlayFrontOffset; // offset of overlay front plane (where overlay app has its overlay)
    NvU32 OverlayFrontPitch ;
    NvU32 OverlayBackOffset ; // offset of overlay back plane (where overlay app has its overlay)
    NvU32 OverlayBackPitch  ;

    int   colordepth        ; // 16 or 32, color depth of above buffers (UBB)

    NvU32 colorref          ; // colorkey for transparent color

    unsigned int   dwAction ; // NV_OVERLAY_xxx

    __GLregionRect *prclClip; // pointer to list of clip rects
    unsigned int    cClip;    // count of valid rects inside prclClip

    __GLregionRect rclUser  ; // single user clip rect 
} __GLMergeBlitData;


typedef struct __GLNVcmdInfoRec {

    void *state;  // __GLNVstate opaque ptr in case we need it.

    // Channel information for syncing in NTx display driver
    NvU32        maskOtherChannels;
    unsigned int ourChannelId;
    void         *ntOther; // PDEV pointer 
    void         *ntDrawable; // clientDrawableInfo pointer 

    // Number of hardware window clipping rectangles available.
    // Should be 8 for NV10GL and 1 for NV10 geForce.
    int numHWClipRects;

    // Client size view of windowChangedCount at time of kickoff; this is
    // compared with the latest count inside the display driver.  If they
    // are out of sync and flushBetween2D is TRUE then fail the kick off
    // in the display driver.
    int windowChangedCount;
    int flushedBetween2D;

    unsigned char isClipped;
    int serverSwapNT4;

    int nvDebugLevel;
    int nvDebugOptions;
    int nvDebugMask;
    int nvControlOptions;

    int temp0;
    int temp1;
    int temp2;
    int temp3;
    int temp4;
} __GLNVcmdInfo;

//
// NV10 flush information structure
//
typedef struct __GLNVflushInfoRec {
    // flush type, client handle and window handle
//    ULONG        type;
    ULONG        hClient;
#if defined(_WIN32)
    HWND         hWnd;
    HDC          hDC;
#endif

    // clip and mode switch info
    ULONG        localModeSwitchCount;

    __GLNVcmdInfo *cmdInfo;
    
    // Core Clip rectangle computed by wgl in screen coordinates
    __GLregionRect coreClipRect;

    // Tells Display driver that scissor has changed.
    ULONG wndClipRectChanged;
    
    // Clip Changed Count 
    ULONG clipChangedCount;

    // TRUE if rendering to the back buffer
    ULONG backBufferIsEnabled;

    // return status from flush command
    unsigned int retStatus;   // status of flush in display driver
} __GLNVflushInfo;

typedef struct __GLNVswapInfoRec {
    ULONG        hClient;
#if defined(_WIN32)
    HWND         hWnd;
    HDC          hDC;
#endif
    // window info
    __GLNVcmdInfo *cmdInfo;

    // clip and mode switch info
    ULONG        localModeSwitchCount;
    PVOID        drawable;

    // SwapHintRect info
    unsigned int        numSwapHintRects;
    __GLregionRect*     pSwapHintRects;

    // return status
    ULONG        retStatus;
} __GLNVswapInfo;


// probably needs to be removed to better place
// bits used in __glNVOverlayMergeFastLock - dwAction
#define NV_OVERLAY_MERGE_BLIT        0x00000001 // do front overlay + front main -> scratch -> primary
#define NV_OVERLAY_UPDATE_OVERLAY    0x00000010 // overlay back -> overlay front (prior to merge)
#define NV_OVERLAY_UPDATE_MAIN       0x00000020 // main back -> overlay front (prior to merge)
#define NV_OVERLAY_USE_CLIPRECT      0x00000100 // only do mergeblit on given rect (else entire window)
#define NV_OVERLAY_DEFERRED          0x00001000 // defer merge blit (do update later)
#define NV_OVERLAY_RUN_IN_DD         0x00002000 // do mergeblit in display driver
#define NV_OVERLAY_NO_OVERLAY_PLANE  0x00010000 // there is no overlay plane, front main -> primary (will overwrite NV_OVERLAY_MERGE_BLIT)


//
// Derived from __GLNVswapInfo to support overlay merge blit through DD.
// Used with the ESC_NV_OPENGL_OVERLAY_MERGEBLIT escape 
// in __glNVOverlayMergeFastLock.
//
typedef struct __GLNVoverlayMergeBlitInfoRec {
    unsigned int hClient;
#if defined(_WIN32)
    HWND         windowHandle;
    HDC          deviceContext;
#endif
    
    // window info
    __GLNVcmdInfo *cmdInfo;

    // clip and mode switch info
    unsigned int localModeSwitchCount;

    // return status
    unsigned int retStatus;

    unsigned int dwAction; // NV_OVERLAY_xxx

    int          x;        // bounding rectangle if NV_OVERLAY_USE_CLIPRECT is set 
    int          y;
    int          width;
    int          height;

    // SwapHintRect info
    unsigned int numSwapHintRects;
    void *       pSwapHintRects;

} __GLNVoverlayMergeBlitInfo;

//
// These routines live in the shared lib and are called from both the ICD
// and from the display driver side of OGL escape calls.  -paul
//

// nvsharedpushbuf.c
extern void STDCALL __glNVMakeJumpSpaceInPushBuffer(__GLNVcmdInfo *cmdInfo);
extern void STDCALL __glNVSetupAndKickoff(__GLNVcmdInfo *cmdInfo,
                                          unsigned int numInclusiveRects,__GLregionRect *inclusiveRects,
                                          unsigned int numExclusiveRects,__GLregionRect *exclusiveRects);
// nvsharedutils.c
extern void STDCALL __glNVInitSharedFnPtrs(__GLNVcmdInfo *cmdInfo);

// nvsharedswap.c
extern GLboolean STDCALL __glNVSetupSwap (__GLNVcmdInfo *cmdInfo, void * drawable,
                                          unsigned int numRects, __GLregionRect *rects,
                                          int screenWidth, int screenHeight);
extern GLboolean STDCALL __glNVSetupMergeBlit (__GLNVcmdInfo *cmdInfo, 
                                          struct __GLMergeBlitDataRec *pMergeBlitData);

// nv4sharedpushbuf.c
extern void STDCALL __glNV4SetupAndKickoff(__GLNVcmdInfo *cmdInfo,
                                           unsigned int numRects,
                                           __GLregionRect *rects);
#endif  // __NV_COM_H
