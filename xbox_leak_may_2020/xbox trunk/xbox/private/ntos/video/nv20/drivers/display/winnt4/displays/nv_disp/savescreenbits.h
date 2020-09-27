#ifndef _SAVESCREENBITS_H
#define _SAVESCREENBITS_H
//******************************Module*Header***********************************
// Module Name: savescreenbits.h
//
//  This module contains the DrvSaveScreenBits realization
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

ULONG_PTR APIENTRY DrvSaveScreenBits(
    SURFOBJ  *pso,
    ULONG    iMode,
    ULONG_PTR ident,
    RECTL    *prcl);

BOOL bSaveScreenBitsGarbageCollection(
    PPDEV ppdev,
    ULONG ulKeep);



#endif // _SAVESCREENBITS_H
