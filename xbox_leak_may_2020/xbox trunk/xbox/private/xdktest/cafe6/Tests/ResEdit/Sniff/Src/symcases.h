///////////////////////////////////////////////////////////////////////////////
//	SYMCASES.H
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Declaration of the CSymTestCases class
//

#ifndef __SYMCASES_H__
#define __SYMCASES_H__

#ifndef __AFXWIN_H__
	#error include 'cafe.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	CSymTestCases class

class CSymTestCases : public CTest
{
	DECLARE_TEST(CSymTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
	BOOL CheckSymbolCounts();
	BOOL CreateDefaultSymbol();
	BOOL QuotedStringID();
	BOOL SymbolDropDown();
	BOOL UsedByList();
	BOOL ViewUse();
	BOOL CreatSymbol();
	BOOL ChangeUnusedSymbol();
	BOOL ChangeInUseSymbol();
	BOOL ChangeUsingIDProp();
	BOOL DeleteUnusedSymbol();
	BOOL DeleteInUseSymbol();

// Utilities
protected:
	int CountSymbols(BOOL bReadOnly, int nExpected = 0);
};

#define SYMBOL_RC	"symbol.rc"

#endif //__SYMCASES_H__
