//+-------------------------------------------------------------------------
//  Microsoft Windows
//--------------------------------------------------------------------------

#pragma once


HRESULT Verify3DDefaults(DS3DBUFFER *pParams);
//HRESULT VerifyListenerDefaults(DS3DLISTENER* pParams);
//HRESULT VerifyPrimaryBufferDefaults(IDirectSoundBuffer8 *pBuffer8);
//HRESULT VerifySecondaryBufferDefaults(DSBCAPS *pDSBCaps);

//HRESULT Verify_IDirectSoundFXWavesReverb_Defaults(DSFXWavesReverb *pReverbParams);


//Helpers to verify that no DMOs exist at a certain place in the audiopath.
HRESULT VerifyDMOAbsence(CtIDirectMusicSegmentState8 *ptSegState8,
                         DWORD dwStage,
                         DWORD dwBuffer,
                         DWORD dwIndex);
HRESULT VerifyDMOAbsence(CtIDirectMusicAudioPath *ptAudioPath,
                         DWORD dwStage,
                         DWORD dwBuffer,
                         DWORD dwIndex);
HRESULT VerifyDMOAbsence(IDirectSoundBuffer8 * pBuffer8,
                         DWORD dwIndex);

//A nice array of all the DMO GUIDs.  Used to loop through to verify that no DMOs exist
//  where they're not supposed to be.
//GUID *All_DMO_GUIDs[]={};
extern DWORD gdw_All_DMO_GUIDs;


///----------------------------- DEFINING AN AUDIOPATH -------------------------------
//A BUFFER_DEFINITION includes a list of DMO definitions, and optionally a 3D Buffer definition.
//A DMO_DEFINITION includes the DMO's GUID and a ptr to its expected params.

/********************************************************************************
DMO_DEFINITION: Lists the type and parameters of a DMO.
********************************************************************************/
struct DMO_DEFINITION
{
    GUID *refguidDMO;                           //IID of the DMO
    void *pvDMOParams;                          //Expected params of the DMO.
};


/********************************************************************************
BUFFER_DEFINITION: A complete definition of what properties a buffer should and
                   shouldn't have.
********************************************************************************/
struct BUFFER_DEFINITION
{
    DWORD               dwStatusFlags[3];   //    "wanted," "notwanted," and "dontcare."
                                            //    The "CapFlags" are those of the buffer,
                                            //    while the "StatusFlags" show the buffer's
                                            //    playing state.  All flags must be specified
                                            //    in exactly one set.  See examples.

    DWORD               dwDMOs;             //How many DMOs we expect.
    DMO_DEFINITION *    pDMOs;              //Definitions of expected DMOs.
};

/********************************************************************************
PATH_DEFINITION: A complete definition of what should and shouldn't be in an
                 audiopath.

                Includes 2 lists of BUFFER_DEFINITIONs, one for SINKIN buffers
                and one for MIXIN buffers.  These 2 types of buffers are
                mutually exclusive (i.e. you can never get a certain buffer by
                specifying the other type of flag)
********************************************************************************/
struct PATH_DEFINITION
{
    DWORD               dwSinkinBuffers;    //How many SINKIN BUFFERs we expect.
    BUFFER_DEFINITION * pSinkinBuffers;     //Definitions for SINKIN buffers.
    DWORD               dwMixinBuffers;     //How many MIXIN BUFFERs we expect.
    BUFFER_DEFINITION*  pMixinBuffers;      //Definitions for MIXIN buffers.
};


HRESULT SanityCheckBufferDef(BUFFER_DEFINITION *pBufferDef);
HRESULT VerifyPathData      (PATH_DEFINITION    *pDef,       CtIDirectMusicAudioPath *ptInterface);
//HRESULT VerifyBufferData    (BUFFER_DEFINITION  *pBufferDef, IDirectSoundBuffer8     *pBuffer8);
HRESULT VerifyBufferData    (BUFFER_DEFINITION  *pBufferDef, IDirectSoundBuffer      *pBuffer);
HRESULT VerifyDMOData       (DMO_DEFINITION     *pDMO,       IUnknown                *pUnk);


// {60383005-A25F-425b-A6FD-6DAEB0F4D222}
//lint -e14
DEFINE_GUID(GUID_SendPlaceholder, 0x60383005, 0xa25f, 0x425b, 0xa6, 0xfd, 0x6d, 0xae, 0xb0, 0xf4, 0xd2, 0x22);
//lint +e14
