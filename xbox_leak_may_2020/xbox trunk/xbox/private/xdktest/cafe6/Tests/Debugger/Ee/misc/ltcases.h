///////////////////////////////////////////////////////////////////////////////
//  LTCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CLessThanCases class
//

#ifndef __LTCASES_H__
#define __LTCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CLessThanCases class

class CLessThanCases : public CDebugTestSet

    {
	DECLARE_TEST(CLessThanCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__LTCASES_H__
