// docobtpl.cpp : implementation of the CDocObjectTemplate class
//

#include "stdafx.h"

#include <objbase.h>

#include "resource.h"
#include "docobvw.h"
#include "docobtpl.h"
#include "docobdoc.h"
#include "docobfrm.h"

#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#endif

#include <objext.h>
#include <vbaguids.h>

#include <cmguids.h>
#include <oleipcid.h>

IMPLEMENT_DYNAMIC(CDocObjectTemplate, CPartTemplate)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int nCurFilter = 0;
extern SRankedStringID arsiOpenFilters[];
extern const int nMaxArsiFilters;
/////////////////////////////////////////////////////////////////////////////

CDocObjectTemplate::CDocObjectTemplate(CPackage* pPackage, REFCLSID clsid,
							const CString &strDefaultExtension, 
							const CString &strDescName, 
							const CString &strDocBaseName, HICON hIcon,
							CRuntimeClass *pDocRuntimeClass,
							CRuntimeClass *pViewRuntimeClass)
	: CPartTemplate(IDR_DOCOBJECT,
					(pDocRuntimeClass != NULL) ? pDocRuntimeClass : RUNTIME_CLASS(CDocObjectDoc), 
					RUNTIME_CLASS(CDocObjectFrame),
					(pViewRuntimeClass != NULL) ? pViewRuntimeClass : RUNTIME_CLASS(CDocObjectView),
					pPackage, NULL, clsid),
	  m_pDocRuntimeClass(pDocRuntimeClass)
{
	int nStart = strDefaultExtension.Find('.');
	int nEnd = strDefaultExtension.Find(',');
	CString strExt = strDefaultExtension.Mid(nStart, nEnd - nStart);
	CString strRest = strDefaultExtension.Right(strDefaultExtension.GetLength() - nEnd - 1);
	strRest.TrimLeft();
	nEnd = strRest.Find('(');
	m_strFilter = strRest.Left(nEnd + 1);
//	m_strFilter += _T("*");
	nStart = strRest.Find('.');
	nEnd = strRest.Find(')');
	strRest = strRest.Mid(nStart, nEnd - nStart + 1);
	m_strFilter += strRest;

	if (!strDocBaseName.IsEmpty())
	{
		m_strDocStrings = _T("\n");
		m_strDocStrings += strDocBaseName;
		m_strDocStrings += _T("\n");
	}
	else
		m_strDocStrings = _T("\nDocObject\n");

	// following line allows File.New for DocObjects
	m_strDocStrings += strDescName;
	m_strDocStrings += _T("\n");
	m_strDocStrings += m_strFilter;
	m_strDocStrings += _T("\n");
	m_strDocStrings += strExt;
	m_strDocStrings += _T("\n");

	m_hIcon = hIcon;

	// Don't let the filters overflow the array!!!
	if(nCurFilter < nMaxArsiFilters)
		arsiOpenFilters[nCurFilter++].ids = (DWORD)(LPCTSTR)m_strFilter;
}

CDocTemplate::Confidence CDocObjectTemplate::MatchDocType(const TCHAR* pszPathName, CDocument*& rpDocMatch)
{
	if (theApp.m_bInvokedCommandLine)
	{
		// this is too expensive to check during a cmdline build
		return (noAttempt);
	}

	USES_CONVERSION;

	CLSID clsid;
	CDocTemplate::Confidence match;

	// See what CDocTemplate thinks.
	match = CDocTemplate::MatchDocType(pszPathName, rpDocMatch);

	if (match == yesAlreadyOpen)
		return(match);				// document is already open, return such
	else if (SUCCEEDED(GetClassFile(T2W(pszPathName), &clsid)) && clsid == m_clsid)
		return(yesAttemptNative);	// clsid matches so document matches exactly
	else if (match == yesAttemptNative)
		return(yesAttemptForeign);	// extension matches so document may match
	else
		return(noAttempt);
}

void CDocObjectTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc, BOOL bMakeVisible)
{
	// The frame has to become the active frame sooner than normal, because it is about to
	// undergo in-place activation, and MFC will ignore the server's IOleInPlaceFrame::SetMenu
	// call if the active COleClientItem is in an inactive CMDIChildWnd.
	pFrame->ActivateFrame();

	CPartFrame::FinishActivation();
	// otherwise defer to base class implementation.
	CPartTemplate::InitialUpdateFrame(pFrame, pDoc, bMakeVisible);

}

CDocument *CDocObjectTemplate::OpenDocumentFile(const TCHAR* pszPathName, BOOL bMakeVisible)
{

	CWaitCursor curWait;

	Menu::UseOLE(TRUE);		// We are entring DocObject mode
	Menu::UpdateMode(FALSE); // and update the mode
	DkDocObjectMode(TRUE);
	((CMainFrame*) theApp.m_pMainWnd)->m_pManager->IdleUpdateAvailableWnds();
	((CMainFrame *)theApp.m_pMainWnd)->LockLayout(TRUE);

	// Bypass CPartTemplate so we get correct name associated with with doc object.
	CDocument *pDoc = CMultiDocTemplate::OpenDocumentFile(pszPathName, bMakeVisible);

	if(pDoc)
	{
		CPartFrame *pFrame = (CPartFrame *)((CMainFrame *)AfxGetMainWnd())->GetActiveFrame();
		ASSERT(pFrame != AfxGetMainWnd());
		// If we don't have a valid hook, then init failed, let's clean-up
		if(pFrame->m_pNotifyHook == NULL)
		{
			CString str;
			MsgBox(Error, MsgText(str, IDS_DOCOBJ_CANT_OPEN, pszPathName ? pszPathName : _T("")));
			pFrame->DestroyWindow();
			pDoc = NULL;
		}
	}


	if(!pDoc)
	{
		Menu::UseOLE(FALSE);		// We failed DocObject mode
		Menu::UpdateMode(FALSE); // and update the mode
		DkDocObjectMode(FALSE);
	}
	else
	{
		// The base class method sets the flag for a menu rebuild.  This will hose us since
		// we want to use the menu created via SetMenus from the server -- so suppress it.
		theApp.m_bMenuDirty = FALSE;
	}

	((CMainFrame*) theApp.m_pMainWnd)->m_pManager->IdleUpdateAvailableWnds();
	((CMainFrame *)theApp.m_pMainWnd)->LockLayout(FALSE);
	((CMainFrame *)theApp.m_pMainWnd)->RecalcLayout();
	theApp.m_pMainWnd->RedrawWindow(NULL, NULL,
	            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);

	return pDoc;
}

/////////////////////////////////////////////////////////////////////////////

CRegistryKeyEnum::CRegistryKeyEnum(HKEY hkeyRoot, LPCTSTR szRequiredSubkey)
	: m_strRequiredSubkey(CString(szRequiredSubkey))
{
	m_ikey = 0;
	m_hkey = (HKEY)INVALID_HANDLE_VALUE;
	m_hkeyRoot = hkeyRoot;
	ASSERT(m_hkeyRoot != (HKEY)INVALID_HANDLE_VALUE);
}

CRegistryKeyEnum::~CRegistryKeyEnum()
{
	if (m_hkey != INVALID_HANDLE_VALUE)
		VERIFY(RegCloseKey(m_hkey) == ERROR_SUCCESS);
}

static HICON GetIconFromExecutable(const CString &strIconExe, int nIconNum);

BOOL CRegistryKeyEnum::Next(LPCTSTR szType)
{
	TCHAR szSubkey[256];
	ULONG cchSubkey;
	FILETIME filetime;
	m_strDefaultExtension.Empty();
	while (cchSubkey = 256, RegEnumKeyEx(m_hkeyRoot, m_ikey++, szSubkey, &cchSubkey, NULL, NULL, NULL, &filetime) == ERROR_SUCCESS)
	{
		if (m_hkey != INVALID_HANDLE_VALUE)
		{
			VERIFY(RegCloseKey(m_hkey) == ERROR_SUCCESS);
			m_hkey = (HKEY) INVALID_HANDLE_VALUE;
		}

		HKEY hkey = (HKEY)INVALID_HANDLE_VALUE;
		HKEY hClsKey = (HKEY)INVALID_HANDLE_VALUE;

		BOOL fReturn = FALSE;

		if (!m_strRequiredSubkey.IsEmpty())
		{
			// reject if it does not have the desired attribute
			HKEY hkeySub;
			CString strOpenKey = szSubkey;
			strOpenKey += "\\";
			strOpenKey += m_strRequiredSubkey;
			if (RegOpenKeyEx(m_hkeyRoot, strOpenKey, 0, KEY_READ, &hkeySub) != ERROR_SUCCESS)
			{
				hkeySub = (HKEY) INVALID_HANDLE_VALUE;
				goto CloseKeys;
			}

			VERIFY(RegCloseKey(hkeySub) == ERROR_SUCCESS);
			hkeySub = (HKEY) INVALID_HANDLE_VALUE;

			// get the CLSID string
			TCHAR szClsID[40];
			LONG cch = 40;
			if (RegOpenKeyEx(m_hkeyRoot, szSubkey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
				goto CloseKeys;
			if (RegQueryValue(hkey, "CLSID", szClsID, &cch) != ERROR_SUCCESS)
				goto CloseKeys;
			// get the default extension string from CLSID key
			TCHAR szClsKey[60], szBuf[256];
			cch = 256;
			_tcscpy(szClsKey, "CLSID\\");
			_tcscat(szClsKey, szClsID);
			if (RegOpenKeyEx(m_hkeyRoot, szClsKey, 0, KEY_READ, &hClsKey) != ERROR_SUCCESS)
			{
				// It turns out that if RegOpenKeyEx() fails, it sets the result HKEY to
				// all zeros, which then causes an assertion failure later on when
				// we try to close it because its value is no longer INVALID_HANDLE_VALUE.
				// To alleviate this, if the open failed, we'll explicitly set it to
				// INVALID_HANDLE_VALUE here.  My registry is hosed enough that I get to
				// find strange things like this... -mgrier 10/28/96
				hClsKey = (HKEY) INVALID_HANDLE_VALUE;
				goto CloseKeys;
			}

			// get the default extension--if there is none then don't return string
			if (RegQueryValue(hClsKey, "DefaultExtension", szBuf, &cch) != ERROR_SUCCESS)
				goto CloseKeys;

			m_strDefaultExtension = szBuf;
			// now attempt to get a default icon for the
			cch = 256;
			if (RegQueryValue(hClsKey, "DefaultIcon", szBuf, &cch) != ERROR_SUCCESS)
				goto CloseKeys;

			CString strDefIcon = szBuf;
			int nIndex = strDefIcon.Find(',');
			CString strIconExe = strDefIcon.Left(nIndex);
			CString strIconNum = strDefIcon.Right(strDefIcon.GetLength() - nIndex - 1);
			int nIconNum = atoi(strIconNum);
			m_hIcon = GetIconFromExecutable(strIconExe, nIconNum);
			if (m_hIcon == (HICON)-1)
				goto CloseKeys;
			// Get the Display name
			cch = 256;
			if(RegQueryValue(hClsKey, NULL, szBuf, &cch) != ERROR_SUCCESS)
				goto CloseKeys;
			m_strDescName = szBuf;
			// Get the base document name
			cch = 256;
			if(RegQueryValue(hClsKey, "AuxUserType\\2", szBuf, &cch) == ERROR_SUCCESS)
				m_strDocNameBase = szBuf;
			fReturn = TRUE;
		}
		else if (RegOpenKeyEx(m_hkeyRoot, szSubkey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		{
			// The key must have evaporated between when the enumeration started
			// and now.  Just move to the next one.  -mgrier 10/28/96
			continue;
		}
		else
			fReturn = TRUE;

CloseKeys:
		if(hClsKey != INVALID_HANDLE_VALUE)
			VERIFY(RegCloseKey(hClsKey) == ERROR_SUCCESS);
		if(hkey != INVALID_HANDLE_VALUE)
			VERIFY(RegCloseKey(hkey) == ERROR_SUCCESS);

		if (fReturn)
		{
			m_strKeyName = CString(szSubkey);
			return TRUE;	// found it
		}
		// continue looping to next key
	}
	// clean exit from loop
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

CTaskAllocator::CTaskAllocator()
{
	VERIFY(CoGetMalloc(MEMCTX_TASK, &m_pmalloc) == S_OK);
}

CTaskAllocator::CTaskAllocator(CTaskAllocator &taskalloc)
{
	(m_pmalloc = taskalloc.m_pmalloc)->AddRef();
}

CTaskAllocator::~CTaskAllocator()
{
	m_pmalloc->Release();
}

/////////////////////////////////////////////////////////////////////////////

static int nCounter = 0;
static int nCountTo = 0;
static HICON hIcon = NULL;

BOOL CALLBACK EnumResNameProc(HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam)
{
	if (nCounter++ == nCountTo)
	{
		hIcon = LoadIcon((HINSTANCE)hModule, lpszName);
		return(FALSE);
	}

	return(TRUE);
}

HICON GetIconFromExecutable(const CString &strIconExe, int nIconNum)
{
	nCounter = 0;
	nCountTo = nIconNum;
	hIcon = NULL;
	if (_access(strIconExe, 00) != 0)
		return((HICON)-1);
	HINSTANCE hModule = LoadLibraryEx(strIconExe, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (hModule == NULL)
		return((HICON)NULL);
	EnumResourceNames(hModule, RT_GROUP_ICON, (ENUMRESNAMEPROC)EnumResNameProc, NULL);
	FreeLibrary(hModule);
	return(hIcon);
}
