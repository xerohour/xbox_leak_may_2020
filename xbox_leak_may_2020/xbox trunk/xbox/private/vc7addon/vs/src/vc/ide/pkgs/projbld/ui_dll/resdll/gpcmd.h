//----------------------------------------------------------------------------
//
//  Microsoft Visual C++
//
//  Microsoft Confidential
//  Copyright 1997-1998 Microsoft Corporation.  All Rights Reserved.
//
//  File:	vccmd.h
//	Area:	VC Package Commands
//
//  Contents:
//		VC Package Menu, Group, Command IDs
//
//----------------------------------------------------------------------------
#ifndef _VCCMD_H
#define _VCCMD_H

#include "stdidcmd.h"

// Menus
#define IDM_VC_VCPROJ				0x0001
#define IDM_VC_VCSCC				0x0003
#define IDMX_VC_PROJECTNODE			0x0004
#define IDMX_VC_FOLDERNODE			0x0005
#define IDMX_VC_FILENODE			0x0006
#define IDM_VC_CSCD_BLDEVENTS		0x0007
#define IDM_VC_CSCD_USERTOOLS		0x0008
#define IDMX_VC_PROJNODE_DEBUG		0x0009
#define IDMX_VC_EXEPROJECT			0x000A
#define IDMX_VC_MULTINODE			0x000B

#define IDM_VC_ADD 					0x000C
#define IDM_VC_PRJ_DEBUG 			0x000D

// Groups
#define IDG_VC_VCP					0x0010
#define IDG_VC_VCPROJ				0x0011
#define IDG_VC_BUILD				0x0013
#define IDG_VC_EXECUTE				0x0014
#define IDG_VC_VCSCC				0x0015
#define IDG_VC_FOLDER				0x0016
#define IDG_VC_FILE					0x0017
#define IDG_VC_PREVIEW				0x0018
#define IDG_VC_DELETE				0x0019
#define IDG_VC_PROPERTIES			0x001A
#define IDG_VC_PROJSAVE				0x001B
#define IDG_VC_PROJECT				0x001C
#define IDG_VC_VCSCCMENU			0x001D
#define IDG_VC_DELETERENAME			0x001E
#define IDG_VC_ADDPROJ				0x001F
#define IDG_VC_REMOVEFROMPROJ       0x0020
#define	IDG_VC_SETTINGS				0x002F
#define	IDG_VC_SAVE					0x003F
#define IDG_VC_PREVIEWSTART			0x004F
#define IDG_VC_PROJECTBUILD			0x005F
#define IDG_VC_NODEPLOY				0x0060
#define IDG_VC_REMOVEUNLOAD			0x0061
#define IDG_VC_BLDEVENTS			0x0062
#define IDG_VC_VCTOOLS				0x0063
#define IDG_VC_USERTOOLS			0x0064
#define IDG_VC_DEBUG				0x0065
#define IDG_VC_PRJ_BUILD			0x0066
#define IDG_VC_ADD					0x0067
#define IDG_VC_START				0x0068
#define IDG_VC_CTXT_ADD				0x0069
#define IDG_VC_CTXT_DEBUG			0x006A
#define IDG_VC_PRJ_REMOVE  		    0x006B
#define IDG_VC_FLD_REMOVE      		0x006C
#define IDG_VC_FIL_REMOVE      		0x006D


//Command IDs are now in src\common\inc\stdidcmd.h
#define cmdidClassWiz				0xF100
#define cmdidAddNewItemCtx			0xF101
#define cmdidAddExistingItemCtx		0xF102
#define cmdidAddClassCtx			0xF103
#define cmdidAddResource			0xF104
#define cmdidAddResourceCtx			0xF105


#endif //_VCCMD_H

