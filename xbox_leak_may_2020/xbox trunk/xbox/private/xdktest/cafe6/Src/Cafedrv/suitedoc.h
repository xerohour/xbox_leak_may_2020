/////////////////////////////////////////////////////////////////////////////
// suitedoc.h
//
// email        date            change
// briancr      10/25/94        created
//
// copyright 1994 Microsoft

// Interface of the CSuiteDoc class

#ifndef __SUITEDOC_H__
#define __SUITEDOC_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "subsuite.h"
#include "portdoc.h"
#include "graph.h"

/////////////////////////////////////////////////////////////////////////////
// CSuiteDoc document

class CSuiteDoc : public CDocument
{
// data types
public:
	struct SubSuiteInfo
	{
		CSubSuite *m_pSubSuite;
		HINSTANCE m_hDLL;
		CString m_strFilename;
		CString m_strParams;
		BOOL m_bRun;
		DWORD m_dwId;
	};
	typedef CTypedPtrList<CPtrList, SubSuiteInfo*> SubSuiteList;

// constructor/destructor
protected:
	CSuiteDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSuiteDoc)

// Attributes
public:
	SubSuiteList* GetSubSuiteList(void);

// Operations
public:

// Initialize doc data members
	void SetConsolePortDoc();
	void SetLogViewPortDoc(); 

	BOOL AddSubSuite(int Index, CString &strFilename, CString &strParams);
	BOOL RemoveSubSuite(int Index);
	BOOL Run(void);
	BOOL CreateViewports(void);

	UINT RunSuite(void);

// Log data attributes
	void SetResultsFile(LPCSTR pszResultsFile)                      { m_strResultsFile = pszResultsFile; }
	void SetSummaryFile(LPCSTR pszSummaryFile)                      { m_strSummaryFile = pszSummaryFile; }
	void SetWorkingDir(LPCSTR pszWorkingDir)                        { m_strWorkingDir = pszWorkingDir; }
	void SetMaxFail(int nMaxFail)                                           { m_nMaxFail = nMaxFail; }
	void SetPostResults(BOOL bPostResults)                          { m_bPostResults = bPostResults; }
	void SetRecordComments(BOOL bRecordComments)            { m_bRecordComments = bRecordComments; }
	void SetLogDebugOutput(BOOL bLogDebugOutput);

	CString GetResultsFile(void)                                            { return m_strResultsFile; }
	CString GetSummaryFile(void)                                            { return m_strSummaryFile; }
	CString GetWorkingDir(void)                                                     { return m_strWorkingDir; }
	int GetMaxFail(void)                                                            { return m_nMaxFail; }
	BOOL GetPostResults(void)                                                       { return m_bPostResults; }
	BOOL GetRecordComments(void)                                            { return m_bRecordComments; }
	BOOL GetLogDebugOutput(void)                                            { return m_bLogDebugOutput; }

// Suite data attributes
	enum RandomLimit { RandomLimitNone, RandomLimitNum, RandomLimitTime };
	
	void SetCleanUpBefore(BOOL bCleanUpBefore)                      { m_bCleanUpBefore = bCleanUpBefore; }
	void SetCleanUpAfter(BOOL bCleanUpAfter)                        { m_bCleanUpAfter = bCleanUpAfter; }
	void SetCycleNumber(int nCycleNumber)                                      { m_nCycleNumber = nCycleNumber; }
	void SetLoopTests(BOOL bLoopTests)                                      { m_bLoopTests = bLoopTests; }
	void SetRandomize(BOOL bRandomize)                                      { m_bRandomize = bRandomize; }
	void SetRandomSeed(int nRandomSeed)                                     { m_nRandomSeed = nRandomSeed; }
	void SetRandomLimit(RandomLimit limit)                          { m_RandomLimit = limit; }
	void SetRandomLimitNum(int nLimit)                                      { m_nRandomLimitNum = nLimit; }
	void SetRandomLimitTime(CTimeSpan& time)                        { m_RandomLimitTime = time; }

	BOOL GetCleanUpBefore(void)                                                     { return m_bCleanUpBefore; }
	BOOL GetCleanUpAfter(void)                                                      { return m_bCleanUpAfter; }
	int GetCycleNumber(void)                                                         { return m_nCycleNumber; }
	BOOL GetLoopTests(void)                                                         { return m_bLoopTests; }
	BOOL GetRandomize(void)                                                         { return m_bRandomize; }
	int GetRandomSeed(void)                                                         { return m_nRandomSeed; }
	RandomLimit GetRandomLimit(void)                                        { return m_RandomLimit; }
	int GetRandomLimitNum(void)                                                     { return m_nRandomLimitNum; }
	CTimeSpan GetRandomLimitTime(void)                                      { return m_RandomLimitTime; }

	BOOL CanRandomize(void)                                                         { ASSERT(m_pGraphForest); return !m_pGraphForest->IsEmpty(); }
	CGraphForest* GetGraphForest(void)                                      { ASSERT(m_pGraphForest); return m_pGraphForest; }

	CSubSuite::TestList* GetTestsRunList(void)                      { return m_plistTestsRun; }
	CState* GetCurrentState(void)                                           { return &m_currentState; }

// Internal operations
protected:
	BOOL RunRandom(CToolset* pToolset);
	BOOL RunSequential(CToolset* pToolset);
	BOOL RunTest(CTest* pTest);

	BOOL ReadSuite(CString &strFilename);
	void AddSubSuiteEntry(CString const &strSubSuiteEntry, LPCSTR SuitePath = NULL);

	BOOL WriteSuite(CString &strFilename);
	BOOL LoadSubSuites(void);
	BOOL LoadSubSuite(SubSuiteInfo *pSubSuiteInfo);

	void BuildGraphForest(void);

	BOOL EliminateLeadingChars(CString &str, CString strSet);
	BOOL EliminateTrailingChars(CString &str, CString strSet);

	int SeedRandomNum(int nSeed);
	int GetRandomNum(void);
	
//      static UINT WrapRunSuite(LPVOID pParam) { return ((CSuiteDoc*)pParam)->RunSuite(); }

  void DelegateSuiteExecution(void);

// Data
protected:
	// data types
	SubSuiteList m_listSubSuites;

	// this suite's log viewport document
	CViewportDoc* m_pLogViewport;

	// this suite's console viewport document
	CViewportDoc* m_pConsoleViewport;

	// suite thread
	CWinThread* m_pthreadSuite;

  static int m_cSubSuites;
  static unsigned m_uDocCount;

	BOOL m_bViews;

	// store a list of the tests run
	CSubSuite::TestList* m_plistTestsRun;

	// store the current state of the app (for random tests)
	CState m_currentState;

// suite data
protected:
	BOOL m_bCleanUpBefore;
	BOOL m_bCleanUpAfter;
	int m_nCycleNumber;
	BOOL m_bLoopTests;
	BOOL m_bRandomize;
	int m_nRandomSeed;
	RandomLimit m_RandomLimit;
	int m_nRandomLimitNum;
	CTimeSpan m_RandomLimitTime;

	CGraphForest* m_pGraphForest;

// log data
protected:
	CString m_strResultsFile;
	CString m_strSummaryFile;
	CString m_strWorkingDir;
	int m_nMaxFail;
	BOOL m_bPostResults;
	BOOL m_bRecordComments;
	BOOL m_bLogDebugOutput;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuiteDoc)
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument() ;

// Implementation
public:
	virtual ~CSuiteDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// the view is a friend of the doc
	friend class CSuiteView;

};

#endif // __TESTDOC_H__
