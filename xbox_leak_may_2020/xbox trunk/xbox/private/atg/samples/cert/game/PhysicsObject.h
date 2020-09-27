//-----------------------------------------------------------------------------
// FILE: PhysicsObject.h
//
// Desc: Physical object in the physics system.
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef TECH_CERT_GAME_PHYSICS_OBJECT_H
#define TECH_CERT_GAME_PHYSICS_OBJECT_H

#include "Common.h"
#include "Math3d.h"


class Polyhedron;
class OverlapRecord;


class PhysicsObject
{
public:
	friend class PhysicsSystem;

	// Construct a static object.
	PhysicsObject( PhysicsSystem* );

	// Construct a static object at a specific location.
	PhysicsObject( PhysicsSystem*, const Vector3& v3Pos, const Quaternion& qRot);

    PhysicsSystem* GetPhysicsSystem();

	// Set the physical properties of the object.
	void SetPhysicalProperties(float fMass, Matrix3 m3Ibody);

	// Set the collision primitive for the object.
	void SetShape(Polyhedron* pShape);

	// Get the current rotation of the object.
	const Quaternion& GetRotation();

	// Set the current rotation of the object.
	void SetRotation(const Quaternion& qRot);

	// Get the current translation of the object.
	const Vector3& GetTranslation();

	// Set the current translation of the object.
	void SetTranslation(const Vector3& v3Trans);

	// Update world space extents.
	void UpdateExtents();

	// Return true if the extenst of the two objects overlap.
	static bool ExtentsOverlap(PhysicsObject* pA, PhysicsObject* pB);

	// Recompute derived variables.
	void ComputeDerivedVariables();

	// Apply an impulse at a point on the object.
	void ApplyImpulse(const Vector3& v3At, const Vector3& v3Impulse);

	// Add force at a point on the object.
	void AddForce(const Vector3& v3At, const Vector3& v3Force);

	// Get the velocity at a point on the object.
	Vector3 VelocityAtPoint(const Vector3& v3At);

    // Get the linear velocity of the object
    Vector3 GetVelocity();

	// Add any non-static constraints acting on the object.
	virtual void AddConstraints(float time);

	// Compute force and torque acting on the object.
	virtual void ComputeForceAndTorque(float time);

	// Is the object ready to sleep?
	bool ReadyToSleep();

	// Methods to support numerical integration.
    static DWORD GetMaxStateSize();
	int  StateSize();
	void GetState( FLOAT* pState );
	void SetState( const FLOAT* pState );
	void GetDerivatives(float* derivs);

private:

    // Disable
	PhysicsObject();
    PhysicsObject( const PhysicsObject& );

private:

    PhysicsSystem* m_pPhysicsSystem; // System in which object participates

	// Properties.
	float	fMass;
	float	fInvMass;		// 1 / Mass
	Matrix3 m3Ibody;		// Inertia tensor in body space.
	Matrix3 m3InvIbody;		// Inverse inertia tensor in body space.

	// State variables.
	Vector3 v3X;			// Position of center of mass.
	Quaternion qR;			// Orientation (local to world rotation).
	Vector3 v3P;			// Linear momentum.
	Vector3 v3L;			// Angular momentum.

	// Derived variables.
	Vector3 v3Vel;			// Linear velocity.
	Vector3 v3Omega;		// Angular velocity.
	Matrix3 m3InvI;			// World space inverse inertia tensor.

	// Computed variables.
	Vector3 v3Force;		// Force accumulator.
	Vector3 v3Torque;		// Torque accumulator.

	// Collision data.
	Polyhedron* pShape;		// Collision primitive.

	// Extent in a dimension.
	struct Extent
	{
		float Value;				// Value.
		bool bStart;				// Start or end?
		PhysicsObject* pObject;		// Object the extent belongs to.
	};

	Extent MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

	// Overlap records involving this object.
	OverlapRecord* pOverlaps;
};


#endif // TECH_CERT_GAME_PHYSICS_OBJECT_H
