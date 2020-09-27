///////////////////////////////////////////////////////////////////////////////
//    CPICASES.H
//
//    Created by :                    Date :
//            MikePie                 2/28/95
//
//    Description :
//            Declaration of the CCpiTestCases class
//
 
#ifndef __CPICASES_H__
#define __CPICASES_H__

#ifndef __STDAFX_H__
      #error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CCpiTestCases class

class CCpiTestCases : public CTest
{
	DECLARE_TEST(CCpiTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
	BOOL CpiNewRC(void);
};

#endif //__CPICASES_H__
