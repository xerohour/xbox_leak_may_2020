///////////////////////////////////////////////////////////////////////////////
//  ASNCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CAssignCases class
//

#ifndef __ASNCASES_H__
#define __ASNCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CAssignCases class

class CAssignCases : public CDebugTestSet

    {
	DECLARE_TEST(CAssignCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__ASNCASES_H__
