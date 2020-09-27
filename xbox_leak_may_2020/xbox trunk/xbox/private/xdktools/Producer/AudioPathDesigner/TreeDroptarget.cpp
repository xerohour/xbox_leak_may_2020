// TreeDropTarget.cpp : implementation file
// For OLE Drag and Drop between tree controls
// Designed and developed by Vinayak Tadas
// vinayakt@aditi.com
// Modified for use in DirectMusic Producer by James Dooley
// jdooley@microsoft.com
// 

#include "stdafx.h"
#include "TreeDroptarget.h"
#include "MultiTree.h"
#include "AudioPathDlg.h"
#include "AudioPath.h"
#include "EffectListDlg.h"
#include "DllJazzDataObject.h"

#define RECT_BORDER	10

/////////////////////////////////////////////////////////////////////////////
// CTreeDropTarget

CTreeDropTarget::CTreeDropTarget()
{
	m_pITargetDataObject = NULL;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_cRef = 0;
	AddRef();
}

HRESULT CTreeDropTarget::QueryInterface( REFIID riid, LPVOID *ppv )
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

ULONG CTreeDropTarget::AddRef( void )
{
	return InterlockedIncrement( &m_cRef );
}

ULONG CTreeDropTarget::Release( void )
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
	Called when the user drags the object in Tree control.
********************************************************************/
HRESULT CTreeDropTarget::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL point, DWORD* pdwEffect)
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
	Called when the user drags the object over Tree control.
********************************************************************/
		
HRESULT CTreeDropTarget::DragOver( DWORD grfKeyState, POINTL pointl, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate our pointers
	if( (m_pAudioPathDlg == NULL)
	||	(m_pITargetDataObject == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Highlight the item under the mouse and 
	CMultiTree *pDestTreeCtrl = &(m_pAudioPathDlg->m_tcTree);
	POINT point = {pointl.x, pointl.y};
	pDestTreeCtrl->ScreenToClient( &point );

	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	pDestTreeCtrl->GetItemBufferEffectUnderPoint( point, &pItemInfo, &pBufferOptions, NULL );
	
	// Scroll Tree control depending on mouse position
	CRect rectClient;
	pDestTreeCtrl->GetClientRect(&rectClient);
	pDestTreeCtrl->ClientToScreen(rectClient);
	pDestTreeCtrl->ClientToScreen(&point);
	int nScrollDir = -1;
	if ( point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
	if ( (point.y <= rectClient.top + RECT_BORDER) )
		nScrollDir = SB_LINEUP;

	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = pDestTreeCtrl->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pDestTreeCtrl->SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if ( point.x <= rectClient.left + RECT_BORDER )
		nScrollDir = SB_LINELEFT;
	else
	if ( point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = pDestTreeCtrl->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pDestTreeCtrl->SendMessage(WM_HSCROLL, wParam);
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject )
	{
		if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( m_pITargetDataObject, CDirectMusicAudioPath::m_scfEffect ) ) )
		{
			if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				dwEffect = DROPEFFECT_COPY;
			}
		}
		else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, CDirectMusicAudioPath::m_scfPChannel ) ) )
		{
			if ( grfKeyState & MK_SHIFT )
			{
				dwEffect = DROPEFFECT_MOVE;
			}
			else
			{
				dwEffect = DROPEFFECT_COPY;
			}
		}

		delete pDataObject;
	}

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = dwEffect;
	}

	*pdwEffect = dwEffect;
	return S_OK;
}


/********************************************************************
OnDragLeave()
	Called when the user drags the object out of Tree control.
********************************************************************/
HRESULT CTreeDropTarget::DragLeave( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate our pointer
	if( m_pAudioPathDlg == NULL )
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
	//m_pAudioPathDlg->m_tcTree.SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);

	return S_OK;
}


/********************************************************************
OnDrop()
	Called when the user drops the object in the  Tree control.
********************************************************************/
HRESULT CTreeDropTarget::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL point, DWORD* pdwEffect)
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
	if( m_pAudioPathDlg == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	//Remove highlighting
	//m_pAudioPathDlg->m_tcTree.SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);

	POINT ptShort = {point.x, point.y};
	m_pAudioPathDlg->m_tcTree.ScreenToClient( &ptShort );

	HRESULT hr = m_pAudioPathDlg->DropOnTree( pIDataObject, m_dwOverDragEffect, ptShort );

	*pdwEffect = m_dwOverDragEffect;

	// Release the target data object, if we haven't already done so (we should have)
	RELEASE( m_pITargetDataObject );

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return hr;
}
