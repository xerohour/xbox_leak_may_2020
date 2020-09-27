/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVCLEAR.H                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)       07Jul99      created            *
*                                                                           *
\***************************************************************************/

#ifndef _NVCLEAR_H_
#define _NVCLEAR_H_

DWORD __stdcall nvClear  (LPD3DHAL_CLEARDATA);
DWORD __stdcall nvClear2 (LPD3DHAL_CLEAR2DATA pc2d);
void  nvCTDecodeBuffer   (CNvObject *pNvObj,DWORD dwAddr,DWORD dwBPP,
                          DWORD dwPitch,DWORD dwWidth,DWORD dwHeight);

#endif // _NVCLEAR_H_

