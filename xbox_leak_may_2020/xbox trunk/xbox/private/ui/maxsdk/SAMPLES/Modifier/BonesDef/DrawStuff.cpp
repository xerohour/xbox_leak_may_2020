
 /**********************************************************************
 
	FILE: DrawStuff.cpp

	DESCRIPTION:  Stuff to draw bones and envelopes

	CREATED BY: Peter Watje

	HISTORY: 8/5/98




 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "iparamm.h"
#include "shape.h"
#include "spline3d.h"
#include "splshape.h"
#include "linshape.h"

// This uses the linked-list class templates
#include "linklist.h"
#include "bonesdef.h"


void BonesDefMod::LoadAxis(Point3 st, float dist, Matrix3 tm, Point3 *plist)
	{
	Point3 AxisStart,AxisEnd;

	AxisStart = st;
	AxisEnd = st;
	AxisStart.x -= dist;
	AxisEnd.x += dist;
	plist[0] = AxisStart * tm;
	plist[1] = AxisEnd * tm;

	AxisStart = st;
	AxisEnd = st;
	AxisStart.y -= dist;
	AxisEnd.y += dist;
	plist[2] = AxisStart * tm;
	plist[3] = AxisEnd * tm;

	AxisStart = st;
	AxisEnd = st;
	AxisStart.z -= dist;
	AxisEnd.z += dist;
	plist[4] = AxisStart * tm;
	plist[5] = AxisEnd * tm;


	}



void BonesDefMod::DrawCrossSection(Point3 a, Point3 align, float length,  Matrix3 tm, GraphicsWindow *gw)

{

#define NUM_SEGS	16

Point3 plist[NUM_SEGS+1];
Point3 mka,mkb,mkc,mkd;

align = Normalize(align);
	{
	int ct = 0;
	float angle = TWOPI/float(NUM_SEGS) ;
	Matrix3 rtm = RotAngleAxisMatrix(align, angle);
	Point3 p(0.0f,0.0f,0.0f);
	if (align.x == 1.0f)
		{
		p.z = length;
		}
	else if (align.y == 1.0f)
		{
		p.x = length;
		}
	else if (align.z == 1.0f)
		{
		p.y = length;
		}
	else if (align.x == -1.0f)
		{
		p.z = -length;
		}
	else if (align.y == -1.0f)
		{
		p.x = -length;
		}
	else if (align.z == -1.0f)
		{
		p.y = -length;
		}
	else 
		{
		p = Normalize(align^Point3(1.0f,0.0f,0.0f))*length;
		}

	for (int i=0; i<NUM_SEGS; i++) {
		p = p * rtm;
		plist[ct++] = p;
		}

	p = p * rtm;
	plist[ct++] = p;


	for (i=0; i<NUM_SEGS+1; i++) 
		{
		plist[i].x += a.x;
		plist[i].y += a.y;
		plist[i].z += a.z;
//		plist[i] = plist[i] * tm;
		}
	}
mka = plist[15];
mkb = plist[3];
mkc = plist[7];
mkd = plist[11];

gw->polyline(NUM_SEGS+1, plist, NULL, NULL, 0);
gw->marker(&mka,BIG_BOX_MRKR);
gw->marker(&mkb,BIG_BOX_MRKR);
gw->marker(&mkc,BIG_BOX_MRKR);
gw->marker(&mkd,BIG_BOX_MRKR);

}



void BonesDefMod::DrawCrossSectionNoMarkers(Point3 a, Point3 align, float length, GraphicsWindow *gw)

{

#define NNUM_SEGS	8

Point3 plist[NNUM_SEGS+1];
Point3 mka,mkb,mkc,mkd;

align = Normalize(align);
	{
	int ct = 0;
	float angle = TWOPI/float(NNUM_SEGS) ;
	Matrix3 rtm = RotAngleAxisMatrix(align, angle);
	Point3 p(0.0f,0.0f,0.0f);
	if (align.x == 1.0f)
		{
		p.z = length;
		}
	else if (align.y == 1.0f)
		{
		p.x = length;
		}
	else if (align.z == 1.0f)
		{
		p.y = length;
		}
	else if (align.x == -1.0f)
		{
		p.z = -length;
		}
	else if (align.y == -1.0f)
		{
		p.x = -length;
		}
	else if (align.z == -1.0f)
		{
		p.y = -length;
		}
	else 
		{
		p = Normalize(align^Point3(1.0f,0.0f,0.0f))*length;
		}

	for (int i=0; i<NNUM_SEGS; i++) {
		p = p * rtm;
		plist[ct++] = p;
		}

	p = p * rtm;
	plist[ct++] = p;


	for (i=0; i<NNUM_SEGS+1; i++) 
		{
		plist[i].x += a.x;
		plist[i].y += a.y;
		plist[i].z += a.z;
		}
	}
mka = plist[15];
mkb = plist[3];
mkc = plist[7];
mkd = plist[11];

gw->polyline(NNUM_SEGS+1, plist, NULL, NULL, 0);

}


void BonesDefMod::DrawEndCrossSection(Point3 a, Point3 align, float length,  Matrix3 tm, GraphicsWindow *gw)

{

#define NUM_SEGS	16
Point3 p_edge[4];
Point3 plist[NUM_SEGS+1];
GetCrossSectionLocal(a,align, length,  p_edge);



//align = Normalize(align);
	
int ct = 0;
float angle = TWOPI/float(NUM_SEGS) *.5f;

align = Normalize(p_edge[1]-a);

Matrix3 rtm = RotAngleAxisMatrix(align, angle);
Point3 p(0.0f,0.0f,0.0f);
p = p_edge[0]-a;

plist[0] = p;

for (int i=1; i<(NUM_SEGS+1); i++) 
	{
	p = p * rtm;
	plist[i] = p;
	}

for (i=0; i<(NUM_SEGS+1); i++) 
	{
	plist[i].x += a.x;
	plist[i].y += a.y;
	plist[i].z += a.z;
//	plist[i] = plist[i] * tm;
	}
	
gw->polyline((NUM_SEGS+1), plist, NULL, NULL, 0);


align = Normalize(p_edge[2]-a);

rtm = RotAngleAxisMatrix(align, angle);


p = p_edge[1]-a;

plist[0] = p;

for ( i=1; i<(NUM_SEGS+1); i++) 
	{
	p = p * rtm;
	plist[i] = p;
	}

for (i=0; i<(NUM_SEGS+1); i++) 
	{
	plist[i].x += a.x;
	plist[i].y += a.y;
	plist[i].z += a.z;
//	plist[i] = plist[i] * tm;
	}
	
gw->polyline((NUM_SEGS+1), plist, NULL, NULL, 0);


}

void BonesDefMod::GetCrossSectionLocal(Point3 a, Point3 align, float length, Point3 *p_edge)
{
#define GNUM_SEGS	4

Point3 plist[GNUM_SEGS];

align = Normalize(align);

	{
	int ct = 0;
	float angle = TWOPI/float(GNUM_SEGS) ;
	Matrix3 rtm = RotAngleAxisMatrix(align, angle);
	Point3 p(0.0f,0.0f,0.0f);
	if (align.x == 1.0f)
		{
		p.z = length;
		}
	else if (align.y == 1.0f)
		{
		p.x = length;
		}
	else if (align.z == 1.0f)
		{
		p.y = length;
		}
	else if (align.x == -1.0f)
		{
		p.z = -length;
		}
	else if (align.y == -1.0f)
		{
		p.x = -length;
		}
	else if (align.z == -1.0f)
		{
		p.y = -length;
		}
	else 
		{
		p = Normalize(align^Point3(1.0f,0.0f,0.0f))*length;
		}

	for (int i=0; i<GNUM_SEGS; i++) {
		p = p * rtm;
		plist[ct++] = p;
		}



	for (i=0; i<GNUM_SEGS; i++) 
		{
		plist[i].x += a.x;
		plist[i].y += a.y;
		plist[i].z += a.z;
		p_edge[i] = plist[i];
		}
	}
}

void BonesDefMod::GetCrossSection(Point3 a, Point3 align, float length,  Matrix3 tm,  Point3 *p_edge)

{

#define GNUM_SEGS	4

Point3 plist[GNUM_SEGS];

align = Normalize(align);

	{
	int ct = 0;
	float angle = TWOPI/float(GNUM_SEGS) ;
	Matrix3 rtm = RotAngleAxisMatrix(align, angle);
	Point3 p(0.0f,0.0f,0.0f);
	if (align.x == 1.0f)
		{
		p.z = length;
		}
	else if (align.y == 1.0f)
		{
		p.x = length;
		}
	else if (align.z == 1.0f)
		{
		p.y = length;
		}
	else if (align.x == -1.0f)
		{
		p.z = -length;
		}
	else if (align.y == -1.0f)
		{
		p.x = -length;
		}
	else if (align.z == -1.0f)
		{
		p.y = -length;
		}

	else 
		{
		p = Normalize(align^Point3(1.0f,0.0f,0.0f))*length;
		}

	for (int i=0; i<GNUM_SEGS; i++) {
		p = p * rtm;
		plist[ct++] = p;
		}



	for (i=0; i<GNUM_SEGS; i++) 
		{
		plist[i].x += a.x;
		plist[i].y += a.y;
		plist[i].z += a.z;
//		plist[i] = plist[i] * tm;
		p_edge[i] = plist[i];
		}
	}
}


void BonesDefMod::DrawEnvelope(Tab<Point3> a, Tab<float> length, int count, Matrix3 tm, GraphicsWindow *gw)
{
	
#define NUM_SEGS	16
Point3 plist[NUM_SEGS+1];
Point3 p_env[2];
Point3 pa_prev,pb_prev,pc_prev,pd_prev;

Point3 align = Normalize(a[1] - a[0]);

for (int j = 0; j < count; j++)
	{

	if (j == 0)
		{
//draw top arcs
		DrawEndCrossSection(a[j], align, length[j], tm, gw);
		}
	else if (j == (count -1))
		{
//draw bottom arcs
		Point3 align2 = Normalize(a[0] - a[1]);

		DrawEndCrossSection(a[j], align2, length[j], tm, gw);
		}

	Point3 p[4];

	GetCrossSection(a[j], align, length[j],  tm,  p);
	if (j == 0)
		{
		pa_prev =p[0];
		pb_prev =p[1];
		pc_prev =p[2];
		pd_prev =p[3];
		}
	else
		{
		p_env[0] = pa_prev;
		p_env[1] = p[0];
		gw->polyline(2, p_env, NULL, NULL, 0);
		pa_prev = p[0];

		p_env[0] = pb_prev;
		p_env[1] = p[1];
		gw->polyline(2, p_env, NULL, NULL, 0);
		pb_prev = p[1];

		p_env[0] = pc_prev;
		p_env[1] = p[2];
		gw->polyline(2, p_env, NULL, NULL, 0);
		pc_prev = p[2];


		p_env[0] = pd_prev;
		p_env[1] = p[3];
		gw->polyline(2, p_env, NULL, NULL, 0);
		pd_prev = p[3];

		

		}
	}



}

