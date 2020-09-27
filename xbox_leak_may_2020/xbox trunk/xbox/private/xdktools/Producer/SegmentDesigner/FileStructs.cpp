//  FileStructs.cpp

#include "stdafx.h"
#include "FileStructs.h"


CCommandStruct::CCommandStruct() {
}
CCommandStruct::~CCommandStruct() {
}

int CCommandStruct::StructToString(char *szBuffer, int iSize) {
	int iPosition;

	if(iSize < GetSize())
	{
		return -1;
	}
	memset(szBuffer, 0, 4);
	iPosition = 4;
	memcpy(szBuffer + iPosition, &m_lTime, sizeof(long));
	iPosition += sizeof(long);
	memcpy(szBuffer + iPosition, &m_nMeasure, sizeof(short));
	iPosition += sizeof(short);
	memcpy(szBuffer + iPosition, &m_dwCommand, sizeof(DWORD));
	iPosition += sizeof(DWORD);
	memcpy(szBuffer + iPosition, &m_dwSignPost, sizeof(DWORD));
	return (iPosition + sizeof(DWORD));
}

HRESULT CCommandStruct::StringToStruct(char *szBuffer, ULONG ulSize) {
	ULONG ulBytesLeft;
	ULONG ulPosition = 4;

	if(ulSize >= (ULONG) GetSize()) {
		memcpy(&m_lTime, szBuffer + ulPosition, sizeof(long));
		ulPosition += sizeof(long);
		memcpy(&m_nMeasure, szBuffer + ulPosition, sizeof(short));
		ulPosition += sizeof(short);
		memcpy(&m_dwCommand, szBuffer + ulPosition, sizeof(DWORD));
		ulPosition += sizeof(DWORD);
		memcpy(&m_dwSignPost, szBuffer + ulPosition, sizeof(DWORD));
		return S_OK;
	}
	ulBytesLeft = ulSize;
	if(ulBytesLeft > sizeof(long)) {
		memcpy(&m_lTime, szBuffer + ulPosition, sizeof(long));
		ulBytesLeft -= sizeof(long);
		ulPosition  += sizeof(long);
	} else {
		return E_FAIL;
	}
	if(ulBytesLeft > sizeof(short)) {
		memcpy(&m_nMeasure, szBuffer + ulPosition, sizeof(short));
		ulBytesLeft -= sizeof(short);
		ulPosition  += sizeof(short);
	} else {
		return E_FAIL;
	}
	if(ulBytesLeft > sizeof(DWORD)) {
		memcpy(&m_dwCommand, szBuffer + ulPosition, sizeof(DWORD));
		ulBytesLeft -= sizeof(DWORD);
		ulPosition  += sizeof(DWORD);
	} else {
		return E_FAIL;
	}
	if(ulBytesLeft > sizeof(DWORD)) {
		memcpy(&m_dwSignPost, szBuffer + ulPosition, sizeof(DWORD));
		ulBytesLeft -= sizeof(DWORD);
		ulPosition  += sizeof(DWORD);
	} else {
		return E_FAIL;
	}
	return S_OK;
}

int CCommandStruct::GetSize() {
	return (sizeof(long) + sizeof(short) + 2 * sizeof(DWORD) + 4);
}

CommandList::CommandList() {
}
CommandList::~CommandList() {
	RemoveAll();
}

void CommandList::RemoveAll() {
	while(!(m_list.IsEmpty())) {
		CCommandStruct *pCommand;

		pCommand = m_list.RemoveHead();
		ASSERT(pCommand != NULL);
		pCommand->m_nMeasure = 2;
		free( pCommand);
	}
}

void CommandList::RemoveSignPosts() {
	POSITION position, positionOld;
	CCommandStruct *pCommand;

	position = m_list.GetHeadPosition();
	while(position != NULL)
	{
		positionOld = position;
		pCommand = m_list.GetNext(position);
		if(pCommand->m_dwCommand == 0)
		{
			m_list.RemoveAt(positionOld);
			delete pCommand;
		}
		pCommand->m_dwSignPost = 0;
		positionOld = position;
	}
}

void CommandList::RemoveCommands() {
	POSITION position, positionOld;
	CCommandStruct *pCommand;

	position = m_list.GetHeadPosition();
	while(position != NULL)
	{
		positionOld = position;
		pCommand = m_list.GetNext(position);
		if(pCommand->m_dwSignPost == 0)
		{
			m_list.RemoveAt(positionOld);
			delete pCommand;
		}
		pCommand->m_dwCommand = 0;
		positionOld = position;
	}
}

CTemplateStruct::CTemplateStruct()

{
    strcpy(m_szName,"Segment");
    strcpy(m_szType,"Verse");
    m_nMeasures			= 16;
	m_wActivityLevel	= 1;
	m_wKey				= KEY_2C;
	m_bLoop				= FALSE;
}

CTemplateStruct::~CTemplateStruct()
{
}

int CTemplateStruct::StructToString(char *szBuffer, int iSize) {
	int iPosition = 0;

	if(iSize < GetSize())
	{
		return -1;
	}
	memset(szBuffer, 0, 4);
	iPosition += 4;
	memcpy(szBuffer + iPosition, m_szName, 20);
	iPosition += 20;
	memcpy(szBuffer + iPosition, m_szType, 20);
	iPosition += 20;
	memcpy(szBuffer + iPosition, &m_nMeasures, sizeof(short));
	iPosition += sizeof(short);
	memset(szBuffer + iPosition, 0, 4);
	iPosition += 4;
	memcpy(szBuffer + iPosition, &m_wActivityLevel, sizeof(WORD));
	iPosition += sizeof(WORD);
	memcpy(szBuffer + iPosition, &m_wKey, sizeof(WORD));
	iPosition += sizeof(WORD);
	memcpy(szBuffer + iPosition, &m_bLoop, sizeof(BOOL));
	iPosition += sizeof(BOOL);
	return iPosition;
}

HRESULT CTemplateStruct::StringToStruct(char *szBuffer, ULONG ulSize) {
	ULONG ulBytesLeft;
	ULONG ulPosition = 4;

	if(ulSize >= (ULONG) GetSize()) {
		memcpy(m_szName, szBuffer + ulPosition, 20);
		ulPosition += 20;
		memcpy(m_szType, szBuffer + ulPosition, 20);
		ulPosition += 20;
		memcpy(&m_nMeasures, szBuffer + ulPosition, sizeof(short));
		ulPosition += sizeof(short) + 4;
		memcpy(&m_wActivityLevel, szBuffer + ulPosition, sizeof(WORD));
		ulPosition += sizeof(WORD);
		memcpy(&m_wKey, szBuffer + ulPosition, sizeof(WORD));
		ulPosition += sizeof(WORD);
		memcpy(&m_bLoop, szBuffer + ulPosition, sizeof(BOOL));
		ulPosition += sizeof(BOOL);
		return S_OK;
	}
	ulBytesLeft = ulSize - 4;
	if(ulBytesLeft >= 20) {
		memcpy(m_szName, szBuffer + ulPosition, 20);
		ulBytesLeft -= 20;
		ulPosition  += 20;
	} else {
		return S_FALSE;
	}
	if(ulBytesLeft >= 20) {
		memcpy(m_szType, szBuffer + ulPosition, 20);
		ulBytesLeft -= 20;
		ulPosition  += 20;
	} else {
		return S_FALSE;
	}
	if(ulBytesLeft >= sizeof(short)) {
		memcpy(&m_nMeasures, szBuffer + ulPosition, sizeof(short));
		ulBytesLeft -= sizeof(short);
		ulPosition  += sizeof(short);
	} else {
		return S_FALSE;
	}
	if(ulBytesLeft >= 4)
	{
		ulBytesLeft -= 4;
		ulPosition	+= 4;
	} else {
		return S_FALSE;
	}
	if(ulBytesLeft >= sizeof(WORD))
	{
		memcpy(&m_wActivityLevel, szBuffer + ulPosition, sizeof(WORD));
		ulBytesLeft -= sizeof(WORD);
		ulPosition	+= sizeof(WORD);
	} else {
		return S_OK;	// Don't return S_FALSE, because we may have an old format segment.
	}
	if(ulBytesLeft >= sizeof(WORD))
	{
		memcpy(&m_wKey, szBuffer + ulPosition, sizeof(WORD));
		ulBytesLeft -= sizeof(WORD);
		ulPosition	+= sizeof(WORD);
	} else {
		return S_OK;	// Don't return S_FALSE, because we may have an old format segment.
	}
	if(ulBytesLeft >= sizeof(BOOL))
	{
		memcpy(&m_bLoop, szBuffer + ulPosition, sizeof(BOOL));
		ulBytesLeft -= sizeof(BOOL);
		ulPosition	+= sizeof(BOOL);
	} else {
		return S_OK;	// Don't return S_FALSE, because we may have an old format segment.
	}
	return S_OK;
}

int CTemplateStruct::GetSize() {
	return (48 + sizeof(short) + 2 * sizeof(WORD) + sizeof(BOOL));
}

CString KeyToStr(WORD wKey)
{
	switch(wKey)
	{
	case KEY_1C:
		return CString(KEYSTRING_1C);
	case KEY_1CS:
		return CString(KEYSTRING_1CS);
	case KEY_1Db:
		return CString(KEYSTRING_1Db);
	case KEY_1D:
		return CString(KEYSTRING_1D);
	case KEY_1DS:
		return CString(KEYSTRING_1DS);
	case KEY_1Eb:
		return CString(KEYSTRING_1Eb);
	case KEY_1E:
		return CString(KEYSTRING_1E);
	case KEY_1F:
		return CString(KEYSTRING_1F);
	case KEY_1FS:
		return CString(KEYSTRING_1FS);
	case KEY_1Gb:
		return CString(KEYSTRING_1Gb);
	case KEY_1G:
		return CString(KEYSTRING_1G);
	case KEY_1GS:
		return CString(KEYSTRING_1GS);
	case KEY_1Ab:
		return CString(KEYSTRING_1Ab);
	case KEY_1A:
		return CString(KEYSTRING_1A);
	case KEY_1AS:
		return CString(KEYSTRING_1AS);
	case KEY_1Bb:
		return CString(KEYSTRING_1Bb);
	case KEY_1B:
		return CString(KEYSTRING_1B);
	case KEY_2C:
		return CString(KEYSTRING_2C);
	case KEY_2CS:
		return CString(KEYSTRING_2CS);
	case KEY_2Db:
		return CString(KEYSTRING_2Db);
	case KEY_2D:
		return CString(KEYSTRING_2D);
	case KEY_2DS:
		return CString(KEYSTRING_2DS);
	case KEY_2Eb:
		return CString(KEYSTRING_2Eb);
	case KEY_2E:
		return CString(KEYSTRING_2E);
	case KEY_2F:
		return CString(KEYSTRING_2F);
	case KEY_2FS:
		return CString(KEYSTRING_2FS);
	case KEY_2Gb:
		return CString(KEYSTRING_2Gb);
	case KEY_2G:
		return CString(KEYSTRING_2G);
	case KEY_2GS:
		return CString(KEYSTRING_2GS);
	case KEY_2Ab:
		return CString(KEYSTRING_2Ab);
	case KEY_2A:
		return CString(KEYSTRING_2A);
	case KEY_2AS:
		return CString(KEYSTRING_2AS);
	case KEY_2Bb:
		return CString(KEYSTRING_2Bb);
	case KEY_2B:
		return CString(KEYSTRING_2B);
	default:
		return CString("");
	}
}

WORD StrToKey(CString str)
{
	if(str == KEYSTRING_1C) {
		return (KEY_1C);
	} else if(str == KEYSTRING_1CS) {
		return (KEY_1CS);
	} else if(str == KEYSTRING_1Db) {
		return (KEY_1Db);
	} else if(str == KEYSTRING_1D) {
		return (KEY_1D);
	} else if(str == KEYSTRING_1DS) {
		return (KEY_1DS);
	} else if(str == KEYSTRING_1Eb) {
		return (KEY_1Eb);
	} else if(str == KEYSTRING_1E) {
		return (KEY_1E);
	} else if(str == KEYSTRING_1F) {
		return (KEY_1F);
	} else if(str == KEYSTRING_1FS) {
		return (KEY_1FS);
	} else if(str == KEYSTRING_1Gb) {
		return (KEY_1Gb);
	} else if(str == KEYSTRING_1G) {
		return (KEY_1G);
	} else if(str == KEYSTRING_1GS) {
		return (KEY_1GS);
	} else if(str == KEYSTRING_1Ab) {
		return (KEY_1Ab);
	} else if(str == KEYSTRING_1A) {
		return (KEY_1A);
	} else if(str == KEYSTRING_1AS) {
		return (KEY_1AS);
	} else if(str == KEYSTRING_1Bb) {
		return (KEY_1Bb);
	} else if(str == KEYSTRING_1B) {
		return (KEY_1B);
	} else if(str == KEYSTRING_2C) {
		return (KEY_2C);
	} else if(str == KEYSTRING_2CS) {
		return (KEY_2CS);
	} else if(str == KEYSTRING_2Db) {
		return (KEY_2Db);
	} else if(str == KEYSTRING_2D) {
		return (KEY_2D);
	} else if(str == KEYSTRING_2DS) {
		return (KEY_2DS);
	} else if(str == KEYSTRING_2Eb) {
		return (KEY_2Eb);
	} else if(str == KEYSTRING_2E) {
		return (KEY_2E);
	} else if(str == KEYSTRING_2F) {
		return (KEY_2F);
	} else if(str == KEYSTRING_2FS) {
		return (KEY_2FS);
	} else if(str == KEYSTRING_2Gb) {
		return (KEY_2Gb);
	} else if(str == KEYSTRING_2G) {
		return (KEY_2G);
	} else if(str == KEYSTRING_2GS) {
		return (KEY_2GS);
	} else if(str == KEYSTRING_2Ab) {
		return (KEY_2Ab);
	} else if(str == KEYSTRING_2A) {
		return (KEY_2A);
	} else if(str == KEYSTRING_2AS) {
		return (KEY_2AS);
	} else if(str == KEYSTRING_2Bb) {
		return (KEY_2Bb);
	} else if(str == KEYSTRING_2B) {
		return (KEY_2B);
	} else {
		return 0xFFFF;
	}
}

BOOL IsBelowFlat(WORD wKey)
{
	if(wKey == KEY_2AS ||
	   wKey == KEY_2GS ||
	   wKey == KEY_2FS ||
	   wKey == KEY_2DS ||
	   wKey == KEY_2CS ||
	   wKey == KEY_1AS ||
	   wKey == KEY_1GS ||
	   wKey == KEY_1FS ||
	   wKey == KEY_1DS ||
	   wKey == KEY_1CS)
	{
		return TRUE;
	}
	return FALSE;
}