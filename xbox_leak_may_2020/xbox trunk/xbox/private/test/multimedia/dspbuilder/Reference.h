/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Reference.h

Abstract:

	Reference counting class

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_REFERENCE_H__199F50B4_36DC_4E56_877F_E61C36770F10__INCLUDED_)
#define AFX_REFERENCE_H__199F50B4_36DC_4E56_877F_E61C36770F10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	CReference
//------------------------------------------------------------------------------
class CReference  
{
public:
	CReference(void);
	virtual ~CReference(void);

	void Reference(void);
	BOOL UnReference(void);

private:

	UINT m_refCount;
};

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define REF(r) (r)->Reference()
#define UNREF(r) if((r)->UnReference()) { delete (r); (r) = NULL; }

#endif // !defined(AFX_REFERENCE_H__199F50B4_36DC_4E56_877F_E61C36770F10__INCLUDED_)
