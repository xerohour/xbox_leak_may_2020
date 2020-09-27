#ifndef __CAR_H
#define __CAR_H

// max number of laps and cars is in here
#ifndef __GAME_H
#include "game.h"
#endif

#ifndef __CARWHEEL_H
#include "CarWheel.h"
#endif
#ifndef __CARWHEELNUMBERS_H
#include "CarWheelNumbers.h"
#endif

struct PhysicsCar;
struct AI;

struct CarItem;

struct CarGeometry;
struct CameraPath;
struct IDirect3DCubeTexture8;

struct EngineSounds;
struct GearSounds;

struct GTCarLodLevel
{
	float size;
	u16 level;
	u16 flags;
	#define GT_CAR_LOD_BODY_1         0x0001
	#define GT_CAR_LOD_BODY_2         0x0002
	#define GT_CAR_LOD_BODY_3         0x0004
	#define GT_CAR_LOD_NO_MIRRORS     0x0008
	#define GT_CAR_LOD_NO_WING        0x0010
	#define GT_CAR_LOD_NO_WHEELS      0x0020
	#define GT_CAR_LOD_NO_ENV_MAP     0x0040
	#define GT_CAR_LOD_NO_SHADOW      0x0080
	#define GT_CAR_LOD_REDUCED_SHADOW 0x0100
	#define GT_CAR_LOD_NO_CAR         0x0200
	#define GT_CAR_LOD_NO_CAR_IN_RVM  0x0400
};

extern GTCarLodLevel g_carLodLevel[];



struct SkidMarkBlock;
struct Car;
struct TrackGate;
struct ParticleSystem;
struct Input;
struct GTTrackEnvironmentMap;

struct CarLapTracker
{
	int   m_lap;
	int   m_lapLast;
	int   m_rankLap;
	int   m_rank;
	float m_raceDistance;
	float m_raceDistanceDelta;

	float m_raceDistanceVsP1;
	int   m_rankVsP1;
	
	CarLapTracker();

	TrackGate *m_prevGate;
	TrackGate *m_nextGate;
	TrackGate *m_trueNextGate;
};

struct CarTimes
{
	float m_lap[MAX_RACE_LAPS];
	float m_bestLap;
	float m_race;
	int   m_rankLap;
};

struct Car
{
	CarGeometry *m_geometry;        // point me at the actual graphics headers
	void        *m_uma;             // point me at the actual graphics objects

	CarItem     *m_item;     // whence I came

	// chad's engine sounds and stuff
	EngineSounds       *m_sound;		// Engine sounds

	// hack to allow switching cars during game for Gabe
	int m_loadNextFrame;
	int m_loadPalNextFrame;
	
	// transform into and out of world coords
	M4 m_transform;
//	M4 m_transformInverse;

	// transform of car at the start of this frame
	M4 m_lastTransform;

	// these are in the same order as the model.  use WHEEL_LF,RF,LR,RR
	CarWheel m_wheel[CAR_WHEELS];
	void m_LeaveSkidMarks(void);
	
	// extents of car in model space (multiplied by GT_CAR_SCALE_F)
	V3 m_min,m_max;
//	V3 minSort,maxSort;

	// where camera's target is, relative to center of mass
	V3      m_target;
	
	int     m_index;                         // player number - 1, basically

	int     m_gridPosition;                  // position in starting grid
	void    m_SetGridPosition(int p);

	// don't touch, this is for predicting the need to check visibility
	// on the brake light lens flares.
	int     m_brakesSoon;                    // brake button pressed in upcoming frame?
	
	int m_brakes;
//	int m_brakeGlow;
	int m_reverse;
//	void m_DoBrakeGlow(void);

//	PhysicsPolygonTracker m_polygon;
	
	// velocities per second
	float   m_speed;         // world space
	float   m_speedXY;       // ignore vertical change, used in skids/smoke
	float   m_topSpeed;      // record for this race

	// note that avgSpeed can be calculated at end of race:
	//   track.trackGfx->paths.oneLapDistance*race.laps / car->raceTime
	
	V3      m_velocity;      // world space
	V3      m_velocityLocal; // car space
	
#if 0
	// where we are
	Path   *m_path;          // path we're on
	int     m_segment;       // segment of path we're on
	float   m_segDist;       // dist along segment from start
	V3      m_segPos;        // actual world position of closest point on path
	bool    m_FindClosestPath(void);
#endif
	
	// transform of the car's shadow on the ground,
 	// obviously not necessarily the same as the car's
	// or the ground's.
	// DO NOT USE THIS FOR ANYTHING THAT CAN AFFECT
	// THE PHYSICS.  THIS IS FOR VISUALS ONLY AND IS
	// *NOT* SAVED IN REPLAYS.
	M4    m_shadowTransform;
	
	// helps decide when to draw car
	float m_distFromCameraForward;
	float m_distFromCameraSide;
	float m_distFromCameraUp;
	float m_distFromCamera;
	
	// vertices defining the basic rectangular shadow
	// shape and the custom outline.
	
//	GTPolyset m_shadowInstancePolyset;
//	GTInstance m_shadowInstance;
//	
//	zVtxSrc m_shadowVtx[8];
	
	// computed before drawing each viewport, used for lod
	// and fading out environment maps
	float m_sizeOnScreen;
	int m_lod;
	
	u8  m_lighting[3];
	int m_carBaseBrightness;
	
	u32 m_unaffectedLightColor;
	// don't touch this.
	u32 m_unaffectedLightColorLast;

	// tracks the position of lens flares on head/tail lights.
	// only calculated once per frame and only if the car is drawn.
	V4 lightFlareWorldPos;
	
	// was the car drawn in any viewport this frame?
	int m_drawn;

	// the following variables are used by the sound driver
//	s16	sound_effect[2];		// upto 2 channels assigned per car, these are the channel ids
//	float	rpmLast;					// last calculated RPM or -1 if not calculated last frame
//	short	gearLast;				// last gear the car was in -1 if initial pass
//	short	wheelLast;				// last wheel that generated a skid noise
//	s16	lastSfxEnvId;			// last env type that generates a sfx encountered
//	u16	lastSfxEnvVol;			// last env volume (this is faded)
//	u32	lastSfxEnvFreq;		// last frequency for sfx
	// end of the sound effects variables
	
	void m_BrakeAtStartLine(void);
	void m_DoPhysics(float dt);
	void m_MakeSparks(V3 norm, V3 point, float size);
	struct CarSpark
	{
		V3    m_norm;
		V3    m_point;
		float m_size;
	};
	#define CAR_SPARKS 4
	CarSpark m_spark[CAR_SPARKS];
	int      m_sparkIndex;

	void m_GateAndLapInit(void);
	void m_GateAndLapCheck(void);
	CarLapTracker m_pos;
	bool m_Finished(void);
	void m_ElapseTime(void);
	CarTimes      m_time;

	// am I connected via an input to a human?
	bool m_human;
	void m_SetHuman(bool h)     { m_human = h; }
	bool m_Human(void)          { return(m_human); }

	AI   *m_ai;
	
	int  m_Load(CarItem *car);
	int  m_Load(int c);
	Car();
	void m_Init(void);
	void m_InitOnce(void);
	void m_MoveToStartLine(void);
	void m_InvalidateLastPosition(void);
	
	void m_SetBrakes(register int b);
	void m_SetReverse(register int r);
	
	void m_StickToGround(void);
	void m_CalculateVelocities(void);
	void m_CalculateWheelPositions(void);
	void m_CalculateWheelAngles(void);
	void m_CalculateRaceDistance(void);
	void m_CheckForLapOrFinishMessage(void);
	void m_AdjustRank(void);
	void m_InitRank(void);

	void m_FrameInit(void);             // call before game frame

	void m_FrameDrawInit(void);         // call before draw frame
	void m_DrawInit(void);              // call before each camera/vp in draw frame

	void m_MakeShadowPolysets(void);
	
	// draws it where it is, right now, with loaded matrices.
	void m_Draw(bool interiorView = false);
	void m_DoLensFlare(void);

	// draws it where it is, right now, but scales it correctly
	// first so you don't need to know hot to do it.  calls
	// Draw() with given level, brightness, and shininess.
	// note that for x in r,g,b,a, lightColor.x+shineColor.x <= 255.
	void m_ExternalDraw(int level,u32 lightColor,u32 shineColor);

	int  m_opacity;
	int  m_particleJitterIndex1;
	int  m_particleJitterIndex2;
	int  m_particleJitterIndex3;
	int  m_particleJitterIndex4;
	
	ParticleSystem *m_smoke;
	ParticleSystem *m_sparks;
	GTTrackEnvironmentMap *m_envMap;

	void                   m_ReleaseDeviceObjects(void);

	// physics
	unsigned long m_physicsRandomSeed;  // ensures ghost mode works right
	PhysicsCar   *m_physicsCar;
	void          m_InitPhysics(void);
};

struct CarWheelSaveState
{
	// we copy the fwd,side,up and wheelAngle of the car to make a matrix.
	// note that we don't actually do this yet, we remake the entire matrix
	// every frame.  but we will.
	V3   m_position;
	int  m_skidding;
	void m_CopyOut(CarWheel *wheel);
	void m_CopyIn (CarWheel *wheel);
};

struct CarSaveState
{
#if 0
	Matrix4 transform;
	Matrix4 lastTransform;
	int brakes;
	int reverse;
//	GTPhysicsPolygonTracker polygon;
	float wheelAngle;
	float steeringAngle;
	GTCarWheelSaveState wheel[4];

	//---------------------------------------------------------
	// starting here, I don't think we really need this.
	
	// velocities per frame
	float   frameSpeed;         // world space
	Vector3 frameVelocity;      // world space
	Vector3 frameVelocityLocal; // car space
	float   frameWheelAngularVelocity;  // how fast are wheels turning?

	// velocities per second
	float   speed;         // world space
	Vector3 velocity;      // world space
	Vector3 velocityLocal; // car space
	float   wheelAngularVelocity;  // how fast are wheels turning?

	// where we are
	GTPath *path;
	int     segment;
	float   segDist;
	Vector3 segPos;

	// end I really don't think we need this
	//---------------------------------------------------------

	GTCarLapTracker pos;
	GTCarTimes      time;
	GTCarAI         ai;
	AIPath          ai_path;
	
	robCar robCar;
#else
	Car   m_carSaved;
#endif
	void  m_CopyOut(Car *car);
	void  m_CopyIn(Car *car);
};

extern Car g_car[MAX_CAR_SLOTS];
extern int g_cars;


#endif
