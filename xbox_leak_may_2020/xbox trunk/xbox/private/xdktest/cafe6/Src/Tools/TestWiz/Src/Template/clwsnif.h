///////////////////////////////////////////////////////////////////////////////
//	$$type_h$$.h
//
//	Created by :			
//		$$TestOwner$$
//
//	Description :
//		Declaration of the $$DriverClass$$ class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__
			 
#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// $$DriverClass$$ class

class $$DriverClass$$ : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE($$DriverClass$$)

	DECLARE_TESTLIST();

	virtual void SetUp(BOOL bCleanUp);
};

#endif //__SNIFF_H__
