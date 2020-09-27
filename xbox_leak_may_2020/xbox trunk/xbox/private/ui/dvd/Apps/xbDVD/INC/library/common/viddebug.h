////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//
// FILE:      library\common\viddebug.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH. All Rights Reserved.
// CREATED:   28.07.99
//
// PURPOSE:   Debug related definitions for video miniport drivers and display drivers
//
// HISTORY:
//

#ifndef VIDDEBUG_H
#define VIDDEBUG_H

#if DBG!=0
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

#if _DEBUG

#if DISPLAY_DRIVER
#define DP(arg) EngDebugPrint(0, arg, 0)
#elif VIDEO_MINIPORT_DRIVER
#define DP(arg) VideoDebugPrint(arg)
#endif

#else

#define DP(arg)

#endif


#endif //VIDDEBUG_H

