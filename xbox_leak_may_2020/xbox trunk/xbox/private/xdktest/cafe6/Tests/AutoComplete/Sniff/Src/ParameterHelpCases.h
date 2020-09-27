///////////////////////////////////////////////////////////////////////////////
//	ParameterHelpCases.H
//
//	Description :
//		Declaration of the CParameterHelpCases class
//

#ifndef __ParameterHelpCases_H__
#define __ParameterHelpCases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "AutoCompleteSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CParameterHelpCases class

class CParameterHelpCases : public CTest
{
	DECLARE_TEST(CParameterHelpCases, CAutoCompleteSuite)

// Operations
public:
	virtual void Run(void);
	BOOL EditSource();

// Member variables
public:
	COSource  src;
};

#endif //__ParameterHelpCases_H__
