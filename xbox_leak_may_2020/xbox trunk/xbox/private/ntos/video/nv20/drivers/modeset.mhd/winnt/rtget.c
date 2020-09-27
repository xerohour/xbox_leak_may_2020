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

// Just in case we can't GlobalAlloc a list, we have a single entry one
// we can use here.
RESTIME     sRTList;

/*
    GetMasterRTList

    This routine returns a ptr to a an array of RESTIME
    structures which describe the list of R&T strings for
    the board specified. This routine needs to ALWAYS return
    some RT list. If you have to just return a blank list.

    You can alloc memory for the mode list because you will be
    given a chance to free the ptr later.
*/
LPRESTIME CFUNC
GetMasterRTList (LPDISPDATA lpBoardData)
{
    LPRESTIME   lpRTList;

    // Here is returning a blank list.
    lpRTList = &sRTList;
    lpRTList[0].dwXRes = 0;
    return  (lpRTList);
}


/*
    FreeMasterRTList

    This routine is called when the caller is done with the RTList.
*/
int CFUNC
FreeMasterRTList (LPRESTIME lpRTList)
{
    return  (TRUE);
}

