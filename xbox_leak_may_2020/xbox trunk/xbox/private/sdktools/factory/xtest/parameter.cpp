// parameter.cpp

#include "stdafx.h"

CParameterItem::CParameterItem (LPCTSTR name, LPCTSTR value):CTreeItem (name)  
{
	m_value.set (value? NewTSTR(value):NULL);
}

bool CParameterItem::GetBoolean ()
{
	if (!_tcsicmp (GetValue(), _T("true")))
		return true;
	else
	if (_tcsicmp (GetValue(), _T("false")))
		g_error.ThrowConversionFailed (GetName(), _T("true/false") );

	return false;
}

int CParameterItem::GetInt ()
{
	errno = 0;
	_TCHAR *end;
	int x = (int) _tcstol (GetValue(), &end, 0);

	if ((*end != 0) || (errno == ERANGE))
		g_error.ThrowConversionFailed (GetName(), _T("int"));
		
	return x;
}

UINT CParameterItem::GetUint ()
{
	errno = 0;
	LPTSTR end;
	LPCTSTR value = GetValue();

	UINT x = (UINT) _tcstoul (value, &end, 0);

	if ((*end != 0)  || (errno == ERANGE))
		g_error.ThrowConversionFailed (GetName (), _T("UINT"));

	return x;
}

int CParameterItem::GetChoice (va_list argptr)
{
	LPCTSTR option, last = NULL; 
	int choice = -1;
	do {
		choice++;
		option = va_arg (argptr, LPCTSTR);
		if (option == NULL) // end of the list, no matches
		{
			g_error.ThrowParameterValueOutOfRange (GetName ());
		}

		// This is an attempt at early detection of an
		// ommitted NULL terminator
		if ((choice > 0) && ((((const int)last)^((const int)option)) > 0xffff))
			g_error.ThrowNullTerminatorMissing (GetName ());
		last = option;

	} while (_tcsicmp (GetValue(), option) != 0);


	return choice;
}

CParameterItem *CParameterTable::FindParameter (LPCTSTR module, LPCTSTR name)
{
	CParameterItem *moduleItem = m_list.FindParameter (module);
	return (moduleItem == NULL)? NULL:moduleItem->FindParameter (name);
}

void CParameterTable::AddParameter (LPCTSTR module, LPCTSTR name, LPCTSTR value)
{
	// If this is the first parameter from a module,
	// we will need to create the module node first
	//
	CParameterItem *moduleItem = m_list.FindParameter (module);
	if (!moduleItem)
	{
		moduleItem = new CParameterItem (module);
		m_list.AddChild (moduleItem);
	}

	// Check for duplicates
	if (moduleItem->FindParameter (name))
		g_error.ThrowDuplicateParameterFound (name);

	// Now add the parameter
	//
	CParameterItem *paramItem = new CParameterItem (name, value);
	moduleItem->AddChild (paramItem);
}

_TCHAR CParameterTable::ParseParameter (LPTSTR *line, LPCTSTR module)
{
	// Now we get the parameter name.  There are two possible forms
	//		1) <name> "=" <value>
	// or	2) <name>
	// The second form is simply shorthand for <name> "= true"
	//
	CParseWord param (_T(",= "));
	CParseSymbol equals (_T('='));
	_TCHAR next = param.GetToken (line);
	if ((next == _T('=')) || equals.GetToken (line))
	{
		// Get the parameter value and add to the table
		//
		CParseWord value (_T(","));
		next = value.GetToken (line);
		if (_tcslen (value) > 0)
			AddParameter (module, param, value);
		else
			g_error.ThrowExpectingParameterValue (param);
	}
	else
	// If this is just a parameter name, then we
	// assume its a boolean and set its value to true.
	//
	if ((next == _T(',')) || (next == _T('\0'))) 
		AddParameter (module, param, _T("true"));
	else
		g_error.ThrowExpectingParameterEnd (param);

	// We return the terminal for the case where
	// we are parsing a local parameter line
	//
	return next;
}

#ifdef PARAM_LIST_SYNTAX
	CParameterItem *CParameterTable::GetLastParameter (LPCTSTR module)
	{
		CParameterItem *moduleItem = m_list.FindParameter (module);
		return (CParameterItem *) moduleItem->GetLastChild (name);
	}
#endif

bool CParameterTable::ParseLine (LPTSTR line)
{
	CParseWord module (_T(". ,="));
	if (module.GetToken (&line) != _T('.'))
		g_error.ThrowExpectingParameterModule ();

	_TCHAR terminal = ParseParameter (&line, module);

	if (terminal == _T(','))

#ifndef PARAM_LIST_SYNTAX
		g_error.ThrowExpectingParameterEnd (_T(","));
#else
		CParameterItem *parentItem = GetLastParameter (module);

	// This loop parses a list of parameter values ...
	int count = 1;
	while (terminal == _T(','))
	{
		_TCHAR number [8];
		_stprintf (number, "%d", count);
		terminal = value.GetToken (&line);
		CParameterItem *newItem = new CParameterItem (number, value);
		parentItem->AddChild (newItem);
		count++;
	}
#endif
	return true;
}

int CParameterTable::GetCfgInt (LPCTSTR module, LPCTSTR name, int defaultInt)
{
	CParameterItem *param = FindParameter (module, name);
	return param? param->GetInt():defaultInt;
}

UINT CParameterTable::GetCfgUint (LPCTSTR module, LPCTSTR name, UINT defaultUint)
{
	CParameterItem *param = FindParameter (module, name);
	return param? param->GetUint():defaultUint;
}

LPCTSTR CParameterTable::GetCfgString (LPCTSTR module, LPCTSTR name, LPCTSTR defaultStr)
{
	CParameterItem *param = FindParameter (module, name);
	return param? param->GetValue():defaultStr;
}

int CParameterTable::GetCfgChoice (LPCTSTR module, LPCTSTR name, int defaultChoice, ...)
{
	va_list arglist;
	va_start (arglist, defaultChoice);

	return GetCfgChoice (module, name, defaultChoice, arglist);
}

int CParameterTable::GetCfgChoice (LPCTSTR module, LPCTSTR name, int defaultChoice, va_list argptr)
{
	CParameterItem *param = FindParameter (module, name);
	return param? param->GetChoice(argptr):defaultChoice;
}

bool CParameterTable::GetCfgBoolean (LPCTSTR module, LPCTSTR name, bool defaultbool)
{
	CParameterItem *param = FindParameter (module, name);
	return param? param->GetBoolean():defaultbool;
}


