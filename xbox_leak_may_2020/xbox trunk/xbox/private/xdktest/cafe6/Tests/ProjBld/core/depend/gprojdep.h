///////////////////////////////////////////////////////////////////////////////
//	Gprojdep.h
//
//	Created by :			Date :
//		t-seanc					8/12/97
//
//	Description :
//		Declaration of the CGenProjectDependencies class.

#ifndef __GENPROJDEP_H__
#define __GENPROJDEP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "depsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CGenProjectDependencies class

class CGenProjectDependencies : public CProjectTestSet
{
	DECLARE_TEST(CGenProjectDependencies, CDependenciesSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL GenProjectsDependantOnProjects(); 
	BOOL GenProjectsDependantOnGenProjects();

// Utilities
};

#endif // __PROJDEP_H__
