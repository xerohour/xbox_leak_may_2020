////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
//  $Workfile: SWCMCFG.h $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/library/support/AVDataBase/SWCMCFG.h $
// $Author: Fhermanson $
// $Modtime: 12/21/00 11:58a $
// $Date: 2/01/01 2:10p $
// $Revision: 2 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////

#ifndef __SWCMCFG_H
#define __SWCMCFG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SWCMCFG_EXPORTS
#define SWCMCFG_API __declspec(dllexport)
#else
#define SWCMCFG_API __declspec(dllimport)
#endif

typedef int HCFGDB;

///////////////////////////////////////////////////////////////////////////////
//
// SWCMCFG_Initialize()
//
// Description:  This function sets the Software CineMaster configuration
//               data according to the following system information:
//
//                   Display Devices (VGA Card(s))
//                   Sound Devices
//                   CPU Type
//                   CPU Speed
//
//               This must be called before using any of the configuration
//               access functions.
//               It may be called more than once (e.g. on device change)
//
// Arguments:    void -
//
// Return:       SWCMCFG_API int  - Error value
//                          0 = Failed
//                          1 = Success
//
///////////////////////////////////////////////////////////////////////////////
SWCMCFG_API int WINAPI SWCMCFG_Initialize(void);

///////////////////////////////////////////////////////////////////////////////
//
// SWCMCFG_OpenDB()
//
// Description:  Returns a handle to a specific database for using GetInt
//
// Arguments:    LPCTSTR lpszDBName - Name of database
//                      (e.g. "Software Cinemaster\\2.0")
//
// Return:       SWCMCFG_API HCFGDB - >0 if successfull
//
///////////////////////////////////////////////////////////////////////////////
SWCMCFG_API HCFGDB WINAPI SWCMCFG_OpenDB(LPCTSTR lpszDBName);

///////////////////////////////////////////////////////////////////////////////
//
// int SWCMCFG_CloseDB()
//
// Arguments:    HCFGDB hDB - Handle to open DB
//
// Return:       SWCMCFG_API int - Error value
//                          0 = Failed
//                          1 = Success
//
///////////////////////////////////////////////////////////////////////////////
SWCMCFG_API int WINAPI SWCMCFG_CloseDB(HCFGDB hDB);

///////////////////////////////////////////////////////////////////////////////
//
// SWCMCFG_GetInt()
//
//
// Description:  Gets the value of the specified Software CineMaster
//               configuration item.
//
// Arguments:    HCFGDB  hDB - handle returned from SWCMCFG_OpenDB().
//               LPCTSTR lpszCfgItemName - Name of config item
//                                         (e.g. "VideoDecoder.PerformanceClass")
//               int nDefValue - this value is returned if no other value is found
//               int nDisplayDev - If this configuration item is
//                      related to the Display Device (VGA card), then this
//                      value is used to select either a default value or
//                      a value from the VGA card database for the specified
//                      VGA device.
//                         0 - This specifies to use the default value (as if
//                             the VGA card was NOT known.
//                         >= 1 - This specifies which VGA card (if multiple)
//                            to retrieve the value for (this is the DDRAW
//                            DevEnum + 1).
//                            If the vga card is not recognized, a default
//                            value will be returned.
//                      If the configuration item is NOT display (VGA)
//                      dependent, then this parameter has no effect.
//               int nSoundDev - If this configuration item is
//                      related to the Sound Card, then this
//                      value is used to select either a default value or
//                      a value from a Sound Card database for the specified
//                      Sound device.
//                         0 - This specifies to use the default value (as if
//                             the Sound card was NOT known.
//                         >= 1 - This specifies which Sound card (if multiple)
//                            to retrieve the value for (this is the DSOUND
//                            DevEnum + 1).
//                            If the sound card is not recognized, a default
//                            value will be returned.
//                      If the configuration item is NOT Sound card
//                      dependent, then this parameter has no effect.
//               int& nValue - This is the returned value if successful.
//
// Return:       SWCMCFG_API int  - Error value
//                          0 = Failed
//                          1 = Success
//
///////////////////////////////////////////////////////////////////////////////
SWCMCFG_API  int WINAPI SWCMCFG_GetInt(HCFGDB hDB, LPCTSTR lpszCfgItemName,
                                int nDefValue, int nDisplayDev, int nSoundDev,
                                int& nValue);

#ifdef __cplusplus
}   // extern "C" {
#endif

#endif // __SWCMCFG_H
