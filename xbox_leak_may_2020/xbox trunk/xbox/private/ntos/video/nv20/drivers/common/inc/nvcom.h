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

// Structure declarations to keep gcc happy
struct __GLNVstateRec;
struct __GLdrawablePrivateRec;

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
    SBD_BACK_1_OVERLAY_PITCH,
    // ogl api stereo buffers below
    SBD_FRONT_RIGHT_OFFSET,
    SBD_FRONT_RIGHT_PITCH,
    SBD_BACK_1_RIGHT_OFFSET,
    SBD_BACK_1_RIGHT_PITCH,
    SBD_BACK_2_RIGHT_OFFSET,
    SBD_BACK_2_RIGHT_PITCH,
    SBD_DEPTH_RIGHT_OFFSET,
    SBD_DEPTH_RIGHT_PITCH,
    SBD_FRONT_MAIN_RIGHT_OFFSET,
    SBD_FRONT_MAIN_RIGHT_PITCH,
    SBD_BACK_1_MAIN_RIGHT_OFFSET,
    SBD_BACK_1_MAIN_RIGHT_PITCH,
    SBD_FRONT_OVERLAY_RIGHT_OFFSET,
    SBD_FRONT_OVERLAY_RIGHT_PITCH,
    SBD_BACK_1_OVERLAY_RIGHT_OFFSET,
    SBD_BACK_1_OVERLAY_RIGHT_PITCH
};

typedef struct __GLNVsbdFlagsRec {
    int windowFlipping;  // Enable window flipping
    int overlaySupport;  // Allocate overlay buffers
    int surfaceInfoType;
    int depthCompressed;
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

    struct __GLNVstateRec *state;

    // Channel information for syncing in NTx display driver
    NvU32        maskOtherChannels;
    unsigned int ourChannelId;
    void         *ntOther;      // PDEV pointer 
    void         *ntDrawable;   // clientDrawableInfo pointer 
    void         *ntClientInfo; // clientInfo pointer 

    // Number of hardware window clipping rectangles available.
    // Should be 8 for NV10GL and 1 for NV10 geForce.
    int numHWClipRects;

    // TRUE if rendering to the back buffer
    unsigned int backBufferIsEnabled;

    // Client size view of windowChangedCount at time of kickoff; this is
    // compared with the latest count inside the display driver.  If they
    // are out of sync and flushBetween2D is TRUE then fail the kick off
    // in the display driver.
    int windowChangedCount;
    int flushedBetween2D;

    unsigned char isClipped;
    unsigned char usingCopyBuf;
    int syncGdi;

    int nvDebugLevel;
    int nvDebugOptions;
    int nvDebugMask;
    int nvControlOptions;

    // Core Clip rectangle computed by wgl in screen coordinates
    __GLregionRect coreClipRect;

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

    // clip and mode switch info
    ULONG        localModeSwitchCount;

    __GLNVcmdInfo *cmdInfo;
    
    // Tells Display driver that scissor has changed.
    ULONG wndClipRectChanged;
    
    // Clip Changed Count 
    ULONG clipChangedCount;

    // return status from flush command
    unsigned int retStatus;   // status of flush in display driver
} __GLNVflushInfo;

typedef struct __GLNVswapInfoRec {
    ULONG        hClient;

    // window info
    __GLNVcmdInfo *cmdInfo;

    // clip and mode switch info
    ULONG        clipChangedCount;
    ULONG        localModeSwitchCount;
    PVOID        drawable;

    // SwapHintRect info
    unsigned int        numSwapHintRects;
    __GLregionRect*     pSwapHintRects;

    // return status
    ULONG        retStatus;
} __GLNVswapInfo;

// ***********************************************************************
// Command interface for OpenGL operations which must be performed inside
// the display driver (or mutexed, depending on the OS)
enum {
    NVOP_STATUS_ERROR,
    NVOP_STATUS_SUCCESS,
    NVOP_STATUS_NEED_UPDATE,
    NVOP_STATUS_UNKNOWN_OP
};

typedef enum {
    NVOP_CLEAR,
    NVOP_SWAP,
} __GLNVopcode;

typedef struct __GLNVclearCmdRec {
    GLbitfield mask;                        // buffers to clear
    __GLregionRect rect;                    // user clear rect
    GLint drawBufferMask;                   // mask of active buffers
    /* Color buffer */
    struct {
        GLfloat r, g, b, a;
    } clearColor;                           // clear color
    GLboolean rMask;                        // color buffer red mask
    GLboolean gMask;                        // color buffer green mask
    GLboolean bMask;                        // color buffer blue mask
    GLboolean aMask;                        // color buffer alpha mask
    /* Depth buffer */
    GLfloat clearDepth;                     // clear depth
    /* Stencil buffer */
    GLuint clearStencil;                    // clear stencil
    GLuint stencilMask;                     // stencil mask
} __GLNVclearCmd;

typedef struct __GLNVoperationRec {
    __GLNVopcode opCode;
    __GLNVcmdInfo *cmdInfo;
    struct __GLNVstateRec *state;             // can this be removed?
    struct __GLdrawablePrivateRec *dp;
    struct {
        GLint numRects;
        __GLregionRect *rects;
    } clip;
    union {
        __GLNVclearCmd clear;
    } u;
    GLint retStatus;
} __GLNVoperation;

// ***********************************************************************


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
extern GLboolean STDCALL __glNVSetupAndKickoff(__GLNVcmdInfo *cmdInfo,
                                          unsigned int numInclusiveRects,__GLregionRect *inclusiveRects,
                                          unsigned int numExclusiveRects,__GLregionRect *exclusiveRects,
                                          unsigned char failOnClipChange);
extern void STDCALL __glNVCopyPseudoKickoff(__GLNVcmdInfo *cmdInfo, int forceRealBuf);

// nvsharedutils.c
extern void STDCALL __glNVInitSharedFnPtrs(__GLNVcmdInfo *cmdInfo);
extern void STDCALL __glNVDoRegistryHacks(unsigned int hwGfxCaps,
                                          unsigned int *flippingControl,
                                          unsigned int *doUnifiedBuffers,
                                          unsigned int *doWindowFlipping,
                                          unsigned int *applicationKey,
                                          unsigned int *appSupportBits,
                                          unsigned int *overlaySupport,
                                          unsigned int *apiStereoSupport,
                                          unsigned int *apiStereoMode);
extern void STDCALL __glNVLoadRegistryDefaults(unsigned int hwGfxCaps,
                                               unsigned int *doUnifiedBuffers,
                                               unsigned int *doWindowFlipping,
                                               unsigned int *flippingControl,
                                               unsigned int *applicationKey,
                                               unsigned int *appSupportBits,
                                               unsigned int *overlaySupport,
                                               unsigned int *apiStereoSupport,
                                               unsigned int *apiStereoMode);

extern void STDCALL __glNVUpdateStereoBuffers(struct __GLdrawablePrivateRec *dp);

// nvsharedswap.c
extern GLboolean STDCALL __glNVWasFlipped(__GLNVcmdInfo *cmdInfo);
extern GLboolean STDCALL __glNVSetupSwap(__GLNVcmdInfo *cmdInfo, void *drawable,
                                         unsigned int numRects, __GLregionRect *rects,
                                         int screenWidth, int screenHeight);
extern int STDCALL __glNVIsVerticalInterlacedApiStereo(void *drawable);
extern GLboolean STDCALL __glNV4SetupSwap(__GLNVcmdInfo *cmdInfo, void *drawable,
                                          unsigned int numRects, __GLregionRect *rects,
                                          int screenWidth, int screenHeight);
extern GLboolean STDCALL __glNVPrepareStereoSwap(void *drawable,
                                                 int stereoBroadcastingNeeded);
extern GLboolean STDCALL __glNVSetupMergeBlit(__GLNVcmdInfo *cmdInfo, 
                                              struct __GLMergeBlitDataRec *pMergeBlitData);

// nv4sharedpushbuf.c
extern void STDCALL __glNV4SetupAndKickoff(__GLNVcmdInfo *cmdInfo,
                                           unsigned int numRects,
                                           __GLregionRect *rects);

// nvsharedclear.c
extern void STDCALL __glNVSetupClear(__GLNVoperation *op);

// oglexpor.c in DD
extern void CDECLCALL __glNVFlushInternal(__GLNVcmdInfo *cmdInfo, unsigned char forceRealBuf);
extern void CDECLCALL __glNV4FlushInternal(__GLNVcmdInfo *cmdInfo);

#endif  // __NV_COM_H
