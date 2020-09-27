#ifndef __TOOL_H__
#define __TOOL_H__

// Tool.h : header file
//

#include "Graph.h"
#include "OlePropPage.h"
#include <RiffStrm.h>
#include <dmusici.h>


#define DEFAULT_TOOL_COLUMN_WIDTH		65


#pragma pack(2)

typedef struct ToolUI
{
	ToolUI()
	{
		nColumnWidth = DEFAULT_TOOL_COLUMN_WIDTH;
	}

	int			nColumnWidth;
} ToolUI;

#pragma pack()


//////////////////////////////////////////////////////////////////////
//  CToolPropPageManager

class CToolPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabTool;

public:
	CToolPropPageManager();
	virtual ~CToolPropPageManager();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
    DWORD						m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	
	CTabTool*					m_pTabTool;
};


//////////////////////////////////////////////////////////////////////
//  CTool

class CTool : public IDMUSProdPropPageObject, public IDMUSProdUpdateObjectNow
{
friend class CDirectMusicGraph;
friend class CGraphDlg;
friend class CTabTool;

public:
    CTool();
    CTool( CLSID clsidTool, IDirectMusicTool* pIDMTool );
	~CTool();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// IDMUSProdUpdateObjectNow functions
    HRESULT STDMETHODCALLTYPE UpdateObjectNow( LPUNKNOWN punkObject );

//Additional functions
private:
    HRESULT SaveHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveUIState( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT LoadUIState( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );

public:
	HRESULT SaveTool( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT LoadTool( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	void SetModified( BOOL fModified );

	void InsertPChannelGroup( CPChannelGroup* pPChannelGroupToInsert );
	void RemovePChannelGroup( CPChannelGroup* pPChannelGroupToRemove );
	void RemoveAllPChannelGroups();
	void CombineAllPChannelGroups();
	void RepositionPChannelGroup( CPChannelGroup* pPChannelGroupToReposition );
	bool ContainsPChannelGroup( CPChannelGroup* pPChannelGroup );

	int GetColumnWidth();
	void SetColumnWidth( int nWidth );
	void GetName( CString& strName );
	void GetCLSID( CLSID* pclsidTool );
	DWORD GetBitsUI();
	CDirectMusicGraph* GetGraph();

	HRESULT RefreshStreamOfData();
	bool IsStreamOfDataEqual( IStream* pIStream );

public:
	// UI
	BOOL				m_fSelected;

	// Persisted for UI
	ToolUI				m_ToolUI;
	CString				m_strName;

private:
    DWORD				m_dwRef;
	CDirectMusicGraph*	m_pGraph;

	// UI
	DWORD				m_dwBitsUI;

	// Persisted for DirectMusic
	CLSID				m_clsidTool;
	IDirectMusicTool*	m_pIDMTool;
	IStream*			m_pIStream;		// Tool's internal data 
	CTypedPtrList<CPtrList, CPChannelGroup*> m_lstPChannelGroups;
};

#endif // __TOOL_H__
