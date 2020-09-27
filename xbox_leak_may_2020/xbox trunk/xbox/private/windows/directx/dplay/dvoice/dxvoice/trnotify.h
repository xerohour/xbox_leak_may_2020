/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		trnotify.h
 *  Content:	Definitions of the IDirectXVoiceNotify interface
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/26/99		rodtoll	Created
 * 09/01/2000 georgioc started port to xbox
 ***************************************************************************/
#ifndef __TRNOTIFY_H
#define __TRNOTIFY_H

#define NODSOUND
#include <xtl.h>
#include <dsoundp.h>
#include "dvoicep.h"
#include "dvshared.h"

#ifdef __cplusplus
extern "C" {
#endif

STDAPI DV_NotifyEvent( LPDIRECTVOICENOTIFYOBJECT lpDVN, DWORD dwType, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
STDAPI DV_ReceiveSpeechMessage( LPDIRECTVOICENOTIFYOBJECT lpDVN, DVID dvidSource, DVID dvidTo, LPVOID lpMessage, DWORD dwSize );
STDAPI DV_Notify_Initialize( LPDIRECTVOICENOTIFYOBJECT lpDVN );

STDAPI DV_Notify_AddRef(LPDIRECTVOICENOTIFYOBJECT lpDVN );
STDAPI DVC_Notify_Release(LPDIRECTVOICENOTIFYOBJECT lpDVN );
STDAPI DVC_Notify_QueryInterface(LPDIRECTVOICENOTIFYOBJECT lpDVN, REFIID riid, LPVOID * ppvObj );
STDAPI DVS_Notify_QueryInterface(LPDIRECTVOICENOTIFYOBJECT lpDVN, REFIID riid, LPVOID * ppvObj );
STDAPI DVS_Notify_Release(LPDIRECTVOICENOTIFYOBJECT lpDVN );

#ifdef __cplusplus
}
#endif

#endif
