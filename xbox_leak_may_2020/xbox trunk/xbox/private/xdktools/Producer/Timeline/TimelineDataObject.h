#if !defined(AFX_TIMELINEDATAOBJECT_H__6442911A_2AFC_11D2_88F9_00C04FBF8D15__INCLUDED_)
#define AFX_TIMELINEDATAOBJECT_H__6442911A_2AFC_11D2_88F9_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimelineDataObject.h : header file
//

#include "timeline.h"
#include <afxtempl.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )

interface IStream;
interface IDMUSProdRIFFStream;

/////////////////////////////////////////////////////////////////////////////
// CClipboardStorage class

class CClipboardStorage
{
public:
	CClipboardStorage( UINT cfClipboardFormat, IStream* pStream )
	{
		ASSERT( pStream );
		m_pIStream = pStream;
		m_pIStream->AddRef();
		m_cfClipboardFormat = cfClipboardFormat;
	};

	virtual ~CClipboardStorage()
	{
		if( m_pIStream )
		{
			m_pIStream->Release();
		}
	}

	UINT		m_cfClipboardFormat;
	IStream*	m_pIStream;
};

/////////////////////////////////////////////////////////////////////////////
// CTimelineDataObject class

class CTimelineDataObject : public IDMUSProdTimelineDataObject
{
public:
	CTimelineDataObject();
	virtual ~CTimelineDataObject();

// IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

// IDMUSProdTimelineDataObject implementation
	STDMETHODIMP AddInternalClipFormat(
		/* [in] */ UINT				uClipFormat,
		/* [in] */ IStream*			pIStream );

	STDMETHODIMP AddExternalClipFormat(
		/* [in] */ UINT				uClipFormat,
		/* [in] */ IStream*			pIStream );

	STDMETHODIMP	IsClipFormatAvailable(
		/* [in] */ UINT				uClipFormat );

	STDMETHODIMP	AttemptRead(
		/* [in] */ UINT				uClipFormat,
		/* [out,retval] */ IStream**ppIStream );

	STDMETHODIMP GetBoundaries(
		/* [out] */ long*			plStartTime,
		/* [out] */ long*			plEndTime );

	STDMETHODIMP SetBoundaries(
		/* [in] */ long				lStartTime,
		/* [in] */ long				lEndTime );

	STDMETHODIMP Import(
		/* [in] */ IDataObject*		pIDataObject );

	STDMETHODIMP Export(
		/* [out,retval] */ IDataObject**ppIDataObject );

// Attributes
public:

// Protected operations
protected:
	CClipboardStorage*	FindClipboardStorage( UINT uClipFormat );
	HRESULT GetInternalListAsStream( IStream** ppIStream );
	HRESULT SaveBoundaries( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveInternalList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT LoadInternalListFromStream( IStream* pIStream );
	HRESULT BuildInternalList( IStream* pStream, IDMUSProdRIFFStream *pIRiffStream, MMCKINFO& ckParent );
	HRESULT ExtractItem( IStream* pStream, IDMUSProdRIFFStream *pIRiffStream, MMCKINFO& ckParent );


// Data
protected:
	long			m_cRef;

	CTypedPtrList<CPtrList, CClipboardStorage*> m_lstInternalClipboard;
	CTypedPtrList<CPtrList, CClipboardStorage*> m_lstExternalClipboard;

	CTypedPtrList<CPtrList, CClipboardStorage*> m_lstUsedInternalClipboard;
	CTypedPtrList<CPtrList, CClipboardStorage*> m_lstUsedExternalClipboard;

	long			m_lEnd;
	long			m_lStart;

	UINT			m_cfTimeline;
};

#endif // !defined(AFX_TIMELINEDATAOBJECT_H__6442911A_2AFC_11D2_88F9_00C04FBF8D15__INCLUDED_)
