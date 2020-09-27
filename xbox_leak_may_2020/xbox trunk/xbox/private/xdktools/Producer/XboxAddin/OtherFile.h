#if !defined(AFX_OTHER_FILES_H__22EF9571_12A3_5073_9CE0_6F8456E61502__INCLUDED_)
#define AFX_OTHER_FILES_H__22EF9571_12A3_5073_9CE0_6F8456E61502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherFile.h : header file
//

#include "FileItem.h"

/////////////////////////////////////////////////////////////////////////////
// COtherFile class

class COtherFile : public CFileItem
{
public:
	COtherFile( IDMUSProdNode *pDMUSProdNode );
	HICON GetIcon( void )
	{
		return m_hIcon;
	}

protected:
    HICON	m_hIcon;
};


#endif // !defined(AFX_OTHER_FILES_H__22EF9571_12A3_5073_9CE0_6F8456E61502__INCLUDED_)
