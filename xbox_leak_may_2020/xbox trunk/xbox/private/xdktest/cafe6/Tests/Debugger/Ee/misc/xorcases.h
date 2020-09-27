///////////////////////////////////////////////////////////////////////////////
//  XORCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CXOrCases class
//

#ifndef __XORCASES_H__
#define __XORCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CXOrCases class

class CXOrCases : public CDebugTestSet

    {
	DECLARE_TEST(CXOrCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__XORCASES_H__
