/////////////////////////////////////////////////////////
//
// CWaveFileHandler Implementation
//
/////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveFileHandler.h"


/////////////////////////////////////////////////////////
//
//	Construction :- CWaveFileHandler::CWaveFileHandler();
//
/////////////////////////////////////////////////////////
CWaveFileHandler::CWaveFileHandler(CString sFileName, UINT nOpenFlags) : CFile(sFileName, nOpenFlags | CFile::shareDenyNone | CFile::typeBinary)
{
	m_sFileName = sFileName;
}


/////////////////////////////////////////////////////////
//
// CWaveFileHandler::GetStartOffset
//
/////////////////////////////////////////////////////////
DWORD CWaveFileHandler::GetStartOffset()
{
	return m_dwStartOffset;
}

/////////////////////////////////////////////////////////
//
// CWaveFileHandler::SetStartOffset
//
/////////////////////////////////////////////////////////
DWORD CWaveFileHandler::GetDataOffset()
{
	return m_dwDataOffset;
}


/////////////////////////////////////////////////////////
//
// CWaveFileHandler::SetStartOffset
//
/////////////////////////////////////////////////////////
void CWaveFileHandler::SetStartOffset(DWORD dwOffset)
{
	m_dwStartOffset = dwOffset;
}

/////////////////////////////////////////////////////////
//
// CWaveFileHandler::SetDataOffset
//
/////////////////////////////////////////////////////////
void CWaveFileHandler::SetDataOffset(DWORD dwOffset)
{
	m_dwDataOffset = dwOffset;
}


/////////////////////////////////////////////////////////
//
// CWaveFileHandler::SeekToBegin()
//
/////////////////////////////////////////////////////////
void CWaveFileHandler::SeekToBegin()
{
	CFile::Seek(m_dwStartOffset, CFile::begin);
}

/////////////////////////////////////////////////////////
//
// CWaveFileHandler::SeekToData()
//
/////////////////////////////////////////////////////////
void CWaveFileHandler::SeekToData()
{
	CFile::Seek(m_dwDataOffset, CFile::begin);
}

//////////////////////////////////////////////////////
//
// CWaveFileHandler::ReadData
//
//////////////////////////////////////////////////////
HRESULT CWaveFileHandler::ReadData(DWORD dwOffset, DWORD dwLength, BYTE* pbData, DWORD& dwBytesRead)
{
	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_POINTER;
	}

	try
	{
		CFile::Seek(dwOffset, CFile::begin);
		dwBytesRead = Read((void*)pbData, dwLength);
		if(dwBytesRead == 0)
		{
			return E_FAIL;
		}
	}
	catch(CFileException e)
	{
		dwBytesRead = 0;
		return E_FAIL;
	}

	return S_OK;
}
