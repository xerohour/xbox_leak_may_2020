///////////////////////////////////////////////////////////////////////////////
//	Filedep.h
//
//	Created by :			Date :
//		t-seanc					8/12/97
//
//	Description :
//		Declaration of the CFileDependencies class.

#ifndef __FILEDEP_H__
#define __FILEDEP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "depsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CFileDependencies class

class CFileDependencies : public CProjectTestSet
{
	DECLARE_TEST(CFileDependencies, CDependenciesSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL FilesDependantOnHeaderFiles();

// Utilities
};

#endif // __FILEDEP_H__
