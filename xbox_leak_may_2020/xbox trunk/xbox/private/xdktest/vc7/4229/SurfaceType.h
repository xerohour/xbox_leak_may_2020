#ifndef __SURFACETYPE_H
#define __SURFACETYPE_H


enum SurfaceType
{
	SURFACETYPE_ROAD = 0, 
	SURFACETYPE_RUNOFF = 1, 
	SURFACETYPE_SAND = 2, 
	SURFACETYPE_GRASS = 3,
	SURFACETYPE_COUNT
};


// g_groundSlowTable located in PhysicsCar.cxx
extern const float g_groundSlowTable[SURFACETYPE_COUNT];

#endif
