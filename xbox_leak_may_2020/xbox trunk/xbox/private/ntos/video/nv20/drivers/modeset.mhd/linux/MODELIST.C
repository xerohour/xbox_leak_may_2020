/*
    FILE:   modelist.c
    DATE:   4/8/99

    This file holds code to get the master mode list.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "restime.h"
#include "osapi.h"

/*
 * List of safe modes...
 */
MODEENTRY   modeEntries[] = 
{
    {640,   480,  8, 60},
    {800,   600,  8, 60},
    {1024,  768,  8, 60},
    {1152,  864,  8, 60},
    {1280, 1024,  8, 60},
    {640,   480, 16, 60},
    {800,   600, 16, 60},
    {1024,  768, 16, 60},
    {1152,  864, 16, 60},
    {1280, 1024, 16, 60},
    {640,   480, 32, 60},
    {800,   600, 32, 60},
    {1024,  768, 32, 60},
    {1152,  864, 32, 60},
    {1280, 1024, 32, 60},
    {1152,  864, 32, 60},
    {1280, 1024, 32, 60},
    {   0,    0,  0,  0}
};
LPMODEENTRY lpMasterModeList = modeEntries;

/*
 * SetMasterModeList
 *
 * The XServer needs to be able to pass in a list of available modes.
 */
void SetMasterModeList(void *lpModeList)
{
    /*
     * Store the pointer to the master mode list.
     */
    lpMasterModeList = (LPMODEENTRY)lpModeList;
}


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
GetMasterModeList (LPBOARDDATA lpBoardData)
{
    // You can't return NULL -- this is just a template.
    return  (lpMasterModeList);
}


/*
    FreeMasterModeList

    This routine is called when the caller is done with the mode
    list it got earlier.
*/
int CFUNC
FreeMasterModeList (LPMODEENTRY lpModeList)
{
    /*
     * Reset the master mode list to the default mode list.
     */
    lpMasterModeList = modeEntries;
    return  (TRUE);
}

