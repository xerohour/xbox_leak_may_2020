#ifndef __PHYPATCH_H
#define __PHYPATCH_H

#ifndef __VECTOR_H
#include "vector.h"
#endif

#define NEIGHBOR_COUNT 4
#define MAX_CONE_COUNT  10
#define MAX_PATCHES_PER_CURB 50
#define MAX_EDGE_COUNT  3000

enum
{
	VPOS = 0,
	UPOS,
	VNEG,
	UNEG
};

enum
{
	EDGE_INSIDE = 1,
	EDGE_OUTSIDE	
};

struct PhyCone
{
	// those orange cones people like to run over
	float m_pos[3];
	float m_posVel[3];
	float m_rotVel[3];
	float m_quat[4];
	float m_transform[4][4];
	float m_radius;
	float m_groundHeight;
	float m_quatFinal[4];
	short m_interpolating;
};

struct PhyCurb
{
	// rumble strips
	float m_controlPts[MAX_PATCHES_PER_CURB][3][3][3];	
	int m_count;
};

struct PhyPatch 
{
public:
	float m_controlPts[3][3][3];
	int m_neighbor[NEIGHBOR_COUNT];  // index into PhyTrack.m_patch[];   
	short m_checked;
	int m_group;
	int m_curbIndex;                   // -1 if no curb, otherwise index into g_phyTrack.m_curb[]
	short m_isEdge;
	short m_divisions;            // number of subdivision along the track (not across)
	short m_forwardDirection;     // is the coord along the track UPOS, UNEG, VPOS, or VNEG
	short m_groundType;
};

// PhyEdge holds all the edges for each group
struct PhyEdge
{
public:
	int m_pt1;         // index into PhyTrack.m_edgeVerts[]
	int m_pt2;         // index into PhyTrack.m_edgeVerts[]
	float m_plane[4];  // wall normal and dist
	short m_side;      // EDGE_INSIDE or EDGE_OUTSIDE 
};

struct PhyEdgeGroup
{
public:
	int m_edgeIndex;           // index into PhyEdge list
	int m_edgeCount;           // edge count for this group
	float m_length;            // distance along midline for this group
	float m_distanceToStart;   // distance from this group to start group
	float m_forward[3];        // vector that points positive direction along track in world space
	float m_center[3];         // center point of group
};


struct PhyTrack
{
public:
	PhyTrack();

	PhyCone m_cone[MAX_CONE_COUNT];
	PhyCurb *m_curb;
	PhyPatch *m_patch;           // array of patches
	PhyEdge *m_edge;
	PhyEdgeGroup *m_edgeGroup;   // each group contains index into m_edge
	V3 *m_edgeVert;
	int m_coneCount;
	int m_curbCount;
	int m_patchCount;
	int m_edgeCount;
	int m_groupCount;
	int m_vertCount;
	int m_isClockwise;               // clockwise is decreasing group numbers, while counterclockwise is increasing
	int m_startGroup;

	void m_UpdateCones(float dt);
	void m_LoadTrack(char *trackName);
	void m_AttachNeighbors();
	void m_FixWhackedPatches();
	void m_GroupPatches();
	short m_PointInPatch(float *pt, int patch);
	void m_AddEdgesToGroups();
	void m_CreateVertsFromBezier(float cp[3][3], int group, int divisions, short side);
	void m_ComputeEdges();
	void m_ComputeGroupForwardVector();
	void m_ComputeGroupDistanceFromStart();
	void m_ComputeCoordinatesAlongPatch();
	void m_FindPatchCurb();

private:
	void m_GetEdgeCornerPoints(PhyPatch *p, int edge, V3 pt1, V3 pt2);
};

#ifdef GAME
extern PhyTrack g_phyTrack;
#endif

#endif // __PHYPATCH_H
