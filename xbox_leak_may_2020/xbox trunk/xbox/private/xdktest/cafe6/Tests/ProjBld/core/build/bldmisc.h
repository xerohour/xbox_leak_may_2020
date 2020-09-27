///////////////////////////////////////////////////////////////////////////////
//	bldwin32.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the CBuildWin32Test class

#ifndef __bldmisc_H__
#define __bldmisc_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "buildsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildMiscTest class

class CBuildMiscTest : public CProjectTestSet
{
	DECLARE_TEST(CBuildMiscTest, CBuildSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:
//	BOOL RunAsDependent(void);
//	BOOL CloseProject( void );

// Test Cases
protected:
	BOOL CreateAndBuildMakefile();
	BOOL BuildISAPIExtensionWizard();

// Utilities

};

#endif //__bldwin32_H__
