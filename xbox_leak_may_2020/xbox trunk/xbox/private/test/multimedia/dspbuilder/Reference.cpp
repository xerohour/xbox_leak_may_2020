/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Reference.cpp

Abstract:

	Reference counting class

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "dspbuilder.h"
#include "Reference.h"

//------------------------------------------------------------------------------
//	Stuff
//------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------
//	CReference::CReference
//------------------------------------------------------------------------------
CReference::CReference(void)
/*++

Routine Description:

	Constructor. Initializes the reference count to 0

Arguments:

	None

Return Value:

	None

--*/
{
	m_refCount = 0;
}

//------------------------------------------------------------------------------
//	CReference::~CReference
//------------------------------------------------------------------------------
CReference::~CReference(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CReference::Reference
//------------------------------------------------------------------------------
void
CReference::Reference(void)
/*++

Routine Description:

	Increments the reference count

Arguments:

	None

Return Value:

	None

--*/
{
	++m_refCount;
}

//------------------------------------------------------------------------------
//	CReference::UnReference
//------------------------------------------------------------------------------
BOOL
CReference::UnReference(void)
/*++

Routine Description:

	Decrements the reference count.

Arguments:

	None

Return Value:

	TRUE if the reference count drops to 0.

--*/
{
	if(m_refCount > 0)
		--m_refCount;
	return (m_refCount == 0);
}
