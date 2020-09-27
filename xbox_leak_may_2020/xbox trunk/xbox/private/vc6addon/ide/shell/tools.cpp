//////////////////////////
//	class : CTool
//	purpose : keep information about command tool  
//////////////////////////

//////////////////////////
// include files
//////////////////////////
#include "stdafx.h"

#include "tools.h"
#include "toolexpt.h"
#include "find.h" // for CButtonEdit
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////

extern BOOL ExpandToolArguments(LPSTR szArg, size_t size);

// wrapper for ExpandToolArguments(char*, size_t)
inline BOOL ExpandToolArguments(CString &strArgs)
{
	BOOL fRet = ExpandToolArguments(strArgs.GetBuffer(MAXARGUMENT), MAXARGUMENT);
	strArgs.ReleaseBuffer();
	return fRet;
}

///////////////////////////////////////////////////////////////////

class CDialogArg: public C3dDialog
{
// Construction
public:
	CDialogArg (CTool *pTool, CWnd* pParent = NULL) ; // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogArg)
	enum { IDD = IDD_TOOLARGUMENTS } ;
	CString m_strArguments;
	BOOL	m_fRedirect;
	//}}AFX_DATA
	CMenuBtn m_mbtnMacros;
	CButtonEdit m_btedEdit;
// Implementation
protected:
	BOOL m_fGUITool ;
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support

	virtual BOOL OnInitDialog();
	// Generated message map functions
	//{{AFX_MSG(CDialogArg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnMacroItem(UINT nId);
};

///////////////////////////////////////////////////////////////////

UINT CTool::Spawn()
{
	UINT nRet;
	CString strArguments, strDirectory;
	if (m_fAskArguments)
	{
		CDialogArg dlg(this);
		if (dlg.DoModal() != IDOK)
		{
			// user canceled the tool arguments dialog, so abort
			return(0);
		}
		else 
		{
			m_fOutputRedirect = dlg.m_fRedirect;
			strArguments = dlg.m_strArguments;
		}
	}
	else
	{
		strArguments = m_strDefaultArgs;
		m_fOutputRedirect = m_fOutputRedirectDefault;
	}

	strDirectory = m_strInitialDir;

	if (!strArguments.IsEmpty() && !ExpandToolArguments(strArguments))
		return(0);
	if (!strDirectory.IsEmpty() && !ExpandToolArguments(strDirectory))
		return(0);

	// if necessary, set a default directory in which to spawn the tool
	if (strDirectory.IsEmpty() || _access(strDirectory, 00)==-1)
		strDirectory = ".";
	
	m_pSpawner = new CConsoleSpawner(&m_pSpawner);
	m_pSpawner->m_fCloseWindow = m_fCloseWindow;
	m_pSpawner->m_fGUITool = m_fGUITool;
	m_pSpawner->m_fReloadNoPrompt = m_fReloadNoPrompt;
	m_pSpawner->InitializeSpawn((LPTSTR)LPCTSTR(strDirectory), m_fOutputRedirect ? m_nOutputWindowID : -1, TRUE);
	CString strCommandLine;
	if(m_strPathName.Find(' ')!=-1)
		strCommandLine = "\"" + m_strPathName + "\" " + strArguments;
	else
		strCommandLine = m_strPathName + " " + strArguments;

	nRet = m_pSpawner->PerformSpawn((LPTSTR)LPCTSTR(strCommandLine), FALSE);
	// m_pSpawner is self-deleting! no need to delete it!
	return(nRet);
}

void CTool::CancelSpawn()
{
	ASSERT(m_pSpawner != NULL);
	m_pSpawner->CancelSpawn();
}

////////////////////////////
// Member function :
//
// operator = 
////////////////////////////
const CTool & CTool::operator = (const CTool& toolSrc)
{
	m_strMenuName = toolSrc.m_strMenuName;
	m_strPathName = toolSrc.m_strPathName;
	m_strDefaultArgs = toolSrc.m_strDefaultArgs;
	m_strCurrentArgs = toolSrc.m_strCurrentArgs;
	m_strInitialDir = toolSrc.m_strInitialDir;
	m_fAskArguments = toolSrc.m_fAskArguments;
	m_fGUITool = toolSrc.m_fGUITool;
	m_fOutputRedirectDefault = toolSrc.m_fOutputRedirectDefault;
	m_fOutputRedirect = toolSrc.m_fOutputRedirect;
	m_nOutputWindowID = toolSrc.m_nOutputWindowID;
	m_fCloseWindow = toolSrc.m_fCloseWindow;
	m_fUseSearchPath = toolSrc.m_fUseSearchPath;
	m_fVisibleOnMenu = toolSrc.m_fVisibleOnMenu;
	m_fReloadNoPrompt = toolSrc.m_fReloadNoPrompt;
	return *this;
}

//////////////////////////////
// class:  CToolList
// purpose : a list of CTool objects	
//////////////////////////////

///////////////////////
// Destructor 
///////////////////////
CToolList::~CToolList()
{
	DestroyContent();
}

///////////////////////
// CToolList::destroyContent
// 
// 	delete all CTool objects in the list
///////////////////////

void CToolList::DestroyContent() 
{
	int iTool = 0;
	m_nCurTool = -1;
	for (iTool = 0; iTool < m_nTools; iTool++)
	{
		delete m_toolArray[iTool];
		m_toolArray[iTool] = NULL;
	}
	m_nTools = 0;
}

//////////////////////////
// CToolList::addTool
//
//	adding CTool to the list
///////////////////////////
int CToolList::AddTool(CTool *newTool)
{
	int pos = 0;

	if (m_nTools > MAXTOOLIST)
		return -1;
	m_toolArray[m_nTools] = newTool;
	m_nCurTool = m_nTools;
	m_nTools++;
	return m_nTools;
}

///////////////////////////
// CToolList::deleteTool
//
//	delete the CTool pointed by the index
////////////////////////////
void CToolList::DeleteTool(int nIndex) 
{
	if (nIndex >= m_nTools)
		return;
	delete m_toolArray[nIndex];
	while (nIndex < (m_nTools - 1))
	{
		m_toolArray[nIndex] = m_toolArray[nIndex+1];
		nIndex++;
	}
	m_toolArray[nIndex] = NULL;
	if (m_nCurTool == (m_nTools - 1))
		m_nCurTool--;
	m_nTools--;
}

///////////////////////////
// CToolList::swapTool
//
//	swaps the positions of the tools with the given indices
////////////////////////////
void CToolList::SwapTool(int nIndex1, int nIndex2)
{
	CTool *ptoolTemp;

	ASSERT(nIndex1 >= 0 && nIndex1 < m_nTools);
	ASSERT(nIndex2 >= 0 && nIndex2 < m_nTools);
	ptoolTemp = m_toolArray[nIndex2];
	m_toolArray[nIndex2] = m_toolArray[nIndex1];
	m_toolArray[nIndex1] = ptoolTemp;
} 

///////////////////////////
// CToolList::getTool
//
//	get the CTool object given the index number
///////////////////////////
CTool *CToolList::GetTool(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nTools)
		return NULL;
	else
		return m_toolArray[nIndex];
}

//////////////////////////
// CToolList::getCurToolObject
//
//	get the current CTool object from the list
//	curTool is an index to a particular CTool object 
//////////////////////////
CTool *CToolList::GetCurToolObject()
{
	if (m_nTools == 0 || m_nCurTool >= m_nTools || m_nCurTool < 0)
		return NULL;
	else
		return m_toolArray[m_nCurTool];
}


///////////////////////////
// CToolList::operator =
//
//	copy CToolList objects
//	actual copying is done (no sharing)
///////////////////////////
CToolList & CToolList::operator = (CToolList& toolListSrc) 
{
	DestroyContent();
	m_nTools = toolListSrc.NumTools();
	m_nCurTool = toolListSrc.GetCurTool();
	for (int i = 0; i < m_nTools; i++)
	{
		m_toolArray[i] = new CTool;
		*m_toolArray[i] = *toolListSrc.GetTool(i);
	}
	
	return *this;
}

////////////////////////////
// Global variables defined
////////////////////////////

CToolList  toolList;
CToolList  toolListBackup;

/////////////////////////////
// Save and load toolList to/from ini file
// SaveToolListSetting()
// LoadToolListSetting()
/////////////////////////////
//
// Strings used for ini files 
// should this be in resource file ?
//
//////////////////////////////

static char szTools[] = "Tools" ;
static char szNumTool[] = "NumTools" ;
static char szMenuName[] = "MenuName" ;
static char szPathName[] = "PathName" ;
static char szDefaultArgs[] = "DefaultArguments" ;
static char szInitialDir[] = "InitialDirectory" ;
static char szAskArguments[] = "AskArgument" ;
static char szGUITool[] = "GUITool" ;
static char szOutputRedirectDefault[] = "OutputRedirect" ;
static char szCloseWindow[] = "CloseWindow" ;
static char szUseSearchPath[] = "UseSearchPath" ;
static char szVisibleOnMenu[] = "VisibleOnMenu" ;
static char szReloadNoPrompt[] = "ReloadNoPrompt" ;

void SaveToRegStr (TCHAR * szData, int index, const TCHAR * szString) 
{
	CString  szBuffer ;
	char szNum[2] ;
	szBuffer = szData ;
	szBuffer += '#' ;
	szBuffer += _itoa(index, szNum, 10) ;
	WriteRegString (szTools, (TCHAR *)(const TCHAR *)szBuffer, szString) ;
}

void SaveToRegInt (TCHAR * szData, int index, int value) 
{
	CString  szBuffer ;
	char szNum[2] ;
	szBuffer = szData ;
	szBuffer += "#" ;
	szBuffer += _itoa(index, szNum, 10) ;
	WriteRegInt (szTools, (TCHAR *)(const TCHAR *)szBuffer, value) ;
}

void SaveToolListSetting () 
{
	int i ;
	int numTools ;
	CTool *pTool ;

	// clean out the existing registry key so we start fresh
	CString strKeyMain = GetRegistryKeyName();
	RegDeleteKey(HKEY_CURRENT_USER, strKeyMain + chKeySep + _T("Tools"));

	// write the number of tools defined 
	numTools = min(toolList.NumTools(), MAXTOOLIST);
	WriteRegInt (szTools, szNumTool, numTools) ;

	// For each tool, save info
	for (i = 0 ; i < numTools ; i++)
	{
		pTool = toolList.GetTool(i) ;
		ASSERT (pTool != NULL) ;
		SaveToRegStr(szMenuName, i, (const TCHAR *) pTool->m_strMenuName);
		SaveToRegStr(szPathName, i, (const TCHAR *)pTool->m_strPathName);
		SaveToRegStr(szDefaultArgs, i, (const TCHAR *)pTool->m_strDefaultArgs);
		SaveToRegStr(szInitialDir, i, (const TCHAR *)pTool->m_strInitialDir);
		SaveToRegInt(szAskArguments, i, pTool->m_fAskArguments);
		SaveToRegInt(szGUITool, i, pTool->m_fGUITool);
		SaveToRegInt(szOutputRedirectDefault, i, pTool->m_fOutputRedirectDefault);
		SaveToRegInt(szCloseWindow, i, pTool->m_fCloseWindow);
		SaveToRegInt(szUseSearchPath, i, pTool->m_fUseSearchPath);
		SaveToRegInt(szVisibleOnMenu, i, pTool->m_fVisibleOnMenu);
		SaveToRegInt(szReloadNoPrompt, i, pTool->m_fReloadNoPrompt);
	}
}


CString LoadFromRegStr (TCHAR * szData, int i) 
{

	CString  szBuffer ;
	char szNum[2] ;
	szBuffer = szData ;
	szBuffer += "#" ; 
	szBuffer += _itoa(i, szNum, 10) ;
	return GetRegString (szTools, (TCHAR *)(const TCHAR *)szBuffer);
}

UINT LoadFromRegInt (TCHAR * szData, int i) 
{

	CString  szBuffer ;
	char szNum[2] ;
	szBuffer = szData ;
	szBuffer += "#" ; 
	szBuffer += _itoa(i, szNum, 10) ;
	return GetRegInt (szTools, (TCHAR *)(const TCHAR *)szBuffer, FALSE);
}

void LoadToolListSetting()
{
	int i;
	int numTools ;
	CTool *pTool ;

	// get the number of tools defined
	numTools = GetRegInt (szTools, szNumTool, FALSE) ;
	numTools = min(numTools, MAXTOOLIST);

	toolList.DestroyContent();
	// For each tool, create info
	for (i = 0 ; i < numTools ; i++) {
		pTool = new CTool ;
		ASSERT (pTool != NULL) ;
		pTool->m_strMenuName = LoadFromRegStr(szMenuName, i);
		pTool->m_strPathName = LoadFromRegStr(szPathName, i);

		// dolphin 2364 [patbr] allow tools without full paths
		pTool->m_fUseSearchPath = (BOOL)LoadFromRegInt(szUseSearchPath, i);
#ifdef CHECK_VALID_TOOL_PATH
		if (!pTool->m_fUseSearchPath)
		{
			CPath pathVerify;
			if (!pathVerify.Create((const TCHAR *) LoadFromRegStr (szPathName,i))) {
				// File does not exist anymore so, skip this one 
				delete pTool ;
				continue ;
			}
		}
		else
		{
			// should we search path now or just tell the user when tool fails to run?
		}
#endif	// CHECK_VALID_TOOL_PATH
		pTool->m_strDefaultArgs = LoadFromRegStr(szDefaultArgs, i);
		pTool->m_strInitialDir = LoadFromRegStr(szInitialDir, i);
		pTool->m_fAskArguments = LoadFromRegInt(szAskArguments, i);
		pTool->m_fGUITool = IsFileGUIExe((LPSTR)(const char *)pTool->m_strPathName);
		if (!pTool->m_fGUITool)
		{
			pTool->m_fOutputRedirectDefault = (BOOL)LoadFromRegInt(szOutputRedirectDefault, i);
			pTool->m_fCloseWindow = (BOOL)LoadFromRegInt(szCloseWindow, i);
			// user tools should only have output window tabs if either 
			// 'Ask for Arguments' or 'Redirect to Output Window' checked.
			if ((g_IdeInterface.GetOutputWindow() != NULL) && (pTool->m_fAskArguments || pTool->m_fOutputRedirectDefault))
				g_IdeInterface.GetOutputWindow()->OutputWindowAddVwin((LPSTR)(const TCHAR *)pTool->m_strMenuName, FALSE, (UINT *)&(pTool->m_nOutputWindowID));
			else
				pTool->m_nOutputWindowID = INVALID_OUTPUT_WINDOW ;
		}
		else
		{
			pTool->m_fOutputRedirectDefault = FALSE;
			pTool->m_fCloseWindow = FALSE;
			pTool->m_nOutputWindowID = INVALID_OUTPUT_WINDOW;
		}
		pTool->m_fVisibleOnMenu = (BOOL)LoadFromRegInt(szVisibleOnMenu, i);
		pTool->m_fReloadNoPrompt = (BOOL)LoadFromRegInt(szReloadNoPrompt, i);
		toolList.AddTool(pTool);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDialogArg

CDialogArg::CDialogArg (CTool * pTool, CWnd* pParent /*=NULL*/)
		: C3dDialog(CDialogArg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogArg)
	m_fRedirect = pTool->m_fOutputRedirectDefault;
	m_strArguments = pTool->m_strDefaultArgs;
	//}}AFX_DATA_MAP
	m_fGUITool = pTool->m_fGUITool;
}

void CDialogArg::DoDataExchange(CDataExchange * pDX)
{
	C3dDialog::DoDataExchange(pDX) ;
	//{{AFX_DATA_MAP(CDialogArg)
	DDX_Check(pDX, IDC_TOOLARGUMENTS_REDIRECT, m_fRedirect);
	DDX_Text(pDX, IDC_TOOLARGUMENTS_ARGS, m_strArguments);
	//}}AFX_DATA_MAP
}

BOOL CDialogArg::OnInitDialog()
{
	C3dDialog::OnInitDialog();
	VERIFY(m_btedEdit.SubclassDlgItem(IDC_TOOLARGUMENTS_ARGS, this));
	m_btedEdit.LimitText(MAXARGUMENT);
	// FUTURE: temporary workaround [patbr]
	// if SRC package not available, disable the 'redirect to output window'
	// checkbox, because the output window still lives in the SRC package.
	if (m_fGUITool || !theApp.GetPackage(PACKAGE_VCPP))
		GetDlgItem(IDC_TOOLARGUMENTS_REDIRECT)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_TOOLARGUMENTS_REDIRECT)->EnableWindow(TRUE);
	VERIFY(m_mbtnMacros.SubclassDlgItem(IDC_TOOLARGUMENTS_MACROS, this));
	VERIFY(m_mbtnMacros.LoadMenu(IDR_TOOL_MACRO));
	return TRUE ;
}

BEGIN_MESSAGE_MAP(CDialogArg,C3dDialog)
	//{AFX_MSG_MAP(CDialogArg)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDS_TOOLMACRO_FilePath, IDS_TOOLMACRO_WkspName, OnMacroItem)
END_MESSAGE_MAP()

void CDialogArg::OnMacroItem(UINT nId)
{
	if (IDS_TOOLMACRO_First <= nId && nId <= IDS_TOOLMACRO_Last)
	{
		CString strMacro;
		VERIFY(strMacro.LoadString(nId));
		m_btedEdit.ReplaceSel(strMacro);
		m_btedEdit.SetFocus();
	}
	else
		ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
