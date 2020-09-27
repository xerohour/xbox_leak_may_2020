#ifndef __CARGEOMETRY_H
#define __CARGEOMETRY_H

#ifdef EXPORTER
#include <stdio.h>
#endif

#ifndef __VECTOR_H
#include "vector.h"
#endif

#ifndef __FLOATCOLOR_H
#include "FloatColor.h"
#endif
#ifndef __CARWHEELNUMBERS_H
#include "CarWheelNumbers.h"
#endif
#ifndef __FMATERIALDATABASE_H
#include "FMaterialDatabase.h"
#endif

struct FShape;
#ifdef ENABLE_FDEFORMABLESHAPE
struct FDeformableShape;
#endif
struct FJoint;

#define CAR_LOD_LEVELS 4

// storage space: 2mb for now max size
#define CAR_MAX_SIZE (1024 * 256)

// storage space: 2mb for now max size
#define CARGFX_MAX_SIZE (1024*1024 + 512*1024)

// actual size: allows for at least 5m x 3m cars
#define CAR_MAX_RADIUS 3.0f

struct CarWheelGeometry
{
	V3               m_position;              // wheel position relative to origin of car, should be at (?,?,radius)
	FShape          *m_shape[CAR_LOD_LEVELS]; // model data, front two point to same, as do back two
	FShape          *m_caliperShape;          // model data, four different ones

	void             m_Relocate(void *base, void *gfxBase);
};

// used for setting up lens flares
struct CarLight
{
	FloatColor       m_color;                 // color of light
	V3               m_brightness;            // brightness of light

	V3               m_position;              // position of point of light cone
	V3               m_direction;             // direction of light cone
	float            m_cone;                  // visibility of light flare

	void             m_Relocate(void *base);
};

struct CarLightGroup
{
	FMaterial      **m_material;              // material to modify when the group changes
	CarLight        *m_light;                 // info per light
	int              m_lights;                // number of lights

#ifdef GAME
	void             m_Relocate(void *base);
#endif
};

struct CarExhaustPipe
{
	V3               m_position;              // position of end of pipe relative to car origin
	V3               m_direction;             // direction pipe faces (some pipes are tilted)

	void             m_Relocate(void *base);
};



#define CAR_COOKIE(n) char n[8] = { 'C','A','R', 13,10,26, 0,0 };

struct CarGeometry
{
	// Car position and orientation:
	//
	// The car's wheels are sitting on the X/Y plane.
	// The car is centered over the X/Y origin.  This is used for camera targeting.
	// Positive X points out through the nose.
	// Positive Y points out through the left door.
	// Positive Z points out through the roof.
	//
	//                  Z
	//                  |
	//                  |        
	//                  |        Y
	//       \___   ____|___   /
	//       _ |___/    :   \ /
	//      |           :    `-----.
	//      |   / \     :       __  \
	//      |__|   |___________/  \__|
	//          \_/     |/_____\__/______ X
	//
	// All positions herein are offsets from the above origin.
	
	char                m_cookie[8];             // CAR,13,10,26,0,0

	int                 m_headerSize;            // size of this structure to check for old files
	int                 m_version;               // make sure we don't load incompatible cars
	#define             CAR_VERSION 3

	V3                  m_head;                  // approximate location of player's head, probably temporary

	V3                  m_seat;                  // where the player model's butt sits
	V3                  m_seatNormal;            // angle of the seat the player model's butt is on
	V3                  m_back;                  // where the player model's back rests
	V3                  m_backNormal;            // angle of the seat back the player model's back is against
	V3                  m_pedal;                 // approximate location of accelerator pedal for player model

	V3                  m_centerOfGravity;       // where the center of mass/gravity is versus the origin of the car

	FMaterialDatabase   m_materials;             // all materials in one place

	CarLightGroup       m_drivingLights;         // on if at least driving lights are on, includes passive tail lights
	CarLightGroup       m_headLights;            // on if headlights are on
	CarLightGroup       m_brakeLights;           // on if brakes are on
	CarLightGroup       m_combinedLights;        // on if brakes are on, else halfway if headlights are on

	CarExhaustPipe     *m_exhaustPipe;           // if we do exhaust
	int                 m_exhaustPipes;          // if we do exhaust

	CarWheelGeometry    m_wheel[CAR_WHEELS];     // each wheel's info and shape
#ifdef ENABLE_FDEFORMABLESHAPE
	FDeformableShape   *m_body;                  // the body's shape, origin is geographical center of car but ON THE GROUND
	FDeformableShape   *m_frontHood;             // the body's shape, origin is geographical center of car but ON THE GROUND
	FDeformableShape   *m_frontEngine;           // the body's shape, origin is geographical center of car but ON THE GROUND
	FDeformableShape   *m_rearHood;              // the body's shape, origin is geographical center of car but ON THE GROUND
	FDeformableShape   *m_rearEngine;            // the body's shape, origin is geographical center of car but ON THE GROUND
	FDeformableShape   *m_wing;                  // the body's shape, origin is geographical center of car but ON THE GROUND
#else
	FShape             *m_body[CAR_LOD_LEVELS];  // the body's shape at various LOD's, origin is geographical center of car but ON THE GROUND
	FShape             *m_wing[CAR_LOD_LEVELS];  // the wing's shape, origin is geographical center of car but ON THE GROUND
#endif
	FShape             *m_cockpit[CAR_LOD_LEVELS];// the cockpit, same model space as body

	FJoint             *m_steeringWheel;         // ui steeringWheel;
	FShape             *m_uiCockpit;             // ui cockpit, same model space as body
	FJoint             *m_uiSteeringWheel;       // ui steeringWheel;

//	FAnimation         *m_flipUpAnimation;       // flip-up headlights, first keyframe down, last keyframe up, unlit until up
//	FHierarchy         *m_flipUpLights;          // draw for cars with flip-up headlights (these are NULL if not)

	void                m_Relocate(void *base, void *gfxBase);

#ifdef EXPORTER
	void                m_Init(void);
	void                m_Write(FILE *fp);
#endif
};

#endif
