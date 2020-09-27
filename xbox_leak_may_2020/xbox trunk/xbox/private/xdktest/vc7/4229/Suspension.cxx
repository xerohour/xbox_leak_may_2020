#include "suspension.h"
#include "CarWheelNumbers.h"
#include "moremath.h"

// Roll and Camber Tables are filled out in TestGraph.cxx

void Suspension::m_Init()
{
	for (int tire=0; tire<TIRECOUNT; tire++)
	{
		if (CAR_WHEEL_FRONT(tire))
		{
			m_springConstA[tire] = 35000.0f;
			m_springConstB[tire] = 35000.0f;
			m_springDamper[tire] = 3000.0f;
			m_rollCenter[tire][0] = 1.35f;
			m_rollCenter[tire][1] = 0.0f;
			m_rollCenter[tire][2] = -0.35f;
			m_springLength[tire] = 0.30f;
		}
		else
		{
			m_springConstA[tire] = 35000.0f;
			m_springConstB[tire] = 35000.0f;
			m_springDamper[tire] = 3200.0f;
			m_rollCenter[tire][0] = -1.4f;
			m_rollCenter[tire][1] = 0.0f;
			m_rollCenter[tire][2] = -0.35f;
			m_springLength[tire] = 0.32f;
		}
		m_camber[tire] = 0.0f;
		m_initialCamber[tire] = 0.0f;
		m_rcToWheelLength[tire] = 0.8f;
	}
	m_springBodyPoint[0][0] = 0.0f;  // x offset is never used
	m_springBodyPoint[0][1] = 0.7f;
	m_springBodyPoint[0][2] = 0.0f;
	m_springBodyPoint[1][0] = 0.0f;  // x offset is never used
	m_springBodyPoint[1][1] = 0.7f;
	m_springBodyPoint[1][2] = 0.0f;
	for (int i=0; i<MAXROLLCENTERTABLE; i++)
	{
		m_rollCenterFrontTable[i] = 0.0f;
		m_rollCenterRearTable[i] = 0.0f;
		m_rcToWheelLengthTable[0][i] = 0.85f;
		m_rcToWheelLengthTable[1][i] = 0.85f;
		m_camberTable[0][i] = 0.0f;
		m_camberTable[1][i] = 0.0f;
	}

	m_maxSpringCompression = 0.12f;  // initialize to 12 cm ( CANNOT BE GREATER THAN 0.15 WITHOUT CHANGING "MAXROLLCENTERTABLE" in suspension.h)
}

void Suspension::m_UpdateRollCenter(float wheelHeight, short wheel)
{
	short index = (short)((m_springLength[wheel] + wheelHeight) * 100.0f + MAXROLLCENTERTABLE * 0.5f);
	if (index < 0)
	{
		index = 0;
	}
	if (index > MAXROLLCENTERTABLE-1)
	{
		index = MAXROLLCENTERTABLE-1;
	}

	if (wheel < 2)
	{
		// front suspension
		m_rollCenter[wheel][2] = m_rollCenterFrontTable[index];
		m_rcToWheelLength[wheel] = m_rcToWheelLengthTable[0][index];
		m_camber[wheel] = m_camberTable[0][index];
	}
	else
	{
		// rear suspension
		m_rollCenter[wheel][2] = m_rollCenterRearTable[index];
		m_rcToWheelLength[wheel] = m_rcToWheelLengthTable[1][index];
		m_camber[wheel] = m_camberTable[1][index];
	}
	if (CAR_WHEEL_RIGHT(wheel))
	{
		m_camber[wheel] = -m_camber[wheel];
	}
//			m_camber[wheel] = 0.1f;
	m_camber[wheel] += m_initialCamber[wheel];
}


