/*===========================================================================*\
 | 
 |  FILE:	ObjectSnap.cpp
 |			Skeleton project and code for an Object Snap extension
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 2-2-99
 | 
\*===========================================================================*/

#include "ObjectSnap.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonOSnapClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create( BOOL loading ) { return new SkeletonSnap; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID() { return OSNAP_CLASS_ID; }
	Class_ID 		ClassID() { return OSNAP_CLASSID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY);  }
};

static SkeletonOSnapClassDesc SkeletonOSNCD;
ClassDesc* GetSkeletonOSnapDesc() {return &SkeletonOSNCD;}



/*===========================================================================*\
 |	Snap identifiers and object checking
\*===========================================================================*/

TSTR *SkeletonSnap::snapname(int index)
{
	return &name[index];
}

TSTR *SkeletonSnap::tooltip(int index)
{
	return &name[index];
}


SkeletonSnap::SkeletonSnap()
{
	tools = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_ICONS));
	masks = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MASK));
	name[DEFSNAP] = TSTR(GetString(IDS_SKELSNAP));
	markerdata[DEFSNAP] = OsnapMarker(CDPD_SIZE,custMarker_dat,custMarker_vis);
}

SkeletonSnap::~SkeletonSnap()
{
	DeleteObject(tools);
	DeleteObject(masks);
}

boolean SkeletonSnap::ValidInput(SClass_ID scid, Class_ID cid)
{
	boolean sc_ok = FALSE;
	sc_ok |= (scid == GEOMOBJECT_CLASS_ID)? TRUE : FALSE;
	return sc_ok;
}



/*===========================================================================*\
 |	Return our custom point
\*===========================================================================*/

OsnapMarker *SkeletonSnap::GetMarker(int index)
{
	return &(markerdata[index]);
} 



/*===========================================================================*\
 |	The actual Snapping function itself
\*===========================================================================*/

void SkeletonSnap::Snap(Object* pobj, IPoint2 *p, TimeValue t)
{	
	//local copy of the cursor position
	Point2 fp = Point2((float)p->x, (float)p->y);

	BOOL got_snap= FALSE; int i;


	// Check to see if the snap #1 is active
	if(	GetActive(DEFSNAP))
	{

		// A simple 'Snap to deformable point' snap routine
		if(pobj->IsDeformable())
		{
			int numPts = pobj->NumPoints();
			Point3 *checkPoints = new Point3[1];

			for(i=0;i<numPts;i++)
			{
				checkPoints[0] = pobj->GetPoint(i);
				if(CheckPotentialHit(checkPoints,0,fp))
					theman->RecordHit(new OsnapHit(checkPoints[0], this, DEFSNAP, NULL));
			}
		}
	
	}


}