/******************************************************************************************
Used to ease porting tests from Windows.


******************************************************************************************/

#pragma once




#define fnsIncrementIndent LogIncrementIndent
#define fnsDecrementIndent LogDecrementIndent

#define FNS_PASS                        0x00000000
#define FNS_FAIL                        0x00000001
#define FNS_ABORTED                     0x00000002
#define FNS_PENDING                     0x00000003
