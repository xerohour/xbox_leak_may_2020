//==================================================================================
// xmclasses.h: header for XMatch request helper classes
//==================================================================================

#if !defined(ASYNCREQUEST_H)
#define ASYNCREQUEST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <xtl.h>
#include <xdbg.h>
#include <xonline.h>

//==================================================================================
// Structures
//==================================================================================
typedef enum
{
	STATE_NOTSTARTED = 0,
	STATE_INPROGRESS,
	STATE_CANCELLED,
	STATE_COMPLETE
} XONLINE_ASYNC_STATE;

//==================================================================================
// CXOnlineAsyncRequest
//==================================================================================
class CXOnlineAsyncRequest
{
public:
	CXOnlineAsyncRequest();
	~CXOnlineAsyncRequest();
	HRESULT DoWork(DWORD dwTimeToWork);
	HRESULT Cancel();
	HRESULT Close();
	HANDLE GetWorkEvent() { return m_hWorkEvent; }
	XONLINETASK_HANDLE GetTaskHandle() { return m_hTaskHandle; }
	XONLINE_ASYNC_STATE GetCurrentState() { return m_nCurrentState; }
	INT GetRequestCategory() { return m_nRequestCategory; }
	void SetAsyncID(DWORD dwAsyncID) { m_dwAsyncID = dwAsyncID; }
	DWORD GetAsyncID() { return m_dwAsyncID; }
	void SetRequestCategory(INT nRequestCategory) { m_nRequestCategory = nRequestCategory; }
	virtual HRESULT StartRequest() = 0;

protected:
	INT m_nRequestCategory;
	HANDLE m_hWorkEvent;
	XONLINETASK_HANDLE m_hTaskHandle;
	XONLINE_ASYNC_STATE m_nCurrentState;
	DWORD m_dwAsyncID;
};

#endif // !defined(ASYNCREQUEST_H)
