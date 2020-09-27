#ifndef _GETUTILS_
#define _GETUTILS_

#define ABITER_MSG "Arbiters please triage this failure, Triage was unable to determine the followup for this Interupt Vector!"

#define EBP 5
#define RETADR 4
#define PARAM3 3
#define PARAM2 2
#define PARAM1 1

#define DEEPER_LOOPCOUNT 10

DWORD GetProblemTitle();
DWORD ScanStackForFailureType(const CHAR *buffer);
BOOL GetAssertDataFromStack(const CHAR *buffer);

BOOL GetESI(ULARGE_INTEGER *Value);

// Does dd on address and gets value at Longword #LNum
BOOL GetDxLongNumber(ULARGE_INTEGER Address, DWORD LNum, ULARGE_INTEGER *Value);
BOOL GetDxLongNumber(DWORD Address, DWORD LNum, DWORD *Value);
// Does a dd on the name and gets longward #X.  eg.  "nt!vibaddriver"
BOOL GetStringLongNumber(CHAR *StringName, DWORD LNum, ULARGE_INTEGER *Value);

// Does Command and gets param1-3 ,EBP or ReturnAddr of ProcName.  (Right to Left)
BOOL GetProcedureParamNumber(CHAR *Command, CHAR *ProcName, DWORD PNum, DWORD *Value);
// Same as GetProcedureParamNumber except takes a buffer you already have
BOOL GetParamFromStackBuffer(CHAR *ProcName, DWORD PNum, DWORD *Value);
BOOL GetParamFromStackBuffer(CHAR *ProcName, DWORD PNum, ULARGE_INTEGER *Value);

// Does a !process -1 4 and gets the Thread ID of the TID passed in.
BOOL GetThreadNumberOfCurrentProcTID(ULARGE_INTEGER TID, ULARGE_INTEGER *Thread);
// Does a !process TID 4 and gets the Thread ID of the TID Passed in.
BOOL GetThreadNumberOfPIDsTID(DWORD TID, DWORD PID, DWORD *Thread);

BOOL GetThreadHoldingLoaderLock(ULARGE_INTEGER *Thread);
// Does a !locks on the Lock supplied and gets holding Thread
BOOL GetThreadHoldingLock(DWORD Lock, DWORD *TheThread);
// Get the Address of the BadDriver address
BOOL GetViBadDriverAddress(ULARGE_INTEGER *Address);
// Gets the unicode string supplied by nt!vibaddriver
BOOL GetViBadDriverString(CHAR *BadString, DWORD len);
// Gets the name of the current process
BOOL GetProcessName(CHAR *Name);
//Tries to determine the failure type based on process name
DWORD GetFailTypeFromProcess(CHAR **descbuff);

// Gets Exr and Cxr by calling wBuffer and FromDD below
BOOL GetEXRAndCXR(ULARGE_INTEGER *Exr, ULARGE_INTEGER *Cxr);
// Given an address it gets the exr and cxr if the address is true
BOOL GetEXRAndCXRFromDx(ULARGE_INTEGER Address, ULARGE_INTEGER *Exr, ULARGE_INTEGER *Cxr);
// Given the initial screen buffer it tries to get the exr and cxr
//BOOL GetEXRAndCXRFromBackSpew(DWORD *Exr, DWORD *Cxr);
//Gets the formated buffer from the exr and cxr
BOOL GetExceptionBuffer(ULARGE_INTEGER Exr, ULARGE_INTEGER Cxr);

// Takes a stack buffer and checks for WaitOn Resource and Critical Sections
// and tries to get the stacks of the holders.  Also follows mutants to owners.
// Returns number of levels we followed down.
DWORD GetDeeperStacks();
// Takes a stack waiting on a Exclusive Resource and tries to replace with its holder
BOOL GetThreadHoldingResourceStack();
BOOL GetThreadHoldingLoaderLockStack();
//Takes a stack waiting on a critical section and replaces it with the holder
BOOL GetThreadHoldingCriticalSectionStack();
// Gets the Thread number of the mutant owning thread
BOOL GetMutantThreadFromBuffer(DWORD *Thread);
// Replaces the buffer and command with the mutant owner
BOOL GetMutantThreadOwnerStack();
BOOL GetThreadHoldingVerifierResourceStack();
BOOL GetThreadHoldingSharedResourceStack();


//Takes the Initial buffer and tries to get the holding thread
BOOL GetResTimeoutThreadFromBuffer(DWORD *TheThread);

// Gets the GPResUser Lock
BOOL GetGPResUser(DWORD *Lock);
// Drops to KernelMode, Gets GPResUser and returns holders stack
BOOL GetGPResUserStack();
//Gets the thread that is holding the kmutant object
BOOL GetKMutantOwnerThread(DWORD *Thread);

//Tries to get the PID and TID of the owner of the Critical Section
BOOL GetCriticalSectionInfoFromBuffer(DWORD *Pid, DWORD *OwnerTid);
// Get the Thread ID of the PID and TID pair
BOOL GetUserThreadId(DWORD Pid, DWORD Tid, DWORD *UserThreadId);
BOOL GetThreadHoldingRtlCriticalSectionStack();

//Tries to get the stack that is holding the display lock
BOOL GetLockDisplayStack();

BOOL GetStringAtAddress(ULARGE_INTEGER Address, CHAR *StringBuffer, size_t StringLen, BOOL fUnicode);
BOOL ExtractString(CHAR *InString, CHAR *OutString, size_t OutLen);

// Some special stuff I added for JoshR
BOOL DoSpecialCasesBasedOnStack(CHAR *Followup, DWORD FollowupSize);

BOOL GetLPCThreadStack();


#endif