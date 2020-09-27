/**************************************************************************
 *
 *  Copyright (C) 2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xact.h
 *  Content:    X-Box Audio Content Tool Runtime Engine.
//@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  01/17/2002  georgioc Created.
//@@END_MSINTERNAL
 *
 **************************************************************************/

#ifndef __XACT_ENGINE_INCLUDED__
#define __XACT_ENGINE_INCLUDED__

#pragma warning(disable:4201)

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p)
#endif // UNREFERENCED_PARAMETER

//
// Forward declarations
//

typedef struct IXACTEngine IXACTEngine;
typedef IXACTEngine *LPXACTENGINE;
typedef IXACTEngine *PXACTENGINE;

typedef struct IXACTSoundBank IXACTSoundBank;
typedef IXACTSoundBank *LPXACTSOUNDBANK;
typedef IXACTSoundBank *PXACTSOUNDBANK;

typedef struct IXACTSoundSource IXACTSoundSource;
typedef IXACTSoundSource *LPXACTSOUNDSOURCE;
typedef IXACTSoundSource *PXACTSOUNDSOURCE;

typedef struct IXACTSoundCue IXACTSoundCue;
typedef IXACTSoundCue *LPXACTSOUNDCUE;
typedef IXACTSoundCue *PXACTSOUNDCUE;

typedef struct IXACTWaveBank IXACTWaveBank;
typedef IXACTWaveBank *LPXACTWAVEBANK;
typedef IXACTWaveBank *PXACTWAVEBANK;

//@@BEGIN_MSINTERNAL
typedef struct XACT_TRACK_EVENT XACT_TRACK_EVENT;
typedef XACT_TRACK_EVENT *PXACT_TRACK_EVENT;
typedef XACT_TRACK_EVENT *LPXACT_TRACK_EVENT;
//@@END_MSINTERNAL
//
// Structures and types
//

#define XACT_SIZEOF_MARKER_DATA		8

//
// Notifications
//

typedef enum _XACT_NOTIFICATION_TYPE {

    eXACTNotification_Start = 0,
    eXACTNotification_Stop,
    eXACTNotification_Marker,
	eXACTNotification_Max

};

#define XACT_MASK_NOTIFICATION_TYPE		0x0000FFFF
#define XACT_MASK_NOTIFICATION_FLAGS	0xFFFF0000


#define XACT_NOTIFICATION_TYPE_UNUSED	0xFFFFFFFF

//
// flags used when registering notifications
//

#define XACT_FLAG_NOTIFICATION_PERSIST	0x00010000


typedef struct _XACT_NOTIFICATION_START {

    DWORD dwFlags;

} XACT_NOTIFICATION_START, *PXACT_NOTIFICATION_START, *LPXACT_NOTIFICATION_START;
 
typedef struct _XACT_NOTIFICATION_STOP {

    DWORD dwFlags;

} XACT_NOTIFICATION_STOP, *PXACT_NOTIFICATION_STOP, *LPXACT_NOTIFICATION_STOP;
 
typedef struct _XACT_NOTIFICATION_MARKER {

    BYTE    bData[XACT_SIZEOF_MARKER_DATA];

} XACT_NOTIFICATION_MARKER, *PXACT_NOTIFICATION_MARKER, *LPXACT_NOTIFICATION_MARKER;

union XACT_NOTIFICATION_UNION {

    XACT_NOTIFICATION_START Start;
    XACT_NOTIFICATION_STOP Stop;
    XACT_NOTIFICATION_MARKER Marker;

}; 

typedef struct _XACT_NOTIFICATION_DESCRIPTION{
    
    DWORD            dwType;
    PXACTSOUNDBANK   pSoundBank;
    PXACTSOUNDCUE    pSoundCue;	
	DWORD			 dwSoundCueIndex;
    PVOID            pvContext;
	HANDLE			 hEvent;

} XACT_NOTIFICATION_DESCRIPTION, *PXACT_NOTIFICATION_DESCRIPTION, *LPXACT_NOTIFICATION_DESCRIPTION;

typedef struct _XACT_NOTIFICATION{
    

    XACT_NOTIFICATION_DESCRIPTION	Header;
    XACT_NOTIFICATION_UNION			Data;
    REFERENCE_TIME					rtTimeStamp;

} XACT_NOTIFICATION, *PXACT_NOTIFICATION, *LPXACT_NOTIFICATION;        
 
typedef struct _XACT_RUNTIME_PARAMETERS {
    DWORD dwMax2DHwVoices;
    DWORD dwMax3DHwVoices;
    DWORD dwMaxConcurrentStreams;
    PVOID pvHeap;
    DWORD dwHeapSize;
} XACT_RUNTIME_PARAMETERS, *PXACT_RUNTIME_PARAMETERS, *LPXACT_RUNTIME_PARAMETERS;

//
// constants
//

#define XACT_FLAG_SOUNDSOURCE_2D            0x00000001
#define XACT_FLAG_SOUNDSOURCE_3D            0x00000002
#define XACT_MASK_SOUNDSOURCE_FLAGS         (XACT_FLAG_SOUNDSOURCE_3D | XACT_FLAG_SOUNDSOURCE_2D)

#define XACT_FLAG_SOUNDCUE_AUTORELEASE			0x00000001
#define XACT_FLAG_SOUNDCUE_SYNCHRONOUS          0x10000000

#define XACT_SOUNDCUE_INDEX_UNUSED				0xFFFFFFFF
#define XACT_TRACK_INDEX_UNUSED	    			0xFFFFFFFF

//
// API definitions
//

//
// IXACTEngine
//

STDAPI XACTEngineCreate(PXACTENGINE *ppEngine, PXACT_RUNTIME_PARAMETERS pParams);
STDAPI_(void) XACTEngineDoWork(void);

STDAPI_(ULONG) IXACTEngine_AddRef(PXACTENGINE pEngine);
STDAPI_(ULONG) IXACTEngine_Release(PXACTENGINE pEngine);
STDAPI IXACTEngine_LoadDspImage(PXACTENGINE pEngine, PVOID pvData, DWORD dwSize, LPCDSEFFECTIMAGELOC pEffectLoc);
STDAPI IXACTEngine_CreateSoundSource(PXACTENGINE pEngine, DWORD dwFlags, PXACTSOUNDSOURCE *ppSoundSource);
STDAPI IXACTEngine_CreateSoundBank(PXACTENGINE pEngine, PVOID pvData, DWORD dwSize, PXACTSOUNDBANK *ppSoundBank);
STDAPI IXACTEngine_RegisterWaveBank(PXACTENGINE pEngine, PVOID pvData, DWORD dwSize, PXACTWAVEBANK * ppWaveBank);
STDAPI IXACTEngine_RegisterStreamedWaveBank(PXACTENGINE pEngine, PVOID pvStreamingBuffer, DWORD dwSize, HANDLE hFile, DWORD dwOffset, PXACTWAVEBANK *ppWaveBank);
STDAPI IXACTEngine_UnRegisterWaveBank(PXACTENGINE pEngine, PXACTWAVEBANK pWaveBank);
STDAPI IXACTEngine_SetMasterVolume(PXACTENGINE pEngine, LONG lVolume);
STDAPI IXACTEngine_SetListenerParameters(PXACTENGINE pEngine, LPCDS3DLISTENER pcDs3dListener, LPCDSI3DL2LISTENER pds3dl, DWORD dwApply);
STDAPI IXACTEngine_GlobalPause(PXACTENGINE pEngine, BOOL bPause);
STDAPI IXACTEngine_RegisterNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc);
STDAPI IXACTEngine_UnRegisterNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc);
STDAPI IXACTEngine_GetNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc, PXACT_NOTIFICATION pNotification);
STDAPI IXACTEngine_FlushNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc);
STDAPI IXACTEngine_CommitDeferredSettings(PXACTENGINE pEngine);
//@@BEGIN_MSINTERNAL
STDAPI IXACTEngine_ScheduleEvent(PXACTENGINE pEngine, XACT_TRACK_EVENT *pEventDesc, PXACTSOUNDCUE pSoundCue, DWORD dwTrackIndex);
//@@END_MSINTERNAL

#if defined(__cplusplus) && !defined(CINTERFACE)

struct IXACTEngine
{

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IXACTEngine_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IXACTEngine_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE LoadDspImage(PVOID pvData, DWORD dwSize, LPCDSEFFECTIMAGELOC pEffectLoc)
    {
        return IXACTEngine_LoadDspImage(this, pvData, dwSize, pEffectLoc);
    }

    __inline HRESULT STDMETHODCALLTYPE CreateSoundSource(DWORD dwFlags,PXACTSOUNDSOURCE *ppSoundSource)
    {
        return IXACTEngine_CreateSoundSource(this, dwFlags, ppSoundSource);
    }

    __inline HRESULT STDMETHODCALLTYPE CreateSoundBank(PVOID pvData, DWORD dwSize, PXACTSOUNDBANK *ppSoundBank)
    {
        return IXACTEngine_CreateSoundBank(this, pvData, dwSize, ppSoundBank);
    }

    __inline HRESULT STDMETHODCALLTYPE RegisterWaveBank(PVOID pvData, DWORD dwSize, PXACTWAVEBANK *ppWaveBank)
    {
        return IXACTEngine_RegisterWaveBank(this, pvData, dwSize, ppWaveBank);
    }

    __inline HRESULT STDMETHODCALLTYPE RegisterStreamedWaveBank(PVOID pvStreamingBuffer, DWORD dwSize, HANDLE hFile, DWORD dwOffset, PXACTWAVEBANK *ppWaveBank)
    {
        return IXACTEngine_RegisterStreamedWaveBank(this, pvStreamingBuffer, dwSize, hFile, dwOffset, ppWaveBank);
    }

    __inline HRESULT STDMETHODCALLTYPE UnRegisterWaveBank(PXACTWAVEBANK pWaveBank)
    {
        return IXACTEngine_UnRegisterWaveBank(this, pWaveBank);
    }

    __inline HRESULT STDMETHODCALLTYPE SetMasterVolume(LONG lVolume)
    {
        return IXACTEngine_SetMasterVolume(this, lVolume);
    }

    __inline HRESULT STDMETHODCALLTYPE SetListenerParameters(LPCDS3DLISTENER pcDs3dListener, LPCDSI3DL2LISTENER pds3dl, DWORD dwApply)
	{
	    return IXACTEngine_SetListenerParameters(this, pcDs3dListener, pds3dl, dwApply);
	}

    __inline HRESULT STDMETHODCALLTYPE GlobalPause(BOOL bPause)
    {
        return IXACTEngine_GlobalPause(this,bPause);
    }

    __inline HRESULT STDMETHODCALLTYPE RegisterNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
    {
        return IXACTEngine_RegisterNotification(this, pNotificationDesc);
    }

    __inline HRESULT STDMETHODCALLTYPE UnRegisterNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
    {
        return IXACTEngine_UnRegisterNotification(this, pNotificationDesc);
    }

    __inline HRESULT STDMETHODCALLTYPE GetNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc, PXACT_NOTIFICATION pNotification)
    {
        return IXACTEngine_GetNotification(this, pNotificationDesc, pNotification);
    }

    __inline HRESULT STDMETHODCALLTYPE FlushNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
    {
        return IXACTEngine_FlushNotification(this, pNotificationDesc);
    }

    __inline HRESULT STDMETHODCALLTYPE CommitDeferredSettings(void)
    {
        return IXACTEngine_CommitDeferredSettings(this);
    }

//@@BEGIN_MSINTERNAL
    __inline HRESULT STDMETHODCALLTYPE ScheduleEvent(XACT_TRACK_EVENT *pEventDesc, PXACTSOUNDCUE pSoundCue, DWORD dwTrackIndex)
	{
        return IXACTEngine_ScheduleEvent(this, pEventDesc, pSoundCue, dwTrackIndex);
	}
//@@END_MSINTERNAL

};

#endif // defined(__cplusplus) && !defined(CINTERFACE)

//
// IXACTSoundSource
//

STDAPI_(ULONG) IXACTSoundSource_AddRef(PXACTSOUNDSOURCE pSoundSource);
STDAPI_(ULONG) IXACTSoundSource_Release(PXACTSOUNDSOURCE pSoundSource);
STDAPI IXACTSoundSource_SetPosition(PXACTSOUNDSOURCE pSoundSource, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
STDAPI IXACTSoundSource_SetAllParameters(PXACTSOUNDSOURCE pSoundSource, LPCDS3DBUFFER pcDs3dBuffer, DWORD dwApply);
STDAPI IXACTSoundSource_SetConeOrientation(PXACTSOUNDSOURCE pSoundSource,FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
STDAPI IXACTSoundSource_SetI3DL2Source(PXACTSOUNDSOURCE pSoundSource,LPCDSI3DL2BUFFER pds3db, DWORD dwApply);
STDAPI IXACTSoundSource_SetVelocity(PXACTSOUNDSOURCE pSoundSource,FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
STDAPI IXACTSoundSource_SetMixBins(PXACTSOUNDSOURCE pSoundSource, LPCDSMIXBINS pMixBins);
STDAPI IXACTSoundSource_SetMixBinVolumes(PXACTSOUNDSOURCE pSoundSource, LPCDSMIXBINS pMixBins);

#if defined(__cplusplus) && !defined(CINTERFACE)

struct IXACTSoundSource
{

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IXACTSoundSource_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IXACTSoundSource_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE SetPosition( FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
    {
        return IXACTSoundSource_SetPosition(this, x, y, z, dwApply);
    }

    __inline HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDS3DBUFFER pcDs3dBuffer, DWORD dwApply)
    {
        return IXACTSoundSource_SetAllParameters(this, pcDs3dBuffer, dwApply);
    }

    __inline HRESULT STDMETHODCALLTYPE SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
    {
        return IXACTSoundSource_SetConeOrientation(this, x, y, z, dwApply);
    }

    __inline HRESULT STDMETHODCALLTYPE SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwApply)
    {
        return IXACTSoundSource_SetI3DL2Source(this, pds3db, dwApply);
    }

    __inline HRESULT STDMETHODCALLTYPE SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
    {
        return IXACTSoundSource_SetVelocity(this, x,  y,  z, dwApply);
    }

    __inline HRESULT STDMETHODCALLTYPE SetMixBins(LPCDSMIXBINS pMixBins)
    {
        return IXACTSoundSource_SetMixBins(this, pMixBins);
    }

    __inline HRESULT STDMETHODCALLTYPE SetMixBinVolumes(LPCDSMIXBINS pMixBins)
    {
        return IXACTSoundSource_SetMixBinVolumes(this, pMixBins);
    }

};

#endif // defined(__cplusplus) && !defined(CINTERFACE)

//
// IXACTSoundBank
//

STDAPI_(ULONG) IXACTSoundBank_AddRef(PXACTSOUNDBANK pBank);
STDAPI_(ULONG) IXACTSoundBank_Release(PXACTSOUNDBANK pBank);
STDAPI IXACTSoundBank_GetSoundCueIndexFromFriendlyName(PXACTSOUNDBANK pBank, LPCSTR lpFriendlyName, PDWORD pdwSoundCueIndex);
STDAPI IXACTSoundBank_Play(PXACTSOUNDBANK pBank, DWORD dwSoundCueIndex, PXACTSOUNDSOURCE pSoundSource, DWORD dwFlags, PXACTSOUNDCUE *ppSoundCue);
STDAPI IXACTSoundBank_Stop(PXACTSOUNDBANK pBank, DWORD dwSoundCueIndex, DWORD dwFlags, PXACTSOUNDCUE pSoundCue);
STDAPI IXACTSoundBank_SetSliderValue(PXACTSOUNDBANK pBank, DWORD dwSoundCueIndex, DWORD dwSliderIndex, PVOID pvValue);

#if defined(__cplusplus) && !defined(CINTERFACE)

struct IXACTSoundBank
{

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IXACTSoundBank_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IXACTSoundBank_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSoundCueIndexFromFriendlyName(LPCSTR lpFriendlyName, PDWORD pdwSoundCueIndex)
    {
        return IXACTSoundBank_GetSoundCueIndexFromFriendlyName(this, lpFriendlyName, pdwSoundCueIndex);
    }

    __inline HRESULT STDMETHODCALLTYPE Play( DWORD dwSoundCueIndex, PXACTSOUNDSOURCE pSoundSource, DWORD dwFlags, PXACTSOUNDCUE *ppSoundCue)
    {
        return IXACTSoundBank_Play(this, dwSoundCueIndex, pSoundSource, dwFlags, ppSoundCue);
    }

    __inline HRESULT STDMETHODCALLTYPE Stop( DWORD dwSoundCueIndex, DWORD dwFlags, PXACTSOUNDCUE pSoundCue)
    {
        return IXACTSoundBank_Stop(this, dwSoundCueIndex, dwFlags, pSoundCue);
    }

    __inline HRESULT STDMETHODCALLTYPE SetSliderValue(DWORD dwSoundCueIndex, DWORD dwSliderIndex, PVOID pvValue)
    {
        return IXACTSoundBank_SetSliderValue(this, dwSoundCueIndex, dwSliderIndex, pvValue);
    }

};

#endif // defined(__cplusplus) && !defined(CINTERFACE)


//@@BEGIN_MSINTERNAL

#define XACT_SOUNDBANK_HEADER_FRIENDLYNAME_LENGTH 16
#define XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH 16
#define XACT_SOUNDBANK_WAVEBANK_FRIENDLYNAME_LENGTH 16

#define XACT_SOUNDBANK_HEADER_SIGNATURE        'KBDS'
#define XACT_SOUNDBANK_HEADER_VERSION          1

typedef struct _XACT_SOUNDBANK_FILE_HEADER{

    DWORD    dwSignature;
    DWORD    dwVersion;
    DWORD    dwFlags;
    DWORD    dwSoundEntryCount;                 // Number of entries in the bank
    DWORD    dwCueEntryCount;                   // Number of cues in the bank;
    CHAR     szFriendlyName[XACT_SOUNDBANK_HEADER_FRIENDLYNAME_LENGTH];   // friendly name

} XACT_SOUNDBANK_FILE_HEADER, *PXACT_SOUNDBANK_FILE_HEADER, *LPXACT_SOUNDBANK_FILE_HEADER; 


//
// content flags defining CU behavior
//

#define XACT_FLAG_CUE_ENTRY_QUEUE     		0x00000001
#define XACT_FLAG_CUE_ENTRY_CROSSFADE 		0x00000002

//
// table of N cue entries follows the header
//

typedef struct _XACT_SOUNDBANK_CUE_ENTRY{

    DWORD    dwFlags;
    DWORD    dwSoundIndex;
    CHAR     szFriendlyName[XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH];

} XACT_SOUNDBANK_CUE_ENTRY, *PXACT_SOUNDBANK_CUE_ENTRY, *LPXACT_SOUNDBANK_CUE_ENTRY; 


#define XACT_FLAG_SOUND_3D              0x00000001
#define XACT_FLAG_SOUND_FXMULTIPASS     0x00000002

#define XACTMIXBINVOLUMEPAIR DSMIXBINVOLUMEPAIR

//
// table of N sound entries follows the cue table
//

typedef struct _XACT_SOUNDBANK_SOUND_ENTRY{

    DWORD					dwFlags;
    DWORD					dw3DParametersOffset;
    DWORD					dwTrackTableOffset;
    DWORD					dwWaveBankTableOffset;    
	WORD					wPriority;
	WORD					wLayer;
    WORD					wGroupNumber;
    WORD					wTrackCount;
    WORD					wWaveBankCount;
	WORD					wSliderCount;

} XACT_SOUNDBANK_SOUND_ENTRY, *PXACT_SOUNDBANK_SOUND_ENTRY, *LPXACT_SOUNDBANK_SOUND_ENTRY; 

//
// 3d parameters data structure that can optionally be associated with a sound
//

typedef struct _XACT_SOUNDBANK_SOUND_3D_PARAMETERS {

    XACTMIXBINVOLUMEPAIR	aVolumePair;	// volume for 8th mixbin on a 3d destination
    DWORD    dwInsideConeAngle;      // Buffer inside cone angle
    DWORD    dwOutsideConeAngle;     // Buffer outside cone angle
    LONG     lConeOutsideVolume;     // Volume outside the cone
    FLOAT    flMinDistance;          // Minimum distance value
    FLOAT    flMaxDistance;          // Maximum distance value
    DWORD    dwMode;                 // 3D processing mode
    FLOAT    flDistanceFactor;       // Distance factor
    FLOAT    flRolloffFactor;        // Rolloff factor
    FLOAT    flDopplerFactor;        // Doppler factor
    DWORD    dwDataEntryCount;       // number of custom rollof data points
    
    //
    // array of FLOATs immediately following if dwTableEntryCount != 0
    //

} XACT_SOUNDBANK_SOUND_3D_PARAMETERS, *PXACT_SOUNDBANK_SOUND_3D_PARAMETERS;


//
// wave banks are associated with a sound through a table. This is because the same wave bank
// can be re-used by multiple sounds so we need something like a handle table to abstract in-between
//
// the table of wavebank offsets follows the sound entry table
//

typedef struct _XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY {

    CHAR  szFriendlyName[XACT_SOUNDBANK_WAVEBANK_FRIENDLYNAME_LENGTH];
    DWORD dwDataOffset;

} XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY, *PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY, *LPXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY;

//
// the track table is the array of track entries and follows the wavebank entry table
//

typedef struct _XACT_SOUNDBANK_TRACK_ENTRY {

    WORD wFlags;
    WORD wEventEntryCount;
    DWORD dwEventDataOffset;

} XACT_SOUNDBANK_TRACK_ENTRY, *PXACT_SOUNDBANK_TRACK_ENTRY, *LPXACT_SOUNDBANK_TRACK_ENTRY;

//
// slider data types
//

//
// the slider table is the array of slider entries and follows the track entry table
//

typedef struct _XACT_SOUNDBANK_SLIDER_ENTRY {
	WORD wNumHwParameters;
	WORD wSoundIndex;
	WORD wTrackIndex;
	WORD wReserved;
	DWORD dwMappingTableOffset;
} XACT_SOUNDBANK_SLIDER_ENTRY, *PXACT_SOUNDBANK_SLIDER_ENTRY, *LPXACT_SOUNDBANK_SLIDER_ENTRY;

//
// after the table of slider entries, there is a list of tables of mapping entries.
// each slider points to a table of mapping entry offsets. This way multiple mappings
// can be re-used by different sliders
//

typedef struct _XACT_SLIDER_MAPPING_TABLE_ENTRY {
	DWORD dwDataOffset;
} XACT_SLIDER_MAPPING_TABLE_ENTRY, *PXACT_SLIDER_MAPPING_TABLE_ENTRY, *LPXACT_SLIDER_MAPPING_TABLE_ENTRY;

//
// a slider mapping entry identifies the hw parameter associated with the slider
// and has a N point table of values (the mapping function). The values are of the native
// format of the hw parameter
//

typedef struct _XACT_SLIDER_MAPPING_ENTRY {
	WORD wParameterId;
	WORD wElementCount;
	DWORD dwData[1];

} XACT_SLIDER_MAPPING_ENTRY, *PXACT_SLIDER_MAPPING_ENTRY, *LPXACT_SLIDER_MAPPING_ENTRY;

//
// each track entry points to an array of variable length event entries. the events for all tracks follow
// the slider table
//

//
// Sequencer events
//

#define XACT_FLAG_EVENT_RUNTIME 	    0x00000001
#define XACT_FLAG_EVENT_USES_FXIN   	0x00000002

typedef struct _XACT_TRACK_EVENT_HEADER {

    WORD	wType;	
    WORD	wSize;
	DWORD	dwFlags;    
	ULONG	lSampleTime;

} XACT_TRACK_EVENT_HEADER, *PXACT_TRACK_EVENT_HEADER, *LPXACT_TRACK_EVENT_HEADER;

//
// Structures and types
//

typedef enum _XACT_TRACK_EVENT_TYPES {

    eXACTEvent_Play = 0,
	eXACTEvent_PlayWithPitchAndVolumeVariation, 
    eXACTEvent_Stop,
	eXACTEvent_PitchAndVolumeVariation,
	eXACTEvent_SetFrequency,
	eXACTEvent_SetVolume,
	eXACTEvent_SetHeadroom,
	eXACTEvent_SetLFO,
	eXACTEvent_SetEG,
	eXACTEvent_SetFilter,
	eXACTEvent_Marker,
	eXACTEvent_LoopStart,
	eXACTEvent_LoopEnd,
	eXACTEvent_SetMixBinVolumes,

	//
	// global events
	//

	eXACTEvent_SetEffectData,
	eXACTEvent_Max

} XACT_TRACK_EVENT_TYPES;

typedef struct _XACT_TRACK_EVENT_MARKER {

	BYTE	bData[XACT_SIZEOF_MARKER_DATA];

} XACT_TRACK_EVENT_MARKER, *PXACT_TRACK_EVENT_MARKER, *LPXACT_TRACK_EVENT_MARKER;

typedef struct _XACT_TRACK_EVENT_SETEFFECTDATA {

    WORD	wEffectIndex;
    WORD	wOffset;    
	WORD	wDataSize;
	WORD    wReserved;
	DWORD   dwData[1];

} XACT_TRACK_EVENT_SETEFFECTDATA, *PXACT_TRACK_EVENT_SETEFFECTDATA, *LPXACT_TRACK_EVENT_SETEFFECTDATA;

typedef struct _XACT_TRACK_EVENT_SETFILTER {

    DSFILTERDESC Desc;
    
} XACT_TRACK_EVENT_SETFILTER, *PXACT_TRACK_EVENT_SETFILTER, *LPXACT_TRACK_EVENT_SETFILTER;


typedef struct _XACT_TRACK_EVENT_SETEG {

    DSENVELOPEDESC Desc;
    
} XACT_TRACK_EVENT_SETEG, *PXACT_TRACK_EVENT_SETEG, *LPXACT_TRACK_EVENT_SETEG;

typedef struct _XACT_TRACK_EVENT_SETLFO {

    DSLFODESC Desc;
    
} XACT_TRACK_EVENT_SETLFO, *PXACT_TRACK_EVENT_SETLFO, *LPXACT_TRACK_EVENT_SETLFO;

typedef struct _XACT_TRACK_EVENT_SETHEADROOM {

    WORD wHeadroom;
    
} XACT_TRACK_EVENT_SETHEADROOM, *PXACT_TRACK_EVENT_SETHEADROOM, *LPXACT_TRACK_EVENT_SETHEADROOM;

typedef struct _XACT_TRACK_EVENT_SETVOLUME {

    SHORT sVolume;
    
} XACT_TRACK_EVENT_SETVOLUME, *PXACT_TRACK_EVENT_SETVOLUME, *LPXACT_TRACK_EVENT_SETVOLUME;

typedef struct _XACT_TRACK_EVENT_SETMIXBINVOLUMES {

	DWORD	dwCount;
    XACTMIXBINVOLUMEPAIR aVolumePairs[8];
    
} XACT_TRACK_EVENT_SETMIXBINVOLUMES, *PXACT_TRACK_EVENT_SETMIXBINVOLUMES, *LPXACT_TRACK_EVENT_SETMIXBINVOLUMES;

typedef struct _XACT_TRACK_EVENT_SETFREQUENCY {

    WORD wFrequency;
    
} XACT_TRACK_EVENT_SETFREQUENCY, *PXACT_TRACK_EVENT_SETFREQUENCY, *LPXACT_TRACK_EVENT_SETFREQUENCY;
 
typedef struct _XACT_TRACK_EVENT_STOP {
    
    
} XACT_TRACK_EVENT_STOP, *PXACT_TRACK_EVENT_STOP, *LPXACT_TRACK_EVENT_STOP;

typedef union XACT_EVENT_PLAY_DESC {

    struct {
        WORD wWaveIndex;
        WORD wBankIndex;
    } WaveSource;

    struct {
        DWORD dwMixBin;
    } EffectSource;

} XACT_EVENT_PLAY_DESC, *PXACT_EVENT_PLAY_DESC, *LPXACT_EVENT_PLAY_DESC;
 
typedef struct _XACT_TRACK_EVENT_PLAY {

	XACT_EVENT_PLAY_DESC PlayDesc;

} XACT_TRACK_EVENT_PLAY, *PXACT_TRACK_EVENT_PLAY, *LPXACT_TRACK_EVENT_PLAY;

typedef struct _XACT_EVENT_PITCH_VOLUME_VAR_DESC {

	struct {
		SHORT sPitchLo;
		SHORT sPitchHi;
	} Pitch;

	struct {
		SHORT sVolLo;
		SHORT sVolHi;
	} Volume;

} XACT_EVENT_PITCH_VOLUME_VAR_DESC, *PXACT_EVENT_PITCH_VOLUME_VAR_DESC, *LPXACT_EVENT_PITCH_VOLUME_VAR_DESC;

typedef struct _XACT_TRACK_EVENT_PITCH_VOLUME_VAR {

    XACT_EVENT_PITCH_VOLUME_VAR_DESC VarDesc;

} XACT_TRACK_EVENT_PITCH_VOLUME_VAR, *PXACT_TRACK_EVENT_PITCH_VOLUME_VAR, *LPXACT_TRACK_EVENT_PITCH_VOLUME_VAR;

typedef struct _XACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR {

    XACT_EVENT_PLAY_DESC				PlayDesc;
	XACT_EVENT_PITCH_VOLUME_VAR_DESC	VarDesc;

} XACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR, *PXACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR, *LPXACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR;

typedef struct _XACT_TRACK_EVENT_LOOPSTART {

	WORD	wLoopCount;

} XACT_TRACK_EVENT_LOOPSTART, *PXACT_TRACK_EVENT_LOOPSTART, *LPXACT_TRACK_EVENT_LOOPSTART;

typedef struct _XACT_TRACK_EVENT_LOOPEND {

} XACT_TRACK_EVENT_LOOPEND, *PXACT_TRACK_EVENT_LOOPEND, *LPXACT_TRACK_EVENT_LOOPEND;

union XACT_TRACK_EVENT_UNION {
    XACT_TRACK_EVENT_PLAY							Play;
	XACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR		PlayWithPitchAndVolumeVariation;
    XACT_TRACK_EVENT_STOP							Stop;
	XACT_TRACK_EVENT_PITCH_VOLUME_VAR				PitchAndVolumeVariation;
	XACT_TRACK_EVENT_SETFREQUENCY					SetFrequency;
	XACT_TRACK_EVENT_SETVOLUME						SetVolume;
	XACT_TRACK_EVENT_SETHEADROOM					SetHeadroom;
	XACT_TRACK_EVENT_SETLFO							SetLFO;
	XACT_TRACK_EVENT_SETEG							SetEG;
	XACT_TRACK_EVENT_SETFILTER						SetFilter;
	XACT_TRACK_EVENT_SETEFFECTDATA					SetEffectData;
	XACT_TRACK_EVENT_MARKER							Marker;
	XACT_TRACK_EVENT_LOOPSTART						LoopStart;
	XACT_TRACK_EVENT_LOOPEND						LoopEnd;
	XACT_TRACK_EVENT_SETMIXBINVOLUMES				SetMixBinVolumes;
	
};

struct XACT_TRACK_EVENT {

    XACT_TRACK_EVENT_HEADER Header;
    XACT_TRACK_EVENT_UNION EventData;

};

//@@END_MSINTERNAL



#endif // __XACT_ENGINE INCLUDED__
