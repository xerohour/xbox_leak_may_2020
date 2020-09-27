#include "globals.h"
#include "cicmusicx.h"

//TODO: Continue conversion here.


/********************************************************************************
********************************************************************************/
HRESULT Performance8_ReferenceToMusicTime_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
CICMusic Music;
MUSIC_TIME mtBase = 0, mtConverted = 0;
REFERENCE_TIME rtBase = 0;

//The 
//768 * 100BPM / 60s = MUSIC_TIME ticks per second.  
//FLOAT fTempo = 100.f;
FLOAT fMaxErrorPercent = 0.0f;  //1/3 percent error is allowed on this one.
FLOAT fErrorPercent = 0.f;
HRESULT hr = S_OK;

//Load media.
MEDIAFILEDESC MediaFileDesc[]=
{
{"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
{"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));

//Play some muzak, it will play at 100 BPM.  That's 2 BPS, or quarter-notes per second.  So each second, DMUS_PPQ*2 
//  MUSIC_TIME units will go by.
CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Test.sgt", DMUS_APATH_DYNAMIC_MONO));

//Get the base time in both formats.
CHECKRUN(ptPerf8->GetTime(&rtBase, &mtBase));

//Convert one to the other.
CHECKRUN(ptPerf8->ReferenceToMusicTime(rtBase, &mtConverted));

//Get the error percent.
fErrorPercent = (FLOAT)CalculateErrorPercentage((double)mtBase, (double)mtConverted);

//Check the conversion.
Log(FYILOGLEVEL, "Error is %6.2f percent, maximum allowed is %6.2f percent.", fErrorPercent, fMaxErrorPercent);
if (fErrorPercent> fMaxErrorPercent)
{
    hr = E_FAIL;
    goto END;
}

END:


return hr;
};



/********************************************************************************
RT units = MT   beats    mt's    seconds   rt units
              / ------ / ----- * ------  * ------
                minute   beat    minute    second

RT units = MT   minutes  beats   seconds   rt units
              * ------  *----- * ------  * ------
                beat     mt      minute    second

RT units = MT   1                       1    60    10000000
              * ------               *  ---- --- * ------  
                (BPM * fTempoFactor)    768  1     1

RT units = MT   1                       1    60    10000000
              * ------               *  ---- --- * ------  
                (BPM * fTempoFactor)    768  1     1


RT units = MT   1                       1           60    10000000
              * ------               *  ----        --- * ------  
                (BPM * fTempoFactor)    DMUS_PPQ    1     1




RT units = MT   600000000
              * ------              
                BPM * fTempoFactor * DMUS_PPQ

MT units = RT Units * BMP * fTempoFactor * DMUS_PPQ
           ----------------------------------------
           600000000

********************************************************************************/

HRESULT Performance8_ReferenceToMusicTime_TempoChange(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
MUSIC_TIME mtBase = 0;
REFERENCE_TIME rtBase = 0;
REFERENCE_TIME rtDeltas[] = {0, 1, 100000, 20000000, 36000000000, 864000000000 }; //2 seconds, 1 hour, 1 day.
FLOAT fTempo = 100.f;
MUSIC_TIME mtConverted = 0;
MUSIC_TIME mtTotalDeltaExpected = 0;
MUSIC_TIME mtConvertedExpected = 0;
FLOAT fTempoFactors[] = {1.f, 0.5f, 0.01f, .2f, 100.f};
DWORD i = 0;
DWORD dwTempoIndex = 0;
FLOAT fNewTempoFactor= 0;
FLOAT fErrorPercent = 0.f, fMaxErrorPercent = 1.f;

//Load media.
MEDIAFILEDESC MediaFileDesc[]=
{
{"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
{"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));


//Play some muzak, it will play at 120 BPM.  That's 2 BPS, or quarter-notes per second.  So each second, DMUS_PPQ*2 
//  MUSIC_TIME units will go by.
CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Test.sgt", DMUS_APATH_DYNAMIC_MONO));


for (dwTempoIndex = 0; dwTempoIndex < AMOUNT(fTempoFactors) && SUCCEEDED(hr); dwTempoIndex++)
{
    //Set the tempo factor.
    fNewTempoFactor = fTempoFactors[dwTempoIndex];
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempoFactor, sizeof(fNewTempoFactor)));
    Log(FYILOGLEVEL, "Setting fNewTempoFactor to %g, will hear music at %g times normal tempo.", fNewTempoFactor, fNewTempoFactor);
    Wait(4000);

    //Get the base time.
    CHECKRUN(ptPerf8->GetTime(&rtBase, &mtBase));
    Log(FYILOGLEVEL, "Base times: mt = %u, rt = %I64u", mtBase, rtBase);

    //Check that all the deltas are converted correctly.
    for (i=0; i<AMOUNT(rtDeltas) && SUCCEEDED(hr); i++)
    {
        CHECKRUN(ptPerf8->ReferenceToMusicTime(rtDeltas[i] + rtBase, &mtConverted));
        
        
        //Calculate expected time.
        REFERENCE_TIME rtTemp = 0;
        mtTotalDeltaExpected = 0;

        rtTemp = (REFERENCE_TIME)(FLOAT(rtDeltas[i]) * FLOAT(fTempo) * FLOAT(fNewTempoFactor) * FLOAT(DMUS_PPQ) / 600000000.f);        
        if (rtTemp > 0x7FFFFFFF)
        {
            Log(FYILOGLEVEL, "ERROR: Expected mtTemp would be %016I64X - greater than mt can hold", rtTemp);
            goto END;
        };

        mtTotalDeltaExpected = (MUSIC_TIME)rtTemp;
        //mtTotalDelta = mtConverted - mtBase;
        mtConvertedExpected = mtBase + mtTotalDeltaExpected;
        
        //Check the error factor here.
        fErrorPercent = (FLOAT)CalculateErrorPercentage((double)mtConvertedExpected, (double)mtConverted);
        Log(FYILOGLEVEL, "TempoFactor = %g, Delta = %020I64u: Error = %5.2f%%, MaxError= %5.2f%%.", fNewTempoFactor, rtDeltas[i], fErrorPercent, fMaxErrorPercent);
        if (fErrorPercent> fMaxErrorPercent)
        {
            Log(ABORTLOGLEVEL, "   InParam  rt=%020I64u", rtDeltas[i] + rtBase);
            Log(ABORTLOGLEVEL, "   OutParam mt=%u",       mtConverted);
            Log(ABORTLOGLEVEL, "   Expected mt=%u",       mtConvertedExpected);
            hr = E_FAIL;
            goto END;
        }

    }

}

END:

//reset the tempo back to normal
fNewTempoFactor = 1.f;
if (ptPerf8)
    ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempoFactor, sizeof(fNewTempoFactor));

return hr;
};

