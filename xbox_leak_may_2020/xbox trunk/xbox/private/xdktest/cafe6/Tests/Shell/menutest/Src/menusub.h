///////////////////////////////////////////////////////////////////////////////
//	Menusub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CMenuTesterSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__
			 
#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CMenuTesterSubSuite class

class CMenuTesterSubSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CMenuTesterSubSuite)

	DECLARE_TESTLIST();

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	BOOL brunmenus;
};

#endif //__SNIFF_H__
