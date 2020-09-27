///////////////////////////////////////////////////////////////////////////////
//    BRZCASES.H
//
//    Created by :                    Date :
//            DougT                                   10/13/93
//
//    Description :
//            Declaration of the CBrzTestCases class
//
 
#ifndef __BRZCASES_H__
#define __BRZCASES_H__

#ifndef __STDAFX_H__
      #error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CBrzTestCases class

class CBrzTestCases : public CTest
{
	DECLARE_TEST(CBrzTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
      BOOL BrzEnumRes(void);
	  BOOL BrzCreateRCT(void);
};

#endif //__BRZCASES_H__
