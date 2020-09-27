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

// debug module, levels, and defines
#include <nv_dbg.h>


// registry path for RM lookups
#define RM_REGISTRY_AGP_BASE_PATH   NV4_REG_GLOBAL_BASE_PATH "\\" NV4_REG_RESOURCE_MANAGER_SUBKEY

#ifndef toupper
#define toupper(c)  (((c)>='a'&&(c)<='z')?((c)-'a'+'A'):(c))
#endif

// correct type to use to convert a pointer to an integer
typedef U032 SIZE_PTR;

//---------------------------------------------------------------------------
//
//  Function prototypes for OS interface.
//
//---------------------------------------------------------------------------

// macro to allow different syntax between NT and Win9x
#define OS_READ_REGISTRY_DWORD(pDev, a, b, c) osReadRegistryDword(pDev, a, b, c)

RM_STATUS  osCopyIn(U008 *src, U008 *target, U032 count);
RM_STATUS  osCopyOut(U008 *src, U008 *target, U032 count);
RM_STATUS  osAllocMem(VOID **, U032);
RM_STATUS  osFreeMem(VOID *);
RM_STATUS  osAllocPages(PHWINFO,VOID **, U032, U032, U032, U032, U032, VOID **);
RM_STATUS  osFreePages(PHWINFO,VOID *, U032, U032, VOID *, BOOL);
RM_STATUS  osVirtualToPhysicalAddr(VOID*, VOID**);
RM_STATUS  osCalculatePteAdjust( PHWINFO, U032, VOID*, U032 *);
RM_STATUS  osGetAddressInfo(PHWINFO, U032, U032, VOID**, U032, VOID**, U032 *);
RM_STATUS  osLockUserMem(PHWINFO, U032, U032, VOID*, VOID*, U032 *, U032, U032, U032 *, VOID**, VOID**);
RM_STATUS  osUnlockUserMem(PHWINFO, U032, VOID*, VOID*, U032, U032, U032, VOID*, U032);
RM_STATUS  osGetCurrentTime(U032 *, U032 *, U032 *, U032 *, U032 *, U032 *, U032 *);
RM_STATUS  osError(POBJECT, U032, V032, RM_STATUS);
RM_STATUS  osExit(char *);
RM_STATUS  osDelay(U032);
RM_STATUS  osDelayUs(U032);
RM_STATUS  osUpdateAGPLimit(PHWINFO);

#ifdef OBSOLETE_FUNCTIONS
RM_STATUS  osRedrawScreen(VOID);
RM_STATUS  osBeginWatchCanvas(U032);
RM_STATUS  osEndWatchCanvas(U032);
RM_STATUS  osValidateGameportSource(char *, U032 *, U032 *, U032 *);
RM_STATUS  osValidateAudioSink(char *, U032 *);
RM_STATUS  osValidateAudioSource(char *, U032 *, U032 *);
RM_STATUS  osValidateVideoSink(char *);
RM_STATUS  osValidateImageVideo(char *, U032 *);
RM_STATUS  osValidateDMAObject(char *, U032, U032 *);
#endif // OBSOLETE_FUNCTIONS

char*      osStringCopy(char *, const char *);
S032       osStringCompare( const char *s1, const char *s2);
U032       osStringLength(const char *);
unsigned char * osMemCopy(unsigned char *, const unsigned char *, U032);
RM_STATUS  osReadRegistryDword(PHWINFO,char *, char *, U032 *);
RM_STATUS  osWriteRegistryDword(PHWINFO,char *, char *, U032);
RM_STATUS  osReadRegistryBinary(PHWINFO,char *, char *, U008 *, U032 *);
RM_STATUS  osWriteRegistryBinary(PHWINFO,char *, char *, U008 *, U032);
RM_STATUS  osDeleteRegistryValue(char *, char *);
RM_STATUS  osMapFrameBuffer(PHWINFO, U032, U032, U032, VOID **);
RM_STATUS  osMapInstanceMemory(PHWINFO, U032, U032, U032, VOID **);
RM_STATUS  osMapFifo(PHWINFO, U032, U032, VOID **);
RM_STATUS  osUnmapMemory(PHWINFO, U032, VOID *, U032);
RM_STATUS  osSetFifoDmaParams(PHWINFO, U032 *, U032 *, U032 *);
RM_STATUS  osDeviceNameToDeviceClass(char *, U032 *);

U032       osGetCpuFrequency(VOID);
U032       osPciReadDword(VOID *, U032);
U016       osPciReadWord(VOID *, U032);
U008       osPciReadByte(VOID *, U032);
VOID       osPciWriteDword (VOID *, U032, U032);
VOID       osPciWriteWord(VOID *, U032, U016);
VOID       osPciWriteByte(VOID *, U032, U008);
VOID*      osPciInitHandle(U008, U008, U008, U016*, U016*);
U016       ReadIndexedpDev(PHWINFO, U032, U008);

VOID       osEnsureDeviceEnabled(PHWINFO);
VOID       osmpCheckCallbacks(PHWINFO, U032);
VOID       osDisplayModeHook(PHWINFO, U032);
VOID       osStateDacEx(PHWINFO, U032);
VOID       osPreModeSet(PHWINFO, U032);
VOID       osPostModeSet(PHWINFO, U032);
VOID       osSetupForNewStyleModeSet(PHWINFO);

VOID 	   osStateFbEx(PHWINFO);
RM_STATUS  osSetVideoMode(PHWINFO);
RM_STATUS  osNotifyEvent(PHWINFO, POBJECT, U032, U032, U032, RM_STATUS, U032);
VOID       NvUpdateAGPConfig(PHWINFO);
RM_STATUS  NvGetAGPBaseLimit(PHWINFO, U032 *, U032 *);
RM_STATUS  mmWaitSemaphore(VOID);
RM_STATUS  mmSignalSemaphore(VOID);
RM_STATUS  initStack_frame(PHWINFO);

RM_STATUS  osCallVideoBIOS(PHWINFO, U032 *, U032 *, U032 *, U032 *, VOID *);
VOID       RmEnableInterrupts(PHWINFO);

#define    osDisableHardwareInterrupts(pDev)    _asm cli
#define    osEnableHardwareInterrupts(pDev)     _asm sti

// These were in modehw.h. There is no real reason these should be os dependent. However,
// in NT these are defined in the miniport. So for now it can't go into the kernel.
//JJV-These are no longer needed.
////VOID OEMEnableExtensions(VOID);
////VOID OEMDisableExtensions(VOID);
////VOID WriteFb32(U032 datum, U032 addr);
////U032 ReadFb32(U032 addr);
////VOID WritePriv32(U032 datum, U032 addr);
////U032 ReadPriv32(U032 addr);
////VOID WriteIndexed(U032 addr, U016 datum);
////VOID WritePriv08(U032 addr, U008 datum);
////U016 ReadIndexed(U032 addr, U008 reg);

// Define away NT only procedures.
#define osRmInitialized() 1
#define osInitMpSync()
#define osInitMpSync() 	
#define osAcquireMpSync()		 
#define osReleaseMpSync()		 
#define osAcquireMpSyncForInt()
#define osReleaseMpSyncForInt()
#define osMapUserMemory(x)	0
#define osUnmapUserMemory(x, y)
#define osIsr(pDev) mcService(pDev)
#define osEnterCriticalCode(p)
#define osExitCriticalCode(p)

#define osMapKernelSpace(a,b,c)   0
#define osMapUserSpace(a,b,c,d)   0
#define osMapIOSpace(a,b,c,d)     0
#define osUnmapIOSpace(a,b,c,d)   0
#define osSetMemRange(a,b,c)      0
#define osAllocContigPages(a)     0
#define osAllocPool(a,b)          1
#define osFreePool(a)
#define osMemSet(a,b,c)
#define osGetAGPPhysAddr(a,b,c,d) 1
#define osGetKernPhysAddr(a)      0
#define osUnmapUserSpace(a,b)
#define osUnmapKernelSpace(a,b) 
#define osFreeContigPages(a)

#define osFlushCpuCache(p) ((VOID)0)

#define osDldLinkImg(p) RM_ERROR
#define osDldUnlinkImg(p) RM_ERROR

// define away mac-only routines
#define osReadRegistryBoolean(a,b,c)    1

// 2/28/96 msk - Used to find out if os is terminating
BOOL osTerminating(VOID);
//
// Misc functions.
//

/* TO DO: remove these
RM_STATUS initMapping(VOID);
RM_STATUS initFbEx(VOID);
RM_STATUS initGrEx(VOID);
RM_STATUS initDacEx(VOID);
RM_STATUS initCodecEx(VOID);
RM_STATUS initFifoEx(VOID);
*/
RM_STATUS initMapping	(PHWINFO);
RM_STATUS initFbEx	 	(PHWINFO);
RM_STATUS initFifoEx 	(PHWINFO);
RM_STATUS initGrEx	 	(PHWINFO);
RM_STATUS initDacEx	 	(PHWINFO);
RM_STATUS initCodecEx	(PHWINFO);

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

typedef struct _def_win9x_hw_info *PWIN9XHWINFO;
typedef struct _def_win9x_hw_info
{
	U008	filler1;
	U016	filler2;

	U032 dllIpcOffset;
    U032 dllIpcSelector;	
    U032 dllNotifyOffset;
    U032 dllNotifySelector;

	U032 osDeviceHandle;

	U008	osScheduled;
	VOID	(*miniVDDPostModeChange)();		// callback to miniVDD PostCRTCModeChange
	char	*osDeviceName;					// device name string
	U032	*NvSaveVmmStack;
	U032	*NvIsrStackFrame;
	U032	*NvIsrStackFrameBase;
} WIN9XHWINFO;


#endif // _OS_H_
