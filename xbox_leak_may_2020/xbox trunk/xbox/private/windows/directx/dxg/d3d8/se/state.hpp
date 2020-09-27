/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       state.hpp
 *  Content:    header file for state.cpp
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

extern BYTE g_SlotMapping[];

HRESULT PresentationParametersParameterCheck(
    D3DPRESENT_PARAMETERS* pPresentationParams);

VOID SetState();

VOID UpdateProjectionViewportTransform();

VOID LazySetCombiners(CDevice* pDevice);

VOID VerifyHeaderFileEncodings();

extern BOOL ValidCombinerInputs(DWORD val, char *err);
extern BOOL ValidAlphaOutputs(DWORD val, char *err);
extern BOOL ValidRGBOutputs(DWORD val, char *err);
extern BOOL ValidFinalCombinerABCD(DWORD val, char *err);
extern BOOL ValidFinalCombinerEFG(DWORD val, char *err);
extern BOOL ValidTextureModes(DWORD val, char *err);
extern BOOL ValidInputTexture(DWORD val, char *err);
extern BOOL ValidCombinerCount(DWORD val, char *err);
extern BOOL ValidCompareMode(DWORD val, char *err);
extern BOOL ValidDotMapping(DWORD val, char *err);

// Prototypes for state-setting routines common to multiple APIs:

PPUSH FASTCALL CommonSetTextureBumpEnv(CDevice* pDevice, PPUSH pPush);

PPUSH FASTCALL CommonSetViewport(CDevice* pDevice, PPUSH pPush);

PPUSH FASTCALL CommonSetControl0(CDevice* pDevice, PPUSH pPush);

PPUSH FASTCALL CommonSetOccludeZStencilEn(CDevice* pDevice, PPUSH pPush);

VOID FASTCALL CommonSetPassthroughProgram(CDevice* pDevice);

VOID FASTCALL CommonSetAntiAliasingControl(CDevice* pDevice);

VOID FASTCALL CommonSetDebugRegisters();

} // end namespace
