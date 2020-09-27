//
// CExeViewSlob
//
// Exe Project View Slob
//
// [colint]
//

#ifndef _INCLUDE_EXEVWSLOB_H
#define _INCLUDE_EXEVWSLOB_H

class CExeViewSlob : public CProjSlob
{
	DECLARE_SERIAL(CExeViewSlob)

public:
	CExeViewSlob();
	~CExeViewSlob();

	void SetFilterTarget(HBLDTARGET hTarget);

	virtual BOOL CanAct(ACTION_TYPE action)	{ return FALSE; }
 
	virtual CObList * GetContentList() { return NULL; }

	virtual BOOL SetIntProp(UINT idProp, int val);
	virtual BOOL SetStrProp(UINT idProp, const CString & val);
	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString & val);

	// Property Window Interface
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

    virtual void Serialize(CArchive & ar);

    virtual void GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds); 

private:
	CString m_strTitle;

	BOOL m_bIsInvisibleNode;

 	CProject * m_pProject;

	DECLARE_SLOBPROP_MAP();
};

#endif // _INCLUDE_EXEVWSLOB_H
