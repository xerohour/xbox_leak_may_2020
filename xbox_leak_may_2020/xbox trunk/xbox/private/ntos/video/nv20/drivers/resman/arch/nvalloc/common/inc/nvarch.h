#ifndef _NVARCH_H
#define _NVARCH_H
/**************************************************************************************************************
*
*    File:  nvarch.h
*
*    Description:
*        Defines the functions that implement the nvalloc architecture
*
**************************************************************************************************************/


// macros for IOCTL argument passing and return values
#define ARGS_PTR(t) t *pArgs = (t*)parameters    // declare and define a pointer to the arg struct (arg struct type)
#define ARG(a) (pArgs->a)                         // reference to a member of the arg struct (element name)
#define ARG_P064(a) NvP64_VALUE(pArgs->a)         // extraction of a pointer from a NvP64 in the arg struct (element name)
#define ARG_SELECTOR(a) NvP64_SELECTOR(pArgs->a) // extraction of a selector from a NvP64 in the arg struct (element name)
#define ARG_U064(a) NvU64_VALUE(pArgs->a)        // extraction of a value from a NvU64 in the arg struct (element name)

// macros for error checking
#define RM_SUCCESS(s) ((s) == RM_OK)

// TO DO: clean this up by retrieving values from the registry
// display info constants and structure taken from win95's vdd.h
#define REGISTRY_BPP_NOT_VALID          0x0008
#define MONITOR_INFO_NOT_VALID          0x0080
#define MONITOR_INFO_DISABLED_BY_USER   0x0100

//---------------------------------------------------------------------------
//
//  External variable definitions.
//
//---------------------------------------------------------------------------

extern PCLIENTINFO clientInfo;

extern U032         rmInService;
extern U032         osInService;
extern U032         rmInSafeService;
extern U032         rmInCallback;
extern U032       * rmStackBase;
extern U032       * rmStack;
extern U032         vmmStack;

//---------------------------------------------------------------------------
//
// Function prototypes.
//
//---------------------------------------------------------------------------

// discardable initialization routines
RM_STATUS   initSemaphore           (VOID);
RM_STATUS   initCallbackFreeList    (VOID);
RM_STATUS   initClientInfo          (VOID);
RM_STATUS   destroyClientInfo       (VOID);
RM_STATUS   initMemoryAllocTable    (VOID);

// exported API functions
VOID        Nv01AllocRoot           (NVOS01_PARAMETERS*);
VOID        Nv01AllocDevice         (NVOS06_PARAMETERS*);
VOID        Nv01AllocContextDma     (NVOS03_PARAMETERS*);
VOID        Nv01AllocChannelPio     (NVOS04_PARAMETERS*);
VOID        Nv03AllocChannelDma     (NVOS07_PARAMETERS*);
VOID        Nv01AllocMemory         (NVOS02_PARAMETERS*);
VOID        Nv01AllocObject         (NVOS05_PARAMETERS*);
VOID        Nv04Alloc               (NVOS21_PARAMETERS*);
VOID        Nv01Free                (NVOS00_PARAMETERS*);
VOID        Nv03DmaFlowControl      (NVOS08_PARAMETERS*);
VOID        Nv01Interrupt           (NVOS09_PARAMETERS*);
VOID        Nv01AllocEvent          (NVOS10_PARAMETERS*);
VOID        Nv03ArchHeap            (NVOS11_PARAMETERS*);
VOID        Nv01ConfigVersion       (NVOS12_PARAMETERS*);
VOID        Nv01ConfigGet           (NVOS13_PARAMETERS*);
VOID        Nv01ConfigSet           (NVOS14_PARAMETERS*);
VOID        Nv01ConfigUpdate        (NVOS15_PARAMETERS*);
VOID        Nv01DebugControl        (NVOS20_PARAMETERS*);
VOID        NvRing0Callback         (NVRM_RING0CALLBACK_PARAMS*);
VOID        Nv04ConfigGetEx         (NVOS_CONFIG_GET_EX_PARAMS*);
VOID        Nv04ConfigSetEx         (NVOS_CONFIG_SET_EX_PARAMS*);
VOID        Nv04I2CAccess           (NVOS_I2C_ACCESS_PARAMS*);

// RM support functions
NvV32       RmAllocClient           (U032*, U032);
NvV32       RmAllocDevice           (U032, U032, U032, char*);
NvV32       RmAllocContextDma       (U032, U032, U032, U032, U032, VOID*, U032);
NvV32       RmAllocChannelPio       (U032, U032, U032, U032, U032, VOID**, U032);
NvV32       RmAllocChannelDma       (U032, U032, U032, U032, U032, U032, U032, VOID**);
RM_STATUS   RmAllocChannel          (U032, U032, U032, U032, U032, U032, U032, VOID**, U032);
NvV32       RmAllocMemory           (U032, U032, U032, U032, U032, VOID**, U032*);
RM_STATUS   RmAllocSystemMemory     (PHWINFO, U032, VOID**, U032*, VOID**, U032);
RM_STATUS   RmAllocInstanceMemory   (PHWINFO, U032, VOID**, U032*, U032*, U032*);
RM_STATUS   RmAllocFrameBufferMapping(PHWINFO, U032, VOID**, U032*);
NvV32       RmAllocObject           (U032, U032, U032, U032);
NvV32       RmAllocEvent            (U032, U032, U032, U032, U032, U064);
NvV32       RmAlloc                 (U032, U032, U032, U032, VOID*);
RM_STATUS   RmAllocObjectEx         (U032, U032, U032, VOID*);
NvV32       RmFree                  (U032, U032, U032);
RM_STATUS   RmFreeClient            (U032);
RM_STATUS   RmFreeDevice            (U032, U032);
RM_STATUS   RmFreeContextDma        (U032, U032);
RM_STATUS   RmFreeChannel           (U032, U032, U032);
RM_STATUS   RmFreeMemory            (U032, U032, U032, U032);
RM_STATUS   RmFreeSystemMemory      (PHWINFO, U032);
RM_STATUS   RmFreeSystemMemoryWithArgs(PHWINFO, VOID**, U032, VOID*, U032);
RM_STATUS   RmFreeInstanceMemory    (PHWINFO, U032, U032, U032);
RM_STATUS   RmFreeFrameBuffer       (PHWINFO, U032, U032, U032);
RM_STATUS   RmFreeObject            (U032, U032);
RM_STATUS   RmFreeEvent             (U032, U032);
NvV32       RmDmaChannelWritePut    (U032, U032);
NvV32       RmDmaChannelWritePutWithGet(U032, U032, U032);
NvV32       RmDmaChannelReadGet     (U032, U032*);
NvV32       RmDmaChannelSetJump     (U032, U032);
NvV32       RmInterrupt             (U032, U032);
NvV32       RmArchHeap              (U032, U032, U032, U032, U032, U032, U032, U032, S032*, U032*, U032*, VOID**, U032*, U032*, U032*);
NvV32       RmConfigVersion         (U032, U032, U032*);
NvV32       RmConfigGet             (U032, U032, U032, U032*);
NvV32       RmConfigSet             (U032, U032, U032, U032*, U032);
NvV32       RmConfigUpdate          (U032, U032, U032);
NvV32       RmConfigGetEx           (U032, U032, U032, VOID*, U032);
NvV32       RmConfigSetEx           (U032, U032, U032, VOID*, U032);
RM_STATUS   RmAllocDeviceInstance   (U032*);
NvV32       RmArchStatus            (RM_STATUS, U032);
VOID        RmInitCpuInfo           (PHWINFO);
VOID        RmInitBusInfo           (PHWINFO);
NvV32       RmI2CAccess             (U032, U032, VOID*);
NvV32       RmDirectMethodCall      (U032, U032, U032, U032, U032);
RM_STATUS   RmDebugEx               (U032, U032, U032, VOID*);
RM_STATUS   RmSwapInit              (PHWINFO);
RM_STATUS   RmSwapExt               (PHWINFO, U032, U032, U032, U032, U032, U032);
RM_STATUS   RmSwapClient            (PHWINFO, U032, U032);
RM_STATUS   RmSwapClientGone        (U032);

#endif // _NVARCH_H
