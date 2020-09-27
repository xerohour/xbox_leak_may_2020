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

/*--
Copyright (c) Microsoft Corporation

Module Name:

    av.h

Abstract:

    This module contains the public data structures and procedure
    prototypes to identify AV packs and program TV encoder

--*/

#ifndef _AV_H
#define _AV_H

#ifdef __cplusplus
extern "C" {
#endif


//
// TV encoder options
//
#define AV_OPTION_MACROVISION_MODE			1
#define AV_OPTION_ENABLE_CC					2
#define AV_OPTION_DISABLE_CC				3
#define AV_OPTION_SEND_CC_DATA				4
#define AV_OPTION_CC_STATUS					5
#define AV_OPTION_AV_CAPABILITIES			6
#define AV_OPTION_PACK_REMOVED				7
#define AV_OPTION_NEW_PACK					8
#define AV_OPTION_BLANK_SCREEN				9
#define AV_OPTION_MACROVISION_COMMIT        10
#define AV_OPTION_FLICKER_FILTER			11
#define AV_OPTION_CGMS                      18
#define AV_OPTION_WIDESCREEN                19

//
// CGMS modes
//

#define AV_CGMS_UNRESTRICTED                0
#define AV_CGMS_ONECOPY                     1
#define AV_CGMS_NOCOPIES                    3


//
// Macrovision modes
//
#define TV_MV_OFF                         0
#define TV_MV_AGC_ONLY                    1
#define TV_MV_TWO_STRIPES_PLUS_AGC        2
#define TV_MV_FOUR_STRIPES_PLUS_AGC       3


typedef LONG NTSTATUS;


NTSTATUS WINAPI
AvSendTVEncoderOption(
    IN  PVOID RegisterBase,
    IN  ULONG Option,
    IN  ULONG Param,
    OUT PULONG Result
    );

//
// Example:
//
// AvSendTVEncoderOption(0, AV_OPTION_MACROVISION_MODE, TV_MV_OFF, &Result);
//


#ifdef __cplusplus
}
#endif

#endif // _AV_H
