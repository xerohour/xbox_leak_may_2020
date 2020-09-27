///////////////////////////////////////////////////////////////////////////////
//  GECASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CGreaterEqCases class
//

#ifndef __GECASES_H__
#define __GECASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CGreaterEqCases class

class CGreaterEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CGreaterEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__GECASES_H__
