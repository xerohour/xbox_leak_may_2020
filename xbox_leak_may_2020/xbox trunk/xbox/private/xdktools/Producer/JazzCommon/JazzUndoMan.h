// Standard Jazz Undo Manager Class - header file

#ifndef __JAZZUNDOMAN_H_
#define __JAZZUNDOMAN_H_

#define MENU_TEXT_SIZE  32
#define DEFAULT_UNDO_LEVEL  8

const int gcbBuffer = 256;

typedef HRESULT (*SaveStateCallback)(void*);

class CJazzUndoNode
{
private:
	// The UndoMan class forms a double linked list that grows and shrinks
	// from both sides.  Only a pointer to the current node is necessary
	// for the app to maintain.  Prev of the current node are undo items.
	// Next of the current node are redo items.
	CJazzUndoNode  *m_pNext;
	CJazzUndoNode  *m_pPrev;

	// Each node maintains an memory stream with the actual state data and
	// a short text string identifying the operation that the data being
	// stored would undo.  This text is usually displayed in the edit menu
	// with the Undo and Redo items as a confirmation to the user.
	IStream  *m_pIMemStream;
	TCHAR  m_szMenuText[MENU_TEXT_SIZE];
	long  m_nUndoLevel;



public:
	CJazzUndoNode();
	~CJazzUndoNode();

	// These functions return a pointer to the new current node.  The current
	// node pointer must be assigned the return value for the class to work
	// properly.
	CJazzUndoNode  *SaveState(IDMUSProdFramework* pFramework,	// for allocating inside function
								FileType ft,
								GUID& guid,
								IPersistStream *pIPStream, TCHAR *psz );
	CJazzUndoNode  *Undo(IDMUSProdFramework* pFramework, 
						FileType ft,
						GUID& guid,
						IPersistStream *pIPStream );
	CJazzUndoNode  *Redo( IPersistStream *pIPStream );

	// GetUndo and GetRedo return FALSE if the operation is not possible, and
	// in this case, nothing is copied into pszText.
	BOOL  GetUndo( TCHAR *pszText, int nMaxCount );
	BOOL  GetRedo( TCHAR *pszText, int nMaxCount );

	// Destroy is a convenience function to delete all undo and redo nodes.
	void  Destroy();
	void  SetUndoLevel( long nUndoLevel );
};


// CJazzUndoMan is a wrapper class to hide the current node pointer.
class CJazzUndoMan
{
	friend CJazzUndoNode;
private:
	CJazzUndoNode  *m_pCurrentNode;
	IDMUSProdFramework	*m_pFramework;
	FileType		m_ftFileType;
	GUID			guidDataFormat;
public:
	CJazzUndoMan(IDMUSProdFramework* pFramework)
	{
		m_pFramework = pFramework;
		pFramework->AddRef();
		memcpy(&guidDataFormat, &GUID_CurrentVersion, sizeof(GUID));
		m_ftFileType = FT_DESIGN;
		m_pCurrentNode = new CJazzUndoNode;
	}
	~CJazzUndoMan()  {
		m_pFramework->Release();
		m_pCurrentNode->Destroy();
	}
	inline bool SaveState( IPersistStream *pStream,
							HINSTANCE hInstance, // handle of module containing string resource 
							UINT uID // resource identifier 
							)
	{
		TCHAR achbuf[gcbBuffer];
		UINT bytes = LoadString(hInstance, uID, achbuf, gcbBuffer);
		if(bytes > 0)
		{
			m_pCurrentNode = m_pCurrentNode->SaveState(m_pFramework, m_ftFileType, guidDataFormat, pStream, achbuf);
			return true;
		}
		else
		{
			return false;
		}
	}
	inline void SaveState( IPersistStream *pIPStream, TCHAR *psz )  
	{
		m_pCurrentNode = m_pCurrentNode->SaveState(m_pFramework, m_ftFileType, guidDataFormat, pIPStream, psz );
	}
	inline void Undo( IPersistStream *pIPStream )  {
		m_pCurrentNode = m_pCurrentNode->Undo(m_pFramework, m_ftFileType, guidDataFormat, pIPStream );
	}
	inline void Redo( IPersistStream *pIPStream )  {
		m_pCurrentNode = m_pCurrentNode->Redo( pIPStream );
	}
	inline BOOL  GetUndo( TCHAR *pszText, int nMaxCount )  {
		return m_pCurrentNode->GetUndo( pszText, nMaxCount );
	}
	inline BOOL  GetRedo( TCHAR *pszText, int nMaxCount )  {
		return m_pCurrentNode->GetRedo( pszText, nMaxCount );
	}
	inline void  SetUndoLevel( long nUndoLevel )  {
		m_pCurrentNode->SetUndoLevel( nUndoLevel );
}	
	inline void	 SetStreamAttributes(FileType ft, GUID guid)
	{
		m_ftFileType = ft;
		memcpy(&guidDataFormat, &guid, sizeof(GUID));
	}
	inline void GetStreamAttributes(FileType& ft, GUID& guid)
	{
		ft = m_ftFileType;
		memcpy(&guid, &guidDataFormat, sizeof(GUID));
	}

};

#endif // !defined(JAZZUNDOMAN)

