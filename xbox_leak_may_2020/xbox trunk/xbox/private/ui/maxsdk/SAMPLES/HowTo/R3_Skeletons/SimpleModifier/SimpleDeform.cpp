/*===========================================================================*\
 | 
 |  FILE:	SimpDeform.cpp
 |			Skeleton project and code for a Simple Modifier
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#include "SimpleMod.h"



/*===========================================================================*\
 |	Deformer class that does most of the work in a simple Modifier
\*===========================================================================*/

SkeletonDeformer::SkeletonDeformer() 
	{ 
	tm.IdentityMatrix();
	time = 0;	
	}


void SkeletonDeformer::SetAxis(Matrix3 &tmAxis)
	{
	Matrix3 itm = Inverse(tmAxis);
	tm    = tm*tmAxis;
	invtm =	itm*invtm;
	}


/*===========================================================================*\
 |	Map is called for every deformable point in the object
\*===========================================================================*/

Point3 SkeletonDeformer::Map(int i, Point3 p)
	{
	p = p * tm;

		p.x+= (bpt.x+def_sparam);
		p.y+= (bpt.y+def_sparam);
		p.z+= (bpt.z+def_sparam);

	p = p * invtm;
	return p;
	}


/*===========================================================================*\
 |	Constructor - copy params into this local class for Map-ing
\*===========================================================================*/

SkeletonDeformer::SkeletonDeformer(
		TimeValue t, ModContext &mc,
		Matrix3& modmat, Matrix3& modinv,
		float sparam) 
	{	
	this->def_sparam	= sparam;
	this->time			= t;

	Matrix3 mat;
	Interval valid;	

	tm = modmat;
	invtm = modinv;
	mat.IdentityMatrix();
	
	assert(mc.box);
	bbox = *mc.box;
	bpt = bbox.Center();

	} 