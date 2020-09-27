/////////////////////////////////////////////////////////////////////////////
// subsuite.cpp
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Implementation of the CSubSuite class

#include "stdafx.h"
#include "subsuite.h"
#include "toolset.h"
#include "test.h"
#include "..\support\guitools\imewrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

IMPLEMENT_DYNAMIC(CSubSuite, CObject);

CSubSuite::CSubSuite(LPCSTR pszName, LPCSTR pszOwner)
: m_strName(pszName),
  m_strOwner(pszOwner),
  m_pToolset(NULL),
  m_pLog(NULL)
{
}

CSubSuite::~CSubSuite()
{
	CTest* pTest;

	// REVIEW(briancr): clean up the test list
	for (POSITION pos = m_listTests.GetHeadPosition(); pos != NULL; ) {
		pTest = m_listTests.GetNext(pos);

		// delete the test data
		delete pTest;
	}

	// remove all entries from the list
	m_listTests.RemoveAll();
	// delete the parameters allocated in CCAafeDrv::ReadCmdLineSwitches()
	if (m_SuiteParams) {
		delete m_SuiteParams;
		m_SuiteParams=NULL;
    }

}

///////////////////////////////////////////////////////////////////////////////
//	Attributes

CString CSubSuite::GetName(void)
{
	// the subsuite name cannot be empty
	ASSERT(!m_strName.IsEmpty());

	return m_strName;
}

CString CSubSuite::GetOwner(void)
{
	// the owner name cannot be empty
	ASSERT(!m_strOwner.IsEmpty());

	return m_strOwner;
}

CSubSuite::TestList* CSubSuite::GetTestList(void)
{
	// the list of tests cannot be empty
	ASSERT(!m_listTests.IsEmpty());

	return &m_listTests;
}

CToolset* CSubSuite::GetToolset(void)
{
	// the toolset cannot be NULL
	ASSERT(m_pToolset);

	return m_pToolset;
}

CLog* CSubSuite::GetLog(void)
{
	// the log cannot be NULL
	ASSERT(m_pLog);

	return m_pLog;
}

CString CSubSuite::GetCWD(void)
{
	// the CWD must be set
	ASSERT(!m_strCWD.IsEmpty());
	
	return m_strCWD;
}

///////////////////////////////////////////////////////////////////////////////
//	Operations

void CSubSuite::SetUp(BOOL bCleanUp)
{
	if (bCleanUp) {
		CleanUp();
	}
}

void CSubSuite::CleanUp(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// supporting operations

void CSubSuite::LoadInitialization(HINSTANCE hDLL)
{
	m_hDLL = hDLL;
	InitializeTestList();

	// get the current working directory for this test and store it
	// get the path name of the subsuite
	char szPathName[MAX_PATH];
	::GetModuleFileName(m_hDLL, szPathName, MAX_PATH);
    // split the full path/filename apart
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	_splitpath(szPathName, szDrive, szDir, NULL, NULL);
	// store the CWD
	m_strCWD = CString(szDrive) + CString(szDir);
}

BOOL CSubSuite::Initialize(CToolset* pToolset)
{
	// the toolset and log must be valid
	ASSERT(pToolset);
	ASSERT(pToolset->GetLog());
	if (!pToolset || !pToolset->GetLog()) {
		return FALSE;
	}

	// store the toolset and log
	m_pToolset = pToolset;
	m_pLog = pToolset->GetLog();

	// start the subsuite
	m_timeStart = CTime::GetCurrentTime();

	if (GetLog()->BeginSubSuite(GetName(), GetOwner())) {

		// Log all version and command line information for all tools.
		const CToolPtrList& list = m_pToolset->GetToolList();

		CTarget* pTool;
  
		for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
		{
			pTool = list.GetNext(pos);
			// skip tools that have no filename
			if (!(pTool->GetSettings()->GetTextValue(settingFilename)).IsEmpty()) {
				GetLog()->RecordInfo("Tool: %s", (LPCSTR)pTool->GetSettings()->GetTextValue(settingFilename));
				GetLog()->RecordInfo("  Cmd Line: %s", (LPCSTR)pTool->GetFullCmdLine());
				GetLog()->RecordInfo("  Version: %s", (LPCSTR)pTool->GetVersion());
			}
		}
  
		GetLog()->RecordInfo("");
	}
	else {
		// if the subsuite doesn't start correctly, indicate this
		return FALSE;
	}

	return TRUE;
}

BOOL CSubSuite::Finalize(void)
{
	// end the subsuite
	m_timeEnd = CTime::GetCurrentTime();
	CTimeSpan elapsed_time(m_timeEnd - m_timeStart);
	m_pToolset->GetLog()->RecordInfo("Subsuite elapsed time %s", elapsed_time.Format("%H:%M:%S"));

	return m_pToolset->GetLog()->EndSubSuite();
}


///////////////////////////////////////////////////////////////////////////////
// CGUISubSuite

IMPLEMENT_DYNAMIC(CGUISubSuite, CSubSuite);

CGUISubSuite::CGUISubSuite(LPCSTR pszName, LPCSTR pszOwner)
: CSubSuite(pszName, pszOwner)
{
}

CGUISubSuite::~CGUISubSuite()
{
}

BOOL CGUISubSuite::Initialize(CToolset* pToolset)
{
	// call the base class
	return CSubSuite::Initialize(pToolset);
}

BOOL CGUISubSuite::Finalize(void)
{
	// call the base class
	return CSubSuite::Finalize();
}


///////////////////////////////////////////////////////////////////////////////
// CIDESubSuite

IMPLEMENT_DYNAMIC(CIDESubSuite, CGUISubSuite);

CIDESubSuite::CIDESubSuite(LPCSTR pszName, LPCSTR pszOwner)
: CGUISubSuite(pszName, pszOwner)
{
}

CIDESubSuite::~CIDESubSuite()
{
}

CIDE* CIDESubSuite::GetIDE(void)
{
	// the IDE object must be valid
	ASSERT(m_pIDE);

	return m_pIDE;
}

BOOL CIDESubSuite::Initialize(CToolset* pToolset)
{
	const char* const THIS_FUNCTION = "CIDESubSuite::Initialize";

	// call the base class
	if (!CGUISubSuite::Initialize(pToolset)) {
		return FALSE;
	}

	// REVIEW(briancr): use RTTI for this cast, or at least to validate it
	// initialize the IDE object
	m_pIDE = ((CVCTools*)GetToolset())->GetIDE();

	// launch the IDE (if it's not already launched)
	if (!GetIDE()->IsLaunched()) {
		GetIDE()->Launch();
	}

	// the IDE must be launched at this point
	ASSERT(GetIDE()->IsLaunched());
	if (!m_pIDE->IsLaunched()) {
		GetLog()->RecordFailure("%s: cannot launch the IDE; unable to continue.", THIS_FUNCTION);
		// throw exception
	}

	// set focus to the IDE
	::SetForegroundWindow(GetIDE()->m_hWnd);

	return TRUE;
}

BOOL CIDESubSuite::Finalize(void)
{
	// only go through the ide exit sequence if it is launched and the user chose to do so.
	if(GetIDE()->IsLaunched() && GetIDE()->GetSettings()->GetBooleanValue(settingClose))
	{
		// if we are persisting the target between subsuites, then we only want to exit the ide after the last subsuite.
		if(!gbPersistTarget || gbLastSubSuite)
		{	// close the IDE
			GetIDE()->Exit();
		}
	}

	// Turn off IMEInUse flag for the next test.
	IME.IMEInUse(FALSE);
	// call the base class
	return CGUISubSuite::Finalize();
}

void CIDESubSuite::ExpectedMemLeaks(int nExpectedLeaks)
{
	GetIDE()->m_nExpectedLeaks = nExpectedLeaks;
}

void CIDESubSuite::CleanUp(void)
{
	GetIDE()->RecordHeapUse();
	CGUISubSuite::CleanUp();
}
