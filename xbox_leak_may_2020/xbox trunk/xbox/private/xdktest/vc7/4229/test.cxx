
#include <stdio.h>
#include "types.h"
#include "game.h"
#include "physicsCar.h"

#define printf myprintf

#if 0
float g_constraintX[3] = {1.0f, 0.0f, 0.0f};
float g_invWheelMass = 0.0227273f;
float g_invBodyMass = 0.000781250f;
	float rollCenter[4][3] = 
	{
		{0.968000f, 0.000000f, -0.249383f},
		{0.968000f, 0.000000f, -0.249383f},
		{-1.450000f, 0.000000f, -0.317419f},
		{-1.450000f, 0.000000f, -0.317419f},
	};
#endif





void main(void)
{
	PhysicsCar phyCar;
	phyCar.m_LoadCar("D:\\AudiTT.pcr");
	phyCar.m_ResetCar();
	while (1)
	{
		phyCar.m_UpdatePosition(1/60.0f);
		PrintCustomFault("Done with first loop:\n");
	}
}



