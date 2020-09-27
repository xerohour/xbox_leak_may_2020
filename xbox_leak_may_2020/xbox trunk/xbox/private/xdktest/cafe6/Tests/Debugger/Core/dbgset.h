//
// dbgset.h
//
//	Created by		Date
//		TomSe		9/2/94
//
//	Description
//		Definition of base class containing common data and methods for all
//		Debug Test Sets.


#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __DBGSET_H__
#define __DBGSET_H__

//#include "coresub.h"

extern int ReturnCode;
extern bp  * ReturnBP;

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


///////////////////////////////////////////////////////////////////////////////
//	class CDebugTestSet used as base for debugging tests

class CDebugTestSet : public CTest
{
//	DECLARE_TEST(CDebugTestSet, CIDESubSuite /*CCORESubSuite*/)
	DECLARE_DYNAMIC(CDebugTestSet)
public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CDebugTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

protected: // data
	CString	m_strProjectDir;
	CString	m_strProjectName;
	CString	m_szErrorText;
	int m_platform;

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
	CODAM dam;
	COThreads threads;
	COLocals locals;
	CORegisters regs;

	typedef enum {PROJECT_EXE, PROJECT_DLL} PROJECT_TYPE;
	typedef enum {fDefault, fBuildOnly} PROJECT_FLAGS;

	BOOL SetProject(LPCSTR projName, PROJECT_TYPE projType = PROJECT_EXE,
					PROJECT_FLAGS = fDefault,LPCSTR cfgName=NULL);

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

	void LogResult(int line, int result,LPCSTR szoperation, int code=0, LPCSTR szComment="");
	LPCSTR HomeDir(void);
	LPCSTR FullPath(LPCSTR szAddPath);
	void LogTestHeader(LPCSTR test_desc_str);

	void VerifyLocalsWndString(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber = 1);
	BOOL LocalsWndStringIs(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber = 1);

};

#endif	// __DBGSET_H__
