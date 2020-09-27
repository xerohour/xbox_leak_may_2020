///////////////////////////////////////////////////////////////////////////////
//	buildsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CBuildSubSuite class
//

#ifndef __buildsub_H__
#define __buildsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CBuildSubSuite class

class CBuildSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CBuildSubSuite)

	DECLARE_TESTLIST()
protected:
//	void CleanUp(void);
};

#endif //__buildsub_H__
