///////////////////////////////////////////////////////////////////////////////
//	FST.H
//
//	Created by :					Date :
//		DougMan							1/14/97
//
//	Description :
//		Declaration of a FST class
//		This test verifies that basic Full Text Search(FTS) works in IV5
//

#ifndef __FTS_H__
#define __FTS_H__

#include "sniff.h"
#include "ivutil.h"
#include "resource.h"

// Used by FTS
const int FTS_TC = 7;  // Used by FTS sniff test, it is the total number of testcase to be run

struct FTS_CASES {
	UINT	iSearchString;
	UINT	iVerificationString;
}; // End TestCases Structure


///////////////////////////////////////////////////////////////////////////////
//	BOOL CGeneric:: class

class CFTS : public CTest 
{
	DECLARE_TEST(CFTS, CSniffDriver)

public:
// Overrides
	virtual void Run(void);
// Operations
	
// Data

protected:
	
private:
int CFTS::VerifySearch(UINT iSearch, UINT iVerify);
void CFTS::InitalCases(FTS_CASES *ptTestCases);
};


#endif // __FTS_H__