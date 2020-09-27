///////////////////////////////////////////////////////////////////////////////
//  I8CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//						   
//  Description :
//      Declaration of the CInt8Cases class
//

#ifndef __I8CASES_H__
#define __I8CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt8Cases class

class CInt8Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt8Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I8CASES_H__
