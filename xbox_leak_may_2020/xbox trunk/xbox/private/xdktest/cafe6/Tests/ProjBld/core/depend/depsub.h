///////////////////////////////////////////////////////////////////////////////
//	depsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CDependenciesSubSuite class
//

#ifndef __depsub_H__
#define __depsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDependenciesSubSuite class

class CDependenciesSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CDependenciesSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__depsub_H__
