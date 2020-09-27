

#ifndef __SHELLREC_H
#define __SHELLREC_H

/*

	Copyright 1996, Microsoft Corp.

	SHELLREC.H
		This header contains declarations for the CShellRecorder object, which
		manages macro recording for the shell.

	HISTORY

		08-23-96	CFlaat	Created in something like the current form.

*/

#include <aut1api.h>
#include <utilauto.h>

class CShellAction;


enum ShellElement
{ 
	FirstElement = 0,

	// Documents elements

	seSaveAll = FirstElement,
	FirstDocumentsElement = seSaveAll,
	LastDocumentsElement = seSaveAll,

	// Window elements
	seCloseWin,
	FirstWindowElement = seCloseWin,
	seNewWin,
	LastEditorElement = seNewWin,

	// Count of Elements
	ElementCount
};

enum ShellConst
{
	FirstConst = 0,
	scSaveChangesPrompt = FirstConst,
	scTrue,
	LastConst = scTrue
};

class CShellRecorder : public CGeneralRecorder
{
friend class CTheApp; // for initialization

protected: // data

	static const TCHAR * const szPkgName;
	static IMacroRecorder *m_pMacroRecorder; // REVIEW(CFlaat): is this a valid assumption?

	CShellAction *m_pCurrentShellAction;

protected: // methods

	void SetMacRec(IMacroRecorder *pMacRec) { VERIFY(m_pMacroRecorder = pMacRec); }
	IMacroRecorder *GetMacRec(void) const { ASSERT(m_pMacroRecorder); return m_pMacroRecorder; }

protected: // methods

	virtual LPCTSTR GetPkgName(void) const { return szPkgName; }
	void EmitCurrentAction(void);

public: // methods

	CShellRecorder(void);
	virtual ~CShellRecorder(void);

	BOOL InRecordingMode() const;

	void EmitPropertyAssignment(ShellElement eProperty, CRecArg &arg);
	void EmitParameterizedPropertyAssignment(ShellElement eProperty, CRecArg &param, CRecArg &value);

	void EmitMethodCall(ShellElement eMethod);
	void EmitMethodCall(ShellElement eMethod, CRecArg &arg1);
};


class CShellConstArg;

class CShellAction
{
public: // types

	enum ActionCat { acProperty = 1, acMethod = 2 }; // categories of actions (other things may come in the future)
	enum { MaxArgCount = 5 };

//protected: // data

	// this needs to be public until DS bug #4579 is resolved, letting us use friend templates
public: // data 

	CRecArg *m_aArgs[MaxArgCount];
	unsigned m_cArgs;
	ActionCat m_eCategory;
	ShellElement m_eElement;

protected: // methods

//	friend class CEditConstArg;

/*	friend template <class ValType, class ArgContainerType>
ValType GetArg(CEditAction *, unsigned, ValType, BOOL);

	friend template <class ValType, class ArgContainerType>
void SetArg(CEditAction *, unsigned int, ValType);*/


	void GetElementText(CString &str, LPCTSTR szParam = 0) const;

//	EditConst GetConstArg(unsigned iArg, EditConst ecDefault, BOOL bCreate = FALSE);
//	int GetIntArg(unsigned iArg, int nDefault, BOOL bCreate = FALSE);
//	LPCTSTR GetQuotedStringArg(unsigned iArg, LPCTSTR szDefault, BOOL bCreate = FALSE);

//	void SetIntArg(unsigned iArg, int nValue);
//	void SetQuotedStringArg(unsigned iArg, LPCTSTR szValue);

#ifdef _DEBUG
	void CheckZBArgType(int nIndex, const type_info & Type);
	void CheckArgCount(int nIndex);
	void CheckArgCountRange(int nLow, int nHigh);
	void VerifyCorrectness(void);
#endif

public: // methods

	CShellAction(ShellElement ee, ActionCat ac);
	CShellAction(ShellElement ee, ActionCat ac, CRecArg &arg1);
	CShellAction(ShellElement ee, ActionCat ac, CRecArg &arg1, CRecArg &arg2);
	CShellAction(ShellElement ee, ActionCat ac, CRecArg &arg1, CRecArg &arg2, CRecArg &arg3);

	~CShellAction(void); // make this virtual if we use polymorphism later on

	void GetText(CString &str); // gets the language-specific representation

};



// shell-specific arg types

class CShellRecorder;


class CShellConstArg : public CRecArg
{
protected: // data

	ShellConst m_scData;

private: // unused ctor

	CShellConstArg(void) { ASSERT(FALSE); } // this ctor shouldn't be used!!

public: // methods

	CShellConstArg(ShellConst scData) : m_scData(scData) {}

	virtual void GetText(CString &str);

	virtual CRecArg *Clone(void) { return new CShellConstArg(m_scData); }

	ShellConst GetValue(void) { return m_scData; }
};


extern CShellRecorder g_theShellRecorder;


#endif // __SHELLREC_H

