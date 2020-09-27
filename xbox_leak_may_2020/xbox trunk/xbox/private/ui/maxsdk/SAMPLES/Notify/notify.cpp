//-----------------------------------------------------------------------------
// Notify.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <direct.h>
#include "Notify.h"
#include "notifdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
// Globals

	//MAPILOGON*			lpMapiLogon;
	//MAPILOGOFF*			lpMapiLogoff;
	//MAPISENDMAIL*			lpMapiSendMail;
	//MAPISENDDOCUMENTS*	lpMapiSendDocuments;
	//MAPIDETAILS*			lpMapiDetails;

//-----------------------------------------------------------------------------
// CNotifyApp

BEGIN_MESSAGE_MAP(CNotifyApp, CWinApp)
	//{{AFX_MSG_MAP(CNotifyApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
// CNotifyApp construction

CNotifyApp::CNotifyApp() {
	
	//-- Set where the .INI file is located
	
	TCHAR szCurDir[_MAX_DIR];
	_getcwd(szCurDir,_MAX_DIR);
	m_sINIname.Format(_T("%s\\Notify.ini"),szCurDir);

}

//-----------------------------------------------------------------------------
// CNotifyApp destruction

CNotifyApp::~CNotifyApp() {
	//if (hlibMAPI) {
	//::FreeLibrary(hlibMAPI);
	//hlibMAPI = 0;
	//}
}
//-----------------------------------------------------------------------------
// The one and only CNotifyApp object

CNotifyApp	theApp;
CNotifyApp* theAppPtr = &theApp;

//-----------------------------------------------------------------------------
// CNotifyApp initialization

BOOL CNotifyApp::InitInstance() {

	AfxEnableControlContainer();

	#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
	#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif

	//-------------------------------------------------------------------------
	//-- Load INI Configuration

	TCHAR buffer[_MAX_DIR];
	
	GetPrivateProfileString(iniMAILTARGETS, iniFAILURE,"",
		(char*)&buffer,sizeof(buffer),m_sINIname);
	if (buffer[0])
		failureTarget = buffer;
	GetPrivateProfileString(iniMAILTARGETS, iniPROGRESS,"",
		(char*)&buffer,sizeof(buffer),m_sINIname);
	if (buffer[0])
		progressTarget = buffer;
	GetPrivateProfileString(iniMAILTARGETS, iniCOMPLETION,"",
		(char*)&buffer,sizeof(buffer),m_sINIname);
	if (buffer[0])
		completionTarget = buffer;

	//GetPrivateProfileString(iniMAILSOURCE, iniFROM,"",
	//	(char*)&buffer,sizeof(buffer),m_sINIname);
	//if (buffer[0])
	//	fromAddress = buffer;
	
	//-------------------------------------------------------------------------
	//-- Check Command Line Arguments

	CString	alertFile;

	if (__argc == 3) {
		
		//-- Get Arguments --------------------------------
		
		alertFile	= __argv[1];
		alertFlags	= (DWORD)(atoi(__argv[2]));
	
		//-- Setup Email Notification ---------------------

		CString notifyType;

		switch (alertFlags) {
			case NOTIFY_FAILURE:
				sndPlaySound(failureTarget,SND_SYNC);
				break;
			case NOTIFY_PROGRESS:
				sndPlaySound(progressTarget,SND_SYNC);
				break;
			case NOTIFY_COMPLETION:
				sndPlaySound(completionTarget,SND_SYNC);
				break;
		}

		return FALSE;
	
	}

	//-------------------------------------------------------------------------
	//-- Standard initialization (Running Interactively)

	CNotifyDlg	dlg;
	m_pMainWnd	= &dlg;
	
	dlg.m_CompletionTarget	= completionTarget;
	dlg.m_ProgressTarget	= progressTarget;
	dlg.m_FailureTarget		= failureTarget;
	//dlg.m_FromAddress		= fromAddress;

	if (dlg.DoModal() == IDOK) {

		//-- Save Settings

		WritePrivateProfileString(iniMAILTARGETS, iniFAILURE,
			dlg.m_FailureTarget, m_sINIname);
		WritePrivateProfileString(iniMAILTARGETS, iniPROGRESS,
			dlg.m_ProgressTarget, m_sINIname);
		WritePrivateProfileString(iniMAILTARGETS, iniCOMPLETION,
			dlg.m_CompletionTarget, m_sINIname);
		//WritePrivateProfileString(iniMAILSOURCE, iniFROM,
		//	dlg.m_FromAddress, m_sINIname);
	}

	//-- Since the dialog has been closed, return FALSE so that we exit the
	//-- application, rather than start the application's message pump.
	
	return FALSE;

}

#if 0

//-----------------------------------------------------------------------------
// CNotifyApp::SendMail()

BOOL CNotifyApp::SendMail( ) {

	static CString maxSubject = "3D Studio Max Notification";
	static CString maxBody;

	if (!InitMapi())
		return FALSE;

	//-- Init Mail Subsystem

	LHANDLE lhSession;
	ULONG res = (*lpMapiLogon)(0,NULL,NULL,0,0,(LPLHANDLE)&lhSession);
	if (res != SUCCESS_SUCCESS)
		return (FALSE);

	//-- Define Target

	CString target;

	switch (alertFlags) {
		case NOTIFY_PROGRESS:	target = progressTarget;	break;
		case NOTIFY_COMPLETION:	target = completionTarget;	break;
		case NOTIFY_FAILURE:
		default:				target = failureTarget;		break;
	}

	//-- Prepare Message Body

	maxBody	=	"3D Studio Max Notification Report\n\n";
	maxBody +=	"Message\n";

	//-- Prepare Message "From" Address

	CString sourceAddress;
	sourceAddress.Format("SMTP:%s",fromAddress);
	MapiRecipDesc sourceDesc;
	memset(&sourceDesc,0,sizeof(MapiRecipDesc));
	sourceDesc.ulRecipClass	= MAPI_ORIG;
	sourceDesc.lpszAddress	= (char *)(void *)LPCTSTR(sourceAddress);

	//-- Prepare Message "TO" Address

	CString targetAddress;
	targetAddress.Format("SMTP:%s",target);
	MapiRecipDesc targetDesc[2];
	memset(&targetDesc[0],0,sizeof(MapiRecipDesc) * 2);
	targetDesc[0].ulRecipClass	= MAPI_TO;
	targetDesc[0].lpszAddress	= (char *)(void *)LPCTSTR(targetAddress);

	//-- Prepeare Message Header

	MapiMessage msg;
	memset(&msg,0,sizeof(MapiMessage));
	msg.lpszSubject		= (char *)(void *)LPCTSTR(maxSubject);
	msg.lpszNoteText	= (char *)(void *)LPCTSTR(maxBody);
	msg.lpOriginator	= &sourceDesc;
	msg.nRecipCount		= 1;
	msg.lpRecips		= targetDesc;

	//-- Send the thing
	
	ULONG result = (*lpMapiSendMail)(lhSession,0,&msg,MAPI_DIALOG,0);
	
	(*lpMapiLogoff)(lhSession,0,0,0);

	if (result == SUCCESS_SUCCESS)
		return(TRUE);
	else
		return (FALSE);

}

//-----------------------------------------------------------------------------
// CNotifyApp::InitMapi()

BOOL CNotifyApp::InitMapi(void) {

    UINT fuError;
    if(!MapiInstalled())
        return FALSE;

    fuError		= ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
    hlibMAPI	= ::LoadLibraryEx("mapi32.dll", NULL, 0);
    ::SetErrorMode(fuError);
    
	if (!hlibMAPI)
	    return (FALSE);

	if (!(lpMapiLogon			= (MAPILOGON *)GetProcAddress (hlibMAPI,"MAPILogon")))
		return (FALSE);
    if (!(lpMapiLogoff			= (MAPILOGOFF *)GetProcAddress (hlibMAPI,"MAPILogoff")))
		return (FALSE);
    if (!(lpMapiSendMail		= (MAPISENDMAIL *)GetProcAddress (hlibMAPI,"MAPISendMail")))
		return (FALSE);
    if (!(lpMapiSendDocuments	= (MAPISENDDOCUMENTS *)GetProcAddress (hlibMAPI,"MAPISendDocuments")))
		return (FALSE);

    return (TRUE);

}

//-----------------------------------------------------------------------------
// CNotifyApp::MapiInstalled()

BOOL CNotifyApp::MapiInstalled(void) {
    OSVERSIONINFO osvinfo;
    LONG lr;
    HKEY hkWMS;
    #define MAPIVSize 8
    unsigned char szMAPIValue[MAPIVSize];
    DWORD dwType;
    DWORD cbMAPIValue = MAPIVSize;
    osvinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&osvinfo))
		return FALSE;
	if( osvinfo.dwMajorVersion > 3 || (osvinfo.dwMajorVersion == 3 && osvinfo.dwMinorVersion > 51)) {
        lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows Messaging Subsystem",0, KEY_READ, &hkWMS);
        if(ERROR_SUCCESS == lr) {
            lr = RegQueryValueEx(hkWMS,"MAPI",0,&dwType,szMAPIValue, &cbMAPIValue);
            RegCloseKey(hkWMS);
            if(ERROR_SUCCESS == lr) {
                //Assert(dwType == REG_SZ);
                if(lstrcmp((char *)(void *)szMAPIValue, "1") == 0)
                    return TRUE;
            }
        }
        return FALSE;
    }
    return GetProfileInt("Mail","MAPI",0);
}

#endif