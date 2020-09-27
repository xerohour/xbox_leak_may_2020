/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Jack.h

Abstract:

	A jack to connect a patch cord to a module

Author:

	Robert Heitkamp (robheit) 08-Nov-2001

Revision History:

	22-Nov-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Jack.h"
#include "Module.h"
#include "Graph.h"

//------------------------------------------------------------------------------
//	CJack::CJack
//------------------------------------------------------------------------------
CJack::CJack(
			 IN CModule*	pModule
			 ) : CUnique()
/*++

Routine Description:

	Constructor

Arguments:

	IN pModule -	Module

Return Value:

	None

--*/

{
	m_pPatchCord	= new CPatchCord(this);
	m_mixbin		= 0;
	m_bInput		= FALSE;
	m_line[0].x		= 0;
	m_line[0].y		= 0;
	m_textFlag		= 0;
	m_bBadData		= FALSE;
	m_pModule		= pModule;
	m_bOverwrite	= FALSE;
	m_rect.SetRect(0,0,0,0);
}

#if 0
//------------------------------------------------------------------------------
//	CJack::CJack
//------------------------------------------------------------------------------
CJack::CJack(
			 IN const CJack&	jack
			 ) : CUnique()
/*++

Routine Description:

	Constructor

Arguments:

	IN jack -	Jack to copy

Return Value:

	None

--*/

{
	m_bBadData		= FALSE;
	m_pPatchCord	= new CPatchCord(this);
	*this			= jack;
}
#endif

//------------------------------------------------------------------------------
//	CJack::~CJack
//------------------------------------------------------------------------------
CJack::~CJack(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	delete m_pPatchCord;
}

#pragma warning(push)
#pragma warning(disable:4701)
//------------------------------------------------------------------------------
//	CJack::Draw
//------------------------------------------------------------------------------
void
CJack::Draw(
			IN CDC*	pDC
			)
/*++

Routine Description:

	Draws the jack (a simple line).

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	COLORREF	oldTextColor;
	CPen*		oldPen;
	CPen		pen;

	if(m_bOverwrite)
	{
		oldTextColor = pDC->SetTextColor(RGB(0,0,255));
		pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
		oldPen = (CPen*)pDC->SelectObject(&pen);
	}

	pDC->Polyline((LPPOINT)m_line, 2);
	pDC->DrawText(m_name, m_rect, m_textFlag);

	if(m_bOverwrite)
	{
		pDC->SetTextColor(oldTextColor);
		pDC->SelectObject(oldPen );
	}
}
#pragma warning(pop)

//------------------------------------------------------------------------------
//	CJack::GetPatchCord
//------------------------------------------------------------------------------
CPatchCord*
CJack::GetPatchCord(void) const
/*++

Routine Description:

	Returns the patch cord connected to the jack or NULL of one is not
	connected. A Jack can only ever have one patch cord connected.

Arguments:

	None

Return Value:

	The patch cord or NULL

--*/
{
	if(m_pPatchCord->GetPatchNode())
		return m_pPatchCord->GetPatchNode()->pPatchCord;
	return NULL;
}

//------------------------------------------------------------------------------
//	CJack::operator =
//------------------------------------------------------------------------------
CJack&
CJack::operator = (
				   IN const CJack&	jack
				   )
/*++

Routine Description:

	Assignment operator. This does not duplicated any patch cords connected
	to this or the module. The module is left alone, and the patch cords are
	cleared. The id is left alone as well.

Arguments:

	IN jack -	Jack

Return Value:

	A reference to this

--*/
{
	m_name		= jack.m_name;
	m_rect		= jack.m_rect;
	m_mixbin	= jack.m_mixbin;
	m_bInput	= jack.m_bInput;
	m_line[0]	= jack.m_line[0];
	m_line[1]	= jack.m_line[1];
	m_textFlag	= jack.m_textFlag;
	m_bBadData	= jack.m_bBadData;
	m_pPatchCord->ClearPatch();
	return *this;
}

//------------------------------------------------------------------------------
//	CJack::SetRect
//------------------------------------------------------------------------------
void
CJack::SetRect(
			   IN const CRect&	rect
			   )
/*++

Routine Description:

	Sets the rect for this and generates a horizontal line through the center

Arguments:

	IN rect -	Rect

Return Value:

	None

--*/
{
	m_rect = rect; 
	SetLine();
}

//------------------------------------------------------------------------------
//	CJack::SetInput
//------------------------------------------------------------------------------
void
CJack::SetInput(
			    IN BOOL	input
			    )
/*++

Routine Description:

	Sets the input status of the jack

Arguments:

	IN input -	TRUE for an input jack, FALSE for an output jack

Return Value:

	None

--*/
{
	m_bInput = input;
	if(m_bInput)
		m_textFlag = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	else
		m_textFlag = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
	SetLine();
}

//------------------------------------------------------------------------------
//	CJack::SetLine
//------------------------------------------------------------------------------
void
CJack::SetLine(void)
/*++

Routine Description:

	Sets the line location

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_bInput)
	{
		m_line[0].x	= m_rect.left - (SCALE / 2) - 2;
		m_line[1].x	= m_line[0].x + (SCALE / 2);
	}
	else
	{
		m_line[0].x	= m_rect.right + 2;
		m_line[1].x	= m_rect.right + (SCALE / 2) + 3;
	}
	m_line[0].y	= (m_rect.bottom - m_rect.top) / 2 + m_rect.top;
	m_line[1].y	= m_line[0].y;
}

//------------------------------------------------------------------------------
//	CJack::Connect
//------------------------------------------------------------------------------
void 
CJack::Connect(
			   IN CPatchCord*	pPatchCord, 
			   IN BOOL			bEnd
			   )
/*++

Routine Description:

	Connects a patch cord to this. This method also connects this to the 
	patch cord. 

	DO NOT CALL THIS METHOD IF THIS IS ALREADY CONNECTED.

Arguments:

	IN pPatchCord -	Patch cord to connect
	IN bEnd -		TRUE to connect this to the end of pPatchCord,
					FALSE to connect this to the beggining of pPatchCord

Return Value:

	None

--*/
{
//	ASSERT(!m_pPatchCord->IsPatched());
	m_pPatchCord->AddPatchCord(pPatchCord, TRUE); 
	pPatchCord->AddPatchCord(m_pPatchCord, bEnd); 
}

//------------------------------------------------------------------------------
//	CJack::Write
//------------------------------------------------------------------------------
void 
CJack::Write(
		     IN CFile& file
		     )
/*++

Routine Description:

	Writes the object to a file

Arguments:

	IN file -	File to write to

Return Value:

	None

--*/
{
	DWORD	id;

	try
	{
		// Write the id
		CUnique::Write(file);

		// Write the module
		id = m_pModule ? m_pModule->GetId() : 0;
		file.Write(&id, sizeof(id));

		// Write the name
		file.Write((LPCTSTR)m_name, ((unsigned)m_name.GetLength() + 1) * sizeof(TCHAR));

		// Write the patch
		m_pPatchCord->Write(file);

		// Write the rect
		file.Write((LPRECT)m_rect, sizeof(RECT));

		// mixbin
		file.Write(&m_mixbin, sizeof(m_mixbin));

		// bInput
		file.Write(&m_bInput, sizeof(m_bInput));

		// m_bOverwrite
		file.Write(&m_bOverwrite, sizeof(m_bOverwrite));

		// line
		file.Write(&m_line[0].x, sizeof(m_line[0].x));
		file.Write(&m_line[0].y, sizeof(m_line[0].y));
		file.Write(&m_line[1].x, sizeof(m_line[1].x));
		file.Write(&m_line[1].y, sizeof(m_line[1].y));

		// text flag
		file.Write(&m_textFlag, sizeof(m_textFlag));
	}
	catch(...)
	{
		throw;
	}
}

//------------------------------------------------------------------------------
//	CJack::FixPointers
//------------------------------------------------------------------------------
void 
CJack::FixPointers(
				   IN const CGraph* pGraph
				   )
/*++

Routine Description:

	Fixes pointer from a read

Arguments:

	IN pGraph -		Graph to use to correct pointers

Return Value:

	None

--*/
{
	m_pModule = pGraph->GetModuleFromId((DWORD)m_pModule);
	m_pPatchCord->FixPointers(pGraph);

	m_bBadData = FALSE;
}

//------------------------------------------------------------------------------
//	CJack::Read
//------------------------------------------------------------------------------
BOOL
CJack::Read(
			IN CFile&	file,
			IN BYTE		version
			)
/*++

Routine Description:

	Constructor. Reads from a file

Arguments:

	IN file -		File to read from
	IN version -	File version

Return Value:

	None

--*/
{
	m_bBadData = TRUE;

	// Read the id
	if(!CUnique::Read(file, version))
		return FALSE;

	// Read the module
	if(file.Read(&m_pModule, sizeof(DWORD)) != sizeof(DWORD))
		return FALSE;

	// Read the name
	if(!CGraph::ReadString(file, m_name))
		return FALSE;

	// Read the patch
	if(!m_pPatchCord->Read(file, version))
	{
		delete m_pPatchCord;
		m_pPatchCord = NULL;
		return FALSE;
	}

	// Read the rect
	if(file.Read((LPRECT)m_rect, sizeof(RECT)) != sizeof(RECT))
		return FALSE;

	// mixbin
	if(file.Read(&m_mixbin, sizeof(m_mixbin)) != sizeof(m_mixbin))
		return FALSE;

	// bInput
	if(file.Read(&m_bInput, sizeof(m_bInput)) != sizeof(m_bInput))
		return FALSE;

	if(version >= 4)
	{
		if(file.Read(&m_bOverwrite, sizeof(m_bOverwrite)) != sizeof(m_bOverwrite))
			return FALSE;
	}

	// line
	if(file.Read(&m_line[0].x, sizeof(m_line[0].x)) != sizeof(m_line[0].x))
		return FALSE;
	if(file.Read(&m_line[0].y, sizeof(m_line[0].y)) != sizeof(m_line[0].y))
		return FALSE;
	if(file.Read(&m_line[1].x, sizeof(m_line[1].x)) != sizeof(m_line[1].x))
		return FALSE;
	if(file.Read(&m_line[1].y, sizeof(m_line[1].y)) != sizeof(m_line[1].y))
		return FALSE;

	// text flag
	if(file.Read(&m_textFlag, sizeof(m_textFlag)) != sizeof(m_textFlag))
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------
//	CJack::GetConnections
//------------------------------------------------------------------------------
void 
CJack::GetConnections(
					  IN OUT CArray<CJack*, CJack*>&	list,
					  IN BOOL							bClearList,		 // = TRUE
					  IN BOOL							bClearPatchCords // = TRUE
					  )
/*++

Routine Description:

	Builds a list of all jacks connected to this.

Arguments:

	IN OUT list -	List of jacks connected to this

Return Value:

	None

--*/
{
	// Make sure the list is clear
	if(bClearList)
		list.RemoveAll();

	if(!m_pModule)
		return;

	// Clear all patch cords
	if(bClearPatchCords)
		m_pModule->GetGraph()->MarkPatchCords(FALSE);
	m_pPatchCord->Mark(TRUE);

	// Build list
	if(m_pPatchCord->GetPatchNode())
		m_pPatchCord->GetPatchNode()->pPatchCord->GetConnections(list);

	// Clear all patch cords
	if(bClearPatchCords)
		m_pModule->GetGraph()->MarkPatchCords(FALSE);
	m_pPatchCord->Mark(FALSE);
}	

//------------------------------------------------------------------------------
//	CJack::GetAllConnections
//------------------------------------------------------------------------------
void 
CJack::GetAllConnections(
						 IN OUT CArray<CJack*, CJack*>&	list
						 )
/*++

Routine Description:

	Builds a list of all jacks connected to this, passing through effects if
	needed

Arguments:

	IN OUT list -	List of jacks connected to this

Return Value:

	None

--*/
{
	int			newSize;
	int			l;
	int			io;
	CModule*	pModule;
	int			lastSize = 0;

	// Make sure the list is clear
	list.RemoveAll();

	if(!m_pModule)
		return;

	// Clear all patch cords
	m_pModule->GetGraph()->MarkPatchCords(FALSE);
	m_pPatchCord->Mark(TRUE);

	// Build list
	if(m_pPatchCord->GetPatchNode())
		m_pPatchCord->GetPatchNode()->pPatchCord->GetConnections(list);
	while(lastSize != list.GetSize())
	{
		newSize = list.GetSize();

		// Step through all new additions to the list
		for(l=lastSize; l<newSize; ++l)
		{
			pModule = list[l]->GetModule();

			// If the connection is part of a module, parse the other connections
			if(pModule)
			{
				// Parse the outputs?
				if(list[l]->GetInput())
				{
					for(io=0; io<pModule->GetOutputs().GetSize(); ++io)
						pModule->GetOutputs()[io]->GetConnections(list, FALSE, FALSE);
				}

				// Or the inputs?
				else
				{
					for(io=0; io<pModule->GetInputs().GetSize(); ++io)
						pModule->GetInputs()[io]->GetConnections(list, FALSE, FALSE);
				}
			}
		}

		lastSize = newSize;
	}

	// Clear all patch cords
	m_pModule->GetGraph()->MarkPatchCords(FALSE);
	m_pPatchCord->Mark(FALSE);
}	

//------------------------------------------------------------------------------
//	CJack::Invalidate
//------------------------------------------------------------------------------
void 
CJack::Invalidate(void)
/*++

Routine Description:

	Invalidates all patch cords connected to this

Arguments:

	None

Return Value:

	None

--*/
{
	if(!m_pModule)
		return;

	// Clear all patch cords
	m_pModule->GetGraph()->MarkPatchCords(FALSE);
	m_pPatchCord->Mark(TRUE);

	// Build list
	if(m_pPatchCord->GetPatchNode())
		m_pPatchCord->GetPatchNode()->pPatchCord->Invalidate();

	// Clear all patch cords
	m_pModule->GetGraph()->MarkPatchCords(FALSE);
	m_pPatchCord->Mark(FALSE);
}
