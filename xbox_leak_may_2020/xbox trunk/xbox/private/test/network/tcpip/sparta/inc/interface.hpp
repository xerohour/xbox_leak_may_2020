/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: Interface.hpp                                                               *
* Description: this defines the Interface class used in the driver API                  *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/21/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_INTERFACE_H__
#define __SPARTA_INTERFACE_H__

#include "sparta.h"
#include "driverapi.h"
#include "Packet.hpp"
#include "Pattern.hpp"
#include "MacPacket.hpp"
#include "ReceiveTimer.hpp"

class CInterfaceList
{
protected:
    TCHAR **m_pszInterfaceNames;
    DWORD m_dwInterfaceCount;

public:
    CInterfaceList();
    ~CInterfaceList();
    DWORD GetCount();
    TCHAR *GetNameAt(DWORD dwIndex);
};


class CInterface
{
protected:
    NI_INTERFACE m_Interface;
    MAC_MEDIA_TYPE m_MediaType;
    CMacAddress *m_pMediaAddress;

    // we share the send and receive buffers for this interface.
    // we could make it asyncronous and do a memory allocation
    // for each send and receive. The SPARTA driver
    // uses asynchronous I/O so all that needs to be changed are the
    // methods in the interface class, as well adding NI_AsyncSend and NI_AsyncReceive
    // in the driver API functions

    CRITICAL_SECTION m_csSendBufferCriticalSection;
    PVOID m_pvSendBuffer;
    DWORD m_dwSendBufferLength;
    DWORD m_dwLastBufferSendLength;

    CRITICAL_SECTION m_csReceiveBufferCriticalSection;
    PVOID m_pvReceiveBuffer;
    DWORD m_dwReceiveBufferLength;
    DWORD m_ReceiveMode;

    CInterface( const CInterface& )    //  Private copy constructor.
    {
        DEBUGMSG(DBG_SPARTA_DRIVERAPI,
            (TEXT("** DON'T LIKE COPY CONSTRUCTOR ****** CInterface::CInterface[]\n")
            ));

    }

public:
    CInterface(const TCHAR *szDescription);
    ~CInterface();

    #if DBG
    // for testing only !!
    NI_INTERFACE GetInterfaceHandle()
    {
        return m_Interface;
    }
    #endif

    MAC_MEDIA_TYPE GetMediaType();

    CMacAddress *GetMediaAddress();

    SPARTA_STATUS Send(CPacket *SendPacket);
    SPARTA_STATUS Send(CPacket &SendPacket);
    SPARTA_STATUS ReSendLastPacket();
    // use ResendLastPacket for higher performance i.e. where identical buffers
    // are sent numerous times. The performance increase is marginal, but this was
    // supplied for completeness sake.

    SPARTA_STATUS SendRawBuffer(PVOID pvBuffer, DWORD dwBufferLength);

    // use SendRawBuffer for performance reasons - the Send command will assemble
    // all packet headers and call the send handlers for each call. If you want
    // to send repeated copies of the same packet, use Packet::BuildRawBuffer and
    // then send it out using SendRawBuffer

    SPARTA_STATUS EnablePromiscuousReceiveMode();
    SPARTA_STATUS EnableBroadcastReceiveMode();
    SPARTA_STATUS EnableDirectedReceiveMode();
    SPARTA_STATUS EnableMulticastReceiveMode();

    SPARTA_STATUS DisablePromiscuousReceiveMode();
    SPARTA_STATUS DisableBroadcastReceiveMode();
    SPARTA_STATUS DisableDirectedReceiveMode();
    SPARTA_STATUS DisableMulticastReceiveMode();

    // the compiler did not like optional parameters for this call, thus the
    // list of variants for this method

    CPacket *Receive();
    CPacket *Receive(int ExpectedPacketType); // throws error if doesn't receive 
                                                                    // expected packet type
    CPacket *Receive(int ExpectedPacketType, HANDLE hEventHandle); // will return timeout if event set
	CPacket *Receive(CReceiveTimer &Timer);
	CPacket *Receive(int ExpectedPacketType, CReceiveTimer &Timer);
    CPacket *Receive(HANDLE hEventHandle); // will return timeout if event set
                                            // this method is not needed in scripts

    SPARTA_STATUS AddMulticastAddress(CMacAddress &McastAddr);
    SPARTA_STATUS RemoveMulticastAddress(CMacAddress &McastAddr);

    SPARTA_STATUS AddPatternFilter(CPattern &Pattern);
    SPARTA_STATUS RemovePatternFilter(CPattern &Pattern);

    SPARTA_STATUS AddPatternFilter(CPattern *pPattern);
    SPARTA_STATUS RemovePatternFilter(CPattern *pPattern);

    SPARTA_STATUS EnablePatternFiltering();
    SPARTA_STATUS DisablePatternFiltering();

    SPARTA_STATUS StartListening();
    SPARTA_STATUS StopListening();

};


#endif // __SPARTA_INTERFACE_H__

