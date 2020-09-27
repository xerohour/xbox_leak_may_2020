// NotifyList.h: interface for the CNotifyList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFYLIST_H__7ADE0889_DB41_11D2_B43E_00105A2796DE__INCLUDED_)
#define AFX_NOTIFYLIST_H__7ADE0889_DB41_11D2_B43E_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNotifyList  
{
public:
	CNotifyList();
	virtual ~CNotifyList();

	HRESULT Detach( CFileNode* pFileNode );
	HRESULT Attach( CFileNode* pFileNode );

protected:
	IDMUSProdNode* m_pIOrigDocRootNode;
	IDMUSProdNode* m_pINewDocRootNode;

    CTypedPtrList<CPtrList, CJzNotifyNode*> m_lstNotifyNodes;
};

#endif // !defined(AFX_NOTIFYLIST_H__7ADE0889_DB41_11D2_B43E_00105A2796DE__INCLUDED_)
