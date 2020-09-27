/********************************************************************************
FILE: DMTEST1.CPP

PURPOSE:    
    Contains the "main code segment" for DANROSE's set of DMusic tests
    from which all the subfunctions (individual test cases) are called.  
********************************************************************************/

#include "globals.h"
#include "tests.h"

CHAR *g_szDefaultMedia = "T:\\Media\\test.sgt";

//BUGBUG: Remove this eventually.
HRESULT Loader8_LoadObjectTwice(CtIDirectMusicPerformance8* ptPerf8);


/******************************************************************************************
******************************************************************************************/
HRESULT DanHaff_Tests(HANDLE LogHandle, CtIDirectMusicPerformance8 *ptPerf8)
{
DWORD dwRes;
HRESULT hr = S_OK;


    //Call some functions, yo.
    LogSetDbgLevel(PARAMLOGLEVEL);

	//SETLOG( LogHandle, "danhaff", "DMusic", "Sample Test", "Regression" );
    //CHECKEXECUTE(SampleTest());

    //Audiopath Tests
//	SETLOG( LogHandle, "danhaff", "DMusic", "AudioPath_Activate", "Regression" );         //PASSES.
//    CHECKEXECUTE(AudioPath_Activate(ptPerf8));                

	SETLOG( LogHandle, "danhaff", "DMusic", "AudioPath_GetObjectInPath", "Regression" );  //PASSES
    CHECKEXECUTE(AudioPath_GetObjectInPath(ptPerf8));
	//SETLOG( LogHandle, "danhaff", "DMusic", "AudioPath_SetVolume", "Regression" );        //FAILs due to 2904 and 2905.
//    CHECKEXECUTE(AudioPath_SetVolume(ptPerf8));                 
    
    //Performance8 Tests
//    CHECKEXECUTE(Performance8_AddNotificationType(ptPerf8));                                //FAILS due to 2918
//    CHECKEXECUTE(Performance8_AllocPMsg(ptPerf8));                                          //NA, Covered in SendPMsg
//    CHECKEXECUTE(Performance8_CloseDown(ptPerf8));                                          //N/A has been removed for February.
//    CHECKEXECUTE(Performance8_CreateStandardAudioPath(ptPerf8));                               //FAILS due to 2762, 2987
//    CHECKEXECUTE(Performance8_FreePMsg(ptPerf8));                                           //NA, Covered in SendPMsg
//    CHECKEXECUTE(Performance8_GetGlobalParam(ptPerf8));                                     //FAILS due to 2921
//    CHECKEXECUTE(Performance8_GetNotificationPMsg(ptPerf8));                                //NA, Covered in SendPMsg
//    CHECKEXECUTE(Performance8_GetSegmentState(ptPerf8));                                    //PASSES, but sounds crappy.  TODO: figure this out.
//    CHECKEXECUTE(Performance8_GetTime(ptPerf8));                                            //PASSES!
//    CHECKEXECUTE(Performance8_IsPlaying(ptPerf8));                                          //FAILS due to 2953
//    CHECKEXECUTE(Performance8_PlaySegmentEx(ptPerf8));                                      //FAILS on ported tests, but they look wrong.  Passes under normal conditions.
//    CHECKEXECUTE(Performance8_RemoveNotificationType(ptPerf8));                             //NA, covered in AddNotificationType.
//    CHECKEXECUTE(Performance8_SendPMsg(ptPerf8));                                           //FAILS due to 2968
//    CHECKEXECUTE(Performance8_SetGlobalParam(ptPerf8));                                     //PASSES!
//    CHECKEXECUTE(Performance8_StopEx(ptPerf8));                                             //FAILS due to 2987

    //Segment8 Tests
//    CHECKEXECUTE(Segment8_Download(ptPerf8));                                               //PASSES
//    CHECKEXECUTE(Segment8_SetRepeats(ptPerf8));                                             //PASSES
//    CHECKEXECUTE(Segment8_Unload(ptPerf8));                                                 //NA, covered in download.

    //SegmentState8 Tests
    CHECKEXECUTE(SegmentState8_GetObjectInPath(ptPerf8));                                      //PASSES


//  Loader Tests
//      CHECKEXECUTE(Loader8_LoadObjectTwice(ptPerf8));

    return hr;


}




/******************************************************************************************
CALLDMTEST calls CHECKEXECUTE and also translates a DMTest return code into an hr.

DanRose is HUGE!!!!  Grrr.
******************************************************************************************/
HRESULT DanRose_Tests( HANDLE LogHandle, CtIDirectMusicPerformance8 *ptPerf8 )
{
	HRESULT hr = S_OK;

	// Script Tests

	SETLOG( LogHandle, "danrose", "DMusic", "Script_Init", "Regression" );
    CHECKEXECUTE( Script_Init             ( ptPerf8 ) );   
	
	SETLOG( LogHandle, "danrose", "DMusic", "Script_CallRoutine", "Regression" );
    CHECKEXECUTE( Script_CallRoutine      ( ptPerf8 ) );                  
	
	SETLOG( LogHandle, "danrose", "DMusic", "Script_SetVariableNumber", "Regression" );
    CHECKEXECUTE( Script_SetVariableNumber( ptPerf8 ) );                  
	
	SETLOG( LogHandle, "danrose", "DMusic", "Script_GetVariableNumber", "Regression" );
    CHECKEXECUTE( Script_GetVariableNumber( ptPerf8 ) );                  

	SETLOG( LogHandle, "danrose", "DMusic", "Script_SetvariableObject", "Regression" );
    CHECKEXECUTE( Script_SetVariableObject( ptPerf8 ) );                  
	
	SETLOG( LogHandle, "danrose", "DMusic", "Script_GetVariableObject", "Regression" );
    CHECKEXECUTE( Script_GetVariableObject( ptPerf8 ) );       
                                                                              
	return hr;
}

/******************************************************************************************
These tests must be called on their own test runs, by themselves
******************************************************************************************/
HRESULT Fatal_Tests(HANDLE LogHandle, CtIDirectMusicPerformance8 *ptPerf8)
{
HRESULT hr = S_OK;
//    CHECKEXECUTE(Performance8_Release(ptPerf8));
//    CHECKEXECUTE(Performance8_IsPlaying_Invalid(ptPerf8));

//    CHECKEXECUTE(AudioPath_SetVolume_Invalid(ptPerf8));

//    CALLDMTEST(Performance8_InitAudio(ptPerf8, 0, 0, 0, 0));                               
//    CALLDMTEST(Performance8_InitAudio_Invalid_Feb1(ptPerf8, 0, 0, 0, 0));

return hr;
};

/********************************************************************************
Main code segment for the dmusic tests.
********************************************************************************/
VOID WINAPI DMTest1StartTest(HANDLE LogHandle)
{
	DWORD dwRes = 0;
    HRESULT hr = S_OK;
    CtIDirectMusicPerformance8 *ptPerf8 = NULL;

    //We create and init a performance up front and then use it in all our tests.
    CHECKEXECUTE(CreateAndInitPerformance(&ptPerf8));
    if (FAILED(hr))
        Log(ABORTLOGLEVEL, "CreateAndInitPerformance returned %s (%08X)", dmthXlatHRESULT(hr), hr);

	CHECKEXECUTE(Fatal_Tests  (LogHandle, ptPerf8));
    CHECKEXECUTE(DanHaff_Tests(LogHandle, ptPerf8));
    CHECKEXECUTE( DanRose_Tests( LogHandle, ptPerf8 ) );

    //Closedown and release the performance.
    
    //TODO: Add these calls back in when bug 2642 is fixed.
    //hr = ptPerf8->CloseDown();
    //SAFE_RELEASE(ptPerf8);

}


/********************************************************************************
Placeholder fuunction - does nothing.
********************************************************************************/
VOID WINAPI DMTest1EndTest( VOID )
{
}


/********************************************************************************
Export function pointers of StartTest and EndTest
********************************************************************************/
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DMTest1 )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DMTest1 )
    EXPORT_TABLE_ENTRY( "StartTest", DMTest1StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DMTest1EndTest )
END_EXPORT_TABLE( DMTest1 )



   
