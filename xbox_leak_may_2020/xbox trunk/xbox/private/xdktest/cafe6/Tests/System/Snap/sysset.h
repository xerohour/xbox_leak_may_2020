///////////////////////////////////////////////////////////////////////////////
//	SYSSET.H
//
//	Created by :			Date :
//		YefimS					5/19/97
//
//	Description :
//		Declaration of base class containing common data and methods for all
//		System Test Sets.

#ifndef __SYSSET_H__
#define __SYSSET_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

extern int ReturnCode;

#define EXPECT_TRUE(f)    ((f)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f,ReturnCode));
#define EXPECT_TRUE_COMMENT(f,comment) ((f)?LogResult(__LINE__, PASSED,#f,0,#comment):LogResult(__LINE__, FAILED,#f,0,#comment));
#define RestoreFocusToSrcWnd() MST.DoKeys("{ESC}")

// Global variables
#define WSPEXTENSION	".DSW"
#define PRJEXTENSION	".DSP"
#define NEWEXEINTDIR	"EXEInt"
#define NEWEXEOUTDIR	"EXEOut"
#define BUILDRES		"buildres.out"
#define PLATFORMS		PLATFORM_WIN32_X86 + PLATFORM_MAC_68K + PLATFORM_MAC_PPC

#if defined(_MIPS_)
#define TOOLSDIR "tools\\mips"
#elif defined(_ALPHA_)
#define TOOLSDIR "tools\\alpha"
#else
#define TOOLSDIR "tools\\x86"
#endif

extern CString cstrRelease ;// =  GetLocString(IDSS_BUILD_RELEASE);
extern CString cstrDebug ;// =  GetLocString(IDSS_BUILD_DEBUG);
extern CString RELEASETARGET; // = " - Win32 " + cstrRelease;
extern CString DEBUGTARGET; // = " - Win32 " + cstrDebug;

///////////////////////////////////////////////////////////////////////////////
//	class CSystemTestSet used as base for project tests

class CSystemTestSet : public CTest
{
//	DECLARE_TEST(CSystemTestSet, CIDESubSuite /*CCORESubSuite*/)
	DECLARE_DYNAMIC(CSystemTestSet)
public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CSystemTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

protected: // data
	CString	m_strProjName;
	CString	m_szErrorText;
	int m_platform;
	CString m_szBuildToolsLoc;
	CString m_strProjLoc;	

	// locale-independent names of projects and other items used by the sys snap test.
	CString m_strWorkspaceName;
	CString m_strWorkspaceLoc;
	CString m_strAppProjName;
	CString m_strAppClassName;
	CString m_strMenuName;
	CString m_strMenuItemName;
	CString m_strAppResourceID;
	CString m_strHTMLPageName;
	CString m_strActiveXControlProjName;
	CString m_strActiveXControlName;
	CString m_strActiveXControlViewClass;
	CString m_strAppsRoot;
	CString m_strHTMLPageRelPath;
	CString m_strATLCOMServerProjName;
	CString m_strATLCOMServerInterface;
	CString m_strATLCOMServerClsID;
	CString m_strATLCOMServerIID;

	COSource src;
	COProject prj;
 	COWorkSpace ws;
	CODebug dbg;
	COBrowse browse;
	COFile file;
	COResScript res;
	COHTML html;
	UIClassWizard uiclswiz;

	COExpEval cxx;
	COBreakpoints bp;
	COStack stk;
	UIMemory uimem;
	COMemory mem;
	UIVariables uivar;
	UIWatch uiwatch;
	UIQuickWatch uiqw;

//	typedef enum {PROJECT_EXE, PROJECT_DLL} PROJECT_TYPE;
//?	typedef enum {fDefault, fBuildOnly} PROJECT_FLAGS;

	void LogResult(int line, int result,LPCSTR szoperation, int code=0, LPCSTR szComment="");
	LPCSTR HomeDir(void);
	LPCSTR FullPath(LPCSTR szAddPath);
	void LogTestHeader(LPCSTR test_desc_str);
	void LogSubTestHeader(LPCSTR test_desc_str);

	void RemoveTestFiles( CString szProjLoc );
	BOOL TouchFile( CString szFileName );
	time_t GetFileTime( CString szFileName );
	BOOL SetToolsDir( CString szToolsLoc );
	BOOL RemoveToolsDir( CString szToolsLoc );
	CString GetBldLog();
	BOOL VerifyBuildOccured( BOOL iBuild );
	int VerifyBuildString( LPCSTR verifyString, BOOL fExist = TRUE );
	BOOL CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc );
	BOOL UpdateBuildVerify( LPCSTR szCurrentFile, LPCSTR szNewFile, LPCSTR szFileString );

	// shorthand method of filling EXPR_INFO object.
	void FillExprInfo(EXPR_INFO &expr_info, int state, LPCSTR type, LPCSTR name, LPCSTR value);

	// compares two EXPR_INFO objects
	BOOL ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
					int total_rows = 1);
	
	// tells whether string represents an address as displayed in an ee window.
	// for example: 0x1234abcd "xyz"
	// support function for ExprInfoIs()
	BOOL ValueIsAddress(CString str);

	void VerifyLocalsWndString(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber = 1);
	BOOL LocalsWndStringIs(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber = 1);


};

#endif //__SYSSET_H__
