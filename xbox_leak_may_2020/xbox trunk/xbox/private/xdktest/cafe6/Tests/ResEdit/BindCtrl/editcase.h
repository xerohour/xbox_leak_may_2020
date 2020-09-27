///////////////////////////////////////////////////////////////////////////////
//	EDITCASE.H
//
//	Created by :			Date :
//		ChrisKoz					2/02/96
//
//	Description :
//		Declaration of classes for DataPane Verification
//

#ifndef __EDITCASE_H__
#define __EDITCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"



///////////////////////////////////////////////////////////////////////////////
// CEditResTest: Drag & drop the controls within the resources

class CEditResTest: public CTest
{
	DECLARE_TEST(CEditResTest, CBindSubSuite)

public:
	virtual void Run(void);
};



///////////////////////////////////////////////////////////////////////////////
// CRandomTest: Put all controls onto the dialog randomly
class CRandomTest: public CTest
{
	DECLARE_TEST(CRandomTest, CBindSubSuite)

public:
	virtual void Run(void);
};




#endif //__EDITCASE_H__
