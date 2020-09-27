/*

deviceSrv.h

*/

#ifndef _DEVICESRV_H_
#define _DEVICESRV_H_

#define CLEARFLAG(var, flag)                (var &= ~(flag))
#define SETFLAG(var, flag)                  (var |=  (flag))
#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE*KILOBYTE)

#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>

int SockPrint(SOCKET sock, char* format, ...);
void DebugPrint(char* format, ...);
bool stristr(char *string, char *substr, size_t len=0);


#endif // _DEVICESRV_H_