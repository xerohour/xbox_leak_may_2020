#include "stdafx.h"
#pragma hdrstop

#include "plist.h"
#include "fbrdbg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// CMapRemoteDllDlg implementation
///////////////////////////////////////////////////////////////////////////////

CMapRemoteDllDlg::CMapRemoteDllDlg(LPCTSTR szRemote,
			       CWnd * pParent /* = NULL */)
	: C3dDialog(CMapRemoteDllDlg::IDD, pParent)
{
	m_strRemote = szRemote;
}

BEGIN_MESSAGE_MAP(CMapRemoteDllDlg, C3dDialog)
	//{{AFX_MSG_MAP(CMapRemoteDllDlg)
		ON_EN_CHANGE(IDC_REMOTE_MAP_LOCAL_NAME, OnNameChange)
		ON_BN_CLICKED(ID_BROWSE, OnBrowse)
		ON_BN_CLICKED(IDC_PROMPT_DLLS, OnPromptDLLs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// BOOL CMapRemoteDllDlg::OnInitDialog()
///////////////////////////////////////////////////////////////////////////////
BOOL CMapRemoteDllDlg::OnInitDialog()
{
	// Set the text for the remote file we are trying to map
	((CStatic *)GetDlgItem(IDC_REMOTE_FILE_TXT))->SetWindowText(m_strRemote);
	((CEdit *)GetDlgItem(IDC_REMOTE_MAP_LOCAL_NAME))->SetWindowText(m_strRemote);

	// Init the state of the "try to locate any more" box in proj
	m_fPrompt = TRUE;
	((CButton *)GetDlgItem(IDC_PROMPT_DLLS))->SetCheck(1);

	// Enable the OK button
	UpdateOKState();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// void CMapRemoteDllDlg::UpdateOKState()
///////////////////////////////////////////////////////////////////////////////
void CMapRemoteDllDlg::UpdateOKState()
{
	// Enable OK if there is a local name entered
	BOOL bOK = ((CEdit *)GetDlgItem(IDC_REMOTE_MAP_LOCAL_NAME))->LineLength() > 0;

	// Enable OK if the "Try to local other DLLs" checkbox is checked
	// (and we're not trying to locate the JIT exe)
	if ( !(theApp.m_jit.GetActive() && !theApp.m_jit.FPathIsReal()) ) {
		bOK |= ( ((CButton *)GetDlgItem(IDC_PROMPT_DLLS))->GetCheck() == 0);
	}

	((CButton *)GetDlgItem(IDOK))->EnableWindow(bOK);
}

///////////////////////////////////////////////////////////////////////////////
// void CMapRemoteDllDlg::OnNameChange()
///////////////////////////////////////////////////////////////////////////////
void CMapRemoteDllDlg::OnNameChange()
{
	UpdateOKState();
}

///////////////////////////////////////////////////////////////////////////////
// void CMapRemoteDllDlg::OnOK()
///////////////////////////////////////////////////////////////////////////////
void CMapRemoteDllDlg::OnOK()
{	
	CString strLocalName;
	CString strDLLPath;

	((CEdit *)GetDlgItem(IDC_REMOTE_MAP_LOCAL_NAME))->GetWindowText(strLocalName);

	// str can be empty if one of the checkboxes is checked.
	if (!strLocalName.IsEmpty()) {
		if (FFindDLL(strLocalName, strDLLPath)
			&& CheckEXEForDebug(strDLLPath, TRUE, FALSE)) {

			m_strLocal = strDLLPath;
			m_fPrompt = ( ((CButton *)GetDlgItem(IDC_PROMPT_DLLS))->GetCheck() == 1 );
			CDialog::OnOK();
		}
		return;
	}

	m_strLocal.Empty();
	m_fPrompt = ( ((CButton *)GetDlgItem(IDC_PROMPT_DLLS))->GetCheck() == 1 );

	CDialog::OnOK();
}

void
CMapRemoteDllDlg::OnCancel(
	)
{
	// If the user doesn't ever want to be prompted for *this*
	// DLL again, save the remote name in DLLInfo and exit.

	if (!theApp.m_jit.GetActive() || theApp.m_jit.FPathIsReal())
	{
		HBLDTARGET hTarget;
		gpIBldSys->GetActiveTarget(ACTIVE_BUILDER, &hTarget);

		UpdateDLLInfoRec (_T(""), m_strRemote, TRUE, (ULONG) hTarget, FALSE);
		
//		AddRecToDLLInfo("", m_strRemote, TRUE, (UINT)hTarget);
	}

	m_fPrompt = ( ((CButton *)GetDlgItem (IDC_PROMPT_DLLS))->GetCheck() == 1 );

	CDialog::OnCancel();
}

void
CMapRemoteDllDlg::OnBrowse(
	)
{
	CString strTitle;
	CString str;
	CString strFilter;
	CFileDialog dlg(TRUE);

	strTitle.LoadString(IDS_FIND_LOCAL_MODULE);
	dlg.m_ofn.lpstrTitle = strTitle;
    dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;

	if (theApp.m_jit.GetActive() && !theApp.m_jit.FPathIsReal()) {
		VERIFY(str.LoadString(IDS_FILTER_EXES));
	} else {
		VERIFY(str.LoadString(IDS_FILTER_DLLS));
	}
	AppendFilterSuffix(strFilter, dlg.m_ofn, str);
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.nFilterIndex = 1;

	for ( ; ; ) {
		if (dlg.DoModal() == IDCANCEL) {
			return;
		}
		str = dlg.GetPathName();
		if (CheckEXEForDebug(str, TRUE, FALSE)) {
			((CEdit *)GetDlgItem(IDC_REMOTE_MAP_LOCAL_NAME))->SetWindowText(str);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void CMapRemoteDllDlg::OnPromptDLLs()
///////////////////////////////////////////////////////////////////////////////
void CMapRemoteDllDlg::OnPromptDLLs()
{
	UpdateOKState();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAssertFailedDlg dialog


CAssertFailedDlg::CAssertFailedDlg(CWnd* pParent /*=NULL*/)
	: C3dDialog(CAssertFailedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAssertFailedDlg)
	m_strAssertText = _T("");
	//}}AFX_DATA_INIT
}


void CAssertFailedDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAssertFailedDlg)
	DDX_Text(pDX, IDC_ASSERT_TEXT, m_strAssertText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAssertFailedDlg, C3dDialog)
	//{{AFX_MSG_MAP(CAssertFailedDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//
//	Attach to Active Dialog
//


//
//	CAttachToActive Message Map

BEGIN_MESSAGE_MAP(CAttachToActive, C3dDialog)
	ON_BN_CLICKED (IDC_SYSTEM_PROCESSES,OnChangeSystemProcess)
END_MESSAGE_MAP()


CAttachToActive::CAttachToActive(
	CWnd*	pParent // = NULL
	)
	: C3dDialog (CAttachToActive::IDD, pParent)
{
	m_ProcessId = 0;
	m_TaskList = NULL;
}

CAttachToActive::~CAttachToActive(
	)
{
	delete [] m_TaskList;
	m_TaskList = NULL;
}

void
CAttachToActive::DoDataExchange(
	CDataExchange*	pDX
	)
{
	C3dDialog::DoDataExchange (pDX);
}

#define PL_INDEX_NAME	0
#define PL_INDEX_PID	1
#define PL_INDEX_TITLE	2


int CALLBACK
PLCompareFunction(
	LPARAM	lParam1,
	LPARAM	lParam2,
	LPARAM	nSortField
	)
{
	TASK_LIST*	Task1 = (TASK_LIST*)lParam1;
	TASK_LIST*	Task2 = (TASK_LIST*)lParam2;
	
	
	switch (nSortField)
	{
		case PL_INDEX_NAME:
			return lstrcmpi (Task1->ProcessName, Task2->ProcessName);

		case PL_INDEX_PID:
			return ((int)Task1->dwProcessId - (int)Task2->dwProcessId);

		case PL_INDEX_TITLE:
		{
			if (!Task1->hwnd && !Task2->hwnd)
				return 0;
				
			if (!Task1->hwnd)
				return 1;

			if (!Task2->hwnd)
				return -1;
			
			return lstrcmpi (Task1->WindowTitle, Task2->WindowTitle);
		}

		default:
			ASSERT (FALSE);
	}

	return 0;
}


BOOL
CAttachToActive::OnNotify(
	WPARAM		wParam,
	LPARAM		lParam,
	LRESULT*	lResult
	)
{
	switch ( ((LPNMHDR)lParam)->code )
	{
		case LVN_COLUMNCLICK:
			return OnLvnColumnClick ((NM_LISTVIEW*) lParam, lResult);
		case NM_DBLCLK:
			return OnNmDblClk((NMHDR*) lParam, lResult);
	}

	return 0;
}

BOOL
CAttachToActive::OnLvnColumnClick(
	NM_LISTVIEW*	ListData,
	LRESULT*		lResult
	)
{
	
	CListCtrl*	ProcessList = (CListCtrl*) GetDlgItem (IDC_PROCESS_LIST);
	ULONG		nSortField;

	ASSERT (ProcessList);

	//
	//	this is a sort request
	
	ProcessList->SortItems (PLCompareFunction, ListData->iSubItem);

	return TRUE;
}

BOOL
CAttachToActive::OnNmDblClk(
	NMHDR*		Data,
	LRESULT*	lResult
	)
{
	OnOK();
	return TRUE;
}

BOOL
CAttachToActive::OnInitDialog(
	)
/*++

Routine Description:

	Initialize dialog with list of processes.

Return Value:

	Returns TRUE to denote success.

--*/
{
	C3dDialog::OnInitDialog ();

	CButton* SysProc = (CButton*) GetDlgItem (IDC_SYSTEM_PROCESSES);

	SysProc->SetCheck (FALSE);

	m_TaskList = new TASK_LIST [512];
	m_nTasks = 512;
	
	CListCtrl*	ProcessList = (CListCtrl*)GetDlgItem (IDC_PROCESS_LIST);
    CString str;

    str.LoadString(IDS_PROCESS);
	ProcessList->InsertColumn (PL_INDEX_NAME, str, LVCFMT_LEFT, 110);

    str.LoadString(IDS_PROCESSID);
	ProcessList->InsertColumn (PL_INDEX_PID, str, LVCFMT_LEFT, 75);

    str.LoadString(IDS_TITLE);
	ProcessList->InsertColumn (PL_INDEX_TITLE, str, LVCFMT_LEFT, 200);

	FillProcessList (FALSE);
	return TRUE;
}


void
CAttachToActive::EmptyProcessList(
	)
{
	CListCtrl*	ProcessList = (CListCtrl*) GetDlgItem (IDC_PROCESS_LIST);

	ProcessList->DeleteAllItems ();
}
	

void
CAttachToActive::FillProcessList(
	BOOL	fSystemProcesses
	)
{
	TASK_LIST_ENUM	TaskListEnum;
	int				nTasks;
	TCHAR			Buffer [15];
	int				nItem;
	ULONG			CurrentProcessId  = GetCurrentProcessId ();
	CListCtrl*		ProcessList = (CListCtrl*)GetDlgItem (IDC_PROCESS_LIST);
	CWaitCursor		boy_oh_boy_is_c_plus_plus_a_neato_language;
	bool            fWinNT = (GetOsVersion() == VER_PLATFORM_WIN32_NT); 

	if (!InitPlistApi ()) 
	{
		return;
	}

	SetDebugPrivilege (fSystemProcesses);
	do 
	{
	    nTasks = GetTaskList (m_TaskList, m_nTasks);

	    if (m_nTasks < nTasks && fWinNT) 
		{
	    	//
	    	// compensate for new PSAPI code - NT alone)
	    	//

	    	m_nTasks   = nTasks; 
	        m_TaskList = new TASK_LIST [nTasks];

		    if (!m_TaskList) 
		    {
		    	return;
		    }
		} else {
			break;
		}

	} while (1);

	TaskListEnum.tlist = m_TaskList;
	TaskListEnum.numtasks = nTasks;
	GetWindowTitles (&TaskListEnum);

	/*
	for (int i = 0; i < nTasks; i++)
	{
		if (m_TaskList [i].dwProcessId == CurrentProcessId)
		{
			// do not list our own process
			
			m_TaskList [i].BinaryType = IMAGE_BAD_EXECUTABLE;
			continue;
		}

		//
		// get imageinfo is already done by GetTaskList on NT
		//
		if (!GetTaskImageInfo (&m_TaskList [i]))
		{
		    m_TaskList [i].BinaryType = IMAGE_BAD_EXECUTABLE;
		}
	}
	*/

	FreePlistApi ();


	
	for (int i = 0; i < nTasks; i++)
	{
		if (m_TaskList [i].dwProcessId == CurrentProcessId) {
			continue;
		}


		
		if (fWinNT) {
			if (!fSystemProcesses &&  
				(m_TaskList[i].flags & TASK_SYSTEM_PROCESS)) {
				continue;
			}
		    _ultoa (m_TaskList[i].dwProcessId, Buffer, 10);
		} else {
		    //
	    	//	if image is anything but a 32 bit GUI or CUI App, ignore
			if (!GetTaskImageInfo(&m_TaskList[i])) 
			{
				continue;
		    }

		    if (m_TaskList [i].BinaryType != IMAGE_SUBSYSTEM_WINDOWS_CUI &&
		    	m_TaskList [i].BinaryType != IMAGE_SUBSYSTEM_WINDOWS_GUI) {
		    	continue;
			}
		    _ultoa (m_TaskList[i].dwProcessId, Buffer, 16);
		}

			
		nItem = ProcessList->InsertItem (0, m_TaskList[i].ProcessName);
		ProcessList->SetItemText (nItem, 1, Buffer);
		ProcessList->SetItemText (nItem, 2, (m_TaskList[i].hwnd ? m_TaskList [i].WindowTitle : "                      "));
		ProcessList->SetItemData (nItem, (LPARAM)&m_TaskList [i]);
	}

	ProcessList->SetItemState (0, LVNI_SELECTED, LVNI_SELECTED);

}



void
CAttachToActive::OnChangeSystemProcess(
	)
{
	CButton*	SysProc = (CButton*) GetDlgItem (IDC_SYSTEM_PROCESSES);

	EmptyProcessList ();
	
	if (SysProc->GetCheck ())
	{
		FillProcessList (TRUE);
	}
	else
	{
		FillProcessList (FALSE);
	}
}


	
void
CAttachToActive::OnOK(
	)
{
	int			nItem;
	CListCtrl*	ProcessList = (CListCtrl*) GetDlgItem (IDC_PROCESS_LIST);
	CString		str;
	PTASK_LIST	Task;

	ASSERT (ProcessList);

	nItem = ProcessList->GetNextItem (-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		Task = (PTASK_LIST) ProcessList->GetItemData (nItem);

		ASSERT (Task);

		m_ProcessId = Task->dwProcessId;
		m_ProcessName = Task->ProcessName;
		m_ImageName = Task->ImageName ;
		C3dDialog::OnOK ();
		return;
	}

	C3dDialog::OnCancel ();
}


//
//	List Module Dialog
//

static const char *GetBaseName( const char *pFullPath )
{
	const char *pSlash = _tcsrchr( pFullPath, '\\' );
	return pSlash ? pSlash+1 : pFullPath;
}
//
//	CModuleDialog Message Map

BEGIN_MESSAGE_MAP(CModuleDialog, C3dDialog)
	ON_BN_CLICKED (IDC_SYSTEM_PROCESSES,OnChangeModule)
END_MESSAGE_MAP()

CModuleDialog::CModuleDialog(
	CWnd*	pParent // = NULL
	)
	: C3dDialog (CModuleDialog::IDD, pParent)
{
	m_pModuleList = NULL;
}

CModuleDialog::~CModuleDialog(
	)
{
	if (m_pModuleList)
		BMFree( m_pModuleList );
	m_pModuleList = NULL;
}

void
CModuleDialog::DoDataExchange(
	CDataExchange*	pDX
	)
{
	C3dDialog::DoDataExchange (pDX);
}

#define PL_BASENAME		0
#define PL_ADDR			1
#define PL_FULLPATH		2
#define	PL_ORDER		3


int CALLBACK
PLCompareFunctionModule(
	LPARAM	lParam1,
	LPARAM	lParam2,
	LPARAM	nSortField
	)
{
	LPMODULE_ENTRY Mod1 = (LPMODULE_ENTRY)lParam1;
	LPMODULE_ENTRY Mod2 = (LPMODULE_ENTRY)lParam2;

	ASSERT( Mod1 && Mod2 );

	switch (nSortField)
	{
		case PL_BASENAME:
			return _tcsicmp (GetBaseName(ModuleEntryName(Mod1)), GetBaseName(ModuleEntryName(Mod2)) );

		case PL_ADDR:
			if (ModuleEntryBase(Mod1) > ModuleEntryBase(Mod2))
				return 1;
			else if (ModuleEntryBase(Mod1) != ModuleEntryBase(Mod2))
				return -1;
			else
				return 0;

		case PL_FULLPATH:
			return _tcsicmp (ModuleEntryName(Mod1), ModuleEntryName(Mod2) );

		case PL_ORDER:
			return (Mod1-Mod2);

		default:
			ASSERT (FALSE);
	}

	return 0;
}


BOOL
CModuleDialog::OnNotify(
	WPARAM		wParam,
	LPARAM		lParam,
	LRESULT*	lResult
	)
{
	switch ( ((LPNMHDR)lParam)->code )
	{
		case LVN_COLUMNCLICK:
			return OnLvnColumnClick ((NM_LISTVIEW*) lParam, lResult);
		case NM_DBLCLK:
			return OnNmDblClk((NMHDR*) lParam, lResult);
	}

	return 0;
}

BOOL
CModuleDialog::OnLvnColumnClick(
	NM_LISTVIEW*	ListData,
	LRESULT*		lResult
	)
{
	
	CListCtrl*	ProcessList = (CListCtrl*) GetDlgItem (IDC_MODULE_LIST);
	ULONG		nSortField;

	ASSERT (ProcessList);

	//
	//	this is a sort request
	
	ProcessList->SortItems (PLCompareFunctionModule, ListData->iSubItem);

	return TRUE;
}

BOOL
CModuleDialog::OnNmDblClk(
	NMHDR*		Data,
	LRESULT*	lResult
	)
{
//	OnOK();
	return TRUE;
}

BOOL
CModuleDialog::OnInitDialog(
	)
/*++

Routine Description:

	Initialize dialog with list of modules

Return Value:

	Returns TRUE to denote success.

--*/
{
	C3dDialog::OnInitDialog ();

	CListCtrl*	ProcessList = (CListCtrl*)GetDlgItem (IDC_MODULE_LIST);
	DWORD dwStyle = ProcessList->GetExtendedStyle();
	ProcessList->SetExtendedStyle( dwStyle | LVS_EX_FULLROWSELECT );

    CString str;

    str.LoadString(IDS_MODULENAME);
	ProcessList->InsertColumn (PL_BASENAME, str, LVCFMT_LEFT, 100 );

    str.LoadString(IDS_ADDRESS);
	ProcessList->InsertColumn (PL_ADDR, str, LVCFMT_LEFT, 200 );

    str.LoadString(IDS_FULLPATH);
	ProcessList->InsertColumn (PL_FULLPATH, str, LVCFMT_LEFT, 300 );

	str = "Order";
	ProcessList->InsertColumn (PL_ORDER, str, LVCFMT_LEFT, 40 );

	FillModuleList ();
	return TRUE;
}


void
CModuleDialog::EmptyModuleList(
	)
{
	if (m_pModuleList)
		BMFree( m_pModuleList );
	m_pModuleList = NULL;

	CListCtrl*	ProcessList = (CListCtrl*) GetDlgItem (IDC_MODULE_LIST);

	ProcessList->DeleteAllItems ();
}
	

void
CModuleDialog::FillModuleList(
	)
{
	int				nTasks;
	int				nItem;
	CListCtrl*		ProcessList = (CListCtrl*)GetDlgItem (IDC_MODULE_LIST);
	CWaitCursor		boy_oh_boy_is_c_plus_plus_a_neato_language;

	XOSD xosd = OSDGetModuleList(
		hpidCurr, 
		htidCurr,
		NULL,
		&m_pModuleList
	);

	if (xosd!=xosdNone)
		return;

	LPMODULE_ENTRY pMod = FirstModuleEntry( m_pModuleList );

	for (int i=0; i<ModuleListCount(m_pModuleList); i++)
	{
// code used for Win95 testing only
//		if (ModuleEntryBase(pMod) > 0x70000000)
//			ModuleEntryBase(pMod) += 0x80000000;

		nItem = ProcessList->InsertItem( i, GetBaseName( ModuleEntryName(pMod) ) );

		TCHAR szAddr[ MAX_PATH ];
		wsprintf( szAddr, "0x%08lX - 0x%08lX", ModuleEntryBase(pMod), ModuleEntryBase(pMod)+ModuleEntryLimit(pMod)-1 );
		ProcessList->SetItemText( nItem, PL_ADDR, szAddr );

		ProcessList->SetItemText( nItem, PL_FULLPATH, ModuleEntryName(pMod) );

		wsprintf( szAddr, "%d", pMod-FirstModuleEntry(m_pModuleList)+1 );
		ProcessList->SetItemText( nItem, PL_ORDER, szAddr );

		ProcessList->SetItemData( nItem, (LPARAM)pMod );

		pMod = NextModuleEntry( pMod );
	}

}



void
CModuleDialog::OnChangeModule(
	)
{
	EmptyModuleList ();
	FillModuleList ();
}


	
void
CModuleDialog::OnOK(
	)
{
	C3dDialog::OnOK ();
}


//
//	No symbolic information dialog
//

BEGIN_MESSAGE_MAP(CNoSymbolInfoDlg, C3dDialog)
END_MESSAGE_MAP()


CNoSymbolInfoDlg::CNoSymbolInfoDlg(
	CWnd*	pParent // = NULL
	)
	: C3dDialog (CNoSymbolInfoDlg::IDD, pParent)
{
	m_fNoPrompt = FALSE;
	m_strCaption = "";
	m_strPrompt = "";
}


void
CNoSymbolInfoDlg::DoDataExchange(
	CDataExchange*	pDX
	)
{
	C3dDialog::DoDataExchange (pDX);

	DDX_Check (pDX, IDC_NO_PROMPT, m_fNoPrompt);
}


BOOL
CNoSymbolInfoDlg::OnInitDialog(
	)
{
	CString 	str;
	TCHAR		buffer [512];
	CStatic*	st = (CStatic*) GetDlgItem (IDC_PROMPT);

	if (m_strCaption != "")
	{
		SetWindowText (m_strCaption);
	}

	str.LoadString (ERR_No_Debug_Info);
	sprintf (buffer, str, (LPCTSTR) m_strPrompt);
	m_strPrompt = buffer;

	st->SetWindowText (m_strPrompt);

	return TRUE;
}


//
//	CFibers Message Map

BEGIN_MESSAGE_MAP(CFibers, C3dDialog)
END_MESSAGE_MAP()


CFibers::CFibers(
	CWnd*	pParent // = NULL
	)
	: C3dDialog (CFibers::IDD, pParent)
{
}

#define FL_INDEX_NAME	0
#define FL_INDEX_PID	1
#define FL_INDEX_TITLE	2

CFibers::~CFibers(
	)
{

	delete [] m_FbrLst;
	m_FbrCntx = NULL;
}

void
CFibers::DoDataExchange(
	CDataExchange*	pDX
	)
{
	C3dDialog::DoDataExchange (pDX);
}

int CALLBACK
FLCompareFunction(
	LPARAM	lParam1,
	LPARAM	lParam2,
	LPARAM	nSortField
	)
{
	PFBRLST fbrl1 = (PFBRLST) lParam1;
	PFBRLST fbrl2 = (PFBRLST) lParam2;
	
	switch (nSortField)
	{
		case FL_INDEX_NAME:
			return lstrcmpi (fbrl1->strFbr, fbrl2->strFbr);
			break;
		case FL_INDEX_PID:
			return ((int)fbrl1->FbrCntx - (int)fbrl2->FbrCntx);
			break;
		case FL_INDEX_TITLE:
		{
			return 0;
		}
		default:
			ASSERT (FALSE);
	}

	return 0;
}

extern BOOL GetSymbolFromAddr(PADDR, CString&);


BOOL
CFibers::OnNotify(
	WPARAM		wParam,
	LPARAM		lParam,
	LRESULT*	lResult
	)
{
	switch ( ((LPNMHDR)lParam)->code )
	{
		case LVN_COLUMNCLICK:
			return OnLvnColumnClick ((NM_LISTVIEW*) lParam, lResult);
	}

	return 0;
}

BOOL
CFibers::OnLvnColumnClick(
	NM_LISTVIEW*	ListData,
	LRESULT*		lResult
	)
{
	
	CListCtrl*	FiberList = (CListCtrl*) GetDlgItem (IDC_FIBERS_LIST);
	ULONG		nSortField;

	ASSERT (FiberList);

	//
	//	this is a sort request
	
	FiberList->SortItems (FLCompareFunction, ListData->iSubItem);

	return TRUE;
}
		
BOOL
CFibers::OnInitDialog(
	)
/*++

Routine Description:

	Initialize dialog with list of processes.

Return Value:

	Returns TRUE to denote success.

--*/
{
	TASK_LIST_ENUM	TaskListEnum;
	TCHAR			Buffer [15];
	TCHAR			Location[256];
	DWORD				nFbrs=0;
	DWORD			nItem;
	OFBRS			ofbrs;
	DWORD			sizebuf[16];
	DWORD			*dwFbrLst;

	C3dDialog::OnInitDialog ();
	
	CListCtrl*	FiberList = (CListCtrl*)GetDlgItem (IDC_FIBERS_LIST);

	FiberList->InsertColumn (FL_INDEX_NAME, "Fiber", LVCFMT_LEFT, 110);
	FiberList->InsertColumn (FL_INDEX_PID, "Fiber Id", LVCFMT_LEFT, 75);
	//FiberList->InsertColumn (FL_INDEX_TITLE, "Title", LVCFMT_LEFT, 150);

	ofbrs.op = OFBR_QUERY_LIST_SIZE;
	// Sytem services return values in the same buffer as the command
	// so copy the command to the return buffer.
	memcpy(sizebuf,&ofbrs,sizeof(OFBRS));
	OSDSystemService (hpidCurr,
					  htidCurr,
					  ssvcFiberDebug,
					  (LPVOID) sizebuf,
					  sizeof(OFBRS),//should use two calls
					  &nFbrs
					  );
	int lstsize = *((int *)sizebuf);
	// must have enough room for OFBRS
	lstsize = (lstsize < sizeof(OFBRS)) ? sizeof(OFBRS) : lstsize;
	dwFbrLst = new DWORD [lstsize];
	m_FbrLst = new FBRLST [lstsize];

	ofbrs.op = OFBR_GET_LIST;
	// Sytem services return values in the same buffer as the command
	// so copy the command to the return buffer.
	memcpy(dwFbrLst,&ofbrs,sizeof(OFBRS));
	OSDSystemService (hpidCurr,
					  htidCurr,
					  ssvcFiberDebug,
					  (LPVOID) dwFbrLst,
					  lstsize,//should use two calls
					  &nFbrs
					  );
	nFbrs = nFbrs>>2;//number of fibers rather than bytes
	for (int i = 0; i < nFbrs; i++)
	{
		CString	strLocation;
		ADDR Addr = {0};
		_snprintf(Buffer,16,"0x%08x",dwFbrLst[i]);
		m_FbrLst[i].FbrCntx = (LPVOID) dwFbrLst[i];
		ofbrs.op = OFBR_SET_FBRCNTX;
		ofbrs.FbrCntx = (LPVOID) dwFbrLst[i];
		OSDSystemService (hpidCurr,
					  htidCurr,
					  ssvcFiberDebug,
					  (LPVOID) &ofbrs,
					  sizeof(OFBRS),
					  NULL
					  );

		HTID htid = htidCurr;
		OSDGetFrame(hpidCurr,htid,1,&htid);		
		OSDGetAddr(hpidCurr,htid,adrPC,(PADDR)&Addr);
		// Get the thread location.  First, see if we can determine a function
		// name for the current CS:EIP.  If we can't, then look up the stack
		// for the first address for which we DO have symbols.
		if (!GetSymbolFromAddr(&Addr,strLocation))
		{
		// If the address that the top of stack doesn't have a symbol,
		// then look up the stack, and if we find something we recognize,
		// put its function name in brackets
			int i;
			HFME hfme;
			LPFME lpfme;

			for (i = 0; (hfme = CLHfmeGetNth(i)) != NULL; ++i)
			{
				lpfme = (LPFME)LLLpvFromHlle(hfme);
				if (lpfme->clt == cltProc)
				{
					// found a function: format as "[func_name]"
					if (GetSymbolFromAddr(&lpfme->addrProc, strLocation))
					{
						_snprintf(Location,256, "[%s]",
							(const char*)strLocation);
						break;
					}
				}
				UnlockHlle(hfme);
			}
			if (hfme) {
				UnlockHlle(hfme);
			}
		}
		else
		{
			_tcsncpy(Location, (LPCTSTR)strLocation, 256);
			Location[255-1] = '\0';
		}
		m_FbrLst[i].strFbr = Location;
		nItem = FiberList->InsertItem (0, Location);
		FiberList->SetItemText (nItem, 1, Buffer);
		//FiberList->SetItemText (nItem, 2, "                      ");
		FiberList->SetItemData (nItem, (LPARAM)&(m_FbrLst[i]));
	}

	FiberList->SetItemState (0, LVNI_SELECTED, LVNI_SELECTED);
	
	// Put the old thread back
	ofbrs.op = OFBR_SET_FBRCNTX;
	ofbrs.FbrCntx = NULL;	
	OSDSystemService (hpidCurr,
					  htidCurr,
					  ssvcFiberDebug,
					  (LPVOID) &ofbrs,
					  sizeof(OFBRS),
					  NULL
					  );
	HTID htid = htidCurr;
	OSDGetFrame(hpidCurr,htid,1,&htid);		
	delete [] dwFbrLst;
	return TRUE;  
}


void
CFibers::OnOK(
	)
{
	int			nItem;
	CListCtrl*	FiberList = (CListCtrl*) GetDlgItem (IDC_FIBERS_LIST);
	CString		str;
	PFBRLST		fbrl;
	
	ASSERT (FiberList);

	nItem = FiberList->GetNextItem (-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		fbrl = (PFBRLST) FiberList->GetItemData (nItem);

		ASSERT (fbrl);

		m_FbrCntx = fbrl->FbrCntx;
		C3dDialog::OnOK ();
		return;
	}

	C3dDialog::OnCancel ();
}
	
