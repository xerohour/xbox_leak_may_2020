#include "globals.h"
#include "cwavefile.h"

#define READBUFFERSIZE  131072 //must be divisible by 2

//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT Error
(
    LPSTR                   szFormat,
    ...
)
{
    va_list va;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    printf("%s\n",szBuffer);
    return S_OK;
};


//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT Log
(
    LPSTR                   szFormat,
    ...
)
{
    va_list va;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    printf("%s\n",szBuffer);
    return S_OK;
};


//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HANDLE WaveOpenFile
(
    IN  LPCSTR          szFileName,
    OUT WAVEFORMATEX**  ppWaveFormatEx,
    OUT PULONG          pcbData
)
{
    HMMIO	        hmmio;              // mmio style file handle
    MMCKINFO        mmckinfoParent;	    // parent chunk info struct
    MMCKINFO        mmckinfoSubchunk;	// sub chunk

    HANDLE          hFile = INVALID_HANDLE_VALUE;
    ULONG	        cbFormat;
    LONG            lDataPos;

    // open the file
    if(!(hmmio = mmioOpen((LPSTR)szFileName, NULL, MMIO_READ | MMIO_ALLOCBUF))) 
    {
        return INVALID_HANDLE_VALUE;
    }

    // verify that the file is of type "WAVE" 
    mmckinfoParent.fccType = MAKEFOURCC('W', 'A', 'V', 'E');
    if(mmioDescend(hmmio, (LPMMCKINFO)&mmckinfoParent, NULL, MMIO_FINDRIFF))
    {
        // not a "WAVE" file!!
        Error("WaveOpenFile:  Missing 'WAVE' chunk.This does not appear to be a wave file.", "Error opening wave file!");
        goto _error_;
    }

    // find the "fmt " chunk (must be a subchunk of the "WAVE" chunk)
    mmckinfoSubchunk.ckid = MAKEFOURCC('f', 'm', 't', ' ');
    if(mmioDescend(hmmio, (LPMMCKINFO)&mmckinfoSubchunk, (LPMMCKINFO)&mmckinfoParent, MMIO_FINDCHUNK))
    {
        // file has no "fmt " chunk
        Error("WaveOpenFile:  Missing 'fmt ' chunk", "Error opening wave file!");
        goto _error_;
    }

    // get the size of the "fmt " chunk
    cbFormat = mmckinfoSubchunk.cksize;

    // allocate waveformat
    //*ppWaveFormatEx = (LPWAVEFORMATEX)LocalAlloc(LPTR, cbFormat);
    
    //Cheap hack.
    *ppWaveFormatEx = (LPWAVEFORMATEX)LocalAlloc(LPTR, sizeof(WAVEFORMATEX));
    if(!*ppWaveFormatEx)
    {
        // failed to allocate
        Error("WaveOpenFile:  Failed to allocate memory", "Error opening wave file!");
        goto _error_;
    }

    // read the "fmt " chunk
    if(mmioRead(hmmio, (HPSTR)*ppWaveFormatEx, cbFormat) != (LONG)cbFormat)
    {
        // failed to read the "fmt " chunk
        Error("WaveOpenFile:  Error reading 'fmt ' chunk", "Error opening wave file!");
        goto _error_;
    }

    // ascend out o' the "fmt " chunk
    mmioAscend(hmmio, &mmckinfoSubchunk, 0);

    // and find the data subchunk.  (current file pos should be at the beginning of the 
    // data chunk, but when you assume...)
    mmckinfoSubchunk.ckid = MAKEFOURCC('d', 'a', 't', 'a');
    if(mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK))
    {
        // wave file doesn't appear to have a data chunk
        Error("WaveOpenFile:  Missing 'data' chunk", "Error opening wave file!");
        goto _error_;
    }

    // get the size of the "data" subchunk
    *pcbData = mmckinfoSubchunk.cksize;
    if(0L == *pcbData)
    {
        // the data chunk contains no data
        Error("WaveOpenFile:  Error reading 'data' chunk", "Error opening wave file!");
        goto _error_;
    }

    // remember the data 
    lDataPos = mmioSeek(hmmio, 0, SEEK_CUR);
    mmioClose(hmmio, 0);

    //
    // now we are done with the mmio functions.  Reopen the file and seek to the data chunk using normal
    // file io routines
    //

    __try
    {
        hFile = 
            CreateFile
            ( 
                szFileName, 
                GENERIC_READ, 
                FILE_SHARE_READ,
                NULL, 
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
    }    
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        Error("HANDLING EXCEPTION THROWN BY CreateFile!", "Error opening wave file!");
        goto _error_;
    }

    if (INVALID_HANDLE_VALUE  == hFile)
    {
        goto _error_;
    }

    SetFilePointer
    (
        hFile,
        lDataPos,  
        NULL, 
        FILE_BEGIN
    ); 

    return hFile;
    
_error_:
    CloseHandle(hFile);
    if(hmmio)
    {
        mmioClose(hmmio, 0);
    }
    return INVALID_HANDLE_VALUE;
}




// -----------------------------------------------------------------------------------------
BOOL
WaveSaveFile
(
    IN  LPCSTR          szFileName,
    OUT WAVEFORMATEX*   pWaveFormatEx,
    IN  PVOID           pvData,
    IN  ULONG           cbData
)
{
    HANDLE          hFile = INVALID_HANDLE_VALUE;
    ULONG           cbWritten;
	HRSRC           hrsrc = (HRSRC)INVALID_HANDLE_VALUE;
    void*           pv = NULL;

    //
    // now we are done with the mmio functions.  Reopen the file and seek to the data chunk using normal
    // file io routines
    //

    __try
    {
        hFile = 
            CreateFile
            ( 
                szFileName, 
                GENERIC_WRITE, 
                0,
                NULL, 
                CREATE_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
    }    
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        Error("HANDLING EXCEPTION THROWN BY CreateFile!", "Error opening wave file!");
        goto _error_;
    }

    if (INVALID_HANDLE_VALUE == hFile)
        goto _error_;

	// 
	// write file header
	//

    // standard wave goo
    struct
    {
        DWORD   fourccData;
        DWORD   dwDataLength;
    } DataHeader;

    DataHeader.fourccData         = MAKEFOURCC('d','a','t','a');
    DataHeader.dwDataLength       = cbData;

    struct 
    {
        DWORD       dwRiff;
        DWORD       dwFileSize;
        DWORD       dwWave;
        DWORD       dwFormat;
        DWORD       dwFormatLength;
    } FileHeader;

    FileHeader.dwRiff             = MAKEFOURCC('R','I','F','F');
    FileHeader.dwWave             = MAKEFOURCC('W','A','V','E');
    FileHeader.dwFormat           = MAKEFOURCC('f','m','t',' ');
    FileHeader.dwFormatLength     = sizeof(WAVEFORMATEX) + pWaveFormatEx->cbSize;

    // File size is size of the whole file except for the dwRiff and dwFileSize fields.

    FileHeader.dwFileSize         = sizeof(FileHeader) + FileHeader.dwFormatLength + sizeof(DataHeader) + cbData - 8;

    if(!WriteFile(hFile, &FileHeader, sizeof(FileHeader), &cbWritten, NULL))
        goto _error_;

    // wave format
    if(!WriteFile(hFile, pWaveFormatEx, sizeof(WAVEFORMATEX) + pWaveFormatEx->cbSize, &cbWritten, NULL))
        goto _error_;

    // data chuck
    if(!WriteFile(hFile, &DataHeader, sizeof(DataHeader), &cbWritten, NULL))
        goto _error_;

    //
    // Write wave data
    //
    if(!WriteFile(hFile, pvData, cbData, &cbWritten, NULL))
        goto _error_;

    CloseHandle(hFile);
    return TRUE;
    
_error_:
    Error("Error Writing File", "Error!");
    CloseHandle(hFile);
    return FALSE;
}



// -----------------------------------------------------------------------------------------
HANDLE
WaveOpenFileForSave
(
    IN  LPCSTR          szFileName,
    OUT WAVEFORMATEX*   pWaveFormatEx,
    DWORD               cbData
)
{
    HANDLE          hFile = INVALID_HANDLE_VALUE;
    ULONG           cbWritten;
	HRSRC           hrsrc = (HRSRC)INVALID_HANDLE_VALUE;
    void*           pv = NULL;

    //
    // now we are done with the mmio functions.  Reopen the file and seek to the data chunk using normal
    // file io routines
    //

    __try
    {
        hFile = 
            CreateFile
            ( 
                szFileName, 
                GENERIC_WRITE, 
                0,
                NULL, 
                CREATE_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
    }    
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        Error("HANDLING EXCEPTION THROWN BY CreateFile!", "Error opening wave file!");
        goto _error_;
    }

    if (INVALID_HANDLE_VALUE == hFile)
        goto _error_;

	// 
	// write file header
	//

    // standard wave goo
    struct
    {
        DWORD   fourccData;
        DWORD   dwDataLength;
    } DataHeader;

    DataHeader.fourccData         = MAKEFOURCC('d','a','t','a');
    DataHeader.dwDataLength       = cbData;

    struct 
    {
        DWORD       dwRiff;
        DWORD       dwFileSize;
        DWORD       dwWave;
        DWORD       dwFormat;
        DWORD       dwFormatLength;
    } FileHeader;

    FileHeader.dwRiff             = MAKEFOURCC('R','I','F','F');
    FileHeader.dwWave             = MAKEFOURCC('W','A','V','E');
    FileHeader.dwFormat           = MAKEFOURCC('f','m','t',' ');
    FileHeader.dwFormatLength     = sizeof(WAVEFORMATEX) + pWaveFormatEx->cbSize;

    // File size is size of the whole file except for the dwRiff and dwFileSize fields.

    FileHeader.dwFileSize         = sizeof(FileHeader) + FileHeader.dwFormatLength + sizeof(DataHeader) + cbData - 8;

    if(!WriteFile(hFile, &FileHeader, sizeof(FileHeader), &cbWritten, NULL))
        goto _error_;

    // wave format
    if(!WriteFile(hFile, pWaveFormatEx, sizeof(WAVEFORMATEX) + pWaveFormatEx->cbSize, &cbWritten, NULL))
        goto _error_;

    // data chuck
    if(!WriteFile(hFile, &DataHeader, sizeof(DataHeader), &cbWritten, NULL))
        goto _error_;

    return hFile;
    
_error_:
    Error("Error Writing File", "Error!");
    CloseHandle(hFile);
    return INVALID_HANDLE_VALUE;
}


/********************************************************************************
********************************************************************************/
BOOL YesOrNo(void)
{
    CHAR c = 'a';

    do
    {
        //c = (char)_getch();
        scanf("%c", &c);
    }
    while (tolower(c) != 'n' && tolower(c) != 'y');

    if (tolower(c) != 'y')
        return FALSE;
    else
        return TRUE;

};


/********************************************************************************
Hack together a CWaveFile object in 5.1 and write it out to disk.
********************************************************************************/
#define MAXCHANNELS 10
HRESULT Merge(CWaveFile WaveFile[], LPSTR szOutputFile, DWORD dwNumChannels)
{
    CWaveFile OutputFile;
    WAVEFORMATEX wfxIn = {0};
    WAVEFORMATEX wfxOut = {0};
    DWORD i = 0;
    DWORD j = 0;
    HRESULT hr = S_OK;
    DWORD dwTemp = 0;
    DWORD dwNextPercentGoal= 0;
    
    DWORD cbDataLongest = 0;        //Data length of longest file.
    DWORD cbSamplesRemaining = 0;   //Samples remaining to write to output file.
    DWORD cbSamplesMix       = 0;   //How many samples to mix this iteration.
    DWORD cbSamplesPerBuffer = 0;
    DWORD dwSampleSize       = 0;
    bool bEightBit = false;
    LPVOID *pvRead = NULL;
    LPVOID pvWrite   = NULL;

    if (READBUFFERSIZE % 2)
    {
        //Log("Error: specified buffer size %u is not divisible by NUMCHANNELS * 2 (%u)", READBUFFERSIZE, NUMCHANNELS * 2);
        Log("Error: specified buffer size %u is not divisible by 2", READBUFFERSIZE);
        return E_FAIL;
    }

    //Allocate an array of void ptrs for the channels.
    pvRead = new LPVOID[dwNumChannels];
    if (!pvRead)
    {
        Log("Error: Couldn't allocate a measly %u bytes; exitting", dwNumChannels * sizeof(LPVOID));
        return E_FAIL;
    }

    ZeroMemory(pvRead, dwNumChannels * sizeof(LPVOID));
    //Check to see if the file exists.
    if (_access(szOutputFile, 00) == 0)
    {

        //Check to see if the file is writeable.
        if (_access(szOutputFile, 02) != 0)
        {
            Log("Error: Cannot write to %s.", szOutputFile);
            return E_FAIL;
        }

        printf("%s exists, overwrite? (y/n) ", szOutputFile);
        if (!YesOrNo())
            return E_FAIL;
    }


    
    //Get the length of the longest file; that will determine the length of our output file.
    for (i=0; i<dwNumChannels; i++)
    {   
        WaveFile[i].GetLength(&dwTemp);
        if (dwTemp > cbDataLongest)
            cbDataLongest = dwTemp;
    }

    //Allocate the read buffers
    for (i=0; i<dwNumChannels; i++)
    {
        pvRead[i] = LocalAlloc(LPTR, READBUFFERSIZE);
        if (!pvRead[i])
        {
            Log("Out of memory, failed to allocate %u bytes", READBUFFERSIZE);
            hr = E_FAIL;
        }
    }

    //our output buffer is NUMCHANNELS times the size of our regular one.
    if (SUCCEEDED(hr))
    {
        pvWrite = LocalAlloc(LPTR, READBUFFERSIZE * dwNumChannels);
        if (!pvWrite)
        {
            Log("Out of memory, failed to allocate %u bytes", READBUFFERSIZE * dwNumChannels);
            hr = E_FAIL;
        }
    }
    
    //Set the WFX to the proper format.  All the wave files (even when silenced) have the same wave format.
    CHECKRUN(WaveFile[0].GetWaveFormatEx(&wfxIn));

    if (SUCCEEDED(hr))
    {
        wfxOut.wFormatTag = wfxIn.wFormatTag;
        wfxOut.nChannels  = (unsigned short)dwNumChannels;
        wfxOut.nSamplesPerSec = wfxIn.nSamplesPerSec;
        wfxOut.nAvgBytesPerSec = wfxIn.nAvgBytesPerSec * dwNumChannels;
        wfxOut.nBlockAlign = wfxIn.nBlockAlign * (unsigned short)dwNumChannels;
        wfxOut.wBitsPerSample = wfxIn.wBitsPerSample;
        wfxOut.cbSize = 0;

        //Samples are half as many if we're 16-bit.
        if (8 == wfxIn.wBitsPerSample)
        {
            bEightBit = true;
            dwSampleSize = 1;
        }
        else if (16 == wfxIn.wBitsPerSample)
        {
            bEightBit = false;
            dwSampleSize = 2;
        }
        else
        {
            Log("Error: Data is not 8 or 16-bit!");
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr))
    {
        //Open this file for writing.
        CHECKRUN(OutputFile.OpenForWrite(szOutputFile, wfxOut, cbDataLongest * dwNumChannels));
        if (SUCCEEDED(hr))
        {
            cbSamplesPerBuffer = READBUFFERSIZE / dwSampleSize;
            cbSamplesRemaining = cbDataLongest / dwSampleSize;
            //Initialize the output:
            printf("% Done: 0%%");
        }

        while (cbSamplesRemaining && SUCCEEDED(hr))
        {
                //Zero out the destination buffer.
                memset(pvWrite, 0, READBUFFERSIZE);

                //How many samples are we mixing?  Take the minimum of what's remaining and the maximum size we can take.
                cbSamplesMix = (cbSamplesRemaining > cbSamplesPerBuffer) ? cbSamplesPerBuffer : cbSamplesRemaining;

                for (j=0; j<dwNumChannels && SUCCEEDED(hr); j++)
                {
                    DWORD cbSamplesRead = 0;

                    //Simply read in the data for this channel.
                    memset(pvRead[j], 0xCC, cbSamplesMix * dwSampleSize);  //if we see a line of 0xCC values when viewing the wave file we'll know something's wrong.
                    CHECKRUN(WaveFile[j].ReadData(pvRead[j], READBUFFERSIZE, &cbSamplesRead));

                    cbSamplesRead /= dwSampleSize;

                    for (i=0; i<cbSamplesMix && SUCCEEDED(hr); i++)
                    {
                        
                        //Again, note that silence for 8-bit buffers is 0x80, while silence for 16-bit buffers is 0x0000                        
                        if (bEightBit)
                        {
                            //Set up our in and out pointers.
                            BYTE *pbOut = (BYTE *)pvWrite;
                            const BYTE *pbIn = (BYTE *)pvRead[j];

                            if (i < cbSamplesRead)
                                pbOut[i * dwNumChannels + j] = pbIn[i];
                            else
                                pbOut[i * dwNumChannels + j] = 0x80;
                        }

                        else
                        {
                            //Set up our in and out pointers.
                            WORD *pwOut = (WORD *)pvWrite;
                            const WORD *pwIn = (WORD *)pvRead[j];

                            if (i < cbSamplesRead)
                                pwOut[i * dwNumChannels + j] = pwIn[i];
                            else
                                pwOut[i * dwNumChannels + j] = 0x0000;
                        }
                    }
                }


                //Write the mixed buffer to disk.
                DWORD cbWritten;
                CHECKRUN(OutputFile.WriteData(pvWrite, cbSamplesMix * dwSampleSize * dwNumChannels, &cbWritten));
                if (cbWritten != cbSamplesMix * dwSampleSize * dwNumChannels)
                { 
                    Log("Error: Only wrote %u bytes instead of %u", cbWritten, cbSamplesMix * dwSampleSize * dwNumChannels);
                    hr = E_FAIL;
                }

                cbSamplesRemaining -= cbSamplesMix;

                //remaining samples
                while (  FLOAT((cbDataLongest / dwSampleSize) - cbSamplesRemaining) * 100.f / FLOAT(cbDataLongest / dwSampleSize) >= FLOAT(dwNextPercentGoal + 10))
                {
                    dwNextPercentGoal += 10;
                    printf("...%d%%", dwNextPercentGoal);
                }
        }
    }
    
    //Carriage return.
    Log("");



    //Close all our files and free our buffers.
    delete []pvRead;        
    CHECKRUN(OutputFile.Close());
    for (i=0; i<dwNumChannels && SUCCEEDED(hr); i++)
    {
        CHECKRUN(WaveFile[i].Close());
        if (pvRead[i])
            LocalFree(pvRead[i]);
    }
    if (pvWrite)
        LocalFree(pvWrite);

return hr;
}