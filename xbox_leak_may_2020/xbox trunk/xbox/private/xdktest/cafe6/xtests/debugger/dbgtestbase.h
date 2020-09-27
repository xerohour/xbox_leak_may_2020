///////////////////////////////////////////////////////////////////////////////
// DbgTestBase.h
//
//	Created by:		MichMa
//	Date:			3/30/98
//
//	Description:	Declaration of base class for all debugger tests.


#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __DBGTESTBASE_H__
#define __DBGTESTBASE_H__

extern int ReturnCode;
extern bp  * ReturnBP;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

#define EXPECT_TRUE(f)    ((f)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_FALSE(f)    ((f)?LogResult(__LINE__, FAILED,#f):LogResult(__LINE__, PASSED,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f,ReturnCode));
#define EXPECT_VALIDBP(f) (((ReturnBP=(f))!=NULL)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_TRUE_COMMENT(f,comment) ((f)?LogResult(__LINE__, PASSED,#f,0,#comment):LogResult(__LINE__, FAILED,#f,0,#comment));

#define ABORT_SUBTEST() THROW( new CSubTestException )

#define EXPECT_SUBTEST_OLD(f)  if(f) { LogResult(__LINE__, PASSED,#f);} \
				else { LogResult(__LINE__, FAILED,#f); ABORT_SUBTEST(); }

#define EXPECT_SUBTEST(f)   if(f) { LogResult(__LINE__, PASSED,#f);} \
							else {LogResult(__LINE__, FAILED,#f); }

#define INFO(txt) WaitStepInstructions(#txt); Sleep (5000);

class CSubTestException : public CException {
	DECLARE_DYNAMIC( CSubTestException );
};

#define InitProject SetProject
#define RestoreFocusToSrcWnd() MST.DoKeys("{ESC}")

enum ProjType
{
	PT_EXE,
	PT_EXT,
	PT_INT
};

enum TestType
{
	TT_TEST,
	TT_SUBTEST,
	TT_TESTCASE
};

enum StartEndTest
{
	START_TEST,
	END_TEST
};

///////////////////////////////////////////////////////////////////////////////
//	class CDbgTestBase used as base for debugging tests

class CDbgTestBase : public CTest

{
	DECLARE_DYNAMIC(CDbgTestBase)
	friend class CStartEndTest;

public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CDbgTestBase(/*int metricID,*/ CIDESubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

protected: // data
	CString	m_strProjectDir;
	CString	m_strSrcDir;
	CString	m_strProjectName;
	CString	m_strProjectLoc;
	CString	m_szErrorText;
	int m_platform;
	ProjType m_projType;

	COSource src;
	COProject prj;
	CODebug dbg;
	COExpEval cxx;
	COBreakpoints bps;
	COStack stk;
	UIStack uistack;
	UIMemory uimem;
	COMemory mem;
	UIVariables uivar;
	COBrowse browse;
	COFile file;
	UIWatch uiwatch;
	UIQuickWatch uiqw;
	UIDAM uidam;
	UIRegisters uiregs;
	CODAM dam;
	COThreads threads;
	COLocals locals;
	CORegisters regs;
	COExpEval ee;
	COWatch watch;
	COClipboard clipboard;
	COApplication app;
	COAuto coauto;
	COConnection *pConnection;

	typedef enum {PROJECT_EXE, PROJECT_DLL, PROJECT_XBE} PROJECT_TYPE;
	typedef enum {fDefault, fBuildOnly} PROJECT_FLAGS;

	BOOL SetProject(LPCSTR projName, PROJECT_TYPE projType = PROJECT_EXE,
					PROJECT_FLAGS = fDefault, LPCSTR cfgName=NULL);
	BOOL SetXboxProject(LPCSTR projName, PROJECT_FLAGS = fDefault, LPCSTR cfgName=NULL);
	BOOL CreateXboxProjectFromSource(LPCSTR strProjBase, LPCSTR strSrcDir,
		LPCSTR strSrcFile, LPCSTR strSrcFile2 = 0);
#if 0
	virtual void InitTestState() { dbg.StopDebugging(); dbg.StepOver(); }
	void RunCases();
#endif

	BOOL PrepareProject(LPCSTR projName);
	BOOL IsProjectReady(ProjType projType);
	BOOL StartDebugging(void);
	BOOL StopDbgCloseProject(void);
	BOOL VerifyProjectRan(int exitcode = 0);
	BOOL StepInUntilFrameAdded(CString addframe);
	BOOL StepInUntilFrameRemoved(void);

	// shorthand method of filling EXPR_INFO object.
	void FillExprInfo(EXPR_INFO &expr_info, int state, LPCSTR type, LPCSTR name, LPCSTR value);

	// compares two EXPR_INFO objects
	BOOL ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
					int total_rows = 1);

	// tells whether string represents an address as displayed in an ee window.
	// for example: 0x1234abcd "xyz"
	// support function for ExprInfoIs()
	BOOL ValueIsAddress(CString str);
	LPCSTR GetThreadIDStr(LPCSTR szThreadIDVar);

	void LogResult(int line, int result,LPCSTR szoperation, int code=0, LPCSTR szComment="");
	LPCSTR HomeDir(void);
	LPCSTR FullPath(LPCSTR szAddPath);

	void LogTestHeader(LPCSTR test_desc_str, TestType testtype = TT_TEST);
	void LogTestStartEnd(LPCSTR test_desc_str, StartEndTest startend = START_TEST);
	void LogSubTestHeader(LPCSTR test_desc_str);
	void LogTestBegin(LPCSTR test_desc_str);
	void LogTestEnd(LPCSTR test_desc_str);
	void LogTestEnd(LPCSTR test_desc_str, CTimeSpan elapsed_time, bool timing);

	void VerifyLocalsWndString(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber = 1);
	BOOL LocalsWndStringIs(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber = 1);

	CString GetBldLog();
	BOOL VerifyBuildOccured( BOOL iBuild );
	int VerifyBuildString( LPCSTR verifyString, BOOL fExist = TRUE );
	int VerifyDebugOutputString( LPCSTR verifyString, BOOL fExist = TRUE  );

	//unused in xbox - void VerifyECSuccess();
	//unused in xbox - void VerifyECCompileError();
	//unused in xbox - void VerifyECUnsupportedEdit();
	

	BOOL CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc );
	void RemoveTestFiles( CString szProjLoc );

	//unused in xbox - BOOL ModifyCodeLine(LPCSTR szFind, LPCSTR szReplace);
	//unused in xbox - BOOL ModifyCode(LPCSTR szFind, LPCSTR szReplace, int nLines = 1);
	BOOL IsMsgBox(CString strMsg, CString strBtn);

	// emmang ADDITION Xbox-specific methods
	CString FindInPath(LPCSTR szAppName);
	BOOL XboxSetTarget(LPCSTR szXbox);
	BOOL XboxDebuggerConnect();
	BOOL XboxCopyToTarget(LPCSTR strXbePath = NULL);
	CString XboxGetTarget();
};


class CStartEndTest
{

public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CStartEndTest(LPCSTR testName, CDbgTestBase *test, bool timing = FALSE);
	~CStartEndTest();

protected: // data
	CString	m_testName;
	bool m_timing;
	CTime m_startTime;
	CTime m_endTime;
	CDbgTestBase *m_Test;
};


void CleanUp(LPCSTR szDirName);
void KillFiles(LPCSTR szPathName, LPCSTR szFileName = NULL);
CString GetRemoteXboxIP();

extern char szecEditTypes[6][256];

// Xbox additions

#define REMOTE_XBOX_IP     GetRemoteXboxIP()
#define REMOTE_XBOX_DIR    "e:\\cafe6"
#define REMOTE_XBOX_FILE   REMOTE_XBOX_DIR "\\dbgtest.xbe"

#define TMP_PROJ_DIR       "c:\\cafe6tmp"

#endif	// __DBGTESTBASE_H__
