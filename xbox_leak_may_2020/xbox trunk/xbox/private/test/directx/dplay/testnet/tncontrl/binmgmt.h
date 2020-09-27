#ifndef __TNCONTROL_BINMGMT__
#define __TNCONTROL_BINMGMT__
//#pragma message("Defining __TNCONTROL_BINMGMT__")






//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG



#define MAX_NUMBER_ATTEMPTS		50





//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNBinary:public LLITEM
{
	public:
		char*		m_pszPath; // place on the machine where the binary was found
		BOOL		m_fDebug; // is this a debug version of the binary
		DWORD		m_dwMS; // most significant 32 bits of binary version
		DWORD		m_dwLS; // least significant 32 bits of binary version
		FILETIME	m_createtime; // time when the file was created
		FILETIME	m_writetime; // time when the file was last written to
		DWORD		m_dwFileSizeHigh; // high order DWORD describing file size
		DWORD		m_dwFileSizeLow; // low order DWORD describing file size


		CTNBinary(void);
		virtual ~CTNBinary(void);

		HRESULT LoadInfo(void);
		BOOL IsEqualTo(PTNBINARY pOtherBinary);
};

class DLLEXPORT CTNBinaryLocsList:public LLITEM
{
	public:
		char*			m_pszName; // (original) name of the binary
		LLIST			m_bins; // list of binaries


		CTNBinaryLocsList(void);
		virtual ~CTNBinaryLocsList(void);
};

class DLLEXPORT CTNTrackedBinaries:public LLIST
{
	// These are declared as friends so they can access the protected functions.
	friend class CTNMachineInfo;
	friend class CTNSlaveInfo;


	private:
		HRESULT SearchFor(char* szFilename, char* szSearchDirPath, BOOL fSubdirs,
						PLSTRINGLIST pResultPaths);


	protected:
		HRESULT PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize);
		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize);
#ifndef _XBOX // no file printing supported
		void PrintToFile(HANDLE hFile);
#endif // ! XBOX


	public:
		CTNTrackedBinaries(void);
		virtual ~CTNTrackedBinaries(void);

		HRESULT LocateAndAddBinary(char* szBinaryName);
		PTNBINARYLOCSLIST GetBinary(char* szBinaryName);

		HRESULT FirstBinIsVersion(char* szBinaryName, DWORD dwHighVersion,
									DWORD dwLowVersion, BOOL* pfResult);
		HRESULT FirstBinIsAtLeastVersion(char* szBinaryName, DWORD dwHighVersion,
										DWORD dwLowVersion, BOOL* pfResult);
		HRESULT LoadSymbolsForFirstBinaries(void);
};

class DLLEXPORT CTNBinaryManager
{
	// This is declared as a friend so it can access the protected functions.
	friend class CTNSlave;


	private:
		PTNTRACKEDBINARIES	m_pPlacedBins; // pointer to list indicating the binaries in their "in-use" locations
		PTNTRACKEDBINARIES	m_pSavedBins; // pointer to list indicating the correspondingly indexed binaries in their "saved" locations


	protected:
#ifndef _XBOX // no swapping supported
		HRESULT SaveSnapshot(PTNTRACKEDBINARIES pBinsToSave, char* szBinSnapshotPath);
		HRESULT RestoreSnapshot(void);
#endif

		
	public:
		DLLSAFE_NEWS_AND_DELETES;

		CTNBinaryManager(void);
		virtual ~CTNBinaryManager(void);

#ifndef _XBOX // no swapping supported
		HRESULT SwitchBinaryTo(char* szBinaryName, char* szNewBinaryPath);
#endif
};






#else //__TNCONTROL_BINMGMT__
//#pragma message("__TNCONTROL_BINMGMT__ already included!")
#endif //__TNCONTROL_BINMGMT__
