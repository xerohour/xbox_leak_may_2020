///////////////////////////////////////////////////////////////////////////////
//  PLUCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CPlusCases class
//

#ifndef __PLUCASES_H__
#define __PLUCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CPlusCases class

class CPlusCases : public CDebugTestSet

    {
	DECLARE_TEST(CPlusCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__PLUCASES_H__
