/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Skeleton project and code for a Simple WSM modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 17-3-99
 | 
\*===========================================================================*/

#include "SimpleWSM.h"




/*===========================================================================*\
 |	Mouse creation - mearly measure a length between two points
 |  and pass it into our pblock2
\*===========================================================================*/

int SkeletonWSMObjCreateCallBack::proc(ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat ) {
	float r;
	Point3 p1, center;

	if (msg == MOUSE_FREEMOVE)
	{
		vpt->SnapPreview(m, m, NULL, SNAP_IN_3D);
	}


	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {

		case 0:
			sp0 = m;
			p0 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			mat.SetTrans(p0);
			break;


		case 1:
			mat.IdentityMatrix();
			
			p1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			r = Length(p1-p0);
			mat.SetTrans(p0);

			swo->pblock2->SetValue(simpwsmobj_radius_param, 0, r);

			if (msg==MOUSE_POINT) {
				return (Length(m-sp0)<3)?CREATE_ABORT:CREATE_STOP;
			}
			break;					   
		}
	} else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}

	return TRUE;
}

static SkeletonWSMObjCreateCallBack WSMCreateCB;

CreateMouseCallBack* SkeletonWSMObject::GetCreateMouseCallBack() 
{
	WSMCreateCB.SetObj(this);
	return(&WSMCreateCB);
}




/*===========================================================================*\
 |	Mesh creation - create the actual object
\*===========================================================================*/

void SkeletonWSMObject::BuildMesh(TimeValue t)
{
	float sz; 
	ivalid = FOREVER;	
	pblock2->GetValue(simpwsmobj_radius_param,t,sz,ivalid);

	mesh.setNumVerts(4);
	mesh.setNumFaces(2);
	mesh.InvalidateTopologyCache();


	mesh.setVert(0, Point3((float)sz,(float)sz,0.0f));
	mesh.setVert(1, Point3((float)-sz,(float)sz,0.0f));
	mesh.setVert(2, Point3((float)sz,(float)-sz,0.0f));
	mesh.setVert(3, Point3((float)-sz,(float)-sz,0.0f));


	mesh.faces[0].setVerts(0,1,2);
	mesh.faces[0].setEdgeVisFlags(1,0,1);
	mesh.faces[1].setVerts(1,3,2);
	mesh.faces[1].setEdgeVisFlags(1,1,0);

	mesh.InvalidateGeomCache();
	mesh.BuildStripsAndEdges();

}
