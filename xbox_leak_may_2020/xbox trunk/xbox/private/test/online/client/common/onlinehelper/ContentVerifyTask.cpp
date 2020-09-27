// ContentVerifyTask.cpp: implementation of the CContentVerifyTask class.
//
//////////////////////////////////////////////////////////////////////

#include "ContentVerifyTask.h"

#define XBOX_HD_SECTOR_SIZE        512

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentVerifyTask::CContentVerifyTask()
{
	m_OfferingID = 0;
}

CContentVerifyTask::~CContentVerifyTask()
{

}

BOOL CContentVerifyTask::StartTask()
{
	DWORD dwVerifyBufferSize = XBOX_HD_SECTOR_SIZE * 2;

	// If this object has been used for a previous verification task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}
	
	m_hrLastResult = XOnlineContentVerify(m_OfferingID, NULL, &dwVerifyBufferSize, m_hWorkEvent, &m_hTask);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CContentVerifyTask: XOnlineContentVerify failed", m_hrLastResult);
		return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

void CContentVerifyTask::SetOfferingID(XONLINEOFFERING_ID OfferingID)
{
	m_OfferingID = OfferingID;
}
