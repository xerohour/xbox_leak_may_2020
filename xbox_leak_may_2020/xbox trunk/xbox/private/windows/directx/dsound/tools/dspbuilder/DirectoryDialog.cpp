/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	DirectoryDialog.cpp

Abstract:

	Dialog for directory selection

Author:

	Robert Heitkamp (robheit) 20-Jul-2001

Revision History:

	20-Jul-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "DirectoryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//	Image indexes
//------------------------------------------------------------------------------
#define ILI_HARD_DISK       0
#define ILI_FLOPPY          1
#define ILI_CD_ROM          2
#define ILI_NET_DRIVE       3
#define ILI_CLOSED_FOLDER   4
#define ILI_OPEN_FOLDER     5

//------------------------------------------------------------------------------
//	CDirectoryDialog::CDirectoryDialog
//------------------------------------------------------------------------------
CDirectoryDialog::CDirectoryDialog(
								   IN CWnd*	pParent
								   ) : CDialog(CDirectoryDialog::IDD, pParent)
/*++

Routine Description:

	Constructor

Arguments:

	IN pParent -	Parent window

Return Value:

	None

--*/
{
	//{{AFX_DATA_INIT(CDirectoryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pTree = NULL;
}


//------------------------------------------------------------------------------
//	CDirectoryDialog::DoDataExchange
//------------------------------------------------------------------------------
void 
CDirectoryDialog::DoDataExchange(
								 IN CDataExchange* pDX
								 )
/*++

Routine Description:

	Handles member variable access

Arguments:

	IN pDX -	Data exchange

Return Value:

	None

--*/
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirectoryDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirectoryDialog, CDialog)
	//{{AFX_MSG_MAP(CDirectoryDialog)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_DIRECTORY_TREE, OnItemExpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//	CDirectoryDialog::OnInitDialog
//------------------------------------------------------------------------------
BOOL 
CDirectoryDialog::OnInitDialog(void)
/*++

Routine Description:

	Sets up the directory tree

Arguments:

	None

Return Value:

	None

--*/
{
	// Get the tree control
	m_pTree = (CTreeCtrl*)GetDlgItem(IDC_DIRECTORY_TREE);

    // Initialize the image list.
    m_ilDrives.Create(IDB_DRIVEIMAGES, 16, 1, RGB (255, 0, 255));
    m_pTree->SetImageList(&m_ilDrives, TVSIL_NORMAL);

    // Populate the tree view with drive items.
    AddDrives();

    // Show the folders on the current drive.
    TCHAR szPath[MAX_PATH];
    ::GetCurrentDirectory(sizeof (szPath) / sizeof (TCHAR), szPath);
    CString strPath = szPath;
    strPath = strPath.Left(3);

    HTREEITEM hItem = m_pTree->GetNextItem(NULL, TVGN_ROOT);
    while(hItem != NULL) 
	{
        if(m_pTree->GetItemText(hItem) == strPath)
            break;
        hItem = m_pTree->GetNextSiblingItem(hItem);
    }

    if(hItem != NULL) 
	{
		m_pTree->Expand(hItem, TVE_EXPAND);
		m_pTree->Select(hItem, TVGN_CARET);
	}

	return TRUE;
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::AddDrives
//------------------------------------------------------------------------------
int 
CDirectoryDialog::AddDrives(void)
/*++

Routine Description:

	Adds the drives to the tree

Arguments:

	None

Return Value:

	The number of drives added

--*/
{
    int		nPos			= 0;
    int		nDrivesAdded	= 0;
    CString string			= _T ("?:\\");
    DWORD	dwDriveList		= ::GetLogicalDrives();

    while(dwDriveList) 
	{
        if(dwDriveList & 1) 
		{
            string.SetAt(0, (TCHAR)(_T ('A') + nPos));
            if(AddDriveItem(string))
                nDrivesAdded++;
        }
        dwDriveList >>= 1;
        nPos++;
    }
    return nDrivesAdded;
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::AddDriveItem
//------------------------------------------------------------------------------
BOOL 
CDirectoryDialog::AddDriveItem(
							   IN LPCTSTR pszDrive
							   )
/*++

Routine Description:

	Addes an individual drive to the tree

Arguments:

	None

Return Value:

	TRUE if a valid item is added

--*/
{
    CString		string;
    HTREEITEM	hItem;

    UINT nType = ::GetDriveType(pszDrive);

	if(!m_pTree)
		return FALSE;

    switch(nType) 
	{
    case DRIVE_REMOVABLE:
        hItem = m_pTree->InsertItem(pszDrive, ILI_FLOPPY, ILI_FLOPPY);
        m_pTree->InsertItem(_T (""), ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER, hItem);
        break;

    case DRIVE_FIXED:
    case DRIVE_RAMDISK:
        hItem = m_pTree->InsertItem(pszDrive, ILI_HARD_DISK, ILI_HARD_DISK);
        SetButtonState(hItem, pszDrive);
        break;

    case DRIVE_REMOTE:
        hItem = m_pTree->InsertItem(pszDrive, ILI_NET_DRIVE, ILI_NET_DRIVE);
        SetButtonState(hItem, pszDrive);
        break;

    case DRIVE_CDROM:
        hItem = m_pTree->InsertItem(pszDrive, ILI_CD_ROM, ILI_CD_ROM);
        m_pTree->InsertItem(_T (""), ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER, hItem);
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::SetButtonState
//------------------------------------------------------------------------------
BOOL 
CDirectoryDialog::SetButtonState(
								 IN HTREEITEM	hItem, 
								 IN LPCTSTR		pszPath
								 )
/*++

Routine Description:

	Sets the state of the tree buttons

Arguments:

	IN hItem -		Item
	IN pszPath -	Path

Return Value:

	TRUE on success

--*/
{
    HANDLE			hFind;
    WIN32_FIND_DATA	fd;
    BOOL			bResult = FALSE;

	if(!m_pTree)
		return FALSE;

    CString strPath = pszPath;
    if (strPath.Right (1) != _T ("\\"))
        strPath += _T ("\\");
    strPath += _T ("*.*");

    if((hFind = ::FindFirstFile(strPath, &fd)) == INVALID_HANDLE_VALUE)
        return bResult;

	do 
	{
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
            CString strComp = fd.cFileName;
            if((strComp != _T (".")) && (strComp != _T (".."))) 
			{
                m_pTree->InsertItem(_T (""), ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER, hItem);
                bResult = TRUE;
                break;
            }
        }
    } 
	while(::FindNextFile (hFind, &fd));

    ::FindClose (hFind);
    return bResult;
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::OnItemExpanding
//------------------------------------------------------------------------------
void 
CDirectoryDialog::OnItemExpanding(
								  IN NMHDR*		pNMHDR, 
								  IN LRESULT*	pResult
								  ) 
/*++

Routine Description:

	Handles the expansion of the nodes

Arguments:

	IN pNMHDR -		?
	IN pResult -	Result of operation, TRUE on success, FALSE on failure

Return Value:

	None

--*/
{
    NM_TREEVIEW*	pNMTreeView	= (NM_TREEVIEW*)(void*)pNMHDR;
    HTREEITEM		hItem		= pNMTreeView->itemNew.hItem;
    CString			string		= GetPathFromItem(hItem);

    *pResult = FALSE;

    if(pNMTreeView->action == TVE_EXPAND) 
	{
        DeleteFirstChild(hItem);
        if(AddDirectories(hItem, string) == 0)
            *pResult = TRUE;
    }

    else if(m_pTree)
	{ 
        DeleteAllChildren(hItem);
        if (m_pTree->GetParentItem(hItem) == NULL)
            m_pTree->InsertItem(_T (""), ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER, hItem);
        else
            SetButtonState(hItem, string);
    }
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::GetPathFromItem
//------------------------------------------------------------------------------
CString 
CDirectoryDialog::GetPathFromItem(
								  IN HTREEITEM hItem
								  )
/*++

Routine Description:

	Gets a pathname from a tree item

Arguments:

	IN hItem -	Item

Return Value:

	Pathname

--*/
{
    CString		strResult;
    HTREEITEM	hParent;

	if(m_pTree)
	{
		strResult = m_pTree->GetItemText (hItem);
		while ((hParent = m_pTree->GetParentItem (hItem)) != NULL) 
		{
			CString string = m_pTree->GetItemText (hParent);
			if (string.Right (1) != _T ("\\"))
				string += _T ("\\");
			strResult = string + strResult;
			hItem = hParent;
		}
	}
    return strResult;
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::DeleteFirstChild
//------------------------------------------------------------------------------
void 
CDirectoryDialog::DeleteFirstChild(
								   IN HTREEITEM hItem
								   )
/*++

Routine Description:

	Deletes a child node

Arguments:

	IN hItem -	Item to delete

Return Value:

	None

--*/
{
    HTREEITEM hChildItem;
	if(m_pTree && (hChildItem = m_pTree->GetChildItem(hItem)) != NULL)
		m_pTree->DeleteItem(hChildItem);
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::DeleteAllChildren
//------------------------------------------------------------------------------
void 
CDirectoryDialog::DeleteAllChildren(
									IN HTREEITEM hItem
									)
/*++

Routine Description:

	Deletes all childred

Arguments:

	IN hItem -	Item to delete

Return Value:

	None

--*/
{
    HTREEITEM hChildItem;

    if(!m_pTree || (hChildItem = m_pTree->GetChildItem (hItem)) == NULL)
        return;

    do 
	{
        HTREEITEM hNextItem = m_pTree->GetNextSiblingItem(hChildItem);
        m_pTree->DeleteItem (hChildItem);
        hChildItem = hNextItem;
    }
	while(hChildItem != NULL);
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::AddDirectories
//------------------------------------------------------------------------------
int 
CDirectoryDialog::AddDirectories(
								 IN HTREEITEM	hItem, 
								 IN LPCTSTR		pszPath
								 )
/*++

Routine Description:

	Adds directories

Arguments:

	IN hItem -		Item number to add directories to
	IN pszPath -	Pathname to add

Return Value:

	Items added

--*/
{
    HANDLE			hFind;
    WIN32_FIND_DATA	fd;
    HTREEITEM		hNewItem;
    int				nCount	= 0;
    CString			strPath	= pszPath;

	if(!m_pTree)
		return 0;

    if (strPath.Right (1) != _T ("\\"))
        strPath += _T ("\\");
    strPath += _T ("*.*");

    if((hFind = ::FindFirstFile(strPath, &fd)) == INVALID_HANDLE_VALUE) 
	{
        if(m_pTree->GetParentItem(hItem) == NULL)
            m_pTree->InsertItem(_T (""), ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER, hItem);

		strPath.Format(_T("Please insert a disk into drive %s"), pszPath);
		strPath.Replace(_T(":\\"), _T(":"));
		AfxMessageBox((LPCTSTR)strPath, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
        return 0;
    }

	do 
	{
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
            CString strComp = fd.cFileName;
            if((strComp != _T (".")) && (strComp != _T (".."))) 
			{
                hNewItem = m_pTree->InsertItem(fd.cFileName, ILI_CLOSED_FOLDER, ILI_OPEN_FOLDER, hItem);

                CString strNewPath = pszPath;
                if(strNewPath.Right (1) != _T ("\\"))
                    strNewPath += _T ("\\");

                strNewPath += fd.cFileName;
                SetButtonState(hNewItem, strNewPath);
                nCount++;
            }
        }
    } 
	while (::FindNextFile (hFind, &fd));

    ::FindClose (hFind);
    return nCount;
}

//------------------------------------------------------------------------------
//	CDirectoryDialog::OnOK
//------------------------------------------------------------------------------
void 
CDirectoryDialog::OnOK(void) 
{
	if(m_pTree)
		m_directory = GetPathFromItem(m_pTree->GetSelectedItem());
	CDialog::OnOK();
}
