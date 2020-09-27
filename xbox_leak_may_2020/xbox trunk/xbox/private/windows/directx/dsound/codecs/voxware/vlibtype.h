/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File:          vLibType.h
*
* Purpose:   define variable types
*
* Functions:
*
* Author/Date:
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vLibType.h_v   1.0   07 Apr 1998 17:58:38   weiwang  $
******************************************************************************/
 
#ifndef __VLIBTYPE
#define __VLIBTYPE

#ifdef __cplusplus
extern "C" {
#endif



#if (defined(WIN32) == 1 || defined(SPARCSTATION) == 1 || defined(IEEE) == 1)
typedef long Uint32;
#else
typedef float Uint32;   /* the trick to undo long to float */
#endif

/* union to do quicker comparison among floating numbers */
typedef union {
  float  f;
  Uint32 l;
} UnionFltLng;


#ifdef _cplusplus
}
#endif

#endif
