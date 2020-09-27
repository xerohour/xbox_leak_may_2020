//-----------------------------------------------------------------------------
// File: Common.h
//
// Desc: ContentDownload global header
//
// Hist: 09.07.01 - New for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef CONTENTDOWNLOAD_COMMON_H
#define CONTENTDOWNLOAD_COMMON_H

#include "xtl.h"
#include "xonline.h"
#include <string>
#include <vector>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
enum
{
    // Confirm menu
    CONFIRM_YES = 0,
    CONFIRM_NO,
    CONFIRM_MAX
};




//-----------------------------------------------------------------------------
// Name: class ContentInfo
// Desc: Content information from the online catalog; simplifies access to
//       XONLINECATALOG data.
//-----------------------------------------------------------------------------
class ContentInfo
{
    DWORD                     m_dwPackageSize;
    DWORD                     m_dwInstallSize;
    DWORD                     m_dwRating;
    DWORD                     m_dwOfferingType;
    DWORD                     m_dwBitFlags;   
    FILETIME                  m_ftCreationDate;
    XONLINEOFFERING_ID        m_ID;
    std::basic_string< BYTE > m_Data;       // Title-specific data

public:

    ContentInfo()
    :   
        m_dwPackageSize ( 0 ),
        m_dwInstallSize ( 0 ),
        m_dwRating( 0 ),
        m_dwOfferingType( 0 ),
        m_dwBitFlags( 0 ),
        m_ftCreationDate( FILETIME() ),
        m_ID            ( XONLINEOFFERING_ID(0) ),
        m_Data          ()
    {
    }

    explicit ContentInfo( const XONLINEOFFERING_INFO& xOnInfo )
    :   
        m_dwPackageSize ( xOnInfo.cbPackageSize ),
        m_dwInstallSize ( xOnInfo.cbInstallSize ),
        m_dwOfferingType( xOnInfo.dwOfferingType ),
        m_dwBitFlags    ( xOnInfo.dwBitFlags ),
        m_dwRating      ( xOnInfo.dwRating ),
        m_ftCreationDate( xOnInfo.ftActivationDate ),
        m_ID            ( xOnInfo.OfferingId ),
        m_Data          ( xOnInfo.pbTitleSpecificData,
                          xOnInfo.cbTitleSpecificData )
    {
    }

    DWORD GetPackageSize() const     { return m_dwPackageSize; }
    DWORD GetInstallSize() const     { return m_dwInstallSize; }
    DWORD GetOfferingType() const    { return m_dwOfferingType; }
    DWORD GetBitFlags() const        { return m_dwBitFlags; }
    DWORD GetRating() const          { return m_dwRating; }
    FILETIME GetCreationDate() const { return m_ftCreationDate; }
    XONLINEOFFERING_ID GetId() const { return m_ID; }

};




//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef std::vector< ContentInfo > ContentList;




#endif // CONTENTDOWNLOAD_COMMON_H
