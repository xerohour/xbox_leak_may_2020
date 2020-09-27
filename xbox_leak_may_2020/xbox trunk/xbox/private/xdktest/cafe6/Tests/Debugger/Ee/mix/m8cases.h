///////////////////////////////////////////////////////////////////////////////
//  M8CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix8Cases class
//

#ifndef __M8CASES_H__
#define __M8CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix8Cases class

class CMix8Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix8Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M8CASES_H__
