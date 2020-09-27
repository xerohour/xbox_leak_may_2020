
#include "Game.h"

#include "names.h"
#include "types.h"
#include "physicscar.h"
#include <string.h>
#include <stdio.h>
#include "moremath.h"
#include "input.h"
#include "PhyTrack.h"
#include "CarWheelNumbers.h"
#include "SurfaceType.h"


#define WQ 0
#define XQ 1
#define YQ 2
#define ZQ 3
#define POSVELOFFSET        3
#define QUATOFFSET          6
#define ROTVELOFFSET       10
#define WHEELOFFSET        13
#define WHEELVELOFFSET     25
#define WHEELSPINOFFSET    37
//#define WHEELTANSLIPOFFSET 41
#define NUMBEROFEQU        41

#define THREE_FOURTHS 0.75f
#define TWO_THIRDS    0.66666666f
#define ONE_THIRD     0.33333333f

#define MAX_LINE_LENGTH   1000  // needs to be bigger than longest line in .phycar file. (text version)

#define MYTIMESTEP					1.0f/300.0f
#define MIN_TIME_STEP               1.0f/(300.0f*4.0f)
#define INTEGRATORLOOPCOUNT			20  
#define MAXGROUNDFORCE				20000  // Newtons
#define EPS                         0.0001f
#define SPRING_Y_CONNECTION_POINT   0.75f

#define INITIALSUSPENSIONANGLE		0.3f

#define FRONTALAREA                 2.0f
#define AIRDENSITY                  1.2256f
#define ROLLING_RESISTANCE          120.0f
#define GIMBLE_LOCK_ANGLE           0.82f   // 35 degrees
#define GIMBLE_LOCK_ROT_ADJUST     -0.2f
#define MIN_PENETRATION_DIST       -0.1f



// boolean defines for testing
#define LOCK_STEERING 0
#define LEVEL_GROUND  0
#define TESTING_WALLS 0

#define VECBYQUAT(r,v,q)  {(r)[0] = -(v)[0]*(q)[1] - (v)[1]*(q)[2] - (v)[2]*(q)[3]; \
                           (r)[1] =  (q)[0]*(v)[0] + (v)[1]*(q)[3] - (v)[2]*(q)[2]; \
                           (r)[2] =  (q)[0]*(v)[1] + (v)[2]*(q)[1] - (v)[0]*(q)[3]; \
                           (r)[3] =  (q)[0]*(v)[2] + (v)[0]*(q)[2] - (v)[1]*(q)[1];}

#define QLENGTH(a)        sqrtf((a)[0]*(a)[0] + (a)[1]*(a)[1] + (a)[2]*(a)[2] + (a)[3]*(a)[3])


void QNORMALISE(register float* q) 
{
  register float oolen = 1.0f/QLENGTH(q);
  q[0]=(q[0]*oolen);
  q[1]=(q[1]*oolen);
  q[2]=(q[2]*oolen);
  q[3]=(q[3]*oolen);
}

void EulerToQuat(float roll, float pitch, float yaw, float *quat)
{
	float cr,cp,cy,sr,sp,sy;
	cr = COS(roll/2.0f);
	cp = COS(pitch/2.0f);
	cy = COS(yaw/2.0f);

	sr = SIN(roll/2.0f);
	sp = SIN(pitch/2.0f);
	sy = SIN(yaw/2.0f);

	// order is important
	// roll, pitch, yaw
	float cpcy = cp * cy;
	float spsy = sp * sy;

	quat[0] = cr * cpcy + sr * spsy;
	quat[1] = sr * cpcy - cr * spsy;
	quat[2] = cr * sp * cy + sr * cp * sy;
	quat[3] = cr * cp * sy - sr * sp * cy;
}


const float g_groundSlowTable[SURFACETYPE_COUNT] = 
{
	1.0f,   // road
	10.0f,   // runofff
	50.0f,  // sand
	20.0f    // grass
};

const float g_jostleTable[SURFACETYPE_COUNT] = 
{
	0.0f,   // road
	(0.06f/(float)(0x7ffffff)),
	(0.10f/(float)(0x7ffffff)),
	(0.03f/(float)(0x7ffffff)),
};

const float g_tireXSlowTable[SURFACETYPE_COUNT] = 
{
	0.0f,   // road
	100.0f,
	200.0f,
	100.0f,
};

const float g_tireYSlowTable[SURFACETYPE_COUNT] = 
{
	0.0f,   // road
	300.0f,
	300.0f,
	300.0f,
};

// g_contraintX is the wheel contraint keeping the wheel in the YZ plane, ie. X vector
float g_constraintX[3] = 
{
	1.0f, 0.0f, 0.0f
};

float g_gravity[3] =
{
	0.0f, 0.0f, -9.8f
};

float XMATRIX[3][4]=
{
	{1.0f/290.0f, 0.0f,          0.0f,         0.0f},
	{0.0f,        1.0f/1094.0f,  0.0f,         0.0f},
	{0.0f,        0.0f,          1.0f/1210.0f, 0.0f}
};

float IMATRIX[3][4] =
{
	{290.0f,    0.0f,    0.0f, 0.0f},
	{  0.0f, 1094.0f,    0.0f, 0.0f},
	{  0.0f,    0.0f, 1210.0f, 0.0f}
};



//extern CCarConstructorApp theApp;
//extern PhyTrack g_phyTrack;

float g_nDotR1 = 0.0f;
float g_nDotR2 = 0.0f;
float g_nDotR = 0.0f;
float g_rDotR = 0.0f;
#ifdef CAR_CONSTRUCTOR
JOYINFO g_joyInfo;


extern short g_accelTest;
short Accel()
{
	if (g_accelTest)
	{
		return (TRUE);
	}
	MMRESULT result = joyGetPos(JOYSTICKID1,&g_joyInfo);
	if (result == JOYERR_NOERROR)
	{
		if (g_joyInfo.wButtons & JOY_BUTTON1)
		{
			return (true);
		}
	}
	return (false);
}
short Brake()
{
	MMRESULT result = joyGetPos(JOYSTICKID1,&g_joyInfo);
	if (result == JOYERR_NOERROR)
	{
		if (g_joyInfo.wButtons & JOY_BUTTON2)
		{
			return (true);
		}
	}
	return (false);
}
short Reverse()
{
	MMRESULT result = joyGetPos(JOYSTICKID1,&g_joyInfo);
	if (result == JOYERR_NOERROR)
	{
		if (g_joyInfo.wButtons & JOY_BUTTON3)
		{
			return (true);
		}
	}
	return (false);
}
#endif


void ReadInShort(char *token,short *offset)
{
	int i=0;
	char seps[] = " ,";
	while( token != NULL )
	{
		token = strtok( NULL, seps );
		if (token)
		{
			offset[i] = (short)atoi(token);
		}
		i++;
	}
}

void ReadInInt(char *token,int *offset)
{
	int i=0;
	char seps[] = " ,";
	while( token != NULL )
	{
		token = strtok( NULL, seps );
		if (token)
		{
			offset[i] = atoi(token);
		}
		i++;
	}
}

void ReadInFloats(char *token,float *offset)
{
	int i=0;
	char seps[] = " ,";
	while( token != NULL )
	{
		token = strtok( NULL, seps );
		if (token)
		{
			sscanf(token,"%f",&offset[i]);
		}
		i++;
	}
}


void PhysicsCar::m_InitCar()
{
}


int PhysicsCar::m_LoadCar(char *filename)
{
	FILE *fp;
	fp = fopen(filename, "rb");
	if (!fp)
	{
#ifdef GAME
		PrintCustomFault("Cannot open file for reading: \"%s\" (Read Error)", filename);
#else
		MessageBox(NULL,"Cannot open file for reading.","Read Error",MB_OK);
		return FALSE;
#endif
	}
	
#if !FINAL_ROM
#endif
	{
		memset(this,0,sizeof(PhysicsCar));
	}
	m_suspension.m_Init();
	m_engine.m_Init();

	// until steertable is saved off in car structure
	m_steerTable[0] = 0.6000f;
	m_steerTable[1] = 0.5500f;
	m_steerTable[2] = 0.5000f;
	m_steerTable[3] = 0.3500f;
	m_steerTable[4] = 0.2000f;
	m_steerTable[5] = 0.1600f;
	m_steerTable[6] = 0.1300f;
	m_steerTable[7] = 0.1025f;
	m_steerTable[8] = 0.0900f;
	m_steerTable[9] = 0.0825f;
	m_steerTable[10] = 0.0750f;
	m_steerTable[11] = 0.0675f;
	m_steerTable[12] = 0.0600f;
	m_steerTable[13] = 0.0525f;
	m_steerTable[14] = 0.0450f;
	m_steerTable[15] = 0.0450f;

	char tmp[MAX_LINE_LENGTH+1];
	char seps[] = " ,";
	char *token;
	while (!feof(fp))
	{
		fgets(tmp,MAX_LINE_LENGTH,fp);
		token = strtok( tmp, seps);
		if (!strcmp(tmp,"m_bodyMass"))
		{
			ReadInFloats(token,&m_bodyMass);
		}
		else if (!strcmp(tmp,"m_invBodyMass"))
		{
			ReadInFloats(token,&m_invBodyMass);
		}
		else if (!strcmp(tmp,"m_wheelMass"))
		{
			ReadInFloats(token,&m_wheelMass);
		}
		else if (!strcmp(tmp,"m_invWheelMass"))
		{
			ReadInFloats(token,&m_invWheelMass);
		}
		else if (!strcmp(tmp,"m_tireRelaxLength"))
		{
			ReadInFloats(token,&m_tireRelaxLength);
		}
		else if (!strcmp(tmp,"m_bodyOffset"))
		{
			ReadInFloats(token,&m_bodyOffset[0]);
		}
		else if (!strcmp(tmp,"m_width"))
		{
			ReadInFloats(token,&m_width);
		}
		else if (!strcmp(tmp,"m_height"))
		{
			ReadInFloats(token,&m_height);
		}
		else if (!strcmp(tmp,"m_length"))
		{
			ReadInFloats(token,&m_length);
		}
		else if (!strcmp(tmp,"m_boxOffset"))
		{
			ReadInFloats(token,&m_boxOffset);
		}
		else if (!strcmp(tmp,"m_iMatrix1"))
		{
			ReadInFloats(token,&m_iMatrix[0][0]);
		}
		else if (!strcmp(tmp,"m_iMatrix2"))
		{
			ReadInFloats(token,&m_iMatrix[1][0]);
		}
		else if (!strcmp(tmp,"m_iMatrix3"))
		{
			ReadInFloats(token,&m_iMatrix[2][0]);
		}
		else if (!strcmp(tmp,"m_xMatrix1"))
		{
			ReadInFloats(token,&m_xMatrix[0][0]);
		}
		else if (!strcmp(tmp,"m_xMatrix2"))
		{
			ReadInFloats(token,&m_xMatrix[1][0]);
		}
		else if (!strcmp(tmp,"m_xMatrix3"))
		{
			ReadInFloats(token,&m_xMatrix[2][0]);
		}
		else if (!strcmp(tmp,"m_stabilizerBar"))
		{
			ReadInFloats(token,&m_stabilizerBar[0]);
		}
		else if (!strcmp(tmp,"m_wheelToe"))
		{
			ReadInFloats(token,&m_wheelToe);
		}
		else if (!strcmp(tmp,"m_differential"))
		{
			ReadInFloats(token,&m_differential[0]);
		}
		else if (!strcmp(tmp,"m_aeroDrag"))
		{
			ReadInFloats(token,&m_aeroDrag);
		}
		else if (!strcmp(tmp,"m_aeroFront"))
		{
			ReadInFloats(token,&m_aeroFront);
		}
		else if (!strcmp(tmp,"m_aeroRear"))
		{
			ReadInFloats(token,&m_aeroRear);
		}
		else if (!strcmp(tmp,"m_steerTable"))
		{
			ReadInFloats(token,&m_steerTable[0]);
		}
		// engine crap
		else if (!strcmp(tmp,"m_maxRPM"))
		{
			ReadInFloats(token,&m_engine.m_maxRPM);
		}
		else if (!strcmp(tmp,"m_maxPower"))
		{
			ReadInFloats(token,&m_engine.m_maxPower);
		}
		else if (!strcmp(tmp,"m_enginePower"))
		{
			ReadInFloats(token,&m_engine.m_enginePower);
		}
		else if (!strcmp(tmp,"m_maxAcceleration"))
		{
			ReadInFloats(token,&m_engine.m_maxAcceleration);
		}
		else if (!strcmp(tmp,"m_finalRatio"))
		{
			ReadInFloats(token,&m_engine.m_finalRatio);
		}
		else if (!strcmp(tmp,"m_wheelRadius"))
		{
			ReadInFloats(token,&m_engine.m_wheelRadius[0]);
		}
		else if (!strcmp(tmp,"m_engineCoefs"))
		{
			ReadInFloats(token,&m_engine.m_engineCoefs[0]);
		}
		else if (!strcmp(tmp,"m_equivalentMass"))
		{
			ReadInFloats(token,&m_engine.m_equivalentMass);
		}
		else if (!strcmp(tmp,"m_gearRatio"))
		{
			ReadInFloats(token,&m_engine.m_gearRatio[0]);
		}
		else if (!strcmp(tmp,"m_shiftPoint"))
		{
			ReadInFloats(token,&m_engine.m_shiftPoint[0]);
		}
		else if (!strcmp(tmp,"m_brakeFront"))
		{
			ReadInFloats(token,&m_engine.m_brakeFront);
		}
		else if (!strcmp(tmp,"m_brakeRear"))
		{
			ReadInFloats(token,&m_engine.m_brakeRear);
		}
		else if (!strcmp(tmp,"m_inertiaEngine"))
		{
			ReadInFloats(token,&m_engine.m_inertiaEngine);
		}
		else if (!strcmp(tmp,"m_inertiaWheel"))
		{
			ReadInFloats(token,&m_engine.m_inertiaWheel);
		}
		else if (!strcmp(tmp,"m_inertiaTransmission"))
		{
			ReadInFloats(token,&m_engine.m_inertiaTransmission);
		}
		else if (!strcmp(tmp,"m_driveWheels"))
		{
			ReadInShort(token,&m_engine.m_driveWheels);
		}
		else if (!strcmp(tmp,"m_gearCount"))
		{
			ReadInInt(token,&m_engine.m_gearCount);
		}
		// suspension settings
		else if (!strcmp(tmp,"m_springLength"))
		{
			ReadInFloats(token,&m_suspension.m_springLength[0]);
		}
		else if (!strcmp(tmp,"m_springConstA"))
		{
			ReadInFloats(token,&m_suspension.m_springConstA[0]);
		}
		else if (!strcmp(tmp,"m_springConstB"))
		{
			ReadInFloats(token,&m_suspension.m_springConstB[0]);
		}
		else if (!strcmp(tmp,"m_springDamper"))
		{
			ReadInFloats(token,&m_suspension.m_springDamper[0]);
		}
		else if (!strcmp(tmp,"m_initialCamber"))
		{
			ReadInFloats(token,&m_suspension.m_initialCamber[0]);
		}
		else if (!strcmp(tmp,"m_rollCenter1"))
		{
			ReadInFloats(token,&m_suspension.m_rollCenter[0][0]);
		}
		else if (!strcmp(tmp,"m_rollCenter2"))
		{
			ReadInFloats(token,&m_suspension.m_rollCenter[1][0]);
		}
		else if (!strcmp(tmp,"m_rollCenter3"))
		{
			ReadInFloats(token,&m_suspension.m_rollCenter[2][0]);
		}
		else if (!strcmp(tmp,"m_rollCenter4"))
		{
			ReadInFloats(token,&m_suspension.m_rollCenter[3][0]);
		}
		else if (!strcmp(tmp,"m_springBodyPoint1"))
		{
			ReadInFloats(token,&m_suspension.m_springBodyPoint[0][0]);
		}
		else if (!strcmp(tmp,"m_springBodyPoint2"))
		{
			ReadInFloats(token,&m_suspension.m_springBodyPoint[1][0]);
		}
		else if (!strcmp(tmp,"m_rollCenterFrontTable"))
		{
			ReadInFloats(token,&m_suspension.m_rollCenterFrontTable[0]);
		}
		else if (!strcmp(tmp,"m_rollCenterRearTable"))
		{
			ReadInFloats(token,&m_suspension.m_rollCenterRearTable[0]);
		}
		else if (!strcmp(tmp,"m_rcToWheelLengthTable1"))
		{
			ReadInFloats(token,&m_suspension.m_rcToWheelLengthTable[0][0]);
		}
		else if (!strcmp(tmp,"m_rcToWheelLengthTable2"))
		{
			ReadInFloats(token,&m_suspension.m_rcToWheelLengthTable[1][0]);
		}
		else if (!strcmp(tmp,"m_camberTable1"))
		{
			ReadInFloats(token,&m_suspension.m_camberTable[0][0]);
		}
		else if (!strcmp(tmp,"m_camberTable2"))
		{
			ReadInFloats(token,&m_suspension.m_camberTable[1][0]);
		}
		else if (!strcmp(tmp,"m_maxSpringCompression"))
		{
			ReadInFloats(token,&m_suspension.m_maxSpringCompression);
		}
		// Tire settings
		else if (!strcmp(tmp,"m_baseLoad1"))
		{
			ReadInFloats(token,&m_tire[0].m_baseLoad);
		}
		else if (!strcmp(tmp,"m_corneringStiffness1"))
		{
			ReadInFloats(token,&m_tire[0].m_corneringStiffness);
		}
		else if (!strcmp(tmp,"m_csSensitivityToFv1"))
		{
			ReadInFloats(token,&m_tire[0].m_csSensitivityToFv);
		}
		else if (!strcmp(tmp,"m_muY1"))
		{
			ReadInFloats(token,&m_tire[0].m_muY);
		}
		else if (!strcmp(tmp,"m_muX1"))
		{
			ReadInFloats(token,&m_tire[0].m_muX);
		}
		else if (!strcmp(tmp,"m_muYSensitivityToFv1"))
		{
			ReadInFloats(token,&m_tire[0].m_muYSensitivityToFv);
		}
		else if (!strcmp(tmp,"m_muXSensitivityToFv1"))
		{
			ReadInFloats(token,&m_tire[0].m_muXSensitivityToFv);
		}
		else if (!strcmp(tmp,"m_camberStiffness1"))
		{
			ReadInFloats(token,&m_tire[0].m_camberStiffness);
		}
		else if (!strcmp(tmp,"m_longStiffness1"))
		{
			ReadInFloats(token,&m_tire[0].m_longStiffness);
		}
		else if (!strcmp(tmp,"m_a1"))
		{
			ReadInFloats(token,&m_tire[0].m_a);
		}
		else if (!strcmp(tmp,"m_b1"))
		{
			ReadInFloats(token,&m_tire[0].m_b);
		}
		else if (!strcmp(tmp,"m_c1"))
		{
			ReadInFloats(token,&m_tire[0].m_c);
		}
		else if (!strcmp(tmp,"m_d1"))
		{
			ReadInFloats(token,&m_tire[0].m_d);
		}
		else if (!strcmp(tmp,"m_e1"))
		{
			ReadInFloats(token,&m_tire[0].m_e);
		}
		else if (!strcmp(tmp,"m_tireSpringConst1"))
		{
			ReadInFloats(token,&m_tire[0].m_tireSpringConst);
		}
		else if (!strcmp(tmp,"m_peakTraction1"))
		{
			ReadInFloats(token,&m_tire[0].m_peakTraction);
		}
		else if (!strcmp(tmp,"m_model1"))
		{
			ReadInInt(token,&m_tire[0].m_model);
		}
		// Rear Tire Settings
		else if (!strcmp(tmp,"m_baseLoad2"))
		{
			ReadInFloats(token,&m_tire[1].m_baseLoad);
		}
		else if (!strcmp(tmp,"m_corneringStiffness2"))
		{
			ReadInFloats(token,&m_tire[1].m_corneringStiffness);
		}
		else if (!strcmp(tmp,"m_csSensitivityToFv2"))
		{
			ReadInFloats(token,&m_tire[1].m_csSensitivityToFv);
		}
		else if (!strcmp(tmp,"m_muY2"))
		{
			ReadInFloats(token,&m_tire[1].m_muY);
		}
		else if (!strcmp(tmp,"m_muX2"))
		{
			ReadInFloats(token,&m_tire[1].m_muX);
		}
		else if (!strcmp(tmp,"m_muYSensitivityToFv2"))
		{
			ReadInFloats(token,&m_tire[1].m_muYSensitivityToFv);
		}
		else if (!strcmp(tmp,"m_muXSensitivityToFv2"))
		{
			ReadInFloats(token,&m_tire[1].m_muXSensitivityToFv);
		}
		else if (!strcmp(tmp,"m_camberStiffness2"))
		{
			ReadInFloats(token,&m_tire[1].m_camberStiffness);
		}
		else if (!strcmp(tmp,"m_longStiffness2"))
		{
			ReadInFloats(token,&m_tire[1].m_longStiffness);
		}
		else if (!strcmp(tmp,"m_a2"))
		{
			ReadInFloats(token,&m_tire[1].m_a);
		}
		else if (!strcmp(tmp,"m_b2"))
		{
			ReadInFloats(token,&m_tire[1].m_b);
		}
		else if (!strcmp(tmp,"m_c2"))
		{
			ReadInFloats(token,&m_tire[1].m_c);
		}
		else if (!strcmp(tmp,"m_d2"))
		{
			ReadInFloats(token,&m_tire[1].m_d);
		}
		else if (!strcmp(tmp,"m_e2"))
		{
			ReadInFloats(token,&m_tire[1].m_e);
		}
		else if (!strcmp(tmp,"m_tireSpringConst2"))
		{
			ReadInFloats(token,&m_tire[1].m_tireSpringConst);
		}
		else if (!strcmp(tmp,"m_peakTraction2"))
		{
			ReadInFloats(token,&m_tire[1].m_peakTraction);
		}
		else if (!strcmp(tmp,"m_model2"))
		{
			ReadInInt(token,&m_tire[1].m_model);
		}

	}
	fclose(fp);
//	m_ResetCar();

	return (true);
}

void PhysicsCar::m_ResetCar()
{
	m_engine.m_phyCar = this;
	m_engine.m_automatic = true;

	// clean out walls
	memset(m_wall, 0, sizeof(m_wall));

	m_posVel[0] = 0.0f;
	m_posVel[1] = 0.0f;
	m_posVel[2] = 0.0f;
	m_rotVel[0] = 0.0f;
	m_rotVel[1] = 0.0f;
	m_rotVel[2] = 0.0f;
#if !FINAL_ROM
	{
#endif
	m_quat[0] = 1.0f;    // rotate 0 degrees
	m_quat[1] = 0.0f;
	m_quat[2] = 0.0f;
	m_quat[3] = 0.0f;
#if !FINAL_ROM
	}
#endif
	for (int i=0; i<4; i++)
	{
		m_wheelVel[i][0] = 0.0f;
		m_wheelVel[i][1] = 0.0f;
		m_wheelVel[i][2] = 0.0f;
		m_wheelSpinVel[i] = 0.0f;
		m_wheelTanSlip[i] = 0.0f;
	}
	m_steerAngle = 0.0f;


#if !FINAL_ROM
	{
#endif
	m_MoveToStart(0);

#if !FINAL_ROM
	}
	{
		// Just for Brian, don't move the car back to the start line.
		m_pos[2] += 0.3f;
		for (int tire=0; tire < TIRECOUNT; tire++)
		{
			m_wheel[tire][0] = m_pos[0] + m_suspension.m_rollCenter[tire][0];
			m_wheel[tire][1] = m_pos[1] + cosf(INITIALSUSPENSIONANGLE)*m_suspension.m_rcToWheelLength[tire] * (CAR_WHEEL_RIGHT(tire)?-1.0f:1.0f);
			m_wheel[tire][2] = m_pos[2] - m_suspension.m_springLength[tire];
		}
		m_ComputeTransformMatrix();
		m_FindStartingPatch();

		// current forward vector is going to be 1, 0, 0
		// dot product with the dir we want to face gives cos angle
		// rotate angle around z axis 
		float angle = -acosf(m_transform[0][0]);
		if (m_transform[0][1] < 0.0f)
		{
			angle = GAME_2PI - angle;
		}
		EulerToQuat(0.0f, 0.0f,-angle,m_quat);
		m_ComputeTransformMatrix();
		// set the wheel positions
		// rotate around Z axis
		float rot[4][4] = 
		{
			{ cosf(angle),  -sinf(angle),  0.0f,  0.0f },
			{ sinf(angle),   cosf(angle),  0.0f,  0.0f },
			{ 0.0f,          0.0f,         1.0f,  0.0f },
			{ 0.0f,          0.0f,         0.0f,  1.0f }
		};
		float tmp[3];
		for (int i=0; i<4; i++)
		{
			m_WorldToBody(tmp,m_wheel[i]);
			V3Mul3x3(tmp,tmp,rot);
			m_BodyToWorld(m_wheel[i],tmp);
		}
		m_WorldToBody(tmp,m_pos);
		V3Mul3x3(tmp,tmp,rot);
		m_BodyToWorld(m_pos,tmp);
	}

#endif

	float wheelbase = m_suspension.m_rollCenter[0][0] - m_suspension.m_rollCenter[2][0];
	// set cog heigth
	float springRayRear[3] = {0.0f,0.0f,0.0f};
	float springRayFront[3] = {0.0f,0.0f,0.0f};
	float wheelPos[3];
	m_WorldToBody(wheelPos,m_wheel[CAR_WHEEL_LR]);
	springRayRear[1] = wheelPos[1] - m_suspension.m_springBodyPoint[1][1];
	springRayRear[2] = m_suspension.m_springLength[CAR_WHEEL_LR] + m_suspension.m_springBodyPoint[1][2];
	V3Norm(springRayRear);
	float cogRear = springRayRear[2] * m_suspension.m_springLength[CAR_WHEEL_LR];
	m_WorldToBody(wheelPos,m_wheel[CAR_WHEEL_LF]);
	springRayFront[1] = wheelPos[1] - m_suspension.m_springBodyPoint[0][1];
	springRayFront[2] = m_suspension.m_springLength[CAR_WHEEL_LF] + m_suspension.m_springBodyPoint[0][2];
	V3Norm(springRayFront);
	float cogFront = springRayFront[2] * m_suspension.m_springLength[CAR_WHEEL_LF];
	m_cogHeight = (cogRear * ABS(m_suspension.m_rollCenter[CAR_WHEEL_LR][0]) / wheelbase) +
				  (cogFront * ABS(m_suspension.m_rollCenter[CAR_WHEEL_LF][0]) / wheelbase);

	// setup spring load const
	for (int tire = 0; tire<TIRECOUNT; tire++)
	{
		float wheelpercentage = 0.5f * (1.0f - (ABS(m_suspension.m_rollCenter[tire][0]) / wheelbase));
		m_springLoadConstant[tire] = wheelpercentage * m_bodyMass * g_gravity[2];
	}

	// initialize the wall segments
	for ( i=0; i<CACHED_WALL_COUNT; i++)
	{
		m_wall[i].m_planeValid = 0;
	}
	m_corner[0][0] = m_length * 0.5f + m_boxOffset;
	m_corner[0][1] = m_width * 0.5f;
	m_corner[1][0] = m_length * 0.5f + m_boxOffset;
	m_corner[1][1] = -m_width * 0.5f;
	m_corner[2][0] = -m_length * 0.5f + m_boxOffset;
	m_corner[2][1] = m_width * 0.5f;
	m_corner[3][0] = -m_length * 0.5f + m_boxOffset;
	m_corner[3][1] = -m_width * 0.5f;

}

void PhysicsCar::m_ComputeTransformMatrix()
{
	float w2,x2,y2,z2,xy,wz,xz,wy,yz,wx;
	if (m_normalizeQuaternion)
	{
		QNORMALISE(m_quat);
	}

	w2 = m_quat[WQ]*m_quat[WQ];
	x2 = m_quat[XQ]*m_quat[XQ];
	y2 = m_quat[YQ]*m_quat[YQ];
	z2 = m_quat[ZQ]*m_quat[ZQ];

	xy = 2.0f*m_quat[XQ]*m_quat[YQ];
	wz = 2.0f*m_quat[WQ]*m_quat[ZQ];
	xz = 2.0f*m_quat[XQ]*m_quat[ZQ];
	wy = 2.0f*m_quat[WQ]*m_quat[YQ];
	yz = 2.0f*m_quat[YQ]*m_quat[ZQ];
	wx = 2.0f*m_quat[WQ]*m_quat[XQ];

	// Fill in the Rotation terms
	m_transform[0][0] = w2+x2-y2-z2;
	m_transform[0][1] = xy+wz;
	m_transform[0][2] = xz-wy;

	m_transform[0][3] = 0.0f;

	m_transform[1][0] = xy-wz;
	m_transform[1][1] = w2-x2+y2-z2;
	m_transform[1][2] = yz+wx;

	m_transform[1][3] = 0.0f;

	m_transform[2][0] = xz+wy;
	m_transform[2][1] = yz-wx;
	m_transform[2][2] = w2-x2-y2+z2;

	m_transform[2][3] = 0.0f;

	// Fill in Translations
	m_transform[3][0] = m_pos[0];
	m_transform[3][1] = m_pos[1];
	m_transform[3][2] = m_pos[2];
	  
	m_transform[0][3] = 0.0f;
	m_transform[1][3] = 0.0f;
	m_transform[2][3] = 0.0f;
	m_transform[3][3] = 1.0f;

	// compute the inverse matrix
	for (int i=0; i<3; i++)
	{
		m_invTransform[3][i] = 0.0f;
		for (int j=0; j<3; j++)
		{
			m_invTransform[i][j] = m_transform[j][i];
			m_invTransform[3][i] += -m_transform[3][j] * m_transform[i][j];
		}
	}
	m_invTransform[0][3] = 0.0f;
	m_invTransform[1][3] = 0.0f;
	m_invTransform[2][3] = 0.0f;
	m_invTransform[3][3] = 1.0f;
}




void PhysicsCar::m_GetTireDemand(int tire, float steerAngle, TRACTION *out)
{
}


#define OVERSTEERTHRESHOLD 4.0f
#define MAXSTEERDEMAND 2.5f
 
void PhysicsCar::m_HandleDriverInput(Input *in)
{	
}



void PhysicsCar::m_LocalWheelVel(float *localWheelVel, int tire)
{

}



float PhysicsCar::m_DiffTanSlip(float wheelTanSlip, float vY, float absVX)
{

#if 0
	float current = wheelTanSlip;
//	float diffTanSlip = (target - current)*15;//m_tireRelaxLength;
	float diffTanSlip = SIGN(target - current) * 1.7f;

#endif

	#define EPS2 1.0f
	// calculate the tan slip angle
//	m_tireRelaxLength = 0.5f;
	float diffTanSlip = (vY - wheelTanSlip*absVX)/m_tireRelaxLength;
//	printf("wheelTanSlip[%d] - %10.6f   ",tire, wheelTanSlip[tire]);
	if (absVX < EPS2) 
	{
		diffTanSlip *= 1.0f + 50.0f * (1.0f-absVX/EPS2);
	}

														  
	/*
	if (diffTanSlip > 150.0f)
		return 150.0f;
	if (diffTanSlip < -150.0f)
		return -150.0f;
	*/

	return diffTanSlip;
}


 
void PhysicsCar::m_UpdatePosition(float dt)
{

	// which walls do we need to check against
	m_UpdateActivePatchGroups();

	int loopCount = (dt == 1.0f/60.0f)? 5 : 6;

	int tire;
	float velTmp = m_posVel[0] * m_posVel[0] + m_posVel[1] * m_posVel[1];
//	float roughness;
	for (tire =0; tire<TIRECOUNT; tire++)
	{

#if LEVEL_GROUND
		m_groundHeight[tire] = 3.3f;
		m_groundNormal[tire][0] = 0.0f;
		m_groundNormal[tire][1] = 0.0f;
		m_groundNormal[tire][2] = 1.0f;
#endif
	}


	float timeStep;
	float timeAdjust;
	short penetrating;
	int wall = 0;
	int hitWall = 0;
	short saveOff = false;
	static float carState[NUMBEROFEQU];
	static float diff[NUMBEROFEQU];
	static float newCarState[NUMBEROFEQU];
#if TESTING_WALLS
	short section;
	WallSegment derekWall[5][CACHED_WALL_COUNT];
	float derekState[5][NUMBEROFEQU];
	float derekTimeStep;
	float derekMatrix[4][4];
	memcpy(derekState[0], carState,sizeof(carState));
	memcpy(derekWall[0], m_wall,sizeof(m_wall));
	derekTimeStep = MYTIMESTEP;
		
#endif


	for (int loop=0; loop<loopCount; loop++)
	{
		V3Set(carState, m_pos);
		V3Set(carState+POSVELOFFSET, m_posVel);
		V4Set(carState+QUATOFFSET, m_quat);
		V3Set(carState+ROTVELOFFSET, m_rotVel);
		V3Set(carState+WHEELOFFSET, m_wheel[0]);
		V3Set(carState+WHEELOFFSET+3, m_wheel[1]);
		V3Set(carState+WHEELOFFSET+6, m_wheel[2]);
		V3Set(carState+WHEELOFFSET+9, m_wheel[3]);
		V3Set(carState+WHEELVELOFFSET, m_wheelVel[0]);
		V3Set(carState+WHEELVELOFFSET+3, m_wheelVel[1]);
		V3Set(carState+WHEELVELOFFSET+6, m_wheelVel[2]);
		V3Set(carState+WHEELVELOFFSET+9, m_wheelVel[3]);
		V4Set(carState+WHEELSPINOFFSET, m_wheelSpinVel);
//		V4Set(carState+WHEELTANSLIPOFFSET, m_wheelTanSlip);

#if TESTING_WALLS
		memcpy(derekState[loop],carState,sizeof(carState));
		memcpy(derekWall[loop],m_wall,sizeof(m_wall));
		section = 1;
#endif
		timeStep = MYTIMESTEP;
		timeAdjust = MYTIMESTEP;
		penetrating = false;
		wall = 0;
		hitWall = 0;
		saveOff = true;
		m_normalizeQuaternion = true;
		m_EvalDiff(carState, diff);
		m_Integrator(carState, diff, timeStep, newCarState);
		m_normalizeQuaternion = false;

		V3Set(m_pos, newCarState);
		V3Set(m_posVel, newCarState+POSVELOFFSET);
		V4Set(m_quat, newCarState+QUATOFFSET);
		V3Set(m_rotVel, newCarState+ROTVELOFFSET);
		V3Set(m_wheel[0], newCarState+WHEELOFFSET);
		V3Set(m_wheel[1], newCarState+WHEELOFFSET+3);
		V3Set(m_wheel[2], newCarState+WHEELOFFSET+6);
		V3Set(m_wheel[3], newCarState+WHEELOFFSET+9);
		V3Set(m_wheelVel[0], newCarState+WHEELVELOFFSET);
		V3Set(m_wheelVel[1], newCarState+WHEELVELOFFSET+3);
		V3Set(m_wheelVel[2], newCarState+WHEELVELOFFSET+6);
		V3Set(m_wheelVel[3], newCarState+WHEELVELOFFSET+9);
		V4Set(m_wheelSpinVel, newCarState+WHEELSPINOFFSET);

		m_AdjustWheelHeights(carState, newCarState);
		m_CheckForGroundingCorners();
		

	}
	m_CheckCollisionWithCone();

	m_CheckGimbleLock();

	m_RePositionWheels();

	// update wheel rotation
	for (tire =0; tire<TIRECOUNT; tire++)
	{
		m_wheelRotationAngle[tire] += m_wheelSpinVel[tire] * dt;
		while (m_wheelRotationAngle[tire] > GAME_2PI)
		{
			m_wheelRotationAngle[tire] -= GAME_2PI;
		}
		while (m_wheelRotationAngle[tire] < 0.0f)
		{
			m_wheelRotationAngle[tire] += GAME_2PI;
		}		
	}
}

void PhysicsCar::m_AdjustWheelHeights(float *carOld, float *carNew)
{
	float dist[2];
	for (int tire=0; tire<TIRECOUNT; tire++)
	{
		V2Sub(dist, carNew+WHEELOFFSET + tire*3, carOld+WHEELOFFSET + tire*3);
		m_groundHeight[tire] -= V2Dot(dist, m_groundNormal[tire]);
	}
}

void PhysicsCar::m_RePositionWheels(void)
{

}

void PhysicsCar::m_CheckGimbleLock()
{
}

//  m_WorldToBodyPos
//  converts world positions to body positions
void PhysicsCar::m_WorldToBody(float *out, float *in)
{
	V3Mul4x4(out,in,m_invTransform);
}


void PhysicsCar::m_WorldToBodyRotate(float *out, float *in)
{
	float tmp[3];
	for (int i=0; i<3; i++)
	{
		tmp[i] = 0.0f;
		for (int j=0; j<3; j++)
		{
			tmp[i] += in[j] * m_invTransform[j][i];
		}
	}
	out[0] = tmp[0];
	out[1] = tmp[1];
	out[2] = tmp[2];
}

void PhysicsCar::m_BodyToWorld(float *out, float *in)
{
	V3Mul4x4(out,in,m_transform);
}

void PhysicsCar::m_BodyToWorldRotate(float *out, float *in)
{
	float tmp[3];
	for (int i=0; i<3; i++)
	{
		tmp[i] = 0.0f;
		for (int j=0; j<3; j++)
		{
			tmp[i] += in[j] * m_transform[j][i];
		}
	}
	out[0] = tmp[0];
	out[1] = tmp[1];
	out[2] = tmp[2];

}

//-------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------

void PhysicsCar::m_UpdateWheelDifferentials()
{
}


//  m_UpdateSpringForces  
//  all forces are in local space. This computes the spring force including damper
//  for each wheel.
void PhysicsCar::m_UpdateSpringForces(float springForce[4][3], float rcToWheel[4][3],
									  float wheelPos[4][3], float wheelVel[4][3],
									  float posVel[3], float rotVel[3])
{
}

void PhysicsCar::m_UpdateAerodynamicForces(float bodyVel[3], float aeroForce[3], float downForce[2])
{
}

// X component of springForce will be 0
void PhysicsCar::m_UpdateBodyTorque(float bodyTorque[3], float springForce[4][3], float downForce[2])
{
}

void PhysicsCar::m_UpdateBodyForces(float bodyForce[3], float springForce[4][3],
									float aeroForce[3], float downForce[2])
{
}


void PhysicsCar::m_UpdateWheelForces(float wheelForce[4][3], float springForce[4][3], float wheelHeightWorld[4])
{
}


// m_UpdateTireForces addes the longitudinal and lateral tire forces to the wheels
void PhysicsCar::m_UpdateTireForces(float wheelForce[4][3], float wheelVel[4][3],
									float wheelSpinVel[4], float wheelTanSlip[4],
									float wheelHeightWorld[4])
{

}



void PhysicsCar::m_ComputeConstraintForces(float fc[8],float tireForce[4][3],
										   float bodyForce[3], float bodyTorque[3], float wheelPos[4][3], 
										   float wheelVel[4][3], float rcToWheel[4][3],
										   float posVel[3], float rotVel[3])
{
#if 0
	for (int i=0; i<4; i++)
	{
		printf("wheel %f, %f, %f\n", wheelPos[i][0], wheelPos[i][1], wheelPos[i][2]);
		printf("rc %f, %f, %f\n", rcToWheel[i][0], rcToWheel[i][1], rcToWheel[i][2]);
	}
#endif
	float a[8][8];
	m_ComputeCoefMatrixForConstraintForces(a, wheelPos, rcToWheel);

	fc[0] = m_ComputeResultFromEqB(rcToWheel[0], m_suspension.m_rollCenter[0], bodyTorque, tireForce[0],
								  bodyForce,wheelVel[0],posVel,rotVel);
	fc[1] = m_ComputeResultFromEqA(wheelPos[0], bodyTorque,tireForce[0], bodyForce,
								  wheelVel[0], posVel, rotVel);
	fc[2] = m_ComputeResultFromEqB(rcToWheel[1], m_suspension.m_rollCenter[1], bodyTorque, tireForce[1],
		                          bodyForce, wheelVel[1], posVel, rotVel);
	fc[3] = m_ComputeResultFromEqA(wheelPos[1], bodyTorque, tireForce[1], bodyForce,
		                          wheelVel[1], posVel, rotVel);

	fc[4] = m_ComputeResultFromEqB(rcToWheel[2], m_suspension.m_rollCenter[2], bodyTorque, tireForce[2],
		                          bodyForce, wheelVel[2], posVel, rotVel);
	fc[5] = m_ComputeResultFromEqA(wheelPos[2], bodyTorque, tireForce[2], bodyForce,
		                          wheelVel[2], posVel, rotVel);

	fc[6] = m_ComputeResultFromEqB(rcToWheel[3], m_suspension.m_rollCenter[3], bodyTorque, tireForce[3],
		                          bodyForce,wheelVel[3], posVel, rotVel);
	fc[7] = m_ComputeResultFromEqA(wheelPos[3], bodyTorque, tireForce[3], bodyForce,
		                          wheelVel[3], posVel, rotVel);

	// use gauss elimination to solve Ax = b
	float tmp;
	// Gauss part
	for (int i=0; i<8; i++)
	{
		float div = 1.0f/a[i][i];
		for (int j=i; j<8; j++)
		{
			a[i][j] *= div;
		}
		fc[i] *= div;

		for (int r=i+1; r<8; r++)
		{
			fc[r] -= fc[i] * a[r][i];
			tmp = a[r][i];
			for (int j=i; j<8; j++)
			{
				a[r][j] -= a[i][j] * tmp;
			}
		}
	}
	// backwards substitution
	for (i=7; i>=0; i--)
	{
		for (int j=i+1; j<8; j++)
		{
			fc[i] -= a[i][j] * fc[j]; 
		}
	}
	// array fc now contains the constraint forces (i hope)
}



void PhysicsCar::m_ComputeCoefMatrixForConstraintForces(float out[8][8], float wheelPos[4][3],
														float rcToWheel[4][3])
{
#if 0
	for (int i=0; i<4; i++)
	{
	printf("rollCenter %f, %f, %f\n", m_suspension.m_rollCenter[i][0], 
									  m_suspension.m_rollCenter[i][1], m_suspension.m_rollCenter[i][2]);
	}
#endif
	// first row 
	out[0][0] = m_ComputeCoefFromEqC(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[0], rcToWheel[0]);
	out[0][1] = m_ComputeCoefFromEqC(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[0], g_constraintX);
	out[0][2] = m_ComputeCoefFromEqD(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[1], rcToWheel[1]);
	out[0][3] = m_ComputeCoefFromEqD2(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[1]);
	out[0][4] = m_ComputeCoefFromEqD(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[2], rcToWheel[2]);
	out[0][5] = m_ComputeCoefFromEqD2(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[2]);
	out[0][6] = m_ComputeCoefFromEqD(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[3], rcToWheel[3]);
	out[0][7] = m_ComputeCoefFromEqD2(rcToWheel[0], m_suspension.m_rollCenter[0], wheelPos[3]);

	// second row
	out[1][0] = m_ComputeCoefFromEqA(wheelPos[0], rcToWheel[0]);
	out[1][1] = m_ComputeCoefFromEqA(wheelPos[0], g_constraintX);
	out[1][2] = m_ComputeCoefFromEqB(wheelPos[0], wheelPos[1], rcToWheel[1]);
	out[1][3] = m_ComputeCoefFromEqB2(wheelPos[0], wheelPos[1]);
	out[1][4] = m_ComputeCoefFromEqB(wheelPos[0], wheelPos[2], rcToWheel[2]);
	out[1][5] = m_ComputeCoefFromEqB2(wheelPos[0], wheelPos[2]);
	out[1][6] = m_ComputeCoefFromEqB(wheelPos[0], wheelPos[3], rcToWheel[3]);
	out[1][7] = m_ComputeCoefFromEqB2(wheelPos[0], wheelPos[3]);

	// third row
	out[2][0] = m_ComputeCoefFromEqD(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[0], rcToWheel[0]);
	out[2][1] = m_ComputeCoefFromEqD2(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[0]);
	out[2][2] = m_ComputeCoefFromEqC(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[1], rcToWheel[1]);
	out[2][3] = m_ComputeCoefFromEqC(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[1], g_constraintX);
	out[2][4] = m_ComputeCoefFromEqD(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[2], rcToWheel[2]);
	out[2][5] = m_ComputeCoefFromEqD2(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[2]);
	out[2][6] = m_ComputeCoefFromEqD(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[3], rcToWheel[3]);
	out[2][7] = m_ComputeCoefFromEqD2(rcToWheel[1], m_suspension.m_rollCenter[1], wheelPos[3]);

	// fourth row
	out[3][0] = m_ComputeCoefFromEqB(wheelPos[1], wheelPos[0], rcToWheel[0]);
	out[3][1] = m_ComputeCoefFromEqB2(wheelPos[1], wheelPos[0]);
	out[3][2] = m_ComputeCoefFromEqA(wheelPos[1], rcToWheel[1]);
	out[3][3] = m_ComputeCoefFromEqA(wheelPos[1], g_constraintX);
	out[3][4] = m_ComputeCoefFromEqB(wheelPos[1], wheelPos[2], rcToWheel[2]);
	out[3][5] = m_ComputeCoefFromEqB2(wheelPos[1], wheelPos[2]);
	out[3][6] = m_ComputeCoefFromEqB(wheelPos[1], wheelPos[3], rcToWheel[3]);
	out[3][7] = m_ComputeCoefFromEqB2(wheelPos[1], wheelPos[3]);

	// fifth row
	out[4][0] = m_ComputeCoefFromEqD(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[0], rcToWheel[0]);
	out[4][1] = m_ComputeCoefFromEqD2(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[0]);
	out[4][2] = m_ComputeCoefFromEqD(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[1], rcToWheel[1]);
	out[4][3] = m_ComputeCoefFromEqD2(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[1]);
	out[4][4] = m_ComputeCoefFromEqC(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[2], rcToWheel[2]);
	out[4][5] = m_ComputeCoefFromEqC(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[2], g_constraintX);
	out[4][6] = m_ComputeCoefFromEqD(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[3], rcToWheel[3]);
	out[4][7] = m_ComputeCoefFromEqD2(rcToWheel[2], m_suspension.m_rollCenter[2], wheelPos[3]);

	// sixth row
	out[5][0] = m_ComputeCoefFromEqB(wheelPos[2], wheelPos[0], rcToWheel[0]);
	out[5][1] = m_ComputeCoefFromEqB2(wheelPos[2], wheelPos[0]);
	out[5][2] = m_ComputeCoefFromEqB(wheelPos[2], wheelPos[1], rcToWheel[1]);
	out[5][3] = m_ComputeCoefFromEqB2(wheelPos[2], wheelPos[1]);
	out[5][4] = m_ComputeCoefFromEqA(wheelPos[2], rcToWheel[2]);
	out[5][5] = m_ComputeCoefFromEqA(wheelPos[2], g_constraintX);
	out[5][6] = m_ComputeCoefFromEqB(wheelPos[2], wheelPos[3], rcToWheel[3]);
	out[5][7] = m_ComputeCoefFromEqB2(wheelPos[2], wheelPos[3]);

	// seventh row
	out[6][0] = m_ComputeCoefFromEqD(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[0], rcToWheel[0]);
	out[6][1] = m_ComputeCoefFromEqD2(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[0]);
	out[6][2] = m_ComputeCoefFromEqD(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[1], rcToWheel[1]);
	out[6][3] = m_ComputeCoefFromEqD2(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[1]);
	out[6][4] = m_ComputeCoefFromEqD(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[2], rcToWheel[2]);
	out[6][5] = m_ComputeCoefFromEqD2(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[2]);
	out[6][6] = m_ComputeCoefFromEqC(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[3], rcToWheel[3]);
	out[6][7] = m_ComputeCoefFromEqC(rcToWheel[3], m_suspension.m_rollCenter[3], wheelPos[3], g_constraintX);

	// eigth row
	out[7][0] = m_ComputeCoefFromEqB(wheelPos[3], wheelPos[0], rcToWheel[0]);
	out[7][1] = m_ComputeCoefFromEqB2(wheelPos[3], wheelPos[0]);
	out[7][2] = m_ComputeCoefFromEqB(wheelPos[3], wheelPos[1], rcToWheel[1]);
	out[7][3] = m_ComputeCoefFromEqB2(wheelPos[3], wheelPos[1]);
	out[7][4] = m_ComputeCoefFromEqB(wheelPos[3], wheelPos[2], rcToWheel[2]);
	out[7][5] = m_ComputeCoefFromEqB2(wheelPos[3], wheelPos[2]);
	out[7][6] = m_ComputeCoefFromEqA(wheelPos[3], rcToWheel[3]);
	out[7][7] = m_ComputeCoefFromEqA(wheelPos[3], g_constraintX);
	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			printf("out[%d][%d] = %f\n", i, j, out[i][j]);
		}
	}
	PrintCustomFault("Done with first loop:\n");
}


// wc is position of wheel contact i, r is vector from body COG to wheel contact j,
// contraint is contraint vector on tire contact j. All are in body space. 
float PhysicsCar::m_ComputeCoefFromEqA(float wheelPos[3], float constraint[3])
{
	float result = m_ComputeCoefFromEqB(wheelPos, wheelPos, constraint);
	result -= (constraint[0] * m_invWheelMass);
	return (result);
}

float PhysicsCar::m_ComputeCoefFromEqB(float wheelPos[3], float ray[3], float constraint[3])
{
	float tmp[3],tmp2[3];
	float result;
#if 0
	V3Cross(tmp,ray,constraint);
	V3Mul3x3(tmp,tmp,m_xMatrix);
	V3Cross(tmp,g_constraintX,tmp);
	result = -V3Dot(wheelPos,tmp);
	V3Mul(tmp,constraint,m_invBodyMass);
	result -= tmp[0]; // V3Dot(g_constraintX,tmp);
#else
	tmp[0] = 0.0f;
	tmp[1] = ray[2] * constraint[0] - ray[0] * constraint[2];
	tmp[2] = ray[0] * constraint[1] - ray[1] * constraint[0];
	tmp[1] *= m_xMatrix[1][1];
	tmp[2] *= m_xMatrix[2][2];
	tmp2[0] = 0.0f;
	tmp2[1] = -tmp[2];
	tmp2[2] = tmp[1];
	result = -(wheelPos[1]*tmp2[1] + wheelPos[2]*tmp2[2]);
	result -= (constraint[0] * m_invBodyMass);
#endif
	return (result);
}

float PhysicsCar::m_ComputeCoefFromEqB2(float wheelPos[3], float ray[3])
{
	float tmp[3];
	float result;
	tmp[0] = 0.0f;
	tmp[1] = ray[1] * m_xMatrix[2][2];
	tmp[2] = ray[2] * m_xMatrix[1][1];
	result = -(wheelPos[1]*tmp[1] + wheelPos[2]*tmp[2]) - m_invBodyMass;

	return (result);
}


// constraint1 is rcToWheel so no X component
float PhysicsCar::m_ComputeCoefFromEqC(float constraint1[3], float rc[3], 
									   float ray[3], float constraint2[3])
{
	float tmp[3];
	float result = m_ComputeCoefFromEqD(constraint1, rc, ray, constraint2);
	tmp[0] = 0.0f;
	tmp[1] = constraint2[1] * m_invWheelMass;
	tmp[2] = constraint2[2] * m_invWheelMass;
	result += (constraint1[1] * tmp[1] + constraint1[2] * tmp[2]);
	return (result);
}

// constraint1 is rcToWheel so no X component
float PhysicsCar::m_ComputeCoefFromEqD(float constraint1[3], float rc[3], 
									   float ray[3], float constraint2[3])
{
//	printf("const1 %5.8f, %5.8f, %5.8f\n", constraint1[0], constraint1[1], constraint1[2]);
//	printf("rc %5.8f, %5.8f, %5.8f\n", rc[0], rc[1], rc[2]);
//	printf("ray %5.8f, %5.8f, %5.8f\n", ray[0], ray[1], ray[2]);
//	printf("const2 %5.8f, %5.8f, %5.8f\n",constraint2[0], constraint2[1], constraint2[2]);
	float tmp[3],tmp2[3];
	float result;
#if 0
	V3Cross(tmp,ray,constraint2);
	V3Mul3x3(tmp,tmp,m_xMatrix);
	V3Cross(tmp,rc,tmp);
	result = -V3Dot(constraint1,tmp);
	V3Mul(tmp,constraint2,m_invBodyMass);
	result += V3Dot(constraint1,tmp);
#else
	tmp[0] = ray[1] * constraint2[2] - ray[2] * constraint2[1];
	tmp[1] = ray[2] * constraint2[0] - ray[0] * constraint2[2];
	tmp[2] = ray[0] * constraint2[1] - ray[1] * constraint2[0];
	tmp[0] *= m_xMatrix[0][0];
	tmp[1] *= m_xMatrix[1][1];
	tmp[2] *= m_xMatrix[2][2];
	tmp2[0] = 0.0f;
	tmp2[1] = rc[2] * tmp[0] - rc[0] * tmp[2];
	tmp2[2] = rc[0] * tmp[1] - rc[1] * tmp[0];
	result = -(constraint1[1] * tmp2[1] + constraint1[2]*tmp2[2]);
	tmp[0] = 0.0f;
	tmp[1] = constraint2[1] * m_invBodyMass;
	tmp[2] = constraint2[2] * m_invBodyMass;
	result += (constraint1[1] * tmp[1] + constraint1[2]*tmp[2]);
#endif
	return (result);

}

// constraint1 is rcToWheel so no X component
float PhysicsCar::m_ComputeCoefFromEqD2(float constraint1[3], float rc[3], 
									    float wheelPos[3])
{
	float tmp[3],tmp2[3];
	float result;
	tmp[0] = 0.0f;
	tmp[1] = wheelPos[2] * m_xMatrix[1][1];
	tmp[2] = -wheelPos[1] * m_xMatrix[2][2];
	tmp2[0] = 0.0f;
	tmp2[1] = -rc[0] * tmp[2];
	tmp2[2] = rc[0] * tmp[1];
	result = -(constraint1[1] * tmp2[1] + constraint1[2] *tmp2[2]);

	return (result);
}

float PhysicsCar::m_ComputeResultFromEqA(float tirePos[3], float bodyTorque[3],
										 float tireForce[3], float bodyForce[3],
										 float wheelVel[3], float posVel[3], float rotVel[3])
{
	float tmp[3],tmp2[3];
	float result;
#if 0
	// first term
	V3Cross(tmp, rotVel, g_constraintX);
	V3Cross(tmp, rotVel, tmp);
	result = V3Dot(tirePos, tmp);
	// second term
	V3Mul3x3(tmp, bodyTorque, m_xMatrix);
	V3Cross(tmp, g_constraintX, tmp);
	result -= V3Dot(tirePos, tmp);
	// third term
	V3Cross(tmp, rotVel, g_constraintX);
	result += (2.0f*V3Dot(tmp, wheelVel));

	// fourth term
	V3Cross(tmp, rotVel, g_constraintX);
	result -= (2.0f*V3Dot(tmp, posVel));
	// fifth term
	V3Mul(tmp,tireForce, m_invWheelMass);
	result += tmp[0]; // V3Dot(g_constraintX,tmp);
	// sixth term
	V3Mul(tmp,bodyForce, m_invBodyMass);
	result -= tmp[0]; // V3Dot(g_constraintX,tmp);
#else
	// first term
	tmp[0] = 0.0f;
	tmp[1] = rotVel[2];
	tmp[2] = -rotVel[1];
	tmp2[0] = rotVel[1] * tmp[2] - rotVel[2] * tmp[1];
	tmp2[1] = -rotVel[0] * tmp[2];
	tmp2[2] = rotVel[0] * tmp[1];
	result = (tirePos[0] * tmp2[0] + tirePos[1] * tmp2[1] + tirePos[2] * tmp2[2]);
	// second term
	tmp[0] = bodyTorque[0] * m_xMatrix[0][0];
	tmp[1] = bodyTorque[1] * m_xMatrix[1][1];
	tmp[2] = bodyTorque[2] * m_xMatrix[2][2];
	tmp2[0] = 0.0f;
	tmp2[1] = -tmp[2];
	tmp2[2] = tmp[1];
	result -= (tirePos[1] * tmp2[1] + tirePos[2] * tmp2[2]);
	// third term
	tmp[0] = 0.0f;
	tmp[1] = rotVel[2];
	tmp[2] = -rotVel[1];
	result += (2.0f*(wheelVel[1] * tmp[1] + wheelVel[2] * tmp[2]));
	// fourth term
	tmp[0] = 0.0f;
	tmp[1] = rotVel[2];
	tmp[2] = -rotVel[1];
	result -= (2.0f*(posVel[1] * tmp[1] + posVel[2] * tmp[2]));
	// fifth term
	result += tireForce[0] * m_invWheelMass; 
	// sixth term
	result -= bodyForce[0] * m_invBodyMass; 
#endif

	return (-result);
}

// m_ComputeResultFromEqB
float PhysicsCar::m_ComputeResultFromEqB(float rcToWheel[3], float rc[3], float bodyTorque[3],
										 float tireForce[3], float bodyForce[3], 
										 float wheelVel[3], float posVel[3], float rotVel[3])
{
	float tmp[3],tmp2[3];
	float result;
#if 0
	// first term
	V3Cross(tmp, rotVel, rc);
	V3Add(tmp, posVel, tmp);
	V3Sub(tmp, wheelVel, tmp);
	result = -V3Dot(tmp, tmp);
	// second term
	V3Mul(tmp,tireForce, m_invWheelMass);
	result -= V3Dot(rcToWheel,tmp);
	// third term
	V3Mul3x3(tmp,bodyTorque, m_xMatrix);
	V3Cross(tmp, rc, tmp);
	result -= V3Dot(rcToWheel, tmp);
	// fourth term
	V3Cross(tmp, rotVel, rc);
	V3Cross(tmp, rotVel, tmp);
	result += V3Dot(rcToWheel, tmp);
	// fifth term
	V3Mul(tmp, bodyForce, m_invBodyMass);
	result += V3Dot(rcToWheel, tmp);
#else
	// first term
	tmp[0] = rotVel[1] * rc[2] - rotVel[2] * rc[1] + posVel[0] - wheelVel[0];
	tmp[1] = rotVel[2] * rc[0] - rotVel[0] * rc[2] + posVel[1] - wheelVel[1];
	tmp[2] = rotVel[0] * rc[1] - rotVel[1] * rc[0] + posVel[2] - wheelVel[2];
	result = -(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]);
	// second term
	tmp[0] = 0.0f;
	tmp[1] = tireForce[1] * m_invWheelMass;
	tmp[2] = tireForce[2] * m_invWheelMass;
	result -= (rcToWheel[1] * tmp[1] + rcToWheel[2] * tmp[2]);
	// third term
	tmp[0] = bodyTorque[0]*m_xMatrix[0][0];
	tmp[1] = bodyTorque[1]*m_xMatrix[1][1];
	tmp[2] = bodyTorque[2]*m_xMatrix[2][2];
	tmp2[0] = 0.0f;
	tmp2[1] = rc[2] * tmp[0] - rc[0] * tmp[2];
	tmp2[2] = rc[0] * tmp[1] - rc[1] * tmp[0];
	result -= (rcToWheel[1] * tmp2[1] + rcToWheel[2] * tmp2[2]);
	// fourth term
	tmp[0] = rotVel[1] * rc[2] - rotVel[2] * rc[1];
	tmp[1] = rotVel[2] * rc[0] - rotVel[0] * rc[2];
	tmp[2] = rotVel[0] * rc[1] - rotVel[1] * rc[0];
	tmp2[1] = rotVel[2] * tmp[0] - rotVel[0] * tmp[2];
	tmp2[2] = rotVel[0] * tmp[1] - rotVel[1] * tmp[0];
	result += (rcToWheel[1] * tmp2[1] + rcToWheel[2] * tmp2[2]);
	// fifth term
	tmp[0] = 0.0f;
	tmp[1] = bodyForce[1] * m_invBodyMass;
	tmp[2] = bodyForce[2] * m_invBodyMass;
	result += (rcToWheel[1] * tmp[1] + rcToWheel[2] * tmp[2]);
#endif
	return (-result);
}


void PhysicsCar::m_EvalDiff(float *in, float *out)
{
	float wheelPos[TIRECOUNT][3];
	float wheelVel[TIRECOUNT][3];
	float rcToWheel[TIRECOUNT][3];
	float springForce[4][3];
	float aeroForce[3];
	float downForce[2]; // front and back
	float wheelForce[4][3];
	float bodyForce[3] = {0.0f, 0.0f, 0.0f};
	float bodyTorque[3] = {0.0f,0.0f,0.0f};
	float fc[8];  // fc(i1), fc(i2), fc(j1), fc(j2), fc(k1), fc(k2), fc(l1), fc(l2)
	float posVel[3];
	float rotVel[3];
	float wheelSpinVel[4];
	float wheelTanSlip[4];
	float wheelHeightWorld[4];

	m_ComputeTransformMatrix();

	// convert from world to body space
	for (int i=0; i<TIRECOUNT; i++)
	{
		m_WorldToBody(wheelPos[i], in+WHEELOFFSET+i*3);
		m_WorldToBodyRotate(wheelVel[i], in+WHEELVELOFFSET+i*3);
		rcToWheel[i][0] = 0.0f;
		rcToWheel[i][1] = wheelPos[i][1] - m_suspension.m_rollCenter[i][1];
		rcToWheel[i][2] = wheelPos[i][2] - m_suspension.m_rollCenter[i][2];

		wheelHeightWorld[i] = *(in+WHEELOFFSET+i*3+2);
	}
	m_WorldToBodyRotate(posVel, in+POSVELOFFSET);
	m_WorldToBodyRotate(rotVel, in+ROTVELOFFSET);
	V4Set(wheelSpinVel, in+WHEELSPINOFFSET);
	V4Set(wheelTanSlip, m_wheelTanSlip);


	// Calculate all the forces acting on the body and wheels
	m_UpdateSpringForces(springForce, rcToWheel, wheelPos, wheelVel, posVel, rotVel);
//	printf("spring force is %5.2f, %5.2f, %5.2f\n", springForce[0][0], springForce[0][1], springForce[0][2]);

	m_UpdateAerodynamicForces(posVel, aeroForce, downForce);
//	printf("aero force is %5.2f, %5.2f, %5.2f\n", aeroForce[0], aeroForce[1], aeroForce[2]);

	m_UpdateBodyForces(bodyForce, springForce, aeroForce, downForce);
//	printf("body force is %5.2f, %5.2f, %5.2f\n", bodyForce[0], bodyForce[1], bodyForce[2]);

	m_UpdateBodyTorque(bodyTorque, springForce, downForce);
//	printf("bodyTorque is %5.2f, %5.2f, %5.2f\n", bodyTorque[0], bodyTorque[1], bodyTorque[2]);

	m_UpdateWheelForces(wheelForce, springForce, wheelHeightWorld);
//	printf("wheel force is %5.2f, %5.2f, %5.2f\n", wheelForce[0][0], wheelForce[0][1], wheelForce[0][2]);

	m_UpdateTireForces(wheelForce, wheelVel, wheelSpinVel, wheelTanSlip, wheelHeightWorld);
//	printf("wheel force is %5.2f, %5.2f, %5.2f\n", wheelForce[0][0], wheelForce[0][1], wheelForce[0][2]);

	m_ComputeConstraintForces(fc, wheelForce, bodyForce, bodyTorque, wheelPos, 
							  wheelVel, rcToWheel, posVel, rotVel);
//	printf("fc is \n");
	for (int i=0; i<8; i++)
	{
//		printf("%5.2f\n", fc[i]);
	}
	float tmp[3];
	float acc[3];

	// set pos to be velocity
	V3Set(out, in+POSVELOFFSET);

	// Fci = fc[0] * rcToWheel + fc[1] * g_constraintX
	// body accel = (Fext + Fci + Fcj + Fck + Fcl) / bodyMass
	V3Set(acc, bodyForce);
	for (int tire = 0; tire<TIRECOUNT; tire++)
	{
		acc[0] += fc[tire*2+1];
		acc[1] += fc[tire*2] * rcToWheel[tire][1];
		acc[2] += fc[tire*2] * rcToWheel[tire][2];
	}
	V3Mul(acc, acc, m_invBodyMass);

	m_BodyToWorldRotate(out+POSVELOFFSET, acc);

	// quaternion update
	V3Set(tmp, in+ROTVELOFFSET);
	V3Mul(tmp, tmp, 0.5f);
	VECBYQUAT(out+QUATOFFSET, tmp, in+QUATOFFSET);

	// angular accel = (Text + (ri x Fci) + (rj x Fcj) + (rk x Fck) + (rl x Fcl) - (w x Iw)) / I
	// update rotational velocity and quaternion
	// store off current rotational velocity
	V3Set(acc, bodyTorque);
	for (tire=0; tire<TIRECOUNT; tire++)
	{
		tmp[0] = fc[tire*2+1];
		tmp[1] = fc[tire*2] * rcToWheel[tire][1];
		tmp[2] = fc[tire*2] * rcToWheel[tire][2];
		V3Cross(tmp, wheelPos[tire], tmp);
		V3Add(acc, acc, tmp);
	}
	// subtract off (w x Iw) 
	V3Mul3x3(tmp, rotVel, m_iMatrix);
	V3Cross(tmp, rotVel, tmp);
	V3Sub(acc, acc, tmp);

	V3Mul3x3(acc, acc, m_xMatrix);
	m_BodyToWorldRotate(out+ROTVELOFFSET, acc);

	// wheel acceleration = (Fext - Fci, Fext - Fcj, etc.) / wheelMass
	for (tire=0; tire<TIRECOUNT; tire++)
	{
		V3Set(out+WHEELOFFSET+tire*3,in+WHEELVELOFFSET+tire*3);
		acc[0] = (wheelForce[tire][0] - fc[tire*2+1]) * m_invWheelMass;
		acc[1] = (wheelForce[tire][1] - fc[tire*2] * rcToWheel[tire][1]) * m_invWheelMass;
		acc[2] = (wheelForce[tire][2] - fc[tire*2] * rcToWheel[tire][2]) * m_invWheelMass;
		m_BodyToWorldRotate(out+WHEELVELOFFSET+tire*3,acc);
	}
	V4Set(out+WHEELSPINOFFSET, wheelSpinVel);
//	V4Set(out+WHEELTANSLIPOFFSET, wheelTanSlip);
}

void PhysicsCar::m_IntegratorEuler(float *y, float *dydx, float h, float *yout)
{
	for (int i = 0; i<NUMBEROFEQU; i++) 
	{
		yout[i] = dydx[i]*h + y[i];
	}
}


void PhysicsCar::m_IntegratorRK2(float *y, float *dydx, float h, float *yout) 
{
	float h34;					// Some step contants
	float dyt[NUMBEROFEQU];		//      ditto
	float yt[NUMBEROFEQU];		//      ditto

	h34 = THREE_FOURTHS*h;
	
	// Step 1
	for (int i = 0; i<NUMBEROFEQU; i++) 
	{
		yt[i] = y[i] + h34*dydx[i];
	}

	m_EvalDiff(yt, dyt);

	for (i = 0; i<NUMBEROFEQU; i++) 
	{
		yout[i] = (TWO_THIRDS*dyt[i] + ONE_THIRD*dydx[i])*h + y[i];
	}

}


void PhysicsCar::m_IntegratorRK4(float *y, float *dydx,float h, float *yout) 
{
	int i;
	float hh,h6;				// Some step contants
	float dym[NUMBEROFEQU];		// partial evaluation
	float dyt[NUMBEROFEQU];		//      ditto
	float yt[NUMBEROFEQU];		//      ditto


	// init some values
	
	hh = 0.5f*h;
	h6 = h*(1.0f/6.0f);

	// First Step
	for (i = 0; i<NUMBEROFEQU; i++) 
	{
		yt[i] = y[i] + hh*dydx[i];		// Same as simple euler method to halfway
	}
	
	// Second Step
	m_EvalDiff(yt, dyt);				// use first order approx to compute dirrivatives
	for (i = 0; i<NUMBEROFEQU; i++) 
	{
		yt[i] = y[i] + hh*dyt[i];		// use new diff as actual step (second order approx)
	}


	// Third Step
	m_EvalDiff(yt, dym);				// use second order approx 
	for (i = 0; i<NUMBEROFEQU; i++) 
	{
		yt[i] = y[i]+h*dym[i];			// compte 3rd order approx
		dym[i] += dyt[i];				// accumulate 2nd and 3rd order dirrivatives
	}

	// Forth Step
	m_EvalDiff(yt, dyt);				// use third order approx but step to end
	
	// Accumulate Increments with appropriate weights
	for (i = 0; i<NUMBEROFEQU; i++) 
	{
		yout[i] = y[i] + h6*(dydx[i] + dyt[i] + 2.0f*dym[i]);
	}

}


void PhysicsCar::m_Integrator(float *y, float *dydx,float h, float *yout)
{
	m_IntegratorRK2(y, dydx,h, yout);
//	m_IntegratorEuler(y, dydx,h, yout);
}


void PhysicsCar::m_SetBrakes(float brakes)
{
}

void PhysicsCar::m_SetSteering(float steer)
{
}

void PhysicsCar::m_SetDrive(float revs)
{
}

void PhysicsCar::m_MoveTo(float transform[4][4])
{
//	V3Set(m_transform[3],transform[3]);
}

void PhysicsCar::m_MoveToPosition(V2 pos)
{
}

void PhysicsCar::m_MoveToStart(int position)
{
}

void PhysicsCar::m_SetGear(int gear)
{
//	m_engine.m_currGear = gear;
}

void PhysicsCar::m_GetTransform(float transform[4][4])
{
	M4Set(transform, m_transform);
	V3Mul4x3(transform[POSITION], m_bodyOffset, transform);
}

void PhysicsCar::m_GetWheelTransform(short tire, float transform[4][4], float caliperTransform[4][4])
{
	float rx  = (CAR_WHEEL_RIGHT(tire)? GAME_PI : 0.0f) - m_suspension.m_camber[tire];
	float ry  = (CAR_WHEEL_RIGHT(tire)? -m_wheelRotationAngle[tire] : m_wheelRotationAngle[tire]);
	float rz  =  CAR_WHEEL_FRONT(tire)? (CAR_WHEEL_RIGHT(tire)? -m_steerAngle : m_steerAngle) : 0.0f;

	M4FromYZXRot(transform,        rx,  ry,   rz);

	rx = -m_suspension.m_camber[tire];
	rz =  CAR_WHEEL_FRONT(tire)? m_steerAngle : 0.0f;
	M4FromYZXRot(caliperTransform, rx, 0.0f, rz);

	M4Mul4x3(transform,        m_transform);
	M4Mul4x3(caliperTransform, m_transform);

	V3Mac(transform[POSITION], m_wheel[tire], m_transform[UP], m_engine.m_wheelRadius[tire]);
	V3Set(caliperTransform[POSITION], transform[POSITION]);
}



