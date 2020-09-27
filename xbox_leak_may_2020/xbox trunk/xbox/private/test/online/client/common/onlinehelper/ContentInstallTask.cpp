// ontentInstallTask.cpp: implementation of the ContentInstallTask class.
//
//////////////////////////////////////////////////////////////////////

#include "ContentInstallTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentInstallTask::CContentInstallTask()
{
	m_OfferingID = 0;
}

CContentInstallTask::~CContentInstallTask()
{

}

BOOL CContentInstallTask::StartTask()
{
	// If this object has been used for a previous installation task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}
	
	m_hrLastResult = XOnlineContentInstall(m_OfferingID, m_hWorkEvent, &m_hTask);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CContentInstallTask: XOnlineContentInstall failed", m_hrLastResult);
		return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

HRESULT CContentInstallTask::GetProgress(DWORD *pdwPercentDone, ULONGLONG *pqwNumerator, ULONGLONG *pqwDenominator)
{
	return XOnlineContentInstallGetProgress(m_hTask, pdwPercentDone, pqwNumerator, pqwDenominator);

}

void CContentInstallTask::SetOfferingID(XONLINEOFFERING_ID OfferingID)
{
	m_OfferingID = OfferingID;
}
