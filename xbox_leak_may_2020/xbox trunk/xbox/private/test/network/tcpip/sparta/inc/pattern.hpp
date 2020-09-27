/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: Pattern.hpp                                                                 *
* Description: this defines the Pattern class used in the driver API                    *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      3/30/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_PATTERN_H__
#define __SPARTA_PATTERN_H__

#include "sparta.h"
#include "driverapi.h"


class CPattern
{
    friend class CInterface; // we want access to the internals

protected:
    NI_PATTERN m_Pattern;

    CPattern( const CPattern& )    //  Private copy constructor.
    {
        DEBUGMSG(DBG_SPARTA_DRIVERAPI,
            (TEXT("**DON'T LIKE COPY CONSTRUCTOR ****** CPattern::CPattern[]\n")
            ));

    }

public:
    CPattern(DWORD dwOffset, DWORD dwLength, PVOID pvBuffer);
    CPattern(MAC_MEDIA_TYPE MediaType, DWORD dwOffset, DWORD dwLength, PVOID pvBuffer);
    // this constructor will take the offset to be relative to the MediaHeader
    // i.e. offset 0 = first byte after the media header

    ~CPattern();
};


#endif // __SPARTA_PATTERN_H__

