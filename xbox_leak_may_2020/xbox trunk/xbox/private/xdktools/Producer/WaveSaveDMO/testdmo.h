//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1998  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// testdmo.h
//

// A custom interface to allow the user to adjust the dump parameters.
// It defines the interface between the user interface component (the
// property sheet) and the filter itself.
// This interface is exported by the code in dump.cpp and is used
// by the code in dumpprop.cpp.

#ifndef __TESTDMO_H__
#define __TESTDMO_H__

#include <mmsystem.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_DUMP_FILE L".\\dump.wav"

//
// IDump
//
DECLARE_INTERFACE_(IDump, IUnknown)
{
    STDMETHOD(SetDumpWave)	(THIS_ WCHAR *wszDumpWave) PURE;
    STDMETHOD(GetDumpWave)	(THIS_ WCHAR *wszDumpWave) PURE;
    STDMETHOD(GetOverwrite)	(THIS_ BOOL *pbOverwrite) PURE;
    STDMETHOD(SetOverwrite)	(THIS_ BOOL bOverwrite) PURE;
    STDMETHOD(Start)		(THIS) PURE;
    STDMETHOD(Stop)			(THIS) PURE;
};


// This GUID also exists in the Audiopath Designer, in audiopath.cpp
// {2D6D1411-DCD7-45e7-ADDE-ACAC85A2425D}
DEFINE_GUID(GUID_DSFX_STANDARD_DUMP, 0x2d6d1411, 0xdcd7, 0x45e7, 0xad, 0xde, 0xac, 0xac, 0x85, 0xa2, 0x42, 0x5d);


// {E13DCE68-47D1-4b77-AEBA-BFAC78F7AA17}
DEFINE_GUID(IID_IDump, 0xe13dce68, 0x47d1, 0x4b77, 0xae, 0xba, 0xbf, 0xac, 0x78, 0xf7, 0xaa, 0x17);
// {E0CF68ED-0E3C-4f8c-ADE1-ECDC04B57907}
DEFINE_GUID(CLSID_DirectSoundPropDump, 0xe0cf68ed, 0xe3c, 0x4f8c, 0xad, 0xe1, 0xec, 0xdc, 0x4, 0xb5, 0x79, 0x7);


#ifdef __cplusplus
}
#endif

#endif // __TESTDMO_H__
