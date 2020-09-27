#ifndef _OS_H_
#define _OS_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: OS.H                                                              *
*       Operating system wrapper functions used to abstract the OS.         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    06/22/94 - rewrote it.                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Debug macros.
//
//---------------------------------------------------------------------------

#if DBG

/*
#pragma warning(disable:4311)   // allow type cast truncation
#pragma warning(disable:4242)   // allow 
#pragma warning(disable:4244)   // allow 
#pragma warning(disable:4047)   // allow 
*/

// Debug level values
#define DEBUGLEVEL_TRACEINFO    0     // For informational debug trace info
#define DEBUGLEVEL_SETUPINFO    1     // For informational debug setup info
#define DEBUGLEVEL_USERERRORS   2     // For debug info on app level errors
#define DEBUGLEVEL_WARNINGS         3     // For RM debug warning info
#define DEBUGLEVEL_ERRORS           4     // For RM debug error info

VOID    osDbgBreakPoint                 (void);
VOID    osDbgPrintString                (int, char*);
VOID    osDbgPrintCR                    (int);
VOID    osDbgPrintValue                 (int, int);
VOID    osDbgPrintStringValue   (int, char*, int);
VOID    osDbgPrintStringDec             (int, char*, U032);
VOID    osDbgPrintStringPtr             (int, char*, VOID*);
VOID    osDbgPrintStringVal64   (int, char*, U064);

extern int cur_debuglevel;
#define DBG_BREAKPOINT()                        osDbgBreakPoint()
#define DBG_PRINT_STRING(d,s)                   osDbgPrintString((d),(s))
#define DBG_PRINT_VALUE(d,v)                    osDbgPrintValue((d),(v))
#define DBG_PRINT_CR(d,v)                               osDbgPrintCR((d))
#define DBG_PRINT_STRING_VALUE(d,s,v)   osDbgPrintStringValue((d),(s),(v))
#define DBG_PRINT_STRING_DEC(d,s,v)             osDbgPrintStringDec((d),(s),(v))
#define DBG_PRINT_STRING_PTR(d,s,v)             osDbgPrintStringPtr((d),(s),(v))
#define DBG_PRINT_STRING_VAL64(d,s,v)   osDbgPrintStringVal64((d),(s),(v))

static U032 DbgLoop;
#define DBG_MAX_LOOP        1000
#define DBG_INIT_LOOP       DbgLoop = 0;
#define DBG_CHECK_LOOP      if (++DbgLoop > DBG_MAX_LOOP) DBG_BREAKPOINT()
#define DBG_VAL_PTR(p)      {if (*((U032*)(p) - 1) != NV_MARKER1){           \
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Invalid pointer!\n\r"); \
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, __FILE__); \
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, ": line "); \
                                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)__LINE__); \
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r"); \
                                DBG_BREAKPOINT();}                              \
                             else if (*(U032*)((U008*)(p) + *((U032*)(p) - 2) - 12) != NV_MARKER2){\
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Trashed buffer end!\n\r");\
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, __FILE__); \
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, ": line "); \
                                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)__LINE__); \
                                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r"); \
                                DBG_BREAKPOINT();}}
#define DBG_VAL_PATCH(c,p)                                                                                                                                                                            \
    {if (((c) != INVALID_CHID) && (p)                                                                                                                                                                 \
      && (((PROPOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UROP))]      && (grTable[(c)].CurrentRop       != (PROPOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UROP))]))        \
      ||  ((PBETAOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UBETA))]    && (grTable[(c)].CurrentBeta      != (PBETAOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UBETA))]))      \
      ||  ((PSOLIDOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UCHROMA))] && (grTable[(c)].CurrentColorKey  != (PSOLIDOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UCHROMA))]))   \
      ||  ((PSOLIDOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UPLANE))]  && (grTable[(c)].CurrentPlaneMask != (PSOLIDOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UPLANE))]))    \
      ||  ((PCLIPOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UCLIP))]    && (grTable[(c)].CurrentClip      != (PCLIPOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UCLIP))]))      \
      ||  ((PPATTERNOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UPATT))] && (grTable[(c)].CurrentPattern   != (PPATTERNOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UPATT))])))) \
     {                                                                                                                                                                                                \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid Patch State on Channel: ", (c));                                                                                                                       \
         DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Current ROP: ");                                                                                                                                                           \
         DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)grTable[(c)].CurrentRop);                                                                                                                                                    \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Patch ROP: ", (int)(PROPOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UROP))]);                                                                                \
         DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Current BETA: ");                                                                                                                                                          \
         DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)grTable[(c)].CurrentBeta);                                                                                                                                                   \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Patch BETA: ", (int)(PBETAOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UBETA))]);                                                                             \
         DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Current ColorKey: ");                                                                                                                                                      \
         DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)grTable[(c)].CurrentColorKey);                                                                                                                                               \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Patch ColorKey: ", (int)(PSOLIDOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UCHROMA))]);                                                                      \
         DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Current PlaneMask: ");                                                                                                                                                     \
         DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)grTable[(c)].CurrentPlaneMask);                                                                                                                                              \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Patch PlaneMask: ", (int)(PSOLIDOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UPLANE))]);                                                                      \
         DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Current Clip: ");                                                                                                                                                          \
         DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)grTable[(c)].CurrentClip);                                                                                                                                                   \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Patch Clip: ", (int)(PCLIPOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UCLIP))]);                                                                             \
         DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Current Pattern: ");                                                                                                                                                       \
         DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int)grTable[(c)].CurrentPattern);                                                                                                                                                \
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Patch Pattern: ", (int)(PPATTERNOBJECT)(p)->UserObjects[GR_DEVICE_INDEX(DEVICE_BASE(NV_UPATT))]);                                                                       \
         DBG_BREAKPOINT();                                                                                                                                                                            \
     }}
#else 

// no debugging -- define out debug macros
#define DBG_BREAKPOINT()
#define DBG_PRINT_STRING(d,s)
#define DBG_PRINT_VALUE(d,v)
#define DBG_PRINT_CR(d,v)
#define DBG_PRINT_STRING_VALUE(d,s,v)
#define DBG_PRINT_STRING_DEC(d,s,v)
#define DBG_PRINT_STRING_PTR(d,s,v)
#define DBG_PRINT_STRING_VAL64(d,s,v)
#define DBG_INIT_LOOP
#define DBG_CHECK_LOOP
#define DBG_VAL_PTR(p)
#define DBG_VAL_PATCH(c,p)

static __inline VOID osDbgPrintString(int level, char* str) {}
static __inline VOID osDbgPrintStringValue(int level, char* str, int val) {}

#endif // DBG

// registry path for RM AGP lookups
#define RM_REGISTRY_AGP_BASE_PATH    strDevNodeRM

#ifndef toupper
#define toupper(c)  (((c)>='a'&&(c)<='z')?((c)-'a'+'A'):(c))
#endif

// data types necessary for accessing binary registry data
// this seems to be what we get back on REG_BINARY -- couldn't find it documented tho
typedef struct
{
    U032 Length;
    U032 type;
    U008 buffer[1];
    
} RM_REG_BINARY_RECORD;
typedef union
{
    RM_REG_BINARY_RECORD record;
    U008 buffer[256];
    
} RM_REG_BINARY_BUFFER;

/* polymorphic 64-bit macros */
#ifdef IA64
typedef NvU64 SIZE_PTR; 
#else
typedef NvU32 SIZE_PTR;
#endif // IA64

#define PHYSICAL_ADDRESS_VALUE(p) ((p).LowPart)

typedef struct _def_winnt_hw_info
{
        U016    filler1;
        U008    filler2;

        U008    oldIrql;
        BOOL    rmInitialized;          // per device init flag
        VOID*   pRmSpinLock;
        VOID*   AgpServices;            // win2k's AGP services func ptrs
        VOID*   dmaAdapter;                     // PVP_DMA_ADAPTER for 64 bit systems

} WINNTHWINFO, *PWINNTHWINFO;

#if (_WIN32_WINNT >= 0x0500)

// System memory allocation tracking (for _WIN64)

typedef struct _def_sys_mem_info
{
    NvU64   physAddr;           // aligned adaper address
    VOID*   pMdl;               // MDL for user mapping
    VOID*   sysAddr;            // original, potentially unaligned address
    NvU64   logicalAddr;        // address to be used by adapter
    NvU32   length;             // actual number of bytes allocated
    NvU32   clientClass;        // NV01_ROOT_USER or NV01_ROOT
} SYS_MEM_INFO, *PSYS_MEM_INFO;

#endif

//---------------------------------------------------------------------------
//
//  Function prototypes for OS interface.
//
//---------------------------------------------------------------------------

// macro to allow different syntax between NT and Win9x
#define OS_READ_REGISTRY_DWORD(pDev, a, b, c) osReadRegistryDword(pDev, a, b, c)

RM_STATUS       osCopyIn                                (U008 *src, U008 *target, U032 count);
RM_STATUS       osCopyOut                               (U008 *src, U008 *target, U032 count);
RM_STATUS       osAllocMem                              (VOID **, U032);
RM_STATUS       osFreeMem                               (VOID *);
RM_STATUS       osAllocPages                    (PHWINFO, VOID **, U032, U032, U032, U032, U032, VOID **);
RM_STATUS       osFreePages                             (PHWINFO, VOID **, U032, U032, VOID *, BOOL);
RM_STATUS   osAllocSystemPages      (PHWINFO, VOID **, U032, U032, VOID **);
RM_STATUS   osFreeSystemPages       (PHWINFO, VOID **, VOID *);
RM_STATUS   osAllocAGPPages         (PHWINFO, VOID **, U032, U032, VOID **);
RM_STATUS   osFreeAGPPages          (PHWINFO, VOID **, VOID *);
RM_STATUS       osAllocDmaListElement   (PDMAUSEROBJECT*);
RM_STATUS       osFreeDmaListElement    (PDMAUSEROBJECT);
RM_STATUS   osVirtualToPhysicalAddr (VOID*, VOID**);
RM_STATUS   osCalculatePteAdjust    (PHWINFO, U032, VOID*, U032*);
RM_STATUS       osGetAddressInfo                (PHWINFO, U032, U032, VOID**, U032, VOID**, U032 *);
RM_STATUS       osLockUserMem                   (PHWINFO, U032, U032, VOID*, VOID*, U032 *, U032, U032, U032 *, VOID**, VOID**);
RM_STATUS       osUnlockUserMem                 (PHWINFO, U032, VOID*, VOID*, U032, U032, U032, VOID*, U032);
RM_STATUS       osSignalSemaphore               (U032 *);
RM_STATUS       osWaitSemaphore                 (U032 *);
RM_STATUS       osGetCurrentTime                (U032 *, U032 *, U032 *, U032 *, U032 *, U032 *, U032 *);
RM_STATUS       osError                                 (POBJECT, U032, V032, RM_STATUS);
RM_STATUS       osDelay                                 (U032);
RM_STATUS       osRedrawScreen                  (VOID);
RM_STATUS       osBeginWatchCanvas              (U032);
RM_STATUS       osEndWatchCanvas                (U032);
RM_STATUS       osValidateVideoSink             (char *);
RM_STATUS       osValidateImageVideo    (char *, U032 *);
RM_STATUS       osValidateDMAObject             (char *, U032, U032 *);
char*           osStringCopy                    (char *, const char *);
U032            osStringLength                  (const char *);
unsigned char * osMemCopy                       (unsigned char *, const unsigned char *, U032);
RM_STATUS       osMapPciMemoryUser              (PHWINFO, U032, U032, VOID**);
RM_STATUS       osUnmapPciMemoryUser    (PHWINFO, VOID*);
RM_STATUS       osMapFrameBuffer                (PHWINFO, U032, U032, U032, VOID **);
RM_STATUS       osMapInstanceMemory             (PHWINFO, U032, U032, U032, VOID **);
RM_STATUS       osMapFifo                               (PHWINFO, U032, U032, VOID **);
RM_STATUS       osUnmapMemory                   (PHWINFO, U032, VOID *, U032);
RM_STATUS   osSetFifoDmaParams      (PHWINFO, U032 *, U032 *, U032 *);
S032            osStringCompare                 (const char *, const char *);
RM_STATUS       osGetCurrentProcess             (U032*);
VOID            osWriteReg008                   (volatile V008*, V008);
VOID            osWriteReg016                   (volatile V016*, V016);
VOID            osWriteReg032                   (volatile V032*, V032);
U008            osReadReg008                    (volatile V008*);
U016            osReadReg016                    (volatile V016*);
U032            osReadReg032                    (volatile V032*);
RM_STATUS       osReadRegistryDword             (PHWINFO, char *, char *, U032 *);
RM_STATUS       osWriteRegistryDword    (PHWINFO, char*, char*, U032);
RM_STATUS       osReadRegistryString    (char*, char*, char*str, U032 *);
RM_STATUS       osWriteRegistryString   (char*, char*, char* str, U032);
RM_STATUS       osReadRegistryBinary    (PHWINFO, char*, char*, U008*, U032 *);
RM_STATUS       osWriteRegistryBinary   (PHWINFO, char*, char*, U008*, U032);
RM_STATUS       osDeviceNameToDeviceClass(char*, U032*);
U032            osGetCpuFrequency               (VOID);
U032        osPciReadDword          (U032, U032);
U016        osPciReadWord           (U032, U032);
U008        osPciReadByte           (U032, U032);
VOID        osPciWriteDword         (U032, U032, U032);
VOID        osPciWriteWord          (U032, U032, U016);
VOID        osPciWriteByte          (U032, U032, U008);
U032        osPciInitHandle         (U008, U008, U008, U016*, U016*);
RM_STATUS       osUpdateAGPLimit                (PHWINFO);

RM_STATUS   osIsr                   (PHWINFO);
RM_STATUS   osMemSet                (VOID *, U032, U032);

// Win2K interrupt handling
#if (_WIN32_WINNT >= 0x0500)
VOID        RmDpcForIsr             (VOID*, VOID*);
#endif

// Win2K processor synchronization
#if (_WIN32_WINNT >= 0x0500)

RM_STATUS   osInitSpinLock          (PHWINFO);
RM_STATUS   osEnterCriticalCode     (PHWINFO);
RM_STATUS   osExitCriticalCode      (PHWINFO);

#else

// define away Win2K only macros
#define osInitSpinLock(p) RM_OK
#define osEnterCriticalCode(p)
#define osExitCriticalCode(p)

#endif

#if defined(_WIN64)

RM_STATUS   osInitDmaAdapter        (PHWINFO);

#else

#define osInitDmaAdapter(p)         RM_OK

#endif

// Define away win9x only functions
#define         osEnsureDeviceEnabled(pDev) 
#define         osExit(s)                               osDbgPrintStr("NVRM: "); osDbgPrintStr((s)); osDbgPrintStr("\n"); return RM_ERROR       
#define         osStateEx(pDev, msg)
#define         osStateDacEx(pDev, msg)
#define     osDisplayModeHook(pDev, msg)
#define     osmpCheckCallbacks(pDev, dwCallbackSelect)
#define     osPreModeSet(pDev, head)
#define     osPostModeSet(pDev, head)

extern BOOL dacMonitorConnectStatus(PHWINFO, U032);
extern BOOL dacFlatPanelConnectStatus(PHWINFO, U032);
VOID        osPostModeSetEx         (PHWINFO, U032);
VOID        osPreModeSetEx          (PHWINFO, U032);

#define     OEMEnableExtensions()
#define     OEMDisableExtensions()

VOID            osLogInstMemAlloc               (U032, U032, U032);
VOID            osUnlogInstMemAlloc             (U032, U032);

// functions needed for nvagp
/* remap IO memory to kernel space */
void *osMapKernelSpace(unsigned long start, unsigned long size, U008 cached);
void osUnmapKernelSpace(void *addr, unsigned long size);

/* remap memory to user space */
void *osMapUserSpace(void *kaddr, void **priv, unsigned int size_bytes, U008 cached);
void osUnmapUserSpace(void *uaddr, void *priv);

// like osAllocMem, but no debugging info written into pages
RM_STATUS osAllocPool(void **addr, unsigned int size_bytes);
RM_STATUS osFreePool(void *addr);

void *osAllocContigPages(unsigned long num_pages);
void osFreeContigPages(void *addr); 
U032 osGetKernPhysAddr(void *kern_virt_addr);
RM_STATUS osGetAGPPhysAddr(PHWINFO pDev, VOID *vaddr, U032 *paddr);


// these are in modehw.h which is in the miniport
VOID WriteIndexed(U032 addr, U016 datum);
U016 ReadIndexed(U032 addr, U008 reg);
VOID WritePriv32(U032 datum, U032 addr);
U032 ReadPriv32(U032 addr);
VOID WritePriv08(U032 addr, U008 datum);

VOID            osStateFbEx                             (PHWINFO);
RM_STATUS       osSetVideoMode                  (PHWINFO);
RM_STATUS       osNotifyEvent                   (PHWINFO, POBJECT, U032, U032, U032, RM_STATUS, U032);
RM_STATUS       initMapping                             (PHWINFO);
RM_STATUS       initFbEx                                (PHWINFO);
RM_STATUS       initFifoEx                              (PHWINFO);
RM_STATUS       initGrEx                                (PHWINFO);
RM_STATUS       initDacEx                               (PHWINFO);
RM_STATUS       initCodecEx                             (PHWINFO);
BOOL        RmInitAGP               (PHWINFO);
VOID        NvUpdateAGPConfig       (PHWINFO);
RM_STATUS   NvGetAGPBaseLimit       (PHWINFO, U032 *, U032 *);

#define PENDING_INTERRUPTS \
    (DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING)|DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING))

VOID            RmEnableInterrupts              (PHWINFO);
VOID            RmDisableInterrupts             (PHWINFO);
BOOL            RmInterruptPending              (PHWINFO, BOOL*);

#if DBG
VOID osLogMemAlloc(VOID *address, U032 size);
VOID osUnlogMemAlloc(VOID *address);
#endif // DBG

//---------------------------------------------------------------------------
//
//  Required defines for OS interface.
//
//---------------------------------------------------------------------------

#define NV_MEMORY_TYPE_SYSTEM       0
#define NV_MEMORY_TYPE_AGP          1

#define NV_MEMORY_NONCONTIGUOUS     0
#define NV_MEMORY_CONTIGUOUS        1

#define NV_MEMORY_DEFAULT           0
#define NV_MEMORY_UNCACHED          1
#define NV_MEMORY_WRITECOMBINED     2
#define NV_MEMORY_WRITETHRU         3
#define NV_MEMORY_WRITEPROTECT      4
#define NV_MEMORY_WRITEBACK         5

/////////////////////////////////
#define DPC_IMPLEMENTATION 1
/////////////////////////////////


typedef struct
{
    U016  HdrSize;
    U016  InfoFlags;
    U032  DevNodeHandle;
    U008  DriverName[16];
    U016  XRes;
    U016  YRes;
    U016  DPI;
    U008  Planes;
    U008  Bpp;
    U016  RefreshRateMax;
    U016  RefreshRateMin;
    U016  LowHorz;
    U016  HighHorz;
    U016  LowVert;
    U016  HighVert;
    U032  MonitorDevNodeHandle;
    U008  HorzSyncPolarity;
    U008  VertSyncPolarity;
} DISPLAYINFO, *PDISPLAYINFO;

// DMA list element pool
#define DMALISTELEMENT_POOL_SIZE 16
typedef struct
{
        BOOL inUse;
        DMAUSEROBJECT dmaListElement;

} DMALISTELEMENT_POOL_ELEMENT, *PDMALISTELEMENT_POOL_ELEMENT;

// more globals
extern DISPLAYINFO osDisplayInfo;
extern PDMALISTELEMENT_POOL_ELEMENT dmaListElementPool;


// NT callbacks...

// notify callback action
#define NV_OS_WRITE_THEN_AWAKEN    0x00000001

// This is callback function in the miniport.
// The argument is a device extension, and must be cast as such to be useful.
typedef VOID (*MINIPORT_CALLBACK)(VOID*);


// memory allocation
#define NV_MEMORY_TAG '  VN'

// AGP macros
#define BYTES_MAPPED_PER_PAGE   ((RM_PAGE_SIZE >> 2) * RM_PAGE_SIZE)
#define MAP_WRITECOMBINE   0x2


#endif // _OS_H_
