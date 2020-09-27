#include "globals.h"

DWORD Segment8_DownloadUnload_BVT(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Segment8_Download (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Segment8_Download())");

    CALLDMTEST(Segment8_DownloadUnload_BVT(ptPerf8, 0, 0, 0, 0));           //PASSES

    return hr;
};


/********************************************************************************
********************************************************************************/
DWORD Segment8_DownloadUnload_BVT(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CHAR *szFileName[2] = {"T:\\Media\\Segment8\\Download\\One.sgt",
                       "T:\\Media\\Segment8\\Download\\Two.sgt"};

CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
BOOL bDownloadToPerf = 0;
DWORD dwSegment = 0;


    //Create a audiopath.
    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }
    
    //Try downloading to both the performance and the audiopath.
    for (bDownloadToPerf = 0; bDownloadToPerf < 2; bDownloadToPerf++)
    {
        //Play both the segments.
        for (dwSegment=0; dwSegment<2; dwSegment++)
        {
            //Load up the media file.
            hr = dmthLoadSegment(szFileName[dwSegment], &ptSegment8);
	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment(%s) returned %s (%08Xh)",szFileName[dwSegment], tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }


            //Download everything.
            fnsLog(FYILOGLEVEL, "Downloading to %s interface", bDownloadToPerf ? "Performance8" : "AudioPath");
            hr = ptSegment8->Download(bDownloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath); 
	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }
            //bDownloaded = TRUE;

            //Play the segment.
	        hr = ptPerf8->PlaySegmentEx( 
		        ptSegment8,
		        0,
                NULL, 			
		        0, 	
		        0,
                NULL,
                NULL,
                ptPath
                );
	        if(FAILED(hr))
	        {
		        fnsLog(ABORTLOGLEVEL, "!!!! BUG: pwPerformance->PlaySegmentEx failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
                hr = ptSegment8->Unload(bDownloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);
	            if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
                    fnsLog(ABORTLOGLEVEL, "**** TEST ERROR: Could not unload - Test case is leaving a segment downloaded!!!!!!");
		            goto TEST_END;
	            }
		        goto TEST_END;
	        }
            fnsLog(FYILOGLEVEL, "Playing segment for 5 seconds, you should hear the phrase ""%d"" being spoken", dwSegment+1);
            Sleep(5000);

            fnsLog(FYILOGLEVEL, "Unloading from %s interface", bDownloadToPerf ? "Performance8" : "AudioPath");
            hr = ptSegment8->Unload(bDownloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);
	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
                fnsLog(ABORTLOGLEVEL, "**** TEST ERROR: Could not unload - Test case is leaving a segment downloaded!!!!!!");
		        goto TEST_END;
	        }

            SAFE_RELEASE(ptSegment8);            
        }
    };




    dwRes = FNS_PASS;

TEST_END:
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);
    return dwRes;
};





