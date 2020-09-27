// FindSpec.cpp

#include "stdafx.h"
#include "find.h"
#include "findspec.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// FindSpecialHandler tables
// reUnix, reZibo, reBrief, reEpsilon

static LPCTSTR rgszSpecialUnix[] = {
	".",		// ID_FIND_ANYCHARACTER
	"[]",		// ID_FIND_RANGE
	"[^]",		// ID_FIND_NOTRANGE
	"^",		// ID_FIND_LINE_BEGIN
	"$",		// ID_FIND_LINE_END
	"\\(\\)",	// ID_FIND_TAG
	"\\~",		// ID_FIND_NOT
	"\\!",		// ID_FIND_OR
	"*",		// ID_FIND_CLOSURE_ZERO
	"+",		// ID_FIND_CLOSURE_ONE
	"\\{\\}",	// ID_FIND_GROUP

	"\\:a", // "[a-zA-Z0-9]"							ID_FIND_ALPHANUM
	"\\:b+", // "[ \t]#"								ID_FIND_WHITESPACE
	"\\:c", // "[a-zA-Z]"								ID_FIND_ALPHA
	"\\:d", // "[0-9]"									ID_FIND_DECIMAL
	"\\:h", // "([0-9a-fA-F]#)"							ID_FIND_HEX
	"\\:n", // "([0-9]#.[0-9]@![0-9]@.[0-9]#![0-9]#)"	ID_FIND_NUMBER
	"\\:z", // "([0-9]#)"								ID_FIND_INTEGER
	"\\:i", // "([a-zA-Z_$][a-zA-Z0-9_$]@)"				ID_FIND_IDENTIFIER
	"\\:w", // "([a-zA-Z]#)"							ID_FIND_WORD
	"\\:q" // "([0-9]#)"								ID_FIND_STRING
};

static LPCTSTR rgszSpecialBrief[] = {
	"?",		// ID_FIND_ANYCHARACTER
	"[]",		// ID_FIND_RANGE
	"[~]",		// ID_FIND_NOTRANGE
	"%",		// ID_FIND_LINE_BEGIN
	"$",		// ID_FIND_LINE_END
	"{}",		// ID_FIND_TAG
	"~",		// ID_FIND_NOT
	"|",		// ID_FIND_OR
	"@",		// ID_FIND_CLOSURE_ZERO
	"+",		// ID_FIND_CLOSURE_ONE
	"{}",		// ID_FIND_GROUP

	"[a-zA-Z0-9]",	// ID_FIND_ALPHANUM
	"[ \\x09]+",	// ID_FIND_WHITESPACE
	"[a-zA-Z]",		// ID_FIND_ALPHA
	"[0-9]",		// ID_FIND_DECIMAL
	"[0-9a-fA-F]+", // ID_FIND_HEX
	"{[0-9]+.[0-9]@}|{[0-9]@.[0-9]+}|{[0-9]+}",	// ID_FIND_NUMBER
	"[0-9]+",								// ID_FIND_INTEGER
	"[a-zA-Z_$][a-zA-Z0-9_$]@",				// ID_FIND_IDENTIFIER
	"[a-zA-Z]+",	// ID_FIND_WORD
	"\"[~\"]@\""	// ID_FIND_STRING
};

static LPCTSTR rgszSpecialEpsilon[] = {
	".",		// ID_FIND_ANYCHARACTER
	"[]",		// ID_FIND_RANGE
	"[^]",		// ID_FIND_NOTRANGE
	"^",		// ID_FIND_LINE_BEGIN
	"$",		// ID_FIND_LINE_END
	"()",		// ID_FIND_TAG
	"~",		// ID_FIND_NOT
	"|",		// ID_FIND_OR
	"*",		// ID_FIND_CLOSURE_ZERO
	"+",		// ID_FIND_CLOSURE_ONE
	"()",		// ID_FIND_GROUP

	"[a-zA-Z0-9]",	// ID_FIND_ALPHANUM
	"[ <tab>]+",	// ID_FIND_WHITESPACE
	"[a-zA-Z]",		// ID_FIND_ALPHA
	"[0-9]",		// ID_FIND_DECIMAL
	"[0-9a-fA-F]+", // ID_FIND_HEX
	"([0-9]+.[0-9]*|[0-9]*.[0-9]+|[0-9]+)",	// ID_FIND_NUMBER
	"[0-9]+",								// ID_FIND_INTEGER
	"[a-zA-Z_$][a-zA-Z0-9_$]@",				// ID_FIND_IDENTIFIER
	"[a-zA-Z]+",	// ID_FIND_WORD
	"\"[~\"]*\""	// ID_FIND_STRING
};


void ReplaceSel( CButtonEdit & ComboEdit, LPCTSTR szRep, NewSelPos selpos, BOOL bWrap, int nWrapBreak )
{
	DWORD nStart = 0;
	DWORD nEnd = 0;
	// where to put the ip after we insert the item
	DWORD ipStart = 0;
	DWORD ipEnd = 0;

	_TCHAR szText[LIMIT_FIND_COMBO_TEXT+1];
	_TCHAR szNew[LIMIT_FIND_COMBO_TEXT+1];
	szText[0] = _T('\0');
	szNew[0] = _T('\0');
	nStart = LOWORD(ComboEdit.m_dwLastSel);
	nEnd = HIWORD(ComboEdit.m_dwLastSel);
	ComboEdit.GetWindowText(szText, LIMIT_FIND_COMBO_TEXT);

	LPTSTR Src = szText;
	LPTSTR Dst = szNew;
	LPCTSTR Rep = szRep;

	// copy left part before the selection
	_tcsncpy(Dst,Src,nStart);
	Src = _tcsninc(Src,nStart);
	Dst = _tcsninc(Dst,nStart);

	if (bWrap)
	{
		// copy left half of rep
		_tcsncpy(Dst,Rep,nWrapBreak);
		Rep = _tcsninc(Rep,nWrapBreak);
		Dst = _tcsninc(Dst,nWrapBreak);
		// copy selected text
		_tcsncpy(Dst,Src,nEnd-nStart);
		Dst = _tcsninc(Dst,nEnd-nStart);
	}

	// copy remaining replacement (all if no wrap)
	_tcscpy(Dst,Rep);
	Dst = _tcsninc(Dst, _tcslen(Rep));

	// point Src to end of selection
	Src = _tcsninc(szText,nEnd);

	// copy remaining Src
	_tcscpy(Dst,Src);

	switch(selpos)
	{
	case spStart:
		ipStart = ipEnd = nStart;
		break;
	case spEnd:
		ipStart = ipEnd = nStart + _tcslen(szRep);
		break;
	case spSel:
		ipStart = nStart;
		if (bWrap)
			ipEnd = nEnd + _tcslen(szRep);
		else
			ipEnd = nStart + _tcslen(szRep);
		break;
	case spInsideStart:
		ipStart = ipEnd = nStart + nWrapBreak;
		break;
	case spInsideEnd:
		if (bWrap)
			ipStart = ipEnd = nEnd + nWrapBreak;
		else
			ipStart = ipEnd = nStart + nWrapBreak;
		break;
	}

	ComboEdit.SetWindowText(szNew);
	ComboEdit.SetFocus();
	ComboEdit.SetSel(MAKELONG(ipStart, ipEnd));
}

LPCTSTR *GetCurrentRegexHintTable(void)
{
	switch (GetRegExprSyntax())
	{
		case reUnix:    return rgszSpecialUnix;
		case reBrief:   return rgszSpecialBrief;
		case reEpsilon: return rgszSpecialEpsilon;
	}

	ASSERT(FALSE); // shouldn't have fallen through!!
	return rgszSpecialUnix; // we'll send back something valid if this happens
}


// FindSpecialHandler
//
// hDlg		Dialog handle
// idEdit	ID of edit control to receive the text.
// cmd		Menu ID from the IDR_FIND_SPECIAL menu
//
// environParams.reSyntax:
//	reUnix, reZibo, reBrief, reEpsilon
BOOL FindSpecialHandler( CButtonEdit & ComboEdit, UINT cmd, BOOL bWrap )
{
	CString strText, strLeft, strRight;
	LPCTSTR * rgszSpecial;
	LPCTSTR szRe;
	NewSelPos selpos = spEnd;

	rgszSpecial = GetCurrentRegexHintTable();

	switch(cmd)
	{

	// smart insert
	case ID_FIND_NOTRANGE:
		selpos = spInsideEnd;
		szRe = rgszSpecial[cmd - ID_FIND_ANYCHARACTER];
		ReplaceSel( ComboEdit, szRe, selpos, bWrap, 2);
		return TRUE;
		break;

	case ID_FIND_RANGE:
	case ID_FIND_GROUP:
	case ID_FIND_TAG:
		selpos = spInsideStart;
		szRe = rgszSpecial[cmd - ID_FIND_ANYCHARACTER];
		ReplaceSel( ComboEdit, szRe, selpos, bWrap, _tcslen(szRe)/2);
		return TRUE;
		break;

	// simple insert
	case ID_FIND_ANYCHARACTER:
	case ID_FIND_LINE_BEGIN:
	case ID_FIND_LINE_END:
	case ID_FIND_NOT:
	case ID_FIND_OR:
	case ID_FIND_CLOSURE_ZERO:
	case ID_FIND_CLOSURE_ONE:
	// shorthand
	case ID_FIND_ALPHANUM:
	case ID_FIND_WHITESPACE:
	case ID_FIND_ALPHA:
	case ID_FIND_DECIMAL:
	case ID_FIND_HEX:
	case ID_FIND_NUMBER:
	case ID_FIND_INTEGER:
	case ID_FIND_IDENTIFIER:
	case ID_FIND_WORD:
	case ID_FIND_STRING:
		szRe = rgszSpecial[cmd - ID_FIND_ANYCHARACTER];
		ReplaceSel( ComboEdit, szRe );
		return TRUE;
		break;

	case ID_REPLACE_FINDWHATTEXT:
	case ID_REPLACE_TAG_1:
	case ID_REPLACE_TAG_2:
	case ID_REPLACE_TAG_3:
	case ID_REPLACE_TAG_4:
	case ID_REPLACE_TAG_5:
	case ID_REPLACE_TAG_6:
	case ID_REPLACE_TAG_7:
	case ID_REPLACE_TAG_8:
	case ID_REPLACE_TAG_9:
	{
		//reZibo, reUnix, reBrief, reEpsilon
		const _TCHAR rgchTag[] = "$\\\\#";
		_TCHAR szRep[3];
		szRep[0] = rgchTag[g_reCurrentSyntax];
		szRep[1] = (cmd - ID_REPLACE_FINDWHATTEXT) + _T('0');
		szRep[2] = _T('\0');
		ReplaceSel( ComboEdit, szRep );
		break;
	}

	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}



