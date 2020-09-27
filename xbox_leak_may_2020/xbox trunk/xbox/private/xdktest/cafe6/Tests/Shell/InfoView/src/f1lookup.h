///////////////////////////////////////////////////////////////////////////////
//	F1Lookup.H
//
//	Created by :					Date :
//		MarcI							9/3/94
//
//	Description :
//		Declaration of the IV's F1Lookup class
//

#ifndef __F1LOOKUP_H__
#define __F1LOOKUP_H__

#include "sniff.h"
#include "resource.h"
#include "ivutil.h"
///////////////////////////////////////////////////////////////////////////////
//	BOOL CF1Lookup:: class

class CF1Lookup : public CTest 
{
	DECLARE_TEST(CF1Lookup, CSniffDriver)

// Operations
public:
	virtual void Run(void);
	void		Initialize_F1();
	void		Lookup(UINT uiID);

// Data
protected:
	CString 	FCN;
private:
	void	FindText(LPCSTR szFind, BOOL bMatchWord, BOOL bMatchCase, BOOL bRegExpr);

};


#endif // __F1LOOKUP_H__


