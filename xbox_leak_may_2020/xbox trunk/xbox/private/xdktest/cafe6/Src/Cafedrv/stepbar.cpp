#include "stdafx.h"
#include "stepbar.h"


#define CELEM(rgfoo)  (sizeof(rgfoo) / sizeof(rgfoo[0]))

static UINT BASED_CODE DebugButtons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_SEPARATOR,  // real separator
	ID_SEPARATOR,  // Text to be displayed
	ID_SEPARATOR   // real separator
};




BOOL CDebugToolBar::Initialize(UINT nID)
{
	int iTries;
	DWORD dwMode;

	LoadBitmap(nID);
	SetButtons(DebugButtons, CELEM(DebugButtons));
	SetButtonInfo(4, ID_SEPARATOR, TBBS_SEPARATOR, MAXIMUMTEXTWIDTH);
	GetItemRect(4, &m_crect); // get the size of the separator will be used in static text settings.
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);

	for (;;)
	{
		m_hPipe = CreateFile(LPSZPIPENAME, GENERIC_READ | GENERIC_WRITE, 0 /*no sharing*/,
						NULL /*no security attr. */, OPEN_EXISTING, 0 /* default attribs */,
						NULL);				/* NO template file */


		if (m_hPipe != INVALID_HANDLE_VALUE)
			break;

		WaitNamedPipe(LPSZPIPENAME, NMPWAIT_WAIT_FOREVER);  // wait until the pipe is available 
	}

	dwMode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(m_hPipe, &dwMode, NULL, NULL))
	{
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
		return FALSE;
	}
		
	return TRUE;
}

CDebugToolBar::~CDebugToolBar()
{
	if (m_pcstaticText != NULL)
		delete m_pcstaticText;

	if (m_hPipe != INVALID_HANDLE_VALUE)
		CloseHandle(m_hPipe);
}

BOOL CDebugToolBar::SetToolBarText()  // read the message from the pipe and display it
{
	char szBuffer[MAXTEXTSIZE];
	unsigned long lcbRead;

	if (m_hPipe == NULL)  // something is going really wrong here
		return FALSE;

	if (!ReadFile(m_hPipe, szBuffer, MAXTEXTSIZE, &lcbRead, NULL /* not overlapped */))
		return FALSE;

	SetStaticText(szBuffer);

	return TRUE;  // RecalcLayout needs to be called to make the new text visible.
}		

BOOL CDebugToolBar::SetStaticText(char *szBuffer)  // read the message from the pipe and display it
{
	if (m_pcstaticText != NULL)
		delete m_pcstaticText;

	m_pcstaticText = new CStatic();
	m_pcstaticText->Create(szBuffer, WS_VISIBLE | WS_CHILD, m_crect, this);  // create the text.

	return TRUE;  // RecalcLayout needs to be called to make the new text visible.
}		
