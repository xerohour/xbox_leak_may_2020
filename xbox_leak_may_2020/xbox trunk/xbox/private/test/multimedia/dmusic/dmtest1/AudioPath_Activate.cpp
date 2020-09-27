#include "globals.h"
#include "cicmusicx.h"



/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT AudioPath_Activate_BVT(CtIDirectMusicPerformance8 *ptPerf8  , DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath     = NULL;


    //Load default segment.
    CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &ptSegment8));

    //Create an inactive audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,16,FALSE,&ptPath));

    //Activate(TRUE) on inactive path.
    CHECKRUN(ptPath->Activate(TRUE));

    //Try to play a segment on it; verify you hear sound.
    CHECKRUN(ptPerf8->PlaySegmentEx( 
        ptSegment8, 
        0,
        NULL,
        0, 
        0,
        NULL,
        NULL,
        ptPath));
    Log(FYILOGLEVEL, "Playing segment on active path, should hear sound for 3 seconds.");
    CHECKRUN(Wait(3000));

    //CLEANUP

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);
    return hr;

};


/********************************************************************************
Go from INACTIVE->INACTIVE, INACTIVE->ACTIVE, ACTIVE->ACTIVE, ACTIVE->INACTIVE
********************************************************************************/
HRESULT AudioPath_Activate_Valid_OnAndOff(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath     = NULL;

    //Load default segment.
    CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &ptSegment8));

    //Create an inactive audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,16,FALSE,&ptPath));

    //Activate(FALSE) on inactive path.
    CHECKRUN(ptPath->Activate(FALSE));
    Log(FYILOGLEVEL, "Calling Activate(FALSE) on inactive path, should hear no sound for 3 seconds.");
    CHECKRUN(Wait(3000));

    //Activate(TRUE) on inactive path.
    CHECKRUN(ptPath->Activate(TRUE));
    Log(FYILOGLEVEL, "Calling Activate(TRUE) on inactive path, should hear no sound for 3 seconds.");
    CHECKRUN(Wait(3000));

    //Try to play a segment on it; verify you don't hear any sound.
    CHECKRUN(ptPerf8->PlaySegmentEx( 
        ptSegment8, 
        0,
        NULL,
        0, 
        0,
        NULL,
        NULL,
        ptPath));
    Log(FYILOGLEVEL, "Playing segment on active path, should hear sound for 3 seconds.");
    CHECKRUN(Wait(3000));


    //Activate(TRUE) on active path.
    CHECKRUN(ptPath->Activate(TRUE));
    Log(FYILOGLEVEL, "Calling Activate(TRUE) on active path, should hear sound for 3 seconds.");
    CHECKRUN(Wait(3000));

    //Activate(FALSE) on active path.
    CHECKRUN(ptPath->Activate(FALSE));
    Log(FYILOGLEVEL, "Calling Activate(FALSE) on active path, should hear no sound for 3 seconds.");
    CHECKRUN(Wait(3000));

    //Activate(TRUE) on deactivated path.
    CHECKRUN(ptPath->Activate(TRUE));
    Log(FYILOGLEVEL, "Calling Activate(TRUE) on deactivated path, no sound expected.");
    CHECKRUN(Wait(3000));

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);
    return hr;
};





/********************************************************************************
Create 999 Inactive paths, then activate them one at a time.
********************************************************************************/
HRESULT AudioPath_Activate_Valid_Many(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwPathType, DWORD dwCount)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath[1000]     = {NULL};
DWORD i=0;

    //Load default segment.
    CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &ptSegment8));

    //Create dwCount inactive paths.
    for (i=0; i<dwCount; i++)
    {
        CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPathType,16,FALSE,&ptPath[i]));
        if (SUCCEEDED(hr))
            Log(MAXLOGLEVEL, "Created path %d", i);
        else
            Log(ABORTLOGLEVEL, "Failed to create audiopath #%d (0-based)", i);
    }

    for (i=0; i<dwCount+1 && SUCCEEDED(hr); i++)
    {   
        if(i < dwCount) {
            //Activate(TRUE) on inactive path.
            CHECKRUN(ptPath[i]->Activate(TRUE));
            Log(MAXLOGLEVEL, "Activated path %d", i);

            //Try to play a segment on it; verify you don't hear any sound.
            CHECKRUN(ptPerf8->PlaySegmentEx( 
                ptSegment8, 
                0,
                NULL,
                0, 
                0,
                NULL,
                NULL,
                ptPath[i]));
        }

        //Deactivate.
        if (i>0)
        {
            CHECKRUN(ptPath[i-1]->Activate(FALSE));
            Log(MAXLOGLEVEL, "Deactivated path %d", i-1);            
        }
    };

    //Stop everything.
    ptPerf8->StopEx(0, 0, 0);

    SAFE_RELEASE(ptSegment8);
    for (i=0; i<dwCount; i++)
    {
        SAFE_RELEASE(ptPath[i]);
    }

    Sleep(5000);
    return hr;
};






/********************************************************************************
Creates two paths of one type.  If it's a shared path, then activate should deactivate
both paths.  If it's not, then activate(FALSE) should only deactivate one path.
********************************************************************************/
HRESULT AudioPath_Activate_Valid_Sharing(CtIDirectMusicPerformance8 *ptPerf8, DWORD bShared, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8[2] = {NULL};
CtIDirectMusicAudioPath *ptPath[2]     = {NULL};
LPSTR szSegment[2] = {"t:\\DMTest1\\test.sgt", "t:\\DMTest1\\test2.sgt"};
int i=0;
DWORD dwPathType= bShared ? DMUS_APATH_SHARED_STEREOPLUSREVERB : DMUS_APATH_DYNAMIC_MONO;

MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"SGT/test2.sgt",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };

    CHECKRUN(LoadMedia(MediaFileDesc));

    //Load default segment.
    for (i=0; i<2; i++)
    {
        CHECKRUN(dmthLoadSegment(szSegment[i], &ptSegment8[i]));
        CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPathType,32,TRUE,&ptPath[i]));

        //Try to play a segment on it; verify you don't hear any sound.
        CHECKRUN(ptPerf8->PlaySegmentEx( 
            ptSegment8[i], 
            0,
            NULL,
            i ? DMUS_SEGF_SECONDARY : 0, 
            0,
            NULL,
            NULL,
            ptPath[i]));

    };

    //Play both segments for a couple seconds.
    Log(FYILOGLEVEL, "You should hear both segments playing simultaneously for 3 seconds");
    CHECKRUN(Wait(3000));


    //Now call Activate FALSE on one of the paths.
    CHECKRUN(ptPath[1]->Activate(FALSE));

    Log(FYILOGLEVEL, "Calling activate(FALSE) on a %s path.", bShared ? "shared" : "dynamic");
    Log(FYILOGLEVEL, "You should hear %s segment(s) playing for 3 seconds", bShared ? "both" : "one");
    CHECKRUN(Wait(3000));

    //Stop everything.
    CHECKRUN(ptPerf8->StopEx(0,0,0));


    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment8[i]);
        SAFE_RELEASE(ptPath[i]);
    }

    return hr;
};
