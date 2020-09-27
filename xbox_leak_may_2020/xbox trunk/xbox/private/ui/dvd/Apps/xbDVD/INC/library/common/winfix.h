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


// FILE:      library\common\winfix.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1997 Viona Development.  All Rights Reserved.
// CREATED:   29.07.97
//
// PURPOSE: Fixes Windows problems.
//
// HISTORY:

#ifndef WINFIX_H
#define WINFIX_H

#include <windows.h>



// Fixes Rectangle() problem of not working with width or height less than 3.

void FixedRectangle (HDC hdc, int left, int top, int right, int bottom);



#endif
