/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Helper object
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 16-3-99
 | 
\*===========================================================================*/

#include "Helper.h"




/*===========================================================================*\
 |	Mouse creation 
\*===========================================================================*/

int SkeletonHelperCreateCallBack::proc(ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat ) {
	float r;
	Point3 p1, center;

	if (msg == MOUSE_FREEMOVE)
	{
		vpt->SnapPreview(m, m, NULL, SNAP_IN_3D);
	}


	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {

		case 0:
			sho->suspendSnap = TRUE;				
			sp0 = m;
			p0 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			mat.SetTrans(p0);
			break;


		case 1:
			mat.IdentityMatrix();
			
			p1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			r = Length(p1-p0);
			mat.SetTrans(p0);

			sho->pblock2->SetValue(shelp_simple_param, 0, r);

			if (msg==MOUSE_POINT) {
				sho->suspendSnap = FALSE;
				return (Length(m-sp0)<3)?CREATE_ABORT:CREATE_STOP;
			}
			break;					   
		}
	} else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}

	return TRUE;
}

static SkeletonHelperCreateCallBack HelperCreateCB;

CreateMouseCallBack* SkeletontHelpObject::GetCreateMouseCallBack() 
{
	HelperCreateCB.SetObj(this);
	return(&HelperCreateCB);
}

void SkeletontHelpObject::SetExtendedDisplay(int flags)
{
	extDispFlags = flags;
}




/*===========================================================================*\
 |	Bounding Box calculation
\*===========================================================================*/

void SkeletontHelpObject::GetLocalBoundBox(
		TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) 
	{
	float sz; Interval ivalid = FOREVER;	
	pblock2->GetValue(shelp_simple_param,t,sz,ivalid);

	box = Box3(Point3((float)-sz,(float)-sz,(float)-sz), Point3((float)sz,(float)sz,(float)sz));
	}

void SkeletontHelpObject::GetWorldBoundBox(
		TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
	{
	Matrix3 tm;
	tm = inode->GetObjectTM(t);
	float sz; Interval ivalid = FOREVER;	
	pblock2->GetValue(shelp_simple_param,t,sz,ivalid);

	Point3 lo = tm.GetTrans();
	Point3 hi = tm.GetTrans();

	lo.x -= sz; lo.y -= sz; lo.z -= sz;
	hi.x += sz; hi.y += sz; hi.z += sz;
	 
	box = Box3(lo, hi);
	}



/*===========================================================================*\
 |	Hit Testing and Object Snapping 
\*===========================================================================*/

int SkeletontHelpObject::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) {
	Matrix3 tm(1);	
	HitRegion hitRegion;
	DWORD	savedLimits;
	float sz; Interval ivalid = FOREVER;	
	pblock2->GetValue(shelp_simple_param,t,sz,ivalid);

	Point3 pt1((float)sz,0.0f,0.0f), pt2((float)-sz,0.0f,0.0f), pt3(0.0f,(float)sz,0.0f), pt4(0.0f,(float)-sz,0.0f);

	vpt->getGW()->setTransform(tm);
	GraphicsWindow *gw = vpt->getGW();	
	Material *mtl = gw->getMaterial();

   	tm = inode->GetObjectTM(t);		
	MakeHitRegion(hitRegion, type, crossing, 4, p);

	gw->setRndLimits(((savedLimits = gw->getRndLimits())|GW_PICK)&~GW_ILLUM);
	gw->setHitRegion(&hitRegion);
	gw->clearHitCode();

	vpt->getGW()->setTransform(tm);
	vpt->getGW()->marker(&pt1,SM_CIRCLE_MRKR);
	vpt->getGW()->marker(&pt2,SM_CIRCLE_MRKR);
	vpt->getGW()->marker(&pt3,SM_CIRCLE_MRKR);
	vpt->getGW()->marker(&pt4,SM_CIRCLE_MRKR);

	gw->setRndLimits(savedLimits);
	
	if((hitRegion.type != POINT_RGN) && !hitRegion.crossing)
		return TRUE;
	return gw->checkHitCode();
	}

void SkeletontHelpObject::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt)
	{
	if(suspendSnap)
		return;

	}



/*===========================================================================*\
 |	Display the actual helper
\*===========================================================================*/

int SkeletontHelpObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) 
{
	Matrix3 tm(1);
	float sz; Interval ivalid = FOREVER;	
	pblock2->GetValue(shelp_simple_param,t,sz,ivalid);

	Point3 pt1((float)sz,0.0f,0.0f), pt2((float)-sz,0.0f,0.0f), pt3(0.0f,(float)sz,0.0f), pt4(0.0f,(float)-sz,0.0f);
	
	tm = inode->GetObjectTM(t);	
	vpt->getGW()->setTransform(tm);

	if(!inode->IsFrozen())
		vpt->getGW()->setColor(LINE_COLOR,GetUIColor(COLOR_POINT_OBJ));

	vpt->getGW()->marker(&pt1,SM_CIRCLE_MRKR);
	vpt->getGW()->marker(&pt2,SM_CIRCLE_MRKR);
	vpt->getGW()->marker(&pt3,SM_CIRCLE_MRKR);
	vpt->getGW()->marker(&pt4,SM_CIRCLE_MRKR);
	return(0);
}

ObjectState SkeletontHelpObject::Eval(TimeValue time)
{
	return ObjectState(this);
}
