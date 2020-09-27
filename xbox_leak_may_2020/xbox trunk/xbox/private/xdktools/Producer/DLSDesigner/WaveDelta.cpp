/////////////////////////////////////////////
//
// CWaveDelta Implementation
//
//////////////////////////////////////////////

#include "stdafx.h"
#include "WaveDelta.h"

//////////////////////////////////////////////
//
// CWaveDelta::CWaveDelta
//
//////////////////////////////////////////////

CWaveDelta::CWaveDelta():
m_dwActualPosition(0), 
m_dwStartSample(0),
m_dwLength(0), 
m_dwFileOffset(0), 
m_bEditDelta(false)
{

}


CWaveDelta::CWaveDelta(DWORD dwActualPosition, DWORD dwStartSample, DWORD dwLength,  DWORD dwFileOffset, BOOL bEditDelta) :
m_dwActualPosition(dwActualPosition), 
m_dwStartSample(dwStartSample),
m_dwLength(dwLength), 
m_dwFileOffset(dwFileOffset), 
m_bEditDelta(bEditDelta)
{

} 

BOOL CWaveDelta::IsSampleInDelta(DWORD dwSample)
{
	if(dwSample >= m_dwActualPosition && dwSample < m_dwActualPosition + m_dwLength)
	{
		return TRUE;
	}

	return FALSE;
}


void CWaveDelta::SetActualPosition(DWORD dwPosition)
{
	m_dwActualPosition = dwPosition;
}


DWORD CWaveDelta::GetLength()
{
	return m_dwLength;
}

void CWaveDelta::SetLength(DWORD dwLength)
{
	m_dwLength = dwLength;
}

void CWaveDelta::SetFileOffset(DWORD dwFileOffset)
{
	m_dwFileOffset = dwFileOffset;
}

DWORD CWaveDelta::GetActualPosition()
{
	return m_dwActualPosition;
}


DWORD CWaveDelta::GetFileOffset()
{
	return m_dwFileOffset;
}


DWORD CWaveDelta::GetStartSample()
{
	return m_dwStartSample;
}


void CWaveDelta::SetStartSample(DWORD dwSample)
{
	m_dwStartSample = dwSample; 
}

BOOL CWaveDelta::IsEditDelta()
{
	return m_bEditDelta;
}
