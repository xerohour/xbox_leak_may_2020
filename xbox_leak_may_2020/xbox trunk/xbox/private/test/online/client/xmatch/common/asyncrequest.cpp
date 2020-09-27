//==================================================================================
// asyncrequest.cpp: implementation of a class wrapper for online async requests
//==================================================================================
#include "asyncrequest.h"

inline DWORD AsyncRequestGetElapsedTime(DWORD dwStartTime, DWORD dwCurrentTime)
{
	dwCurrentTime = GetTickCount();
	if(dwStartTime > dwCurrentTime)
		return (MAXDWORD - dwStartTime + dwCurrentTime);
	else
		return (dwCurrentTime - dwStartTime);
}

//==================================================================================
// CXOnlineAsyncRequest
//==================================================================================
CXOnlineAsyncRequest::CXOnlineAsyncRequest()
{
	m_hWorkEvent = NULL;
	m_hTaskHandle = NULL;
	m_nCurrentState = STATE_NOTSTARTED;
	m_dwAsyncID = 0;
}

CXOnlineAsyncRequest::~CXOnlineAsyncRequest()
{
	DWORD dwWorkFlags = 0, dwCancelPumpsLeft = 0;

	switch(m_nCurrentState)
	{
	case STATE_NOTSTARTED:
		if(!m_hTaskHandle)
			break;
		// Intentional fall-through if we have allocated a task handle and need to cleanup
	case STATE_INPROGRESS:
		ASSERT(!FAILED(XOnlineTaskCancel(m_hTaskHandle, &dwWorkFlags)));
		m_nCurrentState = STATE_CANCELLED;
		if(XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
			break;
		// Intentional fall-through if we need to poll for cancel to complete
	case STATE_CANCELLED:
		dwCancelPumpsLeft = 10;
		do
		{
			ASSERT(!FAILED(XOnlineTaskContinue(m_hTaskHandle, 0, &dwWorkFlags)));
			--dwCancelPumpsLeft;
		}
		while(dwCancelPumpsLeft && !XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags));

		// If we couldn't cancel the task after 10 work pumps, then ASSERT
		ASSERT(XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags));
		break;
	default:
		break;
	}

	if(m_hWorkEvent)
	{
		CloseHandle(m_hWorkEvent);
	}
}

HRESULT CXOnlineAsyncRequest::DoWork(DWORD dwTimeToWork)
{
	HRESULT hr = S_OK;
	DWORD dwStartTime = 0, dwCurrentTime = 0, dwElapsedTime = 0, dwWorkFlags = 0;

	// Verify parameters
	if(!m_hTaskHandle || !m_hWorkEvent || m_nCurrentState == STATE_NOTSTARTED || m_nCurrentState == STATE_COMPLETE)
	{
		__asm int 3;
		return E_INVALIDARG;
	}

	dwStartTime = GetTickCount();
	do
	{
		// Get the new elapsed time.  If we've gone over, try one more pump
		// but set up the elapsed time to show that we won't wait
		dwElapsedTime = AsyncRequestGetElapsedTime(dwStartTime, GetTickCount());
		if(dwElapsedTime >= dwTimeToWork)
			dwElapsedTime = dwTimeToWork;

		// Call the work pump
		dwWorkFlags = 0;

		if(m_hTaskHandle == (VOID *) 0xDDDDDDDD)
		{
			__asm int 3;
		}

		hr = XOnlineTaskContinue(m_hTaskHandle, dwTimeToWork - dwElapsedTime, &dwWorkFlags);
		if(FAILED(hr))
			return hr;

		// If the task is complete then return success
		if(XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			m_nCurrentState = STATE_COMPLETE;
			return S_OK;
		}

		// Get the new elapsed time and exit if we've gone over
		dwElapsedTime = AsyncRequestGetElapsedTime(dwStartTime, GetTickCount());
		if(dwElapsedTime >= dwTimeToWork)
			break;

		// Wait for the work event and return an error if we run out of time waiting
		if(WaitForSingleObject(m_hWorkEvent, dwTimeToWork - dwElapsedTime) == WAIT_TIMEOUT)
			break;

	} while(dwElapsedTime < dwTimeToWork);

	return E_PENDING;
}

HRESULT CXOnlineAsyncRequest::Cancel()
{
	HRESULT hr = S_OK;
	DWORD dwWorkFlags = 0;

	// Verify parameters
	if((m_nCurrentState != STATE_INPROGRESS) && (m_nCurrentState != STATE_NOTSTARTED))
		return E_INVALIDARG;

	hr = XOnlineTaskCancel(m_hTaskHandle, &dwWorkFlags);
	if(FAILED(hr))
		return hr;
	
	// If the task is complete then return success
	if(XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
	{
		m_nCurrentState = STATE_COMPLETE;
		return S_OK;
	}

	m_nCurrentState = STATE_CANCELLED;
	return S_OK;
}

HRESULT CXOnlineAsyncRequest::Close()
{
	HRESULT hr = S_OK;

	// Verify parameters
	if((m_nCurrentState != STATE_COMPLETE) && (m_nCurrentState != STATE_NOTSTARTED))
		return E_INVALIDARG;

	hr = XOnlineTaskCloseHandle(m_hTaskHandle);
	if(FAILED(hr))
		return hr;

	// Clean-up all memver variables
	m_hTaskHandle = NULL;
	CloseHandle(m_hWorkEvent);
	m_hWorkEvent = NULL;
	m_nCurrentState = STATE_NOTSTARTED;

	return S_OK;
}
