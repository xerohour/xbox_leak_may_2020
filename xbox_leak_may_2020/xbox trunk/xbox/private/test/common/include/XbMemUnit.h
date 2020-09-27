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
    DWORD   m_dwPort;           // XDEVICE_PORT0-3
    DWORD   m_dwSlot;           // XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT
    CHAR    m_chLogicalDrive;   // MU drive letter alias

public:

    CXBMemUnit();
    explicit CXBMemUnit( DWORD dwPort, DWORD dwSlot, DWORD& dwError );
    CXBMemUnit( const CXBMemUnit& );
    CXBMemUnit& operator=( const CXBMemUnit& );
    virtual ~CXBMemUnit();

    BOOL  Mount( DWORD dwPort, DWORD dwSlot, DWORD& dwError );
    VOID  Unmount();

    BOOL  IsMounted() const;
    CHAR  GetDrive() const;
    DWORD GetPort() const;
    DWORD GetSlot() const;

    static DWORD GetMemUnitSnapshot();
    static BOOL  GetMemUnitChanges( DWORD& dwInsertions, DWORD& dwRemovals );

    static DWORD GetMemUnitMask( DWORD dwMuIndex );
    static DWORD GetMemUnitPort( DWORD dwMuIndex );
    static DWORD GetMemUnitSlot( DWORD dwMuIndex );
};

#endif // XBMEMUNIT_H
