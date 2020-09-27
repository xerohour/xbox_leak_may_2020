/********************************************************************************
FILE:
    SOUND3D.cpp

PURPOSE:
    This wraps both a DSound and a DSoundBuffer object, so that it can abstract
    the positioning calls.

BY:
    DANHAFF
********************************************************************************/
#include "xtl.h"
#include "SOUND3D.h"

HRESULT FreeDSBD(LPDSBUFFERDESC &pdsbd);
HRESULT LoadWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer, char *szFileName);
HRESULT LoadRandomWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer);



/********************************************************************************
Initializes variables in the function like normal code does.
********************************************************************************/
SOUND3D::SOUND3D(void)
{

    HRESULT hr  = S_OK;
    m_pDS       = NULL;
    m_pDSB      = NULL;

    //Set up as if you had called DSB_TestMode(TESTAPPLY_IMMEDIATE)
    m_dwActualApplyValue = DS3D_IMMEDIATE;   
    m_eApply    =          TESTAPPLY_IMMEDIATE;
    m_bCommit   =          FALSE;


    //Set up the positioning stuff.
    m_vLisPos   = x_D3DVECTOR(0, 0, 0);
    m_vBufPos   = x_D3DVECTOR(0, 0, 0);
 
    //Relative offset positioning stuff.
    m_vRelPos   = x_D3DVECTOR(0, 0, 0);
    m_bUsingRelPos = FALSE;

    //Storage of sound data.
	m_pvSoundData = NULL;
       
};




/********************************************************************************

********************************************************************************/
HRESULT SOUND3D::Init(D3DVECTOR vRelative, char *szFileName)
{
    HRESULT hr = S_OK;

    //Create the DSound object (our "listener")

#ifdef SILVER
    CHECKRUN(Help_DirectSoundCreate(DSDEVID_DEVICEID_MCPX, &m_pDS, NULL));
#else // SILVER
	CHECKRUN(Help_DirectSoundCreate( 0, &m_pDS, NULL ) );
#endif // SILVER

	if ( SUCCEEDED( hr ) )
	{
		//hr= DownloadScratch(m_pDS, "T:\\Media\\DSPCode\\DSSTDFX.bin" );
		hr= DownloadLinkedDSPImage(m_pDS);
	}

	if ( SUCCEEDED( hr ) ) 
	{

        //If we've specified a filename it's nice and simple; we load it.
        if (szFileName)
        {
            CHECKRUN(LoadWaveFile(NULL, &m_pvSoundData, &m_pDSB, szFileName));
        }
        
        //If not, we need to find a random wave file on the drive and load that.
        else
        {
            CHECKRUN(LoadRandomWaveFile(NULL, &m_pvSoundData, &m_pDSB));
        } //end "if szFileName else"

    }

    //Are we relativizing our listener and source?
    m_vRelPos = vRelative;
    if (vRelative == x_D3DVECTOR(0, 0, 0))
        m_bUsingRelPos = FALSE;
    else
        m_bUsingRelPos = TRUE;

    return hr;

};

/********************************************************************************
DESTRUCTOR
********************************************************************************/
SOUND3D::~SOUND3D(void)
{
//Release buffers
RELEASE(m_pDSB);
RELEASE(m_pDS);

//Free up the sound data.
if (m_pvSoundData)
#ifndef DVTSNOOPBUG
    free(m_pvSoundData);
#else
  XPhysicalFree( m_pvSoundData );
#endif

}


/********************************************************************************
Takes debug output and appends stuff to it based on whether we're de
or not.
********************************************************************************/
void SOUND3D::ConsoleOut(CHAR *szFormat, ...)
{

    va_list va;
    static char szBuffer[1000];

    if (NULL == this)
        DbgPrint("SOUND3D ptr is NULL!!!!");


    CHAR *pszWarningDeferred= "(may not apply: Using DEFERRED w/o CommittDeferredSettings)";

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    CONSOLE("%s%s",
            szBuffer, 
            (TESTAPPLY_DEFERRED_NOUPDATE == m_eApply)  ? pszWarningDeferred: ""
            );

};


/********************************************************************************
PURPOSE:
    This tells the wrapper class how to proxy position changes, etc. 
********************************************************************************/
HRESULT SOUND3D::DSB_Test_SetTestingApply(TestApplies dwApply)
{
    HRESULT hr = S_OK;
    
    //Store our apply methodology.
    m_eApply    = dwApply;
        
    //Calculate our actual apply value.
    m_dwActualApplyValue = 0;
    switch (dwApply)
    {
        case TESTAPPLY_DEFERRED_NOUPDATE:
            m_dwActualApplyValue = DS3D_DEFERRED;
            break;
        case TESTAPPLY_DEFERRED_UPDATE:
            m_dwActualApplyValue = DS3D_DEFERRED;
            break;
        case TESTAPPLY_IMMEDIATE:
            m_dwActualApplyValue = DS3D_IMMEDIATE;
            break;
        case DS3D_IMMEDIATE:
        case DS3D_DEFERRED:
            m_dwActualApplyValue = dwApply;
        default: 
            LogBoth(XLL_FAIL, "ERROR: Invalid parameter dwApply=%d passed to GetActualApplyValue", dwApply);
            m_dwActualApplyValue = 0xFFFFFFFF;
            break;
    }

    //Store our commit methodology (do we call CommitDeferredSettings?)
    if (TESTAPPLY_DEFERRED_UPDATE == dwApply)
        m_bCommit = TRUE;
    else
        m_bCommit = FALSE;

    return hr;
};




/********************************************************************************
This intercepts the position call and relativizes the vectors based on the
relative position if any.  
********************************************************************************/
HRESULT SOUND3D::DS_SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;
    m_vLisPos = x_D3DVECTOR(x, y, z);
    CHECKRUN(DS_SetActualPositions());
    return hr;
};




/********************************************************************************
PURPOSE:
    Given our object's apparent positions, this function will adjust them as
    follows:

    1)  The relative position (m_vRelPos) is subtracted from both the listener
    and the buffer position.
********************************************************************************/
HRESULT SOUND3D::DS_SetActualPositions(void)
{
    HRESULT hr = S_OK;
    D3DVECTOR vActualLisPos =   m_vLisPos;
    D3DVECTOR vActualBufPos =   m_vBufPos;

	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    //1)  The relative position (m_vRelPos) is subtracted from both the listener
    //and the buffer position.
    if (m_bUsingRelPos)
    {
        vActualLisPos  -= m_vRelPos;
        vActualBufPos  -= m_vRelPos;
    }

    //Set the positions and apply if necessasry.

    CHECKRUN(m_pDS-> SetPosition(vActualLisPos.x, vActualLisPos.y, vActualLisPos.z, m_dwActualApplyValue));
    CHECKRUN(m_pDSB->SetPosition(vActualBufPos.x, vActualBufPos.y, vActualBufPos.z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());


    return hr;
}


/********************************************************************************
********************************************************************************/
HRESULT SOUND3D::DS_SetAllParameters(LPCDS3DLISTENER pds3dl)
{
HRESULT hr = S_OK;
D3DVECTOR vActualLisPos =   {0, 0, 0};
D3DVECTOR vActualBufPos =   {0, 0, 0};
DS3DLISTENER ds3dl;

	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

ZeroMemory(&ds3dl, sizeof(ds3dl));

//Save the listener data.
ds3dl = *pds3dl;

//Save the current listener position to our class.
m_vLisPos = ds3dl.vPosition;

//1)  The relative position (m_vRelPos) is subtracted from both the listener
//and the buffer position.
if (m_bUsingRelPos)
{
    vActualLisPos = m_vLisPos - m_vRelPos;
    vActualBufPos = m_vBufPos - m_vRelPos;
}
else
{
    vActualLisPos = m_vLisPos;
    vActualBufPos = m_vBufPos;
}

//Stick the new values into the struct.
ds3dl.vPosition = vActualLisPos;

//Set the listener parameters like we're supposed to.
CHECKRUN(m_pDS->SetAllParameters(pds3dl, m_dwActualApplyValue));

//If we're doing some automatic adjustments than set the buffer position too, otherwise leave it alone.
if (m_bUsingRelPos/* || DS3DMODE_HEADRELATIVE == m_dwActualTestMode*/)
{
    CHECKRUN(m_pDSB->SetPosition(vActualBufPos.x, vActualBufPos.y, vActualBufPos.z, m_dwActualApplyValue));
}

//Commit
if (m_bCommit)
    CHECKRUN(m_pDS->CommitDeferredSettings());
    
return hr;
};


/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DS_SetDistanceFactor(FLOAT flDistanceFactor)
{
    HRESULT hr = S_OK;  
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDS->SetDistanceFactor(flDistanceFactor, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DS_SetDopplerFactor(FLOAT flDopplerFactor)
{
    HRESULT hr = S_OK; 
	
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDS->SetDopplerFactor(flDopplerFactor, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DS_SetOrientation(FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop)
{
    HRESULT hr = S_OK; 
	
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDS->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DS_SetRolloffFactor(FLOAT flRolloffFactor)
{
    HRESULT hr = S_OK;  
	
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDS->SetRolloffFactor(flRolloffFactor, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DS_SetVelocity(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;
    
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDS->SetVelocity(x, y, z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}



/********************************************************************************
Just like DS_SetAllParameters
********************************************************************************/
HRESULT SOUND3D::DSB_SetAllParameters(LPCDS3DBUFFER pds3db)
{
    HRESULT hr = S_OK;
    D3DVECTOR vActualLisPos =   {0, 0, 0};
    D3DVECTOR vActualBufPos =   {0, 0, 0};
    DS3DBUFFER ds3db;

    ZeroMemory(&ds3db, sizeof(ds3db));

	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;
    

    //Save the buffer data.
    ds3db = *pds3db;

    //Save the current buffer position to our class.
    m_vBufPos = ds3db.vPosition;

    //1)  The relative position (m_vRelPos) is subtracted from both the listener
    //and the buffer position.
    if (m_bUsingRelPos)
    {
        vActualLisPos = m_vLisPos - m_vRelPos;
        vActualBufPos = m_vBufPos - m_vRelPos;
    }
    else
    {
        vActualLisPos = m_vLisPos;
        vActualBufPos = m_vBufPos;
    }

    //Stick the new values into the struct.
    ds3db.vPosition = vActualBufPos;

    //If we're doing some automatic adjustments than set the listener position too, otherwise leave it alone.
    if (m_bUsingRelPos)
    {
        CHECKRUN(m_pDS->SetPosition(vActualLisPos.x, vActualLisPos.y, vActualLisPos.z, m_dwActualApplyValue));
    }

    //Set the structure.
    CHECKRUN(m_pDSB->SetAllParameters(&ds3db, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DSB_SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle)
{
    HRESULT hr = S_OK;    

	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDSB->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
*********************************************************************************/
HRESULT SOUND3D::DSB_SetConeOrientation(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;   
	
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDSB->SetConeOrientation(x, y, z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DSB_SetConeOutsideVolume(LONG lConeOutsideVolume)
{
    HRESULT hr = S_OK;    

	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDSB->SetConeOutsideVolume(lConeOutsideVolume, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DSB_SetMaxDistance(FLOAT flMaxDistance)
{
    HRESULT hr = S_OK;  
	
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDSB->SetMaxDistance(flMaxDistance, m_dwActualApplyValue))
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DSB_SetMinDistance(FLOAT flMinDistance)
{
    HRESULT hr = S_OK; 
	
	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDSB->SetMinDistance(flMinDistance, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DSB_SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
HRESULT hr = S_OK;

//Set the internal variable.
m_vBufPos = x_D3DVECTOR(x, y, z);

//Finally, set the position.
CHECKRUN(DS_SetActualPositions());

return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT SOUND3D::DSB_SetVelocity(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;    

	if ( NULL == m_pDS || NULL == m_pDSB )
		return hr;

    CHECKRUN(m_pDSB->SetVelocity(x, y, z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}



