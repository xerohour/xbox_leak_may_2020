// OfferingEnumerateTask.cpp: implementation of the COfferingEnumerate class.
//
//////////////////////////////////////////////////////////////////////

#include "OfferingEnumerateTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COfferingEnumerateTask::COfferingEnumerateTask()
{
	SYSTEMTIME st;

	st.wYear = 2000;
	st.wMonth = 1;
	st.wDay = 1;
	st.wDayOfWeek = 6;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &(m_EnumParams.ftActiveAfter));
	m_EnumParams.dwOfferingType = 0xffffffff;
	m_EnumParams.dwBitFilter = 0xffffffff;
	m_EnumParams.dwMaxResults = 10;
	m_EnumParams.dwDescriptionIndex = 0;

	m_dwUserIndex = 0;
	m_dwTitleSpecificDataMaxSize = 0;
}

COfferingEnumerateTask::~COfferingEnumerateTask()
{

}

BOOL COfferingEnumerateTask::StartTask()
{
	DWORD dwRequiredBufferSize = 0;

	// If this object has been used for a previous enumeration task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}
	
	dwRequiredBufferSize = XOnlineOfferingEnumerateMaxSize(&m_EnumParams, m_dwTitleSpecificDataMaxSize);

	m_hrLastResult = XOnlineOfferingEnumerate(XONLINEOFFERING_ENUM_DEVICE_ONLINE, 0, &m_EnumParams, NULL, dwRequiredBufferSize, m_hWorkEvent, &m_hTask);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "COfferingEnumerate: XOnlineOfferingEnumerate failed", m_hrLastResult);
		return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

HRESULT COfferingEnumerateTask::GetResults(PXONLINEOFFERING_INFO **prgpOfferingInfo, DWORD *pdwReturnedResults, BOOL *pfMoreResults)
{
	return XOnlineOfferingEnumerateGetResults(m_hTask, prgpOfferingInfo, pdwReturnedResults, pfMoreResults);
}

void COfferingEnumerateTask::SetTitleSpecificDataMaxSize(DWORD dwTitleSpecificDataMaxSize)
{
	m_dwTitleSpecificDataMaxSize = dwTitleSpecificDataMaxSize;
}

void COfferingEnumerateTask::SetUserIndex(DWORD dwUserIndex)
{
	m_dwUserIndex = dwUserIndex;
}

void COfferingEnumerateTask::SetEnumParams(PXONLINEOFFERING_ENUM_PARAMS pEnumParams)
{
	if(!pEnumParams)
		return;

	memcpy(&m_EnumParams, pEnumParams, sizeof(XONLINEOFFERING_ENUM_PARAMS));
}
