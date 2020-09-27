///////////////////////////////////////////////////////////////////////////////
//	BreakpointsSubsuite.h
//
//	Created by: MichMa		Date: 3/12/98
//
//	Description:
//		Declaration of the CECBreakpointsSubsuite class
//

#ifndef __BREAKPOINTSSUBSUITE_H__
#define __BREAKPOINTSSUBSUITE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


///////////////////////////////////////////////////////////////////////////////
// CECBreakpointsSubsuite class

class CECBreakpointsSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CECBreakpointsSubsuite)
	DECLARE_TESTLIST()
};


#endif //__BREAKPOINTSSUBSUITE_H__
