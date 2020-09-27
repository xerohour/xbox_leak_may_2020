#if !defined(AFX_DROPTARGET_H__345241C3_897C_11D3_A59E_00A02411D21E__INCLUDED_)
#define AFX_DROPTARGET_H__345241C3_897C_11D3_A59E_00A02411D21E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DropTarget.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDropTarget

class CDropControl
{
public:
	virtual void InternalDragOver( DWORD grfKeyState, POINTL ptScreen, IDataObject* pIDataObject, DWORD* pdwEffect ) = 0;
	virtual void InternalDrop( IDataObject* pIDataObject, POINTL ptScreen, DWORD dwEffect) = 0;
	virtual void InternalDragLeave( void ) = 0;
};

class CDropTarget :public IDropTarget	
{
public:
	CDropTarget();

	// IUnknown functions
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL point, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL point, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL point, DWORD* pdwEffect);

// Members
public:
	CDropControl	*m_pDropControl;

private:
	IDataObject 	*m_pITargetDataObject;
	DWORD			m_dwOverDragButton;
	DWORD			m_dwOverDragEffect;
	long			m_cRef;

};
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DROPTARGET_H__345241C3_897C_11D3_A59E_00A02411D21E__INCLUDED_)
