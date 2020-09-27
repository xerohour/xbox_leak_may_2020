/********************************************************************************
FILE: DS3DSTRESS.CPP

See DS3D.CPP for details.

TODO:
    *Make the CONSOLE macro actually print to the screen (need mattbron's help
     for this)
********************************************************************************/

#include "globals.h"
#include "sound3d.h"
#include "testbuf.h"
#include "testlis.h"
#include "util.h"


/**********************************************************************
**********************************************************************/
void GetRandomD3DVECTOR(LPD3DVECTOR lpV)
{
    lpV->x = rand() / 1000.f;
    lpV->y = rand() / 1000.f;
    lpV->z = rand() / 1000.f;
    CONSOLE("Using relative vector %g, %g, %g", lpV->x, lpV->y, lpV->z);
}



/********************************************************************************
Main code segment for the 3D Sound tests.
********************************************************************************/
VOID WINAPI DS3DStressStartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;
	SETLOG( LogHandle, "danhaff", "DSOUND", "3D", "Stress" );
    D3DVECTOR vRelative;
    TestApplies eTestApply;

	DirectSoundUseFullHRTF();

    //Get the random vector which will be used to move both the listener and the buffer.
    GetRandomD3DVECTOR(&vRelative);

    while (true)
    {
        eTestApply = TestApplies(TESTAPPLY_DEFERRED_NOUPDATE + rand() % 3);

        // Buffer Tests
        // ------------
        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetAllParameters" );
        CHECKEXECUTE(Test_Buf_SetAllParameters(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetConeAngles" );
        CHECKEXECUTE(Test_Buf_SetConeAngles(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetConeOrientation_BoundaryCases" );
        CHECKEXECUTE(Test_Buf_SetConeOrientation_BoundaryCases(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetConeOrientation_Listening" );
        CHECKEXECUTE(Test_Buf_SetConeOrientation_Listening(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetConeOutsideVolume" );
        CHECKEXECUTE(Test_Buf_SetConeOutsideVolume(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetMaxDistance_BoundaryCases" );
        CHECKEXECUTE(Test_Buf_SetMaxDistance_BoundaryCases(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetMaxDistance_InsideSource" );
        CHECKEXECUTE(Test_Buf_SetMaxDistance_InsideSource(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetMinDistance_BoundaryCases" );
        CHECKEXECUTE(Test_Buf_SetMaxDistance_BoundaryCases(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetMaxDistance_Listening" );
        CHECKEXECUTE(Test_Buf_SetMinDistance_Listening(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetMinDistance_Listening" );
        CHECKEXECUTE(Test_Buf_SetMinDistance_Listening(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetMode" );
        CHECKEXECUTE(Test_Buf_SetMode(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetPosition_BoundaryCases" );
        CHECKEXECUTE(Test_Buf_SetPosition_BoundaryCases(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetPosition_Listening" );
        CHECKEXECUTE(Test_Buf_SetPosition_Listening(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Buffer", "Test_Buf_SetVelocity" );
        CHECKEXECUTE(Test_Buf_SetVelocity(eTestApply, vRelative, NULL));

        // Listener Tests
        // --------------
        
        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetAllParameters" );
        CHECKEXECUTE(Test_Lis_SetAllParameters(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetDistanceFactor" );
        CHECKEXECUTE(Test_Lis_SetDistanceFactor(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetDopplerFactor" );
        CHECKEXECUTE(Test_Lis_SetDopplerFactor(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetOrientation_TopVector" );
        CHECKEXECUTE(Test_Lis_SetOrientation_TopVector(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetOrientation_FrontVector" );
        CHECKEXECUTE(Test_Lis_SetOrientation_FrontVector(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetPosition_BoundaryCases" );
        CHECKEXECUTE(Test_Lis_SetPosition_BoundaryCases(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetPosition_Listening" );
        CHECKEXECUTE(Test_Lis_SetPosition_Listening(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetRolloffFactor" );
        CHECKEXECUTE(Test_Lis_SetRolloffFactor(eTestApply, vRelative, NULL));

        SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_SetVelocity" );
        CHECKEXECUTE(Test_Lis_SetVelocity(eTestApply, vRelative, NULL));

        //No need to test this twice since it doesn't depend on eTestApply.  Doesn't use the wrapper
        //  class so no need to specify vRelative either.
        if (TESTAPPLY_IMMEDIATE == eTestApply)
        {
            SETLOG( LogHandle, "danhaff", "DSOUND", "3D Listener", "Test_Lis_CommitDeferredSettings" );
            CHECKEXECUTE(Test_Lis_CommitDeferredSettings(NULL));
        }
    }

}


/********************************************************************************
Placeholder fuunction - does nothing.
********************************************************************************/
VOID WINAPI DS3DStressEndTest( VOID )
{
}


/********************************************************************************
Export function pointers of StartTest and EndTest
********************************************************************************/
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( ds3DStrs )
#pragma data_seg()

BEGIN_EXPORT_TABLE( ds3DStrs )
    EXPORT_TABLE_ENTRY( "StartTest", DS3DStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DS3DStressEndTest )
END_EXPORT_TABLE( ds3DStrs )



   