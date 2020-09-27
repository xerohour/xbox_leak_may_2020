// tipmgr.cpp

#include "stdafx.h"
#include "resource.h"
#include "main.h"
#include "totd.h"
#include "tipmgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static const char BASED_CODE szTipStateKey[] = "Help\\Tips";

#define MAX_TIP_LINE 1024
//#define MAX_TIP_FILES 50 // restrict # of tip files

/////////////////////////////////////////////////////////////////////////////
BOOL isTipAvailable()
{
	CTipFileManager TipData;
	return TipData.isTipAvailable();
}

#define KEY_START (_T('{'))
#define KEY_END   (_T('}'))
#define KEY_SEP   (_T(':'))

/////////////////////////////////////////////////////////////////////////////
//  Escape
//  Copy Src to Dst, translating escape sequences and command keys
//
//  Returns: FALSE if a command was found which was not assigned to a key,
//			and TRUE otherwize.
//
//  Beware of aliasing: Src may also be Dst
#pragma optimize( "a", off)
static BOOL Escape( LPTSTR Dst, LPCTSTR Src, UINT cbMax )
{
	BOOL fRet = TRUE;
	ASSERT(Dst!=NULL);
	ASSERT(Src!=NULL);
	UINT cb = 0;
	while (*Src && cb < cbMax)
	{
		if( *Src == _T('\\') )
		{
			++Src;
			switch( *Src )
			{
			//  Octal
			case _T('0'): case _T('1'): case _T('2'): case _T('3'):
			case _T('4'): case _T('5'): case _T('6'): case _T('7'):
				*Dst++ = (_TCHAR)_tcstoul(Src, (LPTSTR*)&Src, 8);
				cb++;
				continue;
			//  Hex
			case _T('x'):
				Src++;
				*Dst++ = (_TCHAR)_tcstoul(Src, (LPTSTR*)&Src, 16);
				cb++;
				continue;
			case _T('a') : *Dst++ = _T('\a'); Src++; continue;
			case _T('b') : *Dst++ = _T('\b'); Src++; continue;
			case _T('f') : *Dst++ = _T('\f'); Src++; continue;
			case _T('n') : *Dst++ = _T('\n'); Src++; continue;
			case _T('r') : *Dst++ = _T('\r'); Src++; continue;
			case _T('t') : *Dst++ = _T('\t'); Src++; continue;
			case _T('v') : *Dst++ = _T('\v'); Src++; continue;
			case _T('\0'): goto EndEscape;
			default:
				//any other character is literal
				goto CopyChar;
				break;
			}
		}
		else if (*Src == KEY_START)
		{
			// {[<id>:][<command>]}
			// <id>		  Command ID #
			// <command>  Command name (case sensitive)
			LPTSTR pch, pchEnd;
			CString strName;

			if ((pch = _tcschr(Src, KEY_END)) == 0)
				goto CopyChar;
			pchEnd = _tcsinc(pch);
			_TCHAR szCmdName[65];
			_tcsncpy(szCmdName, Src+1, pch - (Src+1));
			szCmdName[pch - (Src+1)] = _T('\0');
			
			UINT nId = 0;
			pch = szCmdName;
			if (_istdigit(*pch))
			{
				nId = _tcstoul(pch, &pch, 0);
				if (*pch == KEY_SEP)
					pch = _tcsinc(pch);
			}
			if (nId || theCmdCache.GetCommandID(pch, &nId))
			{
				// Get key string
				CString strKey;
				if (GetCmdKeyStringAll( nId, strKey ))
				{
					cb += strKey.GetLength();
					if (cb >= cbMax) goto EndEscape;
					ASSERT(!strKey.IsEmpty());
					_tcscpy(Dst,strKey);
					Dst = _tcsninc(Dst,_tcslen(strKey));
					Src = pchEnd;
					continue;
				}
				else
				{
					// unassigned
					fRet = FALSE;
					goto EndEscape;
// [fabriced] Turned off if the command is not assigned, the tip will not show up.
#if 0
					if (*pch)
					{
						// use name in syntax, (might be wrong)
CopyName:				// copy command name
						cb += 2*(sizeof KEY_START) + strlen(pch);
						if (cb >= cbMax) goto EndEscape;
						*Dst = KEY_START; Dst = _tcsinc(Dst);
						_tcscpy(Dst,pch); Dst = _tcsninc(Dst,_tcslen(pch));
						*Dst = KEY_END;   Dst = _tcsinc(Dst);
						Src = pchEnd;
						continue;
					}
					else if (nId)
					{
						// id but no command name: look up name
						LPCTSTR pchConst;

						if (theCmdCache.GetCommandString(nId, STRING_COMMAND, &pchConst))
						{
							// should not hold onto return value, so for safety, copy into local cstring bobz
							strName = pchConst;
							pch = (LPTSTR)(LPCTSTR)strName;	// we don't change it, really

							goto CopyName;
						}
					}
#endif //0
				}
			}
		}
CopyChar:
		//copy any other character
		cb += _tclen(Dst);
		if (cb >= cbMax) goto EndEscape;
		_tccpy(Dst,Src);
		Dst = _tcsinc(Dst);
		Src = _tcsinc(Src);
	}
EndEscape:
	*Dst = _T('\0');
	return fRet;

}
#pragma optimize( "", on )


/////////////////////////////////////////////////////////////////////////////
// CTipState
// Handles persistent state for an individual Tip file
//

BOOL CTipState::Read( const CFile & f, BOOL bIsFileOpen )
{
	CFileStatus status;
	if (CFile::GetStatus(f.GetFilePath(), status))
	{
		m_dwCheck = status.m_size;
		if (bIsFileOpen)
			m_dwPos = f.GetPosition();
//		SetStatus(Ok);
		return TRUE;
	}
	else
	{
		SetStatus(Invalid);
		return FALSE;
	}
}

BOOL CTipState::Read( LPCTSTR szName )
{
	ASSERT(szName != 0);
	ASSERT(*szName != 0);
	BOOL bRet = TRUE;
	HGLOBAL hg = NULL;
	hg = GetRegData(szTipStateKey, szName, hg);
	if (hg == NULL)
	{
		SetStatus(Invalid);
		return FALSE;
	}
	LPVOID pData = ::GlobalLock(hg);
	if (pData != NULL && (*((WORD*)pData) == sizeof CTipState))
	{
		memcpy(this, (LPBYTE)pData+sizeof(WORD), sizeof CTipState);
	}
	else
	{
		SetStatus(Invalid);
		bRet = FALSE;
	}
	::GlobalUnlock(hg);
	::GlobalFree(hg);
	return bRet;
}

BOOL CTipState::Write( LPCTSTR szName )
{
	ASSERT(szName != 0);
	ASSERT(*szName != 0);
	HGLOBAL hg = ::GlobalAlloc(GMEM_MOVEABLE, sizeof CTipState + sizeof(WORD));
	if (hg == NULL)
		return FALSE;
	LPVOID pData = ::GlobalLock(hg);
	if (pData == NULL)
		return FALSE;
	*((WORD*)pData) = (WORD)(sizeof CTipState);
	memcpy((LPBYTE)pData+sizeof(WORD), this, sizeof CTipState);
	BOOL bRet = WriteRegData(szTipStateKey, szName, hg);
	::GlobalUnlock(hg);
	::GlobalFree(hg);
	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CTipFile

TipStatus CTipFile::Close()
{
	SaveState();
	if (m_bIsFileOpen)
	{
		m_File.Close();
		m_bIsFileOpen = FALSE;
	}
	return Status();
}

TipStatus CTipFile::Init(LPCTSTR szFile)
{
	ASSERT(szFile != NULL);
	ASSERT(*szFile);

	m_File.SetFilePath(szFile);
	m_State.Read(m_File, m_bIsFileOpen);

	_TCHAR szFName[_MAX_FNAME];
	_tsplitpath(szFile, 0, 0, szFName, 0);
	CTipState tsRegState;
	if (tsRegState.Read(szFName) && CheckState(tsRegState, m_State))
	{
		// The state is in the registry and checks ok against the file, so
		//   we use the saved state.
		m_State = tsRegState;
	}
	else
	{
		// The state was not in registry or doesn't match the file, so
		//  we load the file and save the state
		Next();
		if (Status() == Exhausted)
			// no tips found
			m_State.SetStatus(Invalid);
		SaveState();
	}
	return Status();
}


enum LineType { ltUnknown, ltError, ltBlank, ltComment, ltCommand, ltCategory, ltTip };

#define CHAR_TIP_SEP _T(',')
#define CHAR_CMD_SEP _T('=')
#define CHAR_SPACE   _T(' ')
#define CHAR_COMMENT _T(';')
#define CHAR_COMMAND _T('#')
// commands/categories forced to lowercase for case-insensitivity
#define CMD_PRIORITY _TEXT("priority")
//#define PRI_NORMAL _TEXT("normal")

LineType ParseTipFileLine( CString & sLine, CString & sCmd, CString & sText )
{
	LineType lt = ltUnknown;
	sLine.TrimLeft();
	if (sLine.IsEmpty()) return ltBlank;
	if (sLine[0] == CHAR_COMMENT) return ltComment;
	if (sLine[0] == CHAR_COMMAND)
	{
		sLine = sLine.Mid(1);
		lt = ltCommand;
	}
	int scan;
	for (scan = 0; scan < sLine.GetLength(); scan++)
	{
		switch (sLine[scan])
		{
		case CHAR_CMD_SEP:
			if (lt == ltUnknown) lt = ltCategory;
			goto Parsed;
			break;

		case CHAR_TIP_SEP:
			if (lt == ltUnknown) lt = ltTip;
			goto Parsed;
			break;

		case CHAR_SPACE:
			if (lt != ltCommand) return ltError;
			goto Parsed;
			break;

		default:
			if ((_TUCHAR)sLine[scan] < 32) // control char in all sbcs and dbcs sets
			return ltError;
		}
	}
Parsed:

	if (lt == ltUnknown) return ltError;

	sCmd = sLine.Left(scan);
	sCmd.TrimLeft(); sCmd.TrimRight();
	sCmd.MakeLower();

	scan++;
	if (scan < sLine.GetLength())
	{
 		sText = sLine.Mid(scan);
		sText.TrimLeft(); sText.TrimRight();
		_TCHAR t[MAX_TIP_LINE + sizeof(wchar_t)];
		if(Escape(t,sText,MAX_TIP_LINE))
			sText = t;
		else
			lt = ltBlank;
	}
	return lt;
}

void CTipFile::DoCommand( const CString & sCmd, const CString & sArg )
{
	if (sCmd == CMD_PRIORITY)
	{
		Priority_t pri = DefaultPriority;
		if (!sArg.IsEmpty())
		{
			pri = (Priority_t) _ttoi(sArg);
			if (pri == 0)
			{
				if (!_istdigit(sArg[0]))
					// text arg, just set default priority
					pri = DefaultPriority;
			}
			else
			{
				// restrict to range
				pri = __min(MAX_PRIORITY,pri);
				pri = __max(MIN_PRIORITY,pri);
			}
		}
		m_State.m_nPriority = pri;
	}
	else
		ASSERT(0); // unrecognized command
}

TipStatus CTipFile::Next()
{
	if (Status() == Invalid || Status() == Exhausted)
		return Status();

	try {
		if (!m_bIsFileOpen)
		{
			if (m_File.Open(m_File.GetFilePath(), 
					CFile::shareDenyWrite | CFile::modeNoInherit) == 0)
				return m_State.SetStatus(Invalid);
			else
				m_bIsFileOpen = TRUE;
		}

		CString sCmd;
		_TCHAR szLine[MAX_TIP_LINE];
		LineType lt;
		m_File.Seek(m_State.m_dwPos, CFile::begin);
		while (m_File.ReadString(szLine,MAX_TIP_LINE-1) != NULL)
		{
			lt = ParseTipFileLine(CString(szLine), sCmd, m_sTip);
			switch(lt)
			{
			default:
				ASSERT(0);
			case ltError:
				m_State.SetStatus(Invalid);
				Close();
				m_sTip.LoadString(IDS_SORRY_NO_TIP);
				return Invalid;
				break;

			case ltBlank:
			case ltComment:
			case ltCategory:
				break;

			case ltCommand:
				DoCommand( sCmd, m_sTip );
				break;

			case ltTip:
				return m_State.SetStatus(Ok);
				break;
			}
			// save seek position of line we're about to read
			m_State.m_dwPos = m_File.GetPosition();
		}
	}
	catch( CFileException * pExcept )
	{
		pExcept->Delete();
		Close();
		m_sTip.LoadString(IDS_SORRY_NO_TIP);
		return m_State.SetStatus(Invalid);
	}
	m_sTip.LoadString(IDS_SORRY_NO_TIP);
	return m_State.SetStatus(Exhausted);
}


BOOL CTipFile::SaveState()
{
	_TCHAR szFName[_MAX_FNAME];
	_tsplitpath(m_File.GetFilePath(), 0, 0, szFName, 0);
	return m_State.Write(szFName);
}

BOOL CTipFile::isTipAvailable()
{
	switch (Status())
	{
	case Unknown:
		Next();
		if (Status() == Exhausted)
			Next();
		return Status() == Ok;
		break;

	case Invalid:
		return FALSE;
		break;

	case Exhausted:
	case Ok:
		return TRUE;
		break;

	default:
		ASSERT(0);
		return FALSE;
	}
}

TipStatus CTipFile::Tip( CString & sTip )
{
	if (!m_bIsFileOpen)
		Next();
	if (Status() == Ok)
		m_State.m_dwPos = m_File.GetPosition();
	sTip = m_sTip;
	return Status();
}


/////////////////////////////////////////////////////////////////////////////
//  CTipFileQueue

CTipFileQueue::~CTipFileQueue()
{
	if (m_rgptf)
	{
		// requeue first item
		if (m_bDirty) Add(Get());
		for(; m_nLast >= 0; m_nLast--)
		{
			m_rgptf[m_nLast]->SetOrder(m_nLast);
			delete m_rgptf[m_nLast];
		}
	}
	delete [] m_rgptf;
}

BOOL CTipFileQueue::Create( int nSize )
{
	m_rgptf = new CTipFile *[nSize];
	if (m_rgptf == NULL)
		return FALSE;
	m_nMax = nSize;
	for (int nIndex = 0; nIndex < m_nMax; nIndex++)
		m_rgptf[nIndex] = NULL;
	return TRUE;
}

TipStatus CTipFileQueue::Init(CPtrList & lptf)
{
	// asssume the list contains valid CFile *'s

	if (lptf.IsEmpty()) return Invalid;

	if (m_rgptf == NULL)
		if (!Create(lptf.GetCount()))
		{
			// destroy members of list
			while (!lptf.IsEmpty())
				delete lptf.RemoveHead();
			return Invalid;
		}

	while (!lptf.IsEmpty())
	{
		CTipFile* ptf = (CTipFile *)lptf.RemoveHead();
		ptf->Next(); // kickstart it
		AddByOrder(ptf);
	}
	ASSERT(!IsEmpty());
	return Peek()->Status();
}

CTipFile * CTipFileQueue::Get()
{
	if (IsEmpty()) return 0;

	m_bDirty = TRUE;
	CTipFile * ptf = m_rgptf[0];

	if (m_nLast == 1)
		m_rgptf[0] = m_rgptf[1];
	else if (m_nLast > 0)
		memmove( &m_rgptf[0], &m_rgptf[1], (m_nLast)*(sizeof ptf) );

	m_rgptf[m_nLast--] = 0;
	return ptf;
}

// Add by rank
void CTipFileQueue::Add( CTipFile * ptf )
{
	ASSERT(ptf != NULL);
	ASSERT(m_rgptf != NULL);
	ASSERT(m_nLast < m_nMax); // full

	int scan = m_nLast;
	int nRank = ptf->Rank();
	while (scan >= 0 && m_rgptf[scan]->Rank() < nRank)
		scan--;
	// adjust to insert before or after matched item
	if (scan < 0 || m_rgptf[scan]->Rank() >= nRank)
		scan++;
	memmove(&m_rgptf[scan+1], &m_rgptf[scan], (m_nLast-scan + 1)*(sizeof ptf) );
	m_rgptf[scan] = ptf;
	m_nLast++;
}

// Add by order
void CTipFileQueue::AddByOrder( CTipFile * ptf )
{
	ASSERT(ptf != NULL);
	ASSERT(m_rgptf != NULL);
	ASSERT(m_nLast < m_nMax); // full

	int scan = m_nLast;
	int nOrder = ptf->RankOrder();
	while (scan >= 0 && m_rgptf[scan]->RankOrder() < nOrder)
		scan--;
	// adjust to insert before or after matched item
	if (scan < 0 || m_rgptf[scan]->RankOrder() >= nOrder)
		scan++;
	memmove(&m_rgptf[scan+1], &m_rgptf[scan], (m_nLast-scan + 1)*(sizeof ptf) );
	m_rgptf[scan] = ptf;
	m_nLast++;
}

CTipFile * CTipFileQueue::operator[](int nIndex)
{
	if (m_rgptf == NULL || nIndex < 0 || nIndex > m_nLast)
		return 0;
	return m_rgptf[nIndex];
}

/////////////////////////////////////////////////////////////////////////////
// CTipFileManager

TipStatus CTipFileManager::Init()
{
	LPTSTR pchPoke;
	_TCHAR szTipPattern[_MAX_PATH];
	_TCHAR szTipPath[_MAX_PATH];
	// Get the directory of the IDE executable
	{
		_TCHAR szPath[_MAX_PATH];
		_TCHAR szDir[_MAX_DIR];
		GetModuleFileName(NULL, szPath, _MAX_PATH);
		_tsplitpath(szPath, szTipPattern, szDir, NULL, NULL);
		_tcscat(szTipPattern, szDir);
	}
	// Append default extension dir (if any)
	LPCTSTR szExtension = theApp.GetExeString(DefaultExtensionDir);
	if (szExtension != NULL)
	{
		_tcscat( szTipPattern, szExtension );
		pchPoke = _tcschr(szTipPattern, _T('\0'));
		*pchPoke++ = _T('\\');
		*pchPoke   = _T('\0');
	}
	_tcscpy(szTipPath,szTipPattern);
	// find end of path to poke filename into
	pchPoke = _tcschr(szTipPath, _T('\0'));

	// make Tip file find pattern
	_tcscat( szTipPattern, "*.TIP" );

	// Read list of tip files
	CPtrList lptf;
	int nFiles = 0;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile( szTipPattern, &wfd );
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				_TCHAR szFName[_MAX_FNAME];
				_TCHAR szExt[_MAX_EXT];
				_tsplitpath(wfd.cFileName, NULL, NULL, szFName, szExt);
				_tcscpy(pchPoke,szFName);
				_tcscat(pchPoke,szExt);
				CTipFile* ptf = new CTipFile(szTipPath);
				if (ptf == NULL)
					break;
				lptf.AddTail(ptf);
				nFiles++;
			}
		} while (/*nFiles < MAX_TIP_FILES &&*/ FindNextFile( hFind, &wfd ));
		FindClose(hFind);
	}
	return m_Status = m_qptf.Init(lptf);
}

CTipFileManager::~CTipFileManager()
{
}

BOOL CTipFileManager::isTipAvailable()
{
	if (m_Status == Unknown)
		Init();
	if (m_Status == Invalid || m_qptf.IsEmpty())
		return FALSE;
	else
		// Only need to peek at first in the queue
		//  because elements with available tips have 
		//  priority over those without.
		return m_qptf.Peek()->isTipAvailable();
}

BOOL CTipFileManager::SaveState()
{
	if (m_qptf.IsEmpty()) return FALSE;

	int scan;
	CTipFile *ptf;
	for (scan = 0; (ptf = m_qptf[scan]) != NULL; scan++)
		ptf->SaveState();

	return TRUE;
}

void CTipFileManager::Reset()
{	
	if (m_qptf.IsEmpty()) return;
	int nSize = m_qptf.Size();
	CTipFile **rgptf = new CTipFile *[nSize];
	if (rgptf == NULL)
	{
		m_Status = Invalid;
		return;
	}
	int pos;
	for (pos = 0; pos < nSize; pos++)
	{
		rgptf[pos] = m_qptf.Get();
		ASSERT(rgptf[pos]!=NULL);
		rgptf[pos]->Reset();
	}
	ASSERT(m_qptf.IsEmpty());
	for (pos = 0; pos < nSize; pos++)
	{
		m_qptf.Add(rgptf[pos]);
	}
	delete [] rgptf;
	m_Status = m_qptf.Peek()->Status();
}

TipStatus CTipFileManager::Next()
{
	if (m_Status != Ok) return m_Status;

	CTipFile * ptf = m_qptf.Get();
	if (ptf == NULL)
		return m_Status = Invalid;
	else
	{
		ptf->Next();
		m_qptf.Add(ptf);
		return m_Status = m_qptf.Peek()->Status();
	}
}

TipStatus CTipFileManager::Tip( CString & sTip )
{
	if (m_Status != Ok || m_qptf.IsEmpty())
	{
		sTip.LoadString( IDS_SORRY_NO_TIP );
		return m_Status;
	}
	else
		return m_Status = m_qptf.Peek(1)->Tip(sTip);
}
