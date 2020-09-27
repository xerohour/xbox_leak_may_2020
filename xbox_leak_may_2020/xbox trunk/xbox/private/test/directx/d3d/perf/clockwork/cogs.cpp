/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    cogs.cpp

Description:

    Direct3D Immediate-Mode Sample.

*******************************************************************************/

#include "d3dbase.h"
#include "cogs.h"
#include "clockwork.h"

static struct _LIGHTINIT {
    D3DLIGHTTYPE            d3dlt;
    BOOL                    bSpecular;
    TCHAR                   szVShader[32];
    float                   fRatio;
} g_liLightInit[NUM_LIGHTS] =
{
    {D3DLIGHT_DIRECTIONAL,  FALSE,  TEXT("ldir.vsh"),       0.1f},
    {D3DLIGHT_SPOT,         FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_POINT,        FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_DIRECTIONAL,  TRUE,   TEXT("ldirspec.vsh"),   0.05f},
    {D3DLIGHT_SPOT,         TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_POINT,        TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_DIRECTIONAL,  FALSE,  TEXT("ldir.vsh"),       0.1f},
    {D3DLIGHT_SPOT,         FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_POINT,        FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_DIRECTIONAL,  TRUE,   TEXT("ldirspec.vsh"),   0.05f},
    {D3DLIGHT_SPOT,         TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_POINT,        TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_DIRECTIONAL,  FALSE,  TEXT("ldir.vsh"),       0.1f},
    {D3DLIGHT_SPOT,         FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_POINT,        FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_DIRECTIONAL,  TRUE,   TEXT("ldirspec.vsh"),   0.05f},
    {D3DLIGHT_SPOT,         TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_POINT,        TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_DIRECTIONAL,  FALSE,  TEXT("ldir.vsh"),       0.1f},
    {D3DLIGHT_SPOT,         FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_POINT,        FALSE,  TEXT("ldir.vsh"),       0.025f},
    {D3DLIGHT_DIRECTIONAL,  TRUE,   TEXT("ldirspec.vsh"),   0.05f},
    {D3DLIGHT_SPOT,         TRUE,   TEXT("ldirspec.vsh"),   0.025f},
    {D3DLIGHT_POINT,        TRUE,   TEXT("ldirspec.vsh"),   0.025f},
};
/*
{
    {D3DLIGHT_DIRECTIONAL,  FALSE,  TEXT("ldir.vsh"),       0.4f},
    {D3DLIGHT_SPOT,         FALSE,  TEXT("ldir.vsh"),       0.1f},
    {D3DLIGHT_POINT,        FALSE,  TEXT("ldir.vsh"),       0.1f},
    {D3DLIGHT_DIRECTIONAL,  TRUE,   TEXT("ldirspec.vsh"),   0.2f},
    {D3DLIGHT_SPOT,         TRUE,   TEXT("ldirspec.vsh"),   0.1f},
    {D3DLIGHT_POINT,        TRUE,   TEXT("ldirspec.vsh"),   0.1f},
};
*/
/*
static struct _LIGHTINIT {
    D3DLIGHTTYPE            d3dlt;
    BOOL                    bSpecular;
    TCHAR                   szVShader[32];
    float                   fRatio;
} g_liLightInit[NUM_LIGHTS] =
{
    {D3DLIGHT_DIRECTIONAL,  FALSE,  TEXT("ldir.vsh"),       0.4f},
    {D3DLIGHT_SPOT,         FALSE,  TEXT("lspt.vsh"),       0.1f},
    {D3DLIGHT_POINT,        FALSE,  TEXT("lpnt.vsh"),       0.1f},
    {D3DLIGHT_DIRECTIONAL,  TRUE,   TEXT("ldirspec.vsh"),   0.2f},
    {D3DLIGHT_SPOT,         TRUE,   TEXT("lsptspec.vsh"),   0.1f},
    {D3DLIGHT_POINT,        TRUE,   TEXT("lpntspec.vsh"),   0.1f},
};
*/
/*
static struct _LIGHTINIT {
    LIGHTTYPE               ltType;
    D3DXVECTOR3             vDir;
    D3DCOLOR                cAmbient;
    float                   fRatio;
    TCHAR                   szVShader[32];
} g_ltdLightData[NUM_LIGHTS] =
{
    {LIGHT_DIRECTIONAL,          D3DXVECTOR3(0.0f, -1.0f, 1.0f), RGB_MAKE(64, 64, 64), 0.4f, TEXT("ldir.vsh")},
    {LIGHT_SPOT,                 D3DXVECTOR3(0.0f,  0.0f, 1.0f), RGB_MAKE(48, 48, 48), 0.1f, TEXT("lspt.vsh")},
    {LIGHT_POINT,                D3DXVECTOR3(0.0f,  0.0f, 1.0f), RGB_MAKE(16, 16, 16), 0.1f, TEXT("lpnt.vsh")},
    {LIGHT_SPECULAR_DIRECTIONAL, D3DXVECTOR3(0.0f, -1.0f, 1.0f), RGB_MAKE(32, 32, 32), 0.2f, TEXT("ldirspec.vsh")},
    {LIGHT_SPECULAR_SPOT,        D3DXVECTOR3(0.0f,  0.0f, 1.0f), RGB_MAKE(16, 16, 16), 0.1f, TEXT("lsptspec.vsh")},
    {LIGHT_SPECULAR_POINT,       D3DXVECTOR3(0.0f,  0.0f, 1.0f), RGB_MAKE(48, 48, 48), 0.1f, TEXT("lpntspec.vsh")},
};
*/

//******************************************************************************
CClockwork::CClockwork() {

    UINT i;
    m_pDevice = NULL;
    m_prUnitShaft = NULL;
    m_pd3diShaft = NULL;
    m_pd3diTooth = NULL;
    m_pgearRoot = NULL;
    memset(m_pd3dtCog, 0, NUM_COG_TEXTURES * sizeof(LPDIRECT3DTEXTURE8));
    memset(m_pd3dtShaft, 0, NUM_SHAFT_TEXTURES * sizeof(LPDIRECT3DTEXTURE8));
    for (i = 0; i < NUM_LIGHTS; i++) {
        m_ldLight[i].dwVShader = INVALID_SHADER_HANDLE;
    }
}

//******************************************************************************
BOOL CClockwork::CreateCog(PCOG pcog, DWORD dwSides, float fRadius, float fWidth, float fOffset) {

    PVERTEX     prCenter, prTeeth;
    LPWORD      pwCenter;
    D3DXVECTOR3 vN;
    float       fToothZ, fToothX, fTheta;
    D3DXMATRIX  mRotate;
    UINT        i, j, k;

    if (!pcog) {
        return FALSE;
    }

    memset(pcog, 0, sizeof(COG));

    pcog->dwSides = dwSides;

    pcog->pd3drTeeth = CreateVertexBuffer(m_pDevice, NULL, dwSides * 8 * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!pcog->pd3drTeeth) {
        ReleaseCog(pcog);
        return FALSE;
    }        

    if (!CreateCylinder(&prCenter, &pcog->dwCenterVertices, 
                        &pwCenter, &pcog->dwCenterIndices, 
                        pcog->dwSides, fRadius, fWidth, 
                        CYLCAPS_BOTH))
    {
        ReleaseCog(pcog);
        return FALSE;
    }
    pcog->dwCenterPrimitives = pcog->dwCenterIndices / 3;

    // Position the first tooth
    fToothX = fWidth / 2.0f * 0.9f;
    fToothZ = prCenter[0].vPosition.z - ((TOOTH_WIDTH_BASE / 2.0f) / 
            (prCenter[2].vPosition.y / (prCenter[0].vPosition.z - 
            prCenter[2].vPosition.z)));

    pcog->pd3drTeeth->Lock(0, 0, (LPBYTE*)&prTeeth, 0);

    prTeeth[0] = VERTEX(D3DXVECTOR3( fToothX, -TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f, -1.0f, 0.0f)), 0.0f, 1.0f);
    prTeeth[1] = VERTEX(D3DXVECTOR3(-fToothX, -TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f, -1.0f, 0.0f)), 0.0f, 1.0f);
    prTeeth[2] = VERTEX(D3DXVECTOR3( fToothX, -TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f, -1.0f, 1.0f)), 1.0f, 1.0f);
    prTeeth[3] = VERTEX(D3DXVECTOR3(-fToothX, -TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f, -1.0f, 1.0f)), 1.0f, 1.0f);
    prTeeth[4] = VERTEX(D3DXVECTOR3( fToothX,  TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f,  1.0f, 1.0f)), 1.0f, 0.0f);
    prTeeth[5] = VERTEX(D3DXVECTOR3(-fToothX,  TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f,  1.0f, 1.0f)), 1.0f, 0.0f);
    prTeeth[6] = VERTEX(D3DXVECTOR3( fToothX,  TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f,  1.0f, 0.0f)), 0.0f, 0.0f);
    prTeeth[7] = VERTEX(D3DXVECTOR3(-fToothX,  TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f,  1.0f, 0.0f)), 0.0f, 0.0f);

    // Rotate the first tooth around the cog
    fTheta = M_2PI / (float)dwSides;
    InitMatrix(&mRotate,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, (float)cos(fTheta), (float)sin(fTheta), 0.0f,
        0.0f, (float)-sin(fTheta), (float)cos(fTheta), 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f
    );

    for (i = 1; i < dwSides; i++) {
        for (j = 0; j < 8; j++) {
            D3DXVec3TransformCoord(&prTeeth[i * 8 + j].vPosition, &prTeeth[(i - 1) * 8 + j].vPosition, &mRotate);
            D3DXVec3TransformCoord(&prTeeth[i * 8 + j].vNormal, &prTeeth[(i - 1) * 8 + j].vNormal, &mRotate);
            memcpy(&prTeeth[i * 8 + j].u0, &prTeeth[(i - 1) * 8 + j].u0, 2 * sizeof(float));
        }
    }

    // Offset the cog along the x axis
    for (i = 0; i < pcog->dwCenterVertices; i++)  {
        prCenter[i].vPosition.x += fOffset;
    }
    for (i = 0; i < dwSides; i++) {
        for (j = 0; j < 8; j++) {
            prTeeth[i * 8 + j].vPosition.x += fOffset;
        }
    }

    pcog->pd3drTeeth->Unlock();

#ifdef UNDER_XBOX
    pcog->pd3drTeeth->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    pcog->pd3drCenter = CreateVertexBuffer(m_pDevice, prCenter, pcog->dwCenterVertices * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!pcog->pd3drCenter) {
        ReleaseCylinder(prCenter, pwCenter);
        ReleaseCog(pcog);
        return FALSE;
    }

#ifdef UNDER_XBOX
    pcog->pd3drCenter->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    pcog->pd3diCenter = CreateIndexBuffer(m_pDevice, pwCenter, pcog->dwCenterIndices * sizeof(WORD));
    if (!pcog->pd3diCenter) {
        ReleaseCylinder(prCenter, pwCenter);
        ReleaseCog(pcog);
        return FALSE;
    }

    ReleaseCylinder(prCenter, pwCenter);

    return TRUE;
}

//******************************************************************************
void CClockwork::ReleaseCog(PCOG pcog) {

    UINT i, j;

    if (pcog->pd3drCenter) {
        ReleaseVertexBuffer(pcog->pd3drCenter);
    }

    if (pcog->pd3diCenter) {
        ReleaseIndexBuffer(pcog->pd3diCenter);
    }

    if (pcog->pd3drTeeth) {
        ReleaseVertexBuffer(pcog->pd3drTeeth);
    }

    memset(pcog, 0, sizeof(COG));
}

//******************************************************************************
PGEAR CClockwork::CreateGear(PCOGSIZE pcsCogSize, float* pfCogOffset, 
                 D3DXVECTOR3* pvPos, D3DXVECTOR3* pvDir, float fAngularVelocity, 
                 float fRotation, PD3DRECT3D prectBox, DWORD dwDepth)
{
    PVERTEX         prShaft;
    PGEAR           pgear;
    D3DXMATRIX      mTranslate, mRotate, mWorld;
    float           fOrient, fAngle, fClipHeight, fClipRadius;
    UINT            uLight, i, j;
    BOOL            bRet;
    float f1, f2;

    if (m_uNumGears == NUM_GEARS || dwDepth > m_dwMaxDepth) {
        return NULL;
    }

    if (!pvPos || !pvDir) {
        return NULL;
    }

    if (pfCogOffset[1] < (3.0f * MIN_COG_SPACING)) {
        return NULL;
    }

    pgear = new GEAR;
    if (!pgear) {
        return NULL;
    }

    memset(pgear, 0, sizeof(GEAR));

    pgear->pd3drShaft = CreateVertexBuffer(m_pDevice, NULL, m_dwShaftVertices * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!pgear->pd3drShaft) {
        ReleaseGear(pgear);
        return NULL;
    }

    pgear->pd3drShaft->Lock(0, 0, (LPBYTE*)&prShaft, 0);

    for (i = 0; i < m_dwShaftVertices; i++) {
        prShaft[i] = m_prUnitShaft[i];
        prShaft[i].vPosition.x *= pfCogOffset[1];
    }

    pgear->pd3drShaft->Unlock();

#ifdef UNDER_XBOX
    pgear->pd3drShaft->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    for (j = 0; j < 3; j++) {

        switch (pcsCogSize[j]) {
            case COGSIZE_SMALL:
                bRet = CreateCog(&pgear->cog[j], S_COG_SIDES, S_COG_RADIUS, S_COG_WIDTH, !j ? 0.0f : pfCogOffset[j-1]);
                if (!j) {
                    fAngle = fRotation + M_PI / (float)S_COG_SIDES;
                }
                else {
                    pgear->csCogSize[j-1] = COGSIZE_SMALL;
                }
                break;
            case COGSIZE_MEDIUM:
                bRet = CreateCog(&pgear->cog[j], M_COG_SIDES, M_COG_RADIUS, M_COG_WIDTH, !j ? 0.0f : pfCogOffset[j-1]);
                if (!j) {
                    fAngle = fRotation + M_PI / (float)M_COG_SIDES;
                }
                else {
                    pgear->csCogSize[j-1] = COGSIZE_MEDIUM;
                }
                break;
            case COGSIZE_LARGE:
                bRet = CreateCog(&pgear->cog[j], L_COG_SIDES, L_COG_RADIUS, L_COG_WIDTH, !j ? 0.0f : pfCogOffset[j-1]);
                if (!j) {
                    fAngle = fRotation + M_PI / (float)L_COG_SIDES;
                }
                else {
                    pgear->csCogSize[j-1] = COGSIZE_LARGE;
                }
                break;
        }

        if (!bRet) {
            ReleaseGear(pgear);
            return NULL;
        }
    }

    pgear->fCogOffset[0] = pfCogOffset[0];
    pgear->fCogOffset[1] = pfCogOffset[1];

    for (i = 0; i < 3; i++) {
        pgear->cog[i].dwTextureIndex = m_uCogTextures++ % NUM_COG_TEXTURES;
    }
    pgear->dwShaftTextureIndex = m_uShaftTextures++ % NUM_SHAFT_TEXTURES;

    // Set up the rotation matrix
    InitMatrix(&pgear->mRotate,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // Orient the gear in space
    if (pvDir->x != 0.0f) {

        fOrient = (pvDir->x > 0.0f) ? 0.0f : M_PI;

        InitMatrix(&mRotate,
            (float)( cos(fOrient)), (float)( 0.0f), (float)(-sin(fOrient)), (float)( 0.0f),
            (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
            (float)( sin(fOrient)), (float)( 0.0f), (float)( cos(fOrient)), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );
    }
    else if (pvDir->y != 0.0f) {

        fOrient = (pvDir->y > 0.0f) ? M_PIDIV2 : -M_PIDIV2;

        InitMatrix(&mRotate,
            (float)( cos(fOrient)), (float)( sin(fOrient)), (float)( 0.0f), (float)( 0.0f),
            (float)(-sin(fOrient)), (float)( cos(fOrient)), (float)( 0.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );
    }
    else if (pvDir->z != 0.0f) {

        fOrient = (pvDir->z > 0.0f) ? -M_PIDIV2 : M_PIDIV2;

        InitMatrix(&mRotate,
            (float)( cos(fOrient)), (float)( 0.0f), (float)(-sin(fOrient)), (float)( 0.0f),
            (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
            (float)( sin(fOrient)), (float)( 0.0f), (float)( cos(fOrient)), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );
    }
    else {
        ReleaseGear(pgear);
        return NULL;
    }

    InitMatrix(&mTranslate,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)( pvPos->x), (float)( pvPos->y), (float)( pvPos->z), (float)( 1.0f)
    );

    D3DXMatrixMultiply(&mWorld, &mRotate, &mTranslate);

    InitMatrix(&mRotate,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( cos(fAngle)), (float)( sin(fAngle)), (float)( 0.0f),
        (float)( 0.0f), (float)(-sin(fAngle)), (float)( cos(fAngle)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    D3DXMatrixMultiply(&pgear->mWorld, &mRotate, &mWorld);

    if (m_uNumGears == m_uNumMaterials * (UINT)((float)NUM_GEARS / (float)NUM_MATERIALS + 0.5f)) {
        pgear->nMaterialIndex = (int)m_uNumMaterials;
        m_uNumMaterials++;
    }
    else {
        pgear->nMaterialIndex = -1;
    }

    for (i = 0, uLight = 0; i < m_uNumLights; i++) {
        uLight += (UINT)((float)NUM_GEARS * g_liLightInit[i].fRatio + 0.5f);
    }
    if (m_uNumGears == uLight) {
        pgear->nLightIndex = (int)m_uNumLights;
        m_uNumLights++;
    }
    else {
        pgear->nLightIndex = -1;
    }

    fClipHeight = (float)(tan(((m_fFieldOfView / 2.0f) * 480.0f) / 640.0f)) * ((float)(fabs(VIEW_Z)) - 32.0f);

    for (i = 0; i < 4; i++) {

        m_rectBox[m_uNumBoxes++] = prectBox[i];

        pgear->dwClip[i] = TRUE;

        if (prectBox[i].v1.y > -fClipHeight && prectBox[i].v2.y < fClipHeight) {

            fClipRadius = (float)(cos(m_fFieldOfView / 2.0f)) * (float)(fabs(VIEW_Z));

            if (D3DXVec3Length(&D3DXVECTOR3(prectBox[i].v1.x, 0.0f, prectBox[i].v2.z)) < fClipRadius &&
                D3DXVec3Length(&D3DXVECTOR3(prectBox[i].v2.x, 0.0f, prectBox[i].v2.z)) < fClipRadius) 
            {
                pgear->dwClip[i] = FALSE;
            }
        }
    }

    m_uNumGears++;

    pgear->vPos = *pvPos;
    pgear->vDir = *pvDir;
    pgear->fAngularVelocity = fAngularVelocity;
    pgear->fSpin = fAngle;

    // Create attached gears
    for (i = 0; i < 2; i++) {
        pgear->pgearNext[i] = AddAttachedGear(pgear, i, dwDepth);
    }

    return pgear;
}

//******************************************************************************
PGEAR CClockwork::AddAttachedGear(PGEAR pgearParent, UINT uCog, DWORD dwDepth) {

    PGEAR           pgear = NULL;
    COGSIZE         csCogSize[3];
    D3DXVECTOR3     vPos, vDir, vOffset;
    float           fCogOffset[2];
    float           fRotate, fAngle2;
    UINT            uDir, uBlocked, uBlockedDir[6];
    D3DRECT3D       rectBox[4];
    BOOL            bCollision;
    UINT            i, j;
    static float    fCogRadius[3] = {
                        S_COG_RADIUS,
                        M_COG_RADIUS,
                        L_COG_RADIUS
                    };
    static float    fCogWidth[3] = {
                        S_COG_WIDTH,
                        M_COG_WIDTH,
                        L_COG_WIDTH
                    };
    static float    fGearRatio[3] = {
                        (float)(S_COG_SIDES) / (float)(S_COG_SIDES),
                        (float)(M_COG_SIDES) / (float)(S_COG_SIDES),
                        (float)(L_COG_SIDES) / (float)(S_COG_SIDES)
                    };

    // Choose cog sizes for the attached gear
    for (j = 0; j < 3; j++) {
        switch (rand() % 3) {
            case 0:
                csCogSize[j] = COGSIZE_SMALL;
                break;
            case 1:
                csCogSize[j] = COGSIZE_MEDIUM;
                break;
            case 2:
                csCogSize[j] = COGSIZE_LARGE;
                break;
        }
    }

    // Space the cogs out
    fCogOffset[1] = GEAR_LENGTH;
    fCogOffset[0] = MIN_COG_SPACING + (FRND(fCogOffset[1] - 2.0f * MIN_COG_SPACING));

    memset(uBlockedDir, 0, 6 * sizeof(UINT));

    // Disallow parallel shafts connected to the center gear
    if (!uCog) {
        if (pgearParent->vDir.x != 0.0f) {
            uBlockedDir[0] = 1;
            uBlockedDir[1] = 1;
        }
        else if (pgearParent->vDir.y != 0.0f) {
            uBlockedDir[2] = 1;
            uBlockedDir[3] = 1;
        }
        else {
            uBlockedDir[4] = 1;
            uBlockedDir[5] = 1;
        }
        uBlocked = 2;
    }
    else {
        uBlocked = 0;
    }

    do {

        vPos = pgearParent->vPos + (pgearParent->vDir * pgearParent->fCogOffset[uCog]);

        do {
            uDir = rand() % 6;
        } while (uBlockedDir[uDir]);

        switch (uDir) {
            case 0:
                vDir = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
                break;
            case 1:
                vDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
                break;
            case 2:
                vDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
                break;
            case 3:
                vDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
                break;
            case 4:
                vDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
                break;
            case 5:
                vDir = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
                break;
        }

        if ((pgearParent->vDir.x != 0.0f && vDir.x != 0.0f) ||
            (pgearParent->vDir.y != 0.0f && vDir.y != 0.0f) ||
            (pgearParent->vDir.z != 0.0f && vDir.z != 0.0f))   // Parallel shafts
        {
            vOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            if (vDir.x != 0.0f) {
                if (rand() % 2) {
                    vOffset.y = (rand() % 2) ? 1.0f : -1.0f;
                }
                else {
                    vOffset.z = (rand() % 2) ? 1.0f : -1.0f;
                }
            }
            else if (vDir.y != 0.0f) {
                if (rand() % 2) {
                    vOffset.x = (rand() % 2) ? 1.0f : -1.0f;
                }
                else {
                    vOffset.z = (rand() % 2) ? 1.0f : -1.0f;
                }
            }
            else {
                if (rand() % 2) {
                    vOffset.y = (rand() % 2) ? 1.0f : -1.0f;
                }
                else {
                    vOffset.x = (rand() % 2) ? 1.0f : -1.0f;
                }
            }

            vOffset *= (fCogRadius[pgearParent->csCogSize[uCog]] + fCogRadius[csCogSize[0]] + TOOTH_HEIGHT);
            vPos += vOffset;

            if (pgearParent->vDir == vDir) {
                fRotate = -1.0f;
            }
            else {
                fRotate = 1.0f;
            }
        }
        else {

            vOffset = vDir * (fCogRadius[pgearParent->csCogSize[uCog]] + fCogWidth[csCogSize[0]] / 2.0f);
            fRotate = (rand() % 2) ? 1.0f : -1.0f;
            vOffset += (pgearParent->vDir * (fRotate * (fCogRadius[csCogSize[0]] + fCogWidth[pgearParent->csCogSize[uCog]] / 2.0f)));
            vPos += vOffset;
        }

        if ((pgearParent->vDir.x != 0.0f && vOffset.y > 0.0f) || (pgearParent->vDir.y != 0.0f && vOffset.x > 0.0f) || (pgearParent->vDir.z != 0.0f && vOffset.y > 0.0f)) {
            fAngle2 = M_PIDIV2 * (fGearRatio[pgearParent->csCogSize[uCog]] / fGearRatio[csCogSize[0]]) + M_PIDIV2;
        }
        else if ((pgearParent->vDir.x != 0.0f && vOffset.y < 0.0f) || (pgearParent->vDir.y != 0.0f && vOffset.x < 0.0f) || (pgearParent->vDir.z != 0.0f && vOffset.y < 0.0f)) {
            fAngle2 = M_PIDIV2 * (fGearRatio[pgearParent->csCogSize[uCog]] / fGearRatio[csCogSize[0]]) + M_3PIDIV2;
        }
        else if ((pgearParent->vDir.x != 0.0f && vOffset.z > 0.0f) || (pgearParent->vDir.y != 0.0f && vOffset.z > 0.0f) || (pgearParent->vDir.z != 0.0f && vOffset.x < 0.0f)) {
            fAngle2 = M_PIDIV2 * (fGearRatio[pgearParent->csCogSize[uCog]] / fGearRatio[csCogSize[0]]) + M_PI;
        }
        else {
            fAngle2 = 0.0f;
        }

        fAngle2 += fRotate * pgearParent->fSpin * (fGearRatio[pgearParent->csCogSize[uCog]] / fGearRatio[csCogSize[0]]);
        if (fRotate > 0.0f) {
            fAngle2 += M_PI;
        }

        if (vDir.x > 0.0f) {
            SetRect3D(&rectBox[0], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] / 2.0f), vPos.y - SHAFT_RADIUS, vPos.z - SHAFT_RADIUS), &D3DXVECTOR3(vPos.x + fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f), vPos.y + SHAFT_RADIUS, vPos.z + SHAFT_RADIUS));
            SetRect3D(&rectBox[1], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] / 2.0f), vPos.y - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.z - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogWidth[csCogSize[0]] / 2.0f), vPos.y + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.z + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[2], &D3DXVECTOR3(vPos.x + fCogOffset[0] - (fCogWidth[csCogSize[1]] / 2.0f), vPos.y - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + fCogOffset[0] + (fCogWidth[csCogSize[1]] / 2.0f), vPos.y + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[3], &D3DXVECTOR3(vPos.x + fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f), vPos.y - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f), vPos.y + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)));
        }
        else if (vDir.x < 0.0f) {
            SetRect3D(&rectBox[0], &D3DXVECTOR3(vPos.x - fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f), vPos.y - SHAFT_RADIUS, vPos.z - SHAFT_RADIUS), &D3DXVECTOR3(vPos.x + (fCogWidth[csCogSize[0]] / 2.0f), vPos.y + SHAFT_RADIUS, vPos.z + SHAFT_RADIUS));
            SetRect3D(&rectBox[1], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] / 2.0f), vPos.y - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.z - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogWidth[csCogSize[0]] / 2.0f), vPos.y + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.z + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[2], &D3DXVECTOR3(vPos.x - fCogOffset[0] - (fCogWidth[csCogSize[1]] / 2.0f), vPos.y - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x - fCogOffset[0] + (fCogWidth[csCogSize[1]] / 2.0f), vPos.y + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[3], &D3DXVECTOR3(vPos.x - fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f), vPos.y - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x - fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f), vPos.y + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)));
        }
        else if (vDir.y > 0.0f) {
            SetRect3D(&rectBox[0], &D3DXVECTOR3(vPos.x - SHAFT_RADIUS, vPos.y - (fCogWidth[csCogSize[0]] / 2.0f), vPos.z - SHAFT_RADIUS), &D3DXVECTOR3(vPos.x + SHAFT_RADIUS, vPos.y + fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f), vPos.z + SHAFT_RADIUS));
            SetRect3D(&rectBox[1], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.y - (fCogWidth[csCogSize[0]] / 2.0f), vPos.z - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.y + (fCogWidth[csCogSize[0]] / 2.0f), vPos.z + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[2], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y + fCogOffset[0] - (fCogWidth[csCogSize[1]] / 2.0f), vPos.z - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y + fCogOffset[0] + (fCogWidth[csCogSize[1]] / 2.0f), vPos.z + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[3], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y + fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f), vPos.z - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y + fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f), vPos.z + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)));
        }
        else if (vDir.y < 0.0f) {
            SetRect3D(&rectBox[0], &D3DXVECTOR3(vPos.x - SHAFT_RADIUS, vPos.y - fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f), vPos.z - SHAFT_RADIUS), &D3DXVECTOR3(vPos.x + SHAFT_RADIUS, vPos.y + (fCogWidth[csCogSize[0]] / 2.0f), vPos.z + SHAFT_RADIUS));
            SetRect3D(&rectBox[1], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.y - (fCogWidth[csCogSize[0]] / 2.0f), vPos.z - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.y + (fCogWidth[csCogSize[0]] / 2.0f), vPos.z + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[2], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y - fCogOffset[0] - (fCogWidth[csCogSize[1]] / 2.0f), vPos.z - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y - fCogOffset[0] + (fCogWidth[csCogSize[1]] / 2.0f), vPos.z + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT)));
            SetRect3D(&rectBox[3], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y - fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f), vPos.z - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y - fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f), vPos.z + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT)));
        }
        else if (vDir.z > 0.0f) {
            SetRect3D(&rectBox[0], &D3DXVECTOR3(vPos.x - SHAFT_RADIUS, vPos.y - SHAFT_RADIUS, vPos.z - (fCogWidth[csCogSize[0]] / 2.0f)), &D3DXVECTOR3(vPos.x + SHAFT_RADIUS, vPos.y + SHAFT_RADIUS, vPos.z + fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f)));
            SetRect3D(&rectBox[1], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.y - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.z - (fCogWidth[csCogSize[0]] / 2.0f)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.y + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.z + (fCogWidth[csCogSize[0]] / 2.0f)));
            SetRect3D(&rectBox[2], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z + fCogOffset[0] - (fCogWidth[csCogSize[1]] / 2.0f)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z + fCogOffset[0] + (fCogWidth[csCogSize[1]] / 2.0f)));
            SetRect3D(&rectBox[3], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z + fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z + fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f)));
        }
        else {
            SetRect3D(&rectBox[0], &D3DXVECTOR3(vPos.x - SHAFT_RADIUS, vPos.y - SHAFT_RADIUS, vPos.z - fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f)), &D3DXVECTOR3(vPos.x + SHAFT_RADIUS, vPos.y + SHAFT_RADIUS, vPos.z + (fCogWidth[csCogSize[0]] / 2.0f)));
            SetRect3D(&rectBox[1], &D3DXVECTOR3(vPos.x - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.y - (fCogWidth[csCogSize[0]] + TOOTH_HEIGHT), vPos.z - (fCogWidth[csCogSize[0]] / 2.0f)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.y + (fCogRadius[csCogSize[0]] + TOOTH_HEIGHT), vPos.z + (fCogWidth[csCogSize[0]] / 2.0f)));
            SetRect3D(&rectBox[2], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y - (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z - fCogOffset[0] - (fCogWidth[csCogSize[1]] / 2.0f)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.y + (fCogRadius[csCogSize[1]] + TOOTH_HEIGHT), vPos.z - fCogOffset[0] + (fCogWidth[csCogSize[1]] / 2.0f)));
            SetRect3D(&rectBox[3], &D3DXVECTOR3(vPos.x - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y - (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z - fCogOffset[1] - (fCogWidth[csCogSize[2]] / 2.0f)), &D3DXVECTOR3(vPos.x + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.y + (fCogRadius[csCogSize[2]] + TOOTH_HEIGHT), vPos.z - fCogOffset[1] + (fCogWidth[csCogSize[2]] / 2.0f)));
        }

        bCollision = FALSE;

        for (j = 0; j < 4 && !bCollision; j++) {
            for (i = 0; i < m_uNumBoxes && !bCollision; i++) {
                bCollision = GearCollision(&rectBox[j], &m_rectBox[i]);
            }
        }

        if (bCollision) {
            uBlockedDir[uDir] = 1;
            uBlocked++;
        }

    } while (bCollision && uBlocked < 6);

    if (uBlocked < 6) {
        pgear = CreateGear(csCogSize, fCogOffset, &vPos, &vDir, fRotate * pgearParent->fAngularVelocity * (fGearRatio[pgearParent->csCogSize[uCog]] / fGearRatio[csCogSize[0]]), fAngle2, rectBox, dwDepth+1);
    }

    return pgear;
}

//******************************************************************************
void CClockwork::AddGear(PGEAR pgear, DWORD dwDepth) {

    UINT i;

    if (m_uNumGears == NUM_GEARS || dwDepth > m_dwMaxDepth) {
        return;
    }

    for (i = 0; i < 2; i++) {

        if (!pgear->pgearNext[i]) {
            pgear->pgearNext[i] = AddAttachedGear(pgear, i, dwDepth);
        }
        else {
            AddGear(pgear->pgearNext[i], dwDepth + 1);
        }
    }
}

//******************************************************************************
void CClockwork::RotateGear(PGEAR pgear, float fTheta) {

    float       fAngularVelocity;
    D3DXMATRIX  r;
    UINT        i;

    if (!pgear) {
        return;
    }

    fAngularVelocity = fTheta * pgear->fAngularVelocity;
    pgear->mRotate._22 = (float)(cos(fAngularVelocity));
    pgear->mRotate._23 = (float)(sin(fAngularVelocity));
    pgear->mRotate._32 = -pgear->mRotate._23;
    pgear->mRotate._33 = pgear->mRotate._22;

    D3DXMatrixMultiply(&r, &pgear->mRotate, &pgear->mWorld);

    memcpy(&pgear->mWorld, &r, sizeof(D3DMATRIX));

    for (i = 0; i < 2; i++) {
        RotateGear(pgear->pgearNext[i], fTheta);
    }
}

//******************************************************************************
void CClockwork::RenderGear(PGEAR pgear) {

    UINT i, j;

    if (!pgear) {
        return;
    }

    // Set light states if necessary
    if (pgear->nMaterialIndex >= 0) {
        m_pDevice->SetMaterial(&m_material[pgear->nMaterialIndex]);
    }
    if (pgear->nLightIndex >= 0) {
        m_pDevice->SetLight(0, &m_ldLight[pgear->nLightIndex].light);
        m_pDevice->SetRenderState(D3DRS_AMBIENT, m_ldLight[pgear->nLightIndex].cAmbient);
        m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, m_ldLight[pgear->nLightIndex].bSpecular);
    }

    // Set the world transform
    m_pDevice->SetTransform(D3DTS_WORLD, &pgear->mWorld);

    m_pDevice->SetTexture(0, m_pd3dtShaft[pgear->dwShaftTextureIndex]);

#ifndef UNDER_XBOX
    m_pDevice->SetRenderState(D3DRS_CLIPPING, pgear->dwClip[0]);
#endif

    m_pDevice->SetStreamSource(0, pgear->pd3drShaft, sizeof(VERTEX));
    m_pDevice->SetIndices(m_pd3diShaft, 0);

    // Draw the shaft
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwShaftVertices,
                    0, m_dwShaftPrimitives);

    for (i = 0; i < 3; i++) {

        m_pDevice->SetTexture(0, m_pd3dtCog[pgear->cog[i].dwTextureIndex]);

#ifndef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_CLIPPING, pgear->dwClip[i+1]);
#endif

        m_pDevice->SetStreamSource(0, pgear->cog[i].pd3drCenter, sizeof(VERTEX));
        m_pDevice->SetIndices(pgear->cog[i].pd3diCenter, 0);

        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                    pgear->cog[i].dwCenterVertices, 0, pgear->cog[i].dwCenterPrimitives);

        m_pDevice->SetStreamSource(0, pgear->cog[i].pd3drTeeth, sizeof(VERTEX));

        for (j = 0; j < pgear->cog[i].dwSides; j++) {

            m_pDevice->SetIndices(m_pd3diTooth, j * 8);
            m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                    m_dwToothVertices, 0, m_dwToothPrimitives);
        }
    }

    for (i = 0; i < 2; i++) {
        RenderGear(pgear->pgearNext[i]);
    }
}

//******************************************************************************
void CClockwork::RenderGearP(PGEAR pgear) {

    D3DXMATRIX mR1, mR2;
    UINT i, j;

    if (!pgear) {
        return;
    }

    if (pgear->nMaterialIndex >= 0) {

        D3DXVECTOR4 vPower, vDBase;

        memcpy(&m_matCurrent, &m_material[pgear->nMaterialIndex], sizeof(D3DMATERIAL8));

        vPower = D3DXVECTOR4(m_matCurrent.Power, 0.0f, 1.0f, 0.0f);
        m_pDevice->SetVertexShaderConstant(6, &vPower, 1);

        vDBase = D3DXVECTOR4(m_litCurrent.Diffuse.r * m_matCurrent.Diffuse.r, m_litCurrent.Diffuse.g * m_matCurrent.Diffuse.g, m_litCurrent.Diffuse.b * m_matCurrent.Diffuse.b, m_matCurrent.Diffuse.a);
        m_pDevice->SetVertexShaderConstant(7, &vDBase, 1);
    }

    if (pgear->nLightIndex >= 0) {

        D3DXVECTOR4 vLightDir, vDOffset, vSBase;
        D3DXVECTOR3 vAmbient;

        memcpy(&m_litCurrent, &m_ldLight[pgear->nLightIndex].light, sizeof(D3DMATERIAL8));

        D3DXVec3Normalize((D3DXVECTOR3*)&vLightDir, (D3DXVECTOR3*)&m_litCurrent.Direction);
        m_pDevice->SetVertexShaderConstant(4, &vLightDir, 1);

        vAmbient = D3DXVECTOR3((float)RGBA_GETRED(m_ldLight[pgear->nLightIndex].cAmbient) / 255.0f,
                               (float)RGBA_GETGREEN(m_ldLight[pgear->nLightIndex].cAmbient) / 255.0f,
                               (float)RGBA_GETBLUE(m_ldLight[pgear->nLightIndex].cAmbient) / 255.0f);

        vDOffset = D3DXVECTOR4((vAmbient.x + m_litCurrent.Ambient.r) * m_matCurrent.Ambient.r + m_matCurrent.Emissive.r,
                               (vAmbient.y + m_litCurrent.Ambient.g) * m_matCurrent.Ambient.g + m_matCurrent.Emissive.g,
                               (vAmbient.z + m_litCurrent.Ambient.b) * m_matCurrent.Ambient.b + m_matCurrent.Emissive.b,
                               0.0f);
        m_pDevice->SetVertexShaderConstant(8, &vDOffset, 1);

        vSBase = D3DXVECTOR4(m_litCurrent.Specular.r * m_matCurrent.Specular.r, m_litCurrent.Specular.g * m_matCurrent.Specular.g, m_litCurrent.Specular.b * m_matCurrent.Specular.b, m_matCurrent.Specular.a);
        m_pDevice->SetVertexShaderConstant(9, &vSBase, 1);

        m_pDevice->SetVertexShader(m_ldLight[pgear->nLightIndex].dwVShader);
    }

    D3DXMatrixMultiply(&mR1, &pgear->mWorld, m_pmView);
    D3DXMatrixMultiply(&mR2, &mR1, &m_mProj);
    D3DXMatrixTranspose(&mR1, &mR2);

    m_pDevice->SetVertexShaderConstant(0, &mR1, 4);

    m_pDevice->SetTexture(0, m_pd3dtShaft[pgear->dwShaftTextureIndex]);

#ifndef UNDER_XBOX
    m_pDevice->SetRenderState(D3DRS_CLIPPING, pgear->dwClip[0]);
#endif

    m_pDevice->SetStreamSource(0, pgear->pd3drShaft, sizeof(VERTEX));
    m_pDevice->SetIndices(m_pd3diShaft, 0);

    // Draw the shaft
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwShaftVertices,
                    0, m_dwShaftPrimitives);

    for (i = 0; i < 3; i++) {

        m_pDevice->SetTexture(0, m_pd3dtCog[pgear->cog[i].dwTextureIndex]);

#ifndef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_CLIPPING, pgear->dwClip[i+1]);
#endif

        m_pDevice->SetStreamSource(0, pgear->cog[i].pd3drCenter, sizeof(VERTEX));
        m_pDevice->SetIndices(pgear->cog[i].pd3diCenter, 0);

        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                    pgear->cog[i].dwCenterVertices, 0, pgear->cog[i].dwCenterPrimitives);

        m_pDevice->SetStreamSource(0, pgear->cog[i].pd3drTeeth, sizeof(VERTEX));

        for (j = 0; j < pgear->cog[i].dwSides; j++) {

            m_pDevice->SetIndices(m_pd3diTooth, j * 8);
            m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                    m_dwToothVertices, 0, m_dwToothPrimitives);
        }
    }

    for (i = 0; i < 2; i++) {
        RenderGear(pgear->pgearNext[i]);
    }
}

//******************************************************************************
void CClockwork::ReleaseGear(PGEAR pgear) {

    if (pgear) {
        UINT i;
        for (i = 0; i < 2; i++) {
            if (pgear->pgearNext[i]) {
                ReleaseGear(pgear->pgearNext[i]);
            }
        }
        ReleaseVertexBuffer(pgear->pd3drShaft);
        for (i = 0; i < 3; i++) {
            ReleaseCog(&pgear->cog[i]);
        }
        delete [] pgear;
        pgear = NULL;
    }
}

//******************************************************************************
BOOL CClockwork::Create(LPDIRECT3DDEVICE8 pDevice, PCAMERA pcam, float fWidth, float fHeight) {

    COGSIZE   csCogSize[3];
    float     fCogOffset[2];
    D3DRECT3D rectBox[4];
    float     fBoundHeight = BOUND_RADIUS * (fHeight / fWidth);
    UINT      i, j, uIndex;
    LPWORD    pwShaft, pwTooth;
    TCHAR     szTexture[256];
    DWORD     dwSDecl[] = {
                    D3DVSD_STREAM(0),
                    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                    D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                    D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                    D3DVSD_END()
              };

    m_pDevice = pDevice;

    SetPerspectiveProjection(&m_mProj, pcam->fNearPlane, pcam->fFarPlane, 
            pcam->fFieldOfView, fHeight / fWidth);

    m_fFieldOfView = pcam->fFieldOfView;

    // Create textures
    for (i = 0; i < NUM_COG_TEXTURES; i++) {
        _stprintf(szTexture, TEXT("cog%d.bmp"), i);
        m_pd3dtCog[i] = (LPDIRECT3DTEXTURE8)CreateTexture(pDevice, szTexture, D3DFMT_X8R8G8B8);
        if (!m_pd3dtCog[i]) {
            return FALSE;
        }
#ifdef UNDER_XBOX
        m_pd3dtCog[i]->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX
    }

    for (i = 0; i < NUM_SHAFT_TEXTURES; i++) {
        _stprintf(szTexture, TEXT("shaft%d.bmp"), i);
        m_pd3dtShaft[i] = (LPDIRECT3DTEXTURE8)CreateTexture(pDevice, szTexture, D3DFMT_X8R8G8B8);
        if (!m_pd3dtShaft[i]) {
            return FALSE;
        }
#ifdef UNDER_XBOX
        m_pd3dtShaft[i]->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX
    }

    // Create materials
    for (i = 0; i < NUM_MATERIALS; i++) {
        SetMaterial(&m_material[i], RGB_MAKE(144+rand()%112,144+rand()%112,144+rand()%112), 
                            RGB_MAKE(196,196,196), 
                            RGB_MAKE(128+rand()%64,128+rand()%64,128+rand()%64), 
                            0, FRND(100.0f));
    }

    // Create lights
    for (i = 0; i < NUM_LIGHTS; i++) {
        SetLight(&m_ldLight[i].light, g_liLightInit[i].d3dlt, D3DXVECTOR3(0.0f, 0.0f, VIEW_Z),
                            D3DXVECTOR3(0.0f, -1.0f, 1.0f), 0xFFFFFFFF, RGB_MAKE(196, 196, 196),
                            RGB_MAKE(255, 255, 255), D3DLIGHT_RANGE_MAX, 1.0f, 1.0f, 0.0f, 0.0f,
                            1.57f, 2.35f);
        m_ldLight[i].cAmbient = RGB_MAKE(96 + rand() % 32, 96 + rand() % 32, 96 + rand() % 32);
        m_ldLight[i].bSpecular = g_liLightInit[i].bSpecular;

        // Create vertex shaders
//        m_ldLight[i].dwVShader = CreateVertexShader(pDevice, dwSDecl, g_liLightInit[i].szVShader);
//        if (m_ldLight[i].dwVShader == INVALID_SHADER_HANDLE) {
//            return FALSE;
//        }
    }

    // Create a pentagonal shaft cylinder of length 1.0
    if (!CreateCylinder((PVERTEX*)&m_prUnitShaft, &m_dwShaftVertices, &pwShaft, 
                        &m_dwShaftIndices, 5, SHAFT_RADIUS, 1.0f, CYLCAPS_NONE))
    {
        return FALSE;
    }
    m_dwShaftPrimitives = m_dwShaftIndices / 3;

    for (i = 0; i < m_dwShaftVertices; i++) {
        m_prUnitShaft[i].vPosition.x += 0.5f;
    }

    m_pd3diShaft = CreateIndexBuffer(pDevice, pwShaft, m_dwShaftIndices * sizeof(WORD));

    MemFree(pwShaft);

    if (!m_pd3diShaft) {
        return FALSE;
    }

    // Initialize tooth vertices and indices
    m_dwToothVertices = 8;
    m_dwToothIndices = 30;
    m_dwToothPrimitives = 10;

    m_pd3diTooth = CreateIndexBuffer(pDevice, NULL, m_dwToothIndices * sizeof(WORD));
    if (!m_pd3diTooth) {
        return FALSE;
    }

    m_pd3diTooth->Lock(0, 0, (LPBYTE*)&pwTooth, 0);

    for (i = 0, uIndex = 0; i < 3; i++) {
        pwTooth[uIndex++] = i * 2;
        pwTooth[uIndex++] = i * 2 + 2;
        pwTooth[uIndex++] = i * 2 + 3;
        pwTooth[uIndex++] = i * 2;
        pwTooth[uIndex++] = i * 2 + 3;
        pwTooth[uIndex++] = i * 2 + 1;
    }

    for (i = 1; i < 3; i++) {
        pwTooth[uIndex++] = 0;
        pwTooth[uIndex++] = (i + 1) * 2;
        pwTooth[uIndex++] = i * 2;
    }

    for (i = 1; i < 3; i++) {
        pwTooth[uIndex++] = 1;
        pwTooth[uIndex++] = i * 2 + 1;
        pwTooth[uIndex++] = (i + 1) * 2 + 1;
    }

    m_pd3diTooth->Unlock();

    // Create six bounding boxes to enclose the gears
    SetRect3D(&m_rectBox[0], &D3DXVECTOR3(-BOUND_RADIUS - 1.0f, -fBoundHeight, -BOUND_RADIUS), &D3DXVECTOR3(-BOUND_RADIUS, fBoundHeight, BOUND_RADIUS));
    SetRect3D(&m_rectBox[1], &D3DXVECTOR3(BOUND_RADIUS, -fBoundHeight, -BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS + 1.0f, fBoundHeight, BOUND_RADIUS));
    SetRect3D(&m_rectBox[2], &D3DXVECTOR3(-BOUND_RADIUS, -fBoundHeight - 1.0f, -BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS, -fBoundHeight, BOUND_RADIUS));
    SetRect3D(&m_rectBox[3], &D3DXVECTOR3(-BOUND_RADIUS, fBoundHeight, -BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS, fBoundHeight + 1.0f, BOUND_RADIUS));
    SetRect3D(&m_rectBox[4], &D3DXVECTOR3(-BOUND_RADIUS, -fBoundHeight, -BOUND_RADIUS - 1.0f), &D3DXVECTOR3(BOUND_RADIUS, fBoundHeight, -BOUND_RADIUS));
    SetRect3D(&m_rectBox[5], &D3DXVECTOR3(-BOUND_RADIUS, -fBoundHeight, BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS, fBoundHeight, BOUND_RADIUS + 1.0f));

    m_uNumBoxes = 6;

    m_uNumGears = 0;
    m_uNumMaterials = 0;
    m_uNumLights = 0;

    m_uCogTextures = 0;
    m_uShaftTextures = 0;

    m_dwMaxDepth = ((UINT)((float)(log((double)NUM_GEARS)) / (float)(log(2.0))) + 1);

    csCogSize[0] = COGSIZE_SMALL;
    csCogSize[1] = COGSIZE_LARGE;
    csCogSize[2] = COGSIZE_MEDIUM;
    fCogOffset[1] = GEAR_LENGTH;
    fCogOffset[0] = fCogOffset[1] / 2.0f;
    SetRect3D(&rectBox[0], &D3DXVECTOR3(-10.0f - (S_COG_WIDTH / 2.0f), -SHAFT_RADIUS, -SHAFT_RADIUS), &D3DXVECTOR3(-10.0f + fCogOffset[1] + (M_COG_WIDTH / 2.0f), SHAFT_RADIUS, SHAFT_RADIUS));
    SetRect3D(&rectBox[1], &D3DXVECTOR3(-10.0f - (S_COG_WIDTH / 2.0f), -(S_COG_RADIUS + TOOTH_HEIGHT), -(S_COG_RADIUS + TOOTH_HEIGHT)), &D3DXVECTOR3(-10.0f + (S_COG_WIDTH / 2.0f), (S_COG_RADIUS + TOOTH_HEIGHT), (S_COG_RADIUS + TOOTH_HEIGHT)));
    SetRect3D(&rectBox[2], &D3DXVECTOR3(-10.0f + fCogOffset[0] - (L_COG_WIDTH / 2.0f), -(L_COG_RADIUS + TOOTH_HEIGHT), -(L_COG_RADIUS + TOOTH_HEIGHT)), &D3DXVECTOR3(-10.0f + fCogOffset[0] + (L_COG_WIDTH / 2.0f), (L_COG_RADIUS + TOOTH_HEIGHT), (L_COG_RADIUS + TOOTH_HEIGHT)));
    SetRect3D(&rectBox[3], &D3DXVECTOR3(-10.0f + fCogOffset[1] - (M_COG_WIDTH / 2.0f), -(M_COG_RADIUS + TOOTH_HEIGHT), -(M_COG_RADIUS + TOOTH_HEIGHT)), &D3DXVECTOR3(-10.0f + fCogOffset[1] + (M_COG_WIDTH / 2.0f), (M_COG_RADIUS + TOOTH_HEIGHT), (M_COG_RADIUS + TOOTH_HEIGHT)));

    m_pgearRoot = CreateGear(csCogSize, fCogOffset, &D3DXVECTOR3(-10.0f, 0.0f, 0.0f), &D3DXVECTOR3(1.0f, 0.0f, 0.0f), M_PI / 20.0f, 0.0f, rectBox);
    if (!m_pgearRoot) {
        return FALSE;
    }

//DebugString(TEXT("Created gears: %d"), m_uNumGears);

    for (m_dwMaxDepth++; m_uNumGears < NUM_GEARS && m_dwMaxDepth <= 15; m_dwMaxDepth++) {
        AddGear(m_pgearRoot);
    }

//DebugString(TEXT("Created gears: %d"), g_uNumGears);

    if (m_uNumGears != NUM_GEARS) {
        OutputString(TEXT("WARNING: The requested number of gears (%d) could ")
                    TEXT("not be created: only %d gears fit into the scene.\r\n")
                    TEXT("Please lower the number of gears requested or change ")
                    TEXT("the seed for the pseudo-random number generator to\r\n")
                    TEXT("obtain a different gear set"), NUM_GEARS, m_uNumGears);
    }

    return TRUE;
}

//******************************************************************************
CClockwork::~CClockwork() {

    UINT i;

    MemFree32(m_prUnitShaft);
    ReleaseIndexBuffer(m_pd3diShaft);
    ReleaseIndexBuffer(m_pd3diTooth);

    for (i = 0; i < NUM_LIGHTS; i++) {
        if (m_ldLight[i].dwVShader != INVALID_SHADER_HANDLE) {
            ReleaseVertexShader(m_pDevice, m_ldLight[i].dwVShader);
        }
    }

    for (i = 0; i < NUM_COG_TEXTURES; i++) {
        if (m_pd3dtCog[i]) {
            ReleaseTexture(m_pd3dtCog[i]);
        }
    }
    for (i = 0; i < NUM_SHAFT_TEXTURES; i++) {
        if (m_pd3dtShaft[i]) {
            ReleaseTexture(m_pd3dtShaft[i]);
        }
    }

    ReleaseGear(m_pgearRoot);
}

//******************************************************************************
void CClockwork::Update(float fTimeDilation) {

//    RotateGear(m_pgearRoot, M_PI / 40.0f * fTimeDilation);
    RotateGear(m_pgearRoot, GEAR_ROTATION_ANGLE * fTimeDilation);
}

//******************************************************************************
void CClockwork::Render() {

    RenderGear(m_pgearRoot);
}

//******************************************************************************
void CClockwork::Render(D3DXMATRIX* pmView) {

    m_pmView = pmView;

    RenderGearP(m_pgearRoot);
}

//******************************************************************************
// Create a cylinder oriented along the x axis
BOOL CClockwork::CreateCylinder(PVERTEX* pprVertices, LPDWORD pdwNumVertices, 
                    LPWORD* ppwIndices, LPDWORD pdwNumIndices, 
                    DWORD dwSides, float fRadius, float fWidth,
                    DWORD dwCylinderCaps) 
{
    PVERTEX     prVertices;
    DWORD       dwNumVertices;
    LPWORD      pwIndices;
    DWORD       dwNumIndices;
    float       fSin, fCos, fTheta, fDelta;
    D3DXVECTOR3 vN;
    UINT        uIndex;
    UINT        i;

    if (!pprVertices || !pdwNumVertices || !ppwIndices || !pdwNumIndices) {
        return FALSE;
    }

    dwNumVertices = dwSides * 2;

    prVertices = (PVERTEX)MemAlloc32(dwNumVertices * sizeof(VERTEX));
    if (!prVertices) {
        return FALSE;
    }

    fDelta = M_2PI / (float)dwSides;

    for (i = 0, fTheta = 0.0f; i < dwSides; i++, fTheta += fDelta) {

        fSin = (float)(sin(fTheta));
        fCos = (float)(cos(fTheta));

        prVertices[i*2]   = VERTEX(D3DXVECTOR3(fWidth / 2.0f, fSin * fRadius, fCos * fRadius),
                                      *D3DXVec3Normalize(&vN, &D3DXVECTOR3(1.0f, fSin, fCos)), 
                                      1.0f - ((fCos + 1.0f) / 2.0f), 1.0f - ((fSin + 1.0f) / 2.0f));
        prVertices[i*2+1] = VERTEX(D3DXVECTOR3(-fWidth / 2.0f, fSin * fRadius, fCos * fRadius),
                                      *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f, fSin, fCos)), 
                                      (fCos + 1.0f) / 2.0f, 1.0f - ((fSin + 1.0f) / 2.0f));
    }

    dwNumIndices = 6 * dwSides;
    if (dwCylinderCaps & CYLCAPS_TOP) {
        dwNumIndices += 3 * (dwSides - 2);
    }
    if (dwCylinderCaps & CYLCAPS_BOTTOM) {
        dwNumIndices += 3 * (dwSides - 2);
    }

    pwIndices = (LPWORD)MemAlloc(dwNumIndices * sizeof(WORD));
    if (!pwIndices) {
        MemFree32(prVertices);
        return FALSE;
    }

    for (i = 0, uIndex = 0; i < dwSides - 1; i++) {
        pwIndices[uIndex++] = i * 2;
        pwIndices[uIndex++] = i * 2 + 2;
        pwIndices[uIndex++] = i * 2 + 3;
        pwIndices[uIndex++] = i * 2;
        pwIndices[uIndex++] = i * 2 + 3;
        pwIndices[uIndex++] = i * 2 + 1;
    }

    pwIndices[uIndex++] = i * 2;
    pwIndices[uIndex++] = 0;
    pwIndices[uIndex++] = 1;
    pwIndices[uIndex++] = i * 2;
    pwIndices[uIndex++] = 1;
    pwIndices[uIndex++] = i * 2 + 1;

    if (dwCylinderCaps & CYLCAPS_TOP) {

        for (i = 1; i < dwSides - 1; i++) {
            pwIndices[uIndex++] = 0;
            pwIndices[uIndex++] = (i + 1) * 2;
            pwIndices[uIndex++] = i * 2;
        }
    }

    if (dwCylinderCaps & CYLCAPS_BOTTOM) {

        for (i = 1; i < dwSides - 1; i++) {
            pwIndices[uIndex++] = 1;
            pwIndices[uIndex++] = i * 2 + 1;
            pwIndices[uIndex++] = (i + 1) * 2 + 1;
        }
    }

    *pprVertices = prVertices;
    *pdwNumVertices = dwNumVertices;
    *ppwIndices = pwIndices;
    *pdwNumIndices = dwNumIndices;

    return TRUE;
}

//******************************************************************************
void CClockwork::ReleaseCylinder(PVERTEX prVertices, LPWORD pwIndices) {

    if (prVertices) {
        MemFree32(prVertices);
    }

    if (pwIndices) {
        MemFree(pwIndices);
    }
}

//******************************************************************************
void CClockwork::SetRect3D(PD3DRECT3D prect, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2) {
    
    prect->v1 = *pv1;
    prect->v2 = *pv2;
}

//******************************************************************************
BOOL CClockwork::GearCollision(PD3DRECT3D prectBox1, PD3DRECT3D prectBox2) {

    return !((prectBox1->v1.x >= prectBox2->v2.x || prectBox1->v2.x <= prectBox2->v1.x) ||
             (prectBox1->v1.y >= prectBox2->v2.y || prectBox1->v2.y <= prectBox2->v1.y) ||
             (prectBox1->v1.z >= prectBox2->v2.z || prectBox1->v2.z <= prectBox2->v1.z));
}

//******************************************************************************
void CClockwork::GetFrameStats(PFRAMESTATS pfs) {

    if (!pfs) {
        return;
    }

    GetGearStats(m_pgearRoot, pfs);
}

//******************************************************************************
void CClockwork::GetGearStats(PGEAR pgear, PFRAMESTATS pfs) {

    UINT i, j;

    if (!pgear) {
        return;
    }

    if (pgear->nMaterialIndex >= 0) {
        pfs->dwMaterials++;
    }
    if (pgear->nLightIndex >= 0) {
        pfs->dwLights++;
    }

    pfs->dwTransforms++;

    pfs->dwTextures++;

    pfs->dwVBuffers++;
    pfs->dwIBuffers++;

    pfs->dwMeshes++;
    pfs->dwPolygons += m_dwShaftPrimitives;

    for (i = 0; i < 3; i++) {

        pfs->dwTextures++;

        pfs->dwVBuffers++;
        pfs->dwIBuffers++;

        pfs->dwMeshes++;
        pfs->dwPolygons += pgear->cog[i].dwCenterPrimitives;

        pfs->dwVBuffers++;

        for (j = 0; j < pgear->cog[i].dwSides; j++) {

            // ##REVIEW: Since the SetIndices call for the gear teeth doesn't change the index buffer
            // but instead only changes the offset to add to the indices, should it be counted the 
            // same as other SetIndices calls?
            pfs->dwIBuffers++;
    
            pfs->dwMeshes++;
            pfs->dwPolygons += m_dwToothPrimitives;
        }
    }

    for (i = 0; i < 2; i++) {
        GetGearStats(pgear->pgearNext[i], pfs);
    }
}
