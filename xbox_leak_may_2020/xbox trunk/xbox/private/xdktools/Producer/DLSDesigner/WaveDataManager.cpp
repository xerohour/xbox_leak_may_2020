//////////////////////////////////////////////////////
//
// CWaveDataManager Implementation 
//
//////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveDataManager.h"
#include "WaveUndoManager.h"
#include "Wave.h"
#include "WaveNode.h"
#include "StereoWave.h"
#include "MonoWave.H"
#include "DLSDesignerDLL.h"
#include "riffstrm.h"
#include "WaveFileHandler.h"
#include "WaveTempFileHandler.h"
#include "WaveSourceFileHandler.h"
#include "WaveDelta.h"

/////////////////////////////////////////////////////
//
// CWaveDataManager::CWaveDataManager
//
/////////////////////////////////////////////////////
CWaveDataManager::CWaveDataManager(CWave* pWave, CString sSourceFileName, CString sTempFilePath) : 
m_pWave(pWave), 
m_sSourceFileName(sSourceFileName), 
m_sTempFilePath(sTempFilePath),
m_pTempFileHandler(NULL), 
m_pSourceFileHandler(NULL), 
m_pCopyFileHandler(NULL), 
m_pDecompFileHandler(NULL),
m_pUndoManager(NULL),
m_plstDeltas(NULL),
m_bInInit(FALSE),
m_bDecompressedDesignTime(FALSE),
m_bDecompressedRunTime(FALSE),
m_bNeedToMakeSrcDelta(FALSE),
m_plstUncompressedDeltas(NULL)
{
	ASSERT(pWave);

	CWaveNode* pWaveNode = m_pWave->GetNode(); 
	bool bInACollection = pWaveNode->IsInACollection();

	// Create the source file handler
	m_pSourceFileHandler = new CWaveSourceFileHandler(sSourceFileName, bInACollection);
	ASSERT(m_pSourceFileHandler);

	// Create a temp file name and keep it
	// The actual temp file will be created at the first edit time 
	char szTempFileName[MAX_PATH];

	CString sPrefix = "DMP";
	CString sExtension = "tmp";
	if(FAILED(CreateUniqueTempFile(sPrefix, sExtension, m_sTempFileName)))
	{
		GetTempFileName(m_sTempFilePath, sPrefix, 0, szTempFileName);
		m_sTempFileName = szTempFileName;
	}

	// Create a temp file name to keep the decompressed data 
	// The actual decomp file will be created at the first compression time 
	char szDecompFileName[MAX_PATH];
	if(FAILED(CreateUniqueTempFile(sPrefix, sExtension, m_sDecompFileName)))
	{
		GetTempFileName(m_sTempFilePath, sPrefix, 0, szDecompFileName);
		m_sDecompFileName = szDecompFileName;
	}

	// Create the undo manager
	m_pUndoManager = new CWaveUndoManager(this);

	// Create the delta list
	m_plstDeltas = new CPtrList();
}

// This creates only the decop file handler...used for individual channels of a stereo wave
CWaveDataManager::CWaveDataManager(CWave* pWave, CString sTempFilePath) : 
m_pWave(pWave), 
m_sTempFilePath(sTempFilePath),
m_pTempFileHandler(NULL), 
m_pSourceFileHandler(NULL), 
m_pCopyFileHandler(NULL), 
m_pDecompFileHandler(NULL),
m_plstDeltas(NULL),
m_bInInit(FALSE)
{
	ASSERT(pWave);

	CWaveNode* pWaveNode = m_pWave->GetNode(); 
	bool bInACollection = pWaveNode->IsInACollection();

	// Create a temp file name to keep the decompressed data 
	// The actual decomp file will be created at the first compression time 
	char szDecompFileName[MAX_PATH];
	
	CString sPrefix = "DMP";
	CString sExtension = "tmp";
	if(FAILED(CreateUniqueTempFile(sPrefix, sExtension, m_sDecompFileName)))
	{
		GetTempFileName(m_sTempFilePath, sPrefix, 0, szDecompFileName);
		m_sDecompFileName = szDecompFileName;
	}

	// Create the undo manager
	m_pUndoManager = new CWaveUndoManager(this);
}




CWaveDataManager::~CWaveDataManager()
{
	if(m_pSourceFileHandler)
	{
		delete m_pSourceFileHandler;
		m_pSourceFileHandler = NULL;
	}

	if(m_pTempFileHandler)
	{
		delete m_pTempFileHandler;
		m_pTempFileHandler = NULL;
	}

	// Delete the created temp file
	DeleteFile(m_sTempFileName);

	if(m_pCopyFileHandler)
	{
		delete m_pCopyFileHandler;
		m_pCopyFileHandler = NULL;

		// Delete the copied source file
		DeleteFile(m_sCopyFileName);
	}

	if(m_pDecompFileHandler)
	{
		delete m_pDecompFileHandler;
		m_pDecompFileHandler = NULL;
	}

	// Delete the decomp data file
	DeleteFile(m_sDecompFileName);

	if(m_pUndoManager)
	{
		delete m_pUndoManager;
	}

	// Delete the delta list that's with us
	if(m_plstDeltas)
	{
		while(!m_plstDeltas->IsEmpty())
		{
			delete m_plstDeltas->RemoveHead();
		}

		delete m_plstDeltas;
	}

    if(m_plstUncompressedDeltas)
    {
        while(m_plstUncompressedDeltas->IsEmpty() == FALSE)
        {
            delete (CWaveDelta*)m_plstUncompressedDeltas->RemoveHead();
        }

        delete m_plstUncompressedDeltas;
        m_plstUncompressedDeltas = NULL;
    }

}


///////////////////////////////////////////////////
//
// CWaveDataManager::Initialize
//
// NOTE!! This method assumes that header has been read AND 
// the pIStream is positioned just after (12 bytes) the 
// WAVE chunk header
//
///////////////////////////////////////////////////
HRESULT CWaveDataManager::Initialize(IStream* pIStream)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	if(m_pSourceFileHandler == NULL)
	{
		return E_FAIL;
	}

    CWaveNode* pWaveNode = m_pWave->GetNode();
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
	{
		return E_UNEXPECTED;
	}

	CDLSComponent* pComponent = pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return E_UNEXPECTED;
	}

	// Update the header information
	UpdateHeaderInfo();

	// Set the start offset
	DWORD dwHeaderOffset = StreamTell(pIStream);
	
	// Take out the header bytes
	dwHeaderOffset -= 12; 
	m_pSourceFileHandler->SetStartOffset(dwHeaderOffset);
	if(FAILED(m_pSourceFileHandler->ResetOffsets()))
	{
		return E_FAIL;
	}

	while(!m_plstDeltas->IsEmpty())
	{
		delete m_plstDeltas->RemoveHead();

	}

	m_pUndoManager->Initialize();

	// The first delta is always the entire unmodified wave data
	DWORD dwDataOffset = m_pSourceFileHandler->GetDataOffset();
	DWORD dwDataSize = m_pWave->m_OriginalWavObj.dwDataSize;
	DWORD dwLength = m_pWave->GetWaveLength();

	m_pWave->SetHeaderOffset(dwHeaderOffset);
	m_pWave->SetDataOffset(dwDataOffset);

	// Initialize the decomp file if we're loading a runtime
	// compressed wave file....
	if(m_pWave->m_bLoadingRuntime && m_pWave->m_bCompressed)
	{
		DWORD dwCodec = m_pWave->m_rWaveformat.wFormatTag;
		if(FAILED(DecompressRuntimeData()))
		{
			
			if(pComponent->IsInFailedCodecs(dwCodec) == FALSE)
			{
				pComponent->AddToFailedCodecs(dwCodec);

				CString sErrMsg;
				CString sName = "";
				BSTR bstrName;
				if(SUCCEEDED(pWaveNode->GetName(&bstrName)))
				{
					sName = bstrName;
					::SysFreeString(bstrName);
				}
				if(pWaveNode->IsInACollection())
				{
					sErrMsg.Format(IDS_ERR_DLS_RUNTIME_UNSUPPORTED_COMPRESSION_FORMAT, sName);
				}
				else
				{
					sErrMsg.Format(IDS_ERR_WVP_RUNTIME_UNSUPPORTED_COMPRESSION_FORMAT, sName);
				}

				AfxMessageBox(sErrMsg);
			}

			return E_FAIL;
		}
		
		// Check if this was previously in the failed codecs and remove it
		pComponent->RemoveFromFailedCodecs(dwCodec);

        m_bDecompressedRunTime = TRUE;
        m_bDecompressedDesignTime = FALSE;
	}
	else if(m_pWave->m_bCompressed)
	{
		if(FAILED(DecompressDesigntimeData()))
		{
			static bool bShowMessage = true;
			if(bShowMessage)
			{
				AfxMessageBox(IDS_ERR_UNSUPPORTED_COMPRESSION_FORMAT);
			}

			bShowMessage = false;
			m_pWave->m_bCompressed = false;
			m_pWave->CleanWaveObject(&m_pWave->m_CompressedWavObj);
		}
		else
		{
            m_bDecompressedDesignTime = TRUE;
            m_bDecompressedRunTime = FALSE;
		}
	}

    dwDataSize = m_pWave->m_OriginalWavObj.dwDataSize;
    dwLength = dwDataSize / (m_pWave->m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8);
    dwLength /= m_pWave->m_OriginalWavObj.WaveFormatEx.nChannels;

    // Are we reading from the copied decomp file?
    // In that case our offset is 0
    if(m_pCopyFileHandler != NULL)
    {
        dwDataOffset = 0;
    }

    CWaveDelta* pDelta = new CWaveDelta(0, 0, dwLength, dwDataOffset, FALSE);
	if(pDelta == NULL)
	{
		return E_OUTOFMEMORY;
	}
	
	m_plstDeltas->AddTail(pDelta);

    m_bNeedToMakeSrcDelta = m_bDecompressedDesignTime;

    if(FAILED(UpdateUncompressedDeltas()))
    {
        return E_FAIL;
    }

	return S_OK;
}


/////////////////////////////////////////////////////
//
// CWaveDataManager::GetData
//
/////////////////////////////////////////////////////
HRESULT CWaveDataManager::GetData(DWORD dwStartSample, DWORD dwLength, BYTE* pbData, DWORD& dwBytesRead, bool bGetUncompressed)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_POINTER;
	}

    CWaveNode* pWaveNode = m_pWave->GetNode();
    ASSERT(pWaveNode);
    if(pWaveNode == NULL)
    {
        return E_UNEXPECTED;
    }

	ASSERT(m_plstDeltas);
	if(m_plstDeltas == NULL)
	{
		return E_FAIL;
	}

    bool bReadFromCopy = false;

	// If the wave is compressed we want to read from the decomp file
    // Unless this was a runtime decompressed file....the source then 
    // is comrpessed so we should pick up the data from the decomp file
	if((m_pWave->m_bCompressed == true && bGetUncompressed == false) || 
        (bGetUncompressed == true && m_bDecompressedRunTime == TRUE && pWaveNode->IsInACollection() == false))
	{
		return GetDecompData(dwStartSample, dwLength, pbData, dwBytesRead);
	}
    else if(m_pCopyFileHandler && pWaveNode->IsSourceInTempFile() == TRUE && pWaveNode->IsInACollection() == true)
    {
        bReadFromCopy = true;
	}

	// The wave data might be deleted...so don't fail just return S_OK
	if(dwLength == 0)
	{
		return S_OK;
	}

	BOOL bStereo = m_pWave->IsTrueStereo();
	int nChannels = bStereo ? 2 : 1;
	int nSampleSize = m_pWave->m_rWaveformat.wBitsPerSample / 8;

    CPtrList* plstDeltas = m_plstDeltas;
    if(bGetUncompressed)
    {
        nSampleSize = m_pWave->m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8;

        ASSERT(m_plstUncompressedDeltas);
        if(m_plstUncompressedDeltas == NULL)
        {
            return E_FAIL;
        }
        
        plstDeltas = m_plstUncompressedDeltas;
    }


    // This is set to TRUE only when decompressing design-time waves
    // The only delta we have at this point is the decompressed delta
    // We need to go to the original wave to get the uncompressed wave data
    if(m_bNeedToMakeSrcDelta)
    {
        m_bNeedToMakeSrcDelta = false;

        int nDeltas = plstDeltas->GetCount();
        ASSERT(nDeltas== 1);
        
        if(nDeltas == 1)
        {
            DWORD dwSrcSize = m_pWave->m_OriginalWavObj.dwDataSize;
            DWORD dwSrcLength = dwSrcSize / nChannels;
            if(nSampleSize == 2)
            {
                dwSrcLength = dwSrcSize >> 1;
            }

            // Modify the old delta to point to the uncompressed source....
            // It should have been cloned and saved in SaveUndoState call
            CWaveDelta* pDelta = (CWaveDelta*)plstDeltas->GetHead();
            ASSERT(pDelta);
            if(pDelta)
            {
                pDelta->SetLength(dwSrcLength);
            }

            // Adjust the uncomrpessed deltas too
            if(m_plstUncompressedDeltas && m_plstUncompressedDeltas->GetCount() == 1)
            {
                CWaveDelta* pDelta = (CWaveDelta*)m_plstUncompressedDeltas->GetHead();
                ASSERT(pDelta);
                if(pDelta)
                {
                    pDelta->SetLength(dwSrcLength);
                }
            }
        }
        else
        {
            return E_UNEXPECTED;
        }
    }



	// Search the deltas where the datas lie
	POSITION startPosition = NULL;
	POSITION endPosition = NULL;
	POSITION position = plstDeltas->GetHeadPosition();
	while(position)
	{
		POSITION tempPosition = position;
		CWaveDelta* pDelta = (CWaveDelta*) plstDeltas->GetNext(position);
		ASSERT(pDelta);
		if(pDelta->IsSampleInDelta(dwStartSample))
		{
			startPosition = tempPosition;
		}
		
		if(pDelta->IsSampleInDelta(dwStartSample + (dwLength - 1)))
		{
			endPosition = tempPosition;
		}
	}

	if(startPosition == NULL || endPosition == NULL)
	{
		return E_FAIL;
	}
	
	HRESULT hr = E_FAIL;
	dwBytesRead = 0;
	DWORD dwStartSampleForBuffer = dwStartSample;
	DWORD dwBufferSize = dwLength * nChannels * nSampleSize;
	DWORD dwSamplesToRead = dwLength;
	BYTE* pbCurrent = pbData;
	
	bool bContinue = true;
	position = startPosition;
	while(position && bContinue)
	{
		// Continue until we read the end position delta
		bContinue = position == endPosition ? false : true;

		CWaveDelta* pDelta = (CWaveDelta*) plstDeltas->GetNext(position);		
		ASSERT(pDelta);
		DWORD cb = 0;
		if(FAILED(hr = GetData(pDelta, dwStartSampleForBuffer, dwSamplesToRead, pbCurrent, cb, bReadFromCopy)))
		{
			hr = E_FAIL;
			break;
		}

		// How many smaples did we read just now?
		// Add them to the total number bytes read so far
		dwBytesRead += cb;

		// how many samples have we read so far?
		DWORD dwSamplesRead = cb / nSampleSize;

		// What's the start for the next buffer then?
		// We might be reading interleaved stereo data so consider channels...
		dwStartSampleForBuffer += dwSamplesRead / nChannels; 

		// how much of the buffer is filled up now?
		dwBufferSize -= dwBytesRead;

		// How many samples do we have left to read?
		dwSamplesToRead = dwLength - ((dwBytesRead / nSampleSize) / nChannels);

		// Move the pointer to the data buffer by the correct amount...
		pbCurrent = pbData + dwBytesRead;
		
	}

	return hr;
}

/////////////////////////////////////////////////////
//
// CWaveDataManager::GetData
//
/////////////////////////////////////////////////////
HRESULT CWaveDataManager::GetData(CWaveDelta* pDelta, DWORD dwStartSample, DWORD dwLength, BYTE* pbData, DWORD& dwBytesRead, bool bReadFromCopy)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pDelta);
	if(pDelta == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_POINTER;
	}

	if(bReadFromCopy)
	{
		ASSERT(m_pCopyFileHandler);
		if(m_pCopyFileHandler == NULL)
		{
			return E_FAIL;
		}
	}

	BOOL bStereo = m_pWave->IsTrueStereo();


	int nChannels = bStereo ? 2 : 1;
	int nSampleSize = m_pWave->m_rWaveformat.wBitsPerSample / 8;

	BOOL bIsEditDelta = pDelta->IsEditDelta();
	DWORD dwDeltaLength = pDelta->GetLength();
	DWORD dwDeltaActualStart = pDelta->GetActualPosition();
	DWORD dwDeltaActualEnd = dwDeltaActualStart + dwDeltaLength;
	DWORD dwFileOffset = pDelta->GetFileOffset();

	if(dwStartSample < dwDeltaActualStart || dwStartSample > dwDeltaActualEnd)
	{
		dwBytesRead = 0;
		return E_FAIL;
	}

	if((dwStartSample + dwLength) > dwDeltaActualEnd)
	{
		dwLength = dwDeltaActualEnd - dwStartSample;
	}

	CWaveFileHandler* pFileHandler = m_pSourceFileHandler;
	if(bReadFromCopy)
	{
		pFileHandler = m_pCopyFileHandler;
	}

	UINT nBytesToRead = dwLength * nSampleSize * nChannels;
	if(bIsEditDelta)
	{
		ASSERT(m_pTempFileHandler);
		pFileHandler = m_pTempFileHandler;
	}

	if(pFileHandler == NULL)
	{
		return E_FAIL;
	}

	DWORD dwStartDiff = dwStartSample - dwDeltaActualStart;
	DWORD dwOffsetDiff = dwStartDiff * nSampleSize * nChannels;
	DWORD dwReadOffset = dwFileOffset + dwOffsetDiff;

    // If we're reading from the decompressed file copy then we don't have an offset
    // The offset with the delta in this case is the offset in the original source file
    // which has the compressed data....this is especially important for runtime collections
    /*if(bReadFromCopy)
	{
        dwReadOffset = 0;
    }*/
	
	DWORD cb = 0;
	if(FAILED(pFileHandler->ReadData(dwReadOffset, nBytesToRead, pbData, cb)))
	{
		return E_FAIL;
	}

	dwBytesRead += cb;

	return S_OK;
}


HRESULT CWaveDataManager::InsertData(DWORD dwInsertAt, BYTE* pbData, DWORD dwLength)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_POINTER;
	}

	if(m_pTempFileHandler == NULL)
	{
		m_pTempFileHandler = new CWaveTempFileHandler(m_sTempFileName);
		ASSERT(m_pTempFileHandler);
		if(m_pTempFileHandler == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}

	BOOL bStereo = m_pWave->IsTrueStereo();
	int nChannels = bStereo ? 2 : 1;
	int nSampleSize = m_pWave->m_rWaveformat.wBitsPerSample / 8;

	// Write the insertion data to the temp file and remember the file offset
	DWORD dwTempFileOffset = m_pTempFileHandler->SeekToEnd();
	DWORD dwBytesToWrite = dwLength * nChannels * nSampleSize;
	m_pTempFileHandler->Write((void*)pbData, dwBytesToWrite);
	
	// Create the new insertion delta
	CWaveDelta* pInsertDelta = new CWaveDelta(dwInsertAt, 0, dwLength, dwTempFileOffset, TRUE);
	if(pInsertDelta == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Find out the position where the data needs to be inserted
	POSITION position = m_plstDeltas->GetHeadPosition();
	while(position)
	{
		POSITION insertionPosition = position;
		CWaveDelta* pDelta = (CWaveDelta*) m_plstDeltas->GetNext(position);
		ASSERT(pDelta);

		if(pDelta->IsSampleInDelta(dwInsertAt) == TRUE)
		{
			// Split the existing delta so we can insert the new data
			DWORD dwOldDeltaLength = pDelta->GetLength();
			DWORD dwOldFileOffset = pDelta->GetFileOffset();
			DWORD dwOldDeltaPosition = pDelta->GetActualPosition();
			DWORD dwOldStartSample = pDelta->GetStartSample();
			BOOL bEditDelta = pDelta->IsEditDelta();

			DWORD dwNewLength = 0;
			if(dwInsertAt > 0)
			{
				dwNewLength = dwInsertAt - dwOldDeltaPosition;	
			}

			DWORD dwSplitDeltaPosition = dwInsertAt + dwLength;
			DWORD dwSplitDeltaLength = dwOldDeltaLength - dwNewLength;
			DWORD dwSplitDeltaStart = dwOldStartSample + dwNewLength;
			
			BOOL bIsStereo = m_pWave->IsTrueStereo();
			int nBytesPerSample = m_pWave->m_rWaveformat.wBitsPerSample / 8;

			DWORD dwStartSampleDifference = dwSplitDeltaStart - dwOldStartSample;
			DWORD dwSpitDeltaFileOffset = dwOldFileOffset + (dwStartSampleDifference * nBytesPerSample * nChannels);
				
			CWaveDelta* pSplitDelta = NULL;
			if(dwSplitDeltaLength > 0)
			{
				pSplitDelta = new CWaveDelta(dwSplitDeltaPosition, dwSplitDeltaStart, dwSplitDeltaLength, dwSpitDeltaFileOffset, bEditDelta);
				if(pSplitDelta == NULL)
				{
					return E_OUTOFMEMORY;
				}
			}

			// For the first half of the split delta only the length changes
			pDelta->SetLength(dwNewLength);

			// Insert the new insertion delta before the second half of the split delta
			insertionPosition = m_plstDeltas->InsertAfter(insertionPosition, pInsertDelta);

			// Insert the second half of the split delta
			if(pSplitDelta)
			{
				m_plstDeltas->InsertAfter(insertionPosition, pSplitDelta);
			}

			ResetActualDeltaStarts();

			return S_OK;
		}
	}

	// We didn't find this insertion position in any of the deltas...add it to the end
	m_plstDeltas->AddTail(pInsertDelta);

	return S_OK;
}


HRESULT CWaveDataManager::RemoveData(DWORD dwStartSample, DWORD dwSamples)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}
	
	BOOL bStereo = m_pWave->IsTrueStereo();
	int nChannels = bStereo ? 2 : 1;
	int nSampleSize = m_pWave->m_rWaveformat.wBitsPerSample / 8;

	DWORD dwStart = dwStartSample;
	DWORD dwLength = dwSamples;

	POSITION position = m_plstDeltas->GetHeadPosition();
	while(position)
	{
		POSITION insertionPosition = position;
		CWaveDelta* pDelta = (CWaveDelta*) m_plstDeltas->GetNext(position);
		ASSERT(pDelta);

		// What's our end sample?
		DWORD dwEndSample = dwStart + (dwLength - 1);

		// Get the delta params
		DWORD dwDeltaPosition = pDelta->GetActualPosition();
		DWORD dwDeltaStart = pDelta->GetStartSample();
		DWORD dwDeltaLength = pDelta->GetLength();
		DWORD dwFileOffset = pDelta->GetFileOffset();
		DWORD dwDeltaEnd = dwDeltaPosition + dwDeltaLength;
		BOOL bEditDelta = pDelta->IsEditDelta();

		// If the start and end are in the same delta we need to split that delta
		if(pDelta->IsSampleInDelta(dwStart) && pDelta->IsSampleInDelta(dwEndSample))
		{
			DWORD dwSamplesRemoved = dwLength;
			DWORD dwBytesRemoved = dwLength * nSampleSize * nChannels;
			DWORD dwNewDeltaLength = dwStart - dwDeltaPosition;

			DWORD dwSplitPosition = dwDeltaPosition + dwNewDeltaLength;
			DWORD dwSplitLength = dwDeltaEnd - dwSplitPosition - dwSamplesRemoved;
			DWORD dwSplitStart = dwDeltaStart + dwNewDeltaLength + dwLength;
			DWORD dwStartDiff = dwSplitStart - dwDeltaStart;
			DWORD dwSplitFileOffset = dwFileOffset + (dwStartDiff * nSampleSize * nChannels);

			// Create the split delta
			CWaveDelta* pSplitDelta = new CWaveDelta(dwSplitPosition, dwSplitStart, dwSplitLength, dwSplitFileOffset, bEditDelta);
			ASSERT(pSplitDelta);
			if(pSplitDelta == NULL)
			{
				return E_OUTOFMEMORY;
			}

			// Update old delta and insert the split one
			pDelta->SetLength(dwNewDeltaLength);
			m_plstDeltas->InsertAfter(insertionPosition, pSplitDelta);

			break;
		}
		else if(pDelta->IsSampleInDelta(dwStart))
		{
			// Only the length changes for the start delta
			DWORD dwSamplesRemoved = dwDeltaEnd - dwStart;
			DWORD dwNewDeltaLength = dwDeltaLength - dwSamplesRemoved;

			pDelta->SetLength(dwNewDeltaLength);

			dwStart += dwSamplesRemoved;
			dwLength -= dwSamplesRemoved;
		}
		else if(pDelta->IsSampleInDelta(dwEndSample))
		{
			// The actual position of the delta shifts as well as the start sample, length and the file offset
			DWORD dwNewPosition = dwEndSample;
			DWORD dwSamplesRemoved = dwNewPosition - dwDeltaPosition;
			DWORD dwBytesRemoved = dwSamplesRemoved * nSampleSize * nChannels;
			DWORD dwNewFileOffset = dwFileOffset + dwBytesRemoved;
			DWORD dwNewStart = dwDeltaStart + dwSamplesRemoved;
			DWORD dwNewLength = dwDeltaLength - dwSamplesRemoved;

			// Set the new values on the delta
			pDelta->SetActualPosition(dwNewPosition);
			pDelta->SetLength(dwNewLength);
			pDelta->SetFileOffset(dwNewFileOffset);
			pDelta->SetStartSample(dwNewStart);
		}
	}

	// Make sure all actual positions are correct
	ResetActualDeltaStarts();

	return S_OK;
}


void CWaveDataManager::ResetActualDeltaStarts()
{
	// Go through the list and remove all deltas of 0 length
	POSITION position = m_plstDeltas->GetHeadPosition();
	while(position)
	{
		POSITION oldPosition = position;
		CWaveDelta* pDelta = (CWaveDelta*) m_plstDeltas->GetNext(position);
		ASSERT(pDelta);
		if(pDelta->GetLength() == 0)
		{
			m_plstDeltas->RemoveAt(oldPosition);
			delete pDelta;
		}
	}

	// Now reset the positions
	DWORD dwStart = 0;
	position = m_plstDeltas->GetHeadPosition();
	while(position)
	{
		CWaveDelta* pDelta = (CWaveDelta*) m_plstDeltas->GetNext(position);
		ASSERT(pDelta);
		
		pDelta->SetActualPosition(dwStart);
		dwStart += pDelta->GetLength();
	}
}


HRESULT CWaveDataManager::CopyDecompFile()
{
	// Have we already copied the source??
	// We don't need to do it again, do we?
	ASSERT(m_pCopyFileHandler == NULL);
	if(m_pCopyFileHandler != NULL)
	{
		return S_OK;
	}

	// Create a temp filename for the copy
	if(m_sCopyFileName.IsEmpty())
	{
		char szCopyFileName[MAX_PATH];

		CString sPrefix = "DMP";
		CString sExtension = "tmp";
		if(FAILED(CreateUniqueTempFile(sPrefix, sExtension, m_sCopyFileName)))
		{
			GetTempFileName(m_sTempFilePath, sPrefix, 0, szCopyFileName);
			m_sCopyFileName = szCopyFileName;	
		}
	}

	// Copy the file
	if(CopyFile(m_sDecompFileName, m_sCopyFileName, FALSE) == FALSE)
	{
		DWORD dwLastError = GetLastError();
		LPVOID lpvError;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpvError, 0, NULL);
		CString sDetails;
		sDetails.Format("\n%s", (LPCSTR)lpvError);
		AfxMessageBox(sDetails);
		return E_FAIL;
	}

	// Create the file handler for the copy
	m_pCopyFileHandler = new CWaveFileHandler(m_sCopyFileName, CFile::modeRead);
	ASSERT(m_pCopyFileHandler);
	if(m_pCopyFileHandler == NULL)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;		
}


HRESULT CWaveDataManager::SaveUncompressedDataToStream(IStream* pIStream, DWORD& dwBytesWritten)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if(FAILED(AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return E_OUTOFMEMORY;
	}

	BOOL bStereo = m_pWave->IsTrueStereo();
	int nChannels = bStereo ? 2 : 1;
	int nSampleSize = m_pWave->m_OriginalWavObj.WaveFormatEx.wBitsPerSample / 8;

	dwBytesWritten = 0;
	
	// We need to write the data in chunks as the data size might be really huge
	DWORD dwSamplesWritten = 0;
	DWORD dwStart = 0;
	
	// We'll write WAVE_DATA_BUFFER_SIZE samples at a time
	// Unless the wave is small enough to read in one shot
    DWORD dwDataSize = m_pWave->m_OriginalWavObj.dwDataSize;
    DWORD dwWaveLength = nSampleSize == 1 ? (dwDataSize / nChannels) : ((dwDataSize >> 1) / nChannels);
	DWORD dwLength = dwWaveLength > WAVE_DATA_BUFFER_SIZE ? WAVE_DATA_BUFFER_SIZE : dwWaveLength;

	while(dwSamplesWritten < dwWaveLength)
	{
		DWORD dwBytesToWrite = dwLength * nChannels * nSampleSize;
		BYTE* pbData = NULL;
		DWORD cb = 0;

        if(FAILED(m_pWave->SafeAllocBuffer(&pbData, dwBytesToWrite)))
        {
            dwBytesWritten = 0;
            pIRiffStream->Release();
            return E_OUTOFMEMORY;
        }

		if(FAILED(GetData(dwStart, dwLength, pbData, cb, true)))
		{
            delete[] pbData;
			dwBytesWritten = 0;
			pIRiffStream->Release();
			return E_FAIL;
		}

		DWORD cbStream = 0;
		if(FAILED(pIStream->Write((void*)pbData, cb, &cbStream)) || cbStream != cb)
		{
			delete[] pbData;
			pIRiffStream->Release();
			return E_FAIL;
		}
	
		dwBytesWritten += cb;
		dwSamplesWritten += (cb / nSampleSize) / nChannels;
		dwStart = dwSamplesWritten;
		dwLength = (dwWaveLength - dwSamplesWritten);
		dwLength = dwLength > WAVE_DATA_BUFFER_SIZE ? WAVE_DATA_BUFFER_SIZE : dwLength;
		delete[] pbData;
	}
	
	pIRiffStream->Release();

	ASSERT(dwSamplesWritten == dwWaveLength);
	return S_OK;
}


HRESULT	CWaveDataManager::CreateDecompFile()
{
	// We might have an earlier file with the decompressed data
	// Delete that handler and create a new one...
	if(m_pDecompFileHandler)
	{
		delete m_pDecompFileHandler;
		m_pDecompFileHandler = NULL;
	}
	
	m_pDecompFileHandler = new CWaveFileHandler(m_sDecompFileName, CFile::modeCreate | CFile::modeReadWrite);
	if(m_pDecompFileHandler == NULL)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}


HRESULT CWaveDataManager::WriteDecompressedData(BYTE* pbData, DWORD dwDataSize)
{
	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pDecompFileHandler);
	if(m_pDecompFileHandler == NULL)
	{
		return E_FAIL;
	}

	// Append the passed data
	m_pDecompFileHandler->SeekToEnd();
	
	try
	{
		m_pDecompFileHandler->Write((void*)pbData, dwDataSize);
	}
	catch(CFileException e)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWaveDataManager::GetDecompData(DWORD dwStartSample, DWORD dwLength, BYTE* pbData, DWORD& dwBytesRead)
{
	// Do we have a decomp file handler?
	ASSERT(m_pDecompFileHandler);
	if(m_pDecompFileHandler == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}
	
	BOOL bStereo = m_pWave->IsTrueStereo();
	int nChannels = bStereo ? 2 : 1;
	int nSampleSize = m_pWave->m_rWaveformat.wBitsPerSample / 8;

	// Must have a preallocated buffer passed...
	ASSERT(pbData);
	if(pbData == NULL)
	{
		return E_POINTER;
	}

	// Get the offset to the start sample and the bytes to read
	DWORD dwReadOffset = dwStartSample * nChannels * nSampleSize;
	DWORD dwBytesToRead = dwLength * nChannels * nSampleSize;

	DWORD cb = 0;
	if(FAILED(m_pDecompFileHandler->ReadData(dwReadOffset, dwBytesToRead, pbData, cb)))
	{
		dwBytesRead = 0;
		return E_FAIL;
	}

	dwBytesRead = cb;

	return S_OK;
}


HRESULT CWaveDataManager::RenameSource(CString sNewSourceName)
{
	if(FAILED(CloseSourceHandler()))
	{
		return E_FAIL;
	}

	// Rename the file
	try
	{
		m_pSourceFileHandler->Rename(m_sSourceFileName, sNewSourceName);
	}
	catch(CFileException e)
	{
		return E_FAIL;
	}

	return OnSourceRenamed(sNewSourceName);

}



HRESULT CWaveDataManager::OnSourceRenamed(CString sNewSourceFile)
{
	// Close the previous file if it's open
	ASSERT(m_pSourceFileHandler);
	if(m_pSourceFileHandler == NULL)
	{
		return E_POINTER;
	}
	
	if(m_pSourceFileHandler->Open(sNewSourceFile, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone, NULL) == 0)
	{
		return E_FAIL;
	}

	m_sSourceFileName = sNewSourceFile;

	return S_OK;
}


CString CWaveDataManager::GetSourceFileName()
{
	return m_sSourceFileName;
}


HRESULT CWaveDataManager::Initialize(CString sFileName, DWORD dwHeaderOffset, DWORD dwDataOffset, bool bDeleteOldSource)
{
	ASSERT(!sFileName.IsEmpty());
	if(sFileName.IsEmpty())
	{
		return E_INVALIDARG;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

    CWaveNode* pWaveNode = m_pWave->GetNode();
    ASSERT(pWaveNode);
    if(pWaveNode == NULL)
    {
        return E_UNEXPECTED;
    }

	ASSERT(m_pSourceFileHandler);
	if(m_pSourceFileHandler == NULL)
	{
		return E_FAIL;
	}

	m_bInInit = TRUE;

	// Update the header information
	UpdateHeaderInfo();

    // we don't do anything if our source is outside of a collection
    // and this is not a cleanup after the save where we delete the temp file
    if(bDeleteOldSource == false && 
        pWaveNode->IsSourceInTempFile() == TRUE &&
        pWaveNode->IsInACollection() == TRUE)
    {
        return S_FALSE;
    }

	// Close the old temp file created before save 
	if(m_pSourceFileHandler->m_hFile != CFile::hFileNull)
	{
		m_pSourceFileHandler->Close();
	}
	
	// Delete the temp file only if we're NOT in a collection
	if(bDeleteOldSource)
	{
		if(m_pWave->GetCollection() == NULL)
		{	
			DeleteFile(m_sSourceFileName);
		}

        while(!m_plstDeltas->IsEmpty())
	    {
		    delete m_plstDeltas->RemoveHead();
	    }
	}

	if (FAILED(OnSourceRenamed(sFileName)))
		return E_FAIL;

    BOOL bSetCopyDeltaOffset = FALSE;
    if(m_pCopyFileHandler && m_bDecompressedRunTime == TRUE && pWaveNode->IsInACollection() == FALSE)
    {
        delete m_pCopyFileHandler;
		m_pCopyFileHandler = NULL;

		// Delete the copied source file
		DeleteFile(m_sCopyFileName);
        bSetCopyDeltaOffset = TRUE;
    }


	// The first delta is always the entire unmodified wave data
    // But keep the offset difference first
    // Just adjust the offsets for all deltas
    int nDataOffsetDiff = 0;
    if(bDeleteOldSource == false && pWaveNode->IsSourceInTempFile() == FALSE)
    {
        nDataOffsetDiff = dwDataOffset - m_pSourceFileHandler->GetDataOffset();
    }

	m_pSourceFileHandler->SetDataOffset(dwDataOffset);
	m_pSourceFileHandler->SetStartOffset(dwHeaderOffset);

	DWORD dwDataSize = m_HeaderInfo.m_dwDataSize;
	DWORD dwLength = m_HeaderInfo.m_dwWaveLength;

    if(bDeleteOldSource)
    {
	    CWaveDelta* pDelta = new CWaveDelta(0, 0, dwLength, dwDataOffset, FALSE);
	    if(pDelta == NULL)
	    {
		    return E_OUTOFMEMORY;
	    }
	    
        m_plstDeltas->AddTail(pDelta);

        // If the wave is compressed make sure we create the source delta when required
        if(m_pWave->m_bCompressed)
        {
            m_bNeedToMakeSrcDelta = TRUE;
        }

	    m_pUndoManager->Initialize();

	    // Delete any temporary files created earlier
	    if(m_pTempFileHandler)
	    {
		    delete m_pTempFileHandler;
		    m_pTempFileHandler = NULL;

		    // Delete the created temp file
		    DeleteFile(m_sTempFileName);
	    }

        UpdateUncompressedDeltas();
    }
    else
    {
        POSITION position = m_plstDeltas->GetHeadPosition();
        while(position)
        {
            CWaveDelta* pDelta = (CWaveDelta*)m_plstDeltas->GetNext(position);
            ASSERT(pDelta);
            if(pDelta && pDelta->IsEditDelta() == FALSE)
            {
                if(bSetCopyDeltaOffset)
                {
                    pDelta->SetFileOffset(dwDataOffset);
                }
                else
                {
                    pDelta->SetFileOffset(pDelta->GetFileOffset() + nDataOffsetDiff);
                }
            }
        }

        // Do the same thing for the uncompressed deltas
        if(m_plstUncompressedDeltas)
        {
            position = m_plstUncompressedDeltas->GetHeadPosition();
            while(position)
            {
                CWaveDelta* pDelta = (CWaveDelta*)m_plstUncompressedDeltas->GetNext(position);
                ASSERT(pDelta);
                if(pDelta && pDelta->IsEditDelta() == FALSE)
                {
                    if(bSetCopyDeltaOffset)
                    {
                        pDelta->SetFileOffset(dwDataOffset);
                    }
                    else
                    {
                        pDelta->SetFileOffset(pDelta->GetFileOffset() + nDataOffsetDiff);
                    }
                }
            }
        }
    }

	m_bInInit = FALSE;

    m_bDecompressedDesignTime = FALSE;
    m_bDecompressedRunTime = FALSE;

	return S_OK;
}


HRESULT CWaveDataManager::CloseSourceHandler()
{
	ASSERT(m_pSourceFileHandler);
	if(m_pSourceFileHandler == NULL)
	{
		return E_POINTER;
	}

	if(m_pSourceFileHandler->m_hFile != CFile::hFileNull)
	{
		m_pSourceFileHandler->Close();
	}

	return S_OK;
}


HRESULT CWaveDataManager::DecompressDesigntimeData()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pSourceFileHandler);
	if(m_pSourceFileHandler == NULL)
	{
		return E_POINTER;
	}

	if(FAILED(CreateDecompFile()))
	{
		return E_FAIL;
	}

    CWaveNode* pWaveNode = m_pWave->GetNode();
    ASSERT(pWaveNode);
    if(pWaveNode == NULL)
    {
        return E_UNEXPECTED;
    }

    if(pWaveNode->IsInACollection() == FALSE && 
        (m_pWave->m_CompressedWavObj.WaveFormatEx.wFormatTag == WMAUDIO_FORMAT_TAG || 
        m_pWave->m_CompressedWavObj.WaveFormatEx.wFormatTag == MSAUDIO_FORMAT_TAG))
    {
        return E_FAIL;
    }

	int nExtraBytes = m_pWave->m_CompressedWavObj.WaveFormatEx.cbSize;
	WAVEFORMATEX* pwfxDest = NULL;
	if(FAILED(m_pWave->SafeAllocBuffer((BYTE**)&pwfxDest, sizeof(WAVEFORMATEX) + nExtraBytes)))
	{
		return E_OUTOFMEMORY;
	}

	CopyMemory((BYTE*)pwfxDest, (BYTE*)&(m_pWave->m_CompressedWavObj.WaveFormatEx), sizeof(WAVEFORMATEX));
	CopyMemory((BYTE*)pwfxDest + sizeof(WAVEFORMATEX), m_pWave->m_CompressedWavObj.pbExtractWaveFormatData, nExtraBytes);

	WAVEFORMATEX wfxSource = m_pWave->m_OriginalWavObj.WaveFormatEx;

	int nBytesPerSample = wfxSource.wBitsPerSample / 8;
	int nChannels = wfxSource.nChannels;

	DWORD dwUncompressedSize = m_pWave->m_OriginalWavObj.dwDataSize;
	DWORD dwDecompressedSize = 0;
	DWORD dwCompressedSize = 0;
	DWORD dwBytesConverted = 0;
	DWORD dwBytesToConvert = dwUncompressedSize; 
	DWORD dwSamplesToConvert = (dwUncompressedSize / nBytesPerSample) / nChannels;
	DWORD dwDataOffset = m_pSourceFileHandler->GetDataOffset();
	
	while(dwBytesConverted < dwUncompressedSize)
	{
		/*if(dwSamplesToConvert > WAVE_DATA_BUFFER_SIZE * 2)
		{
			dwBytesToConvert  = WAVE_DATA_BUFFER_SIZE * nChannels * nBytesPerSample;
		}*/

		BYTE* pbData = NULL;
		if(FAILED(m_pWave->SafeAllocBuffer(&pbData, dwBytesToConvert)))
		{
			delete[] pwfxDest;
			return E_OUTOFMEMORY;
		}

		DWORD cbRead = 0;
		if(FAILED(m_pSourceFileHandler->ReadData(dwDataOffset + dwBytesConverted, dwBytesToConvert, pbData, cbRead)))
		{
			delete[] pbData;
			delete[] pwfxDest;
			return E_FAIL;
		}

		// Compress the data now
		DWORD cbCompressed = 0;
		BYTE* pbCompressed = NULL;
		if(FAILED(m_pWave->ConvertWave(&wfxSource, pwfxDest, pbData, cbRead, &pbCompressed, cbCompressed)))
		{
			delete[] pbData;
			delete[] pwfxDest;
			return E_FAIL;
		}

		delete[] pbData;
		dwCompressedSize += cbCompressed;

		// Decompress the data now
		DWORD cbDecompressed = 0;
		BYTE* pbDecompressed = NULL;
		if(FAILED(m_pWave->ConvertWave(pwfxDest, &wfxSource, pbCompressed, cbCompressed, &pbDecompressed, cbDecompressed)))
		{
			delete[] pbCompressed;
			delete[] pwfxDest;
			return E_FAIL;
		}

		dwBytesConverted += cbRead;
		dwBytesToConvert = dwUncompressedSize - dwBytesConverted;
		dwSamplesToConvert = (dwBytesToConvert / nBytesPerSample) / nChannels;
		dwDecompressedSize += cbDecompressed;
	
		if(FAILED(WriteDecompressedData(pbDecompressed, cbDecompressed)))
		{
			delete[] pbCompressed;
			delete[] pbDecompressed;
			delete[] pwfxDest;
			return E_FAIL;
		}

		delete[] pbCompressed;
		delete[] pbDecompressed;
	}

	// Delete the temporary destination waveformat buffer
	if(pwfxDest)
	{
		delete[] pwfxDest;
	}

	m_pWave->m_dwWaveLength = (dwDecompressedSize / nBytesPerSample) / nChannels;
	m_pWave->m_dwDataSize = dwDecompressedSize;

	m_pWave->m_DecompressedWavObj.dwDataSize = dwDecompressedSize;
	CopyMemory(&(m_pWave->m_DecompressedWavObj.WaveFormatEx), &wfxSource, sizeof(WAVEFORMATEX));

	m_pWave->m_CompressedWavObj.dwDataSize = dwCompressedSize;
	
	if(m_pWave->IsStereo())
	{
		CWaveNode* pWaveNode = m_pWave->GetNode();
		ASSERT(pWaveNode);
		if(pWaveNode->IsInACollection() == false)
		{
			CStereoWave* pStereoWave = dynamic_cast<CStereoWave*>(pWaveNode->GetWave());
			ASSERT(pStereoWave);
			if(pStereoWave)
			{
				if(FAILED(pStereoWave->UpdateChannelsForStereoCompression()))
				{
					return E_FAIL;
				}
			}
		}
	}

	UpdateHeaderInfo();

	return S_OK;
}



HRESULT	CWaveDataManager::DecompressRuntimeData()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pSourceFileHandler);
	if(m_pSourceFileHandler == NULL)
	{
		return E_POINTER;
	}

	if(FAILED(CreateDecompFile()))
	{
		return E_FAIL;
	}

    CWaveNode* pWaveNode = m_pWave->GetNode();
    ASSERT(pWaveNode);
    if(pWaveNode == NULL)
    {
        return E_UNEXPECTED;
    }

    if(pWaveNode->IsInACollection() == FALSE && 
        (m_pWave->m_CompressedWavObj.WaveFormatEx.wFormatTag == WMAUDIO_FORMAT_TAG || 
        m_pWave->m_CompressedWavObj.WaveFormatEx.wFormatTag == MSAUDIO_FORMAT_TAG))
    {
        return E_FAIL;
    }

	WAVEFORMATEX wfxDest = m_pWave->m_OriginalWavObj.WaveFormatEx;

	UINT nExtraBytes = m_pWave->m_CompressedWavObj.WaveFormatEx.cbSize;
	WAVEFORMATEX* pwfxCompression = NULL;
	if(FAILED(m_pWave->SafeAllocBuffer((BYTE**)&pwfxCompression, sizeof(WAVEFORMATEX) + nExtraBytes)))
	{
		return E_OUTOFMEMORY;
	}

	CopyMemory((BYTE*)pwfxCompression, &m_pWave->m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
	CopyMemory((BYTE*)pwfxCompression + sizeof(WAVEFORMATEX), m_pWave->m_CompressedWavObj.pbExtractWaveFormatData, nExtraBytes);

	// Gotta decompress in chunks
	DWORD dwCompressedSize = m_pWave->m_CompressedWavObj.dwDataSize;
	DWORD dwDecompressedSize = 0;
	DWORD dwConverted = 0;
	DWORD dwLength = dwCompressedSize;
	DWORD dwDataOffset = m_pSourceFileHandler->GetDataOffset();

	while(dwConverted < dwCompressedSize)
	{
		/*if(dwLength > WAVE_DATA_BUFFER_SIZE * 2)
		{
			dwLength = WAVE_DATA_BUFFER_SIZE;
		}*/

		BYTE* pbData = NULL;
		if(FAILED(m_pWave->SafeAllocBuffer(&pbData, dwLength)))
		{
			delete[] pwfxCompression;
			return E_OUTOFMEMORY;
		}

		DWORD cb = 0;
		if(FAILED(m_pSourceFileHandler->ReadData(dwDataOffset + dwConverted, dwLength, pbData, cb)))
		{
			delete[] pbData;
			delete[] pwfxCompression;
			return E_FAIL;
		}

		// Convert the data now
		DWORD cbDecompressed = 0;
		BYTE* pbDecompressed = NULL;
		if(FAILED(m_pWave->ConvertWave(pwfxCompression, &wfxDest, pbData, cb, &pbDecompressed, cbDecompressed)))
		{
			delete[] pbData;
			delete[] pwfxCompression;
			return E_FAIL;
		}

		if(cb > 0 && cbDecompressed == 0)
		{
			delete[] pbData;
			delete[] pwfxCompression;
			return E_FAIL;
		}

		dwConverted += cb;
		dwLength = dwCompressedSize - dwConverted;
		dwDecompressedSize += cbDecompressed;
	
		if(FAILED(WriteDecompressedData(pbDecompressed, cbDecompressed)))
		{
			delete[] pbData;
			delete[] pbDecompressed;
			delete[] pwfxCompression;
			return E_FAIL;
		}

		delete[] pbData;
		delete[] pbDecompressed;
	}

	if(pwfxCompression)
	{
		delete[] pwfxCompression;
		pwfxCompression = NULL;
	}

	int nBytesPerSample = wfxDest.wBitsPerSample / 8;
	int nChannels = wfxDest.nChannels;
	
	m_pWave->m_dwWaveLength = (dwDecompressedSize / nBytesPerSample) / nChannels;
	m_pWave->m_dwDataSize = dwDecompressedSize;

	m_pWave->m_DecompressedWavObj.dwDataSize = dwDecompressedSize;
	CopyMemory(&(m_pWave->m_DecompressedWavObj.WaveFormatEx), &wfxDest, sizeof(WAVEFORMATEX));

	// The original object also becomes the same format
	m_pWave->m_OriginalWavObj.dwDataSize = dwDecompressedSize;
	CopyMemory(&(m_pWave->m_rWaveformat), &wfxDest, sizeof(WAVEFORMATEX));

	if(m_pWave->IsStereo())
	{
		CWaveNode* pWaveNode = m_pWave->GetNode();
		ASSERT(pWaveNode);
		if(pWaveNode->IsInACollection() == false)
		{
			CStereoWave* pStereoWave = dynamic_cast<CStereoWave*>(pWaveNode->GetWave());
			ASSERT(pStereoWave);
			if(pStereoWave)
			{
				if(FAILED(pStereoWave->UpdateChannelsForStereoCompression()))
				{
					return E_FAIL;
				}
			}
		}
	}

	UpdateHeaderInfo();

    if(pWaveNode->IsInACollection())
    {
        if(FAILED(CopyDecompFile()))
        {
            return E_FAIL;
        }

        pWaveNode->SetSourceInTempFile(TRUE);
    }

	return S_OK;
}

HRESULT CWaveDataManager::SaveUndoState(CString sStateName)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_FAIL;

	ASSERT(m_pUndoManager);
	if(m_pUndoManager == NULL)
		return E_FAIL;

	CPtrList* pClonedList = NULL;
	if(FAILED(CloneDeltaList(m_plstDeltas, &pClonedList)))
		return E_FAIL;

	// Ensure the loop points, sample rate, etc. are all current
	UpdateHeaderInfo();

    LPWAVEFORMATEX pwfxCompression = NULL;
    if(m_pWave->m_bCompressed)
	    {
        pwfxCompression = (LPWAVEFORMATEX) new BYTE[sizeof(WAVEFORMATEX) + m_pWave->m_CompressedWavObj.WaveFormatEx.cbSize];
        if(pwfxCompression == NULL)
        	{
			while (!pClonedList->IsEmpty())
				delete pClonedList->RemoveHead();
			delete pClonedList;
            return E_OUTOFMEMORY;
        	}

        CopyMemory((BYTE*)pwfxCompression, (BYTE*)&m_pWave->m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
        CopyMemory((BYTE*)pwfxCompression + sizeof(WAVEFORMATEX), m_pWave->m_CompressedWavObj.pbExtractWaveFormatData, m_pWave->m_CompressedWavObj.WaveFormatEx.cbSize);
	    }

	return m_pUndoManager->SaveState(sStateName, pClonedList, &m_HeaderInfo, pwfxCompression);
}

HRESULT CWaveDataManager::Undo()
{
	ASSERT(m_pUndoManager);
	if(m_pUndoManager == NULL)
	{
		return E_FAIL;
	}

	// Clone the delta list and pass it
	CPtrList* pDeltaList = NULL;
	if(FAILED(CloneDeltaList(m_plstDeltas, &pDeltaList)))
	{
		return E_FAIL;
	}

	// Ensure the loop points, sample rate, etc. are all current
	UpdateHeaderInfo();

    // We pass the compression format to the undo manager...
    // It may decide to keep it if it's saving a redo state or it will delete it to avoid leaks
    // Our contract is providing all the necessary information to persist in the state....
    LPWAVEFORMATEX pwfxCompression = NULL;
    if(m_pWave->m_bCompressed)
    {
        pwfxCompression = (LPWAVEFORMATEX) new BYTE[sizeof(WAVEFORMATEX) + m_pWave->m_CompressedWavObj.WaveFormatEx.cbSize];
        if(pwfxCompression == NULL)
        {
            return E_OUTOFMEMORY;
        }

        CopyMemory((BYTE*)pwfxCompression, (BYTE*)&m_pWave->m_CompressedWavObj.WaveFormatEx, sizeof(WAVEFORMATEX));
        CopyMemory((BYTE*)pwfxCompression + sizeof(WAVEFORMATEX), m_pWave->m_CompressedWavObj.pbExtractWaveFormatData, m_pWave->m_CompressedWavObj.WaveFormatEx.cbSize);
    }

	CPtrList* pNewList = NULL;
	if(FAILED(m_pUndoManager->Undo(pDeltaList, &pNewList, &m_HeaderInfo, &pwfxCompression)))
	{
		return E_FAIL;
	}

	CWaveNode* pWaveNode = m_pWave->GetNode();
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
	{
		return E_FAIL;
	}

	EnterCriticalSection(&pWaveNode->m_CriticalSection);
	
	CPtrList* pOldDeltas = m_plstDeltas;
	if(FAILED(CloneDeltaList(pNewList, &m_plstDeltas)))
	{
		LeaveCriticalSection(&pWaveNode->m_CriticalSection);
		return E_FAIL;
	}

	LeaveCriticalSection(&pWaveNode->m_CriticalSection);

	// Delete the old list
	while(!pOldDeltas->IsEmpty())
	{
		delete pOldDeltas->RemoveHead();
	}
	delete pOldDeltas;

    if(m_HeaderInfo.m_bCompressed)
    {
        if(FAILED(m_pWave->Compress(pwfxCompression, true)))
        {
            return E_FAIL;
        }

        // Adjust the header info for the newly compressed length
        m_HeaderInfo.m_dwDataSize = m_pWave->GetDataSize();
	    m_HeaderInfo.m_dwWaveLength = m_pWave->GetWaveLength();
    }

	// Get the new wavelength for the wave
	m_pWave->UpdateOnUndo(m_HeaderInfo);

	return S_OK;
}


HRESULT CWaveDataManager::Redo()
{
	ASSERT(m_pUndoManager);
	if(m_pUndoManager == NULL)
	{
		return E_FAIL;
	}

    LPWAVEFORMATEX pwfxCompression = NULL;

	// Get the new delta list
	CPtrList* pNewList = NULL;
	if(FAILED(m_pUndoManager->Redo(&pNewList, &m_HeaderInfo, &pwfxCompression)))
	{
		return E_FAIL;
	}

	CWaveNode* pWaveNode = m_pWave->GetNode();
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
	{
		return E_FAIL;
	}

	EnterCriticalSection(&pWaveNode->m_CriticalSection);
	CPtrList* pOldDeltas = m_plstDeltas;
	if(FAILED(CloneDeltaList(pNewList, &m_plstDeltas)))
	{
		LeaveCriticalSection(&pWaveNode->m_CriticalSection);
		return E_FAIL;
	}

	LeaveCriticalSection(&pWaveNode->m_CriticalSection);

	// Delete the old list
	while(!pOldDeltas->IsEmpty())
	{
		delete pOldDeltas->RemoveHead();
	}
	delete pOldDeltas;


    if(m_HeaderInfo.m_bCompressed)
    {
        if(FAILED(m_pWave->Compress(pwfxCompression, true)))
        {
            return E_FAIL;
        }

        // Adjust the header info for the newly compressed length
        m_HeaderInfo.m_dwDataSize = m_pWave->GetDataSize();
	    m_HeaderInfo.m_dwWaveLength = m_pWave->GetWaveLength();
    }

	// Get the new wavelength for the wave
	m_pWave->UpdateOnUndo(m_HeaderInfo);

	return S_OK;
}


HRESULT CWaveDataManager::PopUndoState()
{
    ASSERT(m_pUndoManager);
    if(m_pUndoManager == NULL)
    {
        return E_UNEXPECTED;
    }

    return m_pUndoManager->PopUndoState();
}


CString CWaveDataManager::GetUndoMenuText(bool bRedo)
{
    ASSERT(m_pUndoManager);
    if(m_pUndoManager == NULL)
    {
        return "";
    }

	if(!bRedo)
	{
		return m_pUndoManager->GetCurrentStateName();
	}

	return m_pUndoManager->GetRedoStateName();
}

HRESULT CWaveDataManager::CloneDeltaList(CPtrList* pDeltaList, CPtrList** ppClonedList)
{
	ASSERT(pDeltaList);
	if(pDeltaList == NULL)
	{
		return E_POINTER;
	}

	CPtrList* pClonedList = new CPtrList;

	POSITION position = pDeltaList->GetHeadPosition();
	while(position)
	{
		CWaveDelta* pDelta = (CWaveDelta*) pDeltaList->GetNext(position);
		ASSERT(pDelta);
		DWORD dwActualPos = pDelta->GetActualPosition();
		DWORD dwStartSample = pDelta->GetStartSample();
		DWORD dwLength = pDelta->GetLength();
		DWORD dwFileOffset = pDelta->GetFileOffset();
		BOOL bEditDelta = pDelta->IsEditDelta();

		CWaveDelta* pClonedDelta = new CWaveDelta(dwActualPos, dwStartSample, dwLength, dwFileOffset, bEditDelta);
		pClonedList->AddTail(pClonedDelta);
	}

	*ppClonedList = pClonedList;
	return S_OK;
}


DWORD CWaveDataManager::GetWavelength()
{
	DWORD dwLength = 0;
	POSITION position = m_plstDeltas->GetHeadPosition();
	while(position)
	{
		CWaveDelta* pDelta = (CWaveDelta*) m_plstDeltas->GetNext(position);
		ASSERT(pDelta);
		dwLength += pDelta->GetLength();
	}

	return dwLength;
}

void CWaveDataManager::UpdateHeaderInfo()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	CWaveNode* pWaveNode = m_pWave->GetNode();
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
		return;

	m_HeaderInfo.m_rWSMP = m_pWave->GetWSMPL();
	m_HeaderInfo.m_rRSMP = m_pWave->GetRSMPL();
	m_HeaderInfo.m_rWLOOP = m_pWave->GetWLOOP();
	m_HeaderInfo.m_rRLOOP = m_pWave->GetRLOOP();
	m_HeaderInfo.m_dwDataSize = m_pWave->GetDataSize();
	m_HeaderInfo.m_dwWaveLength = m_pWave->GetWaveLength();
	m_HeaderInfo.m_bCompressed = m_pWave->m_bCompressed;
	m_HeaderInfo.m_bStreaming = m_pWave->IsStreamingWave();
	m_HeaderInfo.m_bNoPreroll = m_pWave->IsNoPrerollWave();
	m_HeaderInfo.m_dwReadAheadTime = m_pWave->GetReadAheadTime();
	m_HeaderInfo.m_dwDecompressedStart = m_pWave->GetDwDecompressedStart(false);
	m_HeaderInfo.m_dwSamplerate = m_pWave->m_rWaveformat.nSamplesPerSec;

	if(SUCCEEDED(m_pWave->UpdateHeaderStream()))
	{
		if(m_bInInit == FALSE)
			pWaveNode->ReloadDirectSoundWave();
	}
}

HRESULT CWaveDataManager::GetSourceData(DWORD dwStartOffset, DWORD dwLength, BYTE** ppbData, DWORD& cbRead)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pSourceFileHandler);
	if(m_pSourceFileHandler == NULL)
	{
		return E_FAIL;
	}

	if(dwLength == 0)
	{
		cbRead = 0;
		return S_OK;
	}

	BYTE* pbData = NULL;
	if(FAILED(m_pWave->SafeAllocBuffer(&pbData, dwLength)))
	{
		return E_OUTOFMEMORY;
	}

	DWORD dwDataOffset = m_pSourceFileHandler->GetDataOffset();
	DWORD cb = 0;
	if(FAILED(m_pSourceFileHandler->ReadData(dwDataOffset + dwStartOffset, dwLength, pbData, cbRead)))
	{
		delete[] pbData;
		cbRead = 0;
		return E_FAIL;
	}

	*ppbData = pbData;

	return S_OK;
}

HRESULT CWaveDataManager::CreateUniqueTempFile(CString sPrefix, CString sExtension, CString& sTempFileName)
{
	if(sExtension.IsEmpty())
	{
		sExtension = "tmp";
	}

	CString sFileExt = "." + sExtension;

	char szTempPath[MAX_PATH];
	DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
	CString sTempPath = szTempPath;	

	GUID guidFileName;
	if(SUCCEEDED(::CoCreateGuid(&guidFileName)))
	{
		LPOLESTR psz;
		if( SUCCEEDED( ::StringFromIID(guidFileName, &psz) ) )
		{
			TCHAR szGuid[100];
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );
			
			sTempFileName = sTempPath + sPrefix + szGuid + sFileExt;

			CFile tempFile;
			try
			{
				tempFile.Open(sTempFileName, CFile::modeCreate);
				tempFile.Close();
			}
			catch(CFileException e)
			{
				return E_FAIL;
			}

			return S_OK;
		}
	}

	return E_FAIL;
}


HRESULT CWaveDataManager::GetAllUncompressedData(BYTE** ppbData, DWORD* pdwSize)
{
    ASSERT(m_pWave);
    if(m_pWave == NULL)
    {
        return E_UNEXPECTED;
    }

    ASSERT(pdwSize);
    if(pdwSize == NULL)
    {
        return E_POINTER;
    }

    BOOL bTrueStereo = m_pWave->IsTrueStereo();
    int nSampleSize = m_pWave->m_OriginalWavObj.WaveFormatEx.wBitsPerSample;
    int nChannels = bTrueStereo ? 2 : 1;

    DWORD dwSize = m_pWave->m_OriginalWavObj.dwDataSize;
    DWORD dwLength = nSampleSize == 8 ? dwSize : dwSize >> 1;
    dwLength /= nChannels;

    if(FAILED(m_pWave->SafeAllocBuffer(ppbData, dwSize)))
    {
        return E_OUTOFMEMORY;
    }

    DWORD cbRead = 0;
    if(FAILED(GetData(0, dwLength, *ppbData, cbRead, true)))
    {
        delete[] *ppbData;
        return E_FAIL;
    }

    *pdwSize = cbRead;
    return S_OK;
}


HRESULT CWaveDataManager::UpdateUncompressedDeltas()
{
    if(m_plstUncompressedDeltas)
    {
        while(m_plstUncompressedDeltas->IsEmpty() == FALSE)
        {
            delete (CWaveDelta*)m_plstUncompressedDeltas->RemoveHead();
        }

        delete m_plstUncompressedDeltas;
        m_plstUncompressedDeltas = NULL;
    }

    CPtrList* pClonedList = NULL;
	if(FAILED(CloneDeltaList(m_plstDeltas, &pClonedList)))
	{
		return E_FAIL;
	}

    m_plstUncompressedDeltas = pClonedList;

    return S_OK;
}   
