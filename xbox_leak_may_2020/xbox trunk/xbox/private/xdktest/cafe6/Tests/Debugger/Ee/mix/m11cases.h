///////////////////////////////////////////////////////////////////////////////
//  M11CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix11Cases class
//

#ifndef __M11CASES_H__
#define __M11CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix11Cases class

class CMix11Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix11Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M11CASES_H__
