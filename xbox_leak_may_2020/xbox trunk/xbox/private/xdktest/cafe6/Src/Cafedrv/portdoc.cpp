/////////////////////////////////////////////////////////////////////////////
// portdoc.cpp
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Implementation of the CViewportDoc class

#include "stdafx.h"
#include "portdoc.h"
#include "portview.h"					
#include "cafedrv.h"
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

extern CLogView * gLogView ;
/////////////////////////////////////////////////////////////////////////////
// CViewportDoc

IMPLEMENT_DYNCREATE(CViewportDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewportDoc, CDocument)
	//{{AFX_MSG_MAP(CViewportDoc)
	ON_COMMAND(ID_EDIT_CLEARALL, DeleteContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewportDoc construction/destruction

CViewportDoc::LineInfoType CViewportDoc::m_LineInfo[] = { 
	{ '0',  RGB(  0,   0,   0), settingHeaderFilter },	// header
	{ '1',  RGB(  0,   0, 150), settingSummaryFilter },	// summary
	{ '2',  RGB(  0, 128,   0), settingCommentsFilter },	// comments
	{ '3',  RGB(  0, 128, 128), settingResultsFilter },	// results
	{ '4',  RGB(255,   0,   0), settingFailureFilter },	// failure
	{ '5',  RGB(128,   0, 150), settingInfoFilter },		// info
	{ '6',  RGB(128, 128, 128), settingSeparatorFilter }, // separator
	{ '7',  RGB(  0,   0, 255), settingSuccessFilter },	// success
	{ '8',  RGB(100,  32,   0), settingStepFilter },	// step
	{'\0',	RGB(192, 192, 192), settingSelectedFilter }, //selected background
};

int CViewportDoc::nCount = 0;

CViewportDoc::CViewportDoc()
: m_nWidth(0),
  m_cCurrentAttribute(0),
  m_psettingsDoc(NULL)
{
	// store this instances count value (this must be done before the pipe is created, since this value is used to create a unique pipe name)
	m_nCount = nCount++;
	// store a system-wide unique id
	m_nId = ::GetTickCount()+m_nCount;

#ifdef NAMED_PIPES
	// set system-wide unique pipe name
	m_pipeViewport = "\\\\.\\pipe\\Pipe "+GetUniqueName();
	m_hPipeRead=m_hPipeRead=NULL;
#else
  BOOL bResult;
  
  bResult = CreatePipe(
    &m_hPipeRead,
    &m_hPipeWrite,
    0,  // security attributes
    0x4000);  // buffer suggestion
#endif


	// set system-wide unique event names
#ifdef NAMED_PIPES
	m_eventTerminate = "Terminate "+GetUniqueName();
	m_eventOverlapped = "Overlapped "+GetUniqueName();
#endif

	// get a pointer to the app
	CCAFEDriver *pApp = (CCAFEDriver*)AfxGetApp();

	// create the settings object and initialize settings
	m_psettingsDoc = new CSettings(pApp->GetSettings()->GetTextValue(settingCAFEKey)+"\\Viewport");
	InitializeSettings();

#ifdef NAMED_PIPES
	// create an event for signalling the worker thread
	m_hTerminate = ::CreateEvent(NULL, TRUE, FALSE, m_eventTerminate);

	// the event handle must be valid
	ASSERT(m_hTerminate);
	if (!m_hTerminate) {
		TRACE("CViewportDoc::CViewportDoc: unable to create terminate event for pipe thread\n");
	}
	else 
#endif
	{
		// create a worker thread to handle reading from the pipe
		DWORD dwThreadId;
		m_hthreadPipe = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CViewportDoc::WrapReadPipe, this, 0, &dwThreadId);
	}
}

CViewportDoc::~CViewportDoc()
{
#ifdef NAMED_PIPES
	// if the thread was created...
	if (m_hTerminate && m_hthreadPipe)
	{
		// terminate the thread
		::SetEvent(m_hTerminate);

		// wait for the thread to terminate
		if (WaitForSingleObject(m_hthreadPipe, 60000) != WAIT_OBJECT_0) {
			TRACE("CViewportDoc::~CViewportDoc: unable to terminate viewport thread\n");
			::TerminateThread(m_hthreadPipe, DWORD(-1));
		}
		::CloseHandle(m_hthreadPipe);
		::CloseHandle(m_hTerminate);
	}
#else
	if(m_hthreadPipe)
	{
		::TerminateThread(m_hthreadPipe, DWORD(-1));
		::CloseHandle(m_hthreadPipe);
	}
	// we must close the write end of the pipe first so that the thread reading from the pipe will exit
	if(m_hPipeWrite)
		::CloseHandle(m_hPipeWrite);
	if(m_hPipeRead)
		::CloseHandle(m_hPipeRead);
#endif

	// delete the settings object
	if (m_psettingsDoc) {
		delete m_psettingsDoc;
	}
}

BOOL CViewportDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

BOOL CViewportDoc::OnOpenDocument(const char* pszPathName)
{
	CStdioFile file;
	 // Initialize the logview tree.
	m_pLogViewTree = 0;

	// get a pointer to the app
	CCAFEDriver *pApp = (CCAFEDriver*)AfxGetApp();

	if (file.Open(pszPathName, CFile::modeRead | CFile::typeText)) {
		ReadFile(&file);
		file.Close();
	}
	else {
		TRACE("CViewportDoc::OnOpenDocument: unable to open the viewport file for reading\n");
		pApp->ErrorMsg(IDS_ErrOpenViewportFile, pszPathName);
		return FALSE;
	}

	// set the title to the filename
	SetTitle(pszPathName);

	return TRUE;
}

BOOL CViewportDoc::OnSaveDocument(const char* pszPathName)
{
	CStdioFile file;

	// get a pointer to the app
	CCAFEDriver *pApp = (CCAFEDriver*)AfxGetApp();

	if (file.Open(pszPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
		WriteFile(&file);
		file.Close();
	}
	else {
		TRACE("CViewportDoc::OnSaveDocument: unable to open the viewport file for saving\n");
		pApp->ErrorMsg(IDS_ErrOpenViewportFile, pszPathName);
		return FALSE;
	}

	// set the title to the filename
	SetTitle(pszPathName);

	return TRUE;
}

void CViewportDoc::DeleteContents()
{
	m_aLines.RemoveAll();
	m_nWidth = 0;
	UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CViewportDoc operations


void CViewportDoc::SetGenericTitle(LPCSTR szTitleRoot, LPCSTR szNumber)
{
  CString str(szTitleRoot);

  str += szNumber;

  SetTitle(str);
}


CString CViewportDoc::GetLine(int nLine)
{
	CString strLine;

	// the line must be valid
	ASSERT(nLine >= 0 && nLine < m_aFilteredLines.GetSize());

	// if the given line is out of range, put it in range
	if (nLine < 0) {
		nLine = 0;
	}
	if (nLine >= m_aFilteredLines.GetSize()) {
		nLine = m_aFilteredLines.GetSize() - 1;
	}

	// get the text part of the line
	strLine = GetLineText(m_aFilteredLines.GetAt(nLine));
	
	return strLine;
}

COLORREF CViewportDoc::GetColor(int nLine)
{
	int nIndex;

	// the line must be valid
	ASSERT(nLine >= 0 && nLine < m_aFilteredLines.GetSize());
	
	// if the given line is out of range, put it in range
	if (nLine < 0) {
		nLine = 0;
	}
	if (nLine >= m_aFilteredLines.GetSize()) {
		nLine = m_aFilteredLines.GetSize() - 1;
	}

	// get the index of this line's tag
	nIndex = GetTagIndex(m_aFilteredLines.GetAt(nLine));

	// return the color
	return m_LineInfo[nIndex].color;
}

COLORREF CViewportDoc::GetSelBkColor()
{
	const ndim=sizeof(m_LineInfo)/sizeof(LineInfoType);
	return m_LineInfo[ndim-1].color;	
}

BOOL CViewportDoc::GetFilter(UINT nId)
{
	int nIndex;

	// map the id of the filter control to the line info array
	nIndex = nId - IDM_ViewportHeaderFilter;

	// the index must be in range
	ASSERT(nIndex >= 0);
	if (nIndex < 0) {
		return FALSE;
	}

	return m_psettingsDoc->GetBooleanValue(m_LineInfo[nIndex].lpszSetting);
}

BOOL CViewportDoc::SetFilter(UINT nId, BOOL bFilter)
{
	int nIndex;

	// map the id of the filter control to the line info array
	nIndex = nId - IDM_ViewportHeaderFilter;

	// the index must be in range
	ASSERT(nIndex >= 0);
	if (nIndex < 0) {
		return FALSE;
	}

	// set the filter
	m_psettingsDoc->SetBooleanValue(m_LineInfo[nIndex].lpszSetting, bFilter);

	// save it in the registry
	m_psettingsDoc->WriteRegistry();

	// update the set of filtered lines
	FilterAllLines();

	// update the UI
	//UpdateAllViews(NULL);//REVIEW(chriskoz)impossible: selection is bogus (we dont have the selection "m_rSel1" here)

	return TRUE;
}

NodeTypes CViewportDoc::LineHasKeyWord(CString strLine)
{
	if(strLine.Find("Owned by") != -1)
	{
		int i = strLine.Find("Owned") ;
		szLogTreeLine = strLine.Mid(i) ;
		i = szLogTreeLine.Find("]") ;
		szLogTreeLine = szLogTreeLine.Left(i) ;
		szLogTreeLine.TrimRight() ;
		return SUBSUITE ;
	}
	if(strLine.Find("[--------------------< Test:") != -1) 
	{
		int i = strLine.Find("Test:") ;
		szLogTreeLine = strLine.Mid(i) ;
		i = szLogTreeLine.Find(">") ;
		szLogTreeLine = szLogTreeLine.Left(i) ;
		szLogTreeLine.TrimRight() ;
		return TESTCASE ; 
	}
	if(strLine.Find("Suite Summary") != -1) 
	{
		szLogTreeLine = "Suite Summary" ;
		return SUMMARY ;
	}
	if(strLine.Find("General Information") != -1) 
	{
		szLogTreeLine = "General Info.";
		return SUITE ;
	}
	if(strLine.Find("*** CRITICAL ERROR ***")!=-1) 
	{
		szLogTreeLine = "*CRITICAL ERROR" ;
		return FAILURE ;
	}
	if (strLine.Find("***FAILED***") !=-1)
	{
		szLogTreeLine = "* FAILED *" ;
		return FAILURE ;
	}

	return INVALID ;

}

/////////////////////////////////////////////////////////////////////////////
// CViewportDoc operations (internal)

BOOL CViewportDoc::InsertLine(int nLine, CString strLine)
{
	int i;
	BOOL bInserted;

	// the line to insert at must be valid
	ASSERT(nLine >= 0 && nLine <= m_aLines.GetSize());

	// if the given line is out of range, put it in range
	if (nLine < 0) {
		nLine = 0;
	}
	if (nLine > m_aLines.GetSize()) {
		nLine = m_aLines.GetSize();
	}

	// is this line longer that the currently longest line?
	m_nWidth = max(m_nWidth, strLine.GetLength());

	// insert the line
	m_aLines.InsertAt(nLine, strLine);

	NodeTypes KeyType ;
	if((KeyType =LineHasKeyWord(strLine))!= INVALID) // Does the line have any keywords we are interested in?
	{
		m_pLogViewTree->AddNewNode(KeyType,nLine,szLogTreeLine) ;
	}

	// can this line be added to the set of filtered lines?
	if (FilterOneLine(nLine)) {
		// iterate through the filtered lines
		for (i = 0, bInserted = FALSE; i < m_aFilteredLines.GetSize() && !bInserted; i++) {
			// if there's a line stored that has an index greater
			// than this one, insert it there
			if (m_aFilteredLines.GetAt(i) > nLine) {
				m_aFilteredLines.InsertAt(i, nLine);
				bInserted = TRUE;
			}
		}
		// did the line get inserted?
		if (!bInserted) {
			// add it at the end
			m_aFilteredLines.Add(nLine);
		}
	}

	return TRUE;
}

BOOL CViewportDoc::AddLine(CString strLine)
{
	int nIndex;

	// is this line longer that the currently longest line?
	m_nWidth = max(m_nWidth, strLine.GetLength());

	// add the line to the end of the array
	nIndex = m_aLines.Add(strLine);

	// can this line be added to the set of filtered lines?
	if (FilterOneLine(nIndex)) {
		m_aFilteredLines.Add(nIndex);
	}

	return TRUE;
}

BOOL CViewportDoc::InitializeSettings(void)
{
	ASSERT(m_psettingsDoc);

	// initialize default values
	m_psettingsDoc->SetBooleanValue(settingHeaderFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingSummaryFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingCommentsFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingResultsFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingFailureFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingInfoFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingSeparatorFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingSuccessFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingStepFilter, TRUE);
	m_psettingsDoc->SetBooleanValue(settingSelectedFilter, TRUE);

	// read values from the registry
	m_psettingsDoc->ReadRegistry();

	// write updated values to the registry
	m_psettingsDoc->WriteRegistry();

	return TRUE;
}

CString CViewportDoc::GetUniqueName(void)
{
	CString strName;

	strName.Format("Viewport %d", m_nId);

	return strName;
}


/////////////////////////////////////////////////////////////////////////////
// Tag operations (internal)

int CViewportDoc::GetTagIndex(int nLine)
{
	CString strLine;
	int i;

	// the line must be valid
	ASSERT(nLine >= 0 && nLine < m_aLines.GetSize());

	// if the given line is out of range, put it in range
	if (nLine < 0) {
		nLine = 0;
	}
	if (nLine >= m_aLines.GetSize()) {
		nLine = m_aLines.GetSize() - 1;
	}

	// get the string
	strLine = m_aLines.GetAt(nLine);

	// return the default index if there is no string
	if (strLine.IsEmpty()) {
		TRACE("CViewportDoc::GetTagIndex: string without tag\n");
		return 0;
	}

	// find the tag in the array of line info
	for (i = 0; m_LineInfo[i].tag != '\0'; i++) {
		if (strLine.GetAt(0) == m_LineInfo[i].tag) {
			break;
		}
	}
	// did we not find a valid tag? return the default index
	if (m_LineInfo[i].tag == '\0') {
		TRACE("CViewportDoc::GetTagIndex: string with invalid tag\n");
		return 0;
	}
	else {
		return i;
	}
}

CString CViewportDoc::GetLineText(int nLine)
{
	CString strLine;

	// the line must be valid
	ASSERT(nLine >= 0 && nLine < m_aLines.GetSize());

	// if the given line is out of range, put it in range
	if (nLine < 0) {
		nLine = 0;
	}
	if (nLine >= m_aLines.GetSize()) {
		nLine = m_aLines.GetSize() - 1;
	}

	// get the string
	strLine = m_aLines.GetAt(nLine);

	// return the string if it's empty
	if (strLine.IsEmpty()) {
		TRACE("CViewportDoc::GetTagIndex: string without tag\n");
		return strLine;
	}

	// skip the tag and return the real text part of the string
	#ifdef _DEBUG
		return strLine;
	#else
      return strLine.Mid(1);
	#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////////////
// Filter operations (internal)

BOOL CViewportDoc::FilterOneLine(int nLine)
{
	int nIndex;

	// the line must be valid
	ASSERT(nLine >= 0 && nLine < m_aLines.GetSize());

	// if the given line is out of range, put it in range
	if (nLine < 0) {
		nLine = 0;
	}
	if (nLine >= m_aLines.GetSize()) {
		nLine = m_aLines.GetSize() - 1;
	}

	// get the tag index for this line
	nIndex = GetTagIndex(nLine);

	// return the state of the filter for this line
	return m_psettingsDoc->GetBooleanValue(m_LineInfo[nIndex].lpszSetting);
}

BOOL CViewportDoc::FilterAllLines(void)
{
	int i;

	// clear the filtered lines array
	m_aFilteredLines.RemoveAll();

	// interate through all lines
	for (i = 0; i < m_aLines.GetSize(); i++) {
		if (FilterOneLine(i)) {
			m_aFilteredLines.Add(i);
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Pipe operations (internal)

UINT CViewportDoc::WrapReadPipe(LPVOID pParam)  // a static function
{
  return ((CViewportDoc*)pParam)->ReadPipe();
}

UINT CViewportDoc::ReadPipe()
{
#undef THIS_FUNCTION
#define THIS_FUNCTION "CViewportDoc::ReadPipe:"
	HANDLE hPipe;
#ifdef NAMED_PIPES
	OVERLAPPED olPipe;

	HANDLE hTerminate;

	DWORD dwWaitResult;
	HANDLE hOverlapped;
#endif


	CString strBuf;
	CString strRemain;
	DWORD dwRead;
	BOOL bDone;


#ifdef NAMED_PIPES
	// open the termination event
	hTerminate = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, m_eventTerminate);

	// the handle must be valid
	ASSERT(hTerminate);
	if (!hTerminate) {
		TRACE(THIS_FUNCTION "unable to open the terminate event\n");
		return 1;
	}

	// create a pipe for communication to the viewport
	hPipe = ::CreateNamedPipe(GetPipeName(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 2048, 2048, 2000, NULL);
#else
	hPipe = m_hPipeRead;
#endif

	// the pipe must be valid
	ASSERT(hPipe != INVALID_HANDLE_VALUE && hPipe != NULL);
	if (hPipe == INVALID_HANDLE_VALUE || hPipe == NULL) {
		TRACE(THIS_FUNCTION "unable to open the read end of the pipe\n");
#ifdef NAMED_PIPES
		::CloseHandle(hTerminate);
#endif
		return 2;
	}

#ifdef NAMED_PIPES
	// create an event for the overlapped functions
	hOverlapped = ::CreateEvent(NULL, TRUE, FALSE, m_eventOverlapped);

	// the event handle must be valid
	ASSERT(hOverlapped);
	if (!hOverlapped) {
		TRACE(THIS_FUNCTION "unable to create the overlapped event\n");
		::CloseHandle(hTerminate);
		::CloseHandle(hPipe);
		return 3;
	}
#endif

#ifdef NAMED_PIPES

	// loop
	while (1)
	{	
		// initialize the overlapped structure
		olPipe.Offset = 0;
		olPipe.OffsetHigh = 0;
		olPipe.hEvent = hOverlapped;

		// indicate we want to wait until there's a connection to the pipe
		::ConnectNamedPipe(hPipe, &olPipe);

		if (::GetLastError() == ERROR_IO_PENDING) 
		{

			// wait for either the connection or termination
			HANDLE hObjects[] = { hOverlapped, hTerminate };
			dwWaitResult = ::WaitForMultipleObjects(sizeof(hObjects)/sizeof(HANDLE), hObjects, FALSE, INFINITE);

			// if we got some other notification besides the overlapped, terminate
			if (dwWaitResult != WAIT_OBJECT_0) {
				::CloseHandle(hTerminate);
				::CloseHandle(hPipe);
				::CloseHandle(hOverlapped);
				return 0;
			}

			// get the results of the connection
			if (!::GetOverlappedResult(hPipe, &olPipe, &dwRead, FALSE)) {
				TRACE(THIS_FUNCTION "error waiting for pipe connection (%d)\n", ::GetLastError());
				dwRead = 0;
			}

		}
#endif

		// read from the pipe until we're done reading
		for (bDone = FALSE; !bDone; )
		{
#ifdef NAMED_PIPES
			if (!::ReadFile(hPipe, strBuf.GetBuffer(255), 255, &dwRead, &olPipe)) 
#else
			if (!::ReadFile(hPipe, strBuf.GetBuffer(8), 8, &dwRead, 0)) 
#endif
				switch (::GetLastError())
				{  //error from ::ReadFile()
					case ERROR_BROKEN_PIPE: {
						TRACE(THIS_FUNCTION "ReadFile - ERROR_BROKEN_PIPE\n");
						bDone = TRUE;
						break;
					}
#ifdef NAMED_PIPES
					case ERROR_PIPE_LISTENING: {
						TRACE(THIS_FUNCTION "ReadFile - ERROR_PIPE_LISTENING\n");
						bDone = TRUE;
						break;
					}
					case ERROR_IO_PENDING:
					{
						// wait for either the read or termination
						HANDLE hObjects[] = { hOverlapped, hTerminate };
						dwWaitResult = ::WaitForMultipleObjects(sizeof(hObjects)/sizeof(HANDLE), hObjects, FALSE, INFINITE);

						// if we got some other notification besides the overlapped, terminate
						if (dwWaitResult != WAIT_OBJECT_0) {
							TRACE(THIS_FUNCTION "ReadFile - terminate event\n");
							::CloseHandle(hTerminate);
							::CloseHandle(hPipe);
							::CloseHandle(hOverlapped);
							return 0;
						}

						// get the number of bytes read
						if (!::GetOverlappedResult(hPipe, &olPipe, &dwRead, FALSE)) {
							TRACE(THIS_FUNCTION "GetOverlappedResult error (%d)\n", ::GetLastError());
							dwRead = 0;
						}
						break;
					}
#endif
					default:
						TRACE(THIS_FUNCTION "ReadFile - unknown error (%d)\n", ::GetLastError());
						break;
				}//error from ::ReadFile()
			// if we got some bytes to read...
			if (!bDone && dwRead > 0) {
				strBuf.ReleaseBuffer(dwRead);

				// add the data to the viewport
				ReadPipeData(strBuf, strRemain);
			}
		}//read loop from the pipe
#ifdef NAMED_PIPES
		// disconnect from this pipe
		::DisconnectNamedPipe(hPipe);
	} //while(1)
#endif

	return 0;
}

BOOL CViewportDoc::ReadPipeData(CString &strBuf, CString &strRemain, int nLine /*= -1*/)
{
	int nPos, nLen;
	CString strLine;

	CView* pView;
	CWnd* pViewWnd;

	// initialize nLine if not given
	if (nLine == -1)
		nLine = GetNumLines();

	// parse the data in the buffer
	// each line ends with a \n

	while ((nPos = strBuf.Find('\n')) != -1)
  {
		// store the line in the string array
		strLine = strRemain + strBuf.Left(nPos);

    nLen = strLine.GetLength();
    if ((nLen) && (strLine[nLen-1] == '\r'))  // if terminated with \r
    {
      strLine = strLine.Left(nLen - 1);  // axe the \r
    }

    if ((!strLine.IsEmpty()) && (strLine[0] == '\xff'))
    {
      // pump out unused attributes
      
      while ((strLine.GetLength() >= 4) && (strLine[2] == '\xff'))  // while we have leading unused attributes
        strLine = strLine.Mid(2);  // axe 'em

      // use the final one

      m_cCurrentAttribute = strLine[1];  // grab the attribute
      strLine = strLine.Mid(1);  // remove the signal char
    }
    else
      strLine = (char)m_cCurrentAttribute + strLine;

		InsertLine(nLine, strLine);

		// clear the remain string
		strRemain.Empty();

		// remove the stored string from the buffer
		strBuf = strBuf.Mid(nPos+1);

		// iterate through all views associated with this document
		for (POSITION pos = GetFirstViewPosition(); pos != NULL; ) {
			pView = GetNextView(pos);

			// get the CWnd* for the view (can't just use the CView* because this
			// thread doesn't have access to the handle map in the main thread
			// (MFC asserts)
			pViewWnd = (CViewportView*)CWnd::FromHandle(pView->m_hWnd);

#ifdef MST_BUG
			// update the view
			pViewWnd->SendMessage(WM_UPDATE_FROM_PIPE, 0, nLine);

#endif // MST_BUG
		}

		// increment the line count
		nLine++;
	}

	// store the remaining string for the next buffer read
	strRemain += strBuf;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// File operations (internal)

BOOL CViewportDoc::ReadFile(CStdioFile* pfile, int nLine /*= -1*/)
{
	CString strLine;

	// the file ptr must be valid
	ASSERT(pfile);

	// initialize nLine if not given
	if (nLine == -1) {
		nLine = GetNumLines();
	}

	while (pfile->ReadString(strLine.GetBuffer(1024), 1024)) {
		InsertLine(nLine++, strLine);
	}

	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CViewportDoc::WriteFile(CStdioFile* pfile, int nBegLine /*= -1*/, int nEndLine /*= -1*/)
{
	int i;
	CString strLine;

	// the file ptr must be valid
	ASSERT(pfile);

	// initialize nBegLine and nEndLine, if not given
	if (nBegLine == -1) {
		nBegLine = 0;
	}
	if (nEndLine == -1) {
		nEndLine = GetNumLines();
	}

	for (i = nBegLine; i < nEndLine; i++) {
		strLine = GetLine(i) + "\n";					// add "\r\n" if this is not a stdio file
		pfile->WriteString(strLine);
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CViewportDoc serialization

void CViewportDoc::Serialize(CArchive& ar)
{
	ASSERT(FALSE);		// REVIEW(davidga): I think OnOpenDocument and OnSaveDocument do everything I need
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


/////////////////////////////////////////////////////////////////////////////
// CViewportDoc diagnostics

#ifdef _DEBUG
void CViewportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CViewportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

void CViewportDoc::OnCloseDocument()
{
  CDocument::OnCloseDocument() ;
  return ;
}
/////////////////////////////////////////////////////////////////////////////
// CViewportDoc commands
