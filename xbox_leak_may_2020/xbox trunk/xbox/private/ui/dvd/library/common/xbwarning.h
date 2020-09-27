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

#pragma warning(3:4092)   // sizeof returns 'unsigned long'
#pragma warning(4:4121)   // structure is sensitive to alignment
#pragma warning(3:4125)   // decimal digit in octal sequence
#pragma warning(3:4130)   // logical operation on address of string constant
#pragma warning(3:4132)   // const object should be initialized
#pragma warning(4:4206)   // Source File is empty
#pragma warning(4:4101)   // Unreferenced local variable
#pragma warning(4:4208)   // delete[exp] - exp evaluated but ignored
#pragma warning(3:4212)   // function declaration used ellipsis
#pragma warning(3:4242)   // convertion possible loss of data
#pragma warning(4:4267)   // convertion from size_t to smaller type
#pragma warning(4:4312)   // conversion to type of greater size
#pragma warning(error:4700)    // Local used w/o being initialized
//#pragma warning(3:4700)    // Local used w/o being initialized
#pragma warning(error:4259)    // pure virtual function was not defined
#pragma warning(error:4071)    // no function prototype given
#pragma warning(error:4072)    // no function prototype given (fastcall)
#pragma warning(error:4171)    // no function prototype given (old style)
#pragma warning(error:4013)    // 'function' undefined - assuming extern returning int
#pragma warning(error:4551)    // Function call missing argument list
#pragma warning(error:4806)    // unsafe operation involving type 'bool'
#pragma warning(4:4509)   // use of SEH with destructor
#pragma warning(4:4177)   // pragma data_seg s/b at global scope
#pragma warning(disable:4274)  // #ident ignored
#pragma warning(disable:4786)  // identifier was truncated to 255 chararcers in debug information.
#pragma warning(disable:4503)  // decorated name length exceeded, name was truncated.
#pragma warning(disable:4263)  // Derived override doesn't match base - who cares...
#pragma warning(disable:4264)  // base function is hidden - again who cares.
#pragma warning(disable:4710)  // Function marked as inline - wasn't
#pragma warning(disable:4917)  // A GUID can only be associated with a class, interface or namespace
#pragma warning(error:4552)    // <<, >> ops used to no effect (probably missing an = sign)
#pragma warning(error:4553)    // == op used w/o effect (probably s/b an = sign)


#if _MSC_VER>=1300
// Fix these new warnings post NT5 RTM.
#pragma warning(4:4532)   // jump out of __finally block
#pragma warning(4:4288)   // nonstandard extension used (loop counter)
#endif

#ifndef __cplusplus
#undef try
#undef except
#undef finally
#undef leave
#define try                         __try
#define except                      __except
#define finally                     __finally
#define leave                       __leave
#endif

