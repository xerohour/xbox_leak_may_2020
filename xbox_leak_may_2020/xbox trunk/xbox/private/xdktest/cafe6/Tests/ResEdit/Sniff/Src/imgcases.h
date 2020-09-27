///////////////////////////////////////////////////////////////////////////////
//	IMGCASES.H
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Declaration of the CImgTestCases class
//

#ifndef __IMGCASES_H__
#define __IMGCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CImgTestCases class

class CImgTestCases : public CTest
{
	DECLARE_TEST(CImgTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
	UIImgEdit	m_uBmpEd;	// REVIEW(davidga): if the utils needed for these
	UIImgEdit	m_uIcoEd;	// someday differ, derive UBmpEdit and UIcoEdit

// Test Cases
protected:
	BOOL TestBitmap(void);
	BOOL TestIcon(void);
	BOOL TestIconDevices(void);
	BOOL TestCursor(void);
	BOOL TestGIF_JPEG(void);
};

#endif //__IMGCASES_H__
