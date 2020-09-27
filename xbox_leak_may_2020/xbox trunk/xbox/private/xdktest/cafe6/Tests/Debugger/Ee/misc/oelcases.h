///////////////////////////////////////////////////////////////////////////////
//  OELCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the COrElseCases class
//

#ifndef __OELCASES_H__
#define __OELCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  COrElseCases class

class COrElseCases : public CDebugTestSet

    {
	DECLARE_TEST(COrElseCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__OELCASES_H__
