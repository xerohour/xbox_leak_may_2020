/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvshared.h
 *  Content:	Utility functions for DirectXVoice structures.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/06/99		rodtoll	Created It
 * 09/01/2000   georgioc started rewrite/port to xbox
 ***************************************************************************/

#ifndef __DVSHARED_H
#define __DVSHARED_H

#include "dvntos.h"
#include <dvoicep.h>
#include <string.h>
#include "in_core.h"
#include "dvengine.h"


//
// added when talking to compressors since they tend to overrun memory
//

#define COMPRESSION_SLUSH			2
#define DV_CT_FRAMES_PER_PERIOD     32
#define DV_CT_INNER_QUEUE_SIZE		64
#define DV_CT_MAXHIGHWATERMARK	    32

#define DV_FRAMEF_IS_SILENCE        0x00000001
#define DV_FRAMEF_IS_LOST           0x00000002


#define DVLAT_RAW      0x00000001
#define DVLAT_CVOICEPLAYER  0x00000002

struct DIRECTVOICEOBJECT;

volatile struct DIRECTVOICENOTIFYOBJECT
{
	LPVOID						lpNotifyVtble;
	DIRECTVOICEOBJECT			*lpDV;
	LONG						lRefCnt;
};

volatile struct DIRECTVOICEOBJECT
{
	LONG						lIntRefCnt;
	CDirectVoiceEngine			*lpDVEngine;
	CDirectVoiceTransport		*lpDVTransport;
	DIRECTVOICENOTIFYOBJECT		dvNotify;
};

typedef DIRECTVOICEOBJECT *LPDIRECTVOICEOBJECT;
typedef DIRECTVOICENOTIFYOBJECT *LPDIRECTVOICENOTIFYOBJECT;

//
// used to track XMO packets
//

typedef struct _DVMEDIAPACKET  
{

    XMEDIAPACKET xmp;
    DWORD dwCompletedSize;
    DWORD dwStatus;
    PVOID pvReserved;

} DVMEDIAPACKET, *PDVMEDIAPACKET;


typedef struct _DVPOOLALLOC  
{

    LIST_ENTRY ListEntry;
    UCHAR Data[4];

} DVPOOLALLOC, *PDVPOOLALLOC;

VOID DV_INSERT_TAIL(PLIST_ENTRY pListHead, PVOID data);
VOID DV_REMOVE_ENTRY(PVOID pData);
PVOID DV_GET_LIST_HEAD(PLIST_ENTRY pListHead);
PVOID DV_GET_NEXT(PLIST_ENTRY pListHead, PVOID pData);
PVOID DV_REMOVE_TAIL(PLIST_ENTRY pListHead);
VOID DV_INSERT_HEAD(PLIST_ENTRY pListHead, PVOID data);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// function declarations
/////////////////////////////////////////////////////////////////////////////////////////////////////////


HRESULT DV_InitializeList(PLIST_ENTRY pListHead, DWORD dwNumElements,DWORD dwSize, DWORD dwType);
VOID DV_FreeList(PLIST_ENTRY pListHead, DWORD dwType);


HRESULT
DV_InitMediaPacketPool(
    LIST_ENTRY *pPool,
    DWORD dwBufferSize,
    DWORD   bAlloc
    );

VOID
DV_FreeMediaPacketPool(
    LIST_ENTRY *pPool,
    DWORD   bFree
    );



STDAPI DV_Initialize( LPDIRECTVOICEOBJECT lpdvObject, PVOID lpTransport, LPDVMESSAGEHANDLER lpMessageHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements );

HRESULT DV_CreateConverter(
    GUID guidCT,
    XMediaObject **pConverter,
    WAVEFORMATEX *pwfxUnencodedFormat,
    WAVEFORMATEX **ppwfxEncodedFormat,
    DWORD dwUncompressedSize,
    BOOLEAN bCreateEncoder
    );

HRESULT DV_CopySessionDescToBuffer( LPVOID lpTarget, LPDVSESSIONDESC lpdvSessionDesc, LPDWORD lpdwSize );

BOOL DV_ValidBufferAggresiveness( DWORD dwValue );
BOOL DV_ValidBufferQuality( DWORD dwValue );
BOOL DV_ValidSensitivity( DWORD dwValue );

HRESULT DV_ValidClientConfig( LPDVCLIENTCONFIG lpClientConfig );
HRESULT DV_ValidSessionDesc( LPDVSESSIONDESC lpSessionDesc );
HRESULT DV_ValidTargetList( PDVID pdvidTargets, DWORD dwNumTargets );

BOOL DV_ValidDirectVoiceObject( LPDIRECTVOICEOBJECT lpdv );
BOOL DV_ValidDirectXVoiceClientObject( LPDIRECTVOICEOBJECT lpdvc );
BOOL DV_ValidDirectXVoiceServerObject( LPDIRECTVOICEOBJECT lpdvs );
HRESULT DV_ValidMessageArray( LPDWORD lpdwMessages, DWORD dwNumMessages, BOOL fServer );

void DV_DUMP_SessionDesc( LPDVSESSIONDESC lpdvSessionDesc );
void DV_DUMP_ClientConfig( LPDVCLIENTCONFIG lpdvClientConfig );
void DV_DUMP_WaveFormatEx( LPWAVEFORMATEX lpwfxFormat );
void DV_DUMP_GUID( GUID guid );

#ifdef _DEBUG
#define DV_DUMP_SD( sd )		DV_DUMP_SessionDesc( sd )
#define DV_DUMP_CC( cc )		DV_DUMP_ClientConfig( cc )
#define DV_DUMP_CIF( cif, ne )	DV_DUMP_FullCompressionInfo( cif, ne )
#else 
#define DV_DUMP_CIF( cif, ne )
#define DV_DUMP_CI( ci, ne )
#define DV_DUMP_SD( sd )
#define DV_DUMP_SDC( sdc )
#define DV_DUMP_CC( cc )
#endif



#endif
