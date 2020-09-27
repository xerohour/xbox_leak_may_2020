///////////////////////////////////////////////////////////////////////////////
//	AutoCompleteCases.H
//
//	Description :
//		Declaration of the CAutoCompleteCases class
//

#ifndef __AutoCompleteCases_H__
#define __AutoCompleteCases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "AutoCompleteSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CAutoCompleteCases class

class CAutoCompleteCases : public CTest
{
	DECLARE_TEST(CAutoCompleteCases, CAutoCompleteSuite)

// Operations
public:
	virtual void Run(void);
	BOOL EditSource();

// Member variables
public:
	COSource  src;
};

#endif //__AutoCompleteCases_H__
