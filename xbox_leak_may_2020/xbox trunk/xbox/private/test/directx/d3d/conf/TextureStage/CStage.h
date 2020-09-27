//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 1999.
//
// CStage.h
//
// CStage class - Contains all information for a texture blend stage
//
// History: 5/15/99 Jeff Vezina     - Created
//
//////////////////////////////////////////////////////////////////////

#ifndef __CSTAGE_H__
#define __CSTAGE_H__

//#include "cd3dtest.h"
//#include "CImageloader.h"

//USETESTFRAME

class CStage  
{
public:
    CStage();
    virtual ~CStage();

    void SetStates(DWORD dwStage);                  // Sets all states for the specified stage
    void WriteToLog(UINT uStage);                   // Write to log current stage states
    void WriteToStatus(UINT uStage);                // Write to status current stage states

    void LogStage(BOOL bColor, DWORD dwStage);

    CStage *Create();                               // Creates a CStage at the end of the list
    void LoadTexture();                             // Creates texture, releases previous one
    UINT Count();                                   // Count the number of CStages in the list
    CStage *GetStageList(int n);                    // Get a CStage in the list

    DWORD m_dwColorOp,                              // D3D texture color operand
            m_dwColorArg1,                          // D3D texture color argument 1
            m_dwColorArg2,                          // D3D texture color argument 2
            m_dwAlphaOp,                            // D3D texture alpha operand
            m_dwAlphaArg1,                          // D3D texture alpha argument 1
            m_dwAlphaArg2;                          // D3D texture alpha argument 2

    DWORD m_rgdwColor[4];                           // Corner colors to use when creating texture
    CTexture8 *m_paTexture;

    CStage *m_paNext;                               // Next CStage in the list
};

#endif