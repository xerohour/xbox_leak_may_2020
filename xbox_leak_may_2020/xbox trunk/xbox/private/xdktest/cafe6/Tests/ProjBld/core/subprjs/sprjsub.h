///////////////////////////////////////////////////////////////////////////////
//	sprjsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CSubprojectsSubSuite class
//

#ifndef __sprjsub_H__
#define __sprjsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSubprojectsSubSuite class

class CSubprojectsSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CSubprojectsSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__sprjsub_H__
