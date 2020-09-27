#include "stdafx.h"
#include "WaveUndoManager.h"
#include "WaveDataManager.h"

/////////////////////////////////////////////////////////////////
//
// CWaveState Implementation
//
/////////////////////////////////////////////////////////////////
CWaveState::CWaveState(CString sStateName, CPtrList* pDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX pwfxCompression):
m_sStateName(sStateName), 
m_pDeltaList(pDeltaList), 
m_pwfxCompression(pwfxCompression)
{
	CopyMemory(&m_HeaderInfo, pHeaderInfo, sizeof(HEADER_INFO));
}


CWaveState::~CWaveState()
{
	// Delete the delta list
	while(!m_pDeltaList->IsEmpty())
	{
		delete m_pDeltaList->RemoveHead();
	}

	delete m_pDeltaList;

    if(m_pwfxCompression)
    {
        delete[] (BYTE*)m_pwfxCompression;
    }
}


CString CWaveState::GetStateName()
{
	return m_sStateName;
}


CPtrList* CWaveState::GetDeltaList()
{
	return m_pDeltaList;
}

HEADER_INFO	CWaveState::GetHeaderInfo()
{
	return m_HeaderInfo;
}


LPWAVEFORMATEX CWaveState::GetCompressionFormat()
{
    return m_pwfxCompression;
}

/////////////////////////////////////////////////////////////////
//
// CWaveUndoManager Implementation
//
/////////////////////////////////////////////////////////////////
CWaveUndoManager::CWaveUndoManager(CWaveDataManager* pDataManager)
{
	m_pDataManager = pDataManager;
	m_posCurrent = NULL;

	m_bUndoOK = true;
	m_bRedoOK = true;
	m_bCreateRedo = false;
}


CWaveUndoManager::~CWaveUndoManager()
{
	EmptyQueue();
}


HRESULT CWaveUndoManager::Initialize()
{
	EmptyQueue();

	m_bUndoOK = true;
	m_bRedoOK = false;

	return S_OK;
}

void CWaveUndoManager::EmptyQueue()
{
	while(!m_lstStates.IsEmpty())
	{
		delete m_lstStates.RemoveHead();
	}

	m_posCurrent = NULL;
}


HRESULT CWaveUndoManager::SaveState(CString sStateName, CPtrList* pDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX pwfxCompression)
{
	ASSERT(pDeltaList);
	if(pDeltaList == NULL)
	{
		return E_POINTER;
	}

	// Delete all the states after the current position
	POSITION position = m_posCurrent;
	if(position && m_bUndoOK == true && m_bCreateRedo == true)
	{
		m_lstStates.GetNext(position);
	}

	while(position)
	{
		POSITION oldPosition = position;
		CWaveState* pState = (CWaveState*)m_lstStates.GetNext(position);
		m_lstStates.RemoveAt(oldPosition);
		ASSERT(pState);
		if(pState)
		{
			delete pState;
		}
	}

	CWaveState* pNewState = new CWaveState(sStateName, pDeltaList, pHeaderInfo, pwfxCompression);
	m_posCurrent = m_lstStates.AddTail(pNewState);
	
	if(m_bCreateRedo == false)
	{
		m_bUndoOK = true;
		m_bRedoOK = false;
	}

	m_bCreateRedo = true;


	return S_OK;
}


HRESULT CWaveUndoManager::Undo(CPtrList* plstCurrent, CPtrList** pNewDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX* ppwfxCompression)
{
	ASSERT(plstCurrent);
	if(plstCurrent == NULL)
	{
		return E_POINTER;
	}

	if(m_bUndoOK == false)
	{
		return E_FAIL;
	}

	POSITION undoPosition = m_posCurrent;
	m_lstStates.GetPrev(undoPosition);
	if(undoPosition == NULL)
	{
		undoPosition = m_posCurrent;
	}

	CWaveState* pUndoState = (CWaveState*) m_lstStates.GetAt(undoPosition);
	ASSERT(pUndoState);

	// Add a redo state
	if(m_posCurrent == m_lstStates.GetTailPosition() && m_bCreateRedo)
	{
		// Delete all redo nodes from this point
		POSITION position = m_posCurrent;
		if(FAILED(SaveState(pUndoState->GetStateName(), plstCurrent, pHeaderInfo, *ppwfxCompression)))
		{
			return E_FAIL;
		}
		
		undoPosition = m_posCurrent;
		m_lstStates.GetPrev(undoPosition);
		pUndoState = (CWaveState*) m_lstStates.GetAt(undoPosition);
		m_bCreateRedo = false;
	}
    else
    {
        // We don't need to keep the compression format
        delete[] (BYTE*) *ppwfxCompression;
    }

	*pNewDeltaList = pUndoState->GetDeltaList();

	HEADER_INFO headerInfo = pUndoState->GetHeaderInfo();
	CopyMemory(pHeaderInfo, &headerInfo, sizeof(HEADER_INFO));

    if(headerInfo.m_bCompressed)
    {
        *ppwfxCompression = pUndoState->GetCompressionFormat();
    }


	if(undoPosition == m_lstStates.GetHeadPosition())
	{
		m_bUndoOK = false;
		m_bRedoOK = true;
		m_posCurrent = undoPosition;
	}
	else
	{
		m_bRedoOK = true;
		m_bUndoOK = true;
		m_posCurrent = undoPosition;
	}

	return S_OK;
}

HRESULT CWaveUndoManager::Redo(CPtrList** pNewDeltaList, HEADER_INFO* pHeaderInfo, LPWAVEFORMATEX* ppwfxCompression)
{
	ASSERT(pHeaderInfo);
	if(pHeaderInfo == NULL)
	{
		return E_POINTER;
	}

	if(m_bRedoOK == false)
	{
		return E_FAIL;
	}

	POSITION redoPosition = m_posCurrent;
	m_lstStates.GetNext(redoPosition);
	if(redoPosition == NULL)
	{
		redoPosition = m_posCurrent;
	}
	
	CWaveState* pUndoState = (CWaveState*) m_lstStates.GetAt(redoPosition);
	ASSERT(pUndoState);
	*pNewDeltaList = pUndoState->GetDeltaList();

	HEADER_INFO headerInfo = pUndoState->GetHeaderInfo();
	CopyMemory(pHeaderInfo, &headerInfo, sizeof(HEADER_INFO));

    if(headerInfo.m_bCompressed)
    {
        *ppwfxCompression = pUndoState->GetCompressionFormat();
    }

	if(redoPosition == m_lstStates.GetTailPosition())
	{
		m_bRedoOK = false;

		m_bUndoOK = true;
		m_posCurrent = redoPosition;
	}
	else
	{
		m_bRedoOK = true;
		m_bUndoOK = true;
		m_posCurrent = redoPosition;
	}

	return S_OK;
}


HRESULT CWaveUndoManager::PopUndoState()
{
    if(m_lstStates.GetCount() == 0)
    {
        return S_FALSE;
    }

    CWaveState* pState = m_lstStates.RemoveTail();
    if(pState)
    {
        delete pState;
    }

    m_posCurrent = m_lstStates.GetTailPosition();

    return S_OK;
}

CString CWaveUndoManager::GetCurrentStateName()
{
	if(m_posCurrent && m_bUndoOK)
	{
		POSITION undoPosition = m_posCurrent;
		if(m_bCreateRedo == false)
		{
			m_lstStates.GetPrev(undoPosition);
		}
		if(undoPosition == NULL)
		{
			undoPosition = m_posCurrent;
		}

		if(undoPosition)
		{
			CWaveState* pState = m_lstStates.GetAt(undoPosition);
			return pState->GetStateName();
		}
	
	}

	return "";
}

CString CWaveUndoManager::GetRedoStateName()
{
	if(m_posCurrent && m_bRedoOK)
	{
		POSITION redoPosition = m_posCurrent;
		if(redoPosition == NULL)
		{
			redoPosition = m_posCurrent;
		}

		CWaveState* pState = (CWaveState*) m_lstStates.GetAt(redoPosition);
		return pState->GetStateName();
	}

	return "";
}


