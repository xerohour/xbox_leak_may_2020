
/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
*                                                                           *
* Module: surfaces.h                                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           3/26/98                                          *
*                                                                           *
\***************************************************************************/


#ifndef surfaces_h
#define surfaces_h

//#define MAXDATASTORE (0x640)
#define MAXDATASTORE (0x640*4)

/*  FIXES TO GET THIS TO COMPILE UNDER MY NV3 DIRECTORY */

#include "VidTex.h"
#include "VPCallbk.h"
#include "transfer.h"
#include "timing.h"

#ifndef RESMANSIDE

#include "nvddobj.h"

#ifdef NV3

#define info32  otherInfo32
#define info16  otherInfo16
#define nvContextDmaToMemory contextDmaToMemory
#define nv03ExternalVideoDecoder    externalVideoDecoder
#define NV04D_NOTIFICATION_STATUS_IN_PROGRESS   NV_STATUS_IN_PROGRESS
#define NV04D_NOTIFICATION_STATUS_DONE_SUCCESS  NV_STATUS_DONE_OK

#endif // NV3

#endif // !RESMANSIDE

// this file will include the definitions of the common
// data structures between the Ring3 VPE code
// and the ring0 VXD code which will actually program
// things

//---------------------------------------------------------------------------

// defines the number of video ports that this system will handle
#define NV_MAX_VIDEOPORTS   (1)

// these next three are measured in Pixels ie 2 byte chunks
#define NV4_MAX_FIELDWIDTH (0x1000)
#define NV4_MAX_FIELDHEIGHT (0x5dc)	//1500. current max.height=1080p for HD
#define NV4_MAX_VBIWIDTH (0x1000)
#define NV4_MAX_BUFFERLENGTH (0x80000)

#define NV4_MAX_PIXELRATE ( 27000000)

// these next three are measured in Pixels ie 2 byte chunks
#define NV10_MAX_FIELDWIDTH (0x1000)
#define NV10_MAX_FIELDHEIGHT (0x5dc)	//1500. current max.height=1080p for HD
#define NV10_MAX_VBIWIDTH (0x1000)
#define NV10_MAX_BUFFERLENGTH (0x800000)

#define NV10_MAX_PIXELRATE ( 100000000)     // a little arbitrary, need to confirm (jsun)

// Here we'll put chip specific info which may change from part to part...
#ifdef NV3
// defines the number of video ports that this system will handle
#define NV_MAX_VIDEOPORTS   (1)
// these next three are measured in Pixels ie 2 byte chunks
#define NV_MAX_FIELDWIDTH (0x1000)
#define NV_MAX_VBIWIDTH (0x1000)
#define NV_MAX_BUFFERLENGTH (0x80000)
#define NV_MAX_PIXELRATE ( 27000000)
#endif // NV3

//---------------------------------------------------------------------------

#ifdef NV4
// defines the number of video ports that this system will handle
#define NV_MAX_VIDEOPORTS   (1)
// these next three are measured in Pixels ie 2 byte chunks
#define NV_MAX_FIELDWIDTH 2000	//since max now is 1920
#define NV_MAX_VBIWIDTH (0x1000)
#define NV_MAX_BUFFERLENGTH (0x80000)
#define NV_MAX_PIXELRATE ( 27000000)
#define NV_MAX_FIELDHEIGHT	1200	//since max now is 1080i
#endif //NV4

//---------------------------------------------------------------------------

#ifdef NV5
// defines the number of video ports that this system will handle
#define NV_MAX_VIDEOPORTS   (1)
// these next three are measured in Pixels ie 2 byte chunks
#define NV_MAX_FIELDWIDTH (0x1000)
#define NV_MAX_VBIWIDTH (0x1000)
#define NV_MAX_BUFFERLENGTH (0x80000)
#define NV_MAX_PIXELRATE ( 27000000)

const DWORD MY_CONNECTION_CAPS= //DDVPCONNECT_DOUBLECLOCK           |
                                            //DDVPCONNECT_VACT                  |
                                            //DDVPCONNECT_INVERTPOLARITY        |
                                            DDVPCONNECT_DISCARDSVREFDATA    |
                                            //DDVPCONNECT_HALFLINE              |
                                            DDVPCONNECT_INTERLACED          |
                                            //DDVPCONNECT_SHAREEVEN         |
                                            //DDVPCONNECT_SHAREODD              |
                                            0;
#endif //NV5

//---------------------------------------------------------------------------

#ifdef NV10
// defines the number of video ports that this system will handle
#define NV_MAX_VIDEOPORTS   (1)
// these next three are measured in Pixels ie 2 byte chunks
#define NV_MAX_FIELDWIDTH 2000	//since max now is 1920
#define NV_MAX_VBIWIDTH (0x1000)
#define NV_MAX_BUFFERLENGTH (0x80000)
#define NV_MAX_PIXELRATE ( 27000000)
#define NV_MAX_FIELDHEIGHT	1200	//since max now is 1080i
#endif //NV10

//---------------------------------------------------------------------------

// since many compiler have poor enum implementations
// I'll reserve the right to do this manually
//#define DECENTENUMS

#ifdef DECENTENUMS

enum    {
    VP_NOT_RUNNING = 0,
    VP_VBI_CAPTURING =1,
    VP_IMAGE_CAPTURING =2,
    VP_BOTH_CAPTURING = 3
} VPSTATE;

enum {

    MY_EXTERNAL_DECODER_OBJECT      = 0x80024000,
    MY_EXTERNAL_DECODER_NOTIFIER,
    MY_IMAGE0_BUFFER_CONTEXT,
    MY_IMAGE1_BUFFER_CONTEXT,
    MY_VBI0_BUFFER_CONTEXT,
    MY_VBI1_BUFFER_CONTEXT

} MYINSTANCES;

enum {
    VBISURF = (0xFEEDC0DE),
    IMAGESURF
} SURFKIND;

enum {
    NOVPERROR               = 0,
    TOOMANYSURFACESERROR = 20,
    CANTCREATESURFACE,
    SURFACENOTFOUND = 9999,
    NOSURFACEPROGRAMED = 999
} VPERRORS;


enum {
    EVENODD =1,
    EVENEVEN,
    ODDODD
} CAPTUREMODE;

enum {
    SEPERATEBUF = 10,
    INTERLEAVBUF  = 20
} SCANMODE;

// These constants are designed to correspond with the NV3 hardware.
enum {
    ODD = 0,
    EVEN,
    UNKNOWN
} FIELDTYPE;


#else
#define SURFKIND   U032
#define VBISURF (0xFEEDC0DE)
#define IMAGESURF   (0xBAD0BEEF)

#define VPSTATE         U032
#define VP_NOT_RUNNING          (0)
#define VP_VBI_CAPTURING        (1)
#define VP_IMAGE_CAPTURING  (2)
#define VP_BOTH_CAPTURING   (3)

// standard decoder object handle values
#define MY_EXTERNAL_DECODER_OBJECT      (0x80024000)
#define MY_EXTERNAL_DECODER_NOTIFIER    (0x80024001)
#define MY_IMAGE0_BUFFER_CONTEXT        (0x80024002)
#define MY_IMAGE1_BUFFER_CONTEXT        (0x80024003)
#define MY_VBI0_BUFFER_CONTEXT          (0x80024004)
#define MY_VBI1_BUFFER_CONTEXT          (0x80024005)
#define MY_IMAGE1_BUFFER_EVENT          (0x80024006)
#define MY_IMAGE0_BUFFER_EVENT          (0x80024007)
#define MY_VBI1_BUFFER_EVENT                (0x80024008)
#define MY_VBI0_BUFFER_EVENT                (0x80024009)
#define MY_VPE_CHANNEL                      (0x8002400A)
#define  MY_TIMER_NOTIFIER                  (0x8002400B)
#define  MY_TIMER_OBJECT                    (0x8002400C)
#define  MY_TIMER_EVENT                     (0x8002400D)
// handle values for MTM
#define MY_MTM_OBJECT                       (0x80024010)
#define MY_MTM_NOTIFIER                 (0x80024011)
#define MY_MTM_FROM_CONTEXT             (0x80024012)
#define MY_MTM_TO_CONTEXT                   (0x80024013)
#define MY_MTM_EVENT                        (0x80024014)
//#define   MY_MTM_EVENT2                       (0x80024015)
// values for the texture surface
#define MY_SWIZ_OBJECT                      (0x80024020)
#define MY_SWIZ_NOTIFIER                    (0x80024021)
#define  MY_2DSWIZD_OBJECT                  (0x80024022)
#define  MY_2DSWIZD_NOTIFIER                (0x80024023)

#define MY_OVERLAY0_BUFFER_EVENT                (0x80024025)
#define MY_OVERLAY1_BUFFER_EVENT                (0x80024026)

#define MY_DVD_SUBPICTURE_OBJECT                (0x80024027)

#define MYINSTANCES (U032)

#define SURFKIND   U032
#define VBISURF (0xFEEDC0DE)
#define IMAGESURF   (0xBAD0BEEF)

#define STARTINGBUF (99)
#define UNKNOWNBUF  (123)


#define NOVPERROR               (0)
#define TOOMANYSURFACESERROR (20)
#define CANTCREATESURFACE       (21)
#define SURFACENOTFOUND     (0x9999)
#define NOSURFACEPROGRAMED  (0x999)
#define VPERRORS                U032

// Note that PROGRESSIVE is the currently defined to be ODD ODD
#define CAPTUREMODE             U032
#define EVENODD                 (1)
#define EVENEVEN                (2)
#define PROGRESSIVE             (3)
#define ODDODD                  (3)

#define SCANMODE                    U032
#define SEPERATEBUF             (10)
#define INTERLEAVBUF            (20)

// These constants are designed to correspond with the NV3 hardware.
#define FIELDTYPE               U032
#define ODD                     0
#define EVEN                        1
#define UNKNOWN                 3

#endif //DECENTENUMS

// the maximum frame period should be 20% more than 1/24 of a second measured in nanosecs
//    this is  1/24  * 1.2  * 1,000,000,000 = 50000000  = 0x2faf080
#define MAXFRAMEPERIOD          (0x2faf080)

//  There are 9 notifiers for the externalvideodecoder class
//    there are an additional  2 notifiers for the timer class
//    2 notifiers for the MTM class
//    1 notifier for the nvo3 sclaed image
//    1 notifier for the stretch swizzle class
#define  NUM04DNOTIFIER     (9)
#define  NUM004NOTIFIER    (2)
#define NUM039NOTIFIER      (2)
#define  NUM077NOTIFIER    (1)
#define  NUM052NOTIFIER    (1)
#define  NUM04DOFFSET       (0)
#define  NUM004OFFSET       (NUM04DOFFSET+NUM04DNOTIFIER)
#define  NUM039OFFSET       (NUM004OFFSET+NUM004NOTIFIER)

// for swizzler class
#define  NUM077OFFSET       (NUM039OFFSET+NUM039NOTIFIER)

// for swizzled surface
#define  NUM052OFFSET       (NUM077OFFSET+NUM077NOTIFIER)
#define NUMVPNOTIFIERS      (NUM052OFFSET+NUM052NOTIFIER)


// This little goodies is going to be used so that we can distinguish between
// two states which the architecture has traditionally considered to be ONE.
// these states are to distinguish between a notification where Status indicates
// that the HW is done, but the SW has not dealt with that even
// this is DONE_SUCESS
// the second state is HW done and SW dealt with the event, but no new
// task has been delivered back to the HW.    Theoretically this should
// be the power-on state.

#define NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED                     (0x42)


/* Debug stuff */
#ifndef RESMANSIDE
#ifdef DEBUG
    #define ASSERT(ParamAssert)     (ParamAssert)? \
            (void)0:DPF("Assertion failed in %s at line %i\n", __FILE__, __LINE__)

    #define strASSERT( Condition, string )  (Condition)?\
            (void)0:DPF("Assertion failed in %s at line %i, %s", __FILE__, __LINE__, strAssert)

#else
    #define ASSERT( ParamAssert )               (void)0
    #define strASSERT( Condition, string )      (void)0
#endif
#else
    #define ASSERT(x)  (void)0
    #define strASSERT(Condition,string)   (void)0
#endif

// this is the maximum for both VBI and IMAGE combined
#define MAXVPSURFACES   (10)

// used to timeout while's which are depedant on HW so which may never terminate
#define WHILETIMEOUT                        (0x800000)


// this next one can't conflict with MS defined things...
//#define VDD_IOCTL_SET_NOTIFY    0x10000001  // set mode change notify
//#define VDD_IOCTL_GET_DDHAL     0x10000002  // get DDHAL functions from miniVDD

#define VDD_IOCTL_NV_FILLTABLE   0x20000667

typedef struct _NVVDDPROCTABLE
{

    U032        dwPDDDriver;
    MINIPROC    getStoragePointer;
    MINIPROC handleBufferInterrupts;

} NVVDDPROCTABLE;

typedef struct _vpSurfaceInfo
{
    HANDLE              surfHandle;             // directDraw surface handle
    FLATPTR             pVidMem;                    // flat pointer to the surface
    U032                    dwOffset;               // offset from the base of the frame buffer
    U032                    dwPitch;
    U032                    dwLength;
    U032                    dwHeight;
    U032                    dwWidth;                    // ignored for VBI
    U032                    dwStartLine;            // programs Y-crop for image, and startline for vbi
    U032                    dwPreScaleSize;     // ignored for VBI.. represented in 16..16 format
    DWORD                   ddsCaps;                    // capabilites of this DD surface
    NvNotification*     pNvNotify;              // pointer to typical notification structure that was most recently used
    U032                    bAutoFlip;              // do we turn around and auto program this surface?
    U032                    nextSurfaceNum;     // which is the next surface to fire off?
    FIELDTYPE           field;                  // what field was this surface setup to capture?
    U032                    dwRes1;         // for storing the pointer to the "second" surface for the DVD subpicture stuff
} vpSurfaceInfo;

typedef struct _DropSys {
   U032 dwTargetSurface;
   U032 dwbIsBobFromInterleave;
   U032 dwFieldType;
   U032 dwFullBool;
   U032 dwDropFields;
} DropSys;


typedef struct _vpSurfaces {
    U032                    numberOfSurfaces;               // total number of surfaces that we're keeping track of
    vpSurfaceInfo*      Surfaces[MAXVPSURFACES];    // this will be a pointer to an array of surfaces
    U032                    CurrentBufferSurface[4];    // to associate the HW buffer with an arrary index
    CAPTUREMODE         CurrentCaptureMode;         // indicates which fields will be capture Even
    SCANMODE                CurrentScanMode;                // seperate or interleaved buffersx
    FIELDTYPE           lastField;                      // the last buffer programmed was to capture a field of this type
    FIELDTYPE           lastVBIField;                           //same but for VBI
    U032                    bInputInterLaced;               // TRUE if we're running with an interlaced input
    U032                    VBIstart;                       // which surface is the first one which has VBI data?
    // general information
    U032                    bSkipNextField;             // set to true to indicate that the next field should be skipped.  reset by upon skip
    U032                    dwInHeight;                    // input height (into the surface)
    U032                    dwInWidth;                      // input width (into the surface)
    U032                    dwImageStartLine;               // first line of image data to capture
    U032                    dwVBIStartLine;             // the first VBI line to capture
    U032                    dwVBIHeight;                    // total number of lines of VBI to capture
    U032                    dwPreHeight;                    // height and width before prescaling
    U032                    dwPreWidth;                     // width before prescaling
    U032                    dwOriginX;                      // orgins to move data within the surface
    U032                    dwOriginY;
    U032                    bThreadDead;                    // TRUE indicates that the thread has decided to exit
    U032                    bThreadRunning;             // if true, indicates that the thread should continue to run
//  LPVOID              hVPInterruptEvent;          // handle to the event used to notify the thread to wake up
//  DWORD                   VpInterruptId;                  // the thread ID of the thread
//  HANDLE              hThreadHandle;                  // Handle to the thread
    U032                    bStopVP;                            // indicates to the thread that it should NOT reprogram..
    HANDLE              myVXDhandle;                    // used to store the handle to access the mini-vdd

    U032                    CallbackSelector;
    U032                    pNotifyCallbackProcI0;
    U032                    pNotifyCallbackProcI1;
    U032                    pNotifyCallbackProcV0;
    U032                    pNotifyCallbackProcV1;
    U032                    pTimerNotifyProc;
    U032                    pMTMNotifyCallback;
    U032                    pNotifyOverlay0NotifyProc;
    U032                    pNotifyOverlay1NotifyProc;

    DDMINIVDDTABLE      myKMVTTable;                    // for storing the locations of the KVMT functions
    U032*                   pNvDevBase;
    U032                    bInvertedFields;
    // these are temporarly while I do information about the speed of things

    timeStore           Timings;
#if 0
    U032                    marker;
    U032                    storCount;
    U032                    mpFlipTime0[MAXDATASTORE];  // these next are for collection timing statistics
    U032                    mpFlipTime1[MAXDATASTORE];
    U032                    mpFlip[MAXDATASTORE];

    U032                    ovReqTime0[MAXDATASTORE];
    U032                    ovReqTime1[MAXDATASTORE];
    U032                    ovReq[MAXDATASTORE];

    U032                    ovFlipTime0[MAXDATASTORE];  // this should be in MicroSeconds
    U032                    ovFlipTime1[MAXDATASTORE];      // this should be in MicroSeconds
    U032                    ovFlip[MAXDATASTORE];

    U032                    surfPlace[MAXDATASTORE];    // a "packed" version of current surfaces
    U032                    mpLoopCount;
    U032                    ovLoopCount;
    U032                    ovFlLoopCount;
    U032                    loopCount;
#endif
    U032                    curImageSurf;
    U032                    curVBISurf;
    U032                    curSurf;
    U032                    intermediateCycle;

    FLATPTR             pBuffer0Mem;                    // to keep track of which surface is programmed in the VDD
    FLATPTR             pBuffer1Mem;
    U032                    curOverlay;
    NVVDDPROCTABLE      myTable;
    NvChannel*          pVPChanPtr;
    NvChannel*          pOverlayChanPtr;                // used to store the Pointer to the NV channel
                                                                // because of potential multithreading issues we try to keep the overlay in
                                                                // it's own channel.  However, we do a SetObject on it just incase before use
    U032                    dwOverlaySubChan;               // used to store the subchannel in which we'll talk to the Overlay
    U032                    dwOverlayObjectID;          // used to store the OverlayObject ID...
    U032                    dwScalerObjectID;
    U032                    dwDVDObjectID;
    NvNotification* pTheNotifiers;

    CALLBPROC           ImageCallbackRoutine;
    CALLBPROC           VBICallbackRoutine;
    sCallBack           myIMGCallbackInfo;
    sCallBack           myVBICallbackInfo;
    GLOBALDATA*         pDriverData;
    U032                    myFreeCount;
    // THESE are the big ugly so we dont' have to worry about which context the ring0 stuff was called from and there
    // where these guys were allocated from
    U032                    lastsrcDeltaY;
    U032                    surfMemAllocCount;
    vpSurfaceInfo       surfMemory[MAXVPSURFACES];  // this will be a pointer to an array of surfaces
    U032                    DXIRQCallbackSources;       // flags for the callback determination
    U032                    pDXIRQCallback;             // address of the callback proc for the DX stuff
    U032                    dwIrqContext;
    U032                    dwVPFlags;              // same flags as passed in to UpdateVideoPort
    VPSTATE             dwVideoPortStatus;  // yet another global silly
    U032                    bFlipRequested;         // used during non-auto-flip to determine if a flip occurred
    U032                    lastSurfaceFlippedTo;
    volatile U032*                  pNvTimer;

    aTransfer           MyTransfers[MAXTRANSFERS];  // array of transfers from which to allocate
    U032                    dwNextEmpty;    //Points to the location in the array of the next empty we should fill
    U032                    dwLastEmpty;    //Points to the location in the arry which was just emptied...
    //pTransfer         pLastTransfer;                  // the last one which was added
    pTransfer           pCurrentTransfer;               // the current one which just just put into the HW
    U032                    bContextCreated;                // has the context object been created or not
    U032                    savedStart;
    U032                    savedLimit;
    U032                    dwMostRecentFinishTransferId;  // used to store the ID of the most recently finished transfer
    U032                    SetupComplete;
    U032                    dwOvSurfaceOffset;
    U032                    lastSurface;            // bob-interleave field delay
    U032                    lastBuffer;             // bob-interleave field delay
    U032                    bDoingDropFrame;        // used to indicate that we just dropped a field and the leap frogging should stop for one frame
    VidTexSurf*         pVidTexSurf;
    U032                    bRing0Flip;
    DropSys                 DropSystem;
    U032                    lastOvLowTime;          // used to store the last overlay callback time to eliminate duplicates
    U032                    bFirstTime;           // used to indicate first time through on seperate buffer starts to eliminate an NV10 bug!
} vpSurfaces;




vpSurfaces* getStoragePointer(void );
DWORD SetupVP (LPDDHAL_CREATEVPORTDATA lpInput);
BOOL    GetField(void);
DWORD getCurLine(void);
void    tearDownVP(void);
void    StopVP(void);
U032    findSurface(LPDDRAWI_DDRAWSURFACE_LCL pNewSurface );
U032    ProgramBufferWithSurface(U032   dwBufferIndex,U032  newSurfaceIndex, U032 bZeroLength);
VPERRORS replaceSurfaceList(DWORD dwNumAutoflip,DWORD dwNumVBIAutoflip,
                                        LPDDRAWI_DDRAWSURFACE_INT   *lplpDDSurface,
                                        LPDDRAWI_DDRAWSURFACE_INT   *lplpDDVBISurface,
                                        U032 bAutoflip );

// used by surfaces.h internally
DWORD WINAPI VPInterruptThread(PVOID daParameter);

VPERRORS    addSurface(vpSurfaceInfo*   pNewSurface);
VPERRORS replaceSurfaceWithSurface(U032 surfaceToReplace,
                                        LPDDRAWI_DDRAWSURFACE_LCL pNewSurface,
                                        U032  bVBI);

vpSurfaceInfo* createSurface(LPDDRAWI_DDRAWSURFACE_INT pNewSurface, U032 bAutoflip,
                                        U032 bVBI, NvNotification* pToNotify,U032 nextSurface);
VPERRORS clearAllSurfaces();

U032    getNextSurface(U032 currentSurface, U032 bSingleAdvance);
U032    getPrevSurface(U032 currentSurface);
U032    getPrevOverlaySurface(U032 targetSurface);
void    getCurrentNanoTimer(U032*   Most, U032* Least);

U032    HookUpVxdStorage(void);


#define INIT_FREE_COUNT(a) unsigned long freeCount = a;

#ifdef NV3
#define CHECK_FREE_COUNT(a,b)                       \
    ASSERT(b < NV_GUARANTEED_FIFO_SIZE );           \
    while (freeCount < b)                               \
        freeCount = NvGetFreeCount(a, 0);           \
    freeCount -= b;
#else
#define CHECK_FREE_COUNT(a,b)                       \
    ASSERT(b < NV06A_FIFO_GUARANTEED_SIZE );            \
    while (freeCount < b)                               \
        freeCount = NvGetFreeCount(a, 0);           \
    freeCount -= b;

#endif


#ifndef VPE_NT      // this is for win9x

#ifndef INVDDCODE
#define  GLOBDATAPTR         ((GLOBALDATA*)(pMySurfaces->pDriverData))
#else
#define  GLOBDATAPTR         ((GLOBALDATA*)(MySurfaces.pDriverData))
#endif  // INVDDCODE

#define  PBASEADDRESS    BaseAddress
#define  DWFBUFFERLEN    (GLOBDATAPTR->VideoHeapEnd - GLOBDATAPTR->BaseAddress)
#define  DEVHANDLE       dwDeviceHandle
#define  ROOTHANDLE      dwRootHandle
#define  CLIENTHANDLE    hClient

#else           // this is for NT
#define  GLOBDATAPTR        ((PDEV*)(pMySurfaces->pDriverData))
#define  PBASEADDRESS    pjFrameBufbase
#define  DWFBUFFERLEN    cbFrameBuf - 1

#define  DEVHANDLE       hDevice
#define  ROOTHANDLE      hDriver
#define  CLIENTHANDLE    hClient
#define

#endif


#endif //surfaces_h
