// debugopt.cpp : implementation file
// Debugger tab dialog code.
// marklam	14-Oct-93	Created from rundebug.c, mwdlg.cpp, damdlg.cpp, clwdlg.cpp, rwvdlg.cpp

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL FSetupDLL(LPCSTR szFileName, LPCSTR szEntry, MPT mpt);
MPT	 MptFromPlatform (uniq_platform Platform);
BOOL WINAPI GetRegistryRoot (LPTSTR RegistryRoot, LPDWORD pLength);

// Create the tab page
CDlgTab *CreateDebugPage()
{
    if ( !gpIBldPlatforms )
        return NULL;

	return new CDebugOpt;
}

static void InitChildFont(CWnd *pwnd, int nID)
{
	CWnd *pChild = pwnd->GetDlgItem(nID);
	ASSERT(pChild);
	pChild->SetFont(GetStdFont(font_Normal));
}

void CDebugOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebugOpt)
	DDX_Text(pDX, IDC_DEBUG_ADDRESS, m_szExpr);
	DDX_Check(pDX, IDC_DEBUG_ANNOTATE, m_fAnnotate);
	DDX_Check(pDX, IDC_DEBUG_CODEBYTES, m_fCodeBytes);
	DDX_Check(pDX, IDC_DEBUG_FIXEDWIDTH, m_fFixedWidth);
	DDX_Check(pDX, IDC_DEBUG_HEXMODE, m_fHexMode);
	DDX_Check(pDX, IDC_DEBUG_LIVE, m_fLive);
	DDX_Check(pDX, IDC_DEBUG_OLERPC, m_fOLERPC);
	DDX_Check(pDX, IDC_DEBUG_RAW, m_fRaw);
	DDX_Check(pDX, IDC_DEBUG_SHOWARGS, m_fShowArgs);
	DDX_Check(pDX, IDC_DEBUG_SHOWFPU, m_fFPU);
	DDX_Check(pDX, IDC_DEBUG_SHOWTYPES, m_fShowTypes);
	DDX_Text(pDX, IDC_DEBUG_WIDTH, m_iItemsPerLine);
	DDX_Check(pDX, IDC_SYMBOLS, m_fSymbols);
	DDX_CBIndex(pDX, IDC_DEBUG_FORMAT, m_iFormat);
	DDX_Check(pDX, IDC_DEBUG_JIT, m_fEnableJit);
	DDX_Check(pDX, IDC_DEBUG_RETURN_VALUES, m_fReturnValues);
	DDX_Check(pDX, IDC_DEBUG_UNICODE_STRINGS, m_fUnicodeStrings);
    DDX_Check(pDX, IDC_DEBUG_EC, m_fENCDebugging);
    DDX_Check(pDX, IDC_DEBUG_LOADEXPORTS, m_fLoadExports);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDebugOpt, CDlgTab)
	//{{AFX_MSG_MAP(CDebugOpt)
	ON_BN_CLICKED(IDC_DEBUG_FIXEDWIDTH, OnClickedFixedwidth)
	ON_BN_CLICKED(IDC_DEBUG_JIT, OnClickedJit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Construction

CDebugOpt::CDebugOpt() : CDlgTab( IDDP_DEBUG, IDS_CAP_DEBUG )
{
	m_iFormat = runDebugParams.iMWFormat;
	m_fLive = runDebugParams.fMWLive;
	m_fRaw = runDebugParams.fMWShowRaw;
	m_fFPU = runDebugParams.fCPUFloat;
 	m_fShowArgs = runDebugParams.fCallParams;
	m_fShowTypes = runDebugParams.fCallTypes;
	m_fHexMode = !runDebugParams.decimalRadix;
	m_fOLERPC = runDebugParams.fOleRpc;
	m_fAnnotate = runDebugParams.fDAMAnnotate;
	m_fCodeBytes = runDebugParams.fDAMCodeBytes;
	m_fSymbols = runDebugParams.fDAMSymbols;
	m_fEnableJit = GetJitState();
    m_fReturnValues = runDebugParams.fReturnValues;
	m_fUnicodeStrings = runDebugParams.fUnicodeStrings;
	m_fENCDebugging = runDebugParams.fENCDebugging;
	m_fLoadExports = runDebugParams.fLoadExports;

	if (!m_fEnableJit) m_fOLERPC = FALSE;

	if ( DebuggeeAlive() )
    {
		if ( pViewMemory )
        {
			PMWI	pmwi = &((CMemoryView *)pViewMemory)->m_mwi;

			// reset the memory window format from the mem win if it's open
			m_iFormat = pmwi->pmwd - rgmwd;
			m_szExpr = pmwi->szExpr;
		}
		else if ( runDebugParams.strExpr.IsEmpty() )
        {
    	    ADDR    addr = { 0 };

        	if ( !OSDGetAddr ( hpidCurr, htidCurr, adrData, &addr ) ) {
	    		if ( ADDR_IS_FLAT ( addr ) ) {
					ASSERT( ADDR_IS_OFF32( addr ) );
					sprintf ( szTmp, "0x%08lX", GetAddrOff ( addr ) );
				}
				else {
		    		sprintf (
    	                szTmp,
        	            "0x%04X:0x%0*lX",
            	        GetAddrSeg ( addr ),
						ADDR_IS_OFF32( addr ) ? 8 : 4,
    					GetAddrOff ( addr )
	                );
				}
			}
			else {
				_ftcscpy ( szTmp, szDefaultExpr );
    		}
			m_szExpr = szTmp;
		}
		else
        {
			m_szExpr = runDebugParams.strExpr;
		}
    }
	else if ( !runDebugParams.strExpr.IsEmpty() ) {
		m_szExpr = runDebugParams.strExpr;
	}
	else {
		m_szExpr = szNull;
	}

}

/////////////////////////////////////////////////////////////////////////////
// CDebugOpt message handlers

BOOL CDebugOpt::OnInitDialog()
{
	int		isz;
	int		iszMax;
	char	szTmp[ axMax ];

	CButton *pbut = (CButton *)GetDlgItem(IDC_DEBUG_OLERPC);
	pbut->EnableWindow(m_fEnableJit);

	// The build system might not be present!
	if ( gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK )
	{
		int iPlatforms;

		gpIBldPlatforms->GetSupportedPlatformCount(&iPlatforms);

		if ( iPlatforms >= 2 ) // check for project + addons
		{
			uniq_platform upID;

			gpIBldPlatforms->GetCurrentPlatform(ACTIVE_BUILDER, &upID);
		}
	}

	((CComboBox *)GetDlgItem(IDC_DEBUG_FORMAT))->SetExtendedUI(TRUE);
#if _INTEGRAL_MAX_BITS >= 64
		iszMax = DLG_MemOpt_Last_Ext;
#else
		iszMax = DLG_MemOpt_Last;
#endif

	for ( isz = DLG_MemOpt_First; isz <= iszMax; isz++ ) {
		::LoadString(hInst, isz, szTmp, sizeof(szTmp));
		((CComboBox *)GetDlgItem(IDC_DEBUG_FORMAT))->InsertString(-1, (LPSTR)szTmp);	// use InsertString to add in order
	}

	((CComboBox *)GetDlgItem(IDC_DEBUG_FORMAT))->SetCurSel(m_iFormat);

	if ( (int)runDebugParams.iMWItemsPerLine > 0 ) {
		m_iItemsPerLine = runDebugParams.iMWItemsPerLine;
		m_fFixedWidth = TRUE;
		((CEdit *)GetDlgItem(IDC_DEBUG_WIDTH))->EnableWindow(TRUE);
	}
	else {
		m_iItemsPerLine = 0;
		m_fFixedWidth = FALSE;
		((CEdit *)GetDlgItem(IDC_DEBUG_WIDTH))->EnableWindow(FALSE);
	}


	// If the debuggee is running don't allow changing this setting
	if ( DebuggeeRunning() ) {
		((CEdit *)GetDlgItem(IDC_DEBUG_OLERPC))->EnableWindow(FALSE);
	}

	// Java: if the current configuration is Java, disable the memory and registers options
	// and the 'display return values' option and 'just in time debugging'
	if (pDebugCurr && pDebugCurr->IsJava()) {

		// set of controls to disable
		int rgidControls[] = { IDC_DEBUG_UNICODE_STRINGS, IDC_DEBUG_SHOWFPU,
								IDC_MEMWND_GRP, IDC_DEBUG_ADDRESS_TXT,
								IDC_DEBUG_ADDRESS, IDC_DEBUG_FORMAT_TXT,
								IDC_DEBUG_FORMAT, IDC_DEBUG_LIVE,
								IDC_DEBUG_RAW, IDC_DEBUG_FIXEDWIDTH,
								IDC_DEBUG_WIDTH,
								IDC_VARWND_GRP, IDC_DEBUG_RETURN_VALUES,
								IDC_DEBUG_JIT };
		const int cControls = sizeof(rgidControls) / sizeof(int);

		// disable controls
		for (int i = 0; i < cControls; i++) {
			CWnd* pControl = (CWnd*)GetDlgItem(rgidControls[i]);
			ASSERT(pControl != NULL);
			if (pControl != NULL) {
				pControl->EnableWindow(FALSE);
			}
		}
	}

	UpdateData( FALSE );

 	return TRUE;
}


// TAB changed or OK pressed, update and return success
BOOL CDebugOpt::ValidateTab()
{
	if (!UpdateData(TRUE))
		 return FALSE;

	if (DebuggeeAlive())
	{
		MWI		mwi = {0};
		mwi.szExpr = (LPSTR)(LPCSTR)m_szExpr;

		if ( !MWSetAddrFromExpr(&mwi, MW_NO_AUTO_TYPE) )
		{
		    // MWSetAddributes failed, assume that we weren't
		    // out of memory and the expression was garbage
		    ::SetFocus( GetDlgItem(IDC_DEBUG_ADDRESS)->m_hWnd );
		    SendDlgItemMessage(IDC_DEBUG_ADDRESS, EM_SETSEL, 0, (LPARAM)-1);
		    MessageBeep(0);
			return FALSE;
		}
	}
	return TRUE;
}

void CDebugOpt::CommitTab()
{
	if (GetSafeHwnd() == NULL)
		return;

	// Any errors in this function should have been caught in ValidateTab,
	// and simply assert.

	// dolphin 4640 [patbr]
	// if UpdateData() fails we should bail out here, not continue...
	if (!UpdateData( TRUE ))
	{
		ASSERT(FALSE);
		return;
	}

	if ( DebuggeeAlive() )
	{
		MWI		mwi = {0};
		mwi.szExpr = (LPSTR)(LPCSTR)m_szExpr;

        if ( !MWSetAddrFromExpr(&mwi, runDebugParams.iMWAutoTypes) )
        {
			// error handling code moved to ValidateTab, above
			ASSERT(FALSE);
			return;
        }
	}

	char *	psz;
	SetJitState(m_fEnableJit);

	runDebugParams.iMWFormat = m_iFormat;
	runDebugParams.fMWLive = m_fLive;
	runDebugParams.fMWShowRaw = m_fRaw;
	runDebugParams.fDAMAnnotate = m_fAnnotate;
	runDebugParams.fDAMCodeBytes = m_fCodeBytes;
	runDebugParams.fDAMSymbols = m_fSymbols;
	runDebugParams.fCPUFloat = m_fFPU;
 	runDebugParams.fCallParams = m_fShowArgs;
	runDebugParams.fCallTypes = m_fShowTypes;
	runDebugParams.decimalRadix = !m_fHexMode;
	runDebugParams.iMWItemsPerLine = m_iItemsPerLine;
    runDebugParams.fReturnValues = m_fReturnValues;
	runDebugParams.fUnicodeStrings = m_fUnicodeStrings;
	runDebugParams.fENCDebugging = m_fENCDebugging;
	if (runDebugParams.fLoadExports != m_fLoadExports)
	{
		// update registry immediately
		runDebugParams.fLoadExports = m_fLoadExports;
		HammerLoadExportsRegKey();
	}

	if ( runDebugParams.fOleRpc != m_fOLERPC ) {
		runDebugParams.fOleRpc = m_fOLERPC ;
		// See definition of HammerOleRegKey for an explanation
		// of why we special case the OLE flag.
		HammerOleRegKey();
	}
	runDebugParams.strExpr = m_szExpr;

	// If there is a memory window open, update it
	if ( pViewMemory ) {
		pViewMemory->SendMessage( WM_COMMAND, 0, 0L );
	}

	// Update the display window if it's open.
	if ( pViewDisassy ) {
		pViewDisassy->SendMessage( WM_COMMAND, 0, 0L );
	}

	// Update the display window if it's open.
	if ( pViewCpu ) {
		pViewCpu->SendMessage( WM_COMMAND, 0, 0L );
	}

	// Update the display window if it's open.
	if ( pViewCalls ) {
		pViewCalls->SendMessage( WM_COMMAND, 0, 0L );
	}

	// Update OSDebug's OLE state if OSDebug is resident.
	if (DebuggeeAlive() && !DebuggeeRunning())
	{
		if (pDebugCurr->MtrcOleRpc()) {
			if ( xosdNone == OSDSystemService(hpidCurr, htidCurr, ssvcOleRpc, &runDebugParams.fOleRpc, 1, NULL) )
			{
			}
		}
	}

	// update the radix, it may have changed...
	UpdateRadix(runDebugParams.decimalRadix);

}

void CDebugOpt::OnClickedFixedwidth()
{
	int iCheckState;

	iCheckState = ((CButton *)GetDlgItem(IDC_DEBUG_FIXEDWIDTH))->GetCheck();
	((CEdit *)GetDlgItem(IDC_DEBUG_WIDTH))->EnableWindow(iCheckState);

	// dolphin 4640 [patbr]
	// MFC will validate entry field even if it is disabled, so put value in
	// entry field as we are disabling it so that UpdateData() will succeed.
	//
	// if we are enabling the field because the accompanying check box has
	// been checked, move the focus to the entry field for user convenience.
	if (iCheckState)
		((CEdit *)GetDlgItem(IDC_DEBUG_WIDTH))->SetFocus();
	else
		((CEdit *)GetDlgItem(IDC_DEBUG_WIDTH))->SetWindowText("0");
}

void CDebugOpt::OnClickedJit()
{
	// get the state of the Jit button and enable or disable the
	// ole rpc button as appropriate

	int iCheckState = ((CButton *)GetDlgItem(IDC_DEBUG_JIT))->GetCheck();

	CButton *pbutOleRpc = (CButton *)GetDlgItem(IDC_DEBUG_OLERPC);

	pbutOleRpc->EnableWindow(iCheckState);

	// enable OLE RPC if JIT is enabled...
	pbutOleRpc->SetCheck(iCheckState);
}

#define GetSzValue(x,y) ::GetProfileString(x,y,"",szValue, sizeof(szValue))

static LPCTSTR szAeDebug		= _T("AeDebug");
static LPCTSTR szAuto			= _T("Auto");
static LPCTSTR szDebugger		= _T("Debugger");
static LPCTSTR szOldAuto		= _T("OldAuto");
static LPCTSTR szDrWatson		= _T("drwtsn32 -p %ld -e %ld -g");
static LPCTSTR szJitTemplate	= _T("\"%s\" -p %%ld -e %%ld");

BOOL GetJitState() 
{
	// get the state of JIT debugging by inspecting the registry

	char szValue[256], szTmp[256+32];

	GetModuleFileName(AfxGetApp()->m_hInstance, szValue, sizeof(szValue));
	sprintf(szTmp, szJitTemplate, szValue);

	if (!GetSzValue(szAeDebug, szDebugger))
		return FALSE;

	// [dolphin #13999 6/17/94 mikemo]  Do a case-insensitive compare, in
	// case the exe name is in a different case in the registry from what
	// GetModuleFileName returned to us.  This is also the right thing to
	// do because the IDE's command line options (such as -P and -E in
	// this case) are case-insensitive.
	if (_tcsicmp(szValue, szTmp))
		return FALSE;

	if (!GetSzValue(szAeDebug, szAuto))
		return FALSE;

	return (atoi(szValue) == 0);
} // GetJitState

BOOL CDebugOpt::GetJitState()
{
	return ::GetJitState();
}

VOID SetJitState(BOOL fNewState)
{
	BOOL fOldState = GetJitState();
	if (fNewState == fOldState)
		return;

	char szValue[256], szTmp[256+32];

	if (fNewState)
	{
		// fetch and save the previous auto value
		if (!GetSzValue(szAeDebug, szOldAuto) || !szValue[0])
		{
			if (!GetSzValue(szAeDebug, szAuto))
				szValue[0] = 0;
		}

		CString strSection;
		strSection = "Debug";
		strSection += chKeySep;
		strSection += "Just in Time";
		WriteRegString(strSection, szAuto, szValue);

		if (!GetSzValue(szAeDebug, szDebugger) || !szValue[0])
			_tcscpy(szValue, szDrWatson);

		WriteRegString(strSection, szDebugger, szValue);

		// now that we've saved the old values, make the IDE the new JIT debugger

		GetModuleFileName(AfxGetApp()->m_hInstance, szValue, sizeof(szValue));
		sprintf(szTmp, szJitTemplate, szValue);

		WriteProfileString(szAeDebug, szDebugger, szTmp);
		WriteProfileString(szAeDebug, szAuto,     "0");

		// nuke any temporary OldAuto value
		WriteProfileString(szAeDebug, szOldAuto, NULL);
	}
	else
	{
		// restore the old debugger, or if none available, use Dr. Watson
		CString strSection;
		strSection = "Debug";
		strSection += chKeySep;
		strSection += "Just in Time";
		CString cstr = GetRegString(strSection, szDebugger, szDrWatson);
		WriteProfileString(szAeDebug, szDebugger, cstr);

		cstr = GetRegString(strSection, szAuto, "1");
		WriteProfileString(szAeDebug, szAuto, cstr);

		// nuke any temporary OldAuto value
		WriteProfileString(szAeDebug, szOldAuto, NULL);
	}
} // SetJitState

VOID CDebugOpt::SetJitState(BOOL fNewState)
{
	::SetJitState(fNewState);
}

BEGIN_MESSAGE_MAP(CTLOpt, C3dDialog)
	//{{AFX_MSG_MAP(CTLOpt)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_SETUP, OnClickedSetup)
	ON_CBN_SELCHANGE(IDC_PLATFORM_LIST, OnPlatformChange)
	ON_LBN_SELCHANGE(IDC_TRANSPORT_LIST, OnTransportChange)
    ON_BN_CLICKED(IDC_CONNECT_NOW, OnClickedConnectNow )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTLOpt::OnInitDialog()
{
    BOOL    fVisibleConnectNow = FALSE;

	m_plstPlatform  = ((CComboBox *)GetDlgItem(IDC_PLATFORM_LIST));
	m_plstTransport = ((CListBox *)GetDlgItem(IDC_TRANSPORT_LIST));
	m_pbutSetup     = (CButton *)GetDlgItem(IDC_SETUP);

	m_plstPlatform->SetExtendedUI(TRUE);
	m_plstPlatform->ResetContent();

	gpIBldPlatforms->GetSupportedPlatformCount(&m_cPlatforms);
	m_rgPlatforms = (uniq_platform *)malloc(m_cPlatforms * sizeof(uniq_platform));
	m_rgiTL       = (UINT *)malloc(m_cPlatforms * sizeof(UINT));

	if (!m_rgPlatforms)
		return FALSE;

    InitChildFont(this, IDC_CONNECT_NOW_TEXT );

	uniq_platform upCurrent = unknown_platform;

	if ( gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK )
		gpIBldPlatforms->GetCurrentPlatform(ACTIVE_BUILDER, &upCurrent);

	uniq_platform upID = unknown_platform;

	gpIBldPlatforms->InitPlatformEnum();

	int iPlatform = 0;
	BOOL bBuildable = FALSE;
	while ( SUCCEEDED(gpIBldPlatforms->NextPlatform(&upID)) )
	{
		if (!((gpIBldPlatforms->IsPlatformSupported(upID) == S_OK) &&
			 (gpIBldPlatforms->GetPlatformBuildable(upID, &bBuildable) == NOERROR) &&
			 bBuildable))
		{
			m_cPlatforms--;
			continue;	// ignore unsupported platforms
		}

		LPCSTR pszDesc;

		gpIBldPlatforms->GetPlatformUIDescription(upID, &pszDesc);

		m_plstPlatform->AddString((LPSTR)pszDesc);

		if (upID == upCurrent)
			m_plstPlatform->SetCurSel(iPlatform);

        fVisibleConnectNow |= (upID == macppc);

		m_rgPlatforms[iPlatform] = upID;

		gpIBldPlatforms->GetPlatformCurrentTLIndex(upID, &(m_rgiTL[iPlatform]));

		iPlatform++;
	}

	if ( (upCurrent == unknown_platform) || (gpIBldPlatforms->IsPlatformSupported(upCurrent) == S_FALSE) )
		m_plstPlatform->SetCurSel(0);

	ASSERT(iPlatform == m_cPlatforms);

	if (m_cPlatforms < 2)
		m_plstPlatform->EnableWindow(FALSE);

    ((CWnd *)GetDlgItem(IDC_CONNECT_NOW))->ShowWindow(fVisibleConnectNow);
    ((CWnd *)GetDlgItem(IDC_SETTINGS_TEXT))->ShowWindow(!fVisibleConnectNow);
    ((CWnd *)GetDlgItem(IDC_CONNECT_NOW_TEXT))->ShowWindow(fVisibleConnectNow);

	OnPlatformChange();

	return TRUE;
}

CTLOpt::CTLOpt() : C3dDialog(CTLOpt::IDD)
{
	m_rgPlatforms = NULL;
	m_rgiTL       = NULL;
	m_cPlatforms  = 0;
    m_fConnectNow = FALSE;
}

CTLOpt::~CTLOpt()
{
	if (m_rgPlatforms)	free(m_rgPlatforms);
	if (m_rgiTL) 		free(m_rgiTL);
}

void CTLOpt::OnPlatformChange()
{
	int iPlatform = m_plstPlatform->GetCurSel();
	uniq_platform upPlatform = m_rgPlatforms[iPlatform];
    BOOL    fEnableConnectNow;

	m_plstTransport->ResetContent();

	int cTL;

	gpIBldPlatforms->GetPlatformTLCount(upPlatform, &cTL);

	for (int iTL = 0; iTL < cTL; iTL ++)
	{
		const TCHAR *szDesc;

		gpIBldPlatforms->GetPlatformTLDescription(upPlatform, (UINT)iTL, &szDesc);
		m_plstTransport->AddString(szDesc);
	}

	m_plstTransport->SetCurSel(m_rgiTL[iPlatform]);

#pragma message ( "CTLOpt::OnPlatformChange: Need to query EM metrics for remote JIT" )
    fEnableConnectNow = (upPlatform == macppc);
    ((CWnd *)GetDlgItem(IDC_CONNECT_NOW))->EnableWindow(fEnableConnectNow);

	OnTransportChange();
}

void CTLOpt::OnTransportChange()
{
	int iPlatform = m_plstPlatform->GetCurSel();
	uniq_platform upPlatform = m_rgPlatforms[iPlatform];
	int iTL = m_plstTransport->GetCurSel();
	m_rgiTL[iPlatform] = iTL;

	UINT tfl;

	gpIBldPlatforms->GetPlatformTLFlags(upPlatform, iTL, &tfl);

	m_pbutSetup->EnableWindow(!!(tfl & TFL_CFGABLE));
}

void CTLOpt::OnClickedSetup()
{
	int iPlatform = m_plstPlatform->GetCurSel();
	uniq_platform upPlatform = m_rgPlatforms[iPlatform];
	int iTL = m_plstTransport->GetCurSel();

	const TCHAR *szTLName;

	gpIBldPlatforms->GetPlatformTLName(upPlatform, iTL, &szTLName);

	if (!FSetupDLL(szTLName, "TLSetup", MptFromPlatform (upPlatform)))
	{
		const TCHAR *szUIDesc;
		const TCHAR *szTLDesc;

		gpIBldPlatforms->GetPlatformUIDescription(upPlatform, &szUIDesc);
		gpIBldPlatforms->GetPlatformTLDescription(upPlatform, iTL, &szTLDesc);

		ErrorBox(DBG_TL_Config_Err, (LPCSTR)szUIDesc, (LPCSTR)szTLDesc);
	}
}

void CTLOpt::OnClickedConnectNow()
{
    m_fConnectNow = TRUE;
    OnOK();
}

void CTLOpt::OnOK()
{
	m_iPlatform = m_plstPlatform->GetCurSel();
    C3dDialog::OnOK();
}

void CTLOpt::OnHelp()
{
	OnCommandHelp(0, 0);
}


VOID PASCAL InvokeHelp (DWORD dwHelpId)
{
	theApp.HelpOnApplication(dwHelpId);
}


MPT
MptFromPlatform(
	uniq_platform	Platform
	)
{
	switch (Platform)
	{
		case win32x86: return mptix86;
		case macppc: return mptmppc;
		case win32mips: return mptmips;
		case win32alpha: return mptdaxp;
		case win32ppc: return mptntppc;
	}

	return mptUnknown;
}



Protect <HKEY>::~Protect(
	)
/*++

Routine Description:

	Destructor for the HKEY object.

--*/
{
	if (m_h)
	{
		RegCloseKey (m_h);
		m_h = NULL;
	}
}

	

LONG
OSDAPI
RegistryGetSet(
	LPTSTR		KeyName,
	LPTSTR		ValueName,
	DWORD*		pdwType,
	BYTE*		Data,
	DWORD		cbData,
	BOOL		fSet
	)
/*++

Routine Description:

	This function Gets/Sets the registry value ValueName with data Data under the
	debugger's root key.

	KeyName -- relative path for a key name; for example:
			   "Build System\Components\Platforms"
				
	ValueName -- the name of a value to set; for example, "TLN0T"

	Data -- data to set to be set to

	pdwType -- the registry type

	cbData -- size of data to be read/written

	fSet -- TRUE means we are setting (save to registry), FALSE means getting
		  	getting from registry

Return Value:

	size of the data written/read.

--*/
{
	TCHAR			FullKeyName [_MAX_PATH];
	DWORD			dwLength;
	HKEY			hKey = NULL;
	Protect <HKEY>	protect (hKey);
	LONG			Err;
	

	dwLength = sizeof (FullKeyName);
	VERIFY (GetRegistryRoot (FullKeyName, &dwLength));

	if (KeyName && *KeyName)
	{
		//
		// 	this must be a relative registry path
		//
			
		ASSERT (*KeyName != '\\');
			
		if (FullKeyName [dwLength - 1] != '\\')
		{
			FullKeyName [dwLength++] = '\\';
			FullKeyName [dwLength] = '\000';
		}

		strcpy (&FullKeyName [dwLength], KeyName);
	}

	Err = RegCreateKeyEx (HKEY_CURRENT_USER,
						  FullKeyName,
						  0,
						  NULL,		// NULL ok for class ??
						  REG_OPTION_NON_VOLATILE,
						  KEY_ALL_ACCESS,
						  NULL,
						  &hKey,
						  NULL				// NULL ok?
						  );

	if (Err != ERROR_SUCCESS)
		return 0;

	
	if (fSet)
	{
		//
		//	attempting to set value
		//
		
		Err = RegSetValueEx (hKey,
	 						 ValueName,
		 					 0,
							 *pdwType,
							 Data,
 							 cbData
							 );

		if (Err != ERROR_SUCCESS)
			return 0;

		return cbData;
	}
	else
	{
		//
		//	attempting to get value
		//

		DWORD	lcbData = cbData;
		
		Err = RegQueryValueEx (hKey,
							   ValueName,
							   0,
							   pdwType,
							   Data,
							   &lcbData
							   );

		if (Err != ERROR_SUCCESS)
			return 0;

		return lcbData;
	}


	return 0;  // NOTREACHED
}
		
	

		
		
							  
			

BOOL FSetupDLL(LPCSTR szFileName, LPCSTR szEntry, MPT mpt)
{
	BOOL	fRet = FALSE;

	if ( !DebuggeeAlive() )
	{
		char 			szPath[ _MAX_PATH ];
		char 			szDrive[ _MAX_DRIVE ];
		char 			szDir[ _MAX_DIR ];
		char 			szFName[ _MAX_FNAME ];
		char 			szExt[ _MAX_EXT ];
		int 			iOldErrorMode = SetErrorMode ( SEM_NOOPENFILEERRORBOX );
		struct _stat	statbuf;
		HANDLE			h;

		GetModuleFileName(theApp.m_hInstance, szPath, sizeof(szPath));
		_splitpath(szPath, szDrive, szDir, szFName, szExt);
		_makepath(szPath, szDrive, szDir, NULL, NULL);

		// Make sure we are loading a debug or release debugger .dll
		TCHAR rgchDllFile[ _MAX_FNAME ];
		_tcscpy(rgchDllFile, szFileName);
		#ifdef _DEBUG
			_tcscat(rgchDllFile, "D.DLL");
		#else
			_tcscat(rgchDllFile, ".DLL");
		#endif

		_ftcscat(szPath, rgchDllFile);

		// First search for the file in our directory
		if( _stat( szPath, &statbuf ) == 0 && (statbuf.st_mode & _S_IFREG) )
		{
			// Load the dll
			h = LoadLibrary( szPath );
		}

		// Can't find it, let Windows look for it
		else
		{
			h = LoadLibrary( szFileName );
		}

		// Restore the error mode
		SetErrorMode( iOldErrorMode );

		// Handle was valid, continue with setup/config
		// Win32 h non-zero == VALID, Win16 h >= 32
		if ( h )
		{
			typedef void (__stdcall* PFNSETUP) (TLSS* tlss);

			PFNSETUP pfnsetup = (PFNSETUP) GetProcAddress ((HINSTANCE)h, szEntry);

			// If the function/export exists do it
			if ( pfnsetup )
			{

				TLSS	tlss;

				tlss.fInteractive = TRUE;
				tlss.fSave = FALSE;
				tlss.lpvPrivate = (LPVOID) _SushiGetSafeOwner (NULL);
				tlss.lParam = (LPARAM) h;
				tlss.mpt = mpt;
				tlss.fRMAttached = FALSE;
				tlss.lpfnGetSet = RegistryGetSet;

				ASSERT (mpt != mptUnknown);
				pfnsetup (&tlss);
				
				fRet = TRUE;
			}
			else
			{
				ASSERT (FALSE);
			}
			

			// Release the library
			FreeLibrary( (HINSTANCE)h );
		}
	}
	return fRet;
}

