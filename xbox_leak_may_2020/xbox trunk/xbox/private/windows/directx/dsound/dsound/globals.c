/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       globals.c
 *  Content:    Externally exposed DirectSound global variables.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/22/01    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

#ifdef MCPX_BOOT_LIB
#define DSGLOBAL const
#else // MCPX_BOOT_LIB
#define DSGLOBAL const __declspec(selectany)
#endif // MCPX_BOOT_LIB

DSGLOBAL DSMIXBINVOLUMEPAIR DirectSoundDefaultMixBinVolumePairs_Mono[] = { DSMIXBINVOLUMEPAIRS_DEFAULT_MONO };
DSGLOBAL DSMIXBINVOLUMEPAIR DirectSoundDefaultMixBinVolumePairs_Stereo[] = { DSMIXBINVOLUMEPAIRS_DEFAULT_STEREO };
DSGLOBAL DSMIXBINVOLUMEPAIR DirectSoundDefaultMixBinVolumePairs_4Channel[] = { DSMIXBINVOLUMEPAIRS_DEFAULT_4CHANNEL };
DSGLOBAL DSMIXBINVOLUMEPAIR DirectSoundDefaultMixBinVolumePairs_6Channel[] = { DSMIXBINVOLUMEPAIRS_DEFAULT_6CHANNEL };
DSGLOBAL DSMIXBINVOLUMEPAIR DirectSoundRequiredMixBinVolumePairs_3D[] = { DSMIXBINVOLUMEPAIRS_REQUIRED_3D };
DSGLOBAL DSMIXBINVOLUMEPAIR DirectSoundDefaultMixBinVolumePairs_3D[] = { DSMIXBINVOLUMEPAIRS_DEFAULT_3D };

DSGLOBAL DSMIXBINS DirectSoundDefaultMixBins_Mono = 
{
    /* dwMixBinCount        */ NUMELMS(DirectSoundDefaultMixBinVolumePairs_Mono),
    /* lpMixBinVolumePairs  */ DirectSoundDefaultMixBinVolumePairs_Mono
};
        
DSGLOBAL DSMIXBINS DirectSoundDefaultMixBins_Stereo = 
{
    /* dwMixBinCount        */ NUMELMS(DirectSoundDefaultMixBinVolumePairs_Stereo),
    /* lpMixBinVolumePairs  */ DirectSoundDefaultMixBinVolumePairs_Stereo
};
        
DSGLOBAL DSMIXBINS DirectSoundDefaultMixBins_4Channel = 
{
    /* dwMixBinCount        */ NUMELMS(DirectSoundDefaultMixBinVolumePairs_4Channel),
    /* lpMixBinVolumePairs  */ DirectSoundDefaultMixBinVolumePairs_4Channel
};
        
DSGLOBAL DSMIXBINS DirectSoundDefaultMixBins_6Channel = 
{
    /* dwMixBinCount        */ NUMELMS(DirectSoundDefaultMixBinVolumePairs_6Channel),
    /* lpMixBinVolumePairs  */ DirectSoundDefaultMixBinVolumePairs_6Channel
};
        
DSGLOBAL DSMIXBINS DirectSoundRequiredMixBins_3D = 
{
    /* dwMixBinCount        */ NUMELMS(DirectSoundRequiredMixBinVolumePairs_3D),
    /* lpMixBinVolumePairs  */ DirectSoundRequiredMixBinVolumePairs_3D
};
        
DSGLOBAL DSMIXBINS DirectSoundDefaultMixBins_3D = 
{
    /* dwMixBinCount        */ NUMELMS(DirectSoundDefaultMixBinVolumePairs_3D),
    /* lpMixBinVolumePairs  */ DirectSoundDefaultMixBinVolumePairs_3D
};

#ifndef MCPX_BOOT_LIB
        
DSGLOBAL DS3DBUFFER DirectSoundDefault3DBuffer =
{
    /* dwSize               */ sizeof(DS3DBUFFER),
    /* vPosition            */ { DS3D_DEFAULTPOSITION_X, DS3D_DEFAULTPOSITION_Y, DS3D_DEFAULTPOSITION_Z },
    /* vVelocity            */ { DS3D_DEFAULTVELOCITY_X, DS3D_DEFAULTVELOCITY_Y, DS3D_DEFAULTVELOCITY_Z },
    /* dwInsideConeAngle    */ DS3D_DEFAULTCONEANGLE,
    /* dwOutsideConeAngle   */ DS3D_DEFAULTCONEANGLE,
    /* vConeOrientation     */ { DS3D_DEFAULTCONEORIENT_X, DS3D_DEFAULTCONEORIENT_Y, DS3D_DEFAULTCONEORIENT_Z },
    /* lConeOutsideVolume   */ DS3D_DEFAULTCONEOUTSIDEVOLUME,
    /* flMinDistance        */ DS3D_DEFAULTMINDISTANCE,
    /* flMaxDistance        */ DS3D_DEFAULTMAXDISTANCE,
    /* dwMode               */ DS3DMODE_NORMAL,
    /* flDistanceFactor     */ DS3D_DEFAULTDISTANCEFACTOR,                                                         
    /* flRolloffFactor      */ DS3D_DEFAULTROLLOFFFACTOR,                                                          
    /* flDopplerFactor      */ DS3D_DEFAULTDOPPLERFACTOR                                                           
};

DSGLOBAL DSI3DL2BUFFER DirectSoundDefaultI3DL2Buffer =
{
    /* lDirect              */ DSI3DL2BUFFER_DEFAULTDIRECT,
    /* lDirectHF            */ DSI3DL2BUFFER_DEFAULTDIRECTHF,
    /* lRoom                */ DSI3DL2BUFFER_DEFAULTROOM,
    /* lRoomHF              */ DSI3DL2BUFFER_DEFAULTROOMHF,
    /* flRoomRolloffFactor  */ DSI3DL2BUFFER_DEFAULTROOMROLLOFFFACTOR,
    /* Obstruction          */ { DSI3DL2BUFFER_DEFAULTOBSTRUCTION, DSI3DL2BUFFER_DEFAULTOBSTRUCTIONLFRATIO },
    /* Occlusion            */ { DSI3DL2BUFFER_DEFAULTOCCLUSION, DSI3DL2BUFFER_DEFAULTOCCLUSIONLFRATIO}
};

DSGLOBAL DS3DLISTENER DirectSoundDefault3DListener =
{
    /* dwSize               */ sizeof(DS3DLISTENER),                                                               
    /* vPosition            */ { DS3D_DEFAULTPOSITION_X, DS3D_DEFAULTPOSITION_Y, DS3D_DEFAULTPOSITION_Z },         
    /* vVelocity            */ { DS3D_DEFAULTVELOCITY_X, DS3D_DEFAULTVELOCITY_Y, DS3D_DEFAULTVELOCITY_Z },         
    /* vOrientFront         */ { DS3D_DEFAULTORIENTFRONT_X, DS3D_DEFAULTORIENTFRONT_Y, DS3D_DEFAULTORIENTFRONT_Z },
    /* vOrientTop           */ { DS3D_DEFAULTORIENTTOP_X, DS3D_DEFAULTORIENTTOP_Y, DS3D_DEFAULTORIENTTOP_Z },      
    /* flDistanceFactor     */ DS3D_DEFAULTDISTANCEFACTOR,                                                         
    /* flRolloffFactor      */ DS3D_DEFAULTROLLOFFFACTOR,                                                          
    /* flDopplerFactor      */ DS3D_DEFAULTDOPPLERFACTOR                                                           
};

DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Default = { DSI3DL2_ENVIRONMENT_PRESET_DEFAULT };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Generic = { DSI3DL2_ENVIRONMENT_PRESET_GENERIC };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_PaddedCell = { DSI3DL2_ENVIRONMENT_PRESET_PADDEDCELL };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Room = { DSI3DL2_ENVIRONMENT_PRESET_ROOM };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Bathroom = { DSI3DL2_ENVIRONMENT_PRESET_BATHROOM };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_LivingRoom = { DSI3DL2_ENVIRONMENT_PRESET_LIVINGROOM };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_StoneRoom = { DSI3DL2_ENVIRONMENT_PRESET_STONEROOM };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Auditorium = { DSI3DL2_ENVIRONMENT_PRESET_AUDITORIUM };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_ConcertHall = { DSI3DL2_ENVIRONMENT_PRESET_CONCERTHALL };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Cave = { DSI3DL2_ENVIRONMENT_PRESET_CAVE };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Arena = { DSI3DL2_ENVIRONMENT_PRESET_ARENA };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Hangar = { DSI3DL2_ENVIRONMENT_PRESET_HANGAR };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_CarpetedHallway = { DSI3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Hallway = { DSI3DL2_ENVIRONMENT_PRESET_HALLWAY };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_StoneCorridor = { DSI3DL2_ENVIRONMENT_PRESET_STONECORRIDOR };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Alley = { DSI3DL2_ENVIRONMENT_PRESET_ALLEY };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Forest = { DSI3DL2_ENVIRONMENT_PRESET_FOREST };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_City = { DSI3DL2_ENVIRONMENT_PRESET_CITY };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Mountains = { DSI3DL2_ENVIRONMENT_PRESET_MOUNTAINS };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Quarry = { DSI3DL2_ENVIRONMENT_PRESET_QUARRY };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Plain = { DSI3DL2_ENVIRONMENT_PRESET_PLAIN };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_ParkingLot = { DSI3DL2_ENVIRONMENT_PRESET_PARKINGLOT };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_SewerPipe = { DSI3DL2_ENVIRONMENT_PRESET_SEWERPIPE };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_Underwater = { DSI3DL2_ENVIRONMENT_PRESET_UNDERWATER };
DSGLOBAL DSI3DL2LISTENER DirectSoundI3DL2ListenerPreset_NoReverb = { DSI3DL2_ENVIRONMENT_PRESET_NOREVERB };

DSGLOBAL GUID KSDATAFORMAT_SUBTYPE_PCM = { WAVE_FORMAT_PCM, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
DSGLOBAL GUID KSDATAFORMAT_SUBTYPE_XBOX_ADPCM = { WAVE_FORMAT_XBOX_ADPCM, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

DWORD g_dwDirectSoundDebugLevel = DPFLVL_DEFAULT;
DWORD g_dwDirectSoundDebugBreakLevel = DPFLVL_DEFAULT_BREAK;
BOOL g_fDirectSoundDebugBreak = FALSE;

DWORD g_dwDirectSoundPoolMemoryUsage = 0;
DWORD g_dwDirectSoundPhysicalMemoryUsage = 0;

DWORD g_dwDirectSoundPhysicalMemorySlop = 0;
DWORD g_dwDirectSoundPhysicalMemoryRecovered = 0;

DWORD g_dwDirectSoundFree2dVoices = 0;
DWORD g_dwDirectSoundFree3dVoices = 0;
DWORD g_dwDirectSoundFreeBufferSGEs = 0;

DWORD g_dwDirectSoundOverrideSpeakerConfig = DSSPEAKER_USE_DEFAULT;

BOOL g_fDirectSoundInFinalRelease = FALSE;
BOOL g_fDirectSoundDisableBusyWaitWarning = FALSE;
BOOL g_fDirectSoundI3DL2Overdelay = FALSE;

INITIALIZED_CRITICAL_SECTION(g_DirectSoundCriticalSection);

#endif // MCPX_BOOT_LIB

