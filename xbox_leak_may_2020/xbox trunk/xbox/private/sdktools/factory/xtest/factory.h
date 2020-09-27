// factory.h

#pragma once

class CTestObj;

typedef CTestObj* (*CreateFunction) ();

class CTestItem : public CTreeItem
{
	CreateFunction m_func;

public:
	CTestItem (LPCTSTR name, CreateFunction func = NULL)
		:CTreeItem(name), m_func(func) {}
	~CTestItem () {}

	CreateFunction GetFunction () {return m_func;}
};

// CTestFactory
//
// This object contains the list of all available tests.
//
class CTestFactory
{
	CTestItem m_list;

public:
	CTestFactory ():m_list(_T("factory")) {} 
	~CTestFactory () {m_list.DeleteChildren ();}

	void RegisterTest (LPCTSTR name, CreateFunction func); 
	CTestObj *CreateTest (LPCTSTR name);
	CTestItem *LookupTest (LPCTSTR module, LPCTSTR test = NULL);
};


