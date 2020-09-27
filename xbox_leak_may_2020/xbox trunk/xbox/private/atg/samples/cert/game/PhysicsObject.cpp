//-----------------------------------------------------------------------------
// FILE: PhysicsObject.cpp
//
// Desc: Physical object in the physics system.
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <assert.h>
#include "PhysicsObject.h"
#include "Polyhedron.h"


PhysicsObject::PhysicsObject( PhysicsSystem* pPhysicsSystem )
:
    m_pPhysicsSystem( pPhysicsSystem )
{
    assert( pPhysicsSystem != NULL );

	fMass = 0.0f;
	fInvMass = 0.0f;

	m3Ibody.Zero();
	m3InvIbody.Zero();

	v3X.Zero();
	v3P.Zero();
	v3L.Zero();

	v3Vel.Zero();
	v3Omega.Zero();
	m3InvI.Zero();

	v3Force.Zero();
	v3Torque.Zero();

	pShape = 0;

	pOverlaps = 0;

	// Setup extent values.
	MinX.bStart = true;
	MinX.pObject = this;
	MaxX.bStart = false;
	MaxX.pObject = this;

	MinY.bStart = true;
	MinY.pObject = this;
	MaxY.bStart = false;
	MaxY.pObject = this;

	MinZ.bStart = true;
	MinZ.pObject = this;
	MaxZ.bStart = false;
	MaxZ.pObject = this;
}


PhysicsObject::PhysicsObject( PhysicsSystem* pPhysicsSystem, 
                              const Vector3& v3Pos, const Quaternion& qRot)
:
    m_pPhysicsSystem( pPhysicsSystem )
{
    assert( pPhysicsSystem != NULL );

	fMass = 0.0f;
	fInvMass = 0.0f;

	m3Ibody.Zero();
	m3InvIbody.Zero();

	v3X = v3Pos;
	qR = qRot;

	v3P.Zero();
	v3L.Zero();

	v3Vel.Zero();
	v3Omega.Zero();
	m3InvI.Zero();

	v3Force.Zero();
	v3Torque.Zero();

	pShape = 0;

	pOverlaps = 0;

	// Setup extent values.
	MinX.bStart = true;
	MinX.pObject = this;
	MaxX.bStart = false;
	MaxX.pObject = this;

	MinY.bStart = true;
	MinY.pObject = this;
	MaxY.bStart = false;
	MaxY.pObject = this;

	MinZ.bStart = true;
	MinZ.pObject = this;
	MaxZ.bStart = false;
	MaxZ.pObject = this;
}

PhysicsSystem* PhysicsObject::GetPhysicsSystem()
{
    return m_pPhysicsSystem;
}


void PhysicsObject::SetPhysicalProperties(float fMass, Matrix3 m3Ibody)
{
	PhysicsObject::fMass = fMass;
	fInvMass = 1.0f / fMass;

	PhysicsObject::m3Ibody = m3Ibody;
	m3InvIbody = ~m3Ibody;

	// Recompute derived variables.
	ComputeDerivedVariables();
}


void PhysicsObject::SetShape(Polyhedron* pShape)
{
	PhysicsObject::pShape = pShape;
}


void PhysicsObject::SetRotation(const Quaternion& qRot)
{
	qR = qRot;
	ComputeDerivedVariables();
}


const Quaternion& PhysicsObject::GetRotation()
{
	return qR;
}


const Vector3& PhysicsObject::GetTranslation()
{
	return v3X;
}


void PhysicsObject::SetTranslation(const Vector3& v3Trans)
{
	v3X = v3Trans;
}


void PhysicsObject::UpdateExtents()
{
	if (pShape)
	{
		Vector3 min, max;

		pShape->ComputeExtents(Transformation(qR, v3X), &min, &max);

		MinX.Value = min.x;
		MaxX.Value = max.x;
		MinY.Value = min.y;
		MaxY.Value = max.y;
		MinZ.Value = min.z;
		MaxZ.Value = max.z;
	}
}


bool PhysicsObject::ExtentsOverlap(PhysicsObject* pA, PhysicsObject* pB)
{
	if (pA->MinX.Value > pB->MaxX.Value || pB->MinX.Value > pA->MaxX.Value ||
		pA->MinY.Value > pB->MaxY.Value || pB->MinY.Value > pA->MaxY.Value ||
		pA->MinZ.Value > pB->MaxZ.Value || pB->MinZ.Value > pA->MaxZ.Value ||
		(pA->fMass == 0.0f && pB->fMass == 0.0f) )
	{
		return false;
	}
	else
	{
		return true;
	}
}		


void PhysicsObject::ComputeDerivedVariables()
{
	// Renormalize quaternion.
	qR.Normalize();

	// Compute rotation matrix.
	Matrix3 m3R(qR);

	// Compute world space inverse inertia tensor.
	m3InvI = m3R.GetTranspose() * m3InvIbody * m3R;

	// Compute linear velocity.
	v3Vel = v3P * fInvMass;

	// Compute angular velocity.
	v3Omega = v3L * m3InvI;
}


void PhysicsObject::ApplyImpulse(const Vector3& v3At, const Vector3& v3Impulse)
{
	if (fInvMass > 0.0f)
	{
		// Apply impulse to linear and angular momentum.
		v3P += v3Impulse;
		v3L += (v3At - v3X) ^ v3Impulse;

		// Recompute derived variables.
		v3Vel = v3P * fInvMass;
		v3Omega = v3L * m3InvI;
	}
}


void PhysicsObject::AddForce(const Vector3& v3At, const Vector3& v3Force)
{
	PhysicsObject::v3Force += v3Force;
	v3Torque += (v3At - v3X) ^ v3Force;
}


Vector3 PhysicsObject::VelocityAtPoint(const Vector3& v3At)
{
	return v3Vel + (v3Omega ^ (v3At - v3X));
}


Vector3 PhysicsObject::GetVelocity()
{
    return v3Vel;
}


void PhysicsObject::AddConstraints(float time)
{
    USED( time );
	return;
}


void PhysicsObject::ComputeForceAndTorque(float time)
{
    USED( time );
	if (fInvMass > 0.0f)
	{
		v3Force += Vector3(0.0f, -9.8f, 0.0f) * fMass;

		// Dampen any linear and angular velocity.
		v3Force -= v3Vel * 0.2f * fMass;
		v3Torque -= v3Omega * 2.0f * ~m3InvI;
	}
}


bool PhysicsObject::ReadyToSleep()
{
	return	v3Vel.GetLength() < 0.05f && 
			v3Omega.GetLength() < 0.05f &&
			(v3Force * fInvMass).GetLength() < 0.1f &&
			(v3Torque * m3InvI).GetLength() < 0.1f;
}


DWORD PhysicsObject::GetMaxStateSize() // static
{
	return 13;
}


int PhysicsObject::StateSize()
{
	return 13;
}


void PhysicsObject::GetState( FLOAT* pState )
{
	pState[0] = v3X.x;
	pState[1] = v3X.y;
	pState[2] = v3X.z;

	pState[3] = qR.x;
	pState[4] = qR.y;
	pState[5] = qR.z;
	pState[6] = qR.w;

	pState[7] = v3P.x;
	pState[8] = v3P.y;
	pState[9] = v3P.z;

	pState[10] = v3L.x;
	pState[11] = v3L.y;
	pState[12] = v3L.z;

	for (int i = 0; i < 13; i++)
		assert(_finite(pState[i]));
}


void PhysicsObject::SetState( const FLOAT* pState )
{
	for (int i = 0; i < 13; i++)
		assert(_finite(pState[i]));

	v3X.x = pState[0];
	v3X.y = pState[1];
	v3X.z = pState[2];

	qR.x = pState[3];
	qR.y = pState[4];
	qR.z = pState[5];
	qR.w = pState[6];

	v3P.x = pState[7];
	v3P.y = pState[8];
	v3P.z = pState[9];

	v3L.x = pState[10];
	v3L.y = pState[11];
	v3L.z = pState[12];

	// Recompute derived variables.
	ComputeDerivedVariables();
}


void PhysicsObject::GetDerivatives(float* derivs)
{
	derivs[0] = v3Vel.x;
	derivs[1] = v3Vel.y;
	derivs[2] = v3Vel.z;

	Quaternion qdot = 0.5f * (Quaternion(v3Omega.x, v3Omega.y, v3Omega.z, 0.0f) * qR);
	derivs[3] = qdot.x;
	derivs[4] = qdot.y;
	derivs[5] = qdot.z;
	derivs[6] = qdot.w;

	derivs[7] = v3Force.x;
	derivs[8] = v3Force.y;
	derivs[9] = v3Force.z;

	derivs[10] = v3Torque.x;
	derivs[11] = v3Torque.y;
	derivs[12] = v3Torque.z;

	for (int i = 0; i < 13; i++)
		assert(_finite(derivs[i]));
}
