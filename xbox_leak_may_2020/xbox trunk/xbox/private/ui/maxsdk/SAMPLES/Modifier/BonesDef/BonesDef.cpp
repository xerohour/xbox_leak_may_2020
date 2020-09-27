 /**********************************************************************
 
	FILE: BonesDef.cpp

	DESCRIPTION:  Simple Bones Deformation Plugin

	CREATED BY: Peter Watje

	HISTORY: 8/5/98


 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "iparamm.h"
#include "shape.h"
#include "spline3d.h"
#include "splshape.h"
#include "linshape.h"
#include "surf_api.h"

// This uses the linked-list class templates
#include "linklist.h"
#include "bonesdef.h"
#include "macrorec.h"


HWND BonesDefMod::hParam  = NULL;
CreateCrossSectionMode* BonesDefMod::CrossSectionMode   = NULL;
CreatePaintMode*        BonesDefMod::PaintMode   = NULL;
ICustButton* BonesDefMod::iCrossSectionButton   = NULL;
ICustButton* BonesDefMod::iLock   = NULL;
ICustButton* BonesDefMod::iAbsolute   = NULL;
ICustButton* BonesDefMod::iEnvelope   = NULL;
ICustButton* BonesDefMod::iFalloff   = NULL;
ICustButton* BonesDefMod::iCopy   = NULL;
ICustButton* BonesDefMod::iPaste   = NULL;
ICustButton* BonesDefMod::iPaintButton  = NULL;
ICustToolbar* BonesDefMod::iParams = NULL;

//--- ClassDescriptor and class vars ---------------------------------

IParamMap       *BonesDefMod::pmapParam = NULL;
IObjParam       *BonesDefMod::ip        = NULL;
BonesDefMod     *BonesDefMod::editMod   = NULL;
MoveModBoxCMode *BonesDefMod::moveMode  = NULL;
int			    BonesDefMod::LastSelected = 0;		

class BonesDefClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new BonesDefMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_BONESDEFMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
											   
	Class_ID		ClassID() { return Class_ID(9815843,87654); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}


	};

static BonesDefClassDesc bonesDefDesc;
extern ClassDesc* GetBonesDefModDesc() {return &bonesDefDesc;}


BonesRightMenu rMenu;

//watje 10-13-99 212156
BOOL BonesDefMod::DependOnTopology(ModContext &mc) {
	BoneModData *bmd = (BoneModData*)mc.localData;
	BOOL topo = FALSE;
	if (bmd) 
		for ( int i =0; i <bmd->VertexData.Count(); i++)
			{
			if (bmd->VertexData[i]->modified)
				{
				topo = TRUE;
				i = bmd->VertexData.Count();
				}

			}
	return topo;
}


int BonesDefMod::NumRefs() {
	int ct = 1;

	for (int i = 0; i<BoneData.Count();i++)
		{

		if (BoneData[i].RefEndPt1ID > ct) ct = BoneData[i].RefEndPt1ID;
		if (BoneData[i].RefEndPt2ID > ct) ct = BoneData[i].RefEndPt2ID;
		if (BoneData[i].BoneRefID > ct) ct = BoneData[i].BoneRefID;
		for (int j = 0; j<BoneData[i].CrossSectionList.Count();j++)
			{
			if (BoneData[i].CrossSectionList[j].RefInnerID > ct) ct = BoneData[i].CrossSectionList[j].RefInnerID;
			if (BoneData[i].CrossSectionList[j].RefOuterID > ct) ct = BoneData[i].CrossSectionList[j].RefOuterID;
//			ct += BoneData[i].CrossSectionList.Count()*2;
			}
		}
	return (ct+1);
	}



//watje 9-7-99  198721 
class ReevalModEnumProc : public ModContextEnumProc {
public:
	BonesDefMod *lm;
	BOOL ev;
	ReevalModEnumProc(BonesDefMod *l, BOOL e)
		{
		lm = l;
		ev = e;
		}
private:
	BOOL proc (ModContext *mc);
};

BOOL ReevalModEnumProc::proc (ModContext *mc) {
	if (mc->localData == NULL) return TRUE;

	BoneModData *bmd = (BoneModData *) mc->localData;
	bmd->reevaluate = ev;
	return TRUE;
}

//watje 9-7-99  198721 
void BonesDefMod::Reevaluate(BOOL eval)
{
ReevalModEnumProc lmdproc(this,eval);
EnumModContexts(&lmdproc);

}

void BonesDefMod::CopyBone()
{

if ((ModeBoneIndex != -1) && (BoneData.Count() > 0))
	{
//get end point1
	Interval v;
	BoneData[ModeBoneIndex].EndPoint1Control->GetValue(0,&CopyBuffer.E1,v,CTRL_ABSOLUTE);
//get end point2
	BoneData[ModeBoneIndex].EndPoint2Control->GetValue(0,&CopyBuffer.E2,v,CTRL_ABSOLUTE);
//need to set in local space
//	CopyBuffer.E1 = CopyBuffer.E1*BoneData[ModeBoneIndex].tm;
//	CopyBuffer.E2 = CopyBuffer.E2*BoneData[ModeBoneIndex].tm;

	CopyBuffer.absolute = FALSE;
	if (BoneData[ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
		CopyBuffer.absolute = TRUE;

	CopyBuffer.showEnvelope = FALSE;
	if (BoneData[ModeBoneIndex].flags & BONE_DRAW_ENVELOPE_FLAG)
		CopyBuffer.showEnvelope = TRUE;
	
	CopyBuffer.falloffType = BoneData[ModeBoneIndex].FalloffType;


//get cross sections
	CopyBuffer.CList.ZeroCount();
	for (int i = 0; i < BoneData[ModeBoneIndex].CrossSectionList.Count();i++)
		{
		CopyCrossClass c;
		c.u = BoneData[ModeBoneIndex].CrossSectionList[i].u;
		BoneData[ModeBoneIndex].CrossSectionList[i].InnerControl->GetValue(0,&c.inner,v);
		BoneData[ModeBoneIndex].CrossSectionList[i].OuterControl->GetValue(0,&c.outer,v);
		CopyBuffer.CList.Append(1,&c,1);
		}
	}
}

void BonesDefMod::PasteBone()
{
if (ModeBoneIndex != -1)
	{
//transform end points back

//Delete all old cross sections
	int ct = BoneData[ModeBoneIndex].CrossSectionList.Count();

	CopyBuffer.E1 = CopyBuffer.E1;
	CopyBuffer.E2 = CopyBuffer.E2;

	BoneData[ModeBoneIndex].FalloffType = CopyBuffer.falloffType;

	if (CopyBuffer.absolute)
		BoneData[ModeBoneIndex].flags |= BONE_ABSOLUTE_FLAG;
	else BoneData[ModeBoneIndex].flags &= ~BONE_ABSOLUTE_FLAG;

	if (CopyBuffer.showEnvelope)
		BoneData[ModeBoneIndex].flags |= BONE_DRAW_ENVELOPE_FLAG;
	else BoneData[ModeBoneIndex].flags &= ~BONE_DRAW_ENVELOPE_FLAG;


	UpdatePropInterface();

//	BoneData[ModeBoneIndex].EndPoint1Control->SetValue(0,&CopyBuffer.E1,TRUE,CTRL_ABSOLUTE);
//	BoneData[ModeBoneIndex].EndPoint2Control->SetValue(0,&CopyBuffer.E2,TRUE,CTRL_ABSOLUTE);
	theHold.Suspend();
	for (int i =(ct-1); i >= 0 ; i--)
		RemoveCrossSection(ModeBoneIndex, i);
	for (i =0; i < CopyBuffer.CList.Count() ; i++)
		{
		AddCrossSection(ModeBoneIndex,CopyBuffer.CList[i].u,CopyBuffer.CList[i].inner,CopyBuffer.CList[i].outer);
		}
	theHold.Resume();

	}
}



void BonesDefMod::AddCrossSection(int BoneIndex, float u, float inner, float outer)

{
class CrossSectionClass t;
int index = -1;
t. u = u;
//t.Inner = inner;
//t.Outer = outer;


int CrossInnerRefID = GetOpenID();
int CrossOuterRefID = GetOpenID();
t.RefInnerID = CrossInnerRefID;
t.RefOuterID  = CrossOuterRefID;
t.InnerControl = NULL;
t.OuterControl = NULL;


if ( (BoneData[BoneIndex].CrossSectionList.Count() == 0) || (BoneData[BoneIndex].CrossSectionList.Count() == 1)) 
	{
	index = BoneData[BoneIndex].CrossSectionList.Count();
	BoneData[BoneIndex].CrossSectionList.Append(1,&t,1);	
	}
else
	{
	for (int i = 0; i < BoneData[BoneIndex].CrossSectionList.Count();i++)
		{
		if (BoneData[BoneIndex].CrossSectionList[i].u>=u)
			{
			index =i;
			i = BoneData[BoneIndex].CrossSectionList.Count();
			}
		}
	if (index ==-1)
		{
		BoneData[BoneIndex].CrossSectionList.Append(1,&t);	
		index = BoneData[BoneIndex].CrossSectionList.Count()-1;
		}
	else BoneData[BoneIndex].CrossSectionList.Insert(index,1,&t);	
	}
//create 2 float controls
MakeRefByID(FOREVER,CrossInnerRefID,NewDefaultFloatController());
MakeRefByID(FOREVER,CrossOuterRefID,NewDefaultFloatController());

BoneData[BoneIndex].CrossSectionList[index].InnerControl->SetValue(0,&inner,TRUE,CTRL_ABSOLUTE);
BoneData[BoneIndex].CrossSectionList[index].OuterControl->SetValue(0,&outer,TRUE,CTRL_ABSOLUTE);


/*
float ti,to;
Interval v;
BoneData[BoneIndex].CrossSectionList[index].InnerControl->GetValue(0,&ti,v);
BoneData[BoneIndex].CrossSectionList[index].OuterControl->GetValue(0,&to,v);
*/



}

void BonesDefMod::AddCrossSection(float u)

{
//get current selected bone
// compute the falloff at the u of this bone
class CrossSectionClass t;

float ui,uo,li,lo;
float u_dist;
int index = -1;
t.u = u;
for (int i = 0; i < BoneData[ModeBoneIndex].CrossSectionList.Count();i++)
	{
	if (BoneData[ModeBoneIndex].CrossSectionList[i].u>=u)
		{
		index =i;
		i = BoneData[ModeBoneIndex].CrossSectionList.Count();
		}
	}

int lowerbound, upperbound;
lowerbound = index-1;
upperbound = index;
Interval v;
BoneData[ModeBoneIndex].CrossSectionList[lowerbound].InnerControl->GetValue(0,&li,v);
BoneData[ModeBoneIndex].CrossSectionList[lowerbound].OuterControl->GetValue(0,&lo,v);
BoneData[ModeBoneIndex].CrossSectionList[upperbound].InnerControl->GetValue(0,&ui,v);
BoneData[ModeBoneIndex].CrossSectionList[upperbound].OuterControl->GetValue(0,&uo,v);

//li = BoneData[ModeBoneIndex].CrossSectionList[lowerbound].Inner;
//lo = BoneData[ModeBoneIndex].CrossSectionList[lowerbound].Outer;
//ui = BoneData[ModeBoneIndex].CrossSectionList[upperbound].Inner;
//uo = BoneData[ModeBoneIndex].CrossSectionList[upperbound].Outer;
u_dist = BoneData[ModeBoneIndex].CrossSectionList[upperbound].u - BoneData[ModeBoneIndex].CrossSectionList[lowerbound].u;
u = (u-BoneData[ModeBoneIndex].CrossSectionList[lowerbound].u) /u_dist;
float Inner = (ui-li) * u + li;
float Outer = (uo-lo) * u + lo;

int CrossInnerRefID = GetOpenID();
int CrossOuterRefID = GetOpenID();
t.RefInnerID = CrossInnerRefID;
t.RefOuterID  = CrossOuterRefID;
t.InnerControl = NULL;
t.OuterControl = NULL;
//create 2 float controls
BoneData[ModeBoneIndex].CrossSectionList.Insert(index,1,&t);



MakeRefByID(FOREVER,CrossInnerRefID,NewDefaultFloatController());
MakeRefByID(FOREVER,CrossOuterRefID,NewDefaultFloatController());

BoneData[ModeBoneIndex].CrossSectionList[index].InnerControl->SetValue(0,&Inner,TRUE,CTRL_ABSOLUTE);
BoneData[ModeBoneIndex].CrossSectionList[index].OuterControl->SetValue(0,&Outer,TRUE,CTRL_ABSOLUTE);

if (index <= ModeBoneEnvelopeIndex)
	{
	ModeBoneEnvelopeIndex++;
	if (ModeBoneEnvelopeIndex >= BoneData[ModeBoneIndex].CrossSectionList.Count())
		ModeBoneEnvelopeIndex = BoneData[ModeBoneIndex].CrossSectionList.Count()-1;
	}
/*
int c = sel.Count();
sel.SetCount(c+12);
*/
//append bone to this list.

}


void BonesDefMod::GetCrossSectionRanges(float &inner, float &outer, int BoneID, int CrossID)

{

Interval v;
//if ( (BoneID < BoneData.Count()) && 
//	 (CrossID < BoneData[BoneID].CrossSectionList.Count()) 
  //  )
	{
	BoneData[BoneID].CrossSectionList[CrossID].InnerControl->GetValue(0,&inner,v);
	BoneData[BoneID].CrossSectionList[CrossID].OuterControl->GetValue(0,&outer,v);
	}

}

float BonesDefMod::GetU(ViewExp *vpt,Point3 a, Point3 b, IPoint2 p)
{
//mouse spot
//ModContextList mcList;		
INodeTab nodes;
Point2 fp = Point2((float)p.x, (float)p.y);
float u;
if ( !ip ) return 0.0f;

//ip->GetModContexts(mcList,nodes);
//for ( int i = 0; i < mcList.Count(); i++ ) 
	{
	// Find the location on the segment where the user clicked
//	INode *inode = nodes[i];
	GraphicsWindow *gw = vpt->getGW();
	gw->setTransform(Matrix3(1));
	Point2 spa = ProjectPointF(gw, a);
	Point2 spb = ProjectPointF(gw, b);
	u = Length(spa-fp)/Length(spa-spb);
	}

return u;


}





void BonesDefMod::GetEndPoints(BoneModData *bmd, TimeValue t, Point3 &l1, Point3 &l2, int BoneID)
{

ObjectState os;
ShapeObject *pathOb = NULL;
if ((BoneData[BoneID].flags & BONE_SPLINE_FLAG) && (BoneData[BoneID].Node != NULL) )
	{
	ObjectState os = BoneData[BoneID].Node->EvalWorldState(t);
	pathOb = (ShapeObject*)os.obj;
	l1  = pathOb->InterpPiece3D(t, 0,0 ,0.0f ) * Inverse(BoneData[BoneID].tm);
	l2  = pathOb->InterpPiece3D(t, 0,0 ,1.0f ) * Inverse(BoneData[BoneID].tm);
	}
else
	{
	l1 = bmd->tempTableL1[BoneID];
	l2 = bmd->tempTableL2[BoneID];
/*
	Interval v;
	BoneData[BoneID].EndPoint1Control->GetValue(t,&l1,v);
	BoneData[BoneID].EndPoint2Control->GetValue(t,&l2,v);
	
	l1 = l1 * Inverse(BoneData[BoneID].tm) * Inverse(bmd->BaseTM);
	l2 = l2 * Inverse(BoneData[BoneID].tm) * Inverse(bmd->BaseTM);
*/
	}

}



void BonesDefMod::GetEndPointsLocal(BoneModData *bmd, TimeValue t, Point3 &l1, Point3 &l2, int BoneID)
{

//ObjectState os;
if ((BoneData[BoneID].flags & BONE_SPLINE_FLAG) && (BoneData[BoneID].Node != NULL) )
	{
	ShapeObject *pathOb = NULL;
	ObjectState os = BoneData[BoneID].Node->EvalWorldState(t);
	pathOb = (ShapeObject*)os.obj;
	l1  = pathOb->InterpPiece3D(t, 0,0 ,0.0f );
	l2  = pathOb->InterpPiece3D(t, 0,0 ,1.0f );
	}
else
	{

//	Interval v;
//	BoneData[BoneID].EndPoint1Control->GetValue(t,&l1,v);
//	BoneData[BoneID].EndPoint2Control->GetValue(t,&l2,v);
	l1 = bmd->tempTableL1ObjectSpace[BoneID];
	l2 = bmd->tempTableL2ObjectSpace[BoneID];
	
	}

}



float BonesDefMod::ModifyU(TimeValue t, float LineU,  int BoneID, int sid)

{
ObjectState os;
ShapeObject *pathOb = NULL;
if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
	{
	os = BoneData[BoneID].Node->EvalWorldState(t);
	pathOb = (ShapeObject*)os.obj;
	int SubCount = pathOb->NumberOfPieces(t,0);
	float start,inc;
	inc = 1.0f/(float)SubCount;
	start = inc * sid;
	LineU = start + (LineU * inc);
	}
return LineU;


}

float BonesDefMod::ComputeInfluence(TimeValue t, float Influence, float LineU, int BoneID, int StartCross, int EndCross, int sid)

{
float Inner, Outer;
float LInner, LOuter;

GetCrossSectionRanges(Inner, Outer, BoneID, StartCross);
GetCrossSectionRanges(LInner, LOuter, BoneID, EndCross);


LineU = ModifyU(t, LineU,BoneID,sid);
/*
if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
	{
	os = BoneData[BoneID].Node->EvalWorldState(ip->GetTime());
	pathOb = (ShapeObject*)os.obj;
	int SubCount = pathOb->NumberOfPieces(ip->GetTime(),0);
	float start,inc;
	inc = 1.0f/(float)SubCount;
	start = inc * sid;
	LineU = start + (LineU * inc);
	}
*/
float udist = BoneData[BoneID].CrossSectionList[EndCross].u - BoneData[BoneID].CrossSectionList[StartCross].u;
LineU = LineU - BoneData[BoneID].CrossSectionList[StartCross].u;
float per = LineU/udist;


Inner = Inner + (LInner - Inner) * per;
Outer = Outer + (LOuter - Outer) * per;

//float Influence = 0.0f;

//inside inner envelope
if (Influence <= Inner)
	{
	Influence = 1.0f;
	}
// is it oustide  outer
else if (Influence <= Outer)
	{
	float r1,r2;
	r1 = Outer - Inner;
	r2 = Influence - Inner;
	Influence = 1.0f - (r2/r1);
	ComputeFalloff(Influence,BoneData[BoneID].FalloffType);
	}
//outside puter envelope
	else 
	{
	Influence = 0.0f;
	}
return Influence;

}

int BonesDefMod::ConvertSelectedBoneToListID(int fsel)
{
int sel = 0;
for (int i= 0; i < fsel; i++)
	{
	if (BoneData[i].Node != NULL) sel++;
	}
return sel;
}

int BonesDefMod::ConvertSelectedListToBoneID(int fsel)
{
int sel = -1;
int ct = 0;
while ((sel!=fsel) && (ct<BoneData.Count()))
	{
	if (BoneData[ct].Node != NULL) sel++;
	ct++;
	}
return ct-1;
}

void BonesDefMod::RemoveBone()

{
int fsel;

fsel = SendMessage(GetDlgItem(hParam,IDC_LIST1),
			LB_GETCURSEL ,0,0);
int sel = ConvertSelectedListToBoneID(fsel);

if (sel>=0)
	{
	SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_DELETESTRING  ,(WPARAM) fsel,0);

//	if (theHold.Holding() ) theHold.Put(new DeleteBoneRestore(this,sel));


//nuke reference

//nuke cross sections
	int ct = BoneData[sel].CrossSectionList.Count();
	for (int i =(ct-1); i >= 0 ; i--)
		RemoveCrossSectionNoNotify(sel, i);

//nuke end points

	DeleteReference(BoneData[sel].RefEndPt1ID);
	BoneData[sel].EndPoint1Control = NULL;
	DeleteReference(BoneData[sel].RefEndPt2ID);
	BoneData[sel].EndPoint2Control = NULL;

	RefTable[BoneData[sel].RefEndPt1ID-2] = 0;
	RefTable[BoneData[sel].RefEndPt2ID-2] = 0;
//	BoneData[sel].Node = NULL;

	DeleteReference(BoneData[sel].BoneRefID);
	RefTable[BoneData[sel].BoneRefID-2] = 0;
	BoneData[sel].Node = NULL;
	BoneData[sel].flags= BONE_DEAD_FLAG;

// bug fix 207093 9/8/99	watje
	BoneData[sel].RefEndPt1ID = -1;
	BoneData[sel].RefEndPt2ID = -1;
	BoneData[sel].BoneRefID = -1;

	
	int NodeCount = BoneData.Count();


	ModeBoneIndex = sel;
	ModeBoneEndPoint = -1;
	ModeBoneEnvelopeIndex = -1;
	ModeBoneEnvelopeSubType = -1;
	if (BoneData[ModeBoneIndex].flags & BONE_LOCK_FLAG)
		pblock_param->SetValue(PB_LOCK_BONE,0,1);
	else
		pblock_param->SetValue(PB_LOCK_BONE,0,0);
	if (BoneData[ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
		pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
	else
		pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);

//	if (sel != 0)
//		sel--;

	int fsel = ConvertSelectedBoneToListID(sel);

// bug fix 206160 9/8/99	watje
	BOOL noBonesLeft = FALSE;
	if (SendMessage(GetDlgItem(hParam,IDC_LIST1),
				LB_SETCURSEL ,fsel,0) == LB_ERR)
		{
		if (fsel != 0) 
			fsel--;
// bug fix 206160 9/8/99	watje
		else noBonesLeft = TRUE;
		SendMessage(GetDlgItem(hParam,IDC_LIST1),
				LB_SETCURSEL ,fsel,0);
		}
// bug fix 206160 9/8/99	watje
	if (noBonesLeft)
		fsel = -1;
	else fsel = ConvertSelectedListToBoneID(fsel);
	ModeBoneIndex = fsel;
	ModeBoneEndPoint = -1;
	ModeBoneEnvelopeIndex = -1;
	ModeBoneEnvelopeSubType = -1;
	LastSelected = fsel;
	UpdatePropInterface();
	

	BoneMoved = TRUE;
//watje 9-7-99  198721 
	Reevaluate(TRUE);
	cacheValid = FALSE;
	NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	}
int bct = 0;
for (int i =0; i < BoneData.Count(); i ++)
	{
	if (!(BoneData[i].flags &  BONE_DEAD_FLAG)) bct++;
	}


//if  (BoneData.Count() ==0)
if  (bct == 0)
	{
	DisableButtons();
	}

//for (int k = 0;  k < RefTable.Count(); k++)
//	{
//	DebugPrint(" id %d  refid %d\n",k,RefTable[k]);
//	}
}

void BonesDefMod::RemoveBone(int bid)

{
int sel;
//sel = SendMessage(GetDlgItem(hParams,IDC_LIST1),
//			LB_GETCURSEL ,0,0);
sel = bid;



int fsel = ConvertSelectedBoneToListID(sel);


if (sel>=0)
	{
	SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_DELETESTRING  ,(WPARAM) fsel,0);


//nuke reference
//nuke cross sections
	BoneDataClass *b = &BoneData[sel];

	if (theHold.Holding() ) theHold.Put(new DeleteBoneRestore(this,sel));


	int ct = BoneData[sel].CrossSectionList.Count();
	for (int i =(ct-1); i >= 0 ; i--)
		RemoveCrossSectionNoNotify(sel, i);

//nuke end points
	DeleteReference(BoneData[sel].RefEndPt1ID);
	BoneData[sel].EndPoint1Control = NULL;
	DeleteReference(BoneData[sel].RefEndPt2ID);
	BoneData[sel].EndPoint2Control = NULL;

	RefTable[BoneData[sel].RefEndPt1ID-2] = 0;
	RefTable[BoneData[sel].RefEndPt2ID-2] = 0;

	DeleteReference(BoneData[sel].BoneRefID);
	RefTable[BoneData[sel].BoneRefID-2] = 0;
	BoneData[sel].Node = NULL;
	BoneData[sel].flags= BONE_DEAD_FLAG;

// bug fix 207093 9/8/99	watje
	BoneData[sel].RefEndPt1ID = -1;
	BoneData[sel].RefEndPt2ID = -1;
	BoneData[sel].BoneRefID = -1;

	
	
	int NodeCount = BoneData.Count();
	for (int j=sel;j<(BoneData.Count()-1);j++)
		{
//		ReplaceReference(j+2,BoneData[j+1].Node);
//now copy the data down also 
//		mod->BoneData[j].CrossSectionList.ZeroCount();
//		BoneData[j].CrossSectionList=BoneData[j+1].CrossSectionList;

//fix this need to copy contolers around 
//		BoneData[j].l1 = BoneData[j+1].l1;
//		BoneData[j].l2 = BoneData[j+1].l2;
//		BoneData[j].flags = BoneData[j+1].flags;
		}

//	DeleteReference(NodeCount-1+2);

//	BoneData[NodeCount-1].Node = NULL;
//	BoneData[NodeCount-1].CrossSectionList.ZeroCount();


	ModeBoneEndPoint = -1;
	ModeBoneEnvelopeIndex = -1;
	ModeBoneEnvelopeSubType = -1;
	if (BoneData[sel].flags & BONE_LOCK_FLAG)
		pblock_param->SetValue(PB_LOCK_BONE,0,1);
	else
		pblock_param->SetValue(PB_LOCK_BONE,0,0);
	if (BoneData[sel].flags & BONE_ABSOLUTE_FLAG)
		pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
	else
		pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);

//	if (sel != 0)
//		sel--;
	ModeBoneIndex = sel;
	int fsel = ConvertSelectedBoneToListID(sel);
	SendMessage(GetDlgItem(hParam,IDC_LIST1),
				LB_SETCURSEL ,fsel,0);


//this is a complete hack to remove the null node off the end of the linked list
//since there are no deletion tools in the template
/*
	BoneDataClassList TempBoneData;

	TempBoneData.New();
	for (i = 0; i<BoneData.Count(); i++)
		{
		if (BoneData[i].Node != NULL) 
			{
			TempBoneData.Append(BoneData[i]);
			}
		}
	BoneData.New();
	for (i = 0; i<TempBoneData.Count(); i++)
		{
		BoneData.Append(TempBoneData[i]);
		}


	TempBoneData.New();
*/
	BoneMoved = TRUE;
//watje 9-7-99  198721 
	Reevaluate(TRUE);
	NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	}
int bct = 0;
for (int i =0; i < BoneData.Count(); i ++)
	{
	if (!(BoneData[i].flags &  BONE_DEAD_FLAG)) bct++;
	}

//if  (BoneData.Count() ==0)

if  (bct == 0)
	{
	DisableButtons();
	}


/*
	{
	SendMessage(GetDlgItem(hParams,IDC_LIST1),
					LB_DELETESTRING  ,(WPARAM) sel,0);


//nuke reference

	int NodeCount = BoneData.Count();
	for (int j=sel;j<(BoneData.Count()-1);j++)
		{
		ReplaceReference(j+2,BoneData[j+1].Node);
//now copy the data down also 
//		mod->BoneData[j].CrossSectionList.ZeroCount();
		BoneData[j].CrossSectionList=BoneData[j+1].CrossSectionList;

//fix this need to copy contolers around 
//		BoneData[j].l1 = BoneData[j+1].l1;
//		BoneData[j].l2 = BoneData[j+1].l2;
		BoneData[j].flags = BoneData[j+1].flags;
		}
	DeleteReference(NodeCount-1+2);

	BoneData[NodeCount-1].Node = NULL;
	BoneData[NodeCount-1].CrossSectionList.ZeroCount();

	if (sel != 0)
		sel--;

	
	SendMessage(GetDlgItem(hParams,IDC_LIST1),
				LB_SETCURSEL ,sel,0);

	ModeBoneIndex = sel;
	ModeBoneEndPoint = -1;
	ModeBoneEnvelopeIndex = -1;
	ModeBoneEnvelopeSubType = -1;
	if (BoneData[ModeBoneIndex].flags & BONE_LOCK_FLAG)
		pblock->SetValue(PB_LOCK_BONE,0,1);
	else
		pblock->SetValue(PB_LOCK_BONE,0,0);
	if (BoneData[ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
		pblock->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
	else
		pblock->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);



//this is a complete hack to remove the null node off the end of the linked list
//since there are no deletion tools in the template

	BoneDataClassList TempBoneData;

	TempBoneData.New();
	for (int i = 0; i<BoneData.Count(); i++)
		{
		if (BoneData[i].Node != NULL) 
			{
			TempBoneData.Append(BoneData[i]);
			}
		}
	BoneData.New();
	for (i = 0; i<TempBoneData.Count(); i++)
		{
		BoneData.Append(TempBoneData[i]);
		}


	TempBoneData.New();

	BoneMoved = TRUE;
	reevaluate = TRUE;
	NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	}
*/
}


void BonesDefMod::RemoveCrossSection()

{

if ( (ModeBoneEnvelopeIndex <=0)  || (ModeBoneIndex < 0)  ||
	 (ModeBoneEnvelopeIndex >= (BoneData[ModeBoneIndex].CrossSectionList.Count()-1))
	 )
	return;
BoneDataClass b = BoneData[ModeBoneIndex];

DeleteReference(BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].RefInnerID);
BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].InnerControl = NULL;
DeleteReference(BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].RefOuterID);
BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].OuterControl = NULL;

RefTable[BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].RefInnerID-2] = 0;
RefTable[BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].RefOuterID-2] = 0;

BoneData[ModeBoneIndex].CrossSectionList.Delete(ModeBoneEnvelopeIndex,1);

//watje 9-7-99  198721 
Reevaluate(TRUE);
NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);

}


void BonesDefMod::RemoveCrossSection(int bid, int eid)

{
/*
if ( (ModeBoneEnvelopeIndex <=0)  || (ModeBoneIndex < 0)  ||
	 (ModeBoneEnvelopeIndex >= (BoneData[ModeBoneIndex].CrossSectionList.Count()-1))
	 )
	return;
	*/


DeleteReference(BoneData[bid].CrossSectionList[eid].RefInnerID);
BoneData[bid].CrossSectionList[eid].InnerControl = NULL;
DeleteReference(BoneData[bid].CrossSectionList[eid].RefOuterID);
BoneData[bid].CrossSectionList[eid].OuterControl = NULL;

RefTable[BoneData[bid].CrossSectionList[eid].RefInnerID-2] = 0;
RefTable[BoneData[bid].CrossSectionList[eid].RefOuterID-2] = 0;

BoneData[bid].CrossSectionList.Delete(eid,1);

//watje 9-7-99  198721 
Reevaluate(TRUE);
NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);

}


void BonesDefMod::RemoveCrossSectionNoNotify(int bid, int eid)

{


DeleteReference(BoneData[bid].CrossSectionList[eid].RefInnerID);
BoneData[bid].CrossSectionList[eid].InnerControl = NULL;
DeleteReference(BoneData[bid].CrossSectionList[eid].RefOuterID);
BoneData[bid].CrossSectionList[eid].OuterControl = NULL;

RefTable[BoneData[bid].CrossSectionList[eid].RefInnerID-2] = 0;
RefTable[BoneData[bid].CrossSectionList[eid].RefOuterID-2] = 0;

BoneData[bid].CrossSectionList.Delete(eid,1);

//watje 9-7-99  198721 
Reevaluate(TRUE);

}









static int outputIDs[] = {IDC_RADIO1,IDC_RADIO2,IDC_RADIO3,IDC_RADIO4,IDC_RADIO5,IDC_RADIO6};

//
// Parameters


#define PARAMDESC_LENGTH	18

static ParamUIDesc descParam[18] = {
	// Effect
	ParamUIDesc(
		PB_EFFECT,
		EDITTYPE_FLOAT,
		IDC_EFFECT,IDC_EFFECTSPIN,
		0.0f,1.0f,
		SPIN_AUTOSCALE),


	// Lock Bone
   ParamUIDesc(PB_LOCK_BONE,TYPE_SINGLECHEKBOX,IDC_LOCK_BONE_CHECK),

   // Absolute Influence
   ParamUIDesc(PB_ABSOLUTE_INFLUENCE,TYPE_SINGLECHEKBOX,IDC_ABSOLUTE_INFLUENCE_CHECK),

   // Filter Vertices
   ParamUIDesc(PB_FILTER_VERTICES,TYPE_SINGLECHEKBOX,IDC_FILTER_VERTICES_CHECK),

	// Filter Bones
   ParamUIDesc(PB_FILTER_BONES,TYPE_SINGLECHEKBOX,IDC_FILTER_BONES_CHECK),

	// Filter Envelopes
   ParamUIDesc(PB_FILTER_ENVELOPES,TYPE_SINGLECHEKBOX,IDC_FILTER_ENVELOPES_CHECK),

   // draw Envelopes
   ParamUIDesc(PB_DRAW_ENVELOPES,TYPE_SINGLECHEKBOX,IDC_DRAWALL_ENVELOPES_CHECK),

   // draw Vertices
   ParamUIDesc(PB_DRAW_VERTICES,TYPE_SINGLECHEKBOX,IDC_DRAW_VERTICES_CHECK),

	// Rereference frame
	ParamUIDesc(
		PB_REF_FRAME,
		EDITTYPE_INT,
		IDC_REF_FRAME,IDC_REF_FRAME_SPIN,
		-BIGFLOAT,BIGFLOAT,
		SPIN_AUTOSCALE),


	// Radius
	ParamUIDesc(
		PB_RADIUS,
		EDITTYPE_FLOAT,
		IDC_SRADIUS,IDC_SRADIUSSPIN,
		0.0f,5000.0f,
		SPIN_AUTOSCALE),

   // Project Through
   ParamUIDesc(PB_PROJECT_THROUGH,TYPE_SINGLECHEKBOX,IDC_PROJECT_THROUGH_CHECK),
   // Brush Falloff
   ParamUIDesc(PB_FALLOFF,TYPE_SINGLECHEKBOX,IDC_FALLOFF_CHECK),
//curve falloff
	ParamUIDesc(PB_BONE_FALLOFF,TYPE_RADIO,outputIDs,6),

// Feather
	ParamUIDesc(
		PB_FEATHER,
		EDITTYPE_FLOAT,
		IDC_FEATHER,IDC_FEATHERSPIN,
		0.0f,1.0f,
		SPIN_AUTOSCALE),

//Draw Envelope											
   ParamUIDesc(PB_DRAW_BONE_ENVELOPE,TYPE_SINGLECHEKBOX,IDC_DISPLAY_ENVELOPE_CHECK),

// Envelope Radius
	ParamUIDesc(
		PB_ERADIUS,
		EDITTYPE_FLOAT,
		IDC_ERADIUS,IDC_ERADIUSSPIN,
		0.0f,1000000.0f,
		SPIN_AUTOSCALE),

//Always deform											
   ParamUIDesc(PB_ALWAYS_DEFORM,TYPE_SINGLECHEKBOX,IDC_ALWAYSDEFORM_CHECK),


// Paint str
	ParamUIDesc(
		PB_PAINTSTR,
		EDITTYPE_FLOAT,
		IDC_PAINT_STR2,IDC_PAINT_STR_SPIN2,
		0.0f,1.0f,
		SPIN_AUTOSCALE)


	};

static ParamBlockDescID descVer0[17] = {
	{ TYPE_FLOAT, NULL, FALSE,  0 },		// Effect	
	{ TYPE_INT,   NULL, FALSE, 1 },		// Lock Bone
	{ TYPE_INT,   NULL, FALSE, 2 },		// Absolute Influence
	{ TYPE_INT,   NULL, FALSE, 3 },		// Filter Vertices
	{ TYPE_INT,   NULL, FALSE, 4 },		// Filter Bones
	{ TYPE_INT,   NULL, FALSE, 5 },		// Filter Envelopes
	{ TYPE_INT,   NULL, FALSE, 6 },		// Draw All envelopes
	{ TYPE_INT,   NULL, FALSE, 7 },		// Draw vertice
	{ TYPE_INT,   NULL, FALSE, 8 },		// Ref Frame
	{ TYPE_FLOAT, NULL, FALSE,  9},		// Radius	
	{ TYPE_INT, NULL, FALSE,  10},		// Project through	
	{ TYPE_INT, NULL, FALSE,  11},		// falloff	
	{ TYPE_INT, NULL, FALSE,  12},		// bone falloff	
	{ TYPE_FLOAT, NULL, FALSE,  13},		// feather	
	{ TYPE_INT, NULL, FALSE,  14},		// Draw bone envelope	
	{ TYPE_FLOAT, NULL, FALSE,  15},	// envelope raduis	
	{ TYPE_INT, NULL, FALSE,  16}		// always deform
	};


static ParamBlockDescID descVer1[18] = {
	{ TYPE_FLOAT, NULL, FALSE,  0 },		// Effect	
	{ TYPE_INT,   NULL, FALSE, 1 },		// Lock Bone
	{ TYPE_INT,   NULL, FALSE, 2 },		// Absolute Influence
	{ TYPE_INT,   NULL, FALSE, 3 },		// Filter Vertices
	{ TYPE_INT,   NULL, FALSE, 4 },		// Filter Bones
	{ TYPE_INT,   NULL, FALSE, 5 },		// Filter Envelopes
	{ TYPE_INT,   NULL, FALSE, 6 },		// Draw All envelopes
	{ TYPE_INT,   NULL, FALSE, 7 },		// Draw vertice
	{ TYPE_INT,   NULL, FALSE, 8 },		// Ref Frame
	{ TYPE_FLOAT, NULL, FALSE,  9},		// Radius	
	{ TYPE_INT, NULL, FALSE,  10},		// Project through	
	{ TYPE_INT, NULL, FALSE,  11},		// falloff	
	{ TYPE_INT, NULL, FALSE,  12},		// bone falloff	
	{ TYPE_FLOAT, NULL, FALSE,  13},		// feather	
	{ TYPE_INT, NULL, FALSE,  14},		// Draw bone envelope	
	{ TYPE_FLOAT, NULL, FALSE,  15},	// envelope raduis	
	{ TYPE_INT, NULL, FALSE,  16},		// always deform
	{ TYPE_FLOAT, NULL, FALSE,  17}	// paint str	
	};

#define PBLOCK_LENGTH	18

#define CURRENT_VERSION	1


static ParamVersionDesc versions[] = {
	ParamVersionDesc(descVer0,17,0)
	};
#define NUM_OLDVERSIONS	1

// Current version
static ParamVersionDesc curVersion(descVer1,PBLOCK_LENGTH,CURRENT_VERSION);


//--- Affect region mod methods -------------------------------

BonesDefMod::BonesDefMod() 
	{

	MakeRefByID(
		FOREVER, PBLOCK_PARAM_REF, 
		CreateParameterBlock(
			descVer1, PBLOCK_LENGTH, CURRENT_VERSION));


	
	pblock_param->SetValue(PB_EFFECT,0,0.0f);
	pblock_param->SetValue(PB_ERADIUS,0,10.0f);

	pblock_param->SetValue(PB_FILTER_VERTICES,0,0);
	pblock_param->SetValue(PB_FILTER_BONES,0,1);
	pblock_param->SetValue(PB_FILTER_ENVELOPES,0,1);



	pblock_param->SetValue(PB_DRAW_ENVELOPES,0,0);
	pblock_param->SetValue(PB_DRAW_VERTICES,0,1);



	pblock_param->SetValue(PB_PROJECT_THROUGH,0,1);
	pblock_param->SetValue(PB_FALLOFF,0,1);
	pblock_param->SetValue(PB_FEATHER,0,0.7f);
	pblock_param->SetValue(PB_RADIUS,0,24.0f);

	pblock_param->SetValue(PB_PAINTSTR,0,0.1f);

	pblock_param->SetValue(PB_REF_FRAME,0,0);
	pblock_param->SetValue(PB_ALWAYS_DEFORM,0,1);

	RefTable.ZeroCount();

//	NodeCount = 0;
//	effect = -1.0f;
	BoneData.New();
	//watje 9-7-99  198721 
	Reevaluate(FALSE);
	reset = FALSE;
	BoneMoved = FALSE;
	p1Temp = NULL;
	Point3 p(0.0f,0.0f,0.0f);
	MakeRefByID(FOREVER,POINT1_REF,NewDefaultPoint3Controller()); 
	p1Temp->SetValue(0,p,TRUE,CTRL_ABSOLUTE);
	ModeEdit = 0;
	ModeBoneIndex = -1;
	ModeBoneEndPoint  = -1;
	ModeBoneEnvelopeIndex = -1;
	FilterVertices = 0;
	FilterBones = 0;
	FilterEnvelopes = 0;
	DrawEnvelopes = 0;
	paintStr  = 0.1f;

//	VertexData = NULL;
//	VertexDataCount = 0;
//	CurrentCachePiece = -1;
//	bmd = NULL;
	cacheValid = FALSE;
	unlockVerts = FALSE;
	OldVertexDataCount = 0;
	unlockBone = FALSE;

	painting = FALSE;
	inPaint = FALSE;
	reloadSplines = FALSE;

	splineChanged = FALSE;
	forceRecomuteBaseNode = FALSE;
	updateP = FALSE;

	bindNode = NULL;
	initialXRefTM.IdentityMatrix();
	xRefTM.IdentityMatrix();

	}


BonesDefMod::~BonesDefMod()
	{
	DeleteAllRefsFromMe();
	p1Temp = NULL;


//	VertexData.ZeroCount();

	for (int i=0;i<BoneData.Count();i++)
        BoneData[i].CrossSectionList.ZeroCount();

	BoneData.New();



	}


void BonesDefMod::BeginEditParams(
		IObjParam  *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
	editMod  = this;

	// Add our sub object type
	TSTR type1(GetString(IDS_RB_BONESDEFPOINTS));
	const TCHAR *ptype[] = {type1};
	ip->RegisterSubObjectTypes(ptype, 1);

	// Create sub object editing modes.
	moveMode    = new MoveModBoxCMode(this,ip);
	CrossSectionMode    = new CreateCrossSectionMode(this,ip);
	PaintMode    = new CreatePaintMode(this,ip);
	
	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

//	bonesDefDesc.BeginEditParams(ip, this, flags, prev);
//	bones_param_blk.SetUserDlgProc(new MapDlgProc(this));
//	bones_paint_blk.SetUserDlgProc(new PaintDlgProc(this));
//	bones_filter_blk.SetUserDlgProc(new FilterDlgProc(this));
//	bones_advance_blk.SetUserDlgProc(new AdvanceDlgProc(this));

	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGTH,
		pblock_param,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_BONESDEFPARAM),
		GetString(IDS_PW_PARAMETERS),
		0);	

	pmapParam->SetUserDlgProc(new MapDlgProc(this));


	}

void BonesDefMod::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	this->ip = NULL;
	editMod  = NULL;

	TimeValue t = ip->GetTime();

	// NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
	ClearAFlag(A_MOD_BEING_EDITED);

	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);

	ip->DeleteMode(moveMode);	
	if (moveMode) delete moveMode;
	moveMode = NULL;	

	ip->DeleteMode(CrossSectionMode);	
	if (CrossSectionMode) delete CrossSectionMode;
	CrossSectionMode = NULL;	

	ip->DeleteMode(PaintMode);	
	if (PaintMode) delete PaintMode;
	PaintMode = NULL;	


	iCrossSectionButton = NULL;
	ReleaseICustButton(iCrossSectionButton);
	iCrossSectionButton = NULL;

	iLock = NULL;
	ReleaseICustButton(iLock);
	iLock = NULL;

	iAbsolute = NULL;
	ReleaseICustButton(iAbsolute);
	iAbsolute = NULL;

	iEnvelope = NULL;
	ReleaseICustButton(iEnvelope);
	iEnvelope = NULL;

	iFalloff = NULL;
	ReleaseICustButton(iFalloff);
	iFalloff = NULL;

	iCopy = NULL;
	ReleaseICustButton(iCopy);
	iCopy = NULL;

	iPaste = NULL;
	ReleaseICustButton(iPaste);
	iPaste = NULL;


	iPaintButton = NULL;
	ReleaseICustButton(iPaintButton);
	iPaintButton = NULL;



	ip->GetRightClickMenuManager()->Unregister(&rMenu);


	ReleaseICustToolbar(iParams);
	iParams = NULL;


	DestroyCPParamMap(pmapParam);
	}

RefTargetHandle BonesDefMod::Clone(RemapDir& remap)
	{
	BonesDefMod *mod = new BonesDefMod();
	mod->ReplaceReference(PBLOCK_PARAM_REF,pblock_param->Clone(remap));
	mod->ReplaceReference(POINT1_REF,p1Temp->Clone(remap));

//copy controls
	mod->RefTable = RefTable;

	for (int i = 0; i<BoneData.Count(); i++)
		{
		BoneDataClass b = BoneData[i];
		b.Node = NULL;
		b.EndPoint1Control= NULL;
		b.EndPoint2Control = NULL;
		for (int j = 0; j < b.CrossSectionList.Count(); j++)
			{
			b.CrossSectionList[j].InnerControl = NULL;
			b.CrossSectionList[j].OuterControl = NULL;
			}
		mod->BoneData.Append(b);
		}


	for (i=0;i < BoneData.Count();i++)
		{
		if (BoneData[i].EndPoint1Control)
			mod->ReplaceReference(BoneData[i].RefEndPt1ID,BoneData[i].EndPoint1Control->Clone(remap));
		if (BoneData[i].EndPoint2Control)
			mod->ReplaceReference(BoneData[i].RefEndPt2ID,BoneData[i].EndPoint2Control->Clone(remap));
		if (BoneData[i].Node)
			mod->ReplaceReference(BoneData[i].BoneRefID,BoneData[i].Node);

		for (int j=0;j < BoneData[i].CrossSectionList.Count();j++)
			{
			if (BoneData[i].CrossSectionList[j].InnerControl)
				mod->ReplaceReference(BoneData[i].CrossSectionList[j].RefInnerID,BoneData[i].CrossSectionList[j].InnerControl->Clone(remap));
			if (BoneData[i].CrossSectionList[j].OuterControl)
				mod->ReplaceReference(BoneData[i].CrossSectionList[j].RefOuterID,BoneData[i].CrossSectionList[j].OuterControl->Clone(remap));
			}

		}


	mod->forceRecomuteBaseNode = TRUE;





//	for (int i = 0; i < ; i++)
//		mod->ReplaceReference(i,);
//copy boneData


	return mod;
	}



void BonesDefMod::SetVertex(BoneModData *bmd,int vertID, int BoneID, float amount)

{
//Tab<int> vsel;

if (BoneData[BoneID].flags & BONE_LOCK_FLAG)
	return;

ObjectState os;
ShapeObject *pathOb = NULL;

if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
	{
	os = BoneData[BoneID].Node->EvalWorldState(ip->GetTime());
	pathOb = (ShapeObject*)os.obj;
	}

/*
int selcount = bmd->selected.GetSize();

for (int i = 0 ; i <bmd->VertexDataCount;i++)
	{
	if (bmd->selected[i]) vsel.Append(1,&i,1);
	}
*/

float effect,originaleffect;
//for ( i = 0; i < vsel.Count();i++)
	{
	int found = 0;

//	int k = vsel[i];
	int k = vertID;
	for (int j =0; j<bmd->VertexData[k]->d.Count();j++)
		{
		if ( (bmd->VertexData[k]->d[j].Bones == BoneID)&& (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG)))

			{
			originaleffect = bmd->VertexData[k]->d[j].Influences;
			bmd->VertexData[k]->d[j].Influences = amount;
			bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;
			found = 1;
			effect = bmd->VertexData[k]->d[j].Influences;
			j = bmd->VertexData[k]->d.Count();

			}
		}

	if ((found == 0) && (amount > 0.0f))
		{

		VertexInfluenceListClass td;
		td.Bones = BoneID;
		td.Influences = amount;
		td.normalizedInfluences = -1.0f;
//check if spline if so add approriate spline data info also
// find closest spline
		
		if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
			{
			Interval valid;
			Matrix3 ntm = BoneData[BoneID].Node->GetObjTMBeforeWSM(RefFrame,&valid);
			ntm = bmd->BaseTM * Inverse(ntm);

			float garbage = SplineToPoint(bmd->VertexData[k]->LocalPos,
//										pathOb,
										&BoneData[BoneID].referenceSpline,
			                            td.u,
										td.OPoints,td.Tangents,
										td.SubCurveIds,td.SubSegIds,
										ntm);
//										BoneData[BoneID].tm);
			}


		bmd->VertexData[k]->d.Append(1,&td,1);
		effect = amount;
		originaleffect = 0.0f;
		found = 1;
		}

	if (found == 1)
		{
		int bc = bmd->VertexData[k]->d.Count();

//remove 0 influence bones otherwise they skew the reweigthing
		for (j=0;j<bmd->VertexData[k]->d.Count();j++)
			{
			if (bmd->VertexData[k]->d[j].Influences==0.0f)
				{
				bmd->VertexData[k]->d.Delete(j,1);
				j--;
				}
			}


//rebalance rest
		float remainder = 1.0f - effect;
		originaleffect = 1.0f - originaleffect;
		if (bmd->VertexData[k]->d.Count() > 1)
			{
			for (j=0;j<bmd->VertexData[k]->d.Count();j++)
				{
	
				if (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG))
					{
					if (bmd->VertexData[k]->d[j].Bones!=BoneID)
						{
						if (originaleffect == 0.0f)
							 bmd->VertexData[k]->d[j].Influences = remainder/(bmd->VertexData[k]->d.Count()-1.0f);
						else 
							bmd->VertexData[k]->d[j].Influences = bmd->VertexData[k]->d[j].Influences/originaleffect * remainder;
						bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;

						}
					}
				}
			}

		bmd->VertexData[k]->modified = TRUE;

		}
	}
}



void BonesDefMod::SetVertices(BoneModData *bmd,int vertID, Tab<int> BoneIDList, Tab<float> amountList)

{
//Tab<int> vsel;


ObjectState os;
ShapeObject *pathOb = NULL;



float effect,originaleffect;
int k = vertID;

for (int i = 0; i < amountList.Count();i++)
	{
	int found = 0;

	float amount = amountList[i];
	int BoneID = BoneIDList[i];
	if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
		{
		os = BoneData[BoneID].Node->EvalWorldState(ip->GetTime());
		pathOb = (ShapeObject*)os.obj;
		}

	for (int j =0; j<bmd->VertexData[k]->d.Count();j++)
		{
		if ( (bmd->VertexData[k]->d[j].Bones == BoneID)&& (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG)))

			{
			originaleffect = bmd->VertexData[k]->d[j].Influences;
			bmd->VertexData[k]->d[j].Influences = amount;
			bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;
			found = 1;
			effect = bmd->VertexData[k]->d[j].Influences;
			j = bmd->VertexData[k]->d.Count();

			}
		}

	if ((found == 0) && (amount > 0.0f))
		{

		VertexInfluenceListClass td;
		td.Bones = BoneID;
		td.Influences = amount;
		td.normalizedInfluences = -1.0f;
//check if spline if so add approriate spline data info also
// find closest spline
		
		if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
			{
			Interval valid;
			Matrix3 ntm = BoneData[BoneID].Node->GetObjTMBeforeWSM(RefFrame,&valid);
			ntm = bmd->BaseTM * Inverse(ntm);

			float garbage = SplineToPoint(bmd->VertexData[k]->LocalPos,
//										pathOb,
										&BoneData[BoneID].referenceSpline,
			                            td.u,
										td.OPoints,td.Tangents,
										td.SubCurveIds,td.SubSegIds,
										ntm);
//										BoneData[BoneID].tm);
			}


		bmd->VertexData[k]->d.Append(1,&td,1);
		effect = amount;
		originaleffect = 0.0f;
		found = 1;
		}

	if (found == 1)
		{
		int bc = bmd->VertexData[k]->d.Count();

//remove 0 influence bones otherwise they skew the reweigthing
		for (j=0;j<bmd->VertexData[k]->d.Count();j++)
			{
			if (bmd->VertexData[k]->d[j].Influences==0.0f)
				{
				bmd->VertexData[k]->d.Delete(j,1);
				j--;
				}
			}


//rebalance rest

		}
	}
bmd->VertexData[k]->modified = TRUE;
if (bmd->VertexData[k]->d.Count() > 1)
	{
	float totalSum = 0.0f;
	for (int j=0;j<bmd->VertexData[k]->d.Count();j++)
		{
		if (bmd->VertexData[k]->d[j].Influences==0.0f)
			{
			bmd->VertexData[k]->d.Delete(j,1);
			j--;
			}
		}

	for (j=0;j<bmd->VertexData[k]->d.Count();j++)
		totalSum += bmd->VertexData[k]->d[j].Influences;
	for (j=0;j<bmd->VertexData[k]->d.Count();j++)
		{
		 bmd->VertexData[k]->d[j].Influences = bmd->VertexData[k]->d[j].Influences/totalSum;
		}
	}


}



  
void BonesDefMod::SetSelectedVertices(BoneModData *bmd, int BoneID, float amount)

{
Tab<int> vsel;

if (BoneData[BoneID].flags & BONE_LOCK_FLAG)
	return;

ObjectState os;
ShapeObject *pathOb = NULL;

if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
	{
	os = BoneData[BoneID].Node->EvalWorldState(ip->GetTime());
	pathOb = (ShapeObject*)os.obj;
	}


int selcount = bmd->selected.GetSize();

for (int i = 0 ; i <bmd->VertexDataCount;i++)
	{
	if (bmd->selected[i]) vsel.Append(1,&i,1);
	}

float effect,originaleffect;
for ( i = 0; i < vsel.Count();i++)
	{
	int found = 0;

	int k = vsel[i];
	for (int j =0; j<bmd->VertexData[k]->d.Count();j++)
		{
		if ( (bmd->VertexData[k]->d[j].Bones == BoneID)&& (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG)))

			{
			originaleffect = bmd->VertexData[k]->d[j].Influences;
			bmd->VertexData[k]->d[j].Influences = amount;
			bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;
			found = 1;
			effect = bmd->VertexData[k]->d[j].Influences;
			j = bmd->VertexData[k]->d.Count();

			}
		}

	if ((found == 0) && (amount > 0.0f))
		{

		VertexInfluenceListClass td;
		td.Bones = BoneID;
		td.Influences = amount;
		td.normalizedInfluences = -1.0f;
//check if spline if so add approriate spline data info also
// find closest spline
		
		if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
			{
			Interval valid;
			Matrix3 ntm = BoneData[BoneID].Node->GetObjTMBeforeWSM(RefFrame,&valid);
			ntm = bmd->BaseTM * Inverse(ntm);

			float garbage = SplineToPoint(bmd->VertexData[k]->LocalPos,
//										pathOb,
										&BoneData[BoneID].referenceSpline,
			                            td.u,
										td.OPoints,td.Tangents,
										td.SubCurveIds,td.SubSegIds,
										ntm);
//										BoneData[BoneID].tm);
			}


		bmd->VertexData[k]->d.Append(1,&td,1);
		effect = amount;
		originaleffect = 0.0f;
		found = 1;
		}

	if (found == 1)
		{
		int bc = bmd->VertexData[k]->d.Count();

//remove 0 influence bones otherwise they skew the reweigthing
		for (j=0;j<bmd->VertexData[k]->d.Count();j++)
			{
			if ((bmd->VertexData[k]->d[j].Influences==0.0f) && (bmd->VertexData[k]->d[j].Bones==BoneID))
//			if (bmd->VertexData[k]->d[j].Influences==0.0f)
				{
				bmd->VertexData[k]->d.Delete(j,1);
				j--;
				}
			}



//rebalance rest
		float remainder = 1.0f - effect;
		originaleffect = 1.0f - originaleffect;
		if (bmd->VertexData[k]->d.Count() > 1)
			{
			for (j=0;j<bmd->VertexData[k]->d.Count();j++)
				{
	
				if (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG))
					{
					if (bmd->VertexData[k]->d[j].Bones!=BoneID)
						{
//						if (originaleffect == 0.0f)
							 bmd->VertexData[k]->d[j].Influences = remainder/(bmd->VertexData[k]->d.Count()-1.0f);
//						else 
//							bmd->VertexData[k]->d[j].Influences = bmd->VertexData[k]->d[j].Influences/originaleffect * remainder;
						bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;

						}
					}
				}
			}

		bmd->VertexData[k]->modified = TRUE;

		}
	}
}


void BonesDefMod::IncrementVertices(BoneModData *bmd, int BoneID, Tab<float> amount, int flip )

{

ObjectState os;
ShapeObject *pathOb = NULL;


if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
	{
	os = BoneData[BoneID].Node->EvalWorldState(ip->GetTime());
	pathOb = (ShapeObject*)os.obj;
	}

float effect,originaleffect;
int found;
float val;
for ( int i = 0; i < amount.Count();i++)
	{
	if (amount[i] != -10.0f)
		{
		found = 0;
		val = amount[i];
		for (int j =0; j<bmd->VertexData[i]->d.Count();j++)
			{
			if  (bmd->VertexData[i]->d[j].Bones == BoneID) 
				{
				originaleffect = bmd->VertexData[i]->d[j].Influences;
				if (flip)
					{
					bmd->VertexData[i]->d[j].Influences -= amount[i];
					if (bmd->VertexData[i]->d[j].Influences < 0.0f ) bmd->VertexData[i]->d[j].Influences = 0.0f;
					bmd->VertexData[i]->d[j].normalizedInfluences = -1.0f;
					}
				else
					{
					bmd->VertexData[i]->d[j].Influences += amount[i];
					if (bmd->VertexData[i]->d[j].Influences > 1.0f ) bmd->VertexData[i]->d[j].Influences = 1.0f;
					bmd->VertexData[i]->d[j].normalizedInfluences = -1.0f;
					}

				found = 1;
				effect = bmd->VertexData[i]->d[j].Influences;
				j = bmd->VertexData[i]->d.Count();

				}
			}

		if ((!found) && (amount[i] > 0.0f) && !(flip) )
			{
			VertexInfluenceListClass td;
			td.Bones = BoneID;
			td.Influences = amount[i];
			td.normalizedInfluences = -1.0f;
			if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
				{
				Interval valid;
				Matrix3 ntm = BoneData[BoneID].Node->GetObjTMBeforeWSM(RefFrame,&valid);
				ntm = bmd->BaseTM * Inverse(ntm);

				float garbage = SplineToPoint(bmd->VertexData[i]->LocalPos,
//										pathOb,
										&BoneData[BoneID].referenceSpline,
			                            td.u,
										td.OPoints,td.Tangents,
										td.SubCurveIds,td.SubSegIds,
										ntm);
				}

			bmd->VertexData[i]->d.Append(1,&td,1);
			effect = amount[i];
			originaleffect = 0.0f;
			found = 1;
			}

		if (found==1)
			{

//remove 0 influence bones otherwise they skew the reweigthing
/*	for (j=0;j<bmd->VertexData[i]->d.Count();j++)
				{
				if (bmd->VertexData[i]->d[j].Influences==0.0f)
					{
					bmd->VertexData[i]->d.Delete(j,1);
					}
				}
*/
			float remainder = 1.0f - effect;
			originaleffect = 1.0f - originaleffect;
			if (bmd->VertexData[i]->d.Count() > 1)
				{
				for (j=0;j<bmd->VertexData[i]->d.Count();j++)
					{
					if (bmd->VertexData[i]->d[j].Bones!=BoneID)
						{
//				if (originaleffect == 0.0f)
						 bmd->VertexData[i]->d[j].Influences = remainder/(bmd->VertexData[i]->d.Count()-1.0f);
//						else	 
//							bmd->VertexData[i]->d[j].Influences = bmd->VertexData[i]->d[j].Influences/originaleffect * remainder;
						bmd->VertexData[i]->d[j].normalizedInfluences = -1.0f;
						}
					}
				}

			bmd->VertexData[i]->modified = TRUE;
			}

		}
	}

/*
ObjectState os;
ShapeObject *pathOb = NULL;

if (BoneData[BoneID].flags & BONE_LOCK_FLAG)
	return;

if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
	{
	os = BoneData[BoneID].Node->EvalWorldState(ip->GetTime());
	pathOb = (ShapeObject*)os.obj;
	}

Tab<int> vsel;
for (int i = 0 ; i <bmd->VertexDataCount;i++)
	{
	if (bmd->selected[i]) vsel.Append(1,&i,1);
	}
float effect,originaleffect;
for ( i = 0; i < vsel.Count();i++)
	{
	int found = 0;

	int k = vsel[i];
	for (int j =0; j<bmd->VertexData[k]->d.Count();j++)
		{
		if ( (bmd->VertexData[k]->d[j].Bones == BoneID) && (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG)))
			{
			originaleffect = bmd->VertexData[k]->d[j].Influences;
			if (flip)
				{
				if ((1.0f - amount[i]) < bmd->VertexData[k]->d[j].Influences)
					{
					bmd->VertexData[k]->d[j].Influences = (1.0f-amount[i]);
					bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;
					}
				}
			else
				{
				if (amount[i] > bmd->VertexData[k]->d[j].Influences)
					{
					bmd->VertexData[k]->d[j].Influences = amount[i];
					bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;
					}
				}

			found = 1;
			effect = bmd->VertexData[k]->d[j].Influences;
			j = bmd->VertexData[k]->d.Count();

			}
		}

	if (flip)
		{
		}
	else
		{
		if ((!found) && (amount[i] > 0.0f))
			{

			VertexInfluenceListClass td;
			td.Bones = BoneID;
			td.Influences = amount[i];
			td.normalizedInfluences = -1.0f;
			if (BoneData[BoneID].flags & BONE_SPLINE_FLAG)
				{
				Interval valid;
				Matrix3 ntm = BoneData[BoneID].Node->GetObjTMBeforeWSM(RefFrame,&valid);
				ntm = bmd->BaseTM * Inverse(ntm);

				float garbage = SplineToPoint(bmd->VertexData[k]->LocalPos,pathOb,
			                            td.u,
										td.OPoints,td.Tangents,
										td.SubCurveIds,td.SubSegIds,
										ntm);
				}


			bmd->VertexData[k]->d.Append(1,&td,1);
			effect = amount[i];
			originaleffect = 0.0f;
			found = 1;
			}
		}

	if (found==1)
		{

//remove 0 influence bones otherwise they skew the reweigthing
		for (j=0;j<bmd->VertexData[k]->d.Count();j++)
			{
			if (bmd->VertexData[k]->d[j].Influences==0.0f)
				{
				bmd->VertexData[k]->d.Delete(j,1);
				}
			}

		float remainder = 1.0f - effect;
		originaleffect = 1.0f - originaleffect;
		for (j=0;j<bmd->VertexData[k]->d.Count();j++)
			{
			if (!(BoneData[bmd->VertexData[k]->d[j].Bones].flags & BONE_LOCK_FLAG))
				{
				if (bmd->VertexData[k]->d[j].Bones!=BoneID)
					{
					if (originaleffect == 0.0f)
						 bmd->VertexData[k]->d[j].Influences = remainder/(bmd->VertexData[k]->d.Count()-1.0f);
					else 
						bmd->VertexData[k]->d[j].Influences = bmd->VertexData[k]->d[j].Influences/originaleffect * remainder;
					bmd->VertexData[k]->d[j].normalizedInfluences = -1.0f;
					}
				}
			}

		bmd->VertexData[k]->modified = TRUE;
		}

	}
*/
}


class NullView: public View {
	public:
		Point2 ViewToScreen(Point3 p) { return Point2(p.x,p.y); }
		NullView() { worldToView.IdentityMatrix(); screenW=640.0f; screenH = 480.0f; }
	};


//void BonesDefMod::BuildEnvelopes(INode *bnode, INode *mnode, Point3 l1, Point3 l2, float &el1, float &el2)
void BonesDefMod::BuildEnvelopes(INode *bnode, Object *obj, Point3 l1, Point3 l2, float &el1, float &el2)

{

//get mesh
//Object *obj = mnode->EvalWorldState(RefFrame).obj;

//ObjectState os = mnode->EvalWorldState(RefFrame);
//if (os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) return;
if (obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) return;

//BOOL needDel;
//NullView nullView;
//Mesh *mesh = ((GeomObject*)os.obj)->GetRenderMesh(RefFrame,mnode,nullView,needDel);
el1 = BIGFLOAT;
el2 = BIGFLOAT;
//if (!mesh) return;


//tranform x,y,z into object space



Matrix3 tm = bnode->GetObjectTM(RefFrame);

//tm = tm * Inverse(BaseTM);

l1 = l1 * tm;
l2 = l2 * tm;

Point3 p_edge[4];
GetCrossSectionLocal(l1, Normalize(l2-l1), 1.0f, p_edge);

///intersect axis  rays with mesh;
Ray ray;
Point3 norm;
float at;

float dist = 0.0f;
int ct = 0;
	
// See if we hit the object
for (int i=0; i < 4; i++)
	{
	ray.p   = l1 + Normalize(p_edge[i]-l1) * 50000.0f;
	ray.dir = -Normalize(p_edge[i]-l1);
	BOOL hit = FALSE;
	while  (obj->IntersectRay(RefFrame,ray,at,norm)) 
		{
		Point3 tp = ray.dir * (at+0.01f) ;
		ray.p = ray.p + tp;
		hit = TRUE;
		}
	if (hit)
		{
		dist += Length(ray.p - l1);
		ct++;
		}
	}

//if (ct == 0)
//el1 = Length(l2-l1) *.1f;
if (ct!=0) el1 = dist/(float) ct;


dist = 0.0f;
int ct2 = 0;
GetCrossSectionLocal(l2, Normalize(l2-l1), 1.0f, p_edge);
ct2 = 0;	
// See if we hit the object
for (i=0; i < 4; i++)
	{
	ray.p   = l2 + Normalize(p_edge[i]-l2) * 50000.0f;
	ray.dir = -Normalize(p_edge[i]-l2);
	BOOL hit = FALSE;
	while  (obj->IntersectRay(RefFrame,ray,at,norm)) 
		{
		Point3 tp = ray.dir * (at+0.01f) ;
		ray.p = ray.p + tp;
		hit = TRUE;
		}
	if (hit)
		{
		dist += Length(ray.p - l2);
		ct2++;
		}
	}

//if (ct == 0)
//el2 = Length(l2-l1) *.1f;
if (ct2!=0) el2 = dist/(float) ct2;

if ((ct==0) && (ct2!=0))
	el1 = el2;
else if ((ct!=0) && (ct2==0))
	el2 = el1;
else if ((ct==0) && (ct2==0))
	{
	el1 = Length(l2-l1) *.5f;
	el2 = el1;
	}



}


void BonesDefMod::BuildMajorAxis(INode *node, Point3 &l1, Point3 &l2)
	{
//get object state
	ObjectState os;
	
	os = node->EvalWorldState(RefFrame);

//get bounding box
	Box3 bb;
//get longest axis
	os.obj->GetDeformBBox(0,bb);
	
	float dx,dy,dz,axislength;
	dx = bb.pmax.x - bb.pmin.x;
	dy = bb.pmax.y - bb.pmin.y;
	dz = bb.pmax.z - bb.pmin.z;
	int axis;
	axislength = dx;
	axis = 0;

	
	if (dy > axislength)
		{
		axis = 1;
		axislength = dy;				
		}
	if (dz > axislength)
		{
		axis = 2;
		axislength = dz;				
		}

	if (axis ==0)
		{
		l1.x = bb.pmax.x;
		l2.x = bb.pmin.x;
		}

	else if (axis ==1)
		{
		l1.y = bb.pmax.y;
		l2.y = bb.pmin.y;
		}

	else if (axis ==2)
		{
		l1.z = bb.pmax.z;
		l2.z = bb.pmin.z;
		}

	}

Matrix3 BonesDefMod::CompMatrix(TimeValue t,INode *inode,ModContext *mc)
	{
	Interval iv;
	Matrix3 tm(1);	
//	if (mc && mc->tm) 
//		tm = Inverse(*(mc->tm));
	if (inode) 
//		tm = tm * inode->GetObjTMBeforeWSM(t,&iv);
		tm = tm * inode->GetObjectTM(t,&iv);
	return tm;
	}


void BonesDefMod::ClearVertexSelections(BoneModData *bmd)
{
for (int i = 0; i <bmd->selected.GetSize(); i++)
	{
	bmd->selected.Set(i,FALSE);
	}
EnableEffect(FALSE);
ip->RedrawViews(ip->GetTime());
}

void BonesDefMod::ClearBoneEndPointSelections()
{

for (int i = 0; i <BoneData.Count(); i++)
	{
	if (BoneData[i].Node != NULL)
		{
		BoneData[i].end1Selected = FALSE;
		BoneData[i].end2Selected = FALSE;
/*		bmd->sel[ct++] = FALSE;
		bmd->sel[ct++] = FALSE;
		for (int j = 0; j <BoneData[i].CrossSectionList.Count(); j++)
			ct += 8;
*/

		}
	}

ip->RedrawViews(ip->GetTime());
}
void BonesDefMod::ClearEnvelopeSelections()
{
for (int i = 0; i <BoneData.Count(); i++)
	{
	if (BoneData[i].Node != NULL)
		{
//		ct++;
//		ct++;
		for (int j = 0; j <BoneData[i].CrossSectionList.Count(); j++)
			{
			BoneData[i].CrossSectionList[j].innerSelected = FALSE;
			BoneData[i].CrossSectionList[j].outerSelected = FALSE;
//			for (int k = 0; k <8; k++)
//			sel[ct++] =FALSE;
			}

		}
	}
ModeBoneEnvelopeIndex = -1;
ip->RedrawViews(ip->GetTime());

}


void BonesDefMod::SelectFlexibleVerts(BoneModData *bmd)
{
for (int i =0;i<bmd->VertexDataCount;i++)
	{
	bmd->selected.Set(i,FALSE);
	for (int j =0; j <bmd->VertexData[i]->d.Count();j++)
		{
		if (bmd->VertexData[i]->d[j].Bones==ModeBoneIndex)
			{
			if (bmd->VertexData[i]->d[j].Influences != 1.0f)
				bmd->selected.Set(i,TRUE);
//				sel[i] = TRUE;
			}
		}
	}
NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
//ip->RedrawViews(mod->ip->GetTime());

}
void BonesDefMod::BuildFalloff(BoneModData *bmd)
{
for (int i =0;i<bmd->VertexDataCount;i++)
	{
	if (bmd->selected[i])
		{
		float f1 = 999999999.9f, f2 = 999999999.9f;
		BOOL found1 = FALSE,found2 = FALSE;
//find closest non selected red vert
		for (int j =0;j<bmd->VertexDataCount;j++)
			{
			bmd->VertexData[i]->modified = TRUE;
			if (!bmd->selected[j])
				{
				for (int k =0;k<bmd->VertexData[j]->d.Count();k++)
					{
					if ( (bmd->VertexData[j]->d[k].Bones == ModeBoneIndex) &&
						 (bmd->VertexData[j]->d[k].Influences == 1.0f) )
						{
						float dist = Length(bmd->VertexData[j]->LocalPos - bmd->VertexData[i]->LocalPos);
						if (dist < f1 ) 
							{
							f1 = dist;
							found1 = TRUE;
							}
						}
					}

				}
			}
//find closest non selected no influence vert
		for (j =0;j<bmd->VertexDataCount;j++)
			{
			BOOL tfound = FALSE;
			if (!bmd->selected[j])
				{

				for (int k =0;k<bmd->VertexData[j]->d.Count();k++)
					{
					if ( (bmd->VertexData[j]->d[k].Bones == ModeBoneIndex) &&
						 (bmd->VertexData[j]->d[k].Influences == 0.0f) )
						{
						float dist = Length(bmd->VertexData[j]->LocalPos - bmd->VertexData[i]->LocalPos);
						if (dist < f2 ) 
							{
							f2 = dist;
							found2 = TRUE;
							}
						}
					if ( (bmd->VertexData[j]->d[k].Bones == ModeBoneIndex) &&
						 (bmd->VertexData[j]->d[k].Influences != 0.0f) )
						 tfound = TRUE;

					}
				if (!tfound)
					{
					float dist = Length(bmd->VertexData[j]->LocalPos - bmd->VertexData[i]->LocalPos);
					if (dist < f2 ) 
						{
						f2 = dist;
						found2 = TRUE;
						}

					}

				}
			}
		if ((found1) && (found2))
			{
			float influ = f2/(f1+f2);
			ComputeFalloff(influ,BoneData[ModeBoneIndex].FalloffType);

			BOOL tfound = false;
			for (int k =0;k<bmd->VertexData[i]->d.Count();k++)
				{
				if (bmd->VertexData[i]->d[k].Bones == ModeBoneIndex)
					{
					tfound = TRUE;
					bmd->VertexData[i]->d[k].Influences = influ;
					bmd->VertexData[i]->d[k].normalizedInfluences = -1.0f;

					k = bmd->VertexData[i]->d.Count();
					}
				}
			if (!tfound)
				{

				VertexInfluenceListClass td;
				td.Bones = ModeBoneIndex;
				td.Influences = influ;
				td.normalizedInfluences = -1.0f;
				bmd->VertexData[i]->d.Append(1,&td,1);

				}
//now rebablance
			float remainder = 1.0f - influ;
			if (bmd->VertexData[i]->d.Count() > 1)
				{
				remainder = remainder /(bmd->VertexData[i]->d.Count()-1);
				for (k =0;k<bmd->VertexData[i]->d.Count();k++)
					{
					if (bmd->VertexData[i]->d[k].Bones != ModeBoneIndex)
						{
						bmd->VertexData[i]->d[k].Influences = remainder;
						bmd->VertexData[i]->d[k].normalizedInfluences = -1.0f;
						}
					}
				}
			else if (bmd->VertexData[i]->d.Count() == 1 )
				{
				if (BoneData[ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG) 
					{
					bmd->VertexData[i]->d[0].Influences = 1.0f;
					bmd->VertexData[i]->d[0].normalizedInfluences = 1.0f;
					}

				}



			}
		}
	}

NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);


}


void BonesDefMod::SyncSelections()
{
for (int i = 0; i < BoneData.Count();i++)
	{
	if (BoneData[i].Node != NULL)
		{
		BoneData[i].end1Selected = FALSE;
		BoneData[i].end2Selected = FALSE;

		for (int j=0;j<BoneData[i].CrossSectionList.Count();j++)
			{
			BoneData[i].CrossSectionList[j].innerSelected = FALSE;
			BoneData[i].CrossSectionList[j].outerSelected = FALSE;
			}

		}	
	}


if (ModeBoneIndex != -1)
	{
	if (ModeBoneEnvelopeIndex == 0)
		{
		BoneData[ModeBoneIndex].end1Selected = TRUE;
		}
	else if (ModeBoneEnvelopeIndex == 1)
		{
		BoneData[ModeBoneIndex].end2Selected = TRUE;
		}
	if (ModeBoneEnvelopeIndex != -1)
		{
		if (ModeBoneEnvelopeSubType < 4)
			BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].innerSelected = TRUE;
		else BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].outerSelected = TRUE;
		EnableRadius(TRUE);

		}
	else
		EnableRadius(FALSE);


	}
//update list box
if (ModeBoneIndex != -1)
	{
	int fsel = ConvertSelectedBoneToListID(ModeBoneIndex);

	SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_SETCURSEL ,fsel,0);

	}

}


int BonesDefMod::HitTest(
		TimeValue t, INode* inode, 
		int type, int crossing, int flags, 
		IPoint2 *p, ViewExp *vpt, ModContext* mc)
	{

	pblock_param->GetValue(PB_FILTER_VERTICES,t,FilterVertices,FOREVER);
	pblock_param->GetValue(PB_FILTER_BONES,t,FilterBones,FOREVER);
	pblock_param->GetValue(PB_FILTER_ENVELOPES,t,FilterEnvelopes,FOREVER);

	FilterVertices = !FilterVertices;
	FilterBones = !FilterBones;
	FilterEnvelopes = !FilterEnvelopes;


	ModeEdit = 0;


	GraphicsWindow *gw = vpt->getGW();
	Point3 pt;
	HitRegion hr;
	int savedLimits, res = 0;
	Matrix3 tm = CompMatrix(t,inode,mc);

	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);	
	gw->setTransform(tm);

// Hit test start point
//loop through points checking for selection
//get selected bone	
	int fsel;
	fsel = SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_GETCURSEL ,0,0);

	int ssel = ConvertSelectedListToBoneID(fsel);

	BoneModData *bmd = (BoneModData *) mc->localData;

	if (bmd==NULL) return 0;


	if ( (ssel>=0) && (ip && ip->GetSubObjectLevel() == 1) )

		{


		ObjectState os;

		os = inode->EvalWorldState(t);
//loop through points checking for selection and then marking for points
		if (FilterVertices == 0)
			{
			Interval iv;
			Matrix3 atm = inode->GetObjTMAfterWSM(t,&iv);
			Matrix3 ctm = inode->GetObjectTM(t,&iv);


			BOOL isWorldSpace = FALSE;

			if ((atm.IsIdentity()) && (ip->GetShowEndResult ()))
				isWorldSpace = TRUE;


			for (int i=0;i<bmd->VertexDataCount;i++)
				{
				if ((flags&HIT_SELONLY   &&  bmd->selected[i]) ||
					(flags&HIT_UNSELONLY && !bmd->selected[i]) ||
					!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 
					{

					Point3 pt;
					gw->clearHitCode();
					if (isWorldSpace)
						pt = os.obj->GetPoint(i) * Inverse(ctm);
					else pt = os.obj->GetPoint(i);
					gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
					gw->marker(&pt,POINT_MRKR);
					if (gw->checkHitCode()) {

						vpt->LogHit(inode, mc, gw->getHitDistance(), i, 
							new BoneHitDataClass(i,-1,-1,-1,-1)); 
						res = 1;
						}
					}
				}
			}
		int ct = bmd->VertexDataCount;
		for (int i =0;i<BoneData.Count();i++)
			{
			if (BoneData[i].Node != NULL)
				{
//add in envelopes start and end
				ObjectState os;
				ShapeObject *pathOb = NULL;


				if (FilterBones == 0)
					{

					Point3 pta,ptb;
//					GetEndPoints(bmd, t,pta, ptb, i);

					GetEndPointsLocal(bmd, t,pta, ptb, i);

//					Interval valid;
//					Matrix3 ntm = BoneData[i].Node->GetObjTMBeforeWSM(t,&valid);

//					pta = pta * ntm  * bmd->InverseBaseTM;
//					ptb = ptb * ntm  * bmd->InverseBaseTM;


/*					if ((flags&HIT_SELONLY   &&  sel[ct]) ||
						(flags&HIT_UNSELONLY && !sel[ct]) ||
						!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) */

					if ((flags&HIT_SELONLY   &&  BoneData[i].end1Selected) ||
						(flags&HIT_UNSELONLY && !BoneData[i].end1Selected) ||
						!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 

						{

						gw->clearHitCode();
						gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
						Point3 pt;
						Interval v;

						Point3 invA;
						invA = (ptb-pta) *.1f;
						
						pta += invA;

						gw->marker(&pta,POINT_MRKR);
						if (gw->checkHitCode()) {

							vpt->LogHit(inode, mc, gw->getHitDistance(), ct,  
								new BoneHitDataClass(-1,i,0,-1,-1)); 
							res = 1;
							}
						}
					if ((flags&HIT_SELONLY   &&  BoneData[i].end2Selected) ||
						(flags&HIT_UNSELONLY && !BoneData[i].end2Selected) ||
						!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 

/*					if ((flags&HIT_SELONLY   &&  sel[ct]) ||

						(flags&HIT_UNSELONLY && !sel[ct]) ||
						!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) */
						{

						gw->clearHitCode();
						gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
						Point3 pt;
						Interval v;

						Point3 invB;
						invB = (pta-ptb) *.1f;
						
						ptb += invB;



						gw->marker(&ptb,POINT_MRKR);
						if (gw->checkHitCode()) {

							vpt->LogHit(inode, mc, gw->getHitDistance(), ct,
								new BoneHitDataClass(-1,i,1,-1,-1)); 
								
							res = 1;
							}
						}
					}

//add in enevelope inner and outer
				if ((FilterEnvelopes == 0) && (i == ModeBoneIndex))
					{
					if (BoneData[i].flags & BONE_SPLINE_FLAG)
						{
						ObjectState os = BoneData[i].Node->EvalWorldState(t);
						pathOb = (ShapeObject*)os.obj;
						}
					Point3 l1,l2;
					Interval v;

//					GetEndPoints(bmd, t,l1, l2, i);

					GetEndPointsLocal(bmd, t,l1, l2, i);

					Interval valid;
					Matrix3 ntm = BoneData[i].Node->GetObjTMBeforeWSM(t,&valid);

//					l1 = l1 * ntm  * bmd->InverseBaseTM;
//					l2 = l2 * ntm  * bmd->InverseBaseTM;


					Point3 align = (l2-l1);
					Point3 nvec = align;

					for (int j=0;j<BoneData[i].CrossSectionList.Count();j++)
						{

						Point3 p_edge[8];
						Point3 ept(0.0f,0.0f,0.0f);
						Point3 vec;


						if (BoneData[i].flags & BONE_SPLINE_FLAG)
							{
//							ept = pathOb->InterpCurve3D(t, 0,BoneData[i].CrossSectionList[j].u) * Inverse(BoneData[i].tm);
//							align = VectorTransform(Inverse(BoneData[i].tm),pathOb->TangentCurve3D(t, 0,BoneData[i].CrossSectionList[j].u));
							ept = pathOb->InterpCurve3D(t, 0,BoneData[i].CrossSectionList[j].u) * ntm  * bmd->InverseBaseTM;
							align = VectorTransform(ntm  * bmd->InverseBaseTM,pathOb->TangentCurve3D(t, 0,BoneData[i].CrossSectionList[j].u));
							float Inner,Outer;
							BoneData[i].CrossSectionList[j].InnerControl->GetValue(0,&Inner,v);
							BoneData[i].CrossSectionList[j].OuterControl->GetValue(0,&Outer,v);
							GetCrossSection(ept, align, Inner,BoneData[i].temptm, p_edge);
							GetCrossSection(ept, align, Outer,BoneData[i].temptm, &p_edge[4]);

							}
						else
							{
//							align = (BoneData[i].l2-BoneData[i].l1);
//							ept = BoneData[i].l1;
							align = (l2-l1);
							ept = l1;
							float Inner,Outer;
							BoneData[i].CrossSectionList[j].InnerControl->GetValue(0,&Inner,v);
							BoneData[i].CrossSectionList[j].OuterControl->GetValue(0,&Outer,v);


							ept = ept + nvec * BoneData[i].CrossSectionList[j].u;
							GetCrossSection(ept, align, Inner,BoneData[i].temptm, p_edge);
							GetCrossSection(ept, align, Outer,BoneData[i].temptm, &p_edge[4]);
//							GetCrossSection(ept, align, BoneData[i].CrossSectionList[j].Inner,BoneData[i].temptm, p_edge);
//							GetCrossSection(ept, align, BoneData[i].CrossSectionList[j].Outer,BoneData[i].temptm, &p_edge[4]);

							}



						for (int m=0;m<4;m++)
							{
							if ((flags&HIT_SELONLY   &&  BoneData[i].CrossSectionList[j].innerSelected) ||
								(flags&HIT_UNSELONLY && !BoneData[i].CrossSectionList[j].innerSelected) ||
								!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 
								{
		
								gw->clearHitCode();
								gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);


								
								gw->marker(&p_edge[m],POINT_MRKR);
								if (gw->checkHitCode()) {

									vpt->LogHit(inode, mc, gw->getHitDistance(), ct, 
										new BoneHitDataClass(-1,i,-1,j,m)); 
									res = 1;
									}
								}
							}
						for (m=4;m<8;m++)
							{
							if ((flags&HIT_SELONLY   &&  BoneData[i].CrossSectionList[j].outerSelected) ||
								(flags&HIT_UNSELONLY && !BoneData[i].CrossSectionList[j].outerSelected) ||
								!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 
								{
		
								gw->clearHitCode();
								gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);


								
								gw->marker(&p_edge[m],POINT_MRKR);
								if (gw->checkHitCode()) {

									vpt->LogHit(inode, mc, gw->getHitDistance(), ct, 
										new BoneHitDataClass(-1,i,-1,j,m)); 
									res = 1;
									}
								}
							}

						}
					}
				}
			}

		}

	gw->setRndLimits(savedLimits);

	return res;

	}



int BonesDefMod::Display(
		TimeValue t, INode* inode, ViewExp *vpt, 
		int flagst, ModContext *mc)
	{

	if (inode)
		{
		if (ip)
			{
			int nodeCount = ip->GetSelNodeCount();
			BOOL found = FALSE;
			for (int nct =0; nct < nodeCount; nct++)
				{
				if (inode == ip->GetSelNode(nct))
					{
					found = TRUE;
					nct = nodeCount;
					}
				}
			if (!found) return 0;
			}

		}


	BoneModData *bmd = (BoneModData *) mc->localData;

	if (!bmd) return 0;

	Interval iv;
	Matrix3 atm = inode->GetObjTMAfterWSM(t,&iv);
	Matrix3 ctm = inode->GetObjectTM(t,&iv);
	BOOL isWorldSpace = FALSE;
	if ((atm.IsIdentity()) && (ip->GetShowEndResult ()))
		isWorldSpace = TRUE;


	GraphicsWindow *gw = vpt->getGW();
	Point3 pt[4];
	Matrix3 tm = CompMatrix(t,inode,mc);
	int savedLimits;

//	obtm = tm;
//	iobtm = Inverse(tm);

	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);

//get selected bone	
	int fsel;
	fsel = SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_GETCURSEL ,0,0);

	int tsel = ConvertSelectedListToBoneID(fsel);




	if ((tsel>=0) && (ip && ip->GetSubObjectLevel() == 1) )
		{

		if (inPaint)
			{
			if (bmd->isHit)
				{
//draw 3d cursor
				Point3 x(1.0f,0.0f,0.0f),y(0.0f,1.0f,0.0f),z(0.0f,0.0f,1.0f);
				gw->setColor(LINE_COLOR, 1.0f,1.0f,0.0f);

				DrawCrossSectionNoMarkers(bmd->hitPoint, x, Radius, gw); // optimize these can be moved out of the loop
				DrawCrossSectionNoMarkers(bmd->hitPoint, y, Radius, gw); // optimize these can be moved out of the loop
				DrawCrossSectionNoMarkers(bmd->hitPoint, z, Radius, gw); // optimize these can be moved out of the loop

				}
			}
//		else
		{
		ObjectState os;

		os = inode->EvalWorldState(t);
//loop through points checking for selection and then marking
		float r,g,b;

		BOOL isPatch = FALSE;
		int knots = 0;
		PatchMesh *pmesh;
		BitArray interiorVecs;

		Interval iv;
		Matrix3 atm = inode->GetObjTMAfterWSM(t,&iv);
		Matrix3 btm = inode->GetObjTMBeforeWSM(t,&iv);


		if (os.obj->IsSubClassOf(patchObjectClassID))
			{
			PatchObject *pobj = (PatchObject*)os.obj;
			pmesh = &(pobj->patch);
			isPatch = TRUE;
			knots = pmesh->numVerts;
			interiorVecs.SetSize(pmesh->numVecs);
			interiorVecs.ClearAll();
			for (int ipatch=0;ipatch<pmesh->numPatches;ipatch++)
				{
				int pc = 3;
				if (pmesh->patches[ipatch].type == PATCH_QUAD) pc = 4;
				for (int ivec = 0; ivec < pc; ivec++)
//need to check if manual interio and mark if the  manuaul interio bug gets fixed
					interiorVecs.Set(pmesh->patches[ipatch].interior[ivec]);
				}

			}

		for (int i=0;i<bmd->VertexDataCount;i++)
			{

			for (int j=0;j<bmd->VertexData[i]->d.Count();j++)
				{
				if ((bmd->VertexData[i]->d[j].Bones == tsel) && (bmd->VertexData[i]->d[j].Influences != 0.0f) &&
					 (DrawVertices ==1) )
					
					{
					Point3 pt;
					if (isWorldSpace)
						pt = os.obj->GetPoint(i) *Inverse(ctm);
					else pt = os.obj->GetPoint(i);
//					pt = bmd->VertexData[i]->LocalPos;
//gte red is strongest,green, blue is weakest based on influence
					float infl;
//					infl = VertexData[i].d[j].Influences;

					infl = RetrieveNormalizedWeight(bmd,i,j);
					Point3 selColor(0.0f,0.0f,0.0f);
					Point3 selSoft = GetUIColor(COLOR_SUBSELECTION_SOFT);
					Point3 selMedium = GetUIColor(COLOR_SUBSELECTION_MEDIUM);
					Point3 selHard = GetUIColor(COLOR_SUBSELECTION_HARD);

					if (infl > 0.0f)
						{
						if ( (infl<0.5) && (infl > 0.0f))
							{
							selColor = selSoft + ( (selMedium-selSoft) * (infl/0.5f));
//							r =0.0f;
//							g = 0.0f;
//							b = 1.0f;
							}
						else if (infl<1.0)
							{
							selColor = selMedium + ( (selHard-selMedium) * ((infl-0.5f)/0.5f));
//							r =0.0f;
//							g = 1.0f;
//							b = 0.0f;
							}
						else 
							{
							selColor = GetUIColor(COLOR_SUBSELECTION);
//							r =1.0f;
//							g = 0.0f;
//							b = 0.0f;
							}
						r = selColor.x;
						g = selColor.y;
						b = selColor.z;

						gw->setColor(LINE_COLOR, r,g,b);
						if (isPatch)
							{
							if (i< knots)
								{
								gw->marker(&pt,PLUS_SIGN_MRKR);
								if (bmd->selected[i] == FALSE)
									{
//it is a knot draw the handle
									PatchVert pv = pmesh->getVert(i);
									Point3 lp[3];
									lp[0] = pt;
									gw->setColor(LINE_COLOR, 0.8f,0.8f,0.8f);

									for (int vec_count = 0; vec_count < pv.vectors.Count(); vec_count++)
										{
										int	idv = pv.vectors[vec_count];
										if (isWorldSpace)
											lp[1] = pmesh->getVec(idv).p * Inverse(ctm);
										else lp[1] = pmesh->getVec(idv).p;
										gw->polyline(2, lp, NULL, NULL, 0);
		
										}
									}

								}
							else 
								{
								if (!interiorVecs[i-knots])
									gw->marker(&pt,SM_HOLLOW_BOX_MRKR);
								}
							}
						else gw->marker(&pt,PLUS_SIGN_MRKR);
						j = bmd->VertexData[i]->d.Count()+1;
						}
				
					}
				}
			if (bmd->selected[i] == TRUE)
				{
				Point3 pt;
				if (isWorldSpace)
					pt = os.obj->GetPoint(i) *Inverse(ctm);
				else pt = os.obj->GetPoint(i);
				gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
				gw->marker(&pt,HOLLOW_BOX_MRKR);

				if ((i< knots) && (isPatch))
					{
//it is a knot draw the handle
					PatchVert pv = pmesh->getVert(i);
					Point3 lp[3];
					lp[0] = pt;
					gw->setColor(LINE_COLOR, 0.8f,0.8f,0.8f);

					for (int vec_count = 0; vec_count < pv.vectors.Count(); vec_count++)
						{
						int idv = pv.vectors[vec_count];
						if (isWorldSpace)
							lp[1] = pmesh->getVec(idv).p * Inverse(ctm);
						else lp[1] = pmesh->getVec(idv).p;
						gw->polyline(2, lp, NULL, NULL, 0);
//						gw->marker(&pt,HOLLOW_BOX_MRKR);

						}
					}
				}

//kinda sucks need to load this up so the paint has an array of world psace points to work with
//			bmd->VertexData[i]->LocalPos = os.obj->GetPoint(i);

			}


//draw selected bone

		for (i =0;i<BoneData.Count();i++)
			{
			if (BoneData[i].Node != NULL)
				{

				if (i== tsel)
					{
					r = 1.0f;
					g = 1.0f;
					b = 0.0f;
					Point3 l1,l2;
					Interval v;
					BoneData[i].EndPoint1Control->GetValue(0,&l1,v);
					BoneData[i].EndPoint2Control->GetValue(0,&l2,v);

					Interval valid;
					Matrix3 ntm = BoneData[i].Node->GetObjTMBeforeWSM(t,&valid);

					Worldl1 = l1  * ntm;  
					Worldl2 = l2  * ntm;  

					}
				else
					{
					r = 0.3f;
					g = 0.3f;
					b = 0.3f;
					}

				Point3 pta, ptb;
				Point3 pta_tm,ptb_tm;
				Point3 plist[2];

				
				GetEndPointsLocal(bmd, t,pta, ptb, i);


				gw->setColor(LINE_COLOR, r,g,b);

				ObjectState os;
				ShapeObject *pathOb = NULL;

//				pta = (pta ) * bmd->tmCacheToObjectSpace[i];
//				ptb = (ptb ) * bmd->tmCacheToObjectSpace[i];


				if (BoneData[i].flags & BONE_SPLINE_FLAG)
					{


					ObjectState os = BoneData[i].Node->EvalWorldState(t);
					pathOb = (ShapeObject*)os.obj;

					float su = 0.0f;
					float eu = 0.1f;
					float inc = 0.1f;
					Point3 sp_line[10];
					
					Point3 l1,l2;
					l1 = pathOb->InterpPiece3D(t, 0,0 ,0.0f ) * bmd->tmCacheToObjectSpace[i];
					l2 = pathOb->InterpPiece3D(t, 0,0 ,1.0f ) * bmd->tmCacheToObjectSpace[i];

					pta = l1;
					ptb = l2;

					plist[0] = pta;
					plist[1] = ptb;


					for (int cid = 0; cid < pathOb->NumberOfCurves(); cid++)
						{
						for (int sid = 0; sid < pathOb->NumberOfPieces(t,cid); sid++)
							{
								
							for (int spid = 0; spid < 4; spid++)
								{
								sp_line[spid] = pathOb->InterpPiece3D(t, cid,sid ,su ) * bmd->tmCacheToObjectSpace[i];  //optimize reduce the count here 
								su += inc;
								}
							gw->polyline(4, sp_line, NULL, NULL, 0);

							}
						}
					}
				else
					{
					Point3 invA,invB;
	

					invA = (ptb-pta) *.1f;
					invB = (pta-ptb) *.1f;

					plist[0] = pta + invA;
					plist[1] = ptb + invB;

					gw->polyline(2, plist, NULL, NULL, 0);
					}

				if ((BoneData[i].end1Selected) && (i== tsel))
					gw->setColor(LINE_COLOR, 1.0f,0.0f,1.0f);
				else gw->setColor(LINE_COLOR, .3f,.3f,0.3f);


				gw->marker(&plist[0],BIG_BOX_MRKR);

				if ((BoneData[i].end2Selected) && (i== tsel))
					gw->setColor(LINE_COLOR, 1.0f,0.0f,1.0f);
				else gw->setColor(LINE_COLOR, .3f,.3f,0.3f);

				gw->marker(&plist[1],BIG_BOX_MRKR);

//Draw Cross Sections
				Tab<Point3> CList;
				Tab<float> InnerList, OuterList;

				for (int ccount = 0; ccount < BoneData[i].CrossSectionList.Count();ccount++)
					{
					Point3 m;
					float inner;
					float outer;
					Interval v;
					BoneData[i].CrossSectionList[ccount].InnerControl->GetValue(0,&inner,v);
					BoneData[i].CrossSectionList[ccount].OuterControl->GetValue(0,&outer,v);

					GetCrossSectionRanges(inner, outer, i, ccount);

					if (tsel == i)
						{
						gw->setColor(LINE_COLOR, 1.0f,0.0f,0.0f);
						if ( (ModeBoneEnvelopeIndex == ccount) && (ModeBoneEnvelopeSubType<4))
							gw->setColor(LINE_COLOR, 1.0f,0.0f,1.0f);
						}

					Point3 nvec;
					Matrix3 rtm;
					if ((DrawEnvelopes ==1) || (tsel == i) || (BoneData[i].flags & BONE_DRAW_ENVELOPE_FLAG))
						{
						Point3 vec;

						InnerList.Append(1,&inner,1);
						OuterList.Append(1,&outer,1);

						if (BoneData[i].flags & BONE_SPLINE_FLAG)
							{
							vec = pathOb->InterpCurve3D(t, 0,BoneData[i].CrossSectionList[ccount].u) * bmd->tmCacheToObjectSpace[i]; 

							CList.Append(1,&(vec),1);
							nvec = VectorTransform(bmd->tmCacheToObjectSpace[i],pathOb->TangentCurve3D(t, 0,BoneData[i].CrossSectionList[ccount].u));
							DrawCrossSection(vec, nvec, inner, BoneData[i].temptm, gw);  // optimize these can be moved out of the loop
							}
						else
							{
							nvec = (ptb-pta);
							vec = nvec * BoneData[i].CrossSectionList[ccount].u;
							CList.Append(1,&(pta+vec),1);
							DrawCrossSection(pta+vec, nvec, inner, BoneData[i].temptm, gw); // optimize these can be moved out of the loop
							}

						}
					if (tsel == i)
						{	
						gw->setColor(LINE_COLOR, 0.5f,0.0f,0.0f);
						if ( (ModeBoneEnvelopeIndex == ccount) && (ModeBoneEnvelopeSubType>=4))
							gw->setColor(LINE_COLOR, 1.0f,0.0f,1.0f);
						}

					if ((DrawEnvelopes ==1) || (tsel == i) || (BoneData[i].flags & BONE_DRAW_ENVELOPE_FLAG))
						{
						Point3 vec;
						if (BoneData[i].flags & BONE_SPLINE_FLAG)
							{
							vec = pathOb->InterpCurve3D(t, 0,BoneData[i].CrossSectionList[ccount].u) * bmd->tmCacheToObjectSpace[i];
							nvec = VectorTransform(bmd->tmCacheToObjectSpace[i],pathOb->TangentCurve3D(t, 0,BoneData[i].CrossSectionList[ccount].u));  // optimize these can be moved out of the loop
							DrawCrossSection(vec, nvec, outer, BoneData[i].temptm, gw);

							}
						else
							{

							nvec = (ptb-pta);
							vec = nvec * BoneData[i].CrossSectionList[ccount].u;
							DrawCrossSection(pta+vec, nvec, outer, BoneData[i].temptm, gw);
							}

						}
					}

				if ((DrawEnvelopes ==1) || (tsel == i) || (BoneData[i].flags & BONE_DRAW_ENVELOPE_FLAG))
					{
					if (!(BoneData[i].flags & BONE_SPLINE_FLAG))
						{
						gw->setColor(LINE_COLOR, 1.0f,0.0f,0.0f);

						DrawEnvelope(CList, InnerList, CList.Count(), BoneData[i].temptm,  gw);
						gw->setColor(LINE_COLOR, 0.5f,0.0f,0.0f);
						DrawEnvelope(CList, OuterList, CList.Count(), BoneData[i].temptm,  gw);
						}
					}




				}
			}

		}


		}
	gw->setRndLimits(savedLimits);

	return 0;

	}




void BonesDefMod::GetWorldBoundBox(
		TimeValue t,INode* inode, ViewExp *vpt, 
		Box3& box, ModContext *mc)
	{

	BoneModData *bmd = (BoneModData *) mc->localData;

	if (!bmd) return ;



	Matrix3 tm = CompMatrix(t,inode,mc);
	Point3 pt1, pt2;
	box.Init();

	if (ModeEdit == 1)
		{
		Interval iv;
		if ( (ModeBoneEndPoint == 0) || (ModeBoneEndPoint == 1))
			{
			p1Temp->GetValue(t,&pt1,FOREVER,CTRL_ABSOLUTE);
			box += pt1;// * BoneData[ModeBoneIndex].temptm * tm;
			}
		}	

	float l = 0.0f;
	for (int i =0;i<BoneData.Count();i++)
		{
		if (BoneData[i].Node != NULL)
			{


			Interval valid;
			Matrix3 ntm = BoneData[i].Node->GetObjTMBeforeWSM(t,&valid);
//			pta = pta * ntm  * bmd->InverseBaseTM;
//			ptb = ptb * ntm  * bmd->InverseBaseTM;


			Point3 pta;
			Interval v;
			BoneData[i].EndPoint1Control->GetValue(0,&pta,v);

			box += pta* ntm;//  * bmd->InverseBaseTM;;// * BoneData[i].temptm * tm;
			BoneData[i].EndPoint2Control->GetValue(0,&pta,v);
			box += pta* ntm;//  * bmd->InverseBaseTM;;//  * BoneData[i].temptm * tm;
			for (int k = 0; k < BoneData[i].CrossSectionList.Count();k++)
				{
				float outer;
				Interval v;
				BoneData[i].CrossSectionList[k].OuterControl->GetValue(0,&outer,v);

				if (outer > l) l = outer;
				}
			}
		}
	box.EnlargeBy(l+10.0f);  // this is a fudge since I am using large tick boxes
	
	}


void BonesDefMod::Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin)
	{

//check of points
//check for envelopes

	if ( !ip ) return;

	ModContextList mcList;		
	INodeTab nodes;

	ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	for ( int i = 0; i < objects; i++ ) 
		{
		BoneModData *bmd = (BoneModData*)mcList[i]->localData;

		int mode = 1;
		if (mode >0 )
			{
			ModeEdit = 1;
			val = VectorTransform(tmAxis*Inverse(partm),val);



			if (ModeBoneEndPoint == 0)
				{
				val = VectorTransform(bmd->tmCacheToBoneSpace[ModeBoneIndex],val);

				bmd->CurrentCachePiece = -1;
				BoneData[ModeBoneIndex].EndPoint1Control->SetValue(0,&val,TRUE,CTRL_RELATIVE);

//				Interval iv;
//				BoneData[ModeBoneIndex].EndPoint1Control->GetValue(0,&bmd->localCenter,iv);
//				bmd->localCenter = bmd->localCenter *bmd->tmCacheToObjectSpace[ModeBoneIndex];

				}
			else if (ModeBoneEndPoint == 1)
				{
				val = VectorTransform(bmd->tmCacheToBoneSpace[ModeBoneIndex],val);

				bmd->CurrentCachePiece = -1;
				BoneData[ModeBoneIndex].EndPoint2Control->SetValue(0,&val,TRUE,CTRL_RELATIVE);

//				Interval iv;
//				BoneData[ModeBoneIndex].EndPoint2Control->GetValue(0,&bmd->localCenter,iv);

//				bmd->localCenter = bmd->localCenter *bmd->tmCacheToObjectSpace[ModeBoneIndex];

				}

			else
				{
				if ((ModeBoneEnvelopeIndex != -1) && (ModeBoneEnvelopeSubType != -1))
					{

					val = VectorTransform(bmd->tmCacheToBoneSpace[ModeBoneIndex],val);
					p1Temp->SetValue(0,val,TRUE,CTRL_RELATIVE);

					Interval v;
//					p1Temp->GetValue(0,bmd->localCenter,v);
//					bmd->localCenter = bmd->localCenter *bmd->tmCacheToObjectSpace[ModeBoneIndex];


					ObjectState os;
					ShapeObject *pathOb = NULL;
					Point3 nvec;
					Point3 vec;
					Point3 lp;
					Point3 l1,l2;
					BoneData[ModeBoneIndex].EndPoint1Control->GetValue(0,&l1,v);
					BoneData[ModeBoneIndex].EndPoint2Control->GetValue(0,&l2,v);


					Point3 p(0.0f,0.0f,0.0f);
					Interval iv = FOREVER;
					p1Temp->GetValue(0,&p,iv);
					 
//					p = val;


					if (BoneData[ModeBoneIndex].flags & BONE_SPLINE_FLAG)
						{
						ObjectState os = BoneData[ModeBoneIndex].Node->EvalWorldState(t);
						pathOb = (ShapeObject*)os.obj;
//						lp = pathOb->InterpCurve3D(t, 0,BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u) * Inverse(BoneData[ModeBoneIndex].tm);					
						lp = pathOb->InterpCurve3D(t, 0,BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u);					
						}
					else
						{
						nvec = l2-l1;
						lp = l1 + nvec * BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u;
						}


					if (ModeBoneEnvelopeSubType<4)
						{
						lp = lp * Inverse(bmd->tmCacheToBoneSpace[ModeBoneIndex]);
						p = p * Inverse(bmd->tmCacheToBoneSpace[ModeBoneIndex]);
						float inner = Length(lp-p);

						BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].InnerControl->SetValue(0,&inner,TRUE,CTRL_ABSOLUTE);
						}
					else if (ModeBoneEnvelopeSubType<8)
						{
						lp = lp * Inverse(bmd->tmCacheToBoneSpace[ModeBoneIndex]);
						p = p * Inverse(bmd->tmCacheToBoneSpace[ModeBoneIndex]);

						float outer = Length(lp-p);
	
						BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].OuterControl->SetValue(0,&outer,TRUE,CTRL_ABSOLUTE);
						}
					}

				}

			}


//move the right controller		
		}

	}

void BonesDefMod::GetSubObjectCenters(
		SubObjAxisCallback *cb,TimeValue t,
		INode *node,ModContext *mc)
	{
	BoneModData *bmd = (BoneModData *) mc->localData;

	if (!bmd) return; 

	Matrix3 tm = CompMatrix(t,node,mc);
	Point3 pt(0,0,0), p;
	if (ModeBoneEndPoint == 0)
		{
		Interval iv;
		BoneData[ModeBoneIndex].EndPoint1Control->GetValue(0,&bmd->localCenter,iv,CTRL_ABSOLUTE);
		bmd->localCenter = bmd->localCenter *bmd->tmCacheToObjectSpace[ModeBoneIndex];
		}
	else if (ModeBoneEndPoint == 1)
		{
		Interval iv;
		BoneData[ModeBoneIndex].EndPoint2Control->GetValue(0,&bmd->localCenter,iv,CTRL_ABSOLUTE);

		bmd->localCenter = bmd->localCenter *bmd->tmCacheToObjectSpace[ModeBoneIndex];
		}

	else
		{
		if ((ModeBoneEnvelopeIndex != -1) && (ModeBoneEnvelopeSubType != -1))
			{
			Interval v;
			p1Temp->GetValue(0,bmd->localCenter,v,CTRL_ABSOLUTE);
			bmd->localCenter = bmd->localCenter *bmd->tmCacheToObjectSpace[ModeBoneIndex];
			}
		}
	pt = bmd->localCenter;
	tm.PreTranslate(pt);
	cb->Center(tm.GetTrans(),0);
	}

void BonesDefMod::GetSubObjectTMs(
		SubObjAxisCallback *cb,TimeValue t,
		INode *node,ModContext *mc)
	{
	Matrix3 tm = CompMatrix(t,node,mc);
	cb->TM(tm,0);
	}


void BonesDefMod::UpdatePropInterface()

{

if ( (ModeBoneIndex >= 0) && (ModeBoneIndex < BoneData.Count()) )
	{
	if (BoneData[ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
		{
		iAbsolute->SetCheck(FALSE);

		pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
		}
	else
		{
		iAbsolute->SetCheck(TRUE);
		pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);
		}


	if (BoneData[ModeBoneIndex].flags & BONE_DRAW_ENVELOPE_FLAG)
		{
		pblock_param->SetValue(PB_DRAW_BONE_ENVELOPE,0,1);
		iEnvelope->SetCheck(TRUE);
		}
	else
		{
		pblock_param->SetValue(PB_DRAW_BONE_ENVELOPE,0,0);
		iEnvelope->SetCheck(FALSE);
		}
	if (BoneData[ModeBoneIndex].FalloffType == BONE_FALLOFF_X_FLAG)
		iFalloff->SetCurFlyOff(0,FALSE);
	else if (BoneData[ModeBoneIndex].FalloffType == BONE_FALLOFF_SINE_FLAG)
		iFalloff->SetCurFlyOff(1,FALSE);
	else if (BoneData[ModeBoneIndex].FalloffType == BONE_FALLOFF_X3_FLAG)
		iFalloff->SetCurFlyOff(3,FALSE);
	else if (BoneData[ModeBoneIndex].FalloffType == BONE_FALLOFF_3X_FLAG)
		iFalloff->SetCurFlyOff(2,FALSE);
	}


}

void BonesDefMod::UpdateP(BoneModData* bmd)
{

Point3 align;
Point3 vec;

Point3 p_edge[8];
Point3 ept(0.0f,0.0f,0.0f);

ObjectState os;
ShapeObject *pathOb = NULL;

Interval valid;
Matrix3 ntm = BoneData[ModeBoneIndex].Node->GetObjTMBeforeWSM(ip->GetTime(),&valid);

if ((BoneData[ModeBoneIndex].flags & BONE_SPLINE_FLAG) && (BoneData[ModeBoneIndex].Node != NULL))
	{
	ObjectState os = BoneData[ModeBoneIndex].Node->EvalWorldState(ip->GetTime());
	pathOb = (ShapeObject*)os.obj;
	ept = pathOb->InterpCurve3D(ip->GetTime(), 0,BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u);
	align = pathOb->TangentCurve3D(ip->GetTime(), 0,BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u);
	ept = ept * bmd->tmCacheToObjectSpace[ModeBoneIndex];
	align = VectorTransform(bmd->tmCacheToObjectSpace[ModeBoneIndex],align);

	}
else
	{
	Point3 l1,l2;
	Interval v;
	GetEndPointsLocal(bmd,ip->GetTime(),l1,l2, ModeBoneIndex);

	align = l2-l1;
	Point3 nvec = align;
	ept = l1;
	ept = ept + nvec * BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u;
	}

float inner, outer;
Interval v;
BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].InnerControl->GetValue(0,&inner,v);
BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].OuterControl->GetValue(0,&outer,v);
GetCrossSection(ept, align, inner,
				BoneData[ModeBoneIndex].temptm,  p_edge);
GetCrossSection(ept, align, outer,
				BoneData[ModeBoneIndex].temptm,  &p_edge[4]);

if (ModeBoneEnvelopeSubType < 4)
	{
	pblock_param->SetController(PB_ERADIUS, BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].InnerControl, FALSE);
	BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].innerSelected = TRUE;

	}
else{
	pblock_param->SetController(PB_ERADIUS, BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].OuterControl, FALSE);
	BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].outerSelected = TRUE;
	}
Point3 p = p_edge[ModeBoneEnvelopeSubType] * bmd->BaseTM * Inverse(ntm);
bmd->localCenter = p_edge[ModeBoneEnvelopeSubType];


p1Temp->SetValue(0,p,TRUE,CTRL_ABSOLUTE);

//NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
//ip->RedrawViews(ip->GetTime());

}


void BonesDefMod::SelectSubComponent(
		HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
	{
	//if (theHold.Holding()) theHold.Put(new SelRestore(this));
	ModeEdit = 0;

	Tab<BoneModData*> bmdList;
	HitRecord *head = hitRec;
	while (hitRec) 
		{
		BoneModData *tbmd = (BoneModData*)hitRec->modContext->localData;
		BOOL found = FALSE;
		for (int i = 0; i < bmdList.Count(); i++)
			{
			if (bmdList[i] == tbmd)
				{
				found = TRUE;
				i = bmdList.Count();

				}
			}	
		if (!found) bmdList.Append(1,&tbmd,1);
		hitRec = hitRec->Next();
		}
	hitRec = head;


	for (int i = 0; i < bmdList.Count(); i++)
		{
		if (theHold.Holding() ) theHold.Put(new SelectionRestore(this,bmdList[i]));
		}


	BOOL add = GetKeyState(VK_CONTROL)<0;
	BOOL sub = GetKeyState(VK_MENU)<0;

	if (!add && !sub) 
		{
		for (int j = 0; j < bmdList.Count(); j++)
			{
			for (int i =0;i<bmdList[j]->selected.GetSize();i++)
				bmdList[j]->selected.Set(i,FALSE);
			}
		}
	int Count = 0;
	BOOL state = selected;

	BoneHitDataClass *bhd;

	int mode = -1;


	while (hitRec) {
		state = hitRec->hitInfo;
		BoneModData *bmd = (BoneModData*)hitRec->modContext->localData;
		if (sub)
			{
			 if (state < bmd->selected.GetSize()) bmd->selected.Set(state,FALSE);
			}
		else 
			{
			
			bhd = (BoneHitDataClass *) hitRec->hitData;
			if (bhd->VertexId == -1)
				{
				mode = 1;
				if (ModeBoneIndex == bhd->BoneId)
					{
					ModeBoneIndex = bhd->BoneId;
					ModeBoneEnvelopeIndex = bhd->CrossId;
					ModeBoneEndPoint = bhd->EndPoint;
					ModeBoneEnvelopeIndex = bhd->CrossId;
					ModeBoneEnvelopeSubType = bhd->CrossHandleId;
		
					}
				else
					{
					ModeBoneIndex = bhd->BoneId;
					ModeBoneEnvelopeIndex = -1;
					ModeBoneEndPoint = -1;
					ModeBoneEnvelopeIndex = -1;
					ModeBoneEnvelopeSubType = -1;

					}
				UpdatePropInterface();

				}
			else bmd->selected.Set(state,TRUE);
			}

		
		hitRec = hitRec->Next();
		Count++;
		}	
	for (i = 0; i < bmdList.Count(); i++)
		{
		BoneModData *bmd = bmdList[i];
		
		if (bmd->selected.NumberSet() > 0)
			{
			EnableEffect(TRUE);
// JBW: macro-recorder
			macroRecorder->FunctionCall(_T("skinSelectVertices"), 2, 0, mr_reftarg, this, mr_bitarray, &bmd->selected);


			}
			else EnableEffect(FALSE);

	
		if (mode >0 )
			{
			Point3 p;
//clear selection flags;
			for (int bct = 0; bct < BoneData.Count(); bct++)
				{
				BoneData[bct].end1Selected = FALSE;
				BoneData[bct].end2Selected = FALSE;
				for (int cct = 0; cct < BoneData[bct].CrossSectionList.Count(); cct++)
					{
					BoneData[bct].CrossSectionList[cct].innerSelected = FALSE;
					BoneData[bct].CrossSectionList[cct].outerSelected = FALSE;
					}

				}
			if (ModeBoneEndPoint == 0)
				{
				BoneData[ModeBoneIndex].end1Selected = TRUE;
				Point3 tp;
				if (ip)
					GetEndPoints(bmd, ip->GetTime(), bmd->localCenter, tp, ModeBoneIndex);
//			macroRecorder->FunctionCall(_T("select"), 1, 0, mr_index, mr_prop, _T("endpoint1"), 
//						mr_reftarg, this, mr_bitarray, &bmd->selected);

				}
			else if (ModeBoneEndPoint == 1)
				{
				BoneData[ModeBoneIndex].end2Selected = TRUE;
				Point3 tp;
				if (ip)
					GetEndPoints(bmd, ip->GetTime(), tp, bmd->localCenter, ModeBoneIndex);

//			p = BoneData[ModeBoneIndex].l2;

				}
			else if (ModeBoneEnvelopeIndex>=0)
				{
//			EnableRadius(TRUE);



				Point3 align;
				Point3 vec;

				Point3 p_edge[8];
				Point3 ept(0.0f,0.0f,0.0f);

				ObjectState os;
				ShapeObject *pathOb = NULL;

				Interval valid;
				Matrix3 ntm = BoneData[ModeBoneIndex].Node->GetObjTMBeforeWSM(ip->GetTime(),&valid);

				if ((BoneData[ModeBoneIndex].flags & BONE_SPLINE_FLAG) && (BoneData[ModeBoneIndex].Node != NULL))
					{
					ObjectState os = BoneData[ModeBoneIndex].Node->EvalWorldState(ip->GetTime());
					pathOb = (ShapeObject*)os.obj;
					ept = pathOb->InterpCurve3D(ip->GetTime(), 0,BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u);
					align = pathOb->TangentCurve3D(ip->GetTime(), 0,BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u);
					ept = ept * bmd->tmCacheToObjectSpace[ModeBoneIndex];
					align = VectorTransform(bmd->tmCacheToObjectSpace[ModeBoneIndex],align);

					}
				else
					{
					Point3 l1,l2;
					Interval v;
					GetEndPointsLocal(bmd,ip->GetTime(),l1,l2, ModeBoneIndex);

					align = l2-l1;
					Point3 nvec = align;
					ept = l1;
					ept = ept + nvec * BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].u;
					}

				float inner, outer;
				Interval v;
				BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].InnerControl->GetValue(0,&inner,v);
				BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].OuterControl->GetValue(0,&outer,v);

				GetCrossSection(ept, align, inner,
							BoneData[ModeBoneIndex].temptm,  p_edge);
				GetCrossSection(ept, align, outer,
							BoneData[ModeBoneIndex].temptm,  &p_edge[4]);

				if (ModeBoneEnvelopeSubType < 4)
					{
					pblock_param->SetController(PB_ERADIUS, BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].InnerControl, FALSE);
					BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].innerSelected = TRUE;

					}
				else{
					pblock_param->SetController(PB_ERADIUS, BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].OuterControl, FALSE);
					BoneData[ModeBoneIndex].CrossSectionList[ModeBoneEnvelopeIndex].outerSelected = TRUE;
					}
				p = p_edge[ModeBoneEnvelopeSubType] * bmd->BaseTM * Inverse(ntm);
				bmd->localCenter = p_edge[ModeBoneEnvelopeSubType];

				}
		
		
			if  (ModeBoneEnvelopeIndex == -1) EnableRadius(FALSE);
			else EnableRadius(TRUE);


			p1Temp->SetValue(0,p,TRUE,CTRL_ABSOLUTE);

		
//select in list box also 
			int rsel = ConvertSelectedBoneToListID(ModeBoneIndex);

			SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_SETCURSEL ,rsel,0);

			if (BoneData[ModeBoneIndex].flags & BONE_LOCK_FLAG)
				pblock_param->SetValue(PB_LOCK_BONE,0,1);
			else
				pblock_param->SetValue(PB_LOCK_BONE,0,0);
			if (BoneData[ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
				pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
			else
				pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);




			}

		UpdateEffectSpinner(bmd);

		int nset = bmd->selected.NumberSet();
		int total = bmd->selected.GetSize();

		NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
		}
	}



void BonesDefMod::UpdateEffectSpinner(BoneModData*bmd)
{
	if (bmd->selected.NumberSet() > 0)
//   if ((Count == 1) && (state<bmd->VertexDataCount))
		{
//get selected bone	
		int rsel = 0;
		rsel = SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_GETCURSEL ,0,0);

		int tsel = ConvertSelectedListToBoneID(rsel);


		int sct = 0;
		float v = -1.0f;
		BOOL first = TRUE;
		BOOL idnt = FALSE;

		for (int i = 0; i < bmd->selected.GetSize(); i++)
			{
			if (bmd->selected[i])
				{
				BOOL match = FALSE;
				for (int ct =0 ; ct <bmd->VertexData[i]->d.Count(); ct++)
					{

					if (bmd->VertexData[i]->d[ct].Bones == ModeBoneIndex)
						{
						match = TRUE;
						if (first)
							{
							v = RetrieveNormalizedWeight(bmd,i,ct);
//							v = bmd->VertexData[i]->d[ct].Influences;
							first = FALSE;
							}
						else if (v != RetrieveNormalizedWeight(bmd,i,ct))
							{	
							idnt = TRUE;

							}
						}
					}
				 if (!match) 
					{
					if (first)
						{
						v = 0.0f;
						first = FALSE;
						}
					else if (v != 0.0f)
						{
						idnt = TRUE;
						}
					}


				}

			}

		if (idnt)
			{
			ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hParam,IDC_EFFECTSPIN));
			spin2->SetIndeterminate(TRUE);
			}
		else 
			{
			ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hParam,IDC_EFFECTSPIN));
			spin2->SetIndeterminate(FALSE);
			pblock_param->SetValue(PB_EFFECT,0,v);
			bmd->effect = v;
			}



		}

}

void BonesDefMod::ClearSelection(int selLevel)
	{
	//if (theHold.Holding()) theHold.Put(new SelRestore(this));

	if (selLevel == 1)
		{
		ModContextList mcList;		
		INodeTab nodes;

		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();


		for ( int i = 0; i < objects; i++ ) 
			{
			BoneModData *bmd = (BoneModData*)mcList[i]->localData;

			if (theHold.Holding() ) theHold.Put(new SelectionRestore(this,bmd));

			for (int i =0;i<bmd->selected.GetSize();i++)
				bmd->selected.Set(i,FALSE);

			UpdateEffectSpinner(bmd);

			NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			}
		}
	}

void BonesDefMod::SelectAll(int selLevel)
	{
	if (selLevel == 1)
		{
		ModContextList mcList;		
		INodeTab nodes;

		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		for ( int i = 0; i < objects; i++ ) 
			{
			BoneModData *bmd = (BoneModData*)mcList[i]->localData;

			if (theHold.Holding() ) theHold.Put(new SelectionRestore(this,bmd));

			for (int i =0;i<bmd->selected.GetSize();i++)
				bmd->selected.Set(i,TRUE);

			UpdateEffectSpinner(bmd);
	
			NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			}
		}
	}


int BonesDefMod::HoldWeights()
	{
	ModContextList mcList;		
	INodeTab nodes;


	theHold.SuperBegin();
	if (ip)
		{
		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();
		theHold.Begin();
		for ( int i = 0; i < objects; i++ ) 
			{
			BoneModData *bmd = (BoneModData*)mcList[i]->localData;

			theHold.Put(new WeightRestore(this,bmd));


			}
		theHold.Accept(GetString(IDS_PW_WEIGHTCHANGE));
		
		}
	return 1;
	}

int BonesDefMod::AcceptWeights(BOOL accept)
	{

//	theHold.Accept(GetString(IDS_PW_WEIGHTCHANGE));
	if (accept) theHold.SuperAccept(GetString(IDS_PW_WEIGHTCHANGE));
	else theHold.SuperCancel();
	return 1;
	}


void BonesDefMod::InvertSelection(int selLevel)
	{
	if (selLevel == 1)
		{
		ModContextList mcList;		
		INodeTab nodes;

		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		for ( int i = 0; i < objects; i++ ) 
			{
			BoneModData *bmd = (BoneModData*)mcList[i]->localData;

			for (int i =0;i<bmd->selected.GetSize();i++)
				{
				BOOL v = !bmd->selected[i];
				bmd->selected.Set(i,v);
				}

			UpdateEffectSpinner(bmd);

			NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			}
		}
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}


void BonesDefMod::EnableRadius(BOOL enable)

{

if (enable)
	SpinnerOn(hParam,IDC_ERADIUSSPIN,IDC_ERADIUS);
else SpinnerOff(hParam,IDC_ERADIUSSPIN,IDC_ERADIUS);

}

void BonesDefMod::EnableEffect(BOOL enable)
{

if (enable)
	SpinnerOn(hParam,IDC_EFFECTSPIN,IDC_EFFECT);
else SpinnerOff(hParam,IDC_EFFECTSPIN,IDC_EFFECT);

}


void BonesDefMod::EnableButtons()
{

				if (iCrossSectionButton!=NULL)
					iCrossSectionButton->Enable();
				if (iPaintButton!=NULL)
					iPaintButton->Enable();
				EnableWindow(GetDlgItem(hParam,IDC_CREATE_REMOVE_SECTION),TRUE);


				EnableWindow(GetDlgItem(hParam,IDC_FILTER_VERTICES_CHECK),TRUE);
				EnableWindow(GetDlgItem(hParam,IDC_FILTER_BONES_CHECK),TRUE);
				EnableWindow(GetDlgItem(hParam,IDC_FILTER_ENVELOPES_CHECK),TRUE);
				EnableWindow(GetDlgItem(hParam,IDC_DRAWALL_ENVELOPES_CHECK),TRUE);
				EnableWindow(GetDlgItem(hParam,IDC_DRAW_VERTICES_CHECK),TRUE);

// bug fix 206160 9/8/99	watje
			if (iAbsolute!=NULL)
				iAbsolute->Enable();
			if (iEnvelope!=NULL)
				iEnvelope->Enable();
			if (iFalloff!=NULL)
				iFalloff->Enable();
			if (iCopy!=NULL)
				iCopy->Enable();
			if (iPaste!=NULL)
				iPaste->Enable();




}
void BonesDefMod::DisableButtons()
{

			if (iCrossSectionButton!=NULL)
				iCrossSectionButton->Disable();
			if (iPaintButton!=NULL)
				iPaintButton->Disable();

			EnableWindow(GetDlgItem(hParam,IDC_CREATE_REMOVE_SECTION),FALSE);


			EnableWindow(GetDlgItem(hParam,IDC_FILTER_VERTICES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hParam,IDC_FILTER_BONES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hParam,IDC_FILTER_ENVELOPES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hParam,IDC_DRAWALL_ENVELOPES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hParam,IDC_DRAW_VERTICES_CHECK),FALSE);


// bug fix 206160 9/8/99	watje
			if (iAbsolute!=NULL)
				iAbsolute->Disable();
			if (iEnvelope!=NULL)
				iEnvelope->Disable();
			if (iFalloff!=NULL)
				iFalloff->Disable();
			if (iCopy!=NULL)
				iCopy->Disable();
			if (iPaste!=NULL)
				iPaste->Disable();
			   

   
   


}

void BonesDefMod::ActivateSubobjSel(int level, XFormModes& modes)
	{
//	static BOOL isAnimating;
	switch (level) {
		case 0:
/*			GetCOREInterface()->EnableAnimateButton(TRUE);
			if (isAnimating) 
				{
				AnimateOn();
				GetCOREInterface()->SetAnimateButtonState(TRUE);
				}
*/
			DisableButtons();


			if ((ip) && (ip->GetCommandMode() == PaintMode)) {
				ip->SetStdCommandMode(CID_OBJMOVE);
				return;
				}


			if ((ip) && (ip->GetCommandMode() == CrossSectionMode)) {
				ip->SetStdCommandMode(CID_OBJMOVE);
				return;
				}


			break;
		case 1: // Points
			{
/*			isAnimating = Animating();
			GetCOREInterface()->EnableAnimateButton(FALSE);
			GetCOREInterface()->SetAnimateButtonState(FALSE);
			AnimateOff();
*/
			int bct = 0;
			for (int i =0; i < BoneData.Count(); i ++)
				{
				if (!(BoneData[i].flags &  BONE_DEAD_FLAG)) bct++;
				}
//			if (BoneData.Count() > 0)
			if (bct > 0)
				{
				EnableButtons();
				}

			if (ModeBoneEnvelopeIndex != -1)
				EnableRadius(TRUE);
			else EnableRadius(FALSE);


			modes = XFormModes(moveMode,NULL,NULL,NULL,NULL,NULL);
			break;		
			}
		}
	NotifyDependents(FOREVER,PART_DISPLAY,REFMSG_CHANGE);
	}


int BonesDefMod::SubNumToRefNum(int subNum)
	{
	return -1;
	}

RefTargetHandle BonesDefMod::GetReference(int i)

	{
	if (i==PBLOCK_PARAM_REF)
		{
		return (RefTargetHandle)pblock_param;
		}
	else if (i == POINT1_REF)
		{
		return (RefTargetHandle)p1Temp;
		}
	else 
		{
//		int id = i -2;
//		RefTargetHandle t;
		for (int ct = 0; ct < BoneData.Count(); ct++)
			{
			if (i == BoneData[ct].BoneRefID)
				{
				return (RefTargetHandle)BoneData[ct].Node;
				}
			else if (i == BoneData[ct].RefEndPt1ID)
				{
				return (RefTargetHandle)BoneData[ct].EndPoint1Control;
				}
			else if (i == BoneData[ct].RefEndPt2ID)
				{
				return (RefTargetHandle)BoneData[ct].EndPoint2Control;
				}
			else
				{
				for (int j=0;j<BoneData[ct].CrossSectionList.Count();j++)
					{
					if (i == BoneData[ct].CrossSectionList[j].RefInnerID)
						{
						return (RefTargetHandle)BoneData[ct].CrossSectionList[j].InnerControl;
						}
					else if (i == BoneData[ct].CrossSectionList[j].RefOuterID)
						{
						return (RefTargetHandle)BoneData[ct].CrossSectionList[j].OuterControl;
						}

					}
				}
			}
		
//		else return NULL;
		}
	return NULL;
	}

void BonesDefMod::SetReference(int i, RefTargetHandle rtarg)
	{
	if (i==PBLOCK_PARAM_REF)
		{
		pblock_param = (IParamBlock*)rtarg;
		}
	else if (i == POINT1_REF)
		{

		p1Temp     = (Control*)rtarg; 
		}
	else 
		{
//		int id = i -2;
		for (int ct = 0; ct < BoneData.Count(); ct++)
			{
			if (i == BoneData[ct].BoneRefID)
				{
				BoneData[ct].Node = (INode*)rtarg;
				}
			if (i == BoneData[ct].RefEndPt1ID)
				{
				BoneData[ct].EndPoint1Control = (Control*)rtarg;
				}
			if (i == BoneData[ct].RefEndPt2ID)
				{
				BoneData[ct].EndPoint2Control = (Control*)rtarg;

				}
			for (int j=0;j<BoneData[ct].CrossSectionList.Count();j++)
				{
				if (i == BoneData[ct].CrossSectionList[j].RefInnerID)
					{

					BoneData[ct].CrossSectionList[j].InnerControl  = (Control*)rtarg ;
					}
				if (i == BoneData[ct].CrossSectionList[j].RefOuterID)
					{
					BoneData[ct].CrossSectionList[j].OuterControl  = (Control*)rtarg ;
					}

				}
			}
		}
/*
		{

		int id = i-2;
		BoneData[id].Node = (INode*)rtarg;
		}
*/

	}

TSTR BonesDefMod::SubAnimName(int i)
	{
	return _T("");
	}

RefResult BonesDefMod::NotifyRefChanged(
		Interval changeInt,RefTargetHandle hTarget, 
		PartID& partID, RefMessage message)
	{
	int i;
	Interface *tip;


	switch (message) {
		case REFMSG_CHANGE:
			if (editMod==this && pmapParam) pmapParam->Invalidate();
			tip = GetCOREInterface();
			if (tip != NULL)
				{
				for (i =0;i<BoneData.Count();i++)
//				for (i =0;i<MAX_NUMBER_BONES;i++)
					{
					if ((BoneData[i].Node != NULL) && 
						(BoneData[i].Node == hTarget) && 
						(tip->GetTime() == RefFrame) )
						{
						BoneMoved = TRUE;
						}
					if ((BoneData[i].Node != NULL) && 
						(BoneData[i].Node == hTarget)  
						)
						{
//check if bone was spline 
						if (BoneData[i].flags & BONE_SPLINE_FLAG)
							{
							splineChanged = TRUE;
							whichSplineChanged = i;
							}

						}

					}
				}

			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			switch (gpd->index) {
				case PB_EFFECT:	 gpd->dim = stdNormalizedDim; break;
				case PB_REF_FRAME:	 gpd->dim = stdNormalizedDim; break;
				}
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;			
			switch (gpn->index) {

				case PB_EFFECT: gpn->name = GetString(IDS_RB_EFFECT); break;
				}
			return REF_STOP; 
			}
		case REFMSG_TARGET_DELETED: {
				for (int j =0;j<BoneData.Count();j++)
					{
					if (hTarget==BoneData[j].Node) 
						{
						RemoveBone(j);
						}
							
					}
				break;
				}

		}
	return REF_SUCCEED;
	}

IOResult BonesDefMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	IOResult res = IO_OK;
	int NodeID = 0;
	

//	VertexData= NULL;
/* FIX THIS move to local mod load
	for (int i = 0; i< bmd->VertexData.Count(); i++)
		{
		if (bmd->VertexData[i] != NULL)
			delete (bmd->VertexData[i]);
		bmd->VertexData[i] = NULL;
		}

	bmd->VertexData.ZeroCount();
	sel.ZeroCount();

*/
//	BoneData.ZeroCount();
	BoneData.New();
	int currentvt = -1;
	ULONG nb;
	int bonecount = 0;
	int MatrixCount = 0;
	reloadSplines = TRUE;

	int bct = 0;


	while (IO_OK==(res=iload->OpenChunk())) {
		int id = iload->CurChunkID();
		switch(id)  {

			case BASE_TM_CHUNK: 
				{
				OldBaseTM.Load(iload);
				OldInverseBaseTM = Inverse(OldBaseTM);
				break;
				}

			case BONE_COUNT_CHUNK: 
				{
				int c;
				iload->Read(&c,sizeof(c),&nb);
//				BoneData.SetCount(c);

				for (int i=0; i<c; i++)  
					{
					BoneDataClass t;
					BoneData.Append(t);
					BoneData[i].Node = NULL;
					BoneData[i].EndPoint1Control = NULL;
					BoneData[i].EndPoint2Control = NULL;
					BoneData[i].CrossSectionList.ZeroCount();
					}
				break;
				}
			case BONE_DATATM_CHUNK: 
				{
//				for (int i = 0; i < BoneData.Count(); i++)
				BoneData[MatrixCount++].tm.Load(iload);
				break;	
				}
			case BONE_NAME_CHUNK: 
				{
				NameTab names;
				int c = BoneData.Count();
				names.Load(iload);

				for (int i = 0; i < c; i++)
					{
					TSTR temp(names[i]);
					BoneData[i].name = temp;
					}

				break;	
				}

			case BONE_DATA_CHUNK: 
				{
				float load_f;
				Point3 load_p;
				int load_i;
				BYTE load_b;

				for (int i = 0; i < BoneData.Count(); i++)
					{
					iload->Read(&load_i,sizeof(load_i),&nb);
					BoneData[i].CrossSectionList.SetCount(load_i);
					for (int j=0;j<BoneData[i].CrossSectionList.Count();j++)
						{
//						iload->Read(&load_f,sizeof(load_f),&nb);
//						BoneData[i].CrossSectionList[j].Inner = load_f;
//						iload->Read(&load_f,sizeof(load_f),&nb);
//						BoneData[i].CrossSectionList[j].Outer = load_f;
						iload->Read(&load_f,sizeof(load_f),&nb);
						BoneData[i].CrossSectionList[j].u = load_f;
						iload->Read(&load_i,sizeof(load_i),&nb);
						BoneData[i].CrossSectionList[j].RefInnerID = load_i;
						iload->Read(&load_i,sizeof(load_i),&nb);
						BoneData[i].CrossSectionList[j].RefOuterID = load_i;

						BoneData[i].CrossSectionList[j].InnerControl = NULL;
						BoneData[i].CrossSectionList[j].OuterControl = NULL;

						BoneData[i].CrossSectionList[j].outerSelected = FALSE;
						BoneData[i].CrossSectionList[j].innerSelected = FALSE;
						BoneData[i].name.Resize(0);
						}
//					iload->Read(&load_p,sizeof(load_p),&nb);
//					BoneData[i].l1 = load_p;
//					iload->Read(&load_p,sizeof(load_p),&nb);
//					BoneData[i].l2 = load_p;
					
					iload->Read(&load_b,sizeof(load_b),&nb);
					BoneData[i].flags = load_b;

					iload->Read(&load_b,sizeof(load_b),&nb);
					BoneData[i].FalloffType = load_b;

					iload->Read(&load_i,sizeof(load_i),&nb);
					BoneData[i].BoneRefID = load_i;

					iload->Read(&load_i,sizeof(load_i),&nb);
					BoneData[i].RefEndPt1ID = load_i;

					iload->Read(&load_i,sizeof(load_i),&nb);
					BoneData[i].RefEndPt2ID = load_i;

					BoneData[i].end1Selected = FALSE;
					BoneData[i].end2Selected = FALSE;


					
					}

				break;
				}
			case BONE_SPLINE_CHUNK: 
				{
				reloadSplines = FALSE;
				for (int i = bct; i < BoneData.Count(); i++)
					{
					if (BoneData[i].flags & BONE_SPLINE_FLAG) 
						{
						BoneData[i].referenceSpline.Load(iload);
						bct= i+1;
						i = BoneData.Count();
						}
					}
				break;
				}



			case VERTEX_COUNT_CHUNK:
				{
				int c;
				iload->Read(&c,sizeof(c),&nb);
				OldVertexDataCount = c;
				OldVertexData.ZeroCount();
				OldVertexData.SetCount(c);
				for (int i=0; i<c; i++) {
					VertexListClass *vc;
					vc = new VertexListClass;
					OldVertexData[i] = vc;
					OldVertexData[i]->modified = FALSE;
					OldVertexData[i]->selected = FALSE;
 					OldVertexData[i]->d.ZeroCount();
					}

				break;

				}
			case VERTEX_DATA_CHUNK:
				{
				for (int i=0; i < OldVertexDataCount; i++)
					{
					int c;
					BOOL load_b;
					iload->Read(&c,sizeof(c),&nb);
					OldVertexData[i]->d.SetCount(c);

					iload->Read(&load_b,sizeof(load_b),&nb);
					OldVertexData[i]->modified = load_b;
					float load_f;
					int load_i;
					Point3 load_p;
					for (int j=0; j<c; j++) {
						iload->Read(&load_i,sizeof(load_i),&nb);
						iload->Read(&load_f,sizeof(load_f),&nb);
 						OldVertexData[i]->d[j].Bones = load_i;
						OldVertexData[i]->d[j].Influences =load_f;
						OldVertexData[i]->d[j].normalizedInfluences = -1.0f;

						iload->Read(&load_i,sizeof(load_i),&nb);
						OldVertexData[i]->d[j].SubCurveIds =load_i;
						iload->Read(&load_i,sizeof(load_i),&nb);
						OldVertexData[i]->d[j].SubSegIds =load_i;

						iload->Read(&load_f,sizeof(load_f),&nb);
 						OldVertexData[i]->d[j].u = load_f;

						iload->Read(&load_p,sizeof(load_p),&nb);
 						OldVertexData[i]->d[j].Tangents = load_p;

						iload->Read(&load_p,sizeof(load_p),&nb);
 						OldVertexData[i]->d[j].OPoints = load_p;


						}
					}

				break;

				}
			case BONE_BIND_CHUNK: 
				{
				initialXRefTM.Load(iload);
				ULONG id;
				iload->Read(&id,sizeof(ULONG), &nb);
				if (id!=0xffffffff)
					{
					iload->RecordBackpatch(id,(void**)&bindNode);
					}
				break;
				}

			case DELTA_COUNT_CHUNK: 
				{
				int c;
				iload->Read(&c,sizeof(c),&nb);
				endPointDelta.SetCount(c);
				break;
				}

			case DELTA_DATA_CHUNK: 
				{
				for (int i = 0; i < endPointDelta.Count();i++)
					{
					Point3 p;
					iload->Read(&p,sizeof(p),&nb);
					endPointDelta[i] = p;
					}
				break;
				}



			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}	


//build selection list now
//build selection space
/* FIX THIS move into local mod load

    int c = bmd->VertexDataCount+SELMOD*BoneData.Count();
//add m crossection
	for (int i=0;i < BoneData.Count();i++)
		c += BoneData[i].CrossSectionList.Count();
	sel.SetCount(c);
	for (i=0; i<c; i++) 
		sel[i] = FALSE;
*/

//build reftable
	int ref_size = 0;
	for (int i=0;i < BoneData.Count();i++)
		{
		if (BoneData[i].RefEndPt1ID > ref_size) ref_size = BoneData[i].RefEndPt1ID;
		if (BoneData[i].RefEndPt2ID > ref_size) ref_size = BoneData[i].RefEndPt2ID;
		if (BoneData[i].BoneRefID > ref_size) ref_size = BoneData[i].BoneRefID;
		for (int j=0;j < BoneData[i].CrossSectionList.Count();j++)
			{
			if (BoneData[i].CrossSectionList[j].RefInnerID > ref_size) ref_size = BoneData[i].CrossSectionList[j].RefInnerID;
			if (BoneData[i].CrossSectionList[j].RefOuterID > ref_size) ref_size = BoneData[i].CrossSectionList[j].RefOuterID;

			}

		}
	RefTable.SetCount(ref_size+2);
	for (i=0;i < RefTable.Count();i++)
		RefTable[i] = 0;
	for (i=0;i < BoneData.Count();i++)
		{
		if (BoneData[i].flags != BONE_DEAD_FLAG)
			{
			RefTable[BoneData[i].RefEndPt1ID-2] = 1;
			RefTable[BoneData[i].RefEndPt2ID-2] = 1;
			RefTable[BoneData[i].BoneRefID-2] = 1; 
			for (int j=0;j < BoneData[i].CrossSectionList.Count();j++)
				{
				RefTable[BoneData[i].CrossSectionList[j].RefInnerID-2] = 1;
				RefTable[BoneData[i].CrossSectionList[j].RefOuterID-2] = 1;
				}
			}

		}

//for (int k = 0;  k < RefTable.Count(); k++)
//	{
//	DebugPrint(" Load id %d  refid %d\n",k,RefTable[k]);
//	}



//	int s;

	iload->RegisterPostLoadCallback(
		new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,PBLOCK_PARAM_REF));


	return IO_OK;
	}

IOResult BonesDefMod::Save(ISave *isave)
	{
	Modifier::Save(isave);
	ULONG nb;


/*
	isave->BeginChunk(BASE_TM_CHUNK);
	bmd->BaseTM.Save(isave);
	isave->EndChunk();
*/

	int c = BoneData.Count();
	isave->BeginChunk(BONE_COUNT_CHUNK);
	isave->Write(&c,sizeof(c),&nb);
	isave->EndChunk();


//write bone chunks

	for (int i = 0; i < c; i++)
		{
		isave->BeginChunk(BONE_DATATM_CHUNK);
		BoneData[i].tm.Save(isave);
		isave->EndChunk();
		}
	
	isave->BeginChunk(BONE_DATA_CHUNK);
	for (i = 0; i < c; i++)
		{
		Point3 save_pt;
		float save_f;
		BYTE save_b;
		int save_i;

		save_i = BoneData[i].CrossSectionList.Count();
		isave->Write(&save_i,sizeof(save_i),&nb);

		for (int j = 0; j < BoneData[i].CrossSectionList.Count(); j++)
			{
//			save_f = BoneData[i].CrossSectionList[j].Inner;
//			isave->Write(&save_f,sizeof(save_f),&nb);
//			save_f = BoneData[i].CrossSectionList[j].Outer;
//			isave->Write(&save_f,sizeof(save_f),&nb);
			save_f = BoneData[i].CrossSectionList[j].u;
			isave->Write(&save_f,sizeof(save_f),&nb);

			save_i = BoneData[i].CrossSectionList[j].RefInnerID;
			isave->Write(&save_i,sizeof(save_i),&nb);

			save_i = BoneData[i].CrossSectionList[j].RefOuterID;
			isave->Write(&save_i,sizeof(save_i),&nb);

			}

//		save_pt = BoneData[i].l1;
//		isave->Write(&save_pt,sizeof(save_pt),&nb);

//		save_pt = BoneData[i].l2;
//		isave->Write(&save_pt,sizeof(save_pt),&nb);

		save_b = BoneData[i].flags;
		isave->Write(&save_b,sizeof(save_b),&nb);

		save_b = BoneData[i].FalloffType;
		isave->Write(&save_b,sizeof(save_b),&nb);

		save_i = BoneData[i].BoneRefID;
		isave->Write(&save_i,sizeof(save_i),&nb);

		save_i = BoneData[i].RefEndPt1ID;
		isave->Write(&save_i,sizeof(save_i),&nb);

		save_i = BoneData[i].RefEndPt2ID;
		isave->Write(&save_i,sizeof(save_i),&nb);



		}
	isave->EndChunk();


	for (i = 0; i < c; i++)
		{
		if ((BoneData[i].flags & BONE_SPLINE_FLAG) && (BoneData[i].Node != NULL) )
			{
			isave->BeginChunk(BONE_SPLINE_CHUNK);
			BoneData[i].referenceSpline.Save(isave);
			isave->EndChunk();
			}
		}


	if (bindNode)
		{
		isave->BeginChunk(BONE_BIND_CHUNK);
		initialXRefTM.Save(isave);
		
		ULONG id = isave->GetRefID(bindNode);
		isave->Write(&id,sizeof(ULONG),&nb);

		isave->EndChunk();
		}


	NameTab names;
//	names.SetCount(c);
	for (i = 0; i < c; i++)
		{	
		TSTR temp(BoneData[i].name);
		names.AddName(temp);
		}

	isave->BeginChunk(BONE_NAME_CHUNK);
	names.Save(isave);
	isave->EndChunk();


	c = endPointDelta.Count();
	isave->BeginChunk(DELTA_COUNT_CHUNK);
	isave->Write(&c,sizeof(c),&nb);
	isave->EndChunk();

	isave->BeginChunk(DELTA_DATA_CHUNK);
	for (i = 0; i < c; i++)
		{	
		Point3 p = endPointDelta[i];
		isave->Write(&p,sizeof(p),&nb);
		}
	isave->EndChunk();

	return IO_OK;
	}

Interval BonesDefMod::LocalValidity(TimeValue t)
	{
	Interval valid = FOREVER;
//	p1Temp->GetValue(t,&pt,iv,CTRL_ABSOLUTE);
//	for (int i = 0; i<MAX_NUMBER_BONES;i++)
	for (int i =0;i<BoneData.Count();i++)
		{
		if (BoneData[i].Node != NULL) 
			{
			BoneData[i].Node->GetObjTMBeforeWSM(t,&valid);
			if (BoneData[i].flags & BONE_SPLINE_FLAG)
				{
				ObjectState osp = BoneData[i].Node->EvalWorldState (t);
				valid &= osp.obj->ObjectValidity (t);
				}

			}
		}
	return valid;
	}




Point3 BonesDefMod::VertexAnimation(TimeValue t, BoneModData * bmd, int vertex, int bone, Point3 p)
{
Point3 ps(0.0f,0.0f,0.0f),pr(0.0f,0.0f,0.0f),pdef(0.0f,0.0f,0.0f),pt(0.0f,0.0f,0.0f);
Point3 MovedU,MovedTan;

int bid = bmd->VertexData[vertex]->d[bone].Bones;
if (BoneData[bid].Node == NULL) return p;

ShapeObject *pathOb = NULL;
Interface *ip = GetCOREInterface();
//TimeValue t;
//t = ip->GetTime();
ObjectState os = BoneData[bid].Node->EvalWorldState(t);
pathOb = (ShapeObject*)os.obj;

int cid = bmd->VertexData[vertex]->d[bone].SubCurveIds;
int sid = bmd->VertexData[vertex]->d[bone].SubSegIds;
float u = bmd->VertexData[vertex]->d[bone].u;


Matrix3 ntm = BoneData[bid].Node->GetObjectTM(t);	
Matrix3 tm    = Inverse(bmd->BaseTM * Inverse(ntm));

//Matrix3 tm = bmd->BaseTM * Inverse(ntm);

MovedU = pathOb->InterpPiece3D(t, cid,sid ,u );
MovedU = MovedU * tm;
MovedTan = pathOb->TangentPiece3D(t, cid, sid, u);
MovedTan = VectorTransform(tm,MovedTan);

Point3 OPoint;
OPoint = bmd->VertexData[vertex]->d[bone].OPoints * tm;
Point3 OTan = VectorTransform(tm,bmd->VertexData[vertex]->d[bone].Tangents);

float s = 1.0f;  //scale 
float angle = 0.0f;
float influ = RetrieveNormalizedWeight(bmd,vertex,bone);

//float influ = VertexData[vertex].d[bone].Influences;

OTan = Normalize(OTan);
MovedTan = Normalize(MovedTan);
if ( OTan != MovedTan)
 angle = (float) acos(DotProd(OTan,MovedTan)) * influ;



Point3 perp = CrossProd(OTan,MovedTan);
Matrix3 RotateMe(1);

RotateMe = RotAngleAxisMatrix(Normalize(perp), angle);
//RotateMe.Translate(-OPoint);


ps = p-OPoint;
pr = ps * RotateMe + OPoint;
pt = (MovedU - OPoint) * influ;
pdef = pr + pt;
return pdef;

}

class BonesDefDeformer: public Deformer {
	public:
		BonesDefMod *Cluster;
		BoneModData *bmd;
		TimeValue t;
		BonesDefDeformer(BonesDefMod *C, BoneModData *bm, TimeValue tv){Cluster = C;bmd = bm; t= tv;}
		Point3 Map(int i, Point3 p) {
			
			if (bmd->VertexDataCount>0)
				{

				if (bmd->VertexData[i]->d.Count() > 0 )
					{

					Point3 tp(0.0f,0.0f,0.0f);
					float influence = 0.0f;
					if (bmd->VertexData[i]->d.Count()==1)
						{
						Point3 vec;
						float influ = Cluster->RetrieveNormalizedWeight(bmd,i,0);

						vec = (p*Cluster->BoneData[bmd->VertexData[i]->d[0].Bones].temptm);
						vec = vec - p;
						vec = vec * influ;//Cluster->VertexData[i].d[0].Influences;
						p += vec;
						int bid;
						bid = bmd->VertexData[i]->d[0].Bones;
						if ((Cluster->BoneData[bid].flags & BONE_SPLINE_FLAG) && (influ != 0.0f))
							{
							p = Cluster->VertexAnimation(t,bmd,i,0,p);
							}

						return p;
						}
					for (int j=0;j<bmd->VertexData[i]->d.Count();j++)
						{
						float influ = Cluster->RetrieveNormalizedWeight(bmd,i,j);

						if (influ != 0.0f)
							{
							tp  += (p*Cluster->BoneData[bmd->VertexData[i]->d[j].Bones].temptm)*influ;
							influence += influ;
							}
						}
//do vertex snimation if it is a splineanimation

					for (j=0;j<bmd->VertexData[i]->d.Count();j++)
						{
						int bid;
						bid = bmd->VertexData[i]->d[j].Bones;

						if (Cluster->BoneData[bid].flags & BONE_SPLINE_FLAG) 
							{
							float influ = Cluster->RetrieveNormalizedWeight(bmd,i,j);

							if (influ != 0.0f)
								
								{
								tp = Cluster->VertexAnimation(t,bmd,i,j,tp);
								}
							}
						}

					if (influence > 0.00001)
						return tp;
	
					}
				else return p;
				}
			return p;
			}
	};



class StaticBonesDefDeformer: public Deformer {
	public:
		BonesDefMod *Cluster;
		BoneModData *bmd;
		TimeValue t;
		StaticBonesDefDeformer(BonesDefMod *C, BoneModData *bm, TimeValue tv){Cluster = C;bmd = bm; t= tv;}
		Point3 Map(int i, Point3 p) {
			
			if (bmd->VertexDataCount>0)
				{
				if ( i <bmd->VertexData.Count())
					p = bmd->VertexData[i]->LocalPos;
				}
			return p;
			}
	};

void BonesDefMod::RecomputeAllBones(BoneModData *bmd, TimeValue t, ObjectState *os)

{

//watje 9-7-99  198721 
		bmd->reevaluate = FALSE;
		int nv = os->obj->NumPoints();
		if ( (bmd->VertexDataCount != nv) || (reset))
			{
			reset = FALSE;
			bmd->VertexDataCount = nv;
			for (int i = 0; i < bmd->VertexData.Count(); i++)
				{
				if (bmd->VertexData[i] != NULL)
					delete (bmd->VertexData[i]);
				bmd->VertexData[i] = NULL;
				}
			bmd->VertexData.ZeroCount();
			bmd->VertexData.SetCount(nv);
//new char[sizeof( str )];

			for (i=0; i<nv; i++) {
				VertexListClass *vc;
				vc = new VertexListClass;
				bmd->VertexData[i] = vc;
				bmd->VertexData[i]->modified = FALSE;
				bmd->VertexData[i]->selected = FALSE;
 				bmd->VertexData[i]->d.ZeroCount();;

				}
			}


		int bonecount = 0;
		int crosscount = 0;
		for (int i =0;i<BoneData.Count();i++)
			{
			if (BoneData[i].Node != NULL)
				{
				bonecount++;
				for (int ccount = 0; ccount < BoneData[i].CrossSectionList.Count();ccount++)
					crosscount++;
				}
			}

//build bounding box list for hit testing;
		Tab<Box3> BBoxList;
		BBoxList.ZeroCount();
		for ( i =0;i<BoneData.Count();i++) 
			{
			Point3 l1,l2;

	
			Box3 b;
			float Outer,l = 0.0f;
			if (BoneData[i].Node != NULL)
				{
				if ((BoneData[i].flags & BONE_SPLINE_FLAG) 	&& (BoneData[i].Node != NULL))

					{
					ObjectState tos;
					tos = BoneData[i].Node->EvalWorldState(RefFrame);
//get bounding box
					tos.obj->GetDeformBBox(RefFrame,b);
					
					Interval valid;
//watje 10-7-99 212059
					Matrix3 ntm = Inverse(BoneData[i].tm);
//					Matrix3 ntm = BoneData[i].Node->GetObjTMBeforeWSM(RefFrame,&valid);

//					Point3 pt = p * ntm  * bmd->InverseBaseTM;

//					b = b*Inverse(BoneData[i].tm);
					b = b * ntm * bmd->InverseBaseTM;

	
					}
				else
					{
	
//					GetEndPointsLocal(bmd,t,l1, l2, i);
					Interval valid;
//watje 3-11-99
					GetEndPoints(bmd,RefFrame,l1, l2, i);

//					BoneData[i].EndPoint1Control->GetValue(RefFrame,&l1,valid);
//					BoneData[i].EndPoint2Control->GetValue(RefFrame,&l2,valid);

//					Matrix3 ntm = BoneData[i].Node->GetObjTMBeforeWSM(RefFrame,&valid);

//					l1 = l1 * ntm * bmd->InverseBaseTM;
//					l2 = l2 * ntm * bmd->InverseBaseTM;
					b.Init();
					b.MakeCube(l1,1.0f);
					b += l2;

					}
				for (int ccount = 0; ccount < BoneData[i].CrossSectionList.Count();ccount++)
					{
					float inner;

					GetCrossSectionRanges(inner, Outer, i, ccount);
					if (inner>Outer) Outer  = inner;
					if (Outer > l ) l = Outer;
					}
				b.EnlargeBy(l);


				}

			BBoxList.Append(1,&b,1);

			}	

//Get largest radius

//New Fallof method
 		for (i=0; i<nv; i++) {
//get total distance

			float TotalDistance = 0.0f;
			Point3 p,BoneCenter;		
			if (!bmd->VertexData[i]->modified)
				{

//				VertexData[i]->d.ZeroCount();
				p = os->obj->GetPoint(i);

				int FullStrength =0;
				for (int j =0;j<BoneData.Count();j++) 
					{	
					if (BoneData[j].Node != NULL) 

						{
						if (BBoxList[j].Contains(p)) 
//						if (1) 
							{
							int Bone;
							float Influence = 1.0f;
							Bone = j;
							Point3 l1,l2;



							GetEndPoints(bmd,t,l1, l2, j);
						

							float LineU,SplineU = 0.0f;
							Point3 op,otan;
							int cid,sid;
							if ((BoneData[j].flags & BONE_SPLINE_FLAG) && (BoneData[j].Node != NULL))
								{
//3-29-99								ShapeObject *pathOb = NULL;
//								ObjectState os = BoneData[j].Node->EvalWorldState(RefFrame);
//								pathOb = (ShapeObject*)os.obj;

								Interval valid;
//watje 10-7-99 212059
								Matrix3 ntm = BoneData[j].tm;
//								Matrix3 ntm = BoneData[j].Node->GetObjTMBeforeWSM(RefFrame,&valid);

//watje 10-7-99 212059
								ntm =bmd->BaseTM * ntm;
//								ntm =bmd->BaseTM * Inverse(ntm);


//								Influence = SplineToPoint(p,pathOb,LineU,op,otan,cid,sid,ntm);
								Influence = SplineToPoint(p,
														  &BoneData[j].referenceSpline,
														  LineU,op,otan,cid,sid,ntm);

								SplineU = LineU;
								}
							else
								{

								Influence = LineToPoint(p,l1,l2,LineU);
								}

//find cross section that bound this point
							int StartCross = 0, EndCross = 0;
							float tu = ModifyU(t,LineU,  j, sid);

							for (int ccount = 0; ccount < BoneData[j].CrossSectionList.Count();ccount++)
								{
								if (BoneData[j].CrossSectionList[ccount].u>=tu)
									{
									EndCross =ccount;
									ccount = BoneData[j].CrossSectionList.Count();
									}
								}
							StartCross = EndCross -1;
	
							if (StartCross == -1)
								{
								StartCross = 0;
								EndCross++;
								}
							Influence = ComputeInfluence(t,Influence,LineU, j,StartCross, EndCross,sid);

							if (Influence != 0.0f)
								{
								VertexInfluenceListClass td;
								td.Bones = Bone;
								td.Influences = Influence;
								td.normalizedInfluences = -1.0f;

								td.u = SplineU;
								td.Tangents = otan;
								td.OPoints = op;
								td.SubCurveIds = cid;
								td.SubSegIds = sid;
								if (!(BoneData[Bone].flags & BONE_LOCK_FLAG))
									{
									int found = -1;
									for (int vdcount = 0; vdcount < bmd->VertexData[i]->d.Count();vdcount++)
										{
										if (bmd->VertexData[i]->d[vdcount].Bones == Bone)
											{
											bmd->VertexData[i]->d[vdcount] = td;
											found = bmd->VertexData[i]->d.Count();
											}
										}
									if (found == -1)
										bmd->VertexData[i]->d.Append(1,&td,1);

									}

								}
							else
								{
								for (int vdcount = 0; vdcount < bmd->VertexData[i]->d.Count();vdcount++)
									{
									if (bmd->VertexData[i]->d[vdcount].Bones == j)
										{
										bmd->VertexData[i]->d.Delete(vdcount,1);
										vdcount = bmd->VertexData[i]->d.Count();
										}
									}
								}


							}

						else
							{
							if (!(BoneData[j].flags & BONE_LOCK_FLAG))
								{
								for (int vdcount = 0; vdcount < bmd->VertexData[i]->d.Count();vdcount++)
									{
									if (bmd->VertexData[i]->d[vdcount].Bones == j)
										{
										bmd->VertexData[i]->d.Delete(vdcount,1);
										vdcount = bmd->VertexData[i]->d.Count();
										}
									}
								}

							}


						}
					else
						{
						for (int vdcount = 0; vdcount < bmd->VertexData[i]->d.Count();vdcount++)
							{
							if (bmd->VertexData[i]->d[vdcount].Bones == j)
								{
								bmd->VertexData[i]->d.Delete(vdcount,1);
								vdcount = bmd->VertexData[i]->d.Count();
								}
							}

						}
					}
				}

			}

}



void BonesDefMod::RecomputeBone(BoneModData *bmd, int BoneIndex, TimeValue t, ObjectState *os)

{

		if (BoneData[BoneIndex].Node == NULL) return;

		BuildCache(bmd, BoneIndex,  t, os);
//watje 9-7-99  198721 
		bmd->reevaluate = FALSE;
		int nv = os->obj->NumPoints();

			

//Get largest radius

//New Fallof method
 		for (int i=0; i<nv; i++) {
//get total distance
			float TotalDistance = 0.0f;
			Point3 p,BoneCenter;		
			if (!bmd->VertexData[i]->modified)
				{

				p = os->obj->GetPoint(i);

				int FullStrength =0;
				int j = BoneIndex;
					{	
					if ((j < BoneData.Count()) && (BoneData[j].Node != NULL))
						{
						int Bone;
						float Influence = 1.0f;
						Bone = j;
						Point3 l1,l2;

						GetEndPoints(bmd,t,l1, l2, j);

						float LineU,SplineU = 0.0f;
						Point3 op,otan;
						int cid,sid;
						if (1) 
							{
							Influence = bmd->DistCache[i].dist;
							LineU = bmd->DistCache[i].u;
							SplineU = LineU;
							cid = bmd->DistCache[i].SubCurveIds;
							sid = bmd->DistCache[i].SubSegIds;
							otan = bmd->DistCache[i].Tangents;
							op = bmd->DistCache[i].OPoints;
//find cross section that bound this point
							int StartCross = 0, EndCross = 0;
							float tu = ModifyU(t,LineU,  j, sid);

							for (int ccount = 0; ccount < BoneData[j].CrossSectionList.Count();ccount++)
								{
								if (BoneData[j].CrossSectionList[ccount].u>=tu)
									{
									EndCross =ccount;
									ccount = BoneData[j].CrossSectionList.Count();
									}
								}
							StartCross = EndCross -1;
	
							if (StartCross == -1)
								{
								StartCross = 0;
								EndCross++;
								}

							Influence = ComputeInfluence(t,Influence,LineU, j,StartCross, EndCross, sid);


							if (Influence != 0.0f)
								{
								VertexInfluenceListClass td;
								td.Bones = Bone;
								td.Influences = Influence;
								td.normalizedInfluences = -1.0f;
								td.u = SplineU;
								td.Tangents = otan;
								td.OPoints = op;
								td.SubCurveIds = cid;
								td.SubSegIds = sid;

								BOOL found = FALSE;
								if (!(BoneData[Bone].flags & BONE_LOCK_FLAG))
									{

									for (int bic = 0; bic < bmd->VertexData[i]->d.Count(); bic++)
										{
										if (bmd->VertexData[i]->d[bic].Bones == Bone)
											{
											bmd->VertexData[i]->d[bic] = td;
											found = TRUE;
											bic = bmd->VertexData[i]->d.Count();
											}
										}
		
									if (!found)
										bmd->VertexData[i]->d.Append(1,&td,1);
									}

								}
							else
								{
								if (!(BoneData[Bone].flags & BONE_LOCK_FLAG))
									{
									BOOL found = FALSE;
									for (int bic = 0; bic < bmd->VertexData[i]->d.Count(); bic++)
										{
										if (bmd->VertexData[i]->d[bic].Bones == Bone)
											{
											bmd->VertexData[i]->d.Delete(bic,1);
											found = TRUE;
											bic = bmd->VertexData[i]->d.Count();

											}
										}
									}

								}
							}
						else
							{
							for (int bic = 0; bic < bmd->VertexData[i]->d.Count(); bic++)
								{
								if (bmd->VertexData[i]->d[bic].Bones == j)
									{
									bmd->VertexData[i]->d.Delete(bic,1);
									bic = bmd->VertexData[i]->d.Count();
									}
								}
	
							}

						}

					}


				}

			}


}


void BonesDefMod::DumpVertexList()

{
/*
for (int i=0; i<bmd->VertexData.Count(); i++) 
	{
	DebugPrint("Vertex %d ",i);
	for (int j=0; j<bmd->VertexData[i]->d.Count(); j++) 
		{
//		int bid = VertexData[i]->d[j].Bones;
		float inf = RetrieveNormalizedWeight(bmd,i, j);
		DebugPrint("%d/%f ",bmd->VertexData[i]->d[j].Bones,inf);

		}
	DebugPrint("\n");

	}
*/
}
void BonesDefMod::LockThisBone(int bid)
{
/*
for (int i=0; i<bmd->VertexData.Count(); i++) 
	{
	for (int j=0; j<bmd->VertexData[i]->d.Count(); j++) 
		{
		if (bmd->VertexData[i]->d[j].Bones == bid)
			{
			bmd->VertexData[i]->d[j].Influences = RetrieveNormalizedWeight(bmd,i, j);
			bmd->VertexData[i]->d[j].normalizedInfluences = bmd->VertexData[i]->d[j].Influences;
			}

		}
	}
*/
}

float BonesDefMod::RetrieveNormalizedWeight(BoneModData *bmd, int vid, int bid)
{
//need to reqeight based on remainder
double tempdist=0.0f;
double w;

int bd = bmd->VertexData[vid]->d[bid].Bones;

if (BoneData[bd].Node == NULL)
	{
	bmd->VertexData[vid]->d[bid].normalizedInfluences = 0.0f;
	bmd->VertexData[vid]->d[bid].Influences = 0.0f;
	return 0.0f;
	}

if (bmd->VertexData[vid]->d[bid].normalizedInfluences != -1.0f) return bmd->VertexData[vid]->d[bid].normalizedInfluences;

//if more than one bone use a weigthed system
if (bmd->VertexData[vid]->d.Count() >1) 
	{
	double remainder = 0.0f; 
	double offset =0.0f;
	tempdist = 0.0f;
	for (int j=0; j<bmd->VertexData[vid]->d.Count(); j++) 
		{
		float infl = bmd->VertexData[vid]->d[j].Influences;
		int bone=bmd->VertexData[vid]->d[j].Bones;
		if (!(BoneData[bone].flags & BONE_LOCK_FLAG))
			tempdist += infl;
		else 
			offset += infl;

		}
	offset = 1.0f-offset;
	double vinflu = bmd->VertexData[vid]->d[bid].Influences;
	int bn=bmd->VertexData[vid]->d[bid].Bones;
	if 	(!(BoneData[bn].flags & BONE_LOCK_FLAG))
		w = ((bmd->VertexData[vid]->d[bid].Influences)/tempdist) *offset;
	else w = bmd->VertexData[vid]->d[bid].Influences;
	}
else if (bmd->VertexData[vid]->d.Count() == 1) 
	{
//if only one bone and absolute control set to it to max control
	if ( (BoneData[bmd->VertexData[vid]->d[0].Bones].flags & BONE_ABSOLUTE_FLAG) )
//		&& !(bmd->VertexData[vid]->modified) )
		{
//		VertexData[vid].d[0].Influences = 1.0f;
		w = 1.0f;
		}
	else w = bmd->VertexData[vid]->d[0].Influences;


	}
return (float)w;
}

void BonesDefMod::UnlockBone(BoneModData *bmd,TimeValue t, ObjectState *os)
	{
//loop through verts and remove and associations to thos bone
	for (int i=0;i<bmd->VertexDataCount;i++)
		{
		for (int bic = 0; bic < bmd->VertexData[i]->d.Count(); bic++)
			{
			if (bmd->VertexData[i]->d[bic].Bones == ModeBoneIndex)
				{
				bmd->VertexData[i]->d.Delete(bic,1);
				bic = bmd->VertexData[i]->d.Count();
				}
			}

		}
//loop through find all verts in radius and set them to unmodified

	if (BoneData[ModeBoneIndex].Node == NULL) return;

	cacheValid = FALSE;
	bmd->CurrentCachePiece = -1;


	BuildCache(bmd, ModeBoneIndex,  t, os);
//watje 9-7-99  198721 
	bmd->reevaluate=TRUE;
	int nv =  os->obj->NumPoints();


//Get largest radius

//New Fallof method
	for (i=0; i<nv; i++) {
//get total distance
		float TotalDistance = 0.0f;
		Point3 p,BoneCenter;		
		p = os->obj->GetPoint(i);

		int FullStrength =0;
		int j = ModeBoneIndex;
		if ((j < BoneData.Count()) && (BoneData[j].Node != NULL))
			{
			int Bone;
			float Influence = 1.0f;
			Bone = j;
			Point3 l1,l2;

			GetEndPoints(bmd,t,l1, l2, j);

			float LineU,SplineU = 0.0f;
			Point3 op,otan;
			int cid,sid;
			Influence = bmd->DistCache[i].dist;
			LineU = bmd->DistCache[i].u;
			SplineU = LineU;
			cid = bmd->DistCache[i].SubCurveIds;
			sid = bmd->DistCache[i].SubSegIds;
			otan = bmd->DistCache[i].Tangents;
			op = bmd->DistCache[i].OPoints;
//find cross section that bound this point
			int StartCross = 0, EndCross = 0;
			float tu = ModifyU(t,LineU,  j, sid);

			for (int ccount = 0; ccount < BoneData[j].CrossSectionList.Count();ccount++)
				{
				if (BoneData[j].CrossSectionList[ccount].u>=tu)
					{
					EndCross =ccount;
					ccount = BoneData[j].CrossSectionList.Count();
					}
				}
			StartCross = EndCross -1;
	
			if (StartCross == -1)
				{
				StartCross = 0;
				EndCross++;
				}

			Influence = ComputeInfluence(t,Influence,LineU, j,StartCross, EndCross, sid);
			if (Influence > 0.0f)
				{
				bmd->VertexData[i]->modified = FALSE;

				}
			}
		}


	}


void BonesDefMod::UpdateTMCacheTable(BoneModData *bmd, TimeValue t, Interval& valid)
{
	if (bmd == NULL) return;



	if (bmd->tempTableL1.Count() != BoneData.Count())
		bmd->tempTableL1.SetCount(BoneData.Count());

	if (bmd->tempTableL2.Count() != BoneData.Count())
		bmd->tempTableL2.SetCount(BoneData.Count());

	if (bmd->tempTableL1ObjectSpace.Count() != BoneData.Count())
		bmd->tempTableL1ObjectSpace.SetCount(BoneData.Count());

	if (bmd->tempTableL2ObjectSpace.Count() != BoneData.Count())
		bmd->tempTableL2ObjectSpace.SetCount(BoneData.Count());


	if (bmd->tmCacheToBoneSpace.Count() != BoneData.Count())
		bmd->tmCacheToBoneSpace.SetCount(BoneData.Count());

	if (bmd->tmCacheToObjectSpace.Count() != BoneData.Count())
		bmd->tmCacheToObjectSpace.SetCount(BoneData.Count());
	for (int j =0;j<BoneData.Count();j++)
		{
		if (BoneData[j].Node != NULL)
			{
			Point3 l1, l2;

			Interval v;
			BoneData[j].EndPoint1Control->GetValue(t,&l1,v);
			BoneData[j].EndPoint2Control->GetValue(t,&l2,v);
	

			bmd->tempTableL1[j] = l1* Inverse(BoneData[j].tm) * Inverse(bmd->BaseTM);
			bmd->tempTableL2[j] = l2* Inverse(BoneData[j].tm) * Inverse(bmd->BaseTM);


//if BID then 
			Matrix3 ntm;
			ntm = BoneData[j].Node->GetObjTMBeforeWSM(t,&valid);

			if (bindNode)
				{
				xRefTM = bindNode->GetObjectTM(t);

				BoneData[j].temptm = bmd->BaseTM * BoneData[j].tm * ntm * 
					initialXRefTM * Inverse(xRefTM) * bmd->InverseBaseTM;
				}
			else BoneData[j].temptm = bmd->BaseTM * BoneData[j].tm * ntm * bmd->InverseBaseTM; 

//			BoneData[j].temptm = bmd->BaseTM * BoneData[j].tm * ntm * bmd->InverseBaseTM;

//cache for for matrices
			bmd->tmCacheToBoneSpace[j] = bmd->BaseTM * Inverse(ntm);
			bmd->tmCacheToObjectSpace[j] = ntm * bmd->InverseBaseTM;

			bmd->tempTableL1ObjectSpace[j] = l1 * bmd->tmCacheToObjectSpace[j];
			bmd->tempTableL2ObjectSpace[j] = l2 * bmd->tmCacheToObjectSpace[j];

			}
		}


}




class XRefEnumProc : public DependentEnumProc 
	{
      public :
      virtual int proc(ReferenceMaker *rmaker); 
      INodeTab Nodes;              
	  BOOL nukeME;
	};

int XRefEnumProc::proc(ReferenceMaker *rmaker) 
	{ 
	if (rmaker->SuperClassID()==BASENODE_CLASS_ID)    
			{
            Nodes.Append(1, (INode **)&rmaker);            
			nukeME = TRUE;
			}
     return 0;              
	}	

BOOL RecurseXRefTree(INode *n, INode *target)
{
for (int i = 0; i < n->NumberOfChildren(); i++)
	{

	INode *child = n->GetChildNode(i);

	if (child == target) 
		{
		return TRUE;
		}
	else RecurseXRefTree(child,target);
	
	}
return FALSE;
}

void BonesDefMod::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc) {
	if (!mc->localData) return;
//6-18-99
	BoneModData *bmd = (BoneModData *) mc->localData;			
	switch (message) {
		case REFMSG_OBJECT_CACHE_DUMPED:
			{
			if (partID & PART_TOPO)
//6-18-99
				if (!bmd->inputObjectIsNURBS)
//watje 9-7-99  198721 
					bmd->reevaluate=TRUE;
			}
		}

}


void BonesDefMod::UpdateEndPointDelta()
{
int deform;
Interval iv;
pblock_param->GetValue(PB_ALWAYS_DEFORM,0,deform,iv);
endPointDelta.SetCount(BoneData.Count());
if (!deform)
	{
	for (int j =0;j<BoneData.Count();j++)
		{
		if (BoneData[j].Node != NULL)
			{
			Class_ID bid(BONE_CLASS_ID,0);
			Matrix3 ntm =BoneData[j].Node->GetObjectTM(RefFrame);	
			BoneData[j].tm = Inverse(ntm);
//copy initial reference spline into our spline
			ObjectState sos = BoneData[j].Node->EvalWorldState(RefFrame);
			if (sos.obj->ClassID() == bid)  
				{
//now need to look at child and move
//loop through children looking for a matching name
				int childCount;
				childCount = BoneData[j].Node->NumberOfChildren();
				INode *childNode = NULL;
				for (int ci = 0; ci < childCount; ci++)
					{
					childNode = BoneData[j].Node->GetChildNode(ci);
					TSTR childName;
					childName = childNode->GetName();
					if (childName == BoneData[j].name)
						{
						Point3 l2(0.0f,0.0f,0.0f);
						Matrix3 ChildTM = childNode->GetObjectTM(RefFrame);
						l2 = l2 * ChildTM;
						l2 = l2 * BoneData[j].tm; 
						Point3 d;
						BoneData[j].EndPoint2Control->GetValue(0,&d,iv,CTRL_ABSOLUTE);
						ci = childCount;
						endPointDelta[j] = d-l2;

						}
							
					}
				}



			}
		}

	}	

}

void BonesDefMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{
	Interval valid = FOREVER;
	TimeValue tps = GetTicksPerFrame();


    XRefEnumProc dep;              
	dep.nukeME = FALSE;
	EnumDependents(&dep);
	
	INode *XRefNode = dep.Nodes[0];
	INode *rootNode = GetCOREInterface()->GetRootNode();
	int xct = rootNode->GetXRefFileCount();


	for (int xid = 0; xid < xct; xid++)
		{
		INode *xroot = rootNode->GetXRefTree(xid);
		BOOL amIanXRef = RecurseXRefTree(xroot,XRefNode);
		if (amIanXRef)
			{
			INode *tempBindNode = rootNode->GetXRefParent(xid);
			if ((tempBindNode) && (bindNode!=tempBindNode))
				{
				BOOL isThereAnInitialMatrix=FALSE;
				TSTR there("InitialMatrix");
				TSTR entry;
//check if inode has initial matricx property if use that else
				if (bindNode!=NULL)
					{
					bindNode->GetUserPropBool(there,isThereAnInitialMatrix);
					}
				tempBindNode->GetUserPropBool(there,isThereAnInitialMatrix);
				if (isThereAnInitialMatrix) 
					{
//GetUserPropFloat(const TSTR &key,float &val)
					Point3 r1,r2,r3,r4;
					entry.printf(_T("r1x"));
					tempBindNode->GetUserPropFloat(entry,r1.x);
					entry.printf(_T("r1y"));
					tempBindNode->GetUserPropFloat(entry,r1.y);
					entry.printf(_T("r1z"));
					tempBindNode->GetUserPropFloat(entry,r1.z);
					entry.printf(_T("r2x"));
					tempBindNode->GetUserPropFloat(entry,r2.x);
					entry.printf(_T("r2y"));
					tempBindNode->GetUserPropFloat(entry,r2.y);
					entry.printf(_T("r2z"));
					tempBindNode->GetUserPropFloat(entry,r2.z);
					entry.printf(_T("r3x"));
					tempBindNode->GetUserPropFloat(entry,r3.x);
					entry.printf(_T("r3y"));
					tempBindNode->GetUserPropFloat(entry,r3.y);
					entry.printf(_T("r3z"));
					tempBindNode->GetUserPropFloat(entry,r3.z);
					entry.printf(_T("r4x"));
					tempBindNode->GetUserPropFloat(entry,r4.x);
					entry.printf(_T("r4y"));
					tempBindNode->GetUserPropFloat(entry,r4.y);
					entry.printf(_T("rz"));
					tempBindNode->GetUserPropFloat(entry,r4.z);
					initialXRefTM.SetRow(0,r1);
					initialXRefTM.SetRow(1,r2);
					initialXRefTM.SetRow(2,r3);
					initialXRefTM.SetRow(3,r4);

					}
				else 
					{
					initialXRefTM = tempBindNode->GetObjectTM(t);
					tempBindNode->SetUserPropBool(there,TRUE);
					Point3 r1,r2,r3,r4;
					r1 = initialXRefTM.GetRow(0);
					r2 = initialXRefTM.GetRow(1);
					r3 = initialXRefTM.GetRow(2);
					r4 = initialXRefTM.GetRow(3);
					entry.printf(_T("r1x"));
					tempBindNode->SetUserPropFloat(entry,r1.x);
					entry.printf(_T("r1y"));
					tempBindNode->SetUserPropFloat(entry,r1.y);
					entry.printf(_T("r1z"));
					tempBindNode->SetUserPropFloat(entry,r1.z);
					entry.printf(_T("r2x"));
					tempBindNode->SetUserPropFloat(entry,r2.x);
					entry.printf(_T("r2y"));
					tempBindNode->SetUserPropFloat(entry,r2.y);
					entry.printf(_T("r2z"));
					tempBindNode->SetUserPropFloat(entry,r2.z);
					entry.printf(_T("r3x"));
					tempBindNode->SetUserPropFloat(entry,r3.x);
					entry.printf(_T("r3y"));
					tempBindNode->SetUserPropFloat(entry,r3.y);
					entry.printf(_T("r3z"));
					tempBindNode->SetUserPropFloat(entry,r3.z);
					entry.printf(_T("r4x"));
					tempBindNode->SetUserPropFloat(entry,r4.x);
					entry.printf(_T("r4y"));
					tempBindNode->SetUserPropFloat(entry,r4.y);
					entry.printf(_T("rz"));
					tempBindNode->SetUserPropFloat(entry,r4.z);

					}
				bindNode = tempBindNode;

				}
			}
		}




//loop through bone nodes to get new interval
	float ef;
	pblock_param->GetValue(PB_EFFECT,t,ef,valid);


	pblock_param->GetValue(PB_LOCK_BONE,t,LockBone,valid);
	pblock_param->GetValue(PB_ABSOLUTE_INFLUENCE,t,AbsoluteInfluence,valid);

	pblock_param->GetValue(PB_FILTER_VERTICES,t,FilterVertices,valid);
	pblock_param->GetValue(PB_FILTER_BONES,t,FilterBones,valid);
	pblock_param->GetValue(PB_FILTER_ENVELOPES,t,FilterEnvelopes,valid);

	FilterVertices = !FilterVertices;
	FilterBones = !FilterBones;
	FilterEnvelopes = !FilterEnvelopes;


	pblock_param->GetValue(PB_DRAW_ENVELOPES,t,DrawEnvelopes,valid);
	pblock_param->GetValue(PB_DRAW_VERTICES,t,DrawVertices,valid);


	pblock_param->GetValue(PB_RADIUS,t,Radius,valid);
	pblock_param->GetValue(PB_FEATHER,t,Feather,valid);

	pblock_param->GetValue(PB_PROJECT_THROUGH,t,ProjectThrough,valid);
	pblock_param->GetValue(PB_FALLOFF,t,Falloff,valid);

	pblock_param->GetValue(PB_REF_FRAME,t,RefFrame,valid);
	pblock_param->GetValue(PB_ALWAYS_DEFORM,t,AlwaysDeform,valid);


	RefFrame = RefFrame*tps;


//build each instance local data
	if (ip != NULL)
		{
		ModContextList mcList;
		INodeTab nodes;
		if (mc.localData == NULL)
			{
			ip->GetModContexts(mcList,nodes);

			for (int i = 0; i < nodes.Count(); i++)
				{
				BoneModData *d  = new BoneModData();
				d->BaseTM = nodes[i]->GetObjectTM(RefFrame);
				d->InverseBaseTM = Inverse(d->BaseTM);
				UpdateTMCacheTable(d,t,valid);

				if ((OldVertexDataCount != 0) && (i==0))
					{
					d->VertexDataCount = OldVertexDataCount;
					d->VertexData.SetCount(OldVertexDataCount);
					for (int j = 0; j < OldVertexDataCount; j++)
						{
						VertexListClass *vc;
						vc = new VertexListClass;
						d->VertexData[j] = vc;

						d->VertexData[j]->selected = OldVertexData[j]->selected;
						d->VertexData[j]->modified = OldVertexData[j]->modified;
						d->VertexData[j]->LocalPos = OldVertexData[j]->LocalPos;
						d->VertexData[j]->d = OldVertexData[j]->d;
						}
					for (j = 0; j < OldVertexDataCount; j++)
						delete (OldVertexData[j]);

					OldVertexData.ZeroCount();

					OldVertexDataCount = -1;
					}
				mcList[i]->localData = d;


				}
			NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
			valid.SetInstant(t);
			os->obj->UpdateValidity(GEOM_CHAN_NUM,valid);	

			return;

			}
//create a back pointer to the container entry				
		}

	BoneModData *bmd = (BoneModData *) mc.localData;			


	if (bmd == NULL) return;

//6-18-99
	bmd->inputObjectIsNURBS = os->obj->ClassID() == EDITABLE_SURF_CLASS_ID;	


	if (BoneData.Count() == 0) return;

	UpdateTMCacheTable(bmd,t,valid);

//	DebugPrint("num verts %d bmd %d\n",os->obj->NumPoints(),bmd->VertexDataCount);


	if (os->obj->NumPoints()==0) 
		{
		os->obj->UpdateValidity(GEOM_CHAN_NUM,valid);	
		return;
		}




	if ((!painting) || (bmd->forceUpdate))
		{


		bmd->forceUpdate = FALSE;	

		if (splineChanged)
			{
			if (whichSplineChanged < BoneData.Count())
				{
				if (BoneData[whichSplineChanged].Node)
					{

					ObjectState os = BoneData[whichSplineChanged].Node->EvalWorldState(t);
					if (os.obj->SuperClassID()!=SHAPE_CLASS_ID)
						{
//if not convert it to a regular node
						BoneData[whichSplineChanged].flags &= ~BONE_SPLINE_FLAG;
//watje 9-7-99  198721 
						bmd->reevaluate=TRUE;
						Point3 a,b;
						Interval v;
						BuildMajorAxis(BoneData[whichSplineChanged].Node,a,b); 

						BoneData[whichSplineChanged].EndPoint1Control->SetValue(0,a,TRUE,CTRL_ABSOLUTE);
						BoneData[whichSplineChanged].EndPoint2Control->SetValue(0,b,TRUE,CTRL_ABSOLUTE);
						cacheValid = FALSE;
						}
					else
						{
						ObjectState sos = BoneData[whichSplineChanged].Node->EvalWorldState(RefFrame);
						BezierShape bShape;
						ShapeObject *shape = (ShapeObject *)sos.obj;
						if (shape)
							{
							if(shape->CanMakeBezier())
//watje 9-7-99  195862 
								shape->MakeBezier(RefFrame, bShape);
//								shape->MakeBezier(t, bShape);
							else {
								PolyShape pShape;
//watje 9-7-99  195862 
								shape->MakePolyShape(RefFrame, pShape);
//								shape->MakePolyShape(t, pShape);
								bShape = pShape;	// UGH -- Convert it from a PolyShape -- not good!
								}
							}

						if ((shape) && (bShape.splineCount >0) &&
							(bShape.splines[0]->Segments() != BoneData[whichSplineChanged].referenceSpline.Segments()))
							{
							BoneData[whichSplineChanged].referenceSpline = *bShape.splines[0];
//watje 9-7-99  198721 
							bmd->reevaluate=TRUE;
							}

/*						ObjectState sos = BoneData[whichSplineChanged].Node->EvalWorldState(RefFrame);
						SplineShape *shape = (SplineShape *)sos.obj;
//check for a topo change is so recreate reference spline and recompuetall
						if ((shape) && (shape->shape.splines[0]->Segments() != BoneData[whichSplineChanged].referenceSpline.Segments()))
							{
							BoneData[whichSplineChanged].referenceSpline = *shape->shape.splines[0];
							reevaluate = TRUE;
							}
*/
						}

					}


				}
			splineChanged = FALSE;
			}




		if (unlockVerts)
			{
			unlockVerts = FALSE;
//watje 9-7-99  198721 
			bmd->reevaluate=TRUE;
			for (int i=0;i<bmd->VertexDataCount;i++)
				{
				if (bmd->selected[i])
					bmd->VertexData[i]->modified = FALSE;
				}
			}

		if (updateP)
			{
			updateP = FALSE;
			UpdateP(bmd);
			}

		if ((((t == (RefFrame)) && (BoneMoved)) && (!AlwaysDeform))  || forceRecomuteBaseNode)
//	if (((t == (RefFrame *tps)) && (BoneMoved)) )
			{
//readjust TMs for frame 0
			BoneMoved = FALSE;
//watje 9-7-99  198721 
			bmd->reevaluate=TRUE;
			cacheValid = FALSE;
			forceRecomuteBaseNode = FALSE;
			bmd->CurrentCachePiece = -1;

			if (ip != NULL)
				{
				ModContextList mcList;
				INodeTab nodes;
				ip->GetModContexts(mcList,nodes);

				for (int i = 0; i < nodes.Count(); i++)
					{
					BoneModData *d  = (BoneModData *) mcList[i]->localData;
					if (d)
						{
						d->BaseTM = nodes[i]->GetObjectTM(RefFrame);
						d->InverseBaseTM = Inverse(d->BaseTM);
						}
					}
				}

	
			for (int j =0;j<BoneData.Count();j++)
				{
				if (BoneData[j].Node != NULL)
					{
					Class_ID bid(BONE_CLASS_ID,0);
					Matrix3 ntm =BoneData[j].Node->GetObjectTM(RefFrame);	
					BoneData[j].tm = Inverse(ntm);
//copy initial reference spline into our spline
					ObjectState sos = BoneData[j].Node->EvalWorldState(RefFrame);
					if (BoneData[j].flags & BONE_SPLINE_FLAG)
						{
//						SplineShape *shape = (SplineShape *)sos.obj;
//						BoneData[j].referenceSpline = *shape->shape.splines[0];
						BezierShape bShape;
						ShapeObject *shape = (ShapeObject *)sos.obj;
						if (shape)
							{
							if(shape->CanMakeBezier())
								shape->MakeBezier(t, bShape);
							else {
								PolyShape pShape;
								shape->MakePolyShape(t, pShape);
								bShape = pShape;	// UGH -- Convert it from a PolyShape -- not good!
								}
							if (bShape.splineCount >0) 
								BoneData[j].referenceSpline = *bShape.splines[0];
							}

						}
//need to readjust the child endpoints for bone type objects also
					else if (sos.obj->ClassID() == bid)  
						{
//now need to look at child and move
//loop through children looking for a matching name
						int childCount;
						childCount = BoneData[j].Node->NumberOfChildren();
						INode *childNode = NULL;
						for (int ci = 0; ci < childCount; ci++)
							{
							childNode = BoneData[j].Node->GetChildNode(ci);
							TSTR childName;
							childName = childNode->GetName();
							if (childName == BoneData[j].name)
								{
								Point3 l2(0.0f,0.0f,0.0f);
								Matrix3 ChildTM = childNode->GetObjectTM(RefFrame);
								l2 = l2 * ChildTM;
								l2 = l2 * BoneData[j].tm; 
								if (j < endPointDelta.Count())
									{
									l2 = l2+endPointDelta[j];
									BoneData[j].EndPoint2Control->SetValue(0,&l2,TRUE,CTRL_ABSOLUTE);
									}
								else BoneData[j].EndPoint2Control->SetValue(0,&l2,TRUE,CTRL_ABSOLUTE);
								ci = childCount;

								}
							
							}
						}



					}
				}

			}
		if (reloadSplines)
			{
			for (int j =0;j<BoneData.Count();j++)
				{
				if (BoneData[j].Node != NULL)
					{
//copy initial reference spline into our spline
					if (BoneData[j].flags & BONE_SPLINE_FLAG)
						{
						ObjectState sos = BoneData[j].Node->EvalWorldState(RefFrame);
//						SplineShape *shape = (SplineShape *)sos.obj;
//						BoneData[j].referenceSpline = *shape->shape.splines[0];
						BezierShape bShape;
						ShapeObject *shape = (ShapeObject *)sos.obj;
						if (shape)
							{
							if(shape->CanMakeBezier())
								shape->MakeBezier(t, bShape);
							else {
								PolyShape pShape;
								shape->MakePolyShape(t, pShape);
								bShape = pShape;	// UGH -- Convert it from a PolyShape -- not good!
								}
							if (bShape.splineCount >0) 
								BoneData[j].referenceSpline = *bShape.splines[0];
							}

						}


					}
				}
			reloadSplines = FALSE;

			}

		if (bmd->selected.GetSize() != os->obj->NumPoints())
			bmd->selected.SetSize(os->obj->NumPoints(),TRUE);




//get selected bone	
		int rsel = 0;

		rsel = SendMessage(GetDlgItem(hParam,IDC_LIST1),
					LB_GETCURSEL ,0,0);
		int tsel = ConvertSelectedListToBoneID(rsel);


	


		if ( (tsel>=0) && (ip && ip->GetSubObjectLevel() == 1) )
			{
			ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hParam,IDC_EFFECTSPIN));
//			spin2->SetIndeterminate(TRUE);

			if ((!spin2->IsIndeterminate()) && (ef != bmd->effect))
				{
				bmd->effect = ef;
				SetSelectedVertices(bmd,tsel, bmd->effect);
				}
			}

//set validty based on TM's
		for (int i =0;i<BoneData.Count();i++)
			{
			if (BoneData[i].Node != NULL) 
				{
				BoneData[i].Node->GetObjectTM(t,&valid);
				if (BoneData[i].flags & BONE_SPLINE_FLAG)
					{
					ObjectState osp = BoneData[i].Node->EvalWorldState (t);
					valid &= osp.obj->ObjectValidity (t);
					}
				}

			}

//get selected bone	
		if (bmd->VertexDataCount != os->obj->NumPoints())
			{
//readjust vertices using nearest vertices as sample
//DebugPrint("    Reevaluating \n");
//watje 9-7-99  198721 
			bmd->reevaluate = TRUE;
			bmd->CurrentCachePiece = -1;
			}

		if (unlockBone)
			{
			unlockBone = FALSE;
//watje 9-7-99  198721 
			bmd->reevaluate = TRUE;
			UnlockBone(bmd,t,os);
			}



		if ((ip) || (bmd->reevaluate))
			{
			if ( (bmd->reevaluate) )
				{
				RecomputeAllBones(bmd,t,os);
				cacheValid = FALSE;
				bmd->CurrentCachePiece = -1;

				}	

			else if ( (ModeBoneIndex!=-1) && (ip && ip->GetSubObjectLevel() == 1))//&& (ModeEdit ==1) )
				{
				RecomputeBone(bmd,ModeBoneIndex,t,os);
				}
			}

		for (i = 0; i < os->obj->NumPoints(); i++)
			bmd->VertexData[i]->LocalPos = os->obj->GetPoint(i);

		}

	valid &= LocalValidity(t);



	if ((t == RefFrame) && (!AlwaysDeform))
		{
		}
	else
		{
		BonesDefDeformer deformer(this,bmd,t);
		os->obj->Deform(&deformer, TRUE);
		}

//		}
//		else
//		{
//		StaticBonesDefDeformer staticdeformer(this,bmd,t);
//		os->obj->Deform(&staticdeformer, TRUE);

//		}
	
	if ((inPaint) && (!painting))
		{
		bmd->hitState = os;
		if (os->obj->IsSubClassOf(triObjectClassID))
			{
			bmd->isMesh = TRUE;
			bmd->isPatch = FALSE;
			}
		else if (os->obj->IsSubClassOf(patchObjectClassID))
			{
			bmd->isMesh = FALSE;
			bmd->isPatch = TRUE;
			}

		else if (os->obj->IsParamSurface()) 
			{
			
			}
		else 

			{
//ask if can convert to mesh
			if (os->obj->CanConvertToType(triObjectClassID))
				{
				bmd->isMesh = TRUE;
				bmd->isPatch = FALSE;
				}
			else
				{
				bmd->isMesh = FALSE;
				bmd->isPatch = FALSE;
				}

			}
		}

//	DebugPrint("End bonesdef\n");

	os->obj->UpdateValidity(GEOM_CHAN_NUM,valid);	

	}
	


#define ID_CHUNK 0x1000

IOResult BonesDefMod::SaveLocalData(ISave *isave, LocalModData *pld)
{

//IOResult	res;
ULONG		nb;

BoneModData *bmd = (BoneModData*)pld;

//isave->BeginChunk(ID_CHUNK);
//res = isave->Write(&p->id, sizeof(int), &nb);
//isave->EndChunk();

	isave->BeginChunk(BASE_TM_CHUNK);
	bmd->BaseTM.Save(isave);
	isave->EndChunk();



	int c = bmd->VertexDataCount;
//save vertex influence info
	isave->BeginChunk(VERTEX_COUNT_CHUNK);
	isave->Write(&c,sizeof(c),&nb);
	isave->EndChunk();
	
	isave->BeginChunk(VERTEX_DATA_CHUNK);
	for (int i = 0; i < c; i++)
		{
//write number of influences
		int ic;
		ic = bmd->VertexData[i]->d.Count();
		isave->Write(&ic,sizeof(ic),&nb);
		int save_i;
		float save_f;
		BOOL save_b;
		save_b = bmd->VertexData[i]->modified;
		isave->Write(&save_b,sizeof(save_b),&nb);

		for (int j = 0; j < ic; j++)
			{
			save_i = bmd->VertexData[i]->d[j].Bones;
			save_f = bmd->VertexData[i]->d[j].Influences;
			isave->Write(&save_i,sizeof(save_i),&nb);
			isave->Write(&save_f,sizeof(save_f),&nb);

			save_i = bmd->VertexData[i]->d[j].SubCurveIds;
			isave->Write(&save_i,sizeof(save_i),&nb);
			save_i = bmd->VertexData[i]->d[j].SubSegIds;
			isave->Write(&save_i,sizeof(save_i),&nb);

			save_f = bmd->VertexData[i]->d[j].u;
			isave->Write(&save_f,sizeof(save_f),&nb);

			Point3 save_p;
			save_p = bmd->VertexData[i]->d[j].Tangents;
			isave->Write(&save_p,sizeof(save_p),&nb);

			save_p = bmd->VertexData[i]->d[j].OPoints;
			isave->Write(&save_p,sizeof(save_p),&nb);



			}

		}
	isave->EndChunk();


return IO_OK;
}

IOResult BonesDefMod::LoadLocalData(ILoad *iload, LocalModData **pld)

{
	IOResult	res;
	ULONG		nb;

	BoneModData *bmd = new BoneModData();
	*pld = bmd;
	bmd->effect = -1.0f;


//	int id;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
//			case ID_CHUNK:
//				iload->Read(&id,sizeof(int), &nb);
//				break;

			case BASE_TM_CHUNK: 
				{
				bmd->BaseTM.Load(iload);
				bmd->InverseBaseTM = Inverse(bmd->BaseTM);
				break;
				}


			case VERTEX_COUNT_CHUNK:
				{
				int c;
				iload->Read(&c,sizeof(c),&nb);
				bmd->VertexDataCount = c;
//				if (VertexData != NULL) delete VertexData;
//				VertexData = new VertexListClass[c]; 
				bmd->VertexData.ZeroCount();
				bmd->VertexData.SetCount(c);
				for (int i=0; i<c; i++) {
					VertexListClass *vc;
					vc = new VertexListClass;
					bmd->VertexData[i] = vc;
					bmd->VertexData[i]->modified = FALSE;
					bmd->VertexData[i]->selected = FALSE;
 					bmd->VertexData[i]->d.ZeroCount();
//					VertexData[i].Influences.ZeroCount();
					}

				break;

				}
			case VERTEX_DATA_CHUNK:
				{
				for (int i=0; i < bmd->VertexDataCount; i++)
					{
					int c;
					BOOL load_b;
					iload->Read(&c,sizeof(c),&nb);
					bmd->VertexData[i]->d.SetCount(c);

//					VertexData[i].Influences.SetCount(c);
					iload->Read(&load_b,sizeof(load_b),&nb);
					bmd->VertexData[i]->modified = load_b;
					float load_f;
					int load_i;
					Point3 load_p;
					for (int j=0; j<c; j++) {
						iload->Read(&load_i,sizeof(load_i),&nb);
						iload->Read(&load_f,sizeof(load_f),&nb);
 						bmd->VertexData[i]->d[j].Bones = load_i;
						bmd->VertexData[i]->d[j].Influences =load_f;
						bmd->VertexData[i]->d[j].normalizedInfluences = -1.0f;

						iload->Read(&load_i,sizeof(load_i),&nb);
						bmd->VertexData[i]->d[j].SubCurveIds =load_i;
						iload->Read(&load_i,sizeof(load_i),&nb);
						bmd->VertexData[i]->d[j].SubSegIds =load_i;

						iload->Read(&load_f,sizeof(load_f),&nb);
 						bmd->VertexData[i]->d[j].u = load_f;

						iload->Read(&load_p,sizeof(load_p),&nb);
 						bmd->VertexData[i]->d[j].Tangents = load_p;

						iload->Read(&load_p,sizeof(load_p),&nb);
 						bmd->VertexData[i]->d[j].OPoints = load_p;


						}
					}

				break;

				}

			}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
		}


    int c = bmd->VertexDataCount;
//add m crossection
	bmd->selected.SetSize(c);
	bmd->selected.ClearAll();
	bmd->CurrentCachePiece = -1;

//	for (i=0; i<c; i++) 
//		sel[i] = FALSE;

return IO_OK;

}