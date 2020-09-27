///////////////////////////////////////////////////////////////////////////////
//	SRCCASES.H
//
//	Created by :			Date :
//		DavidGa					4/11/94
//
//	Description :
//		Declaration of the CSrcTestCases class
//

#ifndef __SRCCASES_H__
#define __SRCCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	CSourceTestCases class

class CSourceTestCases : public CTest
{
	DECLARE_TEST(CSourceTestCases, CSysSubSuite)

// Data
public:
	COSource m_src;

// Operations
public:
	virtual void Run(void);

// Test Cases
public:
	void Typing(void);
	void Cursor(void);
};

///////////////////////////////////////////////////////////////////////////////
//	CSearchTestCases class

class CSearchTestCases : public CTest
{
	DECLARE_TEST(CSearchTestCases, CSysSubSuite)

// Operations
public:
	virtual void Run(void);

// Test Cases
public:
	void Text(void);
	void Expression(void);
	void InFiles(void);
};

#endif //__SRCCASES_H__
