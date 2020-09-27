///////////////////////////////////////////////////////////////////////////////
//  RSECASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CRShiftEqCases class
//

#ifndef __RSECASES_H__
#define __RSECASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CRShiftEqCases class

class CRShiftEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CRShiftEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__RSECASES_H__
