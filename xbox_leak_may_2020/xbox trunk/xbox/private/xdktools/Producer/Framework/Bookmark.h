// Bookmark.h: interface for the CBookmark class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOOKMARK_H__1FD3A982_F2D7_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_BOOKMARK_H__1FD3A982_F2D7_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CComponentView;

class CBookmark  
{
public:
	CBookmark();
	virtual ~CBookmark();

public:
	BOOL Create();
	HRESULT LoadBookmark( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SaveBookmark( CProject* pProject, IDMUSProdRIFFStream* pIRiffStream );
	void Apply( BOOL fCloseWindows );
	BOOL IsForThisProject( CProject* pProject );
	void MergeBookmark( CBookmark* pBookmark );
	void Duplicate();
	void AdjustFileReferenceChunks( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL* pfChanged );
	BOOL IsValid();

private:
	HRESULT SaveGUID( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveNodeName( IDMUSProdRIFFStream* pIRiffStream, LPCTSTR szNodeName );
	short GetZOrder( CMDIChildWnd* pFrame );
	void AddToEditorList( CComponentDoc* pComponentDoc, CComponentView* pComponentView );
	BOOL OpenEditor( wpWindowPlacement* pWP );
	void SetWP();

// member variables
public:
	CString m_strName;
    GUID	m_guid;			// Bookmark's GUID	

private:
    CTypedPtrList<CPtrList, wpWindowPlacement*> m_lstEditors;
    CTypedPtrList<CPtrList, bkComponentState*> m_lstComponents;

};

#endif // !defined(AFX_BOOKMARK_H__1FD3A982_F2D7_11D0_89AE_00A0C9054129__INCLUDED_)
