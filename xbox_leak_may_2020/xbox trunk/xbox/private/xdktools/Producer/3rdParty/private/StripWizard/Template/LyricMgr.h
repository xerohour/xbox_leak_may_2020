// $$Safe_root$$Mgr.h : Declaration of the C$$Safe_root$$Mgr

#ifndef __$$SAFE_ROOT$$MGR_H_
#define __$$SAFE_ROOT$$MGR_H_

#include "resource.h"		// main symbols
#include "$$Safe_root$$StripMgr.h"
#include "$$Safe_root$$Item.h"
#include "selectedregion.h"
#include "BaseMgr.h"

#define FOURCC_$$SAFE_ROOT$$_LIST		mmioFOURCC('l','y','r','l')
#define FOURCC_$$SAFE_ROOT$$_ITEM		mmioFOURCC('l','y','r','c')

class C$$Safe_root$$Strip;
class C$$Safe_root$$Item;
class CDllJazzDataObject;
interface IDirectMusicTrack;

void	Empty$$Safe_root$$List( CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list );
HRESULT Load$$Safe_root$$List( CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, IStream* pIStream);
HRESULT Save$$Safe_root$$List(	CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, IStream* pIStream);
void Normalize$$Safe_root$$List( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, long lBeatOffset );
HRESULT GetBoundariesOf$$Safe_root$$s( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long & lStartTime, long & lEndTime, CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list);

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr
class ATL_NO_VTABLE C$$Safe_root$$Mgr : 
	public CBaseMgr,
	public CComCoClass<C$$Safe_root$$Mgr, &CLSID_$$Safe_root$$Mgr>,
	public I$$Safe_root$$Mgr
{
friend C$$Safe_root$$Strip;

public:
	C$$Safe_root$$Mgr();
	~C$$Safe_root$$Mgr();

DECLARE_REGISTRY_RESOURCEID(IDR_$$SAFE_ROOT$$MGR)

BEGIN_COM_MAP(C$$Safe_root$$Mgr)
	COM_INTERFACE_ENTRY(I$$Safe_root$$Mgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides

// I$$Safe_root$$Mgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );

// IPersist overrides

// IPersistStream overrides
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );

// IDMUSProdPropPageObject overrides
	HRESULT STDMETHODCALLTYPE GetData( void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);

protected:
	// Overrides of CBaseMgr
	bool	RecomputeTimes(); // Returns true if anything changed
	bool	SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime);
	bool	IsSelected(); // Returns true if any items are selected
	bool	IsEmpty(); // Returns false if there are any items
	
	void DeleteSelected$$Safe_root$$s();
	HRESULT SaveSelected$$Safe_root$$s(LPSTREAM, C$$Safe_root$$Item* p$$Safe_root$$AtDragPoint);
	HRESULT	SaveSelected$$Safe_root$$s(IStream* pStream, long lBeatOffset);

// general helpers
	void	MarkSelected$$Safe_root$$s(DWORD flags);
	void	UnMark$$Safe_root$$s(DWORD flags);
	void	DeleteMarked(DWORD flags);
	void	UnselectAll();
	void	SelectAll();
	bool	RemoveItem( C$$Safe_root$$Item* pItem );
	void	InsertByAscendingTime(C$$Safe_root$$Item *p$$Safe_root$$);
	bool	GetNextGreatestUniqueTime( long lMeasure, long lBeat, MUSIC_TIME* pmtTime );
	C$$Safe_root$$Item* FirstSelected$$Safe_root$$();
	bool	DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd );

protected:
	CTypedPtrList<CPtrList, C$$Safe_root$$Item*> m_lst$$Safe_root$$s;
	C$$Safe_root$$Item					m_Selected$$Safe_root$$Item;

	C$$Safe_root$$Strip*				m_p$$Safe_root$$Strip;

};


class C$$Safe_root$$Strip :
	public CBaseStrip
{
friend C$$Safe_root$$Mgr;

public:
	C$$Safe_root$$Strip( C$$Safe_root$$Mgr* p$$Safe_root$$Mgr );
	~C$$Safe_root$$Strip();

public:
// IDMUSProdStrip overrides
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar overrides

// IDMUSProdTimelineEdit overrides
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );

// IDropSource overrides

// IDropTarget overrides
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// IDMUSProdPropPageObject functions

protected:
	void		UnselectGutterRange( void );

// IDropTarget helpers
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lXPos, bool bDropNotPaste, BOOL &fChanged );

// IDropSource helpers
	HRESULT CreateDataObject(IDataObject**, long position);
	HRESULT	DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos);

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);

	C$$Safe_root$$Mgr*			m_p$$Safe_root$$Mgr;

	C$$Safe_root$$Item* Get$$Safe_root$$FromPoint( long lPos );
	C$$Safe_root$$Item* Get$$Safe_root$$FromMeasureBeat( long lMeasure, long lBeat );
	bool SelectItemsInSelectedRegions();
	void SelectRegionsFromSelected$$Safe_root$$s();	

	UINT		m_cf$$Safe_root$$List;			// Clipboard format
	bool		m_fLeftMouseDown;
	C$$Safe_root$$Item*	m_p$$Safe_root$$ItemToToggle;
};

inline void CListSelectedRegion_AddRegion(CListSelectedRegion& csrList, C$$Safe_root$$Item& item)
{
	CMusicTimeConverter cmtBeg(item.m_lMeasure, item.m_lBeat, csrList.Timeline(), csrList.GroupBits());
	CMusicTimeConverter cmtEnd = cmtBeg;
	cmtEnd.AddOffset(0,1, csrList.Timeline(), csrList.GroupBits());
	CSelectedRegion* psr = new CSelectedRegion(cmtBeg, cmtEnd);
	csrList.AddHead(psr);
}


#endif //__$$SAFE_ROOT$$MGR_H_
