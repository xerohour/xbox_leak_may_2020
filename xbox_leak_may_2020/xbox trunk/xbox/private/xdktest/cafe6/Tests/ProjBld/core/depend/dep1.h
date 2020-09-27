///////////////////////////////////////////////////////////////////////////////
//	dep1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Cdep1Test class

#ifndef __dep1_H__
#define __dep1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "depsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDependenciesTest1 class

class CDependenciesTest1 : public CProjectTestSet
{
	DECLARE_TEST(CDependenciesTest1, CDependenciesSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL BasicDependencyTest();
	BOOL ProjectDependencies();
	BOOL GenericProjectDependencies();

// Utilities
	void TouchAndBuild(
		CString szFileToTouch,
		BOOL	bExpectRebuild = TRUE
		);
	BOOL VerifyTextInBuildLog(
		CString szTextToFind
		);

};

#endif //__dep1_H__
