// codescr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "scramble.h"

int __cdecl main(int argc, char* argv[])
{
	int err;
	CCodeScrambler Scrambler;

	//
	// parse command line and open files
	//
	err = Scrambler.ParseCommandLine(argc,argv);

	//
	// encode the data
	//

	if (err == ERROR_SUCCESS) {

		err = Scrambler.Encode();

	}

	return err;
}

CCodeScrambler::CCodeScrambler()
{

	memset(m_szKey,0,sizeof(m_szKey));
    m_hInputFile = NULL;
    m_hOutputFile = NULL;
	m_dwDebugLevel = DBGLVL_WARN;


}

void CCodeScrambler::DebugPrint(DWORD dwLevel, PCHAR pszFormat, ...)
{
    CHAR                    szString[0x400];
    va_list                 va;

	va_start(va, pszFormat);
	vsprintf(szString, pszFormat, va);
	va_end(va);
	
	if (m_dwDebugLevel >= dwLevel) {
		
		fprintf(stderr,"*** %s ***\n",
			szString);
	}

}

void CCodeScrambler::UsageFailure(int err)
{
	if (err!=ERROR_SUCCESS){
		fprintf(stderr,"\n Error: 0x%x\n", err);
	}

    fprintf(stderr,"\n Usage: %s [key (8 chars)] [DSP binary code filename] [output filename]\n",
        APP_NAME);

}

int CCodeScrambler::ParseCommandLine(int argc, char **argv)
{

    int err = ERROR_SUCCESS;

	if (strstr("?",argv[1])) {
        UsageFailure(ERROR_SUCCESS);
		err = -1;
        return err;
	}

	if (strstr("/?",argv[1])) {
        UsageFailure(ERROR_SUCCESS);
		err = -1;
        return err;
	}

	if (strstr("-?",argv[1])) {
        UsageFailure(ERROR_SUCCESS);
		err = -1;
        return err;
	}

    if (argc < 3) {

        err = -1;
        UsageFailure(err);
        return err;
    }
	

    memcpy(m_szKey,argv[1],sizeof(m_szKey));
    printf("Key %s\n", m_szKey);

    memcpy(m_szDspCodeFileName,argv[2],sizeof(m_szDspCodeFileName));
    printf("Input File to scramble %s\n", m_szDspCodeFileName);

    memcpy(m_szOutputFileName,argv[3],sizeof(m_szOutputFileName));
    printf("Output File %s\n", m_szOutputFileName);
    
    //
    // open the file so we can generating the image
    //

    m_hOutputFile = CreateFile(
        (const char *)m_szOutputFileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        0,
        NULL);

    if (m_hOutputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        fprintf(stderr,"\n Failed to open the output file.Error 0x%x\n", err);
        return err;

    }

    m_hInputFile = CreateFile(
        (const char *)m_szDspCodeFileName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL);

    if (m_hInputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        fprintf(stderr,"\n Failed to open the dsp code file.Error 0x%x\n", err);
        return err;

    }

    return ERROR_SUCCESS;

}

CCodeScrambler::~CCodeScrambler()
{
    if (m_hInputFile != NULL) {
        CloseHandle(m_hInputFile);
    }

    if (m_hOutputFile != NULL) {
        CloseHandle(m_hOutputFile);
    }

}

int CCodeScrambler::Encode()
{

	DWORD dwSize,dwActualSize;
	PUCHAR pSrc,pDst;
	int err = ERROR_SUCCESS;
	BOOL bResult = FALSE;

	//
	// determine size of input file

	dwSize = SetFilePointer(m_hInputFile, 0, NULL, FILE_END);              
	SetFilePointer(m_hInputFile, 0, NULL, FILE_BEGIN);

	pSrc = new UCHAR[dwSize];
	if (pSrc == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	pDst = new UCHAR[dwSize+KEY_SIZE];
	if (pDst == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	memset(pDst,0,dwSize);

	//
	// read file data
	//

	bResult = ReadFile(m_hInputFile,
		pSrc,
		dwSize,
		&dwActualSize,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to read dsp code from file %s.Error 0x%x",
			m_szDspCodeFileName,
			err);

		goto errExit;
		
	}

	//
	// call library function
	//

	err = XAudiopUtility_Encode(m_szKey,pSrc,dwSize,pDst,TRUE);
	
	if (err == ERROR_SUCCESS) {
		
		//
		// write result to output file
		//
		bResult = WriteFile(m_hOutputFile,
			pDst,
			dwSize+KEY_SIZE,
			&dwActualSize,
			0);
		
		if (!bResult) {
			
			err = GetLastError();
			DebugPrint(DBGLVL_ERROR,"Failed to write dsp scrambled output to file %s.Error 0x%x",
				m_szOutputFileName,
				err);
			
			goto errExit;
			
		}

	}

	//
	// destructor will close the handles
	//

errExit:

	if (pDst) {
		delete [] pDst;
	}

	if (pSrc) {
		delete [] pSrc;
	}

	return err;
}

int CCodeScrambler::Decode()
{

	DWORD dwSize,dwActualSize;
	PUCHAR pSrc,pDst;
	int err = ERROR_SUCCESS;
	BOOL bResult = FALSE;

	//
	// determine size of input file

	dwSize = SetFilePointer(m_hInputFile, 0, NULL, FILE_END);              
	SetFilePointer(m_hInputFile, 0, NULL, FILE_BEGIN);

	pSrc = new UCHAR[dwSize];
	if (pSrc == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	pDst = new UCHAR[dwSize-KEY_SIZE];
	if (pDst == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	memset(pDst,0,dwSize);

	//
	// read file data
	//

	bResult = ReadFile(m_hInputFile,
		pSrc,
		dwSize,
		&dwActualSize,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to read dsp code from file %s.Error 0x%x",
			m_szDspCodeFileName,
			err);

		goto errExit;
		
	}

	//
	// call library function
	//

	err = XAudiopUtility_Decode(NULL,pSrc,dwSize,pDst,TRUE);
	
	if (err == ERROR_SUCCESS) {
		
		//
		// write result to output file
		//
		bResult = WriteFile(m_hOutputFile,
			pDst,
			dwSize,
			&dwActualSize,
			0);
		
		if (!bResult) {
			
			err = GetLastError();
			DebugPrint(DBGLVL_ERROR,"Failed to write dsp scrambled output to file %s.Error 0x%x",
				m_szOutputFileName,
				err);
			
			goto errExit;
			
		}

	}

	//
	// destructor will close the handles
	//

errExit:

	if (pDst) {
		delete [] pDst;
	}

	if (pSrc) {
		delete [] pSrc;
	}

	return err;
}
