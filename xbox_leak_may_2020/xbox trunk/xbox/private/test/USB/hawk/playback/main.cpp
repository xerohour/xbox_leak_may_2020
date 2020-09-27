#include "playback.h"

//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------

void __cdecl main()
{
    //Initialize core peripheral port support
    XDEVICE_PREALLOC_TYPE deviceTypes[] = 
    {
        {XDEVICE_TYPE_VOICE_MICROPHONE,4},
        {XDEVICE_TYPE_VOICE_HEADPHONE,4}
    };
    XInitDevices(sizeof(deviceTypes)/sizeof(XDEVICE_PREALLOC_TYPE),deviceTypes);

    CHawkUnits *pHawkUnits;
    CDraw draw;
    pHawkUnits = new CHawkUnits;
    pHawkUnits->Run(draw);
}
