///////////////////////////////////////////////////////////////////////////////
//	bldwin32.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the CBuildWin32Test class

#ifndef __bldwin32_H__
#define __bldwin32_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "buildsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildWin32Test class

class CBuildWin32Test : public CProjectTestSet
{
	DECLARE_TEST(CBuildWin32Test, CBuildSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:
//	BOOL RunAsDependent(void);
//	BOOL CloseProject( void );

// Test Cases
protected:
	BOOL CreateAndBuildWin32App();
	BOOL CreateAndBuildConsoleApp();
	BOOL CreateAndBuildWin32Dll();
	BOOL CreateAndBuildWin32StaticLib();

// Utilities
//	BOOL UpdateBuildVerify( LPCSTR szCurrentFile, LPCSTR szNewFile, LPCSTR szFileString );
/*	BOOL CreateNewProject( void );
	BOOL AddFilesToEXEProject( void );
	BOOL SaveProject( void );
*/
};

#endif //__bldwin32_H__
