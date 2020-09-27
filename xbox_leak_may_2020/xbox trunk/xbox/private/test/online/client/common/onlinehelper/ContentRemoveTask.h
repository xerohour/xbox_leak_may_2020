// ContentRemoveTask.h: interface for the CContentRemoveTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class CContentRemoveTask : public CAsyncTask  
{
public:
	void SetOfferingID(XONLINEOFFERING_ID OfferingID);
	BOOL StartTask();
	CContentRemoveTask();
	virtual ~CContentRemoveTask();

protected:
	XONLINEOFFERING_ID m_OfferingID;
};
