/********************************************************************************
FILE:
    tdmapthHelpDefs2.cpp

PURPOSE:
    This contains definitions for audiopaths containing a bunch of different
    effects settings, namely the minimum and maximum possible.

    Defs_SinkinDMOsAMin.aud
    Defs_MixinDMOsAMin.aud
    Defs_SinkinDMOsAMax.aud
    Defs_MixinDMOsAMax.aud
    B...
    C...

HISTORY:
  09/21/2000   danhaff - Created.
********************************************************************************/


#include "globals.h"
#include "Help_Definitions.h"

/*

// ********************  DEFAULT 3D BUFFER DEFINITION ********************
DS3DBUFFER g_dnDefault3DBufferParams = 
{
    sizeof(DS3DBUFFER),
    {0, 0, 0},
    {0, 0, 0},
    DS3D_DEFAULTCONEANGLE,
    DS3D_DEFAULTCONEANGLE,
    {0, 0, 1},
    DS3D_DEFAULTCONEOUTSIDEVOLUME,
    DS3D_DEFAULTMINDISTANCE,
    DS3D_DEFAULTMAXDISTANCE,
    DS3DMODE_NORMAL
};

// ********************  DEFAULT 3D LISTENER DEFINITION ********************
DS3DLISTENER g_dnDefault3DListenerParams= 
{
    sizeof(DS3DLISTENER),
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 0},
    DS3D_DEFAULTDISTANCEFACTOR,
    DS3D_DEFAULTROLLOFFFACTOR,
    DS3D_DEFAULTDOPPLERFACTOR
};

// ********************  DEFAULT DSFXI3DL2Reverb ********************
DSFXI3DL2Reverb g_dnDefaultFXI3DL2Reverb =
{
    DSFX_I3DL2REVERB_ROOM_DEFAULT,              //lRoom
    DSFX_I3DL2REVERB_ROOMHF_DEFAULT,            //lRoomHF
    DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_DEFAULT, //flRoomRolloffFactor
    DSFX_I3DL2REVERB_DECAYTIME_DEFAULT,         //flDecayTime
    DSFX_I3DL2REVERB_DECAYHFRATIO_DEFAULT,      //flDecayHFRatio
    DSFX_I3DL2REVERB_REFLECTIONS_DEFAULT,       //lReflections
    DSFX_I3DL2REVERB_REFLECTIONSDELAY_DEFAULT,  //flReflectionsDelay
    DSFX_I3DL2REVERB_REVERB_DEFAULT,            //lReverb
    DSFX_I3DL2REVERB_REVERBDELAY_DEFAULT,       //flReverbDelay
    DSFX_I3DL2REVERB_DIFFUSION_DEFAULT,         //flDiffusion
    DSFX_I3DL2REVERB_DENSITY_DEFAULT,           //flDensity
    DSFX_I3DL2REVERB_HFREFERENCE_DEFAULT        //flHFReference
};



// ********************  DEFAULT DSFXI3DL2Send ********************
//DSFXSend g_dnDefaultFXSend=
//{
//    0                               //lVolume
//};


// ********************  DEFAULT Waves Reverb ********************
DSFXWavesReverb g_dnDefaultWavesReverb= 
{
    DSFX_WAVESREVERB_INGAIN_DEFAULT,
    DSFX_WAVESREVERB_REVERBMIX_DEFAULT,
    DSFX_WAVESREVERB_REVERBTIME_DEFAULT,
    DSFX_WAVESREVERB_HIGHFREQRTRATIO_DEFAULT
};
*/

// ******************** DEFAULT PRIMARY BUFFER DEFINITION ********************

//Note: These numbers don't matter and aren't checked.
/*
BUFFER_DEFINITION g_dnDefaultPrimaryBuffer =
{
    //Playback status flags
    //Since the beginning of time primary buffers have always returned zero for this.  Hence,
    //  we must expect this and forego the ability to determine what flags it was created with.
    {
        //Flags the buffer must have.
        0,

        //Flags the buffer must NOT have.
        DSBSTATUS_LOOPING | DSBSTATUS_PLAYING,

        //Flags about which we don't care.
        0
        
    },

    //0 DMOs
    0,
    NULL

};

*/

// ******************** DMUS_APATH_SHARED_STEREOPLUSREVERB DEFINITION ********************
//This is DMO #1 (and the only DMO) on the Buffer 2 in the DMUS_APATH_SHARED_STEREOPLUSREVERB.
//  It is the waves reverb DMO.
/*
static DMO_DEFINITION SPMusic_SinkinBuffer2_DMOs[] =
{
    {
        (GUID *)&IID_IDirectSoundFXWavesReverb,
        (void *)&g_dnDefaultWavesReverb
    }
//    Additional DMOs for the second Sinkin buffer would go here.
//    ,
//    {
//
//    }
};

*/
//The list of Sinkin buffers.  All Sinkin buffers must be listed here.
static BUFFER_DEFINITION SPMusic_SinkinBuffers[] =
{

    ///////////////Sinkin Buffer 1: Stereo, no DMOs
    {
        //Playback status flags
        {
            //Flags the buffer must have.
            DSBSTATUS_LOOPING | DSBSTATUS_PLAYING,

            //Flags the buffer must NOT have.
            0,

            //Flags about which we don't care.
            0
        },

        //0 DMOs
        0,
        NULL

    }//,

    /////////////////Sinkin Buffer 2: contains Waves Reverb.
/*
    {
        //Playback status flags
        {
            //Flags the buffer must have.
            DSBSTATUS_LOOPING | DSBSTATUS_PLAYING,

            //Flags the buffer must NOT have.
            0,

            //Flags about which we don't care.
            0
        },

        //1 DMO, the Waves Reverb.  It's on the second buffer.
        //  If there were more DMOs on this buffer, they would all be addressed by this ptr.        

        //1,
//        SPMusic_SinkinBuffer2_DMOs
//  Note: put this back in!!
          0,
          NULL
        
    }
*/

};




//The DMUS_APATH_SHARED_STEREOPLUSREVERB should contain 1 SINKIN buffers and 0 MIXIN buffers.
PATH_DEFINITION g_DefaultPathStereoPlusReverb =
{
    //Zero Sinkin buffers.
    0,
    NULL,

    //Zero Mixin Buffer
    0,
    NULL,
};





// ******************** DMUS_APATH_DYNAMIC_3D DEFINITION ********************
//The DMUS_APATH_DYNAMIC_3D should contain 1 SINKIN buffer and 0 MIXIN buffers.
static BUFFER_DEFINITION SP3DDry_SinkinBuffers[] =
{
    ///////////////Sinkin Buffer 1: Dynamic (3D)
    {
        //Playback status flags
        {
            //Flags the buffer must have.
            DSBSTATUS_LOOPING | DSBSTATUS_PLAYING,

            //Flags the buffer must NOT have.
            0,

            //Flags about which we don't care.
            0
        },

        //No DMOs
        0,
        NULL
    }
};



PATH_DEFINITION g_DefaultPath3D= 
{
    //One Sinkin buffer (the 3D one with the Source Reverb upon it)
    1,
    SP3DDry_SinkinBuffers,

    //No Mixin buffers
    0,
    NULL

};




// ******************** DMUS_APATH_DYNAMIC_MONO DEFINITION ********************
//The list of SINKIN buffers.  In this case, just one.
static BUFFER_DEFINITION SPMono_SinkinBuffers[] =
{

    ///////////////Sinkin Buffer 1.
    {
        //Playback status flags
        {
            //Flags the buffer must have.
            DSBSTATUS_LOOPING | DSBSTATUS_PLAYING,

            //Flags the buffer must NOT have.
            0,

            //Flags about which we don't care.
            0
        },

        //No DMOs
        0,
        NULL
    },
};




PATH_DEFINITION g_DefaultPathMono = 
{
    //One Sinkin buffer
    1,
    SPMono_SinkinBuffers,

    //No Mixin buffers
    0,
    NULL

};





// ******************** DMUS_APATH_SHARED_STEREO DEFINITION ********************
//The list of SINKIN buffers.  In this case, just one.
static BUFFER_DEFINITION SPStereo_SinkinBuffers[] =
{    ///////////////Sinkin Buffer 1.
    {
        //Playback status flags
        {
            //Flags the buffer must have.
            DSBSTATUS_LOOPING | DSBSTATUS_PLAYING,

            //Flags the buffer must NOT have.
            0,

            //Flags about which we don't care.
            0
        },

        //No DMOs
        0,
        NULL,

    }
};


PATH_DEFINITION g_DefaultPathStereo = 
{
    //One Sinkin buffer
    0,
    NULL,

    //No Mixin buffers
    0,
    NULL

};
