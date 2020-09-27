//----------------------------------------------------------------------------
//
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

#include "vcrange.h" // lists the range of ids that one could use
#include "stdidcmd.h"

// Menus
#define IDM_VCP_VCPROJ				0x00F1
#define IDMX_VCP_PROJECTNODE		0x00F4
// Groups
//#define	IDG_VC_EXECUTE				0xF100
#define	IDG_VCP_PROJECT				0xF200
#define IDG_VCP_VCPROJ				0xF300
#define	IDG_VC_DO					0xF400
#define IDG_VC_CV_PROJECT_WIZARDS   0xF500  // ClassView wizards grp for VC added to the CV ptoject context menu
#define	IDG_VC_CPPSVC				0xF600
#define	IDG_VC_CV_ITEM_WIZARDS	    0xF700  // ClassView wizards grp for VC added to the default CV item context menu


//Command IDs
#define	icmdLaunchDesigner			0xF100
#define	icmdLaunchDDS				0xF200
#define	icmdLaunchToolBox			0xF300
#define	icmdSyncDesigner			0xF400

#define	icmdClassFromTypeLibWiz		0x8916
#define	icmdClassFromAxControlWiz	0x8917
#define	icmdPerfObjectWiz			0x8918
#define	icmdPerfCounterWiz			0x8919

#define	icmdCollapseBlock			0x8924
#define	icmdCollapseAllBlocks		0x8925

#endif //_VCCMD_H

