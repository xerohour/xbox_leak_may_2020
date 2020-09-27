/////////////////////////////////////////////////////////////////////////////
//	UTIL.H
//		Mostly GDI and file utilities.

#ifndef __UTIL_H__
#define __UTIL_H__

#include <msf.h>

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
//	CLockDC
//		A CDC which handles locking and unlocking of updates to its window.
//		Used during drag loops.

class CLockDC : public CDC
{
public:
	CLockDC(CWnd* pWnd);
	~CLockDC();

protected:
	BOOL m_bLock;
	HWND m_hWnd;
};

/////////////////////////////////////////////////////////////////////////////
//	CImageWell
//		class provides a convenient way to handle "tiled" bitmaps.

class CImageWell
{
public:
	CImageWell();
	CImageWell(UINT nBitmapID, CSize imageSize);
	~CImageWell();
	void SetBitmap (CBitmap *pBitmap, CSize imageSize);
	
	BOOL Open();
	BOOL IsOpen() { return m_dc.m_hDC != NULL; }
	void Close();
	BOOL Load(UINT nBitmapID, CSize imageSize);
	BOOL LoadSysColor(UINT nBitmapID, CSize imageSize);
	BOOL Attach(HBITMAP hBitmap, CSize imageSize);
	HBITMAP Detach();
	void Unload();
	
	BOOL CalculateMask();
	
	BOOL DrawImage(CDC* pDestDC, CPoint destPoint, UINT nImage, 
		DWORD rop = 0); // rop==0 means image is drawn through a mask

	BOOL DrawImage(CDC* pDestDC, CPoint destPoint, UINT nImage, CSize sizeOverride,
		DWORD rop = 0); // rop==0 means image is drawn through a mask

	BOOL DrawMask(CDC* pDestDC, CPoint destPoint, UINT nImage, DWORD rop = NOTSRCCOPY);

	void GetImageSize(CSize& csImage)
	{
		csImage = m_imageSize;
	}
	
protected:
		
	CSize m_imageSize;
	
	// We use either this pointer or our member bitmap:
	CBitmap *m_pbitmap;

	UINT m_nBitmapID;
	CBitmap m_bitmap;
	
	CDC m_dc;
	CBitmap m_mask;
	CDC m_maskDC;
};

// hard-coded colors to be used instead of COLOR_BTNFACE, _BTNTEXT, etc.
//  - these are for access via MyGetSysColor() - and GetSysBrush()
#define CMP_COLOR_HILITE    21  // RGB(255, 255, 255)
#define CMP_COLOR_LTGRAY    22  // RGB(192, 192, 192) - instead of BtnFace
#define CMP_COLOR_DKGRAY    23  // RGB(128, 128, 128)
#define CMP_COLOR_BLACK     24  // RGB(0, 0, 0) - instead of frame

// - these are for when all you need is a RGB value)
#define CMP_RGB_HILITE      RGB(255, 255, 255)
#define CMP_RGB_LTGRAY      RGB(192, 192, 192)  // instead of BtnFace
#define CMP_RGB_DKGRAY      RGB(128, 128, 128)
#define CMP_RGB_BLACK       RGB(0, 0, 0)        // instead of frame

extern CBrush* GetSysBrush(UINT nSysColor);
extern void ResetSysBrushes();
extern CPen* GetSysPen(UINT nSysColor);
extern COLORREF MyGetSysColor(UINT nSysColor);

#define DSx		0x00660046L		// BitBlt ROPs
#define DSna	0x00220326L

// Paint this button "out," closer to the user.
#ifndef TBBS_EXTRUDED
#define TBBS_EXTRUDED 0x100
#endif

extern void DrawSplitter(CDC *pdc, CRect rect, BOOL bVert);
// Creates a push-button style bitmap
extern HBITMAP CreateButtonBitmap(CWnd* pWnd, UINT nStyle,  LPCRECT lpRect,
	CBitmap* pbmGlyph = NULL, LPCTSTR lpszText = NULL, BOOL bAddArrow = FALSE);
// creates a command bar style bitmap
extern HBITMAP CreateBarBitmap(CWnd* pWnd, UINT nStyle,  LPCRECT lpRect, CSize sizeButton, CSize sizeImage);

extern void ForceRectOnScreen(CRect& rect);
extern void ForceRectBelowTop(CRect& rect);

// For single pixel borders that are not actually window borders.
// Window borders should use system calculated border dimensions.
//
#define CX_BORDER 1
#define CY_BORDER 1
#define CY_TOOLBAR 28	// Toolbar height without borders.

extern BOOL PollForMouseChange(CPoint& pt, BOOL bYield, BOOL& bCancel, BOOL bAllowPaint=FALSE);
extern void FlushKeys();

//
// Given a EXE filename determine it's executable type if any.
//
typedef enum {
    EXE_FAIL,
    EXE_NONE,
    EXE_DOS,
    EXE_WIN,
    EXE_NT,
    EXE_OS2_NE,
    EXE_OS2_LE
} EXT; /* EXecutable Type */

extern EXT GetExeType(LPCTSTR lpstrEXEFilename, WORD * pwSubsystem);
extern UINT GetExePlatform(LPCTSTR szExeName);
extern BOOL IsFileGUIExe(LPSTR szFileName);
extern BOOL IsExecutableFile(LPCTSTR pszFileName);

#include <mmsystem.h>
extern BOOL PlayDevSound(UINT idsEvent, DWORD dwFlags = SND_ASYNC | SND_NODEFAULT);
extern BOOL PlayDevSound(const char* szEvent, DWORD dwFlags = SND_ASYNC | SND_NODEFAULT);
extern BOOL RegisterDevSoundEvent(UINT idsEventName, UINT idsFriendlyName);
extern BOOL RegisterDevSoundEvent(const char* szEventName, const char* szFriendlyName);

BOOL InEmergencyState();

// TRUE if a CCmdui is any kind of context menu
extern BOOL IsContextMenu(CCmdUI* pCmdUI);
extern BOOL IsMenu(CCmdUI* pCmdUI);
// Enable the menu if it has any visible items
extern void UpdateMenuCommand(CCmdUI* pCmdUI);

#ifdef _DEBUG
BOOL GetLogUndoFlag();
#endif

///////////////////////////////////////////////////////////////////////////////
//	CFileSaver
//		This is a little helper class for writing things to temporary files
//		and giving them the correct name after the save suceeds.

class CFileSaver
{
public:
	CFileSaver(const char* szFileName); // setup and create temp names
	~CFileSaver();                      // make sure temp files are gone
	BOOL CanSave() const;				// checks for R/O
	BOOL MakeWriteable() const;			// forces status of file to NOT R/O
	const char* GetSafeName() const     // return name of file to create
		{ return m_strTempName; }
	const char* GetRealName() const		// return name of final file
		{ return m_strName; }
	BOOL Finish();                      // rename new file as original

private:
	CString m_strName;
	CString m_strBackupName;
	CString m_strTempName;
	
	static const char BASED_CODE c_szAps [];
};

/////////////////////////////////////////////////////////////////////////////
//	CInitFile

class CInitFile : public CMemFile
{
	DECLARE_DYNAMIC(CInitFile)

public:
	enum INIT_DATATYPE { dataNil, dataReg, dataResource, dataStorage, dataMSF };

// Constructors
public:
	CInitFile();
	~CInitFile();
	
// Operations
public:
	BOOL Open(LPCSTR szSection, LPCSTR szKey, UINT nOpenFlags,
		INIT_DATATYPE type = dataReg);
	BOOL Open(LPSTORAGE pStorage, CString strStream, UINT nOpenFlags);
	BOOL Open(MSF *pmsf, WORD sn, UINT nOpenFlags);

	void SetBuffer(HGLOBAL hglobMem, UINT nCountBytes = -1,
		INIT_DATATYPE type = dataReg);
	HGLOBAL GetInitData();

	virtual void Close();	  // throw (CFileException);
	virtual void Abort();

protected:
	BOOL LoadFromSource();
	BOOL WriteToDest();

// Attributes
protected:
	BOOL m_bWrite;
	INIT_DATATYPE m_type;

	MSF *m_pmsf;
	WORD m_sn;

	LPCSTR m_lpszSection;
	LPCSTR m_lpszKey;

	HGLOBAL m_hglobUserMem;

	CString  m_strStream;
	LPSTREAM m_lpStream;

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
};

///////////////////////////////////////////////////////////////////////////////
//	CStateSaver

#define szWorkspaceSection _T("Workspace")
#define chMonikerSep _T('\n')

class CStateSaver
{
public:
	CStateSaver(BOOL bSaving);
	~CStateSaver();

	BOOL OpenMSF(LPCTSTR szFilePath, BOOL bForceWriteable);
	BOOL OpenStorage(LPCTSTR szFilePath);
	BOOL OpenStream(CInitFile& fileInit, CString strStream);
	void CloseStorage();
	BOOL Finish();

    const TCHAR * GetFileName() const { return (const TCHAR *)m_strFileName; }

	BOOL IsStoring() const { return m_bSaving; }
	BOOL IsLoading() const { return !m_bSaving; }
	BOOL IsFileBased() const { return (m_lpStorage != NULL || m_pMSF != NULL); }
	BOOL OpenFile(CInitFile& fileInit, SN nStream, LPCSTR szSection, LPCSTR szKey);

// Attributes
protected:
	BOOL m_bSaving;
	CFileSaver* m_pSaver;
    CString m_strFileName;
	LPSTORAGE m_lpStorage;

	MSF *m_pMSF;
};

/////////////////////////////////////////////////////////////////////////////
//	COLEStorage

class COLEStorage : public CObject
{
// Construction
public:
	COLEStorage(BOOL bReadOnly = TRUE);
	~COLEStorage();

// Attributes
public:
	LPCTSTR GetName();

// Operations
public:
	void Open(LPCTSTR lpszPath);
	void Create(LPCTSTR lpszPath);

// Sub-storage & stream operations
public:
	void OpenStream(LPCTSTR lpszName, COleStreamFile* pStream);
	void CreateStream(LPCTSTR lpszName, COleStreamFile* pStream);

	void OpenStorage(LPCTSTR lpszName, COLEStorage* pStorage);
	void CreateStorage(LPCTSTR lpszName, COLEStorage* pStorage);

// Implementation
protected:
	void* m_pStorage;
	DWORD m_grfMode;
};

// convert the windows ansi code page to the iso code page
// used by the htmx and htm packages for the code page
// meta data html tag
BOOL AnsiCodePageToIsoCodePage(/*[in]*/ LPCTSTR szAnsi, /*[out]*/ CString *pstrIso);

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif  // __UTIL_H__
