/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Unique.h

Abstract:

	Base class for all unique objects

Author:

	Robert Heitkamp (robheit) 27-Nov-2001

Revision History:

	27-Nov-2001 robheit
		Initial Version

--*/
#if !defined(AFX_UNIQUE_H__0287EF65_4786_4D05_898F_04C36A2C4926__INCLUDED_)
#define AFX_UNIQUE_H__0287EF65_4786_4D05_898F_04C36A2C4926__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <afxwin.h>

//------------------------------------------------------------------------------
//	CUnique
//------------------------------------------------------------------------------
class CUnique  
{
public:

	CUnique(void);
	virtual ~CUnique(void);

	virtual void Write(CFile& file); // throw(CFileException);
	virtual BOOL Read(CFile& file, BYTE version);

	DWORD GetId(void) const { return m_id; };
//	void SetId(DWORD id) { m_id = id; };

private:

	static DWORD	m_globalId;
	DWORD			m_id;
};

#endif // !defined(AFX_UNIQUE_H__0287EF65_4786_4D05_898F_04C36A2C4926__INCLUDED_)
