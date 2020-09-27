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

#include "..\prjset.h"
#include "prj1sub.h"
/*
// Global variables
extern CString cstrRelease ;// =  GetLocString(IDSS_BUILD_RELEASE);
extern CString EXERELEASETARGET; //= "Hello - Win32 " + cstrRelease;
extern CString DLLRELEASETARGET; //= "DLL - Win32 " + cstrRelease;
*/

///////////////////////////////////////////////////////////////////////////////
//	CPrjAppTest class

class CPrjAppTest : public CProjectTestSet
{
	DECLARE_TEST(CPrjAppTest, CProj1SubSuite)

// Operations
public:
//	virtual BOOL RunAsDependent(void);
	virtual void PreRun(void);
	virtual void Run(void);

public:
/*	CString m_strCurDir;
	CString szBuildToolsLoc;
	CString szProjEXELoc;	
*/ 		
	BOOL RunAsDependent(void);
	BOOL CloseProject( void );
// Test Cases
protected:
	BOOL CreateNewProject( void );
	BOOL AddFilesToEXEProject( void );
	BOOL SaveProject( void );

};

#endif //__PRJCASES_H__
