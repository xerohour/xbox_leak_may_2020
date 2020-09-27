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

#ifndef YUVFRAMEDEBUG_H
#define YUVFRAMEDEBUG_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"

void __cdecl YUVD_WriteString(BYTE * yuvp, DWORD bpr, int x, int y, char * format, ...);

void __cdecl YUVD_WriteString2(BYTE * yuvp, DWORD bpr, int x, int y, char * format, ...);

void YUVD_DrawChart(BYTE * yuvp, DWORD bpr, int x, int y, int w, int h, int l, int * data);

void YUVD_DrawChart2(BYTE * yuvp, DWORD bpr, int x, int y, int w, int h, int l, int * data1, int * data2);

void YUVD_DrawFlow(BYTE * yuvp, DWORD bpr, int x, int y, int w, int h, int l);

void YUVD_SavePicture(BYTE * yupb, DWORD bpr, int width, int height, char * fname, int no);

#endif
