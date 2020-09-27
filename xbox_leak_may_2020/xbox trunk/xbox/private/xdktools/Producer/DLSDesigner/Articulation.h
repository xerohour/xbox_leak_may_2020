#ifndef __ARTICULATION_H__
#define __ARTICULATION_H__

// Articulation.h : header file
//

#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "DLSLoadSaveUtils.h"
#include "dls1.h"
#include "UnknownChunk.h"

#define TOTAL_CONNECTIONS	25

#define MAXADSRTIME		40000

#define DELAY_COLOR		RGB(0, 0, 0)		// Delay segment color
#define ATTACK_COLOR	RGB(255, 0, 0)		// Attack segment color
#define HOLD_COLOR		RGB(255, 0, 255)	// Hold segment color
#define DECAY_COLOR		RGB(0, 128, 64)		// Decay segment color
#define SUSTAIN_COLOR	RGB(0, 0, 0)		// Sustain segment color same as the delay color
#define RELEASE_COLOR	RGB(0, 0, 255)		// Release segment color

class CConditionalChunk;

class CArticulation : public AListItem, public IDMUSProdNode, public IPersistStream
{

public:
    CArticulation(CDLSComponent* pComponent, CInstrument* pInstrument);
	CArticulation(CDLSComponent* pComponent, CRegion* pRegion);

	~CArticulation();

	// AListItem Method
	CArticulation* GetNext();

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
    
	// IPersist functions
    STDMETHOD(GetClassID)(CLSID* pClsId);

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	//Additional functions
public:

	DWORD PrepareTags(DWORD dwTag);
    HRESULT Load(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT Write(void* pv, DWORD* pdwCurrentOffset, DWORD* pDMWOffsetTable, DWORD* pdwCurIndex, DWORD dwNextArtIndex);
	ArticParams* GetArticParams();

	CInstrument*	GetInstrument();
	CRegion*		GetRegion();

	// General purpose accessors/mutators
	void*	GetOwner(BOOL& bOwnerIsRegion);
	void	SetOwner(CInstrument* pInstrument);
	void	SetOwner(CRegion* pRegion);

	BOOL	IsDLS1();
	void	SetDLS1(BOOL bDLS1);

	void	SetDeleteFlag(bool bDeletingArt);
	bool	GetDeleteFlag();

#ifdef _DEBUG
	// You want to call ValidateParent in debug mode every time you either 
	// reference m_pInstrument or m_pRegion.
	void ValidateParent(); 
#endif

protected:
	void InitDefaultArticParams();
	void CreateConnectionList(UINT& nConnections);
	void GetLevel1Articulation(DMUS_ARTICPARAMS& articParamsDLS1);

public:
	DWORD Count();
	DWORD Size();
	CDLSComponent*		m_pComponent;

private:
    DWORD				m_dwRef;
	IDMUSProdNode*		m_pIRootNode;
	IDMUSProdNode*		m_pIParentNode;

	CONNECTION			m_arrConnections[TOTAL_CONNECTIONS];
	
	CInstrument*		m_pInstrument;		// Instrument that owns this, or
    CRegion*			m_pRegion;			// Region that owns this.

    ArticParams			m_ArticParams;		// Articulation data.
	DWORD				m_dwEditTag;		// Tag for edit downloads.
	bool				m_bDeletingArticulation;
	BOOL				m_bDLS1;
	BOOL				m_bOwnerIsInstrument;
};

#endif // __ARTICULATION_H__
