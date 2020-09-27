///////////////////////////////////////////////////////////////////////////////
//	Dbg_case.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CDbg_mbcIDETest Class
//

#ifndef __Dbg_case_H__
#define __Dbg_case_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "dbg_sub.h"

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
//	CDbg_mbcIDETest class

class CDbg_mbcIDETest : public CTest
{
	DECLARE_TEST(CDbg_mbcIDETest, CDbg_mbcSubSuite)

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

// Test Cases
protected:

};

void CreateDirectoryLong(const char *path);

#endif //__Dbg_case_H__
