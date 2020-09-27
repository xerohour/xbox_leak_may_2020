///////////////////////////////////////////////////////////////////////////////
//	ComponentsSuite.H
//
//	Description :
//		Declaration of the CComponentsSuite class
//

#ifndef __ComponentsSuite_H__
#define __ComponentsSuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CComponentsSuite class

class CComponentsSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CComponentsSuite)

	DECLARE_TESTLIST();

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);
};

#endif //__ComponentsSuite_H__
