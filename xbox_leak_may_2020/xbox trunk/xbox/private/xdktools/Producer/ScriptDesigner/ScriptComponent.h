#ifndef __SCRIPTCOMPONENT_H__
#define __SCRIPTCOMPONENT_H__

// ScriptComponent.h : header file
//

#include <afxtempl.h>
#include <Conductor.h>
#include "DebugDlg.h"
#include "LogTool.h"

class CDirectMusicScript;


////////////////////////////////////////////////////////////////////////////////
class CScriptComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt,
						 public IDMUSProdMenu, public IDMUSProdDebugScript,
						 public IDMUSProdPortNotify
{
friend UINT AFX_CDECL MessageTextThreadProc( LPVOID pParam );

public:
    CScriptComponent();
	~CScriptComponent();

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

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

    // IDMUSProdMenu functions
    HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
    HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
    HRESULT STDMETHODCALLTYPE OnMenuInit( HMENU hMenu, UINT nMenuID );
    HRESULT STDMETHODCALLTYPE OnMenuSelect();

	// IDMUSProdDebugScript functions
    HRESULT STDMETHODCALLTYPE DisplayScriptError( void* pErrorInfo );
    HRESULT STDMETHODCALLTYPE DisplayText( WCHAR* pwszText  );

	// IDMUSProdPortNotify functions
	HRESULT STDMETHODCALLTYPE OnOutputPortsChanged( void );
	HRESULT STDMETHODCALLTYPE OnOutputPortsRemoved( void );

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists();
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE GetScriptImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetScriptRefImageIndex( short* pnNbrFirstImage );
	void AddToScriptFileList( CDirectMusicScript* pScript );
	void RemoveFromScriptFileList( CDirectMusicScript* pScript );
	BOOL StartMessageTextThread();
	BOOL EndMessageTextThread();

public:
	IDMUSProdFramework*			m_pIFramework;
	IDirectMusicPerformance*	m_pIDMPerformance;
	IDMUSProdConductor*			m_pIConductor;
	IDMUSProdComponent*			m_pIContainerComponent;

	CDebugDlg*					m_pDebugDlg;		// Message Window
	WINDOWPLACEMENT*			m_pDebugDlgWP;		// Message Window placement
	CLogTool*					m_pLogTool;			// Captures script trace commands

	short						m_nNextScript;		// appended to name of new Script
	UINT						m_cfProducerFile;	// CF_DMUSPROD_FILE clipboard format
	UINT						m_cfScript;			// CF_SCRIPT clipboard format
	UINT						m_cfScriptList;		// CF_SCRIPTLIST clipboard format

	BOOL						m_fDisplayMessageText;
	BOOL						m_fEndMessageTextThread;
	HANDLE						m_hEvent_WakeUpMessageTextThread;
	HANDLE						m_hEvent_ExitMessageTextThread;
	HANDLE						m_hMessageTextThread;
	CRITICAL_SECTION			m_CrSecMessageText;	        
	CRITICAL_SECTION			m_CrSecDebugDlg;	        
	CTypedPtrList<CPtrList, CString*> m_lstMessageText;

private:
    DWORD						m_dwRef;
	IDMUSProdDocType8*			m_pIScriptDocType8;
	short						m_nFirstImage;
	BOOL						m_fMenuWasAdded;

	CTypedPtrList<CPtrList, CDirectMusicScript*> m_lstScripts;
};

#endif // __SCRIPTCOMPONENT_H__
