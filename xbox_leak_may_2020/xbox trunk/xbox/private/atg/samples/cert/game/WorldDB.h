//-----------------------------------------------------------------------------
// FILE: WorldDB.h
//
// Desc: World database
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

class WorldSection
{
public:
	BoundingBox m_Bounds;
	WorldSection* m_pPrev;
	WorldSection* m_pNext;
	SectionMesh* m_pMesh;			// May not be loaded.
};


class CoordinateFrame
{
public:
	D3DXVECTOR3 pos;
	D3DXQUATERNION quat;
};


class Actor
{
public:
	CoordinateFrame m_Location;
	WorldSection* m_pSections[2];		// Actor can be in at most two sections.
	ActorMesh* m_pMesh;
};
