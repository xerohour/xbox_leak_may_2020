// command.cpp

#include "stdafx.h"

static LPCTSTR CommandNames[6] = 
{	
	_T("invalid"),
	_T("start"),
	_T("run"),
	_T("end"),
	_T("exit"),
	NULL
};

/*
static CArray <LPCTSTR, 12> ControlNames (7, 
	_T("maxthreadcount"),
	_T("maxtimeout"),
	_T("pollingperiod"),
	_T("looplimit"),
	_T("timelimit"),
	_T("haltonerror"),
	_T("debugmask"));
*/

XCMD_TYPE CCommandItem::LookupCommand (LPCTSTR typeName)
{
	for (int ii = 0; CommandNames [ii] != NULL; ii++)
	{
		if (_tcsicmp (CommandNames [ii], typeName) == 0)
			return (XCMD_TYPE) ii;
	}
	return XCMD_INVALID;
}

CCommandItem::CCommandItem (LPCTSTR name, XCMD_TYPE type):
	CTreeItem(name), m_type(type)
{
	CParameterTable *table = new CParameterTable;
	m_parameters.set (table);
}
 
CCommandItem::~CCommandItem () 
{
}

CCommandItem *CCommandItem::AddCommand 
	(LPCTSTR module, LPCTSTR test, XCMD_TYPE type)
{
	_TCHAR name [128] = {0};
	if (module != NULL)
		_tcscpy (name, module);
	if (test != NULL)
	{
		_tcscat (name, _T("."));
		_tcscat (name, test);
	}

	CCommandItem *item = new CCommandItem (name, type);
	AddChild (item);

	return item;
}

bool CCommandList::ParseLine (LPTSTR line)
{
//	_tprintf (L"line: %s\n", line);
	_TCHAR terminal; // stores the terminal returned by GetToken
	CParseSymbol comma (_T(','));
	CParseSymbol equals (_T('='));

	// Parse the command name from the line
	//
	CParseWord command (_T(", "));
	terminal = command.GetToken (&line);
	XCMD_TYPE type = m_list.LookupCommand (command);
	if (type == XCMD_EXIT)
	{
		m_parsingList->AddCommand (NULL, NULL, type);
	}
	else
	if (type == XCMD_ENDLOOP)
	{
		CParseWord loop (_T(""));
		terminal = loop.GetToken (&line);
		if (_tcslen (loop) == 0)
			g_error.ThrowExpectingName ();

		// This statement pops the context up to the parent loop.
		//
		m_parsingList = (CCommandItem*) m_parsingList->GetParent ();
		if (m_parsingList == NULL)
			g_error.ThrowUnexpectedEndStatement ();
		
		if (_tcslen (line) > 0)
			g_error.ThrowUnknownTextFollowingCommand ();
	}
	else
	if (type != XCMD_INVALID)
	{
		CCommandItem *cmd = NULL;

		CParseWord module (_T("., "));
		terminal = module.GetToken (&line);
		if (_tcslen (module) == 0)
			g_error.ThrowExpectingName ();

		// Now we know we are parsing a START or RUN command
		// If the command takes no arguement, then it is a loop.
		//
		if ((terminal != _T('.')) && (_tcsicmp (module, _T("LOOP")) == 0))
		{
			type = (type == XCMD_RUNTEST)? XCMD_RUNLOOP:XCMD_STARTLOOP;
						
			cmd = m_parsingList->AddCommand (module, NULL, type);
			m_parsingList = (CCommandItem*) m_parsingList->GetLastChild ();
		}
		else // This must be a start or run TEST command
		{
			// In the case of a test we need to find module.name syntax
			// 
			if (terminal != _T('.'))
				g_error.ThrowExpectingDot ();

			CParseWord test (_T(", "));
			terminal = test.GetToken (&line);

			// This is a good time to verify that the name 
			// corresponds to a registered test.
			//
			if (!CTestObj::LookupTest (module, test))
				g_error.ThrowTestNotFound (test);
			
			cmd = m_parsingList->AddCommand (module, test, type);
		}

		// Now parse optional comma separated local parameters
		//
		ASSERT (cmd);
		CParameterTable *table = cmd->GetParameters ();
		while ((terminal == _T(',')) || comma.GetToken(&line))
			terminal = table->ParseParameter (&line, module);
	}
	else // flow control parameter
	if ((terminal == _T('=')) || equals.GetToken(&line))
	{
		CParameterTable *table = m_parsingList->GetParameters ();
		CParseWord value (_T("\n"));
		value.GetToken (&line);
		table->AddParameter (_T("testflow"), command, value);
	}
	else
		g_error.ThrowInvalidCommand (command);

	return true;
}

