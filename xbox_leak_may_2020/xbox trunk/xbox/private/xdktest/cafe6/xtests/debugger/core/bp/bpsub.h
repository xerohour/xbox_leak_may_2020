///////////////////////////////////////////////////////////////////////////////
//	bpsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CbpSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CbpSubSuite class

class CbpSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CbpSubSuite)
//	CbpSubSuite() { m_strName = "TestWizard Generated script"; m_strOwner = "VCBU QA"; }


	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__SNIFF_H__
