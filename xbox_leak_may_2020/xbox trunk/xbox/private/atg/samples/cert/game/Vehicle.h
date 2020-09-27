//-----------------------------------------------------------------------------
// File: Vehicle.h
//
// Desc: Vehicle object
//
// Hist: 03.14.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_VEHICLE_H
#define TECH_CERT_GAME_VEHICLE_H

#include "Common.h"
#include "PhysicsObject.h"




//-----------------------------------------------------------------------------
// Name: class Vehicle
// Desc: Vehicle object
//-----------------------------------------------------------------------------
class Vehicle : public PhysicsObject
{
public:
	Vehicle( PhysicsSystem*, const Vector3& v3Pos, const Quaternion& qRot, 
             const Vector3* v3Supports, const FLOAT* fSupportLength);

	// Also add constraints for the vehicle.
	virtual void AddConstraints(float time);

	// Compute force and torque acting on the vehicle.
	virtual void ComputeForceAndTorque(float time);

	inline void SetTurnAngle(float angle)
	{
		m_fTurnAngle = angle;
	}

	inline void SetSpeedControl(float speed)
	{
		m_fSpeedControl = speed;
	}

	inline float GetTurnAngle()
	{
		return m_fTurnAngle;
	}

	inline float GetSupportLength(int i)
	{
		return m_fCurrentSupportLength[i];
	}

protected:
	int m_iNumSupports;
	Vector3 m_v3SupportOffset[4];

	float m_fSupportLength[4];
	float m_fCurrentSupportLength[4];

	Vector3 m_v3ContactPoint[4];
	float m_fContactT[4];

	float m_fTurnAngle;
	float m_fSpeedControl;

private:

    // Disabled
    Vehicle();
};




#endif // TECH_CERT_GAME_VEHICLE
