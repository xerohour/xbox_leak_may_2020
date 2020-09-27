#ifndef __XMATCHTEST_H__
#define __XMATCHTEST_H__

//@@@ drm: XOnlineTaskDoWork replaced with XOnlineTaskContinue
#define XOnlineTaskDoWork(a, b) (FALSE)

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

// Until this is defined in a header, we'll have to reproduce the extended header structure here
typedef enum
{
    xmatchWorking = 0,
    xmatchCancelled,
    xmatchDone

} XMATCH_STATE;

namespace XMatchTestNamespace {

//
// BUGBUG: Dummy stubs to let it compile
//

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionCreate(
    IN XNKID SessionID,
    IN DWORD dwNumAttributes,
    IN DWORD dwAttributesLen,
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE* phSession
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetInt(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN ULONGLONG qwAttributeValue
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetString(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN LPCWSTR pwszAttributeValue
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetBlob(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN DWORD dwAttributeLength,
    IN PVOID pvAttributeValue
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionAddPlayer(
    IN XONLINETASK_HANDLE hSession,
    IN ULONGLONG qwUserPUID
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionRemovePlayer(
    IN XONLINETASK_HANDLE hSession,
    IN ULONGLONG qwUserPUID
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSend(
    IN XONLINETASK_HANDLE hSession
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionGetID (
    IN XONLINETASK_HANDLE hSession,
    OUT XNKID* pSessionID
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionDelete(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phDelete
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionFindFromID(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchCreate(
    IN DWORD dwProcedureIndex,
    IN DWORD dwNumResults,
    IN DWORD dwNumParameters,
    IN DWORD dwParametersLen,
    IN DWORD dwResultsLen,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendInt(
    IN XONLINETASK_HANDLE hSearch,
    IN ULONGLONG qwParameterValue
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendString(
    IN XONLINETASK_HANDLE hSearch,
    IN LPCWSTR pwszParameterValue
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendBlob(
    IN XONLINETASK_HANDLE hSearch,
    IN DWORD dwParameterLen,
    IN LPVOID pvParameterValue
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchSend(
    IN XONLINETASK_HANDLE hSearch
    );
    
XBOXAPI
HRESULT 
WINAPI
XOnlineMatchSearchGetResults(
    IN XONLINETASK_HANDLE hSearch,
    OUT PXMATCH_SEARCHRESULT **prgpSearchResults,
    OUT DWORD *pdwReturnedResults
    );
 
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchGetAttribute(
    IN XONLINETASK_HANDLE hSearch,
    IN DWORD dwSearchResultIndex,
    IN DWORD dwAttributeIndex,
    OUT DWORD *pdwAttributeID,
    OUT VOID *pAttributeValue,
    OUT DWORD *pcbAttributeValue
    );

//==================================================================================
// Defines
//==================================================================================
#define PUMP_CS_EVENT()					fDone = XOnlineTaskDoWork(hLogon, 0); \
										if(fDone) \
										{ \
											xLog(hLog, XLL_FAIL, "Lost connection with CS, exiting test!"); \
										    goto Exit; \
										}
#define FAIL_TESTCASE(FailureMessage)	xLog(hLog, XLL_FAIL, FailureMessage); \
										break
#define PASS_TESTCASE(PassMessage)		xLog(hLog, XLL_PASS, PassMessage);
#define START_TESTCASE(TestCaseName)	while(TRUE) \
										{ \
											xStartVariation(hLog, TestCaseName)
#define END_TESTCASE()						break; \
										}; \
										PUMP_CS_EVENT()\
										xEndVariation(hLog)
#define CLEANUP_TASK(hTask)				if(hTask) \
										{ \
											XOnlineTaskClose(hTask); \
											hTask = NULL; \
										}

#define NUM_SERVICES 1
#define XMATCH_SERVER_DLL_W L"xmatchserv.dll"
#define XMATCH_SERVER_MAIN_FUNCTION_A "XMatchServerCallbackFunction"
#define XMATCH_SERVER_START_FUNCTION_A "XMatchServerStartFunction"
#define XMATCH_SERVER_STOP_FUNCTION_A "XMatchServerStopFunction"

// BUGBUG: Removed X_ATTRIBUTE_TYPE_* since they are no longer supported

#define GLOBALINT_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000002
#define GLOBALINT_ID2 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000003
#define GLOBALINT_ID3 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000004

#define XMATCHUSERINT_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000001

#define GLOBALUSERINT_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000002
#define GLOBALUSERINT_ID2 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000003
#define GLOBALUSERINT_ID3 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000004

#define GLOBALSTR_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000001
#define GLOBALSTR_ID2 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000002
#define GLOBALSTR_ID3 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000003

#define GLOBALUSERSTR_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000001
#define GLOBALUSERSTR_ID2 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000002
#define GLOBALUSERSTR_ID3 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000003

#define GLOBALBLB_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000001
#define GLOBALBLB_ID2 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000002
#define GLOBALBLB_ID3 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000003

#define GLOBALUSERBLB_ID1 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000001
#define GLOBALUSERBLB_ID2 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000002
#define GLOBALUSERBLB_ID3 X_ATTRIBUTE_SCOPE_GLOBAL | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000003

#define TITLEINT_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000001
#define TITLEINT_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000002
#define TITLEINT_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000003

#define TITLEUSERINT_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000001
#define TITLEUSERINT_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000002
#define TITLEUSERINT_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000003

#define TITLESTR_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000001
#define TITLESTR_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000002
#define TITLESTR_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000003

#define TITLEUSERSTR_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000001
#define TITLEUSERSTR_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000002
#define TITLEUSERSTR_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_STRING | 0x00000003

#define TITLEBLB_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000001
#define TITLEBLB_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000002
#define TITLEBLB_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_SESSION |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000003

#define TITLEUSERBLB_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000001
#define TITLEUSERBLB_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000002
#define TITLEUSERBLB_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | /*X_ATTRIBUTE_TYPE_USER |*/ X_ATTRIBUTE_DATATYPE_BLOB | 0x00000003

#define INT1 111222333444555666
#define INT2 777888
#define INT3 999

#define STRING1 L"AAAAABBBBB"
#define STRING2 L"CCCCCDDDDDEEEEE"	// Code expects STRING2 to be longer than STRING1
#define STRING3 L"FFFFF"

#define RESIZESTRING_MID L"1234567890123456789012345678901234567890123456789"
#define RESIZESTRING_MAX L"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
#define RESIZESTRING_MIN L""

#define BLOB1 "ZZZZZYYYYY"
#define BLOB2 "XXXXXWWWWWVVVVV"		// Code expectes BLOB2 to be longer than BLOB1
#define BLOB3 "UUUUU"

#define RESIZEBLOB_MID "12345678901234567890123456789012345678901234567890"
#define RESIZEBLOB_MAX "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"

#define USER1 0xFFFFFFFF00000001
#define USER2 0xFFFFFFFF00000002
#define USER3 0xFFFFFFFF00000003

#define INVALIDSP_INDEX 0xAAAAAAAA
#define NOPARAM_INDEX 0x00000000
#define INTPARAM_INDEX 0x00000001
#define STRINGPARAM_INDEX 0x00000002
#define BLOBPARAM_INDEX 0x00000003
#define INTSTRBLB_INDEX 0x00000004
#define INTBLBSTR_INDEX 0x00000005
#define STRINTBLB_INDEX 0x00000006
#define BLBINTSTR_INDEX 0x00000007
#define STRBLBINT_INDEX 0x00000008
#define BLBSTRINT_INDEX 0x00000009
#define GETSESSSP_INDEX 0x0000000A
#define GETSESSINT_INDEX 0x0000000B
#define GETSESSSTR_INDEX 0x0000000C
#define GETSESSBLB_INDEX 0x0000000D
#define GETSESSALL_INDEX 0x0000000E
#define GETTITLEALL_INDEX 0x0000000F
#define NOEXECPERM_INDEX 0x00000010

#define MAX_DISPLAY_RESULTS 50
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

// XMATCH test case IDs
typedef enum _XMATCHMSG_LOCAL_TESTTYPES
{
	XMATCHMSG_LOCAL_BASE = 0,
	XMATCHMSG_OPENSESSION = 0,
	XMATCHMSG_SETATTRIBUTES,
	XMATCHMSG_SENDSESSION,
	XMATCHMSG_CREATESEARCH,
	XMATCHMSG_APPENDPARAMS,
	XMATCHMSG_SENDSEARCH,
	XMATCHMSG_SEARCHRESULTS,
	XMATCHMSG_ADDREMOVEPLAYERS,
	XMATCHMSG_DELETESESSION,
	XMATCHMSG_FINDFROMID,
	XMATCHMSG_GETSESSIONID,
	
	XMATCHMSG_LOCAL_MAXTEST
} XMATCHMSG_LOCAL_TESTTYPES, *PXMATCHMSG_LOCAL_TESTTYPES;

// XMATCH test function
typedef HRESULT (*PXMATCH_LOCAL_TEST_FUNCTION)(HANDLE hLog, XONLINETASK_HANDLE hLogon);

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI XMatchTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI XMatchTestStartTest(IN HANDLE hLog);
VOID WINAPI XMatchTestEndTest();

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

// Local test cases
HRESULT XMatchTest_OpenSession(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_SetAttributes(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_SendSession(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_CreateSearch(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_AppendParams(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_SendSearch(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_SearchResults(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_AddRemovePlayers(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_DeleteSession(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_FindSessionFromID(HANDLE hLog, XONLINETASK_HANDLE hLogon);
HRESULT XMatchTest_GetSessionID(HANDLE hLog, XONLINETASK_HANDLE hLogon);

} // namespace XMatchTestNamespace

#endif // __XMATCHTEST_H__
