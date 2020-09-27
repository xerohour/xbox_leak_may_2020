///////////////////////////////////////////////////////////////////////////////
//  M13CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix13Cases class
//

#ifndef __M13CASES_H__
#define __M13CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix13Cases class

class CMix13Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix13Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M13CASES_H__
