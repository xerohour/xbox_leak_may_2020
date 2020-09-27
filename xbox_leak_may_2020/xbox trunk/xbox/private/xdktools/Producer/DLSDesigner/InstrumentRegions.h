#ifndef __INSTRUMENTREGIONS_H__
#define __INSTRUMENTREGIONS_H__

// InstrumentRegions.h : header file
//

#include "alist.h"

#define MINIMUM_LAYERS	4

class CRegion;
class CInstrument;

class CInstrumentRegions : public AList, public IDMUSProdNode, public IDMUSProdSortNode
{
friend class CInstrument;
friend class CDLSComponent;

public:
	void ValidateUIA();
	void SetDrumArticulation();
    CInstrumentRegions();
	~CInstrumentRegions();

	enum { SIZE_OF_NOTE_MAP = 128 }; 
	
	//	AList overrides
    CRegion *GetHead();
    CRegion *RemoveHead();
	void Remove(CRegion *pRegion);
	void AddTail(CRegion *pRegion);

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

	// IDMUSProdSortNode : Sorting interface
	HRESULT STDMETHODCALLTYPE CompareNodes(IDMUSProdNode* pINode1, IDMUSProdNode* pINode2, int* pnResult);

	// Additional functions.
	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);

	/* creates a new region */
	HRESULT InsertRegion(IDMUSProdNode* pIChildNode, int nLayer, USHORT lowerNote,
		USHORT upperNote, bool bInsertIntoKeyboard = true, CRegion **pRegion = NULL);

	void	GetNextFreeNote(USHORT& usLayer, int& nNote);
	
	HRESULT	 FindRegionsForNote(short nNote, CPtrList* plstRegions);
	CRegion* FindRegionFromMap(short nLayer, short nStartNote);
	CRegion* FindRegionFromMap(int nNote);
	HRESULT	 FindPlayingRegions(USHORT nPlayNote, USHORT usVelocity, CPtrList* plstRegions);
	
	int		GetNumberOfLayers();
	void	SetNumberOfLayers(int nLayers);
	BOOL	IsLayerEmpty(int nLayer);

	/* copies a region, optionally returns the new region in ppRegion */
	HRESULT	CopyRegion(short nSrcLayer, short nSrcStartNote, short nCopyLayer, short nCopyStartNote, CRegion **ppRegion = NULL);

	/* duplicates a region into a new layer, returning the new region in ppRegion */
	HRESULT CopyRegion(CRegion *pOldRegion, CRegion **ppNewRegion);

	/* get number of regions included in this collection, counting each mono channel separately */
	LONG	GetChannelCount();

private:
    DWORD			m_dwRef;
	IDMUSProdNode*	m_pIRootNode;
	IDMUSProdNode*	m_pIParentNode;

	CDLSComponent*	m_pDLSComponent;	
	CInstrument*	m_pInstrument;
	int				m_nNumberOfLayers;						// Total number of layers
};

#endif // __INSTRUMENTREGIONS_H__
