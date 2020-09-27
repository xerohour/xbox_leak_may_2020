///////////////////////////////////////////////////////////////////////////////
//	TypeInfoCases.H
//
//	Description :
//		Declaration of the CTypeInfoCases class
//

#ifndef __TypeInfoCases_H__
#define __TypeInfoCases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "AutoCompleteSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CTypeInfoCases class

class CTypeInfoCases : public CTest
{
	DECLARE_TEST(CTypeInfoCases, CAutoCompleteSuite)

// Operations
public:
	virtual void Run(void);
	BOOL EditSource();

// Member variables
public:
	COSource  src;
};

#endif //__TypeInfoCases_H__
