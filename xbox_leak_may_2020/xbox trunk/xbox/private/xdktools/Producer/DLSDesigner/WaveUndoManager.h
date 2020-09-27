#ifndef __WAVEUNDOMANAGER_H__
#define __WAVEUNDOMANAGER_H__

#include <afxtempl.h>
#include "WaveDataManager.h"

class CWaveDataManager;

class CWaveState
{
public:
	// Construction
	CWaveState(CString sStateName, CPtrList* pDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX pwfxCompression);
	~CWaveState();

	// Methods
	CString		GetStateName();
	CPtrList*	GetDeltaList();
	HEADER_INFO	GetHeaderInfo();
    
    LPWAVEFORMATEX GetCompressionFormat();

private:

	CPtrList*	    m_pDeltaList;
	CString		    m_sStateName;
	HEADER_INFO	    m_HeaderInfo;
    LPWAVEFORMATEX  m_pwfxCompression; // Valid only if m_bCompressed flag is set in the header info
};


class CWaveUndoManager
{

public:
	// Construction
	CWaveUndoManager(CWaveDataManager* pDataManager);
	~CWaveUndoManager();

public:
	// Methods

	HRESULT Initialize();						 // Gets rid of all the old states and adds the passed list as the first state 
	
	HRESULT SaveState(CString sStateName, CPtrList* pDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX pwfxCompression = NULL);
	HRESULT Undo(CPtrList* plstCurrent, CPtrList** pNewDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX* ppwfxCompression);
	HRESULT Redo(CPtrList** pNewDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX* ppwfxCompression);
    HRESULT PopUndoState();

	CString GetCurrentStateName();
	CString GetRedoStateName();

private:
	void	EmptyQueue();

private:
	CWaveDataManager*	m_pDataManager;
	POSITION			m_posCurrent;

	// flags to keep track of special conditions when we have to 
	// decide if we have a valid undo/redo state to support....
	// e.g. when we have only one state in the state list 
	bool		m_bUndoOK;	 
	bool		m_bRedoOK;
	bool		m_bCreateRedo;


	CTypedPtrList<CPtrList, CWaveState*> m_lstStates;
};


#endif // __WAVEUNDOMANAGER_H__