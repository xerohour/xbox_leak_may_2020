/////////////////////////////////////////////////////////////////////////////
//
// Application initialization and termination code
//

#include "stdafx.h"
#include <windowsx.h>
#include "locale.h"
#include "imeutil.h"
#include "about.h"
#include "shell.h"
#include "cmdcache.h"
#include "toolexpt.h"
#include "resource.h"
#include "ipchost.h"

#include <prjapi.h>
#include <prjguid.h>
#include <bldapi.h>
#include <bldguid.h>

#define DEBUG_CLSID
#include <utilauto.h>
#undef DEBUG_CLSID

#include <cmguids.h>
#include <shlapi_.h>
#include <shellrec.h>
#include <aut1guid.h>
#include "utilauto.h"
#include "autostat.h"
#include <io.h>
#include <fcntl.h>
#include "atlbase.h"

#define VERSION_CHECK_ONLY
#include "afxdllxx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

static const TCHAR** g_packages;
static int g_nPackages;
static const TCHAR * rgszMakePackages[] = {
#ifdef _DEBUG
	// _T("deveditd"),	// FUTURE: get rid of this one
	_T("devprjd"),
	_T("ided\\devbldd"),
	_T("ided\\devfx86d"),
	_T("ided\\devtoold"),
	_T("ided\\devxboxd") };
#else
	// _T("devedit"),	// FUTURE: get rid of this one
	_T("devprj"),
	_T("ide\\devbld"),
	_T("ide\\devfx86"),
	_T("ide\\devtool"),
	_T("ide\\devxbox") };
#endif

BOOL bEnable3dControls;
BOOL InitOle(); //replaces AfxOleInit to supply our own MessageFilter

#ifdef _MTTF
BOOL IsMTTFEnabled(void);
#endif

static BOOL g_bConsoleCreated = FALSE;
BOOL g_bInvokedHelp = FALSE;
HANDLE g_hStdIn = NULL;

/////////////////////////////////////////////////////////////////////////////
// CTheApp construction

extern const TCHAR g_szKeyRoot[]     = _TEXT("Software");
extern const TCHAR g_szCompanyName[] = _TEXT("Microsoft");
#ifdef _DEBUG
static const TCHAR szShellDLL[]                 = _T("DEVSHLD.DLL");
#else
static const TCHAR szShellDLL[]                 = _T("DEVSHL.DLL");
#endif //_DEBUG
static const TCHAR szGeneralKey[]               = _T("General");
static const TCHAR szPackageCountKey[]  = _T("PackageCount");

CTheApp::CTheApp(const TCHAR** rgszPackages, int nPackages):m_RepeatableCmds(10)
{
#ifdef _EXPORT_LAYOUT
        m_bExportLayout = FALSE;
        m_pszLayoutFilePath = NULL;
#endif
        m_pszRegKeyName = NULL;
        m_pActivePacket = NULL;
        m_pMDIActivePacket = NULL;
        m_bMaximizeApp = FALSE;
        m_bMaximizeDoc = FALSE;
        m_bCompressMak = FALSE;
        m_bStatusbar = TRUE;
        m_bMenuDirty = TRUE;
        m_bProjectCommandsDirty=TRUE;
        m_bMenuDead = FALSE;
        m_wHookDialogs = DHF_CENTER | DHF_3DCONTROLS;
        m_nLockRecentFileList = 0;
        m_bOpenedFromMRU = FALSE;
        m_bOpenedFromDrop = FALSE;
        m_bOpenedFromLastLoad = FALSE;
        m_fVCPPIsActive = TRUE;
        m_fVerbose = TRUE;
        m_cCmdRepeat = 1;
        m_RepeatableCmds.InitHashTable(17);
        m_SettingRepeatCount = DoneRepeating;
        m_pAppCmpHost = NULL;
        m_bHideMicrosoftWebMenu=TRUE;
		m_fRunningMacro = FALSE;

#ifndef NO_VBA
        m_fOleAutomation = FALSE;       // suppress OLE automation until a package turns it on
#endif  // NO_VBA

#ifdef _WIN32
        // set flag if running on Chicago.  Detecting Chicago is a bit weird.  GetVersion
        // returns the "Win32s" bit, but returns version 4 or greater
        DWORD dwVersion = ::GetVersion();
        m_bOnChicago = (dwVersion & 0x80000000) && (LOBYTE(dwVersion) > 3);

        m_bWin4 = LOBYTE(dwVersion) > 3;
        m_bWin95 = m_bWin4 && (dwVersion & 0x80000000) != 0;
		m_bWin5 = LOBYTE(dwVersion) > 4;
#endif

        m_dwMenuSelectTime = 0;
        m_pRecentProjectList = NULL;

        m_bEnableWorkspaceLoad = TRUE;
        m_bPkgChk = TRUE; // enabled by default

        g_packages = rgszPackages;
        g_nPackages = nPackages;
        m_bActivated = FALSE;
        m_pAutoApp = NULL;
        m_bQuitting = FALSE;

        // Macro recording.
        m_nRecordState = RecordOff;
        m_bShouldRecordText = FALSE;

        m_nAsyncCmdLevel = 0;

	m_bInvokedCommandLine = FALSE;
	m_hLogFile = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTheApp destruction

CTheApp::~CTheApp()
{
        // free recent project list
        if (m_pRecentProjectList != NULL)
                delete m_pRecentProjectList;
        if (m_pszRegKeyName != NULL)
                free((void*) m_pszRegKeyName);
#ifdef _EXPORT_LAYOUT
        if (m_pszLayoutFilePath != NULL)
                free((void*) m_pszLayoutFilePath);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CTheApp initialization

static BOOL GetCmdLineProfile (LPCTSTR lpszProfileName, CStringList& argList, POSITION posInsertAfter);
static BOOL InitJIT();
static BOOL FFindOtherJitDebugger(CJit* pjit);
static UINT ErrGetJITInfo(CStringList* pArgList, CJit* pjit);
extern void SushiRestoreKey(HKEY hKey, const CString& str);

#ifdef _DEBUG
BOOL AFXAPI  OurAllocHook (size_t nSize, BOOL bObject, LONG lRequestNumber);
AFX_ALLOC_HOOK pfnNextHook;
#endif

extern HINSTANCE GetResourceHandle();

CVshellPackage::CVshellPackage()
: CPackage(GetResourceHandle(), PACKAGE_SUSHI, PKS,
                   MAKEINTRESOURCE(IDCT_SHELLPACKAGE),
                   MAKEINTRESOURCE(IDB_MAINTOOLS),
                   MAKEINTRESOURCE(IDR_MAINFRAME),
                   MAKEINTRESOURCE(IDB_MAINLARGETOOLS))
{
        // also need to add command table from exe
        theCmdCache.AddCommandResource(this, MAKEINTRESOURCE(IDCT_MAINEXE), TRUE);
}

CVshellPackage::~CVshellPackage()
{
        // Delete Named Item list
        while (!m_ptrlAutomationItems.IsEmpty())
        {
                CAutomationItem* pAutomationItem = m_ptrlAutomationItems.RemoveHead();
                delete pAutomationItem;
        }
}

//#define TIMEBOMB
#ifdef TIMEBOMB
#include <timebomb.h>
#pragma comment(lib, "timebomb")

extern "C" static void __cdecl MsvcTBCallBack(FC fc, const char *szMsg)
{
        // Default GUI style callback, bombs out on any error
        char szExe[_MAX_PATH];
        ::GetModuleFileName(NULL, szExe, sizeof(szExe));
        MessageBox(NULL, szMsg, szExe, MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND);
}

#endif


#ifdef _DEBUG
#define APP_CLSID CLSID_ApplicationDebug
#else
#define APP_CLSID CLSID_Application
#endif

static void UnregisterPackageTypeLib(LPCTSTR strFileName)
{
	// See if the package has a type library.  If it does, register it
	//	and remember the TLID in case the package provides a top-level
	//	object.
	USES_CONVERSION;
	ITypeLib* pTypeLib = NULL;
	if (SUCCEEDED(LoadTypeLib(T2W(strFileName), &pTypeLib)) && pTypeLib != NULL)
	{
		TLIBATTR* pTlibAttr = NULL;
		if (SUCCEEDED(pTypeLib->GetLibAttr(&pTlibAttr)) && pTlibAttr != NULL)
		{
			// Register type lib
			::UnRegisterTypeLib(pTlibAttr->guid,
								pTlibAttr->wMajorVerNum, pTlibAttr->wMinorVerNum,
								pTlibAttr->lcid, pTlibAttr->syskind);
			pTypeLib->ReleaseTLibAttr(pTlibAttr);
		}
		pTypeLib->Release();
		pTypeLib = NULL;
	}
}




////////////////////////////////////////////////////////////////////////////
//
// Command Line Log file utility
//

BOOL CTheApp::WriteLog(LPCTSTR szOut, BOOL bAppendCRLF /* = FALSE */)
{
	if (m_hLogFile == NULL)
		return FALSE;

	// NULL means close stdout
	if (szOut == NULL)
	{
		::CloseHandle(m_hLogFile);
		m_hLogFile = NULL;
		return TRUE;
	}

	CString strOut = szOut;
	strOut.AnsiToOem();
	szOut = strOut;

	int len;
	DWORD nBytes;
	LPCTSTR pszC = szOut;
	while ((pszC = _tcschr(szOut, _T('\n'))) != NULL)
	{
		len = pszC - szOut;
		if (len > 0)
		{
			nBytes = (DWORD)len;
			WriteFile(m_hLogFile, szOut, (DWORD)len, &nBytes, NULL);

			// check for bogus \r in calling code
			ASSERT(*_tcsdec(szOut, pszC) != _T('\r'));
		}

		WriteFile(m_hLogFile, _T("\r\n"), 2, &nBytes, NULL);
		szOut = ++pszC;
	}

	// check for any remainder
	len = _tcslen(szOut);
	if (len > 0)
	{
		nBytes = (DWORD)len;
		WriteFile(m_hLogFile, szOut, (DWORD)len, &nBytes, NULL);
	}

	if (bAppendCRLF)
	{
		WriteFile(m_hLogFile, _T("\r\n"), 2, &nBytes, NULL);
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Command Line Processing
//

inline BOOL IsSwitch(TCHAR ch)
{
        return (ch == '/' || ch == '-');
}

// Scan the command line for /szSwitch or -szSwitch.  Return TRUE if it's found
// or FALSE if it's not.  If pstrArg is not NULL, then copy the parameter right
// after the switch into it.  If bRemove is TRUE remove the switch and optional
// argument from the command line.
//
BOOL CTheApp::ScanCommandLine(const TCHAR* szSwitch, CString* pstrArg, BOOL bRemove)
{
	POSITION pos = theApp.m_argList.GetHeadPosition();
	POSITION posThis;

	while ((posThis = pos) != 0)
	{
		const TCHAR* szArg = theApp.m_argList.GetNext(pos);

		if (IsSwitch(szArg[0]) && _tcsicmp(szArg + 1, szSwitch) == 0)
		{
			if (pstrArg != NULL)
			{
				if (pos != NULL)
				{
			      	*pstrArg = theApp.m_argList.GetAt(pos);

					// make sure we don't return a switch as an argument
					if (IsSwitch((*pstrArg)[0]))
					{
						// return no argument
						*pstrArg = _T("");
						pos = NULL; // don't remove a switch
					}
				}
				else
				{
					// return no argument
					*pstrArg = _T("");
				}
			}

			if (bRemove)
			{
				theApp.m_argList.RemoveAt(posThis);
				if (pstrArg != NULL && pos != NULL)
					theApp.m_argList.RemoveAt(pos);
			}

			return TRUE;
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

void CTheApp::ShowSplashScreen(BOOL bShow)
{
        ::ShowSplashScreen(bShow);
}

#ifdef PKG_LOAD_TIME
#define MAX_PACKAGE_NUMBER      40
DWORD dwaPkgLoadLibraryTime[MAX_PACKAGE_NUMBER];
DWORD dwaPkgTypeLibTime[MAX_PACKAGE_NUMBER];
DWORD dwaPkgInitPackageTime[MAX_PACKAGE_NUMBER];
DWORD dwaPkgOnInit[MAX_PACKAGE_NUMBER];
CString straPkgName[MAX_PACKAGE_NUMBER];
int nPkg = 0;
#endif  // PKG_LOAD_TIME
#ifdef METER_IDLE_TIME
BOOL s_bIdleTime = FALSE;
#endif  // METER_IDLE_TIME

BOOL ShouldRegisterTypeLibsOnStartup(int cPackagesLoaded, BOOL& rbForceFullRegistration);
BOOL CTheApp::InitInstance()
{
#ifdef PKG_LOAD_TIME
	DWORD dwInitInstance = ::GetTickCount();
#endif  // PKG_LOAD_TIME
	BOOL fUnregisterAndQuit = FALSE;

	m_pszProfileName = _tcsdup(GetExeString(DefaultRegKeyName));
	m_bRunInvisibly = RunAutomated() || RunEmbedded();
	m_bInvokedCommandLine = FALSE;

	if (!m_bWin4)
	{
		CString strTitle, strMsg;
		VERIFY(strTitle.LoadString(AFX_IDS_APP_TITLE));
		VERIFY(strMsg.LoadString(IDS_NOT_WINDOWS4));
		::MessageBox(NULL, strMsg, strTitle, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	DWORD dwVersion = ::GetVersion();
	if ((dwVersion & 0x80000000) != 0 && (BYTE)dwVersion <= 3)
	{
        CString strTitle, strMsg;
        VERIFY(strTitle.LoadString(AFX_IDS_APP_TITLE));
        VERIFY(strMsg.LoadString(IDS_NOT_WIN32S));
        ::MessageBox(NULL, strMsg, strTitle, MB_OK | MB_ICONSTOP);
        return FALSE;
	}


	char szBuff[_MAX_PATH+1];

	{ CToolBar tb; }        // Sleazy hack to initialize MFC library's toolbar
			// drawing code, prior to ReloadBitmaps calls

	// BLOCK: Stolen from CWinApp::EnableShellOpen, but different...
	{
		// This enabled the App Wiz to talk to us...
		m_atomApp = ::GlobalAddAtom("MSDEV");
		m_atomSystemTopic = ::GlobalAddAtom("system");
	}

	// Figure out what code page we're running under (based on c-runtimes)
	m_codePage = _getmbcp();
	// HACK: old behavior for the code page: m_codePage = 0 for the standard code page.
	if(m_codePage == 1252) // 1252 is the code page for the US and Western Europe
		m_codePage = 0;
	m_fOEM_MBCS = (m_codePage != 0); 

	// Set the c-runtimes locale to be the same as the O.S. locale.
	setlocale(LC_ALL, "");

	if (m_fOEM_MBCS)
		imeInit();

	// Put all command line arguments into m_argList
	for (int i = 1; i < __argc; i++)
		m_argList.AddTail(__argv[i]);

#ifdef TIMEBOMB
#pragma message("WARNING!: TimeBomb is ENABLED!")
	// Block
	{
		FC fc = FcCheckVerDate ( 528, &ctmOpenBy, &ctmCloseBy, MsvcTBCallBack );
		if ( fc == fcPastEndDate || fc == fcDateDecremented )
		{
			return FALSE;
		}
	}
#endif

	g_bInvokedHelp =  ( /* ScanCommandLine("help, NULL, FALSE") || */ ScanCommandLine("?", NULL, FALSE));

	CString strOutFile;
	BOOL bOutSpecified = (ScanCommandLine("out", &strOutFile, TRUE));
	if ((ScanCommandLine("make", NULL, g_bInvokedHelp)) || (g_bInvokedHelp))
	{
		m_bRunInvisibly = TRUE;
		m_bInvokedCommandLine = TRUE;
		{
			g_packages = rgszMakePackages;
			g_nPackages = sizeof(rgszMakePackages) / sizeof(const TCHAR *); // 3
		}

		// make sure we have console output
		// g_hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
		HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		// HANDLE hStdErr = ::GetStdHandle(STD_ERROR_HANDLE);
		HANDLE hFile;
		if (bOutSpecified)
		{
			if ((!strOutFile.IsEmpty()) && (!IsSwitch(strOutFile[0])))
			{
				 // REVIEW: further validation?
				if (((hFile = CreateFile(strOutFile, GENERIC_WRITE, FILE_SHARE_READ, NULL /* REVIEW */, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) && (hFile != NULL) && (hFile != (void *)-1))
				{
					m_hLogFile = hFile;
					if (SetStdHandle(STD_OUTPUT_HANDLE, hFile))
					{
						// force stdout to use specified filename
						::CloseHandle(hStdOut);
						hStdOut = hFile;
						int hCHandle = _open_osfhandle((long)hFile, _O_TEXT);
						FILE * hCStream = _fdopen(hCHandle, "wt");
						ASSERT(hCHandle != -1);
						if (hCHandle != -1)
						{
							if (_fileno(stdout)==-1)
							{
								stdout->_file = 1;
								stdout->_flag = _IOWRT;
							}

							int hRet;
							if ((hRet = _dup2(hCHandle,  1 /* _fileno(stdout) */))!=0)
							{
								// fprintf(hCStream, "errno = %d %x\n", errno, errno);
								ASSERT(0);
							}
						}
						else
						{
							// error: stdout could not be redirected
							// SetStdHandle(STD_OUTPUT_HANDLE, hStdOut);
							// CloseHandle(hFile);
						}
					} 
					else
					{
						// error: stdout could not be redirected
						// CloseHandle(hFile);
					}
				}
				else
				{
					// error: file could not be created
					if ((hStdOut != INVALID_HANDLE_VALUE) && (hStdOut != NULL))
					{
						// syntax error or read-only file
						CString str, strOut;
						m_hLogFile = hStdOut;

						MsgText(str, IDS_ERR_OUT_ARG, (LPCTSTR)strOutFile);

						// REVIEW: give more detailed error info
						strOut.LoadString(IDS_INVALID_FILENAME);
						strOut += _T('\n');
						WriteLog(str, TRUE);
						WriteLog(strOut, TRUE);
					}
				}
			}
			else
			{
				if ((hStdOut != INVALID_HANDLE_VALUE) && (hStdOut != NULL))
				{
					m_hLogFile = hStdOut;
				}
			}
		}
		if ((!m_hLogFile) && ((hStdOut == INVALID_HANDLE_VALUE) || (hStdOut == NULL) || ((hStdOut == (HANDLE)7) && (!m_bWin95)))) // REVIEW
		{
			if (::AllocConsole())
			{
				g_bConsoleCreated = TRUE;
				// g_hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
				hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
				// hStdErr = ::GetStdHandle(STD_ERROR_HANDLE);
				ASSERT(hStdOut != INVALID_HANDLE_VALUE);

#if 0
				int hCHandle = _open_osfhandle((long)hStdOut, _O_TEXT);
				ASSERT(hCHandle != -1);
				if (hCHandle != -1)
				{
					FILE * hCStream = _fdopen(hCHandle, "wt");
					if (_fileno(stdout)==-1)
					{
						stdout->_file = 1;
						stdout->_flag = _IOWRT;
						ASSERT(0);
					}

					VERIFY(!_dup2(hCHandle, 1 /* _fileno(stdout) */));
				}
				else
				{
					ASSERT(0);
				}
#endif

				if (!m_hLogFile)
					m_hLogFile = hStdOut;
				else
					::CloseHandle(hStdOut);
			}
			else
			{
				// error: could not alloc console
				// ASSERT(0);
			}
		}
		else
		{
                     // ASSERT(hStdOut != (void *)7);
			// should be okay
			if (!m_hLogFile)
			{
				m_hLogFile = hStdOut;
#if 0
				int hCHandle = _open_osfhandle((long)hStdOut, _O_TEXT);
				ASSERT(hCHandle != -1);
				if (hCHandle != -1)
				{
					FILE * hCStream = _fdopen(hCHandle, "wt");
					if (_fileno(stdout)==-1)
					{
						stdout->_file = 1;
						stdout->_flag = _IOWRT;
						ASSERT(0);
					}

					VERIFY(!_dup2(hCHandle, _fileno(stdout) /* _fileno(stdout) */));
				}
#endif
			}
		}
	}
#ifndef _SHIP
	if (ScanCommandLine("visible", NULL, TRUE))
	{
		m_bRunInvisibly = FALSE;
	}
#endif
	if (g_bInvokedHelp)
	{
		CString strOut;
		strOut.LoadString(IDS_SHL_HLP_USAGE);
		WriteLog(strOut);
		strOut.LoadString(IDS_SHL_HLP_USAGE2);
		WriteLog(strOut);

		strOut.LoadString(IDS_SHL_HLP_HELP);
		WriteLog(strOut);

		strOut.LoadString(IDS_AUT_HLP_EX);
		WriteLog(strOut);

		strOut.LoadString(IDS_SHL_HLP_OUT);
		WriteLog(strOut);
	}

	InitMsgBoxes();
	InitMetrics();


	// check the CRTs are correct as certain programs (Win98, Symantec CAFE)
	// downgrade the DLL to the V5 one. This hoses us as the Disasm DLL uses
	// MSVCP60 which needs the V6 CRT DLL
	char *szCRTName, *szCIRTName, *szMFCName;
	char szPath[ _MAX_PATH ];
#ifdef _DEBUG
	szCRTName = "MSVCRTD.DLL";
	szMFCName = "MFC42D.DLL";
#else
	szCRTName = "MSVCRT.DLL";
	szMFCName = "MFC42.DLL";
#endif

	HMODULE hMod = ::GetModuleHandle( szCRTName );
	if (hMod && ::GetModuleFileName( hMod, szPath, sizeof(szPath) ))
	{
		BYTE verBuf[1024];
		if (GetFileVersionInfo( (char*)szPath, 0, sizeof(verBuf), &verBuf ))
		{
			VS_FIXEDFILEINFO *pVersion;
			UINT cSize;
			if (VerQueryValue( verBuf, TEXT("\\"), (LPVOID*)&pVersion, &cSize ))
			{
				if (pVersion->dwFileVersionMS < 0x60000)
				{
					// CRT is too old, so warn about it
					CString strTitle, strMsg;
					strTitle.LoadString(AFX_IDS_APP_TITLE);
					strMsg.LoadString(IDS_BAD_CRT_VER);
					::MessageBox(NULL, strMsg, strTitle, MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}
	}
	hMod = ::GetModuleHandle( szMFCName );
	if (hMod && ::GetModuleFileName( hMod, szPath, sizeof(szPath) ))
	{
		BYTE verBuf[1024];
		if (GetFileVersionInfo( (char*)szPath, 0, sizeof(verBuf), &verBuf ))
		{
			VS_FIXEDFILEINFO *pVersion;
			UINT cSize;
			if (VerQueryValue( verBuf, TEXT("\\"), (LPVOID*)&pVersion, &cSize ))
			{
				if (pVersion->dwFileVersionMS < 0x60000)
				{
					// MFC is too old, so warn about it
					CString strTitle, strMsg;
					strTitle.LoadString(AFX_IDS_APP_TITLE);
					strMsg.LoadString(IDS_BAD_MFC42_VER);
					::MessageBox(NULL, strMsg, strTitle, MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}
	}




	// BEFORE the splash screen goes up, and before any time-consuming
	// DLL loads are done, see if we were spawned as a just-in-time
	// debugger, and there's already an MSVC running in the system
	// which wants to be the JIT debugger.  If so, just get out.
	if (!InitJIT())
		return FALSE;

	// we need to instantiate our message filter which support IServiceProvider.
	// overwrite AfxOleInit() which initialize OLE 2.0 libraries
	if (!InitOle())
	{
		//              AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// By default OLE applications are enabled for Compound Files
	//      COleDocument::EnableDocFile();
	AfxEnableControlContainer();

	// Read command line settings.
#ifdef _WIN32
        // FUTURE:  The autosaver will go away from the command line when it goes
        //                       into the options dialogue box.  [w-amilt]

#ifdef AUTOSAVE_ON_COMMAND_LINE
	BOOL fEnableAutosaver = FALSE;
	int nAutosaveTime;
#endif  // AUTOSAVE_ON_COMMAND_LINE
	CString strRestoreFile;
	BOOL fNoLogo = FALSE;
	BOOL bCreateNewProject = FALSE;
	POSITION pos, posCur;
	CPath path;
	CString strExe = ".exe";
	CString strTarg = "";

#ifdef _MTTF
	m_bMTTF = TRUE;
#endif

#ifdef _CMDLOG
#ifdef _SHIP
	m_bCmdLog = FALSE;
#else
	m_bCmdLog = FALSE; // default to OFF for now
#endif // _SHIP
#endif // _CMDLOG

	m_bEncPerfLog = FALSE;

#ifdef _ENCLOG
	m_bEncLog = TRUE;
#endif

#ifdef PKG_LOAD_TIME
	BOOL s_bLoadTime = FALSE;
#endif  // PKG_LOAD_TIME
#ifdef PERF_MEASUREMENTS
	///////////////// Testing code for V4 perf release (fabriced)
	m_bPerfQA_AutoRun = FALSE;
	m_bPerfQA_WorkSet = FALSE;
	m_bPerfQA_Workspace = FALSE;
	m_szPerfQA_Project = NULL;
                /////////////////////////////////////////////////////////////
#endif  // PERF_MEASUREMENTS

#ifdef _IDELOG
#ifdef _SHIP
	m_bIDELog = FALSE;
#else
	m_bIDELog = FALSE; // default to OFF for now
#endif // _SHIP
#endif // _IDELOG

	// initialize m_bRestoreKey to FALSE
	// flag is set if /RESTORE command line is specified.
	// If it is set, then vproj will not prepend the default
	// environment setting for msvc
	m_bRestoreKey = FALSE ;

	// iterate through args
	//
	pos = m_argList.GetHeadPosition();

			BOOL bForceFullRegistration = FALSE;                    // -REGISTER on cmdline?
	CString strCommand;                             // -EXECUTE Command passed on the command line?
	while ((posCur = pos) != 0)
	{
		CString strArg = m_argList.GetNext(pos);

		// see if option
		//
		if(IsSwitch(strArg[0]) && strArg.GetLength() >= 2)
		{
			TCHAR	tch = strArg[1];
			LPCTSTR tsz = strArg;
			BOOL	fRemoveArg = FALSE;

			CharUpperBuff(&tch, 1);
			int nSwitchLength = _tcslen((LPCTSTR) &tsz[1]);
			switch(tch)
			{
				case 'T':
					if( _tcsicmp(&tsz[1], "TARG") == 0 )
					{
							POSITION posLast = pos;
							strTarg = m_argList.GetNext(pos);
							// HACK need to remove the current
							// and next arg
							fRemoveArg = FALSE;
							m_argList.RemoveAt(posCur);
							m_argList.RemoveAt(posLast);
					}
					break;
#ifdef _EXPORT_LAYOUT
				case 'X':
						if( _tcsicmp(&tsz[1], "XPORT_LAYOUT") == 0 )
						{
								fRemoveArg = TRUE;
								m_bExportLayout = TRUE;
						}
						break;
#endif

#ifdef AUTOSAVE_ON_COMMAND_LINE
				// 11/19/93 [patbr] autosave now has UI, so
				// command-line interface no longer necessary
				case 'A':
					// Autosave?
					if (_tcsnicmp(&tsz[1], "AUTOSAVE", 8) == 0 )
					{
						// Yes.  Check for a ':<time>' suffix
						fRemoveArg = TRUE;
						fEnableAutosaver = TRUE;
						if (tsz[9] == ':')
						{
							if ((nAutosaveTime = atoi(&tsz[10])) < 0)
							{
								ErrorBox(IDS_AUTOSAVE_BAD_INTERVAL,
										nAutosaveTime, gAutosaver.GetInterval());
								nAutosaveTime = (int) gAutosaver.GetInterval();
							}
						}
						else
							nAutosaveTime = (int) gAutosaver.GetInterval();
					}
					break;
#endif	// AUTOSAVE_ON_COMMAND_LINE

#ifdef _CMDLOG
#ifdef _SHIP
				case 'C':
					if (_tcsicmp(&tsz[1], "CMDLOG") == 0)
					{
						fRemoveArg = TRUE;
						m_bCmdLog = TRUE;
					}
					break;
#endif // _SHIP
#endif // _CMDLOG

				case 'I':
#ifdef	_IDELOG
#ifndef _SHIP
					if ( _tcsicmp(&tsz[1],"IDELOG") == 0 )
					{
							fRemoveArg = TRUE;
							m_bIDELog = TRUE;
							break;
					}
#endif // _SHIP
#endif // _IDELOG
					// Key name.
					fRemoveArg = TRUE;

					lstrcpy(szBuff, m_pszProfileName);
					lstrcat(szBuff, &tsz[2]);

					{
							// We now allocate m_pszProfileName, so we need to free it
							// here to avoid leaks (fabriced 22May96).
							free((void *) m_pszProfileName);
							m_pszProfileName = _tcsdup(szBuff);
					}
					break;

				case 'N':
					if (_tcsicmp(&tsz[1], "NEWPROJ") == 0)
					{
							bCreateNewProject = TRUE;
							fNoLogo = TRUE; // implied
							fRemoveArg = TRUE;
					}
					else if (_tcsicmp(&tsz[1], "NOLOGO") == 0)
					{
							fRemoveArg = TRUE;
							fNoLogo = TRUE;
					}
#ifdef _MTTF
					else if ( _tcsicmp(&tsz[1],"NOMTTF") == 0 )
					{
							fRemoveArg = TRUE;
							m_bMTTF = FALSE;
					}
#endif
#ifdef _ENCLOG
					else if ( _tcsicmp(&tsz[1],"NOENCNETLOG") == 0 )
					{
							fRemoveArg = TRUE;
							m_bEncLog = FALSE;
					}
#endif
#ifdef	_CMDLOG
#ifndef _SHIP
					else if ( _tcsicmp(&tsz[1],"NOCMDLOG") == 0 )
					{
							fRemoveArg = TRUE;
							m_bCmdLog = FALSE;
					}
#endif // _SHIP
#endif // _CMDLOG
#ifdef	_IDELOG
#ifndef _SHIP
					else if ( _tcsicmp(&tsz[1],"NOIDELOG") == 0 )
					{
							fRemoveArg = TRUE;
							m_bIDELog = FALSE;
					}
#endif // _SHIP
#endif // _IDELOG
#ifndef _SHIP
					else if ( _tcsicmp(&tsz[1],"NOLOG") == 0 )
					{
						// Turn off all types of logging
#ifdef	_IDELOG
						m_bIDELog = FALSE;
#endif
#ifdef	_CMDLOG
						m_bCmdLog = FALSE;
#endif
#ifdef _MTTF
						m_bMTTF = FALSE;
#endif
#ifdef _ENCLOG
						m_bEncLog = FALSE;
#endif
					}
#endif // _SHIP
					break;

				case 'P':
					if (_tcsicmp(&tsz[1], "PROFILE") == 0 )
					{
							fRemoveArg = TRUE;

							// Make sure we have a next arg
							if (pos == NULL)
							{
									TRACE0("/PROFILE: No profile name specified with switch.");
									break;
							}

							// Get the next arg, which should be the profile name
							POSITION posLast = posCur;
							posCur = pos;
							strTarg = m_argList.GetNext(pos);

							// Get the cmd line args from the registry for the specified profile
							if (GetCmdLineProfile(strTarg, m_argList, posCur) == FALSE)
							{
									TRACE1("/PROFILE: Could not find specified profile '%s'", strTarg);
									break;
							}

							// Set pointer correctly to next arg, since list may have changed
							pos = posCur;
							m_argList.GetNext(pos);

							// Manually remove current and last args
							fRemoveArg = FALSE;
							m_argList.RemoveAt(posCur);
							m_argList.RemoveAt(posLast);
					}
#ifdef PKG_LOAD_TIME
					else if(_tcsicmp(&tsz[1], "PKGLOADTIME") == 0 )
					{
							s_bLoadTime = TRUE;
					}
#endif	// PKG_LOAD_TIME
#ifdef METER_IDLE_TIME
					else if(_tcsicmp(&tsz[1], "PERFIDLETIME") == 0 )
					{
							s_bIdleTime = TRUE;
					}
#endif	// METER_IDLE_TIME
#ifdef PERF_MEASUREMENTS
					///////////////// Testing code for V4 perf release (fabriced)
					else if(_tcsicmp(&tsz[1], "PERFQA") == 0 )
					{
							m_bPerfQA_AutoRun = TRUE;
							fRemoveArg = TRUE;
					}
					else if(_tcsicmp(&tsz[1], "PERFWORKSET") == 0 )
					{
							m_bPerfQA_WorkSet = TRUE;
							fRemoveArg = TRUE;
					}
					else if(_tcsicmp(&tsz[1], "PERFWORKSPACE") == 0 )
					{
							m_bPerfQA_Workspace = TRUE;
							fRemoveArg = TRUE;
					}
					/////////////////////////////////////////////////////////////
#endif	// PERF_MEASUREMENTS
					break;
				case 'U':
					if (_tcsicmp(&tsz[1], "UNREGISTER") == 0 )
					{
#ifdef _DEBUG
						UnregisterPackageTypeLib("ided\\devbldd.pkg");
						UnregisterPackageTypeLib("ided\\devdbgd.pkg");
						UnregisterPackageTypeLib("deveditd.pkg");
						UnregisterPackageTypeLib("devshld.dll");
						TCHAR *szObject = "MSDEVD.APPLICATION";
#else
						UnregisterPackageTypeLib("ide\\devbld.pkg");
						UnregisterPackageTypeLib("ide\\devdbg.pkg");
						UnregisterPackageTypeLib("devedit.pkg");
						UnregisterPackageTypeLib("devshl.dll");
						TCHAR *szObject = "MSDEV.APPLICATION";
#endif
						CRegKey key;
						if(key.Open(HKEY_CLASSES_ROOT, NULL) == ERROR_SUCCESS)
						{
							key.RecurseDeleteKey(szObject);
							if(key.Open(key, "CLSID")== ERROR_SUCCESS)
							{
								COleMalloc<OLECHAR>	wsz;
								if(StringFromCLSID(APP_CLSID, &wsz)== S_OK)
								{
									USES_CONVERSION;
									key.RecurseDeleteKey(OLE2T(wsz));
								}

							}
						}
						fUnregisterAndQuit = TRUE;	// Do not continue.
					}
					break;

				case 'R':
					// Restore?
					if (_tcsicmp(&tsz[1], "RESTORE") == 0 )
					{
							// get path of executable
							static char BASED_CODE szLyt[] = ".KEY";

							fRemoveArg = TRUE;
							VERIFY(::GetModuleFileName(m_hInstance, szBuff, _MAX_PATH));
							int nLen = lstrlen(szBuff);
							ASSERT(nLen > 4 && szBuff[nLen-4] == '.');	// must end in .EXE
							nLen -= 4;
							szBuff[nLen] = '\0';		// no suffix
							lstrcat(szBuff, szLyt);
							strRestoreFile = szBuff;
					}
					else if (_tcsicmp(&tsz[1], "REGISTER") == 0 )
					{
							bForceFullRegistration = TRUE;
					}
#ifdef RAD_DEMO
					else if (_tcsicmp(&tsz[1], "RAD") == 0 )
					{
						_spawnl(_P_WAIT, "CreateProj.bat", "CreateProj.bat", NULL);
					}
#endif // RAD_DEMO
					break;

				case 'E':
					// Execute command (macro, e.g.)
					if (nSwitchLength > 1			// 'e' is used for JIT
							&& _tcsnicmp(&tsz[1], "EXECUTE", nSwitchLength) == 0 )
					{
							POSITION posLast = pos;
							if(pos)
							{
								strCommand = m_argList.GetNext(pos);
								// need to remove the current
								//	and next arg.  Manually remove -EXECUTE and
								//	command name here, rather than auto-remove
								//	arg later
								fRemoveArg = FALSE;
								m_argList.RemoveAt(posCur);
								m_argList.RemoveAt(posLast);
							}
					}
					else if (_tcsicmp(&tsz[1], "ENCPERFLOG") == 0 )
					{
							fRemoveArg = TRUE;
							m_bEncPerfLog = TRUE;
					}

					break;

			}	// End switch

			if (fRemoveArg)
				m_argList.RemoveAt(posCur);
		}
		else
		{

#ifdef PERF_MEASUREMENTS
			///////////////// Testing code for V4.x perf release (konradt)
			if(m_bPerfQA_AutoRun)
			{
					if ((strArg.Find(".mdp") != -1) || (strArg.Find(".MDP") != -1))
					{
							m_szPerfQA_Project = new char[strArg.GetLength() + 1];
							strcpy(m_szPerfQA_Project, (LPCTSTR)strArg);
							m_argList.RemoveAt(posCur);
					}
			}
			/////////////////////////////////////////////////////////////
#endif	// PERF_MEASUREMENTS

			// if we see an exe on the command line, the entire remainder
			// of the command line consists of the command line to be
			// passed to that exe, rather than options for MSVC.
			//
			// FUTURE: can't go checking name extensions (might not be .EXE)!
			if (path.Create(strArg) &&
					strExe.CompareNoCase(path.GetExtension()) == 0)
			{
					break;
			}
		}
	}

	// Prime MFC reg DB info.
	m_pszRegKeyName = _tcsdup(m_pszProfileName);	 // Save the profile name.
	SetRegistryKey(g_szCompanyName);		  // SetCompanyName() resets the profile.
	free((void *)m_pszProfileName);
	m_pszProfileName = m_pszRegKeyName;

	// Build full version of the application's root key name.
	lstrcpy(szBuff, g_szKeyRoot);
	*(szBuff + sizeof(g_szKeyRoot) - 1) = chKeySep;
	lstrcpy(szBuff + sizeof(g_szKeyRoot), g_szCompanyName);
	*(szBuff + sizeof(g_szKeyRoot) + sizeof(g_szCompanyName) - 1) = chKeySep;
	lstrcpy(szBuff + sizeof(g_szKeyRoot) + sizeof(g_szCompanyName), m_pszProfileName);
	m_pszRegKeyName = _tcsdup(szBuff);

	// restore the registration DB key
	if (!strRestoreFile.IsEmpty())
	{
		DWORD dwDisp;
		HKEY hKeyMain;
		VERIFY(RegCreateKeyEx(HKEY_CURRENT_USER, GetRegistryKeyName(),
				0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
				&hKeyMain, &dwDisp) 	== ERROR_SUCCESS);

		SushiRestoreKey(hKeyMain, strRestoreFile);
		RegCloseKey(hKeyMain);
		m_bRestoreKey = TRUE ;
	}

	// Init message boxes early in case anything fails
	{
		BOOL bUseCtl3d;
		// CTL3D32 causes many problems on NT-J 3.1...
		if (m_fOEM_MBCS && (LOBYTE(LOWORD(dwVersion)) == 0x03) &&
			(HIBYTE(LOWORD(dwVersion)) < 0x32))
			bUseCtl3d = FALSE;
		else
			bUseCtl3d = TRUE;

		if (GetRegInt(szGeneralKey, "Enable3d", bUseCtl3d))
			bEnable3dControls = m_bWin4 || Enable3dControls();
		else
			bEnable3dControls = FALSE;
	}
	if (!bEnable3dControls)
		SetDialogBkColor(); 	// button face dialogs.

#endif	// _WIN32
	if(fUnregisterAndQuit)
		return FALSE;
#ifdef _EXPORT_LAYOUT
	//BLOCK:
	{
		// get path of executable
		VERIFY(::GetModuleFileName(m_hInstance, szBuff, _MAX_PATH));
		int nLen = lstrlen(szBuff);
		ASSERT(nLen > 4 && szBuff[nLen-4] == '.');	// must end in .EXE
		nLen -= 4;
		szBuff[nLen] = '\0';		// no suffix

		// get path of .LYT file
		if (m_pszLayoutFilePath == NULL)
		{
			static char BASED_CODE szLyt[] = "%d.LYT";		// Used in wsprintf.
			lstrcat(szBuff, szLyt);
			m_pszLayoutFilePath = _tcsdup(szBuff);
		}
	}
#endif

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	m_pMainWnd = pMainFrame;

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		m_pMainWnd = NULL;
		return FALSE;
	}

	LoadPrivateProfileSettings();

	// Determine how we will show our main window.
#ifdef _WIN32
	if (m_nCmdShow == SW_SHOWDEFAULT)
	{
		STARTUPINFO si = {sizeof(si)};
		GetStartupInfo(&si);

		if (si.dwFlags & STARTF_USESHOWWINDOW)
			m_nCmdShow = si.wShowWindow;
	}
#endif

	if (theApp.m_bMaximizeApp &&
					(m_nCmdShow == SW_SHOWNORMAL ||
					m_nCmdShow == SW_SHOW ||
					m_nCmdShow == SW_SHOWDEFAULT))
	{
		m_nCmdShow = SW_SHOWMAXIMIZED;
	}
	if (m_bRunInvisibly)
		m_nCmdShow = SW_HIDE;

	InitProductId(NULL); // init product id info from exe's PID resource





	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	// NOTE: Show splash screen as soon after we create main window
	// as possible.  Try not to add code above here.

	// show spash screen after we have created the main window so
	// we can parent it to the main window.  otherwise it will be
	// hidden when we show the main window. -bm
	//
	if (!m_bRunInvisibly && !fNoLogo && m_nCmdShow != SW_MINIMIZE && m_nCmdShow != SW_SHOWMINIMIZED)
	{
		ShowSplashScreen(TRUE);
	}


#ifdef _CMDLOG
	InitCmdLog();
#endif

#ifdef _IDELOG
	if (m_bIDELog)
	{
		InitIDEstartupLog();
	}
#endif

	pMainFrame->InitKeymap();

#ifdef _WIN32
	// 11/19/93 [patbr]
	// install autosaver before restoring profile settings
#ifdef ENABLE_FILE_AUTOSAVES
	gAutosaver.Install(pMainFrame->m_hWnd);
#endif	// ENABLE_FILE_AUTOSAVES
	LoadPrivateAutosaveSettings();
#endif

	CPackage* pPackage = new CVshellPackage();
	VERIFY(RegisterPackage(pPackage));

#ifndef NOIPCOMP
	//		(this is separate from calling InitStdCompMgr because this inits the
	//		library itself, not the manager object
	//		-- equivalent to DLL_PROCESS_ATTACH for the library).
	CMLibInit(AfxGetInstanceHandle());

	// initialize Office's StdComponentManager after the main frame has been
	// created, but before we instantiate any components or packages that use it.
	if (!InitStdCompMgr())
		return FALSE;
#endif

	// Register all programmable objects which currently exist.
	COleObjectFactory::RegisterAll();				// current state of OLE DLL's

	// Update the persistent registry with info on our object factories.
	COleObjectFactory::UpdateRegistryAll(); // persistent registry state

	// Before we load the packages (particularly the vba package), create
	//	our OLE automation Application object.
	if ((m_pAutoApp = CApplication::Create()) == NULL)
	{
		TRACE("BAD ERROR: CTheApp::InitInstance: Failed to create application object!\n");
		return FALSE;
	}
	
	int cPackagesLoaded=0;
	BOOL bPackageLoadSuccess=LoadPackages(&cPackagesLoaded);
	if (!bPackageLoadSuccess)
	{
		theApp.m_pAutoApp->Zombify();
		theApp.m_pAutoApp->Destroy();
		pMainFrame->DestroyWindow();
		return FALSE;
	}

	// For an explanation of all of this, see the comments on CPackage::GetPackageProperty()
	if(GetExecutableProperty(030370)==0x6d736c)
	{
			m_bHideMicrosoftWebMenu=FALSE;
	}
	else
	{
			// Determine if we should show the microsoft on the web menu
			CPackage* pPackageEnum;
			pos = m_packages.GetHeadPosition();

			while (pos != NULL)
			{
					pPackageEnum = (CPackage*) m_packages.GetNext(pos);
					if(pPackageEnum)
					{
							if(pPackageEnum->GetPackageProperty(030370)==0x6d736c)
							{
									m_bHideMicrosoftWebMenu=FALSE;
									break;
							}
					}
			}
	}

	if (!(m_bRunInvisibly && m_bInvokedCommandLine)) // REVIEW
		LoadMRUList();  // Loads CRuntimeClasses which reside in packages.

	// allow packages to initialise after all of the other packages
	// have been loaded (they might depend on this fact) - done before InitAfterRegistration
	// to avoid CDockManager repainting before the source windows have their coloring information
	CPackage* pPackageEnum;
	pos = m_packages.GetHeadPosition();

#ifdef PKG_LOAD_TIME
	nPkg = -1; // Vshell is first package so we will skip this one
#endif	// PKG_LOAD_TIME

	while (pos != NULL)
	{
#ifdef PKG_LOAD_TIME
		if( nPkg >=0 )
				dwaPkgOnInit[nPkg] = ::GetTickCount();
#endif	// PKG_LOAD_TIME
		pPackageEnum = (CPackage*) m_packages.GetNext(pos);
		if ((pPackageEnum->m_flags & PKS_INIT) != 0)
		{
			if (!pPackageEnum->OnInit())
			{
				// The package whose initialization failed should provide a meaningful
				// error message explaining with more detail WHY initialization failed.

				// The shell simply issues a more generic message just in case this is
				// the only message the user sees before the application exits.

				// due to Win95J limit, error message is broken into 3 resource strings.
				CString strErrorMsg, strErrorMsg2, strErrorMsg3;
				VERIFY(strErrorMsg.LoadString(IDS_PACKAGEINITFAILED));
				VERIFY(strErrorMsg2.LoadString(IDS_PACKAGEINITFAILED2));
				VERIFY(strErrorMsg3.LoadString(IDS_PACKAGEINITFAILED3));
				strErrorMsg += strErrorMsg2;
				strErrorMsg += strErrorMsg3;

				MsgBox(Error, strErrorMsg);
				theApp.m_pAutoApp->Zombify();
				theApp.m_pAutoApp->Destroy();
				pMainFrame->DestroyWindow();
				return FALSE;
			}
		}
#ifdef PKG_LOAD_TIME
		if( nPkg >=0 )
			dwaPkgOnInit[nPkg] = ::GetTickCount() - dwaPkgOnInit[nPkg];
		nPkg++;
#endif	// PKG_LOAD_TIME
	}

	// Should we load the typelibs now?  And if so, should we force
	//	full registration of them all?
	if (bForceFullRegistration ||
			ShouldRegisterTypeLibsOnStartup(cPackagesLoaded, bForceFullRegistration))
	{
			((CVshellPackage*) pPackage)->RegisterTypeLibsIfNecessary(bForceFullRegistration);
	}

	pMainFrame->m_keymap.GetReg();

	// Initialization for cached interfaces
	g_IdeInterface.Initialize();

	// This now depends on OnInit having been called in CVcppPackage
	LoadToolListSetting();

	// Figure out which commands should be repeatable and initialize
	// our table.
	InitRepeatableCommands();

	// Initialize our macro recording infrastructure

	CShellRecorder::m_pMacroRecorder = 0;
	theApp.FindInterface(IID_IMacroRecorder, reinterpret_cast<void**>(&CShellRecorder::m_pMacroRecorder));

	// The automation Application object needs to know whether the mainfrm
	//	was last in a maximized state before the mainfrm became invisible.
	//	It uses this when a controller then makes the mainfrm visible
	//	via Application.Visible = True.  CApplication can NOT just use
	//	theApp.m_bMaximizeApp, since this value becomes FALSE when the
	//	mainfrm is later made invisible.
	m_pAutoApp->m_bMaximizeApp = theApp.m_bMaximizeApp;

	if (!(m_bRunInvisibly && m_bInvokedCommandLine)) // REVIEW
	{
	// haven't loaded a project on the command line.
	pMainFrame->InitAfterRegistration();
	pMainFrame->DragAcceptFiles();

	// notify all packages that the dockmgr is inited
	theApp.NotifyPackages(PN_DOCKMGR_LOADED);

	// Show the MRU menuitems if the relevant recent file list was loaded.
	// This is done after InitAfterRegistration, because the command tables
	// get initialized in that call.
	if (m_pRecentFileList == NULL)
		pPackage->SetVisibleMenuItem(ID_FILE_MRU_FILE1, FALSE);
	if (m_pRecentProjectList == NULL)
		pPackage->SetVisibleMenuItem(ID_PROJ_MRU_FILE1, FALSE);

	}
	// now, finish up the command line processing by opening all files on the
	// command line.
	CString strArg;
	CStringArray astrDocumentNames;
	int iProj = -1;

	CString strFilter;
	CPtrArray paWorkspaceEditors;
	BuildFilterList(strFilter, &paWorkspaceEditors,
					CPackage::GODI_WORKSPACEOPEN);

	LPPROJECTWORKSPACE pProjSysIFace = g_IdeInterface.GetProjectWorkspace();

	CPartFrame::LockWorkspace(TRUE);

	// ensure UI disabled if running from command line before opening doc
	if (m_bRunInvisibly && m_bInvokedCommandLine) // REVIEW
	{
		g_AutomationState.DisableUI();
		g_AutomationState.SetSaveChanges(FALSE);
	}

	LPBUILDSYSTEM pBldSysIFace;
	if (FAILED(theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace)))
		pBldSysIFace = NULL;
	for (pos = m_argList.GetHeadPosition(); pos != NULL;)
	{
		strArg = m_argList.GetNext(pos);

		if (IsSwitch(strArg[0]))
		{
				// FUTURE :UNDONE [dans 10-Oct-1993]
				// need error here about bad switch
		}
		else
		{
			// leave a trail because open exe as proj from cmd line requires
			// us to find an occurence of this arg in the raw command line
			// in order to give the rest of the args to the child correctly.
			m_strCurArg = strArg;

			// Do the same existence check the FileDialog does.
			if (!FileExists(strArg))
			{
				// "File does not exist"
				ErrorBox(ERR_File_NoExist, (LPCTSTR)strArg);
				continue;
			}

			// Convert it to a long file name version
			CString strLongArg=strArg;

			if(!::GetActualFileCase(strLongArg))
			{
					strLongArg=strArg;
			}

			CDocument *pDoc = OpenDocumentFile(strLongArg);
			if (pDoc == NULL)
					continue;

			astrDocumentNames.Add(pDoc->GetPathName());

			// check to see if it is a proj doc, if so, if it is an
			// exe from the command line, it gets the rest of the args
			// and we are done here.

			if (pBldSysIFace != NULL && pProjSysIFace != NULL &&
					(pProjSysIFace->IsWorkspaceDocument(pDoc) == S_OK))
			{
				iProj = astrDocumentNames.GetSize() - 1;
				int iType;
				pBldSysIFace->GetBuilderType(ACTIVE_BUILDER, &iType);
				if (iType == ExeBuilder)
				{
						if(!strTarg.IsEmpty())
								pBldSysIFace->SetRemoteTargetFileName(ACTIVE_BUILDER, strTarg);

						break;	// get out of the loop, we are done
				}
			}
		}
	}

	if (pBldSysIFace != NULL)
		pBldSysIFace->Release();

	if (pProjSysIFace != NULL)
	{
		if (pProjSysIFace->IsWorkspaceInitialised() == S_OK)
		{
			m_bEnableWorkspaceLoad = FALSE;
			bCreateNewProject = FALSE;
		}
	}

	// leave the arg list empty as a signal we are done with command
	// line processing
	m_strCurArg.Empty();
	m_argList.RemoveAll();

	CPartFrame::LockWorkspace(FALSE);

	if (bCreateNewProject)
		m_bEnableWorkspaceLoad = FALSE;

	// Unless it has been disabled (e.g. by JIT, specifying a project on the
	// command line, or by a package like RLTools), load the last workspace...
	if (m_bEnableWorkspaceLoad)
	{
		ASSERT(!m_jit.GetActive());
		ASSERT(iProj == -1);

		LoadLastProject();
	}

	// Move the files loaded from the command line to the front of the workspace [marklam]
	// The list is stepped from the tail (1st file loaded) along the list until the loaded
	//	 project is found or the head is reached. The list is then stepped in reverse order
	//	 placing each document's window on top of the others.
	// The document from the head of the list (last loaded) is then placed at the bottom, and
	//	 all the document windows between that and the active project are placed on top of that.
	// Any document could have disappeared, so names are used in the list which are looked up.

	if (astrDocumentNames.GetSize() != 0)
	{
		pMainFrame->m_pWndMDIClient->ShowWindow(SW_HIDE);

		CDocTemplate* pOpenTemplate;
		CDocument* pOpenDoc;
		CPartFrame* pFrame;

		HWND hwndAfter = HWND_TOP;
		HDWP hdwp = BeginDeferWindowPos(astrDocumentNames.GetSize());

		for (int i = 0; i < astrDocumentNames.GetSize(); i++)
		{
			pFrame = GetOpenFrame(astrDocumentNames[i],
					pOpenTemplate, pOpenDoc);

			if (pFrame != NULL)
			{
				if ((pFrame->GetStyle() & WS_VISIBLE) == 0)
				{
						pFrame->ShowWindow(SW_SHOWNOACTIVATE);
						hdwp = DeferWindowPos(hdwp, pFrame->m_hWnd,
								hwndAfter, 0, 0, 0, 0,
								SWP_NOSIZE | SWP_NOMOVE);
				}

				pOpenDoc->UpdateFrameCounts();
				pFrame->OnUpdateFrameTitle(TRUE);

				pFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE);

				hwndAfter = pFrame->m_hWnd;
			}

			if (i == iProj)
				hwndAfter = HWND_BOTTOM;
		}

		ASSERT(hdwp != NULL);	// Could not move docs due to lack of system resources
		if (hdwp != NULL)
			VERIFY(EndDeferWindowPos(hdwp));

		if (theApp.m_bMaximizeDoc)
		{
			pFrame = (CPartFrame*) pMainFrame->MDIGetActive();
			if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
				pFrame->ShowWindow(SW_SHOWMAXIMIZED);
		}

		pMainFrame->m_pWndMDIClient->ShowWindow(SW_SHOWNA);
		pMainFrame->m_pWndMDIClient->UpdateWindow();
	}

	// If -EXECUTE Command was passed on the command line, then we have to
	//	execute that command
	if (!strCommand.IsEmpty())
	{
		// Convert strCommand name to command ID
		UINT nIDCommand;
		if (theCmdCache.GetCommandID(strCommand, &nIDCommand))
		{
			m_pMainWnd->PostMessage(WM_COMMAND, nIDCommand, 0L);
		}
		else
		{
			ErrorBox(IDS_ERR_BAD_CMD, (LPCTSTR)strCommand);
		}
		m_fRunningMacro = TRUE;
		strCommand.Empty();
	}

	// Unload any package bitmaps that were loaded to help us build the menu structure or toolbars
	// These won't be referenced until customization begins (if ever), so they're just wasted memory
	UnloadPackageBitmaps();

	// NOTE: This needs to be done last, since any message boxes or
	//				 releasing of the CPU after this will cause us to crash.
	// FUTURE: move to MSVCSRC in OnInitPackage (which is last part of init)...
	if (m_jit.GetActive())
	{
		// Post a message to ourselves, so that
		// when the IDE is done initializing, it will
		// simulate an F5 (Go) command from the user.
		m_pMainWnd->PostMessage(
				WM_COMMAND,
				GET_WM_COMMAND_MPS(IDM_RUN_JIT_GO, 0, 0));
	}

	// If /NEWPROJ was specified on the command line and we did not load a
	// project for some reason already, create a new project now...
	if (bCreateNewProject)
	{
		CDocTemplate* pTemplate = GetTemplate(CLSID_WorkspaceDocument);
		if (pTemplate != NULL)
			pTemplate->OpenDocumentFile(NULL);
	}

#ifdef PKG_LOAD_TIME
	dwInitInstance = ::GetTickCount() - dwInitInstance;
	if(s_bLoadTime)
	{
		FILE *outfile = fopen("time.log", "w");

		DWORD dwPkgsLoadLibraryTime = 0;
		DWORD dwPkgsTypeLibTime = 0;
		DWORD dwPkgsInitPackageTime = 0;
		DWORD dwPkgsOnInit = 0;
		DWORD dwPkgsTotal = 0;

		fprintf(outfile, "Package   \tLoadLib\tTypeLib\tInitPkg\tOnInit\tTotal\n");
		for(i=0; i<nPkg; i++) {
			DWORD dwPkgTotal = dwaPkgLoadLibraryTime[i] + dwaPkgTypeLibTime[i] + dwaPkgInitPackageTime[i] + dwaPkgOnInit[i];
			fprintf(outfile, "%-10s\t%d\t%d\t%d\t%d\t%d\n", straPkgName[i],
					dwaPkgLoadLibraryTime[i], dwaPkgTypeLibTime[i], dwaPkgInitPackageTime[i], dwaPkgOnInit[i], dwPkgTotal );
			dwPkgsLoadLibraryTime += dwaPkgLoadLibraryTime[i];
			dwPkgsTypeLibTime	  += dwaPkgTypeLibTime[i];
			dwPkgsInitPackageTime += dwaPkgInitPackageTime[i];
			dwPkgsOnInit		  += dwaPkgOnInit[i];
			dwPkgsTotal 		  += dwPkgTotal;
		}
#ifdef _DEBUG
		fprintf(outfile, "devshld   \t0\t0\t0\t0\t%d\n", dwInitInstance - dwPkgsTotal);
#else
		fprintf(outfile, "devshl    \t0\t0\t0\t0\t%d\n", dwInitInstance - dwPkgsTotal);
#endif
		fprintf(outfile, "Total     \t%d\t%d\t%d\t%d\t%d\n",
				dwPkgsLoadLibraryTime, dwPkgsTypeLibTime, dwPkgsInitPackageTime, dwPkgsOnInit, dwInitInstance);
		fclose(outfile);
	}
#endif	// PKG_LOAD_TIME

#ifdef _MTTF
	// Run	MTTF only if the MTTF/Enable regkey is set to 1
	if(IsMTTFEnabled())
	{
	  InitMTTF( );
	}
#endif


	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// Just-In-Time debugging support
//
// FUTURE: see if this can be moved to MSVCSRC...  Note: for perf-
// ormance reasons, we might need to keep it here; in the case where we find
// some other instance to pass things off to, we don't want to waste time on
// loading MSVCSRC...  Perhaps this stuff can go in MSVC.EXE!
//
static BOOL InitJIT()
{
        // Note, this function returns non-zero if there is an error
        // loading JIT (command line arg errors).  This value is a resource
        // string ID so that we can display an error.  We cannot use
        // AfxMessageBox since we have no main frame yet.
        UINT errJit = ErrGetJITInfo(&theApp.m_argList, &theApp.m_jit);

        // Error with JIT command line.  Display error and bail
        if (errJit != 0)
        {
                CString strErr;
                CString strApp;

                strErr.LoadString(errJit);
                strApp.LoadString(AFX_IDS_APP_TITLE);
                ::MessageBox(NULL, strErr, strApp, MB_OK | MB_ICONEXCLAMATION);
                return FALSE;
        }

        // Note, we're checking GetEvent() here because GetActive() will
        // be set even if the user just did "msvc -p xxx", which is regular
        // attach-to-active-process debugging rather than just-in-time
        // debugging.
        if (theApp.m_jit.GetActive() && theApp.m_jit.GetEvent() && FFindOtherJitDebugger(&theApp.m_jit))
        {
                // We could return FALSE instead of calling exit() ourselves,
                // but that has the mildly undesirable effect of causing Sushi
                // to return a nonzero exit code.
                exit(0);
        }

        return TRUE;
}

BOOL IsMSVCWindow(HWND hwnd)
{
        // Is the passed HWND an MSVC main frame?
        char szClass[256];
        return (GetClassName(hwnd, szClass, sizeof(szClass)) != 0 &&
                lstrcmp(_T("MSVCDBG50"), szClass) == 0);
}

// JitEnumWindows
//
// EnumWindows callback function to look for a just-in-time
// debugger.
//
// NOTE: This routine should NOT put up any error messages!
// When it gets called, very little if any initialization has
// been done.  If anything goes wrong, it should just return
// TRUE (meaning to continue enumerating).

static BOOL CALLBACK JitEnumWindows(HWND hwnd, LPARAM lparam)
{
        static UINT     WU_QUERYJIT = RegisterMessage("QueryJit");
        static UINT     WU_BEGINJIT = RegisterMessage("BeginJit");
        CJit *          pjit = (CJit *) lparam;
        HANDLE          hProcess;
        HANDLE          hEventDup;
        DWORD           pidMSVC;
        DWORD           dwSendResult = 0;

        // Is the passed HWND an MSVC main frame?
        if (!IsMSVCWindow(hwnd))
                return TRUE;    // no: keep enumerating windows

        // Does the other process want to be the just-in-time debugger?
        if (!SendMessageTimeout(hwnd, WU_QUERYJIT, pjit->GetPid(), 0,
                SMTO_ABORTIFHUNG, 2000, &dwSendResult)
                || !dwSendResult)
        {
                return TRUE;    // no: keep enumerating windows
        }

        if (!GetWindowThreadProcessId(hwnd, &pidMSVC))
        {
                // perhaps the window just died or something
                return TRUE;    // okay fine, keep enumerating windows
        }

        // Try to get a handle to the other process
        if (!(hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pidMSVC)))
        {
                // OpenProcess failed, don't know why (security perhaps)
                return TRUE;    // okay fine, keep enumerating windows
        }

        if (!DuplicateHandle(GetCurrentProcess(), pjit->GetEvent(), hProcess,
                &hEventDup, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
                // couldn't duplicate handle for some strange reason
                return TRUE;    // okay fine, keep enumerating windows
        }

        if (!SendMessage(hwnd, WU_BEGINJIT, pjit->GetPid(), (LPARAM) hEventDup))
        {
                // the other process rejected the WU_BEGINJIT for some reason
                return TRUE;    // okay fine, keep enumerating windows
        }

        // The other process has now begun debugging.  We can close our copy
        // of the event handle and disable just-in-time debugging.
        CloseHandle(pjit->GetEvent());

        return FALSE;           // stop enumerating windows
}

// FFindotherJitDebugger
//
// See if there are command line arguments for just-in-time
// debugging, and if so, see also if there is another copy of
// MSVC already running which would like to be the debugger
// for the specified process.  If these conditions are met,
// we will tell that other copy of MSVC to begin debugging,
// and then this new copy will just exit.
//
// NOTE: This routine should NOT put up any error messages!
// When it gets called, very little if any initialization has
// been done.  If anything goes wrong, it should just return
// FALSE.

static BOOL FFindOtherJitDebugger(CJit* pjit)
{
        return !EnumWindows(JitEnumWindows, (LPARAM) pjit);
}

static UINT ErrGetJITInfo(CStringList* pArgList, CJit* pjit)
{
        CPath           path;
        CString         strExe = ".exe";
        POSITION        pos, posCur;

        // "/P xxxxx [/E yyyyy]" is for just-in-time debugging:
        // xxxxx is the (decimal) Process Id, and yyyyy is the
        // (decimal) event handle (see NT's post-mortem debugging
        // documentation).  (This command-line syntax was chosen
        // because it is consistent with NTSD and DRWTSN32.)

        // iterate through args
        //
        pos = pArgList->GetHeadPosition();
        while ((posCur = pos) != 0)
        {
                CString strArg = pArgList->GetNext(pos);

                if (IsSwitch(strArg[0]) && strArg.GetLength() >= 2)
                {
                        BOOL    fRemoveArg = FALSE;

                        TCHAR   tch = strArg[1];
                        CString strT = strArg.Right(strArg.GetLength() - 1);
                        CharUpperBuff ( &tch, 1 );

                        switch (tch)
                        {
                        case 'E':
                                if (strT.GetLength() == 1 || _istdigit(strT[1]))
                                {
                                        // "/P xxxxx [/E yyyyy]" is for Application Error
                                        // (post-mortem) debugging: xxxxx is the (decimal)
                                        // Process Id, and yyyyy is the (decimal) event handle
                                        // (see post-mortem debugging docs).  (This syntax was
                                        // chosen because it is consistent with NTSD and DRWTSN32.)

                                        // always remove the arg--we give the errors
                                        fRemoveArg = TRUE;

                                        // /E eventid already specified?
                                        if( pjit->GetEvent() != 0 )
                                        {
                                                // Two '/E' parameters
                                                return ERR_Switch_TwiceE;
                                        }

                                        if (strT.GetLength() == 1)
                                        {
                                                // remove this arg and get the next one
                                                if (pos != NULL)
                                                {
                                                        pArgList->RemoveAt(posCur);
                                                        posCur = pos;
                                                        strT = pArgList->GetNext(pos);
                                                }
                                                else
                                                {
                                                        return ERR_SlashE_NoEventNumber;
                                                }
                                        }
                                        else
                                        {
                                                // skip past 'E' to get to event id
                                                strT = strT.Right(strT.GetLength() - 1);
                                        }

                                        const TCHAR * ptch = strT;

                                        // Important: the use of _tcstoul() is good because
                                        // it allows the user to enter either a decimal
                                        // number or a hex number (if preceded by "0x")
                                        HANDLE hEvent = (HANDLE) _tcstoul(ptch, (char **)&ptch, 0);
                                        if (hEvent == 0 || *ptch != '\0')
                                        {
                                                return ERR_SlashE_NoEventNumber;
                                        }

                                        // set JIT elements
                                        pjit->SetEvent(hEvent);
                                }
                                break;

                        case 'P':
                                if (strT.GetLength() == 1 || _istdigit(strT[1]))
                                {
                                        // "/P xxxxx [/E yyyyy]" is for Application Error
                                        // (post-mortem) debugging: xxxxx is the (decimal)
                                        // Process Id, and yyyyy is the (decimal) event handle
                                        // (see post-mortem debugging docs).  (This syntax was
                                        // chosen because it is consistent with NTSD and DRWTSN32.)

                                        // always remove the arg--we give the errors
                                        fRemoveArg = TRUE;

                                        if (pjit->GetPid() != 0)
                                        {
                                                // Two '/P' parameters
                                                return ERR_Switch_TwiceP;
                                        }

                                        if (strT.GetLength() == 1)
                                        {
                                                // remove this arg and get the next one
                                                if (pos != NULL)
                                                {
                                                        pArgList->RemoveAt(posCur);
                                                        posCur = pos;
                                                        strT = pArgList->GetNext(pos);
                                                }
                                                else
                                                {
                                                        return ERR_SlashP_NoPidNumber;
                                                }
                                        }
                                        else
                                        {
                                                // skip past 'P' to get to pid
                                                strT = strT.Right(strT.GetLength() - 1);
                                        }

                                        const TCHAR * ptch = strT;

                                        // Important: the use of _tcstoul() is good because
                                        // it allows the user to enter either a decimal
                                        // number or a hex number (if preceded by "0x")
                                        DWORD pid = (DWORD) _tcstoul(ptch, (char **)&ptch, 0);
                                        if (pid == 0 || *ptch != '\0')
                                        {
                                                return ERR_SlashP_NoPidNumber;
                                        }

                                        // set JIT elements
                                        pjit->SetPid(pid);
                                        VERIFY(pjit->FSetPath("UNKNOWN.EXE", FALSE));
                                }
                                break;
                        }

                        if (fRemoveArg)
                                                        pArgList->RemoveAt(posCur);
                }
                else
                {
#if 0
                                        // if we see an exe on the command line, the entire remainder
                                        // of the command line consists of the command line to be
                                        // passed to that exe, rather than options for MSVC.
                                        //
                                        if (path.Create(strArg) &&
                                                strExe.CompareNoCase(path.GetExtension()) == 0)
                                        {
                                                break;
                                        }
#else // 0
                                        // If the argument is NOT a command line-switch, stop processing
                                        // immediately since there may be an exe/makefile which the
                                        // user is passing args to.  (Basically, they're not intended
                                        // for JIT!)
                                        if (!IsSwitch(strArg[0]))
                                        {
                                                break;
                                        }
#endif // 0
                }
        }

        // Did user specify /E without /P?

        if (pjit->GetEvent() && !pjit->GetPid())
        {
                        return ERR_Missing_Jit_SwitchP;
        }

        // If /P specified, activate just-in-time debugging
        if (pjit->GetPid() != 0)
        {
                        pjit->SetActive(TRUE);
                        theApp.m_bEnableWorkspaceLoad = FALSE;
        }

        return 0;
}

int CTheApp::ExitInstance()
{
        while (!m_packets.IsEmpty())
        {
                CPacket* pPacket = (CPacket*) m_packets.RemoveHead();
                delete pPacket;
        }

        g_IdeInterface.Terminate();

        {
                if (CShellRecorder::m_pMacroRecorder)
                        CShellRecorder::m_pMacroRecorder -> Release();
        }

        // This is a hack.  W/o this we assert in debug mode while
        // unloading the edit package because too many references are
        // being held
        if (!m_bInvokedCommandLine)
        {
            UnloadPackages();


        // uninit Office's StdComponentManager library after all packages that
        //      might be using it have been unloaded. (this is separate from calling
        //      TermStdCompMgr because this releases the library, not the manager
        //      itself -- equivalent to DLL_PROCESS_DETACH cleanup).
#ifndef NOIPCOMP
            CMLibTerm();
#endif
        }

#ifdef _MTTF
        // Run  MTTF only if the MTTF/Enable regkey is set to 1
        if(IsMTTFEnabled())
        {
          MTTFOnExit( );
        }
#endif

#ifdef _CMDLOG
        CmdLogOnExit( );
#endif
	if (m_hLogFile != NULL)
	{
		CloseHandle(m_hLogFile);
		m_hLogFile = NULL;
	}

        // If invoked via the command line then pause a bit so the
        // user can view the output results
        if (m_bInvokedCommandLine && g_bConsoleCreated)
        {
            Sleep(10000);
        }

        return m_msgCur.wParam; // Returns the value from PostQuitMessage
}

#ifdef _MTTF

/////////////////////////////////////////////////////////////////////////////
// Mean time to failure hanlding

// FFindAnotherMSVC : Look for another copy of msvc[d].exe in the system.
// Returns TRUE if one is found, FALSE otherwise.

static BOOL CALLBACK MTTFEnumWndProc(HWND hwnd, LPARAM lParam)
{
        ASSERT(lParam == 0);

        return !IsMSVCWindow(hwnd);
}

static BOOL FFindAnotherMSVC( )
{
        return !EnumWindows(MTTFEnumWndProc, 0);
}

static char BASED_CODE szMTTF[] = "MTTF" ;
static char BASED_CODE szAbnormalTermination[] = "AbnormalTermination" ;
static char BASED_CODE szTimeSinceLastFailure[] = "TimeSinceLastFailure" ;
static char BASED_CODE szTimeCurSession[] = "TimeCurSession" ;
static char BASED_CODE szVersion[] = "VersionNo";
static char BASED_CODE szTestRun[] = "AUTO" ;
static char BASED_CODE szDoMTTF[] = "EnableMTTF";
static char BASED_CODE szMTTFEnable[] = "Enable";
static int  tickMinutes = 5;  // Update info every 5 minutes.
static int  timeUpdate = tickMinutes * 60 * 1000  ; // convert to milliseconds.

static int      terminatedNormally = 0 ;
static int      terminatedAbnormally = 1 ;
static int  noPrevEntry = 2 ;


BOOL IsMTTFEnabled(void)  // return true if the MTTF regkey Enable == 1
{
  if(GetRegInt(szMTTF,szMTTFEnable,0) == 1)
   return TRUE;
  return FALSE;
}


VOID CALLBACK MTTFTimerProc(HWND, UINT, UINT, DWORD)
{

        static BOOL  bCurrentlyInTimerCallBack = FALSE;

        if(bCurrentlyInTimerCallBack) return;
        bCurrentlyInTimerCallBack = TRUE;

        //  AFX_MANAGE_STATE(AfxGetModuleState());
        // IStudio bug #5479
        // when one of our COM servers has modal UI displayed and the
        // timer callback is fired, we need to get the app's module
        // state and not the active module's module state.
        AFX_MANAGE_STATE(AfxGetAppModuleState());

        ASSERT( _tcsicmp(_T(theApp.m_pszAppName), _T("Microsoft Visual C++")) == 0 );


        ASSERT( ((CTheApp *)AfxGetApp( ))->m_bMTTF );

        int timeSinceLastFailure = GetRegInt(szMTTF, szTimeSinceLastFailure, 0);
        int timeCurSession = GetRegInt(szMTTF, szTimeCurSession,0);

        ASSERT(timeCurSession <= timeSinceLastFailure);

        // Increment the counts by 1 to indicate a timer tick.

        VERIFY( WriteRegInt(szMTTF, szTimeCurSession, timeCurSession + 1 ));
        VERIFY( WriteRegInt(szMTTF, szTimeSinceLastFailure, timeSinceLastFailure + 1));

        bCurrentlyInTimerCallBack = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CMTTFDialog dialog

class CMTTFDialog : public CDialog
{
        // Construction
public:
        CMTTFDialog(CWnd* pParent = NULL);   // standard constructor
        ~CMTTFDialog(void);                                     // dtor
        // Dialog Data
        //{{AFX_DATA(CMTTFDialog)
        enum { IDD = IDD_MTTF };
        // NOTE: the ClassWizard will add data members here
        //}}AFX_DATA

        BOOL m_bExpected;
        BOOL m_bFeatureSelected;
        BOOL m_bFailureCauseSelected;
        CListBox areaCombo ;
        int areaIndex ;

        CEdit   infoEdit ;
        enum {LIMIT_INFO_TEXT = 512 };
        CString strInformation ;

        enum CauseOfFailure { mttfAssertion, mttfException, mttfHang, mttfOther,mttfRepro,mttfError,mttfOrdinal };
        CauseOfFailure failureType ;
        CString strFailureType;


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CMTTFDialog)
protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation
protected:

        // Generated message map functions
        //{{AFX_MSG(CMTTFDialog)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void OnSelchangeList1();
        afx_msg void OnRadioAssert();
        afx_msg void OnRadioException();
        afx_msg void OnRadioHang();
        afx_msg void OnRadioOther();
        afx_msg void OnRadioRepro();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
private:
        void AttemptEnableOKFromRadioButton(int nID);
};

// CMTTFDialog dialog


CMTTFDialog::CMTTFDialog(CWnd* pParent /*=NULL*/)
: CDialog(CMTTFDialog::IDD, pParent),m_bExpected(FALSE),
m_bFailureCauseSelected(FALSE),m_bFeatureSelected(FALSE)
{
        //{{AFX_DATA_INIT(CMTTFDialog)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
}


CMTTFDialog::~CMTTFDialog(void)
{
        infoEdit.Detach();
        areaCombo.Detach();
}


void CMTTFDialog::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CMTTFDialog)
        // NOTE: the ClassWizard will add DDX and DDV calls here
        //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMTTFDialog, CDialog)
//{{AFX_MSG_MAP(CMTTFDialog)
ON_LBN_SELCHANGE(IDC_FEATURE, OnSelchangeList1)
ON_BN_CLICKED(IDC_ASSERT, OnRadioAssert)    // assertion
ON_BN_CLICKED(IDC_EXCEPTION, OnRadioException)    // exception
ON_BN_CLICKED(IDC_HANG, OnRadioHang)    // hang
ON_BN_CLICKED(IDC_OTHER, OnRadioOther)    // other
ON_BN_CLICKED(IDD_MTTF, OnRadioRepro)    // repro
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTTFDialog message handlers


static char *aszFeature[] =
{
        "Accel Edit",
        "Automation",
        "ActiveX Layout Control",
        "Binary Edit",
        "Browser",
        "Build System",
        "ClassView",
        "Code Edit",
        "Component Gallery",
        "Config",
        "DataTools (daVinci)",
        "Debugger",
        "Denali",
        "Designer Controls",
        "Dialog Edit",
        "File I/O",
        "Global",
        "Image Edit",
        "InfoViewer",
        "Menu Edit",
        "Menus / Toolbars",
        "Multimedia Tools",
        "Other",
        "Project Edit",
        "Res Browser",
        "Res Symbols",
        "Script Editor",
        "Shell",
        "SQL Data Thang",
        "SQL Debugging",
        "String Edit",
        "Trident",
        "Unknown",
        "Version Edit",
        "Web Project Editor",
        "WebScope (daVinci)",
        "WizardBar",
        "Wizards"
};

static char *aszCause[CMTTFDialog::mttfOrdinal] =
{
        "Assertion",
        "Exception",
        "Hang",
        "Other",
        "Repro",
        "Error"
};



BOOL CMTTFDialog::OnInitDialog()
{
        // attach combobox, and add elements
        areaCombo.Attach(GetDlgItem(IDC_FEATURE)->GetSafeHwnd()) ;
        int cItems = sizeof(aszFeature)/sizeof(aszFeature[0]) ;
        for ( int i = 0 ; i < cItems ; i++ ) areaCombo.AddString(aszFeature[i]);

        // Limit the text in the information edit box.
        infoEdit.Attach(GetDlgItem(IDC_INFORMATION)->GetSafeHwnd());
        infoEdit.LimitText(LIMIT_INFO_TEXT);

        // disable OK until user selects failure type and area
        GetDlgItem(IDOK)->EnableWindow( FALSE );

        return CDialog::OnInitDialog( );
}

void CMTTFDialog::OnOK()
{

        m_bExpected = FALSE ;

        // get failure type
        //  int nFailureType = GetCheckedRadioButton(IDC_ASSERT,IDC_OTHER);
        int nFailureType = GetCheckedRadioButton(IDD_MTTF,IDC_OTHER);
        switch(nFailureType)
        {
    case IDC_ASSERT:    failureType = mttfAssertion; break;
    case IDC_EXCEPTION: failureType = mttfException; break;
    case IDC_HANG:      failureType = mttfHang; break;
    case IDC_OTHER:     failureType = mttfOther; break;
    case IDD_MTTF:      failureType = mttfRepro; break;
    default:            failureType = mttfError; break;
        }

    // get failure area: Which part of the product caused the problem.
        areaIndex = areaCombo.GetCurSel();
        ASSERT(areaIndex != CB_ERR);
        strFailureType = aszFeature[areaIndex];

    // get the additional information that the user might have typed in.
        infoEdit.GetWindowText(strInformation);

    // call parent class
        CDialog::OnOK();
}


void CMTTFDialog::OnCancel()
{
        return;   // force the user to input a failure type and feature
}

void CMTTFDialog::OnSelchangeList1()
{
        m_bFeatureSelected = TRUE;
        if(m_bFailureCauseSelected)                   // is failure type selected
                GetDlgItem(IDOK)->EnableWindow( TRUE );      // enable OK to exit dialog
}

void CMTTFDialog::AttemptEnableOKFromRadioButton(int nRadioButtonID)
{                                               // is failure type selected
        if(((CButton*)GetDlgItem(nRadioButtonID))->GetCheck() == 1)
        {
                m_bFailureCauseSelected      = TRUE;
                if(m_bFeatureSelected)                      // is failure area selected
                        GetDlgItem(IDOK)->EnableWindow( TRUE );    // enable OK to exit dialog
        }
}

void CMTTFDialog::OnRadioAssert()
{
        AttemptEnableOKFromRadioButton(IDC_ASSERT);
}

void CMTTFDialog::OnRadioException()
{
        AttemptEnableOKFromRadioButton(IDC_EXCEPTION);
}

void CMTTFDialog::OnRadioHang()
{
        AttemptEnableOKFromRadioButton(IDC_HANG);
}
void CMTTFDialog::OnRadioOther()
{
        AttemptEnableOKFromRadioButton(IDC_OTHER);
}

void CMTTFDialog::OnRadioRepro()
{
        AttemptEnableOKFromRadioButton(IDD_MTTF);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void TransportMTTFInfo( const CMTTFDialog *pDlg, int elapsedTime,
                                                          int timeSinceFailure, const CString& strVerNo)
{
        // Send the MTTF information over the net to the database.
        // Just use a hard-coded file name as this is completely
        // internal.
#if defined(TARGMAC68K) || defined(TARGMACPPC)
        //        static char szLogFileName[] = "\\\\dtqalogs\\db\\vcemttf\\mttf.dat";
        static char szLogFileName[] = "\\\\lang1\\VCQALOGS\\mttfmac.dat";
        static char szLocalLogFileName[] = "mttfmac.dat";
#else
        //        static char szLogFileName[] = "\\\\dtqalogs\\db\\mttf\\mttf.dat";
        static char szLogFileName[] = "\\\\lang1\\VCQALOGS\\mttf.dat";
        static char szLocalLogFileName[] = "mttf.dat";
#endif
        const int maxRetries = 1;
        HANDLE hFile ;

        if ( pDlg->m_bExpected )
                return ; // The failure was expected => no log entry.

        {

                HWND hwnd;

                hwnd = ::CreateWindow(
                        "Button",  // pointer to registered class name
                        "Looking for MTTF Log File on the Network...",     // pointer to window name
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,     // window style
            (::GetSystemMetrics(SM_CXSCREEN) - 340)/2,    // horizontal position of window
            (::GetSystemMetrics(SM_CYSCREEN)-100)/2,        // vertical position of window
            340,   // window width
            100,       // window height
            ::GetDesktopWindow(),  // handle to parent or owner window
            NULL,      // handle to menu or child-window identifier
            AfxGetInstanceHandle(),        // handle to application instance
            NULL       // pointer to window-creation data
                        );

        if(hwnd != NULL)
        {
                        VERIFY(::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW));
                        VERIFY(::UpdateWindow(hwnd));
        }
                //Sleep(10000);

        //try to open file on network drive
        if ( (hFile = CreateFile(szLogFileName,GENERIC_READ|GENERIC_WRITE,
                        FILE_SHARE_READ,NULL,OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,NULL)
                        ) == INVALID_HANDLE_VALUE )
        {  // try to open file on local drive
                        if(hwnd !=NULL) ::SetWindowText(hwnd, "Looking for MTTF Log File on a Local Drive...");

                        CString szFullPath;
                        LPTSTR  szFullPathBuffer = szFullPath.GetBufferSetLength(MAX_PATH);
                        GetModuleFileName(NULL, szFullPathBuffer, (MAX_PATH-(strlen(szLocalLogFileName)+1)));
                        szFullPath.ReleaseBuffer();
                        int nEndPos = szFullPath.ReverseFind('\\');
                        CString szFileName(szFullPath.Left(nEndPos + 1) + szLocalLogFileName);

                        if ( (hFile = CreateFile(szFileName,GENERIC_READ|GENERIC_WRITE,
                                FILE_SHARE_READ,NULL,OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,NULL)
                                ) == INVALID_HANDLE_VALUE )
                        {
                                return;  // failed so just bail
                        }

                        if(hwnd !=NULL)
                        {
                                ::SetWindowText(hwnd, "Logging MTTF data to Local Drive...");
                                Sleep(2000);
                        }
        }
        else
        {
                        if(hwnd !=NULL)
                        {
                                ::SetWindowText(hwnd, "Logging MTTF data to the Network...");
                                Sleep(2000);
                        }
        }
                if(hwnd != NULL) ::DestroyWindow(hwnd);
        }

        // these are commented out for the change above and need to be left in if the create window code from above is removed
        //        for (int i = 0; i < maxRetries ; i++ )
        //        {
        //                if ( (hFile = CreateFile(szLogFileName,GENERIC_READ|GENERIC_WRITE,
        //                                                                FILE_SHARE_READ,NULL,OPEN_ALWAYS,
        //                                                                FILE_ATTRIBUTE_NORMAL,NULL)
        //                          ) != INVALID_HANDLE_VALUE )
        //                {
        //                        break;
        //                }
        //                Sleep(300); // wait 0.3 seconds if it failed.
        //        }
        //        if ( i == maxRetries )
        //                return ; // No luck couldn't open the file.

        // Start writing from the end of the file.
        SetFilePointer(hFile,0,NULL,FILE_END);

        // Write out the relevant information to the file in the
        // following format.
        // 10/6/93,2.00.3278,SANJAYS2,Time Since last failure,
        // time in last session,Cause,Feature,"Comments"
        const DWORD MAX_SIZE = 512;
        char buffer[MAX_SIZE]; // Buffer to hold intermediate values.
        char logString[MAX_SIZE * 2]; // Complete string.
        DWORD  cBytesWritten ;
        //      char *pchQuote;                 // pointer used to strip quotes from additional info

        _strdate(logString); // get current date in mm/dd/yy form.
        lstrcat(logString,",");


        // Write out the version No next.
        lstrcat(logString,strVerNo);
        lstrcat(logString,",");

        // Computer name.
        GetComputerName(buffer,(LPDWORD)&MAX_SIZE);
        lstrcat(logString,buffer);
        lstrcat(logString,",");


        // time since last failure and elapsed time in last session.
        wsprintf(buffer,"%d,%d,",timeSinceFailure,elapsedTime);
        lstrcat(logString,buffer);

        //      Cause of failure.
        wsprintf(buffer,"%s,",aszCause[pDlg->failureType]);
        lstrcat(logString,buffer);

        // Component area where the failure happened.
        wsprintf(buffer,"%s,",aszFeature[pDlg->areaIndex]);
        lstrcat(logString,buffer);

        // Finally the string which holds the additional information.
        int ich;
        while ( (ich = pDlg->strInformation.Find( '\"' )) != -1 )// convert quotes to inquotes
                ((CString )pDlg->strInformation).SetAt( ich, '\'' );

        wsprintf(buffer,"\"%.150s\"\n",pDlg->strInformation);
        lstrcat(logString,buffer);

        WriteFile(hFile,logString, lstrlen(logString),&cBytesWritten,NULL);

        // We are all done, close the file.
        CloseHandle(hFile);
}

// This constant should change for each version
static const TCHAR  * pctcMTTFSemName= "MTTFSem5.0";

static
BOOL
bIsOnlyInstanceRunning(void)
{
        HANDLE hSem;

        // Try to Create or open a named semaphore.
        hSem = CreateSemaphore(
        NULL,          // security attributes
        0,             // initial count
        1,             // maximum count
        pctcMTTFSemName); // semaphore name

        // Close handle and return FALSE if existing semaphore was opened.
        if (hSem != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
        {
                CloseHandle(hSem);
                return FALSE; // another instance is running
        }
        // note: This code depends on the OS to clean up the open semaphore when the program exits.
        return  TRUE ;
}


void CTheApp::InitMTTF( )
{
        CString str = GetRegistryKeyName( );

        // Special check to ensure that sniff test runs,
        // do not contribute to the MTTF numbers.
        // We just check to see if the last four chars
        // are the string specified by szTestRun.

        if ( str.Right(lstrlen(szTestRun)) == szTestRun )
                m_bMTTF = FALSE ;

        // If we have two copies of MSVC running simultaneously,
        // don't do the update stuff this time around.

        // determine if we are running
        //    if ( FFindAnotherMSVC( ) ) m_bMTTF = FALSE;

        if (! bIsOnlyInstanceRunning( ) ) m_bMTTF = FALSE;

#if 0   // fully re-enable MTTF for retail builds (temporary) [waltcr]
#ifndef _DEBUG

        // If this is a retail build and the szDoMTTF key is not set to true,
        // disable MTTF [stevesk]
        if( m_bMTTF )
        {
                const NOMTTF = 0;

                int doMTTF = GetRegInt( szMTTF, szDoMTTF, 0 );

                if( doMTTF == NOMTTF )
                        m_bMTTF = FALSE;
        }
#endif // _DEBUG
#endif // 0

        if ( m_bMTTF )
        {
                int bPrevEntry = GetRegInt(szMTTF, szAbnormalTermination, noPrevEntry);


                if ( bPrevEntry == terminatedAbnormally )
                {
                        // It didn't work the last time over.
                        // Put up the dialog box asking the user
                        // about the failure.
                        CMTTFDialog dlg ;

                        // NOTE: Chicago Build 86 has problems with certain dialog boxes.
                        // Stub this one out since we hit it all the time.
                        // VERIFY ( dlg.DoModal( ) == IDOK );
                        //#pragma message("Remove Chicago hack after Build 87.")
                        dlg.DoModal();

                        int timeCurSession = GetRegInt(szMTTF, szTimeCurSession, 0);
                        int timeSinceLastFailure = GetRegInt(szMTTF, szTimeSinceLastFailure, 0);

                        CString strVerNo = GetRegString(szMTTF, szVersion,"0.00.0000");

                        TransportMTTFInfo(&dlg, (timeCurSession * tickMinutes),
                                (timeSinceLastFailure * tickMinutes),strVerNo );

                        // If the failure was an unexpected one, then
                        // start over counting the timeSinceLastFailure.
                        if ( !dlg.m_bExpected )
                                VERIFY(WriteRegInt(szMTTF, szTimeSinceLastFailure, 0 ));
                }
                else if ( bPrevEntry == noPrevEntry )
                {
                        // Set the elapsed time without failure to 0 as well, if
                        // this is the first time an MTTF entry is being made.
                        VERIFY(WriteRegInt(szMTTF, szTimeSinceLastFailure, 0 ));
                }

                VERIFY(WriteRegInt(szMTTF, szAbnormalTermination, terminatedAbnormally));
                VERIFY(WriteRegInt(szMTTF, szTimeCurSession, 0));

                // Write the version no. of apstudio as a string to the registry.
                {
                        TCHAR   rgchModName[_MAX_PATH];
                        DWORD   dwHandle, dwVerInfoSize;
                        unsigned int dwValueSize;
                        VOID *  pvVerInfo;
                        VOID *  pvValue;

                        GetModuleFileName(NULL, rgchModName, sizeof(rgchModName) / sizeof(TCHAR));

                        if (dwVerInfoSize = GetFileVersionInfoSize(rgchModName, &dwHandle))
                        {
                                pvVerInfo = new TCHAR[dwVerInfoSize];
                                if (GetFileVersionInfo(rgchModName, dwHandle, dwVerInfoSize, pvVerInfo))
                                {
                                        if (VerQueryValue(      pvVerInfo,
                                                "\\StringFileInfo\\040904b0\\FileVersion", // "\\StringFileInfo\\040904E4\\FileVersion",
                                                &pvValue,
                                                &dwValueSize))
                                        {
                                                wsprintf(str.GetBufferSetLength(64),
                                                        "%s", pvValue);
                                                str.ReleaseBuffer();
                                        }
                                }

                                delete [] pvVerInfo;
                        }
                }

                VERIFY(WriteRegString(szMTTF, szVersion, str));

                if ( (m_timerMTTF = ::SetTimer(NULL,0,timeUpdate, MTTFTimerProc)) == 0 )
                {
                        // Could not set a timer succesfully.
                        m_bMTTF = FALSE;
                }
        }
}

void CTheApp::MTTFOnExit ()
{
        if ( m_bMTTF )
        {
                // Update the registry entry to indicate that we succesfully
                // terminated sushi.
                VERIFY(WriteRegInt(szMTTF, szAbnormalTermination, terminatedNormally));

                ::KillTimer(NULL, m_timerMTTF);
        }
}


#endif // _MTTF


#ifdef _CMDLOG

static int  tickCmdLogMinutes = 5;  // Update info every 5 minutes.
static int  timeCmdLogUpdate = tickCmdLogMinutes * 60 * 1000; // convert to milliseconds.

VOID CALLBACK CmdLogTimerProc(HWND, UINT, UINT, DWORD)
{
        ASSERT(((CTheApp *)AfxGetApp())->m_bCmdLog);

        ((CTheApp *)AfxGetApp())->UpdateCmdLog();
}

void CTheApp::InitCmdLog()
{
        if(m_bCmdLog)
        {
                if ( (m_timerCmdLog = ::SetTimer(NULL, 0, timeCmdLogUpdate, CmdLogTimerProc)) == 0 )
                {
                        // Could not set a timer succesfully.
                        m_bCmdLog = FALSE;
                }
        }
}

void CTheApp::CmdLogOnExit()
{
        if(m_bCmdLog)
        {
                ::KillTimer(NULL, m_timerCmdLog);
                UpdateCmdLog();
        }
}

void CTheApp::UpdateCmdLog()
{
        // Write the command log to disk.  The log maintained in memory is a delta since
        // the last disk update.  The log is thus read from the disk first, and then written
        // back out with the deltas added.

        ASSERT(m_bCmdLog);

        CString szFullPath;
        LPTSTR  szFullPathBuffer = szFullPath.GetBufferSetLength(MAX_PATH);
        GetModuleFileName(NULL, szFullPathBuffer, MAX_PATH);
        szFullPath.ReleaseBuffer();
        int nEndPos = szFullPath.ReverseFind('\\');

        CString szFileName(szFullPath.Left(nEndPos + 1) + "command.log");

        CFileSaver saver(szFileName);
    if (!saver.CanSave())
                return;     // Don't bother if we can't save.

        const int               nMaxRetries = 5;
        CFile                   cmdLog;
        CFileException  Error;
        int                             i;

        // Command log file is opened in exclusive mode to lock out other instances temporarily.
        // FUTURE: Is 10 retries enough?  Too much?
        for (i = 0; !cmdLog.Open(szFileName, CFile::modeRead /*| CFile::shareExclusive*/, &Error) && (i < nMaxRetries); i++)
                Sleep(250);

        BOOL            bLogOpen = (i < nMaxRetries);
        CCmdMap         cmdTableTemp;
        CCmdMap*        pCmdTable = &cmdTableTemp;

        TRY
        {
                if(bLogOpen)
                {
                        CArchive arLoad(&cmdLog, CArchive::load | CArchive::bNoFlushOnDelete);
                        cmdTableTemp.Serialize(arLoad);
                        arLoad.Close();

                        for( POSITION pos = m_cmdTable.GetStartPosition(); pos != NULL; )
                        {
                                WORD            key;
                                CObject*        value;

                                m_cmdTable.GetNextAssoc( pos, key, value );
                                cmdTableTemp.Update(key, (DWORD)value);
                        }
                }
                else
                {
                        if (Error.m_cause != CFileException::fileNotFound)
                                return;
                        else
                                pCmdTable= &m_cmdTable;
                }
        }
        CATCH (CFileException, e)
        {
                cmdLog.Abort();
                return;
        }
        CATCH (CArchiveException, e)
        {
                cmdLog.Abort();
                pCmdTable = &m_cmdTable;
        }
        END_CATCH

        CFile tempLog;

        TRY
        {
                if(!tempLog.Open(saver.GetSafeName(), CFile::modeCreate | CFile::modeWrite ))
                        AfxThrowFileException(CFileException::generic);

                CArchive arStore(&tempLog, CArchive::store | CArchive::bNoFlushOnDelete);
                pCmdTable->Serialize(arStore);
                arStore.Close();
                tempLog.Close();

                if(bLogOpen)
                        cmdLog.Close();

                if(saver.Finish())
                        m_cmdTable.RemoveAll();
        }
        CATCH (CException, e)
        {
                if(e->IsKindOf(RUNTIME_CLASS(CFileException)) || e->IsKindOf(RUNTIME_CLASS(CArchiveException)))
                {
                        cmdLog.Abort();  // Need to unlock file.
                        tempLog.Abort();
                }
                else
                {
                        THROW_LAST();
                }
        }
        END_CATCH
}

void CCmdMap::Update(WORD wKey, DWORD nNewCount)
{
        CObject*        nOldCount;

        if(!Lookup(wKey, nOldCount))
                nOldCount = (CObject*)0;
        SetAt(wKey, (CObject*)((DWORD)nOldCount + nNewCount));
}

void CCmdMap::Serialize(CArchive& ar)
{
        ASSERT_VALID(this);

        CObject::Serialize(ar);

        if (ar.IsStoring())
        {
                ar << (WORD) m_nCount;
                if (m_nCount == 0)
                        return;  // nothing more to do

                POSITION        pos;
                WORD            key;
                CObject*        value;

                for( pos = GetStartPosition(); pos != NULL; )
                {
                        GetNextAssoc( pos, key, value );
                        ar << key;
                        ar << (DWORD)value;
                        //                      SetAt(key, (CObject*)0);
                }

                //              FUTURE: The code below is more efficient, but relies on the underlying implementation,
                //                                              which may change.  Do we want to use it anyway?
                //              ASSERT(m_pHashTable != NULL);
                //              for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
                //              {
                //                      CAssoc* pAssoc;
                //                      for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
                //                        pAssoc = pAssoc->pNext)
                //                      {
                //                              ar << pAssoc->key;
                //                              ar << (DWORD)pAssoc->value;
                //                              pAssoc->value = (CObject*)0;
                //                      }
                //              }
        }
        else
        {
                WORD wNewCount;
                ar >> wNewCount;

                WORD newKey;
                DWORD newValue;
                //              CObject* oldValue;
                while (wNewCount--)
                {
                        ar >> newKey;
                        ar >> newValue;
                        //                      if(Lookup(newKey, oldValue))
                        //                              newValue = (DWORD)oldValue + newValue;
                        SetAt(newKey, (CObject*)newValue);
                }
        }
}

#endif // _CMDLOG


/////////////////////////////////////////////////////////////////////////////
// Package handling

extern "C" typedef BOOL (FAR PASCAL* INITPACKAGEPROC)(HWND hShellWnd);

CStringList g_strlstPkgsLoaded;

#ifdef _DEBUG
LPCTSTR StringFromError(
                                                DWORD dwLastError   // last error value associated with API
                                                )
{
    LPTSTR MessageBuffer = NULL;
    DWORD dwBufferLength;
    dwBufferLength=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                dwLastError,
                LANG_NEUTRAL,
                (LPTSTR) &MessageBuffer,
                0,
                NULL);
        return MessageBuffer;
}
#endif

//      IsShellDefFileVersion
//              This function must be called in every package's rawDllMain()
//              to ensure it is compatible with the current shell.

static BOOL bPackageCompatible;
#ifdef _DEBUG
static BOOL bPackageChecked;
#endif

BOOL IsShellDefFileVersion(DWORD dwVersion)
{
        bPackageCompatible &= ((!theApp.m_bPkgChk) || (dwVersion == SHELL_DEFFILE_VERSION));
#ifdef _DEBUG
        // set flag indicating that package performed shell compatibility check
        bPackageChecked = TRUE;
#endif
        return(bPackageCompatible);
}

BOOL GetVersionOfFile(LPSTR, CString &, CString &);

static TCHAR szPackageExt[] = "pkg";

static BOOL LoadPackage(LPCTSTR szPackage, CStringList& strlstPkgsNot,
                                                BOOL bIDEDir, BOOL bRequired)
{
        // set package compatibility default
        bPackageCompatible = TRUE;

#ifdef _DEBUG
        // reset flag indicating that package performed shell compatibility check
        bPackageChecked = FALSE;
#endif
	
        // make sure we don't attempt to load a package more than once
        CString strPkg(szPackage);
        // lower case, file sys is case insensitive
        strPkg.MakeLower();

        if (strlstPkgsNot.Find(strPkg) != (POSITION)NULL)
        {
                TRACE("::LoadPackage: Not loading package %s\n", (const TCHAR *)strPkg);
                return -1;    // just skip, FALSE indicates 'missing file'!
        }
        if (g_strlstPkgsLoaded.Find(strPkg) != (POSITION)NULL)
        {
                TRACE("::LoadPackage: Already loaded package %s\n", (const TCHAR *)strPkg);
                return -1;    // already loaded, FALSE indicates 'missing file'!
        }

        TRACE("::LoadPackage: Loading package %s\n", (LPCTSTR) strPkg);

        HINSTANCE hInst;
        DWORD dwLastError;
        TCHAR szDLL[_MAX_PATH];

        if (!bIDEDir)
        {
                // Default packages
                wsprintf(szDLL, _TEXT("%s.%s"), (LPCTSTR) strPkg, szPackageExt);
        }
        else
        {
                LPCTSTR lpszExtensionDir = theApp.GetExeString(DefaultExtensionDir);
                if (lstrlen(lpszExtensionDir) > 0)
                {
                        TCHAR szIDEDir[_MAX_PATH];

                        {
                                TCHAR szPath [_MAX_PATH];
                                TCHAR szDir [_MAX_DIR];
                                GetModuleFileName(NULL, szPath, _MAX_PATH);
                                _tsplitpath(szPath, szIDEDir, szDir, NULL, NULL);
                                _tcscat(szIDEDir, szDir);
                        }

                        wsprintf(szDLL, _TEXT("%s%s\\%s.%s"), szIDEDir,
                                lpszExtensionDir, (LPCTSTR) strPkg, szPackageExt);
                }

        }

#ifdef PKG_LOAD_TIME
        dwaPkgLoadLibraryTime[nPkg] = ::GetTickCount();
        straPkgName[nPkg] = szPackage;
#endif  // PKG_LOAD_TIME

        int nOldMode = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
        hInst = LoadLibrary(szDLL);
        SetErrorMode(nOldMode);
        dwLastError = GetLastError();

#ifdef PKG_LOAD_TIME
        dwaPkgLoadLibraryTime[nPkg] = ::GetTickCount() - dwaPkgLoadLibraryTime[nPkg];
#endif  // PKG_LOAD_TIME

        if (hInst < (HINSTANCE)HINSTANCE_ERROR)
        {
                if (!bPackageCompatible)
                {
                        CString strOut;
                        strOut.LoadString(IDS_ERR_CANT_LOAD_PKG_VER);
                        wsprintf(szDLL, (const TCHAR *)strOut, (const TCHAR *)strPkg);
#ifndef _DEBUG
                        AfxMessageBox(szDLL, MB_OK | MB_ICONEXCLAMATION);
                        return FALSE;
#else
                        if (!bRequired)
                        {
                                return(AfxMessageBox(szDLL, MB_YESNO | MB_ICONEXCLAMATION) == IDYES);
                        }
                        else
                        {
                                AfxMessageBox(szDLL, MB_OK | MB_ICONEXCLAMATION);
                                return FALSE;
                        }
#endif // _DEBUG
                }
                else
                {
                        // if package load failed for reason other than version check failure, then
                        // set error mode so that "missing DLL" message will be displayed and retry.
                        // this way the user gets a clue as to why the package failed to load.
                        nOldMode = SetErrorMode(0);
                        hInst = LoadLibrary(szDLL);
                        SetErrorMode(nOldMode);
                        ASSERT(dwLastError == GetLastError());
#ifndef _DEBUG
                        // ReportMissingFile will issue message to user.
                        return FALSE;
#else
                        if (bRequired)
                        {
                                // if package is required, then just return FALSE without
                                // asking user if continuing without the package is acceptable.
                                // ReportMissingFile will issue message to user.
                                return FALSE;
                        }
                        else
                        {
                                CString strOut;
                                LPCTSTR szMsg;
                                szMsg = StringFromError(dwLastError);
                                TRACE("  package load failed!\n");
                                TRACE("    error = %lu (0x%08lx) %s\n", dwLastError, dwLastError, szMsg ? szMsg : "");
                                if (szMsg)
                                        ::LocalFree((void*)szMsg);

                                strOut.LoadString(IDS_ERR_CANT_LOAD_PKG);
                                wsprintf(szDLL, (const TCHAR *)strOut, (const TCHAR *)strPkg, dwLastError);
                                return (AfxMessageBox(szDLL, MB_YESNO | MB_ICONEXCLAMATION) == IDYES);
                        }
#endif // _DEBUG
                }
        }

        static char BASED_CODE szInitPackage[] = _TEXT("InitPackage");

        INITPACKAGEPROC lpproc = (INITPACKAGEPROC)GetProcAddress(hInst, szInitPackage);
        if (lpproc == NULL)
        {
                TRACE("  package does not have a InitPackage procedure\n");
                FreeLibrary(hInst);
                return FALSE;
        }

#ifdef PKG_LOAD_TIME
        dwaPkgTypeLibTime[nPkg] = ::GetTickCount();
#endif  // PKG_LOAD_TIME

#ifdef PKG_LOAD_TIME
        dwaPkgTypeLibTime[nPkg] = ::GetTickCount() - dwaPkgTypeLibTime[nPkg];
#endif  // PKG_LOAD_TIME

#ifdef _DEBUG
        // check flag indicating that package performed shell compatibility check
        ASSERT(bPackageChecked);        // every package must perform version check!
#endif

        // we loaded this package, make a note of it
        g_strlstPkgsLoaded.AddTail(strPkg);

#ifdef PKG_LOAD_TIME
        dwaPkgInitPackageTime[nPkg] = ::GetTickCount();
#endif  // PKG_LOAD_TIME

        BOOL bInitSucceeded=(*lpproc)(theApp.m_pMainWnd->m_hWnd);

#ifdef PKG_LOAD_TIME
        dwaPkgInitPackageTime[nPkg] = ::GetTickCount() - dwaPkgInitPackageTime[nPkg];
        nPkg++;
#endif  // PKG_LOAD_TIME

#ifdef _DEBUG
        if(!bInitSucceeded)
        {
                TRACE("::LoadPackage: package returned failure from InitPackage\n");
                FreeLibrary(hInst);

                CString strOut;
                LPCTSTR szMsg;
                szMsg = StringFromError(dwLastError);
                TRACE("::LoadPackage: package initialise failed!\n");
                TRACE("::LoadPackage: error = %lu (0x%08lx) %s\n", dwLastError, dwLastError, szMsg ? szMsg : "");
                if (szMsg)
                        ::LocalFree((void*)szMsg);

                strOut.LoadString(IDS_ERR_CANT_INIT_PKG);
                wsprintf(szDLL, (const TCHAR *)strOut, (const TCHAR *)strPkg, dwLastError);
                bInitSucceeded=(AfxMessageBox(szDLL, MB_YESNO | MB_ICONEXCLAMATION) == IDYES);
        }
#endif
        return bInitSucceeded;
}

// Check for existence of
//  HKEY_CLASSES_ROOT\TypeLib\tlid\MajorVer.MinorVer
static BOOL IsTypeLibRegistered(const GUID& tlid, WORD wMajor, WORD wMinor)
{
        OLECHAR swzTlid[64];
        VERIFY(StringFromGUID2(tlid, swzTlid, 64) != 0);

        TCHAR szRegString[256];
        sprintf(szRegString, "TypeLib\\%S\\%d.%d", swzTlid, wMajor, wMinor);

        LONG lSize;
        return (RegQueryValue(HKEY_CLASSES_ROOT, szRegString, NULL, &lSize)
                == ERROR_SUCCESS);
}

// Check for existence of
//  HKEY_CLASSES_ROOT\TypeLib\ShellTLID\MajorVer.MinorVer
static BOOL IsShellTLIDRegistered()
{
        LONG lSize;
        return (RegQueryValue(HKEY_CLASSES_ROOT, LIBID_Shell_STRING, NULL, &lSize)
                == ERROR_SUCCESS);
}


// Compare against registry's 5.0\General\PackageCount.
//  If they're different, store the new value there and return TRUE
static BOOL IsPackageSetDifferent(int cPackagesLoaded)
{
        if (GetRegInt(szGeneralKey, szPackageCountKey, -1) == cPackagesLoaded)
                return FALSE;

        VERIFY(WriteRegInt(szGeneralKey, szPackageCountKey, cPackagesLoaded));
        return TRUE;
}


// Type libraries should be registered on startup as specified in this table:
//
//              CONDITION                                                                               REGISTER
//              ---------                                                                               --------
//              Set of pkgs loaded different than last time             YES
//              /REGISTER cmdline flag                                                  YES (FULL) [tested for in InitInstance]
//              LIBID_Shell not registered                                              YES (FULL)
//              Otherwise                                                                               NO
//
// Typical registration simply involves loading typelibs, getting application
//  extensions from packages, and calling RegisterTypeLib if the typelib "appears"
//  not to be registered.  FULL registration does all this and calls RegisterTypeLib
//  regardless of whether the typelib appears registered.
//
BOOL ShouldRegisterTypeLibsOnStartup(int cPackagesLoaded, BOOL& rbForceFullRegistration)
{
        // If /REGISTER is passed on cmdline, this isn't even called, since
        //  the shell knows it must register all typelibs.  We still
        //  want to force full registration if the app's CLSID isn't
        //  registered, though.

        if (!IsShellTLIDRegistered())
        {
                rbForceFullRegistration = TRUE;
                return TRUE;
        }

        rbForceFullRegistration = FALSE;

        if (IsPackageSetDifferent(cPackagesLoaded))
                return TRUE;

        return FALSE;
}

// This is called after all the packages have been loaded, and either
//      (A) On startup because package set is different,
//              LIBID_Shell not registered, or /REGISTER cmdline flag passed, OR
//  (B) The first time a macro file is loaded
void CVshellPackage::RegisterTypeLibsIfNecessary(BOOL bForceFullRegistration /* = FALSE */)
{
        static BOOL bTypeLibsLoaded = FALSE;

        // Only load/register once, unless the caller really really
        //  wants us to register them all (i.e., /REGISTER cmdline)
        if (bTypeLibsLoaded && !bForceFullRegistration)
                return;

        // Optimization: This DLL is loaded for each typelib we register, so
        //  load it once now to avoid its loading/unloading/loading/unloading/etc.
#ifdef _DEBUG
        HINSTANCE hinst = LoadLibrary("DevTLDcD.dll");
#else
        HINSTANCE hinst = LoadLibrary("DevTLDc.dll");
#endif

        POSITION pos = theApp.m_packages.GetHeadPosition();
        while (pos != NULL)
        {
                CPackage* pPackage = (CPackage*) theApp.m_packages.GetNext(pos);
                RegisterTypeLib(pPackage, bForceFullRegistration, !bTypeLibsLoaded);
        }

        if (hinst != NULL)
                FreeLibrary(hinst);
        bTypeLibsLoaded = TRUE;
}

// Little helper used in GetAllApplicationExtensions below.
void CVshellPackage::AddNewAutoItem(const GUID* pTlid, WORD wMajorVersion, WORD wMinorVersion,
                                                                        LPCTSTR szName, const CLSID* pClsid)
{
        CAutomationItem* pAutomationItem = new CAutomationItem;

        // Tlid entry stuff
        memcpy(&(pAutomationItem->m_tlid), pTlid, sizeof(GUID));
        pAutomationItem->m_wMajorVersion = wMajorVersion;
        pAutomationItem->m_wMinorVersion = wMinorVersion;

        // SApplicationExtension stuff (name & clsid)
        ASSERT (strlen(szName) < MAX_LENGTH_NAMED_ITEM);
        _tcsncpy(pAutomationItem->m_szName, szName, MAX_LENGTH_NAMED_ITEM-1);
        (pAutomationItem->m_szName)[MAX_LENGTH_NAMED_ITEM-1] = '\0';
        memcpy(&(pAutomationItem->m_clsid), pClsid, sizeof(CLSID));

        // Add it to the list.
        m_ptrlAutomationItems.AddTail(pAutomationItem);
}

// This loads & registers a single package's typelibrary.  It then calls
//  CPackage::GetApplicationExtension to get the names
//  of any top-level named items for VBS, and adds that info to
//  CVshellPackage::m_ptrlAutomationItems.  This list is later used to
//  implement IAutomationItems, which is used exclusively by devaut1 to add named
//  items to VBS.
void CVshellPackage::RegisterTypeLib(CPackage* pPackage, BOOL bForceFullRegistration,
                                                                         BOOL bGetApplicationExtensions)
{
        USES_CONVERSION;

        TCHAR szDLL[MAX_PATH];
        ::GetModuleFileName(pPackage->HInstance(), szDLL, MAX_PATH);

        // SPECIAL CASE:  Skip devres(d).pkg since it has a type library
        //  but not for automation-- it's for doing funky stuff with
        //  data bound controls.  Ignore it.
        if (!_tcsicmp(_tcsrchr(szDLL, '\\'),
#ifdef _DEBUG
                "\\devresd.pkg"))
#else
                "\\devres.pkg"))
#endif
                return;

        // See if the package has a type library.  If it does, register it
        //  and remember the TLID in case the package provides a top-level
        //  object.
        ITypeLib* pTypeLib = NULL;
        if (SUCCEEDED(LoadTypeLib(A2W(szDLL), &pTypeLib)) && pTypeLib != NULL)
        {
                TLIBATTR* pTlibAttr = NULL;
                if (SUCCEEDED(pTypeLib->GetLibAttr(&pTlibAttr)) && pTlibAttr != NULL)
                {
                        // Register type lib
                        if (bForceFullRegistration || !IsTypeLibRegistered(pTlibAttr->guid,
                                                pTlibAttr->wMajorVerNum, pTlibAttr->wMinorVerNum))
                        {
                                VERIFY(SUCCEEDED(::RegisterTypeLib(pTypeLib, A2W(szDLL), NULL)));
                        }

                        // See if this package has any application extensions to offer.
                        if (bGetApplicationExtensions && (pPackage->m_flags & PKS_AUTOMATION))
                        {
                                // Does this package supply any application extensions?
                                SApplicationExtension* pAppExts;
                                int cExts = pPackage->GetApplicationExtensions(&pAppExts);
                                for (int i=0; i < cExts; i++)
                                {
                                        // For each SApplicationExtension, create a CAutomationItem
                                        //  structure in our list
                                        AddNewAutoItem(&pTlibAttr->guid,
                                                pTlibAttr->wMajorVerNum, pTlibAttr->wMinorVerNum,
                                                pAppExts[i].szName, &pAppExts[i].clsid);
                                }

                                if (cExts == 0)
                                {
                                        // For this typelib, the package doesn't provide
                                        //  any application extensions, so create an entry
                                        //  in the list which devaut1 can use just to add the
                                        //  type lib to VBS (and not call AddNamedItem on).
                                        // However, there may be no extension because this is
                                        //  the application object.  If so, be sure to provide
                                        //  the right CLSID
                                        const CLSID* pClsid = (pTlibAttr->guid == LIBID_Shell) ?
                                                &APP_CLSID : &CLSID_NULL;

                                        AddNewAutoItem(&pTlibAttr->guid,
                                                pTlibAttr->wMajorVerNum, pTlibAttr->wMinorVerNum, "", pClsid);
                                }
                        }
                        pTypeLib->ReleaseTLibAttr(pTlibAttr);
                }
                pTypeLib->Release();
                pTypeLib = NULL;
        }
}


BOOL CTheApp::LoadPackages(int *pnPackagesLoaded)
{
        // Load packages used by this type of project shell
        // Errors prevent the IDE from loading...

	// Count # packages we successfully load.  This is our heuristic for knowing
	//  if "new" packages appeared since our last startup, indicating that we
	//  need to try and load & register their typelibs.
	*pnPackagesLoaded = 0;
	
#ifndef _SHIP
        m_bPkgChk = !ScanCommandLine("NoPkgChk");       // optionally disable shell compatibility check
#endif
	
	CString str;
	CStringList strlstPkgsNot;
	while (ScanCommandLine("NotPkg", &str))
	{
		str.MakeLower();
		strlstPkgsNot.AddTail(str);
	}

	BOOL bNoExtraPackages = (g_packages == rgszMakePackages);
	
	BOOL bNoPackages = ScanCommandLine("NoPackages");
	if (!bNoPackages)
	{
		for (int n = 0; n < g_nPackages; n += 1)
		{
			BOOL retval;
			// don't report missing packages for command line build (needed for fortran)
			if (!(retval = LoadPackage(g_packages[n], strlstPkgsNot, FALSE, TRUE)) && !m_bInvokedCommandLine)
			{
				ReportMissingFile(CString(g_packages[n]) + "." + szPackageExt);
				return FALSE;
			}
			else if (retval == TRUE)
				(*pnPackagesLoaded)++;
		}
		
	  if (!bNoExtraPackages)
	  {
		// Scan the IDE directory for *.PKG files and load them as packages
		// Errors prevent the IDE from loading...
		
		LPCTSTR lpszExtensionDir = GetExeString(DefaultExtensionDir);
		if (lstrlen(lpszExtensionDir) > 0)
		{
			TCHAR szIDEDir [_MAX_PATH];
			
			// Get the directory of the IDE executable
			{
				TCHAR szPath [_MAX_PATH];
				TCHAR szDir [_MAX_DIR];
				GetModuleFileName(NULL, szPath, _MAX_PATH);
				_tsplitpath(szPath, szIDEDir, szDir, NULL, NULL);
				_tcscat(szIDEDir, szDir);
			}
			
			WIN32_FIND_DATA findData;
			HANDLE hFindFile;
			_tcscat(szIDEDir, lpszExtensionDir);
#if 1           // hack to fix the build be ensuring proper load ordering
			// REVIEW: make sure we load the BLD pkg first if present
			TCHAR szBldPath [_MAX_PATH];
			_tcscpy(szBldPath, szIDEDir);
			_tcscat(szBldPath, _TEXT("\\DEVBLD*.PKG"));
			hFindFile = FindFirstFile(szBldPath, &findData);
			if (hFindFile != INVALID_HANDLE_VALUE)
			{
				TCHAR szFName [_MAX_FNAME];
				_tsplitpath(findData.cFileName, NULL, NULL, szFName, NULL);
				if ((szFName[0] != '\0') && (!LoadPackage(szFName, strlstPkgsNot, TRUE, FALSE)))
				{
					FindClose(hFindFile);
					ReportMissingFile(szFName);
					return FALSE;
				}
				(*pnPackagesLoaded)++;
				FindClose(hFindFile);
			}
#endif  // 1
			_tcscat(szIDEDir, _TEXT("\\*.PKG"));
			
			hFindFile = FindFirstFile(szIDEDir, &findData);
			if (hFindFile != INVALID_HANDLE_VALUE)
			{
				do
				{
					TCHAR szFName [_MAX_FNAME];
					_tsplitpath(findData.cFileName, NULL, NULL, szFName, NULL);
					if (szFName[0] == '\0')
						break;
					if (!LoadPackage(szFName, strlstPkgsNot, TRUE, FALSE))
					{
						FindClose(hFindFile);
						ReportMissingFile(szFName);
						return FALSE;
					}
					(*pnPackagesLoaded)++;
				}
				while (FindNextFile(hFindFile, &findData));
				FindClose(hFindFile);
			}
		}
	  }
	}
	
	// Scan the command line for /PKG and /IDEPKG switches to load
	// Errors prevent the retail IDE from loading...
	
	for (int i = 0; i < 2; i++)
	{
		static LPCTSTR aszSwitch[2] = { "PathPkg", "IDEPkg" };
		
		while (ScanCommandLine(aszSwitch[i], &str))
		{
			if (!str.IsEmpty())
			{
				if (!LoadPackage(str, strlstPkgsNot, i, FALSE))
				{
					ReportMissingFile(str + "." + szPackageExt);
					return FALSE;
				}
				*(pnPackagesLoaded)++;
				str.Empty();
			}
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//      Resource Range Validating Routines
//

struct SPkgIdRanges
{
        DWORD dwPkgId;
        WORD wMinResource, wMaxResource;
        WORD wMinString, wMaxString;
        WORD wMinCommand, wMaxCommand;
        //      WORD wMinWindow, wMinWindow;
};

BASED_CODE SPkgIdRanges g_aPkgIdRanges[] =
{
        // TODO(judithm): shell is stealing from shared and exe resource ranges
        { PACKAGE_SUSHI, MIN_SHARED_RESOURCE_ID, MAX_SHELL_RESOURCE_ID,
                        MIN_SHARED_RESOURCE_ID, MAX_SHELL_RESOURCE_ID,
                        MIN_SHELL_COMMAND_ID, MAX_SHELL_COMMAND_ID, },
        { PACKAGE_VCPP, MIN_EDIT_RESOURCE_ID, MAX_EDIT_RESOURCE_ID,
                MIN_EDIT_RESOURCE_ID, MAX_EDIT_RESOURCE_ID,
                MIN_SOURCE_COMMAND_ID, MAX_SOURCE_COMMAND_ID, },
        { PACKAGE_VRES, MIN_RES_RESOURCE_ID, MAX_RES_RESOURCE_ID,
                MIN_RES_RESOURCE_ID, MAX_RES_RESOURCE_ID,
                MIN_RES_COMMAND_ID, MAX_RES_COMMAND_ID, },
        { PACKAGE_VPROJ, MIN_BUILD_RESOURCE_ID, MAX_BUILD_RESOURCE_ID,
                MIN_BUILD_RESOURCE_ID, MAX_BUILD_RESOURCE_ID,
                MIN_BUILD_COMMAND_ID, MAX_BUILD_COMMAND_ID, },
        { PACKAGE_PRJSYS, MIN_WORKSPACE_RESOURCE_ID, MAX_WORKSPACE_RESOURCE_ID,
                MIN_WORKSPACE_RESOURCE_ID, MAX_WORKSPACE_RESOURCE_ID,
                MIN_WORKSPACE_COMMAND_ID, MAX_WORKSPACE_COMMAND_ID},
        { PACKAGE_IV, MIN_IV_RESOURCE_ID, MAX_IV_RESOURCE_ID,
                MIN_IV_RESOURCE_ID, MAX_IV_RESOURCE_ID,
                MIN_IV_COMMAND_ID, MAX_IV_COMMAND_ID},
        { PACKAGE_LANGCPP, MIN_CPP_RESOURCE_ID, MAX_CPP_RESOURCE_ID,
                MIN_CPP_RESOURCE_ID, MAX_CPP_RESOURCE_ID,
                0,0,},
        { PACKAGE_LANGFOR, MIN_FORTRAN_RESOURCE_ID, MAX_FORTRAN_RESOURCE_ID,
                MIN_FORTRAN_RESOURCE_ID, MAX_FORTRAN_RESOURCE_ID,
                MIN_FORTRAN_COMMAND_ID, MAX_FORTRAN_COMMAND_ID,},
        { PACKAGE_GALLERY, MIN_GALLERY_RESOURCE_ID, MAX_GALLERY_RESOURCE_ID,
                MIN_GALLERY_RESOURCE_ID, MAX_GALLERY_RESOURCE_ID,
                0,0,},
        { PACKAGE_LANGMST, MIN_TEST_RESOURCE_ID, MAX_TEST_RESOURCE_ID,
                MIN_TEST_RESOURCE_ID, MAX_TEST_RESOURCE_ID,
                MIN_TEST_COMMAND_ID, MAX_TEST_COMMAND_ID,},
        { PACKAGE_CLASSVIEW, MIN_CLSVIEW_RESOURCE_ID, MAX_CLSVIEW_RESOURCE_ID,
                MIN_CLASSVIEW_STRING_ID, MAX_CLASSVIEW_STRING_ID,
                MIN_CLASSVIEW_COMMAND_ID,MAX_CLASSVIEW_COMMAND_ID,},
        { PACKAGE_ENT, MIN_ENT_RESOURCE_ID, MAX_ENT_RESOURCE_ID,
                MIN_ENT_RESOURCE_ID, MAX_ENT_RESOURCE_ID,
                MIN_ENT_COMMAND_ID, MAX_ENT_COMMAND_ID,},
        { PACKAGE_LANGHTML, MIN_HTML_RESOURCE_ID, MAX_HTML_RESOURCE_ID,
                MIN_HTML_RESOURCE_ID, MAX_HTML_RESOURCE_ID,
                MIN_HTML_COMMAND_ID, MAX_HTML_COMMAND_ID,},
        { PACKAGE_BIN, MIN_BIN_RESOURCE_ID, MAX_BIN_RESOURCE_ID,
                MIN_BIN_RESOURCE_ID, MAX_BIN_RESOURCE_ID,
                MIN_BIN_COMMAND_ID, MAX_BIN_COMMAND_ID,},
        { PACKAGE_IMG, MIN_IMG_RESOURCE_ID, MAX_IMG_RESOURCE_ID,
                MIN_IMG_RESOURCE_ID,MAX_IMG_RESOURCE_ID,
                MIN_IMG_COMMAND_ID,MAX_IMG_COMMAND_ID, },
        { PACKAGE_NUMEGA, MIN_NUMEGA_RESOURCE_ID, MAX_NUMEGA_RESOURCE_ID,
                MIN_NUMEGA_RESOURCE_ID, MAX_NUMEGA_RESOURCE_ID,
                MIN_NUMEGA_COMMAND_ID, MAX_NUMEGA_COMMAND_ID,},
        { PACKAGE_SIMPLE, MIN_SAMPLE_RESOURCE_ID, MAX_SAMPLE_RESOURCE_ID,
                MIN_SAMPLE_RESOURCE_ID, MAX_SAMPLE_RESOURCE_ID,
                MIN_SAMPLE_COMMAND_ID, MAX_SAMPLE_COMMAND_ID , },
        { PACKAGE_HTMLEX, 0, 0,
                MIN_HTMLEX_STRING_ID, MAX_HTMLEX_STRING_ID,
                0, 0 },
        { PACKAGE_IATOOL, MIN_IATOOL_RESOURCE_ID, MAX_IATOOL_RESOURCE_ID,
                MIN_IATOOL_STRING_ID, MAX_IATOOL_STRING_ID,
                MIN_IATOOL_COMMAND_ID, MAX_IATOOL_COMMAND_ID },
        { PACKAGE_NCB, MIN_NCB_RESOURCE_ID, MAX_NCB_RESOURCE_ID,
                MIN_NCB_STRING_ID, MAX_NCB_STRING_ID,
                MIN_NCB_COMMAND_ID, MAX_NCB_COMMAND_ID },
        { PACKAGE_IST, MIN_IST_RESOURCE_ID, MAX_IST_RESOURCE_ID,
                MIN_IST_STRING_ID, MAX_IST_STRING_ID,
                MIN_IST_COMMAND_ID, MAX_IST_COMMAND_ID },
        { PACKAGE_LANGJVA, MIN_LANGJVA_RESOURCE_ID, MAX_LANGJVA_RESOURCE_ID,
                MIN_LANGJVA_STRING_ID, MAX_LANGJVA_STRING_ID,
                MIN_LANGJVA_COMMAND_ID, MAX_LANGJVA_COMMAND_ID },
        { PACKAGE_JVA, MIN_JVA_RESOURCE_ID, MAX_JVA_RESOURCE_ID,
                0,0,
                0,0},
        { PACKAGE_AUT1, MIN_AUT1_RESOURCE_ID, MAX_AUT1_RESOURCE_ID,
                0,0,
                MIN_AUT1_COMMAND_ID, MAX_AUT1_COMMAND_ID },
        { PACKAGE_DEBUG, MIN_DEBUG_RESOURCE_ID, MAX_DEBUG_RESOURCE_ID,          // REVIEW(davidga)
                MIN_DEBUG_RESOURCE_ID, MAX_DEBUG_RESOURCE_ID,                                   // this is the same range as the edit package (VCPP)
                MIN_SOURCE_COMMAND_ID, MAX_SOURCE_COMMAND_ID, },
        { PACKAGE_ODL, MIN_ODL_RESOURCE_ID, MAX_ODL_RESOURCE_ID,
                MIN_ODL_STRING_ID, MAX_ODL_STRING_ID,
                MIN_ODL_COMMAND_ID, MAX_ODL_COMMAND_ID },
        { PACKAGE_AUT2, MIN_AUT2_RESOURCE_ID, MAX_AUT2_RESOURCE_ID,
                0,0,
                MIN_AUT2_COMMAND_ID, MAX_AUT2_COMMAND_ID },
        { PACKAGE_HTM, MIN_HTM_RESOURCE_ID, MAX_HTM_RESOURCE_ID,
                MIN_HTM_STRING_ID, MAX_HTM_STRING_ID,
                MIN_HTM_COMMAND_ID, MAX_HTM_COMMAND_ID },
        { PACKAGE_DTG, MIN_DTG_RESOURCE_ID, MAX_DTG_RESOURCE_ID,
                MIN_DTG_STRING_ID, MAX_DTG_STRING_ID,
                MIN_DTG_COMMAND_ID, MAX_DTG_COMMAND_ID },
        { PACKAGE_DDK, MIN_DDK_RESOURCE_ID, MAX_DDK_RESOURCE_ID,
                MIN_DDK_STRING_ID, MAX_DDK_STRING_ID,
                MIN_DDK_COMMAND_ID, MAX_DDK_COMMAND_ID },
};

DWORD CPackage::GetIdRange(RANGE_TYPE rt)
{
        UINT nPkgLoc;
        for(nPkgLoc=0; nPkgLoc < sizeof(g_aPkgIdRanges) / sizeof(SPkgIdRanges);
        nPkgLoc++)
        {
                if(g_aPkgIdRanges[nPkgLoc].dwPkgId == m_id)
                {
                        switch (rt)
                        {
                        case MIN_RESOURCE:
                                return g_aPkgIdRanges[nPkgLoc].wMinResource;

                        case MAX_RESOURCE:
                                return g_aPkgIdRanges[nPkgLoc].wMaxResource;

                        case MIN_STRING:
                                return g_aPkgIdRanges[nPkgLoc].wMinString;

                        case MAX_STRING:
                                return g_aPkgIdRanges[nPkgLoc].wMaxString;

                        case MIN_COMMAND:
                                return g_aPkgIdRanges[nPkgLoc].wMinCommand;

                        case MAX_COMMAND:
                                return g_aPkgIdRanges[nPkgLoc].wMaxCommand;

                        case MIN_WINDOW:
                                return 0;       // derived package should override to change

                        case MAX_WINDOW:
                                return 0;       // derived package should override to change

                        default:
                                ASSERT(FALSE);
                                return 0;
                        }
                }
        }
        return 0;

}

#ifdef _DEBUG
struct SResIdCheckInfo
{
        WORD wMinRes;
        WORD wMaxRes;
        WORD wMinStr;
        WORD wMaxStr;
        //      UINT nPackageId;
        LPCSTR szFilename;
        int nResRangeUsed;
        int nStrRangeUsed;
};

LPCTSTR ResTypeFromRT(LPCTSTR szType)
{
        if( (DWORD)szType > 0x0000FFFF )
                return szType;
        switch( (DWORD)szType )
        {
        case RT_CURSOR:
                return "Cursor";
        case RT_BITMAP:
                return "Bitmap";
        case RT_ICON:
                return "Icon";
        case RT_MENU:
                return "Menu";
        case RT_DIALOG:
                return "Dialog";
        case RT_STRING:
                return "StringSegment";
        case RT_FONTDIR:
                return "FontDir";
        case RT_FONT:
                return "Font";
        case RT_ACCELERATOR:
                return "Accelerator";
        case RT_RCDATA:
                return "RCData";
        case RT_MESSAGETABLE:
                return "MessageTable";
        case RT_GROUP_CURSOR:
                return "CursorGroup";
        case RT_GROUP_ICON:
                return "IconGroup";
        case RT_VERSION:
                return "Version";
        case RT_DLGINCLUDE:
                return "DlgInclude";
        case RT_PLUGPLAY:
                return "PlugPlay";
        case RT_VXD:
                return "VXD";
        }

        static char acResType[32];
        wsprintf(acResType, "0x%X", (DWORD)szType);
        return acResType;
}

BOOL WINAPI EnumPkgResNames(HINSTANCE hModule, LPCTSTR szType, LPTSTR szName,
                                                        LONG lParam)
{
        SResIdCheckInfo* pricInfo = (SResIdCheckInfo*)lParam;

        if( (DWORD)szName > 0x0000ffff )        // all string IDs are valid
        {
                pricInfo->nResRangeUsed++;              // count number of this resource type used by this package
                return TRUE;
        }

        WORD wName = (WORD)(DWORD)szName;

        if( szType == RT_STRING )
        {
                WORD wString0 = (wName - 1) << 4;
                if( wString0 >= 0xE000 )
                        return TRUE;    // ignore MFC overrides

                if(     !((wString0 >= MIN_HELPSTRING_ID)               && ((wString0 + 15) <= MAX_HELPSTRING_ID)) &&
                        !((wString0 >= MIN_SHARED_RESOURCE_ID)  && ((wString0 + 15) <= MAX_SHARED_RESOURCE_ID))
                        )
                {
                        if( ((wString0 < pricInfo->wMinStr) || ((wString0 + 15) > pricInfo->wMaxStr))
                                )
                        {
                                pricInfo->nStrRangeUsed += 16;          // count number of strings used by this package,
                                // provided that this isn't in a special range
                        }
                        if( ((wString0 < pricInfo->wMinRes) || ((wString0 + 15) > pricInfo->wMaxRes))
                                && ((wString0 < pricInfo->wMinStr) || ((wString0 + 15) > pricInfo->wMaxStr))
                                )
                        {
                                char acBuf[512];
                                wsprintf(acBuf, "::EnumPkgResNames: Resource Range Error: String segment out of range in package '%s'.\n"
                                        "\tSegment is strings %d - %d.  Valid range is from %d to %d, and %d to %d.\n",
                                        pricInfo->szFilename, wString0, wString0 + 15,
                                        pricInfo->wMinRes, pricInfo->wMaxRes, pricInfo->wMinStr, pricInfo->wMaxStr);
                                TRACE0(acBuf);
                                //                      return (AfxMessageBox(acBuf, MB_YESNO | MB_ICONEXCLAMATION) == IDYES);
                                ASSERT( FALSE );                // hitting this assert means your package has a string where it shouldn't be
                                        }                                                       // note that empty string can cause this
                }
        }
        else
        {
                pricInfo->nResRangeUsed++;              // count number of this resource type used by this package
                if( ((wName < pricInfo->wMinRes) || (wName > pricInfo->wMaxRes)) &&
                        !((wName >= MIN_SHARED_RESOURCE_ID) && (wName <= MAX_SHARED_RESOURCE_ID))
                        && !((szType == RT_VERSION) && (wName == 0x1)) )
                {
                        char acBuf[512];
                        wsprintf(acBuf, "Resource Range Error in package '%s'.\n"
                                "\tType == %s,  ID == %d.  Valid range is from %d to %d.\n",
                                pricInfo->szFilename, ResTypeFromRT(szType), wName,
                                pricInfo->wMinRes, pricInfo->wMaxRes);
                        TRACE0(acBuf);
                        //                      return (AfxMessageBox(acBuf, MB_YESNO | MB_ICONEXCLAMATION) == IDYES);
                        ASSERT( FALSE );                // hitting this assert means your package has a resource where it shouldn't be
                }
        }

        return TRUE;
}

BOOL WINAPI EnumPkgResTypes(HINSTANCE hModule, LPTSTR szType, LONG lParam)
{
        ASSERT( lParam != NULL );
        if( ((DWORD)szType > 0xffff) || (szType == RT_ICON) || (szType == RT_CURSOR) )          // don't need to scan these
                return TRUE;                                                                            // as the IDs are always 1,2,3...

        SResIdCheckInfo* pricInfo = (SResIdCheckInfo*)lParam;
        int nRangeOld = pricInfo->nResRangeUsed;        // note biggest range so far
        pricInfo->nResRangeUsed = 0;
        if( !EnumResourceNames(hModule, szType, EnumPkgResNames, lParam) )
                return FALSE;

        if( pricInfo->nResRangeUsed < nRangeOld )       // store biggest range of any type
                pricInfo->nResRangeUsed = nRangeOld;

        return TRUE;
}
#endif  // _DEBUG
//////////////////////////////////////////////////////////////////////////////

void CTheApp::UnloadPackages()
{
        // Unload all registered packages
        while (!m_packages.IsEmpty())
        {
                CPackage *pPackage = (CPackage*) m_packages.RemoveHead();
                delete pPackage;
        }
}

BOOL CTheApp::RegisterPackage(CPackage* pPackage)
{
        if (pPackage == NULL)
        {
                ASSERT(FALSE);
                return FALSE;
        }

        m_packages.AddTail(pPackage);
        m_bMenuDirty = TRUE;
        m_bProjectCommandsDirty=TRUE;

#ifdef _DEBUG
        // debug check to make sure all resources in package are valid
        SResIdCheckInfo ricInfo;
        ricInfo.wMinRes = (WORD)pPackage->GetIdRange(CPackage::MIN_RESOURCE);
        ricInfo.wMaxRes = (WORD)pPackage->GetIdRange(CPackage::MAX_RESOURCE);
        ricInfo.wMinStr = (WORD)pPackage->GetIdRange(CPackage::MIN_STRING);
        ricInfo.wMaxStr = (WORD)pPackage->GetIdRange(CPackage::MAX_STRING);
        //      ricInfo.nPackageId = pPackage->PackageID();     // since we can't get the filename
        char acPkgFileName[256];
        GetModuleFileName(pPackage->HInstance(), acPkgFileName, 255);
        ricInfo.szFilename = acPkgFileName;
        ricInfo.nResRangeUsed = 0;
        ricInfo.nStrRangeUsed = 0;

        if( !EnumResourceTypes(pPackage->HInstance(), EnumPkgResTypes, (LPARAM)&ricInfo) )
        {
                TRACE1("Resource Range Check Error: Could not enumerate resources in package '%s'.\n",
                        ricInfo.szFilename);
                return FALSE;
        }

        // martynl: Only display this message if the usage is bad. These shouldn't fail, because the above should fail first
        if(ricInfo.nResRangeUsed > ricInfo.wMaxRes - ricInfo.wMinRes + 1)
        {
                TRACE3("CTheApp::RegisterPackage: Bad Resource Range Usage: Package '%s' uses %d slots out of its ID range of %d slots.\n",
                        ricInfo.szFilename, ricInfo.nResRangeUsed, ricInfo.wMaxRes - ricInfo.wMinRes + 1);
                ASSERT(FALSE);
        }

        if(ricInfo.nStrRangeUsed > ricInfo.wMaxStr - ricInfo.wMinStr + 1)
        {
                TRACE3("CTheApp::RegisterPackage: Bad Resource Range Usage: Package '%s' uses %d string slots out of its String ID range of %d slots.\n",
                        ricInfo.szFilename, ricInfo.nStrRangeUsed, ricInfo.wMaxStr - ricInfo.wMinStr + 1);
                ASSERT(FALSE);
        }
#endif // _DEBUG

        return TRUE;
}

BOOL CTheApp::RegisterPacket(CPacket* pPacket)
{
#ifdef _DEBUG
        if (pPacket == NULL)
        {
                ASSERT(FALSE);
                return FALSE;
        }

        ASSERT(pPacket->PacketID() > 0);

        // There better not be any Packets with identical ids
        CPackage* pPackage = GetPackage(pPacket->PackageID());
        if ((GetPacket(pPacket->PackageID(), pPacket->PacketID()) != NULL)
                || (pPackage == NULL))
        {
                ASSERT(FALSE);
                return FALSE;
        }
#endif

        m_packets.AddTail(pPacket);

        ASSERT(GetPacket(pPacket->PackageID(), pPacket->PacketID()) != NULL);

        m_bMenuDirty = TRUE;
        m_bProjectCommandsDirty=TRUE;

        return TRUE;
}

CPackage* CTheApp::GetPackage(UINT id)
{
        POSITION pos = m_packages.GetHeadPosition();
        while (pos != NULL)
        {
                CPackage* pPackage = (CPackage*) m_packages.GetNext(pos);
                if (pPackage->PackageID() == id)
                        return pPackage;
        }

        return NULL;
}

///////////////////////////////////////////////////////////////////////////////
BOOL GetVersionOfFile(LPSTR pszFileName, CString & strVersion, CString & strDescription)
{
        BOOL retval = FALSE;
        DWORD   dwHandle, dwVerInfoSize;
        unsigned int dwValueSize;
        VOID *  pvVerInfo = NULL;
        VOID *  pvValue = NULL;

        if (dwVerInfoSize = GetFileVersionInfoSize(pszFileName, &dwHandle))
        {
                pvVerInfo = new char[dwVerInfoSize];
                if (GetFileVersionInfo(pszFileName, dwHandle, dwVerInfoSize, pvVerInfo))
                {
                        if (VerQueryValue(      pvVerInfo,
                                "\\StringFileInfo\\040904B0\\FileVersion",
                                &pvValue,
                                &dwValueSize))
                        {
                                if (dwValueSize < (UINT)256)
                                {
                                        ASSERT(_tcslen((LPCSTR)pvValue) <= dwValueSize);
                                        strVersion = (LPCSTR)pvValue;
                                        retval = TRUE;
                                }
                        }
                        if (VerQueryValue(      pvVerInfo,
                                "\\StringFileInfo\\040904B0\\FileDescription",
                                &pvValue,
                                &dwValueSize))
                        {
                                if (dwValueSize < (UINT)256)
                                {
                                        ASSERT(_tcslen((LPCSTR)pvValue) <= dwValueSize);
                                        strDescription = (LPCSTR)pvValue;
                                }
                        }
                        else if (retval)
                        {
                                ASSERT(0);  // no description for this package
                                // retval = FALSE;
                        }
                }

                delete [] pvVerInfo;
        }
        return retval;
}

///////////////////////////////////////////////////////////////////////////////
// Log IDE startup to remote server
//
#ifdef _IDELOG
//#pragma message ("ALERT! : initexit.cpp : Including IDE startup logging for QA...")
void CTheApp::InitIDEstartupLog()
{
        // Send the IDE startup over the net to the database.
        // Just use a hard-coded file name as this is completely
        // internal.
        static char szLogFileName[] = "\\\\dtqalogs\\db\\editgo\\idestart.dat";
        const int maxRetries = 1;
        HANDLE hFile ;

        for (int i = 0; i < maxRetries ; i++ )
        {
                if ( (hFile = CreateFile(szLogFileName,GENERIC_READ|GENERIC_WRITE,
                        FILE_SHARE_READ,NULL,OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,NULL)
                        ) != INVALID_HANDLE_VALUE )
                {
                        break;
                }
                Sleep(300); // wait 0.3 seconds if it failed.
        }
        if ( i == maxRetries )
                return ; // No luck couldn't open the file.

        // Start writing from the end of the file.
        SetFilePointer(hFile,0,NULL,FILE_END);

        // Write out the relevant information to the file in the
        // following format.
        // 10/6/93, COLINT3, Debug or Release IDE, Chicago or Daytona,
        // Elapsed build time
        const DWORD MAX_SIZE = 512;
        char buffer[MAX_SIZE]; // Buffer to hold intermediate values.
        char logString[MAX_SIZE * 2]; // Complete string.
        DWORD  cBytesWritten ;

        logString[0] = '\n';
        _strdate(logString + 1); // get current date in mm/dd/yy form.
        lstrcat(logString,",");

        _strtime(logString + lstrlen(logString));
        lstrcat(logString,",");

        // Computer name.
        GetComputerName(buffer,(LPDWORD)&MAX_SIZE);
        lstrcat(logString,buffer);
        lstrcat(logString,",");

#ifdef _DEBUG
        lstrcat(logString, "Debug IDE,");
#else
        lstrcat(logString, "Release IDE,");
#endif

        if (theApp.m_bWin95)
                lstrcat(logString, "Chicago,");
        else
                lstrcat(logString, "Daytona,");

        if (theApp.m_fOEM_MBCS)
                lstrcat(logString, "Multi-byte OS");
        else
                lstrcat(logString, "Single-byte OS");


        WriteFile(hFile,logString, lstrlen(logString),&cBytesWritten,NULL);

        // We are all done, close the file.
        CloseHandle(hFile);
}
#endif // _IDELOG

BOOL CTheApp::InitStdCompMgr()
{
    BOOL fRet = FALSE;
    COleRef<IMsoStdComponentMgr>  srpStdCompMgr;
    COleRef<IMsoComponentManager>  srpMsoCompMgr;

        ASSERT(m_pAppCmpHost == NULL);
        m_pAppCmpHost = new CIPCompHostApp;
        ASSERT(m_pAppCmpHost != NULL);

    // Create a standard Component Manager and register it
    // as the thread Component Manager.  Then hand its
    // IMsoComponentManager interface to the main window.
    //
        if (MsoFCreateStdComponentManager(NULL,
                m_pAppCmpHost->GetIMsoComponentHost(),
                TRUE,
                IID_IMsoStdComponentMgr,
                (void **)&srpStdCompMgr) &&
        SUCCEEDED(srpStdCompMgr->QueryInterface(IID_IMsoComponentManager, (void **)&srpMsoCompMgr)))
    {
                ASSERT(m_pMainWnd);
                ASSERT(m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)));
                fRet = ((CMainFrame *)m_pMainWnd)->SetComponentManager(srpMsoCompMgr);

        if (fRet)
                        m_pMsoStdCompMgr = srpStdCompMgr.Disown();
        }
        return fRet;
}

void CTheApp::TermStdCompMgr()
{
        if (m_pAppCmpHost)
                m_pAppCmpHost->InternalRelease();

        if (m_pMsoStdCompMgr)
        {
                m_pMsoStdCompMgr->Terminate(TRUE);
                MsoFSetComponentManager(NULL);
                OleRelease(m_pMsoStdCompMgr);
        }
}


/////////////////////////////////////////////////////////////////////////////
// Implementation of IMsoComponentHost

BEGIN_INTERFACE_MAP(CIPCompHostApp, CCmdTarget)
INTERFACE_PART(CIPCompHostApp, IID_IMsoComponentHost, MsoComponentHost)
END_INTERFACE_MAP()

CIPCompHostApp::CIPCompHostApp()
{
}

HRESULT CIPCompHostApp::XMsoComponentHost::QueryService(REFGUID guidService, REFIID riid, void **ppvObj)
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)

        *ppvObj = NULL;
        return theApp.GetService(guidService, riid, ppvObj);
}

STDMETHODIMP_(BOOL) CIPCompHostApp::XMsoComponentHost::FDebugMessage(HMSOINST hinst, UINT message,
                                                                                                                                         WPARAM wParam, LPARAM lParam)
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
        return TRUE;
}

MSOMETHODIMP_(void) CIPCompHostApp::XMsoComponentHost::OnComponentActivate(IMsoComponent *pic,
                                                                                                                                                   const MSOCRINFO *pcrinfo, BOOL fTracking)
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
                // Since there's always an active component (the MDI frame),
                // this doesn't need to keep track of whether there is an
                // active component.
}

MSOMETHODIMP_(void) CIPCompHostApp::XMsoComponentHost::OnComponentEnterState(ULONG uStateId, BOOL fEnter)
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
        //$UNDONE, disable/enable non-component top-level modeless windows

}

MSOMETHODIMP_(BOOL) CIPCompHostApp::XMsoComponentHost::FContinueIdle()
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)

        MSG msg;
        return(!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
}

MSOMETHODIMP_(BOOL) CIPCompHostApp::XMsoComponentHost::FPushMessageLoop(ULONG uReason)
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
                return ((CTheApp *)AfxGetApp())->MainLoop(NULL, TRUE, uReason);
}

ULONG CIPCompHostApp::XMsoComponentHost::AddRef()
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
        return pThis->ExternalAddRef();
}

ULONG CIPCompHostApp::XMsoComponentHost::Release()
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
        return pThis->ExternalRelease();
}

HRESULT CIPCompHostApp::XMsoComponentHost::QueryInterface(REFIID riid, void **ppvObj)
{
        METHOD_PROLOGUE(CIPCompHostApp, MsoComponentHost)
        return pThis->ExternalQueryInterface(&riid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of CIPCompMessageFilter

BEGIN_INTERFACE_MAP(CIPCompMessageFilter, COleMessageFilter)
INTERFACE_PART(CIPCompMessageFilter, IID_IServiceProvider, ServiceProvider)
END_INTERFACE_MAP()

CIPCompMessageFilter::CIPCompMessageFilter()
{
}

ULONG CIPCompMessageFilter::XServiceProvider::AddRef()
{
        METHOD_PROLOGUE(CIPCompMessageFilter, ServiceProvider)
        return pThis->ExternalAddRef();
}

ULONG CIPCompMessageFilter::XServiceProvider::Release()
{
        METHOD_PROLOGUE(CIPCompMessageFilter, ServiceProvider)
        return pThis->ExternalRelease();
}

HRESULT CIPCompMessageFilter::XServiceProvider::QueryInterface(REFIID riid, void **ppvObj)
{
        METHOD_PROLOGUE(CIPCompMessageFilter, ServiceProvider)
        return pThis->ExternalQueryInterface(&riid, ppvObj);
}

HRESULT CIPCompMessageFilter::XServiceProvider::QueryService(REFGUID guidService, REFIID riid, void **ppvObj)
{
        METHOD_PROLOGUE(CIPCompMessageFilter, ServiceProvider)

        *ppvObj = NULL;
        return theApp.GetService(guidService, riid, ppvObj);
}

BOOL InitOle()
{
        _AFX_THREAD_STATE* pState = AfxGetThreadState();
        ASSERT(!pState->m_bNeedTerm);    // calling it twice?

        // first, initialize OLE
        SCODE sc = ::OleInitialize(NULL);
        if (FAILED(sc))
        {
                // warn about non-NULL success codes
                TRACE1("Warning: OleInitialize returned scode = %s.\n",
                        AfxGetFullScodeString(sc));
                goto InitFailed;
        }
        // termination required when OleInitialize does not fail
        pState->m_bNeedTerm = TRUE;

        // hook idle time and exit time for required OLE cleanup
        CWinThread* pThread; pThread = AfxGetThread();
        pThread->m_lpfnOleTermOrFreeLib = AfxOleTermOrFreeLib;

        // allocate and initialize default message filter
        if (pThread->m_pMessageFilter == NULL)
        {
                pThread->m_pMessageFilter = new CIPCompMessageFilter;
                ASSERT(AfxOleGetMessageFilter() != NULL);
                AfxOleGetMessageFilter()->Register();
        }

        return TRUE;

InitFailed:
        AfxOleTerm();
        return FALSE;
}

static BOOL GetCmdLineProfile (LPCTSTR lpszProfileName, CStringList& argList, POSITION posInsertAfter)
{
        // Parameter validation
        if (lpszProfileName == NULL || posInsertAfter == NULL)
        {
                ASSERT(FALSE);
                return FALSE;
        }

        POSITION        pos             = posInsertAfter;
        HKEY            hKey    = NULL;

        // Construct the reg path
        CString         strRegPath;
        strRegPath.Format(_T("%s\\%s\\%s\\Profiles\\%s\\Args"), g_szKeyRoot,
                g_szCompanyName, theApp.GetExeString(DefaultRegKeyName), lpszProfileName);

        // Open the reg key
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strRegPath, 0, KEY_READ, &hKey)
                == ERROR_SUCCESS)
        {
                // Get the length of the longest value
                DWORD           cbMaxValueLen;

                if (RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL,
                        NULL, NULL, &cbMaxValueLen, NULL, NULL)
                        == ERROR_SUCCESS)
                {
                        // Loop through the Arg# entries under the specified profile key
                        CString         strValue;
                        CString         strArg;
                        DWORD           dwType;
                        DWORD           cbBuffer;

                        for (int iArg = 0; ; iArg++)
                        {
                                // Get the next arg
                                strValue.Format(_T("Arg%d"), iArg);
                                cbBuffer = cbMaxValueLen + sizeof(TCHAR);

                                if (RegQueryValueEx(hKey, strValue, NULL, &dwType,
                                        (LPBYTE) strArg.GetBuffer(cbBuffer/sizeof(TCHAR)), &cbBuffer)
                                        != ERROR_SUCCESS)
                                {
                                        break;
                                }
                                strArg.ReleaseBuffer();

                                // If string and non-empty, add arg to master list
                                if (dwType == REG_SZ && !strArg.IsEmpty())
                                {
                                        pos = argList.InsertAfter(pos, strArg);
                                }
                        }
                }
        }

        // If the position hasn't changed, we didn't find the profile
        return (pos != posInsertAfter);
}
