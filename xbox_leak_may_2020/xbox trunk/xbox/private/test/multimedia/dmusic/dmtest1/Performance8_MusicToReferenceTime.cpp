#include "globals.h"
#include "cicmusicx.h"


/********************************************************************************
********************************************************************************/
HRESULT Performance8_MusicToReferenceTime_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
CICMusic Music;
MUSIC_TIME mtBase = 0;
REFERENCE_TIME rtBase = 0, rtConverted = 0;

//The 
//768 * 100BPM / 60s = MUSIC_TIME ticks per second.  
//FLOAT fTempo = 100.f;
FLOAT fMaxErrorPercent = 0.33f;  //1/3 percent error is allowed on this one.
FLOAT fErrorPercent = 0.f;
HRESULT hr = S_OK;


//Load the needed medja.
MEDIAFILEDESC MediaFileDesc[] = {
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
CHECKRUN(ptPerf8->MusicToReferenceTime(mtBase, &rtConverted));

//Get the error percent.
fErrorPercent = (FLOAT)CalculateErrorPercentage((double)rtBase, (double)rtConverted);

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
********************************************************************************/
HRESULT Performance8_MusicToReferenceTime_Repeat(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
CICMusic Music;
MUSIC_TIME mtBase = 0;
REFERENCE_TIME rtBase = 0;

MUSIC_TIME mtConverted = 0;
REFERENCE_TIME rtConverted = 0;


//The 
//768 * 100BPM / 60s = MUSIC_TIME ticks per second.  
//FLOAT fTempo = 100.f;
FLOAT fErrorPercent = 0.f;
FLOAT fMaxErrorPercent = 2.f;
HRESULT hr = S_OK;
DWORD i = 0;

//Load the needed medja.
MEDIAFILEDESC MediaFileDesc[] = {
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

mtConverted = mtBase;
rtConverted = rtBase;

//Repeatedly convert back and forth.
Log(FYILOGLEVEL, "Repeatedly converting MUSIC_TIME->REFERENCE_TIME...");
for (i=0; i<10000; i++)
{
    //Convert one to the other.
    CHECKRUN(ptPerf8->MusicToReferenceTime(mtConverted, &rtConverted));
    CHECKRUN(ptPerf8->ReferenceToMusicTime(rtConverted, &mtConverted));
    if (i%1000 == 0)
    {   
        Log(FYILOGLEVEL, "Iteration %5d: MUSIC_TIME = %08X, REFERENCE_TIME = %I64X", i, mtConverted, rtConverted);
    }
}

//Check REFERENCE_TIME
fErrorPercent = (FLOAT)fabs( FLOAT(rtConverted - rtBase) / FLOAT(rtBase)) * 100.f;
Log(FYILOGLEVEL, "Error is %6.2f percent, maximum allowed is %6.2f percent.", fErrorPercent, fMaxErrorPercent);
if (fErrorPercent> fMaxErrorPercent)
{
    hr = E_FAIL;
    goto END;
}

//Check MUSIC_TIME
fErrorPercent = (FLOAT)CalculateErrorPercentage((double)mtBase, (double)mtConverted);
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

********************************************************************************/
HRESULT Performance8_MusicToReferenceTime_TempoChange(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
MUSIC_TIME mtBase = 0;
REFERENCE_TIME rtBase = 0;
MUSIC_TIME mtDeltas[] = {0, 1, DMUS_PPQ, 0x70000};
REFERENCE_TIME rtConverted = 0;
REFERENCE_TIME rtTotalDeltaExpected = 0;
REFERENCE_TIME rtConvertedExpected = 0;
FLOAT fTempoFactors[] = {1.f, 0.5f, 0.01f, .2f, 100.f};
DWORD i = 0;
DWORD dwTempoIndex = 0;
FLOAT fNewTempoFactor= 0;
FLOAT fErrorPercent = 0.f, fMaxErrorPercent = 1.f;


//Load the needed medja.
MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
CHECKRUN(LoadMedia(MediaFileDesc));


//Play some muzak, it will play at 100 BPM.  That's 2 BPS, or quarter-notes per second.  So each second, DMUS_PPQ*2 
//  MUSIC_TIME units will go by.
CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Test.sgt", DMUS_APATH_DYNAMIC_MONO));


for (dwTempoIndex = 0; dwTempoIndex < AMOUNT(fTempoFactors) && SUCCEEDED(hr); dwTempoIndex++)
{
    //Set the tempo factor.
    fNewTempoFactor = fTempoFactors[dwTempoIndex];
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempoFactor, sizeof(fNewTempoFactor)));
    Log(FYILOGLEVEL, "Setting fNewTempoFactor to %g, will hear music at %g times normal tempo.", fNewTempoFactor, fNewTempoFactor);
    Log(FYILOGLEVEL, "Waiting 2 seconds");
    Wait(2000);

    //Get the base time.
    CHECKRUN(ptPerf8->GetTime(&rtBase, &mtBase));
    Log(FYILOGLEVEL, "Base times: mt = %u, rt = %I64u", mtBase, rtBase);

    //Check that all the deltas are converted correctly.
    for (i=0; i<AMOUNT(mtDeltas) && SUCCEEDED(hr); i++)
    {
        CHECKRUN(ptPerf8->MusicToReferenceTime(mtDeltas[i] + mtBase, &rtConverted));
        rtTotalDeltaExpected = REFERENCE_TIME(mtDeltas[i]) * 600000000 / REFERENCE_TIME( FLOAT(DMUS_PPQ * 100) * fNewTempoFactor);
        //rtTotalDelta = rtConverted - rtBase;

        rtConvertedExpected = rtBase + rtTotalDeltaExpected;
        
        //Check the error factor here.
        fErrorPercent = (FLOAT)CalculateErrorPercentage((double)rtConvertedExpected, (double)rtConverted);
        Log(FYILOGLEVEL, "TempoFactor = %g, Delta = 0x%08X: Error = %5.2f%%, MaxError= %5.2f%%.", fNewTempoFactor, mtDeltas[i], fErrorPercent, fMaxErrorPercent);
        if (fErrorPercent> fMaxErrorPercent)
        {
            Log(ABORTLOGLEVEL, "   InParam  mt=%u", mtDeltas[i] + mtBase);
            Log(ABORTLOGLEVEL, "   OutParam rt=%020I64u", rtConverted);
            Log(ABORTLOGLEVEL, "   Expected rt=%020I64u", rtConvertedExpected);
            hr = E_FAIL;
            goto END;
        }

    }

}

//Reset the tempo.
fNewTempoFactor = 1.f;
ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempoFactor, sizeof(fNewTempoFactor));


END:
return hr;
};





