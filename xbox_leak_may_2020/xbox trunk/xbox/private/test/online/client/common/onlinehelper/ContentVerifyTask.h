// ContentVerifyTask.h: interface for the CContentVerifyTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class CContentVerifyTask : public CAsyncTask  
{
public:
	void SetOfferingID(XONLINEOFFERING_ID OfferingID);
	BOOL StartTask();
	CContentVerifyTask();
	virtual ~CContentVerifyTask();

protected:
	XONLINEOFFERING_ID m_OfferingID;
};
