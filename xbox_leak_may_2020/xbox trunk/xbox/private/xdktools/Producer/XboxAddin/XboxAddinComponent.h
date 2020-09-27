#ifndef __XBOXADDINCOMPONENT_H__
#define __XBOXADDINCOMPONENT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "conductor.h"

// XboxAddinComponent.h : header file
//

class CXboxAddinDlg;
class CSegment;
class COtherFile;
class CAudiopath;
class CXboxSynthMenu;
interface IDirectMusicPerformance8;

////////////////////////////////////////////////////////////////////////////////
class CXboxAddinComponent : public IDMUSProdComponent, public IDMUSProdMenu
{
public:
    CXboxAddinComponent();
	~CXboxAddinComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg );
    HRESULT STDMETHODCALLTYPE CleanUp( void );
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
	HRESULT STDMETHODCALLTYPE OnActivateApp( BOOL fActivate );

    // IDMUSProdMenu functions
    HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
    HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
    HRESULT STDMETHODCALLTYPE OnMenuInit( HMENU hMenu, UINT nMenuID );
    HRESULT STDMETHODCALLTYPE OnMenuSelect();

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists();
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE GetXboxAddinImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetXboxAddinRefImageIndex( short* pnNbrFirstImage );
	BOOL StartMessageTextThread();
	BOOL EndMessageTextThread();
	HRESULT EnsureNodeIsCopied( IDMUSProdNode *pIDMUSProdNode );
	HRESULT DoXboxSynthState( bool fSet, BOOL *pbXboxEnabled, BOOL *pbPCEnabled );

public:
	IDMUSProdFramework8*		m_pIFramework;
	IDMUSProdConductor8*		m_pIConductor;
	IDirectMusicPerformance8*	m_pIDMPerformance;

	CXboxAddinDlg*				m_pXboxAddinDlg;	// Experimenter Window
	WINDOWPLACEMENT*			m_pXboxAddinDlgWP;	// Message Window placement
	CXboxSynthMenu*				m_pXboxSynthMenu;	// Xbox Synth menu item

	CTypedPtrList< CPtrList, CSegment *> m_lstPrimarySegments;
	CTypedPtrList< CPtrList, CSegment *> m_lstSecondarySegments;
	CTypedPtrList< CPtrList, COtherFile *> m_lstOtherFiles;
	CTypedPtrList< CPtrList, CAudiopath *> m_lstAudiopaths;

private:
    DWORD						m_dwRef;
	BOOL						m_fMenuWasAdded;
	BOOL						m_fSynthMenuWasAdded;
};

#endif // __XBOXADDINCOMPONENT_H__
