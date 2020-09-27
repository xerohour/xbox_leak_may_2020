// filegen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int __cdecl main(int argc, char* argv[])
{
    CXACTFileGenerator *pFileGen = new CXACTFileGenerator("XACT BINARY FILE GENERATOR");
    int err;

    if (pFileGen == NULL) {
        return -1;
    }

    err = pFileGen->ParseCommandLine(argc,argv);
    if (err != ERROR_SUCCESS) {
		pFileGen->DebugPrint(DBGLVL_ERROR,"Failed parsing input file");
        goto mainExit;
    }

    err = pFileGen->ParseIniFile();
    if (err != ERROR_SUCCESS) {
		pFileGen->DebugPrint(DBGLVL_ERROR,"Failed parsing input file");
        goto mainExit;
    }

    err = pFileGen->CreateBinaryImage();
    if (err != ERROR_SUCCESS) {
		pFileGen->DebugPrint(DBGLVL_ERROR,"Failed parsing input file");
        goto mainExit;
    }

mainExit:
    delete pFileGen;

	return 0;
}


CXACTFileGenerator::CXACTFileGenerator(char* AppName)
{
    memcpy(m_szAppName,AppName,sizeof(m_szAppName));

    m_hInputFile = NULL;
    m_hOutputFile = NULL;
    m_dwDebugLevel = DBGLVL_WARN;

    memset(m_szIniFileName,0,sizeof(m_szIniFileName));
    memset(m_szOutputFileName,0,sizeof(m_szOutputFileName));

    m_hInputFile = INVALID_HANDLE_VALUE;
    m_hOutputFile = INVALID_HANDLE_VALUE;

    m_pFileHeader = NULL;
    m_pSoundTable  = NULL;
    m_pCueTable = NULL;
	m_pXACTSoundTable = NULL;

}

CXACTFileGenerator::~CXACTFileGenerator()
{
    if (m_hInputFile != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hInputFile);
    }

    if (m_hOutputFile != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hOutputFile);
    }

    if (m_pCueTable) {
        delete [] m_pCueTable;
        m_pCueTable = NULL;
    }

    if (m_pSoundTable) {
        delete [] m_pSoundTable;
        m_pSoundTable = NULL;
    }

    if (m_pXACTSoundTable) {
        delete [] m_pXACTSoundTable;
        m_pXACTSoundTable = NULL;
    }


}

// ****************************************************************************
// binary file generation
// ****************************************************************************

int CXACTFileGenerator::CreateBinaryImage()
{
	int err = ERROR_SUCCESS;
	DWORD dwSize, dwBytesWritten, i,j;
	BOOL bResult;

	DWORD dw3dParamOffset = 0, dwTotalTracks = 0, dwTotalWaveBankEntries = 0;
	DWORD dwTrackTableOffset = 0;
	DWORD dwEventDataBaseOffset = 0;
	DWORD dwEventDataOffset = 0;
	DWORD dwTrackTableBaseOffset = 0;
	DWORD dwWaveBankTableBaseOffset = 0;
	DWORD dwOffset = 0;

    m_hOutputFile = CreateFile(
        (const char *)m_szOutputFileName,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL);

    if (m_hOutputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        Print("\n Failed to open the output file.Error 0x%x\n", err);
        return err;

    }

	dwOffset = 0;

	dwSize = SetFilePointer(m_hOutputFile,
		dwOffset,
		NULL,
		FILE_BEGIN);

	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	//
	// setup file header
	//

	m_pFileHeader->dwSignature = (DWORD) XACT_SOUNDBANK_HEADER_SIGNATURE;
	m_pFileHeader->dwVersion = XACT_SOUNDBANK_HEADER_VERSION;
	m_pFileHeader->dwFlags = 0;

    //
	// first write out the header for the soundbank file
	//

	bResult = WriteFile(m_hOutputFile,
		m_pFileHeader,
		sizeof(*m_pFileHeader),
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
		goto errExit;
		
	}

	//
	// advance offset
	//

	dwOffset += dwBytesWritten;

	//
	// now write the cue entry table
	//

	bResult = WriteFile(m_hOutputFile,
		m_pCueTable,
		sizeof(XACT_SOUNDBANK_CUE_ENTRY)*m_pFileHeader->dwCueEntryCount,
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
		goto errExit;
		
	}

    //
	// advance offset
	//

	dwOffset += dwBytesWritten;

	//
	// calculate the file offset for the variable length data (tracks, wavebank table, 3d params)
	// 3d param data follows the sound entry table. 
	//

	dw3dParamOffset = dwOffset + sizeof(XACT_SOUNDBANK_SOUND_ENTRY)*m_pFileHeader->dwSoundEntryCount;

	//
	// set file pointer to 3d param data offset
	//

	dwSize = SetFilePointer(m_hOutputFile,
		dw3dParamOffset,
		NULL,
		FILE_BEGIN);

	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	//
	// collect total object information
	//

	dwTotalTracks = 0;
	dwTotalWaveBankEntries = 0;

	for (i=0;i<m_pFileHeader->dwSoundEntryCount;i++)
	{
		//
		// every sound has 3d param data in this implementation
		//
	
		dwTotalTracks += m_pSoundTable[i].m_pSoundEntry->wTrackCount;
		dwTotalWaveBankEntries += m_pSoundTable[i].m_pSoundEntry->wWaveBankCount;

		//
		// we have enough information to write out the 3d param data for each sound
		//

		if (m_pSoundTable[i].m_pSoundEntry->dwFlags & XACT_FLAG_SOUND_3D) {

			m_pSoundTable[i].m_pSoundEntry->dw3DParametersOffset = dw3dParamOffset;
     		dw3dParamOffset += sizeof(XACT_SOUNDBANK_SOUND_3D_PARAMETERS);

			bResult = WriteFile(m_hOutputFile,
				m_pSoundTable[i].m_pSound3dData,
				sizeof(XACT_SOUNDBANK_SOUND_3D_PARAMETERS),
				&dwBytesWritten,
				0);
			
			if (!bResult) {
				
				err = GetLastError();
				DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
				goto errExit;
				
			}

		}

	}

	//
	// now calculate offsets for wavebank tables
	//

	dwWaveBankTableBaseOffset = dw3dParamOffset;

	//
	// calc base offset for track table
	//

	dwTrackTableBaseOffset = dwWaveBankTableBaseOffset + sizeof(XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY)*dwTotalWaveBankEntries;

	//
	// calc base offset for track event data
	//

	dwEventDataBaseOffset = dwTrackTableBaseOffset + sizeof(XACT_SOUNDBANK_TRACK_ENTRY)*dwTotalTracks;
	dwEventDataOffset = dwEventDataBaseOffset;

	dwSize = SetFilePointer(m_hOutputFile,
		dwEventDataOffset,
		NULL,
		FILE_BEGIN);
	
	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	//
	// now fill in the offsets for each sound entry
	//

	for (i=0;i<m_pFileHeader->dwSoundEntryCount;i++)
	{
		//
		// every sound has 3d param data in this implementation
		//

		m_pSoundTable[i].m_pSoundEntry->dwTrackTableOffset = dwTrackTableBaseOffset + i*sizeof(XACT_SOUNDBANK_TRACK_ENTRY);
		m_pSoundTable[i].m_pSoundEntry->dwWaveBankTableOffset = dwWaveBankTableBaseOffset + i*sizeof(XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY);

		//
		// calculate the event data offset for each track in the sound
		//
		
		for (j=0;j<m_pSoundTable[i].m_pSoundEntry->wTrackCount;j++)
		{
			CXACTTrackData *pTrackData = &m_pSoundTable[i].m_pTrackTable[j];
			pTrackData->m_pTrackEntry->dwEventDataOffset = dwEventDataOffset;
			dwEventDataOffset += pTrackData->m_dwEventDataSize;

			//
			// write out all the event data to file now that we know their offset
			//

			bResult = WriteFile(m_hOutputFile,
				pTrackData->m_pEventBuffer,
				pTrackData->m_dwEventDataSize,
				&dwBytesWritten,
				0);
			
			if (!bResult) {
				
				err = GetLastError();
				DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
				goto errExit;
				
			}

		}

	}


	//
	// we are now ready to write everything out.
	// start with the sound table
	// reset file pointer to sound entry table base
	//

    dwSize = SetFilePointer(m_hOutputFile,
		dwOffset,
		NULL,
		FILE_BEGIN);
	
	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	bResult = WriteFile(m_hOutputFile,
		m_pXACTSoundTable,
		sizeof(XACT_SOUNDBANK_SOUND_ENTRY)*m_pFileHeader->dwSoundEntryCount,
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
		goto errExit;
		
	}

    //
	// advance offset to wave bank table. 3d params have already been written
	//

	dwOffset = dwWaveBankTableBaseOffset;

    dwSize = SetFilePointer(m_hOutputFile,
		dwOffset,
		NULL,
		FILE_BEGIN);
	
	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	for (i=0;i<m_pFileHeader->dwSoundEntryCount;i++)
	{
		
		
		bResult = WriteFile(m_hOutputFile,
			m_pSoundTable[i].m_pWaveBankTable,
			sizeof(XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY)*m_pSoundTable[i].m_pSoundEntry->wWaveBankCount,
			&dwBytesWritten,
			0);
		
		if (!bResult) {
			
			err = GetLastError();
			DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
			goto errExit;
			
		}
		
	}

	//
	// the offset should now be at the track table base
	//

	dwOffset = dwTrackTableBaseOffset;

    dwSize = SetFilePointer(m_hOutputFile,
		dwOffset,
		NULL,
		FILE_BEGIN);
	
	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	for (i=0;i<m_pFileHeader->dwSoundEntryCount;i++)
	{
		
		bResult = WriteFile(m_hOutputFile,
			m_pSoundTable[i].m_pXACTTrackTable,
			sizeof(XACT_SOUNDBANK_TRACK_ENTRY)*m_pSoundTable[i].m_pSoundEntry->wTrackCount,
			&dwBytesWritten,
			0);
		
		if (!bResult) {
			
			err = GetLastError();
			DebugPrint(DBGLVL_ERROR,"Failed to write data to file %s",m_szOutputFileName);
			goto errExit;
			
		}
		
	}
	





errExit:

    //
    // we dont need the output file anymore
    //

    CloseHandle(m_hOutputFile);
    m_hOutputFile = INVALID_HANDLE_VALUE;

    return err;


}


