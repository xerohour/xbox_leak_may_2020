/********************************************************************************


guidNotificationType
--------------------
Which of these are pertinent to XBox:
GUID_NOTIFICATION_MEASUREANDBEAT 
Measure and beat event. 
GUID_NOTIFICATION_PERFORMANCE 
Performance event, further defined in dwNotificationOption. 
GUID_NOTIFICATION_SEGMENT 
Segment event, further defined in dwNotificationOption. 
YES


dwNotificationOption 
--------------------
There are all pertinent to XBox.
Identifier of the notification subtype. 
If the notification type is GUID_NOTIFICATION_SEGMENT, this member can contain one of the following values: 
DMUS_NOTIFICATION_SEGABORT 
The segment was stopped prematurely, or was removed from the primary segment queue. 
DMUS_NOTIFICATION_SEGALMOSTEND 
The segment has reached the end minus the prepare time. 
DMUS_NOTIFICATION_SEGEND 
The segment has ended. 
DMUS_NOTIFICATION_SEGLOOP 
The segment has looped. 
DMUS_NOTIFICATION_SEGSTART 
The segment has started. 


If the notification type is GUID_NOTIFICATION_PERFORMANCE, this member can contain one of the following values: 
DMUS_NOTIFICATION_MUSICALMOSTEND 
The currently playing primary segment has reached the end minus the prepare time, and no more primary segments are cued to play. 
DMUS_NOTIFICATION_MUSICSTARTED 
Playback has started. 
DMUS_NOTIFICATION_MUSICSTOPPED 
Playback has stopped. 


********************************************************************************/


/********************************************************************************
IDirectMusicPerformance8::AddNotificationType()

//One piece of code can test both addage and removal.

HISTORY:
    Updated     04/05/00      danhaff - created.

//TODO: Since we're not looping yet, we don't get any loop messages.  We need
/       to write another test to check for these.
********************************************************************************/
struct PMSG_COUNTER
{
    DMUS_NOTIFICATION_PMSG * pMsg[32];
    DWORD                    dwCount;
    DWORD                    dwExpected;
};



template <class T>
HRESULT Template_AddRemoveNotifications(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes, DWORD dwRestarts, T* pDoNotUseDoNotRemove = NULL)
{
    HRESULT                     hr              = S_OK;
    CICMusic                    Music;
    CtIDirectMusicAudioPath     *ptAudioPath     = NULL;
    CtIDirectMusicSegment       *ptSegment8      = NULL;
    T                           *ptInterface     = NULL;
    DWORD                       i = 0;

    DMUS_NOTIFICATION_PMSG *    pMsg = NULL;

    PMSG_COUNTER PMsgCounter[3] = { 
                                    {{0}, 0, 32}, //Should have 2 x 16 notifications (2 because it plays twice)
                                    {{0}, 0, 2},  //DMUS_NOTIFICATION_MUSICSTARTED, DMUS_NOTIFICATION_MUSICALMOSTEND (not DMUS_NOTIFICATION_MUSICSTOPPED)
                                    {{0}, 0, 3}   //SEGSTART, SEGLOOP, SEGALMOSTEND, (not SEGEND)
                                  };

    BOOL bAdd[3] = {0};
    BOOL bRem[3] = {0};
    GUID *pGuid[3] = {(GUID *)&GUID_NOTIFICATION_MEASUREANDBEAT,
                      (GUID *)&GUID_NOTIFICATION_PERFORMANCE,
                      (GUID *)&GUID_NOTIFICATION_SEGMENT
                      };
    char *pszName[3] = {"GUID_NOTIFICATION_MEASUREANDBEAT",
                      "GUID_NOTIFICATION_PERFORMANCE",
                      "GUID_NOTIFICATION_SEGMENT"
                      };
    BOOL bNotificationsAdded = FALSE;    
    DWORD j;

    /*
    #define TEST_MEASUREANDBEAT 0
    #define TEST_PERFORMANCE    1
    #define TEST_SEGMENT        2
*/

    
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

    //Get the IDirectMusicSegment interface.
    CHECKRUN(Music.GetInterface(&ptInterface));

    //Stop the music for the moment.
    CHECKRUN(ptPerf8->StopEx(ptSegment8, __int64(0), 0));

    //Tell the segment to repeat once (play twice);
    CHECKRUN(ptSegment8->SetRepeats(1));

    //Set up the array of Booleans.
    for (i=0; i<3; i++)
    {
        bAdd[i] = dwAddNotificationTypes & (1 << i);
        bRem[i] = dwRemNotificationTypes & (1 << i);
    }


    //Add notification types.
    for (i=0; i<3; i++)
    {
        if (bAdd[i])
        {
            CHECKRUN(ptInterface->AddNotificationType(*pGuid[i]));
        }
    }

    //Remove notification types.
    for (i=0; i<3; i++)
    {
        if (bRem[i])
        {
            CHECKRUN(ptInterface->RemoveNotificationType(*pGuid[i]));
        }
    }
    bNotificationsAdded = TRUE;

    //Play and stop a specified # of times.
    for (i=0; i<dwRestarts; i++)
    {
        CHECKRUN(Log(FYILOGLEVEL, "Playing music with notifications on for 4 seconds.  Not polling for pMsgs"));
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, __int64(0), NULL, NULL, ptAudioPath));
        CHECKRUN(Wait(4000));
        CHECKRUN(ptPerf8->StopEx(ptSegment8, NULL, NULL));
        CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 5000, NULL, FALSE));
    }

    //Wipe out all pMsgs accumulated from that run.
    CHECKRUN(Log(FYILOGLEVEL, "Stopped music; clearing all pMsgs."));
    CHECKRUN(ClearAllPMsgs(ptPerf8));

    //Play on the audiopath.
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, __int64(0), NULL, NULL, ptAudioPath));

    //Wait for segment to start playing.
    CHECKRUN(Log(FYILOGLEVEL, "Started music; checking for pMsgs."));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL, FALSE));

    BOOL  bStoppedPlaying = FALSE;

    //Time to start recording the events that come in.   Do this for 10 seconds.
    if (SUCCEEDED(hr))
    {
        HRESULT hrMessage = S_OK;
        do
        {
            pMsg = NULL;
            if (SUCCEEDED(hr))
                hrMessage = ptPerf8->GetNotificationPMsg(&pMsg);

            //If we got one, then store it somewhere.
            if (S_OK == hrMessage)
            {
                if (!pMsg)
                {
                    hr = E_FAIL;
                    Log(ABORTLOGLEVEL, "GetNotificationPMsg returned %s (%08X) but pMsg was NULL!!", tdmXlatHRESULT(hr), hr);
                    break;
                }

                BOOL bFound = FALSE;            
                for (i=0; i<3; i++)
                {
                    if (memcmp((void *)&pMsg->guidNotificationType, (void *)pGuid[i], sizeof(GUID))==0)
                    {
                        MUSIC_TIME mtNow;
                        ptPerf8->GetTime(NULL, &mtNow);
                        Log(FYILOGLEVEL, "%2d. GUID: %s, dwNotificationOption: %d, Field1: %d, Field2: %d, SegState: %lx, ID: %ld ", 
                            PMsgCounter[i].dwCount, pszName[i], pMsg->dwNotificationOption, pMsg->dwField1, pMsg->dwField2, pMsg->punkUser, pMsg->dwVirtualTrackID);
                        if (PMsgCounter[i].dwCount == PMsgCounter[i].dwExpected)
                        {
                            hr = E_FAIL;
                            Log(ABORTLOGLEVEL, "**** ABORT: Got more than %d notifications for %s", PMsgCounter[i].dwExpected, pszName[i]);
                            if (pMsg)
                            {
                                ptPerf8->FreePMsg((DMUS_PMSG*)pMsg);
                                pMsg = NULL;
                            }
                        }
                        else
                        {                            
                            //Move the pMsg from the local variable to the array, which will have FreePMsg called on it later.
                            PMsgCounter[i].pMsg[PMsgCounter[i].dwCount] = pMsg;
                            PMsgCounter[i].dwCount++;                            
                            pMsg = NULL;  
                        }

                        bFound = TRUE;
                        break;
                    }
                }

                
                if (!bFound && pMsg) 
                {
                    ptPerf8->FreePMsg((DMUS_PMSG*)pMsg);
                    pMsg = NULL;
                }
            }

        
            if (!bStoppedPlaying && ptPerf8->IsPlaying(ptSegment8, NULL) != S_OK)
            {
                bStoppedPlaying = TRUE;
            }
        }
        while (!bStoppedPlaying || S_OK == hrMessage);
        
    }//end if (SUCCEEDED(hr))



    //Analyze the counts of each type according to the flags that were passed in.
    if (SUCCEEDED(hr))
    {
        for (i=0; i<3; i++)
        {
            if (bAdd[i] && !bRem[i])
            {
                //Make sure the count is the same.
                if (PMsgCounter[i].dwCount != PMsgCounter[i].dwExpected)
                {
                    hr = E_FAIL;
                    Log(ABORTLOGLEVEL, "**** ABORT: Got %d notifications for %s instead of %d",
                                          PMsgCounter[i].dwCount, pszName[i], PMsgCounter[i].dwExpected);
                }

                for (j=0; j<32; j++)
                {
                    if (PMsgCounter[i].pMsg[j])
                        if (memcmp((void *)&PMsgCounter[i].pMsg[j]->guidNotificationType, (void *)&GUID_NOTIFICATION_MEASUREANDBEAT, sizeof(GUID))==0)
                        {
                            //Check the individual pMsgs.
                            if (PMsgCounter[i].pMsg[j]->dwNotificationOption != DMUS_NOTIFICATION_MEASUREBEAT)
                            {
                                Log(ABORTLOGLEVEL, "PMsg %d's dwNotificationOption was %d instead of GUID_NOTIFICATION_MEASUREANDBEAT", j, PMsgCounter[i].pMsg[j]->dwNotificationOption);
                                hr = E_FAIL;
                            }

                            if (PMsgCounter[i].pMsg[j]->dwField1 != j%4)
                            {
                                Log(ABORTLOGLEVEL, "PMsg %d's dwField1 (BEAT) was %d instead of %d", j, PMsgCounter[i].pMsg[j]->dwField1, j%4);
                                hr = E_FAIL;
                            }

                            if (PMsgCounter[i].pMsg[j]->dwField2 != (j/4) % 4)
                            {
                                Log(ABORTLOGLEVEL, "PMsg %d's dwField2 (MEASURE) was %d instead of %d", j, PMsgCounter[i].pMsg[j]->dwField2, (j/4) % 4);
                                hr = E_FAIL;
                            }
                        }
                }
            }
            else
            {
                if (PMsgCounter[i].dwCount !=  0)
                {
                    hr = E_FAIL;
                    Log(ABORTLOGLEVEL, "**** ABORT: Got %d notifications for %s instead of 0", PMsgCounter[i].dwCount, pszName[i]);
                }
            }
        }
    }

    for (i=0; i<3; i++)
    {
        //Release all the NotificationMeasureAndBeat pMsgs.
        for (j=0; j<32; j++)
        {
            if (PMsgCounter[i].pMsg[j])
            {
                ptPerf8->FreePMsg((DMUS_PMSG*)PMsgCounter[i].pMsg[j]);
                PMsgCounter[i].pMsg[j] = NULL;
            }
        }
    }


    //Remove any notifications that were added.
    if (bNotificationsAdded)
    {
        for (i=0; i<3; i++)
        {
            if (bAdd[i] && !bRem[i])
            {
                if (ptInterface)
                {
                    ptInterface->RemoveNotificationType(*pGuid[i]);
                }
            }
        }
    }

   SAFE_RELEASE(ptInterface);
   SAFE_RELEASE(ptAudioPath);
   SAFE_RELEASE(ptSegment8);

    return hr;
    
}

