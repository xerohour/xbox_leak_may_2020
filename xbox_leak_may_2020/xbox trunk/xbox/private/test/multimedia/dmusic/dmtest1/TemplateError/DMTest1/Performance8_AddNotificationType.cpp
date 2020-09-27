#include "globals.h"
#include "cicmusicx.h"



DWORD Performance8_AddRemoveNotifications(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwParam1, DWORD dwParam2);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_AddNotificationType (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_AddNotificationType())");

    //These tests take care of Add, Remove, and GetNotificationPMsg.
    //These are called first.
    //dwParam3.  bit 1 = AddNotification(MeasureAndBeat)
    //dwParam3.  bit 2 = AddNotification(Performance)
    //dwParam3.  bit 3 = AddNotification(Segment)

    //These are called second.
    //dwParam4.  bit 1 = RemoveNotification(MeasureAndBeat)
    //dwParam4.  bit 2 = RemoveNotification(Performance)
    //dwParam4.  bit 3 = RemoveNotification(Segment)    

    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 1, 0));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 2, 0));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 4, 0));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 7, 0));       

    //Make sure that each Removal cancels the other ones ou
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 1, 1));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 2, 2));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 4, 4));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 7, 7));       

    //Make sure that each removal.is the correct one.
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 1, 2));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 2, 4));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 4, 1));

    //Make sure that duplicate removals don't hurt anything
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 0, 1));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 0, 2));       
    CALLDMTEST(Performance8_AddRemoveNotifications(ptPerf8, 0, 4));
    return hr;
};



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
    DMUS_NOTIFICATION_PMSG * pMsg[48];
    DWORD                    dwCount;
    DWORD                    dwExpected;
};

DWORD Performance8_AddRemoveNotifications(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwParam1, DWORD dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    CtIDirectMusicAudioPath     *ptAudioPath     = NULL;
    CtIDirectMusicSegment8      *ptSegment8        = NULL;
    DWORD                       i = 0;
    HANDLE hNotify            = NULL;

    DMUS_NOTIFICATION_PMSG *    pMsg = NULL;

    PMSG_COUNTER PMsgCounter[3] = { 
                                    {{0}, 0, 48}, //Should have 3 x 16 notifications (3 because of the tracks)
                                    {{0}, 0, 3},
                                    {{0}, 0, 3}
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

    
    //We should get 16 of these.
    DMUS_NOTIFICATION_PMSG *    pMsgNotificationMeasureAndBeat[16] = {0};

    //Should get 3 of these.
    DMUS_NOTIFICATION_PMSG *    pMsgNotificationPerformance[3] = {0};

    //Should get 3 of these, even though there's a total of 5.
    //  We'll test for those later.
    DMUS_NOTIFICATION_PMSG *    pMsgNotificationSegment[3] = {0};

    DWORD dwAddNotificationTypes = dwParam1;
    DWORD dwRemNotificationTypes = dwParam2;

    DWORD dwStartTime;
    DWORD dwDeltaTime;
    DWORD j;

    /*
    #define TEST_MEASUREANDBEAT 0
    #define TEST_PERFORMANCE    1
    #define TEST_SEGMENT        2
*/


    //Create a handle.
    hNotify = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!hNotify)
        goto TEST_END;
    
    //Start the music playing.
    hr = Music.Init(ptPerf8, "T:\\Media\\Perf8\\AddNot\\Notify.sgt", DMUS_APATH_DYNAMIC_STEREO);
    if (S_OK != hr)
        goto TEST_END;

    //Get the IDirectMusicAudioPath interface.
    hr = Music.GetInterface(&ptAudioPath);
    if (S_OK != hr)
        goto TEST_END;

    //Get the IDirectMusicSegment interface.
    hr = Music.GetInterface(&ptSegment8);
    if (S_OK != hr)
        goto TEST_END;


    //Stop the music for the moment.
    hr = ptPerf8->StopEx(ptSegment8, 0, 0);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }


    //Tell the segment to repeat twice.
    hr = ptSegment8->SetRepeats(2);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: SetRepeats() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

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
            hr = ptPerf8->AddNotificationType(*pGuid[i]);
            if (S_OK != hr)
            {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: AddNotificationType(%s) returned %s (%08Xh)",pszName[i],tdmXlatHRESULT(hr), hr);
                goto TEST_END;
            }
        }
    }

    //Remove notification types.
    for (i=0; i<3; i++)
    {
        if (bRem[i])
        {
            hr = ptPerf8->RemoveNotificationType(*pGuid[i]);
            if (S_OK != hr)
            {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: RemNotificationType(%s) returned %s (%08Xh)",pszName[i],tdmXlatHRESULT(hr), hr);
                goto TEST_END;
            }
        }
    }


    //Play on the audiopath.
    hr = ptPerf8->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, NULL, NULL, NULL, ptAudioPath);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    //Wait for segment to start playing.
    fnsLog(FYILOGLEVEL, "Waiting for segment to start playing");
    while (ptPerf8->IsPlaying(ptSegment8, NULL) != S_OK);

    //Time to start recording the events that come in.   Do this for 10 seconds.
    dwStartTime = timeGetTime();
    dwDeltaTime = 0;
    do
    {
        pMsg = NULL;
        hr = ptPerf8->GetNotificationPMsg(&pMsg);
        if (FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: GetNotificationPMsg() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
            goto TEST_END;
        }


        //If we got one, then store it somewhere.
        if (S_OK == hr)
        {
            for (i=0; i<3; i++)
            {
                if (memcmp((void *)&pMsg->guidNotificationType, (void *)&pGuid[i], sizeof(GUID)))
                {
                    fnsLog(FYILOGLEVEL, "Got notification %d for %s", PMsgCounter[i].dwCount, pszName[i]);
                    if (PMsgCounter[i].dwCount == PMsgCounter[i].dwExpected)
                    {
                        fnsLog(ABORTLOGLEVEL, "**** ABORT: Got more than %d notifications for GUID_NOTIFICATION_MEASUREANDBEAT", PMsgCounter[i].dwExpected);
                        goto TEST_END;
                    }

                    PMsgCounter[i].pMsg[PMsgCounter[i].dwCount] = pMsg;
                    PMsgCounter[i].dwCount++;

                    break;
                }
            }        
        }
        dwDeltaTime = timeGetTime() - dwStartTime;
    }
    //while (dwDeltaTime < 10000);
    while (ptPerf8->IsPlaying(ptSegment8, NULL) == S_OK);


    //TODO: Go through all the PMsgs and analyze them.


    //Analyze the counts of each type according to the flags that were passed in.
    for (i=0; i<3; i++)
    {
        if (bAdd[i] && !bRem[i])
        {
            if (PMsgCounter[i].dwCount != PMsgCounter[i].dwExpected)
            {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: Got %d notifications for %s instead of %d",
                                      PMsgCounter[i].dwCount, pszName[i], PMsgCounter[i].dwExpected);
                goto TEST_END;
            }
        }
        else
        {
            if (PMsgCounter[i].dwCount !=  0)
            {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: Got %d notifications for %s instead of 0", PMsgCounter[i].dwCount, pszName[i]);
                goto TEST_END;
            }
        }
    }



    for (i=0; i<3; i++)
    {
        //Release all the NotificationMeasureAndBeat pMsgs.
        for (j=0; j<16; j++)
        {
            if (PMsgCounter[i].pMsg[j])
            {
                hr = ptPerf8->FreePMsg((DMUS_PMSG*)PMsgCounter[i].pMsg[j]);
                if (S_OK != hr)
                {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: FreePMsg() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
                    goto TEST_END;
                }
                PMsgCounter[i].pMsg[j] = NULL;
            }
        }
    }

    dwRes = FNS_PASS;

TEST_END:
   SAFE_RELEASE(ptAudioPath);
   SAFE_RELEASE(ptSegment8);
   if (hNotify)
        CloseHandle(hNotify);

    return dwRes;
    
}

