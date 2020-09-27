#ifndef __WAVETEMPFILEHANDLER_H__
#define __WAVETEMPFILEHANDLER_H__

#include "WaveFileHandler.h"

class CWaveTempFileHandler : public CWaveFileHandler
{
	// Construction
public:
	CWaveTempFileHandler(CString sTempFileName);
};

#endif // __WAVETEMPFILEHANDLER_H__