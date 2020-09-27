#ifndef __PCHANNELPROPPAGEMGR_H__
#define __PCHANNELPROPPAGEMGR_H__

//////////////////////////////////////////////////////////////////////
//  CBandPropPageManager

// {86E67810-2736-11d2-9A65-006097B01078}
static const GUID GUID_PChannelPropPageManager = 
{ 0x86e67810, 0x2736, 0x11d2, { 0x9a, 0x65, 0x0, 0x60, 0x97, 0xb0, 0x10, 0x78 } };

class CPChannelPropPageManager : public CDllBasePropPageManager 
{
friend class CPChannelPropPage;

public:
	CPChannelPropPageManager();
	virtual ~CPChannelPropPageManager();

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
	CPChannelPropPage*	m_pPChannelTab;
};

#endif // __PCHANNELPROPPAGEMGR_H__