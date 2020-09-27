///////////////////////////////////////////////////////////////////////////////
//  I12CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt12Cases class
//

#ifndef __I12CASES_H__
#define __I12CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt12Cases class

class CInt12Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt12Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I12CASES_H__
