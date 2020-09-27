#if !defined _AUDIOOS_H_
#define _AUDIOOS_H_

#if defined __cplusplus
extern "C" {
#endif

// init calls
RM_STATUS   RmInitAudioDevice(VOID *pResList, VOID *pDevObj, PHWINFO_COMMON_FIELDS pDev);
VOID        RmShutdownAudioDevice(PHWINFO_COMMON_FIELDS pDev);


/* specifies the dont care bits from bit 12 onwards */
#define ALIGN_4K					0x00000000
#define ALIGN_8K					0x00000001		
#define ALIGN_16K					0x00000003
#define ALIGN_32K					0x00000007
#define ALIGN_64K					0x0000000F
#define ALIGN_128K					0x0000001F
#define ALIGN_DONT_CARE				0xFFFFFFFF

// exported services
RM_STATUS	aosAllocateMem(U032 uSize, U032 uAlignMask, VOID **ppLinAddr, VOID **ppPhysAddr);
VOID		aosFreeMem(VOID *pLinAddr);

RM_STATUS	aosMapLinearAddress(VOID *pPhys, U032 uSize, VOID **ppLinear);
VOID		aosUnmapLinearAddress(VOID *pLinear, U032 uSize);

RM_STATUS   aosGetPhysicalAddress(VOID *pLinAddr, U032 uSize, VOID **pPhysAddr);

VOID        aosZeroMemory(VOID *pLinAddr, U032 uSize);

// debug print stuff
VOID        aosDbgPrintString(int iDbgLevel, char *pStr);
VOID        aosDbgBreakPoint();

// OS-independent functions for doing file i/o - used for debugging drivers without real HW
VOID *      aosCreateDiskFile();
VOID        aosWriteDiskFile(VOID *pHandle, VOID *pBuffer, U032 uSize);
VOID        aosCloseDiskFile(VOID *pHandle);


#define PAGEOFF(p)			        (((U032)(p)) & 0xFFF)
#define MAX_SIMUL_ALLOCATIONS       256     /* maximum number of simultaneous memory allocations */

#if !defined PAGE_SIZE
#define PAGE_SIZE                   4096
#endif

// this is all defined in os.h for Win9x, but not for NT (??)
#if !defined DEBUGLEVEL_TRACEINFO
#define DEBUGLEVEL_TRACEINFO	    0       // For informational debug trace info
#define DEBUGLEVEL_SETUPINFO	    1       // For informational debug setup info
#define DEBUGLEVEL_USERERRORS	    2       // For debug info on app level errors
#define DEBUGLEVEL_WARNINGS	        3       // For RM debug warning info
#define DEBUGLEVEL_ERRORS	        4       // For RM debug error info
#endif

#if defined __cplusplus
};
#endif

#endif