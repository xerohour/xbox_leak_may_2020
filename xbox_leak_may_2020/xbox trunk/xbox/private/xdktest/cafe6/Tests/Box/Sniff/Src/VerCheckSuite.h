///////////////////////////////////////////////////////////////////////////////
//	VerCheckSuite.h
//
//	Created by :			
//		VSUpdate QA
//		DougMan
//
//	Description :
//		Declaration of the CVerCheckSuite class

#ifndef __VERCHECKSUITE_H__
#define __VERCHECKSUITE_H__
#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CVerCheckSuite class

class CVerCheckSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CVerCheckSuite)

	DECLARE_TESTLIST(); 

public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);
};
#endif //__VERCHECKSUITE_H__ 