///////////////////////////////////////////////////////////////////////////////
//  LECASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CLessThanEqCases class
//

#ifndef __LECASES_H__
#define __LECASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CLessThanEqCases class

class CLessThanEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CLessThanEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__LECASES_H__
