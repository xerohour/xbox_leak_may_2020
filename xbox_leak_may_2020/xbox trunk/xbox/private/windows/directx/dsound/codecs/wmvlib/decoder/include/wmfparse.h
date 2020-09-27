//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wmaparse.h
//
//--------------------------------------------------------------------------

#ifndef WMFPARSE_H
#define WMFPARSE_H

#include "wmftypes.h"
#ifdef macintosh
#include "wmMacSp.h"
#endif
#include "wmc_type.h"
#include "wmcguids.h"

#define MIN_OBJECT_SIZE     24
#define DATA_OBJECT_SIZE    50
#define MAX_PACKET_PAYLOADS 64


#ifdef __cplusplus
extern "C" {
#endif



/****************************************************************************/
tWMCDecStatus LoadHeaderObject(HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadPropertiesObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);

/****************************************************************************/
tWMCDecStatus LoadEncryptionObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadEncryptionObjectEx(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);

/****************************************************************************/
tWMCDecStatus LoadAudioObjectSize(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);

/****************************************************************************/
tWMCDecStatus LoadContentDescriptionObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadMarkerObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);

/****************************************************************************/
tWMCDecStatus LoadExtendedContentDescriptionObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadScriptCommandObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadLicenseStoreObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadExtendedStreamPropertiesObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder);

/****************************************************************************/
tWMCDecStatus LoadIndexObject(HWMCDECODER *phDecoder,
                    U64_WMC cbOffset,
                    U32_WMC cbBuffer,
                    U16_WMC wStreamId,
					WMCINDEXINFO *pIndexInfo
                    );

/****************************************************************************/
tWMCDecStatus LoadObjectHeader(
              HWMCDECODER *phDecoder, GUID_WMC *pObjectId, U64_WMC *pqwSize);

/****************************************************************************/
tWMCDecStatus ParseAsfHeader(
               HWMCDECODER *phDecoder, U8_WMC isFull);
/****************************************************************************/
tWMCDecStatus WMCDecParsePayloadHeader(HWMCDECODER phDecoder);
/****************************************************************************/
tWMCDecStatus WMCDecParsePacketHeader(HWMCDECODER phDecoder);
/****************************************************************************/
LoadCodecListObject(U32_WMC cbBuffer, HWMCDECODER *phDecoder);
/****************************************************************************/
tWMCDecStatus LoadVirtualMetaDataObject(U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder, U32_WMC Offset);
/****************************************************************************/
tWMCDecStatus LoadClockObject(U32_WMC cbBuffer, HWMCDECODER *phDecoder);
/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
