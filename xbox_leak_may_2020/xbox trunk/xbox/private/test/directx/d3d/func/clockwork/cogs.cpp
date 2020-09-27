/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    cogs.cpp

Author:

    Matt Bronder

Description:

    Clockwork animation and rendering.

*******************************************************************************/

#include "d3dlocus.h"
#include "cogs.h"
#include "clockwork.h"

static CDevice8*    g_pDevice;

static D3DRECT3D            g_rectBox[NUM_GEARS * 4 + 6];
static UINT                 g_uNumBoxes;
static UINT                 g_uNumGears;
static DWORD                g_dwMaxDepth;

static UINT                 g_uCogTextures;
static UINT                 g_uShaftTextures;

static float                g_fFieldOfView = M_PI / 4.0f;

//******************************************************************************
BOOL CreateCog(PCOG pcog, DWORD dwSides, float fRadius, float fWidth, float fOffset) {

    float       fToothZ, fToothX, fTheta;
    D3DXVECTOR3 vN;
    D3DXMATRIX  mRotate;
    PVERTEX     pr;
    UINT        i, j;

    if (!pcog) {
        return FALSE;
    }

    memset(pcog, 0, sizeof(COG));

    pcog->dwSides = dwSides;

    pcog->pprTeeth = new PVERTEX[dwSides];
    if (!pcog->pprTeeth) {
        ReleaseCog(pcog);
        return FALSE;
    }
    memset(pcog->pprTeeth, 0, dwSides * sizeof(PVERTEX));

    for (i = 0; i < dwSides; i++) {
        pcog->pprTeeth[i] = (PVERTEX)MemAlloc32(8 * sizeof(VERTEX));
        if (!pcog->pprTeeth[i]) {
            ReleaseCog(pcog);
            return FALSE;
        }        
    }

    if (!CreateCylinder(&pcog->prCenter, &pcog->dwCenterVertices, 
                        &pcog->pwCenter, &pcog->dwCenterIndices, 
                        pcog->dwSides, fRadius, fWidth, 
                        CYLCAPS_BOTH))
    {
        ReleaseCog(pcog);
        return FALSE;
    }

    // Position the first tooth
    fToothX = fWidth / 2.0f * 0.9f;
    fToothZ = pcog->prCenter[0].vPosition.z - ((TOOTH_WIDTH_BASE / 2.0f) / 
            (pcog->prCenter[2].vPosition.y / (pcog->prCenter[0].vPosition.z - 
            pcog->prCenter[2].vPosition.z)));

    pcog->pprTeeth[0][0] = VERTEX(D3DXVECTOR3( fToothX, -TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f, -1.0f, 0.0f)), 0.0f, 1.0f);
    pcog->pprTeeth[0][1] = VERTEX(D3DXVECTOR3(-fToothX, -TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f, -1.0f, 0.0f)), 0.0f, 1.0f);
    pcog->pprTeeth[0][2] = VERTEX(D3DXVECTOR3( fToothX, -TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f, -1.0f, 1.0f)), 1.0f, 1.0f);
    pcog->pprTeeth[0][3] = VERTEX(D3DXVECTOR3(-fToothX, -TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f, -1.0f, 1.0f)), 1.0f, 1.0f);
    pcog->pprTeeth[0][4] = VERTEX(D3DXVECTOR3( fToothX,  TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f,  1.0f, 1.0f)), 1.0f, 0.0f);
    pcog->pprTeeth[0][5] = VERTEX(D3DXVECTOR3(-fToothX,  TOOTH_WIDTH_TOP  / 2.0f, fToothZ + TOOTH_HEIGHT), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f,  1.0f, 1.0f)), 1.0f, 0.0f);
    pcog->pprTeeth[0][6] = VERTEX(D3DXVECTOR3( fToothX,  TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3( 1.0f,  1.0f, 0.0f)), 0.0f, 0.0f);
    pcog->pprTeeth[0][7] = VERTEX(D3DXVECTOR3(-fToothX,  TOOTH_WIDTH_BASE / 2.0f, fToothZ), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(-1.0f,  1.0f, 0.0f)), 0.0f, 0.0f);

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
            D3DXVec3TransformCoord(&pcog->pprTeeth[i][j].vPosition, &pcog->pprTeeth[i-1][j].vPosition, &mRotate);
            D3DXVec3TransformCoord(&pcog->pprTeeth[i][j].vNormal, &pcog->pprTeeth[i-1][j].vNormal, &mRotate);
            memcpy(&pcog->pprTeeth[i][j].u0, &pcog->pprTeeth[i-1][j].u0, 2 * sizeof(float));
        }
    }

    // Offset the cog along the x axis
    for (i = 0; i < pcog->dwCenterVertices; i++)  {
        pcog->prCenter[i].vPosition.x += fOffset;
    }
    for (i = 0; i < dwSides; i++) {
        for (j = 0; j < 8; j++) {
            pcog->pprTeeth[i][j].vPosition.x += fOffset;
        }
    }

    pcog->pd3drCenter = CreateVertexBuffer(g_pDevice, pcog->prCenter, pcog->dwCenterVertices * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!pcog->pd3drCenter) {
        ReleaseCog(pcog);
        return FALSE;
    }

#ifdef UNDER_XBOX
    pcog->pd3drCenter->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    pcog->pd3diCenter = CreateIndexBuffer(g_pDevice, pcog->pwCenter, pcog->dwCenterIndices * sizeof(WORD));
    if (!pcog->pd3diCenter) {
        ReleaseCog(pcog);
        return FALSE;
    }

    g_pDevice->CreateVertexBuffer(dwSides * 8 * sizeof(VERTEX), 0, FVF_VERTEX, D3DPOOL_DEFAULT, &pcog->pd3drTeeth);
    if (!pcog->pd3drTeeth) {
        ReleaseCog(pcog);
        return FALSE;
    }

    pcog->pd3drTeeth->Lock(0, dwSides * 8 * sizeof(VERTEX), (LPBYTE*)&pr, 0);
    for (i = 0; i < dwSides; i++) {
        memcpy(&pr[i * 8], pcog->pprTeeth[i], 8 * sizeof(VERTEX));
    }
    pcog->pd3drTeeth->Unlock();

#ifdef UNDER_XBOX
    pcog->pd3drTeeth->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    return TRUE;
}

//******************************************************************************
void ReleaseCog(PCOG pcog) {

    UINT i;

    ReleaseVertexBuffer(pcog->pd3drCenter);
    ReleaseIndexBuffer(pcog->pd3diCenter);
    ReleaseVertexBuffer(pcog->pd3drTeeth);

    ReleaseCylinder(pcog->prCenter, pcog->pwCenter);

    if (pcog->pprTeeth) {
        for (i = 0; i < pcog->dwSides; i++) {
            if (pcog->pprTeeth[i]) {
                MemFree32(pcog->pprTeeth[i]);
            }
        }
        delete [] pcog->pprTeeth;
    }

    memset(pcog, 0, sizeof(COG));
}

//******************************************************************************
PGEAR CreateGear(PCLOCKWORK pcw, PCOGSIZE pcsCogSize, float* pfCogOffset, 
                 D3DXVECTOR3* pvPos, D3DXVECTOR3* pvDir, float fAngularVelocity, 
                 float fRotation, PD3DRECT3D prectBox, DWORD dwDepth)
{
    PGEAR           pgear;
    D3DXMATRIX      mTranslate, mRotate, mWorld;
    float           fOrient, fAngle, fClipHeight, fClipRadius;
    BOOL            bRet;
    UINT            i, j;

    if (g_uNumGears == NUM_GEARS || dwDepth > g_dwMaxDepth) {
        return NULL;
    }

    if (!pcw || !pvPos || !pvDir) {
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

    pgear->prShaft = (PVERTEX)MemAlloc32(pcw->dwShaftVertices * sizeof(VERTEX));
    if (!pgear->prShaft) {
        ReleaseGear(pgear);
        return NULL;
    }
    for (i = 0; i < pcw->dwShaftVertices; i++) {
        pgear->prShaft[i] = pcw->prUnitShaft[i];
        pgear->prShaft[i].vPosition.x *= pfCogOffset[1];
    }

    pgear->pd3drShaft = CreateVertexBuffer(g_pDevice, pgear->prShaft, pcw->dwShaftVertices * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!pgear->pd3drShaft) {
        ReleaseGear(pgear);
        return NULL;
    }

#ifdef UNDER_XBOX
    pgear->pd3drShaft->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

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
        pgear->cog[i].dwTextureIndex = g_uCogTextures++ % NUM_COG_TEXTURES;
    }
    pgear->dwShaftTextureIndex = g_uShaftTextures++ % NUM_SHAFT_TEXTURES;

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

    fClipHeight = (float)(tan(((g_fFieldOfView / 2.0f) * 480.0f) / 640.0f)) * ((float)(fabs(VIEW_Z)) - 32.0f);

    for (i = 0; i < 4; i++) {
        g_rectBox[g_uNumBoxes++] = prectBox[i];

        if (prectBox[i].v1.y > -fClipHeight && prectBox[i].v2.y < fClipHeight) {

            fClipRadius = (float)(cos(g_fFieldOfView / 2.0f)) * (float)(fabs(VIEW_Z));

            if (D3DXVec3Length(&D3DXVECTOR3(prectBox[i].v1.x, 0.0f, prectBox[i].v2.z)) < fClipRadius &&
                D3DXVec3Length(&D3DXVECTOR3(prectBox[i].v2.x, 0.0f, prectBox[i].v2.z)) < fClipRadius) 
            {
                pgear->dwClip[i] = FALSE;
            }
            else {
                pgear->dwClip[i] = TRUE;
            }
        }
    }

    g_uNumGears++;

    pgear->vPos = *pvPos;
    pgear->vDir = *pvDir;
    pgear->fAngularVelocity = fAngularVelocity;
    pgear->fSpin = fAngle;

    // Create attached gears
    for (i = 0; i < 2; i++) {
        pgear->pgearNext[i] = AddAttachedGear(pcw, pgear, i, dwDepth);
    }

    return pgear;
}

//******************************************************************************
PGEAR AddAttachedGear(PCLOCKWORK pcw, PGEAR pgearParent, UINT uCog, DWORD dwDepth) {

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
            for (i = 0; i < g_uNumBoxes && !bCollision; i++) {
                bCollision = GearCollision(&rectBox[j], &g_rectBox[i]);
            }
        }

        if (bCollision) {
            uBlockedDir[uDir] = 1;
            uBlocked++;
        }

    } while (bCollision && uBlocked < 6);

    if (uBlocked < 6) {
        pgear = CreateGear(pcw, csCogSize, fCogOffset, &vPos, &vDir, fRotate * pgearParent->fAngularVelocity * (fGearRatio[pgearParent->csCogSize[uCog]] / fGearRatio[csCogSize[0]]), fAngle2, rectBox, dwDepth+1);
    }

    return pgear;
}

//******************************************************************************
void AddGear(PCLOCKWORK pcw, PGEAR pgear, DWORD dwDepth) {

    UINT i;

    if (g_uNumGears == NUM_GEARS || dwDepth > g_dwMaxDepth) {
        return;
    }

    for (i = 0; i < 2; i++) {

        if (!pgear->pgearNext[i]) {
            pgear->pgearNext[i] = AddAttachedGear(pcw, pgear, i, dwDepth);
        }
        else {
            AddGear(pcw, pgear->pgearNext[i], dwDepth + 1);
        }
    }
}

//******************************************************************************
void RotateGear(PCLOCKWORK pcw, PGEAR pgear, float fTheta) {

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
        RotateGear(pcw, pgear->pgearNext[i], fTheta);
    }
}

//******************************************************************************
void RenderGear(CDevice8* pDevice, PCLOCKWORK pcw, PGEAR pgear) {

    UINT i, j;

    if (!pgear) {
        return;
    }

    // Set the world transform
    pDevice->SetTransform(D3DTS_WORLD, &pgear->mWorld);

    pDevice->SetTexture(0, pcw->pd3dtShaft[pgear->dwShaftTextureIndex]);

#ifndef UNDER_XBOX
    pDevice->SetRenderState(D3DRS_CLIPPING, pgear->dwClip[0]);
#endif // !UNDER_XBOX

    pDevice->SetStreamSource(0, pgear->pd3drShaft, sizeof(VERTEX));
    pDevice->SetIndices(pcw->pd3diShaft, 0);

    // Draw the shaft
    pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pcw->dwShaftVertices,
                    0, pcw->dwShaftIndices / 3);

    for (i = 0; i < 3; i++) {

        pDevice->SetTexture(0, pcw->pd3dtCog[pgear->cog[i].dwTextureIndex]);

#ifndef UNDER_XBOX
        pDevice->SetRenderState(D3DRS_CLIPPING, pgear->dwClip[i+1]);
#endif // !UNDER_XBOX

        pDevice->SetStreamSource(0, pgear->cog[i].pd3drCenter, sizeof(VERTEX));
        pDevice->SetIndices(pgear->cog[i].pd3diCenter, 0);

        pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                    pgear->cog[i].dwCenterVertices, 0, pgear->cog[i].dwCenterIndices / 3);

        pDevice->SetStreamSource(0, pgear->cog[i].pd3drTeeth, sizeof(VERTEX));

        for (j = 0; j < pgear->cog[i].dwSides; j++) {
            pDevice->SetIndices(pcw->pd3diTooth, j * 8);
            pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                    pcw->dwToothVertices, 0, pcw->dwToothIndices / 3);
        }
    }

    for (i = 0; i < 2; i++) {
        RenderGear(pDevice, pcw, pgear->pgearNext[i]);
    }
}

//******************************************************************************
void ReleaseGear(PGEAR pgear) {

    if (pgear) {
        UINT i;
        for (i = 0; i < 2; i++) {
            if (pgear->pgearNext[i]) {
                ReleaseGear(pgear->pgearNext[i]);
            }
        }
        ReleaseVertexBuffer(pgear->pd3drShaft);
        if (pgear->prShaft) {
            MemFree32(pgear->prShaft);
        }
        for (i = 0; i < 3; i++) {
            ReleaseCog(&pgear->cog[i]);
        }
        delete [] pgear;
        pgear = NULL;
    }
}

//******************************************************************************
BOOL CreateClockwork(CDevice8* pDevice, PCLOCKWORK pcw) {

    COGSIZE      csCogSize[3];
    float        fCogOffset[2];
    D3DRECT3D    rectBox[4];
    UINT         i, uIndex;
    TCHAR        szTexture[256];
    D3DVIEWPORT8 viewport;
    float        fBoundHeight;
    
    if (!pcw) {
        return FALSE;
    }

    // ##HACK
    g_pDevice = pDevice;

    memset(pcw, 0, sizeof(CLOCKWORK));

    // Create textures
    for (i = 0; i < NUM_COG_TEXTURES; i++) {
        _stprintf(szTexture, TEXT("cog%d.bmp"), i);
        pcw->pd3dtCog[i] = (CTexture8*)CreateTexture(pDevice, szTexture, D3DFMT_A8R8G8B8);
        if (!pcw->pd3dtCog[i]) {
            ReleaseClockwork(pcw);
            return FALSE;
        }
#ifdef UNDER_XBOX
        pcw->pd3dtCog[i]->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX
    }

    for (i = 0; i < NUM_SHAFT_TEXTURES; i++) {
        _stprintf(szTexture, TEXT("shaft%d.bmp"), i);
        pcw->pd3dtShaft[i] = (CTexture8*)CreateTexture(pDevice, szTexture, D3DFMT_A8R8G8B8);
        if (!pcw->pd3dtShaft[i]) {
            ReleaseClockwork(pcw);
            return FALSE;
        }
#ifdef UNDER_XBOX
        pcw->pd3dtShaft[i]->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX
    }

    // Create a pentagonal shaft cylinder of length 1.0
    if (!CreateCylinder((PVERTEX*)&pcw->prUnitShaft, &pcw->dwShaftVertices, &pcw->pwShaft, 
                        &pcw->dwShaftIndices, 5, SHAFT_RADIUS, 1.0f, CYLCAPS_NONE))
    {
        ReleaseClockwork(pcw);
        return FALSE;
    }

    for (i = 0; i < pcw->dwShaftVertices; i++) {
        pcw->prUnitShaft[i].vPosition.x += 0.5f;
    }

    // Initialize tooth vertices and indices
    pcw->dwToothVertices = 8;
    pcw->dwToothIndices = 30;
    pcw->pwTooth = new WORD[pcw->dwToothIndices];
    if (!pcw->pwTooth) {
        ReleaseClockwork(pcw);
        return FALSE;
    }

    for (i = 0, uIndex = 0; i < 3; i++) {
        pcw->pwTooth[uIndex++] = i * 2;
        pcw->pwTooth[uIndex++] = i * 2 + 2;
        pcw->pwTooth[uIndex++] = i * 2 + 3;
        pcw->pwTooth[uIndex++] = i * 2;
        pcw->pwTooth[uIndex++] = i * 2 + 3;
        pcw->pwTooth[uIndex++] = i * 2 + 1;
    }

    for (i = 1; i < 3; i++) {
        pcw->pwTooth[uIndex++] = 0;
        pcw->pwTooth[uIndex++] = (i + 1) * 2;
        pcw->pwTooth[uIndex++] = i * 2;
    }

    for (i = 1; i < 3; i++) {
        pcw->pwTooth[uIndex++] = 1;
        pcw->pwTooth[uIndex++] = i * 2 + 1;
        pcw->pwTooth[uIndex++] = (i + 1) * 2 + 1;
    }

//    if (!OptimizeIndexedList(pcw->pwTooth, pcw->dwToothIndices)) {
//        ReleaseClockwork(pcw);
//        return false;
//    }

    pcw->pd3diShaft = CreateIndexBuffer(pDevice, pcw->pwShaft, pcw->dwShaftIndices * sizeof(WORD));
    if (!pcw->pd3diShaft) {
        ReleaseClockwork(pcw);
        return FALSE;
    }

    pcw->pd3diTooth = CreateIndexBuffer(pDevice, pcw->pwTooth, pcw->dwToothIndices * sizeof(WORD));
    if (!pcw->pd3diTooth) {
        ReleaseClockwork(pcw);
        return FALSE;
    }

    // Create six bounding boxes to enclose the gears
//    SetRect3D(&g_rectBox[0], &D3DXVECTOR3(-33.0f, -24.0f, -32.0f), &D3DXVECTOR3(-32.0f, 24.0f, 32.0f));
//    SetRect3D(&g_rectBox[1], &D3DXVECTOR3(32.0f, -24.0f, -32.0f), &D3DXVECTOR3(33.0f, 24.0f, 32.0f));
//    SetRect3D(&g_rectBox[2], &D3DXVECTOR3(-32.0f, -25.0f, -32.0f), &D3DXVECTOR3(32.0f, -24.0f, 32.0f));
//    SetRect3D(&g_rectBox[3], &D3DXVECTOR3(-32.0f, 24.0f, -32.0f), &D3DXVECTOR3(32.0f, 25.0f, 32.0f));
//    SetRect3D(&g_rectBox[4], &D3DXVECTOR3(-32.0f, -24.0f, -33.0f), &D3DXVECTOR3(32.0f, 24.0f, -32.0f));
//    SetRect3D(&g_rectBox[5], &D3DXVECTOR3(-32.0f, -24.0f, 32.0f), &D3DXVECTOR3(32.0f, 24.0f, 33.0f));

    pDevice->GetViewport(&viewport);
    fBoundHeight = BOUND_RADIUS * ((float)viewport.Height / (float)viewport.Width);

    SetRect3D(&g_rectBox[0], &D3DXVECTOR3(-BOUND_RADIUS - 1.0f, -fBoundHeight, -BOUND_RADIUS), &D3DXVECTOR3(-BOUND_RADIUS, fBoundHeight, BOUND_RADIUS));
    SetRect3D(&g_rectBox[1], &D3DXVECTOR3(BOUND_RADIUS, -fBoundHeight, -BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS + 1.0f, fBoundHeight, BOUND_RADIUS));
    SetRect3D(&g_rectBox[2], &D3DXVECTOR3(-BOUND_RADIUS, -fBoundHeight - 1.0f, -BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS, -fBoundHeight, BOUND_RADIUS));
    SetRect3D(&g_rectBox[3], &D3DXVECTOR3(-BOUND_RADIUS, fBoundHeight, -BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS, fBoundHeight + 1.0f, BOUND_RADIUS));
    SetRect3D(&g_rectBox[4], &D3DXVECTOR3(-BOUND_RADIUS, -fBoundHeight, -BOUND_RADIUS - 1.0f), &D3DXVECTOR3(BOUND_RADIUS, fBoundHeight, -BOUND_RADIUS));
    SetRect3D(&g_rectBox[5], &D3DXVECTOR3(-BOUND_RADIUS, -fBoundHeight, BOUND_RADIUS), &D3DXVECTOR3(BOUND_RADIUS, fBoundHeight, BOUND_RADIUS + 1.0f));

//float fBound = BOUND_RADIUS;
//    SetRect3D(&g_rectBox[0], &D3DXVECTOR3(-fBound, -fBound, -fBound), &D3DXVECTOR3(-fBound, fBound, fBound));
//    SetRect3D(&g_rectBox[1], &D3DXVECTOR3(fBound, -fBound, -fBound), &D3DXVECTOR3(fBound, fBound, fBound));
//    SetRect3D(&g_rectBox[2], &D3DXVECTOR3(-fBound, -fBound, -fBound), &D3DXVECTOR3(fBound, -fBound, fBound));
//    SetRect3D(&g_rectBox[3], &D3DXVECTOR3(-fBound, fBound, -fBound), &D3DXVECTOR3(fBound, fBound, fBound));
//    SetRect3D(&g_rectBox[4], &D3DXVECTOR3(-fBound, -fBound, -fBound), &D3DXVECTOR3(fBound, fBound, -fBound));
//    SetRect3D(&g_rectBox[5], &D3DXVECTOR3(-fBound, -fBound, fBound), &D3DXVECTOR3(fBound, fBound, fBound));

    g_uNumBoxes = 6;

    g_uNumGears = 0;

    g_uCogTextures = 0;
    g_uShaftTextures = 0;

    g_dwMaxDepth = ((UINT)((float)(log((double)NUM_GEARS)) / (float)(log(2.0))) + 1);

    csCogSize[0] = COGSIZE_SMALL;
    csCogSize[1] = COGSIZE_LARGE;
    csCogSize[2] = COGSIZE_MEDIUM;
    fCogOffset[1] = GEAR_LENGTH;
    fCogOffset[0] = fCogOffset[1] / 2.0f;
    SetRect3D(&rectBox[0], &D3DXVECTOR3(-10.0f - (S_COG_WIDTH / 2.0f), -SHAFT_RADIUS, -SHAFT_RADIUS), &D3DXVECTOR3(-10.0f + fCogOffset[1] + (M_COG_WIDTH / 2.0f), SHAFT_RADIUS, SHAFT_RADIUS));
    SetRect3D(&rectBox[1], &D3DXVECTOR3(-10.0f - (S_COG_WIDTH / 2.0f), -(S_COG_RADIUS + TOOTH_HEIGHT), -(S_COG_RADIUS + TOOTH_HEIGHT)), &D3DXVECTOR3(-10.0f + (S_COG_WIDTH / 2.0f), (S_COG_RADIUS + TOOTH_HEIGHT), (S_COG_RADIUS + TOOTH_HEIGHT)));
    SetRect3D(&rectBox[2], &D3DXVECTOR3(-10.0f + fCogOffset[0] - (L_COG_WIDTH / 2.0f), -(L_COG_RADIUS + TOOTH_HEIGHT), -(L_COG_RADIUS + TOOTH_HEIGHT)), &D3DXVECTOR3(-10.0f + fCogOffset[0] + (L_COG_WIDTH / 2.0f), (L_COG_RADIUS + TOOTH_HEIGHT), (L_COG_RADIUS + TOOTH_HEIGHT)));
    SetRect3D(&rectBox[3], &D3DXVECTOR3(-10.0f + fCogOffset[1] - (M_COG_WIDTH / 2.0f), -(M_COG_RADIUS + TOOTH_HEIGHT), -(M_COG_RADIUS + TOOTH_HEIGHT)), &D3DXVECTOR3(-10.0f + fCogOffset[1] + (M_COG_WIDTH / 2.0f), (M_COG_RADIUS + TOOTH_HEIGHT), (M_COG_RADIUS + TOOTH_HEIGHT)));

    pcw->pgearRoot = CreateGear(pcw, csCogSize, fCogOffset, &D3DXVECTOR3(-10.0f, 0.0f, 0.0f), &D3DXVECTOR3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f, rectBox);
    if (!pcw->pgearRoot) {
        ReleaseClockwork(pcw);
        return FALSE;
    }

//DebugString(TEXT("Created gears: %d"), g_uNumGears);

    for (g_dwMaxDepth++; g_uNumGears < NUM_GEARS && g_dwMaxDepth <= 15; g_dwMaxDepth++) {
        AddGear(pcw, pcw->pgearRoot);
    }

//DebugString(TEXT("Created gears: %d"), g_uNumGears);

    if (g_uNumGears != NUM_GEARS) {
        DebugString(TEXT("WARNING: The requested number of gears (%d) could ")
                    TEXT("not be created: only %d gears fit into the scene.\r\n")
                    TEXT("Please lower the number of gears requested or change ")
                    TEXT("the seed for the pseudo-random number generator to\r\n")
                    TEXT("obtain a different gear set"), NUM_GEARS, g_uNumGears);
    }

    return TRUE;
}

//******************************************************************************
void UpdateClockwork(PCLOCKWORK pcw, float fTimeDilation) {

    RotateGear(pcw, pcw->pgearRoot, M_PI / 40.0f * fTimeDilation);
}

//******************************************************************************
void RenderClockwork(CDevice8* pDevice, PCLOCKWORK pcw) {

    RenderGear(pDevice, pcw, pcw->pgearRoot);
}

//******************************************************************************
void ReleaseClockwork(PCLOCKWORK pcw) {

    UINT i;

    ReleaseIndexBuffer(pcw->pd3diShaft);
    ReleaseIndexBuffer(pcw->pd3diTooth);
    ReleaseCylinder(pcw->prUnitShaft, pcw->pwShaft);
    if (pcw->pwTooth) {
        delete [] pcw->pwTooth;
    }

    for (i = 0; i < NUM_COG_TEXTURES; i++) {
        if (pcw->pd3dtCog[i]) {
            ReleaseTexture(pcw->pd3dtCog[i]);
        }
    }
    for (i = 0; i < NUM_SHAFT_TEXTURES; i++) {
        if (pcw->pd3dtShaft[i]) {
            ReleaseTexture(pcw->pd3dtShaft[i]);
        }
    }

    ReleaseGear(pcw->pgearRoot);
    memset(pcw, 0, sizeof(CLOCKWORK));
}

//******************************************************************************
// Create a cylinder oriented along the x axis
BOOL CreateCylinder(PVERTEX* pprVertices, LPDWORD pdwNumVertices, 
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

    fDelta = M_2PI / (float)(dwSides);

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

    pwIndices = new WORD[dwNumIndices];
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

//    if (!OptimizeIndexedList(pwIndices, dwNumIndices)) {
//        MemFree32(prVertices);
//        delete [] pwIndices;
//        return false;
//    }

    *pprVertices = prVertices;
    *pdwNumVertices = dwNumVertices;
    *ppwIndices = pwIndices;
    *pdwNumIndices = dwNumIndices;

    return TRUE;
}

//******************************************************************************
void ReleaseCylinder(PVERTEX prVertices, LPWORD pwIndices) {

    if (prVertices) {
        MemFree32(prVertices);
    }

    if (pwIndices) {
        delete [] pwIndices;
    }
}

//******************************************************************************
void SetRect3D(PD3DRECT3D prect, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2) {
    
    prect->v1 = *pv1;
    prect->v2 = *pv2;
}

//******************************************************************************
BOOL GearCollision(PD3DRECT3D prectBox1, PD3DRECT3D prectBox2) {

    return !((prectBox1->v1.x >= prectBox2->v2.x || prectBox1->v2.x <= prectBox2->v1.x) ||
             (prectBox1->v1.y >= prectBox2->v2.y || prectBox1->v2.y <= prectBox2->v1.y) ||
             (prectBox1->v1.z >= prectBox2->v2.z || prectBox1->v2.z <= prectBox2->v1.z));
}



