///////////////////////////////////////////////////////////////////////////////
//  M2CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix2Cases class
//

#ifndef __M2CASES_H__
#define __M2CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix2Cases class

class CMix2Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix2Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M2CASES_H__
