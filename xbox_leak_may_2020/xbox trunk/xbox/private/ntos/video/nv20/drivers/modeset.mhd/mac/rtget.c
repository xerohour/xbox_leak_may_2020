/*
    FILE:   rtget.c
    DATE:   4/8/99

    This file holds code to get the restrction and timing list.
    If there isn't one -- just return the blank list (not a NULL
    ptr.)
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "utils.h"
#include "restime.h"
#include "debug.h"

// Just in cae we can't GlobalAlloc a list, we have a single entry one
// we can use here.
RESTIME     rtList;
LPRESTIME   lpResTimeList = NULL;

LPRESTIME CFUNC GetMasterRTList(LPDISPDATA lpDispData);
int CFUNC FreeMasterRTList(LPRESTIME lpRTList);
/*
 * SetResTimeList
 */
void SetResTimeList(void *lpRtList)
{
    lpResTimeList = (LPRESTIME)lpRtList;
    return;
}
/*
    GetMasterRTList

    This routine extracts the Restriction and Timing list for the
    device requested from the registry. nRTListLen is the length
    in RESTIME structures of the array pointed to by lpRTList.
*/
LPRESTIME CFUNC GetMasterRTList
(
    LPDISPDATA lpDispData
)
{
    LPRESTIME   lpRTList;

    if (lpResTimeList)
        lpRTList = lpResTimeList;
    else
    {
        // Here is returning a blank list.
        lpRTList = &rtList;
        lpRTList[0].dwXRes = 0;
    }
    return  (lpRTList);
}
/*
    FreeMasterRTList

    This routine is called when the caller is done with the RTList.
*/
int CFUNC FreeMasterRTList
(
    LPRESTIME lpRTList
)
{
/*
 * CHASCHAS - How will we do this on Mac?
 */

    return  (TRUE);
}

