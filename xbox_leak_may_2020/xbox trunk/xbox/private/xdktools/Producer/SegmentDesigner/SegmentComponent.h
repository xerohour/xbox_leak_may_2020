#ifndef __SEGMENTCOMPONENT_H__
#define __SEGMENTCOMPONENT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// SegmentComponent.h : header file
//

#include <afxtempl.h>

#include "DMUSProd.h"
#include "Conductor.h"

class CSegment;
class CSegmentPPGMgr;
interface IDirectMusicPerformance8;
interface IDMUSProdConductor8;

class CSegmentComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt, public IDMUSProdPortNotify,
						  public IDMUSProdNotifySink
{
public:
    CSegmentComponent();
	~CSegmentComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg );
    HRESULT STDMETHODCALLTYPE CleanUp( void );
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
	HRESULT STDMETHODCALLTYPE OnActivateApp(BOOL fActivate);

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

	// IDMUSProdPortNotify functions
	HRESULT STDMETHODCALLTYPE OnOutputPortsChanged( void );
	HRESULT STDMETHODCALLTYPE OnOutputPortsRemoved( void );
        
	// IDMUSProdNotifySink
	HRESULT STDMETHODCALLTYPE OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists( void );
	BOOL RegisterClipboardFormats();
	void ReleaseAll();
	void BroadcastNotification( REFGUID rguidNotification );

public:
    HRESULT STDMETHODCALLTYPE LoadSegment( IStream* pIStream, IDMUSProdNode** ppINode );
    HRESULT STDMETHODCALLTYPE GetSegmentImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE GetSegmentRefImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderImageIndex( short* pnFirstImage );
	void AddToSegmentFileList( CSegment* pSegment );
	void RemoveFromSegmentFileList( CSegment* pSegment );

public:
	IDirectMusicPerformance8*m_pIDMPerformance;
	IDMUSProdFramework		*m_pIFramework;
	IDMUSProdConductor8		*m_pIConductor;
	IDMUSProdComponent		*m_pIContainerComponent;
	IDMUSProdComponent		*m_pIAudioPathComponent;
	IDMUSProdComponent		*m_pIToolGraphComponent;
	short					m_nNextSegment;	// appended to name of new Segment
	IDMUSProdPropPageManager*m_pIPageManager;
	UINT					m_cfJazzFile;	// CF_JAZZFILE clipboard format
	UINT					m_cfSegment;	// CF_SEGMENT clipboard format
	UINT					m_cfContainer;	// CF_CONTAINER clipboard format
	UINT					m_cfAudioPath;	// CF_AUDIOPATH clipboard format
	UINT					m_cfGraph;		// CF_GRAPH clipboard format

private:
    DWORD					m_dwRef;
	IDMUSProdDocType8*		m_pIDocType8;
	IDMUSProdDocType8*		m_pITemplateDocType8;
	short					m_nFirstSegmentImage;
	short					m_nFirstSegmentRefImage;
	short					m_nFirstFolderImage;

	CTypedPtrList<CPtrList, CSegment*> m_lstSegments;
};

#endif // __SEGMENTCOMPONENT_H__
