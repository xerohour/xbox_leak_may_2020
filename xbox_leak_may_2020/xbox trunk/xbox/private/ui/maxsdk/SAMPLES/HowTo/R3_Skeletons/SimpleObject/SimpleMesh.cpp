/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Simple Object
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 8-3-99
 | 
\*===========================================================================*/

#include "SimpleObj.h"



/*===========================================================================*\
 |	Basic collapse types
 |  We just pass this onto SimpleObject, as we are nothing special here..
\*===========================================================================*/


Object* SkeletonSimpObj::ConvertToType(TimeValue t, Class_ID obtype)
{
		return SimpleObject::ConvertToType(t, obtype);
}

int SkeletonSimpObj::CanConvertToType(Class_ID obtype)
{
		return SimpleObject::CanConvertToType(obtype);
}


void SkeletonSimpObj::GetCollapseTypes(Tab<Class_ID> &clist, Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
}




/*===========================================================================*\
 |	Mouse creation - mearly measure a length between two points
 |  and pass it into our pblock2
\*===========================================================================*/

int SkeletonObjCreateCallBack::proc(ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat ) {
	float r;
	Point3 p1, center;

	if (msg == MOUSE_FREEMOVE)
	{
		vpt->SnapPreview(m, m, NULL, SNAP_IN_3D);
	}


	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {

		// point one - where we measure from
		case 0:
			sso->suspendSnap = TRUE;				
			sp0 = m;
			p0 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			mat.SetTrans(p0);
			break;

		// point two - where we measure to in worldspace
		case 1:
			mat.IdentityMatrix();
			
			p1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			r = Length(p1-p0);
			mat.SetTrans(p0);

			sso->pblock2->SetValue(simpo_simple_param, 0, r);

			if (msg==MOUSE_POINT) {
				sso->suspendSnap = FALSE;
				return (Length(m-sp0)<3)?CREATE_ABORT:CREATE_STOP;
			}
			break;					   
		}
	} else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}

	return TRUE;
}

static SkeletonObjCreateCallBack soCreateCB;

CreateMouseCallBack* SkeletonSimpObj::GetCreateMouseCallBack() 
{
	soCreateCB.SetObj(this);
	return(&soCreateCB);
}




/*===========================================================================*\
 |	Mesh creation - create the actual object
 |	Just a flat panel of 2 faces, with texture mapping if required
\*===========================================================================*/

void SkeletonSimpObj::BuildMesh(TimeValue t)
{
	float sz; BOOL genUVs;
	ivalid = FOREVER;	
	pblock2->GetValue(simpo_simple_param,t,sz,ivalid);
	pblock2->GetValue(simpo_genuv, 0, genUVs, ivalid);

	mesh.setNumVerts(4);
	mesh.setNumFaces(2);
	mesh.InvalidateTopologyCache();


	mesh.setVert(0, Point3((float)sz,(float)sz,0.0f));
	mesh.setVert(1, Point3((float)-sz,(float)sz,0.0f));
	mesh.setVert(2, Point3((float)sz,(float)-sz,0.0f));
	mesh.setVert(3, Point3((float)-sz,(float)-sz,0.0f));


	if(genUVs)
	{
		mesh.setNumTVerts(4) ;
		mesh.setNumTVFaces(2);		

		mesh.setTVert(0, Point3(1.0f,1.0f,0.0f));
		mesh.setTVert(1, Point3(0.0f,1.0f,0.0f));
		mesh.setTVert(2, Point3(1.0f,0.0f,0.0f));
		mesh.setTVert(3, Point3(0.0f,0.0f,0.0f));

		mesh.tvFace[0].setTVerts(0,1,2);
		mesh.tvFace[1].setTVerts(1,3,2);
	}


	mesh.faces[0].setVerts(0,1,2);
	mesh.faces[0].setEdgeVisFlags(1,0,1);
	mesh.faces[1].setVerts(1,3,2);
	mesh.faces[1].setEdgeVisFlags(1,1,0);


	// Invalidate caches, so the object knows it must update
	mesh.InvalidateGeomCache();
	mesh.BuildStripsAndEdges();
}



/*===========================================================================*\
 |	Turn on/off UVW Mapping automatically
\*===========================================================================*/

BOOL SkeletonSimpObj::HasUVW() { 
	BOOL genUVs;
	pblock2->GetValue(simpo_genuv, 0, genUVs, FOREVER);
	return genUVs; 
	}

void SkeletonSimpObj::SetGenUVW(BOOL sw) {  
	if (sw==HasUVW()) return;
	pblock2->SetValue(simpo_genuv, 0, sw);				
	}
