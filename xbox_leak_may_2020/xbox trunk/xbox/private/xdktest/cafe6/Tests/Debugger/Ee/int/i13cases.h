///////////////////////////////////////////////////////////////////////////////
//  I13CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt13Cases class
//

#ifndef __I13CASES_H__
#define __I13CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt13Cases class

class CInt13Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt13Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I13CASES_H__
