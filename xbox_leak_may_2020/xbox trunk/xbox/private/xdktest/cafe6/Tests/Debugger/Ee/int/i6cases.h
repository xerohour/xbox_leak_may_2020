///////////////////////////////////////////////////////////////////////////////
//  I6CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt6Cases class
//

#ifndef __I6CASES_H__
#define __I6CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt6Cases class

class CInt6Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt6Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I6CASES_H__
