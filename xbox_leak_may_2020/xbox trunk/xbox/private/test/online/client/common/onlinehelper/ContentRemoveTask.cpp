// ContentRemoveTask.cpp: implementation of the CContentRemoveTask class.
//
//////////////////////////////////////////////////////////////////////

#include "ContentRemoveTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentRemoveTask::CContentRemoveTask()
{
	m_OfferingID = 0;
}

CContentRemoveTask::~CContentRemoveTask()
{

}

BOOL CContentRemoveTask::StartTask()
{
	// If this object has been used for a previous content remove task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}
	
	m_hrLastResult = XOnlineContentRemove(m_OfferingID, m_hWorkEvent, &m_hTask);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CContentRemoveTask: XOnlineContentRemove failed", m_hrLastResult);
		return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

void CContentRemoveTask::SetOfferingID(XONLINEOFFERING_ID OfferingID)
{
	m_OfferingID = OfferingID;
}
