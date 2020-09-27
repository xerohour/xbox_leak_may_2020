#ifndef __SUSPENSION_H
#define __SUSPENSION_H

#define MAXROLLCENTERTABLE 31
#define TIRECOUNT 4

struct Suspension
{
public:
	float m_springLength[TIRECOUNT];  // from COG to bottom of wheel
	float m_springConstA[TIRECOUNT];
	float m_springConstB[TIRECOUNT];
	float m_springDamper[TIRECOUNT];
	float m_rollCenter[TIRECOUNT][3];
	float m_rcToWheelLength[TIRECOUNT];  // dist from tire to roll center
	float m_camber[TIRECOUNT];          // from behind, counter-clockwise is a positive angle    
	float m_initialCamber[TIRECOUNT];       // angle in radians, from behind counterclockwise is positive
	float m_springBodyPoint[2][3];  // point where spring connects to body of car (local coords) front & rear

	float m_rollCenterFrontTable[MAXROLLCENTERTABLE];   // table holds all roll center heights based on wheel height
	float m_rollCenterRearTable[MAXROLLCENTERTABLE];
	float m_rcToWheelLengthTable[2][MAXROLLCENTERTABLE];
	float m_camberTable[2][MAXROLLCENTERTABLE];

	float m_maxSpringCompression;  // maximum compression of suspension springs

	void m_Init();
	void m_UpdateRollCenter(float wheelHeight,short wheel);
	void m_FillRollCenterTables();
private:

};




#endif