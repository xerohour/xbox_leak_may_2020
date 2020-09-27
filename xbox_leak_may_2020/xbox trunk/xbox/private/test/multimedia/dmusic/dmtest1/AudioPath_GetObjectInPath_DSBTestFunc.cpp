#include "globals.h"
#include "audiopath_getobjectinpath_DSBTestFunc.h"

/********************************************************************************
Orient a cone away from the listener, then expand the angles to encompass the 
listener.  Try SetAllParameters with dwApply on/off and see whether it changes.
********************************************************************************/
HRESULT DMTest_Buf_SetAllParameters(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DS3DBUFFER ds3db;
char *pStrings[] = {"Using DS3D_IMMEDATE", "Using DS3D_DEFERRED, then CommitDeferredSettings", "Using DS3D_DEFERRED but not CommitDeferredSettings - change shouldn't occur"};
DWORD dwWait = 4000;

ZeroMemory(&ds3db, sizeof(ds3db));


Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetAllParameters:  eTestApply = %s", String(eTestApply));

//Set up our 3DBuffer so that the parameters are such that you shouldn't hear any sound.
ds3db.dwInsideConeAngle         = 0;
ds3db.dwOutsideConeAngle        = 0;
ds3db.dwMode                    = DS3DMODE_NORMAL;  //this doesn't matter anyway, it's goverened by SetMode.
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = FLT_BIG;
ds3db.flMaxDistance             = FLT_BIG;
ds3db.lConeOutsideVolume        = DSBVOLUME_MIN;
ds3db.vConeOrientation          = make_D3DVECTOR(0, 0, 1);  //away
ds3db.vPosition                 = make_D3DVECTOR(FLT_BIG, FLT_BIG, FLT_BIG);
ds3db.vVelocity                 = make_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;

//Play sound, shouldn't hear anything.  Use Immediate mode to set these parameters so they'll always be set.
CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));
Log(FYILOGLEVEL, "Sound is a meter ahead of you and facing away without outside volume minimum:  You should hear nothing for %d ms", dwWait);
CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
Wait(dwWait);

//Set up our 3DBuffer so that the parameters are such that you shouldn't hear any sound.
ds3db.dwInsideConeAngle         = DS3D_DEFAULTCONEANGLE;
ds3db.dwOutsideConeAngle        = DS3D_DEFAULTCONEANGLE;
ds3db.dwMode                    = DS3DMODE_NORMAL;  //this doesn't matter anyway, it's goverened by SetMode.
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = DS3D_DEFAULTMINDISTANCE;
ds3db.flMaxDistance             = DS3D_DEFAULTMAXDISTANCE;
ds3db.lConeOutsideVolume        = DSBVOLUME_MAX;
ds3db.vConeOrientation          = make_D3DVECTOR(0, 0, -1);  //toward
ds3db.vPosition                 = make_D3DVECTOR(0, 0, 0);
ds3db.vVelocity                 = make_D3DVECTOR(0, 0, 0);


//Now that everything is set to pretty much default, we should hear normal sound.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));
Log(FYILOGLEVEL, "Calling SetAllParameters with default parameters: You should hear sound at full volume for %d ms.", dwWait);
ds3db.lConeOutsideVolume = DSBVOLUME_MAX;
CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
Wait(dwWait);
return hr;
}



/********************************************************************************
Rotates between several sets of cone angles and asks user to verify the
correct sound came out.
Fails on software due to 2431; hopefully will succeed on hardware.
********************************************************************************/
HRESULT DMTest_Buf_SetConeAngles(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DS3DBUFFER ds3db;
DS3DLISTENER ds3dl ;
BOOL bPlaying = FALSE;
DWORD dwVolumes[2]          = {DSBVOLUME_MIN, DSBVOLUME_MAX};
DWORD dwAngle;
float fAngle;
D3DVECTOR vLisPos = {0};

struct TESTCASE
{
DWORD dwInsideConeAngle;
DWORD dwOutsideConeAngle;
char *szBehavior;
};


DWORD dwTestCase = 0;

TESTCASE TestCase[] = 
{
{0,     0,  "No sound should be heard"},
{360,   360,"Should should be heard continuously"},
{0,     360,"Sound should slowly fade to maximum"},
{180,   360,"Sound should fade to maximum for time=x and remain there for time=x."},
{0,     180,"Sound should be silent for time=x and fade in over time=x"},
{1,     359, "Sound should slowly fade to maximum"},
};

ZeroMemory(&ds3db, sizeof(ds3db));
ZeroMemory(&ds3dl, sizeof(ds3dl));


//Preset our starting buffer and listener positions.
//Position source at 0, 0, 0.  Point the cone forward.
ds3db.dwInsideConeAngle         = TestCase[dwTestCase].dwInsideConeAngle;           //N/A to be adjusted in loop
ds3db.dwOutsideConeAngle        = TestCase[dwTestCase].dwInsideConeAngle;           //N/A to be adjusted in loop 
ds3db.dwMode                    = DS3DMODE_NORMAL;
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = DS3D_DEFAULTMINDISTANCE;
ds3db.flMaxDistance             = DS3D_DEFAULTMAXDISTANCE;
ds3db.lConeOutsideVolume        = DSBVOLUME_MIN;
ds3db.vConeOrientation          = make_D3DVECTOR(0, 0, 1);
ds3db.vPosition                 = make_D3DVECTOR(0, 0, 0);
ds3db.vVelocity                 = make_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;


//Set up the 3D Listener at 0, 0, -1, pointing forward too.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = make_D3DVECTOR(0, 0, 1);         //facing forward.
ds3dl.vOrientTop                = make_D3DVECTOR(0, 1, 0);         //right side up
ds3dl.vPosition                 = make_D3DVECTOR(0, 0, -1);        //behind the source at (0, 0, 0);
ds3dl.vVelocity                 = make_D3DVECTOR(0, 0, 0);         //will be derivatives of positioning functions cos and sin.



Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetConeAngles: eTestApply = %s", String(eTestApply));


for (dwTestCase = 0; dwTestCase < NUMELEMS(TestCase); dwTestCase++)
{
    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set those source parameters.
    ds3db.dwInsideConeAngle         = TestCase[dwTestCase].dwInsideConeAngle;
    ds3db.dwOutsideConeAngle        = TestCase[dwTestCase].dwInsideConeAngle;
    CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
    CHECKRUN(pSound->DS_SetAllParameters(&ds3dl));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Set up our coneangles as per the test case.
    CHECKRUN(pSound->DSB_SetConeAngles(TestCase[dwTestCase].dwInsideConeAngle, TestCase[dwTestCase].dwOutsideConeAngle));
    
    //Print what is supposed to happen.
    Log(FYILOGLEVEL, "Moving listener from behind source (270) to in front of source (90)");
    Log(FYILOGLEVEL, "ConeAngles are (%3d,%3d) %s", TestCase[dwTestCase].dwInsideConeAngle, TestCase[dwTestCase].dwOutsideConeAngle, TestCase[dwTestCase].szBehavior);
    Log(FYILOGLEVEL, "---------------------------------------------------");
        
    //Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
    bPlaying = FALSE;
    for (dwAngle = 270; dwAngle >= 90  && SUCCEEDED(hr); dwAngle--)
    {
        fAngle = float(((float)dwAngle) * PI / 180.0f);
        vLisPos.x = (float)cos(fAngle);
        vLisPos.z = (float)sin(fAngle);

        CHECKRUN(pSound->DS_SetPosition(vLisPos.x, vLisPos.y, vLisPos.z));
        Wait(5000 / 180);                  //want to spend 5s; going through loop 180 times

        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        if (dwAngle % 20 == 0)
            Log(FYILOGLEVEL, "Listener relative to source: %3d degrees.  %2d percent finished w/sweep", dwAngle, (270 - dwAngle) * 100 / 180);

    }

    CHECKRUN(pSound->DSB_Stop());
}

;
return hr;
}








/********************************************************************************
So we want to test the following vectors with negative numbers.
a) testing 1, FLT_BIG, and FLT_SMALL.
b) testing negative numbers.

Example bunch -0- test cases.

{FLT_BIG, 0      , 0      },
{0      , FLT_BIG, 0      },
{0      , 0      , FLT_BIG},
{FLT_BIG, FLT_BIG, 0      },
{FLT_BIG, 0      , FLT_BIG},
{0      , FLT_BIG, FLT_BIG},
{FLT_BIG, FLT_BIG, FLT_BIG},

********************************************************************************/
HRESULT DMTest_Buf_SetConeOrientation_BoundaryCases(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DWORD dwIterator = 0;


//A couple random positions to test with.
D3DVECTOR vBufPos[] = {{43, -28, 90}, {.2f, -.7f, 2.2f}};
D3DVECTOR vLisPos[] = {{234, 22, 4},  {-2, -.02f , 1.8f}};
DWORD dwBufPos, dwLisPos;

//Test values.
FLOAT fTestValues[8] = {1, FLT_BIG, FLT_BIG, FLT_SMALL, -1, -FLT_BIG, -FLT_BIG, -FLT_SMALL};
DWORD dwTestValue = 0;

D3DVECTOR v = {0};



    Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetConeOrientation_BoundaryCases: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Play the sound!!
    Log(FYILOGLEVEL, "Playing Sound");
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Try all these values.  With all possible values passed in - about 800 iterations.

    for (dwBufPos = 0; dwBufPos > NUMELEMS(vBufPos)  && SUCCEEDED(hr); dwBufPos++)
    {
        CHECKRUN(pSound->DSB_SetPosition(vBufPos[dwBufPos].x, vBufPos[dwBufPos].y, vBufPos[dwBufPos].z));
        for (dwLisPos = 0; dwLisPos > NUMELEMS(vLisPos); dwLisPos++)
        {
            CHECKRUN(pSound->DS_SetPosition(vLisPos[dwBufPos].x, vLisPos[dwBufPos].y, vLisPos[dwBufPos].z));
            for (dwTestValue = 0; dwTestValue < NUMELEMS(fTestValues); dwTestValue++)
            {
                for (dwIterator = 0; dwIterator < 8; dwIterator ++)
                {
                    v.x = dwIterator & (1 << 0) ? fTestValues[dwTestValue] : 0;
                    v.y = dwIterator & (1 << 1) ? fTestValues[dwTestValue] : 0;
                    v.z = dwIterator & (1 << 2) ? fTestValues[dwTestValue] : 0;

                    CHECKRUN(pSound->DSB_SetConeOrientation(v.x, v.y, v.z));
                    Wait(10);
                }
            }
        }
    }

    Log(FYILOGLEVEL, "Stopping Sound");

    ;
    return hr;
};



/********************************************************************************
Spins the buffer cone around and asks user to verify that the volume is attenuated 
properly.

This will fail on software any case where the sound is between the coneangles due to
bug 2431.  Hopefully it will succeed on hardware.
********************************************************************************/
HRESULT DMTest_Buf_SetConeOrientation_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DS3DBUFFER ds3db;
DS3DLISTENER ds3dl;
BOOL bPlaying = FALSE;
DWORD dwVolumes[2]          = {DSBVOLUME_MIN, DSBVOLUME_MAX};
DWORD dwAngle;
float fAngle;
D3DVECTOR vLisPos = {0};


DWORD dwTestCase = 0;
ZeroMemory(&ds3db, sizeof(ds3db));
ZeroMemory(&ds3dl, sizeof(ds3dl));


//Preset our starting buffer and listener positions.
//Position source at 0, 0, 1.  Point the cone backward toward the listener (0, 0, -1).
ds3db.dwInsideConeAngle         = 0;
ds3db.dwOutsideConeAngle        = 360;
ds3db.dwMode                    = DS3DMODE_NORMAL;                              //this doesn't matter anyway, it's goverened by SetMode.
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = DS3D_DEFAULTMINDISTANCE;
ds3db.flMaxDistance             = DS3D_DEFAULTMAXDISTANCE;
ds3db.lConeOutsideVolume        = DSBVOLUME_MIN;
ds3db.vConeOrientation          = make_D3DVECTOR(0, 0, -1);        //Looking back toward the listener
ds3db.vPosition                 = make_D3DVECTOR(0, 0, 1);
ds3db.vVelocity                 = make_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;

//Set up the 3D Listener at 0, 0, 0, pointing forward too.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = make_D3DVECTOR(0, 0, 1);         //facing forward.
ds3dl.vOrientTop                = make_D3DVECTOR(0, 1, 0);         //right side up
ds3dl.vPosition                 = make_D3DVECTOR(0, 0, 0);         //at origin, behind the source.
ds3dl.vVelocity                 = make_D3DVECTOR(0, 0, 0);         //will be derivatives of positioning functions cos and sin.


    //Init
        Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetConeOrientation_Listening: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set starting parameters.
    CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
    CHECKRUN(pSound->DS_SetAllParameters(&ds3dl));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Print what is supposed to happen.
    Log(FYILOGLEVEL, "Cone will start pointing toward the user, and rotate 360 degrees (away and then back toward the user)");
    Log(FYILOGLEVEL, "  The sound should fade out and then in over a period of 5 seconds.");
        
    //Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
    bPlaying = FALSE;
    for (dwAngle = 0; dwAngle <=360  && SUCCEEDED(hr); dwAngle++)
    {        
        //make zero be facing backwards.
        fAngle = float(((float)dwAngle + 270) * PI / 180.0f);
        vLisPos.x = (float)cos(fAngle);
        vLisPos.z = (float)sin(fAngle);

        CHECKRUN(pSound->DSB_SetConeOrientation(vLisPos.x, vLisPos.y, vLisPos.z));
        Wait(5000 / 360);                  //want to spend 5s; going through loop 360 times

        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        if (dwAngle % 20 == 0)
            Log(FYILOGLEVEL, "Listener angle (0 = pointing at source): %3d degrees.  %2d percent finished w/sweep", dwAngle, dwAngle * 100 / 360);

    }

    CHECKRUN(pSound->DSB_Stop());

    ;
    return hr;
}




/********************************************************************************
Fails on software due to 2431; hopefully will succeed on hardware.
********************************************************************************/
HRESULT DMTest_Buf_SetConeOutsideVolume(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DS3DBUFFER ds3db;
DS3DLISTENER ds3dl;
BOOL bPlaying = FALSE;
LONG lOutsideVolume = 0;

ZeroMemory(&ds3db, sizeof(ds3db));
ZeroMemory(&ds3dl, sizeof(ds3dl));

//Preset our starting buffer and listener positions.
//Position source at 0, 0, 1.  Point the cone away from the listener (0, 0, 1).
ds3db.dwInsideConeAngle         = 0;
ds3db.dwOutsideConeAngle        = 359;
ds3db.dwMode                    = DS3DMODE_NORMAL;
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = DS3D_DEFAULTMINDISTANCE;
ds3db.flMaxDistance             = DS3D_DEFAULTMAXDISTANCE;
ds3db.lConeOutsideVolume        = DSBVOLUME_MIN;
ds3db.vConeOrientation          = make_D3DVECTOR(0, 0, 1);        //Facing away from the listener.
ds3db.vPosition                 = make_D3DVECTOR(0, 0, 1);
ds3db.vVelocity                 = make_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;

//Set up the 3D Listener at 0, 0, 0, pointing forward too.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = make_D3DVECTOR(0, 0, 1);         //facing forward.
ds3dl.vOrientTop                = make_D3DVECTOR(0, 1, 0);         //right side up
ds3dl.vPosition                 = make_D3DVECTOR(0, 0, 0);         //at origin, behind the source.
ds3dl.vVelocity                 = make_D3DVECTOR(0, 0, 0);         //would be derivatives of positioning functions cos and sin, if we adjusted this.


        Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetConeOutsideVolume: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set starting parameters.
    CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
    CHECKRUN(pSound->DS_SetAllParameters(&ds3dl));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));
    bPlaying = FALSE;
    LONG lIncValue = 1000;
    DWORD dwTime     = 10000;
    Log(FYILOGLEVEL, "Listener is on outside of sound cone.  Will increase outside volume from -10000 (silence) to 0 (max) over %d seconds", dwTime / 1000);
    for (lOutsideVolume = DSBVOLUME_MIN; lOutsideVolume <= DSBVOLUME_MAX  && SUCCEEDED(hr); lOutsideVolume+=lIncValue)    {        

        CHECKRUN(pSound->DSB_SetConeOutsideVolume(lOutsideVolume));
        if (lOutsideVolume % lIncValue == 0)
            Log(FYILOGLEVEL, "Outside Volume is %5d", lOutsideVolume);
        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        Wait(dwTime / ((DSBVOLUME_MAX - DSBVOLUME_MIN) / lIncValue));
    }

    CHECKRUN(pSound->DSB_Stop());

    ;
    return hr;
}





/********************************************************************************
We set a sound source 50m away.  Then we move MaxDistance from 2 to 100m.
When this value hits 50 the sound should turn on.  However, it turns on 
immediately due to

2478	incrementing IDirectBUFFER3DBuffer::SetMaxDistance will incorrectly enable far-away sound.

This should be fixed when we replace the software with hardware.
********************************************************************************/
HRESULT DMTest_Buf_SetMaxDistance_InsideSource(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
BOOL bPlaying = FALSE;
DWORD dwMaxDistance = 0;

        Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetMaxDistance_InsideSource: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MinDistance to 1m.
    CHECKRUN(pSound->DSB_SetMinDistance(1));

    //Set the source 50m away, to the right.
    CHECKRUN(pSound->DSB_SetPosition(50, 0, 0));
  
    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    bPlaying = FALSE;
    DWORD dwIncValue = 1;        //this MUST be 1 for loop to work.
    DWORD dwTime     = 5000;
    Log(FYILOGLEVEL, "Moving MaxDistance from 2m to 100m over %d seconds - should hear sound continuously", dwTime / 1000);
    for (dwMaxDistance = 2; dwMaxDistance <=100  && SUCCEEDED(hr); dwMaxDistance += dwIncValue)
        {        
        CHECKRUN(pSound->DSB_SetMaxDistance((FLOAT)dwMaxDistance));
        CHECKRUN(pSound->DSB_SetMinDistance((FLOAT)dwMaxDistance - 1));
        if (dwMaxDistance % 10 == 0)
            Log(FYILOGLEVEL, "Max Distance is %5d", dwMaxDistance);
        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        Wait(dwTime / (100 / dwIncValue));
    }

    CHECKRUN(pSound->DSB_Stop());
    ;
    return hr;
}


/********************************************************************************
Try the ugly boundary cases for SetMaxDistance.
********************************************************************************/
HRESULT DMTest_Buf_SetMaxDistance_BoundaryCases(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DWORD dwMaxDistance = 0;
FLOAT fTestValues[] = {FLT_SMALL, 1, FLT_BIG, FLT_BIG};



        Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetMaxDistance_BoundaryCases: eTestApply = %s", String(eTestApply));
    Log(FYILOGLEVEL, "Setting source at -1, 0, 0 (left of you), trying extreme values for SetMaxDistance");

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MinDistance to the lowest possible number.
    CHECKRUN(pSound->DSB_SetMinDistance(FLT_SMALL));

    //Set the source close, to the left of ya.
    CHECKRUN(pSound->DSB_SetPosition(-11, 0, 0));
  
    //Play
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Iterate through test values.
    for (DWORD i=0; i<NUMELEMS(fTestValues) && SUCCEEDED(hr); i++)
    {
        Log(FYILOGLEVEL, "Trying MaxDistance = %g", fTestValues[i]);
        CHECKRUN(pSound->DSB_SetMaxDistance(fTestValues[i]));
        Wait(1000);
    }

    CHECKRUN(pSound->DSB_Stop());
    ;
    return hr;
}







/********************************************************************************
Try the ugly boundary cases for SetMaxDistance.
********************************************************************************/
HRESULT DMTest_Buf_SetMinDistance_BoundaryCases(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
FLOAT fTestValues[] = {FLT_SMALL, 1, 2, 4, 8, 16, FLT_BIG};
DS3DLISTENER ds3dl;
char *szStrings[] = {"Silence", "sound at 1/16 volume", "sound at 1/8 volume", "sound at 1/4 volume", "sound at 1/2 volume", "sound at full volume", "sound at full volume"};



    Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetMinDistance_BoundaryCases: eTestApply = %s", String(eTestApply));
    Log(FYILOGLEVEL, "Setting source at -16, 0, 0 (left of you), trying extreme values for SetMinDistance");

    //Set up the 3D Listener at 0, 0, 0, pointing forward too.
    ZeroMemory(&ds3dl, sizeof(ds3dl));
    ds3dl.dwSize                    = sizeof(ds3dl);
    ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
    ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
    ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
    ds3dl.vOrientFront              = make_D3DVECTOR(0, 0, 1);         //facing forward.
    ds3dl.vOrientTop                = make_D3DVECTOR(0, 1, 0);         //right side up
    ds3dl.vPosition                 = make_D3DVECTOR(0, 0, 0);         //at origin, behind the source.
    ds3dl.vVelocity                 = make_D3DVECTOR(0, 0, 0);         //would be derivatives of positioning functions cos and sin, if we adjusted this.


    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MaxDistance to FLT_BIG.
    CHECKRUN(pSound->DSB_SetMaxDistance(FLT_BIG));

    //Set the source close, 16m to the left of ya.
    CHECKRUN(pSound->DSB_SetPosition(-16, 0, 0));
  
    //Play
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Iterate through test values.
    for (DWORD i=0; i<NUMELEMS(fTestValues)  && SUCCEEDED(hr); i++)
    {
        Log(FYILOGLEVEL, "Trying MinDistance = %g for 4 seconds.  You should hear %s", fTestValues[i], szStrings[i]);
        CHECKRUN(pSound->DSB_SetMinDistance(fTestValues[i]));
        Wait(4000);
    }

    CHECKRUN(pSound->DSB_Stop());
    ;
    return hr;
}




/********************************************************************************
We set the source at some point, then move MinDistance from 1/100 of that distance
to that distance.  The volume should increase from almost nothing to maximum.
********************************************************************************/
HRESULT DMTest_Buf_SetMinDistance_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
FLOAT fTestValues[] = {0.001f, 100.f, FLT_BIG};
FLOAT fMinDistance = 0.0;
DWORD i,j;
DWORD dwInc;
FLOAT fCurrentValue;
FLOAT fIncValue;
BOOL  bPlaying = FALSE;


        Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetMinDistance_Listening: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MaxDistance to MAX, so we don't worry about it.
    CHECKRUN(pSound->DSB_SetMaxDistance(FLT_BIG));

    //100 increments.
    dwInc = 100;

    //for each loop, set source at x, then move MinDistance from 0->x.  The sound should smoothly increase over this period.
    for (i=0; i<NUMELEMS(fTestValues) && SUCCEEDED(hr); i++)
    {
        fIncValue = fTestValues[i] / (FLOAT)dwInc;
        fCurrentValue = 0.f;        
        CHECKRUN(pSound->DSB_SetPosition(fTestValues[i], 0, 0));
        Log(FYILOGLEVEL, "Setting source at           %8g, 0, 0.", fTestValues[i]);
        Log(FYILOGLEVEL, "inc'ing MinDistance from 0->%8g.  Sound should increase from silence->maximum.", fTestValues[i], fTestValues[i]);
        for (j = 0; j<=dwInc && SUCCEEDED(hr); j++)
        {
            if (j % 10 == 0)
                Log(FYILOGLEVEL, "MinDistance = %8g, %2.0f percent done", fCurrentValue, fCurrentValue / fTestValues[i] * 100);
            
            
            //Due to rounding errors, this value may exceed its target on the last iteration.  If this happens, clamp it and
            //  verify that it actually happened on the last iteration.
            fCurrentValue+=fIncValue;
            if (fCurrentValue > FLT_BIG)
            {
                fCurrentValue = FLT_BIG;
                if (j!=dwInc)
                {
                    Log(ABORTLOGLEVEL, "DMTest_Buf_SetMinDistance_Listening Test Error: Had to clamp our test value on iteration %d/%d", j, dwInc);
                    hr = E_FAIL;
                }

            }

            CHECKRUN(pSound->DSB_SetMinDistance(fCurrentValue));

            //Play the source if we haven't started already.  
            if (!bPlaying)
            {
                CHECKRUN(pSound->DSB_SetCurrentPosition(0));
                CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
                bPlaying = TRUE;
            }

            Wait(50);
        }
    }

    CHECKRUN(pSound->DSB_Stop());
    ;
    return hr;
}






/********************************************************************************
This does not use the wrapper class, since the wrapper state determines what
"mode" is used and whether CommitDeferredSettings is called or not.

This test sets a source to our right, then moves the listener across it in
the 3 different modes.  In each mode, the user should hear something different
as described in the pszStrings array below.
********************************************************************************/
HRESULT DMTest_Buf_SetMode(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr= S_OK;
LPDIRECTSOUND       pDS  = NULL;
LPDIRECTSOUNDBUFFER pDSB = NULL;
DWORD dwModes[] = {DS3DMODE_NORMAL,
                 DS3DMODE_HEADRELATIVE,
                 DS3DMODE_DISABLE};

char *pszStrings[] =
                 {"DS3DMODE_NORMAL:       Sound will move from right to left",
                  "DS3DMODE_HEADRELATIVE: Sound will come from the right.",
                  "DS3DMODE_DISABLE:      Sound will come from the center"};

DWORD i = 0;
LONG  j = 0;
DWORD dwTotalTime = 5000;

DWORD dwActualApply = 0;
BOOL bCommit = FALSE;
BOOL bPlaying = FALSE;

    CHECKRUN(pSound->GetDirectSound(&pDS));
    CHECKRUN(pSound->GetDirectSoundBuffer(&pDSB));

    //This is a special case where we're actually testing the mode, and not wrapping it. 
    //  We can't use the wrapper class for this one.
    //Decide the process by which to test this.
    switch (eTestApply)
    {
        case TESTAPPLY_IMMEDIATE:
            dwActualApply = DS3D_IMMEDIATE;
            bCommit = FALSE;
            Log(FYILOGLEVEL, "-->Calling DMTest_Buf_SetMode with TESTAPPLY_IMMEDIATE");
            break;
        case TESTAPPLY_DEFERRED_UPDATE:
            dwActualApply = DS3D_DEFERRED;
            bCommit = TRUE;
            Log(FYILOGLEVEL, "-->Calling DMTest_Buf_SetMode with TESTAPPLY_DEFERRED_UPDATE");
            break;
        case TESTAPPLY_DEFERRED_NOUPDATE:
            dwActualApply = DS3D_DEFERRED;
            bCommit = FALSE;
            Log(FYILOGLEVEL, "-->Calling DMTest_Buf_SetMode with TESTAPPLY_DEFERRED_NOUPDATE");
            break;
        default:
           Log(FYILOGLEVEL, "Error!!!  invalid value passed to DMTest_Buf_SetMode");
           break;
            
    }

    //Source to our right.
    CHECKRUN(pDSB->SetPosition(10, 0, 0, DS3D_IMMEDIATE));
    
    //Move the listener across the sound.
    for (i=0; i<NUMELEMS(dwModes); i++)
    {
        Log(FYILOGLEVEL, pszStrings[i]);
        if (TESTAPPLY_DEFERRED_NOUPDATE == eTestApply)
           Log(FYILOGLEVEL, "not calling CommitDeferredSettings - aforementioned change should not apply!!");
        
        //here we either commit or not.
        CHECKRUN(pDSB->SetMode(dwModes[i], dwActualApply));

        bPlaying = FALSE;
        for (j = 0; j<=20 && SUCCEEDED(hr); j++)
        {
            CHECKRUN(pDS->SetPosition((FLOAT)j, 0, 0, dwActualApply));
            if (bCommit)
            {
                CHECKRUN(pDS->CommitDeferredSettings());
            }

            if (!bPlaying)
            {
                //CHECKRUN(pDSB->Play(0, 0, DSBPLAY_LOOPING));
                bPlaying = TRUE;
            }
            Wait(dwTotalTime / 20);
        }

        //CHECKRUN(pDSB->Stop());

    }


    //Release buffers
    RELEASE(pDSB);
    RELEASE(pDS);

    return hr;
};



/********************************************************************************
So we want to test the following vectors with negative numbers.
a) testing 1, FLT_BIG, and FLT_SMALL.
b) testing negative numbers.

Example bunch -0- test cases.

{FLT_BIG, 0      , 0      },
{0      , FLT_BIG, 0      },
{0      , 0      , FLT_BIG},
{FLT_BIG, FLT_BIG, 0      },
{FLT_BIG, 0      , FLT_BIG},
{0      , FLT_BIG, FLT_BIG},
{FLT_BIG, FLT_BIG, FLT_BIG},

********************************************************************************/
HRESULT DMTest_Buf_SetPosition_BoundaryCases(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DWORD dwIterator = 0;

//Test values.
//FLOAT fTestValues[] = {1, 10, 100, FLT_BIG, FLT_BIG, FLT_SMALL, -1, -10, -100, -FLT_BIG, -FLT_BIG, -FLT_SMALL};
FLOAT fTestValues[] = {1, 10, 100, FLT_BIG, FLT_SMALL, -1, -10, -100, -FLT_BIG, -FLT_SMALL};
DWORD dwTestValue = 0;

D3DVECTOR v = {0};
D3DVECTOR vBufPos = {0};



        Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetPosition_BoundaryCases: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Play the sound!!
    Log(FYILOGLEVEL, "Playing Sound");
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Try all these values.  With all possible values passed in - about 800 iterations.
    for (dwTestValue = 0; dwTestValue < NUMELEMS(fTestValues)  && SUCCEEDED(hr); dwTestValue++)
    {
        for (dwIterator = 0; dwIterator < 8; dwIterator ++)
        {
            vBufPos.x = dwIterator & (1 << 0) ? fTestValues[dwTestValue] : 0;
            vBufPos.y = dwIterator & (1 << 1) ? fTestValues[dwTestValue] : 0;
            vBufPos.z = dwIterator & (1 << 2) ? fTestValues[dwTestValue] : 0;

            CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));
            Log(FYILOGLEVEL, "SetPosition(%g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
            Wait(100);
        }
    }

    Log(FYILOGLEVEL, "Stopping Sound");
    ;
    return hr;
};





/********************************************************************************
Rotates the buffer around the listener about each axis.
********************************************************************************/
HRESULT DMTest_Buf_SetPosition_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
BOOL bPlaying = FALSE;
DWORD dwAngle;
float fAngle;
D3DVECTOR vBufPos = {0};
DWORD dwAxis = 0;  //x=0, y=1, z=2.
FLOAT fPosition[2]; //this will be x,y, x,z, or y,z depending on dwAxis.
char *pszAxes[] = {"x", "y", "z"};
DWORD i = 0;

Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetPosition_Listening: eTestApply = %s", String(eTestApply));

//Make subsequent changes behave according to the specifications of the calling function.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

//Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
bPlaying = FALSE;
for (dwAxis = 0; dwAxis < 3  && SUCCEEDED(hr); dwAxis++)
{

    Log(FYILOGLEVEL, "Rotating source 360 degrees around %s axis", pszAxes[dwAxis]);
    for (dwAngle = 0; dwAngle <= 360; dwAngle++)
    {
        fAngle = float(((float)dwAngle) * PI / 180.0f);
        fPosition[0] = (float)cos(fAngle);
        fPosition[1] = (float)sin(fAngle);

        //Fill the position vector according to axis.
        for (i = 0; i<2; i++)
        {
            switch ((dwAxis + 1 + i) %3)
            {
                case 0: 
                    vBufPos.x = fPosition[i];
                    break;
                case 1: 
                    vBufPos.y = fPosition[i];
                    break;
                case 2: 
                    vBufPos.z = fPosition[i];
                    break;
                default:
                    Log(FYILOGLEVEL, "Test Error, see danhaff!!!!");
                    break;

            }
        }

        //Set this position.
        CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));
        Wait(5000 / 360);                  //want to spend 5s; going through loop 180 times

        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        if (dwAngle % 20 == 0)
            Log(FYILOGLEVEL, "Pos: %-8g, %-8g, %-8g.  Listener relative to source: %3d degrees.  %2d percent finished w/sweep", vBufPos.x, vBufPos.y, vBufPos.z, dwAngle, dwAngle * 100 / 360);

    }
}

CHECKRUN(pSound->DSB_Stop());
;
return hr;
}




/********************************************************************************
Tests the doppler effect on buffers on all 3 axes.
********************************************************************************/
HRESULT DMTest_Buf_SetVelocity(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
BOOL bPlaying = FALSE;
DWORD dwVelocity = 0;
D3DVECTOR vBufPos = {0};
D3DVECTOR vBufVel = {0};
DWORD dwAxis = 0;  //x=0, y=1, z=2.
FLOAT fPosition[2]; //this will be x,y, x,z, or y,z depending on dwAxis.
char *pszAxes[] = {"x", "y", "z"};
DWORD i = 0;
FLOAT fTestVelocity = 375.f; //speed of sound, in m/s
FLOAT fTestVal = 0.f;

Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetVelocity: eTestApply = %s", String(eTestApply));

//Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
bPlaying = FALSE;
for (dwAxis = 0; dwAxis < 3  && SUCCEEDED(hr); dwAxis++)
{

    Log(FYILOGLEVEL, "Testing doppler along %s axis", pszAxes[dwAxis]);

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set the position at 1 along the designated axis.
    vBufPos = make_D3DVECTOR(0, 0, 0);
    CHECK(DMSetComponent(&vBufPos, dwAxis, 1));
    Log(FYILOGLEVEL, "Setting position to %g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
    CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Play the sound.
    Log(FYILOGLEVEL, "Here is the default pitch of the sound for 2 seconds.");
    CHECKRUN(pSound->DSB_SetCurrentPosition(0));
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
    Wait(2000);

    //Fill the vector components not matching test axis - pitch shouldn't change since vel is perpendicular.
    for (i = 0; i<2 && SUCCEEDED(hr); i++)
    {
        Log(FYILOGLEVEL, "Setting %s component of velocity vector to %g for 3s -> expect NO CHANGE in pitch of sound!!", pszAxes[(dwAxis + 1 + i) %3], -fTestVelocity);
        vBufVel = make_D3DVECTOR(0, 0, 0);
        CHECK(DMSetComponent(&vBufVel, (dwAxis + 1 + i) %3, -fTestVelocity));
        CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
        Wait(3000);
    }

   
    //Verify doppler works along test axis.
    if (SUCCEEDED(hr))
    {
        Log(FYILOGLEVEL, "Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to increase", pszAxes[dwAxis], -fTestVelocity);
        vBufVel = make_D3DVECTOR(0, 0, 0);
        CHECK(DMSetComponent(&vBufVel, dwAxis, -fTestVelocity));
        CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
        Wait(3000);
    }

    
    FLOAT fDopplerFactor[] = {1, 2};
    for (i=0; i<NUMELEMS(fDopplerFactor) && SUCCEEDED(hr); i++)
    {    
        //Set doppler factor.
        Log(FYILOGLEVEL, "Setting Doppler Factor to %g", fDopplerFactor[i]);
        CHECKRUN(pSound->DS_SetDopplerFactor(fDopplerFactor[i]));

        //Increase doppler along test axis, verify increase in pitch.
        vBufVel = make_D3DVECTOR(0, 0, 0);
        Log(FYILOGLEVEL, "Increasing %s component of velocity vector from 0 to %g for 3s -> expect pitch of sound to increase", pszAxes[dwAxis], -fTestVelocity);
        for (fTestVal = 0; fTestVal>=-fTestVelocity; fTestVal--)
        {
            CHECK(DMSetComponent(&vBufVel, dwAxis, fTestVal));
            CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
            Wait(5);
        }

        //Decrease doppler.
        vBufVel = make_D3DVECTOR(0, 0, 0);
        Log(FYILOGLEVEL, "Increasing %s component of velocity vector from 0 to %g for 3s -> expect pitch of sound to decrease", pszAxes[dwAxis], fTestVelocity);
        for (fTestVal = 0; fTestVal<=fTestVelocity; fTestVal++)
        {
            CHECK(DMSetComponent(&vBufVel, dwAxis, fTestVal));
            CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
            Wait(5);
        }
    }

    CHECKRUN(pSound->DS_SetDopplerFactor(1));

    //Set doppler to max.
    vBufVel = make_D3DVECTOR(0, 0, 0);
    Log(FYILOGLEVEL, "Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be high", pszAxes[dwAxis], -FLT_BIG);
    CHECK(DMSetComponent(&vBufVel, dwAxis, -FLT_BIG));
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
    Wait(3000);

    //Set doppler to min.
    vBufVel = make_D3DVECTOR(0, 0, 0);
    Log(FYILOGLEVEL, "Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be low", pszAxes[dwAxis], FLT_BIG);
    CHECK(DMSetComponent(&vBufVel, dwAxis, FLT_BIG));
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
    Wait(3000);

}//end for dwAxis = 1 to 3.

CHECKRUN(pSound->DSB_Stop());
;
return hr;
}



/********************************************************************************
Tests the doppler effect on buffers on all 3 axes.
********************************************************************************/
/*
HRESULT DMTest_INFINITY_Bug(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
D3DVECTOR vBufPos = {0};
D3DVECTOR vBufVel = {0};

Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetVelocity: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Set doppler to min infinity.
    vBufVel = make_D3DVECTOR(INFINITY, 0, 0);
    Log(FYILOGLEVEL, "Setting velocity to %g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));


    //Set the position at 1 along the designated axis.
    vBufPos = make_D3DVECTOR(0, 1, 0);
    Log(FYILOGLEVEL, "Setting position to %g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
    CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));


CHECKRUN(pSound->DSB_Stop());
return hr;
}
*/



/********************************************************************************
********************************************************************************/
HRESULT DMTest_Buf_SetFrequency_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
//Test values.
DWORD dwTestValues[] = {DSBFREQUENCY_MIN, 24000, 48000,  96000, DSBFREQUENCY_MAX, DSBFREQUENCY_ORIGINAL};
DWORD dwTestValue = 0;

    Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetFrequency_Listening");

    //Play the sound!!
    Log(FYILOGLEVEL, "Playing Sound");
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Try all these values.  
    for (dwTestValue = 0; dwTestValue < NUMELEMS(dwTestValues)  && SUCCEEDED(hr); dwTestValue++)
    {
            CHECKRUN(pSound->DSB_SetFrequency(dwTestValues[dwTestValue]));
            Log(FYILOGLEVEL, "SetFrequency(%d)", dwTestValues[dwTestValue]);
            Wait(1000);
    }

    Log(FYILOGLEVEL, "Stopping Sound");
    CHECKRUN(pSound->DSB_SetFrequency(DSBFREQUENCY_ORIGINAL));
    CHECKRUN(pSound->DSB_Stop());
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT DMTest_Buf_SetFrequency_Range(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DWORD dwTestValue = 0;

    Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetFrequency_Range");

    //Play the sound!!
    Log(FYILOGLEVEL, "Playing Sound");
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Try all these values.
    for (dwTestValue = DSBFREQUENCY_MIN; dwTestValue < DSBFREQUENCY_MAX && SUCCEEDED(hr); dwTestValue++)
    {
        CHECKRUN(pSound->DSB_SetFrequency(dwTestValue));
        if (dwTestValue % 10000 == 0)
        {
            Log(FYILOGLEVEL, "SetFrequency(%d)", dwTestValue);
        }
    }

    Log(FYILOGLEVEL, "Stopping Sound");
    CHECKRUN(pSound->DSB_SetFrequency(DSBFREQUENCY_ORIGINAL));
    CHECKRUN(pSound->DSB_Stop());
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT DMTest_Buf_SetVolume_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
//Test values.
DWORD dwTestValues[] = {0, -500, -1000, -2000, -5000, -10000};
DWORD dwTestValue = 0;

    Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetVolume_Listening");

    //Play the sound!!
    Log(FYILOGLEVEL, "Playing Sound");
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Try all these values.  
    for (dwTestValue = 0; dwTestValue < NUMELEMS(dwTestValues)  && SUCCEEDED(hr); dwTestValue++)
    {
            CHECKRUN(pSound->DSB_SetVolume(dwTestValues[dwTestValue]));
            Log(FYILOGLEVEL, "SetVolume(%d)", dwTestValues[dwTestValue]);
            Wait(1000);
    }

    Log(FYILOGLEVEL, "Stopping Sound");
    CHECKRUN(pSound->DSB_SetVolume(0));
    CHECKRUN(pSound->DSB_Stop());
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT DMTest_Buf_SetVolume_Range(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DWORD dwTestValue = 0;

    Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetVolume_Range");

    //Play the sound!!
    Log(FYILOGLEVEL, "Playing Sound");
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Try all these values.  With all possible values passed in - about 800 iterations.
    for (dwTestValue = DSBVOLUME_MIN; dwTestValue < DSBVOLUME_MAX && SUCCEEDED(hr); dwTestValue++)
    {
        CHECKRUN(pSound->DSB_SetVolume(dwTestValue));
        if (dwTestValue % 1000 == 0)
        {
            Log(FYILOGLEVEL, "SetVolume(%d)", dwTestValue);
        }
    }

    Log(FYILOGLEVEL, "Stopping Sound");
    CHECKRUN(pSound->DSB_SetVolume(0));
    CHECKRUN(pSound->DSB_Stop());
    return hr;
};



/********************************************************************************
********************************************************************************/
/*

#define NUMCHANNELS 6
LPSTR szChannel[NUMCHANNELS] = {"Front Left", "Front Right", "Front Center", "Low Freq", "Back Left", "Back Right"};


static void SetAllVolumes(LPDSCHANNELVOLUME pdscv, LONG lVolume)
{
    DWORD i = 0;
    ZeroMemory(pdscv, sizeof(DSCHANNELVOLUME));
    pdscv->dwChannelMask = 0x3F;     //All 6 channels.
    for (i=0; i<NUMCHANNELS; i++)
        pdscv->alVolume[i] = lVolume;
};


HRESULT DMTest_Buf_SetChannelVolume_Channel_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DSCHANNELVOLUME dscv = {0};
DWORD i = 0;
DWORD j = 0;

    CHECK   (Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetVolume_Listening"));

    //Play the sound!!
    CHECK   (Log(FYILOGLEVEL, "Playing Sound"));
    CHECK   (SetAllVolumes(&dscv, -10000));
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
    CHECKRUN(pSound->DSB_SetChannelVolume(&dscv));

    //Try all these values.  
    for (i = 0; i < NUMCHANNELS  && SUCCEEDED(hr); i++)
    {
        for (j = 0; j < NUMCHANNELS && SUCCEEDED(hr); j++)
        {
            CHECK  (SetAllVolumes(&dscv, -10000));
            CHECK  (dscv.alVolume[i] = 0);
            CHECK  (dscv.alVolume[j] = 0);
            CHECKRUN(pSound->DSB_SetChannelVolume(&dscv));
            if (i!=j)
            {
                CHECK(Log(FYILOGLEVEL, "Turned on channels %s and %s only", szChannel[i], szChannel[j]));
            }
            else
            {
                CHECK(Log(FYILOGLEVEL, "Turned on %s channel only", szChannel[i]));
            }
            CHECKRUN(Wait(3000));
        }
    }

    CHECK   (Log(FYILOGLEVEL, "Stopping Sound"));
    CHECKRUN(pSound->DSB_Stop());
    return hr;
};



HRESULT DMTest_Buf_SetChannelVolume_Mask_Listening(BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative)
{
HRESULT hr = S_OK;
DSCHANNELVOLUME dscv = {0};
DWORD i = 0;
DWORD j = 0;

    CHECK   (Log(FYILOGLEVEL, "\n----- DMTest_Buf_SetVolume_Listening"));

    //Play the sound!!
    CHECK   (Log(FYILOGLEVEL, "Playing Sound"));
    CHECK   (SetAllVolumes(&dscv, -10000));
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
    CHECKRUN(pSound->DSB_SetChannelVolume(&dscv));

    //Try all these values.  
    for (i = 0; i < NUMCHANNELS  && SUCCEEDED(hr); i++)
    {
        for (j = 0; j < NUMCHANNELS && SUCCEEDED(hr); j++)
        {
            CHECK   (SetAllVolumes(&dscv, 0));
            CHECK   (dscv.dwChannelMask  = 1 << i);
            CHECK   (dscv.dwChannelMask |= 1 << j);
            CHECKRUN(pSound->DSB_SetChannelVolume(&dscv));
            if (i!=j)
            {
                CHECK(Log(FYILOGLEVEL, "Turned on channels %s and %s only", szChannel[i], szChannel[j]));
            }
            else
            {
                CHECK(Log(FYILOGLEVEL, "Turned on %s channel only", szChannel[i]));
            }

            CHECKRUN(Wait(3000));
        }
    }

    CHECK   (Log(FYILOGLEVEL, "Stopping Sound"));
    CHECKRUN(pSound->DSB_Stop());
    return hr;
};

*/