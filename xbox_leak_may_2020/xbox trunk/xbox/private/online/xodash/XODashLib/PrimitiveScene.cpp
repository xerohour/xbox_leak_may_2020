///////////////////////////////////////////////////////////////////////////////
// CPrimitiveScene.h
// base class for all individual screens.
// This class holds all the geometry, plus lists of interactive objects
// all individual screens should derive from this object
//////////////////////////////////////////////////////////////////////////////
#include "std.h"

#include "PrimitiveScene.h"
#include "utilities.h"
#include "XGMath.h"
#include "TGL.h"
#include "globals.h"
#include "XBInput.h"
#include "SmartObject.h"
#include "IniFile.h"
#include "Translator.h"

// static audio clip for all the button sounds
// WE do not want to keep calling set URL, this causes
// the file to reopen and reread the whole contents, painfully slow
CAudioClip* CPrimitiveScene::s_AudioFiles[eLastSoundType][4] = { 0,0,0,0,0,0,0,0};

//////////////////////////////////////////////////////////////////////////////
CPrimitiveScene::CPrimitiveScene()
{
	m_pRoot = 0;
	memset ( &m_Light, 0x00, sizeof ( D3DLIGHT8 ) );

    m_pVideoMem = NULL;

	m_bVerticalButtons = true;
	m_nActiveButtonIndex = 0;
	m_pHelpText = NULL;
    m_pListBox = NULL;
	m_eSceneId = eNullSceneId;
    m_pAnimMem = NULL;
}

//////////////////////////////////////////////////////////////////////////////
CPrimitiveScene::~CPrimitiveScene()
{
	Cleanup();
 }


HRESULT CPrimitiveScene::Initialize(char* szUrl, char* szTextURL, eSceneId eCurSceneId,bool bAutoTest )
{
	m_szUrlName = szUrl;
	m_eSceneId = eCurSceneId;
	HRESULT hr = LoadFromXBG(g_pd3dDevice, szUrl, szTextURL );
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		DbgPrint("CPrimitiveScene::Initialize - fail to load a scene");
	}

	if(FAILED(hr))
	{
		DbgPrint("CPrimitiveScene::Initialize - fail to bind data");
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////////
HRESULT CPrimitiveScene::Cleanup()
{
	
	if(m_eSceneId < countof(SceneName))
	{
		DbgPrint("Deleting scene %d [%s]\n", m_eSceneId, SceneName[m_eSceneId]);
	}
	
     // must do this in case the memory below is being referenced
    g_pd3dDevice->SetStreamSource( 0, 0, 0 );
    g_pd3dDevice->SetIndices( 0, 0 );

 
 

    if ( NULL != m_pVideoMem )
    {
        D3D_FreeContiguousMemory(m_pVideoMem);     

    }

    m_pVideoMem = NULL;


    if ( NULL != m_pAnimMem )
    {
        delete [] m_pAnimMem;
    }

    m_pAnimMem = NULL;


    // must be done BEFORE we kill the buttons
    if ( m_pListBox )
        delete m_pListBox;

	int nSize = m_pButtons.size();
	for (size_t i = 0; i<m_pButtons.size(); i++)
	{
		CButton* pButt = m_pButtons[i];
		delete m_pButtons[i];
		m_pButtons[i] = NULL;
	}
	m_pButtons.erase(m_pButtons.begin(), m_pButtons.end());

	nSize = m_pTexts.size();
	for (size_t i = 0; i<m_pTexts.size(); i++)
	{
		delete m_pTexts[i];
		m_pTexts[i] = NULL;
	}
	m_pTexts.erase(m_pTexts.begin(), m_pTexts.end());

	nSize = m_pSpinners.size();
	for (size_t i = 0; i<m_pSpinners.size(); i++)
	{
		delete m_pSpinners[i];
		m_pSpinners[i] = NULL;
	}
	m_pSpinners.erase(m_pSpinners.begin(), m_pSpinners.end());

    m_pSpinners.resize( 0 );
    m_pButtons.resize( 0 );
    m_pTexts.resize( 0 );

	// help text is deleted as part of the m_pTexts vector
	m_pHelpText = NULL;

 
    m_pListBox = NULL;

    //must be done last because other objects reference this
    if ( m_pRoot )
        delete m_pRoot;

    m_pRoot = NULL;

	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// Draw the entire geometry, starting from the root and trickling down
/////////////////////////////////////////////////////////////////////////////
HRESULT CPrimitiveScene::Render( DWORD flags )
{
	ASSERT(m_pRoot);
    if ( m_pRoot )
	{
		if ( m_Light.Type != 0 )
		{
		   g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		   g_pd3dDevice->SetLight( 0, &m_Light );
		}
		else
		{
			g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x002f2f2f);
		}

		m_Camera.SetUp(g_pd3dDevice);
		m_pRoot->Render();

		for ( unsigned long i = 0; i < m_pTexts.size(); i++ )
		{
			m_pTexts[i]->Render(flags);
		}
	}
	return S_OK;

}


// Controller repeat values
const FLOAT fINITIAL_REPEAT = 0.333f; // 333 mS recommended for first repeat
const FLOAT fSTD_REPEAT     = 0.085f; // 85 mS recommended for repeat rate

HRESULT CPrimitiveScene::FrameMove( XBGAMEPAD& GamepadInput, float elapsedTime )
{	
	HRESULT hr = S_OK;

    m_pRoot->FrameMove( elapsedTime );
    m_Camera.FrameMove( elapsedTime );

	AutoTest(GamepadInput);

    // Spinners have control changes when there is no input, so sits above the eNoInput return
    for (unsigned int i = 0; i < m_pSpinners.size(); i++ )
    {
        m_pSpinners[i]->FrameMove( GamepadInput, elapsedTime );
    }

	if(GetInputState(GamepadInput) == eNoInput)
	{
		return hr;
	}
		// no buttons on the screen, do not care about input
	if(m_pButtons.empty())	
	{
		return hr;
	}

    // need to do the list box BEFORE we do the button highlight switch
    // this just takes care of updating text and scroll buttons
    if ( m_pListBox )
    {
        m_pListBox->FrameMove( GamepadInput, elapsedTime );
    }

	ASSERT(m_eSceneId < countof(SceneName));
	if (m_bVerticalButtons)
	{
		// allow to navigate up & down
		if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
			GamepadInput.fY1 == -1)
		{
			if((size_t)m_nActiveButtonIndex < m_pButtons.size()-1)
			{
				SetActiveButton(false);				
			}
			
		}

		if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ||
			GamepadInput.fY1 == 1)
		{
			if(m_nActiveButtonIndex > 0)
			{
				SetActiveButton(true);
			}
		}
	}
	else
	{
		if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
		   GamepadInput.fX1 == 1)
		{
			if((size_t)m_nActiveButtonIndex < m_pButtons.size() -1 )
			{
				SetActiveButton(false);
			}
			
		}

		if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
			GamepadInput.fX1 == -1)
		{
			if(m_nActiveButtonIndex > 0)
			{
				SetActiveButton(true);
			}
		}
	}
	
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
	{
		CTranslator::Instance()->SetLanguage();
	}

	for(size_t i = 0; i<m_pTexts.size(); i++)
	{
		m_pTexts[i]->FrameMove(elapsedTime);
	}


	return hr;
}


//-----------------------------------------------------------------------------
// Name: LoadFromXBG()
// Desc: Loads a .xbg file
//-----------------------------------------------------------------------------
HRESULT CPrimitiveScene::LoadFromXBG( LPDIRECT3DDEVICE8 pd3dDevice, char* strFilename, char* strButtonTextNames )
{

    pd3dDevice->SetStreamSource( 0, 0, 0 );
	pd3dDevice->SetIndices( 0, 0 );

    // Open the file
    File file;
    if ( NO_ERR != file.open( strFilename ) )
            return E_FAIL;

    // Read the magic number
    DWORD dwFileID;
    file.read( (BYTE*)&dwFileID, sizeof(DWORD) ); 

    if( dwFileID != XBG_FILE_ID )
    {
        char errorStr[256];
        sprintf( errorStr, "FILE %s NEEDS TO BE RESAVED\n", strFilename );
        DbgPrint( errorStr );
        ASSERT( !"Old File format found, please resave" );
        file.close( );
        return E_FAIL;
    }

    // Read in header
    DWORD dwNumMeshes;  // Number of meshes in files
    DWORD dwSysMemSize; // Num bytes needed for system memory objects
    DWORD dwVidMemSize; // Num bytes needed for video memory objects
    DWORD dwAnimMemSize; // num bytes needed for animation info

    file.read( (BYTE*)&dwNumMeshes,  sizeof(DWORD)  );
    file.read( (BYTE*)&dwSysMemSize, sizeof(DWORD)  );
    file.read( (BYTE*)&dwVidMemSize, sizeof(DWORD)  );
    file.read( (BYTE*)&dwAnimMemSize, sizeof(DWORD)  );


    // Read in system memory objects
    VOID* pSysMemData = (VOID*)new BYTE[dwSysMemSize];
    file.read( (BYTE*)pSysMemData, dwSysMemSize );

    if ( NULL != m_pVideoMem )
    {
        D3D_FreeContiguousMemory( m_pVideoMem );
        m_pVideoMem = NULL;
    }

    // Read in video memory objects
    m_pVideoMem = (VOID*)D3D_AllocContiguousMemory( dwVidMemSize, D3DVERTEXBUFFER_ALIGNMENT );
    file.read( (BYTE*)m_pVideoMem, dwVidMemSize ); 

    if ( dwAnimMemSize )
    {
        m_pAnimMem = (VOID*)new BYTE[dwAnimMemSize];
        file.read( (BYTE*)m_pAnimMem, dwAnimMemSize );
    }
    
    // Do post-load, per-mesh processing
    XBMESH_FRAME* pFrameData = (XBMESH_FRAME*)pSysMemData;

    DWORD dwSysMemBaseAddr = (DWORD)pSysMemData;
    DWORD dwVidMemBaseAddr = (DWORD)m_pVideoMem;
    DWORD dwAnimMemBaseAddr = (DWORD)m_pAnimMem - sizeof(XBG_HEADER); 

    for( DWORD i=0; i<dwNumMeshes; i++ )
    {
        XBMESH_FRAME* pFrame = &pFrameData[i];
        XBMESH_DATA*  pMesh  = &pFrameData[i].m_MeshData;

        // Patch the mesh data. Any pointers read from the file were stored as
        // file offsets. So, we simply need to add a base address to patch
        // things up.
        if( pFrame->m_pChild )
            pFrame->m_pChild  = (XBMESH_FRAME*)( (DWORD)pFrame->m_pChild + dwSysMemBaseAddr );
        if( pFrame->m_pNext )
            pFrame->m_pNext   = (XBMESH_FRAME*)( (DWORD)pFrame->m_pNext + dwSysMemBaseAddr );
        if( pMesh->m_pSubsets )
            pMesh->m_pSubsets = (XBMESH_SUBSET*)( (DWORD)pMesh->m_pSubsets + dwSysMemBaseAddr );
        if( pMesh->m_dwNumIndices )
            pMesh->m_IB.Data  = pMesh->m_IB.Data + dwSysMemBaseAddr;

        if ( pMesh->m_pPos )
            pMesh->m_pPos = (D3DXVECTOR3*)(dwAnimMemBaseAddr + (DWORD)pMesh->m_pPos );

        if ( pMesh->m_pQuats )
            pMesh->m_pQuats = (D3DXQUATERNION*)(dwAnimMemBaseAddr + (DWORD)pMesh->m_pQuats);

    }

    // make sure there isn't an old one hanging around
    if ( m_pRoot )
        delete m_pRoot;

    m_pRoot = NULL;

    // In one swoop, create the frame hierarchy
    m_pRoot = new TG_Shape( pd3dDevice, NULL, pFrameData, m_pVideoMem );

    m_Camera.LoadBinary( &file );

    // need to figure out which smart objects to load based on passed in strings
    long numSubSets =  file.readLong();

    char tmpURL[256];
    long offset = file.getLogicalPosition() + numSubSets * sizeof( long );
    bool bFoundOffset =0;

    // pad for header info
    for ( int i = 0; i < numSubSets; i++ )
    {
        
        file.readString( (BYTE*)tmpURL );
        if ( _stricmp( tmpURL, strButtonTextNames ) == 0 )
        {
            offset = file.readLong();
            bFoundOffset = true;
            break;
        }
    
        offset = file.readLong();        

    }

 //   ASSERT( bFoundOffset );

    file.seek( offset );

      // now create the smart objects
    CreateSmartObjects(&file);

	// Cleanup allocated memory
    delete [] pSysMemData;
    pSysMemData = NULL;
    
 // Done with the file
    file.close( );


 
    return S_OK;
}

HRESULT CPrimitiveScene::CreateSmartObjects( File* pFile )
{
    ASSERT( pFile != NULL );
	HRESULT hr = S_OK;
	
    long buttonCount, textCount, listBoxCount, spinnerCount;

    textCount       = pFile->readLong();
    buttonCount     = pFile->readLong();
// TBD remove firstVert, firstHoriz
    long firstVert = pFile->readLong();
    long firstHoriz = pFile->readLong();
    listBoxCount    = pFile->readLong();
    spinnerCount    = pFile->readLong();

    long nTextOffset = pFile->readLong( );
    long buttonOffset = pFile->readLong( );
    long nListBoxOffset = pFile->readLong( );
    long nSpinnerOffset = pFile->readLong( );

    // I can't stand to watch the stl allocate lots of little chunks and recopy all the objects
    // for each push back...

    m_pTexts.resize( textCount );
    m_pButtons.resize( buttonCount );
    m_pSpinners.resize( spinnerCount );

	////////////////////////////// 

	do
	{
		// first create all the Text objects
		for (long i = 0; i< textCount; i++)
		{
			CText* pNewText = new CText;
			if(!pNewText)
			{
				hr = E_OUTOFMEMORY;
				BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to create text");	
			}
				

			hr = pNewText->Initialize();
			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to Init a text");

			hr = pNewText->LoadFromXBG( pFile, m_pRoot );
			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to LoadFromXBG a text");

			m_pTexts[i] = (pNewText);
		}
		// now find a HelpText Text
		bool bHelpTextFound = false;
		for (long i=0; i<textCount;i++)
		{
			if(m_pTexts[i]->IsHelpText())
			{
				m_pHelpText = m_pTexts[i];
				// only one help text per scene is allowed
				ASSERT(!bHelpTextFound);
				bHelpTextFound = true;
			}
		}

		if (bHelpTextFound)
		{
			DbgPrint("CPrimitiveScene::CreateSmartObjects - helpText found");
		}
		else
		{
			DbgPrint("CPrimitiveScene::CreateSmartObjects - no helpText found");
		}

		// create all the buttons
		for (long i = 0; i < buttonCount; i++ )
		{
			CButton* pNewButton = NULL;
			long nPos = pFile->getLogicalPosition();
			long nSize = pFile->readLong();
			char nInputObjectTextId = pFile->readByte();
			
			if(nInputObjectTextId!=-1)
			{
				pNewButton = new CInputButton;
			}
			else
			{
				pNewButton = new CButton;
			}
			// put the pointer back
			pFile->seek(nPos);
			if (!pNewButton)
			{
				hr = E_OUTOFMEMORY;
				BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to create a button");	
			}

			hr = pNewButton ->Initialize();
			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to Init a button");

			hr = pNewButton->LoadFromXBG( pFile, m_pRoot );
			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to LoadFromXBG a button");
			
			m_pButtons[i] = (pNewButton);
			
			CText* pText = NULL;
			CText* pInputText = NULL;
			if ( m_pButtons[i]->m_textObjId != -1 )
			{
				pText = m_pTexts[m_pButtons[i]->m_textObjId];
			}
			if (m_pButtons[i]->m_textObjId2 !=-1)
			{
				pInputText = m_pTexts[m_pButtons[i]->m_textObjId2];
			}

			m_pButtons[i]->SetTextObjects(pText, pInputText);
			m_nActiveButtonIndex = 0;
		}
		
		int nVertButtons = CountButtons(eButtonVerticalOrientation);
		int nHorizButtons = CountButtons(eButtonHorizontalOrientation);
		if (nVertButtons > 0 && nHorizButtons > 0)
		{// do not allow vertical and horizontal buttons on the same page except of 
		 // the Keyboard, NTS_Settings. DO NOT REMOVE THE ASSERT. IT MEANS THAT YOUR CLASS
		// NEEDS TO OVERLOAD FRAMEMOVE(), BECAUSE YOUR NAVIGATION IS NOT CONSIDERED STANADARD
		// ADD YOUR SCENE TO THE IF

			if(m_eSceneId != eKeyboardSceneId && m_eSceneId != eNTS_SettingsId &&
               m_eSceneId != eKeypadSceneId   && m_eSceneId != eKeypadPeriodSceneId &&
               m_eSceneId != eACT_BillingId   && m_eSceneId != eACT_CreditId)
			{
				ASSERT(false);
			}
		}
		
		
		nVertButtons > 0? m_bVerticalButtons = true : m_bVerticalButtons = false;
		
		SetFocus();

        // more than one would be a little bit wierd here...
        // we can't currently handle that many object highlighted, so
        // we'll implement that when (if) the time comes.
        ASSERT( listBoxCount < 2 );
        if ( listBoxCount )
        {
            m_pListBox = new CListBox();

            if ( !m_pListBox )
            {
                hr = E_OUTOFMEMORY;
				BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to create list box");	
            }

            hr = m_pListBox->Initialize();
			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to Init a list box");

            hr = m_pListBox->LoadFromXBG( pFile, m_pRoot, &m_pButtons[0], m_pButtons.size() );
   			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to Load a list box");

        }

        // The number of spinners, and the number of buttons must be equal
        for (long i = 0; i < spinnerCount; i++)
        {
            CSpinner *pNewSpinner = new CSpinner();
            if ( !pNewSpinner )
            {
                hr = E_OUTOFMEMORY;
				BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to create spinner");	
            }

            hr = pNewSpinner->Initialize();
			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to Init a spinner");

            hr = pNewSpinner->LoadFromXBG( pFile, m_pRoot, &m_pButtons[0], i );
   			BREAKONFAIL(hr, "CPrimitiveScene::CreateSmartObjects - Fail to Load a spinner");

            m_pSpinners[i] = (pNewSpinner);
        }
       


	} while(0);
	if (FAILED(hr))
	{
		Cleanup();
	}

	return hr;

}   


int CPrimitiveScene::CountButtons(eButtonOrientation eOrientation)
{
	int nButtons = 0;
	for (size_t i=0; i<m_pButtons.size(); i++)
	{
		if(m_pButtons[i]->GetOrientation() == eOrientation)
		{
			nButtons++;
		}
	
	}
	return nButtons;
}


void CPrimitiveScene::Dump(bool bAllData)
{	

	if(bAllData)
	{
		DbgPrint("=============================================================\n");
		DbgPrint("Scene Dump\n");
		DbgPrint("=============================================================\n");
		DbgPrint("SceneName: [%s]\n", SceneName[m_eSceneId]);
		
		DbgPrint("Vertical Buttons scene: [%s]\n", m_bVerticalButtons? "TRUE":"FALSE");
		for(size_t i = 0; i< m_pButtons.size(); i++)
		{	
			DbgPrint("\n-----------------------------------------------------------\n");
			m_pButtons[i]->Dump(bAllData);
		}
		DbgPrint("\nHelpText:");
		if (m_pHelpText)
		{
			m_pHelpText->Dump(bAllData);
		}
		else
		{
			DbgPrint("[null]");
		}

		DbgPrint("\n");
	}
				
}

const FLOAT JOY_THRESHOLD = 0.25f;
bool CPrimitiveScene::IsAnyButtonActive( const XBGAMEPAD* pGamePad )
{
    // Check digital buttons
    if( pGamePad->wButtons )
        return true;

    // Check analog buttons
    for( DWORD i = 0; i < 8; ++i )
    {
        if( pGamePad->bAnalogButtons[ i ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            return true;
    }

    // Check thumbsticks
    if( pGamePad->fX1 >  JOY_THRESHOLD ||
        pGamePad->fX1 < -JOY_THRESHOLD ||
        pGamePad->fY1 >  JOY_THRESHOLD ||
        pGamePad->fY1 < -JOY_THRESHOLD )
    {
        return true;
    }

    if( pGamePad->fX2 >  JOY_THRESHOLD ||
        pGamePad->fX2 < -JOY_THRESHOLD ||
        pGamePad->fY2 >  JOY_THRESHOLD ||
        pGamePad->fY2 < -JOY_THRESHOLD )
    {
        return true;
    }

    // Nothing active
    return false;
}

void CPrimitiveScene::SetActiveButton(bool bGoUp)
{
	if (m_pButtons.size())
	{
        long oldIndex = m_nActiveButtonIndex;

        // go from disabled but highlighted to disabled
        if ( m_pButtons[m_nActiveButtonIndex]->GetState() == eButtonDisabledHighlighted )
        {
            m_pButtons[m_nActiveButtonIndex]->SetState( eButtonDisabled );
        }
        else
        {
		    m_pButtons[m_nActiveButtonIndex]->SetState(eButtonDefault);
        }
		bGoUp ? FindPrevActiveButton() : FindNextActiveButton();

        if ( m_nActiveButtonIndex != oldIndex )
        {
            eSoundType  soundType = m_pButtons[m_nActiveButtonIndex]->GetSoundType();
            PlaySound( soundType, eSoundHighlight );
        }

		// go from disabled to disabled yet highlighted
        if ( eButtonDisabled == m_pButtons[m_nActiveButtonIndex]->GetState() )
        {
            m_pButtons[m_nActiveButtonIndex]->SetState( eButtonDisabledHighlighted );
        }
        else
        {
            m_pButtons[m_nActiveButtonIndex]->SetState(eButtonHighlighted);
        }

		if (m_pHelpText)
		{
			m_pHelpText->SetTextId(m_pButtons[m_nActiveButtonIndex]->GetHelpText());
		}
		AutoTest(eChangeFocus);
	}
}


int CPrimitiveScene::FindButton(eButtonId eButton)
{
	
	for(size_t nIndex = 0; nIndex < m_pButtons.size(); nIndex++)
	{
		if(eButton == m_pButtons[nIndex]->GetButtonId())
			break;
	}
	if(nIndex == m_pButtons.size())
	{
		return -1;
	}

	return nIndex;
}

int CPrimitiveScene::FindTextObjForId(const TCHAR* szTextId)
{
	ASSERT(szTextId);
	if(!szTextId)
	{
		return -1;
	}
	for(size_t nIndex = 0; nIndex < m_pTexts.size(); nIndex++)
	{
		if(!m_pTexts[nIndex]->GetTextId())
		{
			continue;
		}
		if(_tcsicmp(m_pTexts[nIndex]->GetTextId(), szTextId) == 0)
			break;
	}
	if(nIndex == m_pTexts.size())
	{
		nIndex = -1;
	}
	return nIndex;
}

int CPrimitiveScene::FindTextObjForShape(const char* szNodeName)
{
	ASSERT(szNodeName);
	if(!szNodeName)
	{
		return -1;
	}
	for(size_t nIndex = 0; nIndex < m_pTexts.size(); nIndex++)
	{
		TG_Shape* pAppearance =const_cast<TG_Shape*> (m_pTexts[nIndex]->GetShape());
		if(_stricmp(pAppearance->GetNodeName(), szNodeName) == 0)
			break;
	}
	if(nIndex == m_pTexts.size())
	{
		nIndex = -1;
	}
	return nIndex;
}

eInputState CPrimitiveScene::GetInputState(XBGAMEPAD& GamepadInput)
{	
	if( IsAnyButtonActive(&GamepadInput) )
    {
        // If the timer is running, the button is being held. If it's
        // held long enough, it triggers a repeat. If the timer isn't
        // running, we start it.
        if( m_RepeatTimer.IsRunning() )
        {
            // If the timer is running but hasn't expired, bail out
            if( m_RepeatTimer.GetElapsedSeconds() < m_fRepeatDelay )
                return eNoInput;
			// timer expired, receive new input
            m_fRepeatDelay = fSTD_REPEAT;
            m_RepeatTimer.StartZero();
			return eRepetitivePress;
        }
        else
        {
            m_fRepeatDelay = fINITIAL_REPEAT;
            m_RepeatTimer.StartZero();
			return eFirstTimePress;
        }
    }
    else
    {
        // No buttons or joysticks active; kill the repeat timer
        m_fRepeatDelay = fINITIAL_REPEAT;
        m_RepeatTimer.Stop();
		return eNoInput;
    }
	
}

int CPrimitiveScene::FindButtonForTextObj(const TCHAR* szTextId)
{
	// non localized strings do not have Ids
	if(!szTextId)
	{
		return -1;
	}
	int nIndex; 
	for(nIndex = 0; nIndex < (int) m_pButtons.size(); nIndex++)
	{
		CText* pText = const_cast<CText*> (m_pButtons[nIndex]->GetTextObject());

        // don't crash if we don't find it.
        if ( !pText )
            continue;
		// non localized strings do not have Ids
		if(!pText->GetTextId())
			continue;

		if(_tcsicmp(pText->GetTextId(), szTextId) == 0)
			break;
	}
	if( nIndex == m_pButtons.size())
	{
		nIndex = -1;
	}
	return nIndex;
}

void CPrimitiveScene::AutoTest(eAutoTestData eState)
{
	if(m_eSceneId >= countof(SceneName))
	{
		DbgPrint("CPrimitiveScene::AutoTest - wrong screenId\n");
		return;
	}

	switch(eState)
	{
	case eSceneInit:
		DbgPrint("\nAUTO:SCREEN=SCREEN_ID[%d], SCREEN_NAME[%s]\n",m_eSceneId, SceneName[m_eSceneId]);
		for(size_t i = 0; i< m_pButtons.size(); i++)
		{	
			m_pButtons[i]->AutoTest();
		}

		for(size_t i = 0; i< m_pTexts.size(); i++)
		{	
			int nIndex = FindButtonForTextObj(m_pTexts[i]->GetTextId());
			if (nIndex == -1)
			{
				m_pTexts[i]->AutoTest();
			}
		}
		break;
	case eChangeFocus:
		eButtonId eFocusButton = m_pButtons[m_nActiveButtonIndex]->GetButtonId();
		ASSERT(eFocusButton < countof(ButtonName));
		DbgPrint("\nAUTO:FOCUS=BUTTON_ID[%d], BUTTON_NAME[%s]\n",eFocusButton, ButtonName[eFocusButton]);
		if (m_pHelpText)
		{
			const TCHAR* pszHelp = m_pHelpText->GetTextId();
			char szBuf[256];
			ZeroMemory(szBuf, countof(szBuf));
			Ansi(szBuf,pszHelp, min(countof(szBuf)-1, _tcslen(pszHelp)) );
			DbgPrint("\nAUTO:TEXT=HELP_TEXT[%s]\n",szBuf);
		}
		break;

	}
}

void CPrimitiveScene::AutoTest(XBGAMEPAD& GamepadInput)
{
	if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
			GamepadInput.fY1 == -1)
	{
		DbgPrint("\nAUTO:INPUT=DOWN\n");
	}
	if (GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ||
			GamepadInput.fY1 == 1)
	{
			DbgPrint("\nAUTO:INPUT=UP\n");
	}
	if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
		   GamepadInput.fX1 == 1)
	{
		DbgPrint("\nAUTO:INPUT=RIGHT\n");
	}
	if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
			GamepadInput.fX1 == -1)
	{
		DbgPrint("\nAUTO:INPUT=LEFT\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || 
		GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START)
	{
		DbgPrint("\nAUTO:INPUT=START/A\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
	{
		DbgPrint("\nAUTO:INPUT=BACK/B\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_X])
	{
		DbgPrint("\nAUTO:INPUT=X\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
	{
		DbgPrint("\nAUTO:INPUT=Y\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
	{
		DbgPrint("\nAUTO:INPUT=BLACK\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
	{
		DbgPrint("\nAUTO:INPUT=WHITE\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
	{
			DbgPrint("\nAUTO:INPUT=LEFT_TRIGGER\n");
	}
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
	{
		DbgPrint("\nAUTO:INPUT=RIGHT_TRIGGER\n");
	}
}


int CPrimitiveScene::FindNextActiveButton()
{
	int nOrigPos = m_nActiveButtonIndex; 

	while (1)
	{

		if(	++m_nActiveButtonIndex >= (int) m_pButtons.size())
		{
			break;
		}
		if(!m_pButtons[m_nActiveButtonIndex]->IsShowing())
		{
			continue;
		}
		break;
	}
	// all the rest buttons are hidden, revert to the orig position
	if(m_nActiveButtonIndex >= (int) m_pButtons.size())
	{
		m_nActiveButtonIndex = nOrigPos;
	}
	
	return m_nActiveButtonIndex;
}

int CPrimitiveScene::FindPrevActiveButton()
{
	int nOrigPos = m_nActiveButtonIndex; 
	
	while(1)
	{
		if(--m_nActiveButtonIndex < 0)
		{
			break;
		}

		if(!m_pButtons[m_nActiveButtonIndex]->IsShowing())
		{
			continue;
		}
		break;
	}

		// all the rest buttons are hidden, revert to the orig position
	if(m_nActiveButtonIndex < 0)
	{
		m_nActiveButtonIndex = nOrigPos;
	}
	
	return m_nActiveButtonIndex;
}

void CPrimitiveScene::PlaySound( eSoundType type, eSound whichOne )
{
    ASSERT( type < eLastSoundType && whichOne < 4 );
   

    if ( !s_AudioFiles[type][whichOne] )
    {
        s_AudioFiles[type][whichOne] = new CAudioClip();
        if ( !s_AudioFiles[type][whichOne] )
        {
            // OUT OF MEMORY??
            ASSERT( !"CPrimitiveScene::PlaySound memory error" );
            return;
        }

        TCHAR* url = SoundInfo[type][whichOne];
        if ( url )
        {
             s_AudioFiles[type][whichOne]->SetUrl( url );
        }
    }

    if ( s_AudioFiles[type][whichOne] )
    {
        s_AudioFiles[type][whichOne]->Play();
    }
}

void CPrimitiveScene::CleanupSounds()
{
    for ( int i = 0; i < eLastSound; i++ )
    {
        for ( int j = 0; j < eLastSoundType; j++ )
        {
            if ( s_AudioFiles[j][i] )
            {
                delete s_AudioFiles[j][i];
            }

            s_AudioFiles[j][i] = NULL;
        }
    }
}

void CPrimitiveScene::SetActiveButton(eButtonId eButton)
{
	int nIndex = FindButton(eButton);
	ASSERT(nIndex!=-1);
	if(-1 == nIndex)
	{
		return;
	}

	if(!m_pButtons[nIndex]->IsShowing())
	{
		DbgPrint("CPrimitiveScene::SetActiveButton - can't set invisible button to be active Id = %d\n", eButton);
		return;
	}

	if (m_pButtons.size())
	{
        long oldIndex = m_nActiveButtonIndex;
		
        // go from disabled but highlighted to disabled
        if ( m_pButtons[m_nActiveButtonIndex]->GetState() == eButtonDisabledHighlighted )
        {
            m_pButtons[m_nActiveButtonIndex]->SetState( eButtonDisabled );
        }
        else
        {
		    m_pButtons[m_nActiveButtonIndex]->SetState(eButtonDefault);
        }

		m_nActiveButtonIndex = nIndex;

        if ( m_nActiveButtonIndex != oldIndex )
        {
            eSoundType  soundType = m_pButtons[m_nActiveButtonIndex]->GetSoundType();
            PlaySound( soundType, eSoundHighlight );
        }

		// go from disabled to disabled yet highlighted
        if ( eButtonDisabled == m_pButtons[m_nActiveButtonIndex]->GetState() )
        {
            m_pButtons[m_nActiveButtonIndex]->SetState( eButtonDisabledHighlighted );
        }
        else
        {
            m_pButtons[m_nActiveButtonIndex]->SetState(eButtonHighlighted);
        }
		if (m_pHelpText)
		{
			m_pHelpText->SetTextId(m_pButtons[m_nActiveButtonIndex]->GetHelpText());
		}
		AutoTest(eChangeFocus);
	}
}


void CPrimitiveScene::SetFocus()
{
	// highlight first button
	if(!m_pButtons.size())
	{
		return;
	}
	
	if(!m_pButtons[m_nActiveButtonIndex]->IsShowing())
	{
		FindNextActiveButton();	
	}

    if ( m_pButtons[m_nActiveButtonIndex]->GetState() == eButtonDisabled )
    {
        m_pButtons[m_nActiveButtonIndex]->SetState(eButtonDisabledHighlighted);
    }
    else
    {
    	m_pButtons[m_nActiveButtonIndex]->SetState(eButtonHighlighted);
    }

	if (m_pHelpText)
	{
		m_pHelpText->SetTextId(m_pButtons[0]->GetHelpText());
	}
}