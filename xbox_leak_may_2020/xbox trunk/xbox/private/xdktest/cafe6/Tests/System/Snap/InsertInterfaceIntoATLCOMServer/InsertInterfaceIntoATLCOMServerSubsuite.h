///////////////////////////////////////////////////////////////////////////////
//	InsertInterfaceIntoATLCOMServerSubsuite.h
//
//	Created by :		Date:			
//		MichMa				5/21/97
//
//	Description :
//		Declaration of the CInsertInterfaceIntoATLCOMServerSubsuite class
//

#ifndef __InsertInterfaceIntoATLCOMServerSubsuite_H__
#define __InsertInterfaceIntoATLCOMServerSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CInsertInterfaceIntoATLCOMServerSubsuite class

class CInsertInterfaceIntoATLCOMServerSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CInsertInterfaceIntoATLCOMServerSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__InsertInterfaceIntoATLCOMServerSubsuite_H__
