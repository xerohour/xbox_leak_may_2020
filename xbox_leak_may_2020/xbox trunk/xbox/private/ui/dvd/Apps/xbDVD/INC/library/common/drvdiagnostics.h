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

// FILE:      software\library\common\drvdiagnostics.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 Viona Development GmbH. All Rights Reserved.
// CREATED:   11.05.1999
//
// PURPOSE:   useful routines for debugging kernel mode drivers
//
// HISTORY:
#ifndef _DRVDIAGNOSTICS_H
#define _DRVDIAGNOSTICS_H

#if DBG!=0
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

#ifdef _DEBUG

// A VERIFY macro.
#define VERIFY(x) ASSERT(x)

// The C/C++ ASSERTIRQL define.  This checks that the IRQL is equal to
//  that specified.  If it is not, then the assert fires.
#define ASSERTIRQL(x)                                       \
            ASSERTMSG ( "KeGetCurrentIrql ( ) == "#x ,      \
                         KeGetCurrentIrql ( ) == x    ) ;

// For consistency....
#define ASSERTIRQL_EQ(x)    ASSERTIRQL ( x )

// Is the IRQL less than x?
#define ASSERTIRQL_LT(x)                                    \
            ASSERTMSG ( "KeGetCurrentIrql ( ) < "#x ,       \
                         KeGetCurrentIrql ( )< x     ) ;

// Is the IRQL less than or equal to x?
#define ASSERTIRQL_LE(x)                                    \
            ASSERTMSG ( "KeGetCurrentIrql ( ) <= "#x ,      \
                         KeGetCurrentIrql ( ) <= x    ) ;

// Is the IRQL greater than or equal to x?
#define ASSERTIRQL_GE(x)                                    \
            ASSERTMSG ( "KeGetCurrentIrql ( ) >= "#x ,      \
                         KeGetCurrentIrql ( ) >= x    ) ;

// Is the IRQL greater than x?
#define ASSERTIRQL_GT(x)                                    \
            ASSERTMSG ( "KeGetCurrentIrql ( ) > "#x ,       \
                         KeGetCurrentIrql ( ) > x    ) ;

// Is the IRQL not equal to x?
#define ASSERTIRQL_NE(x)                                    \
            ASSERTMSG ( "KeGetCurrentIrql ( ) != "#x ,      \
                         KeGetCurrentIrql ( ) != x    ) ;

#define guidPrint(guid) DP("GUID: %x %x %x %x%x%x%x%x%x%x%x", guid.Data1, guid.Data2, guid.Data3, (int)guid.Data4[0], \
																													(int)guid.Data4[1], \
																													(int)guid.Data4[2], \
																													(int)guid.Data4[3], \
																													(int)guid.Data4[4], \
																													(int)guid.Data4[5], \
																													(int)guid.Data4[6], \
																													(int)guid.Data4[7]);

#else

#define ASSERTIRQL(x)
#define ASSERTIRQL_EQ(x)
#define ASSERTIRQL_LT(x)
#define ASSERTIRQL_LE(x)
#define ASSERTIRQL_GE(x)
#define ASSERTIRQL_GT(x)
#define ASSERTIRQL_NE(x)

#define guidPrint(guid)

#define VERIFY(x) x


#endif      // _DEBUG


#endif      // _DRVDIAGNOSTICS_H




