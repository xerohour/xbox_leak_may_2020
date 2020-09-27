///////////////////////////////////////////////////////////////////////////////
//	PRJCASES.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//

#ifndef __PRJCASES_H__
#define __PRJCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

// Global variables
extern CString cstrRelease ;// =  GetLocString(IDSS_BUILD_RELEASE);
extern CString EXERELEASETARGET; //= "Hello - Win32 " + cstrRelease;
extern CString DLLRELEASETARGET; //= "DLL - Win32 " + cstrRelease;





///////////////////////////////////////////////////////////////////////////////
//	CPrjAppTest class

class CPrjAppTest : public CTest
{
	DECLARE_TEST(CPrjAppTest, CSniffDriver)

// Operations
public:
	virtual BOOL RunAsDependent(void);
	virtual void PreRun(void);
	virtual void Run(void);
	BOOL CloseProject( void );

public:
	CString m_strCurDir;
	CString szBuildToolsLoc;
	CString szProjEXELoc;	
 		
// Test Cases
protected:
	COProject prj;

	BOOL CreateNewProject( void );
	BOOL AddFilesToEXEProject( void );
	BOOL SaveProject( void );

};

///////////////////////////////////////////////////////////////////////////////
//	CPrjUtil class
class CPrjUtil
{
//		DECLARE_TEST(CPrjUtil, CSniffDriver)
public:

	void RemoveTestFiles( CString szProjLoc );
	BOOL TouchFile( CString szFileName );
	time_t GetFileTime( CString szFileName );
	BOOL SetToolsDir( CString szToolsLoc );
	BOOL RemoveToolsDir( CString szToolsLoc );
	CString GetBldLog();
};

#endif //__PRJCASES_H__
