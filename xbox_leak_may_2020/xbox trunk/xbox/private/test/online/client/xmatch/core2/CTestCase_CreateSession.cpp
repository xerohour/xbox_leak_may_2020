#include <common.h>
#include "TestCaseAndResult_CreateSession.h"
#include "CTestCase_CreateSession.h"
#include "matchcoretest.h"

using namespace MatchCoreTestNamespace;

namespace MatchCoreTestNamespace {

//Constructors
CTestCase_CreateSession::CTestCase_CreateSession()
	:m_nCase(0)
{
	m_pPara = new(CParameter_CreateSession);
	m_pParaUpdate = new(CParameter_CreateSession);
}

//Destructor
CTestCase_CreateSession::~CTestCase_CreateSession()
{
	delete m_pPara;
	delete m_pParaUpdate;
}


//Member functions
		
void	CTestCase_CreateSession::EvaluateParameters(TestCase_CreateSession& tc, CParameter_CreateSession* pPara)
{
	pPara->m_dwPublicCurrent = EvaluateSlot(tc.m_nDwordCasePubCurr);
	pPara->m_dwPublicAvailable = EvaluateSlot(tc.m_nDwordCasePubAvail);
	pPara->m_dwPrivateCurrent = EvaluateSlot(tc.m_nDwordCasePriCurr);
	pPara->m_dwPrivateAvailable = EvaluateSlot(tc.m_nDwordCasePriAvail);
	this->EvaluateAttr(tc, pPara);
	this->EvaluateHEvent(tc, pPara);
	this->EvaluateTask(tc, pPara);
	
	
}

void	CTestCase_CreateSession::EvaluateHEvent(TestCase_CreateSession& tc, CParameter_CreateSession* pPara)
{
	switch(tc.m_nEventCase)
	{
		case NullEvent :
			pPara->m_hWorkEvent = NULL;
			break;
		case ValidEvent :
			pPara->m_hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			pPara->m_bEventCreated = TRUE;
			break;
	// Failure cases	
		case GarbageEvent :
			pPara->m_hWorkEvent = (HANDLE) 0x1234;
			break;
			
	}

}

void	CTestCase_CreateSession::EvaluateAttr(TestCase_CreateSession& tc, CParameter_CreateSession* pPara)
{
	switch(tc.m_nAttributeCase)
	{
		case ZeroNum_Null:
		case ZeroNum_Alloc:
		case ZeroNum_Garbage:
			pPara->m_dwNumAttributes = 0;
			break;
		case One_Int:
		case One_Str:
		case One_Blob:
		case OneNum_Null:	
		case OneNum_Garbage:
			pPara->m_dwNumAttributes = 1;
			break;
		case TwoNum_OneAlloc:	
			pPara->m_dwNumAttributes = 2;
			break;
		case One_EachType:
			pPara->m_dwNumAttributes = 3;
			break;
		case Int1000:
		case Str1000:
		case Blob1000:
			pPara->m_dwNumAttributes = 1000;
			break;
			
	}

	switch(tc.m_nAttributeCase)
	{
		case ZeroNum_Null:
			pPara->m_pAttributes = NULL;
			break;
		case ZeroNum_Alloc:
			pPara->m_pAttributes = new XONLINE_ATTRIBUTE[1];
			pPara->m_bAttrAllocated = TRUE;
			break;
		case ZeroNum_Garbage:
			pPara->m_pAttributes = (PXONLINE_ATTRIBUTE) 0x1234;
			break;
		case One_Int:
			pPara->m_pAttributes = new XONLINE_ATTRIBUTE[1];
			pPara->m_bAttrAllocated = TRUE;
			pPara->m_pAttributes[0].dwAttributeID = GLOBALINT_ID1;
			pPara->m_pAttributes[0].info.integer.qwValue = INT1;
			break;
			
	}
}
void	CTestCase_CreateSession::EvaluateTask(TestCase_CreateSession& tc, CParameter_CreateSession* pPara)
{
	switch(tc.m_nAttributeCase)
	{
		case ValildTask :
			pPara->m_phTask = &pPara->m_hTask;
			break;
// Failure cases	
		case	NullTask :
			pPara->m_phTask = NULL;
			break;
			
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	CTestCase_CreateSession::RunTest()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	HRESULT	hr;
	int		iResult;
	XONLINETASK_HANDLE	hDeleteTask, *phDeleteTask;
	DWORD dwExceptionCode = 0;
	XNKID	SessionID;
	
	__try
	{

	    hr = XOnlineMatchSessionCreate(m_pPara->m_dwPublicCurrent,
	    								m_pPara->m_dwPublicAvailable,
	    								m_pPara->m_dwPrivateCurrent,
	    								m_pPara->m_dwPrivateAvailable,
	    								m_pPara->m_dwNumAttributes,
	    								m_pPara->m_pAttributes,
	    								m_pPara->m_hWorkEvent,
	    								m_pPara->m_phTask);
		
	    iResult = this->CheckResultOf_SessionCreate(hr, m_pPara->m_phTask);
	    RaiseExceptionIfNecessary(iResult)
	    if(SUCCEEDED(hr))
	    {
			PUMP_CS_AND_EXIT_IF_FAILED(g_hTestLog,g_hLogonTask)
			iResult = PumpTaskUntilTimeout(g_hTestLog,*m_pPara->m_phTask, m_pPara->m_hWorkEvent, ALLOWEDPUMPINGTIME, NULL, FALSE, &hr);
		    RaiseExceptionIfNecessary(iResult);
		    
			hr = XOnlineMatchSessionGetID (m_pPara->m_phTask, &SessionID);
		    iResult = this->CheckResultOf_SessionGetID(hr, SessionID);
		    RaiseExceptionIfNecessary(iResult);

			if(m_tcae.m_nUpdate != SY_NOUPDATE)
			{
				iResult = RunTestUpdate(SessionID);
			    RaiseExceptionIfNecessary(iResult)
			}
			
			phDeleteTask = &hDeleteTask;
		    hr = XOnlineMatchSessionDelete(SessionID, hSessionEvent, phDeleteTask);
		    iResult = this->CheckResultOf_SessionDelete(hr, phDeleteTask);
		    RaiseExceptionIfNecessary(iResult)

			PUMP_CS_AND_EXIT_IF_FAILED(g_hTestLog,g_hLogonTask)
			iResult = PumpTaskUntilTimeout(g_hTestLog,phDeleteTask, hSessionEvent, ALLOWEDPUMPINGTIME, NULL, FALSE, &hr);
		    RaiseExceptionIfNecessary(iResult);
	    }
		PASS_TESTCASE(g_hTestLog,"Session successfully advertised");
	}	
	__except((GetExceptionCode() == SY_EXCEPTION_FAIL_THIS_TESTCASE) || (GetExceptionCode() == SY_EXCEPTION_STOP_WHOLE_TEST)?
				EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
	{
		DWORD dwExceptionCode = GetExceptionCode();
		if(dwExceptionCode == SY_EXCEPTION_FAIL_THIS_TESTCASE)
		{
			FAIL_TESTCASE(g_hTestLog,"Creating session failed");
		}
		else if(dwExceptionCode == SY_EXCEPTION_STOP_WHOLE_TEST)
		{
			FAIL_TESTCASE(g_hTestLog,"Creating session failed and met critical test blocking");
		}
		
	}

// Clean up 

//
	if(dwExceptionCode == SY_EXCEPTION_STOP_WHOLE_TEST)
		return SY_EXCEPTION_STOP_WHOLE_TEST;
	else 
		return 0;
	    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	CTestCase_CreateSession::RunTestUpdate(XNKID	SessionID)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	HRESULT	hr;
	XNKID UpdateSessionID;
	
	if(m_tcae.m_nUpdate == SY_VALIDSESSIONID)
	{
		UpdateSessionID = SessionID;
	}
	else if(m_tcae.m_nUpdate == SY_INVALIDSESSIONID)
	{
		*((ULONGLONG *) &UpdateSessionID) = 0x1234;
	}
	else if(m_tcae.m_nUpdate == SY_NULLSESSIONID)
	{
		*((ULONGLONG *) &UpdateSessionID) = 0;
	}

    hr = XOnlineMatchSessionUpdate( UpdateSessionID,
    								m_pParaUpdate->m_dwPublicCurrent,
    								m_pParaUpdate->m_dwPublicAvailable,
    								m_pParaUpdate->m_dwPrivateCurrent,
    								m_pParaUpdate->m_dwPrivateAvailable,
    								m_pParaUpdate->m_dwNumAttributes,
    								m_pParaUpdate->m_pAttributes,
    								m_pParaUpdate->m_hWorkEvent,
    								m_pParaUpdate->m_phTask);

    iResult = this->CheckResultOf_SessionUpdate(hr, m_pParaUpdate->m_phTask);
    RaiseExceptionIfNecessary(iResult)

    if(SUCCEEDED(hr))
    {
		PUMP_CS_AND_EXIT_IF_FAILED(g_hTestLog,g_hLogonTask)
		iResult = PumpTaskUntilTimeout(g_hTestLog,*m_pPara->m_phTask, m_pPara->m_hWorkEvent, ALLOWEDPUMPINGTIME, NULL, FALSE, &hr);
	    RaiseExceptionIfNecessary(iResult);
	    
		hr = XOnlineMatchSessionGetID (m_pPara->m_phTask, &SessionID);
	    iResult = this->CheckResultOf_SessionGetID(hr, SessionID);
	    RaiseExceptionIfNecessary(iResult);
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT	CTestCase_CreateSession::CheckResultOf_SessionCreate(HRESULT hr, XONLINETASK_HANDLE* phMatchTask)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	INT 	iResult;
	
	xLog(g_hTestLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	xLog(g_hTestLog, XLL_INFO, "Returned task for XOnlineMatchSessionCreate is 0x%08x", *phMatchTask);

	iResult = this->CheckResultOf_SessionApiCall(hr, phMatchTask, m_tcae.m_te);
	
	return iResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT	CTestCase_CreateSession::CheckResultOf_SessionUpdate(HRESULT hr, XONLINETASK_HANDLE* phMatchTask)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	INT 	iResult;
	
	xLog(g_hTestLog, XLL_INFO, "XOnlineMatchSessionUpdate returned 0x%08x", hr);
	xLog(g_hTestLog, XLL_INFO, "Returned task for XOnlineMatchSessionUpdate is 0x%08x", *phMatchTask);

	iResult = this->CheckResultOf_SessionApiCall(hr, phMatchTask, m_tcae.m_teUpdate);
	
	return iResult;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT	CTestCase_CreateSession::CheckResultOf_SessionApiCall(HRESULT hr, XONLINETASK_HANDLE* phMatchTask,
														TestExpected_CreateSession tExpected)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	INT 	iResult;
	
	if( (  (tExpected.m_HrExpected == SY_SUCCESS)&& SUCCEEDED(hr)) ||
		(  (tExpected.m_HrExpected == SY_FAIL) && FAILED(hr)) 
	  )
	{
		xLog(g_hTestLog, XLL_INFO, "This hr is expected");
		iResult = SY_SUCCESS;
	}
	else
	{
		xLog(g_hTestLog, XLL_INFO, "This hr is NOT expected");
		iResult = SY_FAIL;
		return iResult;
	}

	if( (  (tExpected.m_HTaskExpected == SY_NONNULL) && (*phMatchTask != NULL) ) ||
		(  (tExpected.m_HTaskExpected == SY_NULL) && (*phMatchTask == NULL) ) 
	  )
	{
		xLog(g_hTestLog, XLL_INFO, "This *phMatchTask is expected");
		iResult = SY_SUCCESS;
	}
	else
	{
		xLog(g_hTestLog, XLL_INFO, "This *phMatchTask is NOT expected");
		iResult = SY_FAIL;
	}

	return iResult;
}


INT	CTestCase_CreateSession::CheckResultOf_SessionGetID(HRESULT hr, XNKID SessionID)
{
	INT 	iResult = SY_SUCCESS;

	xLog(g_hTestLog, XLL_INFO, "XOnlineMatchSessionGetID returned 0x%08x", hr);
	if( SUCCEEDED(hr)) 
	{
		xLog(g_hTestLog, XLL_INFO, "This hr is expected");
		iResult = SY_SUCCESS;
	}
	else
	{
		xLog(g_hTestLog, XLL_INFO, "This hr is NOT expected");
		iResult = SY_FAIL;
		return iResult;
	}

	xLog(g_hTestLog, XLL_INFO, "Session ID 0x%08x returned", SessionID);
	if( (*((ULONGLONG *) &SessionID) != 0) ) 
	{
		xLog(g_hTestLog, XLL_INFO, "This SessionID is expected");
		iResult = SY_SUCCESS;
	}
	else
	{
		xLog(g_hTestLog, XLL_INFO, "This SessionID is NOT expected");
		iResult = SY_FAIL;
	}

	return iResult;
}

INT	CTestCase_CreateSession::CheckResultOf_SessionDelete(HRESULT hr, XONLINETASK_HANDLE* phMatchTask)
{
	INT 	iResult = SY_SUCCESS;

	xLog(g_hTestLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if( SUCCEEDED(hr)) 
	{
		xLog(g_hTestLog, XLL_INFO, "This hr is expected");
		iResult = SY_SUCCESS;
	}
	else
	{
		xLog(g_hTestLog, XLL_INFO, "This hr is NOT expected");
		iResult = SY_FAIL;
		return iResult;
	}

	xLog(g_hTestLog, XLL_INFO, "Returned task for XOnlineMatchSessionDelete is 0x%08x", *phMatchTask);
	if( (*phMatchTask != NULL) ) 
	{
		xLog(g_hTestLog, XLL_INFO, "This *phMatchTask is expected");
		iResult = SY_SUCCESS;
	}
	else
	{
		xLog(g_hTestLog, XLL_INFO, "This *phMatchTask is NOT expected");
		iResult = SY_FAIL;
	}
	return iResult;
}

	
void	CTestCase_CreateSession::BringPameterAndExpected()
{
	m_tcae.m_tc.m_nDwordCasePubCurr = 	g_TCAE_CreateSession[m_nCase].m_tc.m_nDwordCasePubCurr;
	m_tcae.m_tc.m_nDwordCasePubAvail = 	g_TCAE_CreateSession[m_nCase].m_tc.m_nDwordCasePubAvail;
	m_tcae.m_tc.m_nDwordCasePriCurr = 	g_TCAE_CreateSession[m_nCase].m_tc.m_nDwordCasePriCurr;
	m_tcae.m_tc.m_nDwordCasePriAvail = 	g_TCAE_CreateSession[m_nCase].m_tc.m_nDwordCasePriAvail;
	m_tcae.m_tc.m_nAttributeCase = 		g_TCAE_CreateSession[m_nCase].m_tc.m_nAttributeCase;
	m_tcae.m_tc.m_nEventCase = 			g_TCAE_CreateSession[m_nCase].m_tc.m_nEventCase;
	m_tcae.m_tc.m_nTaskCreateCase = 	g_TCAE_CreateSession[m_nCase].m_tc.m_nTaskCreateCase;

	m_tcae.m_te.m_HrExpected =			g_TCAE_CreateSession[m_nCase].m_te.m_HrExpected;
	m_tcae.m_te.m_HTaskExpected =		g_TCAE_CreateSession[m_nCase].m_te.m_HTaskExpected;

	m_tcae.m_nUpdate = g_TCAE_CreateSession[m_nCase].m_nUpdate;

	if(m_tcae.m_nUpdate != SY_NOUPDATE)
	{
		m_tcae.m_tcUpdate.m_nDwordCasePubCurr = 	g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nDwordCasePubCurr;
		m_tcae.m_tcUpdate.m_nDwordCasePubAvail = 	g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nDwordCasePubAvail;
		m_tcae.m_tcUpdate.m_nDwordCasePriCurr = 	g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nDwordCasePriCurr;
		m_tcae.m_tcUpdate.m_nDwordCasePriAvail = 	g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nDwordCasePriAvail;
		m_tcae.m_tcUpdate.m_nAttributeCase = 		g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nAttributeCase;
		m_tcae.m_tcUpdate.m_nEventCase = 			g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nEventCase;
		m_tcae.m_tcUpdate.m_nTaskCreateCase = 	g_TCAE_CreateSession[m_nCase].m_tcUpdate.m_nTaskCreateCase;
	}

}

// Non member Functions

DWORD EvaluateSlot(INT nCaseSlot)
{
	DWORD dwReturn;
	
	switch(nCaseSlot)
	{
		case DWZero:
			dwReturn = 0;
			break;
		case DWOne:
			dwReturn = 1;
			break;
		case DWMaxSignedInt:
			dwReturn = 0x0fffffff;
			break;
		case DWMinSignedInt:
			dwReturn = 0x10000000;
			break;
		case DWMaxDword:
			dwReturn = 0xffffffff;
			break;
	}

	return dwReturn;
}


#if 0
void CTestCase_CreateSession::PrepareFirstParameters()
{
	this->EvaluateParameters();
}

BOOL CTestCase_CreateSession::PrepareNextParameters()
{
	if(!this->IncreaseCaseNumber())
		return FALSE;

	this->EvaluateParameters();
	

	
}

BOOL	CTestCase_CreateSession::IncreaseCaseNumber()
{
	m_nDwordCasePubCurr++;

	if(m_nDwordCasePubCurr == EndOFDWordCase){
		m_nDwordCasePubCurr = 0;
		m_nDwordCasePubAvail++;
	}
	if(m_nDwordCasePubAvail == EndOFDWordCase){
		m_nDwordCasePubAvail = 0;
		m_nDwordCasePriCurr++;
	}
	if(m_nDwordCasePriCurr == EndOFDWordCase){
		m_nDwordCasePriCurr = 0;
		m_nDwordCasePubAvail++;
	}
	if(m_nDwordCasePriAvail == EndOFDWordCase){
		m_nDwordCasePriAvail = 0;
		m_nAttributeCase++;
	}
	if(m_nAttributeCase == EndOfDAttributeCase){
		m_nAttributeCase = 0;
		m_nEventCase++;
	}
	if(m_nEventCase == EndOfEventCase){
		m_nEventCase = 0;
		m_nTaskCreateCase++;
	}
	if(m_nTaskCreateCase == EndOfTaskCreateCase){
		return FALSE
	}

	return TRUE;
		
}
#endif

}
