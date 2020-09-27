///////////////////////////////////////////////////////////////////////////////
//	SQLDebugSuite.h
//
//	Created by :		Date:			
//		CHRISKO				5/21/97
//
//	Description :
//		Declaration of the CSQLDebugSuite class
//

#ifndef __SQLDebugSuite_H__
#define __SQLDebugSuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSQLDebugSuite class

class CSQLDebugSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CSQLDebugSuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__SQLDebugSuite_H__
