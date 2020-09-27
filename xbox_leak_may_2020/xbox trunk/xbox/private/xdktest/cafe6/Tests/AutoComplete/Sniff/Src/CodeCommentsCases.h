///////////////////////////////////////////////////////////////////////////////
//	CodeCommentsCases.H
//
//	Description :
//		Declaration of the CCodeCommentsCases class
//

#ifndef __CodeCommentsCases_H__
#define __CodeCommentsCases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "AutoCompleteSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CCodeCommentsCases class

class CCodeCommentsCases : public CTest
{
	DECLARE_TEST(CCodeCommentsCases, CAutoCompleteSuite)

// Operations
public:
	virtual void Run(void);
	BOOL EditSource();

// Member variables
public:
	COSource  src;
};

#endif //__CodeCommentsCases_H__
