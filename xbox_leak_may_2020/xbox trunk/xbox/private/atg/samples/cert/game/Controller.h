//-----------------------------------------------------------------------------
// File: Controller.h
//
// Desc: Handles game input
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_CONTROLLER_H
#define TECH_CERT_GAME_CONTROLLER_H

#include "Common.h"
#include <XbInput.h>




//-----------------------------------------------------------------------------
// Name: class Controller
// Desc: Handles game input
//-----------------------------------------------------------------------------
class Controller
{

    static INT m_iPrimaryController;
    static BOOL m_bIsThumbstick1Calibrated;

    static FLOAT m_fMaxX1;
    static FLOAT m_fMinX1;
    static FLOAT m_fMaxY1;
    static FLOAT m_fMinY1;

public:

    static const XBGAMEPAD* GetPrimaryController();
    static VOID CheckCalibration( const XBGAMEPAD* );
    static VOID SetVibration( const XBGAMEPAD*, FLOAT fLeft, FLOAT fRight );
    static BOOL IsAnyButtonActive();
    static BOOL IsAnyButtonActive( const XBGAMEPAD* );
    static VOID ClearPrimaryController();
    static BOOL HavePrimaryController();
    static BOOL AnyAdded();

};




#endif // TECH_CERT_GAME_CONTROLLER_H
