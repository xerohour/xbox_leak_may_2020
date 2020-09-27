///////////////////////////////////////////////////////////////////////////////
//	WIZCASES.H
//
//	Created by :			Date :
//		WayneBr				10/4/94
//
//	Description :
//		Declaration of the CDefAppWizCases class
//

#ifndef __WIZCASES_H__
#define __WIZCASES_H__

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	BOOL CDefAppWizCases:: class

class CDefAppWizCases : public CPerformanceCases 
{
	DECLARE_TEST(CDefAppWizCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
	void		Initialize_Def_Appwiz();

// Data
protected:
	CString 	FCN;

};


#endif // WIZCASES_H__
