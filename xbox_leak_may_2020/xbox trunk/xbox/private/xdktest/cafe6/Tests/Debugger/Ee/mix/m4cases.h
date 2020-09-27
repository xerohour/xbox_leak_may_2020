///////////////////////////////////////////////////////////////////////////////
//  M4CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix4Cases class
//

#ifndef __M4CASES_H__
#define __M4CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix4Cases class

class CMix4Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix4Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M4CASES_H__
