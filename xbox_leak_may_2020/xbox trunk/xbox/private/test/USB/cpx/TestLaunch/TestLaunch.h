
#include <XBApp.h>
#include <XBInput.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBSound.h>

extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}


class CXBoxUtility : public CXBApplication
{
public:
    
    HRESULT				Initialize();
    HRESULT				Render();
    HRESULT				FrameMove();
	CXBoxUtility();
	~CXBoxUtility() {;};

	CXBFont				m_Font16;
	WCHAR *				m_pwszTestNames[20];  // should never be more than 4-6

	int					m_iTestCount;
	int					m_iCurrSelection;
	int					m_SelLine;

};

