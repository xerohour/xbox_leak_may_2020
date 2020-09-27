// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFLD.cpp
// Contents:  FLD file wrapper.
// Revisions: 7-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

#include "..\\xbLayout\\FLDVersion.h"

typedef struct
{
    WORD wMajor;
    WORD wMinor;
    WORD wBuild;
    WORD wQFE;
} sVersion;

#define FLD_SIG "XBOXFLDFILE"
#define FLD_SIG_SIZE sizeof(FLD_SIG)

typedef struct
{
    BYTE  szSig[FLD_SIG_SIZE];
    time_t timeSaved;
    sVersion version;
} sFLDHeader;

// Taken from AMC's file "MediaBase.h"
struct TFileHeader
{
    char m_szFileType[32]; // FST? Error Map?
    char m_szMediaType[32]; // DVD, CD, etc
};

struct TMediaHeader
{
    DWORD uNumSectorsLayer0;
    DWORD uNumSectorsLayer1;
    BYTE m_ImpUseArea[120];       // total = 128 bytes
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFLD::CFLD
// Purpose:   Opens the specified FLD file and reads the header information from it.
// Arguments: pdlg          -- Dialog to handle error messages
//            pstep         -- Step that this FLD was created in.
//            szFilename    -- Name of the FLD file to open.
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFLD::CFLD(CDlg *pdlg, CStep_Progress *pstep, char *szFilename) : CFile(pdlg, pstep, szFilename)
{
    sFLDHeader fldhdr;
    
    // Check to see if the file was successfully opened
    if (!IsInited())
        return;

    m_fWrongVersion = false;

    // Read the header (to skip it)
    Read((BYTE*)&fldhdr, sizeof fldhdr, NULL);

    // Save the dvd root since we need it for electronic submission
    ReadString(m_szDVDRoot);

    SetInited(S_OK);
}

#define OBJ_FOLDER    0x0010
#define OBJ_ENDOFLIST 0x0080

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CFLD::ReadNextObjectInfo(DWORD *pdwType, DWORD *pdwLSN, DWORD *pdwBlockSize, char *szName)
{
	DWORD dwDummy;
	char  szDummy[MAX_PATH];

    // Get next object type
	if (FAILED(ReadDWORD(pdwType)))
		return false;
    if (*pdwType == OBJ_ENDOFLIST)
        return true;

	// Read the object-specified data from the file; ignore some of it.
	if (FAILED(ReadDWORD(pdwLSN)))       return false;
	if (FAILED(ReadDWORD(pdwBlockSize))) return false;
    if (FAILED(ReadDWORD(&dwDummy)))     return false;
    if (FAILED(ReadDWORD(&dwDummy)))     return false;
	if (FAILED(ReadString(szName)))      return false;
	if (FAILED(ReadString(szDummy)))     return false;
    if (FAILED(ReadDWORD(&dwDummy)))     return false;
    if (FAILED(ReadDWORD(&dwDummy)))     return false;

    // Skip past folder id if the object read was a folder
    if (*pdwType == OBJ_FOLDER && FAILED(ReadDWORD(&dwDummy)))
        return false;

    return true;
}