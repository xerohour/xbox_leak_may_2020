// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// Module Name:
//
//  ksconn.cpp
//
// Abstract:
//
//  Class implementation of CKsConnection.
//
// -------------------------------------------------------------------------------


#include "stdafx.h"
#include "kslibp.h"

// ------------------------------------------------------------------------
// CKsConnection::CKsConnection
// ------------------------------------------------------------------------
CKsConnection::CKsConnection()
{
    ZeroMemory(&m_Connection, sizeof(m_Connection));
} // CKsConnection

// ------------------------------------------------------------------------
// CKsConnection::CKsConnection()
// ------------------------------------------------------------------------
CKsConnection::CKsConnection(PKSTOPOLOGY_CONNECTION pConnection)
{
    if (pConnection)
    {
        CopyMemory(&m_Connection, pConnection, sizeof(m_Connection));
    }
} // CKsConnection

// ------------------------------------------------------------------------
// CKsConnection::CKsConnection()
// ------------------------------------------------------------------------
CKsConnection::CKsConnection(PCKsConnection pksfCopy)
{
    if (pksfCopy)
    {
        m_Connection.FromNode = pksfCopy->FromNode();
        m_Connection.FromNodePin = pksfCopy->FromNodePin();
        m_Connection.ToNode = pksfCopy->ToNode();
        m_Connection.ToNodePin = pksfCopy->ToNodePin();
    }
} // CKsConnection

// ------------------------------------------------------------------------
// CKsConnection::Initialize
// ------------------------------------------------------------------------
void  
CKsConnection::Initialize
(
    PKSTOPOLOGY_CONNECTION pConnection
)
{
    if (pConnection)
    {
        CopyMemory(&m_Connection, pConnection, sizeof(m_Connection));
    }
} // Initialize

// ------------------------------------------------------------------------
// CKsConnection::FromNode
// ------------------------------------------------------------------------
ULONG 
CKsConnection::FromNode()
{
    return m_Connection.FromNode;
} // FromNode

// ------------------------------------------------------------------------
// CKsConnection::FromNodePin
// ------------------------------------------------------------------------
ULONG 
CKsConnection::FromNodePin()
{
    return m_Connection.FromNodePin;
} // FromNodePin

// ------------------------------------------------------------------------
// CKsConnection::ToNode
// ------------------------------------------------------------------------
ULONG 
CKsConnection::ToNode()
{
    return m_Connection.ToNode;
} // ToNode

// ------------------------------------------------------------------------
// CKsConnection::ToNodePin
// ------------------------------------------------------------------------
ULONG 
CKsConnection::ToNodePin()
{
    return m_Connection.ToNodePin;
} // ToNodePin

