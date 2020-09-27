// Patch1Test.cpp : Defines the entry point for the application.
//

//#include "stdafx.h"
#include "PhysicsCar.h"
#include "PhyTrack.h"
#include "moremath.h"
#include "vector.h"
#if GAME
#include "names.h"
#include "types.h"
#include "Game.h"
#include "CarGeometry.h"
#include "Car.h"
#endif

#define MAXITERATIONS                    7
#define ROOTCHECK_EPSLION                 0.005f
#define MAX_PATCH_SEARCHES               10
#define MAX_UV_STEP                      0.2f
#define MIN_CAR_DISTANCE_FROM_WALL       3.0f
#define SLOW_DOWN_MULTIPLIER             0.975f
#define DEFLECTION_MULTIPLIER            1.05f
#define MIN_DEFLECTION_FORCE            -0.20f
#define MIN_ACTIVE_WALL_DISTANCE         20.0f
#define MIN_WALL_COUNT                   12
#define ONE_FIFTH                        0.2f
#define WALL_COLLISION_DIST              0.1f
#define MIN_SPARK_SPEED                  5.0f
 
const float ONE = 1.0f;
const float TWO = 2.0f;
const AV4 U3 = {1.0f,0.0f,1.0f,0.0f};
const __declspec(align(16)) int ABSMASK[4] = {0x7FFFFFFF,0x7FFFFFFF,
                                              0x7FFFFFFF,0x7FFFFFFF};

//extern PhyTrack g_phyTrack;
extern float g_constraintX[3];


#if GAME
extern Car g_car[MAX_CAR_SLOTS];
extern int g_cars;
#endif


#define BEZIERBASIS 1
#if BEZIERBASIS
// Bezier basis
static float BMatrix[3][4] =
{
    {  1.0f, -2.0f,  1.0f, 0.0f },
    { -2.0f,  2.0f,  0.0f, 0.0f },
    {  1.0f,  0.0f,  0.0f, 0.0f }
};

#else
// B-spline basis
static float BMatrix[3][4] =
{
    {  0.5f, -1.0f,  0.5f, 0.0f },
    { -1.0f,  1.0f,  0.0f, 0.0f },
    {  0.5f,  0.5f,  0.0f, 0.0f }
};
#endif

void PhysicsCar::m_FindStartingPatch()
{
}

// %%% DEREK - look into storing off projected cps for each wheel in phycar (might save some work) - derek
void PhysicsCar::m_SearchForCurb(int tire)
{
}

void PhysicsCar::m_TrackPosition(short tire)
{
}


// Find the u & v values of point (0,0) on the current patch. Return TRUE if point is on the patch,
// return FALSE if not on the patch
short PhysicsCar::m_NewtonIteration(float cpProj[3][3][2], float &uValue, float &vValue)
{
	return (true);
}


// Always project the control points down the world's UP vector. The roads will
// always be pretty damn close to parallel with the XY plane
void PhysicsCar::m_FindProjectedControlPoints(short tire,
											  float cpts[NUM_S_CPS][NUM_T_CPS][3],
										      float cpProj[NUM_S_CPS][NUM_T_CPS][2])
{

}


//-------------------------------------------

void PhysicsCar::m_ComputeMxy(float cp[3][3][2], float Mxy[9][4])
{
}

void PhysicsCar::m_UpdateUV(float Mxy[9][4], float &uValue, float &vValue, float &rootCheck)
{
}



void PhysicsCar::m_ComputeRoadNormal(float s, float t, float cp[NUM_T_CPS][NUM_S_CPS][3],
									 float *normal)
{
}

void PhysicsCar::m_GetPatchIntersectionPoint(float s, float t, float cp[NUM_T_CPS][NUM_S_CPS][3],
											 float *roadPt)
{
}

float PhysicsCar::m_HeightOfRoad(float s, float t,
								 float cp[NUM_T_CPS][NUM_S_CPS][3])
{
	return 0;
}



void PhysicsCar::m_AddToEndOfWallCache()
{
}

void PhysicsCar::m_AddToFrontOfWallCache()
{
}

void PhysicsCar::m_UpdateActivePatchGroups()
{
	
	if (m_wallCount == 0)
	{
		return;
	}
	m_AddToFrontOfWallCache();
	m_AddToEndOfWallCache();
}

// m_DeterminePlaneIntersection returns true if there is an intersection,
// false otherwise.
short PhysicsCar::m_DeterminePlaneIntersection(float *plane)
{
#if 1
	float tmp[3];
	V3Norm(tmp, m_transform[FORWARD]);
	V3Mul(tmp, m_boxOffset);
	V3Add(tmp, m_pos);
#endif
	float carDist = V3Dot(plane, tmp) - plane[3];

	if (carDist > MIN_CAR_DISTANCE_FROM_WALL)
	{
		// if center of car is too far away from wall we trivially reject (no collision)
		return (false);
	}

	float xDist = m_length * 0.5f * V3Dot(m_transform[FORWARD], plane);
	float yDist = m_width * 0.5f * V3Dot(m_transform[SIDE], plane);

	if (((carDist + xDist + yDist) < 0.0f) ||
		((carDist + xDist - yDist) < 0.0f) ||
		((carDist - xDist + yDist) < 0.0f) ||
		((carDist - xDist - yDist) < 0.0f))
	{ 
		return (true);
	}
	return (false);
}


// m_FindSeparatingPlane returns TRUE if a separating plane exists (no collision) 
// or FALSE if there is no separating plane (we have a collision)
short PhysicsCar::m_FindSeparatingPlane(WallSegment *wallSeg, float timeStep, short saveOff)
{
	return (false);
}


// If saveOff is true and we find a new sep plane for a particular wall
// then the curr sep plane for that wall is saved off. Otherwise, we only save off
// on a full integrator step, not on sub-divided steps.
short PhysicsCar::m_CheckCollisionWithWall(int &activeWall, float timeStep, short saveOff)
{
	return (false);
}

void PhysicsCar::m_FindCollidingCorner(float *plane, int &corner, float *vel)
{	
}

// plane, velocity are in body space
void PhysicsCar::m_ApplyWallImpulse(float *planeBS, float *posVelBS, float *rotVelBS, 
									float *cornerVelBS, float *cornerBS)
{
}

void PhysicsCar::m_CheckForGroundingCorners()
{
}

short PhysicsCar::m_CheckCollisionWithCone()
{
	return (false);
}

void V2Mul2x2(float *dest, float *src1, float src2[4][4]) 
{
	register floatType x,y;
	x = src1[0];
	y = src1[1];
	dest[0] = x*src2[0][0] + y*src2[1][0];
	dest[1] = x*src2[0][1] + y*src2[1][1];
}

void V2Mul2x2T(float *dest, float *src1, float src2[4][4]) 
{
	register floatType x,y;
	x = src1[0];
	y = src1[1];
	dest[0] = x*src2[0][0] + y*src2[0][1];
	dest[1] = x*src2[1][0] + y*src2[1][1];
}


// This applies an impulse with a somewhat scaled down rotatinal component
// Also assume car is flat (which is probably not true
void BodgedImpulse(PhysicsCar *c1, float rx, float ry, PhysicsCar *c2, float rx2, float ry2, float *N, float j) 
{
}

// ---------------------------------------------------------------
// I think B[3][3] is in   M*c form 
int BoxIntersection(float B[4][4], float T[3], float a[3], float b[3])
{
	return 0;
}

//float pListIn[5][2] = { {2.22f, 0.996f}, {2.22f, -0.996f}, {-2.22f, -0.996f}, {-2.22f, 0.996f}, {2.22f, 0.996f}};
float pListInT[9][2];
float pListOut1[9][2];


int ClipVsPlane(float pin[][2], int points, float *PE, float pout[][2]) 
{
	return 0;
}


int GetCollisionManifoldCentroid(float B[4][4], float T[3], float a[3], float b[3], float *out) 
{
	return 0;
}



#define THRESHOLD 1.1f
#define MINSEPSPEED 0.5f
#define MAXSEP 1.5f
#define RR 1.02f 
#define RR2 1.001f
#define SPTEST 10.0f
#define SAFEDIST 7.0f
#define CAR_SIZE_ADJUST 0.15f

// Perform a check between 2 cars and resolve response
// Assumes that c1 vertex is hitting c2 (not always right but not a horrible approximation

short PhysicsCar::m_CheckCollisionWithCar(PhysicsCar *c2) 
{
	return 1;
}



