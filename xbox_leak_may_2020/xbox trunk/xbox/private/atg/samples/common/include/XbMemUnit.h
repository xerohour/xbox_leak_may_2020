//-----------------------------------------------------------------------------
// File: XbMemUnit.h
//
// Desc: Memory unit object wraps XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT )
//       and XMount/UnmountMU
//
// Hist: 01.29.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBMEMUNIT_H
#define XBMEMUNIT_H

#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: class CXBMemUnit
// Desc: Xbox memory unit
//-----------------------------------------------------------------------------
class CXBMemUnit
{
    static const DWORD dwINVALID = DWORD(-1);

    DWORD m_dwPort;           // XDEVICE_PORT0-3
    DWORD m_dwSlot;           // XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT
    CHAR  m_chLogicalDrive;   // MU drive letter alias; 0 if not mounted

public:

    explicit CXBMemUnit( DWORD dwPort = dwINVALID, DWORD dwSlot = dwINVALID );
    CXBMemUnit( const CXBMemUnit& );
    CXBMemUnit& operator=( const CXBMemUnit& );
    virtual ~CXBMemUnit();

    VOID  Insert( DWORD dwPort, DWORD dwSlot );
    BOOL  Mount( DWORD& dwError );
    VOID  Unmount();
    VOID  Remove();

    BOOL  IsValid() const;
    BOOL  IsMounted() const;
    CHAR  GetDrive() const;
    DWORD GetPort() const;
    DWORD GetSlot() const;
    VOID  GetName( WCHAR* ) const;

    static DWORD GetMemUnitSnapshot();
    static BOOL  GetMemUnitChanges( DWORD& dwInsertions, DWORD& dwRemovals );

    static DWORD GetMemUnitMask( DWORD dwMuIndex );
    static DWORD GetMemUnitPort( DWORD dwMuIndex );
    static DWORD GetMemUnitSlot( DWORD dwMuIndex );
};

#endif // XBMEMUNIT_H
