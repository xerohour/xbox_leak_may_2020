//-----------------------------------------------------------------------------
// File: Donuts.h
//
// Desc: Header for Donuts3D game
//
// Copyright (C) 1995-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------

// Error codes
#define DONUTS3DERR_NODIRECT3D       0x00000001
#define DONUTS3DERR_NOD3DDEVICE      0x00000002
#define DONUTS3DERR_NOTEXTURES       0x00000003
#define DONUTS3DERR_NOGEOMETRY       0x00000004
#define DONUTS3DERR_NO3DRESOURCES    0x00000005
#define DONUTS3DERR_NOINPUT          0x00000006


// States the app can be in
enum{ APPSTATE_LOADSPLASH, APPSTATE_DISPLAYSPLASH, APPSTATE_ACTIVE, 
      APPSTATE_BEGINLEVELSCREEN, APPSTATE_DISPLAYLEVELSCREEN };


// Game object types
enum{ OBJ_DONUT=0, OBJ_PYRAMID, OBJ_CUBE, OBJ_SPHERE, OBJ_CLOUD, OBJ_SHIP,
      OBJ_BULLET };


// Object dimensions and fixed properties
#define DONUT_WIDTH        32
#define DONUT_HEIGHT       32
#define PYRAMID_WIDTH      32
#define PYRAMID_HEIGHT     32
#define SPHERE_WIDTH       16
#define SPHERE_HEIGHT      16
#define CUBE_WIDTH         16
#define CUBE_HEIGHT        16
#define CLOUD_WIDTH        32
#define CLOUD_HEIGHT       32
#define BULLET_WIDTH        3
#define BULLET_HEIGHT       3

#define NUM_DONUT_FRAMES   30
#define NUM_PYRAMID_FRAMES 40
#define NUM_SPHERE_FRAMES  40
#define NUM_CUBE_FRAMES    40
#define NUM_BULLET_FRAMES 400

#define BULLET_XOFFSET    304
#define BULLET_YOFFSET      0




//-----------------------------------------------------------------------------
// Name: struct DisplayObject
// Desc: A game object that goes in the display list
//-----------------------------------------------------------------------------
struct DisplayObject
{
    DisplayObject* pNext;          // Link to next object
    DisplayObject* pPrev;          // Link to previous object
    
    DWORD          dwType;            // Type of object
    BOOL           bVisible;          // Whether the object is visible
    D3DXVECTOR3    vPos;              // Position
    D3DXVECTOR3    vVel;              // Velocity
    FLOAT          fSize;
    
    // Constructor
    DisplayObject( DWORD type, D3DVECTOR p, D3DVECTOR v );
};




//-----------------------------------------------------------------------------
// Derived classes for displayable game objects
//-----------------------------------------------------------------------------
struct C3DSprite : public DisplayObject
{
    DWORD dwFramesPerLine;   // How anim frames are packed in bitmap
    FLOAT frame;             // Current animation frame
    FLOAT fMaxFrame;         // Max animation frame value
    FLOAT delay;             // Frame/second
    
    DWORD dwColor;

    DWORD dwTextureOffsetX; // Pixel offsets into the game texture
    DWORD dwTextureOffsetY;
    DWORD dwTextureWidth;   // Width and height in pixels
    DWORD dwTextureHeight; 
    
    C3DSprite( DWORD type, D3DVECTOR p, D3DVECTOR v );
};


class CDonut : public C3DSprite
{
public:
    CDonut( D3DVECTOR p, D3DVECTOR v );
};


class CPyramid : public C3DSprite
{
public:
    CPyramid( D3DVECTOR p, D3DVECTOR v );
};


class CSphere : public C3DSprite
{
public:
    CSphere( D3DVECTOR p, D3DVECTOR v );
};


class CCube : public C3DSprite
{
public:
    CCube( D3DVECTOR p, D3DVECTOR v );
};


class CCloud : public C3DSprite
{
public:
    CCloud( D3DVECTOR p, D3DVECTOR v );
};


class CBullet : public C3DSprite
{
public:
    CBullet( D3DVECTOR p, D3DVECTOR v, DWORD dwType );
};


class CShip : public DisplayObject
{
public:
    FLOAT fRoll;

    FLOAT fAngle;

    BOOL  bExploded;
    FLOAT fShowDelay;

public:
    CShip( D3DVECTOR p );
};




//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------

// Main game functions
HRESULT          CreateGameObjects();
VOID             DestroyGameObjects();

// Sound functions
HRESULT          CreateSoundObjects();
VOID             DestroySoundObjects();

// Input functions
HRESULT          CreateInputObjects();
VOID             DestroyInputObjects();
VOID             GetInput();

// Display functions
HRESULT          CreateDisplayObjects();
HRESULT          RestoreDisplayObjects();
HRESULT          InvalidateDisplayObjects();
HRESULT          DestroyDisplayObjects();

// Menu functions
VOID             ConstructMenus();
VOID             DestroyMenus();
VOID             UpdateMenus();


// Rendering functions
HRESULT          FrameMove();
HRESULT          RenderFrame();
VOID             UpdateDisplayList();
VOID             DrawDisplayList();
VOID             ShowFrame();

// Misc game functions
VOID             DisplayLevelIntroScreen();
VOID             AdvanceLevel();
BOOL             IsDisplayListEmpty();
VOID             AddToList( DisplayObject* pObject );
VOID             DeleteFromList( DisplayObject* pObject );
VOID             CheckForHits();
FLOAT            rnd( FLOAT low=-1.0f, FLOAT high=1.0f );

