#ifndef _NVPEOS_H
#define _NVPEOS_H

///////////
// external services
//
extern void KeClearEvent(void *hEvent);
extern U032 KeSetEvent(void *hEvent, void *Increment, U032 Wait);
extern U032 RmAllocChannelDma(U032, U032, U032,	U032, U032,	U032, U032,	U032 *);
extern U032 RmAllocObject(U032, U032, U032, U032);
extern U032 RmFree(U032, U032, U032);
extern U032 RmAllocContextDma(U032, U032, U032, U032, U032, U032, U032);
extern U032 Nv01AllocEvent(NVOS10_PARAMETERS *);
extern U032 RmAlloc(U032, U032, U032, U032, VOID *);

#if DBG

extern int cur_debuglevel;

// Debug level values
#define DEBUGLEVEL_TRACEINFO    0     // For informational debug trace info
#define DEBUGLEVEL_SETUPINFO    1     // For informational debug setup info
#define DEBUGLEVEL_USERERRORS   2     // For debug info on app level errors
#define DEBUGLEVEL_WARNINGS     3     // For RM debug warning info
#define DEBUGLEVEL_ERRORS       4     // For RM debug error info

VOID osDbgPrintString(int, char*);
VOID osDbgPrintStringValue(int, char*, int);

#define DBG_PRINT_STRING(d,s)           osDbgPrintString((d),(s))
#define DBG_PRINT_STRING_VALUE(d,s,v)   osDbgPrintStringValue((d),(s),(v))

#else

#define DBG_PRINT_STRING(d,s)
#define DBG_PRINT_STRING_VALUE(d,s,v)

#endif // DBG

// local prototypes
U032 osNVPAllocDmaBuffer(void *pContext,U032 dwSize,void **pBuffer);
void osNVPReleaseDmaBuffer(void *pContext,void *pBuffer);
void osNVPClearEvent(void *hEvent);
U032 osNVPSetEvent(void *hEvent);
U032 rmNVPAllocContextDma(U032 hClient,U032 hDma,U032 hClass,U032 flags,U032 selector,U032 base,U032 limit);
U032 rmNVPAllocChannelDma(U032 hClient,U032 hDevice,U032 hDmaChannel,U032 hClass,U032 hErrorContext,U032 hDataContext,U032 offset,U032 *ppControl);
U032 rmNVPAllocObject(U032 hClient,U032 hChannel,U032 hObject,U032 hClass);
U032 rmNVPAllocEvent(NVOS10_PARAMETERS *pEvent);
U032 rmNVPFree(U032 hClient,U032 hParent,U032 hObject);
U032 rmNVPAlloc(U032 hClient, U032 hChannel, U032 hObject, U032 hClass, PVOID pAllocParms);

#endif // _NVPEOS_H