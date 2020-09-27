#ifndef __WAVEDATAMANAGER_H__
#define __WAVEDATAMANAGER_H__

#include <afxtempl.h>
#include "wave.h"
#include "dls1.h"


class CWave;
class CWaveUndoManager;
class CWaveFileHandler;
class CWaveSourceFileHandler;
class CWaveDelta;

class CWaveDataManager
{
	
public:
	// Construction
	CWaveDataManager(CWave* pWave, CString sSourceFileName, CString sTempFilePath); // Standard c'tor
	CWaveDataManager(CWave* pWave, CString sTempFilePath);							// Constructs only decomp file handler
	~CWaveDataManager();
	
public:
	// Methods
	HRESULT Initialize(IStream* pIStream);					// Initializes the offsets, primes the delta list
	HRESULT Initialize(CString sFileName, 
						DWORD dwHeaderOffset, 
						DWORD dwDataOffset, 
						bool bDeleteOldSource = false);		// Initializes the delta list

	HRESULT CopyDecompFile();								// Makes a copy of the source file in the temp directory and keeps the name
	HRESULT	CreateDecompFile();								// Create a temp file to keep the decompressed data
	HRESULT CloseSourceHandler();							// Close the source file handler for saving
	HRESULT RenameSource(CString sNewSourceName);			// Rename the source file
	HRESULT OnSourceRenamed(CString sNewSourceFile);		// The source file name changed so reinitialize everything
	HRESULT CloneDeltaList(CPtrList* pDeltaList, 
						  CPtrList** ppClonedList);

	void	UpdateHeaderInfo();								// Gets the header info from the wave

	DWORD	GetWavelength();
	
	// Operations on data
	HRESULT GetData(DWORD dwStartSample, DWORD dwLength, BYTE* pbData, 
					DWORD& dwBytesRead, bool bGetUncompressed = false);	// Gets a chunk of data from source/temp files

	HRESULT GetDecompData(DWORD dwStartSample, DWORD dwLength, 
						BYTE* pbData, DWORD& dwBytesRead);				// Gets a chunk of decompressed data from the decomp file
	
	HRESULT GetSourceData(DWORD dwStartOffset, DWORD dwLength, BYTE** pbData, DWORD& cbRead);

    HRESULT GetAllUncompressedData(BYTE** ppbData, DWORD* pdwSize);


	HRESULT InsertData(DWORD dwInsertAt, BYTE* pbData, DWORD dwLength);	// Inserts data into the temp file
	HRESULT RemoveData(DWORD dwStartSample, DWORD dwSamples);			// Splits the delta list to keep tracks of the deletes
	HRESULT WriteDecompressedData(BYTE* pbData, DWORD dwDataSize);		// Writes the decompressed data to the decomp file
	HRESULT	DecompressRuntimeData();									// Decompresses the runtime comnpressed wavedata and puts in the decomp file
	HRESULT DecompressDesigntimeData();									// Compresses and Decompresses the design-time wave data

    HRESULT UpdateUncompressedDeltas();

	// Methods to work with undo/redo
	HRESULT SaveUndoState(CString sStateName);
	HRESULT Undo();
	HRESULT Redo();
	CString GetUndoMenuText(bool bRedo = false);
    HRESULT PopUndoState();
		
	// Helpers for persistance
	HRESULT SaveUncompressedDataToStream(IStream* pIStream, DWORD& dwBytesWritten);	// Create a 'data' chunk into the stream
	
	// General purpose accessors/mutators
	CString GetSourceFileName();

	// Temp file creation
	static	HRESULT CreateUniqueTempFile(CString sPrefix, CString sExtension, CString& sTempFileName);

private:

	HRESULT GetData(CWaveDelta* pDelta, DWORD dwStartSample, DWORD dwLength, BYTE* pbData, DWORD& dwBytesRead, bool bReadFromCopy = false);
	void	ResetActualDeltaStarts();

	// Attributes
private:
	
	CString m_sSourceFileName;
	CString m_sCopyFileName;
	CString m_sTempFilePath;
	CString m_sTempFileName;
	CString m_sDecompFileName;

	CWave*					m_pWave;
	CWaveUndoManager*		m_pUndoManager;

	CWaveSourceFileHandler* m_pSourceFileHandler;
	CWaveFileHandler*		m_pTempFileHandler;
	CWaveFileHandler*		m_pCopyFileHandler;
	CWaveFileHandler*		m_pDecompFileHandler;
	

	CPtrList*				m_plstDeltas;
    CPtrList*               m_plstUncompressedDeltas;
	HEADER_INFO				m_HeaderInfo;	

	BOOL					m_bInInit;
    BOOL                    m_bDecompressedRunTime;
    BOOL                    m_bDecompressedDesignTime;
    BOOL                    m_bNeedToMakeSrcDelta;
};

#endif // __WAVEDATAMANAGER_H__