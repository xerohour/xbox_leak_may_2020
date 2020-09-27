///////////////////////////////////////////////////////////////////////////////
//  PDCCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CPostDecCases class
//

#ifndef __PDCCASES_H__
#define __PDCCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CPostDecCases class

class CPostDecCases : public CDebugTestSet

    {
	DECLARE_TEST(CPostDecCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__PDCCASES_H__
