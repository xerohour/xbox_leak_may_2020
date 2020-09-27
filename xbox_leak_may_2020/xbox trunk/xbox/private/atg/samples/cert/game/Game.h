//-----------------------------------------------------------------------------
// File: Game.h
//
// Desc: Runs the 3D game
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_GAME_H
#define TECH_CERT_GAME_GAME_H

#include "Common.h"
#include "Math3d.h"
#include "File.h"
#include <XbFont.h>
#include <XbStopWatch.h>
#include "MusicManager.h"
#include "SoundEffect.h"
#include "CXFont.h"

//-----------------------------------------------------------------------------
// Forward references
//-----------------------------------------------------------------------------
struct XBGAMEPAD;
class PhysicsSystem;
class PhysicsObject;
class Polyhedron;
class Mesh;
class Matrix3;




//-----------------------------------------------------------------------------
// Name: class Actor
// Desc: An actor is an object in the world
//-----------------------------------------------------------------------------
class Actor
{
    D3DXMATRIX     m_Matrix;               // object position
    PhysicsSystem* m_pPhysicsSystem;       // system in which actor participates
    Mesh*          m_pDrawMesh;            // object mesh
    BOOL           m_bOwnMesh;             // do we own the mesh
    PhysicsObject* m_pPhysicsController;   // object physics
    Actor*         m_pParent;			   // Parent or NULL

public:

    Actor( PhysicsSystem* );
    ~Actor();

    VOID         Render();
	virtual VOID Update();
    VOID         Destroy();

    inline VOID SetMatrix( const D3DXMATRIX& mat )
    {
        m_Matrix = mat;
    }

    inline D3DXMATRIX* GetMatrix()
    {
        return &m_Matrix;
    }

    inline VOID SetMesh( Mesh* pMesh, BOOL bOwnMesh = TRUE )
    {
        m_pDrawMesh = pMesh;
		m_bOwnMesh = FALSE;
        USED( bOwnMesh );
    }

    inline Mesh* GetMesh()
    {
        return m_pDrawMesh;
    }

    inline VOID SetController( PhysicsObject* pController )
    {
        m_pPhysicsController = pController;
    }

    inline PhysicsObject* GetController()
    {
        return m_pPhysicsController;
    }

	inline VOID SetParent(Actor* pParent)
	{
		m_pParent = pParent;
	}

private:

    // Disable
    Actor();

};




//-----------------------------------------------------------------------------
// Name: enum VehicleIDs
// Desc: Identifiers for the different vehicles.
//-----------------------------------------------------------------------------
enum VehicleIDs
{
	Vehicle_Buggy,	
    Vehicle_MaxID
};




//-----------------------------------------------------------------------------
// Name: class VehicleActor
// Desc: An actor specialized for use as a vehicle.
//-----------------------------------------------------------------------------
class VehicleActor : public Actor
{

public:

	VehicleIDs m_ID;
	Actor*     m_pWheels[4];

    VehicleActor( PhysicsSystem* );
	virtual VOID Update();

};



//-----------------------------------------------------------------------------
// Name: class Game
// Desc: Game mode
//-----------------------------------------------------------------------------
class Game
{
    static const INT MAX_ACTORS = 10;

    mutable CXBFont&   m_Font;
    CXFont&            m_XFontJPN;
    CMusicManager&     m_MusicManager;
    CSoundEffect&      m_SoundEffect;
    PhysicsSystem*     m_pPhysicsSystem;
    CXBStopWatch       m_VibrationTimer;
    CXBStopWatch       m_VehicleFlippedTimer;
    BOOL               m_bIsPaused;
    BOOL               m_bIsVibrationOn;
    INT                m_iNumActors;
    Actor*             m_pActors[MAX_ACTORS];   // Simple world database
    VehicleActor*      m_pVehicle;              // Player vehicle
    LPDIRECT3DTEXTURE8 m_ptScreenShot;          // For saved games
    BOOL               m_bRecordingDemo;
    BOOL               m_bPlayingDemo;
    File               m_DemoFile;
    FLOAT              m_fMusicVolume;
    FLOAT              m_fEffectVolume;

    static Vector3     m_vEyePt;
    static Vector3     m_vLookatPt;
    static Vector3     m_vUpVec;

public:

    Game( CXBFont&, CXFont&, CMusicManager&, CSoundEffect& );

    // Begin new game
    VOID    Start( BOOL bRecordDemo, BOOL bPlayDemo, BOOL bIsVibrationOn, FLOAT fMusicVolume, FLOAT fEffectVolume,
                   const CHAR* strDemoName = NULL );
    VOID    End();                                      // End game
    HRESULT FrameMove( const XBGAMEPAD*, FLOAT fTime,   // Frame advance
                       FLOAT fElapsedTime );
    HRESULT Render( const WCHAR* strFrameRate = NULL ); // Frame render
    VOID    SetPaused( BOOL bPaused = TRUE );
    BOOL    IsPaused() const;
    LPDIRECT3DTEXTURE8 GetScreenShot() const;
    DWORD   GetSaveGameSize();
    VOID    GetSaveGameData( BYTE* pData );
    VOID    LoadSaveGame( const BYTE* pData, DWORD dwSize );
    DWORD   GetMaxSaveGameSize() const;

private:

    VOID    ShowLoadingScreen();

    // Creating actors
    Actor*  AddBoxActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                         D3DCOLOR color, FLOAT width, FLOAT height, 
                         FLOAT depth, FLOAT mass );

    Actor*  AddSphereActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                            D3DCOLOR color, FLOAT radius, FLOAT mass );

    Actor*  AddMeshActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
						  Mesh* pMesh, BOOL bOwnMesh = TRUE );

    VehicleActor*  AddVehicleActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat, 
                                    VehicleIDs ID );

    Actor*  AddActor( const D3DXVECTOR3& pos, const D3DXQUATERNION& quat,
                      Mesh* pMesh, PhysicsObject* pController,
                      Polyhedron* pPoly, FLOAT fMass, 
                      const Matrix3& InertiaTensor );

    VOID CheckForFreeSpace() const;
    VOID RenderMenu();

private:

    // Disabled
    Game( const Game& );
    Game& operator=( const Game& );

};




#endif // TECH_CERT_GAME_GAME_H
