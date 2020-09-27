#define NUM_MATERIALS           14
#define NUM_LIGHTS              24

#define NUM_COG_TEXTURES        17
#define NUM_SHAFT_TEXTURES      7

#define GEAR_ROTATION_ANGLE     (M_PI / 4.0f)

#define NUM_GEARS               200

#define SHAFT_RADIUS            0.375f
#define S_COG_RADIUS            (SHAFT_RADIUS * 4.0f / 1.5f)
#define M_COG_RADIUS            (SHAFT_RADIUS * 6.0f / 1.5f)
#define L_COG_RADIUS            (SHAFT_RADIUS * 10.0f / 1.5f)
#define S_COG_WIDTH             (SHAFT_RADIUS * 4.0f / 1.5f)
#define M_COG_WIDTH             (SHAFT_RADIUS * 2.0f / 1.5f)
#define L_COG_WIDTH             (SHAFT_RADIUS * 1.0f / 1.5f)
#define TOOTH_HEIGHT            (SHAFT_RADIUS * 2.6f / 1.5f)
#define TOOTH_WIDTH_BASE        (SHAFT_RADIUS * 2.6f / 1.5f)
#define TOOTH_WIDTH_TOP         (SHAFT_RADIUS * 1.54f / 1.5f)
#define MIN_COG_SPACING         ((L_COG_RADIUS + TOOTH_HEIGHT) * 2.0f)
#define S_COG_SIDES             6
#define M_COG_SIDES             8
#define L_COG_SIDES             12

//#define FRND(x)                 ((D3DVAL(rand() % RAND_MAX) / D3DVAL(RAND_MAX)) * (x))

#define GEAR_LENGTH             ((3.0f * MIN_COG_SPACING) + FRND(5.0f))

#define CYLCAPS_NONE            0
#define CYLCAPS_TOP             1
#define CYLCAPS_BOTTOM          2
#define CYLCAPS_BOTH            3

typedef enum _COGSIZE {
    COGSIZE_SMALL = 0,
    COGSIZE_MEDIUM = 1,
    COGSIZE_LARGE = 2
} COGSIZE, *PCOGSIZE;

typedef struct _LIGHTDATA {
    D3DLIGHT8                   light;
    D3DCOLOR                    cAmbient;
    BOOL                        bSpecular;
    DWORD                       dwVShader;
} LIGHTDATA, *PLIGHTDATA;

typedef struct _FRAMESTATS {
    DWORD                       dwPolygons;
    DWORD                       dwMeshes;
    DWORD                       dwVBuffers;
    DWORD                       dwIBuffers;
    DWORD                       dwTextures;
    DWORD                       dwMaterials;
    DWORD                       dwTransforms;
    DWORD                       dwLights;
    DWORD                       dwVShaders;
    DWORD                       dwPShaders;
    DWORD                       dwShaderConstants;
} FRAMESTATS, *PFRAMESTATS;

typedef struct _D3DRECT3D {
    D3DVECTOR                   v1;     // -x,-y,-z
    D3DVECTOR                   v2;     // +x,+y,+z
} D3DRECT3D, *PD3DRECT3D;

typedef struct _COG {

    DWORD                       dwSides;
    LPDIRECT3DVERTEXBUFFER8     pd3drCenter;
    DWORD                       dwCenterVertices;
    LPDIRECT3DINDEXBUFFER8      pd3diCenter;
    DWORD                       dwCenterIndices;
    DWORD                       dwCenterPrimitives;
    LPDIRECT3DVERTEXBUFFER8     pd3drTeeth;
    DWORD                       dwTextureIndex;

} COG, *PCOG;

typedef struct _GEAR {

    COG                         cog[3];
    LPDIRECT3DVERTEXBUFFER8     pd3drShaft;
    D3DXMATRIX                  mWorld;
    D3DXMATRIX                  mRotate;
    DWORD                       dwClip[4];

    DWORD                       dwShaftTextureIndex;
    int                         nMaterialIndex;
    int                         nLightIndex;

    D3DXVECTOR3                 vPos;
    D3DXVECTOR3                 vDir;
    float                       fAngularVelocity;
    float                       fSpin;
    COGSIZE                     csCogSize[2];
    float                       fCogOffset[2];

    struct _GEAR*               pgearNext[2];

} GEAR, *PGEAR;

class CClockwork {

private:

    LPDIRECT3DDEVICE8           m_pDevice;
    float                       m_fFieldOfView;
    D3DXMATRIX                  m_mProj;
    D3DXMATRIX*                 m_pmView;

    PVERTEX                     m_prUnitShaft;

    DWORD                       m_dwShaftVertices;
    LPDIRECT3DINDEXBUFFER8      m_pd3diShaft;
    DWORD                       m_dwShaftIndices;
    DWORD                       m_dwShaftPrimitives;

    DWORD                       m_dwToothVertices;
    LPDIRECT3DINDEXBUFFER8      m_pd3diTooth;
    DWORD                       m_dwToothIndices;
    DWORD                       m_dwToothPrimitives;

    PGEAR                       m_pgearRoot;

    LPDIRECT3DTEXTURE8          m_pd3dtCog[NUM_COG_TEXTURES];
    LPDIRECT3DTEXTURE8          m_pd3dtShaft[NUM_SHAFT_TEXTURES];

    D3DMATERIAL8                m_material[NUM_MATERIALS];
    LIGHTDATA                   m_ldLight[NUM_LIGHTS];

    D3DMATERIAL8                m_matCurrent;
    D3DLIGHT8                   m_litCurrent;

    D3DRECT3D                   m_rectBox[NUM_GEARS * 4 + 6];
    UINT                        m_uNumBoxes;
    UINT                        m_uNumGears;
    UINT                        m_uNumMaterials;
    UINT                        m_uNumLights;
    DWORD                       m_dwMaxDepth;

    UINT                        m_uCogTextures;
    UINT                        m_uShaftTextures;

private:

    BOOL                        CreateCog(PCOG pcog, DWORD dwSides, float fRadius, 
                                            float fWidth, float fOffset = 0.0f);
    void                        ReleaseCog(PCOG pcog);

    PGEAR                       CreateGear(PCOGSIZE pcsCogSize, float* pfCogOffset, 
                                            D3DXVECTOR3* pvPos, D3DXVECTOR3* pvDir, 
                                            float fAngularVelocity, float fRotation, 
                                            PD3DRECT3D prectBox, DWORD dwDepth = 1);
    PGEAR                       AddAttachedGear(PGEAR pgearParent, UINT uCog, 
                                            DWORD dwDepth);
    void                        AddGear(PGEAR pgear, DWORD dwDepth = 1);
    void                        RotateGear(PGEAR pgear, float fTheta);
    void                        RenderGear(PGEAR pgear);
    void                        RenderGearP(PGEAR pgear);
    void                        ReleaseGear(PGEAR pgear);

    BOOL                        CreateCylinder(PVERTEX* pprVertices, LPDWORD pdwNumVertices, 
                                            LPWORD* ppwIndices, LPDWORD pdwNumIndices, 
                                            DWORD dwSides, float fRadius, float fWidth,
                                            DWORD dwCylinderCaps = CYLCAPS_BOTH);

    void                        ReleaseCylinder(PVERTEX prVertices, LPWORD pwIndices);

    void                        SetRect3D(PD3DRECT3D prect, D3DXVECTOR3* pv1, 
                                            D3DXVECTOR3* pv2);
    BOOL                        GearCollision(PD3DRECT3D prectBox1, PD3DRECT3D prectBox2);

    void                        GetGearStats(PGEAR pgear, PFRAMESTATS pfs);

public:

                                CClockwork();
                                ~CClockwork();

    BOOL                        Create(LPDIRECT3DDEVICE8 pDevice, PCAMERA pcam, float fWidth, float fHeight);
    void                        Update(float fTimeDilation);
    void                        Render();
    void                        Render(D3DXMATRIX* pmView);

    void                        GetFrameStats(PFRAMESTATS pfs);
};

