/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: sparta.h                                                                    *
* Description: global definitions applicable to the sparta dll                          *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      1/24/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_SPARTA_H__
#define __SPARTA_SPARTA_H__

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <tchar.h>
#include "ownwinsock.h" // we define our own winsock so that we don't have to link or load
                        // the winsock dll or driver !

#include "dbgmsg.h"

/* SPARTA STATUS CODES -- based on WINERROR.H codes */

typedef ULONG SPARTA_STATUS;
#define DWORD_ERROR_VALUE 0xFFFFFFFF

#define ANYSIZE 1

#endif // __SPARTA_SPARTA_H__
