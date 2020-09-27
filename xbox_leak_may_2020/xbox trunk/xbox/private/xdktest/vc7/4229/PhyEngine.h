#ifndef __ENGINE_H
#define __ENGINE_H

#define MAX_GEARS 8

enum
{
	DRIVE_FRONTWHEEL= 0,
	DRIVE_REARWHEEL,
	DRIVE_FOURWHEEL
};

struct PhysicsCar;

struct TRACTION;


struct Engine
{
	PhysicsCar *m_phyCar;   // pointer back to the car to which we belong
	float m_maxRPM;         // (rpm) speed at which power reaches maximum value
	float m_maxPower;         // (kW) maximum power
	float m_enginePower;
	float m_maxAcceleration;
	float m_finalRatio;
	float m_wheelRadius[4];
	float m_engineCoefs[4];
	float m_equivalentMass;
	float m_gearRatio[MAX_GEARS];  // 0 = Reverse, 1-7 = gears 1-7
	float m_shiftPoint[MAX_GEARS-1];   // rad/sec
	float m_accelTorque[4];
	float m_brakeTorque[4];
	float m_brakeFront;
	float m_brakeRear;
	float m_inertiaEngine;
	float m_inertiaWheel;
	float m_inertiaTransmission;
	short m_driveWheels;         // front, rear, four

	int m_gearCount;  // not counting reverse
	int m_currGear;
	float m_revvy;
	float m_acceleration;    // 0.0f to 1.0f  (for Chad's sound stuff)

	bool  m_automatic;

	float m_accelWeight[4];
	float m_brakeWeight[4];

	void m_Init();
	float m_DetermineWheelTorque(float wheelrev, float accIn);
	void m_LimitRevs(float wheelRev,float wheelSpinVel[4]);
	void m_AutoBox(float wheelRev/*, Input *in*/);
	void m_ComputeAccelAndBrakeTorques(float accel, float brake, short isEBrake, 
		                               short isReverse, float torqeMulitplier = 1.0f);
	float m_ComputeWheelAcceleration(float wheelVel, int wheel, float fx);
};

#endif  // __ENGINE_H