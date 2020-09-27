// FileListBox.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "FileListBox.h"
#include "FileItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileListBox

CFileListBox::CFileListBox()
{
}

CFileListBox::~CFileListBox()
{
}


BEGIN_MESSAGE_MAP(CFileListBox, CListBox)
	//{{AFX_MSG_MAP(CFileListBox)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileListBox message handlers

int CFileListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	// Get the File Items
	CFileItem *pFileItem1 = reinterpret_cast<CFileItem *>(lpCompareItemStruct->itemData1);
	CFileItem *pFileItem2 = reinterpret_cast<CFileItem *>(lpCompareItemStruct->itemData2);
	if( !pFileItem1 || !pFileItem2 )
	{
		return 0;
	}

    return pFileItem1->GetName().CompareNoCase( pFileItem2->GetName() );
}

int CFileListBox::IndexFromFile( void *pVoid )
{
	for( int nIndex = GetCount() - 1; nIndex >= 0; nIndex-- )
	{
		if( GetItemDataPtr( nIndex ) == pVoid )
		{
			return nIndex;
		}
	}

	return LB_ERR;
}

void CFileListBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	BOOL bOutside = TRUE;
	int nIndex = ItemFromPoint( point, bOutside );

	if( !bOutside )
	{
		SetCurSel( nIndex );
	}

	CListBox::OnRButtonDown(nFlags, point);
}
