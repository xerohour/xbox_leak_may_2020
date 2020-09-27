////////////////////////////////////////////////////
//
// CWaveTempFileHandler Implementation
//
////////////////////////////////////////////////////

#include "stdafx.h"
#include "winbase.h"
#include "WaveTempFileHandler.h"


////////////////////////////////////////////////////
//
// Construction:- CWaveTempFileHandler::CWaveTempFileHandler
//
////////////////////////////////////////////////////
CWaveTempFileHandler::CWaveTempFileHandler(CString sTempFileName) : CWaveFileHandler(sTempFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite)
{
}

