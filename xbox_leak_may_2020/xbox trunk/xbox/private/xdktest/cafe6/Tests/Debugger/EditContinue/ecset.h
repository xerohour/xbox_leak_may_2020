// ecset.h
//
//	Created by		Date
//		YefimS		7/2/97
//
//	Description
//		Definition of base class containing common data and methods for all
//		Edit And Continue Test Sets.


#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __ECSET_H__
#define __ECSET_H__

extern int ReturnCode;
extern bp  * ReturnBP;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS
#define EXPECT_TRUE(f)    ((f)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f,ReturnCode));
#define EXPECT_VALIDBP(f) (((ReturnBP=(f))!=NULL)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_TRUE_COMMENT(f,comment) ((f)?LogResult(__LINE__, PASSED,#f,0,#comment):LogResult(__LINE__, FAILED,#f,0,#comment));

#define ABORT_SUBTEST() THROW( new CSubTestException )

#define EXPECT_SUBTEST_OLD(f)  if(f) { LogResult(__LINE__, PASSED,#f);} \
				else { LogResult(__LINE__, FAILED,#f); ABORT_SUBTEST(); }

#define EXPECT_SUBTEST(f)   if(f) { LogResult(__LINE__, PASSED,#f);} \
							else {LogResult(__LINE__, FAILED,#f); }

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

typedef enum 
{
	EC_NO_STACK_FRAME, 
	EC_CURRENT_FRAME_AFTER_IP, 
	EC_CURRENT_FRAME_BEFORE_IP, 
	EC_CURRENT_FRAME_AT_IP, 
	EC_PARENT_FRAME_AFTER_IP, 
	EC_PARENT_FRAME_BEFORE_IP
} EC_EDIT_TYPE;

///////////////////////////////////////////////////////////////////////////////
//	class CECTestSet used as base for debugging tests

class CECTestSet : public CTest
{
//	DECLARE_TEST(CECTestSet, CIDESubSuite)
	DECLARE_DYNAMIC(CECTestSet)
	friend class CStartEndTest;

public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CECTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);
//	CECTestSet() {};

protected: // data
	CString	m_strProjectDir;
	CString	m_strSrcDir;
	CString	m_strProjectName;
	CString	m_strProjectLoc;
	CString	m_szErrorText;
	int m_platform;
	ProjType m_projType;

	COSource src;
	COProject proj;
	CODebug dbg;
	COExpEval cxx;
	COBreakpoints bps;
	COStack stk;
	UIMemory uimem;
	COMemory mem;
	UIVariables uivar;
	COBrowse browse;
	COFile file;
	UIWatch uiwatch;
	UIQuickWatch uiqw;
	COLocals locals;

	typedef enum {PROJECT_EXE, PROJECT_DLL} PROJECT_TYPE;
	typedef enum {fDefault, fBuildOnly} PROJECT_FLAGS;

public:

	BOOL SetProject(LPCSTR projName, PROJECT_TYPE projType = PROJECT_EXE,
					PROJECT_FLAGS = fDefault,LPCSTR cfgName=NULL);
	BOOL PrepareProject(LPCSTR projName);
	BOOL IsProjectReady(ProjType projType);

	BOOL StartDebugging(void);
	BOOL StopDbgCloseProject(void);

	// shorthand method of filling EXPR_INFO object.
	void FillExprInfo(EXPR_INFO &expr_info, int state, LPCSTR type, LPCSTR name, LPCSTR value);

	// compares two EXPR_INFO objects
	BOOL ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
					int total_rows = 1);

	// tells whether string represents an address as displayed in an ee window.
	// for example: 0x1234abcd "xyz"
	// support function for ExprInfoIs()
	BOOL ValueIsAddress(CString str);

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
	void VerifyECSuccess();

	BOOL CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc );
	void RemoveTestFiles( CString szProjLoc );

	BOOL ModifyCodeLine(LPCSTR szFind, LPCSTR szReplace);
	BOOL ModifyCode(LPCSTR szFind, LPCSTR szReplace, int nLines = 1);
	BOOL IsMsgBox(CString strMsg, CString strBtn);
};


class CStartEndTest
{

public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CStartEndTest(LPCSTR testName, CECTestSet *test, bool timing = FALSE);
	~CStartEndTest();

protected: // data
	CString	m_testName;
	bool m_timing;
	CTime m_startTime;
	CTime m_endTime;
	CECTestSet *m_Test;
};


extern char szecEditTypes[6][256];


#endif	// __ECSET_H__
