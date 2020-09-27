///////////////////////////////////////////////////////////////////////////////
//	EESUB.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CEESubSuite class
//

#ifndef __EESUB_H__
#define __EESUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CEESubSuite class

class CEESubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CEESubSuite)
	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__SNIFF_H__
