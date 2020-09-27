#ifndef __MATCHCORETEST_H__
#define __MATCHCORETEST_H__


//==================================================================================
// Includes
//==================================================================================
#include <xtl.h>
#include <xdbg.h>
#include <stdio.h>
#include <xtestlib.h>
#include <xlog.h>
#include <netsync.h>
#include <xlog.h>


#include <xonlinep.h>
#include <xnetref.h>

namespace MatchCoreTestNamespace {

//==================================================================================
// Defines
//==================================================================================


#define ALLOWEDPUMPINGTIME              30000


#define NUM_SERVICES 1

extern HANDLE g_hTestLog;
extern XONLINETASK_HANDLE g_hLogonTask;

#if 0
//==================================================================================
// Typedefs
//==================================================================================

//==================================================================================
// Structures
//==================================================================================
typedef struct
{
	ULONGLONG		qwUserID;
	DWORD			dwAttributeID;
	union
	{
		ULONGLONG		qwValue;
		VOID			*pvValue;
	} Value;
} ATTRIBUTE, *PATTRIBUTE;

typedef struct
{
	DWORD			dwParameterType;
	union
	{
		ULONGLONG		qwValue;
		VOID			*pvValue;
	} Value;
} PARAMETER, *PPARAMETER;
#endif

// XMATCH test case IDs
typedef enum _MATCHMSG_LOCAL_TESTTYPES
{
	MATCHMSG_LOCAL_BASE = 0,
	MATCHMSG_CREATESESSION = 0,
#if 0
	MATCHMSG_SETATTRIBUTES,
	MATCHMSG_SENDSESSION,
	MATCHMSG_CREATESEARCH,
	MATCHMSG_APPENDPARAMS,
	MATCHMSG_SENDSEARCH,
	MATCHMSG_SEARCHRESULTS,
	MATCHMSG_ADDREMOVEPLAYERS,
	MATCHMSG_DELETESESSION,
	MATCHMSG_FINDFROMID,
	MATCHMSG_GETSESSIONID,
#endif	
	MATCHMSG_LOCAL_MAXTEST
} MATCHMSG_LOCAL_TESTTYPES, *PMATCHMSG_LOCAL_TESTTYPES;

// XMATCH test function
typedef void (*PMATCH_CORE_TEST_FUNCTION)();

void MatchCoreTest_CreateSession();

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI MatchCoreTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI MatchCoreTestStart(IN HANDLE hLog);
VOID WINAPI MatchCoreTestEnd();

#if 0
// Helper functions
LPVOID MemAlloc(DWORD dwSize);
BOOL MemFree(LPVOID pBuffer);
DWORD GetBufferLenForInt();
DWORD GetBufferLenForString(LPWSTR szString);
DWORD GetBufferLenForBlob(LPSTR szBlob);
DWORD GetBufferLenForUserInt();
DWORD GetBufferLenForUserString(LPWSTR szString);
DWORD GetBufferLenForUserBlob(LPSTR szBlob);
BOOL ReadSearchResults(HANDLE hLog, XONLINETASK_HANDLE hSearch, DWORD dwResultsExpected, BOOL fMoreResultsOK);

BOOL AddAttributes(XONLINETASK_HANDLE, PATTRIBUTE, DWORD);
BOOL AddParameters(XONLINETASK_HANDLE, PPARAMETER, DWORD);
HRESULT PollTaskUntilComplete(XONLINETASK_HANDLE, HANDLE, DWORD, DWORD *, BOOL *, BOOL);
BOOL CreateSession(ULONGLONG, ULONGLONG, PATTRIBUTE, DWORD, XNKID *);
#endif
// Local test cases
HRESULT MatchCoreTest_OpenSession(HANDLE hLog, XONLINETASK_HANDLE hLogon);
#if 0
HRESULT MatchCoreTest_SetAttributes(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_SendSession(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_CreateSearch(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_AppendParams(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_SendSearch(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_SearchResults(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_AddRemovePlayers(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_DeleteSession(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_FindSessionFromID(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT MatchCoreTest_GetSessionID(HANDLE hLog, XONLINETASK_HANDLE hLogon);
#endif

} // namespace MatchCoreTestNamespace

#endif // __XMATCHTEST_H__
