// DropTarget.cpp : implementation file
// For OLE Drag and Drop between tree controls
// Designed and developed by Vinayak Tadas
// vinayakt@aditi.com
// Modified for use in DirectMusic Producer by James Dooley
// jdooley@microsoft.com
// 

#include "stdafx.h"
#include "Droptarget.h"
#include "XboxAddin.h"

#define RECT_BORDER 10

/////////////////////////////////////////////////////////////////////////////
// CDropTarget

CDropTarget::CDropTarget()
{
	m_pITargetDataObject = NULL;
	m_pDropControl = NULL;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_cRef = 0;
	AddRef();
}

HRESULT CDropTarget::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualIID(riid, IID_IDropTarget)
	||	::IsEqualIID(riid, IID_IUnknown) )
	{
		*ppv = (IDropTarget *)this;
	}
	else
	{
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDropTarget::AddRef( void )
{
	return InterlockedIncrement( &m_cRef );
}

ULONG CDropTarget::Release( void )
{
	const long lRes = InterlockedDecrement( &m_cRef );
	if( lRes == 0 )
	{
		delete this;
	}

	return lRes;
}

/********************************************************************
OnDragEnter()
	Called when the user drags the object in control.
********************************************************************/
HRESULT CDropTarget::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL point, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Release the target data object, if we haven't already done so (we should have)
	RELEASE( m_pITargetDataObject );

	// Validate the pointer to the IDataObject
	if( pIDataObject )
	{
		// Store IDataObject associated with current drag-drop operation
		m_pITargetDataObject = pIDataObject;
		m_pITargetDataObject->AddRef();

		// Determine effect of drop
		return DragOver( grfKeyState, point, pdwEffect );
	}
	else
	{
		return E_POINTER;
	}
}

/********************************************************************
OnDragOver()
	Called when the user drags the object over control.
********************************************************************/
		
HRESULT CDropTarget::DragOver( DWORD grfKeyState, POINTL pointl, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate our pointers
	if( (m_pDropControl == NULL)
	||	(m_pITargetDataObject == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	*pdwEffect = DROPEFFECT_NONE;

	m_pDropControl->InternalDragOver( grfKeyState, pointl, m_pITargetDataObject, pdwEffect );

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = *pdwEffect;
	}

	return S_OK;
}


/********************************************************************
OnDragLeave()
	Called when the user drags the object out of control.
********************************************************************/
HRESULT CDropTarget::DragLeave( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate our pointer
	if( m_pDropControl == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Release our pointer to the object being dragged
	RELEASE( m_pITargetDataObject );

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	// Remove Highlighting
	m_pDropControl->InternalDragLeave();

	return S_OK;
}


/********************************************************************
OnDrop()
	Called when the user drops the object in the control.
********************************************************************/
HRESULT CDropTarget::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL point, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(grfKeyState);

	if( (pdwEffect == NULL)
	||	(pIDataObject == NULL) )
	{
		ASSERT(FALSE);
		return E_POINTER;
	}

	ASSERT( m_pITargetDataObject != NULL );
	ASSERT( m_pITargetDataObject == pIDataObject );

	// Validate our pointer
	if( m_pDropControl == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	m_pDropControl->InternalDrop( pIDataObject, point, m_dwOverDragEffect );

	*pdwEffect = m_dwOverDragEffect;

	// Release the target data object, if we haven't already done so (we should have)
	RELEASE( m_pITargetDataObject );

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}
