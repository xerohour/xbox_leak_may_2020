// cafedlg.cpp : implementation file
//

#include "stdafx.h"
#include "cafebld.h"
#include "cafedlg.h"
#include "settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterWindow();
	
	// TODO: Add extra about dlg initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildDlg dialog

char* CCAFEBuildDlg::m_szTargets[][2] = {	{ "All targets",					"all" },
											{ "------------------------------------------", "" },
											{ "CAFE system",					"cafe" },
											{ "Sniff tests",					"sniffs" },
											{ "------------------------------------------", "" },
											{ "CAFE driver",					"cafedrv" },
											{ "Target portion of support",		"target" },
											{ "GUI portion of support",			"gui" },
											{ "Shell portion of support",		"shl" },
											{ "Editors portion of support",		"eds" },
											{ "Project portion of support",		"prj" },
											{ "Debugger portion of support",	"dbg" },
											{ "IDE portion of support",			"ide" },
											{ "Toolset portion of support",		"toolset" },
											{ "Build tools portion of support",	"bldtools" },
											{ "SubSuite library",				"subsuite" },
											{ "Log system",						"log" },
											{ "------------------------------------------", "" },
											{ "Browser sniff test",				"browser" },
											{ "Debugger sniff test",			"debugger" },
											{ "Debugger MBC sniff test",		"dbg_mbc" },
											{ "Edit 'n Go sniff test",			"editgo" },
											{ "Editor sniff test",				"editor" },
											{ "Fortran sniff test",				"fortran" },
											{ "SYS sniff test",					"sys" },
											{ "VProj sniff test",				"vproj" },
											{ "VRes sniff test",				"vres" },
											{ "VShell sniff test",				"vshell" },
											{ "Wizards sniff test",				"wizards" },
											{ NULL, NULL } };

CCAFEBuildDlg::CCAFEBuildDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCAFEBuildDlg::IDD, pParent),
	  m_nBuilding(0),
	  m_bCancelled(FALSE),
	  m_heventTerminate(NULL),
	  m_pthreadReadPipe(NULL),
	  m_strMoreCmdLine("VERB=2")
{
	//{{AFX_DATA_INIT(CCAFEBuildDlg)
	m_bBrowse = FALSE;
	m_bClean = FALSE;
	m_iDebug = FALSE;
	m_bDepend = TRUE;
	m_bSLMIn = FALSE;
	m_bSLMOut = FALSE;
	m_bSsync = FALSE;
	m_bBuild = TRUE;
	//}}AFX_DATA_INIT

	CCAFEBuildApp *pApp = ((CCAFEBuildApp*)AfxGetApp());

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = pApp->LoadIcon(IDR_MAINFRAME);

	// get the registry key
	m_strRegistryKey = pApp->GetRegistryKeyName();

	// initialize settings
	::GetEnvironmentVariable("CAFEDIR", m_strCAFEDIR.GetBuffer(1024), 1024);
	m_strCAFEDIR.ReleaseBuffer();
	::GetEnvironmentVariable("SNIFFDIR", m_strSNIFFDIR.GetBuffer(1024), 1024);
	m_strSNIFFDIR.ReleaseBuffer();
	::GetEnvironmentVariable("IDESRCDIR", m_strIDESRCDIR.GetBuffer(1024), 1024);
	m_strIDESRCDIR.ReleaseBuffer();
	::GetEnvironmentVariable("PATH", m_strPATH.GetBuffer(1024), 1024);
	m_strPATH.ReleaseBuffer();
	::GetEnvironmentVariable("INCLUDE", m_strINCLUDE.GetBuffer(1024), 1024);
	m_strINCLUDE.ReleaseBuffer();
	::GetEnvironmentVariable("LIB", m_strLIB.GetBuffer(1024), 1024);
	m_strLIB.ReleaseBuffer();

	m_strMakefile = "cafebld.mak";

	// read settings from the registry
	ReadRegistry();
}

CCAFEBuildDlg::~CCAFEBuildDlg()
{
	// write settings to the registry
	WriteRegistry();
}

void CCAFEBuildDlg::ReadRegistry(void)
{
	HKEY hKey;
	DWORD dwType;
	DWORD dwDataSize;

	// open the registry key, if it exists
	if (::RegOpenKeyEx(HKEY_CURRENT_USER, BASEREGISTRYKEY + m_strRegistryKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
		// read browse value
		dwDataSize = sizeof(m_bBrowse);
		::RegQueryValueEx(hKey, "Browse", NULL, &dwType, (unsigned char*)&m_bBrowse, &dwDataSize);
		// read Build value
		dwDataSize = sizeof(m_bBuild);
		::RegQueryValueEx(hKey, "Build", NULL, &dwType, (unsigned char*)&m_bBuild, &dwDataSize);
		// read clean value
		dwDataSize = sizeof(m_bClean);
		::RegQueryValueEx(hKey, "Clean", NULL, &dwType, (unsigned char*)&m_bClean, &dwDataSize);
		// read debug value
		dwDataSize = sizeof(m_iDebug);
		::RegQueryValueEx(hKey, "Debug", NULL, &dwType, (unsigned char*)&m_iDebug, &dwDataSize);
		// read depend value
		dwDataSize = sizeof(m_bDepend);
		::RegQueryValueEx(hKey, "Depend", NULL, &dwType, (unsigned char*)&m_bDepend, &dwDataSize);
		// read SLMIn value
		dwDataSize = sizeof(m_bSLMIn);
		::RegQueryValueEx(hKey, "SLMIn", NULL, &dwType, (unsigned char*)&m_bSLMIn, &dwDataSize);
		// read SLMOut value
		dwDataSize = sizeof(m_bSLMOut);
		::RegQueryValueEx(hKey, "SLMOut", NULL, &dwType, (unsigned char*)&m_bSLMOut, &dwDataSize);
		// read Ssync value
		dwDataSize = sizeof(m_bSsync);
		::RegQueryValueEx(hKey, "Ssync", NULL, &dwType, (unsigned char*)&m_bSsync, &dwDataSize);

		char acBuf[1024];
		// read CAFEDIR value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "CAFEDIR", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strCAFEDIR = CString(acBuf);
		}
		// read SNIFFDIR value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "SNIFFDIR", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strSNIFFDIR = CString(acBuf);
		}
		// read IDESRCDIR value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "IDESRCDIR", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strIDESRCDIR = CString(acBuf);
		}
		// read PATH value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "PATH", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strPATH = CString(acBuf);
		}
		// read INCLUDE value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "INCLUDE", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strINCLUDE = CString(acBuf);
		}
		// read LIB value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "LIB", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strLIB = CString(acBuf);
		}
		// read makefile value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "Makefile", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strMakefile = CString(acBuf);
		}
		// read additional command line value
		dwDataSize = 1024;
		if (::RegQueryValueEx(hKey, "MoreCmdLine", NULL, &dwType, (unsigned char*)acBuf, &dwDataSize) == ERROR_SUCCESS) {
			acBuf[dwDataSize] = '\0';
			m_strMoreCmdLine = CString(acBuf);
		}

		// close the key
		::RegCloseKey(hKey);
	}
}

void CCAFEBuildDlg::WriteRegistry(void)
{
	HKEY hKey;
	DWORD dwDisposition;

	// create the registry key
	if (::RegCreateKeyEx(HKEY_CURRENT_USER, BASEREGISTRYKEY + m_strRegistryKey, 0, "CAFEBuild", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
		// write browse value
		::RegSetValueEx(hKey, "Browse", 0, REG_BINARY, (unsigned char*)&m_bBrowse, sizeof(m_bBrowse));
		::RegSetValueEx(hKey, "Build", 0, REG_BINARY, (unsigned char*)&m_bBuild, sizeof(m_bBuild));
		::RegSetValueEx(hKey, "Clean", 0, REG_BINARY, (unsigned char*)&m_bClean, sizeof(m_bClean));
		::RegSetValueEx(hKey, "Debug", 0, REG_BINARY, (unsigned char*)&m_iDebug, sizeof(m_iDebug));
		::RegSetValueEx(hKey, "Depend", 0, REG_BINARY, (unsigned char*)&m_bDepend, sizeof(m_bDepend));
		::RegSetValueEx(hKey, "SLMIn", 0, REG_BINARY, (unsigned char*)&m_bSLMIn, sizeof(m_bSLMIn));
		::RegSetValueEx(hKey, "SLMOut", 0, REG_BINARY, (unsigned char*)&m_bSLMOut, sizeof(m_bSLMOut));
		::RegSetValueEx(hKey, "Ssync", 0, REG_BINARY, (unsigned char*)&m_bSsync, sizeof(m_bSsync));

		::RegSetValueEx(hKey, "CAFEDIR", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strCAFEDIR, m_strCAFEDIR.GetLength());
		::RegSetValueEx(hKey, "SNIFFDIR", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strSNIFFDIR, m_strSNIFFDIR.GetLength());
		::RegSetValueEx(hKey, "IDESRCDIR", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strIDESRCDIR, m_strIDESRCDIR.GetLength());
		::RegSetValueEx(hKey, "PATH", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strPATH, m_strPATH.GetLength());
		::RegSetValueEx(hKey, "INCLUDE", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strINCLUDE, m_strINCLUDE.GetLength());
		::RegSetValueEx(hKey, "LIB", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strLIB, m_strLIB.GetLength());
		::RegSetValueEx(hKey, "Makefile", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strMakefile, m_strMakefile.GetLength());
		::RegSetValueEx(hKey, "MoreCmdLine", 0, REG_SZ, (const unsigned char*)(LPCTSTR)m_strMoreCmdLine, m_strMoreCmdLine.GetLength());

		// close the key
		::RegCloseKey(hKey);
	}
}

void CCAFEBuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCAFEBuildDlg)
	DDX_Control(pDX, IDE_Output, m_editOutput);
	DDX_Control(pDX, IDL_Targets, m_listTargets);
	DDX_Check(pDX, IDC_Browse, m_bBrowse);
	DDX_Check(pDX, IDC_Clean, m_bClean);
	DDX_Check(pDX, IDC_Debug, m_iDebug);
	DDX_Check(pDX, IDC_Depend, m_bDepend);
	DDX_Check(pDX, IDC_SLMIn, m_bSLMIn);
	DDX_Check(pDX, IDC_SLMOut, m_bSLMOut);
	DDX_Check(pDX, IDC_Ssync, m_bSsync);
	DDX_Check(pDX, IDC_Build, m_bBuild);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCAFEBuildDlg, CDialog)
	//{{AFX_MSG_MAP(CCAFEBuildDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnBuild)
	ON_BN_CLICKED(IDB_CancelBuild, OnCancelBuild)
	ON_BN_CLICKED(IDCANCEL, OnExit)
	ON_BN_CLICKED(IDB_Settings, OnSettings)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATEOUTPUT, OnUpdateOutput)
	ON_MESSAGE(WM_BUILDCOMPLETE, OnBuildComplete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildDlg message handlers

BOOL CCAFEBuildDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterWindow();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}
	
	// fill in the target list box
	for (int i = 0; m_szTargets[i][0] != NULL; i++) {
		m_listTargets.AddString(m_szTargets[i][0]);
	}

	UpdateData(FALSE);

	// enable/disable buttons
	UpdateButtons();
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCAFEBuildDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCAFEBuildDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCAFEBuildDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCAFEBuildDlg::OnBuild() 
{
	LPCSTR pszCmdLine = "vcspawn -e %lu nmake -f %s BROWSE=%d BUILD=%d CLEAN=%d DEBUG=%d DEPEND=%d SLMIN=%d SLMOUT=%d SSYNC=%d %s ";
	char acCmdLine[1024];
	const cnBuf = 50;
	int anSelTargets[cnBuf];
	int nSel;
	DWORD dwWritten;
	LPCSTR pszBanner1 = "============================================\r\n";
	LPCSTR pszBanner2 = "==== CAFE Build System -- %s -- %s ====\r\n";
	BOOL bProcess;
	SECURITY_ATTRIBUTES	sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	char acBuf[1024];

	// update member vars with data from dialog
	UpdateData(TRUE);

	// getting ready to build
	m_nBuilding = 2;
	UpdateButtons();

	// at least one target must be selected
	nSel = m_listTargets.GetSelCount();
	if (nSel > 0) {

		// clear the edit control
		m_editOutput.SetReadOnly(FALSE);
		m_editOutput.SetSel(0, -1, TRUE);
		m_editOutput.Clear();
		m_editOutput.SetReadOnly(TRUE);

		// create an event to use for terminating the build
		m_heventTerminate = ::CreateEvent(&sa, TRUE, FALSE, NULL);
		if (m_heventTerminate) {

			// build command line
			sprintf(acCmdLine, pszCmdLine, (DWORD)m_heventTerminate, (LPCSTR)m_strMakefile, m_bBrowse, m_bBuild, m_bClean, m_iDebug, m_bDepend, m_bSLMIn, m_bSLMOut, m_bSsync, (LPCSTR)m_strMoreCmdLine);

			// get the selected items from the list box
			m_listTargets.GetSelItems(cnBuf, anSelTargets);

			// add the targets to the command line
			for (int i = 0; i < nSel; i++) {
				strcat(acCmdLine, m_szTargets[anSelTargets[i]][1]);
				strcat(acCmdLine, " ");
			}

			// create an anonymous pipe
			if (::CreatePipe(&m_hpipeRead, &m_hpipeWrite, &sa, 0)) {

				// we need to mark the read end of the pipe as noninheritable,
				// so the child process doesn't inherit a "garbage" handle
				// that is unclosable.
				::DuplicateHandle(GetCurrentProcess(), m_hpipeRead, GetCurrentProcess(), NULL, 0, FALSE, DUPLICATE_SAME_ACCESS);

				// duplicate the write handle so it can also be used as a write
				// handle for stderr. If we use the same handle, and the child
				// process closes either one, both close and the anonymous pipe
				// closes.
				::DuplicateHandle(GetCurrentProcess(), m_hpipeWrite, GetCurrentProcess(), &m_hpipeWriteErr, 0, TRUE, DUPLICATE_SAME_ACCESS);

				// start a thread to read from the pipe
				m_pthreadReadPipe = AfxBeginThread(WrapReadPipe, this);

				// write out starting banner
				char acTime[10];
				char acDate[10];
				char acBannerSig[100];
				_strtime(acTime);
				_strdate(acDate);
				sprintf(acBannerSig, pszBanner2, acTime, acDate);
				::WriteFile(m_hpipeWrite, pszBanner1, strlen(pszBanner1), &dwWritten, NULL);
				::WriteFile(m_hpipeWrite, acBannerSig, strlen(acBannerSig), &dwWritten, NULL);
				::WriteFile(m_hpipeWrite, pszBanner1, strlen(pszBanner1), &dwWritten, NULL);

				// set additional environment var settings for build
				::SetEnvironmentVariable("CAFEDIR", m_strCAFEDIR);
				::SetEnvironmentVariable("SNIFFDIR", m_strSNIFFDIR);
				::SetEnvironmentVariable("IDESRCDIR", m_strIDESRCDIR);
				::SetEnvironmentVariable("PATH", m_strPATH);
				::SetEnvironmentVariable("INCLUDE", m_strINCLUDE);
				::SetEnvironmentVariable("LIB", m_strLIB);

				// fill in startup info structure for CreateProcess
				STARTUPINFO si = {	sizeof(STARTUPINFO),		// cb
									NULL,						// lpReserved
									NULL,						// lpDesktop
									NULL,						// lpTitle
									0, 0,						// dwX, dwY
									0, 0,						// dwXSize, dwYSize
									0, 0,						// dwXCountChars, dwYCountChars
									0,							// dwFillAttribute
									STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES,		// dwFlags
									SW_HIDE,					// wShowWindow
									0,							// cbReserved2
									NULL,						// lpReserved2
									NULL,						// hStdInput
									m_hpipeWrite,				// hStdOutput
									m_hpipeWriteErr };			// hStdError

				// spawn the build command line
				bProcess = ::CreateProcess(	NULL,				// lpszImageName
											acCmdLine,			// lpszCommandLine
											NULL,				// lpsaProcess
											NULL,				// lpsaThread
											TRUE,				// fInheritHandles
											NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP, // fdwCreate
											NULL,				// lpvEnvironment
											NULL,				// lpszCurDir
											&si,				// lpsiStartInfo
											&m_procinfo );		// lppiProcInfo

				// was CreateProcess successful?
				if (bProcess) {
					sprintf(acBuf, "Spawning build engine: %s...\r\n", acCmdLine);
					::WriteFile(m_hpipeWrite, acBuf, strlen(acBuf), &dwWritten, NULL);

					m_nBuilding = 1;
				}
				else {
					// CreateProcess not successful
					char acBuf[80];
					sprintf(acBuf, "Error (%d) spawning build engine: %s.\r\n", ::GetLastError(), acCmdLine);
					::WriteFile(m_hpipeWrite, acBuf, strlen(acBuf), &dwWritten, NULL);

					m_nBuilding = 0;
				}

				// close our handles to the write end of the pipe, 
				// so that when the child process does, the pipe will close
				::CloseHandle(m_hpipeWrite);
				::CloseHandle(m_hpipeWriteErr);
			}
			else {
				// creating anonymous pipe was not successful
				AfxMessageBox("Creating anonymous pipe was not successful. Unable to build.");
				m_nBuilding = 0;
			}
		}
		else {
			// unable to create event to signal vcspawn
			AfxMessageBox("Unable to create termination event. Unable to build.");
			m_nBuilding = 0;
		}
	}
	else {
		// no targets were selected
		AfxMessageBox("At least one target must be selected.");
		m_nBuilding = 0;
	}

	UpdateButtons();
}

UINT CCAFEBuildDlg::WrapReadPipe(LPVOID pParam)
{
	return ((CCAFEBuildDlg*)pParam)->ReadPipe();
}

UINT CCAFEBuildDlg::ReadPipe(void)
{
	const DWORD ccBuf = 10;
	char acBuf[ccBuf+1];
	DWORD ccRead;

	// read from the pipe until it's broken
	for (BOOL bDone = FALSE; !bDone; ) {
		if (::ReadFile(m_hpipeRead, acBuf, ccBuf, &ccRead, NULL)) {
			// put end of string marker after last char read
			acBuf[ccRead] = '\0';
			// display the data
			m_strOutput = CString(acBuf);
			SendMessage(WM_UPDATEOUTPUT);
		}
		else {
			switch (::GetLastError()) {
				case ERROR_BROKEN_PIPE: {
					PostMessage(WM_BUILDCOMPLETE);
					bDone = TRUE;
					break;
				}
			}
		}
	}
	return 0;
}

void CCAFEBuildDlg::UpdateButtons(void)
{
	// gray Build if building; gray Cancel Build if not
	((CButton*)GetDlgItem(IDOK))->EnableWindow((m_nBuilding == 0));
	((CButton*)GetDlgItem(IDB_CancelBuild))->EnableWindow((m_nBuilding == 1));

	// gray check boxes if building
	for (int i = IDC_Build; i <= IDC_Browse; i++) {
		((CWnd*)GetDlgItem(i))->EnableWindow((m_nBuilding == 0));
	}

	// gray target list box if building
	((CWnd*)GetDlgItem(IDL_Targets))->EnableWindow((m_nBuilding == 0));

	// gray settings button if building
	((CWnd*)GetDlgItem(IDB_Settings))->EnableWindow((m_nBuilding == 0));
}

void CCAFEBuildDlg::OnCancelBuild() 
{
	// set the flag that indicates we're cancelling the build
	m_bCancelled = TRUE;

	// is the event valid (did we spawn vcspawn?)
	if (m_heventTerminate) {
		// signal vcspawn that we want to terminate the build
		::SetEvent(m_heventTerminate);

		// wait for vcspawn to terminate
		if (::WaitForSingleObject(m_procinfo.hProcess, 25000) == WAIT_TIMEOUT) {
			m_editOutput.ReplaceSel("VCSpawn never terminated; terminating it.\r\n");
			::TerminateProcess(m_procinfo.hProcess, 0);
		}
	}

	// is the pipe thread valid?
	if (m_pthreadReadPipe) {
		// wait for read pipe thread to exit
		if (::WaitForSingleObject(m_pthreadReadPipe->m_hThread, 1000) == WAIT_TIMEOUT) {
			m_editOutput.ReplaceSel("Build cancelled, but read pipe thread didn't exit; terminating it.\r\n");
			::TerminateThread(m_pthreadReadPipe->m_hThread, 0);

			// since we had to terminate the read pipe thread, OnBuildComplete will never be called,
			// so we'll fix things up here

			// we're no longer building
			m_nBuilding = 0;
			UpdateButtons();
			UpdateData(FALSE);

			// no longer cancelled
			m_bCancelled = FALSE;
		}
		else {
			m_editOutput.ReplaceSel("Build cancelled.\r\n");
		}
		delete m_pthreadReadPipe; //release thread data
	}
}

LONG CCAFEBuildDlg::OnUpdateOutput(UINT wParam, LONG lParam)
{
	// display the text
	m_editOutput.ReplaceSel(m_strOutput);

	m_strOutput.Empty();

	return 0;
}

LONG CCAFEBuildDlg::OnBuildComplete(UINT wParam, LONG lParam)
{
	// close the terminate event
	::CloseHandle(m_heventTerminate);

	// if the build wasn't cancelled, it completed
	if (!m_bCancelled) {
		// wait for the read pipe thread to exit
		if (::WaitForSingleObject(m_pthreadReadPipe->m_hThread, 2000) == WAIT_TIMEOUT) {
			m_editOutput.ReplaceSel("Build complete, but read pipe thread didn't exit; terminating it.\r\n");
			::TerminateThread(m_pthreadReadPipe->m_hThread, 0);
		}
		else {
			m_editOutput.ReplaceSel("Build complete.\r\n");
		}
		delete m_pthreadReadPipe; //release thread data
	}

	// clear build cancelled flag
	m_bCancelled = FALSE;

	// we're no longer building
	m_nBuilding = 0;
	UpdateButtons();
	UpdateData(FALSE);

	return 0;
}

void CCAFEBuildDlg::OnExit() 
{
	// cancel the build
	OnCancelBuild();

	// call the base class
	CDialog::OnCancel();
}

void CCAFEBuildDlg::OnSettings()
{
	// instantiate the settings dialog
	CSettingsDlg dlgSettings;

	// fill in member variables
	dlgSettings.m_strCAFEDIR = m_strCAFEDIR;
	dlgSettings.m_strSNIFFDIR = m_strSNIFFDIR;
	dlgSettings.m_strIDESRCDIR = m_strIDESRCDIR;
	dlgSettings.m_strMakefile = m_strMakefile;
	dlgSettings.m_strPATH = m_strPATH;
	dlgSettings.m_strINCLUDE = m_strINCLUDE;
	dlgSettings.m_strLIB = m_strLIB;
	dlgSettings.m_strParameters = m_strMoreCmdLine;
	dlgSettings.m_strRegistryKey = m_strRegistryKey;

	// display the dialog
	if (dlgSettings.DoModal() == IDOK) {
		// retrieve settings from dialog
		m_strCAFEDIR = dlgSettings.m_strCAFEDIR;
		m_strSNIFFDIR = dlgSettings.m_strSNIFFDIR;
		m_strIDESRCDIR = dlgSettings.m_strIDESRCDIR;
		m_strMakefile = dlgSettings.m_strMakefile;
		m_strPATH = dlgSettings.m_strPATH;
		m_strINCLUDE = dlgSettings.m_strINCLUDE;
		m_strLIB = dlgSettings.m_strLIB;
		m_strMoreCmdLine = dlgSettings.m_strParameters;
	}
}
