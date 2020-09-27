///////////////////////////////////////////////////////////////////////////////
//	SPYCASES.H
//
//	Created by :			Date :
//		WayneBr				10/4/94
//
//	Description :
//		Declaration of the CSpyCases class
//

#ifndef __SPYCASES_H__
#define __SPYCASES_H__

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	BOOL CSpyCases:: class

class CSpyCases : public CPerformanceCases 
{
	DECLARE_TEST(CSpyCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
	void		Initialize_Spy();

// Data
protected:
	CString 	FCN;

};


#endif // SPYCASES_H__


