///////////////////////////////////////////////////////////////////////////////
//	ParamHlpCases.h
//
//	Description :
//		Declaration of the CParamHlpCases class
//

#ifndef __ParamHlpCases_H__
#define __ParamHlpCases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ParamHlpSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CParamHlpCases class

class CParamHlpCases : public CTest
{
	DECLARE_TEST(CParamHlpCases, CParamHlpSuite)

// Operations
public:
	virtual void Run(void);
	void EditSource();

// Member variables
public:
	COSource  src;
};

#endif //__ParamHlpCases_H__
