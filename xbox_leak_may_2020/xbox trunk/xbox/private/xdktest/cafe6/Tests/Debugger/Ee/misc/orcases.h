///////////////////////////////////////////////////////////////////////////////
//  ORCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the COrCases class
//

#ifndef __ORCASES_H__
#define __ORCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  COrCases class

class COrCases : public CDebugTestSet

    {
	DECLARE_TEST(COrCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__ORCASES_H__
