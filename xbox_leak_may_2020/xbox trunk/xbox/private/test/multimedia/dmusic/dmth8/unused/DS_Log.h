//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1994  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  utils.h
//
//  Description:
//		header file for the utilities used by tDSound
//
//	History:
//		12/08/97	MTang			Started it
//==========================================================================;
#pragma once

void Log_DS3DAlgorithm (int, LPGUID);
void Log_DSErr (TCHAR *, HRESULT, int);
void Log_DSCooperativeLevel (int, DWORD);
void Log_GUID (int, LPGUID);
void Log_GUID (int, REFGUID);
void Log_DSCaps (int, LPDSCAPS);
void Log_DSBCaps (int, LPDSBCAPS);
void Log_DSCBCaps (int, LPDSCBCAPS);
void Log_SpeakerConfig (int, DWORD);
void Log_WaveFormatEx(int, LPCWAVEFORMATEX);
void Log_DSBufferDesc(int, LPCDSBUFFERDESC);
void Log_DSBLockFlags(int, DWORD);
void Log_DSCBLockFlags(int, DWORD);
void Log_DSBPlayFlags(int, DWORD);
void Log_DS3DLAllParameters(int, LPCDS3DLISTENER);
void Log_DS3DApplyMode (int, DWORD);
void Log_DS3DBMode (int, DWORD);
void Log_D3DVector(int, const struct _D3DVECTOR *);
void Log_DS3DBAllParameters(int, LPCDS3DBUFFER);
void Log_DSCCaps (int, LPDSCCAPS);
void Log_DSCBufferDesc (int, LPCDSCBUFFERDESC);
void Log_DSBStatus (int, DWORD);
void Log_DSCBStatus (int, DWORD);
void Log_DSCBStartFlags (int, DWORD);
void Log_DSNPositionNotifies (int, DWORD, LPCDSBPOSITIONNOTIFY);
void Log_DSBCapsFlags(int, DWORD);
void Log_DSCBCapsFlags(int, DWORD);
void Log_wFormatTag (int, WORD);

void tstLof(int, TCHAR *, double);		

void DPF_DS3DAlgorithm (int, LPGUID);
void DPF_DSErr (TCHAR *, HRESULT, int);
void DPF_DSCooperativeLevel (int, DWORD);
void DPF_GUID (int, LPGUID);
void DPF_GUID (int, REFGUID);
void DPF_DSCaps (int, LPDSCAPS);
void DPF_DSBCaps (int, LPDSBCAPS);
void DPF_DSCBCaps (int, LPDSCBCAPS);
void DPF_SpeakerConfig (int, DWORD);
void DPF_WaveFormatEx(int, LPCWAVEFORMATEX);
void DPF_DSBufferDesc(int, LPCDSBUFFERDESC);
void DPF_DSBLockFlags(int, DWORD);
void DPF_DSCBLockFlags(int, DWORD);
void DPF_DSBPlayFlags(int, DWORD);
void DPF_DS3DLAllParameters(int, LPCDS3DLISTENER);
void DPF_DS3DApplyMode (int, DWORD);
void DPF_DS3DBMode (int, DWORD);
void DPF_D3DVector(int, const struct _D3DVECTOR *);
void DPF_DS3DBAllParameters(int, LPCDS3DBUFFER);
void DPF_DSCCaps (int, LPDSCCAPS);
void DPF_DSCBufferDesc (int, LPCDSCBUFFERDESC);
void DPF_DSBStatus (int, DWORD);
void DPF_DSCBStatus (int, DWORD);
void DPF_DSCBStartFlags (int, DWORD);
void DPF_DSNPositionNotifies (int, DWORD, LPCDSBPOSITIONNOTIFY);
void DPF_DSBCapsFlags(int, DWORD);
void DPF_DSCBCapsFlags(int, DWORD);
void DPF_wFormatTag (int, WORD);

void DPF_tstLof(int, TCHAR *, double);	

LRESULT _cdecl DummyLog (DWORD, LPSTR, ...);	
