///////////////////////////////////////////////////////////////////////////////
//	batch1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Cbatch1Test class

#ifndef __batch1_H__
#define __batch1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "batchsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBatchBuildTest1 class

class CBatchBuildTest1 : public CProjectTestSet
{
	DECLARE_TEST(CBatchBuildTest1, CBatchSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL Test1();

// Utilities

};

#endif //__batch1_H__
