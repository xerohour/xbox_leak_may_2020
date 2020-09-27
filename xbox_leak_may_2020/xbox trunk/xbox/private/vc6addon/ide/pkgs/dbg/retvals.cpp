#include "stdafx.h"
#pragma  hdrstop


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#ifdef ERROR_HANDLING
#include <tchar.h>
#endif	

CRetVals g_retVals;

CRetVals::CRetVals ()
{
}

CRetVals::~CRetVals ()
{
	CleanUp ();
}

VOID CRetVals::CleanUp ()
{
	POSITION pos = m_lstTMs.GetHeadPosition();
	while (pos)
	{
		CReturnTM *pTM = (CReturnTM *)m_lstTMs.GetNext(pos);
		ASSERT(pTM->IsKindOf(RUNTIME_CLASS(CReturnTM)));
		delete pTM;
	}

	m_lstTMs.RemoveAll ();
}

VOID CRetVals::AddTM (LPCSTR szName, LPCSTR szValue, LPCSTR szType)
{
	CReturnTM *pTM = new CReturnTM (szName, szValue, szType);

	if (pTM->IsValueValid ())
	{
		m_lstTMs.AddTail (pTM);
	}
	else
	{
		delete pTM;
	}
}
