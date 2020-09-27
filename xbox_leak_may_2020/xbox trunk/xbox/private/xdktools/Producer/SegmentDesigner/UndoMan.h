// Standard Jazz Undo Manager Class - header file

#define MENU_TEXT_SIZE  32
#define DEFAULT_UNDO_LEVEL  24

interface IDMUSProdFramework;

class CUndoNode
{
private:
	// The UndoMan class forms a double linked list that grows and shrinks
	// from both sides.  Only a pointer to the current node is necessary
	// for the app to maintain.  Prev of the current node are undo items.
	// Next of the current node are redo items.
	CUndoNode  *m_pNext;
	CUndoNode  *m_pPrev;

	// Each node maintains an memory stream with the actual state data and
	// a short text string identifying the operation that the data being
	// stored would undo.  This text is usually displayed in the edit menu
	// with the Undo and Redo items as a confirmation to the user.
	IStream  *m_pIMemStream;
	char  m_szMenuText[MENU_TEXT_SIZE];
	long  m_nUndoLevel;

public:
	CUndoNode();
	~CUndoNode();

	// These functions return a pointer to the new current node.  The current
	// node pointer must be assigned the return value for the class to work
	// properly.
	CUndoNode  *SaveState( IPersistStream *pIPStream, const char *psz, IDMUSProdFramework *pIFramework );
	CUndoNode  *Undo( IPersistStream *pIPStream, IDMUSProdFramework *pIFramework );
	CUndoNode  *Redo( IPersistStream *pIPStream );

	// GetUndo and GetRedo return FALSE if the operation is not possible, and
	// in this case, nothing is copied into pszText.
	BOOL  GetUndo( char *pszText, int nMaxCount );
	BOOL  GetRedo( char *pszText, int nMaxCount );

	// Destroy is a convenience function to delete all undo and redo nodes.
	void  Destroy();
	void  SetUndoLevel( long nUndoLevel );
};

// CSegmentUndoMan is a wrapper class to hide the current node pointer.
class CSegmentUndoMan
{
friend class CSegment;

private:
	CUndoNode  *m_pCurrentNode;
	BOOL		m_bNeedDesignInfo;

public:
	CSegmentUndoMan()  {
		m_pCurrentNode = new CUndoNode;
		m_bNeedDesignInfo = FALSE;
	}
	~CSegmentUndoMan()  {
		m_pCurrentNode->Destroy();
	}
	inline void SaveState( IPersistStream *pIPStream, const char *psz, IDMUSProdFramework *pIFramework )  {
		m_bNeedDesignInfo = TRUE;
		m_pCurrentNode = m_pCurrentNode->SaveState( pIPStream, psz, pIFramework );
		m_bNeedDesignInfo = FALSE;
	}
	inline void Undo( IPersistStream *pIPStream, IDMUSProdFramework *pIFramework )  {
		m_bNeedDesignInfo = TRUE;
		m_pCurrentNode = m_pCurrentNode->Undo( pIPStream, pIFramework );
		m_bNeedDesignInfo = FALSE;
	}
	inline void Redo( IPersistStream *pIPStream )  {
		m_bNeedDesignInfo = TRUE;
		m_pCurrentNode = m_pCurrentNode->Redo( pIPStream );
		m_bNeedDesignInfo = FALSE;
	}
	inline BOOL  GetUndo( char *pszText, int nMaxCount )  {
		return m_pCurrentNode->GetUndo( pszText, nMaxCount );
	}
	inline BOOL  GetRedo( char *pszText, int nMaxCount )  {
		return m_pCurrentNode->GetRedo( pszText, nMaxCount );
	}
	inline void  SetUndoLevel( long nUndoLevel )  {
		m_pCurrentNode->SetUndoLevel( nUndoLevel );
}	};

