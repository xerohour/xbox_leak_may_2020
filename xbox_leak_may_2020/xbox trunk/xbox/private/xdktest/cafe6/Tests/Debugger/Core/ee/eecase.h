///////////////////////////////////////////////////////////////////////////////
//	EECASE.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CEEIDETest Class
//

#ifndef __EECASE_H__
#define __EECASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "eesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CEEIDETest class

class CEEIDETest : public CDbgTestBase
	
	{
	DECLARE_TEST(CEEIDETest, CEESubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__EECASE_H__
