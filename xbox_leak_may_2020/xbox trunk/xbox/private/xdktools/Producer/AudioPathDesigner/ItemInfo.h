#if !defined(AFX_ITEMINFO_H__808FB750_0DC9_4AE7_ABE6_56FCB72464A9__INCLUDED_)
#define AFX_ITEMINFO_H__808FB750_0DC9_4AE7_ABE6_56FCB72464A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemInfo.h : header file
//

#include "resource.h"
#include "DMUSProd.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "EffectInfo.h"

/////////////////////////////////////////////////////////////////////////////
// ItemInfo window

#define DEFAULT_NUM_VOICES 48

#define DMUSPROD_FOURCC_ITEMINFO_LIST	mmioFOURCC('i','t','m','l')
#define DMUSPROD_FOURCC_PCHANNEL_CHUNK	mmioFOURCC('p','c','h','n')

#define DMUSPROD_FOURCC_PORTSETTINGS_LIST mmioFOURCC('p','r','t','l')
#define DMUSPROD_FOURCC_PORTSETTINGS_CHUNK	mmioFOURCC('p','s','h','c')

#define DMUSPROD_FOURCC_ITEM_BUFFER_LIST mmioFOURCC('i','b','f','l') 
#define DMUSPROD_FOURCC_BUFFER_CHUNK	mmioFOURCC('i','b','s','l')
#define DMUSPROD_FOURCC_BUS_ID			mmioFOURCC('b','s','i','d')

/*
LIST
(
	'itml'			// List of item information
	[<UNAM-ck>] 	// Optional Mix Group name
	[<pchn-ck>] 	// Optional PChannel chunk
	<prtl-list> 	// Port settings list
	<ibsl-list> 	// List of buses
)

LIST
(
	'prtl'			// List container for one port configuration
	<pshc-ck>		// Port settings header
)

LIST
(
	'ibsl-list' 	// List containter for a number of buses
	<busl-list>...	// Any number of bus list chunks
)

LIST
(
	'busl-list' 	// List container for one bus
	<bush-ck>		// Bus config header chunk
	[<efcl-list>]...// Any number of effect list chunks
)

	// <pchn-ck>			// PChannel listing chunk
	(
		'pchn'
		<DWORD>...	// Array of PChannels.
	)
 
	// <pshc-ck>			// Port settings header chunk
	(
		'pshc'
		<ioPortOptions>   // Port settings
	)
 
	// <bush-ck>			// bus settings header chunk
	(
		'bush'
		<DWORD> 	// Bus ID
	)
*/

#define FOURCC_DMUS_EFFECT_DATA_CHUNK mmioFOURCC('d','a','t','a')
#define FOURCC_PORT_NAME_CHUNK mmioFOURCC('p','n','c','d')
#define FOURCC_EFFECT_NAME_CHUNK mmioFOURCC('e','n','c','d')

/*

ioPortConfig
* DMUS_IO_PORTCONFIG_HEADER
* DMUS_PORTPARAMS8
* Instance name
* Port name
* list of ioPChannelToBuffer
* list of ioDesignMixGroup (used for ports that don't connect to buffers)

ioPChannelToBuffer
* DMUS_IO_PCHANNELTOBUFFER_HEADER
* list of buffer GUIDs
* Name of mix group these PChannels are from

ioDesignMixGroup
* list of PChannels
* Name of mix group these PChannels are from

ioDSoundBuffer
* GUID
* Name
* DSOUND_IO_DSBUFFERDESC
* DSOUND_IO_DSBUSSID (list of DWORDs)
* 3d Parameters
* list of ioDSoundEffect

ioDSoundEffect
* DSOUND_IO_DXDMO_HEADER
* Instance name
* Effect name
* pointer to stream with effect data
* design-time GUID

*/

struct ioPChannelToBuffer
{
	ioPChannelToBuffer()
	{
		ZeroMemory( &ioPChannelToBufferHeader, sizeof(DMUS_IO_PCHANNELTOBUFFER_HEADER) );
	}
	~ioPChannelToBuffer()
	{
		while( !lstGuids.IsEmpty() )
		{
			delete lstGuids.RemoveHead();
		}
		while( !lstDesignGuids.IsEmpty() )
		{
			delete lstDesignGuids.RemoveHead();
		}
	}

	DMUS_IO_PCHANNELTOBUFFER_HEADER ioPChannelToBufferHeader;
	CTypedPtrList< CPtrList, GUID* > lstGuids;
	// List of Design GUIDS, with the buffer GUID, then the design GUID, then the buffer GUID, etc.
	CTypedPtrList< CPtrList, GUID* > lstDesignGuids;
	CString strMixGroupName;
};

struct ioDesignMixGroup
{
	DWORD dwPChannelStart;
	DWORD dwPChannelSpan;
	CString strMixGroupName;
};

struct ioPortConfig
{
	ioPortConfig()
	{
		ZeroMemory( &ioPortConfigHeader, sizeof(DMUS_IO_PORTCONFIG_HEADER) );
		ZeroMemory( &ioPortParams, sizeof(DMUS_PORTPARAMS8) );
	}
	~ioPortConfig()
	{
		while( !lstPChannelToBuffer.IsEmpty() )
		{
			delete lstPChannelToBuffer.RemoveHead();
		}
		while( !lstDesignMixGroup.IsEmpty() )
		{
			delete lstDesignMixGroup.RemoveHead();
		}
	}

	DMUS_IO_PORTCONFIG_HEADER ioPortConfigHeader;
	DMUS_PORTPARAMS8 ioPortParams;
	CString strPortName;
	CTypedPtrList< CPtrList, ioPChannelToBuffer* > lstPChannelToBuffer;
	CTypedPtrList< CPtrList, ioDesignMixGroup* > lstDesignMixGroup;
};

struct ioDSoundEffect
{
	ioDSoundEffect()
	{
		ZeroMemory( &ioFXHeader, sizeof(DSOUND_IO_DXDMO_HEADER) );
		pStreamData = NULL;
		ZeroMemory( &guidDesignGUID, sizeof(GUID) );
	}
	~ioDSoundEffect()
	{
		if( pStreamData )
		{
			pStreamData->Release();
			pStreamData = NULL;
		}
	}

	DSOUND_IO_DXDMO_HEADER ioFXHeader;
	IStream *pStreamData;
	CString strInstanceName;	// Also - other UNFO information?
	CString strEffectName;
	GUID guidDesignGUID;
};

void InitializeDS3DBUFFER( DS3DBUFFER *pDS3DBUFFER );

struct ioDSoundBuffer
{
	ioDSoundBuffer()
	{
		ZeroMemory( &ioBufferHeader, sizeof( DMUS_IO_BUFFER_ATTRIBUTES_HEADER ) );
		ZeroMemory( &ioDSBufferDesc, sizeof( DSOUND_IO_DSBUFFERDESC ) );
		ZeroMemory( &ioDS3D, sizeof( DSOUND_IO_3D ) );
		InitializeDS3DBUFFER( &ioDS3D.ds3d );
	}
	~ioDSoundBuffer()
	{
		while( !lstDSoundEffects.IsEmpty() )
		{
			delete lstDSoundEffects.RemoveHead();
		}
	}

	DMUS_IO_BUFFER_ATTRIBUTES_HEADER ioBufferHeader;
	CString strName;	// Also - other UNFO information?
	DSOUND_IO_DSBUFFERDESC ioDSBufferDesc;
	DSOUND_IO_3D ioDS3D;
	CDWordArray lstBusIDs;
	CTypedPtrList< CPtrList, ioDSoundEffect*> lstDSoundEffects;
};

/*
typedef struct 
{
	DSBUFFERDESC dsbd;
} DSOUND_IO_DSBUFFERDESC;

typedef struct 
{
	DWORD	busid[1];
} DSOUND_IO_DSBUSID;

typedef struct
{
	// data structure with presets for 3d
} DSOUND_IO_3D;
*/

interface IDMUSProdRIFFStream;
class CDirectMusicAudioPath;

struct ioPortOptions
{
	ioPortOptions()
	{
		ZeroMemory( &guidPort, sizeof(GUID) );
		dwEffects = 0;
		dwSupportedEffects = 0xFFFFFFFF;
		dwSampleRate = 0;
		dwVoices = DEFAULT_NUM_VOICES;
		dwMaxVoices = DEFAULT_NUM_VOICES;
		fAudioPath = TRUE;
	}

	GUID	guidPort; // GUID used to create the port
	DWORD	dwEffects; // Effects flags (DMUS_EFFECT_*)
	DWORD	dwSupportedEffects; // Supported effects flags (DMUS_EFFECT_*)
	DWORD	dwSampleRate; // Sample rate (11025, 22050, 44100, 48000)
	DWORD	dwVoices; // Number of voices (default = DEFAULT_NUM_VOICES)
	DWORD	dwMaxVoices; // Maximum number of voices (default = DEFAULT_NUM_VOICES)
	BOOL    fAudioPath; // Whether or not the port supports audio paths
};

struct PortOptions
{
	CString 			m_strName;
	GUID				m_guidPort; // GUID used to create the port
	DWORD				m_dwEffects; // Effects flags (DMUS_EFFECT_*)
	DWORD				m_dwSampleRate; // Sample rate (11000, 22000, 44000)
	DWORD				m_dwVoices; // Number of voices (default = DEFAULT_NUM_VOICES)
	DWORD				m_dwMaxVoices; // Maximum number of voices (default = DEFAULT_NUM_VOICES)
    bool				m_fAudioPath; // Whether or not the port supports audio paths
	DWORD				m_dwSupportedEffects; // Which effects are supported

	PortOptions()
	{
		Empty();
	};

	HRESULT Write( IDMUSProdRIFFStream *pIStream ) const;
	HRESULT Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent );

	void Empty( void );

	void Copy( const PortOptions *pPortOptions );
	bool IsEqual( const PortOptions *pPortOptions ) const;
};

typedef enum { BEI_NONE = 0, BEI_NORMAL = 1, BEI_GREY = 2 } BEI_SELECTION;

struct BusEffectInfo
{
	BusEffectInfo( EffectInfo *pNewEffectInfo )
	{
		if( pNewEffectInfo )
		{
			m_EffectInfo.Copy( *pNewEffectInfo );
		}
		m_beiSelection = BEI_NONE;
		m_lPixelWidth = 0;
		::CoCreateGuid( &m_guidEffectInfo );
	}

	BusEffectInfo( const BusEffectInfo *pBusEffectInfo )
	{
		m_EffectInfo.Copy( pBusEffectInfo->m_EffectInfo );
		m_beiSelection = pBusEffectInfo->m_beiSelection;
		m_lPixelWidth = pBusEffectInfo->m_lPixelWidth;
		::CoCreateGuid( &m_guidEffectInfo );
	}

	BusEffectInfo( const ioDSoundEffect *pioDSoundEffect )
	{
		ASSERT( pioDSoundEffect );
		if( pioDSoundEffect )
		{
			m_EffectInfo.Copy( pioDSoundEffect );
			m_guidEffectInfo = pioDSoundEffect->guidDesignGUID;
		}
		else
		{
			::CoCreateGuid( &m_guidEffectInfo );
		}
		m_beiSelection = BEI_NONE;
		m_lPixelWidth = 0;
	}

	EffectInfo m_EffectInfo;
	BEI_SELECTION	m_beiSelection;
	long		m_lPixelWidth;
	GUID		m_guidEffectInfo;
};

struct ioBufferOptions
{
	ioBufferOptions()
	{
		ZeroMemory( this, sizeof(ioBufferOptions) );
		InitializeDS3DBUFFER( &ds3DBuffer );
	}

	GUID	guidBuffer;		// Buffer's GUID
	GUID	guid3DAlgorithm;// DSBUFFERDESC::guid3DAlgorithm
	DWORD	dwHeaderFlags;  // DMUS_IO_BUFFER_ATTRIBUTES_HEADER::dwFlags
	DWORD	dwBufferFlags;	// DSBUFFERDESC::dwFlags
	WORD	wChannels;		// WAVEFORMATEX::nChannels (only used if number of Bus IDs is 0)
	WORD	wReserved;		// Padding bytes
	DS3DBUFFER	ds3DBuffer; // 3D settings
	LONG	lVolume;		// Initial pan; only used if CTRLVOLUME is specified
	LONG	lPan;			// Initial pan; only used if CTRLPAN is specified
};

struct BufferOptions
{
	CDWordArray lstBusIDs;
	GUID	guidBuffer;		// Buffer's GUID
	DWORD	dwHeaderFlags;  // DMUS_IO_BUFFER_ATTRIBUTES_HEADER::dwFlags
	DWORD	dwBufferFlags;	// DSBUFFERDESC::dwFlags
	GUID	guid3DAlgorithm;// DSBUFFERDESC::guid3DAlgorithm
	WORD	wChannels;		// WAVEFORMATEX::nChannels (only used if number of Bus IDs is 0)
	DS3DBUFFER	ds3DBuffer; // 3D settings
	LONG	lVolume;		// Initial pan; only used if CTRLVOLUME is specified
	LONG	lPan;			// Initial pan; only used if CTRLPAN is specified
	//CString strName;	// Name of buffer
	CTypedPtrList< CPtrList, BusEffectInfo *> lstEffects;
	bool	fSelected;		// Selection state of buffer and buses
	GUID	guidDesignEffectGUID;// Design-time GUID used when this is a standard buffer with an effect in it

	BufferOptions();
	~BufferOptions();

	void Empty( void );
	void Copy( const BufferOptions &bufferOptions );

	HRESULT Write( IDMUSProdRIFFStream *pIStream ) const;
	HRESULT Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent, CDirectMusicAudioPath *pAudioPath );
};

class ItemInfo
{
// Construction
public:
	ItemInfo();
	ItemInfo( DWORD dwPChannel );
	ItemInfo( LPCTSTR strText );
	~ItemInfo();

// Attributes
public:
	CString strBandName;
	int nNumPChannels;
	DWORD *adwPChannels;
	bool fSubFieldSelected;

	PortOptions *pPortOptions;

	CTypedPtrList< CPtrList, BufferOptions *> lstBuffers;

	DWORD dwDisplayIndex;

// Operations
public:
	HRESULT Write( IDMUSProdRIFFStream *pIStream, const ItemInfo *pParentInfo = NULL ) const;
	HRESULT Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent, CDirectMusicAudioPath *pAudioPath );
	bool Select( bool fNewSelectState ); // Returns true if something changed
	HRESULT ConvertToDMusic( CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs,
		CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers ) const;

	// Helper functions
	void Copy( const ItemInfo &itemInfo );
	bool IsAnythingSelected( void );

protected:
	void Empty( void );
};

#endif // !defined(AFX_ITEMINFO_H__808FB750_0DC9_4AE7_ABE6_56FCB72464A9__INCLUDED_)
