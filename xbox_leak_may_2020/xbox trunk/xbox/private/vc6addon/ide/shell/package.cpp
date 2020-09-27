// package.cpp : implementation of packages and packets
//

#include "stdafx.h"
#include "main.h"
#include "package.h"
#include "shell.h"

IMPLEMENT_DYNAMIC(CPack, CCmdTarget)
IMPLEMENT_DYNAMIC(CPackage, CPack)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CPack::CPack(CPackage* pPackage, UINT nIDPacket, LPCTSTR lpAccelID)
{
	m_pPackage = pPackage;
	m_nIDPacket = nIDPacket;

	m_lpAccelID = lpAccelID;
}

CPack::~CPack()
{
}

/////////////////////////////////////////////////////////////////////////////
//	CPackage class

CPackage::CPackage(HINSTANCE hInstance, UINT id, UINT flags,
		LPCTSTR lpCmdTableID, LPCTSTR lpBitmapID, LPCTSTR lpAccelID,
		LPCTSTR lpLargeBitmapID)
	: CPack(NULL, PACKET_NIL, lpAccelID)
{
	m_pPackage = this;
	m_hInstance = hInstance;
	m_id = id;
	m_flags = flags;

	m_lpBitmapID = lpBitmapID;
	m_lpLargeBitmapID = lpLargeBitmapID;
	m_hBitmap = NULL;
	m_bLoadedBitmapLarge=FALSE;

	if (lpCmdTableID != NULL)
		theCmdCache.AddCommandResource(this, lpCmdTableID);
}

extern "C" typedef void (FAR PASCAL* EXITPACKAGEPROC)();

CPackage::~CPackage()
{
	if (m_hBitmap != NULL)
		::DeleteObject(m_hBitmap);

	if (m_hInstance != NULL)
	{
		static char BASED_CODE szExitPackage[] = _TEXT("ExitPackage");

		EXITPACKAGEPROC lpfn = (EXITPACKAGEPROC)GetProcAddress(m_hInstance, szExitPackage);
		if (lpfn != NULL)
			(*lpfn)();
		
		// We used to call FreeLibrary() here, but that's bad.
		// The problem is that someone might have subclassed
		// CPackage, in which case this destructor will return
		// to the subclass's destructor -- which is in code that
		// no longer exists!
		//
		// In fact, it is quite dangerous to free the packages
		// until late in the shutdown -- see CTheApp::ExitInstance().
		//
		// [31-Jul-93 mattg]
		//
		// FreeLibrary(m_hInstance);
	}
}

void CPackage::SetVisibleMenuItem(UINT nID, BOOL bVisible /*=TRUE*/)
{
	SetCommandUIFlags(nID, CT_QUERYMENU, !bVisible, CT_NOUI);
}

// NOTE: This function can be used to change UI availability of
// a command (in menus, toolbars, and customized dialogs).  It does not
// however, update existing toolbars.  Thus it should not be used to
// hide a command UI after startup when toolbars have been created.
// (Intended use: removing commands that do not apply without a
// certain package.)

void CPackage::ShowCommandUI(UINT nID, BOOL bShow /*=TRUE*/)
{
	SetCommandUIFlags(nID, CT_NOUI, !bShow, CT_QUERYMENU);
}

void CPackage::SetCommandUIFlags(UINT nID, UINT flags, BOOL bSet, UINT flagHidden)
{
	CTE *pCTE = theCmdCache.GetCommandEntry(nID);
	if (pCTE == NULL ||
		((pCTE->flags & flags) == 0 && !bSet) ||
		((pCTE->flags & flags) == flags && bSet))
	{
		return;
	}

	if (bSet)
		pCTE->flags |= flags;
	else
		pCTE->flags &= ~flags;

	if ((pCTE->flags & flagHidden) == 0)
		theApp.m_bMenuDirty = TRUE;
}

BOOL CPackage::IsVisibleProjectCommand(UINT nID, GUID *pGuids, UINT cGuidCount)
{
	// Everything is visible by default
	return TRUE;
}


LPCTSTR CPackage::GetCommandString(UINT nID, UINT iString)
{

	// Caller should not hold onto the string this returns, but should copy it immediately.
	// There are dynamic string that are stored in a single global CString, and the CString
	// may be overwritten

	LPCTSTR szCommand;
	if (theCmdCache.GetCommandString(nID, iString, &szCommand))
		return szCommand;

	return NULL;
}

BSTR CPackage::GetDynamicCmdString(UINT nID, UINT iString)
{
	// FAT NOTE for derivers: this is usually only called by theCmdCache.GetCommandString.
	// DO NOT have this call theCmdCache.GetCommandString or you 
	// will loop forever. Instead, if you do not have a string and want to
	// get it from the command table, just return NULL
	// and the cmdCache code will get the string it from itself.
	// We do this because there is some tooltip stuff that wants to allow a 
	// package to add the string itself.

	return NULL; // have the cmdCache get it from the command table
}

UINT CPackage::CmdIDForToolTipKey(UINT nIDCmdOriginal)
{
	// Limited use virtual (bobz)
	// This can be called for a command with the CT_DYNAMIC_CMD_STRING flag set.
	// It allows the caller to specify a different command id to use to get the
	// key assignment for a tool tip. It exists primarily for commands that are
	// toolbar HWND controls that have a different sidedocked command, like the
	// Find control or Wizard Bar combos that otherwise lose their tooltips

	// In the default case, just return the same value passed in
	return nIDCmdOriginal;
}

// unloads the bitmap, if loaded
void CPackage::UnloadBitmap(void)
{
	if ((m_hBitmap != NULL))
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

// This routine (re) loads the package bitmap and returns it, for example after a change of button size.
HBITMAP CPackage::GetBitmap(BOOL bLarge)
{
	if (m_lpBitmapID != NULL)
	{
		// if something's loaded, return or lose that
		if(m_hBitmap!=NULL)
		{
			if(m_bLoadedBitmapLarge==bLarge)
			{
				return m_hBitmap;
			}
			else
			{
				UnloadBitmap();
			}
		}

		// if this isn't true, we'd leak
		ASSERT(m_hBitmap==NULL);
		
		HINSTANCE hInstance = m_hInstance;
		if (hInstance == NULL)
			hInstance = GetResourceHandle();

			// this code spots packages not following correct conventions for their package bitmaps
#ifdef _DEBUG
		{
			// This assert will fail if a package has failed to provide a 
			// large bitmap resource identifier in its CPackage constructor
			// It should be the seventh parameter to the constructor. martynl 04Mar96
			ASSERT(m_lpLargeBitmapID);

			// get size of small button glyphs
			CSize smallSize=CCustomBar::GetDefaultBitmapSize(FALSE);
			CSize largeSize=CCustomBar::GetDefaultBitmapSize(TRUE);

			HBITMAP hbmSmall=NULL;
			HBITMAP hbmLarge=NULL;

			// These assertions would fail if the bitmap wasn't present in the resource file
			HRSRC hRsrc = ::FindResource(hInstance,	m_lpBitmapID, RT_BITMAP);
			hbmSmall=AfxLoadSysColorBitmap(hInstance, hRsrc);
			ASSERT(hbmSmall!=NULL);

			hRsrc = ::FindResource(hInstance,	m_lpLargeBitmapID, RT_BITMAP);
			hbmLarge=AfxLoadSysColorBitmap(hInstance, hRsrc);
			ASSERT(hbmLarge!=NULL);

			BITMAP bmpSmall;
			BITMAP bmpLarge;
			// These asserts should only fail if GDI is in confusion
			VERIFY(GetObject(hbmSmall, sizeof(BITMAP), &bmpSmall));
			VERIFY(GetObject(hbmLarge, sizeof(BITMAP), &bmpLarge));

			// Check height of bitmaps
			ASSERT(bmpSmall.bmHeight==smallSize.cy);
			ASSERT(bmpLarge.bmHeight==largeSize.cy);

			// Check no slack at end of bitmaps
			ASSERT((bmpSmall.bmWidth % smallSize.cx)==0);
			ASSERT((bmpLarge.bmWidth % largeSize.cx)==0);

			// Check same number of glyphs on both bitmaps
			int nSmallGlyphs=bmpSmall.bmWidth / smallSize.cx;
			int nLargeGlyphs=bmpLarge.bmWidth / largeSize.cx;
			ASSERT(nSmallGlyphs==nLargeGlyphs);
		}
#endif

		LPCTSTR lpCurrentBitmapID=NULL;

		// get the right bitmap id
		if(!bLarge)
		{
			lpCurrentBitmapID=m_lpBitmapID;
		}
		else
		{
			lpCurrentBitmapID=m_lpLargeBitmapID;
		}

		// this should have been caught above anyway, so this means some weird logic error
		ASSERT(lpCurrentBitmapID!=NULL);

		HRSRC hRsrc = ::FindResource(hInstance,	lpCurrentBitmapID, RT_BITMAP);
		if (hRsrc == NULL)
		{
			TRACE0("Warning: Unable to load toolbar bitmap.\n"
				"	Use 0 in command table if bitmap is unnecessary.\n");

			// bad package hasn't got the resources it claims
			ASSERT(FALSE);
			return NULL;
		}

		m_hBitmap = AfxLoadSysColorBitmap(hInstance, hRsrc);

		if(m_hBitmap==NULL)
		{
			// this should have been caught above
			ASSERT(FALSE);
		}

		m_bLoadedBitmapLarge=bLarge;

		return m_hBitmap;
	}
	else
	{
		return NULL;
	}
}

BOOL CPackage::OnInit()
{
	LoadGlobalAccelerators();
	return TRUE;
}

BOOL CPackage::CanExit()
{
	return TRUE;
}

void CPackage::OnExit()
{
}

BOOL CPackage::OnIdle(long lCount)
{
	return FALSE;
}

BOOL CPackage::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

void CPackage::SaveSettings()
{
	// Do nothing.
}

void CPackage::SerializeWorkspaceSettings(CStateSaver& stateSave, DWORD dwFlags)
{
	// Do nothing.
}

void CPackage::SerializeWkspcConvSettings(CStateSaver& stateSave, DWORD dwFlags)
{
	// Do nothing.
}

void CPackage::GetDefProvidedNodes(CPtrList &DefNodeList, BOOL bWorkspaceInit)
{
	// Do nothing.  Node provider packages should create an 
	// object of CDefProvidedNode class and AddTail() a pointer
	// to it to the CPtrList supplied.
}

LPCTSTR CPackage::GetProjectWorkspaceInfoText(LPCTSTR lpszProject)
{
	ASSERT(FALSE);	// derived package should not have PKS_PROJECTINFO flag
	return NULL;
}

void CPackage::SetProjectWorkspaceInfoText(LPCTSTR lpszProject, LPCTSTR lpszInfo)
{
	ASSERT(FALSE);	// derived package should not have PKS_PROJECTINFO flag
}

LPCTSTR CPackage::GetGlobalWorkspaceInfoText()
{
	ASSERT(FALSE);	// derived package should not have PKS_GLOBALINFO flag
	return NULL;
}

void CPackage::SetGlobalWorkspaceInfoText(LPCTSTR lpszInfo)
{
	ASSERT(FALSE);	// derived package should not have PKS_GLOBALINFO flag
}

LPCTSTR CPackage::GetHelpFileName(HELPFILE_TYPE helpfileType)
{
	return NULL; //no default
}

// Dockable window interface

HGLOBAL CPackage::GetDockInfo()
{
	return NULL;
}

HWND CPackage::GetDockableWindow(UINT nID, HGLOBAL hglob)
{
	ASSERT(FALSE);	// Override this, if you register dockable windows or toolbar controls.
	return NULL;
}

HGLOBAL CPackage::GetToolbarData(UINT nID)
{
	ASSERT(FALSE);	// Override this, if you register toolbars.
	return NULL;
}

HICON CPackage::GetDockableIcon(UINT nID)
{
	ASSERT(FALSE);	// Override this, if you register dtEdit dockable windows.
	return NULL;
}

BOOL CPackage::AskAvailable(UINT nIDWnd)
{
	ASSERT(FALSE);	// Override this, if you register context sensitive windows.
	return FALSE;
}

// Customization and Options interface

void CPackage::AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type)
{
	// Default is to do nothing
}

BOOL CPackage::OnNotify(UINT id, void *Hint /*= NULL*/, void *ReturnArea /*= NULL */)
{
	return TRUE;
}

int CPackage::IdsFromFerr(int ferr)
{
	return 0;	// No string ID.
}

void CPackage::LoadGlobalAccelerators()
{
	if (m_lpAccelID != NULL)
	{
		HINSTANCE hInst = AfxFindResourceHandle(m_lpAccelID, RT_ACCELERATOR);
		ASSERT(hInst != NULL);

		HACCEL hAccelTable = LoadAccelerators(hInst, m_lpAccelID);
		ASSERT(hAccelTable != NULL);

		AddGlobalAcceleratorTable(hAccelTable);
		DestroyAcceleratorTable(hAccelTable);
	}
}

BOOL CPackage::AppendExtraPropertyPages(REFGUID guidView, IUnknown* piUnk)
{
	return FALSE;
}

// This is our secret function to enable the Microsoft on the web menu. We don't want package partners to know about this,
// So we want to hide the capability in a seemingly undocumented generic function. Packages that represent products 
// (CPP, Java, ISWS) should use the canonical implementation shown below
UINT CPackage::GetPackageProperty(UINT nProperty)
{
	return 0;
}

// The canonical implementation is this:
// For information on why this is the way it is, see the comments in the shell
// UINT CLangCPPPackage::GetPackageProperty(UINT nProperty)
// {
//	if(nProperty=030370)
//	{
//		return 0x6d736c;
//	}
//	else
//	{
//		return 0;
//	}
// }

// CommDlg interface

// In derived packages, override these functions to add your file filters to
// the Open Files Of Type combo, and to add your editors to the Open As combo.
// pnRank is the relative rank of your package's stuff.
// The existing ranks are:
//		Common Files:		0 - 50		(first)
//		Source & Headers:	100 - 150
//		Projects:			200 - 250
//		Resources:			300 - 350
//		Don't care:			-1		(will be tacked on the end)
//
// nOpenDialog is which dialog this is for.  Packages may have different
// things they want to list for the File.Open dialog vs. the Add.Files.To.Project
// dialog.  If additional File.Open dialogs are created, give them an ID in the
// enum in package.h.

void CPackage::GetOpenDialogInfo(SRankedStringID** ppFilters, 
	SRankedStringID** ppEditors, int nOpenDialog)
{
	// derived classes should return an array of string IDs
	ASSERT( ppFilters != NULL );
	*ppFilters = NULL;
	ASSERT( ppEditors != NULL );
	*ppEditors = NULL;
}

CDocTemplate* CPackage::GetTemplateFromEditor(UINT idsEditor, 
	const char* szFilename)
{
	// must handle exactly those editors passed in to GetOpenAsEditors
	return NULL;	// return NULL if the editor isn't yours
}

/////////////////////////////////////////////////////////////////////////////
//	CPacket class

CPacket::CPacket(CPackage* pPackage, UINT id,
		LPCTSTR lpCmdTableID /*=NULL*/, LPCTSTR lpAccelID /*=NULL*/,
		CSushiBarInfo* pSushiBarInfo /*=NULL*/)
	: CPack(pPackage, id, lpAccelID)
{
	m_pSushiBarInfo = pSushiBarInfo;

	if (lpCmdTableID != NULL)
		theCmdCache.AddCommandResource(this, lpCmdTableID);
}

/****************************************************************************

	FUNCTION:	IsPackageLoaded()

	PURPOSE:	Determines whether the specified package is loaded
				given a path and a file name.

	RETURNS:	TRUE if the package is loaded, FALSE otherwise

****************************************************************************/

BOOL IsPackageLoaded(UINT uiPackageId)
{
	POSITION pos = theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage *)theApp.m_packages.GetNext(pos);

		if (pPackage->PackageID() == uiPackageId)
			return TRUE;
	}

	return FALSE;
}

