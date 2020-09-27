/////////////////////////////////////////////////////////
//
// WaveSourceFileHandler Implementation
//
/////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveSourceFileHandler.h"
#include "Wave.h"
#include "stream.h"
#include "RiffStrm.h"


////////////////////////////////////////////////////////////////////////////////////
//
//	Construction :- CWaveSourceFileHandler::CWaveSourceFileHandler
//
/////////////////////////////////////////////////////////////////////////////////////
CWaveSourceFileHandler::CWaveSourceFileHandler(CString sFileName, bool bInACollection) : CWaveFileHandler(sFileName, CFile::modeRead)
{
	m_bInACollection = bInACollection;
}


//////////////////////////////////////
//
//	CWaveSourceFileHandler::LoadChunk
//
//////////////////////////////////////
HRESULT CWaveSourceFileHandler::LoadChunk(MMCKINFO* pckFind,  BYTE* pbData, DWORD& dwBytesRead)
{
	ASSERT(pckFind);
	if(pckFind == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	// Get a stream for the file
	IStream* pIStream = NULL;
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if(SUCCEEDED(hr = AllocRIFFStreamFromFile(&pIStream, &pIRiffStream)))
	{
		// Set the stream pointer to the start of header 
		DWORD dwHeaderOffset = GetStartOffset();
		StreamSeek(pIStream, dwHeaderOffset, STREAM_SEEK_SET);
		
		// Find the WAVE chunk
		MMCKINFO ckMain;
		UINT nFlags = MMIO_FINDRIFF;
		ckMain.ckid = 0;
		ckMain.fccType = mmioFOURCC('W','A','V','E');
			
		UINT nSuccess =  pIRiffStream->Descend(&ckMain, NULL, nFlags);
		if(nSuccess != 0)
		{
			nFlags = MMIO_FINDLIST;
			ckMain.ckid = FOURCC_LIST;
			ckMain.fccType = FOURCC_wave;
			StreamSeek(pIStream, dwHeaderOffset, STREAM_SEEK_SET);
			nSuccess =  pIRiffStream->Descend(&ckMain, NULL, nFlags);
		}

		if(nSuccess == 0)
		{
			// Look for the required chunk
			if(SUCCEEDED(hr = pIRiffStream->Descend(pckFind, &ckMain, MMIO_FINDCHUNK)))
			{
				pbData = new BYTE[pckFind->cksize];
				if(pbData == NULL)
				{
					pIRiffStream->Release();
					pIStream->Release();
					return E_OUTOFMEMORY;
				}

				hr = pIStream->Read(pbData, pckFind->cksize, &dwBytesRead);
				if(FAILED(hr) || pckFind->cksize != dwBytesRead)
				{
					dwBytesRead = 0;
					delete[] pbData;
					pbData = NULL;
				}
			}
		}


		pIRiffStream->Release();
		pIStream->Release();
	}

	return hr;
}


//////////////////////////////////////////////////////
//
// CWaveSourceFileHandler::ResetOffsets
//
//////////////////////////////////////////////////////
HRESULT CWaveSourceFileHandler::ResetOffsets()
{
	HRESULT hr = E_FAIL;

	// Get a stream for the file
	IStream* pIStream = NULL;
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if(SUCCEEDED(hr = AllocRIFFStreamFromFile(&pIStream, &pIRiffStream)))
	{
		// Set the stream pointer to the start of header 
		DWORD dwHeaderOffset = GetStartOffset();
		StreamSeek(pIStream, dwHeaderOffset, STREAM_SEEK_SET);
		
		// Find the WAVE chunk
		MMCKINFO ckMain;
		UINT nFlags = MMIO_FINDRIFF;
		ckMain.fccType = mmioFOURCC('W','A','V','E');

		UINT nSuccess = pIRiffStream->Descend(&ckMain, NULL, nFlags);
		if(nSuccess != 0)
		{
			nFlags = MMIO_FINDLIST;
			ckMain.ckid = FOURCC_LIST;
			ckMain.fccType = FOURCC_wave;
			StreamSeek(pIStream, dwHeaderOffset, STREAM_SEEK_SET);
			nSuccess = pIRiffStream->Descend(&ckMain, NULL, nFlags);
		}
		
		// We have some corrupted DLS collections
		// MANBUGS: 42247
		if(nSuccess != 0)
		{
			nFlags = MMIO_FINDLIST;
			ckMain.ckid = FOURCC_LIST;
			ckMain.fccType = mmioFOURCC('W','A','V','E');
			StreamSeek(pIStream, dwHeaderOffset, STREAM_SEEK_SET);
			nSuccess = pIRiffStream->Descend(&ckMain, NULL, nFlags);
		}

		if(nSuccess == 0)
		{
			DWORD dwHeaderOffset = StreamTell(pIStream);
			
			// Subtract the RIFF header bytes and set the offset
			dwHeaderOffset -= 12; 
			SetStartOffset(dwHeaderOffset);

			// Look for the required chunk
			MMCKINFO ck;
			ck.ckid = FOURCC_DATA;
			//ck.fccType = 0;
			if(pIRiffStream->Descend(&ck, &ckMain, MMIO_FINDCHUNK) == 0)
			{
				DWORD dwDataOffset = StreamTell(pIStream);
				SetDataOffset(dwDataOffset);

				hr = S_OK;
			}
		}
		else
		{
			hr = E_FAIL;
		}

		pIRiffStream->Release();
		pIStream->Release();
	}

	return hr;
}


//////////////////////////////////////////////////////
//
// CWaveSourceFileHandler::AllocRIFFStreamFromFile
//
//////////////////////////////////////////////////////
HRESULT CWaveSourceFileHandler::AllocRIFFStreamFromFile(IStream** ppIStream, IDMUSProdRIFFStream** ppIRiffStream)
{
	HRESULT hr = E_FAIL;
	if(SUCCEEDED(hr = AllocStreamFromFile(ppIStream, m_sFileName, 0)))
	{
		hr = AllocRIFFStream(*ppIStream, ppIRiffStream);
	}

	return hr;
}

