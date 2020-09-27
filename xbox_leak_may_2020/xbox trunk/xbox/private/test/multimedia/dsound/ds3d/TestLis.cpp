#include "globals.h"
#include "Sound3D.h"
#include "TestLis.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

HRESULT LoadRandomWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer);

 
/********************************************************************************
Sets a bunch of parameters, then calls SetAllParameters.  Aurally we
must verify that the new parameters are set.
********************************************************************************/
HRESULT Test_Lis_SetAllParameters(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DS3DLISTENER ds3dl;
DWORD dwWait = 4000;

ZeroMemory(&ds3dl, sizeof(ds3dl));

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetAllParameters: eTestApply = %s", String(eTestApply));
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));



CONSOLEOUT("Playing sound normally");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
SleepEx(2000);

//Set up our 3DListener such that if one of these parameters doesn't get reset, we won't hear anything normal.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_MAXDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_MAXDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_MAXROLLOFFFACTOR;
ds3dl.vOrientFront              = x_D3DVECTOR(FLT_MIN, 0, 0);
ds3dl.vOrientTop                = x_D3DVECTOR(FLT_MIN, 0, 0);
ds3dl.vPosition                 = x_D3DVECTOR(1000, 1000, 1000);
ds3dl.vVelocity                 = x_D3DVECTOR(1000, 10000, 10000);


//Play sound, shouldn't hear anything.  
CONSOLEOUT("Listener parameters are such that you should hear nothing for %d ms", dwWait);
CHECKRUN(pSound->DS_SetAllParameters(&ds3dl));
SleepEx(dwWait);

//Set up our 3DListener normally.
ds3dl.dwSize                    = sizeof(ds3dl);
ds3dl.flDistanceFactor          = DS3D_DEFAULTDISTANCEFACTOR;
ds3dl.flDopplerFactor           = DS3D_DEFAULTDOPPLERFACTOR;
ds3dl.flRolloffFactor           = DS3D_DEFAULTROLLOFFFACTOR;
ds3dl.vOrientFront              = x_D3DVECTOR(FLT_MIN, 0, 0);       
ds3dl.vOrientTop                = x_D3DVECTOR(FLT_MIN, 0, 0);       
ds3dl.vPosition                 = x_D3DVECTOR(0, 0, 0);       
ds3dl.vVelocity                 = x_D3DVECTOR(0, 0, 0);       

//Play sound, should hear it normally.
CONSOLEOUT("Listener parameters are default - you should hear sound for %d ms.", dwWait);
CHECKRUN(pSound->DS_SetAllParameters(&ds3dl));
SleepEx(dwWait);


SAFEDELETE(pSound);
return hr;
}



/********************************************************************************
The doppler effect is also increase if you tell a source it's moving 5m for every
1m, hence doppler is a good way to verify that these values are actually getting set.

We go through a bunch of factors and try a set of relative velocities (i.e. for
100 we'd set the velocity to 200 if the distance factor was 0.5, so what they
multiply to would be 100 and the doppler should be calculated from that)
********************************************************************************/
HRESULT Test_Lis_SetDistanceFactor(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DS3DLISTENER ds3dl;
FLOAT fVelocities[] = {-50.0, -25.0, -150, -100, -50, 1, 50, 100, 150, 25.0, 50.0};
FLOAT fFactors[]   = {0.01f, 0.1f, 1, 5, 10};
DWORD    dwFactor;
DWORD    dwVelocity;
D3DVECTOR vVelocity = {0};
FLOAT fPercentage = {0};

ZeroMemory(&ds3dl, sizeof(ds3dl));

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetDistanceFactor: eTestApply = %s", String(eTestApply));

//Put the sound to our right - this is an absolute for this test.
CHECKRUN(pSound->DSB_SetPosition(1, 0, 0));

//Set up the testing mode.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

//Play the sound.
CONSOLEOUT("Playing sound normally");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
SleepEx(2000);

for (dwFactor=0; dwFactor<NUMELEMS(fFactors) && SUCCEEDED(hr); dwFactor++)
{
    CONSOLEOUT("Setting distance factor to %g", fFactors[dwFactor]);
    CHECKRUN(pSound->DS_SetDistanceFactor(fFactors[dwFactor]));
    for (dwVelocity = 0; dwVelocity<NUMELEMS(fVelocities); dwVelocity++)
    {
        vVelocity.x = fVelocities[dwVelocity] * (1 / fFactors[dwFactor]);
        CHECKRUN(pSound->DSB_SetVelocity(vVelocity.x, 0, 0));
        CONSOLEOUT("Setting source velocity at %g, should sound same pitch as velocity of %g under normal conditions",
                    vVelocity.x,
                    vVelocity.x * fFactors[dwFactor]
                    );
        SleepEx(1000);
    }
}


SAFEDELETE(pSound);
return hr;

}

/********************************************************************************
Verifies that the pitch increases as the doppler factor increases.
********************************************************************************/
HRESULT Test_Lis_SetDopplerFactor(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
D3DVECTOR vVelocity = {0};
DWORD dwDopplerFactor = 0;

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetDopplerFactor: eTestApply = %s", String(eTestApply));

//Put the sound to our right - this is an absolute for this test.
CHECKRUN(pSound->DSB_SetPosition(1, 0, 0));

//Set up the testing mode.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

//Play the sound.
CONSOLEOUT("Playing sound normally");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
SleepEx(2000);

//Now run through the doppler factors.
CONSOLEOUT("Increasing doppler factor, should hear pitch increase.");
for (dwDopplerFactor=(DWORD)DS3D_MINDOPPLERFACTOR; dwDopplerFactor <= (DWORD)DS3D_MAXDOPPLERFACTOR && SUCCEEDED(hr); dwDopplerFactor++)
{
    CONSOLEOUT("Setting doppler factor to %d", dwDopplerFactor);
    CHECKRUN(pSound->DS_SetDopplerFactor((FLOAT)dwDopplerFactor));
    
    //If it's our first time, wait until setting the doppler factor before setting the velocity so we don't get a warble.
    if (0 == dwDopplerFactor)
        CHECKRUN(pSound->DSB_SetVelocity(-10, 0, 0));

    SleepEx(200);
}


SAFEDELETE(pSound);
return hr;

}




/********************************************************************************
1) Set a source to the right of the listener.

2) Point the listener forward.

3) Rotate the top vector about the Z axis.  Sound should oscilate from right to 
   left ear and back again.
********************************************************************************/
HRESULT Test_Lis_SetOrientation_TopVector(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DS3DLISTENER ds3dl;
D3DVECTOR vLisTop   = {0};
D3DVECTOR vLisFront = {0};
DWORD     dwAngle = 0;
FLOAT  fAngle    = 0;


ZeroMemory(&ds3dl, sizeof(ds3dl));

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetOrientation_TopVector: eTestApply = %s", String(eTestApply));

//Put the sound to our right - this is an absolute for this test.
CHECKRUN(pSound->DSB_SetPosition(1, 0, 0));

//Set up the testing mode.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

vLisTop   = x_D3DVECTOR(0, 1, 0);
vLisFront = x_D3DVECTOR(0, 0, 1);

//Play the sound.
CONSOLEOUT("Sound should rotate around the listener's head about the Z axis.");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

for (dwAngle = 0; dwAngle <=360  && SUCCEEDED(hr); dwAngle++)
{
    //We're starting at 90 degrees, actually, so add that on.  Not reducing function for readability.  Rotating about Z axis.
    fAngle = float(((float)dwAngle + 90) * PI / 180.0f);
    vLisTop.x = (FLOAT)cos(fAngle);
    vLisTop.y = (FLOAT)sin(fAngle);

    if (dwAngle % 30 == 0)
    {
        //Since the sound is to our right, we do a 90 deg rotation to describe what the listener should hear cos(x, y) = (y, x)
        CONSOLEOUT("dwAngle = %3d; Should hear sound at x=%-8.3g, y=%-8.3g", dwAngle, vLisTop.y, vLisTop.x);
    }


    CHECKRUN(pSound->DS_SetOrientation(vLisFront.x, vLisFront.y, vLisFront.z, vLisTop.x, vLisTop.y, vLisTop.z));
    SleepEx(50);
}

SAFEDELETE(pSound);
return hr;

}







/********************************************************************************
1) Set a source to the right of the listener.
2) Point the listener to the right (0 degrees)
3) Rotate the front vector from 0->360.
4) Watch the expected pan.
********************************************************************************/
HRESULT Test_Lis_SetOrientation_FrontVector(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DS3DLISTENER ds3dl;
D3DVECTOR vLisTop   = {0};
D3DVECTOR vLisFront = {0};
DWORD     dwAngle = 0;
FLOAT  fAngle    = 0;

ZeroMemory(&ds3dl, sizeof(ds3dl));

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetOrientation_FrontVector: eTestApply = %s", String(eTestApply));

//Put the sound to our right - this is an absolute for this test.
CHECKRUN(pSound->DSB_SetPosition(1, 0, 0));

//Set up the testing mode.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

vLisTop   = x_D3DVECTOR(0, 1, 0);
vLisFront = x_D3DVECTOR(1, 0, 0);  //looking right.

//Play the sound.
CONSOLEOUT("Sound should rotate around the listener's head about the Y axis.");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

for (dwAngle = 0; dwAngle <=360  && SUCCEEDED(hr); dwAngle++)
{
    //We're starting at 0 degrees (facing right).  Rotating about Z axis.
    fAngle = float(((float)dwAngle) * PI / 180.0f);
    vLisFront.x = (FLOAT)cos(fAngle);
    vLisFront.z = (FLOAT)sin(fAngle);

    if (dwAngle % 30 == 0)
    {
        CONSOLEOUT("exp pan = %5d; dwAngle = %3d; vLisFront.x, z = %-10.3g, %-10.3g", 
                    DWORD(10000.f * vLisFront.z),
                    dwAngle,
                    vLisFront.z,
                    vLisFront.x);
    }


    CHECKRUN(pSound->DS_SetOrientation(vLisFront.x, vLisFront.y, vLisFront.z, vLisTop.x, vLisTop.y, vLisTop.z));
    SleepEx(50);
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
HRESULT Test_Lis_SetPosition_BoundaryCases(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DWORD dwIterator = 0;

//Test values.
FLOAT fTestValues[] = {1, 10, 100, 25.0, 50.0, FLT_MIN, -1, -10, -100, -25.0, -50.0, -FLT_MIN};
DWORD dwTestValue = 0;

D3DVECTOR v = {0};
D3DVECTOR vLisPos = {0};



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
            vLisPos.x = dwIterator & (1 << 0) ? fTestValues[dwTestValue] : 0;
            vLisPos.y = dwIterator & (1 << 1) ? fTestValues[dwTestValue] : 0;
            vLisPos.z = dwIterator & (1 << 2) ? fTestValues[dwTestValue] : 0;

            CHECKRUN(pSound->DS_SetPosition(vLisPos.x, vLisPos.y, vLisPos.z));
            CONSOLE("SetPosition(%8g, %8g, %8g", vLisPos.x, vLisPos.y, vLisPos.z);
            SleepEx(100);
        }
    }

    CONSOLEOUT("Stopping Sound");
    SAFEDELETE(pSound);
    return hr;
};


/********************************************************************************
Move the listener around a sound source.
********************************************************************************/
HRESULT Test_Lis_SetPosition_Listening(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
DWORD     dwAngle = 0;
FLOAT  fAngle    = 0;
D3DVECTOR vLisPos = {0};

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetPosition_Listening: eTestApply = %s", String(eTestApply));

//Set up the testing mode.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

//Play the sound.
CONSOLEOUT("Sound should rotate around the listener's head about the Y axis.");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

for (dwAngle = 0; dwAngle <=360  && SUCCEEDED(hr); dwAngle++)
{
    //We're starting at 0 degrees (to the right).  Rotating about Z axis.
    fAngle = float(((float)dwAngle) * PI / 180.0f);
    vLisPos.x = (FLOAT)cos(fAngle);
    vLisPos.z = (FLOAT)sin(fAngle);

    if (dwAngle % 30 == 0)
    {
        //We display the negative values because we're moving the listener, not the source.
        CONSOLEOUT("exp pan = %5d; dwAngle = %3d; vLisPos.x = %-10.3g, z=%-10.3g", 
                    DWORD(-10000.f * vLisPos.x),
                    dwAngle,
                    -vLisPos.x,
                    -vLisPos.z);
    }


    CHECKRUN(pSound->DS_SetPosition(vLisPos.x, vLisPos.y, vLisPos.z));
    SleepEx(50);
}


SAFEDELETE(pSound);
return hr;
}





/********************************************************************************
Verify that rolloff factors are applied correctly in general.
********************************************************************************/
HRESULT Test_Lis_SetRolloffFactor(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
D3DVECTOR vBufPos = {0};
FLOAT fFactors[] = {0, FLT_MIN, 0.001f, .5f, 1, 2, 5, 10};
DWORD    dwFactorIndex = 0;
DWORD    dwPosition = 0;

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetRolloffFactor: eTestApply = %s", String(eTestApply));

//Set up the testing mode.
CHECKRUN(pSound->DSB_Test_SetTestingApply(eTestApply));

//Verify that rolloff factor is irrelevant when set to zero.
CHECKRUN(pSound->DSB_SetMaxDistance(50.0));
CHECKRUN(pSound->DS_SetRolloffFactor(0));

//This is less distance than 50.0, but 25.0^2 == 50.0.w
CHECKRUN(pSound->DSB_SetPosition(25.0, 0, 0)); 
CONSOLEOUT("Sound is at 25.0 but you should still hear it cuz rolloff factor is zero. (2s duration)");
CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
SleepEx(2000);


for (dwFactorIndex = 0; dwFactorIndex < NUMELEMS(fFactors)  && SUCCEEDED(hr); dwFactorIndex++)
{
    CHECKRUN(pSound->DSB_Stop());
    CHECKRUN(pSound->DSB_SetCurrentPosition(0));
    CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));
    CONSOLEOUT("Rolloff Factor is %g; moving source from 1m to 10m away over 5s", fFactors[dwFactorIndex]);
    CHECKRUN(pSound->DS_SetRolloffFactor(fFactors[dwFactorIndex]));
    for (vBufPos.x = 1; vBufPos.x < 10; vBufPos.x+=0.1f)
    {        
        CHECKRUN(pSound->DSB_SetPosition(vBufPos.x, vBufPos.y, vBufPos.z));
        SleepEx(50);
    }
}

SAFEDELETE(pSound);
return hr;
}






/********************************************************************************
Basically a copy of the Buffer test.
Tests the doppler effect on buffers on all 3 axes.
********************************************************************************/
HRESULT Test_Lis_SetVelocity(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
BOOL bPlaying = FALSE;
DWORD dwVelocity = 0;
D3DVECTOR vLisPos = {0};
D3DVECTOR vLisVel = {0};
DWORD dwAxis = 0;  //x=0, y=1, z=2.
FLOAT fPosition[2]; //this will be x,y, x,z, or y,z depending on dwAxis.
char *pszAxes[] = {"x", "y", "z"};
DWORD i = 0;
FLOAT fTestVelocity = 1000.f;
FLOAT fTestVal = 0.f;

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_Lis_SetVelocity: eTestApply = %s", String(eTestApply));

//Move the listener from directly in back to directly in front of the object, going around the left side and staying a meter away.
bPlaying = FALSE;
for (dwAxis = 0; dwAxis < 3; dwAxis++)
{
    CONSOLE("Testing doppler along %s axis", pszAxes[dwAxis]);

    //We want our initial changes guaranteed to be applied.
    CHECKRUN(pSound->DSB_Test_SetTestingApply(TESTAPPLY_IMMEDIATE));

    //Set the position at 1 along the designated axis.
    vLisPos = x_D3DVECTOR(0, 0, 0);
    CHECK(SetComponent(&vLisPos, dwAxis, -1));
    CONSOLE("Setting position to %g, %g, %g", vLisPos.x, vLisPos.y, vLisPos.z);
    CHECKRUN(pSound->DSB_SetPosition(vLisPos.x, vLisPos.y, vLisPos.z));

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
        vLisVel = x_D3DVECTOR(0, 0, 0);
        CHECK(SetComponent(&vLisVel, (dwAxis + 1 + i) %3, -fTestVelocity));
        CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
        SleepEx(3000);
    }

   
    //Verify doppler works along test axis.
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to increase", pszAxes[dwAxis], -fTestVelocity);
    vLisVel = x_D3DVECTOR(0, 0, 0);
    CHECK(SetComponent(&vLisVel, dwAxis, -fTestVelocity));
    CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
    SleepEx(3000);

    
    FLOAT fDopplerFactor[] = {1, 2};
    for (i=0; i<NUMELEMS(fDopplerFactor) && SUCCEEDED(hr); i++)
    {    
        //Set doppler factor.
        CONSOLE("Setting Doppler Factor to %g", fDopplerFactor[i]);
        CHECKRUN(pSound->DS_SetDopplerFactor(fDopplerFactor[i]));
        

        //Increase doppler along test axis, verify increase in pitch.
        vLisVel = x_D3DVECTOR(0, 0, 0);
        CONSOLE("Increasing %s component of velocity vector from 0 to %g for 3s -> expect pitch of sound to increase", pszAxes[dwAxis], -fTestVelocity);
        for (fTestVal = 0; fTestVal>=-fTestVelocity; fTestVal--)
        {
            CHECK(SetComponent(&vLisVel, dwAxis, fTestVal));
            CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
            SleepEx(5);
        }

        //Decrease doppler.
        vLisVel = x_D3DVECTOR(0, 0, 0);
        CONSOLE("Increasing %s component of velocity vector from 0 to %g for 3s -> expect pitch of sound to decrease", pszAxes[dwAxis], fTestVelocity);
        for (fTestVal = 0; fTestVal<=fTestVelocity; fTestVal++)
        {
            CHECK(SetComponent(&vLisVel, dwAxis, fTestVal));
            CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
            SleepEx(5);
        }
    }

    CHECKRUN(pSound->DS_SetDopplerFactor(1));

//BUGBUG: Removed due to 2543

/*
    //Set doppler to max.
    vLisVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be high", pszAxes[dwAxis], -25.0);
    CHECK(SetComponent(&vLisVel, dwAxis, -25.0));
    CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
    SleepEx(3000);

    //Set doppler to min.
    vLisVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be low", pszAxes[dwAxis], 25.0);
    CHECK(SetComponent(&vLisVel, dwAxis, 25.0));
    CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
    SleepEx(3000);

    //Set doppler to max.
    vLisVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be high", pszAxes[dwAxis], -50.0);
    CHECK(SetComponent(&vLisVel, dwAxis, -50.0));
    CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
    SleepEx(3000);

    //Set doppler to min.
    vLisVel = x_D3DVECTOR(0, 0, 0);
    CONSOLE("Setting %s component of velocity vector to %g for 3s -> expect pitch of sound to be low", pszAxes[dwAxis], 50.0);
    CHECK(SetComponent(&vLisVel, dwAxis, 50.0));
    CHECKRUN(pSound->DS_SetVelocity(vLisVel.x, vLisVel.y, vLisVel.z));
    SleepEx(3000);
*/

}//end for dwAxis = 1 to 3.

CHECKRUN(pSound->DSB_Stop());
SAFEDELETE(pSound);
return hr;
}





/********************************************************************************
Verifies that both the listener and buffer settings are committed when
appropriate, and not committed when not appropriate.

There is no apply value because this case is TESTING the apply value, not
abstracting it.

There is no vRelative because we're not using the wrapper class.
********************************************************************************/
HRESULT Test_Lis_CommitDeferredSettings(char *szFileName)
{
HRESULT hr= S_OK;
LPDIRECTSOUND       pDS  = NULL;
LPDIRECTSOUNDBUFFER pDSB = NULL;
LPVOID pvSoundData       = NULL;
DWORD dwWait             = 3000;


    //Create the DSound object (our "listener")
#ifdef SILVER
    CHECKRUN(Help_DirectSoundCreate(DSDEVID_DEVICEID_MCPX, &pDS, NULL));
#else // SILVER
    CHECKRUN(Help_DirectSoundCreate(0, &pDS, NULL));
#endif 

    //CHECKRUN(DownloadScratch(pDS, "T:\\Media\\DSPCode\\DSSTDFX.bin" ));
    CHECKRUN(DownloadLinkedDSPImage(pDS));

    CONSOLE("\n----- Test_Lis_CommitDeferredSettings");

    //Load a random wave file into a buffer.
    if (szFileName)
    {
        CHECKRUN(LoadWaveFile(NULL, &pvSoundData, &pDSB, szFileName));
    }
    else
    {
        CHECKRUN(LoadRandomWaveFile(NULL, &pvSoundData, &pDSB));
    }

    //Play the file.
    CHECKRUN(pDSB->Play(0, 0, DSBPLAY_LOOPING));
    CONSOLE("Default sound playing for %d ms", dwWait);
    SleepEx(dwWait);

 
    //SetPosition(deferred).
    CHECKRUN(pDSB->SetPosition(4, 0, 0, DS3D_DEFERRED));
    CHECKRUN(pDS ->SetPosition(2, 0, 0, DS3D_DEFERRED));
    CONSOLE("Moved source right 4m and listener right 2m.  Should hear no change in sound (playing %d ms)", dwWait);
    SleepEx(dwWait);


    //Solidify everything.
    CHECKRUN(pDS->CommitDeferredSettings());
    CONSOLE("Called CommitDeferredSettings, sound should be 2m to right of listener and decrease in volume (playing %d ms)", dwWait);
    SleepEx(dwWait);

    //Verify the sound changes.
    CHECKRUN(pDS ->SetPosition(0, 0, 0, DS3D_IMMEDIATE));
    CONSOLE("Moved listener to origin.  Sound should move 4m to right of listener and decrease in volume again (playing %d ms)", dwWait);
    SleepEx(dwWait);


    //Release buffers
    CONSOLE("Done.  Releasing buffers");
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

