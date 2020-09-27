/*
    FILE:   modelist.c
    DATE:   4/8/99

    This file holds code to get the master mode list.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "utils.h"
//#include "..\..\common\inc\nvreg.h"
#include "restime.h"
#include "osapi.h"

/*
    GetMasterModeList

    This routine returns a ptr to a an array of MODEENTRY
    structures which describe the list of available modes for
    the board specified. This routine needs to ALWAYS return
    some mode list. If you have to just return a list of safe
    modes like 640,480,8,60Hz.

    You can alloc memory for the mode list because you will be
    given a chance to free the ptr later.
*/
LPMODEENTRY CFUNC
GetMasterModeList (LPDISPDATA lpBoardData)
{
    // You can't return NULL -- this is just a template.
    return  (NULL);
}


/*
    FreeMasterModeList

    This routine is called when the caller is done with the mode
    list it got earlier.
*/
int CFUNC
FreeMasterModeList (LPMODEENTRY lpModeList)
{
    return  (TRUE);
}

