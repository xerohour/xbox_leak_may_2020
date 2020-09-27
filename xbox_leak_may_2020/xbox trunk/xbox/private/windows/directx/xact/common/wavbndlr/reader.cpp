/***************************************************************************
 *
 *  Copyright (C) 11/7/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       reader.cpp
 *  Content:    Wave Bundler file reader.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/7/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wavbndli.h"


/****************************************************************************
 *
 *  CWaveBankReader
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankReader::CWaveBankReader"

CWaveBankReader::CWaveBankReader
(
    void
)
{
    DPF_ENTER();

    m_pvBaseAddress = NULL;
    m_dwBankSize = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CWaveBankReader
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankReader::~CWaveBankReader"

CWaveBankReader::~CWaveBankReader
(
    void
)
{
    DPF_ENTER();

    Flush();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Opens a wave bank.
 *
 *  Arguments:
 *      LPCSTR [in]: wave bank file path.
 *      LPWAVEBANKSECTIONDATA [out]: wave bank data.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankReader::Open"

HRESULT
CWaveBankReader::Open
(
    LPCSTR                  pszBankFile
)
{
    CStdFileStream          BankFile;
    LPCWAVEBANKHEADER       pHeader;
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Open the file
    //

    hr = BankFile.Open(pszBankFile, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN);

    //
    // Read the whole damn thing
    //

    if(SUCCEEDED(hr))
    {
        hr = BankFile.GetLength(&m_dwBankSize);
    }

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_pvBaseAddress = MEMALLOC(BYTE, m_dwBankSize));
    }

    if(SUCCEEDED(hr))
    {
        hr = BankFile.Read(m_pvBaseAddress, m_dwBankSize);
    }

    //
    // Validate the header
    //

    if(SUCCEEDED(hr))
    {
        pHeader = (LPCWAVEBANKHEADER)m_pvBaseAddress;

        if(WAVEBANKHEADER_SIGNATURE != pHeader->dwSignature)
        {
            DPF_ERROR("Invalid signature");
            hr = E_FAIL;
        }

        if(WAVEBANKHEADER_VERSION != pHeader->dwVersion)
        {
            DPF_ERROR("Invalid version");
            hr = E_FAIL;
        }
    }

    //
    // If anything went wrong, free everything
    //

    if(FAILED(hr))
    {
        Flush();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Frees wave bank data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankReader::Flush"

void
CWaveBankReader::Flush
(
    void
)
{
    DPF_ENTER();

    MEMFREE(m_pvBaseAddress);

    m_dwBankSize = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetSectionData
 *
 *  Description:
 *      Gets wave bank section data.
 *
 *  Arguments:
 *      LPWAVEBANKSECTIONDATA [out]: wave bank data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankReader::GetSectionData"

void
CWaveBankReader::GetSectionData
(
    LPWAVEBANKSECTIONDATA   pSectionData
)
{
    DPF_ENTER();

    ASSERT(m_pvBaseAddress);
    ASSERT(m_dwBankSize);

    pSectionData->pHeader = (LPWAVEBANKHEADER)m_pvBaseAddress;
    pSectionData->paMetaData = (LPWAVEBANKENTRY)(pSectionData->pHeader + 1);
    pSectionData->pvData = &pSectionData->paMetaData[pSectionData->pHeader->dwEntryCount];
    pSectionData->dwDataSize = m_dwBankSize - sizeof(*pSectionData->pHeader) - (sizeof(pSectionData->paMetaData[0]) * pSectionData->pHeader->dwEntryCount);

    DPF_LEAVE_VOID();
}


