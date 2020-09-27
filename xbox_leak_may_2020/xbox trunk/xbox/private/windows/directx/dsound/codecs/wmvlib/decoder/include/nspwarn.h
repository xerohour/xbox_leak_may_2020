/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1994 Intel Corporation. All Rights Reserved.
//
//      $Workfile: nspwarn.h $
//      $Revision: 3 $
//      $Modtime: 3/05/97 19:06 $
//
//   Purpose:
//
*M*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NSPWARN_H__
#define __NSPWARN_H__

#if !defined (_OWN_BLDPCS)


#if _MSC_VER >= 1000

/* nonstandard extension used : nameless struct/union          */
#pragma warning(disable : 4201)
/* nonstandard extension used : bit field types other than int */
#pragma warning(disable : 4214)
/* unreferenced inline function has been removed               */
#pragma warning(disable : 4514)
/* named type definition in parentheses                        */
#pragma warning(disable : 4115)

#endif /* _MSC_VER >= 1000 */
#endif /* (_OWN_BLDPCS)    */


#endif /* __NSPWARN_H__    */

#ifdef __cplusplus
}
#endif
