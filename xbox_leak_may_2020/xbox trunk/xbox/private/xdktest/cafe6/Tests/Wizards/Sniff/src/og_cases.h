///////////////////////////////////////////////////////////////////////////////
//	og_cases.h
//
//	Created by :			
//		Anita George - modified version of og_cases.h
//
//	Description :
//		Declaration of the CObjGalTestCases Class
//

#ifndef __OGCASES_H__
#define __OGCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	Symbols used in test

#define	OG_PROJNAME1			"og_proj1"
#define	OG_PROJNAME2			"og_proj2"
#define OG_NEW_CLASSNAME		"CNewOGX"
#define OG_CBUTTON				1
#define OG_OGX_FULLNAME			"New OGX.ogx"
#define OG_OGX_NAME				"New OGX"
#define OG_THIRD_COMPONENT		2


///////////////////////////////////////////////////////////////////////////////
//	CObjGalTestCases class

class CObjGalTestCases : public CTest
{
	DECLARE_TEST(CObjGalTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
		UIProjectWizard m_prjwiz;

// Test Cases
protected:

	BOOL CreateNewOGX(void);
	BOOL CreateNewProj(void);
	BOOL ApplyOglet(void);
	void BuildOglet(void);

};

#endif //__OGCASES_H__
