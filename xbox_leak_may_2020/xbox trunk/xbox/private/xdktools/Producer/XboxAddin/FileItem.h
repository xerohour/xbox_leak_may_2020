#if !defined(AFX_FILEITEM_H__52ED9541_11B3_4073_9CE0_6F8456E61502__INCLUDED_)
#define AFX_FILEITEM_H__52ED9541_11B3_4073_9CE0_6F8456E61502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileItem.h : header file
//

interface IDMUSProdNode;

/////////////////////////////////////////////////////////////////////////////
// CFileItem class

class CFileItem
{
public:
	CFileItem( IDMUSProdNode *pDMUSProdNode );
	~CFileItem();
	CString GetFileName( void ) const
	{
		return m_strFileName;
	}
	CString GetName( void ) const
	{
		return m_strDisplayName;
	}
	virtual HRESULT CopyToXbox( void );
	virtual HRESULT RemoveFromXbox( void );
	virtual bool UsesFile( const CFileItem *pFileItem );
	bool ContainsNode( const IDMUSProdNode *pNode );
	void SetAppendValue( int nNewAppendValue );

	IDMUSProdNode	*m_pFileNode;
	int				m_nAppendValue;

protected:
	static HRESULT CreateUniqueTempFile(CString sPrefix, CString sExtension, CString& sTempFileName);
	static IDMUSProdNode *FindChildNode( IDMUSProdNode *pDMUSProdNode, REFGUID rguidNodeId );
	static void GetRefsFromStyleBands( IDMUSProdNode *pDMUSProdNode, DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText );
	static void GetContainerNodes( bool fRefNodes, IDMUSProdNode *pDMUSProdNode, DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode );
	HRESULT CopyNodeReferences( IDMUSProdNode *pDMUSProdNode, bool &fDisplayMessageBoxOnError );

	CString 		m_strFileName;
	CString			m_strDisplayName;
	CString			m_strSourcePath;
	CTypedPtrList<CPtrList, IDMUSProdNode *> m_lstReferencedNodes;
	CTypedPtrList<CPtrList, IDMUSProdNode *> m_lstEmbeddedNodes;
};


#endif // !defined(AFX_FILEITEM_H__52ED9541_11B3_4073_9CE0_6F8456E61502__INCLUDED_)
