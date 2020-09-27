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



Point3 SkeletonWSMDeformer::Map(int i, Point3 p)
	{
	Point3 pt = p * tm;

		pt.x += (sparam_obj * sparam_wsm);
		pt.y += (sparam_obj * sparam_wsm);
		pt.z += (sparam_obj * sparam_wsm);

	return pt * itm;
	}