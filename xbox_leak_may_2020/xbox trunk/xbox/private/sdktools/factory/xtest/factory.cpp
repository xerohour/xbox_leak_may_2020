// factory.cpp
//

#include "stdafx.h"

#define SPLIT(original, first, second) \
		int _len_ = _tcslen (original) + 1; \
		LPTSTR _buffer_ = (LPTSTR) alloca (_len_*2); \
		_tcscpy (_buffer_, original); \
		first = _buffer_; \
		_buffer_ = _tcschr (_buffer_, _T('.')); \
		if (_buffer_) *_buffer_++ = 0; \
		second = _buffer_

void CTestFactory::RegisterTest (LPCTSTR name, CreateFunction func)
{
	LPCTSTR module,  test;
	SPLIT (name, module, test);

	// If this is the first parameter from a module,
	// we will need to create the module node first
	//
	CTestItem *moduleItem = (CTestItem *) m_list.FindChild (module);
	if (!moduleItem)
	{
		moduleItem = new CTestItem (module);
		m_list.AddChild (moduleItem);
	}

	// Check for duplicates
	ASSERT (!moduleItem->FindChild (test));

	CTestItem *paramItem = new CTestItem (test, func);
	moduleItem->AddChild (paramItem);
}

CTestObj *CTestFactory::CreateTest (LPCTSTR name)
{
	CTestItem *testItem = LookupTest (name);
	return (*testItem->GetFunction())();
}

CTestItem *CTestFactory::LookupTest (LPCTSTR module, LPCTSTR test)
{
	if (test == NULL)
	{
		SPLIT (module, module, test);
	}

	CTestItem *moduleItem = (CTestItem *) m_list.FindChild (module);
	if (!moduleItem)
		return NULL;

	return (CTestItem *) moduleItem->FindChild (test);
}

