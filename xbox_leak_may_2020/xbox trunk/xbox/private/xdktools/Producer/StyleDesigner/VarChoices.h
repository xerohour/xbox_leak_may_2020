#ifndef __VARCHOICES_H__
#define __VARCHOICES_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// VarChoices.h : header file
//

#include "RiffStrm.h"
#include "VarChoicesCtl.h"
#include "TabVarChoices.h"
#include "Pattern.h"
#include "StyleDesigner.h"
#include "SharedPattern.h"

interface IDMUSProdTimelineCallback;


//////////////////////////////////////////////////////////////////////
//  CVarChoicesPropPageManager

class CVarChoicesPropPageManager : public CDllBasePropPageManager 
{
friend class CTabVarChoices;

public:
	CVarChoicesPropPageManager();
	virtual ~CVarChoicesPropPageManager();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
	CTabVarChoices*		m_pTabVarChoices;
};


//////////////////////////////////////////////////////////////////////
//  CVarChoices

class CVarChoices : public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject, public IVarChoices
{
friend class CVarChoicesCtrl;
friend class CVarChoicesDlg;
friend class CVarChoicesFlagsBtn;
friend class CVarChoicesRowsBtn;
friend class CVarChoicesPropPageManager;
friend class CTabVarChoices;

public:
    CVarChoices();
	~CVarChoices();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode** ppIFirstChildNode );
    HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode );

    HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
    HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
    HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
    HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
    HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );
    HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
    HRESULT STDMETHODCALLTYPE GetNodeName( BSTR* pbstrName );
    HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
    HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
    HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );
	HRESULT STDMETHODCALLTYPE GetNodeListInfo( DMUSProdListInfo* pListInfo );

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL *pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL *pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

    // IVarChoices functions
	HRESULT	STDMETHODCALLTYPE SetVarChoicesTitle( BSTR bstrTitle );
	HRESULT STDMETHODCALLTYPE SetDataChangedCallback( IUnknown *punkCallback );
	HRESULT STDMETHODCALLTYPE GetUndoText( BSTR *pbstrUndoText );

	//Additional functions
protected:
	HRESULT PasteCF_VARCHOICES( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
    HRESULT SaveVarChoices( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT LoadVarChoices( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );

private:
    DWORD					m_dwRef;

	IDMUSProdNode*			m_pIDocRootNode;
	IDMUSProdNode*			m_pIParentNode;

	CString					m_strTitle;
	IDMUSProdTimelineCallback* m_pCallback;

	int						m_nUndoText;
	HWND					m_hWndEditor;
	WINDOWPLACEMENT			m_wp;
	DWORD					m_dwVariationChoices[NBR_VARIATIONS];		// MOAW choices bitfield
	DWORD				    m_dwSelectedFlagBtns[NBR_VARIATIONS];		// selected function buttons
	BYTE				    m_bSelectedRowBtns[NBR_VARIATIONS];			// selected function buttons
	
	bool					m_fInPaste;

public:
	CVarChoicesCtrl*		m_pVarChoicesCtrl;
};

#endif // __VARCHOICES_H__
