///////////////////////////////////////////////////////////////////////////////
//	FILEcase.h
//
//	Created by :			
//		WinslowF
//
//	Description :
//		Declaration of the CFILENAMEIDETest Class
//

#ifndef __FILEcase_H__
#define __FILEcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "filesub.h"

//
//	Macros
//
#define VERIFY_TEST_SUCCESS(TestCase)\
	((TestCase) == ERROR_SUCCESS)

#define VERIFY_BP_SUCCESS(BpCase)\
	((BpCase) != NULL)

// ComboBox index for Tools.Options.Debug Memory window format
#define MEM_WIN_ASCII	1
///////////////////////////////////////////////////////////////////////////////
//	CFILENAMEIDETest class

class CFILENAMEIDETest : public CTest
{
	DECLARE_TEST(CFILENAMEIDETest, CFILENAMESubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Test Cases
protected:
	void CopyFile(CString szFrom, CString szTo);
	void SetUp(void);
// Data
protected:
	/*
		Project/General
	*/
	CString		m_strCurDir;
	CString		m_szSourceDir;
	CString		m_szTestDir;
	CString		m_szProject;
	CString		m_szProjectFiles;
	CString		m_szNewIntermediateDir;
	CString		m_szNewOutputDir;
	CString		m_szErrorText;
	CString		m_szComment;
 	PROJOPTIONS ProjOptions;
  	COProject	proj;
	/*
		Breakpoint
	*/
	CString			m_szBPLocation;
	bp				*bpString;
	COBreakpoints	bp;

	/*
		Disassembly
	*/
	CString			m_szDisasm;

	/*
		Quick Watch
	*/
	CString			m_szQWExpr1;
	CString			m_szQWExpr2;
	
	/*
		Memory
	*/
	CString			m_szMemBuf;
	CString			m_szMem1;
	CString			m_szMem2;

	CODebug		dbg;
	COExpEval	qw;
	
// Test Cases
protected:
	BOOL CreateFileNameList(CStringList *szFileList);
	BOOL CreateBuildProject( void );
	BOOL CreateProject( void );    // not really a dbg test
	BOOL SetBreakpoints( void );
	BOOL ReadDBCSource( void );
	BOOL ReadDBCDisasm( void );
	BOOL QuickWatchTest( void );
	BOOL MemoryTest( void );
	BOOL ENDThisTest( void );
	void CopySourceFiles();

// Test Cases
protected:

};


#endif //__FILEcase_H__
