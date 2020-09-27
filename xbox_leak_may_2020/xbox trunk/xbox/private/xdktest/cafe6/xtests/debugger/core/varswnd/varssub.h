///////////////////////////////////////////////////////////////////////////////
//	VARSSUB.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CVarsWndSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CVarsWndSubSuite class

class CVarsWndSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CVarsWndSubSuite)
	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__SNIFF_H__
