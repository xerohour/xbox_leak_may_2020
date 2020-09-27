	//----------------------------------------------------------------------------
//
//  Microsoft Visual Studio
//
//  Microsoft Confidential
//  Copyright 1997-1998 Microsoft Corporation.  All Rights Reserved.
//
//  File:	menucmds.h
//	Area:	Help Package Commands
//
//  Contents:
//		Helps System Package Menu, Group, Command IDs
//
//----------------------------------------------------------------------------
#ifndef __HELPIDS_H_
#define __HELPIDS_H_


#ifndef NOGUIDS

#ifdef DEFINE_GUID
  // WB package object CLSID
  DEFINE_GUID (guidHelpCmdId,
    0x4a79114a, 0x19e4, 0x11d3, 0xb8, 0x6b, 0x0, 0xc0, 0x4f, 0x79, 0xf8, 0x2);

  DEFINE_GUID (guidHelpGrpId,
    0x4a79114b, 0x19e4, 0x11d3, 0xb8, 0x6b, 0x0, 0xc0, 0x4f, 0x79, 0xf8, 0x2);

  DEFINE_GUID (guidHelpPkg,
    0x4a791146, 0x19e4, 0x11d3, 0xb8, 0x6b, 0x0, 0xc0, 0x4f, 0x79, 0xf8, 0x2);

#else
// {4A79114A-19E4-11d3-B86B-00C04F79F802}
#define guidHelpCmdId    {0x4a79114a, 0x19e4, 0x11d3, {0xb8, 0x6b, 0x0, 0xc0, 0x4f, 0x79, 0xf8, 0x2 }}

// {4A79114B-19E4-11d3-B86B-00C04F79F802}
#define guidHelpGrpId    {0x4a79114b, 0x19e4, 0x11d3, {0xb8, 0x6b, 0x0, 0xc0, 0x4f, 0x79, 0xf8, 0x2 }}


// The following is the same as CLSID_HelpPackage but for consumption by CTC.
// {4A791146-19E4-11d3-B86B-00C04F79F802}
#define guidHelpPkg			{0x4a791146, 0x19e4, 0x11d3, {0xb8, 0x6b, 0x0, 0xc0, 0x4f, 0x79, 0xf8, 0x2}}

#endif //DEFINE_GUID

#endif //NOGUIDS

	
// Menus
#define IDM_HELP_CONTENTS	    0x0001
#define IDM_HELP_KEYWORDS	    0x0002
#define IDM_HELP_SEARCH		    0x0003

#define IDM_HELP_MENU_MSONTHWEB     0x0100

#define IDM_HLPTOC_CTX		    0x0200
#define IDM_HELP_RESLIST_CTX        0x0300
#define IDM_HELP_RESLIST_CTX_SORTBY 0x0400

// Groups
#define IDG_HELP_GRP		    0x0010
#define IDG_HELPVIEW_GRP            0x0020
#define IDG_HELP_FILTERS            0x0030

#define IDG_HLPTOC_CTX_PRINT        0x0050

#define IDG_HELP_RESLIST_CTX_SORTBY 0x0060
#define IDG_HELP_RESLIST_CTX_COLUMNS 0x0070

#define IDG_HELP_MSONTHEWEB_NEWS    0x0100
#define IDG_HELP_MSONTHEWEB_INFO    0x0200
#define IDG_HELP_MSONTHEWEB_HOME    0x0300


//Command IDs
#define icmdHelpContents	    0x0100
#define icmdHelpKeywords            0x0101
#define icmdHelpSearch		    0x0102
#define icmdModifyFilter            0x0103
#define icmdHelpOnHelp              0x0104

#define icmdSearchResults           0x0110
#define icmdIndexResults            0x0111

// TOC contex menu                  
#define icmdPrintTopic              0x0120
#define icmdPrintChildren           0x0121

#define icmdSortByCol1              0x0130
#define icmdSortByCol2              0x0131
#define icmdSortByCol3              0x0132
#define icmdSortByCol4              0x0133
#define icmdSortByCol5              0x0134
#define icmdSortByCol6              0x0135
#define icmdSortByCol7              0x0136
#define icmdSortByCol8              0x0137
#define icmdSortByCol9              0x0138
#define icmdSortByCol10             0x0139

#define icmdSortByColMin            icmdSortByCol1
#define icmdSortByColMax            icmdSortByCol10


#define icmdHelpOnTheWeb_FIRST      0x1000
#define icmdHelpOnTheWebFree        0x1000 // Must be consecutive.
#define icmdHelpOnTheWebNews        0x1001
#define icmdHelpOnTheWebFAQ         0x1002
#define icmdHelpOnTheWebSupport     0x1003
#define icmdHelpOnTheWebMSDN        0x1004
#define icmdHelpOnTheWebFeedback    0x1005
#define icmdHelpOnTheWebBest        0x1006
#define icmdHelpOnTheWebSearch      0x1007
#define icmdHelpOnTheWebTutorial    0x1008
#define icmdHelpOnTheWebHome        0x1009
#define icmdHelpOnTheWeb_LAST       0x1009

///////////////////////////////////////////////////////////////////////////////
// Button Bitmap IDs

#define bmpidVsHelpContents           1
#define bmpidVsHelpIndex              2
#define bmpidVsHelpSearch             3
#define bmpidVsHelpIndexResults       4
#define bmpidVsHelpSearchResults      5



#endif //__HELPIDS_H_