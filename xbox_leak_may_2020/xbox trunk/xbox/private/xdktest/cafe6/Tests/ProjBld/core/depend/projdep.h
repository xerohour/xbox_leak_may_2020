///////////////////////////////////////////////////////////////////////////////
//	Gprojdep.h
//
//	Created by :			Date :
//		t-seanc					8/12/97
//
//	Description :
//		Declaration of the CProjectDependencies class.

#ifndef __PROJDEP_H__
#define __PROJDEP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "depsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CProjectDependencies class

class CProjectDependencies : public CProjectTestSet
{
	DECLARE_TEST(CProjectDependencies, CDependenciesSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL ProjectsDependantOnFiles();
	BOOL ProjectsDependantOnProjects();
	BOOL ProjectsDependantOnGenProjects();

// Utilities
};

#endif // __PROJDEP_H__
