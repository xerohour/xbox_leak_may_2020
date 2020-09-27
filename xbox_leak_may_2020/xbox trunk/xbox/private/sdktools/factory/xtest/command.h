// command.h 

#pragma once

#include "tree.h"
#include "parser.h"

class CParameterTable;
class CTestFlowManager;

enum XCMD_TYPE {XCMD_INVALID, XCMD_STARTTEST, XCMD_RUNTEST, XCMD_ENDLOOP, XCMD_EXIT,
					XCMD_STARTLOOP, XCMD_RUNLOOP};

class CCommandItem : public CTreeItem
{
	XCMD_TYPE m_type;
	CAutoDelete <CParameterTable> m_parameters;

public:
//	static LPCTSTR *CommandNames [7]; 

public:
	CCommandItem (LPCTSTR name, XCMD_TYPE type);
	~CCommandItem ();

	XCMD_TYPE LookupCommand (LPCTSTR typeName);
	XCMD_TYPE GetCommandType () {return m_type;}
	CParameterTable *GetParameters () {return m_parameters.get ();}
	CCommandItem *AddCommand (LPCTSTR module, LPCTSTR test, XCMD_TYPE type);
};

class CCommandList : public CParseLine
{
	CCommandItem m_list;
	CCommandItem *m_parsingList;

public:
	CCommandList ():m_list(CCommandItem (_T("commands"), XCMD_INVALID))
		{m_parsingList = &m_list;}
	~CCommandList () {m_list.DeleteChildren();}

	bool ParseLine (LPTSTR line);
	CCommandItem *GetList () {return &m_list;}
};

