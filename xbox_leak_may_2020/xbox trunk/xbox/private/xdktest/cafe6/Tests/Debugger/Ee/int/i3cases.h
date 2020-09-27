///////////////////////////////////////////////////////////////////////////////
//  I3CASES.H
//
//  Created by :            Date :
//      MichMa              	12/10/93
//
//  Description :
//      Declaration of the CInt3Cases class
//

#ifndef __I3CASES_H__
#define __I3CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt3Cases class

class CInt3Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt3Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I3CASES_H__
