/*===========================================================================*\
 | 
 |  FILE:	PureEdit.cpp
 |			Skeleton project and code for a full modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 31-3-99
 | 
\*===========================================================================*/

#include "PureModifier.h"



/*===========================================================================*\
 |	ModifyObject will do all the work in a full modifier
 |  This includes casting objects to their correct form, doing modifications
 |  changing their parameters, etc
\*===========================================================================*/

void SkeletonModifier::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{
	// Get parameters from pblock
	float sparam = 0.0f; 
	Interval valid = FOREVER;
	pblock->GetValue(skpurem_simple_param, t, sparam, valid);

	float decay;

	// Get number of deformable points
	int modCount = os->obj->NumPoints();

	// Loop through, adding the simple parameter's value to each one
	// We support softselection - using the PointSelection call
	for(int i=0;i<modCount;i++)
	{
		Point3 original = os->obj->GetPoint(i);

		// Get softselection, if applicable
		decay = 1.0f;
		if(os->obj->GetSubselState()!=0) decay = os->obj->PointSelection(i);

		original.x += (sparam*decay);
		original.y += (sparam*decay);
		original.z += (sparam*decay);

		os->obj->SetPoint(i,original);
	}


	// Update all the caches etc
	os->obj->UpdateValidity(GEOM_CHAN_NUM,valid);
	os->obj->PointsWereChanged();
}



/*===========================================================================*\
 |	NotifyInputChanged is called each time the input object is changed in some way
 |	We can find out how it was changed by checking partID and message
\*===========================================================================*/

void SkeletonModifier::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{

}

