///////////////////////////////////////////////////////////////////////////////
//	GRDCASES.H
//
//	Created by :			Date :
//		MikePie					4/13/95
//
//	Description :
//		Declaration of the CGrdTestCases class
//

#ifndef __GRDCASES_H__
#define __GRDCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CGrdTestCases class

class CGrdTestCases : public CTest
{
	DECLARE_TEST(CGrdTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
	BOOL AddToGrid(void);
	BOOL ReorderGrid(void);
	BOOL ReopenGrid(void);
	BOOL CheckMBC(void);
	BOOL CloseGrid(void);
};

#endif //__GRDCASES_H__
