///////////////////////////////////////////////////////////////////////////////
//	Namesub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CNameSpceSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CNameSpceSubSuite class

class CNameSpceSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CNameSpceSubSuite)
	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__SNIFF_H__
