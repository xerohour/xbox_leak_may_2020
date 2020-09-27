/////////////////////////////////////////////////////////////////////////////
// suitedoc.cpp
//
// email    date        change
// briancr  10/25/94    created
//
// copyright 1994 Microsoft

// Implementation of the CSuiteDoc class

#include "stdafx.h"
#include "cafexcpt.h"
#include "testxcpt.h"
#include "log.h"
#include "textlog.h"
#include "hotkey.h"
#include "suitedoc.h"
#include "applog.h"
#include "cafedrv.h"
#include "rawstrng.h"
#include "guiv1.h"
#include "test.h"
#include "randtest.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// REVIEW(briancr): provided only for backward compatibility
extern AFX_EXT_DATA CLog* gpLog;
extern CViewportView * vConsole, *vTextLog ;
/////////////////////////////////////////////////////////////////////////////
// CSuiteDoc

IMPLEMENT_DYNCREATE(CSuiteDoc, CDocument)

int CSuiteDoc::m_cSubSuites = 0;
unsigned CSuiteDoc::m_uDocCount = 0;

CSuiteDoc::CSuiteDoc()
: m_pLogViewport(NULL),
  m_pConsoleViewport(NULL),
  m_bCleanUpBefore(TRUE),
  m_bCleanUpAfter(TRUE),
  m_bLoopTests(FALSE),
  m_bRandomize(FALSE),
  m_nRandomSeed(0),
  m_RandomLimit(RandomLimitNum),
  m_nRandomLimitNum(100),
  m_RandomLimitTime(CTimeSpan(0, 1, 0, 0)),
  m_pGraphForest(NULL),
  m_bPostResults(FALSE),
  m_bRecordComments(FALSE),
  m_bLogDebugOutput(FALSE),
  m_plistTestsRun(NULL),
  m_nCycleNumber(1)
{
	m_bViews=TRUE;
	CSettings *appSettings=((CCAFEDriver*)AfxGetApp())->GetLogSettings();
	SetResultsFile(appSettings->GetTextValue(settingLogResultsFile) );
	SetSummaryFile(appSettings->GetTextValue(settingLogSummaryFile) );
	SetWorkingDir (appSettings->GetTextValue(settingLogWorkDir) );
	SetMaxFail    (appSettings->GetIntValue(settingLogMaxFail) );

	m_plistTestsRun = new CSubSuite::TestList;
}

BOOL CSuiteDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    CreateViewports();

    return TRUE;
}

CString TestName ;
void CSuiteDoc::OnCloseDocument( )
{	
	
	CDocument::OnCloseDocument() ;
	return ;
}

BOOL CSuiteDoc::OnOpenDocument(LPCTSTR lpszPathName)
{


    // defer to base class
    if (!CDocument::OnOpenDocument(lpszPathName)) //REVIEW(chriskoz)maybe I shouldn't do it for DLL Template
        return FALSE;
    
    CreateViewports();

    // read the suite file
    if (ReadSuite(CString(lpszPathName))) {
        // load the subsuites
    LoadSubSuites();
	return TRUE;
    }

    return FALSE;
}

extern "C" typedef BOOL (APIENTRY* DESTROYSUBSUITE)(CSubSuite *pSS);

CSuiteDoc::~CSuiteDoc()
{
    CSuiteDoc::SubSuiteInfo* pSubSuiteInfo;
    DESTROYSUBSUITE funcDestroySubSuite;

	// delete the list of tests run
	m_plistTestsRun->RemoveAll();
	delete m_plistTestsRun;

	// delete the graph forest
	delete m_pGraphForest;

    // delete each entry in the sub suite list
    for (POSITION pos = m_listSubSuites.GetHeadPosition(); pos != NULL; ) {
        pSubSuiteInfo = m_listSubSuites.GetNext(pos);

        // does a DestroySubSuite function exist?
        funcDestroySubSuite = (DESTROYSUBSUITE)::GetProcAddress(pSubSuiteInfo->m_hDLL, "DestroySubSuite");
        if (funcDestroySubSuite != NULL) {
            // call DestroySubSuite, if it exists
            funcDestroySubSuite(pSubSuiteInfo->m_pSubSuite);  // param added by CFlaat
        }

        // unload the DLL
        ::FreeLibrary(pSubSuiteInfo->m_hDLL);

        APPLOG("Unloaded subsuite (%s).", (LPCSTR)pSubSuiteInfo->m_strFilename);

        // delete the entry data
        delete pSubSuiteInfo;
    }

    // only close the write ends of the viewport pipes if the viewports exist
/*    if (m_pLogViewport) {
        ::CloseHandle(m_pLogViewport -> GetWriteHandle());
    }
    if (m_pConsoleViewport) {
        ::CloseHandle(m_pConsoleViewport -> GetWriteHandle());
    } */

    // remove all entries from the list
    m_listSubSuites.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// attributes

CSuiteDoc::SubSuiteList* CSuiteDoc::GetSubSuiteList(void)
{
    return &m_listSubSuites;
}

/////////////////////////////////////////////////////////////////////////////
// Operations

void CSuiteDoc::SetConsolePortDoc()
{
   CView  * pSpVw  ;
   int i = 0 ;
   // Find the view associated with the ConsolePort in the splitter pane, it's the last one (2nd) in the list.
   POSITION pos = GetFirstViewPosition();
   for (; i < 2; i++)
   { 
	   pSpVw =GetNextView(pos);
	   vConsole = (CViewportView *)pSpVw ;
   }
	m_pConsoleViewport->m_pLogViewTree = (CLogView *) GetNextView(pos);	//The LogTreeView .
   // Remove this view from the splitter MDIChild so that we can add it as a view fot the ConsoleLog.
   RemoveView(pSpVw) ;
    // Add the Splitter view to the log so it now logs to the splitter
   m_pConsoleViewport->AddView(pSpVw) ;

}

void CSuiteDoc::SetLogViewPortDoc() 
{
   CView * pSpVw  ;
   int i = 0 ;
   // Find the view associated with the ConsolePort in the splitter pane, it's the 3rd one after 
   // we removed the Console one in SetConsolePortDoc() above. 
   POSITION pos = GetFirstViewPosition();
   for (; i < 3; i++ )
   { 
	   pSpVw =GetNextView(pos);
	   if (i == 1)
		 m_pLogViewport->m_pLogViewTree = (CLogView *)pSpVw ;	//The LogTreeView .
	   vTextLog =(CViewportView *) pSpVw ;
   }
  // Remove this view from the splitter MDIChild so that we can add it as a view fot the ConsoleLog.
   RemoveView(pSpVw) ;
   // Add the Splitter view to the log so it now logs to the splitter
   m_pLogViewport->AddView(pSpVw) ;
}

BOOL CSuiteDoc::CreateViewports(void)
{
 
  CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

  ((CFrameWnd*)(pApp -> m_pMainWnd)) -> RecalcLayout();

  m_bViews = (CMDLINE->GetBooleanValue("views", TRUE)? TRUE:FALSE);  //disable views (ie -views:false)
  if (m_bViews) // are viewports disabled? (ie -views:false )
  {
	m_pConsoleViewport = pApp ->NewViewport();  // for console app output
	
	// Remove all views and attach the splitter view.
	SetConsolePortDoc() ;

	CString strNum;

	m_uDocCount++;
	strNum.Format("%u", m_uDocCount);

//	m_pConsoleViewport -> SetGenericTitle("Console", strNum);

    m_pLogViewport = pApp->NewViewport();  // for log output

	// Detach the default view and attach the splitter pane to this document.
	SetLogViewPortDoc();
//	m_pLogViewport -> SetGenericTitle("Log", strNum);

  }
    	
  //pApp -> DocToRegion(this, CCAFEDriver::Suite);
  if (m_bViews) // are viewports disabled? (ie -views:false )
  {
//	pApp -> DocToRegion(m_pLogViewport, CCAFEDriver::Log);
//	pApp -> DocToRegion(m_pConsoleViewport, CCAFEDriver::Console);
  }; 
  return TRUE;

}


BOOL CSuiteDoc::AddSubSuite(int Index, CString &strFilename, CString &strParams)
{
    ASSERT(0);

    // the index cannot be negative
    ASSERT(Index >= 0);

    // the filename cannot be empty
    ASSERT(!strFilename.IsEmpty());

    // TODO(briancr): complete

    return FALSE;

    UNREFERENCED_PARAMETER(strParams);
}


BOOL CSuiteDoc::RemoveSubSuite(int Index)
{
    ASSERT(0);

    // the index cannot be negative
    ASSERT(Index >= 0);

    // TODO(briancr): complete

    return FALSE;
}

BOOL CSuiteDoc::Run(void)
{
    // get a pointer to the app
    CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

    // create viewport docs/views

#ifndef MST_BUG
#error Didn't get MST_BUG!!!
    // REVIEW(briancr): to work around an MS-Test bug we run the suite in the main thread
    // the MS-Test bug: when calling into t3ctrl32.dll from multiple threads, the second
    // thread to call into the DLL never returns.
    RunSuite();
	if (m_bViews) // are viewports disabled? (ie -views:false )
    {
    m_pLogViewport->UpdateAllViews(NULL, m_pLogViewport->GetNumLines()+1);
    m_pConsoleViewport->UpdateAllViews(NULL, m_pConsoleViewport->GetNumLines()+1);
	}

#else
    // create a thread to execute the suite
//  m_pthreadSuite = AfxBeginThread(CSuiteDoc::RunSuite, this);
  DelegateSuiteExecution();
#endif // MST_BUG

    return TRUE;
}

UINT CSuiteDoc::RunSuite(void)
{
	const char* const THIS_FUNCTION = "CSuiteDoc::RunSuite";

    HANDLE hLogPipe, hConsolePipe;
    CTextLog* pLog = NULL;

    // get a pointer to the app
    CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

    // REVIEW(briancr): in order to use the old single step window, I've added this code
	if (pApp->GetStepWnd()!=NULL) //in case we don't have a StepWnd (ie -hotkey_thread:false)
	{
		if (pApp->GetSettings()->GetBooleanValue(settingDoSingleStep) ||
			g_stopStep>0){ //force single step mode when the breakpoint set
			// register hot keys
			pApp->GetStepWnd()->SendMessage(WM_REGISTERHOTKEYS);
			ResetEvent(g_hSingleStep);
			g_hkToDo = HOTKEY_SINGLE;
			APPLOG("%s: starting in single step mode; hot keys registered, event reset.", THIS_FUNCTION);
		}
		else {
			// unregister hot keys
			pApp->GetStepWnd()->SendMessage(WM_UNREGISTERHOTKEYS);
			SetEvent(g_hSingleStep);
			g_hkToDo = HOTKEY_RUN;
			APPLOG("%s: starting in run mode; hot keys unregistered, event set.", THIS_FUNCTION);
		}
	}

	if (m_bViews) // are viewports disabled? (ie -views:false )
    {
#ifdef NAMED_PIPES
		hLogPipe = INVALID_HANDLE_VALUE;
		if(::WaitNamedPipe(m_pLogViewport->GetPipeName(),
			NMPWAIT_USE_DEFAULT_WAIT)!=FALSE)
			hLogPipe = CreateFile(m_pLogViewport->GetPipeName(),
				GENERIC_WRITE, 0, NULL,
				OPEN_EXISTING,
				0, //don't overlap IO
				NULL);
#else
	    // get a handle to the pipe
	    hLogPipe = m_pLogViewport->GetWriteHandle();
#endif
	    ASSERT(hLogPipe != INVALID_HANDLE_VALUE && hLogPipe != NULL);
	    if (hLogPipe == INVALID_HANDLE_VALUE || hLogPipe == NULL) {
			pApp->ErrorMsg("Error getting the write end of the pipe to the viewport. GetLastError = %d. Test output will not be visible; view results.log instead.", ::GetLastError());
	        hLogPipe = NULL;
	    }
#ifdef NAMED_PIPES
		hConsolePipe = INVALID_HANDLE_VALUE;
		if(::WaitNamedPipe(m_pConsoleViewport->GetPipeName(),
			NMPWAIT_USE_DEFAULT_WAIT)!=FALSE)
			hConsolePipe = CreateFile(m_pConsoleViewport->GetPipeName(),
				GENERIC_WRITE, 0, NULL,
				OPEN_EXISTING,
				0, //don't overlap IO
				NULL);
#else
	    // get a handle to the pipe
	    hConsolePipe = m_pConsoleViewport->GetWriteHandle();
#endif	
	    ASSERT(hConsolePipe != INVALID_HANDLE_VALUE && hConsolePipe != NULL);
	    if (hConsolePipe == INVALID_HANDLE_VALUE || hConsolePipe == NULL) {
			pApp->ErrorMsg("Error getting the write end of the pipe to the console window. GetLastError = %d; Console output will not be visible.", ::GetLastError());
	        hConsolePipe = NULL;
	    }
	} else {
		hLogPipe=NULL;
		hConsolePipe = NULL;
	};

    // store the current directory
	char szOrigPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szOrigPath);

    // create a log object
    pLog = new CTextLog(m_strResultsFile, m_strSummaryFile, m_strWorkingDir, hLogPipe);
	// UGLY_HACK(briancr): this code is here to log info to a server about who runs CAFE
	// remove it as soon as possible
	// do we disable the big brother option?
	if (!pApp->GetCmdLine()->GetBooleanValue(settingBigBro, TRUE)) {
		pLog->m_bBigBro = FALSE;
	}

    // REVIEW(briancr): just to make things work
    // set the log and cmd line in the support system
    gpLog = pLog;
	gpLog->SetLogDebugOutput(GetLogDebugOutput());

    // create a new toolset
    CToolset* pToolset = pApp->NewToolset(hConsolePipe);

    // tell the toolset the log to use
    pToolset->SetLog(pLog);

	// clear the list of tests run
	m_plistTestsRun->RemoveAll();

	// do we run tests randomly
	if (GetRandomize()) {
		RunRandom(pToolset);
	}
	else {
		RunSequential(pToolset);
	}

	// delete the toolset
	pApp->DeleteToolset(pToolset);

	// delete the log object
    delete pLog;

    // reset the current directory
    ::SetCurrentDirectory(szOrigPath);

#ifdef NAMED_PIPES
//	disconnect from named pipes
	if(hLogPipe!=NULL)
		CloseHandle(hLogPipe);
	if(hConsolePipe != NULL)
		CloseHandle(hConsolePipe);
#endif
    // was the driver launched with the -run switch?
    if (pApp->GetCmdLine()->IsDefined(settingRun)) {
        // if so, close the app
        PostMessage(pApp->m_pMainWnd->m_hWnd, WM_CLOSE, 0, 0);
    }

    return 0;
}

BOOL CSuiteDoc::RunRandom(CToolset* pToolset)
{
	const char* const THIS_FUNCTION = "CSuiteDoc::RunRandom";
	
	APPLOG("Running tests randomly.");
	if (m_RandomLimit == RandomLimitNone) {
		APPLOG("No limit set by user for random test run.");
	}
	else if (m_RandomLimit == RandomLimitNum) {
		APPLOG("Limit of %d tests set by user for random test run.", m_nRandomLimitNum);
	}
	else if (m_RandomLimit == RandomLimitTime) {
		APPLOG("Limit of %s set by user for random test run.", m_RandomLimitTime.Format("%D:%H:%M"));
	}

    // get a pointer to the app
    CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

    // run the tests randomly
    try {

        WaitStepInstructions(INITIALIZE_STRING);

		// get the list of subsuites
        CSuiteDoc::SubSuiteList* plistSubSuites = GetSubSuiteList();

		// initialize all subsuites
        for (POSITION posSubSuite = plistSubSuites->GetHeadPosition(); posSubSuite != NULL; ) {
            CSuiteDoc::SubSuiteInfo* pSubSuiteInfo = plistSubSuites->GetNext(posSubSuite);
            // is this subsuite selected to run?
            if (pSubSuiteInfo->m_bRun) {
				APPLOG("Initializing subsuite '%s'.", (LPCSTR)pSubSuiteInfo->m_strFilename);
				// set the current directory to the subsuite's directory
				::SetCurrentDirectory(pSubSuiteInfo->m_pSubSuite->GetCWD());
				// initialize the subsuite
				if (!pSubSuiteInfo->m_pSubSuite->Initialize(pToolset)) {
					// if the subsuite initialization fails, we need to bail out
					throw CTestException("SubSuite initialization failed; unable to run tests.", CTestException::causeOperationFail);
				}
				// setup the subsuite
				pSubSuiteInfo->m_pSubSuite->SetUp(m_bCleanUpBefore);
			}
			else {
				APPLOG("Skipping initialization of subsuite '%s'.", (LPCSTR)pSubSuiteInfo->m_strFilename);
			}
		}

		// seed the random number generator
		int nSeed = SeedRandomNum(m_nRandomSeed);
		pToolset->GetLog()->RecordInfo("Random seed: %d", nSeed);
		
		// reset the state of the application
		m_currentState.Reset();

		// loop and randomly run tests until we get a failure
		// or meet the specified limit criterion
		int nTestCount = 0;
		CTime timeStart = CTime::GetCurrentTime();
		CAdjacencyList* pAdjacencyList = new CAdjacencyList;
		BOOL bMoreTests = TRUE;
		BOOL bUnderLimit = TRUE;
		BOOL bTestPass = TRUE;
		while (bMoreTests && bUnderLimit && bTestPass) {
			// get the list of tests that can be run from the current state
			// in the graph forest, this is the list of edges given the
			// current state
			pAdjacencyList->RemoveAll();
// (if a test has multiple post conditions, it will have been
//  added to the graph multiple times, so it will have a higher
//  chance of being randomly picked to run.)
			m_pGraphForest->GetStateEdges(m_currentState, pAdjacencyList);
			// remove duplicates from the list
			// (tests that have multiple post conditions will have been added
			// to the graph multiple times, so it will show up in the list
			// multiple times, too)
			// for each element in the list, check all subsequent elements
			// for potential duplicity. I think there's a more efficient
			// algorithm, but this will work for now
			for (POSITION posItem = pAdjacencyList->GetHeadPosition(); posItem != NULL; ) {
				// store the current position
				POSITION posCurrentItem = posItem;
				// get the the current item in the list
				AdjacencyListItem* pItem = pAdjacencyList->GetNext(posItem);
				CTest* pTest = pItem->m_pEdge;
				// iterate through the rest of the list
				for (POSITION posDupItem = posItem; posDupItem != NULL; ) {
					POSITION posCurrentDupItem = posDupItem;
					pItem = pAdjacencyList->GetNext(posDupItem);
					// delete any duplicates
					if (pTest == pItem->m_pEdge) {
						pAdjacencyList->RemoveAt(posCurrentDupItem);
					}
				}
				// get the next position (posItem may not be valid because
				// that item may have been deleted
				pAdjacencyList->GetNext(posCurrentItem);
				posItem = posCurrentItem;
			}

			// remove all tests marked as don't run
			for (posItem = pAdjacencyList->GetHeadPosition(); posItem != NULL; ) {
				POSITION posCurrentItem = posItem;
				AdjacencyListItem* pItem = pAdjacencyList->GetNext(posItem);
				CRandomTest* pTest = (CRandomTest*)pItem->m_pEdge;
				ASSERT(pTest->GetType() == TEST_TYPE_RANDOM);
				if (pTest->GetRunState() == CTest::RS_NoRun) {
					pAdjacencyList->RemoveAt(posCurrentItem);
				}
			}
			// are there any tests to run from the current state?
			if (pAdjacencyList->IsEmpty()) {
				bMoreTests = FALSE;
				continue;
			}
			
			// dump the list of tests to choose from
			APPLOG("List of tests to choose from:");
			for (posItem = pAdjacencyList->GetHeadPosition(); posItem != NULL; ) {
				AdjacencyListItem* pItem = pAdjacencyList->GetNext(posItem);
				CTest* pTest = pItem->m_pEdge;
				APPLOG("    %s", (LPCSTR)pTest->GetName());
			}

			// get a random number
			int nTest = GetRandomNum() % pAdjacencyList->GetCount();
			// get the test
			AdjacencyListItem* pItem = pAdjacencyList->GetAt(pAdjacencyList->FindIndex(nTest));
			CRandomTest* pTest = (CRandomTest*)pItem->m_pEdge;
			ASSERT(pTest->GetType() == TEST_TYPE_RANDOM);

			// run the test
			RunTest(pTest);
			nTestCount++;
			
			// update the current state
			// get the post-conditions from the test
			// iterate through the set (post-conditions) and update the
			// current state for each one
			CSet<CVertex> destSet = pTest->GetPostCond();
			for (POSITION destPos = destSet.GetStartPosition(); destPos != NULL; ) {
				// get the destination vertex (post-condition)
				CVertex destVertex = destSet.GetNext(destPos);
				// update the current state
				m_currentState.Update(destVertex);
			}
			
			// did the test pass?
			bTestPass = (pToolset->GetLog()->GetTestFailCount() == 0);
			
			// have we passed the limit set?
			switch (m_RandomLimit) {
				case RandomLimitNum: {
					if (nTestCount >= m_nRandomLimitNum) {
						bUnderLimit = FALSE;
					}
					break;
				}
				case RandomLimitTime: {
					if (CTime::GetCurrentTime() - timeStart >= m_RandomLimitTime) {
						bUnderLimit = FALSE;
					}
					break;
				}
			}
		}

		// record the reason for ended the random test run
		CString strReason;
		if (!bMoreTests) {
			strReason = "Random test run ended because there were no more tests to run.";
		}
		else if (!bUnderLimit) {
			strReason = "Random test run ended because the limit set by the user was hit.";
		}
		else if (!bTestPass) {
			strReason = "Random test run ended because the last test run failed.";
		}
		else {
			strReason = "Reason for ending random test run unknown.";
		}

		pToolset->GetLog()->RecordInfo("%s", (LPCSTR)strReason);
		APPLOG("%s", (LPCSTR)strReason);

		pAdjacencyList->RemoveAll();
		delete pAdjacencyList;

		// finalize all subsuites
        for (posSubSuite = plistSubSuites->GetHeadPosition(); posSubSuite != NULL; ) {
            CSuiteDoc::SubSuiteInfo* pSubSuiteInfo = plistSubSuites->GetNext(posSubSuite);
            // is this subsuite selected to run?
            if (pSubSuiteInfo->m_bRun) {
				APPLOG("Finalizing subsuite '%s'.", (LPCSTR)pSubSuiteInfo->m_strFilename);
				// set the current directory to the subsuite's directory
				::SetCurrentDirectory(pSubSuiteInfo->m_pSubSuite->GetCWD());
				// finalize the subsuite
				pSubSuiteInfo->m_pSubSuite->Finalize();
			}
			else {
				APPLOG("Skipping finalization of subsuite '%s'.", (LPCSTR)pSubSuiteInfo->m_strFilename);
			}
		}
    }
    
    // some critical system error occurred that prevents us from completing the test run
    catch (CCAFEException exception) {
    }

    // some critical error occurred that prevents us from completing the test run
    catch (CTestException exception) {
        CString strMsg;

        strMsg = "Critical error. " + exception.GetMessage() + " Aborting test run.";

        // is a log available?
        if (pToolset->GetLog()) {
            // report the error in the log
            pToolset->GetLog()->RecordCriticalError(strMsg);
        }
        else {
            // report the error via a message box
            AfxMessageBox(strMsg);
        }
    }

	return TRUE;
}

BOOL CSuiteDoc::RunSequential(CToolset* pToolset)
{
	const char* const THIS_FUNCTION = "CSuiteDoc::RunSequential";
	

    // run the tests in order
    try {

        WaitStepInstructions(INITIALIZE_STRING);

        // determine if we are persisting the target between subsuites.
		//REVIEW(chriskoz) we'd better use RTTI rather than rely on so ugly cast in order to get IDETarget
		gbPersistTarget = ((CVCTools*)pToolset)->GetIDE()->GetSettings()->GetBooleanValue(settingPersist, FALSE);
		// if we are persisting the target between subsuites, then we need to know when the last subsuite is
		// being executed so we will know to exit the target during that subsuite's Finalize routine. 
		if(gbPersistTarget)
			gbLastSubSuite = FALSE;
		
		// iterate through the selected subsuites...

		// get the list of subsuites
        CSuiteDoc::SubSuiteList* plistSubSuites = GetSubSuiteList();
		for (int CurrentCycle = 0; CurrentCycle < m_nCycleNumber; CurrentCycle++)
			{
			do 
			{
				// for each subsuite in the list,
				for (POSITION posSubSuite = plistSubSuites->GetHeadPosition(); posSubSuite != NULL; ) {
					CSuiteDoc::SubSuiteInfo* pSubSuiteInfo = plistSubSuites->GetNext(posSubSuite);

					// is this subsuite selected to run?
					if (pSubSuiteInfo->m_bRun) {

						APPLOG("Running subsuite '%s'.", (LPCSTR)pSubSuiteInfo->m_strFilename);
						
						// set the current directory to the subsuite's directory
						::SetCurrentDirectory(pSubSuiteInfo->m_pSubSuite->GetCWD());

						// get the list of tests
						CSubSuite::TestList* plistTests = pSubSuiteInfo->m_pSubSuite->GetTestList();

						// call the subsuite's Initialize function here
						if (!pSubSuiteInfo->m_pSubSuite->Initialize(pToolset)) {
							// if we can't initialize the subsuite, we need to bail out
							throw CTestException("Subsuite intialization failed; unable to run tests.", CTestException::causeOperationFail);
						}

						// let the subsuite set up before running tests
						// pass the users preference on whether to clean up before running the subsuite or not
						pSubSuiteInfo->m_pSubSuite->SetUp(m_bCleanUpBefore);

						// loop continuously, if specified
						// REVIEW(briancr): note that this will only loop through the tests in the first subsuite
						// this is due to the way running tests is designed
						// when we loop through tests, we want to do it in the same instance of the app,
						// but the app is shut down between each subsuite
						// NOTE: this is an infinite loop if m_bLoopTests is TRUE.
						// it is expected that the user will abort the test run by pressing Scroll Lock (or Ctrl+F2)
						// to break the test and then Ctrl+Shift+Alt+End to end the test (throw an exception)


						// for each test in the subsuite,
						for (POSITION posTest = plistTests->GetHeadPosition(); posTest != NULL; ) {
							CTest* pTest = plistTests->GetNext(posTest);
							// is this test selected to run?
							if (pTest->GetRunState() == CTest::RS_Run) {

								// run the test
								RunTest(pTest);
							}
							else {
								APPLOG("Skipping test '%s'.", pTest->GetName());
							}
						}
						// did the user specify to clean up after the subsuite?
						if (m_bCleanUpAfter) {
							// as long as there were no failures in the subsuite, let it clean up
							// otherwise, don't clean up
							if (pToolset->GetLog()->GetSubSuiteFailCount() == 0) {
								pSubSuiteInfo->m_pSubSuite->CleanUp();
							}
						}

						// if the next subsuite position is NULL, then we are currently in the last subsuite.
						// we only need to know this if we are persisting the target between subsuites.
						// for the last subsuite the Finalize() routine will exit the target.
						if((posSubSuite == NULL) && gbPersistTarget)
							gbLastSubSuite = TRUE;

						// call the subsuite's Finalize function
						pSubSuiteInfo->m_pSubSuite->Finalize();

					}
					else {
						APPLOG("Skipping subsuite '%s'.", (LPCSTR)pSubSuiteInfo->m_strFilename);
					}
				}
			}
		while (m_bLoopTests);
		}//end for
    }
    
    // some critical system error occurred that prevents us from completing the test run
    catch (CCAFEException exception) {
    }

    // some critical error occurred that prevents us from completing the test run
    catch (CTestException exception) {
        CString strMsg;

        strMsg = "Critical error. " + exception.GetMessage() + " Aborting test run.";

        // is a log available?
        if (pToolset->GetLog()) {
            // report the error in the log
            pToolset->GetLog()->RecordCriticalError(strMsg);
        }
        else {
            // report the error via a message box
            AfxMessageBox(strMsg);
        }
    }

	return TRUE;
}

BOOL CSuiteDoc::RunTest(CTest* pTest)
{
	// build the list of tests to run
	// generally this is just the single test
	// if dependencies are specified for a test, though
	// we'll build the list of tests here and then run them in order
	CSubSuite::TestList* plistTestsToRun = new CSubSuite::TestList;

	// always run the selected test
	plistTestsToRun->AddHead(pTest);

	// walk through dependencies
	pTest = pTest->GetDependency();
	while (pTest != NULL) {
		// should we run the dependency?
		if (pTest->RunAsDependent()) {
			// the test pointed to by pTest must be run
			// before any of the others in the list, so
			// put it at the beginning of the list
			plistTestsToRun->AddHead(pTest);
		}

		// get the test's dependency
		pTest = pTest->GetDependency();
	}

	// interate through the list of tests
	for (POSITION posTestsToRun = plistTestsToRun->GetHeadPosition(); posTestsToRun != NULL; ) {
		pTest = plistTestsToRun->GetNext(posTestsToRun);
		// add the test to the list of tests run
		m_plistTestsRun->AddTail(pTest);
		APPLOG("Running test '%s'.", pTest->GetName());

		// set the directory for the test
		::SetCurrentDirectory(pTest->GetSubSuite()->GetCWD());
		
		// determine whether the test is a normal test or a list test
		if (pTest->GetType() != TEST_TYPE_LIST) {
			// normal tests are run by this block
			pTest->PreRun();
			// dump the current state if we're randomizing
			if (GetRandomize()) {
				pTest->GetSubSuite()->GetLog()->RecordInfo("Current State: %s.", (LPCSTR)m_currentState.GetName());
			}
			pTest->Run();
			pTest->PostRun();
		}
		else {
			// list tests are run by this block
			try {
				CStdioFile file(pTest->GetListFilename(), CFile::modeRead);

				CRawListString strLine;
				file.ReadString(strLine);
				for(;!strLine.IsEmpty(); file.ReadString(strLine)) {
					strLine.TrimLeft();                                    
					if (strLine[0] != ';') {
						// list tests are run by this block
						pTest->PreRun(strLine);
						strLine.Reset();

						pTest->Run(strLine);
						strLine.Reset();

						pTest->PostRun(strLine);
						strLine.Reset();
					}
				}
			}
			catch(CFileException* e) {
				if(e->m_cause == CFileException::fileNotFound ) {
					APPLOG("Couldn't find list file: %s\n", (LPCSTR)pTest->GetListFilename());
					pTest->GetSubSuite()->GetLog()->RecordFailure("Couldn't find list file: %s", (LPCSTR)pTest->GetListFilename());
				}
				else {
					APPLOG("Couldn't read the list file: %s\n", (LPCSTR)pTest->GetListFilename());
					pTest->GetSubSuite()->GetLog()->RecordFailure("Couldn't read the list file: %s", (LPCSTR)pTest->GetListFilename());
				}
			}
		}
	}
	// delete the list of tests that we ran
	delete plistTestsToRun;
	
	return TRUE;
}

void CSuiteDoc::SetLogDebugOutput(BOOL bLogDebugOutput)
{
	m_bLogDebugOutput = bLogDebugOutput;

	if (gpLog)
		gpLog->SetLogDebugOutput(bLogDebugOutput);
}

/////////////////////////////////////////////////////////////////////////////
// Internal operations

BOOL CSuiteDoc::ReadSuite(CString &strFilename)
{
    // the filename must not be empty
    ASSERT(strFilename);
    ASSERT(!strFilename.IsEmpty());
	
	//here's the very obscure method of determining what template are we in
	//there is no easier way to determine it in MFC
	CString strTemplExt; //file extention opened in the current document
	if(GetDocTemplate( )->GetDocString(strTemplExt, CDocTemplate::filterExt))
	{
		CString strDLLExt;
		strDLLExt.LoadString(IDR_DLLTYPE); //contains ".dll"
		if (strDLLExt.Find(strTemplExt) >0) //we are in IDR_DLLTYPE template
		{ // open the subsuite DLL directly
			// clear the list of subsuites
			// REVIEW(briancr): do we need to unload anything here?
			m_listSubSuites.RemoveAll();
			AddSubSuiteEntry(strFilename);
			return TRUE;
		}
	}
    // get a pointer to the app
    CCAFEDriver *pApp = ((CCAFEDriver *)AfxGetApp());

    CStdioFile fileSuite;
    CString strSubSuiteEntry;
	
    // open the suite file
    if (fileSuite.Open(strFilename, CFile::modeRead))
	{
        APPLOG("Reading suite file (%s).", (LPCTSTR)strFilename);
		// Determine the abolute paths to each subsuite to be loaded.

	   	CString fname, Drive, Ext, Path, FullPath ;
		FullPath = strFilename;
		_splitpath((const char *) FullPath,Drive.GetBuffer(_MAX_DRIVE), Path.GetBuffer(_MAX_DIR),
		fname.GetBuffer(_MAX_FNAME), Ext.GetBuffer(_MAX_EXT) );

		Drive.ReleaseBuffer() ;
		Path.ReleaseBuffer() ;
		Path = Drive+Path;

        // clear the list of subsuites
        // REVIEW(briancr): do we need to unload anything here?
        m_listSubSuites.RemoveAll();

        // read each subsuite entry from the suite file
        while (fileSuite.ReadString(strSubSuiteEntry.GetBuffer(1023), 1023) != NULL) 
		{
            strSubSuiteEntry.ReleaseBuffer();

            // allow comments by placing a semi-colon (;) in the first position
            if (strSubSuiteEntry.GetAt(0) != ';') 
			{
                // remove leading white space
                EliminateLeadingChars(strSubSuiteEntry, " \t");

                // remove trailing \n
                EliminateTrailingChars(strSubSuiteEntry, "\n");
				// find if it's the subsuite entry which matches the build of CADEDRV
				
				// as long as the entry's not empty
				if (!strSubSuiteEntry.IsEmpty())
				{
					CString strOtherSign,strCurrentSign;
					#ifdef _DEBUG
					strCurrentSign="<DEBUG>";
					strOtherSign="<RETAIL>";
					#else
					strCurrentSign="<RETAIL>";
					strOtherSign="<DEBUG>";
					#endif
					if(strSubSuiteEntry.Find(strOtherSign)==0)
						continue;
					if(strSubSuiteEntry.Find(strCurrentSign)==0)
					{
						strSubSuiteEntry=strSubSuiteEntry.Mid(strCurrentSign.GetLength());
		                EliminateLeadingChars(strSubSuiteEntry, " \t");
					}
					else
					{
						TRACE("CAFE warning: Subsuite entry: '%s' build not marked",strSubSuiteEntry);
						APPLOG("CAFE warning: Subsuite entry: '%s' build not marked",strSubSuiteEntry);
					}

					AddSubSuiteEntry(strSubSuiteEntry, Path);
				}
            }
        }

        // close the suite file
        fileSuite.Close();

        // if there were no entries in the suite file, report an error
        if (m_listSubSuites.IsEmpty()) {
            pApp->ErrorMsg(IDS_ErrNoSubSuiteEntries, (LPCTSTR)strFilename);
            return FALSE;
        }

        return TRUE;
    }
    else
	{
        pApp->ErrorMsg(IDS_ErrOpenSuiteFile, (LPCTSTR)strFilename);
        return FALSE;
    }
}

void CSuiteDoc::AddSubSuiteEntry(CString const &strSubSuiteEntry, LPCSTR SuitePath /* =NULL */)
{	//REVIEW(chriskoz)
	//when SuitePath is NULL, it means the DLL is loading directly, without any params
	//so I skip the param parsing (maybe will need to review later)
    CString strSubSuiteFilename;
    CString strSubSuiteParams;
    CSuiteDoc::SubSuiteInfo *pSubSuiteInfo;
	int nPos;
	
	ASSERT(!strSubSuiteEntry.IsEmpty());
	// separate the entry into the filename and params
	if(strSubSuiteEntry[0]=='\"')
	{ // the filename ends at the matching quote
		nPos = strSubSuiteEntry.Find('\"',1); //skip the first quote
	}
	else
	{	// the filename ends at the first white space
		nPos = strSubSuiteEntry.FindOneOf(" \t");
	}
	if (nPos == -1) 
	{
		nPos = strSubSuiteEntry.GetLength();
	}
	if(SuitePath!=NULL)
	{
		strSubSuiteFilename = strSubSuiteEntry.Left(nPos);
		if(strSubSuiteEntry[0]=='\"')
		{	//name was enclosed in quotes
			strSubSuiteFilename=strSubSuiteFilename.Mid(1); //cut the first quote
			nPos++; //skip the enclosing quote while taking params
		}
		strSubSuiteParams = strSubSuiteEntry.Mid(nPos);
	}
	else
		strSubSuiteFilename=strSubSuiteEntry;
		// remove leading white space from params
		EliminateLeadingChars(strSubSuiteParams, " \t");
		// If the entry does not contain a drive letter or "\"
		// prepend to it the suite directory path.
		if((strSubSuiteFilename.Find(":") == -1) &&
			(strSubSuiteFilename.Left(1) != "\\") &&
			SuitePath!=NULL)
			strSubSuiteFilename = CString(SuitePath) + strSubSuiteFilename ;

		// store this entry in the subsuite list
		pSubSuiteInfo = new SubSuiteInfo;
		pSubSuiteInfo->m_pSubSuite = NULL;
		pSubSuiteInfo->m_hDLL = NULL;
		pSubSuiteInfo->m_strFilename = strSubSuiteFilename;
		pSubSuiteInfo->m_strParams = strSubSuiteParams;
		pSubSuiteInfo->m_bRun = TRUE;
		pSubSuiteInfo->m_dwId = 0;
	
		m_listSubSuites.AddTail(pSubSuiteInfo);
}


BOOL CSuiteDoc::WriteSuite(CString &strFilename)
{
    // the filename must not be empty
    ASSERT(strFilename);
    ASSERT(!strFilename.IsEmpty());

    // get a pointer to the app
    CCAFEDriver *pApp = ((CCAFEDriver *)AfxGetApp());

    CStdioFile fileSuite;
    CSuiteDoc::SubSuiteInfo *pSubSuiteInfo;
    CString strSubSuiteEntry;

    // open the suite file
    if (fileSuite.Open(strFilename, CFile::modeCreate | CFile::modeWrite)) {

        ::OutputDebugString("Writing suite file: "+strFilename+"\n");

        // REVIEW(briancr): note that comments are thrown away

        // write each subsuite entry to the suite file
        for (POSITION pos = m_listSubSuites.GetHeadPosition(); pos != NULL; ) {
            pSubSuiteInfo = m_listSubSuites.GetNext(pos);
            strSubSuiteEntry = pSubSuiteInfo->m_strFilename+" "+pSubSuiteInfo->m_strParams+"\n";
            fileSuite.WriteString(strSubSuiteEntry);
        }

        // close the suite file
        fileSuite.Close();
        return TRUE;
    }
    else {
        pApp->ErrorMsg(IDS_ErrOpenSuiteFile, (LPCTSTR)strFilename);
        return FALSE;
    }
}


BOOL CSuiteDoc::LoadSubSuites(void)
{
    POSITION pos;
    POSITION posCur;

    // the subsuite list cannot be empty
    ASSERT(!m_listSubSuites.IsEmpty());

    CSuiteDoc::SubSuiteInfo *pSubSuiteInfo;

    // REVIEW(briancr): should we unload any that are loaded? Will ReadSuite take care of this?

    // load each subsuite
    for (pos = m_listSubSuites.GetHeadPosition(); pos != NULL; ) {
        posCur = pos;
        pSubSuiteInfo = m_listSubSuites.GetNext(pos);

        // load the subsuite into at the given position
        if (!LoadSubSuite(pSubSuiteInfo)) {
            m_listSubSuites.RemoveAt(posCur);
        }
    }

	// build the graph forest for all randomizable tests loaded
	BuildGraphForest();

    return TRUE;
}

// typedef for IsDebugBuild and CreateSubSuite function in subsuite DLL
extern "C" typedef BOOL (*ISDEBUGBUILD)(void);
extern "C" typedef CSubSuite* (* CREATESUBSUITE)(LPCSTR);

BOOL CSuiteDoc::LoadSubSuite(CSuiteDoc::SubSuiteInfo *pSubSuiteInfo)
{
    const int cBuf = 1000;
	char aBuf[cBuf];

	// the filename cannot be empty
    ASSERT(!pSubSuiteInfo->m_strFilename.IsEmpty());

    // get a pointer to the app
    CCAFEDriver *pApp = ((CCAFEDriver *)AfxGetApp());

    BOOL bLoadSuccess = TRUE;

    HINSTANCE hSubSuiteDLL;
    ISDEBUGBUILD funcIsDebugBuild;
    CREATESUBSUITE funcCreateSubSuite;
    CSubSuite *pSubSuite;

    // load the subsuite (DLL)
    hSubSuiteDLL = ::LoadLibrary(pSubSuiteInfo->m_strFilename);
    if (hSubSuiteDLL != NULL) {
        // get the address of the IsDebugBuild function is subsuite DLL
        funcIsDebugBuild = (ISDEBUGBUILD)::GetProcAddress(hSubSuiteDLL, "IsDebugBuild");
        if (funcIsDebugBuild != NULL) {
            // if the build types don't match, report error
            #ifdef _DEBUG
            if (!funcIsDebugBuild()) {
				sprintf(aBuf, "Unable to properly load the subsuite '%s' because it is a retail build and the driver is a debug build.", (LPCTSTR)pSubSuiteInfo->m_strFilename);
                pApp->ErrorMsg(aBuf);
            #else
            if (funcIsDebugBuild()) {
                sprintf(aBuf, "Unable to properly load the subsuite '%s' because it is a debug build and the driver is a retail build.", (LPCTSTR)pSubSuiteInfo->m_strFilename);
				pApp->ErrorMsg(aBuf);
            #endif
                bLoadSuccess = FALSE;
            }
            // if the build types match, continue loading
            else {
                // get the address of the CreateSubSuite function in the subsuite
                funcCreateSubSuite = (CREATESUBSUITE)::GetProcAddress(hSubSuiteDLL, "CreateSubSuite");
                if (funcCreateSubSuite != NULL) {

                    // call the CreateSubSuite function in the subsuite
                    // get a pointer to the subsuite object in the subsuite DLL
                    pSubSuite = funcCreateSubSuite(pSubSuiteInfo->m_strParams);
					pSubSuite->m_SuiteParams=pApp->ReadCmdLineSwitches(pSubSuite->m_CmdLine);

                    // store the pointer to the subsuite and the handle to the DLL in the subsuite info
                    pSubSuiteInfo->m_pSubSuite = pSubSuite;
                    pSubSuiteInfo->m_hDLL = hSubSuiteDLL;
                    pSubSuiteInfo->m_dwId = ++m_cSubSuites;

                    // initialize the subsuite
                    pSubSuiteInfo->m_pSubSuite->LoadInitialization(pSubSuiteInfo->m_hDLL);
                    APPLOG("Loaded subsuite (%s).", (LPCSTR)pSubSuiteInfo->m_strFilename);
                }
                // CreateSubSuite entry point not found
                else {
					sprintf(aBuf, "Unable to properly load the subsuite '%s' because it does not contain a CreateSubSuite function.", (LPCTSTR)pSubSuiteInfo->m_strFilename);
                    pApp->ErrorMsg(aBuf);
                    bLoadSuccess = FALSE;
                }
            }
        }
        // IsDebugBuild entry point not found
        else {
            sprintf(aBuf, "Unable to properly load the subsuite '%s' because it does not contains an IsDebugBuild function.", (LPCTSTR)pSubSuiteInfo->m_strFilename);
			pApp->ErrorMsg(aBuf);
            bLoadSuccess = FALSE;
        }

        // was there some problem with the DLL loaded?
        if (!bLoadSuccess) {
            // unload the DLL
            ::FreeLibrary(hSubSuiteDLL);
        }
    }
    // error from LoadLibrary
    else {
		// report the error to the user
		char aBufErr[500];
		DWORD dwError = ::GetLastError();

		switch (dwError) {
			// general failure
			case ERROR_GEN_FAILURE: {
				sprintf(aBufErr, "A general failure occurred");
				break;
			}
			// DLL init failed
			case ERROR_DLL_INIT_FAILED: {
				sprintf(aBufErr, "A dynamic link library (DLL) initialization routine failed");
				break;
			}
			// invalid DLL
			case ERROR_BAD_EXE_FORMAT:
			case ERROR_INVALID_DLL: {
				sprintf(aBufErr, "The DLL file is invalid, corrupt, or damaged");
				break;
			}
			// DLL not found
			case ERROR_MOD_NOT_FOUND:
			case ERROR_DLL_NOT_FOUND: {
				sprintf(aBufErr, "The DLL file cannot be found");
				break;
			}
			default: {
				sprintf(aBufErr, "Unknown error");
				break;
			}
		}

		sprintf(aBuf, "An error occurred loading subsuite '%s': %s. GetLastError = %d.", (LPCTSTR)pSubSuiteInfo->m_strFilename, aBufErr, dwError);
		pApp->ErrorMsg(aBuf);
        bLoadSuccess = FALSE;
    }

    return bLoadSuccess;
}

void CSuiteDoc::BuildGraphForest(void)
{
	m_pGraphForest = new CGraphForest;

	// iterate through all the loaded tests and add all tests that are randomizable
    SubSuiteInfo* pSubSuiteInfo;
	CSubSuite::TestList* plistTests;
	CTest* pTest;
	POSITION posSubSuite;
	POSITION posTest;
	// iterate through each subsuite
    for (posSubSuite = m_listSubSuites.GetHeadPosition(); posSubSuite != NULL; ) {
        pSubSuiteInfo = m_listSubSuites.GetNext(posSubSuite);
		// get the list of tests
		plistTests = pSubSuiteInfo->m_pSubSuite->GetTestList();
		// iterate through each test
		for (posTest = plistTests->GetHeadPosition(); posTest != NULL; ) {
			pTest = plistTests->GetNext(posTest);

			// is this a randomizable test?
			if (pTest->GetType() == TEST_TYPE_RANDOM) {
				// add the test to the graph forest
				// get the pre- and post-condition(s)
				CSet<CVertex> srcSet = ((CRandomTest*)pTest)->GetPreCond();
				CSet<CVertex> destSet = ((CRandomTest*)pTest)->GetPostCond();
				// multiple pre-conditions are ORed together
				// multiple post-conditions are ANDed together
				// so for each pre-condition vertex, add all the post-conditions
				// vertices eminating from it
				for (POSITION srcPos = srcSet.GetStartPosition(); srcPos != NULL; ) {
					// get the source vertex (pre-condition)
					CVertex srcVertex = srcSet.GetNext(srcPos);
					for (POSITION destPos = destSet.GetStartPosition(); destPos != NULL; ) {
						// get the destination vertex (post-condition)
						CVertex destVertex = destSet.GetNext(destPos);
						// add the edge (test) to the graph
						m_pGraphForest->AddEdge(pTest, srcVertex, destVertex);
					}
				}
			}
		}
	}
}

BOOL CSuiteDoc::EliminateLeadingChars(CString &str, CString strSet)
{
    // the set string should not be empty
    ASSERT(!strSet.IsEmpty());

    while (str.FindOneOf(strSet) == 0) {
        str = str.Mid(1);
    }

    return TRUE;
}

BOOL CSuiteDoc::EliminateTrailingChars(CString &str, CString strSet)
{
    // the set string should not be empty
    ASSERT(!strSet.IsEmpty());

    if (str.IsEmpty()) {
        return TRUE;
    }
    while (!str.IsEmpty() && str.FindOneOf(strSet) == str.GetLength()-1) {
        str = str.Left(str.GetLength()-1);
    }

    return TRUE;
}

void CSuiteDoc::DelegateSuiteExecution(void)
{
  ((CCAFEDriver*)AfxGetApp())->CallRunSuite(this);
}

int CSuiteDoc::SeedRandomNum(int nSeed)
{
	// if a seed is not specified (zero), use the system time
	if (!nSeed) {
		nSeed = (int)time(NULL);
	}
	srand(nSeed);

	return nSeed;
}

int CSuiteDoc::GetRandomNum(void)
{
	return rand();
}

#ifdef _DEBUG
void CSuiteDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CSuiteDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSuiteDoc serialization

void CSuiteDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}
