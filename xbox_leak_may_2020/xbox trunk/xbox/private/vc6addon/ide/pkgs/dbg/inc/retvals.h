// retvals.h : header file
//
#ifndef __RETVALS_H__
#define __RETVALS_H__


class CRetVals : public CObject
{
public:
	CRetVals ();
	~CRetVals ();

	VOID CleanUp ();
	VOID AddTM (LPCSTR szName, LPCSTR szValue, LPCSTR szType = "");
	CPtrList &GetList () { return m_lstTMs; }


private:
	CPtrList m_lstTMs;
};


extern CRetVals g_retVals;	// the global instance


#endif // __RETVALS_H__
