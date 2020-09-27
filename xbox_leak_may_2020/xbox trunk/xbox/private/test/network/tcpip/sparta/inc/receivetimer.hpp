/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ReceiveTimer.hpp                                                            *
* Description: This is the implementation of Timers for the packet receive methods      *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      4/7/2000     created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_RECEIVETIMER_H__
#define __SPARTA_RECEIVETIMER_H__

#include "sparta.h"

class CReceiveTimer
{
protected:
    HANDLE m_hWaitableTimer;
public:
    CReceiveTimer();
    ~CReceiveTimer();
    SPARTA_STATUS Start(DWORD dwTimeOut); // timeout in milliseconds
    HANDLE GetTimerObject();
};

#endif // __SPARTA_RECEIVETIMER_H__

