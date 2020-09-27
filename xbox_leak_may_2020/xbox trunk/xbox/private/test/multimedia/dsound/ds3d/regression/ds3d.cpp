/********************************************************************************
FILE: DS3D.CPP

PURPOSE:    
    Contains the "main code segment" for 3D sound tests from which all the
    subfunctions (individual test cases) are called.  

NOTES:
    All of the main test functions take 3 parameters:
    
    [enum] TestApplies eTestApply
    ----------------------
    This is passed to the test class.  It is one of 3 values:
    TESTAPPLY_DEFERRED_NOUPDATE,
        Tells the test class to call methods with DS3D_DEFERRED but not update 
        them.  If you're using the CONSOLEOUT macro, this class will inject a warning
        into your console spew the fact that results will be different from what the
        debug output says to expect.

    TESTAPPLY_DEFERRED_UPDATE
        Tells the test class to call methods with DS3D_DEFERRED and then call 
        CommitDeferredSettings() immediately after.  This should behave exactly like
        TESTAPPLY_IMMEDIATE.

     TESTAPPLY_IMMEDIATE
        Tells the test class to call methods with DS3D_IMMEDIATE

    D3DVECTOR vRelative
    -------------------
    This instructs the wrapper class to move both the source and listener by this
    amount in 3D space.  This is so we don't keep running the simple "around the
    origin" test scenarios and miss something.  If NULL_VECTOR (0, 0, 0) is passed in here
    then this will be totally ignored.  However, if a real vector is passed in, it
    will necessitate a call to IDirectSound::SetPosition as well as
    IDirectSoundBuffer::SetPosition even if only one was called.  


    char *szFileName
    ----------------
    Full path of file with which to test.  NULL means "choose a random .wav file
    from the path WAVEFILESDIR" (defined in Sound3D.cpp).  Stereo and mono 8 and 16
    bit files are the only ones that will work here.  Stereo files are manually downmixed
    to mono, since 3D stereo buffers are illegal.



CONSOLEOUT AND CONSOLE MACROS
    The tests are designed to spew stuff to the screen.  Unfortunately the XBox text
    output lib, "dxconio", doesn't work on the NV15.  So my macro currently points to
    a function named xDebugStringA, which spews to the debugger saying "someday this will be
    console output" :)  It should be real easy to fix this, but some of the strings
    may need to be shortened up depending on the width of console text.
********************************************************************************/

#include "globals.h"
#include "sound3d.h"
#include "testbuf.h"
#include "testlis.h"
#include "testother.h"
#include "util.h"

#define NULL_VECTOR (x_D3DVECTOR(0, 0, 0))

static const LPCSTR szTestFiles[] = { 
	"2723.wav"  // 48000, 16, 2												
/*	"2615.wav",  // 48000, 16, 1												
	"2603.wav",  // 48000,  8, 2
	"2674.wav",  // 48000,  8, 1 
	"167.wav",   // 44100, 16, 2 
	"677.wav",   // 44100, 16, 1
	"682.wav",   // 44100,  8, 2
	"681.wav",   // 44100,  8, 1
	"1947.wav",  // 32000, 16, 2
	"2301.wav",  // 32000, 16, 1
	"1892.wav",  // 32000,  8, 2
	"1891.wav",  // 32000,  8, 1
	"1596.wav",  // 22050, 16, 2
	"1210.wav",  // 22050, 16, 1
	"1226.wav",  // 22050,  8, 2
	"5.wav",     // 22050,  8, 1
	"664.wav",   // 16000, 16, 2
	"663.wav",   // 16000, 16, 1
	"666.wav",   // 16000,  8, 2
	"665.wav",   // 16000,  8, 1
	"991.wav",   // 11025, 16, 2
	"823.wav",   // 11025, 16, 1
	"831.wav",   // 11025,  8, 2
	"816.wav",   // 11025,  8, 1
	"2770.wav",  //  8000, 16, 2
	"2768.wav",  //  8000, 16, 1
	"2772.wav",  //  8000,  8, 2
	"2761.wav"   //  8000,  8, 1 */
};


/********************************************************************************
Main code segment for the 3D Sound tests.
********************************************************************************/
VOID WINAPI DS3DStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;
	SETLOG( LogHandle, "danrose", "DSOUND", "3D", "Regression" );
    static char szFullPath[1000];
    DWORD i = 0;
    D3DVECTOR vRelative = NULL_VECTOR;

	DirectSoundUseFullHRTF();


//    sprintf(szFullPath, "%s%s", WAVEFILESDIR, szTestFiles[0]);
//    CHECKEXECUTE(Test_INFINITY_Bug(TESTAPPLY_IMMEDIATE, vRelative, szFullPath);

    TestApplies eTestApply;
    for (i=0; i<NUMELEMS(szTestFiles); i++)
    {
        sprintf(szFullPath, "%s%s", WAVEFILESDIR, szTestFiles[i]);
        for (eTestApply = TESTAPPLY_IMMEDIATE; eTestApply >= TESTAPPLY_DEFERRED_NOUPDATE; eTestApply = TestApplies((int)eTestApply - 1))
        {
            // Buffer Tests
            // ------------

	    	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetAllParameters" );
            CHECKEXECUTE(Test_Buf_SetAllParameters(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetConeAngles" );
            CHECKEXECUTE(Test_Buf_SetConeAngles(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetConeOrientation_BoundaryCases" );
            CHECKEXECUTE(Test_Buf_SetConeOrientation_BoundaryCases(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetConeOrientation_Listening" );
            CHECKEXECUTE(Test_Buf_SetConeOrientation_Listening(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetConeOutsideVolume" );
            CHECKEXECUTE(Test_Buf_SetConeOutsideVolume(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetMaxDistance_BoundaryCases" );
            CHECKEXECUTE(Test_Buf_SetMaxDistance_BoundaryCases(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetMaxDistance_InsideSource" );
            CHECKEXECUTE(Test_Buf_SetMaxDistance_InsideSource(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetMinDistance_BoundaryCases" );
            CHECKEXECUTE(Test_Buf_SetMinDistance_BoundaryCases(eTestApply, vRelative, szFullPath));
    
			SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetMinDistance_Listening" );
            CHECKEXECUTE(Test_Buf_SetMinDistance_Listening(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetMode" );
            CHECKEXECUTE(Test_Buf_SetMode(eTestApply, vRelative, szFullPath));

			SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetPosition_BoundaryCases" );
            CHECKEXECUTE(Test_Buf_SetPosition_BoundaryCases(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetPosition_Listening" );
            CHECKEXECUTE(Test_Buf_SetPosition_Listening(eTestApply, vRelative, szFullPath));

         	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetVelocity" );
            CHECKEXECUTE(Test_Buf_SetVelocity(eTestApply, vRelative, szFullPath));

            // Listener Tests
            // --------------  
			
            SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetAllParameters" );
            CHECKEXECUTE(Test_Lis_SetAllParameters(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetDistanceFactor" );
            CHECKEXECUTE(Test_Lis_SetDistanceFactor(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetDopplerFactor" );
            CHECKEXECUTE(Test_Lis_SetDopplerFactor(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetOrientation_TopVector" );
            CHECKEXECUTE(Test_Lis_SetOrientation_TopVector(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetOrientation_FrontVector" );
            CHECKEXECUTE(Test_Lis_SetOrientation_FrontVector(eTestApply, vRelative, szFullPath));

         	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetPosition_BoundaryCases" );
            CHECKEXECUTE(Test_Lis_SetPosition_BoundaryCases(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetPosition_Listening" );
            CHECKEXECUTE(Test_Lis_SetPosition_Listening(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetRolloffFactor" );
            CHECKEXECUTE(Test_Lis_SetRolloffFactor(eTestApply, vRelative, szFullPath));

        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_SetVelocity" );
            CHECKEXECUTE(Test_Lis_SetVelocity(eTestApply, vRelative, szFullPath));

        	//No need to test this twice since it doesn't depend on eTestApply.  Doesn't use the wrapper
            //  class so no need to specify vRelative either.
           if (TESTAPPLY_IMMEDIATE == eTestApply)
            {
                SETLOG( LogHandle, "danrose", "DSOUND", "3D Listener", "Test_Lis_CommitDeferredSettings" );
                CHECKEXECUTE(Test_Lis_CommitDeferredSettings(szFullPath));
            }

            //Other tests
        	SETLOG( LogHandle, "danrose", "DSOUND", "3D Buffer", "Test_Buf_SetVolume" );
            CHECKEXECUTE(Test_Buf_SetVolume(eTestApply, vRelative, szFullPath));
			
        }

    }
}


/********************************************************************************
Placeholder fuunction - does nothing.
********************************************************************************/
VOID WINAPI DS3DEndTest( VOID )
{
}


/********************************************************************************
Export function pointers of StartTest and EndTest
********************************************************************************/
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( ds3D )
#pragma data_seg()

BEGIN_EXPORT_TABLE( ds3D )
    EXPORT_TABLE_ENTRY( "StartTest", DS3DStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DS3DEndTest )
END_EXPORT_TABLE( ds3D )



   