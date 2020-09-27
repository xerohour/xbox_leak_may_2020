///////////////////////////////////////////////////////////////////////////////
//  DIVCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CDivCases class
//

#ifndef __DIVCASES_H__
#define __DIVCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CDivCases class

class CDivCases : public CDebugTestSet

    {
	DECLARE_TEST(CDivCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__DIVCASES_H__
