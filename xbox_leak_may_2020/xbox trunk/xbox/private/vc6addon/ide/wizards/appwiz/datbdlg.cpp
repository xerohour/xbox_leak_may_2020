// datbdlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcappwz.h"
#include "symbols.h"
#include "datbdlg.h"

#pragma warning(disable:4103)
#ifndef VS_PACKAGE
#include <initguid.h>
#include <clwzguid.h>
#endif	// VS_PACKAGE
#pragma warning(default:4103)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBDlg dialog

CDBDlg::CDBDlg(CWnd* pParent /*=NULL*/)
    : CSeqDlg(CDBDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDBDlg)
    m_nDBOptions = -1;
    //}}AFX_DATA_INIT
#ifndef VS_PACKAGE
	m_lpWiz = NULL;
#endif	// VS_PACKAGE
}

void CDBDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDBDlg)
    DDX_Radio(pDX, IDC_DB_RADIO, m_nDBOptions);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDBDlg, CSeqDlg)
    //{{AFX_MSG_MAP(CDBDlg)
    ON_BN_CLICKED(IDC_DATA_SOURCE, OnClickedDataSource)
    ON_COMMAND_EX(IDC_DB_RADIO, OnClickedDBOption)
    ON_COMMAND_EX(IDC_DB_HEADER, OnClickedDBOption)
    ON_COMMAND_EX(IDC_DB_SIMPLE, OnClickedDBOption)
    ON_COMMAND_EX(IDC_DB_DOCVIEW, OnClickedDBOption)
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBDlg message handlers

void RemoveCarriageReturns(CString& str)
{
	CString strOld = str;
	str.Empty();
	int nLength = strOld.GetLength();
	for (int i=0; i < nLength; i++)
	{
		if (strOld[i] == '\r')
			continue;
		str += strOld[i];
	}
}

void CDBDlg::RunDataSource(CDialog* dlg)
{
	BOOL bBindAll = TRUE;
	CString strSQL, strConnect, strTableType;
	CStringList columnList;
	CStringList defVarsList;
	CString strParamVars, strParamBindings, strParamMap;

	GetSymbol("DB_SOURCE", strSQL);
	GetSymbol("DB_TABLE_TYPE", strTableType);
	GetSymbol("DB_CONNECT", strConnect);

	BOOL bEnableDAO = !DoesSymbolExist("TARGET_MAC");
	// ASSERT(!projOptions.m_bDAO || bEnableDAO);	// Can't default to DAO if we're disabling DAO
	
#ifndef VS_PACKAGE
	CString     strOpen;
	if (m_lpWiz != NULL)
	{
		CString strTableClass;
		if (SUCCEEDED(m_lpWiz->PickDataSource(projOptions.m_nDataSource, bBindAll, projOptions.m_bAutoDetect,
			bEnableDAO, strSQL, strConnect, strTableType, columnList, defVarsList,
			strParamVars, strParamBindings, strParamMap, strOpen, strTableClass, dlg)))
		{
			SetSymbol("DB_CONNECT", strConnect);
			//SetSymbol("DB_DSN", srcDlg.m_strDatasource);
			if(projOptions.m_nDataSource != dbOledb ){
				SetSymbol("DB_SOURCE", strSQL);
				SetSymbol("DB_DSN", strConnect);
				SetSymbol("DB_TABLE_TYPE", strTableType);
			
				char szCols[MAX_SHORT_TMP];
				wsprintf(szCols, "%d", columnList.GetCount());
				SetSymbol("DB_NUMCOLS", szCols);
	
				CString strInfo = "";
				int n = 1;
				char szT[MAX_SHORT_TMP];
				while (!columnList.IsEmpty())
				{
					wsprintf(szT, "Column%d=", n++);
					strInfo += szT;
					strInfo += columnList.RemoveHead() + '\n';
				}
				SetSymbol("DB_COLSINFO", strInfo);
	
				strInfo.Empty();
				while (!defVarsList.IsEmpty())
					strInfo += defVarsList.RemoveHead() + '\n';
				SetSymbol("DB_VARSINFO", strInfo);
	
				RemoveCarriageReturns(strParamVars);
				RemoveCarriageReturns(strParamBindings);
				RemoveCarriageReturns(strParamMap);
				SetSymbol("PARAM_VARS", strParamVars);
				SetSymbol("PARAM_VAR_BINDINGS", strParamBindings);
				SetSymbol("PARAM_RFX", strParamMap);
			}
			else
			{
				CString strClass = strTableClass; // GetTag destroys the first param before reading the second.
				GetTag(strTableClass,strClass);
				strTableClass =  "C" + strTableClass;
				SetSymbol("DB_SOURCE", strSQL);
				SetSymbol("DB_TABLECLASS",strTableClass);
				SetSymbol("DB_NUMCOLS", columnList.RemoveHead());
				SetSymbol("DB_OPEN", strOpen);
				if( !columnList.IsEmpty() )
					SetSymbol("DB_COLSINFO", columnList.RemoveHead());
				if( !defVarsList.IsEmpty() )
					SetSymbol("DB_VARSINFO", defVarsList.RemoveHead());
			}
		}
		else
		{
			RemoveSymbol("DB_SOURCE");
			RemoveSymbol("DB_DSN");
		}
	}
#endif	// VS_PACKAGE
}

void CDBDlg::UpdateDatasrcText()
{
	CString strMessage;
	CString strSource, strDSN;
#ifndef VS_PACKAGE
	if (m_lpWiz != NULL)
	{
		if( DoesSymbolExist("DB_SOURCE") &&
					(projOptions.m_nDataSource == dbOledb) )
		{
			strMessage.LoadString(IDS_OLEDATASOURCE_TEXT);
			GetDlgItem(IDC_DATASOURCE_TEXT)->SetWindowText(strMessage);
		} else {
			if (m_lpWiz->IsDBAvailable() != S_OK)
			{
				strMessage.LoadString(IDS_NO_ODBC);
				RemoveSymbol("DB_SOURCE");
				RemoveSymbol("DB_DSN");
			}
			else if (!DoesSymbolExist("DB_SOURCE")
				|| !DoesSymbolExist("DB_DSN"))
			{
				AfxFormatString1(strMessage, IDS_NODATASOURCE_TEXT, "");
			}
			else
			{
				GetSymbol("DB_SOURCE", strSource);
				GetSymbol("DB_DSN", strDSN);
				MakeUserString(strDSN);		// need to kill any double backslashes
				AfxFormatString2(strMessage, IDS_DATASOURCE_TEXT, strSource, strDSN);
			}
		}

		GetDlgItem(IDC_DATASOURCE_TEXT)->SetWindowText(strMessage);
	}
#endif	// VS_PACKAGE
}

void CDBDlg::MakeUserString(CString& str)
{
	CString strResult;

	// the output string can't be longer than the input
	LPTSTR lpszOut = strResult.GetBuffer(str.GetLength());
	LPCTSTR lpszIn = str;

	while (*lpszIn)
	{
		// if we have 2 slashes in a row, skip one of them
		_tccpy(lpszOut, lpszIn);
		lpszOut = _tcsinc(lpszOut); 
		lpszIn = _tcsinc(lpszIn);
		if (*lpszIn == _T('\\'))
			lpszIn = _tcsinc(lpszIn);
	}
	*lpszOut = _T('\0');
	strResult.ReleaseBuffer();
	str = strResult;
}

void CDBDlg::OnClickedDataSource()
{
    RunDataSource((CDialog*) m_pParentWnd);
	UpdateDatasrcText();
}

// Hide or show Data Sources button depending current option.
//  Also, update bitmap
BOOL CDBDlg::OnClickedDBOption(UINT nID)
{
    CButton* pDataSrc = (CButton*) GetDlgItem(IDC_DATA_SOURCE);
    if (nID == IDC_DB_RADIO || nID == IDC_DB_HEADER)
    {
        //   pDataSrc->ShowWindow(SW_HIDE);
        pDataSrc->EnableWindow(FALSE);
    }
    else
    {
        // pDataSrc->ShowWindow(SW_SHOW);
        pDataSrc->EnableWindow(TRUE);
		pDataSrc->SetButtonStyle(BS_PUSHBUTTON);
    }
	CMainDlg* pMainDlg = (CMainDlg*) GetParent();
	pMainDlg->RedrawBitmap(BMPSIZE_SUM);
    return TRUE;
}

BOOL CDBDlg::OnInitDialog()
{
#ifndef VS_PACKAGE
	theApp.FindInterface(IID_IClassWizard, (LPVOID*) &m_lpWiz);
#endif	// VS_PACKAGE

    CSeqDlg::OnInitDialog();
	VERIFY(m_btnDataSource.SubclassDlgItem(IDC_DATA_SOURCE, this));
	CString tmp;

	// On the Mac, there is no DAO
	if (DoesSymbolExist("TARGET_MAC"))
		GetDlgItem(IDC_NOMACDAO)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_NOMACDAO)->ShowWindow(SW_HIDE);

	if (DoesSymbolExist("TARGET_MAC") && !IsMacODBC())
	{
		// MacODBC is not around, so select no ODBC support & disable the controls
		DisableAll();
		CString strMessage;
		strMessage.LoadString(IDS_NO_MACODBC);
		GetDlgItem(IDC_DATASOURCE_TEXT)->SetWindowText(strMessage);
	}
#ifndef VS_PACKAGE
    // ClassWizard interface will be NULL if Java Package only is loaded
	else if (m_lpWiz != NULL && m_lpWiz->IsDBAvailable() != S_OK)
	{
		DisableAll();
		UpdateDatasrcText();
	}
#endif	// VS_PACKAGE
	else if(DoesSymbolExist("NODOCVIEW"))
	{
		GetDlgItem(IDC_DB_SIMPLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DB_DOCVIEW)->EnableWindow(FALSE);
		m_nDBOptions = 0;
	    UpdateData(FALSE);
		UpdateDatasrcText();
	}
	else
	{
	    ReadValues();
	
	    // Hide/set Data Source button
	    CButton *pDataSrc = (CButton*) GetDlgItem(IDC_DATA_SOURCE);
	    if (m_nDBOptions == 0 || m_nDBOptions == IDC_DB_HEADER-IDC_DB_RADIO)
	    {
	        //   pDataSrc->ShowWindow(SW_HIDE);
	        pDataSrc->EnableWindow(FALSE);
	    }
	    else
	    {
	        // pDataSrc->ShowWindow(SW_SHOW);
	        pDataSrc->EnableWindow(TRUE);
	    }
	
	    UpdateData(FALSE);
		UpdateDatasrcText();
	}
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDBDlg::DisableAll()
{
	m_nDBOptions = 0;
	UpdateData(FALSE);
	GetDlgItem(IDC_STATIC1)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC2)->EnableWindow(FALSE);
	GetDlgItem(IDC_DB_RADIO)->EnableWindow(FALSE);
	GetDlgItem(IDC_DB_HEADER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DB_SIMPLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DB_DOCVIEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_DATA_SOURCE)->EnableWindow(FALSE);
}

// We override PreTranslate to make sure that if Data Sources button is default,
//  <return> causes it to be activated.
BOOL CDBDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN
		&& ::GetFocus() == GetDlgItem(IDC_DATA_SOURCE)->GetSafeHwnd())
	{
		SendMessage(WM_COMMAND,
			MAKEWPARAM(IDC_DATA_SOURCE, BN_CLICKED),
			(LPARAM) (GetDlgItem(IDC_DATA_SOURCE)->GetSafeHwnd()));
		return TRUE;
	}

	return CSeqDlg::PreTranslateMessage(pMsg);
}

void SetOleSymbols();  // In mfcappwz.cpp.  Referenced in CanDestroy().
BOOL CDBDlg::CanDestroy()
{
	WriteValues();
    CString tmp;
    // We can destroy the dialog, as long as either (1) we don't need a
    //  a data source, or (2) we have a data source.
    if ((m_nDBOptions != IDC_DB_DOCVIEW-IDC_DB_RADIO
        && m_nDBOptions != IDC_DB_SIMPLE-IDC_DB_RADIO)
        || (DoesSymbolExist("DB_SOURCE")))
    {
        // If the user chose an OLE option, give him the chance to escape, lest
        //  OLE return to zero support.  (simple + OLE don't mix).
        if (projOptions.m_bDBSimple == TRUE
            && projOptions.m_nOleType != OLE_NO)
        {
            int nResult = AfxMessageBox(IDP_OLE_THEN_SIMPLE, MB_OKCANCEL);
            if (nResult == IDCANCEL)
                return FALSE;
            projOptions.m_nOleType = OLE_NO;
			SetOleSymbols();    // Reset corresponding symbols to reflect this change.
        }

 		if (m_nDBOptions ==	(IDC_DB_SIMPLE-IDC_DB_RADIO) && IsMDI())	
		{
			// no file support, so we can't be MDI
			// need to give him the chance to escape or at least know
			// what is happening to him
			int nResult = AfxMessageBox(IDP_MDI_THEN_SIMPLE, MB_OKCANCEL);
			if (nResult == IDCANCEL)
				return FALSE;
			SetMDI(FALSE);
		}

		// Ditto for previously selected MAPI support
		if ((projOptions.m_options & (1 << (IDCD_POMAPI-IDCD_POBASE)))
			&& projOptions.m_bDBSimple)
        {
            int nResult = AfxMessageBox(IDP_MAPI_THEN_SIMPLE, MB_OKCANCEL);
            if (nResult == IDCANCEL)
                return FALSE;
            projOptions.m_options &= ~(1 << (IDCD_POMAPI-IDCD_POBASE));
        }
    }
    else
    {
        AfxMessageBox(IDP_DB_NO_DATA_SOURCE);
        return FALSE;
    }
	SetDBSymbols();
	SetBaseViewSymbols();
    return TRUE;
}

void CDBDlg::OnDestroy()
{
#ifndef VS_PACKAGE
    // ClassWizard interface will be NULL if Java Package only is loaded
	if (m_lpWiz != NULL)
        m_lpWiz->Release();
#endif	// VS_PACKAGE
    WriteValues();
    CSeqDlg::OnDestroy();
}

static UINT DBbmp[] =
{
	IDB_B121000, IDB_B120100, IDB_B120001, IDB_B120010,
};

UINT CDBDlg::ChooseBitmap()
{
	for (int i=0; i < 4; i++)
	{
		if (((CButton*) GetDlgItem(IDC_DB_RADIO+i))->GetCheck())
			break;
	}
	ASSERT (((CButton*) GetDlgItem(IDC_DB_RADIO+i))->GetCheck());
	return DBbmp[i];
}

void CDBDlg::ReadValues()
{
	GetDBSymbols();
    // Determine what DBOption is currently selected by examining
    //  data gotten from other dialogs.
    if (projOptions.m_bDBSimple)
        m_nDBOptions = IDC_DB_SIMPLE-IDC_DB_RADIO;
    else if (projOptions.m_names.strBaseClass[classView] == "CRecordView" ||
			projOptions.m_names.strBaseClass[classView] == "COleDBRecordView" ||
			projOptions.m_names.strBaseClass[classView] == "CDaoRecordView")
        m_nDBOptions = IDC_DB_DOCVIEW-IDC_DB_RADIO;
    else if (projOptions.m_bDBHeader)
        m_nDBOptions = IDC_DB_HEADER-IDC_DB_RADIO;
    else
        m_nDBOptions = 0;
}

void CDBDlg::WriteValues()
{
	UpdateData(TRUE);
    // Affect data elsewhere, based on what DB option was selected
    switch(m_nDBOptions)
    {
    case 0:
        projOptions.m_bDBSimple = projOptions.m_bDBHeader = FALSE;
        if (projOptions.m_names.strBaseClass[classView] == "CRecordView" ||
			projOptions.m_names.strBaseClass[classView] == "COleDBRecordView" ||
			projOptions.m_names.strBaseClass[classView] == "CDaoRecordView")
            projOptions.m_names.strBaseClass[classView] = "CView";
        break;
    case IDC_DB_HEADER-IDC_DB_RADIO:
        projOptions.m_bDBSimple = FALSE;
        projOptions.m_bDBHeader = TRUE;
        if (projOptions.m_names.strBaseClass[classView] == "CRecordView" ||
			projOptions.m_names.strBaseClass[classView] == "COleDBRecordView" ||
			projOptions.m_names.strBaseClass[classView] == "CDaoRecordView")
            projOptions.m_names.strBaseClass[classView] = "CView";
        break;
    case IDC_DB_DOCVIEW-IDC_DB_RADIO:
        projOptions.m_bDBSimple = FALSE;
        projOptions.m_bDBHeader = TRUE;
		switch(projOptions.m_nDataSource) {
			case dbOdbc:
				projOptions.m_names.strBaseClass[classView] = "CRecordView";
				break;
			case dbDao:	
				projOptions.m_names.strBaseClass[classView] = "CDaoRecordView" ;
				break;
			case dbOledb:	
				projOptions.m_names.strBaseClass[classView] = "COleDBRecordView" ;
				break;
			default:
				break;
		}
        break;
    case IDC_DB_SIMPLE-IDC_DB_RADIO:
        projOptions.m_bDBSimple = projOptions.m_bDBHeader = TRUE;
		switch(projOptions.m_nDataSource) {
			case dbOdbc:
				projOptions.m_names.strBaseClass[classView] = "CRecordView";
				break;
			case dbDao:	
				projOptions.m_names.strBaseClass[classView] = "CDaoRecordView" ;
				break;
			case dbOledb:	
				projOptions.m_names.strBaseClass[classView] = "COleDBRecordView" ;
				break;
			default:
				break;
		}
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CMyButton
// This class serves the purpose of making an appwiz inner dlg that has a pushbutton
//  act like the user will expect.  When the button gains focus, we take defaultness
//  away from CMainDlg's IDOK, and make it the default.  When it loses focus,
//  defaultness returns to CMainDlg's IDOK.  All this enhances the illusion that
//  the inner dialog is really the same dialog as the outer CMainDlg, and so we don't
//  end up with two or no default pushbuttons.

CMyButton::CMyButton()
{
}

CMyButton::~CMyButton()
{
}


BEGIN_MESSAGE_MAP(CMyButton, CButton)
	//{{AFX_MSG_MAP(CMyButton)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyButton message handlers

void MakeOKDefault(CMainDlg*);

void CMyButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);
	CDialog* pParent = (CDialog*) GetParent();

	// This is shameful, but it works.  IDC_BOGUS_DEFBTN is an invisible btn.
	//  In order to tell NT that we REALLY mean it when we say
	//  "SetButtonStyle(BS_PUSHBUTTON)", apparently, NT needs a new defbtn in
	//  its place.  Otherwise, IDC_DATA_SOURCE magically becomes the default
	//  again.  So, we make this hacky call to SetDefID to make sure that
	//  IDC_DATA_SOURCE stays nondefault.
	pParent->SetDefID(IDC_BOGUS_DEFBTN);
	SetButtonStyle(BS_PUSHBUTTON);

	if (pNewWnd == NULL || pNewWnd->GetParent() == NULL
		|| GetParent()->GetParent()->GetSafeHwnd()  // i.e., mainDlgs hwnd
		!= pNewWnd->GetParent()->GetSafeHwnd())     // i.e., wnd containing pNewWnd
	{
		// Here, the new guy with the focus is NOT one of CMainDlg's buttons.
		//  So OK should be the default.  (Otherwise, the new guy with focus should be.)
		MakeOKDefault((CMainDlg*) (GetParent()->GetParent()));
				
	}  
}

void CMyButton::OnSetFocus(CWnd* pOldWnd)
{
	CButton::OnSetFocus(pOldWnd);

	// Now that we have the focus, we should be the default btn, and
	//  none of CMainDlg's buttons should be default.
	CMainDlg* pMainDlg = (CMainDlg*) (GetParent()->GetParent());
    ((CButton*) (pMainDlg->GetDlgItem(IDCANCEL)))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton*) (pMainDlg->GetDlgItem(IDC_BACK)))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton*) (pMainDlg->GetDlgItem(IDC_END)))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton*) (pMainDlg->GetDlgItem(IDOK)))->SetButtonStyle(BS_PUSHBUTTON);
	CDialog* pParent = (CDialog*) GetParent();
	pParent->SetDefID(GetDlgCtrlID());
	SetButtonStyle(BS_DEFPUSHBUTTON);
}
