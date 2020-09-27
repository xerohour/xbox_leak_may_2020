///////////////////////////////////////////////////////////////////////////////
//	Generic.H
//
//	Created by :					Date :
//		DougMan							1/6/97
//
//	Description :
//		Declaration of a Generic class
//		A Starting Point

#ifndef __GENERIC_H__
#define __GENERIC_H__

#include "sniff.h"
#include "ivutil.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////
//	BOOL CGeneric:: class

class CGeneric : public CTest 
{
	DECLARE_TEST(CGeneric, CSniffDriver)

public:
// Overrides
	virtual void Run(void);
// Operations
	
// Data

protected:
	
private:
	
};


#endif // __GENERIC_H__