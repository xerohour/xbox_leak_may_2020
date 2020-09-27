///////////////////////////////////////////////////////////////////////////////
//	ATLWizsuite.h
//
//	Created by :		Date:			
//		ChrisKoz				1/28/98
//
//	Description :
//		Declaration of the CSysATLWizSubsuite class
//

#ifndef __ATLWizSubsuite_H__
#define __ATLWizSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSysATLWizSubsuite class

class CSysATLWizSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CSysATLWizSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__ATLWizSubsuite_H__
