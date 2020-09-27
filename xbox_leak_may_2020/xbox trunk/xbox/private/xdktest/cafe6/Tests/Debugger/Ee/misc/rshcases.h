///////////////////////////////////////////////////////////////////////////////
//  RSHCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CRShiftCases class
//

#ifndef __RSHCASES_H__
#define __RSHCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CRShiftCases class

class CRShiftCases : public CDebugTestSet

    {
	DECLARE_TEST(CRShiftCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__RSHCASES_H__
