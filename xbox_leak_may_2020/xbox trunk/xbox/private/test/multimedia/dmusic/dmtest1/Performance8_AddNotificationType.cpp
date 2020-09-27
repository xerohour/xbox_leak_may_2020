#include "globals.h"
#include "cicmusicx.h"
#include "Performance8_AddNotificationType.hpp"


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_AddNotificationType_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling Valid test function Performance8_AddNotificationType())");  //PASS
    DMTEST_EXECUTE(Template_AddRemoveNotifications<CtIDirectMusicPerformance8>(ptPerf8, 1, 0, 0));      
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_AddNotificationType_NoRestart(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes)
{
    return Template_AddRemoveNotifications<CtIDirectMusicPerformance8>(ptPerf8, dwAddNotificationTypes, dwRemNotificationTypes, 0);
}

/********************************************************************************
********************************************************************************/
HRESULT Performance8_AddNotificationType_Restart(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes)
{
    return Template_AddRemoveNotifications<CtIDirectMusicPerformance8>(ptPerf8, dwAddNotificationTypes, dwRemNotificationTypes, 1);
}


