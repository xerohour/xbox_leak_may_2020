///////////////////////////////////////////////////////////////////////////////
//	PRJSET.H
//
//	Created by :			Date :
//		YefimS					4/28/97
//
//	Description :
//		Declaration of base class containing common data and methods for all
//		Project Test Sets.

#ifndef __PRJSET_H__
#define __PRJSET_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

extern int ReturnCode;

#define EXPECT_TRUE(f)    ((f)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f,ReturnCode));
#define EXPECT_TRUE_COMMENT(f,comment) ((f)?LogResult(__LINE__, PASSED,#f,0,#comment):LogResult(__LINE__, FAILED,#f,0,#comment));

// Global variables
#define WSPEXTENSION	".DSW"
#define PRJEXTENSION	".DSP"
#define NEWEXEINTDIR	"EXEInt"
#define NEWEXEOUTDIR	"EXEOut"
#define BUILDRES		"buildres.out"
#define PLATFORMS		PLATFORM_WIN32_X86 + PLATFORM_MAC_68K + PLATFORM_MAC_PPC
//#define V4PROJDIR		"Hello.V4"

#if defined(_MIPS_)
#define TOOLSDIR "tools\\mips"
#elif defined(_ALPHA_)
#define TOOLSDIR "tools\\alpha"
#else
#define TOOLSDIR "tools\\x86"
#endif

// extern CString cstrRelease ;// =  GetLocString(IDSS_BUILD_RELEASE);
// extern CString cstrDebug ;// =  GetLocString(IDSS_BUILD_DEBUG);
extern CString RELEASETARGET; // = " - Win32 " + cstrRelease;
extern CString DEBUGTARGET; // = " - Win32 " + cstrDebug;

///////////////////////////////////////////////////////////////////////////////
//	class CProjectTestSet used as base for project tests

class CProjectTestSet : public CTest
{
//	DECLARE_TEST(CProjectTestSet, CIDESubSuite /*CCORESubSuite*/)
		DECLARE_DYNAMIC(CProjectTestSet)
public:	
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	CProjectTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

protected: // data
//	CString	m_strProjectDir;
	CString	m_strProjName;
	CString	m_szErrorText;
	int m_platform;
//	CString m_strCurDir;
	CString m_szBuildToolsLoc;
//	CString m_szProjEXELoc;	
	CString m_strProjLoc;	

	COSource src;
	COProject prj;
 	COWorkSpace ws;
	CODebug dbg;
	COBrowse browse;
	COFile file;

//	typedef enum {PROJECT_EXE, PROJECT_DLL} PROJECT_TYPE;
//?	typedef enum {fDefault, fBuildOnly} PROJECT_FLAGS;

	void LogResult(int line, int result,LPCSTR szoperation, int code=0, LPCSTR szComment="");
	LPCSTR HomeDir(void);
	LPCSTR FullPath(LPCSTR szAddPath);
	void LogTestHeader(LPCSTR test_desc_str);
	void LogSubTestHeader(LPCSTR test_desc_str);

	void RemoveTestFiles( CString szProjLoc );
	BOOL TouchFile( CString szFileName );

	BOOL TouchAndVerifyRebuild(
		CString szFileToTouch,
		BOOL	bShouldRebuild = TRUE
		);

	time_t GetFileTime( CString szFileName );
	BOOL SetToolsDir( CString szToolsLoc );
	BOOL RemoveToolsDir( CString szToolsLoc );
	CString GetBldLog();
	BOOL VerifyBuildOccured( BOOL iBuild );
	int VerifyBuildString( LPCSTR verifyString, BOOL fExist = TRUE );
	BOOL CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc );
	BOOL UpdateBuildVerify( LPCSTR szCurrentFile, LPCSTR szNewFile, LPCSTR szFileString );

};

#endif //__PRJSET_H__
