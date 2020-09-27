//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 1999.
//
// CTextureStage.h
//
// CTextureStage class - Tests many possible texture blend combinations
//
// History: 5/15/99 Jeff Vezina     - Created
//
//////////////////////////////////////////////////////////////////////

#ifndef __CTEXTURESTAGE_H__
#define __CTEXTURESTAGE_H__

//#include "TestFrameBase.h"
//#include "cd3dtest.h"
//#include "CStage.h"

//USETESTFRAME

#define MAX_BLENDMODES D3DTOP_MAX

struct TName
{
    char szShortName[50];           // Code name
    char szLongName[50];            // Real name
    BOOL bUsed;                     // Is it being used?
};

class CTextureStage: public TESTFRAME(CD3DTest)
{
friend class CStage;

protected:
    D3DTLVERTEX m_rgTLVertex[8];
    int m_cVertexMax;

    UINT m_uStageListMax;           // Length of CStage list
    CStage m_StageList;             // CStage list - first node has both color and alpha DISABLED
    CStage *m_rgpStage[8];          // Array of CStages used for the current test

    UINT m_uTexFormatMax;           // Max texture formats used
    int m_cStageMax;			    // Max supported # of blend stages
    UINT m_uTestMax;                // Max # of tests
    int m_cCombinationMax;          // Number of different combinations to try on each stage
    BOOL m_bMultiPass;              // Render using multipass
    BOOL m_bPassValidate;           // Did ValidateDevice pass or failed correctly?
    UINT m_uLastFailedStage;        // Last stage that failed validate device

    BOOL m_bFog;                    // Use vertex fog?
    BOOL m_bAlpha;                  // Use alpha blending?
    BOOL m_bSpecular;               // Use specular lighting?

    int nTextureFormat;

public:
    TName m_rgTexFormats[MAX_D3D_TEXTURES];             // Array of all ref texture formats
    TName m_rgBlendModes[MAX_BLENDMODES];               // Array of all ref blend modes

    CTextureStage();
    ~CTextureStage();

    virtual void CommandLineHelp(void);
    virtual UINT TestInitialize(void);
    virtual bool ExecuteTest(UINT);
    virtual void SceneRefresh(void);
    virtual bool TestTerminate(void);

    virtual bool SetDefaultMaterials(void);
    virtual bool SetDefaultLightStates(void);
    virtual bool SetDefaultRenderStates(void);
    virtual bool SetDefaultLights(void);
    virtual bool ProcessFrame(void);

    virtual CDisplay* GetDisplay(void);

    BOOL LoadStages();              // Loads CStage list
    void CreateTexFormatNames();    // Creates texture format names, and stores it in m_rgTexFormats[]
    HRESULT ValidateDevice(DWORD *pdwPasses);           // Validates both ref and src devices
    HRESULT ValidateDevice8(DWORD *pdwPasses);          // Validates both ref and src devices on DX8 or higher
};

#endif
