/*===========================================================================*\
 | 
 |  FILE:	AtmosShade.cpp
 |			Skeleton project and code for a Atmospheric effect
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 31-3-99
 | 
\*===========================================================================*/

#include "Atmospheric.h"



/*===========================================================================*\
 |	This method is called once per frame when the renderer begins.  
\*===========================================================================*/

void SkeletonAtmospheric::Update(TimeValue t, Interval& valid)
{
}


/*===========================================================================*\
 |	Called to initialize or clear up before and after rendering  
 |	These will get called once each, not per frame
\*===========================================================================*/

int SkeletonAtmospheric::RenderBegin(TimeValue t, ULONG flags)
{
	return 0;
}

int SkeletonAtmospheric::RenderEnd(TimeValue t)
{
	return 0;
}



/*===========================================================================*\
 |	The actual shading of our atmospheric space
 |	Creates a simple raycast, tests each gizmo, shades gizmo space in param color
\*===========================================================================*/

// Hacked out of Combustion, for demonstration purposes
inline BOOL IntersectSphere(
		Matrix3 tm, Ray &ray,float r)
{	
	Ray ray2;

	ray2 = ray;

	ray2.p   = ray2.p * tm;
	ray2.dir = VectorTransform(tm,ray2.dir);

	float a, b, c, ac4, b2;

	a = DotProd(ray2.dir,ray2.dir);
	b = DotProd(ray2.dir,ray2.p) * 2.0f;
	c = DotProd(ray2.p,ray2.p) - r*r;
	
	ac4 = 4.0f * a * c;
	b2 = b*b;

	if (ac4 > b2) return FALSE;

	return TRUE;
}


void SkeletonAtmospheric::Shade(
		ShadeContext& sc,const Point3& p0,const Point3& p1,
		Color& color, Color& trans, BOOL isBG)
{
	if(NumGizmos()<=0) return;

	// Get the color from our paramblock
	Color pColor;
	pblock->GetValue(skatmos_simple_param, sc.CurTime(), pColor, FOREVER);


	// Setup the ray to cast
	Point3 wp0, wp1, v;
	float len;
	Ray ray;

	wp0   = sc.PointTo(p0,REF_WORLD);
	wp1   = sc.PointTo(p1,REF_WORLD);
	ray.p = wp0;
	v     = wp1-wp0;
	len   = Length(v);
	if (len==0.0f) return;
	ray.dir = v/len;


	// Start out color is clear and black
	Color c;
	c.Black();
	float o = 0.0f;

	// For each gizmo, do some tracing
	// If we hit a sphere, centered on each gizmo's center, set to color
	for (int i=0; i<NumGizmos(); i++) 
	{
		INode *gizmo = GetGizmo(i);
		if(gizmo)
		{
			Matrix3 tm = Inverse(gizmo->GetObjTMAfterWSM(sc.CurTime()));
			if(IntersectSphere(tm,ray,20.0f)) { c=pColor; o=0.6f; }
		}
	}

	// Combine with incoming color.
	color += c-(color*o);
	trans *= 1.0f-o;
}