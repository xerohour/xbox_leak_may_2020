//-----------------------------------------------------------------------------
// FILE: PhysicsSystem.h
//
// Desc: Physics system
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef TECH_CERT_GAME_PHYSICS_SYSTEM_H
#define TECH_CERT_GAME_PHYSICS_SYSTEM_H

#pragma warning( disable: 4702 )
#include "Common.h"
#include "PhysicsObject.h"
#include "PhysicsShape.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <vector>
#include <list>
#pragma warning( pop )



const int iMaxConstraints = 16;
class HeightField;

//
// Record of two object whose bounding boxes overlap.
//
class OverlapRecord
{
public:
	OverlapRecord()
	{
		pA = 0;
		pB = 0;
	}

	OverlapRecord(PhysicsObject* a, PhysicsObject* b)
	{
		pA = a;
		pB = b;
	}

	bool operator== (const OverlapRecord& lhs) const
	{
		return (pA == lhs.pA && pB == lhs.pB) || (pA == lhs.pA && pB == lhs.pB);
	}

	PhysicsObject* pA;
	PhysicsObject* pB;
	OverlapRecord* pNextA;		// Next pointer for object A's list.
	OverlapRecord* pNextB;		// Next pointer for object B's list.

	OverlapRecord* pPrev;		// Previous pointer for global list.
	OverlapRecord* pNext;		// Next pointer for global list.
};


//
// Record of two intersecting objects.
//
class CollisionRecord
{
public:
	CollisionRecord(PhysicsObject* a, PhysicsObject* b)
	{
		pA = a;
		pB = b;
	}

	PhysicsObject* pA;
	PhysicsObject* pB;
	IntersectionInfo info;
};

typedef std::vector<CollisionRecord> CollisionList;


//
// A constraint between two objects.
//
class Constraint
{
public:
	Constraint(PhysicsObject* a, PhysicsObject* b)
	{
		pA = a;
		pB = b;
	}

	enum ConstraintType 
	{
		Bilateral, UnilateralVertFace, UnilateralEdgeEdge, StaticFriction 
	};

	ConstraintType Type;

	PhysicsObject* pA;
	PhysicsObject* pB;

	Vector3 v3Point;
	Vector3 v3Normal;
	Vector3 v3EdgeA, v3EdgeB;

	float fDist;			// D value of plane point is on for bilateral constraints.

	float fFrictionForce;	// Maximum value for static friction constraints.
	float fConstraintAccel;	// Extra acceleration at constraint.
};

typedef std::vector<Constraint*> ConstraintList;


//
// The physics system.
//
class PhysicsSystem
{
public:
	// Constructor/Destructor.
	PhysicsSystem();
	~PhysicsSystem();

    // Shutdown the physics system.
    void Shutdown();

    // Simulate over the specified time period. Returns the rumble level
	FLOAT Simulate(float start_time, float end_time);

	// Find intersections, apply collision impulses, and add forces.
	bool ProcessCollisionsAndForces(float cur_time, FLOAT& fRumble );

	// Add an object to the simulation.
	void AddObject(PhysicsObject* pObj);

	// Remove an object from the simulation.
	void RemoveObject(PhysicsObject* pObj);

	// Add a heightfield object.
	void AddHeightField(HeightField* pHF);

	// Add a pin joint constriant between two objects.
	void AddPinJointConstraint(PhysicsObject* pA, PhysicsObject* pB, const Vector3& v3Point);

	// Add a constraint between two objects.
	// Note: The constraint data needs to be filled in.
	Constraint* AddConstraint(PhysicsObject* pA, PhysicsObject* pB);

	// Check a line segment from p1 to p2 for intersection again all objects in the physics system.
	bool CheckLineSegment(const Vector3& p1, const Vector3& p2, float* p_t, Vector3* p_q);

	// Save/Load physics state.
    static DWORD GetMaxStateSize();
	int GetStateSize();
	void GetState( FLOAT* pState );
	void SetState( const FLOAT* pState );

	// Draw debugging info.
	void DrawDebug();

	// Physics object representing the world
	PhysicsObject* GetWorldObject();

private:
	typedef std::list<PhysicsObject*> PhysObjList;
	typedef std::vector<PhysicsObject::Extent*> ExtentList;

	// Add an overlap record.
	void AddOverlap(PhysicsObject* pA, PhysicsObject* pB);

	// Remove an overlap record.
	void RemoveOverlap(PhysicsObject* pA, PhysicsObject* pB);

	// Add an extent the an extent list maintaining sorted order.
	void InsertExtent(ExtentList& extents, PhysicsObject::Extent* extent);

	// Remove an extent the an extent list maintaining sorted order.
	void RemoveExtent(ExtentList& extents, PhysicsObject::Extent* extent);

	// Sort the extent list and update the overlaps.
	void SortExtentsAndUpdate(ExtentList& extents);

	// Compute the A matrix for a set of constraints.
	void ComputeAMatrix(const ConstraintList& Constraints, float matA[][iMaxConstraints]);

	// Compute the b vector for a set of constraints.
	void ComputeBVector(const ConstraintList& Constraints, float* vecB);

	// Solve a linear system of equations A * x = b
	bool SolveLinearSystem(float A[][iMaxConstraints], int n, float* b, float* x);

	// Matrix vector multiplication (x = A * b)
	void MatVecMul(float A[][iMaxConstraints], int n, float* b, float* x);

	// Sets used by the constraint solver.
	enum SolverSets
	{
		SetNone, SetC, SetNC, SetCf, SetNCfplus, SetNCfminus
	};

	// Solve for forces that will cause constraints to be met.
	void ConstraintSolver(const ConstraintList& Constraints, float A[][iMaxConstraints], float* b, float* f);

    PhysicsObject m_WorldObject;

	// List of physics objects.
	PhysObjList Objects;
	
	HeightField* pHeightField;

	// Sorted extent lists for physics objects.
	ExtentList ExtentsX, ExtentsY, ExtentsZ;

	// List of overlap records.
	OverlapRecord OverlapsHead;
	OverlapRecord OverlapsTail;

	// List of static constraints.
	ConstraintList StaticConstraints;

	// List of active constraints.
	ConstraintList ActiveConstraints;

	// List of currently intersecting objects.
	CollisionList Collisions;

	// Maximum number of constraints in a group.
	static const float fMaxTimeStep;
	static const float fMinTimeStep;
};


#endif // TECH_CERT_GAME_PHYSICS_SYSTEM_H
