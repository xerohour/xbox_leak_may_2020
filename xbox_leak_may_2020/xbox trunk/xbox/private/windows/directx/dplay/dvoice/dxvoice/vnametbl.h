/*==========================================================================
 *
 *  Copyright (C) 1999 - 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       vnametbl.h
 *  Content:	Voice Name Table Routines
 *				
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  03/26/00  rodtoll    Created
 *  09/01/2000 georgioc started rewrite/port to xbox
 ***************************************************************************/

#ifndef __NAMETABLE_H
#define __NAMETABLE_H

#include "dvoicep.h"
#include "vplayer.h"
#include "in_core.h"

#define VSIG_VOICENAMETABLE			'BTNV'
#define VSIG_VOICENAMETABLE_FREE	'BTN_'

volatile class CVoiceNameTable
{
public:
	CVoiceNameTable( )
	{
		m_fInitialized = FALSE;
	};
	
	~CVoiceNameTable()
	{
		DeInitialize(FALSE, NULL, NULL);
	}

    HRESULT Initialize();
	HRESULT DeInitialize(BOOL fUnRavel, PVOID pvContext, LPDVMESSAGEHANDLER pvMessageHandler);
	DWORD GetLowestHostOrderID(DVID *pdvidHost);

    BOOL IsEntry( const DVID dvidID );
    HRESULT GetEntry( const DVID dvidID, CVoicePlayer **ppEntry, BOOL fAddReference );
    HRESULT DeleteEntry( const DVID dvidID );
    HRESULT AddEntry( const DVID dvidID, CVoicePlayer *pEntry );

    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'btnv');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

protected:

	LIST_ENTRY	                    m_NameTableList;
	BOOL							m_fInitialized;
};

#endif
