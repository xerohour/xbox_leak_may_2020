/*==========================================================================
 *
 *  Copyright (C) 1999 - 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       vnametbl.h
 *  Content:    Voice Player Name Table
 *              
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/26/00    rodtoll Created
 *  06/02/00    rodtoll  Updated so host migration algorithm returns ID as well as order ID 
 *  07/01/2000  rodtoll Bug #38280 - DVMSGID_DELETEVOICEPLAYER messages are being sent in non-peer to peer sessions
 *                      Nametable will now only unravel with messages if session is peer to peer.
 *  07/09/2000  rodtoll Added signature bytes 
 ***************************************************************************/

#include "dvntos.h"
#include "in_core.h"
#include <dvoicep.h>
#include "dvprot.h"
#include "vplayer.h"
#include "sndutils.h"
#include "vnametbl.h"

#undef DPF_FNAME
#define DPF_FNAME "CVoiceNameTable::Initialize"

HRESULT CVoiceNameTable::Initialize()
{
    BOOL fResult;
    ULONG i;
    HRESULT hr;

    InitializeListHead(&m_NameTableList);

    m_fInitialized = TRUE;

    return DV_OK;
};

// DeInitialize
//
// Cleanup the name table
HRESULT CVoiceNameTable::DeInitialize(BOOL fUnRavel, PVOID pvContext, LPDVMESSAGEHANDLER pvMessageHandler )
{
    CVoicePlayer *pPlayer;
    DVID dvID;
    PVOID pvPlayerContext;
    DVMSG_DELETEVOICEPLAYER dvMsgDelete;
    KIRQL irql;
    PLIST_ENTRY pEntry;

    if( !m_fInitialized )
        return DV_OK;

    RIRQL(irql);

    if (IsListEmpty(&m_NameTableList)) {
        LIRQL(irql);
        return DV_OK;
    }

    pEntry = m_NameTableList.Flink;

    while (pEntry != &m_NameTableList) {

        pPlayer = (CVoicePlayer *) CONTAINING_RECORD(pEntry, CVoicePlayer, m_PoolEntry);

        //
        // Mark it as disconnected
        //

        pPlayer->SetDisconnected();

        dvID = pPlayer->GetPlayerID();
        pvPlayerContext = pPlayer->GetContext();

        if( pvMessageHandler != NULL )
        {
            if( fUnRavel )
            {
                //
                // Drop locks to call up to user
                //

                LIRQL(irql);                    

                dvMsgDelete.dvidPlayer = dvID;
                dvMsgDelete.dwSize = sizeof( DVMSG_DELETEVOICEPLAYER );
                dvMsgDelete.pvPlayerContext = pvPlayerContext;

                (*pvMessageHandler)( pvContext, DVMSGID_DELETEVOICEPLAYER, &dvMsgDelete );
           
                RIRQL(irql);
            }
        }

        //
        // remove player from this first, because release will stick it in the pool
        //

        RemoveEntryList(&pPlayer->m_PoolEntry);

        //
        // Release the player record reference we had
        //

        LIRQL(irql);
        pPlayer->Release();
        RIRQL(irql);

        pEntry = m_NameTableList.Flink;
    }

    LIRQL(irql);    

    m_fInitialized = FALSE;

    return DV_OK;
}

DWORD CVoiceNameTable::GetLowestHostOrderID(DVID *pdvidHost)
{
    DWORD dwLowestID = DVPROTOCOL_HOSTORDER_INVALID;
    KIRQL irql;
    CVoicePlayer *pPlayer;
    PLIST_ENTRY pEntry;
    
    RIRQL(irql);

    pEntry = m_NameTableList.Flink;

    while ( pEntry != &m_NameTableList)
    {
        pPlayer = (CVoicePlayer *) CONTAINING_RECORD(pEntry, CVoicePlayer, m_PoolEntry);

        DPVF(DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: ID [0x%x] ORDERID [0x%x]", 
             pPlayer->GetPlayerID(),
             pPlayer->GetHostOrder() );

        if( pPlayer->GetPlayerID() < dwLowestID )
        {
            DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL,
                  "HOST MIGRATION: ID [0x%x] IS CURRENT CANDIDATE",
                  pPlayer->GetPlayerID() );

            dwLowestID = pPlayer->GetHostOrder();

            *pdvidHost = pPlayer->GetPlayerID();
        }

        pEntry = pEntry->Flink;
    }

    LIRQL(irql);

    return dwLowestID;
}


BOOL CVoiceNameTable::IsEntry( const DVID dvidID )
{
    CVoicePlayer *pEntry;

    if (GetEntry(dvidID,&pEntry,FALSE) != DV_OK) {
        return FALSE;
    } else {
        return TRUE;
    }
}

HRESULT CVoiceNameTable::GetEntry( const DVID dvidID, CVoicePlayer **ppEntry, BOOL fAddReference )
{
    BOOL fFound = FALSE;
    KIRQL irql;
    CVoicePlayer *pPlayer;
    PLIST_ENTRY pEntry;    

    RIRQL(irql);

    if (IsListEmpty(&m_NameTableList)) {

        *ppEntry = NULL;
        LIRQL(irql);
        return DVERR_INVALIDPLAYER;

    }

    pEntry = m_NameTableList.Flink;

    while ( pEntry != &m_NameTableList)
    {
        pPlayer = (CVoicePlayer *) CONTAINING_RECORD(pEntry, CVoicePlayer, m_PoolEntry);

        if( pPlayer->GetPlayerID() == dvidID )
        {
            *ppEntry = pPlayer;
            fFound = TRUE;

        }

        pEntry = pEntry->Flink;
    }

    LIRQL(irql);

    if( !fFound )
    {
        *ppEntry = NULL;
        LIRQL(irql);
        return DVERR_INVALIDPLAYER;
    }

    ASSERT( *ppEntry != NULL );

    if( fAddReference )
    {
        (*ppEntry)->AddRef();
    }

    LIRQL(irql)

    return DV_OK;
}

HRESULT CVoiceNameTable::AddEntry( const DVID dvidID, CVoicePlayer *pEntry )
{
    HRESULT hr;
    CVoicePlayer *pTmpEntry;
    KIRQL irql;

    hr = GetEntry( dvidID, &pTmpEntry, FALSE);

    if(SUCCEEDED(hr))
    {
        return DVERR_GENERIC;
    }

    pEntry->AddRef();

    RIRQL(irql);

    InsertTailList(&m_NameTableList,&pEntry->m_PoolEntry);

    LIRQL(irql);

    return DV_OK;
}

HRESULT CVoiceNameTable::DeleteEntry( const DVID dvidID )
{
    BOOL fFound;
    CVoicePlayer *pTmpEntry;
    KIRQL irql;
    HRESULT hr;

    hr = GetEntry( dvidID, &pTmpEntry, FALSE);

    if(FAILED(hr))
    {
        return hr;
    }

    pTmpEntry->Release();

    RIRQL(irql);

    RemoveEntryList(&pTmpEntry->m_PoolEntry);

    LIRQL(irql);

    return DV_OK;
}


