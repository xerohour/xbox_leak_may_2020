/*===========================================================================*\
 | 
 |  FILE:	wM3_pick.cpp
 |			Weighted Morpher for MAX R3
 |			Pick mode methods
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 27-9-98
 | 
\*===========================================================================*/

#include "wM3.h"


BOOL  GetMorphNode::Filter(INode *node)
{
	Interval valid; 
	
	ObjectState os = node->GetObjectRef()->Eval(mp->ip->GetTime());

	if( os.obj->IsDeformable() == FALSE ) return FALSE;

	// Check for same-num-of-verts-count
	if( os.obj->NumPoints()!=mp->MC_Local.Count) return FALSE;

//	if (os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
//		return FALSE;
//		}

	node->BeginDependencyTest();
	mp->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
	if (node->EndDependencyTest()) {		
		return FALSE;
	} else {
		return TRUE;
		
		}
}


BOOL  GetMorphNode::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
{	
	if (ip->PickNode(hWnd,m,this)) {
		return TRUE;
	} else {
		return FALSE;
		}
}

BOOL  GetMorphNode::Pick(IObjParam *ip,ViewExp *vpt)
	{
	
	INode *node = vpt->GetClosestHit();
	if (node) {
		// Make the node reference, and then ask the channel to load itself

		UI_MAKEBUSY

		mp->ReplaceReference(101+mp->chanSel+mp->chanNum,node);
		mp->chanBank[mp->chanSel+mp->chanNum].buildFromNode(node);

		UI_MAKEFREE
	}
	
	return TRUE;
}


void  GetMorphNode::EnterMode(IObjParam *ip)
{
	// FIX: select the currently active viewport so that
	// the user can use the H shortcut
	ViewExp *ve = mp->ip->GetActiveViewport();
	SetFocus(ve->GetHWnd());
	mp->ip->ReleaseViewport(ve);

	// flag that we are infact picking
	isPicking=TRUE;

	ICustButton *iBut;

	iBut = GetICustButton(GetDlgItem(mp->hwChannelParams,IDC_PICK));
		if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);

	for( int i=IDC_P1;i<IDC_P10+1;i++){
	HWND button = GetDlgItem(mp->hwChannelList,i);
	iBut = GetICustButton(button);
		if (iBut) 
		{
			iBut->SetHighlightColor(GREEN_WASH);
			InvalidateRect(button, NULL, FALSE);
		}
	ReleaseICustButton(iBut);
	}
}

void  GetMorphNode::ExitMode(IObjParam *ip)
{
	isPicking=FALSE;

	ICustButton *iBut;

	iBut = GetICustButton(GetDlgItem(mp->hwChannelParams,IDC_PICK));
		if (iBut) iBut->SetCheck(FALSE);
	ReleaseICustButton(iBut);

	for( int i=IDC_P1;i<IDC_P10+1;i++){
	HWND button = GetDlgItem(mp->hwChannelList,i);
	iBut = GetICustButton(button);
		if (iBut) 
		{
			iBut->SetHighlightColor(RGB(210,210,210));
			InvalidateRect(button, NULL, FALSE);
		}
	ReleaseICustButton(iBut);
	}
}