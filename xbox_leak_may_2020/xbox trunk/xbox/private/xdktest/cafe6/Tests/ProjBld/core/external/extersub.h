///////////////////////////////////////////////////////////////////////////////
//	extersub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CextersubSuite class
//

#ifndef __extersub_H__
#define __extersub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CExternalMakefileSubSuite class

class CExternalMakefileSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CExternalMakefileSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__extersub_H__
