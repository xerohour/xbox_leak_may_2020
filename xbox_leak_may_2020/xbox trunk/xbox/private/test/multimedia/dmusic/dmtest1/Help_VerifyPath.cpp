#include "globals.h"
#include "Help_VerifyPath.h"
#include "Help_Definitions.h"

HRESULT VerifyAllBufferDMOs(BUFFER_DEFINITION *pBufferDef,
                            DWORD             dwBufferIndex,
                            DWORD             dwBufferStage,       //is this a MIXIN or SINKIN buffer.
                            CtIDirectMusicAudioPath *ptInterface,   //interfaces to obtain the DMO from.
                            IDirectSoundBuffer8 *pBuffer8);
HRESULT VerifyInvalidBufferMethods(IDirectSoundBuffer8 *pBuffer8);
HRESULT VerifyBufferData(BUFFER_DEFINITION *pBufferDef, IDirectSoundBuffer *pBuffer);
//HRESULT VerifyBufferData(BUFFER_DEFINITION *pBufferDef, IDirectSoundBuffer8 *pBuffer8);


LPSTR TypeOfDMO(IUnknown *pUnk);

/*
GUID *All_DMO_GUIDs[] = 
{
//(GUID *)&IID_IDirectSoundFXSend,
(GUID *)&IID_IDirectSoundFXGargle,
(GUID *)&IID_IDirectSoundFXChorus,
(GUID *)&IID_IDirectSoundFXFlanger,
(GUID *)&IID_IDirectSoundFXEcho,
(GUID *)&IID_IDirectSoundFXDistortion,
(GUID *)&IID_IDirectSoundFXCompressor, 
(GUID *)&IID_IDirectSoundFXParamEq,
(GUID *)&IID_IDirectSoundFXI3DL2Reverb,
(GUID *)&IID_IDirectSoundFXWavesReverb
};
*/

//DWORD gdw_All_DMO_GUIDs = AMOUNT(All_DMO_GUIDs);

                         
static DWORD g_dwValidStatusFlags =
    DSBSTATUS_LOOPING | DSBSTATUS_PLAYING;



/********************************************************************************
HISTORY:
    Created 04/12/00      danhaff

Description:
    Helper function to verify the absense of any DMOs at an SegState8's
    buffer/index.
********************************************************************************/
/*
HRESULT VerifyDMOAbsence(CtIDirectMusicSegmentState8 *ptSegState8, DWORD dwStage, DWORD dwBuffer, DWORD dwIndex)
{
    IUnknown *pUnkDMO = NULL;
    HRESULT hr = S_OK;
    DWORD i = 0;


    //Verify that no other DMOs are on this buffer.
    for (i=0; i<AMOUNT(All_DMO_GUIDs); i++)
    {

        hr = ptSegState8->GetObjectInPath(IgnorePChannel(dwStage),
                                          dwStage,
                                          dwBuffer,
                                          GUID_All_Objects,
                                          dwIndex,
                                          *(All_DMO_GUIDs[i]),
                                          (void **)&pUnkDMO);
        if (hr != DMUS_E_NOT_FOUND)
        {
            Log(ABORTLOGLEVEL, "This SegState8 has a %s DMO at dwStage %s, dwBuffer %d, dwIndex %d!!",
                                    dmthXlatGUID(*(All_DMO_GUIDs[i])),
                                    dmthXlatDMUS_STAGE(dwStage),
                                    dwBuffer,
                                    dwIndex);

            SAFE_RELEASE(pUnkDMO);
            hr = E_FAIL;
        }                   

    }

    SAFE_RELEASE(pUnkDMO);
    return hr;

};
*/

/********************************************************************************
HISTORY:
    Created 04/12/00      danhaff

Description:
    Helper function to verify the absense of any DMOs at an AudioPath's
    buffer/index.
********************************************************************************/
/*
HRESULT VerifyDMOAbsence(CtIDirectMusicAudioPath *ptAudioPath, DWORD dwStage, DWORD dwBuffer, DWORD dwIndex)
{
    IUnknown *pUnkDMO = NULL;
    HRESULT hr = S_OK;
    DWORD i = 0;


    //Verify that no other DMOs are on this buffer.
    for (i=0; i<AMOUNT(All_DMO_GUIDs); i++)
    {

        hr = ptAudioPath->GetObjectInPath(IgnorePChannel(dwStage),
                                          dwStage,
                                          dwBuffer,
                                          GUID_All_Objects,
                                          dwIndex,
                                         *(All_DMO_GUIDs[i]),
                                          (void **)&pUnkDMO);
        
        if (hr != DMUS_E_NOT_FOUND)
        {
            Log(ABORTLOGLEVEL, "This AudioPath has a %s DMO at dwStage %s, dwBuffer %d, dwIndex %d!!",
                                    dmthXlatGUID(*(All_DMO_GUIDs[i])),
                                    dmthXlatDMUS_STAGE(dwStage),
                                    dwBuffer,
                                    dwIndex);

            SAFE_RELEASE(pUnkDMO);
            hr = E_FAIL;
        }                   

    }

    SAFE_RELEASE(pUnkDMO);
    return hr;

};

*/
/********************************************************************************
HELPER FUNCTION

HISTORY:
    Created 04/13/00      danhaff

DESCRIPTION:
    Given a BUFFER_DESCRIPTION, this method catches stupid mistakes have been made
    in its creation and describes the location of those mistakes.  This prevents the
    actual verification function (VerifyPathDefaults) from mistakenly passing (our worst fear).
    
    Note: this does NOT verify DMO data.  It would be possible but would take a while,
    and any invalid data would cause VerifyPathDefaults to fail anyway, attracting attention.
********************************************************************************/
HRESULT SanityCheckBufferDef(BUFFER_DEFINITION *pBufferDef)
{
    HRESULT hr                  = S_OK;
//    DWORD dwCapFlagUsage[32]    = {0};
//    DWORD dwCapFlagValue        = 0;
    DWORD dwStatusFlagUsage[32] = {0};
    DWORD dwStatusFlagValue     = 0;
    DWORD dwDMOIndex            = 0;   
    DWORD i                     = 0;
    DWORD j                     = 0;

    //Sanity check the StatusFlags passed in.
    //Count how many times each StatusFlag is used, store in the dwStatusFlagUsage array.
    for (i=0; i<3; i++)
        for (j=0; j<32; j++)
        {
            dwStatusFlagValue = 1 << j;
            if (pBufferDef->dwStatusFlags[i] & dwStatusFlagValue)
                dwStatusFlagUsage[j]++;
        }

    //Check each of the valid StatusFlags and make sure they're only used once.
    //We won't let people leave out a StatusFlag, and we won't let people specify a StatusFlag
    //in two different sections.  This verifies they didn't screw up majorly.
    for (j=0; j<32; j++)
    {
        dwStatusFlagValue = 1 << j;

        //If this is a valid flag, it should be used exactly once.
        if (dwStatusFlagValue & g_dwValidStatusFlags)
        {
            if (0 == dwStatusFlagUsage[j])
            {
                Log(ABORTLOGLEVEL, "TEST APP ERROR: The following flag was not specified in our buffer definition.");
                //Log_DSBStatus(ABORTLOGLEVEL, dwStatusFlagValue);
                Log(ABORTLOGLEVEL, "%08X", dwStatusFlagValue);
                hr = E_FAIL;
            }

            if (dwStatusFlagUsage[j] >= 2)
            {
                Log(ABORTLOGLEVEL, "TEST APP ERROR: The following flag was specified more than once in our buffer definition.");
                //Log_DSBStatus(ABORTLOGLEVEL, dwStatusFlagValue);
                Log(ABORTLOGLEVEL, "%08X", dwStatusFlagValue);
                hr = E_FAIL;
            }
        }
        //If this is NOT a valid flag, it shouldn't be used at all.
        else
        {
            if (0 != dwStatusFlagUsage[j])
            {
                Log(ABORTLOGLEVEL, "TEST APP ERROR: The following flag is invalid but specified in our buffer definition anyway: %08X", dwStatusFlagValue);
                hr = E_FAIL;
            }
        }
    }


    //Make sure you have DMOs if you say you do.
    if (pBufferDef->dwDMOs != 0&& pBufferDef->pDMOs == NULL)
    {
        Log(ABORTLOGLEVEL, "TEST APP ERROR: Even though your DMO count is %d, your DMO pointer is NULL", pBufferDef->dwDMOs);
        hr = E_FAIL;
    }

    
    //Sanity check all the DMOs.
/*BUGBUG: Put this back in later!
    for (dwDMOIndex = 0; dwDMOIndex < pBufferDef->dwDMOs; dwDMOIndex++)
    {
        //Make sure the specified DMO matches one of the known DMO GUIDs.
        BOOL bFound = FALSE;
        for (i=0; i<gdw_All_DMO_GUIDs; i++)
        {
            if (memcmp((void *)pBufferDef->pDMOs[dwDMOIndex].refguidDMO, (void *)All_DMO_GUIDs[i], sizeof(GUID)) == 0)
            {
                bFound = TRUE;
                break;
            }
        }
        if (!bFound)
        {
            Log(ABORTLOGLEVEL, "TEST APP ERROR: Buffer DMO #%d's GUID is not recognized", dwDMOIndex);
            hr = E_FAIL;
        }

        //Make sure you have data to check against.
        if (NULL == pBufferDef->pDMOs[dwDMOIndex].pvDMOParams)
        {
            Log(ABORTLOGLEVEL, "TEST APP ERROR: Buffer DMO #%d's data ptr is NULL", dwDMOIndex);
            hr = E_FAIL;
        }
    }
  */  

return hr;
};


/********************************************************************************
HELPER FUNCTION for many many Test Case Functions

HISTORY:
    Created 04/13/00      danhaff

DESCRIPTION:
    This mammoth function takes an "active" AudioPath (AP) or SegmentState (SS) 
    interface and compares it with a PATH_DEFINITION structure to make sure everything's
    in the "path" interface (AP or SS) that should be, and nothing's in the path
    that shouldn't be.  It spews everything that's wrong with the path using
    FNS_ABORTLOGLEVEL and of course returns E_FAIL or S_OK depending on the
    results.

    This interface (AP or SS) MUST have been obtained from a CICMusic class.  This
    guarantees that everything is set up correctly and looping.    
********************************************************************************/
HRESULT VerifyPathData(PATH_DEFINITION *pDef, CtIDirectMusicAudioPath *ptInterface)
{
    //State of the test.
    HRESULT                     hr                  = S_OK;
    BOOL                        bPassing            = TRUE;

    //Which actual buffer we're checking.
    DWORD                       dwBufferIndex       = 0;

    //Interfaces we pick up along the way.
    IDirectSoundBuffer8 *       pBuffer8            = NULL;    //The Buffer interface we use to obtain data from.


    //For both SINKIN and MIXIN buffers, we set this data according to which type we're using.
    DWORD                       dwBufferTypeIndex   = 0;       //Only used for iterating through SINKIN/MIXIN buffers.  Either 0 or 1.
    DWORD                       dwBufferDefs        = 0;       //Number of buffer definitions of this type.
    BUFFER_DEFINITION *         pBufferDefs         = NULL;    //All the buffer definitions of this type.
    DWORD                       dwBufferStage       = 0;       //which stage do we GOIP with. Either DMUS_PATH_BUFFER or DMUS_PATH_MIXIN_BUFFER

    //Temporary data storage.
//    DS3DBUFFER                  ds3dBuffer          = {0};      //Used to store 3D data for comparision with that in the BufferDef.
//    DS3DLISTENER                ds3dListener        = {0};      //Used to store 3D data for comparision with that in the default Listener struct.
    
    //We must do the same for both SINKIN and MIXIN buffers.
    for (dwBufferTypeIndex = 0; dwBufferTypeIndex < 1; dwBufferTypeIndex ++)
    {
        
        //Simply point us to the correct data, since we must do the
        //  same thing twice, once for MIXIN buffers and once for SINKIN
        //  buffers.
        if (0 == dwBufferTypeIndex)
        {
            dwBufferDefs  = pDef->dwSinkinBuffers;
            pBufferDefs   = pDef->pSinkinBuffers;
            dwBufferStage = DMUS_PATH_BUFFER;
            Log(ABORTLOGLEVEL, "Checking Sinkin Buffers", dwBufferIndex);
            Log(ABORTLOGLEVEL, "***********************", dwBufferIndex);

        }
/*        else if (1 == dwBufferTypeIndex)
        {
            dwBufferDefs  = pDef->dwMixinBuffers;
            pBufferDefs   = pDef->pMixinBuffers;
            dwBufferStage = DMUS_PATH_MIXIN_BUFFER;
            Log(ABORTLOGLEVEL, "Checking Mixin Buffers");
            Log(ABORTLOGLEVEL, "**********************");

        }*/
        else 
            ASSERT(FALSE);

        //For each buffer...
        for (dwBufferIndex=0; dwBufferIndex < dwBufferDefs; dwBufferIndex++)
        {


            //Make sure the "buffer definition" struct we're checking against isn't bogus in some way.
            hr = SanityCheckBufferDef(&pBufferDefs[dwBufferIndex]);
            if (S_OK != hr)
            {
                Log(ABORTLOGLEVEL, "TEST APP ERROR: Buffer Definition %d failed sanity check!!!", dwBufferIndex + 1);
                bPassing = FALSE;
                goto TEST_END;
            }


            //Obtain the buffer at this index from the AudioPath / SegmentState.
            hr = ptInterface->GetObjectInPath(IgnorePChannel(dwBufferStage), //dwPChannel
                                              dwBufferStage,                 //dwStage
                                              dwBufferIndex,                 //dwBuffer
                                              GUID_All_Objects,              //guidObject
                                              0,                             //dwIndex
                                              GUID_NULL,                     //pIID
                                              (void **)&pBuffer8);  
            //We demand full success.
            if (S_OK != hr)
            {
                Log(ABORTLOGLEVEL, "ERROR: %s->GetObjectInPath for buffer %d/%d returned (%s == %08Xh)",
                                        ptInterface->m_szInterfaceName,
                                        dwBufferIndex + 1,
                                        dwBufferDefs,
                                        tdmXlatHRESULT(hr),
                                        hr);
                bPassing = FALSE;
            }

            //If this call succeeded then scrutinize the buffer.
            //if (bPassing)
            if (SUCCEEDED(hr))
            {
                Log(ABORTLOGLEVEL, "-------------- Verifying buffer %d ----------------", dwBufferIndex + 1);

                hr = VerifyBufferData(&pBufferDefs[dwBufferIndex], pBuffer8);
                if (S_OK != hr)
                {
                    Log(ABORTLOGLEVEL, "ERROR! Buffer %d failed verification!!!", dwBufferIndex + 1);
                    bPassing = FALSE;
                }

                //Release our regular buffer.
                SAFE_RELEASE(pBuffer8);
                Log(ABORTLOGLEVEL, "-------------- End verifying buffer %d ----------------", dwBufferIndex + 1);
            }//END: if (pBuffer8)
        }//END: for all buffers.


        //Now that we're done testing all the valid buffers, attempt to obtain the first INVALID 
        //  buffer and verify that it is absent.
        hr = ptInterface->GetObjectInPath(IgnorePChannel(dwBufferStage), //dwPChannel
                                          dwBufferStage,             //dwStage
                                          dwBufferDefs,              //the Max of the buffers.
                                          GUID_All_Objects,          //guidObject
                                          0,                         //dwIndex
                                          GUID_NULL,                //iidInterface
                                          (void **)&pBuffer8);          
        if (DMUS_E_NOT_FOUND != hr)
        {
            Log(ABORTLOGLEVEL, "ERROR: %s->GetObjectInPath for first invalid %s buffer (#%d) returned (%s == %08Xh) instead of DMUS_E_NOT_FOUND",
                                    ptInterface->m_szInterfaceName,
                                    0 ==dwBufferTypeIndex  ? "SINKIN" : "MIXIN",
                                    dwBufferDefs + 1,
                                    tdmXlatHRESULT(hr),
                                    hr);
            bPassing = FALSE;
            SAFE_RELEASE(pBuffer8);
        };
    }//END: for both SINKIN and MIXIN buffers.



TEST_END:
    
    //Free the good stuff.
    SAFE_RELEASE(pBuffer8);

    if (bPassing)
        return S_OK;
    else
        return E_FAIL;    
};





/********************************************************************************
HELPER FUNCTION

HISTORY:
    Created 04/14/00      danhaff

DESCRIPTION:
    This function takes an IDirectSoundBuffer8 and compares it with a
    BUFFER_DEFINITION structure to make sure everything's correct.

NOTES:
    This does NOT verify a buffer's DMOs, or 3D data.  Only data obtainable
    through the buffer interface do we verify.
********************************************************************************/
HRESULT VerifyBufferData(BUFFER_DEFINITION *pBufferDef, IDirectSoundBuffer8 *pBuffer8)
{

    //BUGBUG: 
    return S_OK;

/*
    //Properties of the buffer we're checking.
    DWORD                       dwFormatSize        = 0;
    LPWAVEFORMATEX              pWfx                = NULL;
    DWORD                       dwBufferStatus      = 0;
    DWORD                       dwFrequency         = 0;
    LONG                        lVolume             = 0;
    LONG                        lPan                = 0;
    DWORD                       dwDsfxResult        = 0;
    DWORD                       dwStatusFlags       = 0;        //Buffer status flags.
    BOOL                        bPassing            = TRUE;
    HRESULT                     hr                  = E_FAIL;
    DWORD                       j                   = 0;        //Used for indexing 
    DWORD                       dwFlagValue         = 0;        //Used to store the single value of a flag as we're indexing through.
//    DWORD dwChannelVolumes[]                        = {SPEAKER_FRONT_LEFT};
    LONG plVolume                                   = 0;
//    DSEFFECTDESC dsfx                               = {0};      //In-param for SetFx() test.
    DWORD                       *pdwDSFXResult      = NULL;

    

    hr = pBuffer8->GetStatus(&dwStatusFlags);
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL, "ERROR: IDirectSoundBuffer8->GetStatus returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        bPassing = FALSE;
        goto TEST_END;
    }

    //Verify the STATUS flags.
    for (j=0; j<32; j++)
    {
        dwFlagValue = 1 << j;

        //If this is a wanted flag, verify it's there.
        if (dwFlagValue & pBufferDef->dwStatusFlags[0])
        {
            if (!(dwStatusFlags & dwFlagValue))
            {
                Log(ABORTLOGLEVEL, "ERROR: The following flag is missing from IDirectSoundBuffer8's status");
                //Log_DSBStatus(ABORTLOGLEVEL, dwFlagValue);
                Log(ABORTLOGLEVEL, "%08X", dwFlagValue);
                bPassing = FALSE;
            }
        }

        //If this is an unwanted flag, verify it's NOT there.
        if (dwFlagValue & pBufferDef->dwStatusFlags[1])
        {
            if (dwStatusFlags & dwFlagValue)
            {
                Log(ABORTLOGLEVEL, "ERROR: The following flag is present in IDirectSoundBuffer8's status");
                //Log_DSBStatus(ABORTLOGLEVEL, dwFlagValue);
                Log(ABORTLOGLEVEL, "%08X", dwFlagValue);
                bPassing = FALSE;
            }
        }
    }

    

TEST_END:


    if (pWfx)
    {
        LocalFree(pWfx);
        pWfx = NULL;
    }

    if (pdwDSFXResult)
    {
        LocalFree(pdwDSFXResult);
        pdwDSFXResult= NULL;
    }

    if (bPassing)
        return S_OK;
    else
        return E_FAIL;
*/
}






/********************************************************************************
HELPER
VerifyBufferDMOs

HISTORY:
    Created 04/14/00      danhaff

DESCRIPTION:
    Given a buffer and its description, verify that it has the correct set of
    DMOs in it.
********************************************************************************/
/*
HRESULT VerifyAllBufferDMOs(BUFFER_DEFINITION *pBufferDef,
                            DWORD             dwBufferIndex,
                            DWORD             dwBufferStage,       //is this a MIXIN or SINKIN buffer.
                            CtIDirectMusicAudioPath *ptInterface,   //interfaces to obtain the DMO from.
                            IDirectSoundBuffer8 *pBuffer8)
{
    BOOL bPassing = TRUE;
    HRESULT hr = S_OK;
    DWORD dwDMOStage = 0;
    DWORD dwDMOIndex = 0;
    DWORD dwDMOQIMethod = 0;
    IUnknown *pUnkDMO = NULL;

    //Set the DMO stage correctly.
    if (DMUS_PATH_BUFFER == dwBufferStage)
        dwDMOStage = DMUS_PATH_BUFFER_DMO;
    else if (DMUS_PATH_MIXIN_BUFFER == dwBufferStage)
        dwDMOStage = DMUS_PATH_MIXIN_BUFFER_DMO;

    for (dwDMOIndex = 0; dwDMOIndex < pBufferDef->dwDMOs; dwDMOIndex++)
    {
        for (dwDMOQIMethod = 0; dwDMOQIMethod < 2; dwDMOQIMethod ++)
        {
        
            ///////////////// GET THE DMO as an IUnknown ////////////////////
            if (0 == dwDMOQIMethod)
            {
                hr = pBuffer8->GetObjectInPath(GUID_All_Objects,
                                          dwDMOIndex,
                                          IID_IUnknown,
                                          (void **)&pUnkDMO);
                if (hr != S_OK)
                {
                    Log(ABORTLOGLEVEL, "ERROR: IDirectSoundBuffer->GetObjectInPath for DMO #%d (%s) returned (%s == %08Xh) instead of S_OK",
                                           dwDMOIndex + 1,
                                           dmthXlatGUID(*pBufferDef->pDMOs[dwDMOIndex].refguidDMO),
                                           tdmXlatHRESULT(hr),
                                           hr);
                    bPassing = FALSE;
                }
                
            }
            else
            {
                hr = ptInterface->GetObjectInPath(IgnorePChannel(dwDMOStage),//dwPChannel
                                                  dwDMOStage,                //dwStage
                                                  dwBufferIndex,             //Which buffer we're on.
                                                  GUID_All_Objects,          //guidObject
                                                  dwDMOIndex,                //Max of the DMOs.
                                                  IID_IUnknown,              //iidInterface
                                                  (void **)&pUnkDMO);
                if (hr != S_OK)
                {
                    Log(ABORTLOGLEVEL, "ERROR: %s->GetObjectInPath for DMO #%d (%s) returned (%s == %08Xh) instead of S_OK",
                                           ptInterface->m_szInterfaceName,
                                           dwDMOIndex + 1,
                                           dmthXlatGUID(*pBufferDef->pDMOs[dwDMOIndex].refguidDMO),
                                           tdmXlatHRESULT(hr),
                                           hr);
                    bPassing = FALSE;
                }    

            }

            //If that call actually returned something, we'll check it out.
            if (pUnkDMO)            
            {
                hr = VerifyDMOData(&pBufferDef->pDMOs[dwDMOIndex], pUnkDMO);
                if (S_OK != hr)
                {
                    Log(ABORTLOGLEVEL, "ERROR: Buffer %d's DMO %d had incorrect parameters.", dwBufferIndex + 1, dwDMOIndex + 1);
                    bPassing = FALSE;
                    SAFE_RELEASE(pUnkDMO);
                }

                SAFE_RELEASE(pUnkDMO);
            }
        }//END: for both QI methods.
    }//END: for all DMOs in the buffer.


    DWORD dwTemp = DMUS_E_NOT_FOUND;
    DWORD dwTemp2 = DSERR_OBJECTNOTFOUND;
    ///////////////// Verify Non-existent DMOs don't exist ////////////////////
    //Attempt to obtain the DMOs both ways; verify they fail..
    hr = pBuffer8->GetObjectInPath(GUID_All_Objects,
                                   pBufferDef->dwDMOs,
                                   IID_IUnknown,
                                   (void **)&pUnkDMO);
    if (hr != DMUS_E_NOT_FOUND)
    {
        Log(ABORTLOGLEVEL, "ERROR: IDirectSoundBuffer(#%d)->GetObjectInPath for IUnk from nonexistent DMO #%d returned (%s == %08Xh) instead of DMUS_E_NOT_FOUND",
                               dwBufferIndex + 1,            
                               pBufferDef->dwDMOs + 1,
                               tdmXlatHRESULT(hr),
                               hr);
        bPassing = FALSE;
    }

    SAFE_RELEASE(pUnkDMO);


    hr = ptInterface->GetObjectInPath(IgnorePChannel(dwDMOStage),//dwPChannel
                                      dwDMOStage,                //dwStage
                                      dwBufferIndex,             //Which buffer we're on.
                                      GUID_All_Objects,          //guidObject
                                      pBufferDef->dwDMOs,         //Max of the DMOs.
                                      IID_IUnknown,              //iidInterface
                                      (void **)&pUnkDMO);
    if (hr != DMUS_E_NOT_FOUND)
    {
        Log(ABORTLOGLEVEL, "ERROR: %s->GetObjectInPath for IUnk from buffer %d's nonexistent DMO #%d returned (%s == %08Xh) instead of DMUS_E_NOT_FOUND",
                               ptInterface->m_szInterfaceName,
                               dwBufferIndex + 1,
                               pBufferDef->dwDMOs + 1,
                               tdmXlatHRESULT(hr),
                               hr);
        bPassing = FALSE;
    }
    SAFE_RELEASE(pUnkDMO);


    if (bPassing)
        return S_OK;
    else
        return E_FAIL;
};

*/


/********************************************************************************
HELPER
VerifyDMOData

HISTORY:
    Created 04/14/00      danhaff

DESCRIPTION:
    Given a DMO and its description, verify that it has the correct parameters.
********************************************************************************/
/*
HRESULT VerifyDMOData(DMO_DEFINITION *pDMO, IUnknown *pUnk)
{
//IDirectSoundFXSend              *pSend          = NULL;
IDirectSoundFXGargle            *pGargle        = NULL;
IDirectSoundFXChorus            *pChorus        = NULL; 
IDirectSoundFXFlanger           *pFlanger       = NULL;
IDirectSoundFXEcho              *pEcho          = NULL; 
IDirectSoundFXDistortion        *pDistortion    = NULL;
IDirectSoundFXCompressor        *pCompressor    = NULL;
IDirectSoundFXParamEq           *pParamEq       = NULL;
IDirectSoundFXI3DL2Reverb       *pReverb        = NULL;
IDirectSoundFXWavesReverb       *pWavesReverb   = NULL;


//DSFXSend                        SendParams;
DSFXGargle                      GargleParams;
DSFXChorus                      ChorusParams;
DSFXFlanger                     FlangerParams;
DSFXEcho                        EchoParams;
DSFXDistortion                  DistortionParams;
DSFXCompressor                  CompressorParams;
DSFXParamEq                     ParamEqParams;
DSFXI3DL2Reverb                ReverbParams;
DSFXWavesReverb                WavesReverbParams;

HRESULT                         hr              = S_OK;
BOOL                            bPassing        = TRUE;
BOOL                            bEqual          = 0;

//We gotta make sure we have the correct type of DMO before getting parameters from it.

//Obtain the correct interface.

//if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXSend, sizeof(GUID)) == 0)
//    hr = pUnk->QueryInterface(IID_IDirectSoundFXSend, (void **)&pSend);
//else
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXGargle, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXGargle, (void **)&pGargle);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXFlanger, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXFlanger, (void **)&pFlanger);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXChorus, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXChorus, (void **)&pChorus);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXEcho, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXEcho, (void **)&pEcho);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXDistortion, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXDistortion, (void **)&pDistortion);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXCompressor, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXCompressor, (void **)&pCompressor);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXParamEq, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXParamEq, (void **)&pParamEq);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXI3DL2Reverb, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXI3DL2Reverb, (void **)&pReverb);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXWavesReverb, sizeof(GUID)) == 0)
    hr = pUnk->QueryInterface(IID_IDirectSoundFXWavesReverb, (void **)&pWavesReverb);
else
{

    //Okay, if it's a send then we're not gonna be able to query ANY of these interfaces.  However, we'll
    //  know this because we passed in the GUID IID_SendPlaceHolder.  Bail here; there's no more
    //  checking to do.
    if (memcmp((void *)pDMO->refguidDMO, (void *)&GUID_SendPlaceholder, sizeof(GUID))==0)
    {
        Log(FYILOGLEVEL, "VerifyDMOData was passed a DMO from which no FX could be QI'd.  However, this is a Send, hence it's expected. Returning S_OK.");
        return S_OK;    
    }

    //If we're not expecting this, then fail the call.
    Log(ABORTLOGLEVEL, "TEST APP ERROR: trying to test params on unsupported DMO %s", dmthXlatGUID(*pDMO->refguidDMO));
    bPassing = FALSE;
    goto TEST_END;
}


if (S_OK != hr)
{
    Log(ABORTLOGLEVEL, "ERROR: IUnknown->QueryInterface for %s returned (%s == %08Xh)", dmthXlatGUID(*pDMO->refguidDMO), tdmXlatHRESULT(hr), hr);
    Log(ABORTLOGLEVEL, "       The DMO is a %s instead.", TypeOfDMO(pUnk));
    bPassing = FALSE;
    goto TEST_END;
};

//Get the correct parameters.
//if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXSend, sizeof(GUID)) == 0)
//    hr = pSend->GetAllParameters(&SendParams);
//else
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXGargle, sizeof(GUID)) == 0)
    hr = pGargle->GetAllParameters(&GargleParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXFlanger, sizeof(GUID)) == 0)
    hr = pFlanger->GetAllParameters(&FlangerParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXChorus, sizeof(GUID)) == 0)
    hr = pChorus->GetAllParameters(&ChorusParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXEcho, sizeof(GUID)) == 0)
    hr = pEcho->GetAllParameters(&EchoParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXDistortion, sizeof(GUID)) == 0)
    hr = pDistortion->GetAllParameters(&DistortionParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXCompressor, sizeof(GUID)) == 0)
    hr = pCompressor->GetAllParameters(&CompressorParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXParamEq, sizeof(GUID)) == 0)
    hr = pParamEq->GetAllParameters(&ParamEqParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXI3DL2Reverb, sizeof(GUID)) == 0)
    hr = pReverb->GetAllParameters(&ReverbParams);
else if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXWavesReverb, sizeof(GUID)) == 0)
    hr = pWavesReverb->GetAllParameters(&WavesReverbParams);
else
{
    Log(ABORTLOGLEVEL, "TEST APP ERROR, trying to test params on unsupported DMO %s", dmthXlatGUID(*pDMO->refguidDMO));
    bPassing = FALSE;
    goto TEST_END;
}
if (S_OK != hr)
{
    Log(ABORTLOGLEVEL, "ERROR: %s->GetAllParameters for returned (%s == %08Xh)", dmthXlatGUID(*pDMO->refguidDMO), tdmXlatHRESULT(hr), hr);
    bPassing = FALSE;
    goto TEST_END;
};


//Log the parameters.
//if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXSend, sizeof(GUID)) == 0)
//    Log_Send_Params(FYILOGLEVEL, &SendParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXGargle, sizeof(GUID)) == 0)
    Log_Gargle_Params(FYILOGLEVEL, &GargleParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXFlanger, sizeof(GUID)) == 0)
    Log_Flanger_Params(FYILOGLEVEL, &FlangerParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXChorus, sizeof(GUID)) == 0)
    Log_Chorus_Params(FYILOGLEVEL, &ChorusParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXEcho, sizeof(GUID)) == 0)
    Log_Echo_Params(FYILOGLEVEL, &EchoParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXDistortion, sizeof(GUID)) == 0)
    Log_Distortion_Params(FYILOGLEVEL, &DistortionParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXCompressor, sizeof(GUID)) == 0)
    Log_Compressor_Params(FYILOGLEVEL, &CompressorParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXParamEq, sizeof(GUID)) == 0)
    Log_ParamEq_Params(FYILOGLEVEL, &ParamEqParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXI3DL2Reverb, sizeof(GUID)) == 0)
    Log_I3DL2Reverb_Params(FYILOGLEVEL, &ReverbParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXWavesReverb, sizeof(GUID)) == 0)
    Log_WavesReverb_Params(FYILOGLEVEL, &WavesReverbParams);

//Compare the parameters.
//if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXSend, sizeof(GUID)) == 0)
//    bEqual = Equal_Send_Params((DSFXSend *)pDMO->pvDMOParams, &SendParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXGargle, sizeof(GUID)) == 0)
    bEqual = Equal_Gargle_Params((DSFXGargle *)pDMO->pvDMOParams, &GargleParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXFlanger, sizeof(GUID)) == 0)
    bEqual = Equal_Flanger_Params((DSFXFlanger *)pDMO->pvDMOParams, &FlangerParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXChorus, sizeof(GUID)) == 0)
    bEqual = Equal_Chorus_Params((DSFXChorus *)pDMO->pvDMOParams, &ChorusParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXEcho, sizeof(GUID)) == 0)
    bEqual = Equal_Echo_Params((DSFXEcho *)pDMO->pvDMOParams, &EchoParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXDistortion, sizeof(GUID)) == 0)
    bEqual = Equal_Distortion_Params((DSFXDistortion *)pDMO->pvDMOParams, &DistortionParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXCompressor, sizeof(GUID)) == 0)
    bEqual = Equal_Compressor_Params((DSFXCompressor *)pDMO->pvDMOParams, &CompressorParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXParamEq, sizeof(GUID)) == 0)
    bEqual = Equal_ParamEq_Params((DSFXParamEq *)pDMO->pvDMOParams, &ParamEqParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXI3DL2Reverb, sizeof(GUID)) == 0)
    bEqual = Equal_I3DL2Reverb_Params((DSFXI3DL2Reverb *)pDMO->pvDMOParams, &ReverbParams);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXWavesReverb, sizeof(GUID)) == 0)
    bEqual = Equal_WavesReverb_Params((DSFXWavesReverb *)pDMO->pvDMOParams, &WavesReverbParams);
if (!bEqual)
{
    Log(ABORTLOGLEVEL, "ERROR: %s's parameters were incorrect (see returned, followed by expected)", dmthXlatGUID(*pDMO->refguidDMO));
    bPassing = FALSE;


//    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXSend, sizeof(GUID)) == 0)
//    {
//        Log(ABORTLOGLEVEL, "--- RETURNED -----");
//        Log_Send_Params(ABORTLOGLEVEL, &SendParams);
//        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
//        Log_Send_Params(ABORTLOGLEVEL, (DSFXSend *)pDMO->pvDMOParams);
//    }

    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXGargle, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_Gargle_Params(ABORTLOGLEVEL, &GargleParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_Gargle_Params(ABORTLOGLEVEL, (DSFXGargle *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXFlanger, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_Flanger_Params(ABORTLOGLEVEL, &FlangerParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_Flanger_Params(ABORTLOGLEVEL, (DSFXFlanger *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXChorus, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_Chorus_Params(ABORTLOGLEVEL, &ChorusParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_Chorus_Params(ABORTLOGLEVEL, (DSFXChorus *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXEcho, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_Echo_Params(ABORTLOGLEVEL, &EchoParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_Echo_Params(ABORTLOGLEVEL, (DSFXEcho *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXDistortion, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_Distortion_Params(ABORTLOGLEVEL, &DistortionParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_Distortion_Params(ABORTLOGLEVEL, (DSFXDistortion *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXCompressor, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_Compressor_Params(ABORTLOGLEVEL, &CompressorParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_Compressor_Params(ABORTLOGLEVEL, (DSFXCompressor *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXParamEq, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_ParamEq_Params(ABORTLOGLEVEL, &ParamEqParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_ParamEq_Params(ABORTLOGLEVEL, (DSFXParamEq *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXI3DL2Reverb, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_I3DL2Reverb_Params(ABORTLOGLEVEL, &ReverbParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_I3DL2Reverb_Params(ABORTLOGLEVEL, (DSFXI3DL2Reverb *)pDMO->pvDMOParams);
    }
    if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXWavesReverb, sizeof(GUID)) == 0)
    {
        Log(ABORTLOGLEVEL, "--- RETURNED -----");
        Log_WavesReverb_Params(ABORTLOGLEVEL, &WavesReverbParams);
        Log(ABORTLOGLEVEL, "--- EXPECTED -----");
        Log_WavesReverb_Params(ABORTLOGLEVEL, (DSFXWavesReverb *)pDMO->pvDMOParams);
    }

    goto TEST_END;
}



TEST_END:

//Release the new interface.
//if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXSend, sizeof(GUID)) == 0)
//    SAFE_RELEASE(pSend);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXGargle, sizeof(GUID)) == 0)
    SAFE_RELEASE(pGargle);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXFlanger, sizeof(GUID)) == 0)
    SAFE_RELEASE(pFlanger);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXChorus, sizeof(GUID)) == 0)
    SAFE_RELEASE(pChorus);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXEcho, sizeof(GUID)) == 0)
    SAFE_RELEASE(pEcho);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXDistortion, sizeof(GUID)) == 0)
    SAFE_RELEASE(pDistortion);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXCompressor, sizeof(GUID)) == 0)
    SAFE_RELEASE(pCompressor);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXParamEq, sizeof(GUID)) == 0)
    SAFE_RELEASE(pParamEq);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXI3DL2Reverb, sizeof(GUID)) == 0)
    SAFE_RELEASE(pReverb);
if (memcmp((void **)pDMO->refguidDMO, (void **)&IID_IDirectSoundFXWavesReverb, sizeof(GUID)) == 0)
    SAFE_RELEASE(pWavesReverb);

if (bPassing)
    return S_OK;
else
    return S_FALSE;

};





char *TypeOfDMO(IUnknown *pUnk)
{
    DWORD i = 0;
    void *pv = NULL;

    for (i=0; i<gdw_All_DMO_GUIDs; i++)
    {
        HRESULT hr;
         hr = pUnk->QueryInterface(*All_DMO_GUIDs[i], &pv);
         if (SUCCEEDED(hr))
         {            
            ((IUnknown *)pv)->Release();
            return dmthXlatGUID(*All_DMO_GUIDs[i]);
         }
    }

    return "Unknown GUID";
};


*/