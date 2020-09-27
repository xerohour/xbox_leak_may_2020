// main.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <malloc.h>
#include <mbctype.h>
#include "about.h"
#include "string.h"
#include "bardockx.h"
#include "bardlgs.h"
#include "undoctl.h"
#include "tools.h"
#include "msgboxes.h"
#include "totd.h"
#include "findctrl.h"
#include "docobtpl.h"
#include "ipchost.h"
#include "utilauto.h"
#include "autostat.h"
#include "shell.h"
#include "fnwiztpl.h"
#include "commapi.h"
#include <ocdevo.h>
#include "toolexpt.h"
#include <ObjModel\appdefs.h>
#include "prxycust.h"
#include "shlsrvc.h"

#include "resource.h"

#ifdef _CMDLOG
IMPLEMENT_SERIAL(CCmdMap, CMapWordToOb, 1)
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#include "errno.h"
#endif

#include "toolexpt.h"

#include <bldapi.h>
#include <clvwapi.h>

// #include <initguid.h>
#include <clvwguid.h>
#include <ObjModel\appguid.h> //Shell Automation Guids
#include <aut1guid.h>
#include <srcguid.h>
#include <cmguids.h>    // component manager guids
#include <multinfo.h>
#include "bwizmgr.h"
#include <shlobj.h>
#include <shlapi_.h>
#include <shlguid_.h>


#define DEVSTUDIO_FOLDER_ICON_INDEX 6


// { 0c539790-12e4-11cf-b661-00aa004cd6d8 }
DEFINE_GUID(SID_SApplicationObject, 
0x0c539790, 0x12e4, 0x11cf, 0xb6, 0x61, 0x00, 0xaa, 0x00, 0x4c, 0xd6, 0xd8);

/////////////////////////////////////////////////////////////////////////////
// CTheApp

BOOL bEnableIdleToolbarUpdate = TRUE;
BOOL fForceIdleCycle = FALSE;

CInternalApp theInternalApp;

#define theClass CAppSlob
BEGIN_SLOBPROP_MAP(CAppSlob, CSlob)
	BOOL_PROP(PoppingProperties)
END_SLOBPROP_MAP()
#undef theClass

BEGIN_MESSAGE_MAP(CTheApp, CWinApp)
	//{{AFX_MSG_MAP(CTheApp)
	ON_UPDATE_COMMAND_UI(ID_PROJ_MRU_FILE1, OnUpdateRecentProjMenu)
	ON_COMMAND_EX_RANGE(ID_PROJ_MRU_FIRST, ID_PROJ_MRU_LAST, OnOpenRecentFile)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnOpenRecentFile)
//      ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAppSlob::CAppSlob()
{
    m_bPoppingProperties = TRUE;
    m_pCurrentSlob = NULL;
}

CAppSlob::~CAppSlob()
{
    if (m_pCurrentSlob != NULL)
		m_pCurrentSlob->RemoveDependant(this);
}

BOOL CAppSlob::SetSlobProp(UINT nID, CSlob* val)
{
    if (nID == P_CurrentSlob && val != m_pCurrentSlob)
	{
		if (m_pCurrentSlob != NULL)
			m_pCurrentSlob->RemoveDependant(this);

		if (val != NULL)
			val->AddDependant(this);

		TRACE1("CAppSlob::SetSlobProp: Selecting 0x%08lx\n", val);
		m_pCurrentSlob = val;

		SetPropertyBrowserSelection(val);

		InformDependants(nID);
		return TRUE;
    }

    return CSlob::SetSlobProp(nID, val);
}

void CAppSlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
    if (idChange == SN_DESTROY)
    {
		if (m_pCurrentSlob == pChangedSlob)
		{
			if (GetPropertyBrowser() != NULL)
			{
				// Update the prop sheet right away so we don't have
				// dangling slob pointers...

				TRACE("Current slob was destroyed; selecting NULL\n");
						GetPropertyBrowser()->CurrentSlobDestroyed();
			}

			m_pCurrentSlob = NULL;
			InformDependants(SN_ALL);
		}
    }
    else if (idChange == SN_ALL)
    {
		if (GetPropertyBrowser() != NULL)
		{
			SetPropertyBrowserSelection(m_pCurrentSlob, -1,
				CSheetWnd::editorupdate);
		}
    }
    else
    {
		// Otherwise, tell our dependants that a property we reflect changed.
		InformDependants(idChange);
    }

    CSlob::OnInform(pChangedSlob, idChange, dwHint);
}

BEGIN_MESSAGE_MAP(CVshellPackage, CPackage)
	//{{AFX_MSG_MAP(CVshellPackage)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_HELP_MSPROD, OnHelpPSS)
	ON_COMMAND(IDM_HELP_EXTHELP, OnHelpExtHelp)
	ON_COMMAND(IDM_HELP_TIPOFTHEDAY, OnHelpTip)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(IDM_FILE_NEW_IN_PROJ, OnFileNewInProject)
	ON_COMMAND(IDM_FILE_SAVE_ALL, OnFileSaveAll)
	ON_COMMAND(ID_FILE_SAVE_ALL_EXIT, OnFileSaveAllExit)
	ON_COMMAND(IDM_TOOLBAR_EDIT, OnToolbarOptions)
	ON_COMMAND(IDM_TOGGLE_MDI, OnToggleMDI)
	ON_COMMAND(ID_REPEAT_CMD, OnCmdRepeat)
	ON_COMMAND(ID_REPEAT_CMD0, OnCmdRepeat0)
	ON_COMMAND(ID_REPEAT_CMD1, OnCmdRepeat1)
	ON_COMMAND(ID_REPEAT_CMD2, OnCmdRepeat2)
	ON_COMMAND(ID_REPEAT_CMD3, OnCmdRepeat3)
	ON_COMMAND(ID_REPEAT_CMD4, OnCmdRepeat4)
	ON_COMMAND(ID_REPEAT_CMD5, OnCmdRepeat5)
	ON_COMMAND(ID_REPEAT_CMD6, OnCmdRepeat6)
	ON_COMMAND(ID_REPEAT_CMD7, OnCmdRepeat7)
	ON_COMMAND(ID_REPEAT_CMD8, OnCmdRepeat8)
	ON_COMMAND(ID_REPEAT_CMD9, OnCmdRepeat9)
	ON_COMMAND(IDM_CUSTOMIZE, OnCustomize)
	ON_COMMAND(IDM_TOOLBAR_CUSTOMIZE, OnToolbarCustomize)
	ON_COMMAND(IDM_OPTIONS, OnOptions)
	ON_COMMAND(IDM_INSERT_FILES, OnInsertFiles)
	ON_COMMAND(ID_WINDOW_HIDE, OnWindowHide)
	ON_COMMAND(ID_WINDOW_POPUP_HIDE, OnWindowHide)

	ON_COMMAND_RANGE(IDM_HELP_MOW_FIRST, IDM_HELP_MOW_LAST, OnHelpMSOnTheWeb)

	ON_UPDATE_COMMAND_UI(ID_APP_ABOUT,      OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_HELP_TIPOFTHEDAY, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_HELP_EXTHELP, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN,      OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW,       OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_FILE_NEW_IN_PROJ, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVE_ALL, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SEND, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_TOOLBAR_EDIT, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_CUSTOMIZE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_TOOLBAR_CUSTOMIZE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_OPTIONS, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_FILES, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(IDM_TOGGLE_MDI, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_VERT, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSE_ALL, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_HIDE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_POPUP_HIDE, OnUpdateCommandUI)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateCommandUI)

	ON_UPDATE_COMMAND_UI_RANGE(IDM_USERTOOLS_BASE, IDM_USERTOOLS_BASE + 15, OnUpdateCommandUI_Tools)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_HELP_MOW_FIRST, IDM_HELP_MOW_LAST, OnUpdateCommandUI_MSOnTheWeb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CVshellPackage, CPackage)
	INTERFACE_PART(CVshellPackage, IID_IAutomationItems, AutomationItems)
	INTERFACE_PART(CVshellPackage, IID_IRecordNotifyShell, RecordNotifyShell)
END_INTERFACE_MAP()

void CVshellPackage::OnAppAbout()
{
	theApp.OnAppAbout();
}

BOOL CVshellPackage::OnInit()
{
	USES_CONVERSION;

	CPackage::OnInit();

	if (theApp.m_bInvokedCommandLine && theApp.m_bRunInvisibly)
		return TRUE; // skip all this


	// enumerate all doc objects in the registry ... create a template for each kind.
	CRegistryKeyEnum rke(HKEY_CLASSES_ROOT, __T("DocObject"));
	while (rke.Next())
	{
		CLSID clsid;
		if (CLSIDFromProgID(A2W(LPCTSTR(rke.m_strKeyName)), &clsid) != ERROR_SUCCESS)
			continue;       // might be the CLSID key or some other non-ProgID

		// don't create doctemplate for DocObjects with no default extension
		if (rke.m_strDefaultExtension.IsEmpty())
			continue;

		// fix by martynl to be more tolerant of bad registry entries; specifically those missing a dot at the front, which some appwizards generate in some circumstances
		if(rke.m_strDefaultExtension[0]!='.')
		{
			rke.m_strDefaultExtension="."+rke.m_strDefaultExtension;
		}

		CDocObjectTemplate *ptemp = new CDocObjectTemplate(this, clsid,
			rke.m_strDefaultExtension, rke.m_strDescName, rke.m_strDocNameBase, rke.m_hIcon);

		theApp.AddDocTemplate(ptemp);
	}

	///////////////////////////////////
	// must build this key ourselves.  we can not use theApp.GetRegistryKeyName()
	// as this builds a key that takes into account the /i switch.  since these
	// settings are in HKLM and /i is an HKCU setting, we would end up with 
	// a bogus HKLM path.
	CString         strKey;
	strKey = g_szKeyRoot;
	strKey += _T("\\");
	strKey += g_szCompanyName;
	strKey += _T("\\");
	strKey += theApp.GetExeString(DefaultRegKeyName);
	strKey += _T("\\Component Categories");
	CString         strDef = _T("Default Component");
	CString         strDesc = _T("Description");
	HKEY            hKeyFileNewBuilder;
	LPOLESTR        szCategoryID = NULL;
	CString         strFileNewBuilderID;

	// convert the category id for FileNewBuilder from guid to a string
	// concatenate it to the key above and open the reg key
	::StringFromCLSID(CATID_NewFileBuilder, &szCategoryID);
	strFileNewBuilderID = OLE2T(szCategoryID);
	AfxFreeTaskMem(szCategoryID);
	szCategoryID = NULL;

	strKey += _T("\\");
	strKey += strFileNewBuilderID;

	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, LPCTSTR(strKey), 0,
		KEY_READ, &hKeyFileNewBuilder))
	{

		DWORD   dwNumOfKeys = 0;
		DWORD   dwMaxKeyLen = 0;

		// get the number of sub keys under this category in the registry
		if(ERROR_SUCCESS == ::RegQueryInfoKey(hKeyFileNewBuilder, NULL, NULL,
			NULL, &dwNumOfKeys, &dwMaxKeyLen, NULL, NULL, NULL, NULL, NULL,
			NULL))
		{

			DWORD   dwSize;
			DWORD   dwDescSize;
			DWORD   dwType;
			DWORD   dwMaxDescLen = 0;
			TCHAR*  pszKey = new TCHAR[++dwMaxKeyLen];
			CLSID   clsid;
			CString strDescOut;
			CString strIcon;
			LPTSTR  pszDescOut;
			HKEY    hSubKey = NULL;

			CFileNewWizardTemplate* pFNWizTpl;

			// classes used for FileNewWizard ExtractIcon
			CString strDefaultIconKey;
			CString strIconPath;
	 
			// now that we have the number of subkeys, in a loop, get the
			// description of each one and add it to application as new templates
			for(DWORD i = 0; i < dwNumOfKeys; i++)
			{
				dwSize = dwMaxKeyLen;

				// assumption:
				// when any regkey operations fail, just continue.
				// don't know how else to handle the error if we can't get
				// the key, for now, just continue

				if (ERROR_SUCCESS != ::RegEnumKeyEx(hKeyFileNewBuilder, i,
					pszKey, &dwSize, NULL, NULL, NULL, NULL))
					continue;

				// We do not want the "Default Component" sub key
				if(0 == _tcscmp(LPCTSTR(strDef), pszKey))
					continue;

				if(SUCCEEDED(::CLSIDFromString(T2OLE(pszKey), &clsid)))
				{
					CString strDescKey = _T("CLSID\\");
					strDescKey += pszKey;
					// Open clsid key. Value of key is the description
					if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, strDescKey, 0, KEY_READ, &hSubKey))
						continue;

					// Alloc mem for description and read it in
					DWORD dwLength = 1024;
					LPTSTR pszDesc = strDesc.GetBuffer(dwLength);
					DWORD regErr = ::RegQueryValueEx(hSubKey, NULL, 0, &dwType, (LPBYTE)pszDesc, &dwLength);
					strDesc.ReleaseBuffer();
					::RegCloseKey(hSubKey);
					hSubKey = NULL;

					if(regErr != ERROR_SUCCESS)
						continue;

					ASSERT(REG_SZ == dwType);
					ASSERT(dwLength);

					// Now extract the Icon 
					strDefaultIconKey = _T("CLSID\\");
					strDefaultIconKey += pszKey;
					strDefaultIconKey += _T("\\DefaultIcon");
					// Open Default Icon Key
					HICON hIcon = NULL;
					if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, strDefaultIconKey, 0, KEY_READ, &hSubKey))
						continue;
												
					// Alloc mem for icon path, and read it in
					dwLength = 1024;
					LPTSTR pszIcon = strIcon.GetBuffer(dwLength);
					regErr = ::RegQueryValueEx(hSubKey, NULL, 0, &dwType, (LPBYTE)pszIcon, &dwLength);
					::RegCloseKey(hSubKey);
					hSubKey = NULL;
					if(regErr != ERROR_SUCCESS)
					{
						strIcon.ReleaseBuffer();
						continue;
					}
					ASSERT(REG_SZ == dwType);
					ASSERT(dwLength);

					// Get Icon Index
					TCHAR *pch = _tcsrchr(pszIcon, _T(','));
					int nOffset = 0;
					if(pch)
					{
						pch[0] = _T('\0');
						nOffset = _ttoi(&pch[1]);
					}
					// Get small (16x16) icon
					::ExtractIconEx(pszIcon, nOffset, NULL, &hIcon, 1);
					ASSERT(hIcon);

					// if they didn't specify an icon, or gave us a bogus offset
					// let's give them one of ours
					if(NULL == hIcon)
						hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_FILENEWWIZDEFAULT));

					strIcon.ReleaseBuffer();

					// Finally create new FileNewWizardTemplate
					pFNWizTpl = new CFileNewWizardTemplate(this, clsid,
						strDesc, hIcon);

					// register the template with the app
					theApp.AddDocTemplate(pFNWizTpl);
				}
			}
			delete [] pszKey;
		}
		::RegCloseKey(hKeyFileNewBuilder);
	}

	return TRUE;
}

HRESULT DevStudioLoadStreamFromRegistry(LPCTSTR szSection, LPCTSTR szKey, const CLSID &rclsid,
										REFIID riid, LPVOID *ppvUnknown,
										IUnknown *pIUnknownOuter, CLSCTX clsctx)
{
	HRESULT hr = NOERROR;

	*ppvUnknown = NULL;

	IStream *pIStream = ::GetRegIStream(szSection, szKey);
	if (pIStream != NULL)
	{
		LPVOID pvTemp = NULL;

		hr = ::CoCreateInstance(rclsid, pIUnknownOuter, clsctx, IID_IPersistStream, &pvTemp);
		if (SUCCEEDED(hr))
		{
			IPersistStream *pIPersistStream = reinterpret_cast<IPersistStream *>(pvTemp);

			hr = pIPersistStream->Load(pIStream);
			if (SUCCEEDED(hr))
				hr = pIPersistStream->QueryInterface(riid, ppvUnknown);

			pIPersistStream->Release();
			pIPersistStream = NULL;
		}
	
		if (pIStream != NULL)
		{
			pIStream->Release();
			pIStream = NULL;
		}
	}

	return hr;
}

HRESULT DevStudioPersistStreamToRegistry(IUnknown *pIUnknown, LPCTSTR pctstrKey, LPCTSTR pctstrValue)
{
	HRESULT hr;
	IStream *pIStream = NULL;
	hr = ::CreateStreamOnHGlobal(NULL, TRUE, &pIStream);
	if (FAILED(hr))
		return hr;

	IPersistStream *pIPersistStream = NULL;
	LPVOID pvTemp = NULL;
	hr = pIUnknown->QueryInterface(IID_IPersistStream, &pvTemp);

	if (FAILED(hr))
	{
		pIStream->Release();
		return hr;
	}

	pIPersistStream = reinterpret_cast<IPersistStream *>(pvTemp);

	hr = pIPersistStream->Save(pIStream, TRUE);
	if (FAILED(hr))
	{
		pIStream->Release();
		pIPersistStream->Release();
		return hr;
	}

	::WriteRegIStream(pctstrKey, pctstrValue, pIStream);

	pIPersistStream->Release();
	pIStream->Release();

	return NOERROR;
}

BOOL CVshellPackage::CanExit()
{
	for (int i = 0; i < toolList.NumTools(); i++)
	{
		CTool *pTool = toolList.GetTool(i);
		if (pTool->m_pSpawner != NULL)
		{
			MsgBox(Error, IDS_ERR_TOOL_RUNNING);
			return FALSE;
		}
	}
	return TRUE;
}

void CVshellPackage::OnPreExit()
{
	ReleaseHelpInterface();
}

void CVshellPackage::OnToolbarOptions()
{
	// !!!
}

void CVshellPackage::OnWindowHide()
{
	CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
	if (pDocker == NULL)
	{
		CMDIChildDock* pMDIChild = (CMDIChildDock*)
			((CMDIFrameWnd*) theApp.m_pMainWnd)->MDIGetActive();
		if (pMDIChild != NULL && pMDIChild->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
			pDocker = pMDIChild->GetWorker();
	}

	if (pDocker != NULL)
		pDocker->ShowWindow(FALSE);
}

void CVshellPackage::OnToggleMDI()
{
	CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
	if (pDocker == NULL)
	{
		CMDIChildDock* pMDIChild = (CMDIChildDock*)
			((CMDIFrameWnd*) theApp.m_pMainWnd)->MDIGetActive();
		if (pMDIChild != NULL && pMDIChild->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
			pDocker = pMDIChild->GetWorker();
	}

	if (pDocker != NULL)
		pDocker->ToggleMDIState();
}

// this array should contain enough slots for all doc object
// description strings, plus an extra for NULL termination.

// third field in SRankedStringID structure is CLSID of project type
// for which the filter should be ranked first if ranks are identical

// REVIEW [fabriced]: this array should be sized dynamically
extern const int nMaxArsiFilters = 16;
SRankedStringID arsiOpenFilters[] =
{
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE,
	NULL,   1000,   &CLSID_NULL,  FALSE
};

void CVshellPackage::GetOpenDialogInfo(SRankedStringID** ppFilters, 
	SRankedStringID** ppEditors, int nOpenDialog)
{
	ASSERT (ppEditors != NULL);
	ASSERT (ppFilters != NULL);
	*ppEditors = NULL;

	switch (nOpenDialog)
	{
		case GODI_FILEOPEN:
		case GODI_ADDFILE:
			*ppFilters = arsiOpenFilters;
			return;
		case GODI_FINDINFILES:
		case GODI_RESIMPORT:
			*ppFilters = NULL;
			return;
	}
}

void CTheApp::BeginSetRepeatCount(INT cRpt, BOOL fReplacable /* = FALSE */)
{
	// The negative arg indicates that we're augmenting the repeat count
	// by invoking the command again
	if (cRpt < 0)
		SetRepCount(max(GetRepCount(), 4 * GetRepCount()));
	else
		SetRepCount(cRpt);

	m_SettingRepeatCount = FinishedNot;
	m_fReplaceRepeatCount = fReplacable;

	UpdateRepeatStatus();
}

void CTheApp::EndSetRepeatCount(REPEAT_COUNT_STATE rcs)
{
	m_SettingRepeatCount = rcs;
	UpdateRepeatStatus();
}

void CTheApp::UpdateRepCount(INT i)
{
	// If we've just begun a new repeat count, this number
	// should replace the existing count
	if (m_fReplaceRepeatCount)
	{
		SetRepCount(i);
		m_fReplaceRepeatCount = FALSE;
	}
	else
	{
		// Otherwise, it should augment the existing count
		SetRepCount(max(GetRepCount(), 10 * GetRepCount() + i));
	}
}

void CTheApp::UpdateRepeatStatus()
{
	static CString  strRepeat;
	static BOOL             fInitd = FALSE;

	if (!fInitd)
	{
		strRepeat.LoadString(IDS_RepeatCount);
		fInitd = TRUE;
	}

	if (m_SettingRepeatCount != FinishedAbsolute)
	{
		CString strStatus;
		strStatus.Format("%s%d", strRepeat, GetRepCount());

		// Make sure this is reflected in the status bar
		SetIdlePrompt(strStatus, TRUE);
		SetPrompt(strStatus, TRUE);
	}
	else if (m_SettingRepeatCount == FinishedAbsolute)
	{
		SetIdlePrompt(AFX_IDS_IDLEMESSAGE, TRUE);
		SetPrompt(AFX_IDS_IDLEMESSAGE, TRUE);
		m_SettingRepeatCount = DoneRepeating;
	}
}

BOOL CTheApp::FCmdRepeatPretranslate(MSG *pMsg)
{
	BOOL    fRet = FALSE;
	WPARAM  wParam = pMsg->wParam;
	TCHAR   tch = (TCHAR)wParam;

	switch (pMsg->message)
	{
		case WM_CHAR:

			if (tch >= _T('0') && tch <= _T('9'))
				break;
			else if (tch == '-')
			{
				SetRepCount(-GetRepCount());
				fRet = TRUE;
			}
			else
				EndSetRepeatCount(FinishedAbsolute);
			break;

		case WM_KEYDOWN:

			if (tch >= _T('0') && tch <= _T('9'))
			{
				UpdateRepCount(_ttoi(&tch));
				fRet = TRUE;
			}
			else if (wParam == VK_SUBTRACT)
				fRet = TRUE;
			else
			{
				// We don't absolutely know if we're finished yet.  If the next
				// thing we get is a wm_char that's a digit or '-', we're not done.
				EndSetRepeatCount(FinishedMaybe);
			}

			break;

		default:
			break;
	}

	UpdateRepeatStatus();

	return fRet;
}


void CVshellPackage::OnCmdRepeat()
{
	theApp.BeginSetRepeatCount(-1, TRUE);
}

void CVshellPackage::OnCmdRepeat0()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(0);
	else
		theApp.BeginSetRepeatCount(0);
}

void CVshellPackage::OnCmdRepeat1()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(1);
	else
		theApp.BeginSetRepeatCount(1);
}

void CVshellPackage::OnCmdRepeat2()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(2);
	else
		theApp.BeginSetRepeatCount(2);
}

void CVshellPackage::OnCmdRepeat3()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(3);
	else
		theApp.BeginSetRepeatCount(3);
}

void CVshellPackage::OnCmdRepeat4()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(4);
	else
		theApp.BeginSetRepeatCount(4);
}

void CVshellPackage::OnCmdRepeat5()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(5);
	else
		theApp.BeginSetRepeatCount(5);
}

void CVshellPackage::OnCmdRepeat6()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(6);
	else
		theApp.BeginSetRepeatCount(6);
}

void CVshellPackage::OnCmdRepeat7()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(7);
	else
		theApp.BeginSetRepeatCount(7);
}

void CVshellPackage::OnCmdRepeat8()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(8);
	else
		theApp.BeginSetRepeatCount(8);
}

void CVshellPackage::OnCmdRepeat9()
{
	if (theApp.FSettingRepeatCount())
		theApp.UpdateRepCount(9);
	else
		theApp.BeginSetRepeatCount(9);
}

void CVshellPackage::OnUpdateCommandUI(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case IDM_FILE_SAVE_ALL:
		pCmdUI->Enable(CPartDoc::CanSaveAll());
		break;

	case ID_FILE_SAVE:
	case ID_FILE_CLOSE:
	case ID_WINDOW_CASCADE:
	case ID_WINDOW_TILE_HORZ:
	case ID_WINDOW_TILE_VERT:
	case ID_WINDOW_CLOSE_ALL:
		pCmdUI->Enable(((CMainFrame*) theApp.m_pMainWnd)->MDIGetActive() != NULL);
		break;

	case ID_WINDOW_POPUP_HIDE:
		if (CDockWorker::s_pActiveDocker == NULL)
		{
			CString str;
			VERIFY(str.LoadString(IDS_POPUP_CLOSE));

			pCmdUI->SetText(str);
		}
		pCmdUI->Enable(TRUE);
		break;

	case ID_WINDOW_HIDE:
		pCmdUI->Enable(CDockWorker::s_pActiveDocker != NULL);
		break;

	case IDM_TOGGLE_MDI:
		{
			CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
			if (pDocker == NULL)
			{
				CMDIChildDock* pMDIChild = (CMDIChildDock*)
					((CMDIFrameWnd*) theApp.m_pMainWnd)->MDIGetActive();
				if (pMDIChild != NULL && pMDIChild->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
					pDocker = pMDIChild->GetWorker();
			}

			pCmdUI->Enable(pDocker!=NULL);

			if(pDocker)
			{
				pCmdUI->SetCheck(CDockWorker::s_pActiveDocker != NULL);
			}
		}
		break;

	case IDM_FILE_NEW_IN_PROJ:
	case IDM_INSERT_FILES:
	{
		IProjectWorkspace *pInterface;
		pInterface = g_IdeInterface.GetProjectWorkspace();
		if(pInterface)
		{
			BOOL fTempWorkspace = (pInterface->IsWorkspaceTemporary() == S_OK);
			if (fTempWorkspace)
			{
				pCmdUI->Enable(FALSE);
				break;
			}
			else
			{
				IPkgProject *pProject;
				pInterface->GetActiveProject(&pProject);
				pCmdUI->Enable(pProject != NULL);
				if (pProject)
				{
					if(pCmdUI->m_nID==IDM_INSERT_FILES)
					{

					}
					pProject->Release();
				}
				break;
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
			break;
		}
	}

	case ID_APP_ABOUT:
		if(IsPackageLoaded(PACKAGE_LANGFOR) || IsPackageLoaded(PACKAGE_LANGMST))
		{
			CString str;
			str.LoadString(IDS_ABOUT_DEVSTUD);
			pCmdUI->SetText(str);
		}
		pCmdUI->Enable(TRUE);

		break;

	case ID_FILE_OPEN:
	case ID_FILE_NEW:
	case IDM_HELP_TIPOFTHEDAY:
	case ID_APP_EXIT:
	case IDM_TOOLBAR_EDIT:
	case IDM_CUSTOMIZE:
	case IDM_TOOLBAR_CUSTOMIZE:
	case IDM_OPTIONS:
		pCmdUI->Enable(TRUE);
		break;

	case IDM_HELP_EXTHELP:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(UseExtHelp());
		break;

	default:
		pCmdUI->Enable(FALSE);
		break;
	}
}


////////////////////////////////////////////////////////////////////////////
// Microsoft on the Web
//
void CVshellPackage::OnUpdateCommandUI_MSOnTheWeb(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

////////////////////////////////////////////////////////////////////////////
// Tools menu custom item handling
//

static UINT nRunningTool = 0;

void CVshellPackage::OnUpdateCommandUI_Tools(CCmdUI* pCmdUI)
{
	ASSERT(pCmdUI->m_nID >= IDM_USERTOOLS_BASE);
	ASSERT(pCmdUI->m_nID <= IDM_USERTOOLS_LAST);

	CTool *pTool = toolList.GetTool(pCmdUI->m_nID - IDM_USERTOOLS_BASE);
	if (pTool != NULL)
	{
		BOOL bToolRunning = !(pTool->CanSpawn());

		if (bToolRunning)
		{
			static CString strPrefix;

			if (strPrefix.IsEmpty())
				strPrefix.LoadString(IDS_STOP_TOOL_PREFIX);
			// REVIEW [patbr]
			// used to add Ctrl+Break to menu; still possible to handle?

			CString str = strPrefix;
			str += pTool->m_strMenuName;
			pCmdUI->SetText(str);
		}
		else
		{
			extern BOOL GetCmdKeyString(UINT nCmdID, CString& str);  // from keymap.cpp
			CString strAccel;
			GetCmdKeyString(pCmdUI->m_nID, strAccel);
			pCmdUI->SetText(pTool->m_strMenuName + '\t' + strAccel);
		}
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVshellPackage::OnCommand_Tools(UINT nID)
{
	ASSERT(nID >= IDM_USERTOOLS_BASE);
	ASSERT(nID <= IDM_USERTOOLS_LAST);

	CTool *pTool = toolList.GetTool(nID - IDM_USERTOOLS_BASE);
	if (pTool->CanSpawn())
		pTool->Spawn();
	else
		pTool->CancelSpawn();
}

////////////////////////////////////////////////////////////////////////////

BOOL CVshellPackage::OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		if (nID >= IDM_USERTOOLS_BASE && nID <= IDM_USERTOOLS_LAST)
		{
			OnCommand_Tools(nID);
			return TRUE;
		}
	}

	return CPackage::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/* Implement only if different than EXE
LPCTSTR CVshellPackage::GetHelpFileName(HELPFILE_TYPE helpfileType)
{
	switch (helpfileType)
	{
		case APPLICATION_HELP:
			return _TEXT("xxx");

		case REFERENCE_HELP:
			return _TEXT("xxx");

		default:
			ASSERT(FALSE);
	}

	return CPackage::GetHelpFileName(helpfileType);
}
*/

HGLOBAL CVshellPackage::GetDockInfo()
{
	// When adding enties to rgdi[] make sure to add corresponding entries in
	// rgCaptionID[].
	static UINT rgCaptionID[] = {
		IDS_MENUBAR,
		IDS_STANDARD,
		IDS_FULLSCREEN
	};

    static DOCKINIT BASED_CODE rgdi[] = {
		{IDTB_MENUBAR,
	PACKAGE_SUSHI,
		PACKET_NIL,
	"",     // IDS_MENUBAR
	INIT_VISIBLE | INIT_CANHORZ | INIT_CANVERT | INIT_POSITION | INIT_ASKAVAILABLE | INIT_AVAILABLE,
	dtToolbar,
	dpTop,
	dpNil,
	{0,0,0,0}},

		{IDTB_STANDARD,
	PACKAGE_SUSHI,
		PACKET_NIL,
	"",     // IDS_STANDARD
	INIT_VISIBLE | INIT_CANHORZ | INIT_CANVERT | INIT_POSITION,
	dtToolbar,
	dpTop,
	dpNil,
	{0,0,0,0}},

		{IDTB_FULLSCREEN,
	PACKAGE_SUSHI,
		PACKET_NIL,
	"",     // IDS_FULLSCREEEN
	INIT_CANHORZ | INIT_CANVERT | INIT_POSITION | INIT_ASKAVAILABLE,
	dtToolbar,
	dpNil,
	dpNil,
	{0,0,0,0}},
	};

	CString str;
	for (int i = 0; i < sizeof(rgCaptionID) / sizeof(UINT); i++)
	{
		VERIFY(str.LoadString(rgCaptionID[i]));
		ASSERT(str.GetLength() <= _MAX_DOCK_CAPTION);
		lstrcpy(rgdi[i].szCaption, str);
	}

	return DkInfoData(sizeof(rgdi) / sizeof(DOCKINIT), rgdi);
}

HWND CVshellPackage::GetDockableWindow(UINT nID, HGLOBAL hglob)
{
	switch (nID)
	{
	case ID_EDIT_UNDO:
	case ID_EDIT_REDO:
		//BLOCK:
		{
			// Create off screen to avoid flashing with SetParent().
			CRect rect;

			// account for different possible sizes of the impending control
			rect.SetRect(-CDropControl::GetExpectedWidth(), -CDropControl::GetExpectedHeight(), 0, 0);

			CDropControl* pDropControl;
			if (nID == ID_EDIT_UNDO)
				pDropControl = new CUndoControl;
			else
				pDropControl = new CRedoControl;

			if (!pDropControl->Create(WS_CHILD | WS_VISIBLE,
					rect, AfxGetMainWnd(), nID))
			{
				delete pDropControl;
				return NULL;
			}

			return pDropControl->GetSafeHwnd();
	    }
	case ID_EDIT_FIND:
		//BLOCK:
		{
			// The find combo is smaller when we have a small screen
		   int cxScreen = ::GetSystemMetrics(SM_CXSCREEN) ;
				
			CRect rect(-208, -120, 0, 0);   // Off screen.
			if (UseWin4Look())
				rect.left += 9;

			// 750 is a somewhat arbitrary value, intended to allow for 800x600 screens where some clever utility has stolen some
			// realestate.
			if(cxScreen<750)
			{
				rect.left=-100;
			}

			return CreateFindCombo(
				WS_CHILD | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_DROPDOWN,
				&rect, AfxGetMainWnd()->GetSafeHwnd(), ID_EDIT_FIND);
		}
	case ID_PROJECT_COMBO:
		{
			CPackage* pPackage = theApp.GetPackage(PACKAGE_PRJSYS);
			return pPackage->GetDockableWindow(nID, hglob);
		}
	}

	return CPackage::GetDockableWindow(nID, hglob);
}

HGLOBAL CVshellPackage::GetToolbarData(UINT nID)
{
	// Custom bars have no default.
	if(nID>= IDTB_CUSTOM_BASE)
	{
		return NULL;
	}

	static UINT BASED_CODE full_buttons[] =
	{
		ID_WINDOW_FULLSCREEN
	};

	TOOLBARINIT tbi;
	tbi.nIDWnd = nID;

	switch(nID)
	{
	case IDTB_STANDARD:
		return theApp.GetStandardToolbarData();

	case IDTB_FULLSCREEN:
	    tbi.nIDCount = sizeof(full_buttons)/sizeof(UINT);
	    return DkToolbarData(&tbi, full_buttons);


	case IDTB_MENUBAR:
		{
			// translate the main menu into a toolbar
			// The menu bar is copied from the popup menu bar. This is by design, I think.
			CBMenuPopup *pMenu=theCmdCache.GetMenu(IDM_MENU_BAR);

			// must have already built menus by this point, I hope.
			ASSERT(pMenu!=NULL);

			// iterate over menu, extracting command ids
			int nItems=pMenu->GetMenuItemCount();
			UINT *pIds=new UINT[nItems];

			int iDest=0;
			for(int iItem=0; iItem<nItems; ++iItem)
			{
				CBMenuItem *pItem=pMenu->GetMenuItem(iItem);
				if(pItem->GetItemType()!=CBMenuItem::MIT_Separator)
				{
					pIds[iDest++]=pItem->GetCmdID();
				}
			}

			// set up for docking
			tbi.nIDCount = iDest;
			HGLOBAL hGlob=DkToolbarData(&tbi, pIds);

			// don't keep ids around
			delete pIds;

			return hGlob;
		}
	}

	return CPackage::GetToolbarData(nID);
}

BOOL CVshellPackage::AskAvailable(UINT nIDWnd)
{
	if (nIDWnd == IDTB_FULLSCREEN)
		return ((CMainFrame*) theApp.m_pMainWnd)->IsFullScreen();

	if(nIDWnd==IDTB_MENUBAR)
	{
		return Menu::IsShowingCmdBars();
	}

	return TRUE;
}

UINT CVshellPackage::CmdIDForToolTipKey(UINT nIDCmdOriginal)
{
	// Limited use virtual (bobz)
	// This can be called for a command with the CT_DYNAMIC_CMD_STRING flag set.
	// It allows the caller to specify a different command id to use to get the
	// key assignment for a tool tip. It exists primarily for commands that are
	// toolbar HWND controls that have a different sidedocked command, like the
	// Find control or Wizard Bar combos that otherwise lose their tooltips


	switch (nIDCmdOriginal)
	{
	default:
		break;  // fall thru to default handler
	case ID_EDIT_FIND:
		return ID_EDIT_FIND_COMBO;
	}

	return nIDCmdOriginal;
}



/////////////////////////////////////////////////////////////////////////////
// Interfaces implemented by CVshellPackage

/////////////////////////////////////////////////////////////////////////////
// IAutomationItems interface

// IUnknown methods

STDMETHODIMP_(ULONG) CVshellPackage::XAutomationItems::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CVshellPackage, AutomationItems)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CVshellPackage::XAutomationItems::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CVshellPackage, AutomationItems)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CVshellPackage::XAutomationItems::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CVshellPackage, AutomationItems)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

// IAutomationItems methods

STDMETHODIMP CVshellPackage::XAutomationItems::GetAutomationItems (CAutomationItem* pAutomationItems,
															   DWORD* pdwNumItems)
{
	METHOD_PROLOGUE_EX(CVshellPackage, AutomationItems)

	// Make sure automation items list is filled up
	pThis->RegisterTypeLibsIfNecessary();

	if (pAutomationItems == NULL)
	{
		// If caller gives us a NULL array, then just return how many
		//  named items we have
		*pdwNumItems = pThis->m_ptrlAutomationItems.GetCount();
		return S_OK;
	}

	// An allocated array has been passed.  Fill in as many entries as
	//  we can with the named items we collected on startup
	POSITION pos = pThis->m_ptrlAutomationItems.GetHeadPosition();
	DWORD i=0;
	while (pos != NULL && i < *pdwNumItems)
	{
		// Get next named item in our list, and copy it to caller's array
		CAutomationItem* pAutomationItem = pThis->m_ptrlAutomationItems.GetNext(pos);
		_tcscpy(pAutomationItems[i].m_szName, pAutomationItem->m_szName);
		memcpy(&(pAutomationItems[i].m_tlid), &(pAutomationItem->m_tlid), sizeof(GUID));
		memcpy(&(pAutomationItems[i].m_clsid), &(pAutomationItem->m_clsid), sizeof(GUID));
		pAutomationItems[i].m_wMajorVersion = pAutomationItem->m_wMajorVersion;
		pAutomationItems[i].m_wMinorVersion = pAutomationItem->m_wMinorVersion;
		i++;
	}
	*pdwNumItems = i;
	return S_OK;
}

STDMETHODIMP CVshellPackage::XAutomationItems::GetIDispatch (LPCTSTR szItemName,
															  IDispatch** ppDispatch)
{
	METHOD_PROLOGUE_EX(CVshellPackage, AutomationItems)
	*ppDispatch = theApp.GetPackageExtension(szItemName);
	if (*ppDispatch == NULL)
		return E_FAIL;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IRecordNotifyShell interface

// IUnknown methods

STDMETHODIMP_(ULONG) CVshellPackage::XRecordNotifyShell::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CVshellPackage::XRecordNotifyShell::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CVshellPackage::XRecordNotifyShell::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

// IRecordNotifyShell methods

// Everything turned to YES (recording session, recording cursor, recrod text)
STDMETHODIMP CVshellPackage::XRecordNotifyShell::OnStartRecord()
{
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ASSERT (pMainFrame != NULL);
	ASSERT_KINDOF(CMainFrame, pMainFrame);

	pThis->SetShouldRecordText(TRUE);
	pThis->SetRecordState(CTheApp::RecordOn);
	return S_OK;
}

// Everything turned to NO (recording session, recording cursor, recrod text)
STDMETHODIMP CVshellPackage::XRecordNotifyShell::OnStopRecord()
{
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ASSERT (pMainFrame != NULL);
	ASSERT_KINDOF(CMainFrame, pMainFrame);

	pThis->SetShouldRecordText(FALSE);
	pThis->SetRecordState(CTheApp::RecordOff);
	return S_OK;
}

// The user has paused recording a macro
//  In Recording Session (REC in statusbar)?            YES
//  Show Record Cursor?                                                         NO
//  Packages Record Text?                                                       NO
STDMETHODIMP CVshellPackage::XRecordNotifyShell::OnPauseRecord()
{
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ASSERT (pMainFrame != NULL);
	ASSERT_KINDOF(CMainFrame, pMainFrame);

	pThis->SetShouldRecordText(FALSE);
	pThis->SetRecordState(CTheApp::RecordPaused);
	return S_OK;
}

// The user has unpaused recording a macro
//  For now, the semantics are the same as beginning a recording session
//  (as far as the shell is concerned).
//  In Recording Session (REC in statusbar)?            YES
//  Show Record Cursor?                                                         YES
//  Packages Record Text?                                                       YES
STDMETHODIMP CVshellPackage::XRecordNotifyShell::OnResumeRecord()
{
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	return OnStartRecord();
}

// This is called because a macro is being run while another is recorded.
//  In Recording Session (REC in statusbar)?            YES
//  Show Record Cursor?                                                         YES
//  Packages Record Text?                                                       NO
STDMETHODIMP CVshellPackage::XRecordNotifyShell::SetShouldRecordText(BOOL bShouldRecordText)
{
	METHOD_PROLOGUE_EX(CVshellPackage, RecordNotifyShell)
	pThis->SetShouldRecordText(bShouldRecordText);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_COMMAND_MENU_(EmptyBar, 0)
END_POPUP_MENU()
 
POPDESC *CVshellPackage::GetMenuDescriptor(UINT nId)
{
	if(nId>=IDM_CUSTOMMENU_BASE && nId<=IDM_CUSTOMMENU_LAST)
	{
		(*(MENU_CONTEXT_POPUP(EmptyBar))).cmdID=nId;
		return MENU_CONTEXT_POPUP(EmptyBar);
	}
	else
	{
		return theApp.GetMenuDescriptor(nId);
	}
}

BOOL CVshellPackage::IsMenuVisible(UINT nId)
{
	if( nId==IDM_MENU_BUILD ||
		nId==IDM_MENU_DEBUG)
	{
		CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
		BOOL bDesignMenu=(nId==IDM_MENU_BUILD);
		BOOL bDesignMode=(pMainFrame->m_nManagerState & 1) == 0;

		if(bDesignMenu!=bDesignMode)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else if(nId==IDM_MENU_HELP_MSONTHEWEB)
	{
		return !theApp.m_bHideMicrosoftWebMenu;
	}
	else
	{
		return TRUE;
	}
}

BOOL CTheApp::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	CPackage*       pPackage = NULL;

	if (nID < 0xf000) // System menu.
	{
		// We haven't yet found the destination package
		POSITION pos = m_packages.GetHeadPosition();
		while (pos != NULL)
		{
			pPackage = (CPackage*)m_packages.GetNext(pos);
			if ((pPackage->m_flags & PKS_COMMANDS) != 0 &&
				pPackage->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			{
				return TRUE;
			}
		}
	}
	
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CTheApp::OnDirChange()
{
	((CMainFrame*) m_pMainWnd)->OnDirChange();
}

////////////////////////////////////////////////////////////////////////////


#ifdef PERF_MEASUREMENTS
/////////////////// Testing code for V4.X Perf release  /////////////////////////
//////////////// 8/25/95        fabriced
static HANDLE hBuildSynch;
static BOOL fBuildStarted = FALSE;
static char *(* OutputWindowGetLine)(ULONG iline, ULONG *pcb);
#define SLEEP_TIME      2500
//////////////// 11/21/95       konradt
static DWORD (* GetCurrentWorkingSet)(void);
static void QACurrentWorkingSet(LPCTSTR str = NULL);
static BOOL QATrimWorkingSet();

static void QARunUnderShell(char *Command)
{
  static STARTUPINFO SUInfo = 
	{sizeof(STARTUPINFO),0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static PROCESS_INFORMATION PInfo;
  static char buf[300], acCPBuf[100];

  int v = GetVersion();

  switch(LOBYTE(LOWORD(v)))    // examine high bit for major version #
  { 
    case 3:
		  sprintf(buf, "cmd.exe /c \"%s\"", Command );
		  break;
	case 4: //TODO: axe this mega-hack when CreateProcess works properly on Chicago
		  GetEnvironmentVariable("COMSPEC", acCPBuf, 100);
		  sprintf(buf, "%s /c %s", acCPBuf, Command);
			break;
  }

  if (CreateProcess(
	0,
	buf, 
	0,     // process sec. attr.
	    0,     // thread sec. attr.
	0,
	    0,
	    NULL,     // env.
	    NULL,     // dir.
	&SUInfo,
	    &PInfo))
	{
	  WaitForSingleObject(PInfo.hProcess, INFINITE);
	}
}

static void QAPump( CTheApp *pApp )
{
	Sleep(SLEEP_TIME);
	while(::PeekMessage(&pApp->m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) )
		pApp->PumpMessage();
	LONG lCount=0;
	while(pApp->OnIdle(lCount++));
	Sleep(SLEEP_TIME);
	while(::PeekMessage(&pApp->m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) )
		pApp->PumpMessage();
    lCount=0;
    while(pApp->OnIdle(lCount++));
	while(::PeekMessage(&pApp->m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) )
		pApp->PumpMessage();
}

static void QADumpOutputWindow( const char *logfile )
{
	FILE *outFile;
	char *szLine;
	ULONG cbLine;
	Sleep(SLEEP_TIME);
	outFile = fopen( logfile, "wt" );
	for (int i=0; szLine = (*OutputWindowGetLine)(i, &cbLine); i++ )
		fprintf( outFile, szLine );
	fclose(outFile);
}

#ifdef _DEBUG
static const char *szIdeModuleName = "MSVCSRCD";
#else
static const char *szIdeModuleName = "MSVCSRC";
#endif

static BOOL QADoBuild(CTheApp *pApp, const char *szOutputLog)
{
	QAPump( pApp );
	if (pApp->m_bPerfQA_WorkSet) 
		QATrimWorkingSet();
	pApp->OnCmdMsg( IDM_PROJITEM_BUILD,0,0,0);
	QAPump( pApp );
	Sleep(SLEEP_TIME);
	if(fBuildStarted)
	{
		WaitForSingleObject( hBuildSynch, INFINITE );
		fBuildStarted = FALSE;
	}
	else // Build error!
	{
		FILE *outFile;
		outFile = fopen( szOutputLog, "wt" );
		fprintf( outFile, "Error trying to spawn the build!\n" );
		fclose(outFile);
		return( FALSE );
	}
	QAPump( pApp );
	QADumpOutputWindow(szOutputLog);
	return( TRUE );
}

static void QADoDebug(CTheApp *pApp, const char *szOutputLog)
{
	QAPump( pApp );
	if (pApp->m_bPerfQA_WorkSet) 
	{
		QATrimWorkingSet();
		pApp->OnCmdMsg( IDM_RUN_TRACEINTO,0,0,0);
		QACurrentWorkingSet("after debug startup");
	}
	else
	{
		pApp->OnCmdMsg( IDM_RUN_TRACEINTO,0,0,0);
	}
	QAPump( pApp );
	pApp->OnCmdMsg( IDM_RUN_TRACEINTO,0,0,0);
	QAPump( pApp );
	pApp->OnCmdMsg( IDM_RUN_STOPDEBUGGING,0,0,0);
	QAPump( pApp );
	QADumpOutputWindow(szOutputLog);
}

static void QARunAutomation(CTheApp *pApp)
{
	HMODULE hIDEModule;
	hIDEModule = GetModuleHandle (szIdeModuleName);
	OutputWindowGetLine = (char *(*)(ULONG , ULONG *))GetProcAddress ( 
												hIDEModule, "OutputWindowGetLine");
	GetCurrentWorkingSet = (DWORD(*)(void))GetProcAddress (
												hIDEModule, "GetCurrentWorkingSet");
	ASSERT(GetCurrentWorkingSet != NULL);
	if (pApp->m_bPerfQA_WorkSet) 
		QACurrentWorkingSet("startup");
	hBuildSynch = CreateSemaphore( NULL, 0, 1, NULL);
	if (pApp->m_szPerfQA_Project == NULL)
		return;
	else
	{
		IProjectWorkspace *pInterface;
		pInterface = g_IdeInterface.GetProjectWorkspace();
		ASSERT(pInterface != NULL);
	    CDocument *pDoc;
		if (pApp->m_bPerfQA_WorkSet) 
		{
			QATrimWorkingSet();
			VERIFY(SUCCEEDED(pInterface->OpenWorkspace(&pDoc, pApp->m_szPerfQA_Project, FALSE);
			QACurrentWorkingSet("after project load");
		}
		else
		{
			VERIFY(SUCCEEDED(pInterface->OpenWorkspace(&pDoc, pApp->m_szPerfQA_Project, FALSE);
		}
		delete pApp->m_szPerfQA_Project;
	}
	QAPump( pApp );
	pApp->OnCmdMsg( ID_FILE_NEW_SOURCE,0,0,0);
	if(!QADoBuild(pApp, "buildfull.log"))
		return;
	QADoDebug(pApp, "debugfull.log");
	QARunUnderShell("minor.bat");
	if(!QADoBuild(pApp, "buildminor.log"))
		return;
	QADoDebug(pApp, "debugminor.log");
	QARunUnderShell("major.bat");
	if(!QADoBuild(pApp, "buildmajor.log"))
		return;
	QADoDebug(pApp, "debugmajor.log");
}


static void QACurrentWorkingSet(LPCTSTR str /* = NULL */)
{
	static LONG lCount = 0;
	FILE *outFile;
	DWORD current;
	current = (*GetCurrentWorkingSet)();
	current>>=10;
	switch(lCount++)
	{
	case 0:  /* startup.log in startup directory */
		outFile = fopen( "startup.log", "wt" );
		break;
	case 1:  /* workset.log in loaded project directory */
		outFile = fopen( "workset.log", "wt" );
		break;
	default: /* workset.log in loaded project directory */
		outFile = fopen( "workset.log", "at" );
		break;
	}
	if (!outFile) return;
	fprintf(outFile, "IDE working set size %s: %dKB\n", (str? str : "current"), current);
	fclose(outFile);
}

static BOOL QATrimWorkingSet()
{
	return SetProcessWorkingSetSize(
		GetCurrentProcess(),
		(DWORD)-1,
		(DWORD)-1
		);
}

///////////////////// End of testing code //////////////////////////////////
#endif  // PERF_MEASUREMENTS


void SushiRestoreKey(HKEY hKey, const CString& str)
{
#ifdef _WIN32
	if (!str.IsEmpty())
	{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return;

	// Get the LUID for restore privilege
	if (!LookupPrivilegeValue(NULL, SE_RESTORE_NAME,
		&tkp.Privileges[0].Luid))
			return;

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Get restore privilege for this process.
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0))
			return;

		RegRestoreKey(hKey, str, 0);

		// Reset privilege.
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_USED_FOR_ACCESS;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);
	}
#endif
}

#ifdef _DEBUG

BOOL g_bFatalExit = FALSE;

#ifndef _WIN32
CATCHBUF fatalExitBuf;

int CTheApp::Run()
{
	ShowSplashScreen(FALSE);

	int nRet;
	if ((nRet = Catch(fatalExitBuf)) != 0)
	{
		g_bFatalExit = TRUE;
		m_pMainWnd->DestroyWindow();
		return nRet;
	}
	TipOfTheDay(TRUE);
	return CWinApp::Run();

}
#endif  /* !_WIN32 */
#endif  /* _DEBUG */


#ifdef _WIN32

#define WM_NCCARETBLINK 0x118   /* special message that causes problems */

extern BOOL g_bInvokedHelp;	// defined in initexit.cpp

int CTheApp::Run()
{
	if (!(m_bInvokedCommandLine && m_bRunInvisibly))
	{
		ShowSplashScreen(FALSE);
		((CMainFrame *)m_pMainWnd)->RebuildMenus();
		if (!m_jit.GetActive() && !m_fRunningMacro)
			TipOfTheDay(TRUE);
	}

	// Acquire and dispatch messages until a WM_QUIT message is received.
//      LONG lIdleCount = 0;
//      BOOL bMoreIdle = TRUE;
	HANDLE hDummyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	ASSERT(hDummyEvent);

	bEnableIdleToolbarUpdate = TRUE;


	// need to exit if invoked /make and no workspace loaded
	if (m_bInvokedCommandLine)
	{
		LPPROJECTWORKSPACE pProjSysIFace = g_IdeInterface.GetProjectWorkspace();
		BOOL bWorkspaceInit = ((pProjSysIFace != NULL) && (pProjSysIFace->IsWorkspaceInitialised() == S_OK));
        // if (pProjSysIFace != NULL) pProjSysIFace->Release();
		// can't release here

		if (g_bInvokedHelp)
		{
			WriteLog(NULL); // close stdout
			AfxGetMainWnd()->PostMessage(WM_CLOSE, 0L, 0);
		}
		else if (!bWorkspaceInit)
		{
			CString strOut;
			strOut.LoadString(IDS_ERR_NO_PROJECT_LOADED);
			WriteLog(strOut);
			WriteLog(NULL); // close stdout
			AfxGetMainWnd()->PostMessage(WM_CLOSE, 0L, 1);
		}
	}

#ifdef PERF_MEASUREMENTS
///////////////// Testing code for V4 perf release (fabriced)
	else if(m_bPerfQA_AutoRun)
	{
		QARunAutomation(this);
		QAPump(this);
		OnAppExit();
		PostQuitMessage(0);
	}
/////////////////////////////////////////////////////////////
#endif  // PERF_MEASUREMENTS

	MainLoop(hDummyEvent, FALSE, 0);

	// this should never have been set in this case.
	ASSERT(WaitForSingleObject(hDummyEvent, 0) == WAIT_TIMEOUT);

	CloseHandle(hDummyEvent);

	return ExitInstance();
}

#endif  /* _WIN32 */


#ifdef _DEBUG
int HeapTestCheck()
{
	return _heapchk();
}

static void HeapUseCheck()
{
	// Heap use diagnostics.
	static BOOL bHeapUseCheck = TRUE;
	static CMemoryState msCheck;
	static LPBYTE lpDummyData = NULL;       // ensure allocation dump stops

	// Adjust nCount in debugger to control debug output.
	static int nCount = 0;

	if (((CMainFrame*) theApp.m_pMainWnd)->MDIGetActive() != NULL)
		bHeapUseCheck = TRUE;
	else if (bHeapUseCheck)
	{
		bHeapUseCheck = FALSE;

		if (nCount > 1)
			msCheck.DumpAllObjectsSince();
		if (nCount > 0)
		{
			CMemoryState msNow, msDiff;
			msNow.Checkpoint();
			msDiff.Difference(msCheck, msNow);
			msDiff.DumpStatistics();

			delete lpDummyData;
			lpDummyData = new BYTE; // This will leak.
			msCheck.Checkpoint();
		}


#ifdef _TEST
		extern BOOL FillBufHeapUse(TCHAR *szBuf);

		TCHAR szBuf[256];
		FillBufHeapUse(szBuf);
		TRACE(szBuf);
#endif

	}
}
#endif
#ifdef METER_IDLE_TIME
#define STARTIDLETIMING	if (s_bIdleTime) g_IdleProcessingTime = ::GetTickCount();
#define STOPIDLETIMING	if (s_bIdleTime) g_IdleProcessingTime = ::GetTickCount() - g_IdleProcessingTime;
extern DWORD g_dwIdlePackageTime[];
extern DWORD g_IdleProcessingTime;
extern int g_idxIdlePackage;
extern BOOL s_bIdleTime;
BOOL TIMEDIDLE(CPackage *pPackage, LONG lCount)
{
	BOOL fRet;

	if (!s_bIdleTime)		// Fast in/out when not actually working
		return pPackage->OnIdle(lCount);

	g_dwIdlePackageTime[g_idxIdlePackage] = ::GetTickCount();
	fRet = pPackage->OnIdle(lCount);
	g_dwIdlePackageTime[g_idxIdlePackage] =
		::GetTickCount() - g_dwIdlePackageTime[g_idxIdlePackage];
	g_idxIdlePackage++;

	return fRet;
}
#else	//!METER_IDLE_TIME
#define TIMEDIDLE(pPackage, lCount) (pPackage->OnIdle(lCount))
#define STARTIDLETIMING
#define STOPIDLETIMING
#endif	//!METER_IDLE_TIME

BOOL CTheApp::OnIdle(long lCount)
{
#ifdef _DEBUG
#if 0 // display idle count
	{
		HWND hWnd = ((CMainFrame*)m_pMainWnd)->m_hWndMDIClient;
		HDC hDC = GetDC(hWnd);
		char szBuf [16];
		TextOut(hDC, 0, 0, szBuf, sprintf(szBuf, "%ld  ", lCount));
		ReleaseDC(hWnd, hDC);
	}
#endif
#endif

	// If a manager show is pending and a drag operation is in process,
	// idle processing may cause screen dirt.

	ASSERT(m_pMainWnd);
	if (((CMainFrame*) m_pMainWnd)->m_pManager != NULL &&
		((CMainFrame*) m_pMainWnd)->m_pManager->IsShowDelayed() &&
		::GetCapture() != NULL)
		return FALSE;

#if 1
	// don't allow idle processing for command-line builds
	if (m_bInvokedCommandLine && m_bRunInvisibly)
		return FALSE;
#endif

	BOOL bMore = FALSE;
	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*)m_packages.GetNext(pos);
		bMore |= ((pPackage->m_flags & PKS_IDLE)!= 0 &&
			TIMEDIDLE(pPackage, lCount));
	}

	m_bActivated = FALSE; // reset for next time

	STARTIDLETIMING
	switch(lCount)
	{
	case 0:
		if (m_bMenuDirty)
			((CMainFrame*) m_pMainWnd)->RebuildMenus();
		if (((CMainFrame*) m_pMainWnd)->m_pManager != NULL &&
			  ((CMainFrame*) m_pMainWnd)->m_pManager->NeedIdle())
			((CMainFrame*) m_pMainWnd)->m_pManager->IdleUpdateAvailableWnds();
		STOPIDLETIMING
		return TRUE;

	case 1:
		ASSERT_VALID(m_pMainWnd);
	    if (!m_pMainWnd->IsIconic())
	    {
			// Handle property sheet visibility.
			//
			PropPageIdle();
	    }
		StatusOnIdle();
		STOPIDLETIMING
		return TRUE;

	case 2:
		if (((CMainFrame*)m_pMainWnd)->m_dragDrop != NULL)
		{
			// if either the capture has been stolen
			// or we aren't the top app anymore, or the user has
			// the escape key pressed then abort
			// the drag and drop
			CWnd *capWnd = CWnd::GetCapture();
			CWnd *actWnd = CWnd::GetActiveWindow();

			// might be a floating window.
			while (actWnd != m_pMainWnd && actWnd != NULL)
				actWnd = actWnd->GetParent();

			if ((actWnd != m_pMainWnd) || (capWnd != m_pMainWnd) ||
				(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
			{
				((CMainFrame*)m_pMainWnd)->m_dragDrop->AbortDrop();
				delete ((CMainFrame*)m_pMainWnd)->m_dragDrop;
				((CMainFrame*)m_pMainWnd)->m_dragDrop =  NULL;
			}
			else
				((CMainFrame*)m_pMainWnd)->m_dragDrop->IdleUpdate();
		}
		STOPIDLETIMING
		return TRUE;

	default:
		// Obviously you will need to increment the PRE_MFC_IDLE_PROCESSING_COUNT number
		// below if you add any additional idle time processing here.
		if (bMore | CWinApp::OnIdle(lCount - PRE_MFC_IDLE_PROCESSING_COUNT))
		{
			STOPIDLETIMING
			return TRUE;
		}
		// Update floating toolbar commands last.
		ASSERT(m_pMainWnd);
		if (bEnableIdleToolbarUpdate && ((CMainFrame*) m_pMainWnd)->m_pManager != NULL)
		{
			((CMainFrame*) m_pMainWnd)->m_pManager->IdleUpdate();
		}

#ifdef _DEBUG
		HeapUseCheck();
#endif
		STOPIDLETIMING
		return FALSE;
	}
}

BOOL CTheApp::MainLoop(HANDLE hEventEarlyOut, BOOL fPushed, ULONG uReason)
{
	if (!fPushed)
	{
		// This is the application's main loop.  Process messages until
		// WM_QUIT is found and then return.
		//
		return IdleAndPump(hEventEarlyOut);
	}
	else
	{
		// Process messages until we hit WM_QUIT or the component
		// manager tells us to stop.  Changed 12/11/96 by Mgrier
		// for bug 18092 in DevStudio '96 wherein the infoview was
		// executing embedded VB5 controls very slowly; evidently
		// someone is mistakenly doing work when the FContinueMessageLoop()
		// is called, so we make sure to call it before trying to
		// pump messages. -mgrier 12/11/96
		while ((m_pMsoStdCompMgr == NULL) || (m_pMsoStdCompMgr->FContinueMessageLoop(NULL)))
		{
			if (!IdleAndPump(0))
				return FALSE;
		}

		return TRUE;
	}
}

// Process a single message from the message queue or perform idle-time
// processing.  This function is a worker for MainLoop.
BOOL CTheApp::CmpMgrDoIdle()
{
    BOOL fMore = FALSE;

	if (m_pMsoStdCompMgr == NULL)
		return FALSE;
		
	// Disable OLE message filter dialogs while in idle
	//                      (VOID)m_crpMessageFilter->FEnableDialogs(FALSE);
	
    // give the components a chance to do periodic
    // idle processing.
    //
    ASSERT(m_pAppCmpHost != NULL);
    
    IMsoComponentHost * pCompHost = m_pAppCmpHost->GetIMsoComponentHost();
    ASSERT(pCompHost != NULL);
    
    if (m_pMsoStdCompMgr->FDoIdle(msoidlefPeriodic))
	fMore = TRUE;
    else if(!pCompHost->FContinueIdle())
	fMore = TRUE;

    // Let the high priority component have some time:
    else if (m_pMsoStdCompMgr->FDoIdle(msoidlefPriority))
	fMore = TRUE;
    else if(!pCompHost->FContinueIdle())
	fMore = TRUE;

    // And give everybody else a shot:
    else if (m_pMsoStdCompMgr->FDoIdle(msoidlefNonPeriodic))
		fMore = TRUE;

//                      (VOID)m_crpMessageFilter->FEnableDialogs(TRUE);
	
    return fMore;
}

// this is basically stolen from MFC with the changes ifndef'd.
BOOL CTheApp::PumpMessage()
{
	ASSERT_VALID(this);

#ifndef NOIPCOMP
	if (m_pMsoStdCompMgr != NULL)
		m_pMsoStdCompMgr->OnWaitForMessage();
#endif

	if (!::GetMessage(&m_msgCur, NULL, NULL, NULL))
	{
#ifdef _DEBUG
		if (afxTraceFlags & traceAppMsg)
			TRACE0("CTheApp::PumpMessage - Received WM_QUIT.\n");
		m_nDisablePumpCount++; // application must die
			// Note: prevents calling message loop things in 'ExitInstance'
			// will never be decremented
#endif
		return FALSE;
	}

#ifdef _DEBUG
	if (m_nDisablePumpCount != 0)
	{
		TRACE0("Error: CTheApp::PumpMessage called when not permitted.\n");
		ASSERT(FALSE);
	}
#endif

	// process this message, and give component manager to handle it.
	// Note: Our CMainFrame is managed as a IMsoComponent, and registered with the component manager.
	// The implementation of CTheApp::PreTranslateMessage will be called when  
	// the component manager calls back to our IMsoComponent::FPreTranslateMessage.  
	// CIPCompMainFrame which the CMainFrame holds an AddRef pointer to, implements IMsoComponent.
	if (m_msgCur.message != WM_KICKIDLE 
#ifndef NOIPCOMP
		&& (m_pMsoStdCompMgr == NULL || 
			(m_pMsoStdCompMgr && !m_pMsoStdCompMgr->FPreTranslateMessage(&m_msgCur))))
#else
		&& !PreTranslateMessage(&m_msgCur))
#endif
	{
		::TranslateMessage(&m_msgCur);
		::DispatchMessage(&m_msgCur);
	}
	return TRUE;
}

// extern HANDLE g_hStdIn; // defined in initexit.cpp

BOOL CTheApp::IdleAndPump(HANDLE hEventEarlyOut /* =0 */)
{
	if ( hEventEarlyOut == 0 )
	{
		// do a one shot message
		LONG lIdleCount = 0;
		// check to see if we can do idle work
		while (m_msgCur.message != WM_NCCARETBLINK &&
			!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
#ifndef NOIPCOMP
			CmpMgrDoIdle())
#else
			OnIdle(lIdleCount))
#endif
		{
			// more work to do
			lIdleCount++;
		}

		// either we have a message, or OnIdle returned false

		return PumpMessage();
	}
	else
	{
		// loop until someone resets our early out flag or we receive WM_QUIT

		// it is important to start bMoreIdle at FALSE so that we don't
		// get an idle cycle whenever someone pushes a pump.  This happens
		// during builds after each spawn in particular [rm]

		LONG lIdleCount = 0;
		BOOL bMoreIdle = FALSE;
		for ( ; WaitForSingleObject(hEventEarlyOut, 0) == WAIT_TIMEOUT; )
		{
			if (bMoreIdle)
			{
				// check to see if we can do idle work
				while (!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
#ifndef NOIPCOMP
			(bMoreIdle = CmpMgrDoIdle())) 
#else
			(bMoreIdle = OnIdle(lIdleCount)))
#endif
				{
					// more work to do
					lIdleCount++;
				}
			}

#if 0
			if (m_bInvokedCommandLine /* && bSpawnActive */ /* UNDONE */)
			{
				// poll stdin for a ctrl-c
				char buffer[80];
				DWORD i, nBytesRead, nBytesAvail, nBytesLeft;
				if ((g_hStdIn != NULL) && (::PeekNamedPipe(g_hStdIn, buffer, 1, &nBytesRead, &nBytesAvail, &nBytesLeft)) && (nBytesRead > 0))

				{
					if (nBytesAvail > 80)
						nBytesAvail = 80;

					if (::ReadFile(g_hStdIn, buffer, nBytesAvail, &nBytesRead, NULL))
					{
						for (i = 0; i < nBytesRead; i++)

						{

							if (buffer[i] == '')
							{
								AfxGetMainWnd()->PostMessage(WM_COMMAND, IDM_PROJECT_STOP_BUILD);
								break;
							}
						}
					}
				}
			}
#endif
			// either we have a message, or OnIdle returned false

			if (!PumpMessage()) // this will block if no messages
				break;

			bEnableIdleToolbarUpdate = TRUE;
			if ( IsIdleMessage(&m_msgCur) )
			{
				bMoreIdle = TRUE;
				lIdleCount = 0;
			}
		}
	}
	return TRUE;
}

BOOL CTheApp::IsIdleMessage(MSG * pMsg)
{
	BOOL    fRet = TRUE;    // default return value is to kick idle into gear

	switch (pMsg->message)
	{
	// These messages DO NOT reset the idler..
	case WM_NCCARETBLINK:
	case WM_PAINT:
	case WM_TIMER:
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
	case WM_NULL:
		fRet = FALSE;
		break;

	case WM_KEYDOWN:
		if ((m_msgCur.lParam & 0x40000000L) )
		{
			//Don't reset for repeated keydown messages
			fRet = FALSE;
		}
		break;

	default:
		// Indicate messages that reset the idler
		break;

	}
	if(fForceIdleCycle)
	{
		fRet = TRUE;
		fForceIdleCycle = FALSE;
	}
	return fRet;
}

// Like CWinApp::OpenDocumentFile, but has option to open file via ShellExecute
CDocument* CTheApp::OpenDocumentFileEx(LPCSTR lpszFilename, BOOL bTryShellToo)
{
//      if( !bTryShellToo )
//              return OpenDocumentFile(lpszFilename);

	// NOTE: The following code is mostly copied directly from appui.cpp in
	// the MFC sources (some version a while ago).
    // The only difference is that after iterating through all the templates,
    // if the confidence level is not perfect (CDocTemplate::yesAttemptNative)
	// then we try to use ShellExecute to open the file.

	// find the highest confidence
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocument* pOpenDocument = NULL;

	// Fully qualify the path, if possible
	CPath path;

    LPCTSTR pszPath = NULL;
	if (path.Create(lpszFilename))
	    pszPath = path.GetFullPath();
    if (NULL == pszPath || !*pszPath)
	pszPath = lpszFilename;
	while (pos)
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

		CDocTemplate::Confidence match;
		ASSERT(pOpenDocument == NULL);
		match = pTemplate->MatchDocType(pszPath, pOpenDocument);
		if (match > bestMatch)
			bestMatch = match;
		if (match == CDocTemplate::yesAlreadyOpen)
		{
			// Dolphin #1946 [mannyv] need to go through OpenDocumentFile
			// to handle activation of open window
			return OpenDocumentFile(pszPath);
		}
	}

	// if the match confidence is not perfect, try ShellExecute
	if( bTryShellToo && bestMatch <= CDocTemplate::maybeAttemptForeign /*&& *path.GetExtension()*/ )
	{
		int iRet;

		iRet = (int) ShellExecute( m_pMainWnd->m_hWnd, NULL, pszPath, NULL, NULL, SW_SHOWNORMAL);
		if( iRet != SE_ERR_NOASSOC )
		{
			if (iRet == ERROR_FILE_NOT_FOUND ||
				iRet == ERROR_PATH_NOT_FOUND) {
				CString strError;
				CPath path;

				path.Create (pszPath);

				CString strErrMsg;
				VERIFY( strErrMsg.LoadString( IDS_CANNOTOPEN ) );
				MsgBox (Error, MsgText (strError, path, strErrMsg, &CFileException(iRet== ERROR_FILE_NOT_FOUND ? CFileException::fileNotFound : CFileException::badPath)));
				}
			// It either worked or failed for some funky reason.  In either case, we don't
			// try the templates again.
			return NULL;
		}
	}

	// try old OpenDocumentFile
	//return OpenDocumentFile(pszPath);
    return OpenDocumentFile(lpszFilename);
}

CPartFrame* CTheApp::GetOpenFrame(LPCSTR lpszFilename,
	CDocTemplate*& pTemplate, CDocument*& pDoc)
{
	CDocument* pOpenDoc;
	CDocTemplate* pOpenTemplate = GetOpenTemplate(lpszFilename, pOpenDoc);
	if (pOpenTemplate == NULL)
		return NULL;

	POSITION pos = pOpenDoc->GetFirstViewPosition();
	if (pos == NULL)
		return NULL;

	CView* pView = pOpenDoc->GetNextView(pos);
	if (pView == NULL)
		return NULL;

	CPartFrame* pFrame = (CPartFrame*) pView->GetParentFrame();
	if (pFrame == NULL || !pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
		return NULL;

	pTemplate = pOpenTemplate;
	pDoc = pOpenDoc;

	return pFrame;
}

HWND CTheApp::GetActiveView()
{
	if (CDockWorker::s_pActiveDocker != NULL)
	{
		CView* pView = CDockWorker::s_pActiveDocker->GetView();

		return pView->GetSafeHwnd();
	}
	else
	{
		CPartFrame *pFrame = (CPartFrame*)((CMDIFrameWnd *)m_pMainWnd)->MDIGetActive();

		if (pFrame == NULL || !pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
			return (HWND) NULL;

		CPartView *pSView = (CPartView *)pFrame->GetActiveView();
		if(pSView)
		{
			ASSERT_VALID (pSView);
			ASSERT (pSView->IsKindOf(RUNTIME_CLASS(CPartView)));
		}

		return pSView->GetSafeHwnd();
	}
}

void CTheApp::SetActiveView(HWND hwndView)
{
	CWnd* pWnd = CWnd::FromHandle(hwndView);
	CWnd* pWndParent = pWnd->GetParentFrame();

	if (pWndParent == NULL)
		return;

	if (pWndParent->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
	{
		CMDIChildWnd* pFrame = (CMDIChildWnd*) pWndParent;
		pFrame->ActivateFrame();

		if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
			pFrame->SetActiveView((CView*) pWnd, !CPartFrame::s_bLockFrame);
	}
	else if (pWndParent->IsKindOf(RUNTIME_CLASS(CPaletteWnd)))
	{
		// Bring the palette to the top of palette z-order, and set focus.
		((CPaletteWnd*) pWndParent)->ActivateNoFocus();

		pWnd->SetFocus();
	}
	else
	{
		// This must be a docked view.
		pWnd->SetFocus();
	}
}

void CTheApp::ReactivateView()
{
	if (CDockWorker::s_bLockDocker)
	{
		ASSERT(CDockWorker::s_pActiveDocker != NULL &&
			CDockWorker::s_pActiveDocker->m_pWnd != NULL);
		CDockWorker::s_pActiveDocker->m_pWnd->SetFocus();
	}
	else
	{
		m_pMainWnd->SetFocus();
	}
}

//      CTheApp::GetTemplate
//              Return the template given its CLSID
CPartTemplate* CTheApp::GetTemplate(REFCLSID clsid)
{
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CPartTemplate* pTemplate = (CPartTemplate*)GetNextDocTemplate(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
		// REVIEW: this is very, very bad performance-wise
		if (IsEqualGUID(pTemplate->GetTemplateClsid(), clsid))
			return pTemplate;
	}

	return NULL;
}

//      CTheApp::GetTemplate
//              Return the template given its automation type string
//   This is used in the implementation of Documents::Add.
CPartTemplate* CTheApp::GetTemplate(LPCTSTR szType)
{
	if (szType == NULL || !*szType)
		return NULL;

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CPartTemplate* pTemplate = (CPartTemplate*)GetNextDocTemplate(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
		// REVIEW: this is very, very bad performance-wise
		if (!_tcsicmp(pTemplate->GetTemplateAutomationType(), szType))
			return pTemplate;
	}

	return NULL;
}


//      CTheApp::GetOpenTemplate
//              Give the path name, return the template and document for the open
//              file with this path name, if it is open.
CPartTemplate* CTheApp::GetOpenTemplate(LPCTSTR lpszPathName, CDocument*& pDoc)
{
	// Fully qualify path
	CPath path;
	if (!path.Create(lpszPathName))
		return NULL;

	LPCTSTR lpszFullPath = path.GetFullPath();

	CDocument* pOpenDoc;
	CPartTemplate* pOpenTemplate;

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		pOpenTemplate = (CPartTemplate*)GetNextDocTemplate(pos);
		if (!pOpenTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)))
			continue;

		// We use the base CDocTemplate::MatchDocType because we are only interested
		// in whether the file is open.  Otherwise we would run through a lot of
		// file reading code that we don't care about.

		if (pOpenTemplate->CDocTemplate::MatchDocType(lpszFullPath, pOpenDoc)
			== CDocTemplate::yesAlreadyOpen)
		{
			pDoc = pOpenDoc;

			// When we use proxy templates, the template that opened and owns
			// the document might not be the one that truly reflects its type.
			CPartTemplate* pProxyTemplate = (CPartTemplate*)
				((CPartDoc*)pDoc)->GetDocLong(GDL_PROXYTEMPLATE);

			if(pProxyTemplate == NULL)
				return pOpenTemplate;
			else
				return pProxyTemplate;
		}
	}

	return NULL;
}

BOOL CTheApp::PreTranslateMessage(MSG* pMsg)
{
	// these messages don't get pre-translated because it helps
	// keep our working set down during a build [rm]
	switch (pMsg->message) {
		case WM_TIMER:
		case WM_NULL:
		case WM_NCCARETBLINK:
				return FALSE;
	}

#ifdef _DEBUG
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F12 &&
		(GetKeyState(VK_CONTROL) & ~1) != 0 &&
		(GetKeyState(VK_MENU) & ~1) != 0)
	{
		// Break on Ctrl+Alt+F12 so we can JIT Debug.
		AfxDebugBreak();
	}
#endif

	CPackage* pActiveUI = NULL;
	if (CDockWorker::s_pActiveDocker != NULL)
	{
		pActiveUI = GetPackage(CDockWorker::s_pActiveDocker->m_nIDPackage);
		ASSERT(pActiveUI != NULL);
	}
	else
	{
		// Give each package a chance to PreTranslate the message, but the
		// "active" package gets first crack...
		CMDIChildWnd* pActiveFrame;
		ASSERT(m_pMainWnd != NULL);
		if (m_pMainWnd != NULL && (pActiveFrame = ((CMDIFrameWnd*)m_pMainWnd)->MDIGetActive()) != NULL)
		{
			CDocument* pActiveDoc = pActiveFrame->GetActiveDocument();
			if (pActiveDoc != NULL &&
				pActiveDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)))
			{
				pActiveUI = ((CPartDoc*)pActiveDoc)->GetPackage();
			}
		}
	}

	if (pActiveUI != NULL && pActiveUI->PreTranslateMessage(pMsg))
		return TRUE;

	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*)m_packages.GetNext(pos);
		if (pPackage != pActiveUI &&
			(pPackage->m_flags & PKS_PRETRANSLATE) != 0 &&
			pPackage->PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
	}

	if (CWinApp::PreTranslateMessage(pMsg))
		return TRUE;

	return FALSE;
}

CPacket* CTheApp::GetPacket(UINT idPackage, UINT idPacket, BOOL bActivate /* =FALSE */)
{
	// Search the Packet list to find the specified one.  If the user
	// requested bActivate, then we will activate the packet (by
	// bringing it to the front of the list) if we find it.
	CPacket* pPacket;
	POSITION pos = m_packets.GetHeadPosition();
	while (pos != NULL)
	{
		pPacket = (CPacket*) m_packets.GetNext(pos);
		if ((pPacket->PacketID() == idPacket) && (pPacket->PackageID() == idPackage))
		{
			// found the right one
			if (bActivate)
				SetActivePacket(pPacket);

			return pPacket;
		}
	}

	if (bActivate)
		SetActivePacket(NULL);

	return NULL;
}

BOOL CTheApp::DeactivatePacket()
{
	SetActivePacket(NULL);
	return TRUE;
};

void CTheApp::SetActivePacket(CPacket* pPacket)
{
	TRACE("CTheApp::SetActivePacket: 0x%08x\n", pPacket);
	if (m_pActivePacket == pPacket)
	{
		// If the packet menu is not currently being used, then it needs
		// to be rebuilt and made the active menu.  (switch from OLE UI active).

		if (GetMenu(m_pMainWnd->GetSafeHwnd()) != ((CMainFrame*)m_pMainWnd)->m_hMenuDefault)
			m_bMenuDirty = TRUE;

		return;
	}

	m_pActivePacket = pPacket;
	m_bMenuDirty = TRUE;

	DkUpdateAvailableWnds();        // Delayed

	ASSERT_VALID(m_pMainWnd);
	if (pPacket)
		((CMainFrame*) m_pMainWnd)->m_statusBar.SetIndicators(pPacket->m_pSushiBarInfo);
	else
		((CMainFrame*) m_pMainWnd)->m_statusBar.SetIndicators();

}

void CTheApp::UnloadPackageBitmaps()
{
	CPackage* pPackage;

	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*) m_packages.GetNext(pos);
		pPackage->UnloadBitmap();
	}
}

#ifdef _DEBUG
void CTheApp::PrintCommandTables()
{
	theCmdCache.PrintCommandTable();
}
#endif

UINT CTheApp::GetCommandBtnInfo(UINT nCommand, CPackage** ppPackage)
{
	*ppPackage = NULL;
	if (nCommand == ID_SEPARATOR)
		return TBBS_SEPARATOR;

	CTE* pCTE = theCmdCache.GetCommandEntry(nCommand);
	// package is missing
	if (pCTE == NULL)
	{
		return TBBS_INVISIBLE;
	}

	// package is present, but has disabled command
	if((pCTE->flags & CT_NOUI) != 0)
	{
		return TBBS_INVISIBLE;
	}

	ASSERT(pCTE->GetFirstPack() != NULL);
	*ppPackage = pCTE->GetFirstPack()->GetPackage();
	if ((pCTE->flags & CT_HWND) == 0)
	{
		if(pCTE->flags & CT_MENU)
		{
			return TBBS_TEXTUAL | TBBS_MENU;
		}

		if(pCTE->flags & CT_TEXTBUTTON)
		{
			return TBBS_BUTTON | TBBS_TEXTUAL;
		}

		BOOL bHasGlyph=HasCommandBitmap(nCommand);

		if(bHasGlyph)
		{
			return TBBS_BUTTON | TBBS_GLYPH;
		}
		else
		{
			return TBBS_BUTTON | TBBS_TEXTUAL;
		}
	}

	UINT nFlags = TBBS_HWND;

	if (pCTE->flags & CT_HWNDSIZE)
	{
		nFlags |= TBBS_SIZABLE;
	}

	if (pCTE->flags & CT_HWNDENABLE)
	{
		nFlags |= TBBS_ENABLEWND;
	}

	if(pCTE->flags & CT_TEXTBUTTON)
	{
		nFlags |= TBBS_TEXTUAL;
	}
	else
	{
		nFlags |= TBBS_GLYPH;
	}

	return nFlags;
}

// This function is used to find the bitmap and glyph index for a given command (nID),
// It returns true if the command could be found. If pCTE is non-null, it should point to the
// the CTE of nID; this is a simple optimisation.
BOOL CTheApp::GetCommandBitmap(UINT nID, /* OUT */ HBITMAP *phbmWell, /* OUT */ int *pnGlyph, /* IN */ BOOL bLarge, /* IN */ CTE *pCTE)
{
	// If we don't already know the cte, then find it
	if(pCTE==NULL)
	{
		pCTE=theCmdCache.GetCommandEntry(nID);
	}
	else
	{
		// otherwise, verify it.
		ASSERT(pCTE->id==nID);
	}

	HBITMAP hbmWell=NULL;
	int nGlyph=-1;
	CPackage *pPackage=NULL;

	if(     pCTE &&
		pCTE->GetFirstPack() &&                                                                 // should never fail 
		(pPackage=pCTE->GetFirstPack()->GetPackage())!=NULL)    // nor should this. All commands must have at least one pack, which must have a package.
	{
		ICommandProvider *pCP=pPackage->GetCommandProvider();
		if(pCP)
		{
			DWORD nLongGlyph;
			hbmWell=pCP->GetBitmap((WORD)nID, bLarge, &nLongGlyph);

			ASSERT(sizeof(long)==sizeof(int));

			// GetBitmap doesn't setup nLongGlyph except when it returns a bitmap, so we only copy it then
			if(hbmWell)
			{
				nGlyph=nLongGlyph;
			}

			pCP->Release();
		}

		if(nGlyph==-1)
		{
			if(phbmWell)
			{
				hbmWell=pPackage->GetBitmap(bLarge);
			}
			nGlyph = (int)(short)pCTE->glyph; // WORD to INT...fix -1 case.
		}

#ifdef _DEBUG
		// ensure that the glyph number isn't outside the valid range for the package
		BITMAP bmpInfo;
		if(hbmWell!=NULL)
		{
			CSize sizeGlyph=CCustomBar::GetDefaultBitmapSize(bLarge);

			::GetObject(hbmWell,sizeof(BITMAP), &bmpInfo);

			int nGlyphs=bmpInfo.bmWidth/sizeGlyph.cx;

			// If this fails then the button hasn't been correctly added to the bar, or
			// the .cmd file has the wrong glyph index.
			ASSERT(nGlyph<nGlyphs);
		}
#endif

	}

	BOOL bHasGlyph=(nGlyph!=-1);
	if(phbmWell)
	{
		*phbmWell=hbmWell;
	}
	if(pnGlyph)
	{
		*pnGlyph=nGlyph;
	}

	return bHasGlyph;
}

LPDISPATCH CTheApp::GetPackageExtension(LPCTSTR szExtensionName)
{
	LPDISPATCH pDispReturn = NULL;
	CPackage* pPackage;
	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*) m_packages.GetNext(pos);
		if ((pPackage->m_flags & PKS_AUTOMATION) == 0)
			continue;

		if ((pDispReturn = pPackage->GetPackageExtension(szExtensionName))
				!= NULL)
		{
			return pDispReturn;
		}
	}
	return NULL;
}

HRESULT CTheApp::FindInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	CPackage* pPackage;
	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*) m_packages.GetNext(pos);
		if ((pPackage->m_flags & PKS_INTERFACES) == 0)
			continue;

		HRESULT hr = pPackage->ExternalQueryInterface(&riid, ppvObj);
		if (SUCCEEDED(hr))
			return hr;
	}

	*ppvObj = NULL;
	return ResultFromScode(E_NOTIMPL);
}

HRESULT CTheApp::GetService(REFGUID guidService, REFIID riid, void **ppvObj)
{
	if(NULL == ppvObj)
		return E_INVALIDARG;

	*ppvObj = NULL;

	// if the guid is NULL, then pass it off to
	// find interface
	if(CLSID_NULL == guidService)
		return FindInterface(riid, ppvObj);
	else if(SID_SMsoComponentManager == guidService)
	{
		IUnknown *punk = ((CMainFrame *)AfxGetMainWnd())->GetComponentMgr();
		ASSERT(NULL != punk);
		return punk->QueryInterface(riid, ppvObj);
	}
	else if(SID_SBuilderWizardManager == guidService)
	{
		// theInternalApp is a global object
		// GetBuilderWizardManager returns a non addref'ed pointer
		CBuilderWizardManager *pBWM = theInternalApp.GetBuilderWizardManager();
		ASSERT(NULL != pBWM);
		return pBWM->ExternalQueryInterface(&riid, ppvObj); 
	}
	else if(SID_SApplicationObject == guidService)
	{
		ASSERT(NULL != theApp.m_pAutoApp);
		return theApp.m_pAutoApp->ExternalQueryInterface(
						&riid, (void**)ppvObj);
	}

	// walk through each package and see if one supports the service
	CPackage* pPackage;
	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*) m_packages.GetNext(pos);
		if ((pPackage->m_flags & PKS_INTERFACES) == 0)
			continue;

		HRESULT hr = pPackage->GetService(guidService, riid, ppvObj);
		if(SUCCEEDED(hr))
			return hr;
	}

	return E_NOINTERFACE;
}

HRESULT CTheApp::DoesIntrinsicBuilderExist(REFGUID rguidBuilder)
{
	// validate parameters
	ASSERT(CLSID_NULL != rguidBuilder);
	if(CLSID_NULL == rguidBuilder)
		return E_INVALIDARG;
	
	// walk through each package and see if one supports the service
	COleRef<IBuilderWizardManager> srpBWM;
	HRESULT hr;
	CPackage* pPackage;
	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*) m_packages.GetNext(pos);
		if ((pPackage->m_flags & PKS_INTERFACES) == 0)
			continue;

		hr = pPackage->GetService(SID_SIntrinsicBuilderWizardManager, 
								IID_IBuilderWizardManager, (void**)&srpBWM);
		if(SUCCEEDED(hr))
		{
			hr = srpBWM->DoesBuilderExist(rguidBuilder);
			srpBWM.SRelease();
			if(S_OK == hr)
				return hr;
		}
	}
	return S_FALSE;
}

HRESULT CTheApp::GetIntrinsicBuilder(
	REFGUID rguidBuilder, 
	DWORD grfGetOpt, 
	HWND hwndPromptOwner, 
	IDispatch **ppdispApp,
	HWND *pwndBuilderOwner, 
	REFIID riidBuilder, 
	IUnknown **ppunkBuilder)
{
	ASSERT(CLSID_NULL != rguidBuilder);
	ASSERT(NULL != ppunkBuilder);
	if(CLSID_NULL == rguidBuilder || NULL == ppunkBuilder)
		return E_INVALIDARG;

	if(NULL != ppdispApp)
		*ppdispApp = NULL;
	if(NULL != pwndBuilderOwner)
		*pwndBuilderOwner = NULL;
	*ppunkBuilder = NULL;

	// walk through each package and see if one supports the service
	COleRef<IBuilderWizardManager> srpBWM;
	HRESULT hr;
	CPackage* pPackage;
	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*) m_packages.GetNext(pos);
		if ((pPackage->m_flags & PKS_INTERFACES) == 0)
			continue;

		hr = pPackage->GetService(SID_SIntrinsicBuilderWizardManager, 
								IID_IBuilderWizardManager, (void**)&srpBWM);
		if(SUCCEEDED(hr))
		{
			hr = srpBWM->GetBuilder(rguidBuilder, grfGetOpt, 
							hwndPromptOwner, ppdispApp, pwndBuilderOwner, 
							riidBuilder, ppunkBuilder);
			srpBWM.SRelease();
			if(SUCCEEDED(hr))
				return hr;
		}
	}
	return E_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

UINT SushiRegisterWindowMessage(const char* szMsg)
{
	UINT nMsg = RegisterWindowMessage(szMsg);
#if 0
	TRACE("::SushiRegisterWindowMessage: Message '%s' is 0x%04x (WM_USER+%d)\n",
		szMsg, nMsg, nMsg - WM_USER);
#endif
	return nMsg;
}

#endif

///////////////////////////////////////////////////////////////////////////////

BOOL CTheApp::OnDDECommand(TCHAR *pszCommand)
{
	while (_istspace(*pszCommand))
		pszCommand = _tcsinc(pszCommand);

	BOOL bBracketed;
	if (bBracketed = (*pszCommand == '['))
		pszCommand = _tcsinc(pszCommand);

	while (_istspace(*pszCommand))
		pszCommand = _tcsinc(pszCommand);

	char* pchOpcode = pszCommand;
	char ch;
	while ((ch = *pszCommand) != '\0')
	{
		if (_istspace(ch) || ch == ',' || ch == '(' || ch == ')' || ch == '"')
			break;

		pszCommand = _tcsinc(pszCommand);
	}

	CString strOpcode(pchOpcode, (int)(pszCommand - pchOpcode));

	while (_istspace(*pszCommand))
		pszCommand = _tcsinc(pszCommand);

	CStringArray parameters;
	while (*pszCommand == ((parameters.GetSize() == 0) ? '(' : ','))
	{
		pszCommand = _tcsinc(pszCommand);

		while (_istspace(*pszCommand))
			pszCommand = _tcsinc(pszCommand);

		if (*pszCommand == '\0')
		{
			TRACE("Unexpected end of DDE_EXECUTE string\n");
			return FALSE;
		}

		if (*pszCommand == ')')
		{
			pszCommand = _tcsinc(pszCommand);
			break;
		}

		BOOL bQuoted;
		if (bQuoted = (*pszCommand == '"'))
			pszCommand = _tcsinc(pszCommand);

		char* pchParameter = pszCommand;
		while ((ch = *pszCommand) != '\0')
		{
			if (ch == '"')
				break;

			if (ch == ',' || ch == '(' || ch == ')' || ch == '[' || ch == ']')
			{
				if (!bQuoted)
					break;

				if (ch != ',')
				{
					// Within quotes, parentheses and brackets must be doubled
					if (*(pszCommand + 1) != ch)
						break;
					pszCommand = _tcsinc(pszCommand);
				}
			}

			pszCommand = _tcsinc(pszCommand);
		}

		CString strParam(pchParameter, (int)(pszCommand - pchParameter));

		if (bQuoted != (ch == '"'))
		{
			TRACE("Mismatched quote in DDE_EXECUTE string\n");
			return FALSE;
		}

		if (ch == '"')
		{
			pszCommand = _tcsinc(pszCommand);
			while (_istspace(*pszCommand))
				pszCommand = _tcsinc(pszCommand);
		}

		parameters.Add(strParam);
	}

	///////////////////////////////////

#ifdef _DEBUG
	TRACE("DDE_EXECUTE: op-code = '%s'\n", (const char*)strOpcode);
	for (int i = 0; i < parameters.GetSize(); i += 1)
		TRACE("\t%d:%s\n", i + 1, (const char*)parameters[i]);
#endif

	///////////////////////////////////

	// DDE 'open' command?
	if (strOpcode.CompareNoCase("open") == 0)
	{
		if (parameters.GetSize() != 1)
		{
			TRACE("Illegal parameters for DDE_EXECUTE 'open' command\n");
			return FALSE;
		}

		// only open the document from DDE 'open' if it 
		// isn't already open
		CPath path;
		if (!path.Create(parameters[0]))
		{
			TRACE("Illegal pathname '%s' for DDE_EXECUTE 'open' command\n", (const char*)parameters[0]);
			return FALSE;
		}

		m_pMainWnd->OpenIcon();
		m_pMainWnd->SetForegroundWindow();
		OpenDocumentFile((const TCHAR *)path);
		return TRUE;
	}

	TRACE("Unknown DDE_EXECUTE op-code: '%s'\n", (const char*)strOpcode);
	return FALSE;
}

UINT g_cPendingMessages = 0;

void CTheApp::FlashStatePump()
{
	ASSERT(!theApp.m_fVCPPIsActive || m_pMainWnd->IsIconic());
	static BOOL fFlashTimerDead;
	CMainFrame* pMainWnd = (CMainFrame *) m_pMainWnd;  // For casting only

	// If we're not flashing, start.

	if (m_cActiveFlashPumps == 0)
	{
		pMainWnd->FlashWindow(FALSE);
		m_nTimerID = pMainWnd->SetTimer(IDT_WINDOW_FLASH, 500, NULL);
		fFlashTimerDead = FALSE;
	}
	m_cActiveFlashPumps++;

	// Pump the message queue from here until we become active
	BOOL    fIconic;

	while((fIconic = pMainWnd->IsIconic()) || !theApp.m_fVCPPIsActive)
	{
		// only do idle processing if we are visible
		// dolphin:#4435, msgbox while we are loading an last project and we are
		//      minimized, thereby starting a flashpump, but the project is not yet
		//      fully loaded, leading to problems in the idle processing.
		if (fIconic)
		{
			if (!PumpMessage())
			{
				// repost the quit so we get it in the underlying msg pump
				PostQuitMessage(m_msgCur.message == WM_QUIT ? m_msgCur.wParam : 0);
				break;
			}
		}
		else
			IdleAndPump();
	}

	// Kill the window flash timer on the first message box

	if (!fFlashTimerDead)
	{
		VERIFY(pMainWnd->KillTimer(m_nTimerID));
		pMainWnd->FlashWindow(TRUE);
		fFlashTimerDead = TRUE;
	}

	m_cActiveFlashPumps--;
	return;
}

// get parent window for modal dialogs and message boxes.
// never uses floating window.
HWND PASCAL _SushiGetSafeOwner(CWnd* pParent)
{
	if (pParent != NULL)
	{
		ASSERT_VALID(pParent);
		return pParent->m_hWnd;
	}
	HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	if (hWnd != NULL)
	{
		HWND hWndParent;
		while ((hWndParent = ::GetParent(hWnd)) != NULL)
			hWnd = hWndParent;
		hWnd = ::GetLastActivePopup(hWnd);

		// If it is a floating window, use the main window
		// unless it's the property sheet.
		// N.B.  The two kind of floating windows that we
		// detect here are the mini frame windows (dockables) [bm]
		// and the browser window popup which is a CFloatingFrameWnd
		// derived class [rm]
		//
		CWnd* pWnd = CWnd::FromHandle(hWnd);
		if (pWnd != (CWnd *)GetPropertyBrowser() &&
			(pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) ||
			 pWnd->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd)) ||
			 pWnd == PwndSplashScreen()))
			hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	}
	return hWnd;
}

// Ripped from MFC so we can do our own GetSafeOwner that excludes
// Dockable windows, return focus to dockable windows.
int CTheApp::DoMessageBox(LPCSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	HWND hWnd = _SushiGetSafeOwner(NULL);

	DWORD dwOldPromptContext = m_dwPromptContext;
	if (nIDPrompt != 0)
		m_dwPromptContext = HID_BASE_PROMPT+nIDPrompt;

	if ((nType & MB_ICONMASK) == 0)
	{
		switch (nType & MB_TYPEMASK)
		{
		case MB_OK:
		case MB_OKCANCEL:
		case MB_YESNO:
		case MB_YESNOCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_ABORTRETRYIGNORE:
		case MB_RETRYCANCEL:
			// No default icon for these types, since they are rarely used.
			// The caller should specify the icon.
			break;
		}
	}

#ifdef _DEBUG
	if ((nType & MB_ICONMASK) == 0)
		TRACE0("Warning: no icon specified for message box.\n");
#endif

	PreModalWindow();
	EnableModeless(FALSE);
	int nRet = ::MessageBox(hWnd, lpszPrompt, m_pszAppName, nType);
	EnableModeless(TRUE);
	PostModalWindow();

	m_dwPromptContext = dwOldPromptContext;
	return nRet;
}

// Build Notification:
BOOL CTheApp::NotifyPackages(UINT id, void *Hint /*= NULL*/,
	void *ReturnArea /*= NULL*/)
{
#ifdef PERF_MEASUREMENTS
///////////// Testing code for V4.X perf release (fabriced)
	if(id == PN_END_BUILD )
		ReleaseSemaphore( hBuildSynch, 1, NULL);
	else if(id== PN_BEGIN_BUILD)
		fBuildStarted = TRUE;
/////////////////////////////////////////////////////////////
#endif  // PERF_MEASUREMENTS
	fForceIdleCycle = TRUE;
	if (id == PN_WORKSPACE_INIT)
	{
		if (m_bRunInvisibly &&
			m_bInvokedCommandLine // REVIEW
			)
		{
			g_AutomationState.DisableUI();
			g_AutomationState.SetSaveChanges(FALSE);
		}
	}

	POSITION pos = m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*) m_packages.GetNext(pos);
		if ((pPackage->m_flags & PKS_NOTIFY) != 0 &&
			!pPackage->OnNotify(id, Hint, ReturnArea) &&
			id >= PN_FIRST_VETO)
		{
			return FALSE;
		}
	}

	if (id == PN_WORKSPACE_INIT)
		CPartDoc::UpdateSaveAll(TRUE);
	else if (id == PN_WORKSPACE_CLOSE)
		CPartDoc::UpdateSaveAll(FALSE);

	if(	id==PN_WORKSPACE_INIT ||
		id==PN_WORKSPACE_CLOSE ||
		id==PN_NEW_PROJECT ||
		id==PN_OPEN_PROJECT ||
		id==PN_CLOSE_PROJECT)
	{
		m_bMenuDirty=TRUE;
		m_bProjectCommandsDirty=TRUE;
	}

	return TRUE;
}

// Node provider querying (asks for default nodes and layout info)
void CTheApp::GetPackagesDefProvidedNodes(CPtrList &DefNodeList, BOOL bWorkspaceInit)
{
	POSITION pos;
	CPackage *pPackage;
	for (pos = m_packages.GetHeadPosition(); pos != NULL;)
	{
		pPackage = (CPackage *)m_packages.GetNext(pos);
		pPackage->GetDefProvidedNodes(DefNodeList, bWorkspaceInit);
	}
}

// Map a file error code to a string id.
static struct FERRID
{
    int ferr;
    int ids;

} mpidsferr [] =
{
    { ferrCantSaveReadOnly,     IDS_ERROR_CANT_SAVERO },
	{ ferrCantSaveTempFiles,        IDS_ERROR_CANT_SAVETEMP },
    { ferrIllformedFile,        IDS_ERROR_BOGUSFILE },
};

int CTheApp::IdsFromFerr(int ferr)
{
    if( HIWORD(ferr) == 0 )
		return IDS_ERROR_FILE + ferr; // was an exception cause

	if( HIWORD(ferr) == PACKAGE_SUSHI )
	{
		// Check the error map above.
		for (int i = 0; i < sizeof (mpidsferr) / sizeof (FERRID); i += 1)
		{
		if (mpidsferr[i].ferr == ferr)
			return mpidsferr[i].ids;
		}
	}

	// Determine the package from the HIWORD.
	CPackage* pPackage = GetPackage(HIWORD(ferr));
	if( pPackage == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}

    return pPackage->IdsFromFerr(ferr);         // do NOT strip the HIWORD
}

void CTheApp::InitRepeatableCommands()
{
	void* pv = NULL;

	for (int i = 0; i < theCmdCache.m_cCommands; i++)
	{
		CTE* pCTE = &theCmdCache.m_rgCommands[i];

		// If the command is repeatable, add it to the map
		if (pCTE->flags & CT_REPEATABLE)
			m_RepeatableCmds.SetAt(pCTE->id, pv);
	}
}

// Takes the name of the command and returns its command ID.
//  This searches the entire list of commands so THIS IS VERY
//  INEFFICIENT-- USE IT ONLY IF YOU HAVE TO
// Returns BOOL indicating whether the ID was found.
BOOL CTheApp::GetCommandID(LPCTSTR szCommandName, UINT* pnCmdID)
{
	return theCmdCache.GetCommandID(szCommandName, pnCmdID);
}

// Packages may use this to check whether a command is currently bound to a key
// This is used by devaut1 to remember to never change the cmdID of bound commands.
BOOL CTheApp::IsCommandKeyBound(WORD id)
{
	POSITION pos = CKeymap::c_keymaps.GetHeadPosition();
	while (pos != NULL)
	{
		WORD unused1, unused2;
		CKeymap* pKeymap = (CKeymap*)(void*)CKeymap::c_keymaps.GetNext(pos);
		if(pKeymap->LookupCommand(id, unused1, unused2, NULL))
			return TRUE;
	}
	return FALSE;
}

// Packages may use this to add a command dynamically instead of using
//  cmdcomp.  This is used by devaut1 to add commands for macros and DLL AddIns
BOOL CTheApp::AddCommand(WORD id, WORD group, WORD flags, WORD glyph,
							CPack* pPack, LPCTSTR szCommand, int nCmdLength)
{
	return theCmdCache.AddCommand(id, group, flags, glyph, pPack, szCommand, nCmdLength);
}

// Packages can use this to replace a substring within the command string:
//  either the menutext, prompt text, or tip text.  Devaut1 uses this to
//  update the command's description when the user updates a macro's description
//  in the macro file.  You may not use this to change the name of the command
//  itself.  This will dirty the command cache.
void CTheApp::ReplaceCommandString(UINT nID, UINT iString, LPCTSTR szNewSubString)
{
	theCmdCache.ReplaceCommandString(nID, iString, szNewSubString);
}

// Packages may use this to see if a command of this name already exists
//  This is used by devaut1 to make sure macros/addin cmds don't clash
//  with preexisting ones.
BOOL CTheApp::DoesCommandNameExist(LPCTSTR szCmdName)
{
	UINT nDummy;
	return theCmdCache.GetCommandID(szCmdName, &nDummy);
}

LPCTSTR CTheApp::GetCommandName(WORD id)
{
	LPCTSTR szCmdName = NULL;
	if(!theCmdCache.GetCommandString(id, STRING_COMMAND, &szCmdName))
		szCmdName = NULL;
	return szCmdName;
}

// Packages should use this to remove commands that are being permanently removed from the system.
// UI elements related to the command will be irretrievably removed from the UI. If pPack is not
// the owner of the command, then the command will not be removed. This stops most commands being removed
// erroneously by confused packages. 
BOOL CTheApp::RemoveCommand(WORD id, CPack* pPack)
{
	return theCmdCache.RemoveCommand(id, pPack);
}

void CTheApp::SetAppWizHwnd(HWND hWnd)
{
	((CMainFrame*) AfxGetMainWnd())->SetAppWizHwnd(hWnd);
}

BOOL CTheApp::ShouldExtrude(void)
{
	CWnd* pParentWnd = AfxGetMainWnd();
	CWnd* pActiveWnd = CWnd::GetForegroundWindow();
	return (!IsCustomizing()) &&
			((pParentWnd == pActiveWnd) ||
			 (pParentWnd->GetLastActivePopup() == pActiveWnd));
}

BOOL CTheApp::IsCustomizing(void)
{
	return CASBar::s_pCustomizer!=NULL;
}

// Packages may use this to create one-shot toolbars. The bar would not be owned by the package, and is 
// drawn from the users stock of custom bars. Bars should be created all at one time. If pPackage or dwCookie
// are different from the last time this function was called, then a new bar will be created.
BOOL CTheApp::AddCommandBarButton(CPackage *pPackage, DWORD dwCookie, UINT nId,
								  long nButtonType)
{
	static CPackage *pLastPackage=NULL;                     // last package to create a bar
	static DWORD dwLastCookie=0;                            // last cookie to create a bar
	static CASBar *pLastBar=NULL;                           // last bar created
	static CPoint ptLastBar(-1,-1);                         // top left of last bar created

	int nInsertIndex=-1;                                            // index in bar at which button was inserted
	BOOL bNeedRecalc=FALSE;                                         // TRUE if a recalclayout will be required

	// If it's a new request, do a new bar
	if(pPackage!=pLastPackage || dwCookie!=dwLastCookie)
	{
		pLastPackage=pPackage;
		dwLastCookie=dwCookie;

		pLastBar= new CASBar;

		// Be sure the dock manager is around.  If not, the add-in is likely
		//  calling this on startup (a no-no).
		if (((CMainFrame*) AfxGetMainWnd())->m_pManager == NULL)
			return FALSE;

		// create the bar containing the right id.
		if (!pLastBar->Create(((CMainFrame*) AfxGetMainWnd())->m_pManager, nId, NULL))
		{
			// autodeletion inside this create function
			pLastBar=NULL;
			return FALSE;
		}
		else
		{
			// mark the bar dirty, because create will assume clean when passed an id like that.
			pLastBar->SetDirty(TRUE);

			// button has been inserted at the first position, of course
			nInsertIndex=0;

			if(ptLastBar.x==-1)
			{
				// this is the first bar; just cache its position.
				CRect rectBar;

				// the bar will be floating, so we need to get to the palette window
				CWnd *pBarParent=pLastBar->GetParent();

				pBarParent->GetWindowRect(&rectBar);

				// this is the parent of the palette
				CWnd *pParent=pBarParent->GetParent();

				if(pParent)
				{
					// should always succeed
					pParent->ScreenToClient(&rectBar);
					ptLastBar.x=rectBar.left;
					ptLastBar.y=rectBar.top;
				}
			}
			else
			{
				// second and subsequent bars are cascaded

				// the bar will be floating, so we need to get to the palette window
				CWnd *pBarParent=pLastBar->GetParent();

				CRect rectBar;
				pBarParent->GetWindowRect(&rectBar);

				ptLastBar.x+=10;
				ptLastBar.y+=10;
				// 10 is an arbitrary step value
				pBarParent->SetWindowPos(NULL, ptLastBar.x, ptLastBar.y, 0,0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
	}
	else
	{
		if (pLastBar == NULL)
			return FALSE;
	
		nInsertIndex=pLastBar->AppendButton(nId);
		bNeedRecalc=TRUE;
	}

	// Set the button's style.  It's currently glyph-only.  If the user
	//  wants text instead, change it.
	if (nButtonType == dsText &&
		nInsertIndex!=-1)
	{
		pLastBar->AddText(nInsertIndex);
		pLastBar->RemoveBitmap(nInsertIndex);
	}

	if(bNeedRecalc)
	{
		ASSERT(pLastBar);

		// make sure it comes up as a single row.
		pLastBar->RecalcLayout(HTBOTTOM);
	}
		
	return TRUE;
}

// Packages can use this to implement dragable areas in customization pages
// call this in the page's Activate function
void CTheApp::ActivateDraggableCustomizePage(CDlgTab *pTab)
{
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();

	pFrame->EnableWindow(TRUE);
	CMenu* pMenu = pFrame->GetSystemMenu(FALSE);
	if (pMenu != NULL)
		pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
	::EnableWindow(pFrame->m_hWndMDIClient, FALSE);

	CObArray aWorkers;
	pFrame->m_pManager->ArrayOfType(dtEmbeddedToolbar, &aWorkers, FALSE, TRUE);

	for(int i=0; i<aWorkers.GetSize(); i++)
	{
		CDockWorker *pWorker=(CDockWorker *)(aWorkers[i]);
		if(pWorker->m_pWnd)
		{
			pWorker->m_pWnd->GetParent()->EnableWindow(TRUE);
		}
	}
	
	if(CASBar::s_pCustomizer!=NULL)
	{
		delete CASBar::s_pCustomizer;
	}

	CASBar::s_pCustomizer = new CToolCustomizer(pFrame->m_pManager, pTab, CASBar::s_aUsage, FALSE, FALSE);
}

// Call this in LBUTTONDOWN. Note that this will eat LBUTTONUP, so you will need to fake one if
// the drag source is something like a list box, which relies on the lbutton up.
void CTheApp::DragCommandButton(CPoint ptSource, UINT nId)
{
	CASBar *pBar=new CASBar;        // DoButtonMove requires a valid ASBar in case it
								// needs to create a new bar from scratch
	// it's a command
	CASBar::s_pCustomizer->DoButtonMove(ptSource, pBar->GetProxy(), -1, NULL, CToolCustomizer::cCommand, nId);

	delete pBar;
}

// call this in the page's DeActivate function
void CTheApp::DeactivateDraggableCustomizePage(CWnd *pParentWnd)
{
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();

	if(CASBar::s_pCustomizer!=NULL)
	{
		delete CASBar::s_pCustomizer;
		CASBar::s_pCustomizer = NULL;
	}

	::EnableWindow(pFrame->m_hWndMDIClient, TRUE);
	CMenu* pMenu = AfxGetMainWnd()->GetSystemMenu(FALSE);
	if (pMenu != NULL)
		pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	AfxGetMainWnd()->EnableWindow(FALSE);
	pParentWnd->EnableWindow(TRUE); // MFC will disable us.
}

// For information on why this is the way it is, see the comments in the shell implementation of CPackage::GetPacakgeProperty
UINT CTheApp::GetExecutableProperty(UINT nProperty)
{
	return 0;
}

// CTheApp::MDITypedGetNext
//    We use this function when we want to close all windows of a certain
//    type, excluding others.  It may appear a little inefficient, but
//    is necessary, since closing a resource browser, closes associated
//    editor windows, which may invalidate any cached window handles we hold.

// We used to always do this backwards, but that's not what the window menu wants

CPartFrame* CTheApp::MDITypedGetNext(CPartFrame* pFrame, TYPE_FILTER tf, BOOL bForwards /*=FALSE*/)
{
	UINT nFirst=0;
	UINT nNext=0;
	if(bForwards)
	{
		nFirst=GW_HWNDFIRST;
		nNext=GW_HWNDNEXT;
	}
	else
	{
		nFirst=GW_HWNDLAST;
		nNext=GW_HWNDPREV;
	}

	extern CPartFrame* GetProjectFrame();

	CPartFrame* pProjFrame = GetProjectFrame();
	LPBUILDSYSTEM pBldSysIFace;
	theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace);
	CPath cp;
	CPartDoc *pDoc;

	// Actually, we get the last possible window that fits the type filter,
	// so that closing a lot of maximized windows doesn't change the main
	// frame caption.

	if (pFrame != NULL)
		pFrame = (CPartFrame*) pFrame->GetWindow(nNext);
	else
	{
		pFrame = (CPartFrame*) ((CMainFrame *)m_pMainWnd)->MDIGetActive();
		if (pFrame != NULL)
			pFrame = (CPartFrame*) pFrame->GetWindow(nFirst);
	}

	for ( ; pFrame != NULL; pFrame = (CPartFrame *)pFrame->GetWindow(nNext))
	{
		if(!IsValidMDICycleMember(pFrame))
		{
			continue;
		}

		ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));
		if ((tf & FILTER_DEBUG) != 0 && pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
		{
			continue;
		}
		if ((tf & FILTER_PROJECT) != 0 && pFrame == pProjFrame)
		{
			continue;
		}

		pDoc = (CPartDoc *)pFrame->GetActiveDocument();
		ASSERT(pDoc == NULL || pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));

		if (((tf & FILTER_DOCUMENT) != 0) && (pDoc != NULL) && (!pDoc->FDisplayTitle()))
		{
			continue;
		}
		if ((tf & FILTER_NON_PROJECT) != 0 && (pBldSysIFace == NULL || (pDoc == NULL) || !cp.Create(pDoc->GetPathName())
			|| (pBldSysIFace->IsActiveBuilderValid() == S_OK && pBldSysIFace->IsProjectFile(&cp, FALSE, ACTIVE_BUILDER) != S_OK)))
		{
			// REVIEW [patbr]: do we need to check ALL projects, not just active builder?
			continue;
		}

		break;
	}

	if (pBldSysIFace)
		pBldSysIFace->Release();
	return pFrame;
}

CPartFrame* GetProjectFrame()
{
	LPBUILDSYSTEM pBldSysIFace;
	theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace);
	CWnd* pSlobWnd = NULL;
	if (pBldSysIFace != NULL)
	{
		if (pBldSysIFace->IsActiveBuilderValid() == S_OK)
			pBldSysIFace->GetSlobWnd(ACTIVE_BUILDER, &pSlobWnd);
		pBldSysIFace->Release();
		if (pSlobWnd != NULL)
		{
#ifdef _DEBUG
			CPartFrame* pFrameProj = (CPartFrame*) pSlobWnd->GetParentFrame();
			ASSERT(pFrameProj == NULL ||
				pFrameProj->IsKindOf(RUNTIME_CLASS(CPartFrame)));
#endif
			return (CPartFrame*) pSlobWnd->GetParentFrame();
		}
	}

	return NULL;
}

HCURSOR AfxLoadCursor(LPCTSTR lpResourceID)
{
	HINSTANCE hInst = AfxFindResourceHandle(lpResourceID, RT_GROUP_CURSOR);
	ASSERT(hInst != NULL); // resource not found anywhere!
	return LoadCursor(hInst, lpResourceID);
}

HICON AfxLoadIcon(LPCTSTR lpResourceID)
{
	HINSTANCE hInst = AfxFindResourceHandle(lpResourceID, RT_GROUP_ICON);
	ASSERT(hInst != NULL); // resource not found anywhere!
	return LoadIcon(hInst, lpResourceID);
}

HICON AfxLoadIcon(UINT  nResourceID)
{
	return AfxLoadIcon(MAKEINTRESOURCE(nResourceID));
}

/////////////////////////////////////////////////////////////////////////////

// Use this to report a missing required file.  The setup type indicates which
// setup program the user will have to run in order to get the specific DLL.
//
void ReportMissingFile(const TCHAR* szFileName, SETUP_TYPE setupType)
{
	UINT ids;
	switch (setupType)
	{
	default:
		ASSERT(FALSE);

	case stStandard:
		ids = IDS_CANT_LOAD_PACKAGE;
		break;

	case stMacintosh:
		ids = IDS_CANT_LOAD_MAC;
		break;
	}

	CString str;
	VERIFY(str.LoadString(ids));
	MsgBox(Error, CString(szFileName) + str);
}


// Use this instead of LoadLibrary() to load DLL's.  It will prevent Windows
// from displaying its ugly DLL missing message.  If bRequired is TRUE, we
// will display our own message asking the user to re-install the development
// environment.  The setup type indicates which setup program the user will
// have to run in order to get the specific DLL.
//
HINSTANCE SushiLoadLibrary(const TCHAR* szName, BOOL bRequired,
	SETUP_TYPE setupType /* = stStandard*/)
{
#ifndef _DEBUG
	int nOldErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
#endif
	HINSTANCE hInst = LoadLibrary(szName);
#ifndef _DEBUG
	SetErrorMode(nOldErrorMode);
#endif

#ifndef _WIN32
	if (hInst <= HINSTANCE_ERROR)
		hInst = NULL;
#endif

	if (hInst == NULL && bRequired)
		ReportMissingFile(szName, setupType);

	return hInst;
}

#undef AfxMessageBox
int DsMessageBox(LPCTSTR lpszText, UINT nType /*= MB_OK*/, UINT nIDHelp /*= 0*/)
{
	return AfxMessageBox(lpszText, nType, nIDHelp); 
}

int DsMessageBox(UINT nIDPrompt, UINT nType /*= MB_OK*/)
{ 
	return AfxMessageBox(nIDPrompt, nType, 0);
}

int DsMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp)
{ 
	return AfxMessageBox(nIDPrompt, nType, nIDHelp);
};

#define AfxMessageBox DsMessageBox
