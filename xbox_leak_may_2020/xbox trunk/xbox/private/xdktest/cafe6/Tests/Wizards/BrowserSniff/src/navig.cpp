///////////////////////////////////////////////////////////////////////////////
//	NAVIG.CPP
//
//	Created by :			Date :
//		TomSe					1/31/94
//
//	Description :
//		Implementation of the CNavigate class
//

#include "stdafx.h"
#include "sniff.h"
#include "navig.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

enum GotoType {
	GotoDef,
	GotoRef,
};

struct SymbolInfo {
	GotoType type;
	LPCSTR	symbolname;
	LPCSTR	file_line;
	int amb;
};


static SymbolInfo gotoSymbols[] = {
	{	GotoDef, "WinMain",	"browse.cpp(12)" },
	{	GotoDef, "theApp",	"browse.cpp(10)" },
	{	GotoRef, "theApp",	"browse.cpp(14)" },
	{	GotoRef, "ctest",	"ctest.h(43)" },
	{	GotoDef, "ctest",	"ctest.c(17)" },
	{	GotoDef, "m_i",		"cpptest.h(34)", 4 },	//	Should be MI::m_i
	{	GotoDef, "MI",		"cpptest.h(27)" },
};


const int numTests = sizeof( gotoSymbols ) / sizeof( SymbolInfo ) + 6;

IMPLEMENT_TEST(CNavigate, CTest, "Navigation Tests", -1, CSniffDriver)

void CNavigate::Run(void)

	{
	COWorkSpace ws;
	int result;

	if(GetSubSuite()->InitProject() == ERROR_SUCCESS)

		{
		//
		//	Goto definition for different types.
		//
		GotoTest( gotoSymbols[0] );
		GotoTest( gotoSymbols[1] );
		GotoTest( gotoSymbols[2] );	// First reference for theApp

		result = brz.NextReference();
		result = result && brz.VerifyNavigation( "browse.cpp(15)" );
		if (!result) {
			m_pLog->RecordFailure("NextReference: theApp");
		}

		result = brz.NextReference();
		result = result && brz.VerifyNavigation( "browse.cpp(16)" );
		if (!result) {
			m_pLog->RecordFailure("NextReference: theApp");
		}

		result = brz.PrevReference();
		result = result && brz.VerifyNavigation( "browse.cpp(15)" );
		if (!result) {
			m_pLog->RecordFailure("PrevReference: theApp");
		}

		GotoTest( gotoSymbols[3] );

		result = brz.PopContext();
		result = result && brz.VerifyNavigation( "browse.cpp(15)" );
		if (!result) {
			m_pLog->RecordFailure("PopContext: Goto Reference of ctest");
		}

		result = brz.PopContext();
		result = result && brz.VerifyNavigation( "browse.cpp(10)" );
		if (!result) {
			m_pLog->RecordFailure("PopContext: Goto Reference of theApp");
		}

		result = brz.PopContext();
		result = result && brz.VerifyNavigation( "browse.cpp(12)" );
		if (!result) {
			m_pLog->RecordFailure("PopContext: Goto Definition of theApp");
		}

		GotoTest( gotoSymbols[4] );
		GotoTest( gotoSymbols[5] );
		GotoTest( gotoSymbols[6] );
		}

	else
		m_pLog->RecordFailure("src\\browse.mak failed to open or build");
    ws.CloseAllWindows();
	}

///////////////////////////////////////////////////////////////////////////////
//	Test Goto

//
//		Goto location of symbol and verify
//		that correct location is hit.
//
BOOL CNavigate::GotoTest( const SymbolInfo& symdef) {
	//
	//		Need to have GotoNthDefinition that takes a symbol name
	//		as the 2nd parameter.
	//
	int amb = symdef.amb;
	if( amb == 0 )
		amb = NoAmbiguity;
	else
		amb--;

	int result;
	CString strGotoDef( "Goto definition" );
	CString strGotoRef( "Goto reference" );
	CString strAmb( " with Ambiguity" );
	CString strResult;

	switch ( symdef.type ) {
		case GotoDef:
			result = brz.GotoNthDefinition( 1, symdef.symbolname, amb );
			strResult = strGotoDef;
			break;

		case GotoRef:
			result = brz.GotoNthReference( 1, symdef.symbolname, amb );
			strResult = strGotoRef;
			break;

	}
	if( amb != NoAmbiguity )
			strResult += strAmb;

	result = ( result && brz.VerifyNavigation( symdef.file_line )) ? TRUE : FALSE;
	if (!result) {
		m_pLog->RecordFailure("%s: %s", (LPCSTR)strResult, symdef.symbolname );
	}

	return result;
}
