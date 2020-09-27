// parameter.h

#pragma once

class CLineParser;

class CParameterItem : public CTreeItem
{
	CAutoDelete<_TCHAR> m_value;

public:
	CParameterItem (LPCTSTR name, LPCTSTR value = NULL);

	bool GetBoolean ();
	int GetInt ();
	UINT GetUint ();
	LPCTSTR GetValue () {return m_value.get();}
	int GetChoice (va_list argptr);
	CParameterItem *FindParameter (LPCTSTR name) 
		{return (CParameterItem *) ((name)? FindChild(name):GetFirstChild());}
};

class CParameterTable : public CParseLine
{
	CParameterItem m_list;

public:
	CParameterTable ():m_list(_T("parameters")) {}
	~CParameterTable () {m_list.DeleteChildren();}

	CParameterItem *FindParameter (LPCTSTR module, LPCTSTR name);
	void AddParameter (LPCTSTR module, LPCTSTR name, LPCTSTR value);
	_TCHAR ParseParameter (LPTSTR *line, LPCTSTR module = NULL);
	bool ParseLine (LPTSTR line);

	bool GetCfgBoolean (LPCTSTR module, LPCTSTR name, bool defaultbool);
	int GetCfgInt (LPCTSTR module, LPCTSTR name, int defaultInt);
	UINT GetCfgUint (LPCTSTR module, LPCTSTR name, UINT defaultUint);
	LPCTSTR GetCfgString (LPCTSTR module, LPCTSTR name, LPCTSTR defaultStr);
	int GetCfgChoice (LPCTSTR module, LPCTSTR name, int defaultChoice, ...);
	int GetCfgChoice (LPCTSTR module, LPCTSTR name, int defaultChoice, va_list argptr);
};

