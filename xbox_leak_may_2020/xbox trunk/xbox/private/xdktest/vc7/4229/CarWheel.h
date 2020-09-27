#ifndef __CARWHEEL_H
#define __CARWHEEL_H

#ifndef __VECTOR_H
#include "vector.h"
#endif

struct Car;
struct SkidMarkBlock;


struct CarWheel
{
	// transform wheel into world coords
	M4    m_transform;                  // transform of wheel
	M4    m_caliperTransform;           // transform of caliper, does not rotate
	V3    m_lastPosition;               // position before physics this frame
	float m_lastSpinVel;                // copied from m_car->m_physicsCar
	V2    m_slip;                       // amount of wheel-local slip this frame in m/s
	
	// position of last skid mark, even if it was not actually laid down.
	// essentially works out to previous two corners of strip.
	V3 m_skidPosition[4];               // corners of current skid quad
	// 0 = no skid mark, 255 = black skid mark.
	int m_skidding;                     // how much I am skidding
	int m_skiddingLast;                 // how much I was skidding last frame
	int m_skidTimer;                    // how long I have been skidding

	int m_index;                        // what wheel we are

	Car           *m_car;               // my car
	SkidMarkBlock *m_skidMark;          // my skid marks

	CarWheel();
	void  m_Init();
	void  m_FrameInit();
	void  m_InvalidateLastPosition(void);
	void  m_CalculateVelocities(void);

	void  m_AbortSkidMarks(void);
	void  m_LeaveSkidMarks(void);
};


#endif
