/*===========================================================================*\
 | 
 |  FILE:	wM3_cache.cpp
 |			Weighted Morpher for MAX R3
 |			MorphCache class
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 3-10-98
 | 
\*===========================================================================*/

#include "wM3.h"


void morphCache::NukeCache()
{
	if(oPoints) delete [] oPoints;
	oPoints = NULL;
	if(oWeights) delete [] oWeights;
	oWeights = NULL;

	sel.SetSize(0,0);
	sel.ClearAll();

	CacheValid = FALSE;
}

void morphCache::MakeCache(Object *obj)
{
	Count = obj->NumPoints();
	oPoints = new Point3[Count];
	oWeights = new double[Count];

	sel.SetSize(Count);
	sel.ClearAll();

	for(int t=0;t<Count;t++)
	{
		oPoints[t] = obj->GetPoint(t);
		oWeights[t] = obj->GetWeight(t);
		
		sel.Set( t, (obj->PointSelection(t)>0.0f)?1:0 );
	}

	CacheValid = TRUE;
}

BOOL morphCache::AreWeCached()
{
	return CacheValid;
}

morphCache::morphCache()
{
	CacheValid	= FALSE;
	oPoints		= NULL;
	oWeights	= NULL;
	Count = 0;
	sel.SetSize(0,0);
	sel.ClearAll();
}
