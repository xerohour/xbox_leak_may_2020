//----------------------------------------------------------------------------
//
//  Microsoft Visual C++
//
//  Microsoft Confidential
//  Copyright 1997-1998 Microsoft Corporation.  All Rights Reserved.
//
//  File:	gpmenus.cmd
//	Area:	VC Package Commands
//
//  Contents:
//		VC Package Menus
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
// Note that most of the buttons previously defined in this file have been
// moved into the shared command group CMDSETID_StandardCommandSet2K.
// Buttons icmdOpen and icmdOpenWith have been replaced with commands
// cmdidOpen and cmdidOpenWith from CMDSETID_StandardCommandSet97.
// Any new  commands should be added to CMDSETID_StandardCommandSet2K.
// The command IDs for the shared command group are in src\common\inc\stdidcmd.h 
// (together with the IDs for CMDSETID_StandardCommandSet97). 
// To add a new command:-
//       1. Add the ID to the end of the CMDSETID_StandardCommandSet2K IDs.
//       2. Add the button definition to src\appid\cmddef\pkgui\SharedCmdDef.ctc.
//       3. Add the new command to the CMDUSED_SECTION in this file (see below)
//       4. Place the command either within this file in CMDPLACEMENT_SECTION
//          below or by adding it to a shared menu in src\appid\inc\SharedCmdPlace.ctc
//          as appropriate. Note that it may be necessary to add the command to
//          more than one menu or toolbar.
//----------------------------------------------------------------------------


#include "stdidcmd.h"
#include "vsshlids.h"
#include "msobtnid.h"

#include "gpcmd.h"
#include "sccmnid.h"

#define guidOfficeIcon		    { 0xd309f794, 0x903f, 0x11d0, { 0x9e, 0xfc, 0x00, 0xa0, 0xc9, 0x11, 0x00, 0x4f } }

// {77280048-B6B7-11d0-AD88-00C04FB6BE4C}
#define guidVCGrpId { 0x77280048, 0xb6b7, 0x11d0, { 0xad, 0x88, 0x0, 0xc0, 0x4f, 0xb6, 0xbe, 0x4c } }

#define BTN_FLAGS      DYNAMICVISIBILITY|DEFAULTINVISIBLE|DEFAULTDISABLED
#define BTNTXT_FLAGS   DYNAMICVISIBILITY|DEFAULTINVISIBLE|DEFAULTDISABLED|TEXTCHANGESBUTTON|TEXTCHANGES

//----------------------------------------------------------------------------
//  CMDS_SECTION
//
//  Description:
//	  This section defines all the commands that the genproj package adds
//	  to the shell
//
// BUTTON SUBSECTION:
//  CMDBTNNORM	     msotbbtAuto|msotbbtChgAccel
//  CMDBTNTEXT	     msotbbtText|msotbbtChgAccel
//  CMDBTNPICT	     msotbbtPict|msotbbtChgAccel
//  CMDBTNOWNERDRAW  msotbbtOwnerDraw|msotbbtChgAccel
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
// Note that we are using a number of the the shell/shared menus and groups
// in the sections below and where appropriate adding our VC specific commands
// to those menus and groups. The shell/shared menus and groups are defined in
// src\appid\inc\SharedCmdPlace.ctc and src\appid\inc\ShellCmdPlace.ctc. 
// Examine those files to determine the correct placements when adding further
// VC specific commands to the context or main menus.
//
//----------------------------------------------------------------------------
CMDS_SECTION guidVCGrpId
  MENUS_BEGIN
    // GUID:MENUID						PARENT GROUP							PRIORITY    TYPE		BUTTONTEXT			MENUTEXT	      TOOLTIPTEXT	    COMMANDTEXT
	
	// context menus
	guidVCGrpId:IDMX_VC_EXEPROJECT,		guidVCGrpId:0,							0x0000,   CONTEXT,		"Exe Project";
	
	guidVCGrpId:IDM_VC_ADD,				guidVCGrpId:0,							0x0000,   CONTEXT,		"Add";
	guidVCGrpId:IDM_VC_PRJ_DEBUG,		guidVCGrpId:0,							0x0000,   CONTEXT,		"Debug";

  MENUS_END



  NEWGROUPS_BEGIN

	// Project Main Menu Groups
    // Place our "Compile" and "link" commands on the project menu	
	guidVCGrpId:IDG_VC_VCPROJ,		    guidSHLMainMenu:IDM_VS_MENU_BUILD,	    0xF000;

	// VC Project Menu Groups 
	guidVCGrpId:IDG_VC_EXECUTE,			guidVCGrpId:IDM_VC_VCPROJ,				0x0200;
	guidVCGrpId:IDG_VC_REMOVEUNLOAD,	guidVCGrpId:IDM_VC_VCPROJ,				0x0300;


	
	guidVCGrpId:IDG_VC_DEBUG,			guidVCGrpId:IDM_VC_PRJ_DEBUG,			0x0100;
	
	// Folder Node Context Menu Groups

	// File Node Context Menu Groups

	// ExeProject Context Menu Groups
	guidVCGrpId:IDG_VC_PREVIEWSTART,	guidVCGrpId:IDMX_VC_EXEPROJECT,			0x0100;

	// Multi Node Context Menu Groups
 
  NEWGROUPS_END


  BUTTONS_BEGIN
    // GUID:CMDID						PRIMARY GROUP			PRIORITY	ICONID						BUTTONTYPE	FLAGS					BUTTONTEXT		 	MENUTEXT		  		TOOLTIPTXT	    	COMMANDNAME
  BUTTONS_END
CMDS_END


CMDUSED_SECTION   // for shared commands
	// GUID:GROUPID	
	guidVSStd97:cmdidOpen;
	guidVSStd97:cmdidOpenWith;
	guidVSStd2K:ECMD_COMPILE;
	guidVSStd2K:ECMD_LINKONLY;
	guidVSStd2K:ECMD_WEBDEPLOY;
	guidVSStd2K:ECMD_XBOXIMAGE;
	guidVSStd2K:ECMD_XBOXDEPLOY;
	guidVSStd2K:cmdidAddClass;
	guidVSStd2K:ECMD_PROJSTARTDEBUG;
	guidVSStd2K:ECMD_PROJSTEPINTO;
	guidVSStd97:cmdidAddExistingItem;
	guidVSStd97:cmdidAddNewItem;
	guidVSStd2K:ECMD_ADDWEBREFERENCE;
	guidVSStd2K:ECMD_ADDWEBREFERENCECTX;
	guidVSStd2K:ECMD_ADDRESOURCE;
	guidVSStd2K:ECMD_UPDATEWEBREFERENCE;
	guidVSStd2K:ECMD_SYNCHRONIZEFOLDER;
	guidVSStd2K:ECMD_SYNCHRONIZEALLFOLDERS;
	guidVSStd2K:ECMD_SHOWALLFILES;
	guidVSStd2K:ECMD_SLNREFRESH;
	guidVSStd2K:ECMD_COPYPROJECT;
CMDUSED_END

//////////////////////////////////////////////////////////////////////////////
// CMDPLACEMENT_SECTION
// 
// This sections defines where the commands will be placed if not just in 
// their primary groups.
//
//////////////////////////////////////////////////////////////////////////////

CMDPLACEMENT_SECTION
  // put individual cmds in groups and groups in menus.
  // GUID:GROUPID			    		PARENT MENU				PRIORITY 

	// Build commands
 	guidVSStd2K:ECMD_LINKONLY,			guidVCGrpId:IDG_VC_PRJ_BUILD, 		0x0100;
 	guidVSStd2K:ECMD_WEBDEPLOY,			guidVCGrpId:IDG_VC_PRJ_BUILD,		0x0100;
 	guidVSStd2K:ECMD_XBOXIMAGE,			guidVCGrpId:IDG_VC_PRJ_BUILD,		0x0100;
 	guidVSStd2K:ECMD_XBOXDEPLOY,		guidVCGrpId:IDG_VC_PRJ_BUILD,		0x0100;
	guidVSStd2K:ECMD_COMPILE,			guidVCGrpId:IDG_VC_PRJ_BUILD,		0x0200;

	
	guidVSStd2K:ECMD_PROJSTARTDEBUG,		guidVCGrpId:IDG_VC_DEBUG,		0x0120;
	guidVSStd2K:ECMD_PROJSTEPINTO,			guidVCGrpId:IDG_VC_DEBUG,		0x0130;
	
	guidVSStd2K:ECMD_UPDATEWEBREFERENCE,		guidSHLMainMenu:IDG_VS_CTXT_ITEM_OPEN,	0x0200;
	
	// EXE project context menu placements
	guidSHLMainMenu:IDG_VS_CTXT_PROJECT_START,	guidVCGrpId:IDMX_VC_EXEPROJECT,		0x0100;
	guidSHLMainMenu:IDG_VS_CTXT_ITEM_PROPERTIES,   	guidVCGrpId:IDMX_VC_EXEPROJECT,  	0xF000;
    
	// Project Main Menu Commands
	guidVSStd2K:ECMD_LINKONLY,			guidVCGrpId:IDG_VC_VCPROJ,		0x0100;
	guidVSStd2K:ECMD_WEBDEPLOY,			guidVCGrpId:IDG_VC_VCPROJ,		0x0100;
	guidVSStd2K:ECMD_XBOXDEPLOY,		guidVCGrpId:IDG_VC_VCPROJ,		0x0100;
	guidVSStd2K:ECMD_XBOXIMAGE,			guidVCGrpId:IDG_VC_VCPROJ,		0x0100;
	guidVSStd2K:ECMD_COMPILE,			guidVCGrpId:IDG_VC_VCPROJ,		0x0200;

CMDPLACEMENT_END
