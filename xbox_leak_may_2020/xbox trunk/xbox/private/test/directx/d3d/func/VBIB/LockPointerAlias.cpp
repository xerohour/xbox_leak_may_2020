//
//  LockPointerAlias.cpp
//

#include "LockPointerAlias.h"



// create instance of this class
GROUP_CREATE(LockPointerAlias);



//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

LockPointerAlias::LockPointerAlias()
{
    m_szTestName = _T("Lock Pointer Alias");
    m_szCommandKey = _T("LockPointerAlias");

    m_ModeOptions.fReference = false;
    m_ModeOptions.fRefEqualsSrc = false;
    m_ModeOptions.fTextures = false;
    m_ModeOptions.fZBuffer = false;
	m_ModeOptions.bSWDevices = true;
	m_ModeOptions.bHWDevices = true;
    m_ModeOptions.dwDeviceTypes = DEVICETYPE_ALL;
    m_ModeOptions.dwWinMode = WINMODE_FULLSCREEN;
    m_ModeOptions.uMinDXVersion = 0x700;
    m_ModeOptions.uMaxDXVersion = 0x800;
    m_ModeOptions.bFlipVersions = false;
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT LockPointerAlias::TestInitialize()
{
    if( m_dwVersion == 0x800 )
    {
        return(TestInitialize8());
    }
    else if( m_dwVersion == 0x700 )
    {
        return(TestInitialize7());
    }
    else
    {
        return(0);
    }
}

//-----------------------------------------------------------------------------

bool
LockPointerAlias::ExecuteTest
(
    UINT nTest
)
{
    if( m_dwVersion == 0x800 )
    {
        return(ExecuteTest8(nTest));
    }
    else if( m_dwVersion == 0x700 )
    {
        return(ExecuteTest7(nTest));
    }
    else
    {
        return(false);
    }
}

