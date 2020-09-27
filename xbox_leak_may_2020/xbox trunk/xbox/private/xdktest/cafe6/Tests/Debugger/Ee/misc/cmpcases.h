///////////////////////////////////////////////////////////////////////////////
//  CMPCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CComplCases class
//

#ifndef __CMPCASES_H__
#define __CMPCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CComplCases class

class CComplCases : public CDebugTestSet

    {
	DECLARE_TEST(CComplCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__CMPCASES_H__
