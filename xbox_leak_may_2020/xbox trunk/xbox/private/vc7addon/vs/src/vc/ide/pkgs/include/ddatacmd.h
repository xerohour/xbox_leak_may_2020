//-----------------------------------------------------------------------------
// Microsoft DaVinci
//
// Microsoft Confidential
// Copyright 1995 - 1996 Microsoft Corporation. All Rights Reserved.
//
// @doc 
//
// @module		ddatacmd.h - Data Tools Command Set |
// Defines the command ids and group ids as well as the guids for the Data 
// tools (Query, Schema)
// 
// @owner		RahulK
//-----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////
//
// GUIDs for data tools command set and menu/group set 
//
//////////////////////////////////////////////////////////////////////////////
#ifdef DEFINE_GUID

	// Guid for daVinci data tools' group and menu ids
	//{732abe74-cd80-11d0-a2db-00aa00a3efff}
	DEFINE_GUID(CMDSETID_DaVinciDataToolsMenuSet, 0x732abe74, 0xcd80, 0x11d0, 0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff);

	// Guid for daVinci data tools' command ids
	//{732abe75-cd80-11d0-a2db-00aa00a3efff}
	DEFINE_GUID(CMDSETID_DaVinciDataToolsCommandSet, 0x732abe75, 0xcd80, 0x11d0, 0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff);

#else

	// Guid for daVinci data tools' group and menu ids
	//{732abe74-cd80-11d0-a2db-00aa00a3efff}
	#define CMDSETID_DaVinciDataToolsMenuSet	{0x732abe74, 0xcd80, 0x11d0, {0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff} }
  
	// Guid for daVinci data tools' command ids
	//{732abe75-cd80-11d0-a2db-00aa00a3efff}
	#define CMDSETID_DaVinciDataToolsCommandSet	{0x732abe75, 0xcd80, 0x11d0, {0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff} }

	// These following are only needed by the CTC compiler
	// Guid for daVinci query tool's glyph bitmap
	//{732abe76-cd80-11d0-a2db-00aa00a3efff}
	#define guidDavQryBmpId	{0x732abe76, 0xcd80, 0x11d0, {0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff} }

	// Guid for daVinci schema tool's glyph bitmap
	//{732abe77-cd80-11d0-a2db-00aa00a3efff}
	#define guidDavSchBmpId	{0x732abe77, 0xcd80, 0x11d0, {0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff} }

	// Guid for daVinci tools' shared glyph bitmap
	//{732abe78-cd80-11d0-a2db-00aa00a3efff}
	#define guidDavFrmBmpId	{0x732abe78, 0xcd80, 0x11d0, {0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff} }

#endif

#define guidDavDataGrpId	CMDSETID_DaVinciDataToolsMenuSet
#define guidDavDataCmdId	CMDSETID_DaVinciDataToolsCommandSet

#ifndef _DDATACMD_H_
#define _DDATACMD_H_


//////////////////////////////////////////////////////////////////////////////
//
// Command IDs
//
//////////////////////////////////////////////////////////////////////////////
// Note we can't expressions like (cmdidStart + 1) since those cause the
// CTC compiler to fail

#define	cmdidAddRelatedTables			0x0001 
#define cmdidLayoutDiagram				0x0002
#define cmdidLayoutSelection			0x0003
#define cmdidInsertColumn				0x0004
#define cmdidDeleteColumn				0x0005
#define cmdidNewTextAnnotation			0x0006
#define cmdidShowRelLabels				0x0007
#define cmdidViewPageBreaks				0x0008
#define cmdidRecalcPageBreaks			0x0009
#define cmdidViewUserDefined			0x000a
#define cmdidGenerateQuery				0x000b
#define cmdidDeleteFromDB				0x000c
#define cmdidAutoSize					0x000d
#define cmdidEditViewUserDefined		0x000e
#define cmdidSetAnnotationFont			0x000f
#define cmdidZoomPercent200				0x0010 
#define cmdidZoomPercent150				0x0011 
#define cmdidZoomPercent100				0x0012 
#define cmdidZoomPercent75				0x0013 
#define cmdidZoomPercent50				0x0014 
#define cmdidZoomPercent25				0x0015 
#define cmdidZoomPercent10				0x0016 
#define cmdidZoomPercentSelection		0x0017 
#define cmdidZoomPercentFit				0x0018




//////////////////////////////////////////////////////////////////////////////
//
// Menu / Group IDs
//
//////////////////////////////////////////////////////////////////////////////
// Menu / toolbar ids are prefixed with a 0, group ids by a 1
// Note we can't expressions like (cmdidStart + 1) since those cause the
// CTC compiler to fail

// Query Design menus and groups ----------------------------------------------
	
#define	IDM_QRY_TOOLBAR_QUERY			0x0001		// query toolbar
#define IDM_QRY_MAINMENU				0x0002
#define	IDMX_QRY_DIAGRAM_NONE			0x0003		// diagram pane ctx menu
#define	IDMX_QRY_DIAGRAM_TABLE			0x0004		// diagram pane table (ECL) ctx menu
#define	IDMX_QRY_DIAGRAM_FIELD			0x0005		// diagram pane field (ECOL) ctx menu
#define	IDMX_QRY_DIAGRAM_JOINLINE		0x0006		// diagram pane join line (JLN) ctx menu
#define	IDMX_QRY_DIAGRAM_MULTIPLE		0x0007		// diagram pane multi-select ctx menu
#define IDMX_QRY_QBE					0x0008		// qbe pane ctx menu
#define IDMX_QRY_SQL					0x0009		// sql pane ctx menu
#define IDMX_QRY_DATA					0x000a		// data pane ctx menu
#define	IDM_QRY_TOOLBAR_VIEW			0x000b		// view toolbar

#define	IDMC_QRY_GOTOROW				0x0010
#define IDMC_QRY_CHANGETYPE				0x0011
#define IDMC_DTOOLS_TABLE_VIEW			0x0012
#define IDMC_QRY_SHOWPANES				0x0013

#define	IDG_QRY_MAIN_SQL				0x1001	
#define	IDG_QRY_MAIN_SYNTAX				0x1002 
#define	IDG_QRY_MAIN_QUERY				0x1003 
#define IDG_QRY_MAIN_OUTPUT				0x1004 
#define	IDG_QRY_MAIN_SORT				0x1005
#define	IDG_QRY_MAIN_FILTER				0x1006 	 
#define	IDG_QRY_MAIN_JOIN				0x1007
												
#define IDG_QRY_CHANGETYPE				0x1008
#define IDG_QRY_SHOWPANES				0x1009 

#define IDG_QRY_TOOLBAR_PANES			0x100a
#define	IDG_QRY_TOOLBAR_QUERYTYPE		0x100b
#define	IDG_QRY_TOOLBAR_SQL				0x100c
#define	IDG_QRY_TOOLBAR_QUERYSHAPE		0x100d
#define	IDG_QRY_TOOLBAR_VIEWSHAPE		0x100e

#define IDG_QRY_DIAGRAM_NONE_SHAPE		0x100f
#define IDG_QRY_DIAGRAM_NONE_PANE		0x1010
#define IDG_QRY_DIAGRAM_TBL_VIEW		0x1011
#define	IDG_QRY_DIAGRAM_TBL_EDIT		0x1012
#define	IDG_QRY_DIAGRAM_FIELD_OUTPUT	0x1013
#define	IDG_QRY_DIAGRAM_FIELD_SORT		0x1014
#define	IDG_QRY_DIAGRAM_FIELD_FILTER	0x1015
#define IDG_QRY_DIAGRAM_JLN_EDIT		0x1016
#define	IDG_QRY_DIAGRAM_JLN_OUTER		0x1017
#define	IDG_QRY_DIAGRAM_MULTI_EDIT		0x1018
#define	IDG_QRY_QBE_EDIT				0x1019
#define	IDG_QRY_SQL_RUN					0x101a
#define	IDG_QRY_SQL_SYNTAX				0x101b
#define	IDG_QRY_SQL_EDIT				0x101c
#define IDG_QRY_DATA_RUN				0x101d
#define	IDG_QRY_DATA_GOTO				0x101e
#define IDG_QRY_DATA_EDIT				0x101f
#define IDG_QRY_DIAGRAM_NONE_RUN		0x1020
#define IDG_QRY_QBE_RUN					0x1021
#define IDG_QRY_QBE_SHAPE				0x1022

// Shared between Query & Schema
#define	IDG_DTOOLS_VIEW_DT				0x1023
#define IDG_DTOOLS_TABLE_VIEW			0x1024

// Schema Designer menus and groups -------------------------------------------
	
#define	IDM_SCH_TOOLBAR_DIAGRAM			0x0101		// database diagram toolbar
#define	IDMX_SCH_DIAGRAM				0x0102		// diagram ctx menu (nothing selected)
#define	IDMX_SCH_TABLE					0x0103		// table ctx menu
#define	IDMX_SCH_RELATIONSHIP			0x0104		// relationship ctx menu
#define	IDMX_SCH_TEXTANNOTATION			0x0105		// Text annotation ctx menu
#define IDMC_SCH_ZOOM					0x0106		// Zoom cascade menu
#define IDM_SCH_TOOLBAR_TABLE			0x0107	    // table toolbar

#define	IDG_SCH_DIAG_ADDOBJS			0x1101	
#define	IDG_SCH_DIAG_EDIT				0x1102 
#define	IDG_SCH_DIAG_DIAGRAM			0x1103 
#define IDG_SCH_DIAG_LAYOUT				0x1104 

#define	IDG_SCH_TABLE_VIEW				0x1105
#define IDG_SCH_TABLE_ADDREL			0x1106		
#define IDG_SCH_TABLE_OPS				0x1107
#define	IDG_SCH_TABLE_REMOVE			0x1108
#define IDG_SCH_TABLE_LAYOUT			0x1109

#define	IDG_SCH_REL_REMOVE				0x110a

#define IDG_SCH_TOOLBAR_TABLE			0x110b
#define IDG_SCH_TOOLBAR_TABLEOPS		0x110c	
#define IDG_SCH_TOOLBAR_ANNOTATION		0x110d
#define IDG_SCH_TOOLBAR_SAVE			0x110e
#define IDG_SCH_TOOLBAR_TABLEVIEW		0x110f 
#define IDG_SCH_TOOLBAR_DIAGRAM			0x1110
#define IDG_SCH_TOOLBAR_LAYOUT			0x1111
#define IDG_SCH_TOOLBAR_ZOOM			0x1112

#define IDG_SCH_EDIT_KEYS				0x1113		// Edit main menu keys grp
#define IDG_SCH_EDIT_TA					0x1114
#define IDG_SCH_DIAGRAM_SHOWLABELS		0x1115		// Diagram main menu show labels grp	

#define IDG_SCH_ZOOM_LEVELS				0x1116		// Zoom cascade menu groups
#define IDG_SCH_TA_EDIT					0x1117		// TA ctx menu grps
#define IDG_SCH_TA_FONT					0x1118									  

#define IDG_SCH_TABLE_UDV				0x1119

#endif // _DDATACMD_H_