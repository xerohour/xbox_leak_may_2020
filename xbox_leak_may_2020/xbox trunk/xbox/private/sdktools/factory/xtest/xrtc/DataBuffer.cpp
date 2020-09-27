#include "..\stdafx.h"
#include "..\testobj.h"
//#include "WinRTC.h"
#include "DataBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataBuffer::CDataBuffer(ULONG uSize):m_uSize(uSize),m_uBase(0)
{
	//
	// Create a buffer of twice the required size so we can
	// do the Rotate Up/Down manipulations
	//
	m_pData = new UCHAR[2 * uSize];
}

CDataBuffer::~CDataBuffer()
{
	delete m_pData;
}

void CDataBuffer::FillLinear()
{
	for(ULONG i = 0; i < m_uSize; i++)
		m_pData[m_uSize + i] = m_pData[i] = (UCHAR)i;
}

void CDataBuffer::FillRandom(UINT *uiRandScratch)
{
	for(ULONG i = 0; i < m_uSize; i++)
		m_pData[m_uSize + i] = m_pData[i] = (UCHAR)random(uiRandScratch);
}

void CDataBuffer::RotateUp()
{
	if(++m_uBase == m_uSize)
		m_uBase = 0;
}

void CDataBuffer::RotateDown()
{
	if(m_uBase == 0)
		m_uBase = m_uSize;
	m_uBase--;
}

DWORD CDataBuffer::random(UINT *pScratch)
{
	DWORD ret;
	ret  = (*pScratch = *pScratch * 214013L + 2531011L) >> 16;
	ret |= (*pScratch = *pScratch * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}
