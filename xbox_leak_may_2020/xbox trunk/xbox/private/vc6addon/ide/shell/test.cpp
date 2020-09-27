// Testing Support
//
// The _TEST flag is defined for non-release/non-debug builds.  These builds
// do not have _DEBUG defined, but still have some additional functionality
// beyond the shipping build.
//

#include "stdafx.h"

#include "errno.h"
#include "toolexpt.h"

#include <bldapi.h>
#include <bldguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _TEST
LRESULT CMainFrame::OnGetProp(WPARAM nPropID, LPARAM)
{

	TRACE1("Remote GetProp: %d\n", nPropID);
	CString str;

	CSlobWnd* pSlobWnd = GetActiveSlobWnd();
	if (pSlobWnd == NULL)
	{
		TRACE("  ** no active slob!\n");
		return FALSE;
	}


	if (pSlobWnd->GetSelection()->GetStrProp(nPropID, str) != valid)
	{
		TRACE("  ** Unknown property!\n");
		return FALSE;
	}

	ATOM atom = GlobalAddAtom((LPCSTR)str);
	TRACE1("Added Global Atom: %u\n", atom);

	return (LONG)atom;
}

LRESULT CMainFrame::OnSetProp(WPARAM nPropID, LPARAM lAtom)
{
	char szBuf[256];
	VERIFY(GlobalGetAtomName((ATOM)lAtom, szBuf, 255));

	TRACE2("Remote SetProp: %d = %s\n", nPropID, szBuf);

	CSlobWnd* pSlobWnd = GetActiveSlobWnd();
	if (pSlobWnd == NULL)
	{
		TRACE("  ** no current slob!\n");
		return FALSE;
	}

	pSlobWnd->GetSelection()->SetStrProp(nPropID, (LPSTR)szBuf);

	return TRUE;
}

// help for OnHeapUse - caller should have szBuf point to at leaset a 256 TCHAR buffer
// return TRUE only if normal termination - false indicates error
BOOL FillBufHeapUse(TCHAR *szBuf)
{
	size_t sizeUsed, sizeFree;

	BOOL fRet = FALSE;

	_HEAPINFO hinfo;
	int heapstatus;
	hinfo._pentry = NULL;
	sizeUsed = 0;
	sizeFree = 0;
	while (( heapstatus = _heapwalk( &hinfo)) == _HEAPOK)
	{
		if (hinfo._useflag == _USEDENTRY)
			sizeUsed += hinfo._size;
		else
			sizeFree += hinfo._size;
	}

	switch( heapstatus )
	{
	default:
		wsprintf(szBuf, "Heap: ERROR - unexpected return\n");
		break;
	case _HEAPEMPTY:
		wsprintf(szBuf, "Heap: ERROR - Empty heap\n");
		break;
	case _HEAPEND:
		wsprintf(szBuf, "Heap: Used = 0x%x(%d); Free = 0x%x(%d)\n",(int) sizeUsed, (int) sizeUsed, (int)sizeFree, (int)sizeFree);
		fRet = TRUE;
		break;
	case _HEAPBADPTR:
		wsprintf(szBuf, "Heap: ERROR - bad pointer to heap\n");
		break;
	case _HEAPBADBEGIN:
		wsprintf(szBuf, "Heap: ERROR -  bad start of heap\n");
		break;
	case _HEAPBADNODE:
		wsprintf(szBuf, "Heap: ERROR -  bad node in heap\n");
		break;
	}

	if (errno == ENOSYS)  // override other info
	{
		wsprintf(szBuf, "Heap: ERROR - _heapwalk not supported, no heap information available\n");
		fRet = FALSE;
	}
	
	return fRet;

}


LRESULT CMainFrame::OnHeapUse(WPARAM, LPARAM)
{
	TCHAR szBuf[256];
	FillBufHeapUse(szBuf);

	ATOM atom = GlobalAddAtom(szBuf);
	TRACE1("Added Global Atom: %u\n", atom);

	return (LONG)atom;
}


LRESULT CMainFrame::OnTestDockable(WPARAM nWorkerID, LPARAM)
{
	CDockWorker* pDocker = m_pManager->WorkerFromID(nWorkerID);
	if (pDocker == NULL)
		return (LRESULT) NULL;
	else
		return (LRESULT) (LPVOID*) pDocker->m_pWnd->GetSafeHwnd();
}

LRESULT CMainFrame::OnIsBuildPossible(WPARAM nPropID, LPARAM lpstrBuf)
{
	LPBUILDSTATUS pInterface = NULL;
	if (FAILED(theApp.FindInterface(IID_IBuildStatus, (LPVOID FAR *)&pInterface)) || (pInterface == NULL))
		return((LONG)FALSE);
	BOOL fCanBuild = pInterface->IsBuildPossible() == S_OK;
	pInterface->Release();
	return((LONG)fCanBuild);
}

LRESULT CMainFrame::OnGetErrorCount(WPARAM nPropID, LPARAM lpstrBuf)
{
	static DWORD dwErrCnt;
	LPBUILDSTATUS pInterface = NULL;
	if (FAILED(theApp.FindInterface(IID_IBuildStatus, (LPVOID FAR *)&pInterface)) || (pInterface == NULL))
		return(0L);
	pInterface->GetErrorCount(&dwErrCnt);
	pInterface->Release();
	return((LONG)dwErrCnt);
}

LRESULT CMainFrame::OnGetWarningCount(WPARAM nPropID, LPARAM lpstrBuf)
{
	static DWORD dwWarnCnt;
	LPBUILDSTATUS pInterface = NULL;
	if (FAILED(theApp.FindInterface(IID_IBuildStatus, (LPVOID FAR *)&pInterface)) || (pInterface == NULL))
		return(0L);
	pInterface->GetWarningCount(&dwWarnCnt);
	pInterface->Release();
	return((LONG)dwWarnCnt);
}

LRESULT CMainFrame::OnGetProjPath(WPARAM ui, LPARAM lng)
{		// Returns a full project path.
	LPBUILDSYSTEM pBldSysIFace;
	theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace);
	if (pBldSysIFace == NULL)
		return 0L;

  	LPCSTR pszPath = NULL;
	if (pBldSysIFace->IsActiveBuilderValid() == S_OK)
		pBldSysIFace->GetBuilderFile(ACTIVE_BUILDER, &pszPath);
	pBldSysIFace->Release();
	if (pszPath == NULL)
		return 0L;

  	CString str = pszPath;
	ATOM atom = GlobalAddAtom((LPCSTR)((const char  *) str)) ;
	TRACE1("Added Global Atom: %u\n", atom) ;
	return  (LONG)atom ;
}

LRESULT CMainFrame::OnGetItemProp(WPARAM nPropID, LPARAM lpstrBuf)
{

	TRACE1("Remote GetProp: %d\n", nPropID);
	CString str;
	int iProp ;

  	CMultiSlob *pMultiSlob;
	CSlob *pSlob = NULL;
    
    CSlobWnd * pActiveSlob = (CSlobWnd *)GetActiveSlobWnd();
    if (pActiveSlob == NULL)
    {
        TRACE("  ** no active slobwnd!\n");
        return FALSE;
    }

	pMultiSlob = (CMultiSlob *) (pActiveSlob->GetSelection()) ;

	if (pMultiSlob == NULL ||
		!pMultiSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)) ||
		pMultiSlob->GetContentList() == NULL ||
		pMultiSlob->GetContentList()->GetCount() == 0)
	{
	
		TRACE("  ** invalid selection!\n");
		return FALSE;
	}

   	pSlob = (CSlob *)pMultiSlob->GetDominant();

    if (pSlob == NULL)
    {
        TRACE(" ** no active slob!\n");
        return FALSE;
    }

	if (pSlob->GetStrProp(nPropID, str) != valid)
	{
	  // Attempt getting an INT property.
	   char sz[20] ; 

		if (pSlob->GetIntProp(nPropID, iProp) != valid)
		{
			CRect rct;

			if (pSlob->GetRectProp(nPropID, rct) != valid)
			{
				TRACE("  ** Unknown property!\n") ;
				return FALSE; 
			}
			else
			{
				_itoa(rct.top, sz, 10);
				str  = sz;
				str += _T(',');
	
				_itoa(rct.left, sz, 10);
				str += sz;
				str += _T(',');

				_itoa(rct.bottom, sz, 10);
				str += sz;
				str += _T(',');

				_itoa(rct.right, sz, 10);
				str += sz;
			}
		}
		else
		{
			_itoa(iProp,sz ,10) ;
		   str = sz ;
		}
	}
			
	ATOM atom = GlobalAddAtom((LPCSTR)str);
	TRACE1("Added Global Atom: %u\n", atom);

	return (LONG)atom;
}

LRESULT CMainFrame::OnSetItemProp(WPARAM nPropID, LPARAM lAtom)
{
	char szBuf[256];
	VERIFY(GlobalGetAtomName((ATOM)lAtom, szBuf, 255));

	TRACE2("Remote SetProp: %d = %s\n", nPropID, szBuf);

	CSlobWnd* pSlobWnd = GetActiveSlobWnd();
	if (pSlobWnd == NULL)
	{
		TRACE("  ** no current slob!\n");
		return FALSE;
	}

	pSlobWnd->GetSelection()->SetStrProp(nPropID, (LPSTR)szBuf);

	return TRUE;
}
#endif  // _TEST

void NYI()
{
	AfxMessageBox("Sorry, that has not been implemented yet!",
		MB_OK | MB_ICONSTOP);
}
