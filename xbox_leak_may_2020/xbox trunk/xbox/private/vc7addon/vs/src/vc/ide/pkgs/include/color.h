//-----------------------------------------------------------------------
//
// File:	 color.h
//
// Contains: common structures, helpers for colorizer
//
//-----------------------------------------------------------------------

#pragma once

#include <cppsvc.h>
#include <textmgr.h>

//syntax coloring stuff
struct SYNTAXITEM
{
    COLORINDEX  iDefFgColor;        // Default foreground color index
    COLORINDEX  iDefBkColor;        // Default background color index
    UINT		idDisplayName;      // Resource ID for display name (localized)
    DWORD       dwDefFontFlags;     // UNUSED:  Reserved for future version
};


enum
{
    VC_COLITEM_TEXT = COLITEM_TEXT,
    // Default Color indexes taken from _DEFAULTITEMS
    VC_COLITEM_KEYWORD = COLITEM_KEYWORD,
    VC_COLITEM_COMMENT = COLITEM_COMMENT,
    VC_COLITEM_IDENTIFIER = COLITEM_IDENTIFIER,
    VC_COLITEM_STRING = COLITEM_STRING,
    VC_COLITEM_NUMBER = COLITEM_NUMBER,
	// C++ Custom Color indexes
	VC_COLITEM_OPERATOR,
	VC_COLITEM_PPKEYWORD,
	VC_COLITEM_USERKEYWORD,
	VC_COLITEM_WIZARDCODE
};

#ifdef __VS_COLORIZER_IMPL__ //only colorizer needs this structure
__declspec(selectany) SYNTAXITEM  m_rgSyntaxItems[] = {
	// Default Color indexes taken from _DEFAULTITEMS
	{CI_BLUE,		CI_USERTEXT_BK,	IDS_SYNTAX_KEYWORD,		0},
	{CI_DARKGREEN,		CI_USERTEXT_BK,	IDS_SYNTAX_COMMENT,		0},
	{CI_USERTEXT_FG,	CI_USERTEXT_BK,	IDS_SYNTAX_IDENTIFIER,		0},
	{CI_USERTEXT_FG,	CI_USERTEXT_BK,	IDS_SYNTAX_STRING,		0},
	{CI_USERTEXT_FG,	CI_USERTEXT_BK,	IDS_SYNTAX_NUMBER,		0},
	// C++ Custom Color indexes
	{CI_USERTEXT_FG,	CI_USERTEXT_BK,	IDS_SYNTAX_OPERATOR,		0},
	{CI_BLUE,		CI_USERTEXT_BK,	IDS_SYNTAX_PPKEYWORD,		0},
	{CI_BLUE,		CI_USERTEXT_BK,	IDS_SYNTAX_USERKEYWORD,		0},
	{CI_DARKGRAY,		CI_USERTEXT_BK,	IDS_SYNTAX_WIZARDCODE,		0}
};

#define N_VC_COLOR_ITEMS sizeof(m_rgSyntaxItems)/sizeof(SYNTAXITEM)	// Number of items in m_rgSyntaxItems above
#endif // __VS_COLORIZER_IMPL__
