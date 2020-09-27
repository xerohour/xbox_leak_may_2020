/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/
 
/*******************************************************************************
*
* Filename:       vLibDef.h
*
* Purpose:        Machine in-dependent definitions for Research Library
*
* Author/Date:    Wei Wang, 2/23/98
*
********************************************************************************
*
* Modifications:
*
* Comments: 
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vLibDef.h_v   1.3   03 Mar 1998 14:37:06   bobd  $
*******************************************************************************/
#ifndef _VLIBDEF_H_
#define _VLIBDEF_H_

#ifdef PI
#undef PI
#endif
#define PI                3.141592653589793F

#ifdef DB_PI
#undef DB_PI
#endif
#define DB_PI             6.283185307179586F

#ifdef PI_2
#undef PI_2
#endif
#define PI_2              1.570796326794897F

#ifdef VERY_LARGE_FLOAT
#undef VERY_LARGE_FLOAT
#endif
#define VERY_LARGE_FLOAT  1.0e38F

#ifdef VERY_SMALL_FLOAT
#undef VERY_SMALL_FLOAT
#endif
#define VERY_SMALL_FLOAT  1.0e-37F

#ifdef MAX_LPC_ORDER
#undef MAX_LPC_ORDER
#endif
#define MAX_LPC_ORDER          18

#endif
