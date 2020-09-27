#ifndef __PHYSICSCAR_H
#define __PHYSICSCAR_H

#ifndef __PHYENGINE_H
#include "PhyEngine.h"
#endif
#ifndef __PHYTIRE_H
#include "PhyTire.h"
#endif
#ifndef __SUSPENSION_H
#include "Suspension.h"
#endif
#ifndef __VECTOR_H
#include "vector.h"
#endif

#define TIRECOUNT   4
#define NUM_S_CPS 3 
#define NUM_T_CPS 3
#define CACHED_WALL_COUNT  150
#define MAX_STEER_ENTRIES 16



struct Input;
struct PhyPatch;
struct PhyEdge;

enum
{
	SUSP_FRONT_LEFT = 0,
	SUSP_FRONT_RIGHT,
	SUSP_BACK_LEFT,
	SUSP_BACK_RIGHT,
	SUSP_LENGTH_COUNT
};

struct TRACTION
{
	float x, y, t;
};

struct WallSegment
{
	float m_pt1[3];
	float m_pt2[3];
	float m_plane[4];        // wall's normal and dist
	float m_sepPlane[4];     // last valid separating plane with car
	float m_sepPlaneOld[4];  // previous valid sep plane
	int m_group;
	short m_planeValid;
	short m_planeValidOld;
	short m_side;            
};



struct PhysicsCar 
{
public:
	Engine m_engine;
	Suspension m_suspension;
	Tire  m_tire[2];      // front and back

	float m_pos[3];       // position
	float m_posVel[3];    // positional velocities, world space
	float m_quat[4];      // quaternion
	float m_rotVel[3];    // rotational velocities, world space
	float m_wheel[TIRECOUNT][3];      // Contact point of wheel, world space
	float m_wheelVel[TIRECOUNT][3];   // wheel positional velocity, world space
	float m_wheelSpinVel[TIRECOUNT];  // spinning speed of wheels, rads/sec
	float m_wheelTanSlip[TIRECOUNT];
	float m_wheelRotationAngle[TIRECOUNT];

	float m_steerTable[MAX_STEER_ENTRIES];
	float m_cogHeight;
	float m_bodyMass;
	float m_invBodyMass;
	float m_wheelMass;
	float m_invWheelMass;
	float m_tireRelaxLength;
	float m_differential[3];       // front, back, front to back
	float m_aeroDrag;              // aerodynamic drag
	float m_aeroFront;             
	float m_aeroRear;              
	float m_oldAeroFront;

	float m_bodyOffset[3];         // offset from cog to car model origin
	float m_width;                 // car width
	float m_height;
	float m_length;
	float m_corner[4][3];          // corners in body space
	float m_boxOffset;             // forward offset of collision box

	float m_transform[4][4];       // transformation matrix (body -> world)
	float m_invTransform[4][4];    // world to body
	float m_iMatrix[3][4];         // inertia matrix (really 3x3)
	float m_xMatrix[3][4];         // inverse inertia matrix (really 3x3)

	float m_stabilizerBar[2];      // force applied to prevent body roll, front, back

	float m_wheelToe;              // angle in radians
	float m_springLoadConstant[TIRECOUNT];  // ensures the car "rests" at 0 spring compression

	float m_groundNormal[TIRECOUNT][3];     // normal of ground under car
	float m_groundHeight[TIRECOUNT];        // height of ground below each wheel
	PhyPatch *m_patch[TIRECOUNT];           // patch tracked for each tire
	float m_groupDist[TIRECOUNT];           // distance percentage (0 - 1) along group for each tire

	float m_steerAngle;
	short m_inContact[TIRECOUNT];           // in contact with the road

	WallSegment m_wall[CACHED_WALL_COUNT];
	short m_wallIndex;
	short m_wallCount;

	float m_overSteerWeight;
	TRACTION m_traction[4];

	short m_normalizeQuaternion;        // don't re-normalize during collision checks
public:
	void m_InitCar(void);
	int  m_LoadCar(char* filename);
	void m_ResetCar(void);
	void m_HandleDriverInput(Input *in);
	void m_UpdatePosition(float dt);
	void m_RePositionWheels(void);
	void m_BodyToWorld(float *out, float *in);
	void m_BodyToWorldRotate(float *out, float *in);
	void m_WorldToBody(float *out, float *in);
	void m_WorldToBodyRotate(float *out, float *in);
	void m_UpdateWheelDifferentials(void);	
	void m_LocalWheelVel(float *localWheelVel, int tire);
	float m_DiffTanSlip(float wheelTanSlip, float vY, float absVX);
	void m_GetTireDemand(int tire, float steerAngle, TRACTION *out);



private:
	void m_AdjustWheelHeights(float *carOld, float *carNew);
	void m_CheckGimbleLock(void);
	void m_ComputeTransformMatrix(void);  // compute transform from pos & rotation
	void m_UpdateSpringForces(float springForce[4][3], float rcToWheel[4][3],
							float wheelPos[4][3], float wheelVel[4][3],
							float posVel[3],float rotVel[3]);
	void m_UpdateAerodynamicForces(float posVel[3], float aeroForce[3], float downForce[2]);
	void m_UpdateBodyForces(float bodyForce[3],float springForce[4][3],
							float aeroForce[3],float downForce[2]);
	void m_UpdateBodyTorque(float bodyTorque[3], float springForce[4][3], float downForce[2]);
	void m_UpdateWheelForces(float wheelForce[4][3], float springForce[4][3], float wheelHeightWorld[4]);
	void m_UpdateTireForces(float wheelForce[4][3],float wheelVel[4][3],
							float wheelSpinVel[4],float wheelTanSlip[4],float wheelHeightWorld[4]);
	void m_ComputeConstraintForces(float fc[8],float wheelForce[4][3],float bodyForce[3], float bodyTorque[3],
								   float wheelPos[4][3],float wheelVel[4][3],
								   float rcToWheel[4][3], float posVel[3], float rotVel[3]);
	void m_ComputeCoefMatrixForConstraintForces(float out[8][8],float wheelPos[4][3],
												float rcToWheel[4][3]);
	float m_ComputeCoefFromEqA(float wheel[3],float contraint[3]);
	float m_ComputeCoefFromEqB(float wheel[3], float rayToWheel[3], float contraint[3]);
	float m_ComputeCoefFromEqB2(float wheel[3], float rayToWheel[3]);
	float m_ComputeCoefFromEqC(float constraint1[3], float rc[3], 
							   float ray[3], float constraint2[3]);
	float m_ComputeCoefFromEqD(float constraint1[3], float rc[3], 
							   float ray[3], float constraint2[3]);
	float m_ComputeCoefFromEqD2(float constraint1[3], float rc[3],float ray[3]);
	float m_ComputeResultFromEqA(float tirePos[3], float bodyTorque[3],
								 float tireForce[3], float bodyForce[3], 
								 float wheelVel[3], float posVel[3], float rotVel[3]);
	float m_ComputeResultFromEqB(float ray[3], float rc[3], float bodyTorque[3],
								 float tireForce[3], float bodyForce[3], 
								 float wheelVel[3], float posVel[3], float rotVel[3]);
	void m_EvalDiff(float *in, float *out); 
	void m_IntegratorEuler(float *y, float *dydx, float h, float *yout);
	void m_IntegratorRK2(float *y, float *dydx, float h, float *yout);
	void m_IntegratorRK4(float *y, float *dydx, float h, float *yout);
	void m_Integrator(float *y, float *dydx, float h, float *yout);

	// Collision functions
	void m_SearchForCurb(int tire);
	void m_TrackPosition(short tire);
	short m_NewtonIteration(float cpProj[NUM_S_CPS][NUM_T_CPS][2], float &uValue, float &vValue);
	void m_FindProjectedControlPoints(short tire,float cpts[NUM_S_CPS][NUM_T_CPS][3],
									  float cpProj[NUM_S_CPS][NUM_T_CPS][2]);
	void m_UpdateUV(float Mxy[9][4], float &uValue, float &vValue, float &rootCheck);
	void m_ComputeMxy(float cp[3][3][2], float M[9][4]);
	void m_AddToFrontOfWallCache(void);
	void m_AddToEndOfWallCache(void);
	void m_UpdateActivePatchGroups(void);
	short m_DeterminePlaneIntersection(float *plane);
	short m_FindSeparatingPlane(WallSegment *wall, float timeStep, short saveOff);
	short m_CheckCollisionWithWall(int &activeWall, float timeStep, short saveOff);
	void m_FindCollidingCorner(float *planeBS, int &corner, float *vel);
	void m_ApplyWallImpulse(float *wallNorm, float *posVelBS, float *rotVelBS, 
						    float *cornerVel, float *cornerBS);
	void m_CheckForGroundingCorners(void);
	short m_CheckCollisionWithCone();
	short m_CheckCollisionWithCar(PhysicsCar *c2);

public:
	void m_FindStartingPatch(void);
	void m_ComputeRoadNormal(float s, float t, float cp[NUM_T_CPS][NUM_S_CPS][3],
							 float *normal);
	void m_GetPatchIntersectionPoint(float s, float t, float cp[NUM_T_CPS][NUM_S_CPS][3],
								     float *result);
	float m_HeightOfRoad(float s, float t, float cp[NUM_T_CPS][NUM_S_CPS][3]);

	// overrides for AI cars
	void m_SetBrakes(float brake);
	void m_SetDrive(float drive);
	void m_SetGear(int gear);
	void m_SetSteering(float steer);
	// for setting the car's approximate start line position
	void m_MoveTo(float transform[4][4]);
	void m_MoveToPosition(V2 pos);
	void m_MoveToStart(int position);
	void m_GetTransform(float transform[4][4]); 
	void m_GetWheelTransform(short wheel, float transform[4][4], float caliperTransform[4][4]);
};

#endif  // __PHYSICSCAR_H
