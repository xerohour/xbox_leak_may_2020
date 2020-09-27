#define DBGLVL_INFO 2
#define DBGLVL_WARN 1
#define DBGLVL_ERROR 0

//
// context for a single XACT_SOUNDBANK_TRACK_ENTRY
class CXACTTrackData {
public:
    CXACTTrackData::CXACTTrackData()
    {
        m_pEventBuffer = NULL;
		m_dwEventDataSize = 0;
		m_pTrackEntry = NULL;
		
    }

	void SetTrackEntry(PXACT_SOUNDBANK_TRACK_ENTRY pTrackEntry)
	{

		m_pTrackEntry = pTrackEntry;

	}

	CXACTTrackData::~CXACTTrackData()
	{

		if (m_pEventBuffer) {
			delete [] m_pEventBuffer;
			m_pEventBuffer = NULL;
		}

	}

	PXACT_SOUNDBANK_TRACK_ENTRY m_pTrackEntry;
	XACT_TRACK_EVENT *m_pEventBuffer;

	DWORD		m_dwEventDataSize;
	

};

class CXACTSoundData {
public:
    CXACTSoundData::CXACTSoundData()
    {
        m_pWaveBankTable = NULL;
        m_pTrackTable = NULL; 
		m_pXACTTrackTable = NULL;
        m_pSoundEntry = NULL;
        m_pSound3dData = NULL;
    }

    BOOL IsValid()
    {
        return ((m_pSoundEntry!=NULL) && (m_pSoundEntry->wWaveBankCount != 0));
    }

    int Initialize(PXACT_SOUNDBANK_SOUND_ENTRY pSoundEntry)
    {

        int err = ERROR_SUCCESS;

		//
		// the caller passes us the memory allocation for a single
		// sound entry that comes from a big contigious buffer
		//
                
        m_pSoundEntry = pSoundEntry;

        //
        // allocate cue data structures
        //

        m_pSound3dData = (PXACT_SOUNDBANK_SOUND_3D_PARAMETERS) new XACT_SOUNDBANK_SOUND_3D_PARAMETERS[1];
        if (m_pSound3dData == NULL) {
            err = ERROR_OUTOFMEMORY;
            return -1;
        }

        memset(m_pSound3dData,0,sizeof(XACT_SOUNDBANK_SOUND_3D_PARAMETERS));

        //
        // set proper initial parameters
        //

        m_pSound3dData->flMinDistance = 0.5f;
        m_pSound3dData->flMaxDistance = 1.0f;
        m_pSound3dData->flDistanceFactor = DS3D_MINDISTANCEFACTOR;
        m_pSound3dData->flDopplerFactor = DS3D_MINDOPPLERFACTOR;
        m_pSound3dData->flRolloffFactor = DS3D_MINROLLOFFFACTOR;

        return err;
    }


    CXACTSoundData::~CXACTSoundData()
    {
        if (m_pWaveBankTable) {
            delete [] m_pWaveBankTable;
            m_pWaveBankTable = NULL;
        }

        if (m_pTrackTable) {

            delete [] m_pTrackTable;
            m_pTrackTable = NULL;
        }

		if (m_pXACTTrackTable ) {
            delete [] m_pXACTTrackTable;
            m_pXACTTrackTable = NULL;
		}

        if (m_pSound3dData) {
            delete [] m_pSound3dData;
            m_pSound3dData = NULL;
        }

    }

    int GetWaveBankTableIndex(const PCHAR pszName)
    {
        for (DWORD i=0;i<m_pSoundEntry->wWaveBankCount;i++){

            if (!strcmp(m_pWaveBankTable[i].szFriendlyName,pszName)){
                return i;
            }
        }

        return -1;
    }

    PXACT_SOUNDBANK_SOUND_ENTRY m_pSoundEntry;
    PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY m_pWaveBankTable;

    CXACTTrackData *m_pTrackTable;
	PXACT_SOUNDBANK_TRACK_ENTRY m_pXACTTrackTable;

    PXACT_SOUNDBANK_SOUND_3D_PARAMETERS m_pSound3dData;

};

class CXACTFileGenerator {

protected:

    TCHAR m_szAppName[64];

    TCHAR m_szIniFileName[MAX_PATH];
    TCHAR m_szOutputFileName[MAX_PATH];

    HANDLE m_hInputFile;
    HANDLE m_hOutputFile;

	DWORD m_dwDebugLevel;	

    //
    // soundbank data
    //

    PXACT_SOUNDBANK_FILE_HEADER m_pFileHeader;
    CXACTSoundData  *m_pSoundTable;
	PXACT_SOUNDBANK_SOUND_ENTRY m_pXACTSoundTable;

    PXACT_SOUNDBANK_CUE_ENTRY m_pCueTable;
   
public:

    CXACTFileGenerator::CXACTFileGenerator(char* AppName);
    virtual CXACTFileGenerator::~CXACTFileGenerator();

	//
	// utility functions
	//
    
    int ParseCommandLine(int argc, char * argv[]);
    void UsageFailure(int err);
	int ErrorCheckOnParser(PCHAR pResult, PCHAR pDefault,DWORD dwBytesRead, DWORD dwSize);

    void PrintParsingError(PCHAR pSection, PCHAR pKey,PCHAR szFormat, ...);
	void DebugPrint(DWORD dwLevel, PCHAR pszFormat, ...);

	virtual void Print(PCHAR pFormat, ...);

	//
	// core functions
	//

	int CreateBinaryImage();

	//
	// parsing functions
	//

    int ParseIniFile();
    int ParseSoundData(DWORD dwSoundIndex);
    int ParseTrackEvents(DWORD dwSoundIndex, DWORD dwTrackIndex);

    int ParseEvent(LPSTR lpSectionName,
        DWORD dwSoundIndex,
        PXACT_TRACK_EVENT pEvent);


};
