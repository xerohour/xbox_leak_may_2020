/**************************************************************************************************************
**************************************************************************************************************/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <stdio.h>
#include <io.h>
#include <objbase.h>
#include <assert.h>
#include "detours.h"
#include <tchar.h>
#include <initguid.h>
#include <dmusici.h>
#include <string.h>
#include "dmscriptautguids.h"
#include "macros.h"
#include "helpers.h"

#define APP_NAME "AUDANALYZE"
#define ASSERT assert

DEFINE_GUID(CLSID_DirectMusicIgnoreThisOne,0xaee78e4d,0x8818,0x4020,0xAA,0x5D,0x99,0x77,0x56,0xAF,0x53,0xfb);
DEFINE_GUID(CLSID_DirectMusicFileStream, 0xf12f2c7d, 0x3651, 0x486f, 0xb9, 0xfa, 0x16, 0xe1, 0x1d, 0x15, 0x24, 0xfd);
DEFINE_GUID(CLSID_DirectMusicStreamStream, 0xf34feac1, 0xe3af, 0x49ad, 0x83, 0x97, 0xb, 0xed, 0x32, 0x3e, 0xf9, 0x6b);
DEFINE_GUID(CLSID_DirectMusicMemStream, 0x75ccb447, 0x8d3f, 0x4154, 0xab, 0xad, 0x59, 0x60, 0xae, 0xd4, 0xba, 0x63);


DEFINE_GUID(CLSID_DirectMusicAudioVBScriptIgnore, 0x4ee17959, 0x931e, 0x49e4, 0xa2, 0xc6, 0x97, 0x7e, 0xcf, 0x36, 0x28, 0xf3);


extern DWORD dwCLSIDTableSize;



