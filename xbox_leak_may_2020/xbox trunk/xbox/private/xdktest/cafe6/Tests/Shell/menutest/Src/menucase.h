///////////////////////////////////////////////////////////////////////////////
//	Menucase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CMenuTesterIDETest Class
//

#ifndef __MENCASES_H__
#define __MENCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "Menusub.h"

///////////////////////////////////////////////////////////////////////////////
//	CMenuTesterIDETest class

class CMenuTesterIDETest : public CTest
{
    DECLARE_TEST(CMenuTesterIDETest, CMenuTesterSubSuite)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
  	void TestCase1( );

};

#endif //__MENCASES_H__
