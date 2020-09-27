///////////////////////////////////////////////////////////////////////////////
//	RESCASES.H
//
//	Created by :			Date :
//		DavidGa					2/23/94
//
//	Description :
//		Declaration of the DBCS Resource testcase classes
//

#ifndef __RESCASES_H__
#define __RESCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	CStringTestCases class

class CStringTestCases : public CTest
{
	DECLARE_TEST(CStringTestCases, CSysSubSuite)

// Operations
public:
	virtual void Run(void);
};

///////////////////////////////////////////////////////////////////////////////
//	CAccelTestCases class

class CAccelTestCases : public CTest
{
	DECLARE_TEST(CAccelTestCases, CSysSubSuite)

// Operations
public:
	virtual void Run(void);
};

///////////////////////////////////////////////////////////////////////////////
//	CImageTestCases class

class CImageTestCases : public CTest
{
	DECLARE_TEST(CImageTestCases, CSysSubSuite)

// Operations
public:
	virtual void Run(void);
};

///////////////////////////////////////////////////////////////////////////////
//	CMenuTestCases class

class CMenuTestCases : public CTest
{
	DECLARE_TEST(CMenuTestCases, CSysSubSuite)

// Operations
public:
	virtual void Run(void);
};

#endif //__RESCASES_H__
