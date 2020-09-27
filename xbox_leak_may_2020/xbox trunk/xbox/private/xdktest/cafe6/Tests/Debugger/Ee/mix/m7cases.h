///////////////////////////////////////////////////////////////////////////////
//  M7CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix7Cases class
//

#ifndef __M7CASES_H__
#define __M7CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix7Cases class

class CMix7Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix7Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M7CASES_H__
