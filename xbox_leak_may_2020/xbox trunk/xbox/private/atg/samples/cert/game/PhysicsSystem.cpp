//-----------------------------------------------------------------------------
// FILE: PhysicsSystem.cpp
//
// Desc: Physics system
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <assert.h>
#include "PhysicsSystem.h"
#include "Polyhedron.h"
#include "HeightField.h"
#include "PerfTimer.h"
#include "Globals.h"
#include "Controller.h"
#pragma warning(disable: 4355)



//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_PHYSICS_OBJECTS = 20; // arbitrary




PhysicsSystem::PhysicsSystem()
:
    m_WorldObject( this )
{
	// Initilize global overlap list.
	OverlapsHead.pPrev = 0;
	OverlapsHead.pNext = &OverlapsTail;
	OverlapsTail.pPrev = &OverlapsHead;
	OverlapsTail.pNext = 0;

	pHeightField = 0;
}




//-----------------------------------------------------------------------------
// Name: ~PhysicsSystem()
// Desc: Destroy the system
//-----------------------------------------------------------------------------
PhysicsSystem::~PhysicsSystem()
{
    Shutdown();
}




//-----------------------------------------------------------------------------
// Name: Shutdown()
// Desc: Shutdown the physics system
//-----------------------------------------------------------------------------
void PhysicsSystem::Shutdown()
{
    // Bye bye static constraints
    for( ConstraintList::iterator i = StaticConstraints.begin();
         i != StaticConstraints.end(); ++i )
    {
        delete *i;
    }
    StaticConstraints.clear();

	// Remove any objects left in the object list.
	PhysObjList::iterator it = Objects.begin();
	while (it != Objects.end())
	{
		RemoveObject((*it));
		it = Objects.begin();
	}

    // Initilize global overlap list
    OverlapsHead.pPrev = NULL;
    OverlapsHead.pNext = &OverlapsTail;
    OverlapsTail.pPrev = &OverlapsHead;
    OverlapsTail.pNext = NULL;

	// Make sure extent lists are empty.
	ExtentsX.clear();
	ExtentsY.clear();
	ExtentsZ.clear();

    // Nuke height field
    delete pHeightField;
    pHeightField = NULL;
}




FLOAT PhysicsSystem::Simulate(float start_time, float end_time)
{
	static float fLeftOverTime = 0.0f;

	float state0[13*32], state1[13*32], derivatives[13*32];
	PhysObjList::iterator it;
	TimeStamp start, end;

	// Add in left over time.
	start_time -= fLeftOverTime;
	fLeftOverTime = 0.0f;

	// Clamp timestep.
	if (end_time > start_time+0.05f)
		end_time = start_time+0.05f;

#if (0)
	// Fixed time step for debugging.
	end_time = start_time + 0.002f;
#endif

	GetTimeStamp(&start);

    FLOAT fRumble = 0.0f;
	while (start_time < end_time)
	{
		float step = end_time - start_time;

		if (step > fMaxTimeStep)
			step = fMaxTimeStep;

		if (step < fMinTimeStep)
		{
			fLeftOverTime = step;
			break;
		}

		// Take half step.
		ProcessCollisionsAndForces( start_time, fRumble );

		float half_step = step*0.5f;

		int offset = 0;
		for (it = Objects.begin(); it != Objects.end(); ++it )
		{
			// Get state.
			(*it)->GetState(state0+offset);

			// Get derivatives.
			(*it)->GetDerivatives(derivatives+offset);

			int state_size = (*it)->StateSize();

			// Take half step.
			for (int i = offset; i < offset+state_size; i++)
				state1[i] = state0[i] + half_step * derivatives[i];

			// Set state.
			(*it)->SetState(state1+offset);

			offset += state_size;
		}

		bool bDiscontinous = ProcessCollisionsAndForces( start_time+half_step, fRumble );

		if (bDiscontinous)
		{
			// Bail out of integrator.
			start_time += half_step;
			continue;
		}

		// Take full step with derivatives from midpoint.
		offset = 0;
		for (it = Objects.begin(); it != Objects.end(); ++it )
		{
			// Get derivatives at midpoint.
			(*it)->GetDerivatives(derivatives+offset);

			int state_size = (*it)->StateSize();

			// Take half step.
			for (int i = offset; i < offset+state_size; i++)
				state1[i] = state0[i] + step * derivatives[i];

			// Set state.
			(*it)->SetState(state1+offset);

			offset += state_size;
		}

		start_time += step;
	}

	GetTimeStamp(&end);
	g_PhysicsTime += SecondsElapsed(start, end);

    if( fRumble < 0.0f )
        fRumble = 0.0f;
    else if( fRumble > 1.0f )
        fRumble = 1.0f;
    return fRumble;
}


bool PhysicsSystem::ProcessCollisionsAndForces( float cur_time, FLOAT& fRumble )
{
	float A[iMaxConstraints][iMaxConstraints];
	float b[iMaxConstraints];
	TimeStamp start, end;
	PhysObjList::iterator it;
	bool bAppliedImpulse = false;

	g_DebugLines.clear();
	Collisions.clear();

	// Update object extents.
	for (it = Objects.begin(); it != Objects.end(); ++it )
		(*it)->UpdateExtents();

	// Find any intersections.
	SortExtentsAndUpdate(ExtentsX);
	SortExtentsAndUpdate(ExtentsY);
	SortExtentsAndUpdate(ExtentsZ);

	static IntersectionInfo IntInfos[32];

	for (OverlapRecord* pRec = OverlapsHead.pNext; pRec != &OverlapsTail; pRec = pRec->pNext)
	{
		PhysicsObject* pA = pRec->pA;
		PhysicsObject* pB = pRec->pB;

		// Check objects we overlap.
		Transformation TransformA(pA->qR, pA->v3X);
		Transformation TransformB(pB->qR, pB->v3X);

		if (Polyhedron::CheckForIntersection(TransformA, *pA->pShape, 
											 TransformB, *pB->pShape))
		{
			GetTimeStamp(&start);

			int iNumInfos = 0;
			Polyhedron::ComputeIntersectionInfo(TransformA, *pA->pShape, 
												TransformB, *pB->pShape, 
												&iNumInfos, IntInfos);

			for (int i = 0; i < iNumInfos; i++)
			{
				if (IntInfos[i].bAtowardsB)
					Collisions.push_back(CollisionRecord(pA, pB));
				else
					Collisions.push_back(CollisionRecord(pB, pA));

				CollisionRecord& crec = Collisions.back();
				crec.info = IntInfos[i];

#if (0)
				// Add debug lines for intersection.
				g_DebugLines.push_back(DebugLine());
				DebugLine& line = g_DebugLines.back();
				line.pos1 = crec.info.v3CenterOfIntersection;
				line.color1 = 0x00ff0000;
				line.pos2 = crec.info.v3CenterOfIntersection 
							+ crec.info.v3ExtractionDirection * 0.5f;
				line.color2 = 0x0000ff00;
#endif
			}

			GetTimeStamp(&end);
			g_CollisionTime += SecondsElapsed(start, end);
		}
	}

	// Check against heightfield.
	if (pHeightField)
	{
		for (it = Objects.begin(); it != Objects.end(); ++it )
		{
			// Skip static objects.
			if ((*it)->fMass == 0.0f)
				continue;

			int iNumInfos = 0;
			Transformation TransformA;
			Transformation TransformB((*it)->qR, (*it)->v3X);
			pHeightField->ComputeIntersectionInfo(TransformA, TransformB, *(*it)->pShape, &iNumInfos, IntInfos);

			for (int i = 0; i < iNumInfos; i++)
			{
				if (IntInfos[i].bAtowardsB)
					Collisions.push_back(CollisionRecord( GetWorldObject(), (*it)));
				else
					Collisions.push_back(CollisionRecord((*it), GetWorldObject() ));

				CollisionRecord& crec = Collisions.back();
				crec.info = IntInfos[i];

#if (0)
				// Add debug lines for intersection.
				g_DebugLines.push_back(DebugLine());
				DebugLine& line = g_DebugLines.back();
				line.pos1 = crec.info.v3CenterOfIntersection;
				line.color1 = 0x00ff0000;
				line.pos2 = crec.info.v3CenterOfIntersection 
							+ crec.info.v3ExtractionDirection * 0.5f;
				line.color2 = 0x0000ff00;
#endif
			}
		}
	}

	// Setup active constraint list.
	for (it = Objects.begin(); it != Objects.end(); ++it )
	{
		(*it)->AddConstraints(cur_time);
	}

	// Add static constraints to active list.
	for (UINT i = 0; i < StaticConstraints.size(); i++)
	{
		const Constraint& sc = *StaticConstraints[i];

		Constraint* dc = new Constraint(sc.pA, sc.pB);

		dc->Type = sc.Type;
		dc->v3Point = sc.v3Point * sc.pA->qR + sc.pA->v3X;
		dc->v3Normal = sc.v3Normal * sc.pB->qR;
		dc->fDist = sc.fDist - dc->v3Normal * sc.pB->v3X;

		ActiveConstraints.push_back(dc);
	}

	// Compute A matrix for constraint solver.
	ComputeAMatrix(ActiveConstraints, A);

	// Resolve any collisions.
	bool bCollisionsResolved = false;
	while (!bCollisionsResolved)
	{
		bCollisionsResolved = true;

		CollisionList::iterator cit;
		for (cit = Collisions.begin(); cit != Collisions.end(); ++cit )
		{
			PhysicsObject* pA = (*cit).pA;
			PhysicsObject* pB = (*cit).pB;
			const Vector3& n = (*cit).info.v3ExtractionDirection;
			const Vector3& p = (*cit).info.v3CenterOfIntersection;

			Vector3 ra = p - pA->v3X;
			Vector3 rb = p - pB->v3X;

			Vector3 padot = pA->VelocityAtPoint(p);
			Vector3 pbdot = pB->VelocityAtPoint(p);
			float vrel = n * (padot - pbdot);

			// Stop any velocity that will cause further penetration.
			if (vrel < -0.1f)
			{
				float numerator = -(1.0f + 0.0f) * vrel;
				float term3 = n * (((ra ^ n) * pA->m3InvI) ^ ra);
				float term4 = n * (((rb ^ n) * pB->m3InvI) ^ rb);
				float j = numerator / (pA->fInvMass + pB->fInvMass + term3 + term4);

				Vector3 impulse = j * n;

				assert(_finite(impulse.x) && _finite(impulse.y) && _finite(impulse.z));

				pA->ApplyImpulse(p,  impulse);
				pB->ApplyImpulse(p, -impulse);

				// We may have messed up another collision.
				bCollisionsResolved = false;
				bAppliedImpulse = true;
			}
		}
	}

	// Compute b vector (velocities) for constriants.
	for (i = 0; i < ActiveConstraints.size(); i++)
	{
		Constraint& c = *ActiveConstraints[i];

		Vector3 pa_dot = c.pA->VelocityAtPoint(c.v3Point);
		Vector3 pb_dot = c.pB->VelocityAtPoint(c.v3Point);
		b[i] = c.v3Normal * (pa_dot - pb_dot);

		if (c.Type == Constraint::Bilateral)
		{
			// Return to correct position.
			float error_dist = c.v3Normal * c.v3Point + c.fDist;
			b[i] += 0.1f * error_dist;
		}
	}

	// Solve for constraint impulses.
	float constraint_impulses[iMaxConstraints];
	ConstraintSolver(ActiveConstraints, A, b, constraint_impulses);

	// Apply constraint impulses.
	for (i = 0; i < ActiveConstraints.size(); i++)
	{
		Constraint& c = *ActiveConstraints[i];

		c.pA->ApplyImpulse(c.v3Point, c.v3Normal * constraint_impulses[i]);
		c.pB->ApplyImpulse(c.v3Point, c.v3Normal * -constraint_impulses[i]);
	}

	// Add external forces.
	for (it = Objects.begin(); it != Objects.end(); ++it )
	{
		(*it)->v3Force.Zero();
		(*it)->v3Torque.Zero();

		(*it)->ComputeForceAndTorque(cur_time);
	}

	// Add non-penetration forces.
	CollisionList::iterator cit;
	for (cit = Collisions.begin(); cit != Collisions.end(); ++cit )
	{
		PhysicsObject* pA = (*cit).pA;
		PhysicsObject* pB = (*cit).pB;
		const Vector3& n = (*cit).info.v3ExtractionDirection;
		const Vector3& p = (*cit).info.v3CenterOfIntersection;

		// Add non-penetration force.
		Vector3 padot = pA->VelocityAtPoint(p);
		Vector3 pbdot = pB->VelocityAtPoint(p);
		float vrel = n * (padot - pbdot);
		Vector3 tvel = (padot - pbdot) - (n * vrel);

		if (vrel < 0.0f) vrel = 0.0f;

		float ks = 100.0f * (pA->fMass + pB->fMass);
		float kd = 10.0f * (pA->fMass + pB->fMass);

		Vector3 force = n * (ks * (*cit).info.fPenetrationDepth - kd * vrel);

		if (tvel.GetLength() > 0.01f)
		{
			// Add friction force.
			float mag = force.GetLength();
			force -= tvel * mag * 0.2f;
		}

		assert(_finite(force.x) && _finite(force.y) && _finite(force.z));

		pA->AddForce(p,  force);
		pB->AddForce(p, -force);

        fRumble += force.GetLength();
	}

	// Compute b vector (accelerations) for constriants.
	ComputeBVector(ActiveConstraints, b);

	// Solve for constraint forces.
	float constraint_forces[iMaxConstraints];
	ConstraintSolver(ActiveConstraints, A, b, constraint_forces);

	// Add constraint forces.
	for (i = 0; i < ActiveConstraints.size(); i++)
	{
		Constraint& c = *ActiveConstraints[i];

		c.pA->AddForce(c.v3Point, c.v3Normal * constraint_forces[i]);
		c.pB->AddForce(c.v3Point, c.v3Normal * -constraint_forces[i]);
	}

#if (0)
	// Check to make sure accelerations were zeroed.
	ComputeBVector(ActiveConstraints, b);
	for (i = 0; i < ActiveConstraints.size(); i++)
	{
		switch (ActiveConstraints[i]->Type)
		{
		case Constraint::UnilateralVertFace:
		case Constraint::UnilateralEdgeEdge:
			assert(b[i] > -0.001f);
		break;

		case Constraint::StaticFriction:
			assert(fabs(b[i]) < 0.001f || fabs(constraint_forces[i])+0.001f > ActiveConstraints[i]->fFrictionForce);
		break;

		case Constraint::Bilateral:
			assert(fabs(b[i]) < 0.001f);
		break;
		}
	}
#endif;

	// Get rid of active constraints.
	for (i = 0; i < ActiveConstraints.size(); i++)
		delete ActiveConstraints[i];

	ActiveConstraints.clear();

	return bAppliedImpulse;
}


void PhysicsSystem::AddObject(PhysicsObject* pObj)
{
	pObj->UpdateExtents();

	// Add object extents.
	InsertExtent(ExtentsX, &pObj->MinX);
	InsertExtent(ExtentsX, &pObj->MaxX);

	InsertExtent(ExtentsY, &pObj->MinY);
	InsertExtent(ExtentsY, &pObj->MaxY);

	InsertExtent(ExtentsZ, &pObj->MinZ);
	InsertExtent(ExtentsZ, &pObj->MaxZ);

	// Add overlaps.
	for (PhysObjList::iterator it = Objects.begin(); it != Objects.end(); ++it )
	{
		PhysicsObject* pB = *it;

		if (PhysicsObject::ExtentsOverlap(pObj, pB))
		{
			AddOverlap(pObj, pB);
		}
	}

	// Add object to object list.
	Objects.push_back(pObj);
    assert( Objects.size() < MAX_PHYSICS_OBJECTS );
}


void PhysicsSystem::RemoveObject(PhysicsObject* pObj)
{
	// Remove object from object list.
	Objects.remove(pObj);

	// Remove object extents.
	RemoveExtent(ExtentsX, &pObj->MinX);
	RemoveExtent(ExtentsX, &pObj->MaxX);

	RemoveExtent(ExtentsY, &pObj->MinY);
	RemoveExtent(ExtentsY, &pObj->MaxY);

	RemoveExtent(ExtentsZ, &pObj->MinZ);
	RemoveExtent(ExtentsZ, &pObj->MaxZ);

	// Remove overlaps.
	OverlapRecord* pRec = pObj->pOverlaps;
	while (pRec)
	{
		RemoveOverlap(pRec->pA, pRec->pB);
		pRec = pObj->pOverlaps;
	}
}


void PhysicsSystem::AddHeightField(HeightField* pHF)
{
	pHeightField = pHF;
}


void PhysicsSystem::AddPinJointConstraint(PhysicsObject* pA, PhysicsObject* pB, const Vector3& v3Point)
{
	Vector3 pos_a = v3Point * ~Transformation(pA->qR, pA->v3X);
	Vector3 pos_b = v3Point * ~Transformation(pB->qR, pB->v3X);

	Constraint* c1 = new Constraint(pA, pB);
	Constraint* c2 = new Constraint(pA, pB);
	Constraint* c3 = new Constraint(pA, pB);

	c1->Type = Constraint::Bilateral;
	c1->v3Point = pos_a;
	c1->v3Normal = Vector3(1.0f, 0.0f, 0.0f);
	c1->fDist = -(c1->v3Normal * pos_b);

	c2->Type = Constraint::Bilateral;
	c2->v3Point = pos_a;
	c2->v3Normal = Vector3(0.0f, 1.0f, 0.0f);
	c2->fDist = -(c2->v3Normal * pos_b);

	c3->Type = Constraint::Bilateral;
	c3->v3Point = pos_a;
	c3->v3Normal = Vector3(0.0f, 0.0f, 1.0f);
	c3->fDist = -(c3->v3Normal * pos_b);

	StaticConstraints.push_back(c1);
	StaticConstraints.push_back(c2);
	StaticConstraints.push_back(c3);
}


Constraint* PhysicsSystem::AddConstraint(PhysicsObject* pA, PhysicsObject* pB)
{
	Constraint* c = new Constraint(pA, pB);

	ActiveConstraints.push_back(c);

	return c;
}


bool PhysicsSystem::CheckLineSegment(const Vector3& p1, const Vector3& p2, float* p_t, Vector3* p_q)
{
	if (pHeightField)
	{
		return( pHeightField->CheckLineSegment(p1, p2, p_t, p_q) == TRUE );
	}
	else
	{
		// Check line (p1,p2).
		Plane3 ground(0.0f, 1.0f, 0.0f, -0.5f);

		float d1 = ground.GetDistance(p1);
		float d2 = ground.GetDistance(p2);

		if (d2 < 0.0f)
		{
			float t = d1 / (d1 - d2);
			*p_q = (1.0f - t) * p1 + t * p2;
			*p_t = t;
			return true;
		}
	}

	return false;
}



//-----------------------------------------------------------------------------
// Name: GetMaxStateSize()
// Desc: Returns the maximum state size of the physics system
//-----------------------------------------------------------------------------
DWORD PhysicsSystem::GetMaxStateSize() // static
{
    return MAX_PHYSICS_OBJECTS * PhysicsObject::GetMaxStateSize();
}




int PhysicsSystem::GetStateSize()
{
	// Return the total of the sizes of each object.
	int state_size = 0;

	PhysObjList::iterator it;
	for (it = Objects.begin(); it != Objects.end(); ++it )
	{
		state_size += (*it)->StateSize();
	}

	return state_size;
}



void PhysicsSystem::GetState( FLOAT* pState )
{
    assert( pState != NULL );

	// Get the state of each object
	PhysObjList::iterator it;
	for (it = Objects.begin(); it != Objects.end(); ++it )
	{
		// Get state
		(*it)->GetState( pState );

		pState += (*it)->StateSize();
	}
}



void PhysicsSystem::SetState( const FLOAT* pState )
{
    assert( pState != NULL );

	// Set the state of each object
	PhysObjList::iterator it;
	for (it = Objects.begin(); it != Objects.end(); ++it )
	{
		// Get state
		(*it)->SetState( pState );

		pState += (*it)->StateSize();
	}
}



PhysicsObject* PhysicsSystem::GetWorldObject()
{
    return &m_WorldObject;
}



void PhysicsSystem::AddOverlap(PhysicsObject* pA, PhysicsObject* pB)
{
	// Check for duplicates.
	OverlapRecord* pRec = pA->pOverlaps;
	while (pRec)
	{
		if (pRec->pA == pA)
		{
			if (pRec->pB == pB)
				return;

			pRec = pRec->pNextA;
		}
		else // if (pRec->pB == pA)
		{
			if (pRec->pA == pB)
				return;

			pRec = pRec->pNextB;
		}
	}

	// Allocate new record.
	pRec = new OverlapRecord(pA, pB);

	// Add to overlap list for pA.
	pRec->pNextA = pA->pOverlaps;
	pA->pOverlaps = pRec;

	// Add to overlap list for pA.
	pRec->pNextB = pB->pOverlaps;
	pB->pOverlaps = pRec;

	// Add to global overlap list.
	OverlapsTail.pPrev->pNext = pRec;
	pRec->pPrev = OverlapsTail.pPrev;
	pRec->pNext = &OverlapsTail;
	OverlapsTail.pPrev = pRec;
}


void PhysicsSystem::RemoveOverlap(PhysicsObject* pA, PhysicsObject* pB)
{
	// Remove record from A's list.
	OverlapRecord** pPrevPtr = &pA->pOverlaps;
	OverlapRecord* pRec = pA->pOverlaps;
	while (pRec)
	{
		if (pRec->pA == pA)
		{
			if (pRec->pB == pB)
			{
				*pPrevPtr = pRec->pNextA;
				break;
			}

			pRec = pRec->pNextA;
		}
		else // if (pRec->pB == pA)
		{
			if (pRec->pA == pB)
			{
				*pPrevPtr = pRec->pNextB;
				break;
			}

			pRec = pRec->pNextB;
		}
	}

	// Remove record from B's list.
	pPrevPtr = &pB->pOverlaps;
	pRec = pB->pOverlaps;
	while (pRec)
	{
		if (pRec->pA == pB)
		{
			if (pRec->pB == pA)
			{
				*pPrevPtr = pRec->pNextA;
				break;
			}

			pRec = pRec->pNextA;
		}
		else // if (pRec->pB == pB)
		{
			if (pRec->pA == pA)
			{
				*pPrevPtr = pRec->pNextB;
				break;
			}

			pRec = pRec->pNextB;
		}
	}

	if (pRec)
	{
		// Remove from global overlap list.
		pRec->pPrev->pNext = pRec->pNext;
		pRec->pNext->pPrev = pRec->pPrev;

		delete pRec;
	}
}


void PhysicsSystem::InsertExtent(ExtentList& extents, PhysicsObject::Extent* extent)
{
	int n = extents.size();

	// Find where the extent goes and insert it.
	int i;
	for (i = 0; i < n; i++)
	{
		if (extent->Value < extents[i]->Value)
			break;
	}

	// Make room.
	extents.resize(n+1);
	for (int j = n; j > i; j--)
	{
		extents[j] = extents[j-1];
	}

	extents[i] = extent;
}


void PhysicsSystem::RemoveExtent(ExtentList& extents, PhysicsObject::Extent* extent)
{
	int n = extents.size();

	// Find where the extent is and remove it.
	int i;
	for (i = 0; i < n; i++)
	{
		if (extents[i] == extent)
			break;
	}

	assert(i < n);

	// Make room.
	for (int j = i; j < n-1; j++)
	{
		extents[j] = extents[j+1];
	}

	extents.resize(n-1);
}


void PhysicsSystem::SortExtentsAndUpdate(ExtentList& extents)
{
	const int n = extents.size();

	for (int step = 1; step < n; step++)
	{
		int i;
		PhysicsObject::Extent* temp;

		temp = extents[step];
		for (i = step-1; i >= 0; i--)
		{
			if (extents[i]->Value > temp->Value)
			{
				// Make room.
				extents[i+1] = extents[i];

				// See if overlap status has changed.
				if (temp->bStart ^ extents[i]->bStart)
				{
					// REVISIT: Should we save up a count of swaps to minimize checks?
					if (PhysicsObject::ExtentsOverlap(temp->pObject, extents[i]->pObject))
					{
						// Add overlap.
						AddOverlap(temp->pObject, extents[i]->pObject);
					}
					else
					{
						// Remove overlap.
						RemoveOverlap(temp->pObject, extents[i]->pObject);
					}
				}
			}
			else
			{
				// Found place.
				break;
			}
		}

		// Insert value from Array[step].
		extents[i+1] = temp;
	}
}


void PhysicsSystem::ComputeAMatrix(const ConstraintList& Constraints, float matA[][iMaxConstraints])
{
	const int iNumConstraints = Constraints.size();

	for (int i = 0; i < iNumConstraints; i++)
	{
		const Constraint& ci = *Constraints[i];

		for (int j = 0; j < iNumConstraints; j++)
		{
			const Constraint& cj = *Constraints[j];

			// Check to see if constraints are distict.
			if ((ci.pA != cj.pA) && (ci.pB != cj.pB) &&
				(ci.pA != cj.pB) && (ci.pB != cj.pA))
			{
				matA[i][j] = 0.0f;
				continue;
			}

			PhysicsObject* pA = ci.pA;
			PhysicsObject* pB = ci.pB;
			Vector3 ra = (ci.v3Point - pA->v3X);
			Vector3 rb = (ci.v3Point - pB->v3X);

			// What force and torque does constraint j exert on body A?
			Vector3 force_on_a(0.0f, 0.0f, 0.0f);
			Vector3 torque_on_a(0.0f, 0.0f, 0.0f);

			if (cj.pA == ci.pA)
			{
				force_on_a = cj.v3Normal;
				torque_on_a = (cj.v3Point - pA->v3X) ^ cj.v3Normal;
			}
			else if (cj.pB == ci.pA)
			{
				force_on_a = -cj.v3Normal;
				torque_on_a = (cj.v3Point - pA->v3X) ^ -cj.v3Normal;
			}

			// What force and torque does constraint j exert on body B?
			Vector3 force_on_b(0.0f, 0.0f, 0.0f);
			Vector3 torque_on_b(0.0f, 0.0f, 0.0f);

			if (cj.pA == ci.pB)
			{
				force_on_b = cj.v3Normal;
				torque_on_b = (cj.v3Point - pB->v3X) ^ cj.v3Normal;
			}
			else if (cj.pB == ci.pB)
			{
				force_on_b = -cj.v3Normal;
				torque_on_b = (cj.v3Point - pB->v3X) ^ -cj.v3Normal;
			}

			// Compute how the j'th contact force affects the linear and
			// angular acceleration of the contact point on body A.
			Vector3 a_linear = force_on_a * pA->fInvMass;
			Vector3 a_angular = (torque_on_a * pA->m3InvI) ^ ra;

			// Do the same for body B.
			Vector3 b_linear = force_on_b * pB->fInvMass;
			Vector3 b_angular = (torque_on_b * pB->m3InvI) ^ rb;

			matA[i][j] = ci.v3Normal * ((a_linear + a_angular) - (b_linear + b_angular));
		}
	}
}


void PhysicsSystem::ComputeBVector(const ConstraintList& Constraints, float* vecB)
{
	const int iNumConstraints = Constraints.size();

	for (int i = 0; i < iNumConstraints; i++)
	{
		const Constraint& c = *Constraints[i];

		PhysicsObject* pA = c.pA;
		PhysicsObject* pB = c.pB;

		Vector3 ra = (c.v3Point - pA->v3X);
		Vector3 rb = (c.v3Point - pB->v3X);

		// Compute the part of pa_dot_dot due to external force and torque.
		Vector3 pa_dot_dot = pA->v3Force * pA->fInvMass +
							 ((pA->v3Torque * pA->m3InvI) ^ ra);

		// .. and similarly for pb_dot_dot.
		Vector3 pb_dot_dot = pB->v3Force * pB->fInvMass +
							 ((pB->v3Torque * pB->m3InvI) ^ rb);

		// Compute the part of pa_dot_dot due to velocity
		pa_dot_dot += (pA->v3Omega ^ (pA->v3Omega ^ ra)) +
					  (((pA->v3L ^ pA->v3Omega) * pA->m3InvI) ^ ra);

		// .. and similarly for pb_dot_dot.
		pb_dot_dot += (pB->v3Omega ^ (pB->v3Omega ^ rb)) +
					  (((pB->v3L ^ pB->v3Omega) * pB->m3InvI) ^ rb);

		vecB[i] = c.v3Normal * (pa_dot_dot - pb_dot_dot);

		Vector3 pa_dot = c.pA->VelocityAtPoint(c.v3Point);
		Vector3 pb_dot = c.pB->VelocityAtPoint(c.v3Point);
	
		switch(c.Type)
		{
			/*
			case Constraint::Bilateral:
			{
				//Vector3 ndot = pB->v3Omega ^ c.v3Normal;
				//vecB[i] += 2.0f * ndot * (pa_dot - pb_dot);

				float error_dist = c.v3Normal * c.v3Point + c.fDist;
				vecB[i] += 1.0f * error_dist + (c.v3Normal * (pa_dot - pb_dot) * 0.1f);
			}
			break;
			*/

			case Constraint::UnilateralVertFace:
			{
				Vector3 ndot = pB->v3Omega ^ c.v3Normal;
				vecB[i] += 2.0f * ndot * (pa_dot - pb_dot);
			}
			break;

			case Constraint::UnilateralEdgeEdge:
			{
				Vector3 eadot = pA->v3Omega ^ c.v3EdgeA;
				Vector3 ebdot = pB->v3Omega ^ c.v3EdgeB;
				Vector3 z = (eadot ^ c.v3EdgeB) + (c.v3EdgeA ^ ebdot);
				Vector3 n = c.v3EdgeA ^ c.v3EdgeB;

				float l = n.GetLength();
				n /= l;

				Vector3 ndot = (z - ((z * n) * n)) / l;

				vecB[i] += 2.0f * ndot * (pa_dot - pb_dot);
			}
			break;

			case Constraint::StaticFriction:
			{
				//Vector3 ndot = pB->v3Omega ^ c.v3Normal;
				//vecB[i] += 2.0f * ndot * (pa_dot - pb_dot);
			}
			break;
		}
	}
}


//
// Solve a linear system of equations A * x = b
//
bool PhysicsSystem::SolveLinearSystem(float A[][iMaxConstraints], int n, float* b, float* x)
{
	float p[iMaxConstraints];

	// Cholesky decomposition.
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float sum = A[i][j];

			for (int k = i-1; k >= 0; k--)
				sum -= A[i][k] * A[j][k];

			if (i == j)
			{
				if (sum <= 0.0f)
					return false;

				p[i] = sqrtf(sum);
			}
			else
			{
				A[j][i] = sum / p[i];
			}
		}
	}

	// Backsubstitution.
	for (i = 0; i < n; i++)
	{
		float sum = b[i];
		for (int k = i-1; k >= 0; k--)
			sum -= A[i][k] * x[k];

		x[i] = sum / p[i];
	}

	for (i = n-1; i >= 0; i--)
	{
		float sum = x[i];
		for (int k = i+1; k < n; k++)
			sum -= A[k][i] * x[k];

		x[i] = sum / p[i];
	}

	return true;
}


//
// Vector matrix multiplication (x = A * b)
//
void PhysicsSystem::MatVecMul(float A[][iMaxConstraints], int n, float* b, float* x)
{
	for (int i = 0; i < n; i++)
	{
		float sum = 0.0f;

		for (int j = 0; j < n; j++)
		{
			sum += A[i][j] * b[j];
		}

		x[i] = sum;
	}
}


void PhysicsSystem::ConstraintSolver(const ConstraintList& Constraints, float A[][iMaxConstraints], float* b, float* f)
{
	int i;
	float A11[iMaxConstraints][iMaxConstraints];	// Subset of A matrix.
	float a[iMaxConstraints];						// Current accelerations.
	float delta_f[iMaxConstraints];					// Change in forces.
	float delta_a[iMaxConstraints];					// Change in accelerations.
	SolverSets ConstraintSet[iMaxConstraints];		// Current classification of each constraint.
	int A11_index[iMaxConstraints];					// Indices used for build A11 matrix.
	float v1[iMaxConstraints];
	float x[iMaxConstraints];
	int A11_size;

	const int iNumConstraints = Constraints.size();

	// C = NC = Cf = NC+ = NC- = empty
	for (i = 0; i < iNumConstraints; i++)
		ConstraintSet[i] = SetNone;

	// f = 0
	for (i = 0; i < iNumConstraints; i++)
		f[i] = 0.0f;

	// a = b
	for (i = 0; i < iNumConstraints; i++)
		a[i] = b[i];

	//
	// Solve for bilateral constraints and place them in C.
	//
	A11_size = 0;
	for (i = 0; i < iNumConstraints; i++)
	{
		if (Constraints[i]->Type == Constraint::Bilateral)
		{
			ConstraintSet[i] = SetC;
			A11_index[A11_size++] = i;
		}
	}

	// A11 = A(CC)
	for (i = 0; i < A11_size; i++)
		for (int j = 0; j < A11_size; j++)
			A11[i][j] = A[A11_index[i]][A11_index[j]];

	// -v1 = -b
	for (i = 0; i < A11_size; i++)
		v1[i] = -b[A11_index[i]];

#if (0)
	float Temp[iMaxConstraints][iMaxConstraints];
	memcpy(Temp, A11, sizeof(A11));
#endif

	// Solve A11*x = -v1
	SolveLinearSystem(A11, A11_size, v1, x);

#if (0)
	// Check solution.
	float y[iMaxConstraints];
	MatVecMul(Temp, A11_size, x, y);

	for (i = 0; i < A11_size; i++)
		assert(fabs(v1[i] - y[i]) < 0.001f);
#endif

	// df = 0
	for (i = 0; i < iNumConstraints; i++)
		delta_f[i] = 0.0f;

	// Transfer x into delta_f
	for (i = 0; i < A11_size; i++)
		delta_f[A11_index[i]] = x[i];

	// delta_a = A * delta_f
	MatVecMul(A, iNumConstraints, delta_f, delta_a);

	// f = f + delta_f
	for (i = 0; i < iNumConstraints; i++)
		f[i] +=  delta_f[i];

	// a = a + delta_a
	for (i = 0; i < iNumConstraints; i++)
		a[i] += delta_a[i];

	//
	// Now use Dantzig's algorithm for solving LCP's to solve for the unilateral
	// and friction constraint forces.
	//
	for (int d = 0; d < iNumConstraints; d++)
	{
		// Scan for a constraint where the conditions are not met.
		if (ConstraintSet[d] != SetNone)
			continue;

		if ((Constraints[d]->Type == Constraint::UnilateralVertFace ||
			 Constraints[d]->Type == Constraint::UnilateralVertFace) &&
			a[d] > 0.0f)
		{
			// Go ahead and put this constraint in NC.
			ConstraintSet[d] = SetNC;
			continue;
		}

		if (Constraints[d]->Type == Constraint::StaticFriction && fabs(a[d]) < 1e-6f)
		{
			// Go ahead and put this constraint in Cf.
			ConstraintSet[d] = SetCf;
			continue;
		}

L1:
		//
		// drive-to-zero(d)
		//

		//
		// delta_f = fdirection(d)
		//
		for (i = 0; i < iNumConstraints; i++)
		{
			delta_f[i] = 0.0f;
		}

		if (a[d] < 0.0f)
			delta_f[d] = 1.0f;
		else
			delta_f[d] = -1.0f;

		// A11 = A(CC)
		A11_size = 0;
		for (i = 0; i < iNumConstraints; i++)
			if (ConstraintSet[i] == SetC || ConstraintSet[i] == SetCf)
				A11_index[A11_size++] = i;

		for (i = 0; i < A11_size; i++)
			for (int j = 0; j < A11_size; j++)
				A11[i][j] = A[A11_index[i]][A11_index[j]];

		// -v1 = -A(Cd)
		for (i = 0; i < A11_size; i++)
			v1[i] = -A[A11_index[i]][d] * delta_f[d];

#if (0)
		float Temp[iMaxConstraints][iMaxConstraints];
		memcpy(Temp, A11, sizeof(A11));
#endif

		// Solve A11*x = -v1
		SolveLinearSystem(A11, A11_size, v1, x);

#if (0)
		// Check solution.
		float y[iMaxConstraints];
		MatVecMul(Temp, A11_size, x, y);

		for (i = 0; i < A11_size; i++)
			assert(fabs(v1[i] - y[i]) < 0.001f);
#endif

		// Transfer x into delta_f
		for (i = 0; i < A11_size; i++)
			delta_f[A11_index[i]] = x[i];

		// delta_a = A * delta_f
		MatVecMul(A, iNumConstraints, delta_f, delta_a);

		//
		// (s,j) = maxstep(f,a,delta_f,delta_a,d)
		//
		float s = FLT_MAX;
		int j = -1;
		SolverSets NextSet = SetNone;

		if (Constraints[d]->Type == Constraint::StaticFriction)
		{
			if (delta_a[d]*delta_f[d] > 0.0f)
			{
				// Either reduce a to zero ..
				j = d;
				s = -a[d] / delta_a[d];
				NextSet = SetCf;
			}
				
			// .. or increase f to maximum.
			if (delta_f[d] > 0.0f)
			{
				float s_prime = (Constraints[d]->fFrictionForce - f[d]) / delta_f[d];
				if (s_prime < s)
				{
					j = d;
					s = s_prime;
					NextSet = SetNCfplus;
				}
			}
			else if (delta_f[d] < 0.0f)
			{
				float s_prime = (-Constraints[d]->fFrictionForce - f[d]) / delta_f[d];
				if (s_prime < s)
				{
					j = d;
					s = s_prime;
					NextSet = SetNCfminus;
				}
			}
		}
		else
		{
			assert(delta_a[d] > 0.0f);

			// Step that will reduce accel to zero.
			j = d;
			s = -a[d] / delta_a[d];
			NextSet = SetC;
		}

		for (i = 0; i < iNumConstraints; i++)
		{
			if (ConstraintSet[i] == SetC && Constraints[i]->Type != Constraint::Bilateral)
			{
				// Step that would reduce force to zero.
				if (delta_f[i] < 0.0f)
				{
					float s_prime = -f[i] / delta_f[i];
					if (s_prime < s)
					{
						j = i;
						s = s_prime;
					}
				}	
			}
			else if (ConstraintSet[i] == SetNC)
			{
				// Step that would reduce accel to zero.
				if (delta_a[i] < 0.0f)
				{
					float s_prime = -a[i] / delta_a[i];
					if (s_prime < s)
					{
						j = i;
						s = s_prime;
					}
				}
			}
			else if (ConstraintSet[i] == SetCf)
			{
				// Step that would increase force to maximum.
				if (delta_f[i] > 0.0f)
				{
					float s_prime = (Constraints[i]->fFrictionForce - f[i]) / delta_f[i];
					if (s_prime < s)
					{
						j = i;
						s = s_prime;
						NextSet = SetNCfplus;
					}
				}
				else if (delta_f[i] < 0.0f)
				{
					float s_prime = (-Constraints[i]->fFrictionForce - f[i]) / delta_f[i];
					if (s_prime < s)
					{
						j = i;
						s = s_prime;
						NextSet = SetNCfminus;
					}
				}
			}
			else if (ConstraintSet[i] == SetNCfplus)
			{
				// Step that would reduce accel to zero.
				if (delta_a[i] > 0.0f)
				{
					float s_prime = -a[i] / delta_a[i];
					if (s_prime < s)
					{
						j = i;
						s = s_prime;
					}
				}
			}
			else if (ConstraintSet[i] == SetNCfminus)
			{
				// Step that would reduce accel to zero.
				if (delta_a[i] < 0.0f)
				{
					float s_prime = -a[i] / delta_a[i];
					if (s_prime < s)
					{
						j = i;
						s = s_prime;
					}
				}
			}
		}

		assert(j != -1);

		// f = f + s * delta_f
		for (i = 0; i < iNumConstraints; i++)
			f[i] += s * delta_f[i];

		// a = a + s * delta_a
		for (i = 0; i < iNumConstraints; i++)
			a[i] += s * delta_a[i];

		if (ConstraintSet[j] == SetC)
		{
			// Move to NC.
			ConstraintSet[j] = SetNC;
			goto L1;
		}
		else if (ConstraintSet[j] == SetNC)
		{
			// Move to C.
			ConstraintSet[j] = SetC;
			goto L1;
		}
		else if (ConstraintSet[j] == SetCf)
		{
			// Move to NCf+ or NCf- based on a.
			ConstraintSet[j] = NextSet;
			goto L1;
		}
		else if (ConstraintSet[j] == SetNCfplus)
		{
			// Move to Cf.
			ConstraintSet[j] = SetCf;
			goto L1;
		}
		else if (ConstraintSet[j] == SetNCfminus)
		{
			// Move to Cf.
			ConstraintSet[j] = SetCf;
			goto L1;
		}
		else
		{
			// Place in C, Cf, NCf+, or NCf-.
			ConstraintSet[j] = NextSet;
		}
	}
}


const float PhysicsSystem::fMaxTimeStep = 0.02f;
const float PhysicsSystem::fMinTimeStep = 0.001f;
