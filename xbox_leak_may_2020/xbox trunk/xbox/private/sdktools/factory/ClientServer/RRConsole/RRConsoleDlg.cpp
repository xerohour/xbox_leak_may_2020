//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
// RRConsoleDlg.cpp : 
//
/*++
 Copyright (c) 2001, Microsoft.

    Author:  Dennis Krueger [a-denkru]
    Date:    6/26/2001

    Description:
        This implements the primary functions of the RR Console.  The RRConsole is
		a client to the XBRRServer which serves commands on the XBox and accesses
		a private share named \\TestServer\Data\Working to access Genealogy data.
		This program takes genealogy data, generates a new EEProm image based on that
		data and updates the XBox using packetized commands to the XBox.

--*/
//===================================================================

#include "stdafx.h"
#include "RRConsole.h"
#include "RRConsoleDlg.h"
#include <string>

#include <av.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SUCCESS 0



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

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
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
// CRRConsoleDlg dialog

CRRConsoleDlg::CRRConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRRConsoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRRConsoleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pBuffer = NULL;
}

void CRRConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRRConsoleDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRRConsoleDlg, CDialog)
	//{{AFX_MSG_MAP(CRRConsoleDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ConnectXBOX, OnConnectXBOX)
	ON_BN_CLICKED(IDC_HDReplace, OnHDReplace)
	ON_BN_CLICKED(IDC_VerifyXBox, OnCompleteXBox)
//	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_BN_CLICKED(IDC_VerifyOnly, OnVerifyOnly)
	ON_BN_CLICKED(IDRESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRRConsoleDlg message handlers

BOOL CRRConsoleDlg::OnInitDialog()
{
	CFont TitleFont;
	
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// setup the localized button strings
	CString TempStr;
	TempStr.LoadString(IDS_SERNO);
	SetDlgItemText(IDC_SERNO,TempStr);
	TempStr.LoadString(IDS_CONNEXTXBOX);
	SetDlgItemText(IDC_ConnectXBOX,TempStr);
	TempStr.LoadString(IDS_CANCEL);
	SetDlgItemText(IDRESET,TempStr);
	TempStr.LoadString(IDS_QUIT);
	SetDlgItemText(IDCANCEL,TempStr);
	TempStr.LoadString(IDS_UPDATE);
	SetDlgItemText(IDC_HDReplace,TempStr);
	TempStr.LoadString(IDS_LOCK);
	SetDlgItemText(IDC_VerifyXBox,TempStr);
	TempStr.LoadString(IDS_LOCKONLY);
	SetDlgItemText(IDC_VerifyOnly,TempStr);


// fixup font for title line
	TitleFont.CreateFont(
				12,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_BOLD,	               // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"Arial");                 // lpszFacename

	GetDlgItem(IDC_TITLE)->SetFont(&TitleFont);
	TempStr.LoadString(IDS_TITLE);
	SetDlgItemText(IDC_TITLE,TempStr);


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// set focus to serial number entry
	GetDlgItem(IDC_SerialNumber)->EnableWindow(TRUE);  // Disabled until connected
	GetDlgItem(IDC_SerialNumber)->SetFocus();
	m_pbmRed = new CBitmap;
	m_pbmRed->LoadBitmap(IDB_RED); // setup red stoplight
	return FALSE;  // return TRUE  unless you set the focus to a control
}


void CRRConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRRConsoleDlg::OnPaint() 
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
HCURSOR CRRConsoleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


CRRConsoleDlg::ShowErrorStr(UINT nID)
{
	UINT IconType;
	CString ErrString,CaptionString;
	ErrString.LoadString(nID);
	if(IDS_SUCCESS == nID || IDS_SUCCESSFULOP == nID)
	{
		IconType = MB_ICONINFORMATION;
		CaptionString.LoadString(IDS_CAPOK);
	} else
	{
		IconType = MB_ICONSTOP;
		CaptionString.LoadString(IDS_CAPFAIL);
	}
	
	MessageBox(ErrString,CaptionString,IconType);
	
}

char * FindTaggedData(char * pInputBuff,char * szTargetStr)
{
	char * pTemp;
	int iResult;
	int iLimit = strlen(pInputBuff);
	for(int i = 0; i < iLimit; i++)
	{
		if('<' == pInputBuff[i])
		{
			// found the start of a tag
			if('/' == pInputBuff[i+1])
				continue; // end of a tagged field
			iResult = strnicmp(&pInputBuff[i+1],szTargetStr,strlen(szTargetStr));
			if(0 == iResult)
			{
				// found the tag, copy the data for the client
				pTemp = pInputBuff + i + strlen(szTargetStr) + 1;
				return pTemp;
			}
		}
	}
	return NULL;
}


BOOL GetGenString(char *pInputBuff,char *pOutputBuff,int BuffSize,char * szTargetStr)
{
	int iLimit = strlen(pInputBuff);
	char * pTemp;
	BOOL	fResult = FALSE;
	pTemp = FindTaggedData(pInputBuff, szTargetStr);
	if(NULL == pTemp)
		return FALSE;
	while(*pTemp != '<')
	{
		*pOutputBuff++ = *pTemp++;
	}
	*pOutputBuff = 0x00; // delemit string
	return TRUE;
}

#define HDKEYSTR		"HDDKey>"
#define GAMEREGIONSTR	"GameRegionCode>"
#define AVREGIONSTR		"TVRegionCode>"
#define MACADDRSTR		"MACAddr>"

DWORD CRRConsoleDlg::GetGenealogyData(char * pSerialNumber)
{

	char	cHDKey[100];
	char	cAVRegion[20];
	char	cGameRegion[20];
	char	cMACAddr[20];
	CNetErrors cnError;
	BOOL	fResult;
	DWORD	dwFileSize;
	HANDLE  hGenSource;

	CString GenSourcePath;
	GenSourcePath = m_GenSource;
	BOOL	fTryAgain = TRUE;

	// returns here once if we need to try the alternate source path
GetGen:

	if("\\" != GenSourcePath.Right(1))
	{
		GenSourcePath += "\\";
	}
	GenSourcePath += pSerialNumber;
	GenSourcePath += ".xml";
	
	// make access path name

	hGenSource = CreateFile(GenSourcePath,
						GENERIC_READ | GENERIC_WRITE,
						0,				// don't share
						NULL,			// don't inherit security
						OPEN_EXISTING,	// must exist
						FILE_ATTRIBUTE_NORMAL, // nothing special about the file
						NULL			// No template
						);

	
	if(INVALID_HANDLE_VALUE == hGenSource 
						&& TRUE == fTryAgain
						&& TRUE == m_fAltExists)
	{
		// try alternate
		GenSourcePath = m_GenAltSource;
		fTryAgain = FALSE;
		goto GetGen;
	}
		
	if(INVALID_HANDLE_VALUE == hGenSource)
	{
		DWORD dwResult = GetLastError();
		if(ERROR_BAD_NETPATH == dwResult)
		{
			return IDS_NETWORKERROR;
		} else
		{
			return IDS_NOGENREC;
		}
	}
	
	dwFileSize = GetFileSize(hGenSource,NULL); // get the file size
	m_pBuffer = (char *) malloc(dwFileSize+10); // allocate buffer for file
	if(NULL == m_pBuffer)
	{
		// memory allocation failed, return error
		return IDS_NOMEM;
	}
	// read xml-formatted genealogy data into buffer
	fResult = ReadFile(hGenSource,m_pBuffer,dwFileSize,&m_dwFileSize,NULL);
	if(FALSE == fResult)
	{
		CloseHandle(hGenSource);
		free(m_pBuffer);
		m_pBuffer = NULL;
		return IDS_NOGENREC;
	}

	CloseHandle(hGenSource); // close the handle now so we don't have to do so in all the error cases
							// we'll reopen it for the write after successful completion
	
	fResult = GetGenString(m_pBuffer,cHDKey,sizeof(cHDKey),HDKEYSTR);
	if(FALSE == fResult)
	{
		return IDS_BADPARSE;
	}

	fResult = GetGenString(m_pBuffer,cGameRegion,sizeof(cGameRegion),GAMEREGIONSTR);
	if(FALSE == fResult)
	{
		return IDS_BADPARSE;
	}
	fResult = GetGenString(m_pBuffer,cAVRegion,sizeof(cAVRegion),AVREGIONSTR);
	if(FALSE == fResult)
	{
		return IDS_BADPARSE;
	}
	fResult = GetGenString(m_pBuffer,cMACAddr,sizeof(cMACAddr),MACADDRSTR);
	if(FALSE == fResult)
	{
		return IDS_BADPARSE;
	}
	// got the necessary values, copy into local GenData Structure

	strncpy(m_GenData.cSerialNumber,pSerialNumber,13); // serial number string
	
	char ** ppStopString = NULL;
	ULONG ulTemp = strtoul(cGameRegion,ppStopString,16); // convert GameRegion
	m_GenData.dwGameRegion = ulTemp;

	ulTemp = strtoul(cAVRegion,ppStopString,16); // convert avregion
	m_GenData.dwAVRegion = ulTemp;

	sscanf(cMACAddr, "%2x",	&m_GenData.bMACAddress[0]);
	sscanf(&cMACAddr[2], "%2x",	&m_GenData.bMACAddress[1]);
	sscanf(&cMACAddr[4], "%2x",	&m_GenData.bMACAddress[2]);
	sscanf(&cMACAddr[6], "%2x",	&m_GenData.bMACAddress[3]);
	sscanf(&cMACAddr[8], "%2x",	&m_GenData.bMACAddress[4]);
	sscanf(&cMACAddr[10], "%2x",	&m_GenData.bMACAddress[5]);

	strncpy(m_GenData.bHDKey,cHDKey,strlen(cHDKey));
	

	cnError = CNetSuccess;
	return cnError;
}


void CRRConsoleDlg::OnConnectXBOX() 
{
	// Set connect text and bmp
	CBitmap * pBitMap;

	GetDlgItem(IDC_ConnectXBOX)->EnableWindow(FALSE);	// disable button so no double-clicks
	pBitMap = new CBitmap;
	pBitMap->LoadBitmap(IDB_YELLOW); 
	((CStatic *)GetDlgItem(IDC_STOPNGO))->SetBitmap(*pBitMap);
	delete pBitMap;

	// setup source and update directory paths
	// build genealogy source and destination paths

	// first source path in form \\servername\data\working
	DWORD dwResult;
	dwResult = GetPrivateProfileString(
					"ClientConfig",		// section
					"GenSourcePath",		// key name caseinsensitive
					"Fail",		// Default Name
					m_GenSource,		// targer buffer
					sizeof(m_GenSource),
					IniName				// needs to be in Windows directory
					);

	if(0 == dwResult || !strcmp(m_GenSource,"Fail"))
	{
		((CStatic *)GetDlgItem(IDC_STOPNGO))->SetBitmap(*m_pbmRed);
		CString Errmsg;
		Errmsg.LoadString(IDS_FILENOTFOUND);
		Errmsg += " "; Errmsg += IniName;
		MessageBox(Errmsg,NULL,MB_OK);
		GetDlgItem(IDC_ConnectXBOX)->EnableWindow(TRUE);	// enable button so no double-clicks
		return;
	}

	// next get alternate source path if exists
	m_fAltExists = TRUE; 
	dwResult = GetPrivateProfileString(
					"ClientConfig",		//  section
					"GenAltSourcePath",		// key name caseinsensitive
					"Fail",		// Default Name
					m_GenAltSource,		// targer buffer
					sizeof(m_GenAltSource),
					IniName				// needs to be in Windows directory
					);

	if(0 == dwResult || !strcmp(m_GenSource,"Fail"))
	{
		m_fAltExists = FALSE;
	}

	// get update path in form \\servername\data\working
	dwResult = GetPrivateProfileString(
					"ClientConfig",		// No section
					"GenUpdatePath",		// key name caseinsensitive
					"Fail",		// Default Name
					m_GenUpdate,		// targer buffer
					sizeof(m_GenUpdate),
					IniName				// needs to be in Windows directory
					);

	if(0 == dwResult || 0x00 == m_GenUpdate[0] || !strcmp(m_GenSource,"Fail"))
	{
		
		((CStatic *)GetDlgItem(IDC_STOPNGO))->SetBitmap(*m_pbmRed);
		CFont WarningFont;
		WarningFont.CreateFont(
					16,                        // nHeight
					0,                         // nWidth
					0,                         // nEscapement
					0,                         // nOrientation
					FW_BOLD,	               // nWeight
					FALSE,                     // bItalic
					FALSE,                     // bUnderline
					0,                         // cStrikeOut
					ANSI_CHARSET,              // nCharSet
					OUT_DEFAULT_PRECIS,        // nOutPrecision
					CLIP_DEFAULT_PRECIS,       // nClipPrecision
					DEFAULT_QUALITY,           // nQuality
					DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
					"Arial");                 // lpszFacename

		m_fNoUpdate = TRUE;  // set no update to TRUE
		GetDlgItem(IDC_UPDATEWARNING)->SetFont(&WarningFont);
		CString TempStr;
		TempStr.LoadString(IDS_UPDATEWARNING);
		SetDlgItemText(IDC_UPDATEWARNING,TempStr);
		GetDlgItem(IDC_UPDATEWARNING)->Invalidate(TRUE);
	}

	// initialize connection to XBox
	m_szXBIPString[0] = 0x00; // null string
	CNetErrors InitError = m_XBServer.InitXB(m_szXBIPString);
	// check CNet initialization errors and pass to dialog
	if(InitError != CNetSuccess)
	{
		((CStatic *)GetDlgItem(IDC_STOPNGO))->SetBitmap(*m_pbmRed);
		ShowErrorStr(CNetErrorIDs[InitError]);
		GetDlgItem(IDC_ConnectXBOX)->EnableWindow(TRUE);	// re-enable button so no double-clicks
		return;
	}

	GetDlgItem(IDC_SerialNumber)->EnableWindow(TRUE);	// Enable once connected
	GetDlgItem(IDC_SerialNumber)->SetFocus();			// set focus to serial number
	GetDlgItem(IDC_HDReplace)->EnableWindow(TRUE);		// enable hdkey button
	GetDlgItem(IDC_VerifyXBox)->EnableWindow(TRUE);		// Enable Commit button
	GetDlgItem(IDRESET)->EnableWindow(TRUE);
	GetDlgItem(IDC_VerifyOnly)->EnableWindow(TRUE);
	pBitMap = new CBitmap;
	pBitMap->LoadBitmap(IDB_GREEN); 
	((CStatic *)GetDlgItem(IDC_STOPNGO))->SetBitmap(*pBitMap);
	delete pBitMap; // release bitmap
	return;
}



void CRRConsoleDlg::OnHDReplace() 
{
	DATA_PACKET Packet;
	DWORD dwResult;
	CNetErrors cnError;
	HRESULT hResult;
    BYTE OutputBuffer[512];
    DWORD dwBufferSize = 512;
 	char SerialNumber[30];


	// get serial number
	int CharCount = ((CEdit *)GetDlgItem(IDC_SerialNumber))->GetLine(0,SerialNumber,sizeof(SerialNumber));
	SerialNumber[12] = 0x00; // ensure terminated string, truncate an <cr>s
	if(0 == CharCount)
	{
		// no serial number, fail
		ShowErrorStr(IDS_NOSERIAL);
		return;
	}
	if(CharCount < 12)
	{
		ShowErrorStr(IDS_INVALSERIAL);
		CEdit * pSerEdit = (CEdit *) GetDlgItem(IDC_SerialNumber);  // get handle to Serial Number edit box
		pSerEdit->SetSel(0,-1) ; // select all the characters
		pSerEdit->Clear();
		pSerEdit->SetFocus();			// set focus to serial number

		return;
	}

	dwResult = GetGenealogyData(SerialNumber);
	if(SUCCESS != dwResult)
	{
		// show error dialog
		ShowErrorStr(dwResult);
		return;
	}

	// create eeprom image
	hResult = GeneratePerBoxDataEx(
		m_GenData.bHDKey,
		m_GenData.dwGameRegion,
		m_GenData.dwAVRegion,
		m_GenData.bMACAddress,
		m_GenData.cSerialNumber,
		m_GenData.bHDKey,
		m_GenData.cRecoveryKey,
		&m_GenData.dwOnlineKeyVersion,
		m_GenData.cOnlineKey,
		OutputBuffer,
		&dwBufferSize
		);

	if(SUCCESS != hResult)
	{
		// show error dialog
		ShowErrorStr(IDS_BADGEN);
		return;
	}
	Packet.dwCommand = DO_WRITEPERBOXDATA;
	memcpy(Packet.byteData,OutputBuffer,dwBufferSize);
	Packet.ulDataSize = dwBufferSize;
	cnError = SendVerifyXBoxCommand(&Packet);
	if(CNetSuccess != cnError)
	{
		ShowErrorStr(CNetErrorIDs[cnError]);
		return;
	}

	
	if(FALSE == m_fNoUpdate)
	{
		// Update genealogy database if update path was found in .ini file
		dwResult = UpdateGenealogy();
	}

	if(dwResult)
	{
		ShowErrorStr(dwResult);
		return;
	}

	ResetDlgButtons();  // end of processing wrapup
	ShowErrorStr(IDS_SUCCESSFULOP);
	return;

}


CNetErrors CRRConsoleDlg::SendVerifyXBoxCommand(PDATA_PACKET pPacket)
{
	DATA_PACKET RPacket;
	CNetErrors cnError;
	
	cnError = m_XBServer.SendData(m_XBServer.m_sXBClient,pPacket,sizeof(DATA_PACKET));

	if(CNetSuccess != cnError)
	{
		return SendFailed;
	}

	cnError = m_XBServer.RecvData(m_XBServer.m_sXBClient,&RPacket,sizeof(DATA_PACKET));

	if(CNetSuccess != cnError)
	{
		return ReceiveFailed;
	}

	if(RPacket.dwCommand != pPacket->dwCommand || RPacket.dwStatus != 0)
	{
		return XBCommandFailed;
	}
	return CNetSuccess;
}

// assumes that the m_GenData has the current Genealogy

#define ONLINEKEYSTR "OnLineKey>"
#define RECOVERYKEYSTR "RecoveryKey>"

DWORD CRRConsoleDlg::UpdateGenealogy()
{

	char *	pTemp;
	BOOL	fResult;
	DWORD	dwBytesWritten;

	HANDLE  hGenData;
	// need to replace online key, recovery key, date

	// build up xml file
	
	// locate online key tag in buffer
	pTemp = FindTaggedData(m_pBuffer,ONLINEKEYSTR);
	if(NULL == pTemp)
	{
		return IDS_BADPARSE;
	}

	memcpy(pTemp,m_GenData.cOnlineKey,sizeof(m_GenData.cOnlineKey)-1);
	
	pTemp = FindTaggedData(m_pBuffer,RECOVERYKEYSTR);
	if(NULL == pTemp)
	{
		return IDS_BADPARSE;
	}
	memcpy(pTemp,m_GenData.cRecoveryKey,4);
	// write back to gendata directory
	// reopen the file
	// rebuild path
	CString sGenDataPath(m_GenUpdate);
	if("\\" != sGenDataPath.Right(1))
	{
		sGenDataPath += "\\";
	}
	sGenDataPath += m_GenData.cSerialNumber;
	sGenDataPath += ".xml";
	hGenData = CreateFile(sGenDataPath,
						GENERIC_READ | GENERIC_WRITE,
						0,				// don't share
						NULL,			// don't inherit security
						OPEN_EXISTING,	// must exist
						FILE_ATTRIBUTE_NORMAL, // nothing special about the file
						NULL			// No template
						);

	
	if(INVALID_HANDLE_VALUE == hGenData)
	{
		DWORD dwResult = GetLastError();
		if(ERROR_PATH_NOT_FOUND == dwResult)
		{
			return IDS_NETWORKERROR;
		} else
		{
			return IDS_NOGENREC;
		}
	}
	// now write the file
	fResult = WriteFile(hGenData,m_pBuffer,m_dwFileSize,&dwBytesWritten,NULL);
	
	// and close the handle
	CloseHandle(hGenData);
	// free the buffer
	free(m_pBuffer);
	m_pBuffer = NULL;
	return CNetSuccess;

}

void CRRConsoleDlg::ResetDlgButtons()
{
	CEdit * pSerEdit = (CEdit *) GetDlgItem(IDC_SerialNumber);  // get handle to Serial Number edit box
	
	GetDlgItem(IDC_ConnectXBOX)->EnableWindow(TRUE);	// Enable Connect button
	pSerEdit->SetSel(0,-1) ; // select all the characters
	pSerEdit->Clear();
	pSerEdit->SetFocus();	// reset focus for serial number input
	GetDlgItem(IDC_HDReplace)->EnableWindow(FALSE);		// disable hdkey button
	GetDlgItem(IDC_VerifyXBox)->EnableWindow(FALSE);		// disable newxkey button
	GetDlgItem(IDRESET)->EnableWindow(FALSE);
	GetDlgItem(IDC_VerifyOnly)->EnableWindow(FALSE);
	((CStatic *)GetDlgItem(IDC_STOPNGO))->SetBitmap(*m_pbmRed);
	// ensure file buffer freed
	if(NULL != m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	return;

}

void CRRConsoleDlg::OnCompleteXBox() 
{
	CNetErrors cnError;
	DATA_PACKET Packet;
	CString MessageStr,CapStr;
	MessageStr.LoadString(IDS_WARNMFR);
	CapStr.LoadString(IDS_CAPWARN);
	int iResult = MessageBox(MessageStr,CapStr,MB_YESNO);
	if(IDNO == iResult)
	{
		ShowErrorStr(IDS_CANCELED);
		return;
	}
	Packet.dwCommand = DO_VERIFYPERBOXDATA;
	cnError = SendVerifyXBoxCommand(&Packet);

	if(CNetSuccess != cnError)
	{
		// had an error, display message box
		ShowErrorStr(CNetErrorIDs[cnError]);
	} else
	{
		ShowErrorStr(IDS_SUCCESSFULOP);
	}

	ResetDlgButtons();  // end of processing wrapup
	return;	
}


/*
void CRRConsoleDlg::OnClose() 
{
	CDialog::OnCancel();
}
*/

void CRRConsoleDlg::OnVerifyOnly() 
{
	CNetErrors cnError;
	DATA_PACKET Packet;
	
	Packet.dwCommand = DO_LOCK_HARDDRIVE;
	cnError = SendVerifyXBoxCommand(&Packet);

	if(CNetSuccess != cnError)
	{
		// had an error, display message box
		ShowErrorStr(CNetErrorIDs[cnError]);
	} else
	{
		ShowErrorStr(IDS_SUCCESSFULOP);
	}

	ResetDlgButtons();  // end of processing wrapup
	return;	
}

void CRRConsoleDlg::OnReset() 
{
	// TODO: Add your control notification handler code here
	ResetDlgButtons();  // end of processing wrapup
	
}
