///////////////////////////////////////////////////////////////////////////////
//	AddnBuildCases.H
//
//	Description :
//		Declaration of the CAddnBuildCases class
//

#ifndef __AddnBuildCases_H__
#define __AddnBuildCases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ComponentsSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CAddnBuildCases class

class CAddnBuildCases : public CTest

	{
	DECLARE_TEST(CAddnBuildCases, CComponentsSuite)
	
	// Operations
	public:
		virtual void Run(void);
	};

#endif //__AddnBuildCases_H__
