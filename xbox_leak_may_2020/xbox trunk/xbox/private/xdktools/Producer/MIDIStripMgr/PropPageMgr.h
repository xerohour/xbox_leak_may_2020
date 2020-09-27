// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include "PropCurve.h"
#include "PropSwitchPoint.h"
#include "staticproppagemanager.h"

class PropPianoRoll;

//////////////////////////////////////////////////////////////////////
//  CNotePropPageMgr

class CNotePropPageMgr : public CStaticPropPageManager
{
friend class PropPageNote;
friend class PropPageNoteVar;

public:
	CNotePropPageMgr();
	~CNotePropPageMgr();

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
								LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

protected:
	PropPageNote*		m_pPropPageNote;
	PropPageNoteVar*	m_pPropPageNoteVar;

public:
	IDMUSProdFramework*	m_pDMUSProdFramework;
	static short		sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CCurvePropPageMgr

class CCurvePropPageMgr : public CStaticPropPageManager 
{
friend class CPropPageCurve;
friend class CPropPageCurveVar;
friend class CPropPageCurveReset;

public:
	CCurvePropPageMgr( REFGUID rguidManager );
	virtual ~CCurvePropPageMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
	CPropCurve			m_PropCurve;	// Used to relay changes to CurveStrip

	CPropPageCurve*		m_pTabCurve;
	CPropPageCurveVar*	m_pTabCurveVar;
	CPropPageCurveReset* m_pTabCurveReset;

public:
	static short		sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CMarkerPropPageMgr

class CMarkerPropPageMgr : public CStaticPropPageManager 
{
friend class CPropPageMarker;

public:
	CMarkerPropPageMgr( REFGUID rguidManager );
	virtual ~CMarkerPropPageMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
	CPropSwitchPoint	m_PropSwitchPoint;	// Used to relay changes to VarSwitchStrip

	CPropPageMarker*	m_pTabMarker;

public:
	static short		sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CPianoRollPropPageMgr

class CPianoRollPropPageMgr : public CStaticPropPageManager
{
friend class CPianoRollStrip;
public:
	CPianoRollPropPageMgr();
	~CPianoRollPropPageMgr();

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
								LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

	BOOL				m_fShowPatternPropPage;

protected:
	PropPianoRoll*		m_pPropPianoRoll;
	CTabPatternPattern*	m_pTabPatternPattern;
	class CGroupBitsPPG*m_pGroupBitsPPG;
	class CTrackFlagsPPG*m_pTrackFlagsPPG;

public:
	static short		sm_nActiveTab;
};


#endif // __PROPPAGEMGR_H_