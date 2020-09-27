#if !defined(AFX_EFFECTINFO_H__F2CDDA70_482F_4CEB_9BCB_ED886E21599A__INCLUDED_)
#define AFX_EFFECTINFO_H__F2CDDA70_482F_4CEB_9BCB_ED886E21599A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffectInfo.h : header file
//

#include "PPGItemBase.h"

#define DMUSPROD_FOURCC_EFFECTINFO_LIST mmioFOURCC('e','f','c','l')
#define DMUS_FOURCC_UNAM_CHUNK			mmioFOURCC('U','N','A','M')
#define DMUSPROD_FOURCC_EFFECTINFO_HEADER mmioFOURCC('e','f','c','h')
#define DMUSPROD_FOURCC_EFFECTINFO_DATA mmioFOURCC('e','f','c','d')
#define DMUSPROD_FOURCC_EFFECTINFO_NAME mmioFOURCC('e','f','c','n')

#define CH_EFFECT_NAME		0x00000001
#define CH_EFFECT_FLAGS		0x00000002
#define CH_EFFECT_DATA		0x00000004
#define CH_EFFECT_SEND		0x00000008

/*
LIST
(
	'efcl-list' 	// List container for one effect
	[<UNAM-ck>] 	// Optional user-defined name
	<efch-ck>		// Effect header information
	[<efcd-ck>] 	// Optional effect data
	[<efcn-ck>] 	// Optional standard effect name
)
 
	// <efch-ck>			// Effect header chunk
	(
		'efch'
		<CLSID> 	// Effect's CLSID
	)
 
	// <efcd-ck>			// Effect data chunk
	(
		'efcd'
		<BYTE>...	// Effect-specific data
	)
 
	// <efcn-ck>			// Effect name chunk
	(
		'efcn'
		<WCHAR>...	// Effect name
	)
*/

interface IDMUSProdRIFFStream;
interface IStream;
interface IDMUSProdPropPageObject;
class ItemInfo;
struct ioDSoundEffect;

bool StreamsAreEqual( IStream *pIStream1, IStream *pIStream2 );

class EffectInfo
{
// Construction
public:
	EffectInfo();
	EffectInfo( CString strNewName, CString strNewInstanceName, REFCLSID rclsidNewObject, REFCLSID rclsidSendBuffer );
	~EffectInfo();

// Attributes
public:
	CString		m_strInstanceName;
	CString		m_strName;
	CLSID		m_clsidObject;
	CLSID		m_clsidSendBuffer;
	IStream		*m_pIStream;
	DWORD		m_dwFlags;

	ItemInfo	*m_pSendDestinationMixGroup;

// Operations
public:
	HRESULT Write( IDMUSProdRIFFStream *pIStream ) const;
	HRESULT Read( IStream *pIStream );
	HRESULT Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent );

	void Empty( void );

	void Copy( const EffectInfo &effectInfo );
	void Copy( const ioDSoundEffect *pioDSoundEffect );
	bool IsEqualExceptName( const EffectInfo *pEffectInfo ) const;
};

class EffectInfoForPPG : public PPGItemBase
{
// Construction
public:
	EffectInfoForPPG() : PPGItemBase()
	{
		ZeroMemory( &m_clsidObject, sizeof(CLSID) );
		ZeroMemory( &m_clsidSendBuffer, sizeof(CLSID) );
		ZeroMemory( &m_guidMyBuffer, sizeof(GUID) );
		m_pIStream = NULL;
		m_dwFlags = 0;
		m_ppgIndex = PPG_EFFECT;
	};
	virtual ~EffectInfoForPPG()
	{
		if( m_pIStream )
		{
			m_pIStream->Release();
			m_pIStream = NULL;
		}
	}

// Attributes
public:
	CString		m_strInstanceName;
	CString		m_strName;
	CLSID		m_clsidObject;
	CLSID		m_clsidSendBuffer;
	IStream		*m_pIStream;
	DWORD		m_dwFlags;
	GUID		m_guidMyBuffer;

// Operations
public:
	void Copy( const EffectInfoForPPG &effectInfoForPPG )
	{
		PPGItemBase::Copy( effectInfoForPPG );
		m_strInstanceName = effectInfoForPPG.m_strInstanceName;
		m_strName = effectInfoForPPG.m_strName;
		m_clsidObject = effectInfoForPPG.m_clsidObject;
		m_clsidSendBuffer = effectInfoForPPG.m_clsidSendBuffer;
		m_guidMyBuffer = effectInfoForPPG.m_guidMyBuffer;
		m_dwFlags = effectInfoForPPG.m_dwFlags;
		if( m_pIStream )
		{
			m_pIStream->Release();
			m_pIStream = NULL;
		}
		if( effectInfoForPPG.m_pIStream )
		{
			effectInfoForPPG.m_pIStream->Clone( &m_pIStream );
		}
	}
	void Import( const EffectInfo &effectInfo )
	{
		m_strInstanceName = effectInfo.m_strInstanceName;
		m_strName = effectInfo.m_strName;
		m_clsidObject = effectInfo.m_clsidObject;
		m_clsidSendBuffer = effectInfo.m_clsidSendBuffer;
		m_dwFlags = effectInfo.m_dwFlags;
		if( m_pIStream )
		{
			m_pIStream->Release();
			m_pIStream = NULL;
		}
		if( effectInfo.m_pIStream )
		{
			effectInfo.m_pIStream->Clone( &m_pIStream );
		}
	}
};

#endif // !defined(AFX_EFFECTINFO_H__F2CDDA70_482F_4CEB_9BCB_ED886E21599A__INCLUDED_)
