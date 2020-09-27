///////////////////////////////////////////////////////////////////////////////
//  LSHCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CLShiftCases class
//

#ifndef __LSHCASES_H__
#define __LSHCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CLShiftCases class

class CLShiftCases : public CDebugTestSet

    {
	DECLARE_TEST(CLShiftCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__LSHCASES_H__
