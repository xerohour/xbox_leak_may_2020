///////////////////////////////////////////////////////////////////////////////
//	stacsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CstackSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CstackSubSuite class

class CstackSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CstackSubSuite)
//	CstackSubSuite() { m_strName = "TestWizard Generated script"; m_strOwner = "VCBU QA"; }


	DECLARE_TESTLIST()
public:
	void CleanUp(void);
};

#endif //__SNIFF_H__
