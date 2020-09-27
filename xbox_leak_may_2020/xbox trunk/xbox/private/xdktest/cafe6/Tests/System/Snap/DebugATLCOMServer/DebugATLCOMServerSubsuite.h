///////////////////////////////////////////////////////////////////////////////
//	DebugATLCOMServerSubsuite.h
//
//	Created by :		Date:			
//		MichMa				5/21/97
//
//	Description :
//		Declaration of the CDebugATLCOMServerSubsuite class
//

#ifndef __DebugATLCOMServerSubsuite_H__
#define __DebugATLCOMServerSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDebugATLCOMServerSubsuite class

class CDebugATLCOMServerSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CDebugATLCOMServerSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__DebugATLCOMServerSubsuite_H__
