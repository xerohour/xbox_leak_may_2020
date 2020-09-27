#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_GetNotificationPMsg_Valid(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


HRESULT Performance8_GetNotificationPMsg_BVT(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Performance8_GetNotificationPMsg_Valid(ptPerf8, dwUnused1, dwUnused2);
}


/********************************************************************************
********************************************************************************/
#define MAX 100
HRESULT Performance8_GetNotificationPMsg_Valid(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT                     hr              = S_OK;
    CICMusic                    Music;
    CtIDirectMusicAudioPath     *ptAudioPath     = NULL;
    CtIDirectMusicSegment8      *ptSegment8        = NULL;
    DWORD                       i = 0;
//    DMUS_NOTIFICATION_PMSG *    pMsg = NULL;
    DMUS_NOTIFICATION_PMSG *    pMsgStored[MAX] = {0};
    DWORD                       dwStored = 0;
/*
    GUID *pGuid[3] = {(GUID *)&GUID_NOTIFICATION_MEASUREANDBEAT,
                      (GUID *)&GUID_NOTIFICATION_PERFORMANCE,
                      (GUID *)&GUID_NOTIFICATION_SEGMENT
                      };
    char *pszName[3] = {"GUID_NOTIFICATION_MEASUREANDBEAT",
                      "GUID_NOTIFICATION_PERFORMANCE",
                      "GUID_NOTIFICATION_SEGMENT"
                      };
*/
    BOOL bNotificationsAdded = FALSE;    
    DWORD j;

    MEDIAFILEDESC MediaFileDesc[] = {
                                    {"DMusic/DMTest1/Perf8/AddNot/Notify.sgt", "T:\\DMTest1\\Perf8\\AddNot\\",    COPY_IF_NEWER},
                                    {"DLS/Main1.DLS",          "T:\\DMTest1\\Perf8\\AddNot\\",    COPY_IF_NEWER},
                                    {TERMINATE}
                                    };
    CHECKRUN(LoadMedia(MediaFileDesc));
   
    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Perf8\\AddNot\\Notify.sgt", DMUS_APATH_DYNAMIC_MONO));

    //Get the IDirectMusicAudioPath interface.
    CHECKRUN(Music.GetInterface(&ptAudioPath));

    //Get the IDirectMusicSegment interface.
    CHECKRUN(Music.GetInterface(&ptSegment8));

    //Stop the music for the moment.
    CHECKRUN(ptPerf8->StopEx(ptSegment8, __int64(0), 0));

    //Tell the segment to repeat once (play twice);
    CHECKRUN(ptSegment8->SetRepeats(1));

    CHECKRUN(ptPerf8->AddNotificationType(GUID_NOTIFICATION_SEGMENT));
    bNotificationsAdded = TRUE;

    CHECKRUN(ClearAllPMsgs(ptPerf8));

    //Play and stop a specified # of times.
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, __int64(0), NULL, NULL, ptAudioPath));

    //Wait for segment to start playing.
    CHECKRUN(Log(FYILOGLEVEL, "Started music; checking for pMsgs."));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL, FALSE));

    //Get and free everything that comes in.
    if (SUCCEEDED(hr))
    {
        HRESULT hrPMSG = S_OK;
        do
        {
            hrPMSG= ptPerf8->GetNotificationPMsg(&pMsgStored[dwStored++]);
            if (dwStored > MAX)
            {
                Log(ABORTLOGLEVEL, "Got over %d pMsgs", MAX);
                break;
            }
        }
        while (ptPerf8->IsPlaying(ptSegment8, NULL) == S_OK);
    }//end if (SUCCEEDED(hr))

    for (i=0; i<MAX; i++)
    {
        if (pMsgStored[i])
        {
            ptPerf8->FreePMsg((DMUS_PMSG *)pMsgStored[i]);
            pMsgStored[i] = NULL;
        }
    }

    //Remove any notifications that were added.
    if (bNotificationsAdded)
    {
        ptPerf8->RemoveNotificationType(GUID_NOTIFICATION_SEGMENT);
    }


   SAFE_RELEASE(ptAudioPath);
   SAFE_RELEASE(ptSegment8);

    return hr;
    
}




