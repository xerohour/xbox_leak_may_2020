#ifndef __COLLECTIONWAVES_H__
#define __COLLECTIONWAVES_H__

// CollectionWaves.h : header file
//

#include "alist.h"
#include "DllJazzDataObject.h"

class CCollection;
class CWaveNode;
class CWave;

class CCollectionWaves : public AList, public IDMUSProdNode, public IDMUSProdNodeDrop
{
friend class CCollection;
friend class CDLSComponent;

public:
    CCollectionWaves();
	~CCollectionWaves();

	//	AList overrides
    CWaveNode* GetHead();
    CWaveNode* RemoveHead();
	void Remove(CWaveNode* pWave);
	void AddTail(CWaveNode* pWave);

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

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

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

	// IDMUSProdNodeDrop : interface that handles special drag-drop cases for this node
	HRESULT STDMETHODCALLTYPE OnDropFiles(HANDLE hDropInfo);

	//		Additional functions.
	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT PasteCF_WAVE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject);
	HRESULT PasteWaves( IDMUSProdRIFFStream* pRiffStream);

	HRESULT InsertWaveFile(CString sFileName, CWaveNode** ppInsertedWave = NULL);
	BOOL	AddToCollection(IDMUSProdNode* pIChildNode);

	void	NotifyInstrumentsWavesChanged();						// Goes through the instrument list and asks the instrument editor to update 
	void	ReplaceWave(CWaveNode* pWave, CString sWaveFileName);	// Replaces pWave wherever it's referenced with the sWaveFileName pWave is deleted
	HRESULT	GetListOfMonoWaves(CPtrList* pWaveList);				// Goes through the list of nodes and collects the mono waves (even the individual channels of a stereo wave) and returns the list
	HRESULT	GetListOfWaves(CPtrList* pWaveList);					// Goes through the list of nodes and collects the mono and stereo waves
	CWave*	GetFirstWave();											// Returns wave pointed to by first node, NULL if none
	BOOL	IsNodeInCollection(CWaveNode* pWaveNode);				// Returns TRUE if the the node is already in the collection

	CCollection*	GetCollection();

	void	ReleaseWaveNodeDummyInstruments();
	void	UnRegisterWavesFromTransport();
	void	RegisterWavesWithTransport();

private:
    DWORD				m_dwRef;
	CDLSComponent*		m_pDLSComponent;
	IDMUSProdNode*		m_pIRootNode;
	IDMUSProdNode*		m_pIParentNode;

	CCollection*	 m_pCollection;
};

#endif // __COLLECTIONWAVES_H__
