#include <xtl.h>
#include <dsound.h>

#include "devices.h"
#include "XBSound.h"
#include "xvoice.h"
#include "XbInput.h"
#include "XbUtil.h"
#include <xgraphics.h>


enum{
	WAIT_FOR_ENUM,
	WAIT_FOR_RECORD_BUTTON,
	RECORD_SOUND,
	WAIT_FOR_PLAY_BUTTON,
	PLAY_SOUND,
	WAIT_FOR_UNPLUG,

};


//-----------------------------------------------------------------------------
// Name: class CXHawkPlayback
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXHawkPlayback
{
private:
    // Main objects used for creating and rendering the 3D scene
	D3DPRESENT_PARAMETERS m_d3dpp;
    LPDIRECT3D8           m_pD3D;              // The D3D enumerator object
    LPDIRECT3DDEVICE8     m_pd3dDevice;        // The D3D rendering device

    // Variables for timing
    BOOL       m_bPaused;           // Whether app time is paused by user
   
    // Members to init the XINPUT devices.
	XDEVICE_PREALLOC_TYPE* m_InputDeviceTypes;
	DWORD                  m_dwNumInputDeviceTypes;
    XBGAMEPAD*             m_Gamepad;
    XBGAMEPAD              m_DefaultGamepad;

    BOOL					m_bStartedSound;
	BOOL					m_bXHawkConnected;
	// Sounds
    CXBSound*               m_pActiveSound;
    CXBSound*               m_pNewSound;
    CXBSound                m_pSound[5];
	CHawkUnits*				m_pHawkUnits;
	int						m_eTestStep;
	LPDIRECT3DTEXTURE8      m_pGamepadTexture[6];
	LPDIRECT3DVERTEXBUFFER8 m_pVB;
public:
	// Functions to create, run, and clean up the application
    HRESULT Create();
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();
	HRESULT Run();
    CXHawkPlayback();
};

// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}


#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}