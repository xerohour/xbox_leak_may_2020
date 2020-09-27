#if !defined(AFX_BUFFERPPGMGR_H__F80ABA51_4947_4D98_B9AB_BA0550B4DE9D__INCLUDED_)
#define AFX_BUFFERPPGMGR_H__F80ABA51_4947_4D98_B9AB_BA0550B4DE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BufferPPGMgr.h : header file
//

#include <DMUSProd.h>
#include "DllBasePropPageManager.h"
#include "PPGItemBase.h"
#include "dsound.h"

// {1EE539BC-4328-4c19-8BA0-D146404D5159}
static const GUID GUID_BufferPPGMgr = 
{ 0x1ee539bc, 0x4328, 0x4c19, { 0x8b, 0xa0, 0xd1, 0x46, 0x40, 0x4d, 0x51, 0x59 } };

struct BufferOptions;
class CTabBuffer;

#define CH_BUFFER_STANDARDGUID	0x00000001
#define CH_BUFFER_HEADERFLAGS	0x00000002
#define CH_BUFFER_BUFFERFLAGS	0x00000004
#define CH_BUFFER_BUFFER3DALG	0x00000008
#define CH_BUFFER_BUFFER3DPOS	0x00000010
#define CH_BUFFER_BUFFERCHANNELS	0x00000020
#define CH_BUFFER_USERGUID		0x00000040
#define CH_BUFFER_BUFFERPAN		0x00000080
#define CH_BUFFER_BUFFERVOLUME	0x00000100

#define DESIGN_BUFFER_HAS_PCHANNELS	0x00000001

void InitializeDS3DBUFFER( DS3DBUFFER *pDS3DBUFFER );

struct BufferInfoForPPG : public PPGItemBase
{
	BufferInfoForPPG() : PPGItemBase()
	{
 		m_ppgIndex = PPG_BUFFER;
		m_dwHeaderFlags = 0;
		m_dwBufferFlags = 0;
		m_dwNumChannels = 0;
		m_dwDesignFlags = 0;
		m_dwMinNumChannels = 0;
		InitializeDS3DBUFFER( &m_ds3DBuffer );
		ZeroMemory( &m_guidBufferID, sizeof(GUID) );
		ZeroMemory( &m_clsid3dAlgorithm, sizeof(GUID) );
	}

	void Copy( const BufferInfoForPPG &bufferInfoForPPG );
	void Import( const BufferOptions *pBufferOptions, bool fItemHasPChannels );
 	DWORD		m_dwHeaderFlags;
	DWORD		m_dwBufferFlags;
	DWORD		m_dwNumChannels;
	DWORD		m_dwMinNumChannels; // minimum number of channels
	GUID		m_guidBufferID;
	CLSID		m_clsid3dAlgorithm;
	DS3DBUFFER	m_ds3DBuffer;  // 3D settings
	DWORD		m_dwDesignFlags;
	LONG		m_lVolume;
	LONG		m_lPan;
};

/////////////////////////////////////////////////////////////////////////////
// CBufferPPGMgr

class CBufferPPGMgr : public CDllBasePropPageManager
{
friend CTabBuffer;
public:
	CBufferPPGMgr();
	virtual ~CBufferPPGMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
	HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	CTabBuffer		*m_pTabBuffer;

public:
	static short	sm_nActiveTab;
};

#endif // !defined(AFX_BUFFERPPGMGR_H__F80ABA51_4947_4D98_B9AB_BA0550B4DE9D__INCLUDED_)
