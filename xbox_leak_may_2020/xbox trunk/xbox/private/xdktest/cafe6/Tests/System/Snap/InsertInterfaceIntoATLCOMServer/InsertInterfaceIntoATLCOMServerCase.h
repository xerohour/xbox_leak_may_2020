///////////////////////////////////////////////////////////////////////////////
//	InsertInterfaceIntoATLCOMServerCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CInsertInterfaceIntoATLCOMServerCase class

#ifndef __InsertInterfaceIntoATLCOMServerCase_H__
#define __InsertInterfaceIntoATLCOMServerCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "InsertInterfaceIntoATLCOMServerSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CInsertInterfaceIntoATLCOMServerCase class

class CInsertInterfaceIntoATLCOMServerCase : public CSystemTestSet
{
	DECLARE_TEST(CInsertInterfaceIntoATLCOMServerCase, CInsertInterfaceIntoATLCOMServerSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__InsertInterfaceIntoATLCOMServerCase_H__
