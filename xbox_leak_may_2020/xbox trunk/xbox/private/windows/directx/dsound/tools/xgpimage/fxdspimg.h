
//
// enviroment variables we read
//

#define ENV_VAR_INI_PATH "_XGPIMAGE_INI_PATH"
#define ENV_VAR_DSP_CODE_PATH  "_XGPIMAGE_DSP_CODE_PATH"

#define MAX_FX_NAME 256
#define APP_NAME "XGPIMAGE"

#define PAGE_SIZE 4096

class CFxData {
public:

	CFxData::CFxData()
	{

		DWORD i=0;
		memset(m_szEffectName,0,sizeof(m_szEffectName));
		memset(m_szFriendlyEffectName,0,sizeof(m_szFriendlyEffectName));
		memset(m_szDspCodeFileName,0,sizeof(m_szDspCodeFileName));
		memset(m_szDspStateFileName,0,sizeof(m_szDspStateFileName));

		m_dwEffectIndex = 0;
		m_dwNumInputs = 0;
		m_dwNumOutputs = 0;
		m_dwNumParams = 0;
		
		m_pDspCode = 0;
		m_dwDspCodeSize = 0;

		m_dwDspCyclesUsed = 0;
		
		m_pDspState = 0;
		m_dwDspStateSize = 0;

		for (i=0;i<MAX_FX_INPUTS;i++){

			m_dwInputIDs[i] = 0;

		}

		for (i=0;i<MAX_FX_OUTPUTS;i++){

			m_dwOutputIDs[i] = 0;

		}

        m_pEffectStateParameters = NULL;
	}

	CFxData::~CFxData()
	{

		if (m_pDspCode)
			delete [] m_pDspCode;

		if (m_pDspState)
			delete [] m_pDspState;

        if (m_pEffectStateParameters)
            delete [] m_pEffectStateParameters;
	}


	TCHAR m_szEffectName[MAX_FX_NAME];
	TCHAR m_szFriendlyEffectName[MAX_FX_NAME];
	TCHAR m_szDspCodeFileName[MAX_PATH];
	TCHAR m_szDspStateFileName[MAX_PATH];

	UCHAR m_szDevKey[KEY_SIZE];

    DWORD m_dwEffectIndex;
	DWORD m_dwNumInputs;
	DWORD m_dwNumOutputs;
	DWORD m_dwNumParams;

	PDWORD m_pDspCode;
	DWORD  m_dwDspCodeSize;
	
	PDWORD m_pDspState;
	DWORD  m_dwDspStateSize;

	DWORD  m_dwDspCyclesUsed;

	DWORD  m_dwInputIDs[MAX_FX_INPUTS];
	DWORD  m_dwOutputIDs[MAX_FX_OUTPUTS];

    PFX_STATE_PARAMETERS m_pEffectStateParameters;

	DWORD  CalcStateSize()
	{		
		return (sizeof(DSP_STATE_FIXED) + (m_dwNumParams + m_dwNumInputs + m_dwNumOutputs) * sizeof(DWORD) - sizeof(DWORD)*2 );
	}

	DWORD GetParamOffset()
	{

		return (sizeof(DSP_STATE_FIXED)/sizeof(DWORD) + (m_dwNumInputs + m_dwNumOutputs) - 2); 

	}

	DWORD GetScratchSize()
	{
		return ((PDSP_STATE_FIXED)m_pDspState)->dwScratchLength;
	}

	DWORD GetYMemSize()
	{
		return ((PDSP_STATE_FIXED)m_pDspState)->dwYMemLength;
	}

	DWORD ParamNameToParamID(PCHAR pName);
	DWORD InputNameToInputID(char * pName);
    DWORD OutputNameToOutputID(char * pName);


};

#define MAX_GRAPHS 20
#define MAX_FX_PER_GRAPH 32

class CFxGraph {

	DWORD m_dwNumEffects;
	TCHAR m_szGraphName[MAX_FX_NAME];
	CFxData *m_pFxArray[MAX_FX_PER_GRAPH];
public:

	CFxGraph::CFxGraph()
	{
		memset(m_szGraphName,0,sizeof(m_szGraphName));
		memset(m_pFxArray,0,sizeof(m_pFxArray));
		m_dwNumEffects = 0;
	}

	CFxGraph::~CFxGraph()
	{
		for (ULONG i=0;i<m_dwNumEffects;i++) {
			delete m_pFxArray[i];
		}
		m_dwNumEffects = 0;
	}

	DWORD GetNumEffects() { return m_dwNumEffects;}

	CFxData * GetFxData(DWORD dwIndex)
	{

		if (dwIndex < m_dwNumEffects){

			return m_pFxArray[dwIndex];

		} else {

			return NULL;

		}

	}

	void SetGraphName(TCHAR szName[])
	{
		memcpy(m_szGraphName,szName,sizeof(m_szGraphName));
	}

    TCHAR *GetGraphName()
	{
		return m_szGraphName;
	}

	void SetFx(CFxData *pFxData) 
	{
		pFxData->m_dwEffectIndex = m_dwNumEffects;
		m_pFxArray[m_dwNumEffects++] = pFxData;
	}

};

#define DBGLVL_INFO 2
#define DBGLVL_WARN 1
#define DBGLVL_ERROR 0

class CDspImageBuilder {

protected:

    TCHAR m_szAppName[64];

	TCHAR m_szDspCodePath[MAX_PATH];
	TCHAR m_szIniPath[MAX_PATH];

    TCHAR m_szDspDataFileName[MAX_PATH];
    TCHAR m_szScratchImageFileName[MAX_PATH];
    TCHAR m_szCHeaderFileName[MAX_PATH];

	TCHAR m_szImageFriendlyName[MAX_FX_NAME];

    HANDLE m_hInputFile;
    HANDLE m_hOutputFile;

	CFxGraph *m_pGraphs[MAX_GRAPHS];
	DWORD m_dwGraphCount;
	DWORD m_dwDebugLevel;

	DWORD m_dwTotalScratchSize;
	DWORD m_dwTotalDspCodeSize;
	DWORD m_dwTotalDspStateSize;
	DWORD m_dwTotalDspYMemSize;
	DWORD m_dwTotalDspCyclesUsed;

	DWORD m_dwTotalFX;

	UCHAR m_aTempBins[MAX_TEMPBINS];
	DWORD m_dwNumTempBins;

public:

    CDspImageBuilder::CDspImageBuilder(char* AppName);
    virtual CDspImageBuilder::~CDspImageBuilder();

	//
	// utility functions
	//
    
    int ParseCommandLine(int argc, char * argv[]);
    void UsageFailure(int err);
	int ErrorCheckOnParser(PCHAR pResult, PCHAR pDefault,DWORD dwBytesRead, DWORD dwSize);

	int ReadDspCodeFile(CFxData *pFx);

    void PrintParsingError(PCHAR pSection, PCHAR pKey,PCHAR szFormat, ...);
	void PrintValidationError(DWORD dwDebugLevel, DWORD dwGraphIndex, DWORD dwFxIndex, PCHAR pszFormat, ...);

	void DebugPrint(DWORD dwLevel, PCHAR pszFormat, ...);

	virtual void Print(PCHAR pFormat, ...);

	//
	// core functions
	//

    int ParseInputFile();
	int ValidateFxGraphs();
    int BuildDspImage();
    int CreateEnumHeader();
	//
	// parsing functions
	//
    int ParseParameterDescriptions(const char *pFileName, PFX_STATE_PARAMETERS *ppDesc);

	int ParseStateParameters( PCHAR pFxName, CFxData *pFxData);
    int ParseInputs( PCHAR pFxName, CFxData *pFxData);
    int ParseOutputs( PCHAR pFxName, CFxData *pFxData);

	//
	// DSP image generation functions
	//

	DWORD BufferIDToDSPAddress(DWORD dwBinBaseIndex,DWORD dwId);
	void DefaultCommandBlock(HOST_TO_DSP_COMMANDBLOCK *d);

	// Tools to programatically set the paths instead of relying on env vars
	void SetIniPath(LPCTSTR pPath);
	void SetDspCodePath(LPCTSTR pPath);

};

