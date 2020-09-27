/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Unique.cpp

Abstract:

	Base class for all unique objects

Author:

	Robert Heitkamp (robheit) 27-Nov-2001

Revision History:

	27-Nov-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Unique.h"

//------------------------------------------------------------------------------
//	Static Member Variables
//------------------------------------------------------------------------------
DWORD CUnique::m_globalId = 0;

//------------------------------------------------------------------------------
//	CUnique::CUnique
//------------------------------------------------------------------------------
CUnique::CUnique(void)
/*++

Routine Description:

	Constructor. Generates a unique id.

Arguments:

	None

Return Value:

	None

--*/
{
	m_id = ++m_globalId;
}

//------------------------------------------------------------------------------
//	CUnique::~CUnique
//------------------------------------------------------------------------------
CUnique::~CUnique(void)
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
//	CUnique::Write
//------------------------------------------------------------------------------
void 
CUnique::Write(
			   IN CFile& file
			   )
/*++

Routine Description:

	Writes the ID to a file

Arguments:

	IN file -	File to write to

Return Value:

	None

--*/
{
	try
	{
		file.Write(&m_id, sizeof(m_id));
	}
	catch(...)
	{
		throw;
	}
}

//------------------------------------------------------------------------------
//	CUnique::Read
//------------------------------------------------------------------------------
BOOL
CUnique::Read(
			  IN CFile& file,
			  IN BYTE//	version
			  )
/*++

Routine Description:

	Reads the ID from a file

Arguments:

	IN file -		File to read from
	IN version -	File version

Return Value:

	TRUE on success, FALSE on failure

--*/
{	
	BOOL	ret = (file.Read(&m_id, sizeof(m_id)) == sizeof(m_id));

	if(ret)
	{
		if(m_id > m_globalId)
			m_globalId = m_id;
		return TRUE;
	}
	return FALSE;
}
