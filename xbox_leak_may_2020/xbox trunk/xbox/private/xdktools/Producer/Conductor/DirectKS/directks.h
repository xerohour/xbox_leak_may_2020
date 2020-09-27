// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000
//
// Module Name:
//
//  kslib.h
//
// Abstract:
//  Header file for KsLib.lib and DirectKs.dll
//  This header contains class declarations for CKsFilter and CKsPin and their derived
//  classes.  The purpose of KsLib.lib and DirectKs.dll are to simplify the creation
//  of user mode KS clients.
//  The derived classes are audio specific, however classes for video capture, or other 
//  technologies that use KS/AvStream can be derived from CKsFilter and CKsPin.
//
// -------------------------------------------------------------------------------

//
// Abstract base class for KernelStreaming render & capture
//
#ifndef _KSLIB_H
#define _KSLIB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;

#include <windows.h>
#include <mmsystem.h>
#include <ks.h>
#include <ksmedia.h>
#include <setupapi.h>
#include "ilog.h"
#include "list.h"

typedef struct
{
    KSPIN_CONNECT               Connect;
    KSDATAFORMAT_WAVEFORMATEX   Format;
} KSPIN_CONNECT_WFX, *PKSPIN_CONNECT_WFX;

typedef struct 
{
    KSPROPERTY      Property;
    ULONG           DeviceId;
} KSP_SYSAUDIO, *PKSP_SYSAUDIO;


typedef enum {eUnknown, eTopology, ePCMAudio, eMidi, eDMusic, eSysAudioDevice, eSysAudio} ETechnology;
typedef enum {render, capture} STACKWISE_DATAFLOW;

// forward ho
class CKsPin;
class CKsNode;
class CKsConnection;

// ----------------------------------------------------------------------------------
// CKsIrpTarget
//  This is a base class for controlling Ks file objects, i.e. filters and pins
//  CKsFilter and CKsPin derive from this class
// ----------------------------------------------------------------------------------
class CKsIrpTarget
{
public:
    HANDLE              m_handle;           // Allocated handle of the filter instance
    DWORD               m_dwDevInst;        // Device Instance Handle.
    LPARAM              m_UserDef;          // use this for whatever you want...

    CKsIrpTarget(HANDLE handle) : m_handle(INVALID_HANDLE_VALUE) { UNREFERENCED_PARAMETER(handle);return; }

    // get ----------
    BOOL
    GetPropertySimple
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    GetPropertyMulti
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
    );

    BOOL
    GetPropertyMulti
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        IN  PVOID               pvData,
        IN  ULONG               cbData,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
    );

    BOOL
    GetNodePropertyChannel
    (
        IN  ULONG               nChannel,
        IN  ULONG               nNodeID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    GetNodePropertySimple
    (
        IN  ULONG               nNodeID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nPropertySet,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    GetNodePropertyMulti
    (
        IN  ULONG               nNodeID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem    OPTIONAL
    );

    // set ----------
    BOOL
    SetPropertySimple
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    SetPropertyMulti
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
    );

    BOOL
    SetNodePropertyChannel
    (
        IN  ULONG               nChannel,
        IN  ULONG               nNodeID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    SetNodePropertyMulti
    (
        IN  ULONG               nNodeID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem    OPTIONAL
    );

    BOOL
    SetNodePropertySimple
    (
        IN  ULONG               nNodeID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    HANDLE  GetHandle(void)     { return m_handle; }
};

typedef CKsIrpTarget *          PCKsIrpTarget;

// ----------------------------------------------------------------------------------
// CKsFilter
//  This is the base class for classes that proxy Ks filters from user mode.
//  Basic usage is 
//      instantiate a CKsFilter (or derived class)
//      call Instantiate, which creates a file object (instantiates the KS filter)
//                  whose handle is stored as m_handle
//      call EnumeratePins, EnumerateNodes, EnumerateConnections to deduce the filter's
//                  topology
//      call CKsIrpTarget functions to get/set properties
// ----------------------------------------------------------------------------------
class CKsFilter : public CKsIrpTarget
{
public:
	TCHAR               m_szFilterName[MAX_PATH];       // Filter path
	TCHAR               m_szFriendlyName[MAX_PATH];	    // Friendly name of the filter
	TCHAR               m_szCLSID[MAX_PATH];	        // Class ID of the filter
	TCHAR               m_szService[MAX_PATH];	        // Name of service, e.g. "kmixer"
	TCHAR               m_szBinary[MAX_PATH];	        // Name of Binary, e.g. "kmixer.sys"

    CList<CKsPin>       m_listPins;

    CList<CKsPin>       m_listRenderSinkPins;
    CList<CKsPin>       m_listRenderSourcePins;
    CList<CKsPin>       m_listCaptureSinkPins;
    CList<CKsPin>       m_listCaptureSourcePins;

    CList<CKsNode>      m_listNodes;

    CList<CKsConnection> m_listConnections;

    CKsPin*             m_pRenderPin;
    CKsPin*             m_pCapturePin;

    ETechnology         m_eType;

//    KSCOMPONENTID       m_ksComponentID;

public:
    CKsFilter();
    CKsFilter
	(
		CKsFilter* pksfCopy
	);
    CKsFilter
	(
		LPCTSTR  pszName,
		LPCTSTR  pszFriendlyName,
		LPCTSTR  pszCLSID,
		LPCTSTR  pszService,
		LPCTSTR  pszBinary
	);
    virtual __stdcall ~CKsFilter(void);

    virtual void    DestroyLists(void);
    BOOL    Instantiate(void);
    BOOL    EnumerateConnections(void);
    BOOL    EnumerateNodes(void);
    virtual BOOL    EnumeratePins(void);

    BOOL
    GetPinPropertySimple
    (
        IN  ULONG               nPinID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    GetPinPropertyMulti
    (
        IN  ULONG               nPinID,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem    OPTIONAL
    );

protected:
    BOOL
    ClassifyPins(CList <CKsPin> *plistPins);
};

typedef CKsFilter *             PCKsFilter;

typedef struct 
{
    ULONG                       cInterfaces;
    PKSIDENTIFIER               pInterfaces;
    ULONG                       cMediums;
    PKSIDENTIFIER               pMediums;
    ULONG                       cDataRanges;
    PKSDATARANGE                pDataRanges;
    ULONG                       cConstrDataRanges;
    PKSDATARANGE                pConstrDataRanges;
    KSPIN_DATAFLOW              DataFlow;
    KSPIN_COMMUNICATION         Communication;
    KSPIN_CINSTANCES            CInstances;
    KSPIN_CINSTANCES            CInstancesGlobal;
    KSPIN_CINSTANCES            CInstancesNecessary;
    PKSPIN_PHYSICALCONNECTION   PhysicalConnection;
    GUID                        Category;
    PWCHAR                      Name;
} 
PIN_DESCRIPTOR, *PPIN_DESCRIPTOR;

// ----------------------------------------------------------------------------------
// CKsConnection
// ----------------------------------------------------------------------------------
class CKsConnection
{
public:
    CKsConnection();
    CKsConnection(PKSTOPOLOGY_CONNECTION pConnection);
    CKsConnection(CKsConnection * pksfCopy);

    void  Initialize(PKSTOPOLOGY_CONNECTION pConnection);

    ULONG FromNode();
    ULONG FromNodePin();
    ULONG ToNode();
    ULONG ToNodePin();

private:
    KSTOPOLOGY_CONNECTION       m_Connection;
};
typedef CKsConnection *         PCKsConnection;

// ----------------------------------------------------------------------------------
// CKsPin
//  This is the base class for classes that proxy Ks pins from user mode.
//  Basic usage is 
//      CKsFilter::EnumeratePins builds a list of CKsPins
//      look at the PIN_DESCRIPTORs to figure out which pins meet your needs
//      call Instantiate, which creates a file object (instantiates the KS pin)
//                  whose handle is stored as m_handle
//      call CKsIrpTarget functions to get/set properties
//      call WriteData
// ----------------------------------------------------------------------------------
class CKsPin : public CKsIrpTarget
{
public:
    CKsFilter*          m_pFilter;
    ULONG               m_nId;

    PIN_DESCRIPTOR      m_Descriptor;             // description of pin

    KSSTATE             m_ksState;                // state of pin
    PKSPIN_CONNECT      m_pksPinCreate;           // creation parameters of pin
    PKSDATAFORMAT       m_pksDataFormat;          // description of data
    KSSTREAM_HEADER     m_ksStreamHeader[10];
    ULONG               m_cFramesUsed;
    PBYTE               m_pbStreamData;
    DWORD               m_cbStreamData;
	TCHAR               m_szFriendlyName[MAX_PATH];   // Friendly name of the pin

    STACKWISE_DATAFLOW  m_nStackwiseFlow;

    OVERLAPPED          m_ov[10];

    BOOL                m_fLooped;
    ETechnology         m_eType;

protected:
    DWORD               m_dwAlignment;

public:
    CKsPin(CKsFilter* pFilter, CKsPin* pkspCopy);
    CKsPin(CKsFilter* pFilter, ULONG nId);
    virtual ~CKsPin(void);

    BOOL    Instantiate(BOOL fLooped = FALSE );
    void    ClosePin(void);

    BOOL    Wait(DWORD dwMilliseconds, ULONG nFrame);
    BOOL    Wait(DWORD dwMilliseconds);

    BOOL    IsSignaled(ULONG nFrame);
    BOOL    IsSignaled(void);

    OVERLAPPED      &GetOverlapped(UINT unIndex = 0);
    KSSTREAM_HEADER &GetStreamHeader(UINT unIndex = 0);

    BOOL    GetState(PKSSTATE pksState);
    BOOL    SetState(KSSTATE ksState);
    BOOL    Reset(void);

    BOOL    WriteData(ULONG nFrame, BOOL fPause);
    BOOL    ReadData(ULONG nFrame);

    BOOL    WriteData(void);
    BOOL    ReadData(void);
};

typedef CKsPin *            PCKsPin;

/*
typedef struct
{
    REFGUID     guidPropertySet;
    ULONGLONG   ullPropertySupportMaskGet;
    ULONGLONG   ullPropertySupportMaskSet;
} SUPPORTED_PROPERTIES, *SUPPORTED_PROPERTIES;
*/

// ----------------------------------------------------------------------------------
// CKsNode
//  This class wraps an irptarget and associated node id.  It simplifies property
//  calls on nodes.
// ----------------------------------------------------------------------------------
class CKsNode
{
public:
    CKsNode(CKsIrpTarget* pTarget, ULONG nID);
    CKsNode(CKsIrpTarget* pTarget, CKsNode* pksnCopy);

    CKsIrpTarget*   m_pIrpTarget;
    ULONG           m_nId;

    ULONGLONG       m_ullSuptdPropsMaskGet;
    ULONGLONG       m_ullSuptdPropsMaskSet;

    GUID            m_guidType;

    BOOL
    GetNodePropertyChannel
    (
        IN  ULONG               nChannel,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    GetNodePropertyMulti
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem    OPTIONAL
    );

    BOOL
    SetNodePropertyChannel
    (
        IN  ULONG               nChannel,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );

    BOOL
    SetNodePropertyMulti
    (
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem    OPTIONAL
    );
};

typedef CKsNode *               PCKsNode;

// forward Ho
class CPCMAudioPin;

// ----------------------------------------------------------------------------------
// CTopologyFilter
// ----------------------------------------------------------------------------------
class CTopologyFilter : public CKsFilter
{
public:
    CTopologyFilter
	(
		LPCTSTR  pszName,
		LPCTSTR  pszFriendlyName,
		LPCTSTR  pszCLSID,
		LPCTSTR  pszService,
		LPCTSTR  pszBinary
    );
};

typedef CTopologyFilter *       PCTopologyFilter;

// ----------------------------------------------------------------------------------
// CPCMAudioFilter
// ----------------------------------------------------------------------------------
class CPCMAudioFilter : public CKsFilter
{
public:
    __stdcall CPCMAudioFilter
	(
		LPCTSTR  pszName,
		LPCTSTR  pszFriendlyName,
		LPCTSTR  pszCLSID,
		LPCTSTR  pszService,
		LPCTSTR  pszBinary
    );

    virtual BOOL    EnumeratePins(void);
    CPCMAudioPin*   __stdcall FindViablePin(CList<CKsPin>* pList, WAVEFORMATEX* pwfx);
    BOOL            CreateCapturePin(WAVEFORMATEX* pwfx, DWORD dwBufferSeconds, DWORD dwAlignment = 0);
    BOOL            CreateRenderPin(WAVEFORMATEX* pwfx, DWORD dwBufferSeconds, DWORD dwAlignment = 0);
};

typedef CPCMAudioFilter *       PCPCMAudioFilter;

// ----------------------------------------------------------------------------------
// CSysAudioDeviceFilter
// ----------------------------------------------------------------------------------
class CSysAudioDeviceFilter : public CKsFilter
{
public:
    ULONG   m_DeviceID;

    CSysAudioDeviceFilter
	(
		LPCTSTR  pszName,
		LPCTSTR  pszFriendlyName,
		LPCTSTR  pszCLSID,
		LPCTSTR  pszService,
		LPCTSTR  pszBinary
    );

    CSysAudioDeviceFilter
	(
		LPCTSTR  pszName,
		LPCTSTR  pszFriendlyName
    );
};

typedef CSysAudioDeviceFilter * PCSysAudioDeviceFilter;

// ----------------------------------------------------------------------------------
// CSysAudio
// ----------------------------------------------------------------------------------
class CSysAudio : public CKsFilter
{
public:
    CList <CSysAudioDeviceFilter>   m_listDevices;

    ULONG                           m_ulDeviceCount;
    
    CSysAudio
	(
		LPCTSTR  pszName,
		LPCTSTR  pszFriendlyName,
		LPCTSTR  pszCLSID,
		LPCTSTR  pszService,
		LPCTSTR  pszBinary
    );
    virtual ~CSysAudio();

    virtual void    DestroyLists(void);
    virtual BOOL    EnumeratePins(void);

    BOOL
    GetDevicePropertySimple
    (
        IN  ULONG               ulDeviceId,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );
    BOOL
    SetDevicePropertySimple
    (
        IN  ULONG               ulDeviceId,
        IN  REFGUID             guidPropertySet,
        IN  ULONG               nProperty,
        OUT PVOID               pvDest,
        OUT ULONG               cbDest
    );
};

typedef CSysAudio * PCSysAudio;

// ------------------------------------------------------------------------------
// the following FMT_XXX flags are used to simplify the caps reported by KSDATARANGE_AUDIO
// They are OR'd and stored in CPCMAudioPin::m_dwWaveFormatMask, which is set in the call 
// to KslEnumFilters if ePCMAudio is specified for the filter type
#define FMT_8BIT           0x00000001
#define FMT_16BIT          0x00000002
#define FMT_20BIT          0x00000004
#define FMT_24BIT          0x00000008
#define FMT_32BIT          0x00000010
#define FMTM_BITDEPTH      0x000000FF

#define FMT_8000HZ         0x00000100
#define FMT_11025HZ        0x00000200
#define FMT_16000HZ        0x00000400
#define FMT_22050HZ        0x00000800
#define FMT_32000HZ        0x00001000
#define FMT_44100HZ        0x00002000
#define FMT_48000HZ        0x00004000
#define FMT_96000HZ        0x00008000
#define FMTM_SAMPLERATE    0x0000FF00

#define FMT_MONO           0x00010000
#define FMT_STEREO         0x00020000
#define FMT_QUAD           0x00040000
#define FMT_SURROUND       0x00080000
#define FMT_5_1            0x00100000
#define FMT_7_1            0x00200000
#define FMTM_SPEAKCONFIG   0x00FF0000

// ----------------------------------------------------------------------------------
// CPCMAudioPin
// ----------------------------------------------------------------------------------
class CPCMAudioPin : public CKsPin
{
public:
    KSAUDIO_POSITION    m_nPosition;
    DWORD               m_dwWaveFormatMask;       // ORed WAVEFORMAT_XXX flags above
	DWORD				m_dwMinSampleFreq;
	DWORD				m_dwMaxSampleFreq;

    CPCMAudioPin(CKsFilter* pFilter, ULONG nId);
    CPCMAudioPin(CKsPin* pPin);     // copy constructor

    BOOL    Initialize(CKsFilter* pFilter, ULONG nID, KSPIN_DATAFLOW nDataFlow, DWORD dwFormatFlags);
    BOOL    SetFormat(WAVEFORMATEX* pwfx);
    BOOL    CreateDataBuffer(DWORD dwBufferSeconds, DWORD dwAlignment = 0);
    BOOL    CreateDataBufferMS(DWORD dwBufferMilliSeconds, DWORD dwAlignment = 0);

    BOOL    GetPosition(KSAUDIO_POSITION* Pos);
    BOOL    SetPosition(KSAUDIO_POSITION* Pos);
};

typedef CPCMAudioPin *          PCPCMAudioPin;

// ----------------------------------------------------------------------------------
// CMidiMusicPin
// ----------------------------------------------------------------------------------
class CMidiMusicPin : public CKsPin
{
public:
    CMidiMusicPin(CKsFilter* pFilter, ULONG nId);
    virtual ~CMidiMusicPin(void);

    BOOL    Initialize(CKsFilter* pFilter, ULONG nID, KSPIN_DATAFLOW nDataFlow, DWORD dwFormatFlags);
    BOOL    CreateDataBuffer(DWORD dwBufferSeconds);
};

typedef CMidiMusicPin *         PCMiniMusicPin;

// ----------------------------------------------------------------------------------
// CDirectMusicPin
// ----------------------------------------------------------------------------------
class CDirectMusicPin : public CKsPin
{
public:
    CDirectMusicPin(CKsFilter* pFilter, ULONG nId);

    BOOL    Initialize(CKsFilter* pFilter, ULONG nID, KSPIN_DATAFLOW nDataFlow, DWORD dwFormatFlags);
    BOOL    CreateDataBuffer(DWORD dwBufferSeconds);
};

typedef CDirectMusicPin *       PCDirectMusicPin;

// ==================================================================================
// these are the APIs exposed by DirectKs
// ==================================================================================

// ------------------------------------------------------------------------------
// Initializes SetupAPI and KsUser
// Call this before KslEnumFilters
// ------------------------------------------------------------------------------
BOOL
KslInitKsLib();

// ------------------------------------------------------------------------------
// Closes SetupAPI and KsUser
// Call when you are done using DirectKs/KsLib
// ------------------------------------------------------------------------------
BOOL
KslCloseKsLib();

// ------------------------------------------------------------------------------
// KslEnumFilters
//   Enumerates available KS filters of type <eFilterType>
// 
//   Also instantiates, and enumerates pins and nodes on each filter if specified.
//   You can instantiate the filter later by calling Instantiate method on the filter.
//   You can also enumerate pins and nodes later, but it is difficult (you need to 
// call the appropriate KSPROPSETID_Pin and KSPROPSETID_Topology properties).  It 
// is therefore, highly recommended that you use this API for that.
// ------------------------------------------------------------------------------
BOOL
KslEnumFilters
(   
    IN      CList<CKsFilter>*   plistFilters,       // pass a ptr to a CList to be populated
    IN      ETechnology         eFilterType,        // populate the list with filters of this type.  Implies that some filters are screened.
    IN OUT  LPGUID*             argpguidCategories, // pass in an array of LPGUIDs that specify the KSCATEGORYs to look for.  Members of this array are ORed.
    IN      ULONG               cguidCategories,    // number of elements in argpguidCategories
    IN      BOOL                fNeedPins = TRUE,   // TRUE -> each CKsFilter::m_listPins will be populated
    IN      BOOL                fNeedNodes = TRUE,  // TRUE -> each CKsFilter::m_listNodes will be populated
    IN      BOOL                fInstantiate = TRUE // TRUE -> filter will be left instantiated
);

// ------------------------------------------------------------------------------
// KslLogEnumResults
// Displays a list of filters using the registered ILog interface (specified by KslRegisterLog)
// ------------------------------------------------------------------------------
void
KslLogEnumResults
(
    CList<CKsFilter>*   plistFilters,       // list to display
    CKsFilter*          pFilterDefault      // highlight this filter
);

// ------------------------------------------------------------------------------
// KslregisterLog
// Specify the ILog-derived interface to use for KslLogenumResuls and in error messages.
// ------------------------------------------------------------------------------
void
KslRegisterLog(ILog* iLog);

// ------------------------------------------------------------------------------
// SyncIoctl
// DeviceIoControl wrapped with OVERLAPPED structure and logging
// ------------------------------------------------------------------------------
BOOL
SyncIoctl
(
    IN      HANDLE  handle,
    IN      ULONG   ulIoctl,
    IN      PVOID   pvInBuffer,
    IN      ULONG   ulInSize,
    OUT     PVOID   pvOutBuffer = NULL,
    IN      ULONG   ulOutSize = 0,
    OUT     PULONG  pulBytesReturned = NULL
);

#endif


