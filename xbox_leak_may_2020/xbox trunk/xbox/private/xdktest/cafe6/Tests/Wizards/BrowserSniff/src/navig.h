///////////////////////////////////////////////////////////////////////////////
//	NAVIG.H
//
//	Created by :			Date :
//		TomSe					1/31/94
//
//	Description :
//		Declaration of the CBrzTestGoto class
//

#ifndef __NAVIG_H__
#define __NAVIG_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

//
//	Foward decl for SymbolInfo needed for CBrzTestGoto::GotoSingleDef() decl.
//	Definition is in BrzGoto.cpp.
//
struct SymbolInfo;

///////////////////////////////////////////////////////////////////////////////
//	BrzTestGoto class

enum GotoType;

class CNavigate : public CTest
{
	DECLARE_TEST(CNavigate, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
	COFile   file;
	COBrowse brz;
	COSource src;


// Test Goto
protected:
	BOOL GotoTest( const SymbolInfo&);
};

#endif //__NAVIG_H__
