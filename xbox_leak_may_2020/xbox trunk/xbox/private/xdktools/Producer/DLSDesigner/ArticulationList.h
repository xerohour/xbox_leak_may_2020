#ifndef __ARTICULATIONLIST_H__
#define __ARTICULATIONLIST_H__

// ArticulationList.h : header file
//

#include "alist.h"
#include "RiffStrm.h"
#include "UnknownChunk.h"

class CRegion;
class CArticulation;
class CInstrument;
class CArticulationListPropPageMgr;
class CConditionalChunk;
class CSystemConfiguration;
class CCollection;

class CArticulationList : public AList, public IDMUSProdNode, public IDMUSProdPropPageObject, public IPersistStream
{
friend class CInstrument;
friend class CDLSComponent;

public:
    CArticulationList(CDLSComponent* pComponent, CInstrument* pOwner);
	CArticulationList(CDLSComponent* pComponent, CRegion* pOwner);

	~CArticulationList();

	//	AList overrides
    CArticulation* GetHead();
    CArticulation* RemoveHead();
	void Remove(CArticulation* pArticulation);
	void AddTail(CArticulation* pArticulation);

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );

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

    HRESULT STDMETHODCALLTYPE GetEditorClsId(CLSID* pclsid);
    HRESULT STDMETHODCALLTYPE GetEditorTitle(BSTR* pbstrTitle);
    HRESULT STDMETHODCALLTYPE GetEditorWindow(HWND* hWndEditor);
    HRESULT STDMETHODCALLTYPE SetEditorWindow(HWND hWndEditor);

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnResourceId);
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit(HMENU hMenu);
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect(long lCommandId);

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE GetFirstChild(IDMUSProdNode** ppIFirstChildNode);
	HRESULT STDMETHODCALLTYPE GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode);
	HRESULT STDMETHODCALLTYPE GetNodeListInfo(DMUSProdListInfo* pListInfo);

	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );
    // IDMUSProdNode : drag & drop methods
	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject );

	// IPersist functions
    STDMETHOD(GetClassID)(CLSID* pClsId);

	// IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();
	HRESULT STDMETHODCALLTYPE OnShowProperties();

	// Additional functions.
public:
	
	HRESULT	Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream);

	DWORD	Count();
	DWORD	Size();
	HRESULT Write(void* pv, DWORD* pdwRelativeCurOffset, DWORD* pdwCurrentOffset, DWORD* pDMWOffsetTable, DWORD* pdwCurIndex);

	void	SetDLS1(BOOL bDLS1);
	BOOL	IsDLS1();
	void	SetOwner(CInstrument* pInstrument);
	void	SetOwner(CRegion* pRegion);

	void				CheckConfigAndRefreshNode();
	void				OnConditionConfigChanged(CSystemConfiguration* pCurrentConfig, bool bRefreshNode = true);
	HRESULT				SetCondition(const CString& sCondition);
	BOOL				IsConditionOK();
	void				SetConditionOK(BOOL bConditionOK = TRUE, bool bRefreshNode = true);
	CConditionalChunk*	GetConditionalChunk();
	UINT				GetArticulationsUsed();	// Returns the number of arts actually OK to download

	CCollection*		GetCollection();	// The collection this articulation list belongs to...
	CInstrument*		GetInstrument();	// The instrument this list belongs to...

private:
	void	DeleteAllArticulations();
	
	
private:

    DWORD			m_dwRef;
	IDMUSProdNode*	m_pIRootNode;
	IDMUSProdNode*	m_pIParentNode;

	CDLSComponent*	m_pDLSComponent;	
	CInstrument*	m_pInstrument;

	CRegion*		m_pOwnerRegion;			

	BOOL			m_bDLS1;
	BOOL			m_bOwnerIsInstrument;

	BOOL			m_bConditionOK;

	CConditionalChunk* m_pConditionalChunk;
	CUnknownChunk      m_UnknownChunk;

	CArticulationListPropPageMgr* m_pArticListPropPageMgr;
};

#endif // __ARTICULATIONLIST_H__