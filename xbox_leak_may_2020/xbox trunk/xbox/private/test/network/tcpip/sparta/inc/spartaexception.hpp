/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: SpartaException.hpp                                                         *
* Description: this defines the Exceptions for this project                             *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/16/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_EXCPETION_H__
#define __SPARTA_EXCEPTION_H__

#include "sparta.h"
#include "packet.hpp"


class CSpartaException
{
protected:
    TCHAR *szExceptionMessage;
    SPARTA_STATUS Status;

public:
    CSpartaException()
    {
        // do nothing
        szExceptionMessage = NULL;
        Status = ERROR_GEN_FAILURE;
    }

    CSpartaException(SPARTA_STATUS status, TCHAR *szMsg)
    {
        szExceptionMessage = szMsg;
        Status = status;
    }

    TCHAR *GetMessage()
    {
        return szExceptionMessage;
    }

    SPARTA_STATUS GetStatus()
    {
        return Status;
    }


};

class CSpartaMallocException : public CSpartaException
{
public:

    CSpartaMallocException() : CSpartaException(ERROR_NOT_ENOUGH_MEMORY,
                                                TEXT("Memory Allocation Failure."))
    {
    }
};


class CSpartaTimeoutException : public CSpartaException
{
public:

    CSpartaTimeoutException() : CSpartaException(ERROR_TIMEOUT,
                                                    TEXT("Timeout period expired."))
    {
    }
};

class CSpartaInValidParameterException : public CSpartaException
{
public:

    CSpartaInValidParameterException() : CSpartaException(ERROR_INVALID_PARAMETER,
                                                    TEXT("The parameter is incorrect. "))
    {
    }
};

class CSpartaUnexpectedApiErrorException : public CSpartaException
{
public:
    CSpartaUnexpectedApiErrorException(SPARTA_STATUS status, TCHAR *szMsg) :
                                        CSpartaException(status,szMsg)
    {
    }
};

class CSpartaPacketParseException : public CSpartaException
{
public:
    CSpartaPacketParseException(SPARTA_STATUS status, TCHAR *szMsg) :
                                        CSpartaException(status,szMsg)
    {
    }
};

class CSpartaPacketReceiveException : public CSpartaException
{
public:

    CSpartaPacketReceiveException(SPARTA_STATUS status) : 
                        CSpartaException(status,TEXT("Error receiving packet."))
    {
    }

    CSpartaPacketReceiveException(SPARTA_STATUS status,
                                    TCHAR *szMsg) : CSpartaException(status,szMsg)
    {
    }

};

class CSpartaUnexpectedPacketReceivedException : public CSpartaPacketReceiveException
{
protected:
    CPacket *m_pPacket;
    BOOL m_fDontDeletePacket;

public:

    CSpartaUnexpectedPacketReceivedException(SPARTA_STATUS status) :
         CSpartaPacketReceiveException(status,TEXT("Unexpected Packet Type Received.")),
        m_pPacket(NULL),m_fDontDeletePacket(FALSE)
    {
    }

    CSpartaUnexpectedPacketReceivedException(SPARTA_STATUS status, CPacket *pPacket) :
         CSpartaPacketReceiveException(status,TEXT("Unexpected Packet Type Received.")),
        m_pPacket(pPacket),m_fDontDeletePacket(FALSE)
    {
    }

    ~CSpartaUnexpectedPacketReceivedException()
    {
        if(m_pPacket != NULL && m_fDontDeletePacket == FALSE)
        {
            delete m_pPacket;
        }
    }

    CPacket *CreatePacketReference()
    {
        m_fDontDeletePacket = TRUE;
        return m_pPacket;
    }

};

// ThrowOnErrorIfEnabled - checks to see whether status != ERROR_SUCCESS. If it is,
// it throws an CSpartaUnexpectedApiErrorException with that error code. 

VOID ThrowOnError(SPARTA_STATUS status, TCHAR *szMsg);

#endif // __SPARTA_EXCEPTION_H__

