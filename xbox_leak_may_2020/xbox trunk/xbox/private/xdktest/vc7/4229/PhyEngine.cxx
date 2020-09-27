#include "names.h"
#include "types.h"
#ifdef GAME
#include "Game.h"
//#include "CarGeometry.h"
//#include "Car.h"
#endif

#include "PhyEngine.h"
#include "moremath.h"
#include "CarWheelNumbers.h"
#include "PhysicsCar.h"



#define STALLSPEED ((800.0f/60.0f) * 2.0f * 3.14159265f)
#define CLUTCHRATE (1.0f/1.8f)

#define RPM2RADSEC(x) ((x)*2.0f*3.14159265f/60.0f)

#define MAXRPM                     12000.0f
#define MAXWHEELSPEEDDIFF          3000.0f//30000.0f
#define MIN_GROUND_SPEED                  0.01f
#define MIN_BRAKE_TORQUE				  50.0f
#define EPS                               0.01f

void Engine::m_Init()
{
	m_gearCount = 5;
	m_finalRatio = 4.1f;
	m_gearRatio[0] = 0.0f;
	m_gearRatio[1] = 3.83f;
	m_gearRatio[2] = 2.20f;
	m_gearRatio[3] = 1.40f;
	m_gearRatio[4] = 1.0f;
	m_gearRatio[5] = 0.83f;
	m_gearRatio[6] = 0.0f;
	m_gearRatio[7] = 0.0f;
	m_shiftPoint[0] = 33.0f;  // rads/sec
	m_shiftPoint[1] = 76.0f;
	m_shiftPoint[2] = 130.0f;
	m_shiftPoint[3] = 170.0f;
	m_shiftPoint[4] = 700.0f;
	m_shiftPoint[5] = 700.0f;
	m_shiftPoint[6] = 700.0f;
	m_maxAcceleration = 0.0f;
	m_maxPower = 149.2f;
	m_maxRPM = 6750.0f;
	m_currGear = 0;
	m_revvy = 0.0f;
	m_driveWheels = DRIVE_FRONTWHEEL;

	float maxW = RPM2RADSEC(m_maxRPM);  // convert to Rad/sec
	m_engineCoefs[0] = m_maxPower/maxW;
	m_engineCoefs[1] = m_maxPower/(maxW*maxW);
	m_engineCoefs[2] = -m_maxPower/(maxW*maxW*maxW);
	m_engineCoefs[3] = 0.0f;

	m_inertiaWheel = 7.0f;   // inertia per wheel
	m_inertiaEngine = 0.7f;   // inertia of engine, clutch 
	m_inertiaTransmission = 0.08f;  // inertia of propeller shaft & all elements of transmission
	for (int i=0; i<4; i++)
	{
		m_accelTorque[i] = 0.0f;
		m_brakeTorque[i] = 0.0f;
		m_wheelRadius[i] = 0.33f;  
	}
	m_brakeFront = 2000.0f;
	m_brakeRear = 2000.0f;
 
	m_automatic = true;

	m_accelWeight[0] = 1.0f;
	m_accelWeight[1] = 1.0f;
	m_accelWeight[2] = 1.0f;
	m_accelWeight[3] = 1.0f;
	m_brakeWeight[0] = 1.0f;
	m_brakeWeight[1] = 1.0f;
	m_brakeWeight[2] = 1.0f;
	m_brakeWeight[3] = 1.0f;
}

float Engine::m_DetermineWheelTorque(float wheelrev, float accIn) 
{
	return 0;
}

void Engine::m_LimitRevs(float wheelRev,float wheelSpinVel[4]) 
{
	float rev = ABS(wheelRev * m_finalRatio * m_gearRatio[m_currGear]);
	// Compute Engine revs

	if (rev>RPM2RADSEC(MAXRPM)) 
	{
		rev = RPM2RADSEC(MAXRPM);
		wheelRev = SIGN_AS(ABS(rev/(m_finalRatio * m_gearRatio[m_currGear])), wheelRev);
		switch (m_driveWheels)
		{
		case (DRIVE_FRONTWHEEL):
			wheelSpinVel[0] = wheelSpinVel[1] = wheelRev;
			break;
		case (DRIVE_REARWHEEL):
			wheelSpinVel[2] = wheelSpinVel[3] = wheelRev;
			break;
		default: // (DRIVE_FOURWHEEL):
			wheelSpinVel[0] = wheelSpinVel[1] = wheelSpinVel[2] = wheelSpinVel[3] = wheelRev;
			break;
		}
	}
}

void Engine::m_AutoBox(float wheelRev/*, Input *in*/) 
{
	if (m_currGear == -1)
	{
		return;
	}


#if 0
	if (in && (in->Pressed(PAD_REVERSE) && m_currGear ==0)) 
	{
		return;
	}
#endif

/*
#ifdef GT
	if (c->flags & RCF_MANUAL_TRANSMISSION)
	{
		if (in->PressedAndDebounced(PAD_GEARUP) && (m_currGear < (m_gearCount-1))) 
		{
			m_currGear++;
		}
		if (in->PressedAndDebounced(PAD_GEARDOWN) && (m_currGear > 0)) 
		{
			m_currGear--;
		}
	}
	else
#endif
*/
	{
		// we only shift gears if the car is on the ground.
//		if (!c->noWheelContact)
		{
			if (m_currGear < (m_gearCount))
			{
				if (wheelRev > (m_shiftPoint[m_currGear-1]*1.05f))
				{
					m_currGear++;
				}
			}
			if (m_currGear > 1)
			{
				if (wheelRev < (m_shiftPoint[m_currGear-2]*0.80f))
				{
					m_currGear--;
				}
			}
		}
	}

}



#define TIREMAXDEMAND 3.5f

void Engine::m_ComputeAccelAndBrakeTorques(float accel, float brakes, short isEBrake,
										   short isReverse, float torqueMultiplier)
{	
}


// Input is Wheel and applied Longitudinal force
float Engine::m_ComputeWheelAcceleration(float wheelSpinVel, int wheel, float fx) 
{
	float ret = 0.0f;
	return ret;	
}
