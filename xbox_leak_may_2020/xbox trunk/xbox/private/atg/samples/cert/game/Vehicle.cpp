//-----------------------------------------------------------------------------
// File: Vehicle.cpp
//
// Desc: Vehicle object
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Vehicle.h"
#include "PhysicsSystem.h"
#include "Globals.h"


Vehicle::Vehicle( PhysicsSystem* pPhysicsSystem, const Vector3& v3Pos, 
                  const Quaternion& qRot, const Vector3* v3Supports,
                  const float* fSupportLength)
: 
    PhysicsObject( pPhysicsSystem, v3Pos, qRot )
{
	m_iNumSupports = 4;

	for (int i = 0; i < m_iNumSupports; i++)
	{
		m_v3SupportOffset[i] = v3Supports[i];
		m_fSupportLength[i] = fSupportLength[i];
		m_fCurrentSupportLength[i] = m_fSupportLength[i];

	}

	m_fTurnAngle = 0.0f;
	m_fSpeedControl = 0.0f;
}


void Vehicle::AddConstraints(float time)
{
    USED( time );
	Vector3 down = Vector3(0.0f, -1.0f, 0.0f) * GetRotation();
	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f) * GetRotation();
	Vector3 right = Vector3(1.0f, 0.0f, 0.0f) * GetRotation();
	Vector3 turn = right * Quaternion(Vector3(0.0f,1.0f,0.0f), m_fTurnAngle);

	// Find wheel contacts.
	for (int i = 0; i < m_iNumSupports; i++)
	{
		Vector3 p1 = GetTranslation() + m_v3SupportOffset[i] * GetRotation();
		Vector3 p2 = p1 + down * m_fSupportLength[i];

		m_fContactT[i] = 1.0f;
		GetPhysicsSystem()->CheckLineSegment(p1, p2, &m_fContactT[i], &m_v3ContactPoint[i]);

		if (m_fContactT[i] < 1.0f)
			m_fCurrentSupportLength[i] = (p1 - m_v3ContactPoint[i]).GetLength();
		else
			m_fCurrentSupportLength[i] = m_fSupportLength[i];
	}

	if (m_fContactT[0] < 1.0f || m_fContactT[1] < 1.0f)
	{
		// Add friction constraints.
		Constraint* c = GetPhysicsSystem()->AddConstraint( 
                                    GetPhysicsSystem()->GetWorldObject(), this );
		c->Type = Constraint::StaticFriction;

		if (m_fContactT[0] < 1.0f && m_fContactT[1] < 1.0f)
			c->v3Point = (m_v3ContactPoint[0] + m_v3ContactPoint[1]) * 0.5f;
		else if (m_fContactT[0] < 1.0f)
			c->v3Point = m_v3ContactPoint[0];
		else if (m_fContactT[1] < 1.0f)
			c->v3Point = m_v3ContactPoint[1];

		c->v3Normal = turn;
		c->fFrictionForce = 5.0f;

#if (0)
		g_DebugLines.push_back(DebugLine());
		DebugLine& line0 = g_DebugLines.back();
		line0.pos1 = c->v3Point;
		line0.c	assert(temp1.PointInPolyhedron(pInfo->v3CenterOfIntersection, 0.1f));
olor1 = 0x000000ff;
		line0.pos2 = c->v3Point + forward * Quaternion(Vector3(0.0f,1.0f,0.0f), m_fTurnAngle);
		line0.color2 = 0x000000ff;

		g_DebugLines.push_back(DebugLine());
		DebugLine& line1 = g_DebugLines.back();
		line1.pos1 = c->v3Point;
		line1.color1 = 0x00ff0000;
		line1.pos2 = c->v3Point + c->v3Normal * 0.5f;
		line1.color2 = 0x00ff0000;
#endif
	}

	if (m_fContactT[2] < 1.0f || m_fContactT[3] < 1.0f)
	{
		// Rear.
		Constraint* c = GetPhysicsSystem()->AddConstraint(
                                    GetPhysicsSystem()->GetWorldObject(), this );
		c->Type = Constraint::StaticFriction;

		if (m_fContactT[2] < 1.0f && m_fContactT[3] < 1.0f)
			c->v3Point = (m_v3ContactPoint[2] + m_v3ContactPoint[3]) * 0.5f;
		else if (m_fContactT[2] < 1.0f)
			c->v3Point = m_v3ContactPoint[2];
		else if (m_fContactT[3] < 1.0f)
			c->v3Point = m_v3ContactPoint[3];

		c->v3Normal = right;
		c->fFrictionForce = 5.0f;

#if (0)
		g_DebugLines.push_back(DebugLine());
		DebugLine& line3 = g_DebugLines.back();
		line3.pos1 = c->v3Point;
		line3.color1 = 0x00ff0000;
		line3.pos2 = c->v3Point + c->v3Normal * 0.5;
		line3.color2 = 0x00ff0000;
#endif
	}
}


void Vehicle::ComputeForceAndTorque(float time)
{
	// Add standard forces.
	PhysicsObject::ComputeForceAndTorque(time);

	const float ks = 3000.0f;
	const float kd = 300.0f;

	Vector3 down = Vector3(0.0f, -1.0f, 0.0f) * GetRotation();
	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f) * GetRotation();
	Vector3 right = Vector3(1.0f, 0.0f, 0.0f) * GetRotation();
	Vector3 turn = right * Quaternion(Vector3(0.0f,1.0f,0.0f), m_fTurnAngle);

	// Add spring forces.
	for (int i = 0; i < m_iNumSupports; i++)
	{
		if (m_fContactT[i] < 1.0f)
		{
			float t = m_fContactT[i];
			Vector3 ip = m_v3ContactPoint[i];

			// Add spring force.
			float vrel = -down * VelocityAtPoint(ip);
			Vector3 force = -down * (ks * (1.0f - t) - kd * vrel);

			float force_magnitude = force.GetLength();

			if (i < 2)
			{
				// Add dynamic friction force.
				float tvel = VelocityAtPoint(ip) * turn;
				if (fabs(tvel) > 0.01f)
				{
					// Add friction force.
					force -= tvel * turn * force_magnitude * 0.2f;
				}
			}
			else
			{
				// Add dynamic friction force.
				float tvel = VelocityAtPoint(ip) * right;
				if (fabs(tvel) > 0.01f)
				{
					// Add friction force.
					force -= tvel * right * force_magnitude * 0.2f;
				}

				// Apply acceleration/braking.
				if (m_fSpeedControl > 0.0f)
				{
					force += forward * m_fSpeedControl * 500.0f;
				}
				else
				{
					force += forward * m_fSpeedControl * 250.0f;
				}
			}

			AddForce(ip, force);
		}
	}
}
