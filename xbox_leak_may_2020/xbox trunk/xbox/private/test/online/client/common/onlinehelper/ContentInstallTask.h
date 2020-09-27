// ontentInstallTask.h: interface for the ContentInstallTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class CContentInstallTask : public CAsyncTask  
{
public:
	void SetOfferingID(XONLINEOFFERING_ID OfferingID);
	HRESULT GetProgress(DWORD *pdwPercentDone, ULONGLONG *pqwNumerator, ULONGLONG *pqwDenominator);
	BOOL StartTask();
	CContentInstallTask();
	virtual ~CContentInstallTask();

protected:
	XONLINEOFFERING_ID m_OfferingID;
};
