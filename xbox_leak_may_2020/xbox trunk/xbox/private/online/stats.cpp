/*++

Copyright (c) Microsoft Corporation.  All rights reserved

Description:
    Module implementing Xbox Statistics APIs

Module Name:

    stats.cpp

--*/

#include "xonp.h"
#include "xonver.h"

HRESULT
CXo::XOnlineStatSet(
    IN DWORD dwNumStatsSpecs,
    IN PXONLINE_STAT_SPEC pStatSpecs,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
)
{
    return E_NOTIMPL;
}

HRESULT 
CXo::XOnlineStatGet(
    IN DWORD dwNumStatSpecs,
    IN OUT PXONLINE_STAT_SPEC pStatSpecs,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
)
{
    return E_NOTIMPL;
}

HRESULT 
CXo::XOnlineStatLeaderEnumerate(
	IN XUID* pxuidPagePivot,                           
	IN DWORD dwPageStart,                                              
	IN DWORD dwPageSize,
	IN DWORD dwLeaderboardID,
	IN DWORD dwNumStatsPerUser,
	IN DWORD *pStatsPerUser,
	OUT PXONLINE_STAT_USER pUsers,
	OUT PXONLINE_STAT pStats,
	IN HANDLE hWorkEvent,
	OUT PXONLINETASK_HANDLE phTask
)
{
    return E_NOTIMPL;
}

HRESULT
CXo::XOnlineStatLeaderEnumerateGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT DWORD *pdwReturnedResults
)
{
    return E_NOTIMPL;
}

	
