///////////////////////////////////////////////////////////////////////////////
//	prj1sub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CProj1SubSuite class
//

#ifndef __prj1sub_H__
#define __prj1sub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDisAsmSubSuite class

class CProj1SubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CProj1SubSuite)
//	CDisAsmSubSuite() { m_strName = "TestWizard Generated script"; m_strOwner = "VCBU QA"; }


	DECLARE_TESTLIST()
protected:
//	void CleanUp(void);
};

#endif //__prj1sub_H__
