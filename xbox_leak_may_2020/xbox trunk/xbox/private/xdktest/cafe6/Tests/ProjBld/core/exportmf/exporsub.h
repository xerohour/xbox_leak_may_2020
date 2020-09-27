///////////////////////////////////////////////////////////////////////////////
//	exporsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CExportMakefileSubSuite class
//

#ifndef __exporsub_H__
#define __exporsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CExportMakefileSubSuite class

class CExportMakefileSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CExportMakefileSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__exporsub_H__
