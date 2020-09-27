///////////////////////////////////////////////////////////////////////////////
//  M9CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix9Cases class
//

#ifndef __M9CASES_H__
#define __M9CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix9Cases class

class CMix9Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix9Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M9CASES_H__
