///////////////////////////////////////////////////////////////////////////////
//	optsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CBuildOptionsSubSuite class
//

#ifndef __optsub_H__
#define __optsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CBuildOptionsSubSuite class

class CBuildOptionsSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CBuildOptionsSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__optsub_H__
