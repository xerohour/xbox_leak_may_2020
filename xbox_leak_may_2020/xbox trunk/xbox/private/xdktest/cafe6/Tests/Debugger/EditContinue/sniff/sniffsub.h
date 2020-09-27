///////////////////////////////////////////////////////////////////////////////
//	sniffsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CSniffSubSuite class
//

#ifndef __SNIFFSUB_H__
#define __SNIFFSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSniffSubSuite class

class CSniffSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CSniffSubSuite)
	DECLARE_TESTLIST()

public:
	void SetUp(BOOL bCleanUp);
	BOOL fProjBuilt;

protected:
	void CleanUp(void);
};

#endif //__SNIFFSUB_H__
