///////////////////////////////////////////////////////////////////////////////
//	BldXQuakesub.h
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDisAsmSubSuite class

class CBldXQuakeSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CBldXQuakeSubSuite)
//	CDisAsmSubSuite() { m_strName = "TestWizard Generated script"; m_strOwner = "VCBU QA"; }


	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
	void SetUp(BOOL bCleanUp);
};

#endif //__SNIFF_H__
