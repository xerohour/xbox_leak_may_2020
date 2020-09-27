///////////////////////////////////////////////////////////////////////////////
//  GTCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CGreaterCases class
//

#ifndef __GTCASES_H__
#define __GTCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CGreaterCases class

class CGreaterCases : public CDebugTestSet

    {
	DECLARE_TEST(CGreaterCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__GTCASES_H__
