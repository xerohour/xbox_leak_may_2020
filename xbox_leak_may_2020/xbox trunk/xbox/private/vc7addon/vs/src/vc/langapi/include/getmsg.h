// getmsg.h

#ifndef _VC_VER_INC
#include "..\include\vcver.h"
#endif

#ifndef __GETMSG_H__
#define __GETMSG_H__

#ifdef  __cplusplus
extern "C" {
#endif

char *get_err(int);
const wchar_t *get_errW(int);
long SetHInstace(long hInstModule);

#ifdef  __cplusplus
}
#endif

#endif __GETMSG_H__
