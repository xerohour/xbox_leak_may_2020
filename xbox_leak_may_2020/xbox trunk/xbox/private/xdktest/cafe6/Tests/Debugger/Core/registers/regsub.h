///////////////////////////////////////////////////////////////////////////////
//	Regsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CRegistersSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CRegistersSubSuite class

class CRegistersSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CRegistersSubSuite)
//	CRegistersSubSuite() { m_strName = "TestWizard Generated script"; m_strOwner = "VCBU QA"; }


	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
	void SetUp(BOOL bCleanUp);
};

#endif //__SNIFF_H__
