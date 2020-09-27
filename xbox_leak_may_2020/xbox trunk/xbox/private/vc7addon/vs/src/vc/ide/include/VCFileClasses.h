// (DianeMe) This was the MFC file manipulation stuff
// Now it has been purged of all other MFC influences and is a stand alone entity.
// NOTE: exceptions are no longer thrown on various errors...

#pragma once

#include <time.h>
#include <vcstring.h>
#include <vctimeclasses.h>

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class CVCVCFile;
class CVCStdioFile;
class CVCMemFile;
class CVCSharedFile;

struct CVCFileStatus;                   // file status information

/////////////////////////////////////////////////////////////////////////////
// File - raw unbuffered disk file I/O

class CVCFile
{
public:
// Flag values
	enum OpenFlags {
		modeRead =          0x0000,
		modeWrite =         0x0001,
		modeReadWrite =     0x0002,
		shareCompat =       0x0000,
		shareExclusive =    0x0010,
		shareDenyWrite =    0x0020,
		shareDenyRead =     0x0030,
		shareDenyNone =     0x0040,
		modeNoInherit =     0x0080,
		modeCreate =        0x1000,
		modeNoTruncate =    0x2000,
		typeText =          0x4000, // typeText and typeBinary are used in
		typeBinary =   (int)0x8000 // derived classes only
		};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	static const HANDLE hFileNull; // INVALID_HANDLE_VALUE

// Constructors
	CVCFile();

// Attributes
	HANDLE m_hFile;
	operator HANDLE() const { return m_hFile; } 

	virtual ULONGLONG GetPosition() const;
	BOOL GetStatus(CVCFileStatus& rStatus) const;
	virtual CVCString GetFileName() const;
	virtual CVCString GetFileTitle() const;
	virtual CVCString GetFilePath() const;
	virtual void SetFilePath(LPCTSTR lpszNewName) { m_strFileName = lpszNewName; }

// Operations
	virtual BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags, DWORD dwAttribs = FILE_ATTRIBUTE_NORMAL);

	static void PASCAL Rename(LPCTSTR lpszOldName, LPCTSTR lpszNewName);
	static void PASCAL Remove(LPCTSTR lpszFileName);
	static BOOL PASCAL GetStatus(LPCTSTR lpszFileName, CVCFileStatus& rStatus);
	static void PASCAL SetStatus(LPCTSTR lpszFileName, const CVCFileStatus& status);

	ULONGLONG SeekToEnd() { return Seek(0, CVCFile::end); }
	void SeekToBegin() { Seek(0, CVCFile::begin); }

// Overridables
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual ULONGLONG GetLength() const;

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual BOOL Write(const void* lpBuf, UINT nCount);

	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

// Implementation
public:
	virtual ~CVCFile();
	enum BufferCommand { bufferRead, bufferWrite, bufferCommit, bufferCheck };
	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
		void** ppBufStart = NULL, void** ppBufMax = NULL);

protected:
	BOOL m_bCloseOnDelete;
	CVCString m_strFileName;
};

/////////////////////////////////////////////////////////////////////////////
// STDIO file implementation

class CVCStdioFile : public CVCFile
{
public:
// Constructors
	CVCStdioFile();
	CVCStdioFile(FILE* pOpenStream);
	CVCStdioFile(LPCSTR lpszFileName, UINT nOpenFlags);

// Attributes
	FILE* m_pStream;    // stdio FILE
						// m_hFile from base class is _fileno(m_pStream)

// Operations
	// reading and writing strings
	virtual BOOL WriteString(LPCSTR lpsz);
	virtual LPTSTR ReadString(LPTSTR lpsz, UINT nMax);
	virtual BOOL ReadString(CVCString& rString);

// Implementation
public:
	virtual ~CVCStdioFile();
	virtual ULONGLONG GetPosition() const;
   virtual ULONGLONG GetLength() const;
	virtual BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags, DWORD dwAttribs = FILE_ATTRIBUTE_NORMAL);
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual BOOL Write(const void* lpBuf, UINT nCount);
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();

	// Unsupported APIs
	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);
};

////////////////////////////////////////////////////////////////////////////
// Memory based file implementation

class CVCMemFile : public CVCFile
{
public:
// Constructors
	CVCMemFile(SIZE_T nGrowBytes = 1024);
	CVCMemFile(BYTE* lpBuffer, SIZE_T nBufferSize, SIZE_T nGrowBytes = 0);

// Operations
	void Attach(BYTE* lpBuffer, SIZE_T nBufferSize, SIZE_T nGrowBytes = 0);
	BYTE* Detach();

// Advanced Overridables
protected:
	virtual BYTE* Alloc(SIZE_T nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, SIZE_T nBytes);
	virtual BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource, SIZE_T nBytes);
	virtual void Free(BYTE* lpMem);
	virtual void GrowFile(SIZE_T dwNewLen);

// Implementation
protected:
	SIZE_T m_nGrowBytes;
	SIZE_T m_nPosition;
	SIZE_T m_nBufferSize;
	SIZE_T m_nFileSize;
	BYTE* m_lpBuffer;
	BOOL m_bAutoDelete;

public:
	virtual ~CVCMemFile();
	virtual ULONGLONG GetPosition() const;
	BOOL GetStatus(CVCFileStatus& rStatus) const;
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual BOOL Write(const void* lpBuf, UINT nCount);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();
	virtual SIZE_T GetBufferPtr(UINT nCommand, SIZE_T nCount = 0, void** ppBufStart = NULL, void** ppBufMax = NULL);
	virtual ULONGLONG GetLength() const;

	// Unsupported APIs
	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class CVCSharedFile : public CVCMemFile
{
public:
// Constructors
	CVCSharedFile(UINT nAllocFlags = GMEM_DDESHARE|GMEM_MOVEABLE,
		UINT nGrowBytes = 4096);

// Attributes
	HGLOBAL Detach();
	void SetHandle(HGLOBAL hGlobalMemory, BOOL bAllowGrow = TRUE);

// Implementation
public:
	virtual ~CVCSharedFile();

protected:
	virtual BYTE* Alloc(DWORD nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, DWORD nBytes);
	virtual void Free(BYTE* lpMem);

	UINT m_nAllocFlags;
	HGLOBAL m_hGlobalMemory;
	BOOL m_bAllowGrow;
};

/////////////////////////////////////////////////////////////////////////////
// File status

struct CVCFileStatus
{
	CVCTime m_ctime;          // creation date/time of file
	CVCTime m_mtime;          // last modification date/time of file
	CVCTime m_atime;          // last access date/time of file
	ULONGLONG m_size;            // logical size of file in bytes
	BYTE m_attribute;       // logical OR of CVCFile::Attribute enum values
	BYTE _m_padding;        // pad the structure to a WORD
	TCHAR m_szFullName[_MAX_PATH]; // absolute path name
};

extern BOOL VCFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
extern void VCGetRoot(LPCTSTR lpszPath, CVCString& strRoot);
extern UINT VCGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
extern UINT VCGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
extern void __cdecl VCTimeToFileTime(const CVCTime& time, LPFILETIME pFileTime);
