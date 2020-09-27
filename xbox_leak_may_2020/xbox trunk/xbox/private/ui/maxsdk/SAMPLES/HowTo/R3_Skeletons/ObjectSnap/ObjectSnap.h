/*===========================================================================*\
 | 
 |  FILE:	ObjectSnap.h
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

#ifndef __OSNAPSKEL__H
#define __OSNAPSKEL__H

#include "max.h"
#include "osnapapi.h"
#include "simpobj.h"
#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	OSNAP_CLASSID		Class_ID(0x6ce9517c, 0x3c3133f7)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonOSnapDesc();


// Number of snaps supported
#define SNAPCOUNT	1

// Index's of snaps
#define DEFSNAP		0



/*===========================================================================*\
 |	Custom display point data -- a larger box than normal
\*===========================================================================*/

#define CDPD_SIZE	5

static IPoint3 custMarker_dat[CDPD_SIZE]=
{ 
 IPoint3(5,      5,      0),
 IPoint3(-5,      5,      0),
 IPoint3(-5,      -5,      0),
 IPoint3(5,      -5,      0),
 IPoint3(5,      5,      0)
};

static int custMarker_vis[CDPD_SIZE]=
{
	GW_EDGE_VIS,
	GW_EDGE_VIS,
	GW_EDGE_VIS,
	GW_EDGE_VIS,
	GW_EDGE_VIS
};

/*===========================================================================*\
 |	SkeletonSnap class defn
\*===========================================================================*/

class SkeletonSnap : public Osnap 
{

private:
	//A local array of subsnap names 
	TSTR name[SNAPCOUNT]; 

	// Static list of our custom markers
	OsnapMarker markerdata[SNAPCOUNT];

	// Appearance bitmaps
	HBITMAP tools; 
	HBITMAP masks; 

public:

	SkeletonSnap();
	~SkeletonSnap();

	// The number of snaps we support, and their names
	int numsubs(){return SNAPCOUNT;} 
	TSTR *snapname(int index); 
	TSTR *tooltip(int index);

	// plugin naming and category
	TCHAR* Category(){return GetString(IDS_CATEGORY);}
	Class_ID ClassID() { return OSNAP_CLASSID; }

	// Check to see if the input object is something we support
	boolean ValidInput(SClass_ID scid, Class_ID cid);

	// Get the type of marker to display. NULL = default.
	OsnapMarker *GetMarker(int index);

	// Accelerator key mapping
	WORD AccelKey(int index){return 0;};
	
	// Get the visual display of our snap
	HBITMAP getTools(){return tools;} 
	HBITMAP getMasks(){return masks;} 
	
	// Snap the point
	void Snap(Object* pobj, IPoint2 *p, TimeValue t);
};

#endif