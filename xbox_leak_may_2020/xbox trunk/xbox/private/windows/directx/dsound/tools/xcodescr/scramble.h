#include "..\inc\cipher.h"

#define DBGLVL_INFO 2
#define DBGLVL_WARN 1
#define DBGLVL_ERROR 0

#define APP_NAME "xcodescr"

class CCodeScrambler {

protected:

	UCHAR m_szKey[KEY_SIZE+1];
    UCHAR m_szDspCodeFileName[MAX_PATH];
    UCHAR m_szOutputFileName[MAX_PATH];


    HANDLE m_hInputFile;
    HANDLE m_hOutputFile;

	DWORD m_dwDebugLevel;

public:

    CCodeScrambler::CCodeScrambler();
    CCodeScrambler::~CCodeScrambler();

	//
	// utility functions
	//
    void CCodeScrambler::UsageFailure(int err);
    int ParseCommandLine(int argc, char * argv[]);
	void DebugPrint(DWORD dwLevel, PCHAR pszFormat, ...);

	//
	// encode
	//

	int Encode();
	int Decode();

};
