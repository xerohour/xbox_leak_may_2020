/********************************************************************************
FILE:
    BUFFER3D.cpp

PURPOSE:
    This wraps both a DSound and a DSoundBuffer object, so that it can abstract
    the positioning calls.

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "Help_Buffer3D.h"



/********************************************************************************
Initializes variables in the function like normal code does.
********************************************************************************/
DMBUFFER::BUFFER3D::BUFFER3D(void)
{

    HRESULT hr  = S_OK;
    m_pDS       = NULL;
    m_pDSB      = NULL;

    //Set up as if you had called DSB_TestMode(TESTAPPLY_IMMEDIATE)
    m_dwActualApplyValue = DS3D_IMMEDIATE;   
    m_eApply    =          TESTAPPLY_IMMEDIATE;
    m_bCommit   =          FALSE;

    //Set up the positioning stuff.
    m_vLisPos   = make_D3DVECTOR(0, 0, 0);
    m_vBufPos   = make_D3DVECTOR(0, 0, 0);
 
    //Relative offset positioning stuff.
    m_vRelPos   = make_D3DVECTOR(0, 0, 0);
    m_bUsingRelPos = FALSE;

};




/********************************************************************************

********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::Init(D3DVECTOR vRelative, IDirectSoundBuffer *pDSB)
{
    HRESULT hr = S_OK;
    static char szFullPath[1000];
    WIN32_FIND_DATA Data = {0};
    DWORD dwCurrentFile     = 0;
    DWORD dwChosenFile      = 0;
    BOOL bRes               = FALSE;

    //Create the DSound object (our "listener")
	CHECK(DMHelp_DirectSoundCreate( 0, &m_pDS, NULL ) );

	if ( SUCCEEDED( hr ) ) 
	{
        m_pDSB = pDSB;
    }


    //Are we relativizing our listener and source?
    m_vRelPos = vRelative;
    if (vRelative == make_D3DVECTOR(0, 0, 0))
        m_bUsingRelPos = FALSE;
    else
        m_bUsingRelPos = TRUE;

    return hr;

};

/********************************************************************************
DESTRUCTOR
********************************************************************************/
DMBUFFER::BUFFER3D::~BUFFER3D(void)
{
//Release buffers
RELEASE(m_pDSB);
RELEASE(m_pDS);

}


/********************************************************************************
Takes debug output and appends stuff to it based on whether we're de
or not.
********************************************************************************/
/*
void BUFFER3D::ConsoleOut(CHAR *szFormat, ...)
{

    va_list va;
    static char szBuffer[1000];

    if (NULL == this)
        Log(ABORTLOGLEVEL, "BUFFER3D ptr is NULL!!!!");


    CHAR *pszWarningDeferred= "(may not apply: Using DEFERRED w/o CommittDeferredSettings)";

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    Log(FYILOGLEVEL, szBuffer);
    va_end(va);

};
*/

/********************************************************************************
PURPOSE:
    This tells the wrapper class how to proxy position changes, etc. 
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_Test_SetTestingApply(TestApplies dwApply)
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
            Log(ABORTLOGLEVEL, "ERROR: Invalid parameter dwApply=%d passed to GetActualApplyValue", dwApply);
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
HRESULT DMBUFFER::BUFFER3D::DS_SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;
    m_vLisPos = make_D3DVECTOR(x, y, z);
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
HRESULT DMBUFFER::BUFFER3D::DS_SetActualPositions(void)
{
    HRESULT hr = S_OK;
    D3DVECTOR vActualLisPos =   m_vLisPos;
    D3DVECTOR vActualBufPos =   m_vBufPos;

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
HRESULT DMBUFFER::BUFFER3D::DS_SetAllParameters(LPCDS3DLISTENER pds3dl)
{
HRESULT hr = S_OK;
D3DVECTOR vActualLisPos =   {0, 0, 0};
D3DVECTOR vActualBufPos =   {0, 0, 0};
DS3DLISTENER ds3dl;

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
HRESULT DMBUFFER::BUFFER3D::DS_SetDistanceFactor(FLOAT flDistanceFactor)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDS->SetDistanceFactor(flDistanceFactor, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DS_SetDopplerFactor(FLOAT flDopplerFactor)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDS->SetDopplerFactor(flDopplerFactor, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DS_SetOrientation(FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDS->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DS_SetRolloffFactor(FLOAT flRolloffFactor)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDS->SetRolloffFactor(flRolloffFactor, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DS_SetVelocity(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDS->SetVelocity(x, y, z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}



/********************************************************************************
Just like DS_SetAllParameters
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetAllParameters(LPCDS3DBUFFER pds3db)
{
    HRESULT hr = S_OK;
    D3DVECTOR vActualLisPos =   {0, 0, 0};
    D3DVECTOR vActualBufPos =   {0, 0, 0};
    DS3DBUFFER ds3db;

    ZeroMemory(&ds3db, sizeof(ds3db));
    

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
HRESULT DMBUFFER::BUFFER3D::DSB_SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDSB->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
*********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetConeOrientation(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDSB->SetConeOrientation(x, y, z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetConeOutsideVolume(LONG lConeOutsideVolume)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDSB->SetConeOutsideVolume(lConeOutsideVolume, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetMaxDistance(FLOAT flMaxDistance)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDSB->SetMaxDistance(flMaxDistance, m_dwActualApplyValue))
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetMinDistance(FLOAT flMinDistance)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDSB->SetMinDistance(flMinDistance, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
HRESULT hr = S_OK;

//Set the internal variable.
m_vBufPos = make_D3DVECTOR(x, y, z);

//Finally, set the position.
CHECKRUN(DS_SetActualPositions());

return hr;
}

/********************************************************************************
Simple wrapper function; performs call based on m_eApply (which translates
into m_dwActualApplyValue and m_bCommit).
********************************************************************************/
HRESULT DMBUFFER::BUFFER3D::DSB_SetVelocity(FLOAT x, FLOAT y, FLOAT z)
{
    HRESULT hr = S_OK;    
    CHECKRUN(m_pDSB->SetVelocity(x, y, z, m_dwActualApplyValue));
    if (m_bCommit)
        CHECKRUN(m_pDS->CommitDeferredSettings());
    return hr;
}








/********************************************************************************
Note: These are crap functions cuz the D3DOVERLOADS don't work.
BUG 2371 Overloaded functions unusable on D3DVECTOR due to inclusion of less functional D3DVECTOR class in D3D8TYPES.H
********************************************************************************/
D3DVECTOR DMBUFFER::operator - (const D3DVECTOR& v, const D3DVECTOR& w)
{
    D3DVECTOR a;
    a.x = v.x - w.x;
    a.y = v.y - w.y;
    a.z = v.z - w.z;
    return a;    
};

D3DVECTOR DMBUFFER::operator -= (D3DVECTOR& v, const D3DVECTOR& w)
{
    v.x -= w.x;
    v.y -= w.y;
    v.z -= w.z;
    return v;    
};

BOOL DMBUFFER::operator == (D3DVECTOR& v, const D3DVECTOR& w)
{
    return
    (
    v.x == w.x &&
    v.y == w.y &&
    v.z == w.z
    );
};


//constructor
D3DVECTOR DMBUFFER::make_D3DVECTOR(FLOAT _x, FLOAT _y, FLOAT _z)
{
D3DVECTOR v;

v.x = _x;
v.y = _y;
v.z = _z;
return v;
}


/********************************************************************************
********************************************************************************/
HRESULT DMBUFFER::DMHelp_DirectSoundCreate(DWORD dwDeviceId, LPDIRECTSOUND *ppDirectSound, LPUNKNOWN pUnkOuter)
{
HRESULT hr = S_OK;
CHECK(DirectSoundCreate(0, ppDirectSound, pUnkOuter));
return hr;
};


/********************************************************************************
Makes it easy to print out which mode you're in.
********************************************************************************/
static char *pszApplyStrings[] = {"TESTAPPLY_DEFERRED_NOUPDATE", "TESTAPPLY_DEFERRED_UPDATE", "TESTAPPLY_IMMEDIATE"};
char *DMBUFFER::String(TestApplies eTestApply)
{
    return pszApplyStrings[eTestApply - TESTAPPLY_DEFERRED_NOUPDATE];
}


/********************************************************************************
PURPOSE:
    Sets the x, y, or z component of a vector where x, y, z are indexed by
    dwComponent values 0-2.  Is nice for loops that test all 3 axes.
********************************************************************************/
void DMBUFFER::DMSetComponent(D3DVECTOR *pVector, DWORD dwComponent, FLOAT fValue)
{
    switch (dwComponent)
    {
        case 0: 
            pVector->x = fValue;
            break;
        case 1: 
            pVector->y = fValue;
            break;
        case 2: 
            pVector->z = fValue;
            break;
        default:
            Log(ABORTLOGLEVEL, "Test Error in DMSetComponent, see danhaff!!!!");
            break;
    }

}
