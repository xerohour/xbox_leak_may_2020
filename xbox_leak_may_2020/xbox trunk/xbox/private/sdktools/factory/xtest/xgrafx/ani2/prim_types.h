//
//	prim_types.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __PRIM_TYPES_H__
#define __PRIM_TYPES_H__


enum PrimitiveTypes
{
	pt_Sphere    = 0,
	pt_Cylinder  = 1,
	pt_Box       = 2,
	pt_Torus     = 3,
	pt_Cone      = 4,
	pt_SurfOfRev = 5,
	pt_NoTypes   = 6
};

const float OO_PRIM_TRANS_SCALE_X = 0.004131f;
const float OO_PRIM_TRANS_SCALE_Y = 0.008252f;
const float OO_PRIM_TRANS_SCALE_Z = 0.004421f;
const float PRIM_TRANS_DELTA_X =    -27.844984f;
const float PRIM_TRANS_DELTA_Y =    -0.228729f;
const float PRIM_TRANS_DELTA_Z =    0.497086f;

struct SphereInst
{
	short tx,ty,tz;
	char idVersion;
	char idPosAnim;
	char idRotAnim;
	float fRad;
};

struct SphereVers
{
	char nSegs;
};

struct CylinderInst
{
	char  idQuat;
	short tx,ty,tz;
	char  idVersion;
	char  idPosAnim;
	char  idRotAnim;
	float fRad;
	float fHalfHeight;
};

struct CylinderVers
{
	char nHeightSeg;
	char nSides;
};

struct BoxInst
{
	char  idQuat;
	short tx,ty,tz;
	char  idVersion;
	char  idPosAnim;
	char  idRotAnim;
	float fLen;
	float fHeight;
	float fWidth;
};

struct TorusInst
{
	char idQuat;
	short tx,ty,tz;
	char idVersion;
	char idPosAnim;
	char idRotAnim;
	float fRad1;
};

struct TorusVers
{
	float fRatio;
	char nSegs;
	char nSides;
};

struct ConeInst
{
	char idQuat;
	short tx,ty,tz;
	char idVersion;
	char idPosAnim;
	char idRotAnim;
};

struct ConeVers
{
	float fRad1;
	float fRad2;
	float fHeight;
	char nHeightSeg;
	char nSides;
};

#define MAX_SOR_PTS 19
struct SurfOfRevInst
{
	char idQuat;
	short tx,ty,tz;
	char idVersion;
	char idPosAnim;
	char idRotAnim;
};

struct SurfOfRevPt
{
	float x,y,z;
	char flags;
};

struct SurfOfRevVers
{
	SurfOfRevPt pts[MAX_SOR_PTS];
	float ax,ay,az;
	float px,py,pz;
	char nSegs;
	char nPts;
};

enum SurfOfRevPtFlags
{
	sr_Smooth = 1
};

#define MAX_POS_SAMPLES 30
#define NUM_POS_SEQ 32
const float OO_POS_ANIM_SCALE_X = 0.002755f;
const float OO_POS_ANIM_SCALE_Y = 0.002755f;
const float OO_POS_ANIM_SCALE_Z = 0.002440f;
const float POS_ANIM_DELTA_X = -0.159046f;
const float POS_ANIM_DELTA_Y = -0.741611f;
const float POS_ANIM_DELTA_Z = 2.155624f;

struct PosAnimSeq
{
	char posIds[MAX_POS_SAMPLES*3];
};

#define MAX_ROT_SAMPLES 30
#define NUM_ROT_SEQ 29
struct RotAnimSeq
{
	char quatIds[MAX_ROT_SAMPLES*3];
};

#endif // __PRIM_TYPES_H__