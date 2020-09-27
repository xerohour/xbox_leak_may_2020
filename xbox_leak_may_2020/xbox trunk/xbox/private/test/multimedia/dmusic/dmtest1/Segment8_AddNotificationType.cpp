#include "globals.h"
#include "cicmusicx.h"
#include "Performance8_AddNotificationType.hpp"

/********************************************************************************
********************************************************************************/
HRESULT Segment8_AddNotificationType_NoRestart(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes)
{
    return Template_AddRemoveNotifications<CtIDirectMusicSegment8>(ptPerf8, dwAddNotificationTypes, dwRemNotificationTypes, 0);
}

/********************************************************************************
********************************************************************************/
HRESULT Segment8_AddNotificationType_Restart(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwAddNotificationTypes, DWORD dwRemNotificationTypes)
{
    return Template_AddRemoveNotifications<CtIDirectMusicSegment8>(ptPerf8, dwAddNotificationTypes, dwRemNotificationTypes, 1);
}
