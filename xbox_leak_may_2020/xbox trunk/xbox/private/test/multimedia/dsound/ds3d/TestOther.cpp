#include "globals.h"
#include "Sound3D.h"


/********************************************************************************
Tests the doppler effect on buffers on all 3 axes.
********************************************************************************/
HRESULT Test_Buf_SetVolume(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName)
{
HRESULT hr = S_OK;
SOUND3D *pSound = NULL;
LONG i = 0;

ALLOCATEANDCHECK(pSound, SOUND3D);
CHECKRUN(pSound->Init(vRelative, szFileName));
CONSOLEOUT("\n----- Test_SetVolume: eTestApply = %s", String(eTestApply));

CHECKRUN(pSound->DSB_Play(0, 0, DSBPLAY_LOOPING));

CONSOLEOUT("\n----- Should hear sound at normal volume for 1s");
Sleep(1000);

//1000 iterations
for (i=DSBVOLUME_MAX; i>=DSBVOLUME_MIN; i-=10)
{
    CHECKRUN(pSound->DSB_SetVolume(i));
    Sleep(5);
    if (i % 200 == 0)
        CONSOLEOUT("SetVolume(%d)", i);
}


CHECKRUN(pSound->DSB_Stop());
SAFEDELETE(pSound);
return hr;
}







