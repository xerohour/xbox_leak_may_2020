#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "restime.h"
#include "osapi.h"

#include "xf86_ansic.h" // need the declaration of xf86memcpy()	

/*****************************************************************************/
/*

  osapi.c - this source file contains the linux implementation of the following
  functions:

  GetPitchAndSize ()
  SetMonitorLimitMode ()
  GetMonitorLimitMode ()
  SetMonitorInfRangeLimits ()
  GetMonitorInfRangeLimits ()
  SetMasterModeList ()
  GetMasterModeList ()
  FreeMasterModeList ()
  SetMasterRTList ()
  GetMasterRTList ()
  FreeMasterRTList ()  
  GetTimingStandardOverride ()
  GetAllowAllModesFlag ()


*/
/*****************************************************************************/




typedef ULONG   (*LPFNGETMODESIZE) (PVOID, PVOID, ULONG, ULONG,
                                    ULONG, ULONG *, ULONG *);


  
/* List of safe modes; these are the defaults, in case a good master mode
   list isn't set with a call to SetMasterModeList () */

MODEENTRY defaultModeEntries[] = 
{
	{640,   480,  0, 60},
	{640,   480,  0, 72},
	{640,   480,  0, 75},
	{640,   480,  0, 85},
	{800,   600,  0, 56},
	{800,   600,  0, 60},
	{800,   600,  0, 72},
	{800,   600,  0, 75},
	{800,   600,  0, 85},
	{1024,  768,  0, 60},
	{1024,  768,  0, 70},
	{1024,  768,  0, 75},
	{1024,  768,  0, 85},
	{1152,  864,  0, 75},
	{1280, 1024,  0, 60},
	{1280, 1024,  0, 75},
	{1280, 1024,  0, 85},
	{1600, 1024,  0, 60},
	{1600, 1024,  0, 65},
	{1600, 1024,  0, 70},
	{1600, 1024,  0, 75},
	{1600, 1024,  0, 85},
	{1600, 1200,  0, 60},
	{1600, 1200,  0, 65},
	{1600, 1200,  0, 70},
	{1600, 1200,  0, 75},
	{1600, 1200,  0, 85},
	{1792, 1344,  0, 60},
	{1792, 1344,  0, 75},
	{1856, 1392,  0, 60},
	{1856, 1392,  0, 75},
	{1920, 1440,  0, 60},
	{1920, 1440,  0, 75},
	{   0,    0,  0,  0}
};

RESTIME defaultRTList [] =
{
    { 0, 0, 0, 0,
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
    }
};


/* data that is global to these routines */

MONITORLIMITS *lpMonitorRangeLimits[8] = {NULL, NULL, NULL, NULL,
                                          NULL, NULL, NULL, NULL};

MODEINFO *lpMonitorLimitMode[8] = {NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, NULL};

MODEENTRY *lpMasterModeList[8] = {NULL, NULL, NULL, NULL,
                                  NULL, NULL, NULL, NULL};

RESTIME *lpResTimeList[8] = {NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL};









/*****************************************************************************/
/* GetPitchAndSize () - This routine returns the pitch for a given mode;
   this implementation assumes that the function pointer
   pDispData->lpfnGetModeSize has been set by the caller of FindModeEntry.

   Arguments:  lpDispData  - identifies the specific board that we
                             want the active display port for.
               dwXRes      - The X res in pixels for the mode
                             should be returned.
               dwBpp       - the bits per pixel for the mode

   Returns:    The pitch in bytes */
/*****************************************************************************/

ULONG GetPitchAndSize (LPDISPDATA lpDispData, ULONG dwXRes, ULONG dwYRes,
                       ULONG dwBpp, ULONG *dwPitch, ULONG *dwModeSize)
{
    LPFNGETMODESIZE lpfnGetModeSize;
    
    lpfnGetModeSize = (LPFNGETMODESIZE) lpDispData->lpfnGetModeSize;
    return  ((*lpfnGetModeSize) (lpDispData->dwContext1,
                                 lpDispData->dwContext2,
                                 dwXRes, dwYRes, dwBpp, dwPitch, dwModeSize));
} // GetPitchAndSize




/*****************************************************************************/
/* SetMonitorLimitMode ()
   GetMonitorLimitMode ()

   This pair of functions provides a means for the caller of modeset code to
   define a mode which should be used as an upper bound for a specific
   display device (aka. physical head).  In general, the user should only
   need to call SetMonitorLimitMode, and GetMonitorLimitMode will be used
   internally by modeset.

   Note that GetMonitorLimitMode is currently only called if both EDIDs and
   inf fail to define maximum mode values. */
/*****************************************************************************/

void SetMonitorLimitMode (void *lpModeLimit, int display_device)
{
    lpMonitorLimitMode[display_device] = (LPMODEINFO)lpModeLimit;
}


int GetMonitorLimitMode (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo)
{
    int display_device = lpDispData->dwDeviceID;
    
    if (lpMonitorLimitMode[display_device]) {
        xf86memcpy (lpModeInfo, lpMonitorLimitMode[display_device],
                sizeof (MODEINFO));
        return (TRUE);
    }
    return (FALSE);
}



/*****************************************************************************/
/* SetMonitorInfRangeLimits ()
   GetMonitorInfRangeLimits ()

   This pair of functions provides a means for the caller of modeset code to
   define range limits for a specific display device (aka. physical head).
   In general, the user should only need to call SetMonitorInfRangeLimits,
   and GetMonitorInfRangeLimits will be called internally by modeset.

   Note that the implementation of GetMonitorInfRangeLimits requires both
   the lpMonitorRangeLimits AND lpMonitorLimitMode arrays to be defined
   for the given display device (the user should be calling both
   SetMonitorInfRangeLimits and SetMonitorLimitMode).

   Note also that these range limits are only used if no EDIDs are available
   (see ModeWithinMonitorTimingLimits() in digmode.c) */
/*****************************************************************************/

void SetMonitorInfRangeLimits (void *lpRangeLimits, int display_device)
{
    lpMonitorRangeLimits[display_device] = (LPMONITORLIMITS)lpRangeLimits;
}

int GetMonitorInfRangeLimits (LPDISPDATA lpDispData, LPMONITORLIMITS lpML,
                              ULONG *lpdwXRes, ULONG *lpdwYRes)
{
    int display_device = lpDispData->dwDeviceID;

    if ((lpMonitorRangeLimits[display_device]) &&
        (lpMonitorLimitMode[display_device])) {
        xf86memcpy (lpML, lpMonitorRangeLimits[display_device],
                sizeof (MONITORLIMITS));
        
        *lpdwXRes = lpMonitorLimitMode[display_device]->dwXRes;
        *lpdwYRes = lpMonitorLimitMode[display_device]->dwYRes;
        
        return (TRUE);
    }
    return FALSE;
}



/*****************************************************************************/
/* SetMasterModeList ()
   GetMasterModeList ()
   FreeMasterModeList ()

   This set of functions provides a mechanism for the user to define the master
   list of modes for a specific display device.

   SetMasterModeList should be called to set the modelist for a given display
   device; GetMasterModeList is called internally by modeset to retrieve the
   list set by the user (a default is supplied because GetMasterModeList is
   not allowed to fail); and finally, FreeMasterModeList is called internally
   by modeset to free any memory allocated by the implementation of the
   {Set,Get}MasterModeList routines. */
/*****************************************************************************/

void SetMasterModeList (void *lpModeList, int display_device)
{
    lpMasterModeList[display_device] = (LPMODEENTRY)lpModeList;
}

LPMODEENTRY GetMasterModeList (LPDISPDATA lpDispData)
{
    int display_device = lpDispData->dwDeviceID;

    if (lpMasterModeList[display_device])
        return (lpMasterModeList[display_device]);

    return (defaultModeEntries);
}

int FreeMasterModeList (LPMODEENTRY lpModeList)
{
    return (TRUE);
}







/*****************************************************************************/
/* SetMasterRTList ()
   GetMasterRTList ()
   FreeMasterRTList ()

   This set of functions provides an interface for the user of modeset to
   define a restriction and timing list for a specific head.  

   SetMasterRTList should be called to set the RT list for a given display
   device; GetMasterRTList is called internally by modeset to retrieve the
   list set by the user (a default empty list is supplied because
   GetMasterRTList is not allowed to fail); and finally, FreeMasterRTList is
   called internally by modeset to free any memory allocated by the
   implementation of the {Set,Get}MasterRTList routines. */
/*****************************************************************************/

void SetMasterRTList(void *lpRtList, int display_device)
{
    lpResTimeList[display_device] = (LPRESTIME)lpRtList;
}

LPRESTIME GetMasterRTList (LPDISPDATA lpDispData)
{
    int display_device = lpDispData->dwDeviceID;
    
    if (lpResTimeList[display_device])
        return (lpResTimeList[display_device]);
    
    // Here is returning a blank list.
    return (defaultRTList);
}

int FreeMasterRTList (LPRESTIME lpRTList)
{
    return  (TRUE);
}



/*****************************************************************************/
/* GetTimingStandardOverride () - This routine gets any timing standard
   override for the board specified. It should return one of the constants
   defined in restime.h like DMTV_ID. A timing standard override is a timing
   standard that applies to all modes. Just something that the user might want
   to enforce.

   This is currently not implemented on linux. */
/*****************************************************************************/

ULONG GetTimingStandardOverride (LPDISPDATA lpDispData)
{
    /* what should we do here? */
    
    return (NONE_ID);
}


/*****************************************************************************/
/* GetAllowAllModesFlag () - not needed on linux */
/*****************************************************************************/

ULONG GetAllowAllModesFlag (LPREGDATA lpRegData)
{
    return 0;
}
