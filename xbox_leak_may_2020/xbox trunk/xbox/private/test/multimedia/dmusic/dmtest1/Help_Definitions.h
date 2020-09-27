/********************************************************************************
Microsoft Windows
Copyright (C) Microsoft Corporation, 1999 - 2000
File:
    tdmapthDefinitions.h

History:
    04/15/2000 - danhaff - Created
    05/04/2000 - danhaff - split into separate file.
    08/29/2000 - danhaff - Removed instances of DYNAMIC_APATH_ENV3D

Purpose:
    * Contains the definitions of certain default structs which may be
      compared against for default-ness, etc.        

    * To compare PATH_DEFINITIONs, BUFFER_DEFINITIONs, and DMO_DEFINITIONs,
      use the VerifyData functions defined in tdmapthHelp.h

    * To compare standard structures including DSFX settings and 3DBuffer/Listener
      data, use the Compare_Type functions in dmth.h.
********************************************************************************/

#pragma once

#include "Help_VerifyPath.h"


//----------------- STANDARD AudioPath Defaults ---------------------
//The default data for the DMUS_APATH_SHARED_STEREOPLUSREVERB.
extern  PATH_DEFINITION     g_DefaultPathStereoPlusReverb;
extern  PATH_DEFINITION     g_DefaultPath3D;
extern  PATH_DEFINITION     g_DefaultPathMono;
extern  PATH_DEFINITION     g_DefaultPathStereo;

//----------------- DMO Defaults ---------------------
//The default parameters for the WavesReverb DMO.
//extern DSFXWavesReverb     g_dnDefaultWavesReverb;
//extern DSFXI3DL2Reverb     g_dnDefaultFXI3DL2Reverb;

//----------------- Buffer Defaults-----------------
//This is what you should see when you pull it out of the standard audio path.
//extern BUFFER_DEFINITION    g_dnDefaultPrimaryBuffer;


//This is what you should see when you pull a 3D buffer
//  out of the standard audio path.
extern BUFFER_DEFINITION    g_dnDefault3DBuffer;


//----------------- Misc Defaults _-----------------
//The default params of a 3D Buffer.
extern DS3DBUFFER           g_dnDefault3DBufferParams;

//The default params of a 3D Listener.
//extern DS3DLISTENER         g_dnDefault3DListenerParams;


//----------------- NONSTANDARD Audiopath Defaults -------------------
/*
extern PATH_DEFINITION g_DefsSinkinDMOsAMin;
extern PATH_DEFINITION g_DefsMixinDMOsAMin;
extern PATH_DEFINITION g_DefsSinkinDMOsAMax;
extern PATH_DEFINITION g_DefsMixinDMOsAMax;
extern PATH_DEFINITION g_DefsSinkinDMOsBMin;
extern PATH_DEFINITION g_DefsMixinDMOsBMin;
extern PATH_DEFINITION g_DefsSinkinDMOsBMax;
extern PATH_DEFINITION g_DefsMixinDMOsBMax;
extern PATH_DEFINITION g_DefsSinkinDMOsCMin;
extern PATH_DEFINITION g_DefsMixinDMOsCMin;
extern PATH_DEFINITION g_DefsSinkinDMOsCMax;
extern PATH_DEFINITION g_DefsMixinDMOsCMax;

extern PATH_DEFINITION g_Defs2_3DAlg;
extern PATH_DEFINITION g_Defs2_3DMuteAtMax;
extern PATH_DEFINITION g_Defs2_3DParams;
extern PATH_DEFINITION g_Defs2_DopplerNo3D;
extern PATH_DEFINITION g_Defs2_DopplerWith3D;
extern PATH_DEFINITION g_Defs2_Mono3D;
extern PATH_DEFINITION g_Defs2_Stereo3D;
extern PATH_DEFINITION g_Defs2_NoVolume;
extern PATH_DEFINITION g_Defs2_NULLGUID;
extern PATH_DEFINITION g_Defs2_OneBussTwoChannels;
extern PATH_DEFINITION g_Defs2_PanLeft;
extern PATH_DEFINITION g_Defs2_PanRight;
*/
