/**********************************************************************
 *<
	FILE: clstnode.cpp   

	DESCRIPTION:  Vertex cluster animating modifier that uses nodes

	CREATED BY: Rolf Berteig

	HISTORY: created 27 October, 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mods.h"

class ClustNodeMod : public Modifier {	
	public:
		INode *node;
		static IObjParam *ip;
		static HWND hParams;
		Matrix3 tm, invtm;

		ClustNodeMod();
		~ClustNodeMod();

		// From Animatable
		void DeleteThis() {delete this;}
		void GetClassName(TSTR& s) { s= GetString(IDS_RB_CLUSTNODEMOD); }  
		virtual Class_ID ClassID() { return Class_ID(CLUSTNODEOSM_CLASS_ID,0);}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() {return GetString(IDS_RB_NODEXFORM);}
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO|PART_SELECT|PART_SUBSEL_TYPE;}
		ChannelMask ChannelsChanged() {return PART_GEOM;}
		Class_ID InputType() {return defObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return node;}
		void SetReference(int i, RefTargetHandle rtarg) {node=(INode*)rtarg;}
		
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
				
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
	};


//--- ClassDescriptor and class vars ---------------------------------

IObjParam* ClustNodeMod::ip = NULL;
HWND ClustNodeMod::hParams  = NULL;

class ClustNodeClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new ClustNodeMod;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_NODEXFORM_CLASS);}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return Class_ID(CLUSTNODEOSM_CLASS_ID,0); }
	const TCHAR* 	Category() {return GetString(IDS_RB_DEFDEFORMATIONS);}
	};

static ClustNodeClassDesc clustNodeDesc;
extern ClassDesc* GetClustNodeModDesc() {return &clustNodeDesc;}


//--- CustMod dlg proc ------------------------------

class PickControlNode : 
		public PickModeCallback,
		public PickNodeCallback {
	public:				
		ClustNodeMod *mod;
		PickControlNode() {mod=NULL;}
		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);		
		BOOL Pick(IObjParam *ip,ViewExp *vpt);		
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);		
		BOOL Filter(INode *node);
		PickNodeCallback *GetFilter() {return this;}
		BOOL RightClick(IObjParam *ip,ViewExp *vpt) {return TRUE;}
	};
static PickControlNode thePickMode;

BOOL PickControlNode::Filter(INode *node)
	{
	node->BeginDependencyTest();
	mod->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
	if (node->EndDependencyTest()) {		
		return FALSE;
	} else {
		return TRUE;
		}
	}

BOOL PickControlNode::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{	
	if (ip->PickNode(hWnd,m,this)) {
		return TRUE;
	} else {
		return FALSE;
		}
	}

BOOL PickControlNode::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	if (node) {
		// RB 3/1/99: This should use the node tm not the object TM. See ModifyObject() imp.
		Matrix3 ourTM,ntm = node->GetNodeTM(ip->GetTime()); //node->GetObjectTM(ip->GetTime());	

		ModContextList mcList;
		INodeTab nodes;
		ip->GetModContexts(mcList,nodes);
		assert(nodes.Count());
		ourTM = nodes[0]->GetObjectTM(ip->GetTime());
		mod->tm    = ourTM * Inverse(ntm);
		mod->invtm = Inverse(ourTM);
		mod->ReplaceReference(0,node);
		mod->NotifyDependents(FOREVER,0,REFMSG_CHANGE);

		SetWindowText(GetDlgItem(mod->hParams,IDC_CLUST_NODENAME),
					mod->node->GetName());
		nodes.DisposeTemporary();
		}
	return TRUE;
	}

void PickControlNode::EnterMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_CLUST_PICKNODE));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);
	}

void PickControlNode::ExitMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_CLUST_PICKNODE));
	if (iBut) iBut->SetCheck(FALSE);
	ReleaseICustButton(iBut);
	}

static BOOL CALLBACK ClustParamDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	ClustNodeMod *mod = (ClustNodeMod*)GetWindowLong(hWnd,GWL_USERDATA);
	
	switch (msg) {
		case WM_INITDIALOG: {
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod = (ClustNodeMod*)lParam;
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_CLUST_PICKNODE));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(iBut);
			if (mod->node) {
				SetWindowText(GetDlgItem(hWnd,IDC_CLUST_NODENAME),
					mod->node->GetName());
			} else {
				SetWindowText(GetDlgItem(hWnd,IDC_CLUST_NODENAME),
					GetString(IDS_RB_NONE));
				}
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CLUST_PICKNODE:
					thePickMode.mod  = mod;					
					mod->ip->SetPickMode(&thePickMode);
					break;
				}
			break;
		
		default:
			return FALSE;
		}
	return TRUE;
	}

//--- ClustMod methods -------------------------------

ClustNodeMod::ClustNodeMod()
	{
	node  = NULL;
	tm    = Matrix3(1);
	invtm = Matrix3(1);
	}

ClustNodeMod::~ClustNodeMod()
	{
	DeleteAllRefsFromMe();
	}

#define CLUSTNODE_TM_CHUNK		0x0100
#define CLUSTNODE_INVTM_CHUNK	0x0110

IOResult ClustNodeMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	IOResult res = IO_OK;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case CLUSTNODE_TM_CHUNK:
				tm.Load(iload);
				break;

			case CLUSTNODE_INVTM_CHUNK:
				invtm.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}
	return IO_OK;
	}

IOResult ClustNodeMod::Save(ISave *isave)
	{
	Modifier::Save(isave);
	isave->BeginChunk(CLUSTNODE_TM_CHUNK);
	tm.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(CLUSTNODE_INVTM_CHUNK);
	invtm.Save(isave);
	isave->EndChunk();
	
	return IO_OK;
	}

void ClustNodeMod::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
	hParams  = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_CLUSTNODEPARAM),
		ClustParamDlgProc,
		GetString(IDS_RB_PARAMETERS),
		(LPARAM)this);
	}

void ClustNodeMod::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	ip->ClearPickMode();
	this->ip = NULL;
	ip->DeleteRollupPage(hParams);
	}



class ClustNodeDeformer: public Deformer {
	public:
		Matrix3 tm,invtm;
		ClustNodeDeformer(Matrix3 m,Matrix3 mi) {tm=m;invtm=mi;}
		Point3 Map(int i, Point3 p) {return (p*tm)*invtm;}
	};

void ClustNodeMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	if (this->node) {
		Interval valid = FOREVER;
		Matrix3 ntm = this->node->GetNodeTM(t,&valid);
		ClustNodeDeformer deformer(tm*ntm,invtm);
		os->obj->Deform(&deformer, TRUE);
		os->obj->UpdateValidity(GEOM_CHAN_NUM,valid);	
		}
	}

Interval ClustNodeMod::LocalValidity(TimeValue t)
	{
	Interval valid = FOREVER;
	if (node) {
		node->GetNodeTM(t,&valid);
		}
	return valid;
	}

RefTargetHandle ClustNodeMod::Clone(RemapDir& remap)
	{
	ClustNodeMod *newmod = new ClustNodeMod;
	newmod->ReplaceReference(0,node);
	newmod->tm    = tm;
	newmod->invtm = invtm;
	return newmod;
	}

RefResult ClustNodeMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message)
	{
	switch (message) {
		case REFMSG_TARGET_DELETED:
			if (hTarget==node) {
				node = NULL;
				}
			break;
		}
	return REF_SUCCEED;
	}
