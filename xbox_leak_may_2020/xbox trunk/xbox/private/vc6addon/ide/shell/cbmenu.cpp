// MenuPopup.cpp : implementation file
//

#include "stdafx.h"
#include "shlmenu.h"

#include "barglob.h"

#include "afxpriv.h" //WM_IDLEUPDATECMDUI
#include <tchar.h> // For command moving to a menu...
#include "barcust.h"
#include "menuprxy.h"
#include "bardockx.h"
#include "prxycust.h"
#include "resource.h"
#include "shell.h"

// [multimon] 7 mar 97 - der
#include "mmonitor.h"
#include "math.h"

extern HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);
static void MdiRefreshMenu() ;

static const int c_iBorderSize = 4 ;
static const int c_iArraySize = 20 ;
static const int c_iArrayGrow = 20 ;
// width and height of an empty menu
static const int c_iEmptyWidth = 100;
static const int c_iEmptyHeight = 16;
static const int c_iNoItem = -1 ;
static const int c_iButtonTextGap = 2 ;
static const int c_cyScrollAreaHeight=16;
static const int c_nInitialScrollDelay=300;
static const int c_nScrollDelay=100;

// timer ids for auto menus. All of these are arbitrary values.
static const int c_iExpandTimerID=0xEAE1E;
static const int c_iContractTimerID=0xC0ACE1E;
static const int c_iScrollDownTimerID=0x5C011d01;
static const int c_iScrollUpTimerID=0x5C0111e1;

BOOL CBMenuPopup::s_bLastWasKey=FALSE;
BOOL CBMenuPopup::s_bSuppressedMouseMove=FALSE;

// bitmaps used for scrolling menus
CBitmap CBMenuPopup::s_bmpScrollDown;
CBitmap CBMenuPopup::s_bmpScrollUp;
CSize CBMenuPopup::s_sizeScroll;

BOOL CBMenuPopup::s_bCreateTransients=FALSE;

int CBMenuPopup::s_nLastSystemClick=0;
CPoint CBMenuPopup::s_ptLastSystemClick(0,0);

CPoint CBMenuPopup::s_ptLastMove(0,0);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#undef new
#endif

extern UINT DSM_TESTMENU;

IMPLEMENT_DYNAMIC(CBMenuPopup, CWnd)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Dfine this to one to trace on Cmdui handlers that are in the slowest 10% of the range
// See barcust.cpp for an explanation of this #define
#ifdef _DEBUG
#define FIND_SLOW_CMDUI 0
#else
#define FIND_SLOW_CMDUI 0
#endif

/////////////////////////////////////////////////////////////////////////////
// CForwardMouseMessage
//
// It is used when the user clicks outside of the menu bar area.
//

class CForwardMouseMessage
{
public:
	//
	// Constructor
	//
	CForwardMouseMessage(const MSG* pmsg);

	//
	// Member function
	//
	void Send(HWND hWndHadCapture) ;

	// Member variables.
	UINT m_message ;
	WPARAM m_wParam ;
	POINT m_pt;

#ifdef _DEBUG
	BOOL m_bSent ;
#endif

};

//
// Constructor
//
CForwardMouseMessage::CForwardMouseMessage(const MSG* pmsg)
{
	// Save the imnportant parts of the message.
	m_message       = pmsg->message ;
	m_wParam        = pmsg->wParam ;

	// This class only works if the message is WM_LBUTTONDOWN or WM_RBUTTONDOWN. or double click
	ASSERT((m_message == WM_LBUTTONDOWN) || (m_message == WM_LBUTTONDBLCLK) || (m_message == WM_RBUTTONDOWN)) ;

	// The window in the message will be gone when we forward the message.
	// Therefore we need to convert the point from client to screen coordinates here.
	// NOTE: We have to cast to short because these values are signed!!!
	m_pt.x = (short)LOWORD(pmsg->lParam);  // horizontal position of cursor 
	m_pt.y = (short)HIWORD(pmsg->lParam);  // vertical position of cursor 

	VERIFY(::ClientToScreen(pmsg->hwnd, &m_pt)) ;

#ifdef _DEBUG
	m_bSent = FALSE ;
#endif
}

//
// Forward the message on.
//
void CForwardMouseMessage::Send(HWND hWndHadCapture)
{
	ASSERT(!m_bSent) ;

	// Do not first try to send the message to the window which had the capture,
	// because this doesn't work correctly with the dialog editor.

	// Get the window corresponding to the point where the mouse was pressed 
	// and send the message to it.
	CWnd *pWndDestination = CWnd::WindowFromPoint(m_pt) ;                                

	if (pWndDestination == NULL)
	{
		// We don't have a window to send to, so lets get out of here.
		TRACE0("CmdBars: No window to forward messages.");
		return ;
	}

	// We need to adjust the mouse point into client coordinates for 
	// the window we are forwarding to. We also need to determine if this is a non-client
	// message
	CRect rectClient;
	pWndDestination->GetClientRect(&rectClient);
	pWndDestination->ClientToScreen(&rectClient);

	if(!rectClient.PtInRect(m_pt))
	{
		// It must be a nonclient message
		switch(m_message)
		{
			case WM_LBUTTONDOWN:
				m_message=WM_NCLBUTTONDOWN;
				break;
			case WM_RBUTTONDOWN:
				m_message=WM_NCRBUTTONDOWN;
				break;
			case WM_LBUTTONDBLCLK:
				m_message=WM_NCLBUTTONDBLCLK;
				break;
		}

		// don't convert coords, because NC messages expect screen coords

		// get the NC hittest code
		LPARAM lParam = MAKELPARAM(m_pt.x, m_pt.y) ;
		m_wParam=pWndDestination->SendMessage(WM_NCHITTEST, 0, lParam);
	}
	else
	{
		pWndDestination->ScreenToClient(&m_pt);
	}

	// Now, let's forward the message
	LPARAM lParam = MAKELPARAM(m_pt.x, m_pt.y) ;
	pWndDestination->SendMessage(m_message, m_wParam, lParam) ;
}

/////////////////////////////////////////////////////////////////////////////
// CBMenuPopup

CBMenuPopup::CBMenuPopup()
:
   m_nMaxIndex(0),
   m_bDropDown(FALSE),
   m_bDirty(FALSE),
   m_pWndMenuBar(NULL),
   m_nCurrentTimer(0),
   m_pForwardMsg(NULL),
   m_pProxy(NULL),
   m_bAutoEnable(TRUE),
   m_bPassBackCancel(FALSE)
{
	// Initialize the Array
   m_MenuItems.SetSize(c_iArraySize, c_iArrayGrow) ;

   // Initialize all of the other stuff.
   Initialize() ;

   // Get the current size of the large toolbar buttons
   m_bLargeToolbar = globalData.GetLargeMenuButtons();

   // Initialize the widths to zero.
   m_iHeight = c_iBorderSize*2;
   for (int i = 0 ; i < c_iNumFields ; i++)
   {
	 m_iFieldWidths[i] = 0 ;
   }
   AllocateProxy();
}

CBMenuPopup::~CBMenuPopup()
{
	ASSERT(m_nCurrentTimer==0);

	DeleteAll();

	FreeProxy();

	if(m_pForwardMsg)
	{
		delete m_pForwardMsg;
		m_pForwardMsg=NULL;
	}
}

void CBMenuPopup::DeleteAll()
{
   for(int i = 0 ; i < m_nMaxIndex  ; i++)
   {
      CBMenuItem* pItem = m_MenuItems[i] ;

      pItem->Release() ;
   }
   m_MenuItems.RemoveAll();
   m_nMaxIndex = 0;

}

//The popups for the main menus are created once and then 
// exist for the life of the product. Therefore, reset stuff, at
// on trackpopup...
void CBMenuPopup::Initialize()
{
	m_nSelectedMenuItem = c_iNoItem;
	m_nExpandedMenuItem = c_iNoItem;
	m_nExpandDeferredItem= c_iNoItem;
	m_pParentWnd = NULL ;
	m_pChildPopup = NULL ;
	m_bSubPopup = FALSE ;
	m_bContinueModal = TRUE ;
	m_bPainted=FALSE;
	m_nPostCmdID = 0 ;
	m_bReturnCmdID=FALSE;
	m_bControl = FALSE ;
	m_rectAvoid  = CRect(0,0,0,0) ;
	m_bScrolling=FALSE;                             
	m_bHasTopScroll=FALSE;                  
	m_bHasBottomScroll=FALSE;
	m_nTopIndex=0;                  
	m_cyTopOffset=0;
	if (m_pForwardMsg != NULL)
	{
		delete m_pForwardMsg;
		m_pForwardMsg = NULL;
	}
	m_nFlags = 0;
	m_bBarFocus=FALSE;
	s_bLastWasKey=FALSE;
	s_bSuppressedMouseMove=FALSE;
	EndDefer();

	// The follow items are not reset.
	// m_rectScreen.SetRectEmpty(); 
	// m_sizeDesired = CSize(0,0) ;
	// m_bLargeToolbar = theApp.m_bLargeToolBars; //VerifySize handles in Display.
}

BEGIN_MESSAGE_MAP(CBMenuPopup, CWnd)
	//{{AFX_MSG_MAP(CBMenuPopup)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SYSKEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	
	ON_REGISTERED_MESSAGE(DSM_TESTMENU, OnTestMenu)

END_MESSAGE_MAP()

///////////////////////////////////////////////////////////
//
//
//
UINT CBMenuPopup::TrackDropDownMenu(int x, int y, 
									CWnd* pWnd, 
									LPCRECT lpRect /*= 0 */, 
									CRect rectAvoid, 
									CWnd* pWndMenuBar, /*= NULL */
									BOOL bSelectFirstItem, /*=FALSE*/
									BOOL *pbBarFocus,
									BOOL bReturnCmd
									)
{
	m_pWndMenuBar = pWndMenuBar ;
	m_bDropDown = TRUE;

	UINT nFlags=TPM_LEFTALIGN | TPM_LEFTBUTTON;
	if(bReturnCmd)
	{
		nFlags|=TPM_RETURNCMD;
	}

	UINT nCmd=TrackMenu(nFlags, x, y, pWnd, lpRect , rectAvoid, bSelectFirstItem) ;
	if(pbBarFocus)
	{
		*pbBarFocus=m_bBarFocus;
	}
	return nCmd;
}

///////////////////////////////////////////////////////////
//
//
//
UINT CBMenuPopup::TrackPopupMenu( UINT nFlags, int x, int y, 
				CWnd* pWnd, LPCRECT lpRect /*= 0 */)
{
	m_pWndMenuBar = NULL ; 
	m_bDropDown = FALSE ;
	return TrackMenu(nFlags, x, y, pWnd, lpRect) ;
}

// This function was duplicated and renamed rather than overloaded, 
// because of the default paramater and the
// potential for unwanted promotions from LPRECT to CRect.
UINT CBMenuPopup::TrackPopupMenuEx( UINT nFlags, int x, int y, 
				CWnd* pWnd, CRect rectAvoid, LPCRECT lpRect /*= 0 */)
{
	m_pWndMenuBar = NULL ; 
	m_bDropDown = FALSE ;
	return TrackMenu(nFlags, x, y, pWnd, lpRect, rectAvoid) ;
}

//
//
//
UINT CBMenuPopup::TrackMenu(UINT nFlags, int x, int y, 
							CWnd* pWnd, LPCRECT lpRect /*= 0 */, CRect rectAvoid,
							BOOL bSelectFirstItem /*=FALSE*/)
{
#if _DER
   if (!(nFlags & TPM_RIGHTALIGN))
      TRACE0("TPM_RIGHTALIGN ignored.\r\n") ;
   if (!(nFlags & TPM_RIGHTBUTTON))
      TRACE0("TPM_RIGHTBUTTON ignored\r\n") ;
   if (lpRect != 0)
      TRACE("lpRect ignored.\r\n") ;
#endif

   // Reset everything!
   Initialize() ;

   if(nFlags & TPM_RETURNCMD)
   {
		m_bReturnCmdID=TRUE;
   }

   // Set up the new stuff.
   m_pParentWnd = pWnd ; // Set Parent Windows for DoModal
   m_rectAvoid=rectAvoid;

	m_bSelectFirstItem=bSelectFirstItem;
	if (bSelectFirstItem)
	{
		s_bLastWasKey=TRUE;
	}

   if (!IsCustomizing())
   {
		DoModal(x, y);
   }
   else
   {
		Create(m_pParentWnd, CPoint(x,y));
   }

   return m_bReturnCmdID ? m_nPostCmdID : FALSE;
}

//
// Field Helpers
//


/////////////////////////////////////////////////////////////////////////////
// CBMenuPopup Virtual Overrides

LRESULT CBMenuPopup::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
   //TRACE("CBMenuPopup::OnUpdateCmdUI\r\n") ;
 //  CCmdTarget::OnUpdateCmdUI(pTarget, bDisableIfNoHndler) ;
   return 0L ;
}

/////////////////////////////////////////////////////////////////////////////
// CBMenuPopup Members

//
//
//
CBMenuItem* CBMenuPopup::GetMenuItem(int index)
{
   ASSERT( index >= 0) ;
   ASSERT( index < m_nMaxIndex) ;
   return m_MenuItems[index] ;
}


//
// Inserts an item before the menu at the index.
// If the index is negative, the item is appended to the end.
// 
void CBMenuPopup::AddMenuItem(int index, CBMenuItem* pMenuItem)
{
	// Add menu item to list.
   ASSERT( pMenuItem != NULL) ;

   if (index < 0)
   {
      m_MenuItems.SetAtGrow(m_nMaxIndex++, pMenuItem) ;
   }
   else
   {
      m_MenuItems.InsertAt(index, pMenuItem) ;
      m_nMaxIndex++ ;
   }
   m_nVisibleItems++;

   AdjustMenuSize(pMenuItem) ;

   // Menu has changed.
   ChangedByOnUpdate(TRUE) ;
}
///////////////////////////////////////////////////////////
//
// HitTest -	Determine which item on the menu contains
//				point pt. If will return HT_MISSED if there
//				the point isn't in any menu item. The point
//				may still be in the menu (ie scrolling area).
//		
//				See PtInMenu.
//
int CBMenuPopup::HitTest(CPoint pt)
{
	// first ensure we are inside menu
	CRect rectItems;
	GetItemsRect(&rectItems);
	if(!rectItems.PtInRect(pt))
	{
		return HT_MISSED;
	}
	
	// OPTIMIZATION Start with current index and ++ and -- first.   
	if ((m_nSelectedMenuItem != c_iNoItem) && 
		(m_MenuItems[m_nSelectedMenuItem]->HitTest(pt, 0)))
	{
		return HT_HITSELECTEDITEM ;
	}
	
	for(int i = 0 ; i < m_nMaxIndex ; i++)
	{
		if (i == m_nSelectedMenuItem) 
		{
			continue ;
		}

		// get offset rect for the item
		CRect rectItem=GetMenuItem(i)->m_rectFields[e_FieldAll];

		if(     rectItem.top>rectItems.top &&           // inside items region
			rectItem.bottom<rectItems.bottom &&     // inside items region
			rectItem.PtInRect(pt))
		{
			ASSERT(m_MenuItems[i]->HitTest(pt, 0));
			return i;
		}
	}

	return HT_MISSED ;
}

///////////////////////////////////////////////////////////
//
// PtInMenus -	This function checks to see if the point
//				is in any submenu or the menu bar. It uses
//				the client area.
//
//				This version is not recusive.
//
BOOL CBMenuPopup::PtInMenus(CPoint point)
{
#if 1
	// Mouse is relative to client area, convert to screen coords.
	CPoint ptScreen(point) ;
	ClientToScreen(&ptScreen);

	CRect rectMenu;
	// Check to see if the button was pressed in the menubar.
	if (m_pWndMenuBar != NULL && m_pWndMenuBar->GetSafeHwnd() != NULL)
	{
		// Get the menu bar's client rectangle
		m_pWndMenuBar->GetClientRect(&rectMenu) ;

		// Convet to screen coordinates
		m_pWndMenuBar->ClientToScreen(&rectMenu) ;
		if (rectMenu.PtInRect(ptScreen))
		{
			return TRUE ;
		}
	}

	// Now check to see if the point is in any of the subpopups.
	CBMenuPopup* pNextChildPopup = this ;
	while(pNextChildPopup != NULL)
	{
		if (pNextChildPopup->GetSafeHwnd() != NULL)
		{
			pNextChildPopup->GetWindowRect(&rectMenu) ;
			if (rectMenu.PtInRect(ptScreen))
			{
				return TRUE ;
			}
		}
		pNextChildPopup = pNextChildPopup->m_pChildPopup ;
	}
	return FALSE ;

#else
	HWND hWnd = ::WindowFromPoint(pt) ;
	if (hWnd == NULL)
	{
		return FALSE ;
	}

	// Check to see if the button was pressed in the menubar.
	if (m_pWndMenuBar != NULL && m_pWndMenuBar->GetSafeHwnd() == hWnd)
	{
		return TRUE ;
	}

	// Now check to see if the point is in any of the subpopups.
	CBMenuPopup* pNextChildPopup = this ;
	while(pNextChildPopup != NULL)
	{
		if (pNextChildPopup->GetSafeHwnd() == hWnd)
		{
			// We have found the window.
			return TRUE ;
		}
		pNextChildPopup = pNextChildPopup->m_pChildPopup ;
	}
	return FALSE ;
#endif
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::CalcMenuSize()
{
	// Zero cached widths.
	m_iHeight = c_iBorderSize*2;
	for (int i = 0 ; i < c_iNumFields ; i++)
	{
		m_iFieldWidths[i] = 0 ;
	}

	// Calc new widths.
	for(i = 0 ; i < m_nMaxIndex ; i++)
	{
		CBMenuItem* pItem = m_MenuItems[i] ;
		AdjustMenuSize(pItem) ;
	}

	// If there are no items, this might have just come about. 
	if(m_nMaxIndex==0)
	{
		// Resize the window
		if (m_hWnd != NULL)
			SetWindowPos(NULL, 0, 0, GetWidth(), GetWindowHeight(), SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE);
	}

}

//
// Checks to see if the toolbar button size has changed.
// If it has, it recalcs the size.
//
void CBMenuPopup::VerifyMenuSize(BOOL bAlways)
{
	if (m_bLargeToolbar != globalData.GetLargeMenuButtons() ||
		bAlways)
	{
		// We menus are now draw for the new size.
		m_bLargeToolbar = globalData.GetLargeMenuButtons();

		// Zero cached widths.
		m_iHeight = c_iBorderSize*2;
		for (int i = 0 ; i < c_iNumFields ; i++)
		{
			m_iFieldWidths[i] = 0 ;
		}


		// Recalc the new sizes for the menu.
		for(i = 0 ; i < m_nMaxIndex ; i++)
		{
			CBMenuItem* pItem = m_MenuItems[i] ;
			pItem->Sync() ; // Force the item to change its size and glyph
			AdjustMenuSize(pItem) ;
		}
	}
}

// MergeMenu
// Merge menu pSection into this menu at nPosition according to nFlags
// [paulde]
//
BOOL CBMenuPopup::MergeMenu(UINT nPosition, UINT nFlags, CBMenuPopup* pSection)
{
	// Get the index of the menu to insert before.
	int index = -1 ;
	if (nFlags & MF_BYPOSITION)
	{
		index = (int)nPosition ;
		if (index >= m_nMaxIndex)
			return FALSE ;
	}
	else
	{
		//BYCOMMAND
		index = GetMenuIndexForCommand(nPosition) ;
		if (index < 0) return FALSE ;
	}

	// insert elements
	pSection->m_MenuItems.SetSize(pSection->m_nMaxIndex); // set size so we don't insert empty items
	m_MenuItems.InsertAt(index, &pSection->m_MenuItems);  // copy pointers from pSection's
	m_nMaxIndex += pSection->m_nMaxIndex;
	m_nVisibleItems+=pSection->m_nMaxIndex;
    // remove all the entries from the source so we don't delete them twice
	pSection->m_MenuItems.RemoveAll();
	pSection->m_nMaxIndex = 0;

	// delete adjacent separators
	for (int i = m_nMaxIndex - 1; i > 0; i--)
	{
		if (CBMenuItem::MIT_Separator == m_MenuItems[i-1]->m_iItemType &&
			CBMenuItem::MIT_Separator == m_MenuItems[i  ]->m_iItemType)
		{
			DeleteMenu(MF_BYPOSITION, i);
		}
	}

	// recalculate size
	CalcMenuSize();

	// Menu has changed.
	ChangedByOnUpdate(TRUE) ;

	return TRUE;
}

//
//
//
//
void CBMenuPopup::AdjustMenuSize(CBMenuItem* pItem)
{
	// In most cases, there will not be a window around
	// when we call this function. However, we still need a DC.
	// First, check to see if we do have a window. If not, use
	// the MainWnd.

	CWnd* pWnd = NULL ;
	if (m_hWnd == NULL)
	{
		pWnd = AfxGetMainWnd() ;
	}
	else
	{
		pWnd = this ;
	}
	CClientDC dc(pWnd) ;

	// Get the widths from the item
	int* ItemWidths;
	pItem->GetWidths(&dc, &ItemWidths) ;

	// Only Name and Accel are variable. Optimization Possible.
	m_iFieldWidths[e_FieldButton] = max(m_iFieldWidths[e_FieldButton], ItemWidths[e_FieldButton]) ;
	m_iFieldWidths[e_FieldName] = max(m_iFieldWidths[e_FieldName], ItemWidths[e_FieldName]) ;
	m_iFieldWidths[e_FieldAccel] = max(m_iFieldWidths[e_FieldAccel], ItemWidths[e_FieldAccel]) ;
	m_iFieldWidths[e_FieldArrow] = max(m_iFieldWidths[e_FieldArrow], ItemWidths[e_FieldArrow]) ;
	m_iFieldWidths[e_FieldGap] = max(m_iFieldWidths[e_FieldGap], ItemWidths[e_FieldGap]) ;

#ifndef _FIELD_ALIGN_ACCEL_
	// Add the name and accel field together so we don't align
	int iNameAccelWidth = ItemWidths[e_FieldName] + ItemWidths[e_FieldAccel] ;
	if (ItemWidths[e_FieldAccel] > 0 ) 
	{
		// Add in the gap if there is an accelerator
		iNameAccelWidth += ItemWidths[e_FieldGap];
	} 	
	m_iFieldWidths[e_FieldNameAccel] = max(m_iFieldWidths[e_FieldNameAccel], iNameAccelWidth);
#endif


	// Get the height from the item
	m_iHeight += pItem->GetHeight(&dc) ;

	// Resize the window
	if (m_hWnd != NULL)
	{
		SetWindowPos(NULL, 0, 0, GetWidth(), GetWindowHeight(), SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE);
		Invalidate();
	 }
}

//
//
//
int CBMenuPopup::GetMenuIndexForCommand(UINT nCmdId) const
{
   for (int i = 0 ; i < m_nMaxIndex ; i++)
   {
      if (m_MenuItems[i]->GetCmdID() == nCmdId)
      {
	 return i ;
      }
   }

   return -1 ;
}
///////////////////////////////////////////////////////////////////
//
// CMenu Similation Functions
//
BOOL CBMenuPopup::CreateMenu() 
{
	return TRUE;
}

BOOL CBMenuPopup::CreatePopupMenu(CWnd* pParentWnd) 
{
	/* Todo, determine popup status at display time! */
	//m_pParentWnd = pParentWnd ;
	//m_bSubPopup = TRUE  ;
	return TRUE;
}


BOOL CBMenuPopup::InsertMenu( UINT nPosition, 
			      UINT nFlags, 
			      UINT nIDNewItem /*= 0*/, 
			      LPCTSTR lpszNewItem /*= NULL*/,
			      CBMenuPopup* pSubPopup /*= NULL*/, 
				  BOOL bTransient /*=FALSE */) 
{
   // Get the index of the menu to insert before.
   int index ;
   if (nFlags & MF_BYPOSITION)
   {
		index = (int)nPosition ;
		if(index==m_nMaxIndex)
		{
		   index=-1;
		}

		if ((index >= m_nMaxIndex) || (index < -1))
		{
			return FALSE ;
		}
	  
	  // A -1 index means append menu. AddMenuItem handles appending.
   }
   else
   {
      //BYCOMMAND
      index = GetMenuIndexForCommand(nPosition) ;
      if (index < 0) return FALSE ;
   }

   // Build the menu items that we are inserting.

   ASSERT(!(nFlags & MF_OWNERDRAW)) ;
   if (nFlags & MF_OWNERDRAW) return FALSE ;
   ASSERT(!(nFlags & MF_BITMAP));
   if (nFlags & MF_BITMAP) return FALSE ;
   ASSERT(!(nFlags & MF_MENUBARBREAK));
   if (nFlags & MF_MENUBARBREAK) return FALSE ;
   ASSERT(!(nFlags & MF_MENUBREAK)) ;
   if (nFlags & MF_MENUBREAK) return FALSE ;

   // Menu has changed.
   // Not required - called by AddMenu... ChangedByOnUpdate(TRUE) ;

   BOOL bSeparator = FALSE ;
   BOOL bResult = FALSE ;
   CBMenuItem* pItem = new CBMenuItem;

   // If the item is a command that's really a menu, get its menu pointer
   CTE *pCTE=theCmdCache.GetCommandEntry(nIDNewItem);
   if(pCTE &&
	   (pCTE->flags & CT_MENU)!=0)
   {
	   //it's really a menu
	   pSubPopup=NULL;

	   nFlags |=MF_POPUP;
   }

   if (MF_SEPARATOR & nFlags)
   {
      bResult = pItem->CreateSeparator() ;
      bSeparator = TRUE ;
   }
   else
	{
	   CString strMenuItem;
	   if(lpszNewItem==NULL)
	   {
			theCmdCache.GetCommandString(nIDNewItem, STRING_MENUTEXT, &lpszNewItem);
			// must be copied
			strMenuItem = lpszNewItem;

			// get the accelerator for this item
			CString strKey;
			if (GetCmdKeyString(nIDNewItem, strKey))
			{
				strMenuItem += CString(_T("\t")) + strKey;
			}

			lpszNewItem=strMenuItem;
	   }

	   if (MF_POPUP & nFlags)
	   {
			ASSERT(lpszNewItem != 0) ;
			// 'real' menu commands are deferred, so that pSubPopup is null till it's needed
			ASSERT(pSubPopup != NULL ||
				(pCTE!=NULL && pCTE->flags & CT_MENU)) ;

			if(pSubPopup)
			{
				bResult = pItem->CreatePopup(this, pSubPopup, lpszNewItem) ;
			}
			else
			{
				bResult = pItem->CreatePopup(this, nIDNewItem, lpszNewItem) ;
			}
	   }
	   else 
	   {
		  // Assume its a string...MF_STRING == 0x0 ;
		  ASSERT(nIDNewItem != 0) ;
		  ASSERT(lpszNewItem != 0) ;
		  bResult = pItem->Create(this, nIDNewItem, lpszNewItem) ;
	   }
   }

   // Insert the menu item.
   if (bResult)
   {
	   if ((MF_CHECKED & nFlags) && !bSeparator)
	   {
		   pItem->Check(TRUE) ;
	   }
	   
	   if ((MF_GRAYED & nFlags) && !bSeparator)
	   {
		   pItem->Enable(FALSE) ;
	   }
	   
	   AddMenuItem(index,pItem) ; // Append to the end.
	   
	   // If the insert asked us to make it transient do so, otherwise, item will take default
	   if(bTransient)
	   {
		   pItem->m_bTransient=TRUE;
	   }
   }
   else
   {
	   // Cleanup...
	   delete pItem ;
   }
   return bResult ;
}

//
//
//
BOOL CBMenuPopup::AppendMenu(  UINT nFlags, 
			      UINT nIDNewItem /*= 0*/, 
			      LPCTSTR lpszNewItem /*= NULL*/,
			      CBMenuPopup* pSubPopup /*= NULL*/)
{
   return InsertMenu((UINT)-1, 
		     nFlags | MF_BYPOSITION,
		     nIDNewItem,
		     lpszNewItem,
		     pSubPopup); 
}

//
//
//
BOOL CBMenuPopup::ModifyMenu( UINT nPosition, 
			      UINT nFlags, 
			      UINT nIDNewItem /*= 0*/, 
			      LPCTSTR lpszNewItem /*= NULL*/,
			      CBMenuPopup* pSubPopup /*= NULL*/) 
{
   // DER WARNING: Only handles ModifyMenu in src/TextView.cpp

   // Get the index of the menu to modify.
   int index = -1 ;
   if (nFlags & MF_BYPOSITION)
   {
      index = (int)nPosition ;
      if (index >= m_nMaxIndex)
	 return FALSE ;
   }
   else
   {
      //BYCOMMAND
      index = GetMenuIndexForCommand(nPosition) ;
      if (index < 0) return FALSE ;
   }

   // Here's some of the things we can't handle

   ASSERT(!(nFlags & MF_OWNERDRAW)) ;
   if (nFlags & MF_OWNERDRAW) return FALSE ;
   ASSERT(!(nFlags & MF_BITMAP));
   if (nFlags & MF_BITMAP) return FALSE ;
   ASSERT(!(nFlags & MF_MENUBARBREAK));
   if (nFlags & MF_MENUBARBREAK) return FALSE ;
   ASSERT(!(nFlags & MF_MENUBREAK)) ;
   if (nFlags & MF_MENUBREAK) return FALSE ;

   int iNewItemType =  CBMenuItem::MIT_Unknown;
   if ((MF_SEPARATOR & nFlags) 
      && (m_MenuItems[index]->GetItemType() != CBMenuItem::MIT_Separator))
   {
      iNewItemType = CBMenuItem::MIT_Separator;
   }
   else if ((MF_POPUP & nFlags)
	    && (m_MenuItems[index]->GetItemType() != CBMenuItem::MIT_Popup))
   {
      iNewItemType = CBMenuItem::MIT_Popup ;
   }
   else if (m_MenuItems[index]->GetItemType() != CBMenuItem::MIT_Command)
   {
      iNewItemType = CBMenuItem::MIT_Command;
   }

   BOOL bResult = FALSE ;
   bResult = m_MenuItems[index]->ModifyItem(nIDNewItem, lpszNewItem, pSubPopup, iNewItemType);

   // Insert the menu item.
   if (bResult)
   {
      BOOL bSeparator = (m_MenuItems[index]->GetItemType() == CBMenuItem::MIT_Separator) ;
      if ((MF_CHECKED & nFlags) && !bSeparator)
      {
	 m_MenuItems[index]->Check(TRUE) ;
      }

      if ((MF_GRAYED & nFlags) && !bSeparator)
      {
	 m_MenuItems[index]->Enable(FALSE) ;
      }

      // Recalculate the size of the menu.
      CalcMenuSize() ;
   }
   return bResult ;

}

//
//
//
BOOL CBMenuPopup::DeleteMenu(UINT nPosition, UINT nFlags)
{
   int index = -1 ;
   if (nFlags & MF_BYPOSITION)
   {
      index = (int)nPosition ;
      if (index >= m_nMaxIndex)
	 return FALSE ;
   }
   else
   {
      //BYCOMMAND
      index = GetMenuIndexForCommand(nPosition) ;
      if (index < 0) 
	 return FALSE ;
   }

   CBMenuItem* pItem = m_MenuItems[index] ;
   if(  pItem->m_bVisible &&
		pItem->GetItemType()!=CBMenuItem::MIT_Separator) // separators don't count to the visible count anyway
   {
	   --m_nVisibleItems;
   }
   pItem->Release() ;
   m_MenuItems.RemoveAt(index) ;
   m_nMaxIndex-- ;
   CalcMenuSize() ;

   // Menu has changed.
   ChangedByOnUpdate(TRUE) ;

   return TRUE ;
}

//
//
//
int CBMenuPopup::GetMenuString(UINT nIDItem, 
		  CString& rString, 
		  UINT nFlags) const
{
   int index = -1 ;
   if (nFlags & MF_BYPOSITION)
   {
      index = (int)nIDItem ;
      if (index >= m_nMaxIndex)
	 return 0;
   }
   else
   {
      //BYCOMMAND
      index = GetMenuIndexForCommand(nIDItem) ;
      if (index < 0) 
	 return 0 ;
   }

   rString = m_MenuItems[index]->GetName();
   return rString.GetLength() ;
}

//
//
//

CBMenuPopup* CBMenuPopup::GetSubMenu(int nPos) const
{
   if (nPos < 0 || nPos >= m_nMaxIndex) 
      return NULL;

   if (m_MenuItems[nPos]->GetItemType() != CBMenuItem::MIT_Popup)
   {
      return NULL ;
   }

   return m_MenuItems[nPos]->GetPopup();
}

//
// Doesn't fully act like a Windows menu. Doesn't navigate popups...
// Doesn't do disabled.
//
UINT CBMenuPopup::EnableMenuItem(UINT nIDItem, UINT nFlags)
{
#if _DEBUG
	// Dale, you're a liar. This trace statement didn't correspond to your code. This is typical of 
	// your slipshod attitude. :( In fact, you fell over the crafty Windows people, who defined 
	// MF_ENABLED as 0. Nice people, eh?
   if (nFlags & MF_DISABLED)
      TRACE0("CBMenuPopup::EnableMenuItem doesn't do disable...graying instead.\r\n") ;
#endif

   int index = -1 ;
   if (nFlags & MF_BYPOSITION)
   {
      index = (int)nIDItem ;
      if (index >= m_nMaxIndex)
	 return (UINT)-1;
   }
   else
   {
      //BYCOMMAND
      index = GetMenuIndexForCommand(nIDItem) ;
      if (index < 0) 
	 return (UINT)-1 ;
   }

   return m_MenuItems[index]->Enable((nFlags & MF_DISABLED) ? FALSE : TRUE) ;

}

//
// Doesn't fully act like a Windows menu. Doesn't navigate popups...
// Doesn't do disabled.
//
// nShow is a boolean, possibly or-ed with MF_BYPOSITION
UINT CBMenuPopup::ShowMenuItem(UINT nIDItem, UINT nShow)
{
   int index = -1 ;
   if (nShow & MF_BYPOSITION)
   {
	   nShow &=~MF_BYPOSITION;
      index = (int)nIDItem ;
      if (index >= m_nMaxIndex)
	 return (UINT)-1;
   }
   else
   {
      //BYCOMMAND
      index = GetMenuIndexForCommand(nIDItem) ;
      if (index < 0) 
	 return (UINT)-1 ;
   }

   UINT nResult=m_MenuItems[index]->Show(nShow);

   CalcMenuSize();

   return nResult;
}

//
//
//
UINT CBMenuPopup::GetMenuItemID(int nPos)
{       
	ASSERT(nPos >= 0) ;
	ASSERT(nPos < m_nMaxIndex) ;

	switch(m_MenuItems[nPos]->GetItemType())
	{
	case CBMenuItem::MIT_Separator:
		return 0 ;
	case CBMenuItem::MIT_Popup:
		return -1;
	case CBMenuItem::MIT_Command:
		return m_MenuItems[nPos]->GetCmdID() ;
	default:
		ASSERT(0) ;
		return 0 ;
	}

}


///////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!
// class private to this file
BOOL CBMenuPopup::IsCBMenuCmdUI(CCmdUI* pCmdUI)
{
   return (pCmdUI->m_pOther != NULL && 
	   (CBMenuPopup*)pCmdUI->m_pOther->IsKindOf(RUNTIME_CLASS(CBMenuPopup)) ) ;
}


class CBMenuItemCmdUI : public CCmdUI
{
public:
   virtual void Enable(BOOL bOn) ;
   virtual void SetCheck(int nCheck) ;
   virtual void SetText(LPCTSTR lpszText) ;
   virtual void SetRadio(BOOL bOn = TRUE) ;
};

void CBMenuItemCmdUI::Enable(BOOL bOn)
{
   CBMenuPopup* pMenu = (CBMenuPopup*)m_pOther ;
   ASSERT(pMenu->IsKindOf(RUNTIME_CLASS(CBMenuPopup))) ;
   pMenu->GetMenuItem(m_nIndex)->Enable(bOn) ;

   m_bEnableChanged=TRUE;
}

void CBMenuItemCmdUI::SetCheck(int nCheck)
{
   CBMenuPopup* pMenu = (CBMenuPopup*)m_pOther ;
   ASSERT(pMenu->IsKindOf(RUNTIME_CLASS(CBMenuPopup))) ;
   pMenu->GetMenuItem(m_nIndex)->Check(nCheck != 0 ) ; //DER: Indeterminate
}

void CBMenuItemCmdUI::SetText(LPCTSTR lpszText)
{
	// Get pointer to popup menu.
   CBMenuPopup* pMenu = (CBMenuPopup*)m_pOther ;
   ASSERT(pMenu->IsKindOf(RUNTIME_CLASS(CBMenuPopup))) ;

   // Get pointer to menu item and set the text.
   CBMenuItem* pMenuItem = pMenu->GetMenuItem(m_nIndex);
   pMenuItem->SetText(lpszText) ;
   pMenuItem->SetTextModified(TRUE);

   // Adjust the size of the menu for the new text.
   pMenu->CalcMenuSize() ;
}

void CBMenuItemCmdUI::SetRadio(int nCheck)
{
   //DER DO: Should use a dot.
   CBMenuPopup* pMenu = (CBMenuPopup*)m_pOther ;
   ASSERT(pMenu->IsKindOf(RUNTIME_CLASS(CBMenuPopup))) ;
   pMenu->GetMenuItem(m_nIndex)->Check(nCheck != 0 ) ; 
}


//!!!!!!!!!!!!!!!!!!!!!!!!
/////////////////////////////////////////////////////////////////////////////

//
//
//
//static int g_iCount = 0 ;

void CBMenuPopup::InitMenuPopup()
{
#if FIND_SLOW_CMDUI
	UINT nMaxCmdUI=1;
#endif

	//
	//
	// The following code was modified from CMainFrame::OnInitMenuPopup
	//
	//
	if (m_bDropDown)
	{
		MainFrameInitMenuPopup();
	}

	// OnCommandUpdate menu handlers can insert or delete menus. The 
	// code needs to handle this. First, we mark all menus as not having
	// been updated:
	for (int i = 0 ; i < m_nMaxIndex ; i++)
	{
		m_MenuItems[i]->Updated(FALSE) ;
	}
	// Also mark the menu as unchanged:
	ChangedByOnUpdate(FALSE) ;

	//
	//
	// The following code was stolen from MFC's CFrame::OnInitMenuPopup
	//
	//
   CBMenuItemCmdUI state ;
   state.m_pMenu = NULL ;
   state.m_pSubMenu = NULL ;
   state.m_nIndex = 0 ;
   state.m_nIndexMax = m_nMaxIndex ;

	state.m_pOther =  this ;
	state.m_nIndex = 0 ;

	// all items created during this period are transient
	s_bCreateTransients=TRUE;

	BOOL bFinished=FALSE;
	
	// Update each item, but only those that have not already been updated
	while (!bFinished && state.m_nIndex < state.m_nIndexMax)
	{
		CBMenuItem* pItem = m_MenuItems[state.m_nIndex] ;
		state.m_nID = pItem->GetCmdID() ;
		if ((state.m_nID == 0) || (pItem->Updated()) || (!pItem->IsVisible()))
		{
			state.m_nIndex++ ;
			continue ;
		}

		if (!IsCustomizing())
		{
			BOOL bAutoEnable = FALSE;
			
			if(m_bAutoEnable)
			{
				CWnd* pParentWnd = m_pParentWnd;
				while (pParentWnd != NULL)
				{
					if (pParentWnd->IsFrameWnd())
					{
						bAutoEnable = static_cast<CFrameWnd*>(pParentWnd)->m_bAutoMenuEnable;
						break;
					}
					pParentWnd = pParentWnd->GetParent();
				}
			}

			// Mark that the current menu item has been updated.
			pItem->Updated(TRUE) ;

#if FIND_SLOW_CMDUI
			int nTick=::GetTickCount();
#endif

			// Update Item
			state.DoUpdate(AfxGetMainWnd(), bAutoEnable && state.m_nID < 0xF000);

#if FIND_SLOW_CMDUI
			int nNewTick=::GetTickCount();

			UINT nDiffTick=nNewTick-nTick;

			// Trace if it's in the slowest 10%
			if((double) nDiffTick > (double) nMaxCmdUI * 0.9)
			{
				TRACE2("cbmenu@InitMenuPopup: Cmd %d took %d ticks.\n\r", state.m_nID, nDiffTick);
			}

			if(nDiffTick>nMaxCmdUI)
			{
				nMaxCmdUI=nDiffTick;
			}
#endif

			// adjust for menu deletions and additions
			// DoUpdate may change m_nMaxIndex...

			if (ChangedByOnUpdate())
			{
				// The menu has been changed by the on update command.
				// Start at the beginning of the menu and find the first item
				// which has not been updated. Inserted items are treated as updated.

				// Also mark the menu as unchanged:
				ChangedByOnUpdate(FALSE) ;

				int i = 0 ;

				while ((i < m_nMaxIndex) && m_MenuItems[i]->Updated())
				{
					i++ ;
				}

				if (i == m_nMaxIndex)
				{
					// We have done all items, so quit.
					bFinished=TRUE;
					break;
				}

				// i is the next item we should do.
				// Re-adjust the menu items...
				state.m_nIndex = i ;
				state.m_nIndexMax = m_nMaxIndex;
				

				// We don't need to increment the menu item, so..
				continue ;
			}
	   }
		else
		{
			// If the item is transient, then is should be disabled, as transients can't be dragged around.
			if(pItem->m_bTransient)
			{
				state.Enable(FALSE);
			}
			else
			{
				state.Enable(TRUE);
			}
		}

		// On to the next menu item
		state.m_nIndex++ ;
	}   

	// go back to creating normal menu items
	s_bCreateTransients=FALSE;
}

//
// CTestUI
//
// A miniature CCmdUI class which is used to determine whether a
// given submenu should be enabled or disabled (i.e. whether there
// are any enabled commands on the submenu).
//
// DER - 6 Jun 96 This class was copied from CMainFrame to do exactly what it is doing
// here. 
//
class CTestUI2 : public CCmdUI
{
public:
	CTestUI2()
	: m_bEnabled(FALSE)
	{}

	virtual void Enable(BOOL bEnabled = TRUE)
	{
		m_bEnabled = m_bEnabled || bEnabled;
	}

	virtual void SetCheck(int nCheck = 1) // 0, 1 or 2 (indeterminate)
	{}   

	virtual void SetRadio(BOOL bOn = TRUE)
	{}

	virtual void SetText(LPCSTR lpszText)
	{
		m_bEnabled = TRUE;
	}

	BOOL m_bEnabled;
};


//
//
// The following code was modified from CMainFrame::OnInitMenuPopup
//
//

void CBMenuPopup::MainFrameInitMenuPopup()
{
#if 0
   // If this is a system menu, do the default thing.  In order to
   // determine whether it's a SysMenu, however, we also have to check
   // the id of the first item, since maximized MDI child windows will
   // not have bSysMenu set for their system menus.
   // (-1 for menuitem means sub popup) -bm
   int nItem = pPopupMenu->GetMenuItemID(0);
   if (bSysMenu || (nItem != -1 && nItem > 0xf000 && nItem < 0xff00))
   {
      pPopupMenu->EnableMenuItem(SC_CLOSE,
	 !IsCustomizing() ? MF_ENABLED : MF_GRAYED);

      if (IsFullScreen())
      {
	 pPopupMenu->EnableMenuItem(SC_MAXIMIZE, MF_GRAYED);
	 pPopupMenu->EnableMenuItem(SC_MOVE, MF_GRAYED);
	 pPopupMenu->EnableMenuItem(SC_SIZE, MF_GRAYED);
      }
      else
	 ;  // Let the system enable/disable it.

      return;
   }
#endif ;


	// Cycle through all the items.  If it is a submenu, it should
	// be enabled only if it contains at least one enabled menu item.
	// Use the CTestUI class (derived from CCmdUI) to determine this.
	CWnd* pWnd = CWnd::FromHandle(theApp.GetActiveView());

	// also determine if this is the Window menu
	// and if the MDI window list is on the menu

	BOOL bWndMenu = FALSE;
	BOOL bMDIList = FALSE;

	for (int iItem = 0 ; iItem < GetMenuItemCount(); iItem++)
	{
		// This code kind of mixes CMenu style interaction with direct CBMenuItem access.
		CBMenuItem* pMenuItem = m_MenuItems[iItem] ;
		// -1 is a submenu
		if (pMenuItem->GetItemType() == CBMenuItem::MIT_Popup)
		{
			CBMenuPopup* pSubMenu = pMenuItem->GetPopup();
			if(pSubMenu)
			{
				CTestUI2 testUI;
				for (UINT i = 0 ; i < pSubMenu->GetMenuItemCount() ; i++)
				{
					testUI.m_nID = pSubMenu->GetMenuItemID(i);
					if (testUI.m_nID != ID_SEPARATOR)
					{
						if (pWnd != NULL)
							pWnd->OnCmdMsg(testUI.m_nID, CN_UPDATE_COMMAND_UI, (void*)&testUI, NULL);
						if (!testUI.m_bEnabled)
							this->OnCmdMsg(testUI.m_nID, CN_UPDATE_COMMAND_UI, (void*)&testUI, NULL);
					} //if
				} //for

				if (testUI.m_bEnabled)
					EnableMenuItem(iItem, MF_BYPOSITION | MF_ENABLED);
				else
					EnableMenuItem(iItem, MF_BYPOSITION | MF_GRAYED);
			}
			else
			{
				EnableMenuItem(iItem, MF_BYPOSITION | MF_ENABLED);
			}

		} // IF 

		// Get the command ID of the current menu item.
		int nID = m_MenuItems[iItem]->GetCmdID() ; //pPopupMenu->GetMenuItemID(iItem);
		// is this the Windows menu?
		// use ID_WINDOW_CASCADE to determine this
		if (nID == ID_WINDOW_CASCADE)
		{
			bWndMenu = TRUE;
		}
		// is the MDI window list on the menu?
		// 0xff00 is the ID of the first MDI child window menu item
		if (nID == 0xff00)
		{
			bMDIList = TRUE;
		}
	} // for

	// if this is the Window menu, remove More Windows...
	if (bWndMenu)
	{
		// (More Windows... )
		DeleteMenu(ID_WINDOW_LIST_DOCS, MF_BYCOMMAND);

		// only add a separator if there's no MDI window list
		// and a separator hasn't already been added
#if 0
		// !!! Review - needs to be fixed.
		if (!bMDIList && (m_MenuItems[GetMenuItemCount()- 1]->GetItemType() != CBMenuItem::MIT_Separator))
		{
			AppendMenu(MF_SEPARATOR);
		}
#endif

		// add the Doc List menu item to the end of the menu
		LPCTSTR lpszCommand;
		theCmdCache.GetCommandString(ID_WINDOW_LIST_DOCS, STRING_MENUTEXT,
		&lpszCommand);
		CString strMenuItem = lpszCommand;

		// get the accelerator for this item
		CString strKey;
		if (GetCmdKeyString(ID_WINDOW_LIST_DOCS, strKey))
		{
			strMenuItem += CString(_T("\t")) + strKey;
		}
		// add the menu item to the menu
		AppendMenu(MF_ENABLED, ID_WINDOW_LIST_DOCS, strMenuItem);

	
	/*
	Refresh the windows menu. 
	
	My first solution was to implement WM_MDIREFRESHMENU. However, the
	windows menu will get updated about THREE times whenever a window
	is added or removed. Two times is the same as v5. However, I would have
	had to add the third time below is not normal. The problem is that the
	first two times would have been wrong most of the time. Only the last
	one would be correct. Therefore, I skip MDIREFRESHMENU and do the
	right thing once. Anyway, here is the reason that this
	extra WM_MDIREFRESHMENU is sent. 

	Each MDI child window has an ID. This id starts at 0xFF00 and happens
	to be the same ID as the menu associated with it. When an MDI Child 
	Window is deleted, Windows decrements the ID of all MDI child windows 
	with an ID greater than the window being deleted. The window being deleted
	has its id changed to 0xFF00 + #ChildWindows. An internal count of the
	number of child windows is decremented.

	Windows then sends an WM_MDIREFESHMENU message. The code inside of windows
	using the internal count. However, I couldn't find anyway to get the internal 
	count of the number of child windows. If you use GetWindow or EnumWindows,
	you get all of the windows, since the pointers of the windows have not
	changed at this point.

	Btw, WM_MDIREFRESHMENU results into a call to the same internal windows
	function that WM_SETMENU does. 
	*/
	//CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	//ASSERT_KINDOF(CMainFrame, pMainFrame);
	//::SendMessage(pMainFrame->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
	MdiRefreshMenu() ;

   }

   // Default processing to send UPDATE_COMMAND_UI messages.
   //CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, 0, bSysMenu);
}

//
//
//
void MdiRefreshMenu()
{
	// don't refresh it if the window menu hasn't been created
	CBMenuPopup* pWindowsMenu = theCmdCache.GetMenu(IDM_MENU_WINDOW) ;
	
	if (pWindowsMenu == NULL)
	{
		return ;
	}
	
	// [multimon] 7 mar 97 - der - NOT TESTED
	//int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	HWND hwnd = pWindowsMenu->m_hWnd ;
	if (hwnd == NULL)
	{
		hwnd = pWindowsMenu->GetParent()->m_hWnd ;
	}
	ASSERT(hwnd != NULL) ;
	int cxScreen = ::multiGetCXMetrics(hwnd) ;
	
	TEXTMETRIC tm;

	// determine maximum size of menu item we're willing to create
	{
		CDC dc;               
		dc.CreateCompatibleDC(NULL);

		// select the menu font
		CFont *pOldFont=dc.SelectObject(globalData.GetMenuFont(FALSE));

		dc.GetTextMetrics(&tm);

		// ensure that zero wasn't returned for some reason
		if(tm.tmAveCharWidth==0)
		{
			// arbitrary
			tm.tmAveCharWidth=15;
		}

		// clean up context
		dc.SelectObject(pOldFont);
	}

	// menu should be allowed to hold enough characters to fill half the screen
	int nMaxDisplayLength=(cxScreen/2)/tm.tmAveCharWidth;

	// small max len implies large fonts/small screen. Allow item to almost fill screen (90% fill)
	if(nMaxDisplayLength<10)
	{
		nMaxDisplayLength=(((double)cxScreen)*0.9)/(double)tm.tmAveCharWidth;

		ASSERT(nMaxDisplayLength>1);
	}

	// If we don't delete anything, insert at the end.
	UINT indexInsertBefore = (UINT)-1 ;  

	// Delete all of the windows currently listed in the menu.
	UINT index = 0 ;

	while(index < pWindowsMenu->GetMenuItemCount()) 
	{
		UINT id = pWindowsMenu->GetMenuItemID(index) ;
		if (id >= IDM_WINDOWS_BASE && id <= IDM_WINDOWS_LAST)
		{
			pWindowsMenu->DeleteMenu(index, MF_BYPOSITION) ;
			indexInsertBefore = index ;

			// We have just deleted an item from the array.
			// So don't increment the counter.
			continue ;
		}
		else if (id == ID_WINDOW_LIST_DOCS)
		{
			// We always want to insert before the Windows... menu item 
			// if it exists.
			indexInsertBefore = index ;
		}
		// Increment the count only if we haven't deleted something.
		index++ ;
	}

	// Get the list of windows. 
	// This code originated in COpenDocsDlg (opendocs.h|cpp)

	// iterate through all MDI windows except: FILTER_DEBUG | FILTER_PROJECT
	int nFilter = FILTER_PROJECT;
	CPartFrame* pMDIChild = theApp.MDITypedGetActive(nFilter, theApp.m_bWindowMenuSorted);

	// If there's not already a separator before the list of windows, add one
	CBMenuItem *pPrevItem=NULL;
	if(indexInsertBefore==-1)
	{
		int nItem=pWindowsMenu->GetMenuItemCount()-1;
		if(nItem>0)
		{
			pPrevItem=pWindowsMenu->GetMenuItem(nItem);
		}
	}
	else
	{
		pPrevItem=pWindowsMenu->GetMenuItem(indexInsertBefore-1);
	}

	if(pPrevItem->GetItemType()!=CBMenuItem::MIT_Separator)
	{
		pWindowsMenu->InsertMenu(   indexInsertBefore++, 
									MF_BYPOSITION | MF_SEPARATOR) ;
	}


	CString buf ;
	CString strTitle ;
	int iMenuId = IDM_WINDOWS_BASE;
	while (pMDIChild != NULL && iMenuId < IDM_WINDOWS_BASE+theApp.m_nWindowMenuItems)
	{
		// Get the name of this window.
		pMDIChild->GetWindowText(strTitle);
		if(strTitle.GetLength()> nMaxDisplayLength)
		{
			strTitle=strTitle.Left(nMaxDisplayLength)+" ...";
		}

		if(iMenuId - IDM_WINDOWS_BASE + 1<10)
		{
			buf.Format("&%d %s", (iMenuId - IDM_WINDOWS_BASE + 1)%10, strTitle);
		}
		else if (iMenuId - IDM_WINDOWS_BASE + 1==10)
		{
			buf.Format("1&0 %s", strTitle);
		}
		else
		{
			buf.Format("%s", strTitle);
		}

		pWindowsMenu->InsertMenu(   indexInsertBefore++, 
									MF_BYPOSITION | MF_STRING | MF_ENABLED,
									iMenuId++,
									buf, 
									NULL,
									TRUE); // menu item is a transient
		// get the next MDI window
		pMDIChild = theApp.MDITypedGetNext(pMDIChild, nFilter, theApp.m_bWindowMenuSorted);
	}
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::ShowSubPopup(int nItem)
{
	EndDefer();
	HideSubPopup();
	m_pChildPopup = m_MenuItems[nItem]->Expand(TRUE) ;
	m_nExpandedMenuItem=nItem;
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::HideSubPopup()
{
	EndDefer();
	if (m_nExpandedMenuItem != c_iNoItem)
	{
		m_pChildPopup = m_MenuItems[m_nExpandedMenuItem]->Expand(FALSE);
		m_nExpandedMenuItem=c_iNoItem;
	}
}

///////////////////////////////////////////////////////////
//
// finds the next visible item after the one specified
//
int CBMenuPopup::FindNextVisibleItem(int nIndex, BOOL bForward)
{
	int nNextVisible=-1;
	int iItem=nIndex;
	int nOffset=1;

	// determine direction
	if(!bForward)
	{
		nOffset=-1;
	}

	iItem+=nOffset;

	// iterate over menu
	while(  iItem>=0 &&
			iItem<m_nMaxIndex &&    // iterate all items
			nNextVisible==-1)               // until we find the answer
	{
		if(     m_MenuItems[iItem]->m_bVisible!=FALSE &&                                                        // visible item
			m_MenuItems[iItem]->GetItemType() != CBMenuItem::MIT_Separator) // that's not a separator
		{
			nNextVisible=iItem;
		}
		else
		{
			iItem+=nOffset;
		}
	}

	// check if we overran
	if(     iItem>=m_nMaxIndex ||
		iItem<0)
	{
		iItem=-1;
	}

	return iItem;
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::GetItemsRect(CRect *pRectItems)
{
	ASSERT(pRectItems);

	GetClientRect(pRectItems);

	// calculate top and bottom of the visible region
	if(m_bHasTopScroll)
	{
		pRectItems->top+=c_cyScrollAreaHeight;
	}
	if(m_bHasBottomScroll)
	{
		pRectItems->bottom-=c_cyScrollAreaHeight;
	}
}

///////////////////////////////////////////////////////////
//
// This is the easier one. User has caused the menu to be scrolled down, so we do so.
//
void CBMenuPopup::ScrollMenu(BOOL bForward, BOOL bEnd)
{
	ASSERT(m_bScrolling);

	// stop any pending scrolls or submenus
	EndDefer();
	// remove any existing submenus
	HideSubPopup();

	int cyOldTopOffset=m_cyTopOffset;

	// get the rectangle containing the items
	CRect rectOldItems;
	GetItemsRect(&rectOldItems);

	// handle case where we're showing the top item currently, so need to add the scroll arrow at the top
	if(!m_bHasTopScroll && bForward)
	{
		// when there's no top scroll, we should be at the top
		ASSERT(m_cyTopOffset==0);

		// add scroll arrow
		m_bHasTopScroll=TRUE;
		m_cyTopOffset+=c_cyScrollAreaHeight;

		// When we add the scroll arrow, we obscure the first item. Scrolling forward would leave us drawing the second item
		// right under the scroll arrow. But that would leave the user having not scrolled at all, just added an arrow. So we
		// do an extra scroll here
		int m_nOldTopIndex=m_nTopIndex;
		m_nTopIndex=FindNextVisibleItem(m_nTopIndex, bForward);

		// move the top index up to accomodate the new row
		m_cyTopOffset-=(m_MenuItems[m_nTopIndex]->m_rectFields[e_FieldAll].top - 
						m_MenuItems[m_nOldTopIndex]->m_rectFields[e_FieldAll].top);
	}

	if(!m_bHasBottomScroll & !bForward)
	{
		m_bHasBottomScroll=TRUE;

		// When we add the scroll arrow, we obscure the first item. Scrolling forward would leave us drawing the second item
		// right under the scroll arrow. But that would leave the user having not scrolled at all, just added an arrow. So we
		// do an extra scroll here
		int m_nOldTopIndex=m_nTopIndex;
		m_nTopIndex=FindNextVisibleItem(m_nTopIndex, bForward);

		// move the top index up to accomodate the new row
		m_cyTopOffset-=(m_MenuItems[m_nTopIndex]->m_rectFields[e_FieldAll].top - 
						m_MenuItems[m_nOldTopIndex]->m_rectFields[e_FieldAll].top);
	}

	BOOL bFinished=FALSE;

	do
	{
		// check if we've hit the end of the menu
		if(bForward)
		{
			if(GetHeight()+m_cyTopOffset < GetWindowHeight())
			{
				m_bHasBottomScroll=FALSE;
				bFinished=TRUE;
			}
		}
		else
		{
			if(m_nTopIndex==0)
			{
				m_cyTopOffset=0;
				m_nTopIndex=0;
				m_bHasTopScroll=FALSE;
				bFinished=TRUE;
			}
		}

		if(!bFinished)
		{
			// move to next visible item
			int m_nOldTopIndex=m_nTopIndex;
			m_nTopIndex=FindNextVisibleItem(m_nTopIndex, bForward);

			// move the top index up to accomodate the new row
			m_cyTopOffset-=(m_MenuItems[m_nTopIndex]->m_rectFields[e_FieldAll].top - 
							m_MenuItems[m_nOldTopIndex]->m_rectFields[e_FieldAll].top);

			if(!bEnd)
			{
				bFinished=TRUE;

				// check if we've hit the end of the menu
				if(bForward)
				{
					if(GetHeight()+m_cyTopOffset < GetWindowHeight())
					{
						m_bHasBottomScroll=FALSE;
					}
				}
				else
				{
					if(m_nTopIndex==0)
					{
						m_cyTopOffset=0;
						m_nTopIndex=0;
						m_bHasTopScroll=FALSE;
					}
				}
			}
		}
	} while(!bFinished);

	// redraw the whole menu REVIEW: Optimize.
	Invalidate();
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::ScrollMenuDown()
{
	ScrollMenu(TRUE, FALSE);
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::ScrollMenuUp()
{
	ScrollMenu(FALSE, FALSE);
}

///////////////////////////////////////////////////////////
//
// to first item at top
//
void CBMenuPopup::ScrollMenuTop()
{
	ScrollMenu(FALSE, TRUE);
}

///////////////////////////////////////////////////////////
//
// to last item at bottom
//
void CBMenuPopup::ScrollMenuBottom()
{
	ScrollMenu(TRUE, TRUE);
}

///////////////////////////////////////////////////////////
//
// make selected menu item scroll into view
// should only have to move by one, cos this is in response to an up or down key
//
void CBMenuPopup::EnsureSelectionScrolled()
{
	if(m_bScrolling && m_nSelectedMenuItem!=c_iNoItem)
	{
		if(m_nSelectedMenuItem<m_nTopIndex)
		{
			ScrollMenuUp();
		}
		else
		{
			if(m_nSelectedMenuItem>0)
			{
				CRect rectItems;
				GetItemsRect(&rectItems);
				int nPreviousItem=FindNextVisibleItem(m_nSelectedMenuItem, FALSE);
				CRect rectSelected=GetItemRect(m_nSelectedMenuItem);
				CRect rectPrevious=GetItemRect(nPreviousItem);
				int nError=rectSelected.top-rectPrevious.bottom;
				rectSelected.bottom-=nError;
				if(rectSelected.bottom>rectItems.bottom)
				{
					ScrollMenuDown();
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::DeferShowSubPopup(int nItem)
{
	EndDefer();
	m_nCurrentTimer=SetTimer(c_iExpandTimerID,globalData.nMenuShowDelay,NULL);
	if(!m_nCurrentTimer)
	{
		ShowSubPopup(nItem);
	}
	else
	{
		m_nExpandDeferredItem=nItem;
	}
}

void CBMenuPopup::DeferHideSubPopup()
{
	EndDefer();
	m_nCurrentTimer=SetTimer(c_iContractTimerID,globalData.nMenuShowDelay,NULL);
	if(!m_nCurrentTimer)
	{
		HideSubPopup();
	}
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::DeferScrollMenuDown(BOOL bInitial)
{
	if(m_nCurrentTimer!=c_iScrollDownTimerID)
	{
		EndDefer();
		m_nCurrentTimer=SetTimer(c_iScrollDownTimerID,bInitial? c_nInitialScrollDelay : c_nScrollDelay,NULL);
		if(!m_nCurrentTimer)
		{
			// REVIEW: Need to delay somehow. Yuk!
			ScrollMenuDown();
		}
	}
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::DeferScrollMenuUp(BOOL bInitial)
{
	if(m_nCurrentTimer!=c_iScrollUpTimerID)
	{
		EndDefer();
		m_nCurrentTimer=SetTimer(c_iScrollUpTimerID,bInitial? c_nInitialScrollDelay : c_nScrollDelay,NULL);
		if(!m_nCurrentTimer)
		{
			// REVIEW: Need to delay somehow. Yuk!
			ScrollMenuUp();
		}
	}
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::EndScrollDefer(void)
{
	if(     m_nCurrentTimer==c_iScrollUpTimerID || 
		m_nCurrentTimer==c_iScrollDownTimerID)
	{
		// clever compiler could drop thru here...
		EndDefer();
	}
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::EndDefer(void)
{
	if(m_nCurrentTimer!=0)
	{
		KillTimer(m_nCurrentTimer);
		m_nCurrentTimer=0;
	}
}


///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::ForwardEndKey()
{
	if (m_nSelectedMenuItem != c_iNoItem)
	{
		m_MenuItems[m_nSelectedMenuItem]->Select(FALSE);

		// invalidate old item and
		InvalidateItem(m_nSelectedMenuItem);
	}

	if(m_bScrolling)
	{
		ScrollMenuBottom();
	}

	int nNextMenuItem = m_nMaxIndex-1; // ok to select disabled

	m_MenuItems[nNextMenuItem]->Select(TRUE);
	// change item and invalidate that
	m_nSelectedMenuItem = nNextMenuItem ;
	InvalidateItem(m_nSelectedMenuItem);
	UpdateWindow();
}

///////////////////////////////////////////////////////////
//
//
//
void CBMenuPopup::ForwardHomeKey()
{
   int nNextMenuItem = 0 ; // ok to select disabled

   if (m_nSelectedMenuItem != c_iNoItem)
   {
		m_MenuItems[m_nSelectedMenuItem]->Select(FALSE) ;
		InvalidateItem(m_nSelectedMenuItem);
   }

	if(m_bScrolling)
	{
		ScrollMenuTop();
	}

   m_MenuItems[nNextMenuItem]->Select(TRUE);

   // change item and invalidate that
   m_nSelectedMenuItem = nNextMenuItem ;
   InvalidateItem(m_nSelectedMenuItem);
   UpdateWindow();
}

//
//
//
void CBMenuPopup::ForwardDownKey()
{
	// Precondition
	if (m_nVisibleItems == 0)
	{
		return ;
	}

   int nNextMenuItem = m_nSelectedMenuItem + 1 ; // Okay, if no item selected.
   if (nNextMenuItem == m_nMaxIndex)
   {
      nNextMenuItem = 0 ;
	}

   while (m_MenuItems[nNextMenuItem]->GetItemType() == CBMenuItem::MIT_Separator ||
	      m_MenuItems[nNextMenuItem]->m_bVisible==FALSE)
   {
      nNextMenuItem++ ;
//      if (nNextMenuItem == m_nSelectedMenuItem)
//         return ; // In case all menu items are separators. 
		  // However, separators cannot be selected...

     if (nNextMenuItem == m_nMaxIndex)
	  {
	 nNextMenuItem = 0 ;
	  }
   }

   m_MenuItems[nNextMenuItem]->Select(TRUE);
   if (m_nSelectedMenuItem != c_iNoItem)
   {
      m_MenuItems[m_nSelectedMenuItem]->Select(FALSE) ;
   }
   // invalidate old item and
   if(m_nSelectedMenuItem!=c_iNoItem)
   {
		InvalidateItem(m_nSelectedMenuItem);
   }

	// change item and invalidate that
	m_nSelectedMenuItem = nNextMenuItem;
	// make sure that the selected menu item is scrolled onto the screen
	EnsureSelectionScrolled();

	InvalidateItem(m_nSelectedMenuItem);
	UpdateWindow();

}
//
//
//
void CBMenuPopup::ForwardUpKey()
{
	// Precondition
	if (m_nVisibleItems == 0)
	{
		return ;
	}

	int nNextMenuItem = m_nSelectedMenuItem - 1 ; // Okay, if no item selected.
	if (nNextMenuItem < 0)
	{
		nNextMenuItem = m_nMaxIndex - 1 ;
	}

   while (m_MenuItems[nNextMenuItem]->GetItemType() == CBMenuItem::MIT_Separator ||
	      m_MenuItems[nNextMenuItem]->m_bVisible==FALSE)
   {
      nNextMenuItem-- ;
      if (nNextMenuItem < 0)
      {
	 nNextMenuItem = m_nMaxIndex - 1 ;
      }
   }

   m_MenuItems[nNextMenuItem]->Select(TRUE);
   if (m_nSelectedMenuItem != c_iNoItem)
   {
      m_MenuItems[m_nSelectedMenuItem]->Select(FALSE) ;
	   InvalidateItem(m_nSelectedMenuItem);
   }

   m_nSelectedMenuItem = nNextMenuItem ;

	// make sure that the selected menu item is scrolled onto the screen
	EnsureSelectionScrolled();

   InvalidateItem(m_nSelectedMenuItem);
   UpdateWindow();
}
//
//
//
BOOL CBMenuPopup::ForwardRightKey()
{
	BOOL bPassToBar=FALSE;
	if(m_nSelectedMenuItem!=c_iNoItem)
	{
		int itemType = m_MenuItems[m_nSelectedMenuItem]->GetItemType();
		// Selected Item Should never be a separator.
		ASSERT(itemType != CBMenuItem::MIT_Separator) ; 

		if (itemType == CBMenuItem::MIT_Command)
		{
			// move to next item on menu bar, if such exists
			bPassToBar=TRUE;
		}
		else if (itemType == CBMenuItem::MIT_Popup)
		{
			if (!m_MenuItems[m_nSelectedMenuItem]->m_bGrayed)
			{
				InvalidateItem(m_nSelectedMenuItem);
				UpdateWindow();
				ShowSubPopup(m_nSelectedMenuItem);
			}
			else
			{
				// If the menu item is not enabled, pass it on to the menu bar as if its a command.
				bPassToBar=TRUE ;
			}
		}
	}
	else
	{
		// nothing is selected, and we got the right key. Someone is probably trying to move to 
		// this submenu having expose it with the mouse.
		ForwardDownKey();
	}

	return bPassToBar;
}

//
// CBMenuPopup - 
//    Returns the command to run. Pass command to Done, if its non-zero.
//
UINT CBMenuPopup::ForwardReturnKey()
{
	// might be called when no item is selected
	if(m_nSelectedMenuItem!=c_iNoItem)
	{
	   int itemType = m_MenuItems[m_nSelectedMenuItem]->GetItemType() ;
	   ASSERT(itemType != CBMenuItem::MIT_Separator) ;

	   if (itemType == CBMenuItem::MIT_Command)
	   {
		  return m_MenuItems[m_nSelectedMenuItem]->Click();
	   }
	   else if (itemType == CBMenuItem::MIT_Popup)
	   {
		   if(m_nExpandedMenuItem!=c_iNoItem)
		   {
			   HideSubPopup();
		   }
		   else
		   {
			   ShowSubPopup(m_nSelectedMenuItem);
		   }

			InvalidateItem(m_nSelectedMenuItem);
			UpdateWindow();
	   }
	}
	return 0;
}

//
// This function handles moving from one menu item to
// the next by typing the first letter.
//
//
UINT CBMenuPopup::ForwardCharKeyDown(TCHAR keychar, BOOL *pbUseCmd)
{
   UINT nCmdID = 0;
   ASSERT(pbUseCmd);
   *pbUseCmd=FALSE;
   int iNewSelection=-1;

   // Code here was copied from barcust.cpp; no way for these two to share code, as yet.

	// this is the case where we need to scan for accelerators - nasty, because we
	// have to scan for all that menu related crap, and we have to be sure that it'll 
	// work right on J systems, etc.

	// davidga tells me (martynl) that accelerators can only ever be sbcs, even in an
	// mbcs environment, which makes things somewhat simpler

	// we scan the menu for one full iteration, taking note of the number of times
	// we meet this accelerator, the first time we meet it, and the first time
	// we meet it when iButton is greater than the current iButton. If there is 
	// exactly one instance of this accelerator, then we drop it down. If there is
	// more than one, then we move to the next one after the current position,
	// wrapping around if appropriate.

	// martynl 24May96, and 27 Jun 96

	int iFirstFind=-1;  // first one (nearest start of bar)
	int iNextFind=-1;   // first one after current item (can be same as iFirstFind)
	int nFinds=0;       // total number of finds
	int nDisabledFinds=0; // number of finds which were disabled

	// Map numpad presses to normal presses
	if(LOBYTE(keychar)>=VK_NUMPAD0 && LOBYTE(keychar)<=VK_NUMPAD9)
	{
		// VK_0 === '0' etc
		keychar=HIBYTE(keychar)+LOBYTE(keychar)-VK_NUMPAD0+'0'; 
	}

	// this loop will terminate early once iNextFind is found - any button
	// after that would never get selected this tiem anyway
	for(int iButton=0; iButton<m_nMaxIndex && (iNextFind==-1 || nFinds<2); ++iButton)
	{
		if(m_MenuItems[iButton]->m_bVisible)
		{
			CString strLabel = m_MenuItems[iButton]->GetName() ;
			if (!strLabel.IsEmpty())
			{
				TCHAR chAccel=GLOBAL_DATA::ExtractAccelerator(strLabel);
				
				if(chAccel!=0)
				{
					if(tolower(chAccel)==tolower(LOBYTE(keychar)))
					{
						 if(m_MenuItems[iButton]->m_bGrayed)
						 {
							 ++nDisabledFinds;
						 }
						 else
						 {
							// same key - this is one
							++nFinds;

							if(iFirstFind==-1)
							{
								iFirstFind=iButton;
							}

							if(iNextFind==-1 &&
								iButton> m_nSelectedMenuItem)
							{
								iNextFind=iButton;
							}
						 }
					}
				}
			}
		}
	}

	// now know how many matches
	if(nFinds>0)
	{
		ASSERT(iFirstFind!=-1);
		// some matches, some action
		if(nFinds==1)
		{
			if(m_MenuItems[iFirstFind]->GetItemType()!=CBMenuItem::MIT_Popup)
			{
				// if it's a command, do it
				nCmdID = m_MenuItems[iFirstFind]->GetCmdID();
				*pbUseCmd=TRUE;
			}
			else
			{
				// if it's a submenu, show it
				ShowSubPopup(iFirstFind);
			}
			iNewSelection=iFirstFind;

		}
		else
		{
			// if there's one just after us, go for that
			if(iNextFind!=-1)
			{
				iNewSelection=iNextFind;
			}
			else
			{
				// otherwise, wrap around. 

				// Because we found more than one valid possibility, and because
				// iNextFind is -1, there must
				// be one other selection before iNextFind.
				iNewSelection=iFirstFind;
			}
		}
	}
	else
	{
		// if we hit a disabled item, we close the menu
		if(nDisabledFinds>0)
		{
			nCmdID=0;
			*pbUseCmd=TRUE;
		}
	}

	if(iNewSelection!=-1)
	{
		m_MenuItems[iNewSelection]->Select(TRUE);
		if (m_nSelectedMenuItem != c_iNoItem)
		{
			m_MenuItems[m_nSelectedMenuItem]->Select(FALSE) ;
			InvalidateItem(m_nSelectedMenuItem);
		}
		m_nSelectedMenuItem = iNewSelection;

		// make sure that the selected menu item is scrolled onto the screen
		EnsureSelectionScrolled();

		InvalidateItem(m_nSelectedMenuItem);
		UpdateWindow();
	}

	return nCmdID;
}


//
//
//
CBMenuPopup* CBMenuPopup::FindLastChild()
{
	CBMenuPopup* pLastChild = this;      
	while (pLastChild->m_pChildPopup != NULL)
	{
		pLastChild = pLastChild->m_pChildPopup ;
	}
	return pLastChild ;
}


//
//
//
CBMenuPopup* CBMenuPopup::FindNextToLastChild()
{
	CBMenuPopup* pLastChild = this;      
	CBMenuPopup* pNextToLastChild = this ;
	while (pLastChild->m_pChildPopup != NULL)
	{
		pNextToLastChild = pLastChild ;
		pLastChild = pLastChild->m_pChildPopup ;
	}
	return pNextToLastChild ;
}
///////////////////////////////////////////////////////////////////////////////
//
//					Modal Message Loop Section
//
BOOL CBMenuPopup::ContinueModal()
{
	return m_bContinueModal ;
}

/////////////////////////////////////////////////////////////////////////////
//
//	DoModel
//
// DoModal is copied from CPropertySheet, but HEAVILY modified.
//
void CBMenuPopup::DoModal(int x, int y) 
{
	ASSERT_VALID(this);
	ASSERT(m_hWnd == NULL);

	// no menu, no dice
	if(m_nMaxIndex==0)
	{
		return ;
	}

	// MSL - now allows reinit
	m_bContinueModal=TRUE;

	// find parent HWND
	ASSERT(m_pParentWnd != NULL) ;

	HWND hParentWnd = _SushiGetSafeOwner(m_pParentWnd);

	HWND hWndCapture = ::GetCapture();
	if (hWndCapture != NULL)
	{
		::SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);
	}

	// Hide caret
	CWnd *pWndFocus=GetFocus();
	if(pWndFocus)
	{
		pWndFocus->HideCaret();
	}

	if (!Create(m_pParentWnd, CPoint(x,y)) )
	{
		return ;
	}

	// disable the IME inside the menu
	imeEnableIME( NULL, FALSE );

	// setup for modal loop and creation
	SetCapture() ;

	// Get/Set Cursor
	HCURSOR hCursor = ::GetCursor() ;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	// Signal that we are entering the menu loop.
	AfxGetMainWnd()->SendMessage(WM_ENTERMENULOOP, !m_bDropDown, 0) ;

	// Signal that we are opening a popup menu.
	AfxGetMainWnd()->SendMessage(WM_MENUSELECT, MAKELONG(0, MF_POPUP), NULL) ;

	// If from keyboard, select the first item
	if(m_bSelectFirstItem)
	{
		SelectFirstItem();
	}

	// Pump the messages.
	MessageLoop();

	::SetCursor(hCursor);

	// hide the window before enabling parent window, etc.
	if (m_hWnd != NULL)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}
	if(GetCapture()==this)
	{
		::ReleaseCapture();
	}

	// cleanup
	DestroyWindow();

	// Signal that we are closing the menu. 
	// Sub popups don't send this message. 
	AfxGetMainWnd()->SendMessage(WM_MENUSELECT, MAKELONG(0, 0xFFFF), NULL) ;

	// Signal that we are done with the modal loop.
	AfxGetMainWnd()->SendMessage(WM_EXITMENULOOP, !m_bDropDown, 0) ;

	// Send the command.
	if(m_nPostCmdID != 0 && !m_bReturnCmdID)
	{
		// Only send the command if 
		//   1) It is NOT 0. 
		//   2) The caller wants it sent and doesn't want the command choosen.
		MyPlaySound("MenuCommand", NULL, SND_ASYNC | SND_NODEFAULT) ;
		m_pParentWnd->PostMessage(WM_COMMAND, m_nPostCmdID);
	}

	// enable the IME 
	imeEnableIME( NULL, TRUE );

	// reshow the caret
	if(pWndFocus)
	{
		pWndFocus->ShowCaret();
	}

	// We may have clicked outside of the menu and we need to forward the mouse
	// down message to the window with the point in it. 
	// We will do this regardless of the state of m_nPostCmdID or m_bReturndCmdId.
	// If we have a forward message, we should forward it...
	if (m_pForwardMsg != NULL)
	{
		ASSERT(m_nPostCmdID == 0) ;

		// Forward the mouse down or other messages on to the application.
		m_pForwardMsg->Send(hWndCapture) ;

		delete m_pForwardMsg ;
		m_pForwardMsg = NULL ;
	}

	return;
}

void CBMenuPopup::Done(UINT nCmdID /*= NULL*/) 
{
	ASSERT(!m_bSubPopup) ; 
	m_nPostCmdID = nCmdID; 
	m_bContinueModal = FALSE;

	if (IsCustomizing())
	{
		DestroyWindow();
	}

	if(m_pChildPopup)
	{
		HideSubPopup();
	}

	if (m_pWndMenuBar != NULL)
	{
		// nasty !!! temp fix. Tell the menu bar to give up on tracking
		CCustomBar *pBar=(CCustomBar *)m_pWndMenuBar;

		pBar->MenuDone();
	}
}


///////////////////////////////////////////////////////////
//
//	MessageLoops - This is the popup menu/menu bar's message pump
//
void CBMenuPopup::MessageLoop()
{
	MSG msg;
	MSG* pMsg = &msg ;

	ASSERT(::IsWindow(m_hWnd)); // window must be created
	ASSERT(!IsCustomizing());
	ASSERT_VALID(this);

	HWND hWndParent = ::GetParent(m_hWnd);
	BOOL bSendEnterIdle = TRUE ;

	while(ContinueModal())
	{
		//
		// Part 1: If there are no messages then wait.
		//

		// Peek at the message. Do Not remove it.
		if (!::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (bSendEnterIdle)
			{
				// There are no messages. Do idle processing.
				if (hWndParent != NULL)
				{
					::SendMessage(hWndParent, WM_ENTERIDLE, MSGF_MENU, (LPARAM)m_hWnd);
					bSendEnterIdle = FALSE ;
				}
			}
			else
			{
				// Wait until there is a message in the queue
				bSendEnterIdle = TRUE ; 
				WaitMessage() ;
			}

			// Restart the loop.
			continue ;
		}

		//
		// Part 2: Check for certain messages WITHOUT REMOVING them from the queue.
		//

		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		//case WM_NCLBUTTONDOWN:
		//case WM_NCRBUTTONDOWN:
			// Check to see if the point is in the menu. If is is we continue as normal.
			// If it is not in the window, we get out of here without eating the message.
			// Windows should then send the message to the correct window.
			{
				CPoint pt((DWORD)pMsg->lParam);
				if (!PtInMenus(pt))
				{
					Done() ; // Kind of funny that we are done, but continuing :-)
					continue ;
				}
			}
			break;

		case WM_QUIT:
			Done();		
			continue ;
			break ;
		};

		//
		// Part 3: Remove and process the message. If the message is a keyboard key, steal
		//			it because we do not have the focus.
		//

		// Make sure that we pull off the same message we peeked at above.
		::PeekMessage(pMsg, NULL, pMsg->message, pMsg->message, PM_REMOVE) ;

		// Let's steal the keyboard messages.
		switch(pMsg->message)
		{
		case WM_KEYDOWN:
			OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) ; //AfxSig_vwww
			break ;
		case WM_SYSKEYDOWN:
			OnSysKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) ; //AfxSig_vwww
			break ;
		case WM_SYSKEYUP:
			// Munch Munch
			break ;
		case WM_KEYUP:
			OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) ; //AfxSig_vwww
			break ;
		case WM_SYSCHAR:
		case WM_CHAR:
			TRACE0("CBMenuPopup::MessageLoop: Why are we getting char's?\r\n") ;
			// Munch Munch
			break ;
		case WM_SYSDEADCHAR:
		case WM_DEADCHAR:
			//{REVIEW} Is is okay to eat These chars? or should the be translated?
			TRACE0("CBMenuPopup::MessageLoop: We are eating dead chars?\r\n") ;
			// Munch Munch
			break ;
		case WM_KICKIDLE:
			// Munch Munch
			break ;
		default:
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			break ;
		}
	}

    // Throw in an extra peek here when we exit the menu loop to ensure that the input queue
    // for this thread gets unlocked if there is no more input left for him.
    ::PeekMessage(pMsg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE);

	return ;
}


/////////////////////////////////////////////////////////////////////////////
// 
//						CBMenuPopup message handlers
//
/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
BOOL CBMenuPopup::Create(CWnd* pParentWnd, POINT pt, BOOL bSubPopup /*= FALSE*/, CRect* prectAvoid /*=NULL*/) 
{
	if (pParentWnd != NULL)
		ASSERT_VALID(pParentWnd);   // must have a parent

	// If this is a SubPopup, then reset all of the variables.
	// Non-subpopups are initialized in TrackPopupMenu.
	if (bSubPopup)
	{
		Initialize() ;
	}

	// Set the avoid rectangle.
	if (prectAvoid != NULL)
	{
		m_rectAvoid = *prectAvoid ;
	}

	// Set up members
	m_pParentWnd = pParentWnd ;
	m_bSubPopup = bSubPopup ;

   VerifyMenuSize(FALSE);
   InitMenuPopup() ;

	// Adjust x,y so menu will fit on screen.
   	/* [multimon] 7 mar 97 - der
	The menu avoidance code was modified for the 5x product to support 
	multiple monitors. Instead of using 0 - cxScreen & 0 - cyScreen for
	the screen boundaries, we use the rectMonitor string. Therefore,
	rectMonitor.top replaced 0 and rectmonitor.bottom replaced cyScreen in
	most cases.

	Note, that we do not use:
		::GetMonitorRect(m_pParentWnd->GetSafeHwnd(), &rectMonitor, TRUE) ;
	because this does not really give us control over which monitor things
	are going to appear on. The monitor corresponding to a window is the 
	monitor which contains most of that window. So, just move a window so
	that a little more than half is on one monitor and suddenly that becomes
	the primary monitor. It is better to use a point.
	*/
	CRect rectMonitor ;
	::multiMonitorRectFromPoint(pt, &rectMonitor, c_fWork) ;

	// [multimon] 7 mar 97 - der
	//int cxScreen = ::GetSystemMetrics(SM_CXSCREEN) ;
	//int cyScreen = ::GetSystemMetrics(SM_CYSCREEN) ;

	//NOTE that c?Screen is always positive.
	int cxScreen = abs(rectMonitor.Width()) ;
	int cyScreen = abs(rectMonitor.Height()) ;


   // Did this bit get complicated, or what? :) :)
   CPoint ptCenterSource=m_rectAvoid.CenterPoint();

   BOOL bXBeforeCenter=pt.x<ptCenterSource.x;
   BOOL bYBeforeCenter=pt.y<ptCenterSource.y;

   if(m_rectAvoid.Width()!=0 || m_rectAvoid.Height()!=0)
   {
	   // True if we've already detected and resolved an X direction problem
	   BOOL bXMoved=FALSE;
	   // True if we've already detected and resolved a Y direction problem
	   BOOL bYMoved=FALSE;

	   // Rect which we will skip to overlap - the 2 pixels is the amount of overlap we will tolerate in silence.
	   CRect rectRealAvoid=m_rectAvoid;
	   rectRealAvoid.top+=2;
	   rectRealAvoid.bottom-=2;
	   rectRealAvoid.left+=2;
	   rectRealAvoid.right-=2;

	   // Before we start moving the menu, we should ensure that it has enough theoretical vertical space.

	   // if the menu is currently below the button, rather than to its right
	   if(bXBeforeCenter)
	   {
		   // if the menu won't fit above or below the avoid area, then we need to move it diagonally
		   if(  pt.y+GetHeight() > rectMonitor.bottom &&                  // check below
				pt.y-m_rectAvoid.Height()-GetHeight() < rectMonitor.top) // check above
		   {
			   pt.x+=m_rectAvoid.Width();
			   pt.y-=m_rectAvoid.Height();

			   bXBeforeCenter=FALSE;
		   }
	   }

	   // We want first to attempt to move the button in the direction that will cause it to cross its
	   // center point forward (i.e. down or right); then we handle the other case. The reason for this is that
	   // moving the menu backwards across its center point involves the menu dislocating further, and makes it
	   // hard for the user to continue their expected drag.

	   // There are up to four passes of this loop
	   // 0: Checks and moves in the nice-move direction (described above)
	   // 1: Checks and moves in the not nice move direction
	   // 2: If the above have failed, try to move again in the same direction as 0
	   // 3: If the above have failed, try to move again in the same direction as 1
	   for(int i=0; i<4; ++i)
	   {
		   if((bXBeforeCenter && (i==0 || i==2)) ||
			  (!bXBeforeCenter && (i==1 || i==3)))
		   {
			   // We only move if we haven't moved and we're either in a bad position or at a stage of the game where we're desperate.
				if (!bXMoved &&
					(pt.x + GetWidth() > rectMonitor.right || i>1))
				{
					//Menu too wide. Put pt on the left side.
					pt.x -= GetWidth();

					if(bXBeforeCenter)
					{
						pt.x += m_rectAvoid.Width();
					}
					else
					{
						pt.x -= m_rectAvoid.Width();
					}
					bXMoved=TRUE;
					// MSL ASSERT( pt.x >= 0) ; //Menu too wide for screen!
				}
		   }
		   else
		   {
			   // We only move if we haven't moved and we're either in a bad position or at a stage of the game where we're desperate.
				if (!bYMoved &&
					(pt.y + GetHeight() > rectMonitor.bottom || i>1))
				{
					// Menu too tall for position.
					pt.y -= GetHeight();
					if(bYBeforeCenter)
					{
						pt.y += m_rectAvoid.Height();
					}
					else
					{
						pt.y -= m_rectAvoid.Height();
					}
					bYMoved=TRUE;
					// MSL ASSERT(pt.y >= 0 ) ; // Put in scrolling arrow...
				}
		   }

		   CRect rectTest;

		   // After we've done the initial adjustments, we only continue looping if we've got an intersection between
		   // the avoidance rectangle and the menu rectangle. Because the menu will probably want to slightly overlap
		   // the button that drops it, we 
		   if(i>=1 &&
			   !rectTest.IntersectRect(rectRealAvoid, CRect(pt.x, pt.y, pt.x+GetWidth(), pt.y+GetHeight())))
		   {
			   //we're done - the menu is in place
			   break;
		   }
	   }
	}


   // If the menu is now positioned off screen
	if( (pt.y < rectMonitor.top) || (pt.y+GetHeight() > rectMonitor.bottom))  // is off screen
	{
		if(GetHeight() < cyScreen)                                        // might fit vertically
		{
			if(pt.y < rectMonitor.top)
			{
				pt.y = rectMonitor.top;
			}
			else
			{
				pt.y=rectMonitor.bottom-GetHeight();
			}
			// don't need to check avoid rectangle because, in this case, we can be sure that we won't overlap it. If we would have
			// we would have made the diagonal move before the loop.
		}
		else
		{
			// too tall for the screen, just make sure the top is visible
			pt.y = rectMonitor.top;

			m_bScrolling=TRUE;
			m_bHasBottomScroll=TRUE;
		}
	}

	// [multimon] - 7 mar 97 - der	
	// If the menu is now positioned off screen
	if((pt.x < rectMonitor.left) || (pt.x + GetWidth() > rectMonitor.right) )  // is off screen
	{
		if(GetWidth() < cxScreen)                                         // might fit vertically
		{
			if(pt.x < rectMonitor.left)
			{
				pt.x = rectMonitor.left;
			}
			else
			{
				pt.x = rectMonitor.right - GetWidth();
			}
			// don't need to check avoid rectangle because, in this case, we can be sure that we won't overlap it. If we would have
			// we would have made the diagonal move before the loop.
		}
		else
		{
			// too wide for the screen, just make sure the left is visible
			pt.x = rectMonitor.left ;
		}
	}

	int nStyle=CS_SAVEBITS ;
	
	LPCTSTR szWndClass = AfxRegisterWndClass( nStyle ,LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_BTNFACE + 1), NULL);
	
	DWORD dwStyle = WS_POPUP ; 
	DWORD dwExStyle=0;
	if(!IsCustomizing())
	{
		dwExStyle|= WS_EX_TOPMOST ; 
	}
	
	if (!CWnd::CreateEx(dwExStyle, szWndClass, NULL, dwStyle, 
		pt.x, pt.y, 
		GetWidth(), GetWindowHeight(),
		pParentWnd->GetSafeHwnd(),// Try again m_bSubPopup ? pParentWnd->GetSafeHwnd() : NULL, // BUG 1270
		NULL))
	{
		return FALSE;
	}

	MyPlaySound("MenuPopup", NULL, SND_ASYNC | SND_NODEFAULT) ;

	// To get the focusing set correctly, we should not activate the new
	// window. The window with the current focus, should not lose that focus.
	// We will get the keyboard by stealing the messages in MessageLoop.
	::SetWindowPos(GetSafeHwnd(), HWND_TOP, 
					pt.x, pt.y, 0, 0,
					SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	return TRUE ;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Don't Erase Background
//
BOOL CBMenuPopup::OnEraseBkgnd(CDC* pDC) 
{
   return TRUE ;
	//return CWnd::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	MouseMove
//
void CBMenuPopup::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint ptScreen=point;
	ClientToScreen(&ptScreen);
	int nXMove=ptScreen.x-s_ptLastMove.x;
	int nYMove=ptScreen.y-s_ptLastMove.y;

	if(nXMove>=-2 && nXMove<=2 && nYMove>=-2 && nYMove<=2)
	{
		// NOTE: don't update s_ptLastMove here, otherwise slow movement wouldn't register
		return;
	}
	else
	{
		s_ptLastMove=ptScreen;
	}

	DoMouseMove(nFlags, point); 
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::SelectExpanded(void)
{
	if(m_nExpandedMenuItem!=c_iNoItem)
	{
		EndDefer();

		if(m_nSelectedMenuItem!=m_nExpandedMenuItem)
		{
			if(m_nSelectedMenuItem!=c_iNoItem)
			{
				m_MenuItems[m_nSelectedMenuItem]->Select(FALSE) ;
				InvalidateItem(m_nSelectedMenuItem);
			}
			m_nSelectedMenuItem=m_nExpandedMenuItem;
			m_MenuItems[m_nSelectedMenuItem]->Select(TRUE) ;
			InvalidateItem(m_nSelectedMenuItem);
			UpdateWindow();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::DoScroll(CPoint point, BOOL bInitial)
{
	if(m_bScrolling)
	{
		BOOL bDeferred=FALSE;
		CRect rectClient;
		GetClientRect(&rectClient);

		// if we're in the menu
		if(     point.x>rectClient.left && 
			point.x< rectClient.right)
		{
			// are we in the top part
			if(m_bHasTopScroll)
			{
				if(point.y<c_cyScrollAreaHeight)
				{
					DeferScrollMenuUp(bInitial);
					bDeferred=TRUE;
				}
			}

			// of in the bottom
			if(m_bHasBottomScroll)
			{
				if(point.y > rectClient.bottom-c_cyScrollAreaHeight)
				{
					DeferScrollMenuDown(bInitial);
					bDeferred=TRUE;
				}
			}
		}
		
		if(!bDeferred)
		{
			// if we move out of the scroll area, don't defer any more
			EndScrollDefer();
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
int CBMenuPopup::DoMouseMove(UINT nFlags, CPoint point)
{
	// Suppress the very first move that arrives at a menu, since it will spuriously deflect keyboard control
	if(s_bLastWasKey && !s_bSuppressedMouseMove)
	{
		s_bSuppressedMouseMove=TRUE;
		return cMoveHandled;
	}

	DoScroll(point, TRUE);

	// dropdown's don't track in customization mode
	if(!IsCustomizing())
	{
		if (m_pChildPopup != NULL)
		{
			// Give child popup a chance to handle mouse
			CPoint ptChild(point) ;
			ClientToScreen(&ptChild) ;
			m_pChildPopup->ScreenToClient(&ptChild) ;

			int rv=m_pChildPopup->DoMouseMove(nFlags, ptChild);
			if(rv & cMoveSelectExpanded)
			{
				SelectExpanded();
			}
			if(rv & cMoveHandled)
			{
				// Child handled the mouse move message.
				return rv;
			}
		}
	
		int nMenuIndexHit = HitTest(point) ;

	   // We hit the currently selected item. 
	   if (HT_HITSELECTEDITEM == nMenuIndexHit )
	   {
		  return cMoveHandled + cMoveSelectExpanded ; // Signal done.
	   }
	
		// If we have hit an item, and it is a separator, return that we are done.   
		if ((nMenuIndexHit >=0) && 
		  (m_MenuItems[nMenuIndexHit]->GetItemType() == CBMenuItem::MIT_Separator))
		{
			s_bLastWasKey=FALSE;
			return cMoveHandled + cMoveSelectExpanded ;
		}
	
		//
		// Unselect selected item if it's outside and we're the lowest level menu, or we've hit something in ourselves
		//
		if (m_nSelectedMenuItem != c_iNoItem &&
		   ((!s_bLastWasKey && m_pChildPopup==NULL) || nMenuIndexHit>=0))
		{
			m_MenuItems[m_nSelectedMenuItem]->Select(FALSE);

			if (CBMenuItem::MIT_Popup == m_MenuItems[m_nSelectedMenuItem]->GetItemType())
			{
				// if we've just moved off of the expanded item, defer hiding it for a short while
				if(m_nExpandedMenuItem==m_nSelectedMenuItem)
				{
					DeferHideSubPopup();
				}
			}

			InvalidateItem(m_nSelectedMenuItem);
			m_nSelectedMenuItem = c_iNoItem;
		}

	   // If we have missed, we want the current item unselected.
	   if (HT_MISSED == nMenuIndexHit)
	   {
			// Mouse is not on a menu item. 
			if (m_pWndMenuBar != NULL)
			{
			   // We have been dropped down by a "menu bar".
			   // Therefore, we will forward the mouse message to the menubar.

			   // Covert mouse to client coordinates of the menubar.
				CPoint ptMenuBar(point) ;
				ClientToScreen(&ptMenuBar) ;
				m_pWndMenuBar ->ScreenToClient(&ptMenuBar);
				m_pWndMenuBar->SendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(ptMenuBar.x, ptMenuBar.y));

				CRect rectMenuBar;
				m_pWndMenuBar->GetClientRect(&rectMenuBar);

				if(rectMenuBar.PtInRect(ptMenuBar))
				{
					s_bLastWasKey=FALSE;
				}
				// The menubar is in control of our life in response to this message.
			}
			UpdateWindow();
			// if it's in the child, select expanded
			// if we're in a submenu, then ensure the right items are selected in parents
			CRect rectClient;
			GetClientRect(rectClient);
			if(rectClient.PtInRect(point))
			{
				s_bLastWasKey=FALSE;
				return cMoveHandled + cMoveSelectExpanded ;
			}
			else
			{
				if(s_bLastWasKey)
				{
					return cMoveHandled;
				}
				else
				{
					return cMoveUnhandled;
				}
			}
		}
	
	   //
	   // Catch any error codes.
	   //
	   ASSERT(nMenuIndexHit >= 0) ;

	   if (CBMenuItem::MIT_Command == 
			m_MenuItems[nMenuIndexHit]->GetItemType()) // don't move to greyed items with mouse
	   {
		  m_MenuItems[nMenuIndexHit]->Select(TRUE);
		  m_nSelectedMenuItem = nMenuIndexHit;

		  DeferHideSubPopup();
		  InvalidateItem(m_nSelectedMenuItem);
		  UpdateWindow();
		  return cMoveHandled + cMoveSelectExpanded ;
	   }
		
	   if (CBMenuItem::MIT_Popup
				== m_MenuItems[nMenuIndexHit]->GetItemType())
	   {
		  m_MenuItems[nMenuIndexHit]->Select(TRUE);
      
		  DeferShowSubPopup(nMenuIndexHit);
		 if(m_nSelectedMenuItem!=-1)
		 {
			InvalidateItem(m_nSelectedMenuItem);
		 }
	   
		  m_nSelectedMenuItem = nMenuIndexHit ;
		  InvalidateItem(m_nSelectedMenuItem);
		  UpdateWindow();
		  return cMoveHandled + cMoveSelectExpanded ;
	   }

	   UpdateWindow();
		CRect rectClient;
		GetClientRect(rectClient);
		if(rectClient.PtInRect(point))
		{
			return cMoveHandled + cMoveSelectExpanded ;
		}
		else
		{
			return cMoveUnhandled;
		}
	}
	return cMoveUnhandled;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//[REVIEW] Move the double click checking into MessageLoop?

	// translate to double click if appropriate
	if(s_nLastSystemClick!=0 &&
		m_bPassBackCancel)
	{
		CPoint ptScreen=point;
		ClientToScreen(&ptScreen);
		if(GetMessageTime()-s_nLastSystemClick < globalData.tmDoubleClick &&
			(abs(ptScreen.x-s_ptLastSystemClick.x) < globalData.cxDoubleClick) &&
			(abs(ptScreen.y-s_ptLastSystemClick.y) < globalData.cyDoubleClick))
		{
			// it's not in our bar, so forward the message
			if(m_pForwardMsg)
			{
				delete m_pForwardMsg;
				m_pForwardMsg=NULL;
			}

			m_pForwardMsg = new CForwardMouseMessage(GetCurrentMessage());
			m_pForwardMsg->m_message=WM_LBUTTONDBLCLK;
			ClearSystemClick();
			Done();
			return;
		}
	}

	ClearSystemClick();

	DoLButtonDown(nFlags, point) ;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
BOOL CBMenuPopup::DoLButtonDown(UINT nFlags, CPoint point) 
{
	// First update position.
	DoMouseMove(nFlags, point);
	
	if (IsCustomizing())
	{
		int nIndex = HitTest(point);
		if (nIndex != HT_MISSED)
		{
			if (nIndex == HT_HITSELECTEDITEM)
				nIndex = m_nSelectedMenuItem;
			
			if ((nIndex >=0) && 
				(m_MenuItems[nIndex]->GetItemType() == CBMenuItem::MIT_Separator))
			{
				return FALSE;
			}
			else
			{
				BOOL bExpand=TRUE; // true if we should expand if a drag doesn't happen

				// close any existing subpopup
				if(m_nExpandedMenuItem!=c_iNoItem)
				{
					if(nIndex==m_nExpandedMenuItem)
					{
						// don't expand if we just closed it.
						bExpand=FALSE;
					}

					// deselect the item
					m_MenuItems[m_nExpandedMenuItem]->Select(FALSE);

					HideSubPopup();
				}

				if(m_nSelectedMenuItem!=c_iNoItem)
				{
					// deselect the item
					m_MenuItems[m_nSelectedMenuItem]->Select(FALSE);
					InvalidateItem(m_nSelectedMenuItem);
					UpdateWindow();
					m_nSelectedMenuItem=c_iNoItem;
				}

				// do the move, if appropriate
				ClientToScreen(&point);

				// actually ignored, since menus are never deleted
				BOOL bDeletePending;

				// transient items can't be moved around or selected
				if(!m_MenuItems[nIndex]->m_bTransient)
				{
					CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;
					if(!pCustomizer->DoButtonMove(point, GetProxy(), nIndex, &bDeletePending))
					{
						// if no move happened, we'll try to expand a subpopup if there is one
						if(bExpand &&
							m_MenuItems[nIndex]->GetItemType()==CBMenuItem::MIT_Popup)
						{
							m_MenuItems[nIndex]->Select(TRUE);
      
							m_nSelectedMenuItem = nIndex;
							InvalidateItem(m_nSelectedMenuItem);
							UpdateWindow();

							ShowSubPopup(nIndex);
						}
					}
				}
			}
			return TRUE;
		}
	}
	
	// [TODO] This no longer needs to be recusive.
	if (m_pChildPopup != NULL)
	{
		// Give child popup a chance to handle mouse
		CPoint ptChild(point) ;
		ClientToScreen(&ptChild) ;
		m_pChildPopup->ScreenToClient(&ptChild) ;
		if (m_pChildPopup->DoLButtonDown(nFlags, ptChild))
		{
	 // Point is in a child .
			return TRUE ;
		}
	}
   
	// Button not pressed in child, is it pressed in us?
	CRect rcClient;
	GetClientRect(rcClient);
	if (!rcClient.PtInRect(point))
	{
		if (!m_bSubPopup)
		{
			// MessageLoop should handle clicks outside of our
			// menu. Therefore, we should never get here.
			// The above message is not true, click on a menu dropdown move mouse click on another...ASSERT(0) ;
			TRACE0("*****NOT ON MENU FORWARDING MESSAGE.*****\r\n") ;

			// The main popup is in charge of the lifetime.

			// We have pressed the mouse button outside of our window.
			// We need to quit, but we also need to forward the mouse
			// message on down the chain.

			// First, was the mouse captured, before we started:

			// If the click outside us was in our menu bar, then we don't pass the message on, but just cancel ourselves
			CPoint ptScreen;
			ClientToScreen(&point);
			if( m_pWndMenuBar==NULL ||
				CWnd::WindowFromPoint(point)!=m_pWndMenuBar)
			{
				// it's not in our bar, so forward the message
				if(m_pForwardMsg)
				{
					delete m_pForwardMsg;
					m_pForwardMsg=NULL;
				}

				m_pForwardMsg = new CForwardMouseMessage(GetCurrentMessage()) ;
			}
			Done();
		}
		return FALSE ;
	}

	return TRUE ;
	//CWnd::OnLButtonDown(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
void CBMenuPopup::OnLButtonUp(UINT nFlags, CPoint point) 
{
   UINT nCmdID = DoLButtonUp(nFlags, point) ;
   if (nCmdID)
      Done(nCmdID) ;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
UINT CBMenuPopup::DoLButtonUp(UINT nFlags, CPoint point) 
{
	// First update position.
	DoMouseMove(nFlags, point);

	if (m_pChildPopup != NULL)
	{
		// Give child popup a chance to handle mouse
		CPoint ptChild(point) ;
		ClientToScreen(&ptChild) ;
		m_pChildPopup->ScreenToClient(&ptChild) ;
		UINT nCmdID = m_pChildPopup->DoLButtonUp(nFlags, ptChild);

		if (nCmdID != 0)
		{
			// Child handled the mouse move message.
			return nCmdID ;
		}
	}

	if (HitTest(point) == HT_HITSELECTEDITEM)
	{
		return m_MenuItems[m_nSelectedMenuItem]->Click();
	}

	return 0 ;
	//CWnd::OnLButtonUp(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
void CBMenuPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	KeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
void CBMenuPopup::KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// don't handle any keyboard input in menus during customization
	if(IsCustomizing())
	{
		return;
	}

	s_bLastWasKey=TRUE;

	ASSERT( !m_bSubPopup ) ;
	//[NoSelect 1st] ASSERT() ;

	CBMenuPopup* pChild ;

	//DER DO: Internationalization

	// Only if its a popup.
	TCHAR tChar = (TCHAR)nChar ;
	if ((tChar >= '0') && (tChar <= '9') ||
		(tChar >= 'A') && (tChar <= 'Z') ||
		(tChar >= VK_NUMPAD0) && (tChar <= VK_NUMPAD9))
	{

		if (m_bControl)
		{
			::MessageBeep(0);
			return ;
		}

		 pChild  = FindLastChild() ;
		 BOOL bUseCmd;
		 UINT nCmdID = pChild->ForwardCharKeyDown(tChar, &bUseCmd) ;
		 if (bUseCmd)
		 {
			Done(nCmdID) ;
		 }
	}

   switch (nChar)
   {
   case VK_HOME:
      pChild = FindLastChild() ;
      pChild->ForwardHomeKey() ;
      break;
   case VK_END:
      pChild = FindLastChild() ;
      pChild->ForwardEndKey() ;
      break;
   case VK_DOWN:
      pChild = FindLastChild() ;
      pChild->ForwardDownKey() ;
      break;
   case VK_UP:
      pChild = FindLastChild() ;
      pChild->ForwardUpKey() ;
      break;
   case VK_RIGHT:
      if (m_nSelectedMenuItem != c_iNoItem) //[NoSelect 1st]
      {
			// might be trying to pull out submenu
			pChild = FindLastChild() ;
			if(pChild->ForwardRightKey())
			{
				// Move to next item on the main menu bar if we are a drop down.
				if (m_pWndMenuBar != NULL)
				{
						// We have been dropped down by a "menu bar".
						// Therefore, we will forward the mouse message to the menubar.

						m_pWndMenuBar->SendMessage(WM_KEYDOWN, (WPARAM)VK_RIGHT, NULL); // !!! review: WHat if the bar is vertically docked - how will it respond to this?
						// The menu bar is now in control of our destiny.
				}
			}
      }
	  else
	  {
		  // just forward it to the menu bar
		   if ((m_pWndMenuBar != NULL))         //with no subpopup's?
		   {
			   m_pWndMenuBar->SendMessage(WM_KEYDOWN, (WPARAM)nChar, NULL) ;
		   }
	  }
      break;
   case VK_LEFT:
	  // The user has pressed the left arrow. If we are a drop down menu,
	   // we need to go to the next menu item, if there are no subpopups.
	   if ((m_pWndMenuBar != NULL) &&       // Are we a drop down?
		   (m_pChildPopup == NULL))             //with no subpopup's?
	   {
		   // Send the left key to the menu bar.
		   m_pWndMenuBar->SendMessage(WM_KEYDOWN, (WPARAM)nChar, NULL) ;
	   }
	   else 
	   {

			 pChild = FindNextToLastChild() ;
			 pChild->HideSubPopup() ;
	   }
      break;
   case VK_RETURN:
      {            
	 // If nothing is selected, exit the menu.
	 if (m_nSelectedMenuItem == c_iNoItem) //[NoSelect 1st]
	 {
		  Done(0) ;
	 }
	 else
	 {
	    pChild = FindLastChild() ;
	    UINT nCmdID = pChild->ForwardReturnKey();
	    if (nCmdID)
	    {
	       Done(nCmdID) ;
	    }
	 }
      }
      break;
   case VK_CONTROL:
      m_bControl = TRUE ;
      break ;
   case VK_ESCAPE:
		if (m_pChildPopup == NULL)
		{
			if(m_pWndMenuBar)
			{
				m_bBarFocus=TRUE;
			}
			Done(0);
		}
	    else
		{
		   // If there are subpopups, ESC should close the youngest one.
		   // Find the next to last subpopup.
		   pChild = FindNextToLastChild() ;
		   pChild->HideSubPopup() ;
		}
		break;

   case VK_MENU:
		Done(0); 
		break;
   default:
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
   }
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
void CBMenuPopup::OnSetFocus(CWnd* pOldWnd) 
{
	if (!m_bSubPopup || IsCustomizing())
	{
		CWnd::OnSetFocus(pOldWnd);
	}
	else
	{
		if(pOldWnd)
		{
			pOldWnd->SetFocus();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
void CBMenuPopup::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(IsCustomizing())
	{
		return;
	}

	if (nChar==VK_MENU ||
		(!m_bDropDown && nFlags & (1<<13)))
	{
		// This is an ALT key combination.
		// Get out of here.
		Done(0);
	}
	else
	{
		KeyDown(nChar, nRepCnt, nFlags);
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Create the Menu Popup
//
void CBMenuPopup::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (IsCustomizing())
	{
		int nIndex = HitTest(point);
		if (nIndex != HT_MISSED)
		{
			if (nIndex == HT_HITSELECTEDITEM)
				nIndex = m_nSelectedMenuItem;

		   if ((nIndex >=0) && 
			  (m_MenuItems[nIndex]->GetItemType() == CBMenuItem::MIT_Separator))
		   {
			  return;
		   }
		   else
		   {
				CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;
				pCustomizer->SetSelection(GetProxy(), nIndex);

				ClientToScreen(&point);
				pCustomizer->DisplayButtonMenu(nFlags, point);
		   }
		}
	}
	else
	{
		DoLButtonDown(nFlags, point) ;
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::OnRButtonUp(UINT nFlags, CPoint point) 
{
   UINT nCmdID = DoLButtonUp(nFlags, point) ;
   if (nCmdID)
      Done(nCmdID) ;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   switch (nChar)
   {
   case VK_CONTROL:
      m_bControl = FALSE ;
      break ;   
   default:
	   CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
   }
}

#ifdef _DEBUG
void CBMenuPopup::AssertValid() const
{
	CWnd::AssertValid();

	// TODO
}

void CBMenuPopup::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);

	dc << "Items: " << m_nMaxIndex;
	if(m_nMaxIndex>0)
	{
		CBMenuItem *pItem=(CBMenuItem *)(m_MenuItems[m_nMaxIndex-1]);
		dc << "First item: " << pItem->m_strName;
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::SelectFirstItem()
{
	// Precondition
	if(m_nVisibleItems==0)
	{
		return;
	}

	// Select the first selectable item...
	if (m_nSelectedMenuItem == c_iNoItem)
	{
		int i = 0 ;
		while(m_MenuItems[i]->GetItemType() == CBMenuItem::MIT_Separator ||             // don't select separators
				!m_MenuItems[i]->IsVisible())                                                                   // and don't select invisible items
		{
			i++ ;
			if (i >= m_nMaxIndex) return ; // No selectable items...
		}
		m_nSelectedMenuItem = i;
		m_MenuItems[i]->Select(TRUE) ;
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::PostNcDestroy()
{
	// Deselect the currently selected menu item.
	if (m_nSelectedMenuItem != c_iNoItem)
	{
		m_MenuItems[m_nSelectedMenuItem]->Select(FALSE) ;
	}

	// Does Nothing
	CWnd::PostNcDestroy() ;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
void CBMenuPopup::OnPaint()
{
	m_bPainted=TRUE;

	CPaintDC dc(this);
   
   // Calculate the required dimensions of the popup.
   CRect rectClientArea;
   GetClientRect(rectClientArea);

   // rectClientArea should be relative to 0,0.
   ASSERT((rectClientArea.top == 0) && (rectClientArea.left == 0)) ;

   // Use button colors for menu.
   dc.FillSolidRect(rectClientArea, globalData.clrBtnFace) ; // Does not fill right/bottom edge.

   // Draw Highlights
   // Places border inside rectangle... 
	dc.FillSolidRect( rectClientArea.left+1, rectClientArea.top+1,
		     rectClientArea.Width()-2, 1, globalData.clrBtnHilite);
	dc.FillSolidRect( rectClientArea.left+1, rectClientArea.top+1,
		     1, rectClientArea.Height() - 2, globalData.clrBtnHilite);
	dc.FillSolidRect( rectClientArea.right-2, rectClientArea.top+1,
		     1, rectClientArea.Height() - 1, globalData.clrBtnShadow);
	dc.FillSolidRect( rectClientArea.left+1, rectClientArea.bottom-2, 
		     rectClientArea.Width()-1, 1, globalData.clrBtnShadow);
	dc.FillSolidRect( rectClientArea.right-1, rectClientArea.top,
		     1, rectClientArea.Height()+1, globalData.clr3dDkShadow);
	dc.FillSolidRect( rectClientArea.left, rectClientArea.bottom-1,
		     rectClientArea.Width()+1, 1, globalData.clr3dDkShadow);

	// determine the drawing area
	CRect rectItems;
	GetItemsRect(&rectItems);

	// draw any scrolling details
	if(m_bScrolling)
	{
		// load bitmaps if required
		if(s_bmpScrollDown.m_hObject == NULL)
		{
			HBITMAP hbmScrollDown=GLOBAL_DATA::LoadSysColorBitmap(IDB_MENUSCROLLDOWN);
			ASSERT(hbmScrollDown);
			s_bmpScrollDown.Attach(hbmScrollDown);
		}

		if(s_bmpScrollUp.m_hObject == NULL)
		{
			HBITMAP hbmScrollUp=GLOBAL_DATA::LoadSysColorBitmap(IDB_MENUSCROLLUP);
			ASSERT(hbmScrollUp);
			s_bmpScrollUp.Attach(hbmScrollUp);
		}

		BITMAP bm;

		VERIFY(s_bmpScrollDown.GetObject(sizeof(BITMAP),(void*)&bm)) ;
		s_sizeScroll.cx = bm.bmWidth;
		s_sizeScroll.cy = bm.bmHeight;

		// draw
		CDC dcArrow;
		dcArrow.CreateCompatibleDC( &dc );

		if(m_bHasTopScroll)
		{
			// determine position
			CRect rectTop(0,0,GetWidth(), c_cyScrollAreaHeight);
			CPoint ptCenter=rectTop.CenterPoint();

			// draw it
			CBitmap *pOldBitmap = dcArrow.SelectObject(&s_bmpScrollUp);
			dc.BitBlt(ptCenter.x-s_sizeScroll.cx/2, ptCenter.y-s_sizeScroll.cy/2, s_sizeScroll.cx,s_sizeScroll.cy, &dcArrow, 0, 0, SRCCOPY);

			dcArrow.SelectObject( pOldBitmap );
		}
		if(m_bHasBottomScroll)
		{
			// determine position
			CRect rectBottom(0 ,GetWindowHeight()-c_cyScrollAreaHeight, GetWidth(), GetWindowHeight());
			CPoint ptCenter=rectBottom.CenterPoint();

			// draw it
			CBitmap *pOldBitmap = dcArrow.SelectObject(&s_bmpScrollDown);
			dc.BitBlt(ptCenter.x-s_sizeScroll.cx/2, ptCenter.y-s_sizeScroll.cy/2, s_sizeScroll.cx,s_sizeScroll.cy, &dcArrow, 0, 0, SRCCOPY);

			dcArrow.SelectObject( pOldBitmap );
		}
	}

	// Build a rect for each item.

	// Initialize Temp variables
	int left =  rectClientArea.left+c_iBorderSize ;
	int right = rectClientArea.right-c_iBorderSize ; 
	int bottom = 0;
	int nFirstIndex=m_nTopIndex;

	int top =   rectItems.top+c_iBorderSize ;

   // Build Rectangles
   CRect rectFields[c_iNumFields] ;

   rectFields[e_FieldAll].left = left ;   // Rectangle around whole item.
   rectFields[e_FieldAll].top = top ; 
   rectFields[e_FieldAll].right = right ;
   rectFields[e_FieldAll].bottom = bottom ;

   right = left + m_iFieldWidths[e_FieldButton] ; // right boundary is width of item.
   rectFields[e_FieldButton].left = left ;
   rectFields[e_FieldButton].top = top ;
   rectFields[e_FieldButton].right = right ;
   rectFields[e_FieldButton].bottom = 0;

#ifdef  _FIELD_ALIGN_ACCEL_   
   left = right + m_iFieldWidths[e_FieldGap];     // add gap to last right boundary.
   right = left + m_iFieldWidths[e_FieldName] ;   // right boundary is now moved over by filed width.
   rectFields[e_FieldName].left  = left ;
   rectFields[e_FieldName].top   = top ;
   rectFields[e_FieldName].right = right ;
   rectFields[e_FieldName].bottom= 0;

   left = right + m_iFieldWidths[e_FieldGap];
   right = left + m_iFieldWidths[e_FieldAccel] ;
   rectFields[e_FieldAccel].left  = left ;
   rectFields[e_FieldAccel].top   = top ;
   rectFields[e_FieldAccel].right = right ;
   rectFields[e_FieldAccel].bottom= 0 ;
#else
   // Don't align the accelerators
   left = right + c_iButtonTextGap ;                                    // add gap to last right boundary.
   right = left + m_iFieldWidths[e_FieldNameAccel] ;   // right boundary is now moved over by filed width.
   rectFields[e_FieldName].left  = left ;
   rectFields[e_FieldName].top   = top ;
   rectFields[e_FieldName].right = right ;
   rectFields[e_FieldName].bottom= 0;

   rectFields[e_FieldAccel].left  = left ;
   rectFields[e_FieldAccel].top   = top ;
   rectFields[e_FieldAccel].right = right ;
   rectFields[e_FieldAccel].bottom= 0 ;
#endif

   left = right + c_iButtonTextGap ;
   right = left + m_iFieldWidths[e_FieldArrow] ;
   rectFields[e_FieldArrow].left  = left ;
   rectFields[e_FieldArrow].top   = top ;
   rectFields[e_FieldArrow].right = right ;
   rectFields[e_FieldArrow].bottom= 0 ;

   // Draw the menu items.
   for(int i = nFirstIndex ; i < m_nMaxIndex  && rectFields[e_FieldAll].bottom < rectItems.bottom; i++)
   {
	   CBMenuItem* pItem = m_MenuItems[i] ;
	   int iItemHeight = pItem->GetHeight(&dc) ;

	   // Adjust height if needed.
	   if (iItemHeight  != rectFields[e_FieldName].Height())
	   {
		   rectFields[e_FieldAll].bottom = rectFields[e_FieldAll].top + iItemHeight ;
		   rectFields[e_FieldButton].bottom = rectFields[e_FieldButton].top + iItemHeight ;
		   rectFields[e_FieldName].bottom = rectFields[e_FieldName].top + iItemHeight ;
		   rectFields[e_FieldAccel].bottom = rectFields[e_FieldAccel].top + iItemHeight ;
		   rectFields[e_FieldArrow].bottom = rectFields[e_FieldArrow].top + iItemHeight ;
	   }
	   
	   // Draw
	   pItem->Draw(&dc, rectFields) ;
	   
	   // Move rects down one menu item.
	   rectFields[e_FieldAll].OffsetRect(0, iItemHeight) ;
	   rectFields[e_FieldButton].OffsetRect(0, iItemHeight)  ;
	   rectFields[e_FieldName].OffsetRect(0, iItemHeight) ;   
	   rectFields[e_FieldAccel].OffsetRect(0, iItemHeight);  
	   rectFields[e_FieldArrow].OffsetRect(0, iItemHeight);  
   }

	// draw customizer selection if appropriate
	CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;

	if (IsCustomizing() &&
		pCustomizer->m_pSelectBar->GetRealObject() == this)
	{
		COLORREF crBk = dc.SetBkColor(RGB(0xFF, 0xFF, 0xFF));
		COLORREF crFG = dc.SetTextColor(RGB(0, 0, 0));

		pCustomizer->DrawSelection(&dc);

		dc.SetBkColor(crBk);
		dc.SetTextColor(crFG);
	}

}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
HGLOBAL CBMenuPopup::GetData()
{
	HGLOBAL hData=GetProxy()->GetData();

	return hData;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	
//
BOOL CBMenuPopup::SetData(HGLOBAL hglob)
{
	BOOL bFound=GetProxy()->SetData(hglob);

	return bFound;
}

//////////////////////////////////////////////////////////////////////////
//
//OLD_MENU Support using the old windows menu for accessibility reasons
//
//
namespace Menu
{
	enum eMenuMode
	{
		e_CmdBarMode = 1,
		e_OleBarMode = 2
	};

	static UINT s_MenuMode = e_CmdBarMode ;

	void UseHMENU(BOOL bUseOldWindowsMenu)
	{
		if (bUseOldWindowsMenu)
		{
			s_MenuMode &=~e_CmdBarMode ;
		}
		else
		{
			s_MenuMode |= e_CmdBarMode;
		}
		// Other functions will (possibly) set the e_OleMode flag...
	};

	void UseOLE(BOOL bStart)
	{
		if(bStart)
		{
			s_MenuMode |= e_OleBarMode;
		}
		else
		{
			s_MenuMode &= ~e_OleBarMode;
		}
	}

	BOOL IsInCmdBarMode() 
	{
		return (s_MenuMode & e_CmdBarMode)!=0 ; // any other bits set imply regular menus
	}

	BOOL IsShowingCmdBars() 
	{
		return s_MenuMode == e_CmdBarMode ; // any other bits set imply regular menus
	}

	void UpdateMode(BOOL bUpdateNow)
	{
		CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();

		if(IsShowingCmdBars())
		{
			POSITION pos;
			if(     pFrame &&
				pFrame->m_pManager)
			{
				CDockWorker* pDocker = pFrame->m_pManager->WorkerFromID(MAKEDOCKID(PACKAGE_SUSHI,IDTB_MENUBAR), &pos);

				CASBar* pBar = (CASBar*) pDocker->m_pWnd;
				if (pBar != NULL &&
					pBar->m_nCount==0)
				{
					CPackage* pGrp = theApp.GetPackage(pDocker->m_nIDPackage);
					if (pGrp == NULL)
						return;
					
					HGLOBAL hglob = pGrp->GetToolbarData(LOWORD(pDocker->m_nIDWnd));

					if (hglob != NULL)
					{
						TOOLBARINIT FAR* lptbi = (TOOLBARINIT*) ::GlobalLock(hglob);
						UINT FAR* lpIDArray = (UINT FAR*) (lptbi + 1);

						pBar->DynSetButtons(lpIDArray, lptbi->nIDCount);
						pBar->RecalcLayout();
						
						::GlobalUnlock(hglob);
						::GlobalFree(hglob);
					}
				}
			}
		}
		DkUpdateAvailableWnds(bUpdateNow);
		if(IsShowingCmdBars())
		{
			// HACK ALERT!
			// when setting the menu bar to NULL, we risk loosing the sys menus gadgets
			// so let's make sure that the window is not maxed before switching 
			// menu bars
			BOOL bMaxed;
			CWnd *pParentWnd = NULL;
			CMDIChildWnd *pChildTop = pFrame->MDIGetActive(&bMaxed);
			if(pChildTop)
				pParentWnd = pChildTop->GetParent();

			if(bMaxed)
			{
				if(pParentWnd)
					pParentWnd->SetRedraw(FALSE);
				if(pChildTop)
					pChildTop->MDIRestore();
			}

			pFrame->SetMenu(NULL);

			if(bMaxed)
			{
				if(pChildTop)
					pChildTop->MDIMaximize();
				if(pParentWnd)
					pParentWnd->SetRedraw(TRUE);
			}
		}
		pFrame->RebuildMenus();
	}
};

BOOL CBMenuPopup::ContainsMenu(int nCmdID)
{
	// returns true if the specified command id is contained within this menu or one of its subs
	BOOL bFound=FALSE;
	for(int i = 0 ; i < m_nMaxIndex  && !bFound; i++)
	{
		CBMenuItem* pItem = m_MenuItems[i] ;

		if(pItem->GetItemType()==CBMenuItem::MIT_Popup)
		{
			ASSERT(pItem->GetCmdID()!=0);
			
			if(pItem->GetCmdID()==nCmdID)
			{
				bFound=TRUE;
			}
			else
			{
				CBMenuPopup *pSub=theCmdCache.GetMenu(pItem->GetCmdID());
				ASSERT(pSub);
				bFound=pSub->ContainsMenu(pItem->GetCmdID());
			}
		}
	}
	return bFound;
}

CTypedPtrList<CObList, CBMenuPopup *> CBMenuPopup::s_listMenus;

int CBMenuPopup::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int rv;
	if((rv=CWnd::OnCreate(lpCreateStruct))==0)
	{
		s_listMenus.AddTail(this);

	}
	return rv;
}

void CBMenuPopup::AllocateProxy(void)
{
	ASSERT(m_pProxy==NULL);

	m_pProxy = new CMenuPopupProxy;
	m_pProxy->AttachMenu(this);
}

void CBMenuPopup::OnDestroy( )
{
	EndDefer();

	// deselect inside this menu.
	if(	CASBar::s_pCustomizer &&
		CASBar::s_pCustomizer->m_pSelectBar !=NULL &&
		CASBar::s_pCustomizer->m_pSelectBar ->GetWindow()==this)
	{
		CASBar::s_pCustomizer->SetSelection(NULL,0);
	}

	
	POSITION pos=s_listMenus.Find(this);

	ASSERT(pos);
	if(pos)
	{
		// delete it
		s_listMenus.RemoveAt(pos);
	}

	CWnd::OnDestroy();
}

void CBMenuPopup::FreeProxy(void)
{
	if(m_pProxy)
	{
		delete m_pProxy;
		m_pProxy=NULL;
	}
}

LRESULT CBMenuPopup::OnTestMenu(WPARAM wParam, LPARAM lParam)
{
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	
	return pFrame->SendMessage(DSM_TESTMENU, wParam,lParam);
}

void CBMenuPopup::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
		case c_iExpandTimerID:
			ASSERT(m_nExpandDeferredItem!=c_iNoItem);
			ShowSubPopup(m_nExpandDeferredItem);
			break;

		case c_iContractTimerID:
			HideSubPopup();
			break;

		case c_iScrollDownTimerID:
			ScrollMenuDown();
			DoScroll(GetCurrentMessage()->pt, FALSE);
			break;

		case c_iScrollUpTimerID:
			ScrollMenuUp();
			DoScroll(GetCurrentMessage()->pt, FALSE);
			break;

		default:
			ASSERT(FALSE);
			return;
	}
	// don't need to kill the timers here, as the Show/Hide funcs always kill them
}

void CBMenuPopup::RemoveCommandReferences(UINT id)
{
	// remove any reference to this command id
	for(int i=0; i<m_nMaxIndex; ++i)
	{
		CBMenuItem* pItem = m_MenuItems[i];

		if(pItem &&
			pItem->GetCmdID()==id)
		{
			// REVIEW: Separators need to be handled
			DeleteMenu(i, MF_BYPOSITION);
		}
	}
}

CMenuPopupProxy* CBMenuPopup::GetProxy()
{
	ASSERT(m_pProxy);
	return m_pProxy;
}

int CBMenuPopup::GetHeight()
{
	if(m_nVisibleItems==0)
	{
		return c_iEmptyHeight;
	}
	else
	{
		return m_iHeight;
	}
}

int CBMenuPopup::GetWindowHeight()
{
	int nHeight=GetHeight();

	// [multimon] 7 mar 97 - der
	// Its possible that the window has not been created when
	// we call this function. Therefore, we use the parent window.
	// BUGBUG: Could the parent window be on the wrong monitor?
	int cyScreen = ::multiGetCYMetrics(m_hWnd != NULL ? m_hWnd : m_pParentWnd->GetSafeHwnd()) ;

	if(nHeight<=cyScreen)
	{
		return nHeight;
	}
	else
	{
		return cyScreen;
	}
}

int CBMenuPopup::GetWidth()
{
	if(m_nVisibleItems==0)
	{
		return c_iEmptyWidth;
	}
	else
	{
	#ifdef  _FIELD_ALIGN_ACCEL_   
		return m_iFieldWidths[e_FieldButton] + m_iFieldWidths[e_FieldName] + m_iFieldWidths[e_FieldAccel] +
			m_iFieldWidths[e_FieldArrow] + m_iFieldWidths[e_FieldGap]*3/2; //The separation is 1/2 char width.
	#else
		return m_iFieldWidths[e_FieldButton] + m_iFieldWidths[e_FieldNameAccel] +
			m_iFieldWidths[e_FieldArrow] + c_iButtonTextGap*2; 
					  // The separation for the gap between the the name and the accelerator has already been added.
	#endif
	}
}

afx_msg void CBMenuPopup::OnKillFocus(CWnd *pNew)
{
	CToolCustomizer* pCustomizer = CASBar::s_pCustomizer;
	if (!IsCustomizing())
	{
		// when the app loses focus, we must finish
		if(pNew!=this && !IsChild(pNew) && pNew==NULL || !pNew->IsKindOf(RUNTIME_CLASS(CBMenuPopup)))
		{
			if(m_bSubPopup)
			{
				// let the parent finish
				if(GetParent())
				{
					GetParent()->SendMessage(WM_KILLFOCUS, (WPARAM)(pNew->GetSafeHwnd()), NULL);
				}
			}
			else
			{
				// don't call done if it's already been called
				if(m_bContinueModal)
				{
					Done(0);
				}
			}
		}
	}
}

void CBMenuPopup::InvalidateItem(int nIndex)
{
	InvalidateRect(GetItemRect(nIndex), FALSE);
}

CRect CBMenuPopup::GetItemRect(int nIndex)
{
	CRect rectItems;
	GetItemsRect(&rectItems);

	CRect rectItem=GetMenuItem(nIndex)->m_rectFields[e_FieldAll];

	if(rectItem.top>rectItems.top && rectItem.bottom<rectItems.bottom)
	{
		return rectItem;
	}
	else
	{
		return CRect(0,0,0,0);
	}
}

// Right now, during customization, all menus are being customized apart from the right button context menu.
BOOL CBMenuPopup::IsCustomizing(void)
{
	return CASBar::s_pCustomizer!=NULL &&   // are we customizing
			(m_bDropDown ||                                 // is this a top level dropdown
			m_bSubPopup);                                   // or a submenu on a dropdown
}

// searches for this command in this menu and all submenus
BOOL CBMenuPopup::ContainsMenu(UINT nId)
{
	// iterate over the whole menu
	for(int i = 0 ; i < m_nMaxIndex  ; i++)
	{
		CBMenuItem* pItem = m_MenuItems[i] ;
		
		// look for submenus
		if(pItem->GetItemType()==CBMenuItem::MIT_Popup)
		{
			// if this is the thing we're looking for, that's it
			if(pItem->GetCmdID()==nId)
			{
				return TRUE;
			}
			else
			{
				// if this contains the thing we're looking for, that's it too
				CBMenuPopup *pSubPopup=theCmdCache.GetMenu(pItem->GetCmdID());
				if(pSubPopup)
				{
					if(pSubPopup->ContainsMenu(nId))
					{
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

// implements specific menu cancelling for bars
void CBMenuPopup::HideInvalidMenuDestination(UINT nId)
{
	if(m_nExpandedMenuItem!=c_iNoItem)
	{
		CBMenuItem *pItem=m_MenuItems[m_nExpandedMenuItem];

		ASSERT(pItem);

		if(pItem->GetCmdID()==nId)
		{
			HideSubPopup();
		}
		else
		{
			CBMenuPopup *pMenuTest=theCmdCache.GetMenu(nId);
			if(pMenuTest->ContainsMenu(nId))
			{
				HideSubPopup();
			}
			else
			{
				if(m_pChildPopup)
				{
					m_pChildPopup->HideInvalidMenuDestination(nId);
				}
			}
		}
	}
}

// Flushes all cached menu size information
void CBMenuPopup::FlushSizeCache()
{
	VerifyMenuSize(TRUE);
}

// Call when a click could not be a system double click part 1
void CBMenuPopup::ClearSystemClick(void)
{
	s_nLastSystemClick=0;
	s_ptLastSystemClick.x=s_ptLastSystemClick.y=0;
}

void CBMenuPopup::SetSystemClick(int nTime, CPoint pt)
{
	s_nLastSystemClick=nTime;
	s_ptLastSystemClick=pt;
}

CFont *CBMenuPopup::GetMenuFont(BOOL bDefault)
{
	return globalData.GetMenuFont(bDefault);
}

void CBMenuPopup::SysColorChange(void)
{
	// update all glyphs
	// Flush the size caches on all of the menus and items
	POSITION pos=theCmdCache.GetFirstMenuPosition();
	CBMenuPopup *pMenu;
	UINT nIDMenu;

	while(pos!=NULL)
	{
		theCmdCache.GetNextMenu(pos, nIDMenu, pMenu);

		if(pMenu)
		{
			CBMenuItem *pItem=NULL;

			for(int i=0; i < pMenu->GetMenuItemCount(); ++i)
			{
				pItem=pMenu->m_MenuItems[i];
				if(pItem)
				{
					pItem->UpdateGlyph();
				}
			}
		}
	}

	// update stock bitmaps
	s_bmpScrollUp.DeleteObject();
	s_bmpScrollDown.DeleteObject();

	CBMenuItem::FlushSizeCache();
}

