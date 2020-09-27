// TrackMgr.h : Declaration of the CTrackMgr

#ifndef __PARAM_TRACKMGR_H_
#define __PARAM_TRACKMGR_H_

class CTrackMgr;
class CParamStrip;
class CTrackItem;
class CDllJazzDataObject;
interface IDirectMusicTrack;

#include "resource.h"		// main symbols
#include "ParamStripMgr.h"
#include "TrackItem.h"
#include "selectedregion.h"
#include "BaseMgr.h"
#include "SegmentDesigner.h"
#include "SegmentGUIDs.h"
#include <conductor.h>
#include <dmusicf.h>
#include "TrackObject.h"


class CCurveTracker;

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define MAX_TICK (DMUS_PPQ << 1)

#ifndef REFCLOCKS_PER_MINUTE
#define REFCLOCKS_PER_MINUTE			600000000
#endif

#ifndef REFCLOCKS_PER_SECOND
#define REFCLOCKS_PER_SECOND			10000000
#endif

#ifndef REFCLOCKS_PER_MILLISECOND	
#define REFCLOCKS_PER_MILLISECOND		10000
#endif

#define INSERT_CURVE_START_VALUE		1

#define CRV_MINIMIZE_HEIGHT				20
#define CRV_DEFAULT_HEIGHT				80
#define CRV_MAXHEIGHT_HEIGHT			200
#define CURVE_MIN_SIZE					6
#define	MAX_DEFAULT_CURVE_VALUE			127

#define INSERT_CURVE_FALSE				0
#define INSERT_CURVE_START_VALUE		1
#define INSERT_CURVE_END_VALUE			2

#define MIN_NBR_GRIDS_PER_SECOND		1
#define MAX_NBR_GRIDS_PER_SECOND		100
#define DEFAULT_NBR_GRIDS_PER_SECOND	1

#define STRIP_MINIMIZE_HEIGHT			20
#define STRIP_MIN_HEIGHT				STRIP_MINIMIZE_HEIGHT
#define STRIP_MAX_HEIGHT				(STRIP_MINIMIZE_HEIGHT << 3)
#define STRIP_DEFAULT_HEIGHT			STRIP_MINIMIZE_HEIGHT

// Chunk ID used for strip's design data
#define DMUS_FOURCC_STRIP_UI_LIST       mmioFOURCC('s','p','l','u')
#define DMUS_FOURCC_STRIP_UI_CHUNK      mmioFOURCC('s','p','c','u')
#define DMUS_FOURCC_OBJECT_UI_CHUNK     mmioFOURCC('o','p','c','u')
#define DMUS_FOURCC_COPYPASTE_UI_CHUNK	mmioFOURCC('c','p','c','u')

// Strip State flags
#define STRIPUI_ACTIVESTRIP				0x00000001

#define TRACKCONFIG_VALID_MASK			DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_PLAY_CLOCKTIME
#define PARAMTRACK_DEFAULT				DMUS_TRACKCONFIG_PLAY_ENABLED
/////////////////////////////////////////////////////////////////////////////
// DirectMusicTimeSig structure

struct DirectMusicTimeSig
{
	// Time signatures define how many beats per measure, which note receives
	// the beat, and the grid resolution.
	DirectMusicTimeSig() : m_bBeatsPerMeasure(0), m_bBeat(0), m_wGridsPerBeat(0) { }
	BYTE	m_bBeatsPerMeasure;		// beats per measure (top of time sig)
	BYTE	m_bBeat;				// what note receives the beat (bottom of time sig.)
									// we can assume that 0 means 256th note
	WORD	m_wGridsPerBeat;		// grids per beat
};


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr
class ATL_NO_VTABLE CTrackMgr : public CBaseMgr, public CComCoClass<CTrackMgr, &CLSID_ParamMgr>,public IParamMgr
{

public:
	CTrackMgr();
	~CTrackMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_TRACKMGR)

BEGIN_COM_MAP(CTrackMgr)
	COM_INTERFACE_ENTRY_IID(IID_IParamMgr,IParamMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides

// IParamMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IPersist overrides

// IPersistStream overrides
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );

// IDMUSProdPropPageObject overrides
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

protected:
	// Overrides of CBaseMgr
	bool	RecomputeTimes();			 // Returns true if anything changed
	
	// Performs action on CTrackMgr
	void		InsertByAscendingPChannel(CTrackObject* pTrackObject);
	void		InsertInAlphabeticalOrder(CPtrList* plstObjects, CTrackObject* pTrackObject);
	CPtrList*	SortListAlphabetically(CPtrList* plstObjects);
	int			CompareObjectAlphabetical(CTrackObject* pObjectInList, CTrackObject* pTrackObject);



	// Performs action on all CTrackObjects
	void	AllObjects_OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData );
	void	AllObjects_RefreshInfo(); 
	void	AllObjects_Invalidate();
	bool	AllObjects_RecomputeTimes(); // Returns true if anything changed
	void	AllObjects_RecomputeMeasureBeats();
	void	AllObjects_AddAllStripsToTimeline();
	void	AllObjects_RemoveAllStripsFromTimeline();
	void	AllObjects_SwitchTimeBase();
	void	DeleteAllObjects();

	// Performs action on one CParamStrip
	HRESULT AddStripToTimeline( CParamStrip* pParamStrip, int nPosition );
	HRESULT RemoveStripFromTimeline( CParamStrip* pParamStrip );

public:
	
	// Add and delete objects from the track
	HRESULT AddNewObject();
	HRESULT AddNewObject(CTrackObject* pTrackObject, StripInfo* pStripInfo);
	HRESULT DeleteAllStripsInObject(CTrackObject* pTrackObject);

	// General Helpers
	IDMUSProdSegmentEdit8* GetSegment();

	HRESULT	RefTimeToMinSecGridMs(CParamStrip* pParamStrip, REFERENCE_TIME rtTime, long* plMinute, long* plSecond, long* plGrid, long* plMillisecond);
	HRESULT	MinSecGridMsToRefTime(CParamStrip* pParamStrip, long lMinute, long lSecond, long lGrid, long lMillisecond, REFERENCE_TIME* prtTime);
	HRESULT	ClocksToMeasureBeatGridTick(MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick);
	HRESULT	MeasureBeatGridTickToClocks(long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime);
	HRESULT RefTimeToUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );
	HRESULT ClocksToUnknownTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime );
	HRESULT	MeasureBeatGridTickToUnknownTime( long lMeasure, long lBeat, long lGrid, long lTick, REFERENCE_TIME* prtTime);

	
	HRESULT	UnknownTimeToMeasureBeatGridTick( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick );
	HRESULT UnknownTimeToClocks( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime );
	HRESULT UnknownTimeToRefTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );

	HRESULT GetPChannelName( DWORD dwPChannel, CString& strPChannelName );
	bool	PChannelExists( DWORD dwPChannel );
	DWORD	GetGreatestUsedPChannel();

	void	RemoveFromPropSheet(CParamStrip* pParamStrip);
	bool	IsRefTimeTrack();

	void	DeleteSelectedStripsFromObjects();
	

	void	NotifyDataChange(UINT nLastEdit); 

	// Performs action on CTrackMgr
	HRESULT	SyncWithDirectMusic( void ); // Returns S_OK if succeeded, S_FALSE if no track, E_* if failed.
	void	SetDirtyFlag( bool fDirty );
	void	TrackDataChanged();			// Dowa nothing but calles th


	HRESULT	ClocksToMeasureBeatTick( MUSIC_TIME mTime, long* plMeasure, long* plBeat, long* plTick );
	HRESULT	MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmTime );
	
	REFERENCE_TIME GetSegmentLength( void );

	HRESULT GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

	// Accessors / Mutators
	IDMUSProdPropPageManager* GetPropPageMgr();

	IDMUSProdNode*		GetDocRootNode();
	IDMUSProdFramework* GetFramework();
	IDMUSProdConductor* GetConductor();
	IDMUSProdTimeline*  GetTimeline();

	UINT				GetLastEdit();
	void				SetLastEdit(UINT nLastEdit);
	
	CParamStrip*		GetParamStripForPropSheet();
	void				SetParamStripForPropSheet(CParamStrip* pParamStrip, BOOL bSetToNULL = FALSE);


	CLSID				GetCLSID();
	DWORD				GetGroupBits();

	


protected:
	IDirectMusicPerformance* m_pIDMPerformance;
	CParamStrip*			 m_pParamStripForPropSheet;

	// Fields that are persisted (DirectMusic parameter control track data)
	CTypedPtrList<CPtrList, CTrackObject*> m_lstTrackObjects;
};




#endif //__PARAM_TRACKMGR_H_
