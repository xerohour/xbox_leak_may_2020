/////////////////////////////////////////////////////////////////////////////
//	SHLFUTR_.H
//		Microsoft only header file with features we don't yet want to
//		expose to the rest of the world.  Right now, just autosave, which
//		was originally implemented in v2.0, but has not yet been supported
//		by more than the text editor.

#ifndef __SHLFUTR__H__
#define __SHLFUTR__H__

#include "path.h"
#include "msgboxes.h"

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
//	CAutosave

#define IDT_AUTOSAVE 100

class CAutosave
{
private:
	BOOL m_fEnabled;
	BOOL m_fInTick;	 			
	UINT m_nSaveInterval;
	HWND m_hWndTimerOwner;
	UINT m_nTimerID;
	CDir m_dirUntitled;
	CObList m_pathlistErrors;
	void GetUntitledDir();
public:
	CAutosave();
	~CAutosave();

// Public Enumerations

	enum DirAction { 
		DontCreateDir = FALSE, 
		CreateDir = TRUE 
	};

	enum AutosaveErrors {
		Success = TRUE, 
		NoSubdirectory,
		CantMakeSubdirectory,
		PathTooLong
	} m_ErrorStatus;

// Installation Method.  
// The autosaver must be installed on an HWND before it is enabled.

	inline void Install(HWND hWnd);

// Interface to the Options dialogue.

	BOOL IsEnabled()   	{ return(m_fEnabled); }
	UINT GetInterval() 	{ return(m_nSaveInterval); }
	BOOL Enable(UINT nInterval);
    BOOL Enable()	   	{ return(Enable(m_nSaveInterval)); }
	BOOL Disable();

// Interface to the HWND timer message.

	inline void EnterTick();
	BOOL InTick() 		{ return (m_fEnabled && m_fInTick);}
	inline void RegisterTickError(CPath &pathBadAutosave);
	void ExitTick();

// Autosave Services.  Virtual so we get pointers to the methods.

	virtual BOOL GetAutosaveName(BOOL fUntitled, const CPath &pathDirtyFile, 
						 CPath &pathAutosave, 
						 enum DirAction fCreateDir = CreateDir);
	virtual void RemoveAutosaveFile(BOOL fUntitled, const CPath &pathUserFile);
	virtual void RemoveAutosaveFile(const CPath &pathUserFile) {
		RemoveAutosaveFile(FALSE, pathUserFile);
	}
	virtual BOOL AutosaveFileExists(const CPath &pathUserFile, CPath &pathAutosave);
	virtual BOOL RecoverAutosaveFileCheck(const CPath &pathUserFile);
};

inline void CAutosave::Install(HWND hWnd) 
{
	if (!m_fEnabled) 
		m_hWndTimerOwner = hWnd;
	else
	{
		VERIFY(Disable());
		m_hWndTimerOwner = hWnd;
		VERIFY(Enable());
	}
}

inline void CAutosave::EnterTick()
{
	ASSERT(!m_fInTick);
	ASSERT(m_pathlistErrors.IsEmpty());
	m_fInTick = TRUE;
}

inline void CAutosave::RegisterTickError(CPath &pathError)
{
	CPath *ppathErrEntry = new CPath(pathError);
	m_pathlistErrors.AddTail(ppathErrEntry);
}

extern CAutosave AFX_EXT_DATA gAutosaver;	

#undef AFX_DATA
#define AFX_DATA NEAR

#endif	// !__SHLFUTR__H__

