#include "globals.h"
#include "Sound3D.h"
#include "testbuf.h"
#include "dxconio.h"

HRESULT LoadRandomWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer);

/********************************************************************************
Orient a cone away from the listener, then expand the angles to encompass the 
listener.  Try SetAllParameters with dwApply on/off and see whether it changes.
********************************************************************************/
HRESULT Test_Buf_SetAllParameters(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DS3DBUFFER ds3db;
char *pStrings[] = {"Using DS3D_IMMEDATE", "Using DS3D_DEFERRED, then CommitDeferredSettings", "Using DS3D_DEFERRED but not CommitDeferredSettings - change shouldn't occur"};
DWORD dwWait = 4000;

ZeroMemory(&ds3db, sizeof(ds3db));



//Test the textout stuff.
/*mattbron
xCreateConsole(NULL);
xDisplayConsole();
xprintf("Hello!!!");
xReleaseConsole();
*/

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Buf_SetAllParameters:  eTestApply = %s", String(eTestApply));

//Set up our 3DBuffer so that the parameters are such that you shouldn't hear any sound.
ds3db.dwInsideConeAngle         = 0;
ds3db.dwOutsideConeAngle        = 0;
ds3db.dwMode                    = DS3DMODE_NORMAL;  //this doesn't matter anyway, it's goverened by SetMode.
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = 50.0;
ds3db.flMaxDistance             = 50.0;
ds3db.lConeOutsideVolume        = DSBVOLUME_MIN;
ds3db.vConeOrientation          = x_D3DVECTOR(0, 0, 1);  //away
ds3db.vPosition                 = x_D3DVECTOR(50.0, 50.0, 50.0);
ds3db.vVelocity                 = x_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor			= DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor			= DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor			= DS3D_DEFAULTDOPPLERFACTOR;

//Play sound, shouldn't hear anything.  Use Immediate mode to set these parameters so they'll always be set.
CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));
CONSOLEOUT("Sound is a meter ahead of you and facing away without outside volume minimum:  You should hear nothing for %d ms", dwWait);
CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
SleepEx(dwWait);

//Set up our 3DBuffer so that the parameters are such that you shouldn't hear any sound.
ds3db.dwInsideConeAngle         = DS3D_DEFAULTCONEANGLE;
ds3db.dwOutsideConeAngle        = DS3D_DEFAULTCONEANGLE;
ds3db.dwMode                    = DS3DMODE_NORMAL;  //this doesn't matter anyway, it's goverened by SetMode.
ds3db.dwSize                    = sizeof(ds3db);
ds3db.flMinDistance             = DS3D_DEFAULTMINDISTANCE;
ds3db.flMaxDistance             = DS3D_DEFAULTMAXDISTANCE;
ds3db.lConeOutsideVolume        = DSBVOLUME_MAX;
ds3db.vConeOrientation          = x_D3DVECTOR(0, 0, -1);  //toward
ds3db.vPosition                 = x_D3DVECTOR(0, 0, 0);
ds3db.vVelocity                 = x_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor			= DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor			= DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor			= DS3D_DEFAULTDOPPLERFACTOR;

//Now that everything is set to pretty much default, we should hear normal sound.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));
CONSOLEOUT("Calling SetAllParameters with default parameters: You should hear sound at full volume for %d ms.", dwWait);
ds3db.lConeOutsideVolume = DSBVOLUME_MAX;
CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
SleepEx(dwWait);

SAFEDELETE(pSound);
return hr;
}



/*******************************************************************************
Rotates between several sets of cone angles and asks user to verify the
correct sound came out.
Fails on software due to 2431; hopefully will succeed on hardware.
********************************************************************************/
HRESULT Test_Buf_SetConeAngles(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
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
ds3db.vConeOrientation          = x_D3DVECTOR(0, 0, 1);
ds3db.vPosition                 = x_D3DVECTOR(0, 0, 0);
ds3db.vVelocity                 = x_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor			= DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor			= DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor			= DS3D_DEFAULTDOPPLERFACTOR;

//Set up the 3D Listener at 0, 0, -1, pointing forward too.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = x_D3DVECTOR(0, 0, 1);         //facing forward.
ds3dl.vOrientTop                = x_D3DVECTOR(0, 1, 0);         //right side up
ds3dl.vPosition                 = x_D3DVECTOR(0, 0, -1);        //behind the source at (0, 0, 0);
ds3dl.vVelocity                 = x_D3DVECTOR(0, 0, 0);         //will be derivatives of positioning functions cos and sin.



ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Buf_SetConeAngles: eTestApply = %s", String(eTestApply));


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
    CONSOLEOUT("Moving listener from behind source (270) to in front of source (90)");
    CONSOLEOUT("ConeAngles are (%3d,%3d) %s", TestCase[dwTestCase].dwInsideConeAngle, TestCase[dwTestCase].dwOutsideConeAngle, TestCase[dwTestCase].szBehavior);
    CONSOLEOUT("---------------------------------------------------");
        
    //Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
    bPlaying = FALSE;
    for (dwAngle = 270; dwAngle >= 90  && SUCCEEDED(hr); dwAngle--)
    {
        fAngle = float(((float)dwAngle) * PI / 180.0f);
        vLisPos.x = (float)cos(fAngle);
        vLisPos.z = (float)sin(fAngle);

        CHECKRUN(pSound->DS_SetPosition(vLisPos.x, vLisPos.y, vLisPos.z));
        SleepEx(5000 / 180);                  //want to spend 5s; going through loop 180 times

        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        if (dwAngle % 20 == 0)
            CONSOLEOUT("Listener relative to source: %3d degrees.  %2d percent finished w/sweep", dwAngle, (270 - dwAngle) * 100 / 180);

    }

    CHECKRUN(pSound->DSB_Stop());
}

SAFEDELETE(pSound);
return hr;
}








/********************************************************************************
So we want to test the following vectors with negative numbers.
a) testing 1, 25.0, and FLT_MIN.
b) testing negative numbers.

Example bunch -0- test cases.

{25.0, 0      , 0      },
{0      , 25.0, 0      },
{0      , 0      , 25.0},
{25.0, 25.0, 0      },
{25.0, 0      , 25.0},
{0      , 25.0, 25.0},
{25.0, 25.0, 25.0},

********************************************************************************/
HRESULT Test_Buf_SetConeOrientation_BoundaryCases(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DWORD dwIterator = 0;


//A couple random positions to test with.
D3DVECTOR vBufPos[] = {{43, -28, 90}, {.2f, -.7f, 2.2f}};
D3DVECTOR vLisPos[] = {{234, 22, 4},  {-2, -.02f , 1.8f}};
DWORD dwBufPos, dwLisPos;

//Test values.
FLOAT fTestValues[8] = {1, 25.0, 50.0, FLT_MIN, -1, -25.0, -50.0, -FLT_MIN};
DWORD dwTestValue = 0;

D3DVECTOR v = {0};



    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetConeOrientation_BoundaryCases: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Play the sound!!
    CONSOLEOUT("Playing Sound");
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
                    SleepEx(10);
                }
            }
        }
    }

    CONSOLEOUT("Stopping Sound");

    SAFEDELETE(pSound);
    return hr;
};



/********************************************************************************
Spins the buffer cone around and asks user to verify that the volume is attenuated 
properly.

This will fail on software any case where the sound is between the coneangles due to
bug 2431.  Hopefully it will succeed on hardware.
********************************************************************************/
HRESULT Test_Buf_SetConeOrientation_Listening(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
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
ds3db.vConeOrientation          = x_D3DVECTOR(0, 0, -1);        //Looking back toward the listener
ds3db.vPosition                 = x_D3DVECTOR(0, 0, 1);
ds3db.vVelocity                 = x_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor			= DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor			= DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor			= DS3D_DEFAULTDOPPLERFACTOR;

//Set up the 3D Listener at 0, 0, 0, pointing forward too.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = x_D3DVECTOR(0, 0, 1);         //facing forward.
ds3dl.vOrientTop                = x_D3DVECTOR(0, 1, 0);         //right side up
ds3dl.vPosition                 = x_D3DVECTOR(0, 0, 0);         //at origin, behind the source.
ds3dl.vVelocity                 = x_D3DVECTOR(0, 0, 0);         //will be derivatives of positioning functions cos and sin.


    //Init
    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetConeOrientation_Listening: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set starting parameters.
    CHECKRUN(pSound->DSB_SetAllParameters(&ds3db));
    CHECKRUN(pSound->DS_SetAllParameters(&ds3dl));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Print what is supposed to happen.
    CONSOLEOUT("Cone will start pointing toward the user, and rotate 360 degrees (away and then back toward the user)");
    CONSOLEOUT("  The sound should fade out and then in over a period of 5 seconds.");
        
    //Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
    bPlaying = FALSE;
    for (dwAngle = 0; dwAngle <=360  && SUCCEEDED(hr); dwAngle++)
    {        
        //make zero be facing backwards.
        fAngle = float(((float)dwAngle + 270) * PI / 180.0f);
        vLisPos.x = (float)cos(fAngle);
        vLisPos.z = (float)sin(fAngle);

        CHECKRUN(pSound->DSB_SetConeOrientation(vLisPos.x, vLisPos.y, vLisPos.z));
        SleepEx(5000 / 360);                  //want to spend 5s; going through loop 360 times

        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        if (dwAngle % 20 == 0)
            CONSOLEOUT("Listener angle (0 = pointing at source): %3d degrees.  %2d percent finished w/sweep", dwAngle, dwAngle * 100 / 360);

    }

    CHECKRUN(pSound->DSB_Stop());

    SAFEDELETE(pSound);
    return hr;
}




/********************************************************************************
Fails on software due to 2431; hopefully will succeed on hardware.
********************************************************************************/
HRESULT Test_Buf_SetConeOutsideVolume(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
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
ds3db.vConeOrientation          = x_D3DVECTOR(0, 0, 1);        //Facing away from the listener.
ds3db.vPosition                 = x_D3DVECTOR(0, 0, 1);
ds3db.vVelocity                 = x_D3DVECTOR(0, 0, 0);
ds3db.flDistanceFactor			= DS3D_DEFAULTDISTANCEFACTOR;
ds3db.flRolloffFactor			= DS3D_DEFAULTROLLOFFFACTOR;
ds3db.flDopplerFactor			= DS3D_DEFAULTDOPPLERFACTOR;

//Set up the 3D Listener at 0, 0, 0, pointing forward too.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = x_D3DVECTOR(0, 0, 1);         //facing forward.
ds3dl.vOrientTop                = x_D3DVECTOR(0, 1, 0);         //right side up
ds3dl.vPosition                 = x_D3DVECTOR(0, 0, 0);         //at origin, behind the source.
ds3dl.vVelocity                 = x_D3DVECTOR(0, 0, 0);         //would be derivatives of positioning functions cos and sin, if we adjusted this.


    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetConeOutsideVolume: eTestApply = %s", String(eTestApply));

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
    CONSOLEOUT("Listener is on outside of sound cone.  Will increase outside volume from -10000 (silence) to 0 (max) over %d seconds", dwTime / 1000);
    for (lOutsideVolume = DSBVOLUME_MIN; lOutsideVolume <= DSBVOLUME_MAX  && SUCCEEDED(hr); lOutsideVolume+=lIncValue)    {        

        CHECKRUN(pSound->DSB_SetConeOutsideVolume(lOutsideVolume));
        if (lOutsideVolume % lIncValue == 0)
            CONSOLEOUT("Outside Volume is %5d", lOutsideVolume);
        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        SleepEx(dwTime / ((DSBVOLUME_MAX - DSBVOLUME_MIN) / lIncValue));
    }

    CHECKRUN(pSound->DSB_Stop());

    SAFEDELETE(pSound);
    return hr;
}





/********************************************************************************
We set a sound source 50m away.  Then we move MaxDistance from 2 to 100m.
When this value hits 50 the sound should turn on.  However, it turns on 
immediately due to

2478	incrementing IDirectSound3DBuffer::SetMaxDistance will incorrectly enable far-away sound.

This should be fixed when we replace the software with hardware.
********************************************************************************/
HRESULT Test_Buf_SetMaxDistance_InsideSource(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
BOOL bPlaying = FALSE;
DWORD dwMaxDistance = 0;

    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetMaxDistance_InsideSource: eTestApply = %s", String(eTestApply));

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
    CONSOLEOUT("Moving MaxDistance from 2m to 100m over %d seconds - sound should turn off at %d seconds", dwTime / 1000, dwTime / 2000);
    for (dwMaxDistance = 2; dwMaxDistance <=100  && SUCCEEDED(hr); dwMaxDistance += dwIncValue)
        {        

        if (dwMaxDistance == 50)
            CONSOLEOUT("SOUND SHOULD TURN ON NOW!!");

        CHECKRUN(pSound->DSB_SetMaxDistance((FLOAT)dwMaxDistance));
        CHECKRUN(pSound->DSB_SetMinDistance((FLOAT)dwMaxDistance - 1));
        if (dwMaxDistance % 10 == 0)
            CONSOLEOUT("Max Distance is %5d", dwMaxDistance);
        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        SleepEx(dwTime / (100 / dwIncValue));
    }

    CHECKRUN(pSound->DSB_Stop());
    SAFEDELETE(pSound);
    return hr;
}


/********************************************************************************
Try the ugly boundary cases for SetMaxDistance.
********************************************************************************/
HRESULT Test_Buf_SetMaxDistance_BoundaryCases(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DWORD dwMaxDistance = 0;
FLOAT fTestValues[] = {/*0,*/ /*FLT_MIN*/DS3D_MINMAXDISTANCE, 1, 25.0, 50.0};



    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetMaxDistance_BoundaryCases: eTestApply = %s", String(eTestApply));
    CONSOLEOUT("Setting source at -1, 0, 0 (left of you), trying extreme values for SetMaxDistance");

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MinDistance to 0.
    CHECKRUN(pSound->DSB_SetMinDistance(/*0*/ /*FLT_MIN*/DS3D_MINMINDISTANCE));

    //Set the source close, to the left of ya.
    CHECKRUN(pSound->DSB_SetPosition(-11, 0, 0));
  
    //Play
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Iterate through test values.
    for (DWORD i=0; i<NUMELEMS(fTestValues) && SUCCEEDED(hr); i++)
    {
        CONSOLEOUT("Trying MaxDistance = %g", fTestValues[i]);
        CHECKRUN(pSound->DSB_SetMaxDistance(fTestValues[i]));
        SleepEx(1000);
    }

    CHECKRUN(pSound->DSB_Stop());
    SAFEDELETE(pSound);
    return hr;
}







/********************************************************************************
Try the ugly boundary cases for SetMaxDistance.
********************************************************************************/
HRESULT Test_Buf_SetMinDistance_BoundaryCases(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
FLOAT fTestValues[] = {/*0,*/ /*FLT_MIN*/DS3D_MINMINDISTANCE, 1, 25.0, 50.0};
char *szStrings[] = {"Silence", "Silence", "the sound quietly to your left", "the sound loudly to your left", "At infiniti" };



    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetMinDistance_BoundaryCases: eTestApply = %s", String(eTestApply));
    CONSOLEOUT("Setting source at -10, 0, 0 (left of you), trying extreme values for SetMaxDistance");

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MaxDistance to 0.
    CHECKRUN(pSound->DSB_SetMaxDistance(50.0));

    //Set the source close, to the right of ya.
    CHECKRUN(pSound->DSB_SetPosition(10, 0, 0));
  
    //Play
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Iterate through test values.
    for (DWORD i=0; i<NUMELEMS(fTestValues)  && SUCCEEDED(hr); i++)
    {
        CONSOLEOUT("Trying MinDistance = %g for 4 seconds.  You should hear %s", fTestValues[i], szStrings[i]);
        CHECKRUN(pSound->DSB_SetMinDistance(fTestValues[i]));
        SleepEx(4000);
    }

    CHECKRUN(pSound->DSB_Stop());
    SAFEDELETE(pSound);
    return hr;
}




/********************************************************************************
We set the source at some point, then move MinDistance from 1/100 of that distance
to that distance.  The volume should increase from almost nothing to maximum.
********************************************************************************/
HRESULT Test_Buf_SetMinDistance_Listening(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
FLOAT fTestValues[] = {0.001f, 100.f, 25.0};
FLOAT fMinDistance = 0.0;
DWORD i,j;
DWORD dwInc;
FLOAT fCurrentValue;
FLOAT fIncValue;
BOOL  bPlaying = FALSE;


    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetMinDistance_Listening: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set MaxDistance to MAX, so we don't worry about it.
    CHECKRUN(pSound->DSB_SetMaxDistance(50.0));

    //100 increments.
    dwInc = 100;

    //for each loop, set source at x, then move MinDistance from 0->x.  The sound should smoothly increase over this period.
    for (i=0; i<NUMELEMS(fTestValues) && SUCCEEDED(hr); i++)
    {
        fIncValue = fTestValues[i] / (FLOAT)dwInc;
        fCurrentValue = 0.f;        
        CHECKRUN(pSound->DSB_SetPosition(fTestValues[i], 0, 0));
        CONSOLEOUT("Setting source at           %8g, 0, 0.", fTestValues[i]);
        CONSOLEOUT("inc'ing MinDistance from 0->%8g.  Sound should increase from silence->maximum.", fTestValues[i], fTestValues[i]);
        for (j = 0; j<=dwInc; j++)
        {
            if (j % 10 == 0)
                CONSOLEOUT("MinDistance = %8g, %2.0f percent done", fCurrentValue, fCurrentValue / fTestValues[i] * 100);
            fCurrentValue+=fIncValue;
			fCurrentValue = fCurrentValue <= 50.0f ? fCurrentValue : 50.0f;
            CHECKRUN(pSound->DSB_SetMinDistance(fCurrentValue));

            //Play the source if we haven't started already.  
            if (!bPlaying)
            {
                CHECKRUN(pSound->DSB_SetCurrentPosition(0));
                CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
                bPlaying = TRUE;
            }

            SleepEx(50);
        }
    }

    CHECKRUN(pSound->DSB_Stop());
    SAFEDELETE(pSound);
    return hr;
}






/********************************************************************************
This does not use the wrapper class, since the wrapper state determines what
"mode" is used and whether CommitDeferredSettings is called or not.

This test sets a source to our right, then moves the listener across it in
the 3 different modes.  In each mode, the user should hear something different
as described in the pszStrings array below.
********************************************************************************/
HRESULT Test_Buf_SetMode(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr= S_OK;
LPDIRECTSOUND       pDS  = NULL;
LPDIRECTSOUNDBUFFER pDSB = NULL;
LPVOID pvSoundData       = NULL;
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


    //This is a special case where we're actually testing the mode, and not wrapping it. 
    //  We can't use the wrapper class for this one.
    //Decide the process by which to test this.
    switch (eTestApply)
    {
        case TESTAPPLY_IMMEDIATE:
            dwActualApply = DS3D_IMMEDIATE;
            bCommit = FALSE;
            CONSOLE("-->Calling Test_Buf_SetMode with TESTAPPLY_IMMEDIATE");
            break;
        case TESTAPPLY_DEFERRED_UPDATE:
            dwActualApply = DS3D_DEFERRED;
            bCommit = TRUE;
            CONSOLE("-->Calling Test_Buf_SetMode with TESTAPPLY_DEFERRED_UPDATE");
            break;
        case TESTAPPLY_DEFERRED_NOUPDATE:
            dwActualApply = DS3D_DEFERRED;
            bCommit = FALSE;
            CONSOLE("-->Calling Test_Buf_SetMode with TESTAPPLY_DEFERRED_NOUPDATE");
            break;
        default:
           CONSOLE("Error!!!  invalid value passed to Test_Buf_SetMode");
           break;
            
    }

    //Create the DSound object (our "listener")
#ifdef SILCER
    CHECKRUN(Help_DirectSoundCreate(DSDEVID_DEVICEID_MCPX, &pDS, NULL));
#else // SILVER
    CHECKRUN(Help_DirectSoundCreate(0, &pDS, NULL));
#endif // SILVER

//	CHECKRUN(DownloadScratch(pDS, "T:\\Media\\DSPCode\\DSSTDFX.bin" ));
    CHECKRUN(DownloadLinkedDSPImage(pDS));

    //If we've specified a filename it's nice and simple; we load it.
    if (szFileName)
    {
        CHECKRUN(LoadWaveFile(NULL, &pvSoundData, &pDSB, szFileName));
    }
    
    //If not, we need to find a random wave file on the drive and load that.
    else
    {
        CHECKRUN(LoadRandomWaveFile(NULL, &pvSoundData, &pDSB));
    } //end "if szFileName else"


    //Source to our right.
    CHECKRUN(pDSB->SetPosition(10, 0, 0, DS3D_IMMEDIATE));

    
    //Move the listener across the sound.
    for (i=0; i<NUMELEMS(dwModes); i++)
    {
        CONSOLE(pszStrings[i]);
        if (TESTAPPLY_DEFERRED_NOUPDATE == eTestApply)
           CONSOLE("not calling CommitDeferredSettings - aforementioned change should not apply!!");
        
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
                CHECKRUN(pDSB->Play(0, 0, DSBPLAY_LOOPING));
                bPlaying = TRUE;
            }
            SleepEx(dwTotalTime / 20);
        }

        CHECKRUN(pDSB->Stop());

    }


    //Release buffers
    RELEASE(pDSB);
    RELEASE(pDS);

    //Free up the sound data.
    if (pvSoundData)
#ifndef DVTSNOOPBUG
        free(pvSoundData);
#else
		XPhysicalFree( pvSoundData );
#endif

    return hr;
};



/********************************************************************************
So we want to test the following vectors with negative numbers.
a) testing 1, 25.0, and FLT_MIN.
b) testing negative numbers.

Example bunch -0- test cases.

{25.0, 0      , 0      },
{0      , 25.0, 0      },
{0      , 0      , 25.0},
{25.0, 25.0, 0      },
{25.0, 0      , 25.0},
{0      , 25.0, 25.0},
{25.0, 25.0, 25.0},

********************************************************************************/
HRESULT Test_Buf_SetPosition_BoundaryCases(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DWORD dwIterator = 0;

//Test values.
FLOAT fTestValues[] = {1, 10, 100, 25.0, 50.0, FLT_MIN, -1, -10, -100, -25.0, -50.0, -FLT_MIN};
DWORD dwTestValue = 0;

D3DVECTOR v = {0};
D3DVECTOR vBufPos = {0};



    ALLOCATEANDCHECK(pSound, SOUND3D);
    CHECKRUN(pSound->Init(vRelative, szFileName));
    CONSOLEOUT("\n----- Test_Buf_SetPosition_BoundaryCases: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Play the sound!!
    CONSOLEOUT("Playing Sound");
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
            CONSOLE("SetPosition(%g, %g, %g)", vBufPos.x, vBufPos.y, vBufPos.z);
            SleepEx(100);
        }
    }

    CONSOLEOUT("Stopping Sound");
    SAFEDELETE(pSound);
    return hr;
};





/********************************************************************************
Rotates the buffer around the listener about each axis.
********************************************************************************/
HRESULT Test_Buf_SetPosition_Listening(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
BOOL bPlaying = FALSE;
DWORD dwAngle;
float fAngle;
D3DVECTOR vBufPos = {0};
DWORD dwAxis = 0;  //x=0, y=1, z=2.
FLOAT fPosition[2]; //this will be x,y, x,z, or y,z depending on dwAxis.
char *pszAxes[] = {"x", "y", "z"};
DWORD i = 0;

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Buf_SetPosition_Listening: eTestApply = %s", String(eTestApply));

//Make subsequent changes behave according to the specifications of the calling function.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

//Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
bPlaying = FALSE;
for (dwAxis = 0; dwAxis < 3  && SUCCEEDED(hr); dwAxis++)
{

    CONSOLE("Rotating source 360 degrees around %s axis", pszAxes[dwAxis]);
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
                    CONSOLE("Test Error, see danhaff!!!!");
                    break;

            }
        }

        //Set this position.
        CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));
        SleepEx(5000 / 360);                  //want to spend 5s; going through loop 180 times

        //Play the source if we haven't started already.  
        if (!bPlaying)
        {
            CHECKRUN(pSound->DSB_SetCurrentPosition(0));
            CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
            bPlaying = TRUE;
        }

        if (dwAngle % 20 == 0)
            CONSOLEOUT("Pos: %-8g, %-8g, %-8g.  Listener relative to source: %3d degrees.  %2d percent finished w/sweep", vBufPos.x, vBufPos.y, vBufPos.z, dwAngle, dwAngle * 100 / 360);

    }
}

CHECKRUN(pSound->DSB_Stop());
SAFEDELETE(pSound);
return hr;
}




/********************************************************************************
Tests the doppler effect on buffers on all 3 axes.
********************************************************************************/
HRESULT Test_Buf_SetVelocity(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
BOOL bPlaying = FALSE;
DWORD dwVelocity = 0;
D3DVECTOR vBufPos = {0};
D3DVECTOR vBufVel = {0};
DWORD dwAxis = 0;  //x=0, y=1, z=2.
FLOAT fPosition[2]; //this will be x,y, x,z, or y,z depending on dwAxis.
char *pszAxes[] = {"x", "y", "z"};
DWORD i = 0;
FLOAT fTestVelocity = 1000.f;
FLOAT fTestVal = 0.f;

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Buf_SetVelocity: eTestApply = %s", String(eTestApply));

//Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
bPlaying = FALSE;
for (dwAxis = 0; dwAxis < 3  && SUCCEEDED(hr); dwAxis++)
{

    CONSOLE("Testing doppler along %s axis", pszAxes[dwAxis]);

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set the position at 1 along the designated axis.
    vBufPos = x_D3DVECTOR(0, 0, 0);
    CHECK(SetComponent(&vBufPos, dwAxis, 1));
    CONSOLE("Setting position to %g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
    CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));

    //Make subsequent changes behave according to the specifications of the calling function.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

    //Play the sound.
    CONSOLE("Here is the default pitch of the sound for 2 seconds.");
    CHECKRUN(pSound->DSB_SetCurrentPosition(0));
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
    SleepEx(2000);

    //Fill the vector components not matching test axis - pitch shouldn't change since vel is perpendicular.
    for (i = 0; i<2 && SUCCEEDED(hr); i++)
    {
        CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect NO CHANGE in pitch of sound!!", pszAxes[(dwAxis + 1 + i) %3], -fTestVelocity);
        vBufVel = x_D3DVECTOR(0, 0, 0);
        CHECK(SetComponent(&vBufVel, (dwAxis + 1 + i) %3, -fTestVelocity));
        CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
        SleepEx(3000);
    }

   
    //Verify doppler works along test axis.
    if (SUCCEEDED(hr))
    {
        CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to increase", pszAxes[dwAxis], -fTestVelocity);
        vBufVel = x_D3DVECTOR(0, 0, 0);
        CHECK(SetComponent(&vBufVel, dwAxis, -fTestVelocity));
        CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
        SleepEx(3000);
    }

    
    FLOAT fDopplerFactor[] = {1, 2};
    for (i=0; i<NUMELEMS(fDopplerFactor) && SUCCEEDED(hr); i++)
    {    
        //Set doppler factor.
        CONSOLE("Setting Doppler Factor to %g", fDopplerFactor[i]);
        CHECKRUN(pSound->DS_SetDopplerFactor(fDopplerFactor[i]));

        //Increase doppler along test axis, verify increase in pitch.
        vBufVel = x_D3DVECTOR(0, 0, 0);
        CONSOLE("Increasing %s component of velocity vector from 0 to %g for 3s -> expect pitch of sound to increase", pszAxes[dwAxis], -fTestVelocity);
        for (fTestVal = 0; fTestVal>=-fTestVelocity; fTestVal--)
        {
            CHECK(SetComponent(&vBufVel, dwAxis, fTestVal));
            CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
            SleepEx(5);
        }

        //Decrease doppler.
        vBufVel = x_D3DVECTOR(0, 0, 0);
        CONSOLE("Increasing %s component of velocity vector from 0 to %g for 3s -> expect pitch of sound to decrease", pszAxes[dwAxis], fTestVelocity);
        for (fTestVal = 0; fTestVal<=fTestVelocity; fTestVal++)
        {
            CHECK(SetComponent(&vBufVel, dwAxis, fTestVal));
            CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
            SleepEx(5);
			
        }

    }

    CHECKRUN(pSound->DS_SetDopplerFactor(1));

    //Set doppler to max.
    vBufVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be high", pszAxes[dwAxis], -25.0);
    CHECK(SetComponent(&vBufVel, dwAxis, -25.0));
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
    SleepEx(3000);

    //Set doppler to min.
    vBufVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be low", pszAxes[dwAxis], 25.0);
    CHECK(SetComponent(&vBufVel, dwAxis, 25.0));
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
    SleepEx(3000);


//BUGBUG: Removed due to 2543

/*
   //Set doppler to max 50.0.
    vBufVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be high", pszAxes[dwAxis], -50.0);
    CHECK(SetComponent(&vBufVel, dwAxis, -50.0));
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
    SleepEx(3000);

    //Set doppler to min 50.0.
    vBufVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be low", pszAxes[dwAxis], 50.0);
    CHECK(SetComponent(&vBufVel, dwAxis, 50.0));
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));
    SleepEx(3000);
*/
}//end for dwAxis = 1 to 3.

CHECKRUN(pSound->DSB_Stop());
SAFEDELETE(pSound);
return hr;
}







/********************************************************************************
Tests the doppler effect on buffers on all 3 axes.
********************************************************************************/
HRESULT Test_INFINITY_Bug(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
D3DVECTOR vBufPos = {0};
D3DVECTOR vBufVel = {0};

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Buf_SetVelocity: eTestApply = %s", String(eTestApply));

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

    //Set doppler to min 50.0.
    vBufVel = x_D3DVECTOR(50.0, 0, 0);
    CONSOLE("Setting velocity to %g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
    CHECKRUN(pSound->DSB_SetVelocity(vBufVel.x, vBufVel.y, vBufVel.z));


    //Set the position at 1 along the designated axis.
    vBufPos = x_D3DVECTOR(0, 1, 0);
    CONSOLE("Setting position to %g, %g, %g", vBufPos.x, vBufPos.y, vBufPos.z);
    CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));


CHECKRUN(pSound->DSB_Stop());
SAFEDELETE(pSound);
return hr;
}







