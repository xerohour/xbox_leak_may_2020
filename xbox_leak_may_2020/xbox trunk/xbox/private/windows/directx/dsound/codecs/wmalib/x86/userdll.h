/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1999 Intel Corporation. All Rights Reserved.
//
//  Purpose: Declarations of the required functions
//   
*M*/


/***********************************************************/
/* Only copy the function prototype of all the             */
/* functions you want from the release include files       */
/***********************************************************/
#ifdef __USERLIST__
    NSPAPI(void,nspcFft,     (SCplx  *samps, int order, int flags))
    NSPAPI(void,nspsCcsFft,  (float  *samps, int order, int flags))
#endif /* __USERLIST__ */
