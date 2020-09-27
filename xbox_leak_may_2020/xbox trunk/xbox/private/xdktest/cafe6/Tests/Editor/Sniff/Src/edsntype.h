///////////////////////////////////////////////////////////////////////////////
//	edsntype.h
//
//	Created by :			
//		GeorgeCh
//
//	Description :
//		Declaration of the Edsnif01Driver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// Edsnif01Driver class

class EditorSniff : public CIDESubSuite
{
	DECLARE_SUBSUITE(EditorSniff)

	DECLARE_TESTLIST();

	// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
};

#endif //__SNIFF_H__
