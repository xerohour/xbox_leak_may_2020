///////////////////////////////////////////////////////////////////////////////
//	TMPLSUB.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CTemplatesSubSuite class
//

#ifndef __TMPLSUB_H__
#define __TMPLSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CTemplatesSubSuite class

class CTemplatesSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CTemplatesSubSuite)
	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__TMPLSUB_H__
